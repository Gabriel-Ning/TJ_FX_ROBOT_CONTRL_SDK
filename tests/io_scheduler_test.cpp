#include "MarvinSDK.h"

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

namespace
{
void add_ns(timespec & value, long ns)
{
  value.tv_nsec += ns;
  if (value.tv_nsec >= 1000000000L)
  {
    value.tv_nsec -= 1000000000L;
    ++value.tv_sec;
  }
}
}

int main()
{
  const int receiver = socket(AF_INET, SOCK_DGRAM, 0);
  if (receiver < 0) return 1;

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_port = htons(4729);
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  if (bind(receiver, reinterpret_cast<sockaddr *>(&address), sizeof(address)) != 0)
  {
    std::fprintf(stderr, "receiver bind failed: %s\n", std::strerror(errno));
    close(receiver);
    return 2;
  }
  fcntl(receiver, F_SETFL, fcntl(receiver, F_GETFL, 0) | O_NONBLOCK);

  std::atomic<bool> receiving{true};
  std::atomic<uint64_t> received{0};
  std::thread receiver_thread([&]() {
    char packet[1600];
    while (receiving.load(std::memory_order_relaxed))
    {
      const ssize_t length = recv(receiver, packet, sizeof(packet), 0);
      if (length > 0) received.fetch_add(1, std::memory_order_relaxed);
      else std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    while (recv(receiver, packet, sizeof(packet), 0) > 0)
      received.fetch_add(1, std::memory_order_relaxed);
  });

  OnLocalLogOff();
  if (!OnLinkTo(127, 0, 0, 1))
  {
    receiving = false;
    receiver_thread.join();
    close(receiver);
    return 3;
  }

  constexpr int kPublishCount = 2000;
  double joints[7]{};
  timespec deadline{};
  clock_gettime(CLOCK_MONOTONIC, &deadline);
  for (int i = 0; i < kPublishCount; ++i)
  {
    if (!OnClearSet() || !OnSetJointCmdPos_A(joints) || !OnSetJointCmdPos_B(joints) ||
        !OnSetSend())
    {
      std::fprintf(stderr, "publish failed at iteration %d\n", i);
      OnRelease();
      receiving = false;
      receiver_thread.join();
      close(receiver);
      return 4;
    }
    add_ns(deadline, 1000000L);
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, nullptr) == EINTR) {}
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  MarvinIoStats stats{};
  const bool have_stats = OnGetIoStats(&stats);
  OnRelease();
  receiving = false;
  receiver_thread.join();
  close(receiver);

  std::printf("ticks=%lu published=%lu overwritten=%lu sent=%lu received=%lu "
              "missed=%lu max_late_us=%.1f errors=%lu\n",
              static_cast<unsigned long>(stats.tick_count),
              static_cast<unsigned long>(stats.publish_count),
              static_cast<unsigned long>(stats.overwrite_count),
              static_cast<unsigned long>(stats.send_success_count),
              static_cast<unsigned long>(received.load()),
              static_cast<unsigned long>(stats.deadline_miss_count),
              static_cast<double>(stats.max_lateness_ns) / 1000.0,
              static_cast<unsigned long>(stats.send_error_count));

  if (!have_stats || stats.publish_count != kPublishCount || stats.send_error_count != 0) return 5;
  // A non-RT CI host may miss many deadlines. The invariant under test is that
  // every publication is accepted and late work is skipped instead of replayed
  // in a burst. Field RT qualification applies a stricter miss-rate threshold.
  if (stats.tick_count + stats.deadline_miss_count < 1900 ||
      received.load() != stats.send_success_count) return 6;
  return 0;
}
