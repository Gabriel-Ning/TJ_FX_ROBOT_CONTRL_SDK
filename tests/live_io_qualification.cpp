#include "MarvinSDK.h"

#include <atomic>
#include <cerrno>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <unistd.h>

namespace
{
std::atomic<bool> running{true};

void stop_handler(int) { running.store(false); }

void add_ns(timespec & value, long ns)
{
  value.tv_nsec += ns;
  while (value.tv_nsec >= 1000000000L)
  {
    value.tv_nsec -= 1000000000L;
    ++value.tv_sec;
  }
}

double elapsed(const timespec & now, const timespec & start)
{
  return static_cast<double>(now.tv_sec - start.tv_sec) +
         static_cast<double>(now.tv_nsec - start.tv_nsec) * 1e-9;
}

bool publish_empty_handshake()
{
  return OnClearSet() && OnSetSend();
}

bool wait_for_feedback(DCSS & frame, int timeout_ms)
{
  int previous = 0;
  for (int i = 0; i < timeout_ms / 10; ++i)
  {
    if (OnGetBuf(&frame))
    {
      const int serial = frame.m_Out[0].m_OutFrameSerial;
      if (serial != 0 && serial != previous) return true;
      previous = serial;
    }
    usleep(10000);
  }
  return false;
}

bool publish_idle()
{
  return OnClearSet() && OnSetTargetState_A(0) && OnSetTargetState_B(0) && OnSetSend();
}

bool enter_position_with_retry(DCSS & frame)
{
  for (int attempt = 0; attempt < 3; ++attempt)
  {
    // 10% is the lowest value verified by the vendor's 1003 examples on this robot.
    if (!OnClearSet() || !OnSetJointLmt_A(10, 10) || !OnSetTargetState_A(1) ||
        !OnSetJointLmt_B(10, 10) || !OnSetTargetState_B(1) || !OnSetSend())
      return false;
    for (int poll = 0; poll < 60; ++poll)
    {
      OnGetBuf(&frame);
      if (frame.m_State[0].m_CurState == 1 && frame.m_State[1].m_CurState == 1)
      {
        std::printf("position attempt %d reached state=1/1\n", attempt + 1);
        return true;
      }
      if (frame.m_State[0].m_CurState == 100 || frame.m_State[1].m_CurState == 100)
        break;
      usleep(50000);
    }

    OnGetBuf(&frame);
    std::printf("position attempt %d state=%d/%d error=%d/%d\n", attempt + 1,
                frame.m_State[0].m_CurState, frame.m_State[1].m_CurState,
                frame.m_State[0].m_ERRCode, frame.m_State[1].m_ERRCode);
    const bool clear_a = frame.m_State[0].m_CurState == 100;
    const bool clear_b = frame.m_State[1].m_CurState == 100;
    if (clear_a) OnClearErr_A();
    if (clear_b) OnClearErr_B();
    usleep(200000);
  }
  return false;
}

void print_stats(const char * phase)
{
  MarvinIoStats stats{};
  if (!OnGetIoStats(&stats)) return;
  std::printf("[%s] ticks=%lu missed=%lu max_late_us=%.1f publish=%lu overwrite=%lu "
              "send_ok=%lu send_err=%lu errno=%d\n",
              phase,
              static_cast<unsigned long>(stats.tick_count),
              static_cast<unsigned long>(stats.deadline_miss_count),
              static_cast<double>(stats.max_lateness_ns) / 1000.0,
              static_cast<unsigned long>(stats.publish_count),
              static_cast<unsigned long>(stats.overwrite_count),
              static_cast<unsigned long>(stats.send_success_count),
              static_cast<unsigned long>(stats.send_error_count),
              stats.last_send_errno);
	std::printf("[%s] work avg/max us send=%.2f/%.2f recv=%.2f/%.2f sched=%d/%d\n",
	            phase,
	            stats.tick_count ? stats.send_work_total_ns / 1000.0 / stats.tick_count : 0.0,
	            stats.send_work_max_ns / 1000.0,
	            stats.tick_count ? stats.recv_work_total_ns / 1000.0 / stats.tick_count : 0.0,
	            stats.recv_work_max_ns / 1000.0,
	            stats.io_sched_policy, stats.io_sched_priority);
}
}

int main(int argc, char ** argv)
{
  const char * mode = argc > 1 ? argv[1] : "handshake";
  const int rate_hz = argc > 2 ? std::atoi(argv[2]) : 500;
  const double duration_sec = argc > 3 ? std::atof(argv[3]) : 10.0;
  const double amplitude_deg = std::strcmp(mode, "sine") == 0 ? 0.25 : 0.0;
  if (std::strcmp(mode, "handshake") != 0 && std::strcmp(mode, "observe") != 0 &&
      std::strcmp(mode, "poll") != 0 &&
      std::strcmp(mode, "empty-stream") != 0 &&
      std::strcmp(mode, "recover") != 0 &&
      std::strcmp(mode, "hold") != 0 && std::strcmp(mode, "sine") != 0)
  {
    std::fprintf(stderr, "usage: %s [handshake|observe|poll|empty-stream|recover|hold|sine] [rate_hz] [duration_sec]\n", argv[0]);
    return 2;
  }
  if (rate_hz < 100 || rate_hz > 1000 || duration_sec <= 0.0 || duration_sec > 60.0)
  {
    std::fprintf(stderr, "rate must be 100..1000 Hz and duration 0..60 s\n");
    return 2;
  }

  std::signal(SIGINT, stop_handler);
  std::signal(SIGTERM, stop_handler);
  OnLocalLogOff();
  if (!OnLinkTo(10, 19, 0, 191))
  {
    std::fprintf(stderr, "OnLinkTo failed\n");
    return 3;
  }
  if (std::strcmp(mode, "handshake") == 0 && !publish_empty_handshake())
  {
    std::fprintf(stderr, "empty handshake publish failed\n");
    OnRelease();
    return 4;
  }

  DCSS frame{};
  if (!wait_for_feedback(frame, 3000))
  {
    std::fprintf(stderr, "no fresh feedback after handshake\n");
    print_stats("handshake-failed");
    OnRelease();
    return 5;
  }
  std::printf("feedback serial=%d/%d state=%d/%d error=%d/%d\n",
              frame.m_Out[0].m_OutFrameSerial, frame.m_Out[1].m_OutFrameSerial,
              frame.m_State[0].m_CurState, frame.m_State[1].m_CurState,
              frame.m_State[0].m_ERRCode, frame.m_State[1].m_ERRCode);
  print_stats("handshake");
  if (std::strcmp(mode, "handshake") == 0)
  {
    OnRelease();
    return 0;
  }
  if (std::strcmp(mode, "observe") == 0)
  {
    usleep(static_cast<useconds_t>(duration_sec * 1000000.0));
    print_stats("observe");
    OnRelease();
    return 0;
  }
  if (std::strcmp(mode, "poll") == 0)
  {
    const long period_ns = 1000000000L / rate_hz;
    timespec start{}, deadline{}, now{};
    clock_gettime(CLOCK_MONOTONIC, &start);
    deadline = start;
    uint64_t reads = 0;
    do
    {
      OnGetBuf(&frame);
      ++reads;
      add_ns(deadline, period_ns);
      while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, nullptr) == EINTR) {}
      clock_gettime(CLOCK_MONOTONIC, &now);
    } while (elapsed(now, start) < duration_sec);
    std::printf("poll reads=%lu\n", static_cast<unsigned long>(reads));
    print_stats("poll");
    OnRelease();
    return 0;
  }
  if (std::strcmp(mode, "empty-stream") == 0)
  {
    const long period_ns = 1000000000L / rate_hz;
    timespec start{}, deadline{}, now{};
    clock_gettime(CLOCK_MONOTONIC, &start);
    deadline = start;
    do
    {
      if (!publish_empty_handshake()) break;
      add_ns(deadline, period_ns);
      while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, nullptr) == EINTR) {}
      clock_gettime(CLOCK_MONOTONIC, &now);
    } while (elapsed(now, start) < duration_sec);
    print_stats("empty-stream");
    OnRelease();
    return 0;
  }
  if (std::strcmp(mode, "recover") == 0)
  {
    OnClearErr_A();
    OnClearErr_B();
    if (!publish_idle()) return 6;
    usleep(500000);
    OnGetBuf(&frame);
    std::printf("after recovery state=%d/%d error=%d/%d\n",
                frame.m_State[0].m_CurState, frame.m_State[1].m_CurState,
                frame.m_State[0].m_ERRCode, frame.m_State[1].m_ERRCode);
    OnRelease();
    return (frame.m_State[0].m_ERRCode == 0 && frame.m_State[1].m_ERRCode == 0 &&
            frame.m_State[0].m_CurState == 0 && frame.m_State[1].m_CurState == 0) ? 0 : 6;
  }
  double initial_a[7]{};
  double initial_b[7]{};
  for (int joint = 0; joint < 7; ++joint)
  {
    initial_a[joint] = frame.m_Out[0].m_FB_Joint_Pos[joint];
    initial_b[joint] = frame.m_Out[1].m_FB_Joint_Pos[joint];
    if (!std::isfinite(initial_a[joint]) || !std::isfinite(initial_b[joint]))
    {
      std::fprintf(stderr, "non-finite measured position\n");
      OnRelease();
      return 7;
    }
  }

  // Match the ROS HI: request position, clear intermittent err=4, retry up to 3 times.
  if (!enter_position_with_retry(frame))
  {
    OnGetBuf(&frame);
    std::fprintf(stderr, "position mode failed: state=%d/%d error=%d/%d\n",
                 frame.m_State[0].m_CurState, frame.m_State[1].m_CurState,
                 frame.m_State[0].m_ERRCode, frame.m_State[1].m_ERRCode);
    publish_idle();
    OnRelease();
    return 8;
  }

  // Seed the first target only after both arms have acknowledged position mode.
  OnGetBuf(&frame);
  for (int joint = 0; joint < 7; ++joint)
  {
    initial_a[joint] = frame.m_Out[0].m_FB_Joint_Pos[joint];
    initial_b[joint] = frame.m_Out[1].m_FB_Joint_Pos[joint];
  }
  if (!OnClearSet() || !OnSetJointCmdPos_A(initial_a) ||
      !OnSetJointCmdPos_B(initial_b) || !OnSetSend())
  {
    std::fprintf(stderr, "initial hold command failed\n");
    publish_idle();
    OnRelease();
    return 10;
  }

  const long period_ns = 1000000000L / rate_hz;
  timespec start{}, deadline{}, now{};
  clock_gettime(CLOCK_MONOTONIC, &start);
  deadline = start;
  int last_serial = frame.m_Out[0].m_OutFrameSerial;
  int last_serial_b = frame.m_Out[1].m_OutFrameSerial;
  uint64_t read_calls = 0;
  uint64_t fresh_frames_a = 0;
  uint64_t fresh_frames_b = 0;
  uint64_t serial_steps_a = 0;
  uint64_t serial_steps_b = 0;
  uint64_t j2_position_changes = 0;
  uint64_t j2_velocity_changes = 0;
  uint64_t all_position_changes = 0;
  uint64_t j2_same_run = 0;
  uint64_t j2_same_run_max = 0;
  double last_j2_position = frame.m_Out[0].m_FB_Joint_Pos[1];
  double last_j2_velocity = frame.m_Out[0].m_FB_Joint_Vel[1];
  double last_positions[14]{};
  for (int joint = 0; joint < 7; ++joint)
  {
    last_positions[joint] = frame.m_Out[0].m_FB_Joint_Pos[joint];
    last_positions[joint + 7] = frame.m_Out[1].m_FB_Joint_Pos[joint];
  }
  timespec last_fresh = start;
  int last_report_second = -1;
  bool safe_completion = true;
  while (running.load())
  {
    clock_gettime(CLOCK_MONOTONIC, &now);
    const double t = elapsed(now, start);
    if (t >= duration_sec) break;

    double command_a[7];
    double command_b[7];
    std::memcpy(command_a, initial_a, sizeof(command_a));
    std::memcpy(command_b, initial_b, sizeof(command_b));
    // Left J2 only: ±0.25 degree at 0.1 Hz by default.
    command_a[1] += amplitude_deg * std::sin(2.0 * M_PI * 0.1 * t);
    if (!OnClearSet() || !OnSetJointCmdPos_A(command_a) ||
        !OnSetJointCmdPos_B(command_b) || !OnSetSend())
    {
      std::fprintf(stderr, "command publish failed at %.3f s\n", t);
      safe_completion = false;
      break;
    }

    if (OnGetBuf(&frame))
    {
      ++read_calls;
      const int serial_a = frame.m_Out[0].m_OutFrameSerial;
      const int serial_b = frame.m_Out[1].m_OutFrameSerial;
      if (serial_a != last_serial)
      {
        int delta = serial_a - last_serial;
        if (delta < 0) delta += 1000000;
        serial_steps_a += static_cast<uint64_t>(delta);
        ++fresh_frames_a;
        const double j2_position = frame.m_Out[0].m_FB_Joint_Pos[1];
        const double j2_velocity = frame.m_Out[0].m_FB_Joint_Vel[1];
        if (j2_position != last_j2_position)
        {
          ++j2_position_changes;
          last_j2_position = j2_position;
          j2_same_run = 0;
        }
        else
        {
          ++j2_same_run;
          if (j2_same_run > j2_same_run_max) j2_same_run_max = j2_same_run;
        }
        if (j2_velocity != last_j2_velocity)
        {
          ++j2_velocity_changes;
          last_j2_velocity = j2_velocity;
        }
        bool positions_changed = false;
        for (int joint = 0; joint < 7; ++joint)
        {
          const double pos_a = frame.m_Out[0].m_FB_Joint_Pos[joint];
          const double pos_b = frame.m_Out[1].m_FB_Joint_Pos[joint];
          if (pos_a != last_positions[joint] || pos_b != last_positions[joint + 7])
            positions_changed = true;
          last_positions[joint] = pos_a;
          last_positions[joint + 7] = pos_b;
        }
        if (positions_changed) ++all_position_changes;
        last_serial = serial_a;
        last_fresh = now;
      }
      if (serial_b != last_serial_b)
      {
        int delta = serial_b - last_serial_b;
        if (delta < 0) delta += 1000000;
        serial_steps_b += static_cast<uint64_t>(delta);
        ++fresh_frames_b;
        last_serial_b = serial_b;
      }
    }
    if (elapsed(now, last_fresh) > 0.100 || frame.m_State[0].m_ERRCode != 0 ||
        frame.m_State[1].m_ERRCode != 0)
    {
      std::fprintf(stderr, "safety stop: feedback stale/error at %.3f s\n", t);
      safe_completion = false;
      break;
    }
    const int report_second = static_cast<int>(t);
    if (report_second != last_report_second)
    {
      last_report_second = report_second;
      print_stats(mode);
    }

    add_ns(deadline, period_ns);
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, nullptr) == EINTR && running.load()) {}
  }

  // Return to the measured start target, then leave position mode.
  OnClearSet();
  OnSetJointCmdPos_A(initial_a);
  OnSetJointCmdPos_B(initial_b);
  OnSetSend();
  usleep(100000);
  publish_idle();
  for (int poll = 0; poll < 60; ++poll)
  {
    OnGetBuf(&frame);
    if (frame.m_State[0].m_CurState == 0 && frame.m_State[1].m_CurState == 0) break;
    usleep(50000);
  }
  print_stats("final");
  std::printf("[final] reads=%lu fresh=%lu/%lu serial_steps=%lu/%lu gaps=%lu/%lu "
              "state=%d/%d error=%d/%d\n",
              static_cast<unsigned long>(read_calls),
              static_cast<unsigned long>(fresh_frames_a),
              static_cast<unsigned long>(fresh_frames_b),
              static_cast<unsigned long>(serial_steps_a),
              static_cast<unsigned long>(serial_steps_b),
              static_cast<unsigned long>(serial_steps_a > fresh_frames_a ?
                serial_steps_a - fresh_frames_a : 0),
              static_cast<unsigned long>(serial_steps_b > fresh_frames_b ?
                serial_steps_b - fresh_frames_b : 0),
              frame.m_State[0].m_CurState, frame.m_State[1].m_CurState,
              frame.m_State[0].m_ERRCode, frame.m_State[1].m_ERRCode);
  std::printf("[final] physical_updates all_pos=%lu left_j2_pos=%lu left_j2_vel=%lu "
              "j2_max_identical_fresh_frames=%lu\n",
              static_cast<unsigned long>(all_position_changes),
              static_cast<unsigned long>(j2_position_changes),
              static_cast<unsigned long>(j2_velocity_changes),
              static_cast<unsigned long>(j2_same_run_max));
  const bool idle = frame.m_State[0].m_CurState == 0 && frame.m_State[1].m_CurState == 0;
  OnRelease();
  return safe_completion && idle ? 0 : 11;
}
