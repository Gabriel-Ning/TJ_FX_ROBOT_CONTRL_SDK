#include <cstdio>
#include <cstdlib>
#include <cstring>
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

  OnClearSet();
  OnClearErr_A();
  OnClearErr_B();
  OnSetSend();
  usleep(100000);

  char torque_key[30] = "R.A0.L0.BASIC.TorqueMax";
  long before = -1;
  const long read_ret = OnGetIntPara(torque_key, &before);
  std::printf("read TorqueMax ret=%ld value=%ld\n", read_ret, before);
  if (read_ret != 0 || before <= 0)
  {
    std::fprintf(stderr, "[FAILED] read TorqueMax\n");
    OnRelease();
    return 1;
  }

  // Round-trip the same value. Do not OnSavePara — runtime RAM only.
  const long set_ret = OnSetIntPara(torque_key, before);
  std::printf("write TorqueMax ret=%ld value=%ld (same as read, no save)\n", set_ret, before);
  usleep(200000);

  long after = -1;
  const long verify_ret = OnGetIntPara(torque_key, &after);
  std::printf("verify TorqueMax ret=%ld value=%ld\n", verify_ret, after);

  OnRelease();

  if (set_ret != 0 || verify_ret != 0 || after != before)
  {
    std::fprintf(stderr, "[FAILED] param write round-trip\n");
    return 1;
  }
  std::printf("[PASSED] param_write round-trip (no OnSavePara)\n");
  return 0;
}
