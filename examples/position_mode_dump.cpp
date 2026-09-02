#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "MarvinSDK.h"

namespace
{

bool parse_ip(int argc, char ** argv, unsigned char & ip1, unsigned char & ip2,
  unsigned char & ip3, unsigned char & ip4)
{
  if (argc >= 5)
  {
    ip1 = static_cast<unsigned char>(std::atoi(argv[1]));
    ip2 = static_cast<unsigned char>(std::atoi(argv[2]));
    ip3 = static_cast<unsigned char>(std::atoi(argv[3]));
    ip4 = static_cast<unsigned char>(std::atoi(argv[4]));
    return true;
  }

  const char * ip_env = std::getenv("MARVIN_IP");
  if (ip_env == nullptr)
  {
    ip1 = 10;
    ip2 = 19;
    ip3 = 0;
    ip4 = 191;
    return true;
  }

  unsigned a = 0, b = 0, c = 0, d = 0;
  if (std::sscanf(ip_env, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
  {
    return false;
  }
  ip1 = static_cast<unsigned char>(a);
  ip2 = static_cast<unsigned char>(b);
  ip3 = static_cast<unsigned char>(c);
  ip4 = static_cast<unsigned char>(d);
  return true;
}

void print_array(const FX_FLOAT * arr, const char * name)
{
  std::printf("%s=[", name);
  for (int i = 0; i < 7; ++i)
  {
    std::printf("%s%.4f", i == 0 ? "" : ",", arr[i]);
  }
  std::printf("]\n");
}

bool wait_state(int arm, int target, int timeout_ms)
{
  DCSS dcss{};
  for (int elapsed = 0; elapsed < timeout_ms; elapsed += 50)
  {
    if (OnGetBuf(&dcss) && dcss.m_State[arm].m_CurState == target)
    {
      return true;
    }
    usleep(50000);
  }
  return false;
}

}  // namespace

int main(int argc, char ** argv)
{
  unsigned char ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
  if (!parse_ip(argc, argv, ip1, ip2, ip3, ip4))
  {
    std::fprintf(stderr, "Usage: %s [ip1 ip2 ip3 ip4]\n", argv[0]);
    return 1;
  }

  if (!OnLinkTo(ip1, ip2, ip3, ip4))
  {
    std::fprintf(stderr, "[FAILED] OnLinkTo failed\n");
    return 1;
  }
  usleep(200000);

  DCSS dcss{};
  OnGetBuf(&dcss);
  if (dcss.m_State[0].m_ERRCode != 0 || dcss.m_State[0].m_CurState == ARM_STATE_ERROR)
  {
    OnClearSet();
    OnClearErr_A();
    OnSetSend();
    usleep(200000);
  }
  if (dcss.m_State[1].m_ERRCode != 0 || dcss.m_State[1].m_CurState == ARM_STATE_ERROR)
  {
    OnClearSet();
    OnClearErr_B();
    OnSetSend();
    usleep(200000);
  }

  // showcase_position_two_arms: set position mode + 10% vel/acc, then subscribe.
  // This program does not send OnSetJointCmdPos.
  OnClearSet();
  OnSetTargetState_A(1);
  OnSetJointLmt_A(10, 10);
  OnSetTargetState_B(1);
  OnSetJointLmt_B(10, 10);
  OnSetSend();
  usleep(200000);

  if (!wait_state(0, ARM_STATE_POSITION, 3000) || !wait_state(1, ARM_STATE_POSITION, 3000))
  {
    std::fprintf(stderr, "[FAILED] did not reach position mode\n");
    OnGetBuf(&dcss);
    std::fprintf(stderr, "A state=%d err=%d B state=%d err=%d\n",
      dcss.m_State[0].m_CurState, dcss.m_State[0].m_ERRCode,
      dcss.m_State[1].m_CurState, dcss.m_State[1].m_ERRCode);
    OnClearSet();
    OnSetTargetState_A(0);
    OnSetTargetState_B(0);
    OnSetSend();
    OnRelease();
    return 1;
  }

  OnGetBuf(&dcss);
  std::printf("A arm current state:%d error:%d vel/acc:%d %d\n",
    dcss.m_State[0].m_CurState, dcss.m_State[0].m_ERRCode,
    dcss.m_In[0].m_Joint_Vel_Ratio, dcss.m_In[0].m_Joint_Acc_Ratio);
  std::printf("B arm current state:%d error:%d vel/acc:%d %d\n",
    dcss.m_State[1].m_CurState, dcss.m_State[1].m_ERRCode,
    dcss.m_In[1].m_Joint_Vel_Ratio, dcss.m_In[1].m_Joint_Acc_Ratio);
  print_array(dcss.m_In[0].m_Joint_CMD_Pos, "CMD joints of arm A");
  print_array(dcss.m_Out[0].m_FB_Joint_Pos, "current joints of arm A");
  print_array(dcss.m_In[1].m_Joint_CMD_Pos, "CMD joints of arm B");
  print_array(dcss.m_Out[1].m_FB_Joint_Pos, "current joints of arm B");

  OnClearSet();
  OnSetTargetState_A(0);
  OnSetTargetState_B(0);
  OnSetSend();
  wait_state(0, ARM_STATE_IDLE, 3000);
  wait_state(1, ARM_STATE_IDLE, 3000);

  std::printf("[PASSED] position_mode_dump (no joint command)\n");
  OnRelease();
  return 0;
}
