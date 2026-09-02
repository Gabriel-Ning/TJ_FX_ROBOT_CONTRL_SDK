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
    std::printf("%s%.4f", i == 0 ? "" : " ", arr[i]);
  }
  std::printf("]\n");
}

void print_arm(const DCSS & dcss, int arm)
{
  std::printf("arm %c state=%d err=%d serial=%d lowspd=%d\n",
    arm == 0 ? 'A' : 'B',
    dcss.m_State[arm].m_CurState,
    dcss.m_State[arm].m_ERRCode,
    dcss.m_Out[arm].m_OutFrameSerial,
    static_cast<int>(dcss.m_Out[arm].m_LowSpdFlag));
  print_array(dcss.m_Out[arm].m_FB_Joint_Pos, "  pos_deg");
  print_array(dcss.m_Out[arm].m_FB_Joint_PosE, "  posE_deg");
  print_array(dcss.m_Out[arm].m_FB_Joint_Cmd, "  fb_cmd_deg");
  print_array(dcss.m_In[arm].m_Joint_CMD_Pos, "  in_cmd_deg");
  print_array(dcss.m_Out[arm].m_FB_Joint_Vel, "  vel_deg");
  print_array(dcss.m_Out[arm].m_FB_Joint_CToq, "  current");
  print_array(dcss.m_Out[arm].m_FB_Joint_SToq, "  torque");
  print_array(dcss.m_Out[arm].m_FB_Joint_Them, "  temp");
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
  int last_serial = -1;
  int updates = 0;
  for (int i = 0; i < 5; ++i)
  {
    if (!OnGetBuf(&dcss))
    {
      std::fprintf(stderr, "[FAILED] OnGetBuf failed\n");
      OnRelease();
      return 1;
    }
    const int serial = dcss.m_Out[0].m_OutFrameSerial;
    std::printf("connect frames:%d\n", serial);
    if (serial != 0 && serial != last_serial)
    {
      ++updates;
      last_serial = serial;
    }
    usleep(100000);
  }

  if (updates == 0)
  {
    std::fprintf(stderr, "[FAILED] frame serial did not update\n");
    OnRelease();
    return 1;
  }

  print_arm(dcss, 0);
  print_arm(dcss, 1);
  std::printf("[PASSED] dump_feedback sdk=%ld updates=%d\n", OnGetSDKVersion(), updates);
  OnRelease();
  return 0;
}
