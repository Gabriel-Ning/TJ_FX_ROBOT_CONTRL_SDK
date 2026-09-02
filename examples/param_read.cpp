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
  std::printf("sdk=%ld\n", OnGetSDKVersion());

  // Vendor order: OnLinkTo -> OnClearSet -> OnClearErr -> OnSetSend -> OnGet*Para
  OnClearSet();
  OnClearErr_A();
  OnClearErr_B();
  OnSetSend();
  usleep(100000);

  char version_key[30] = {};
  std::strncpy(version_key, "VERSION", sizeof(version_key) - 1);
  long version = -1;
  const long version_ret = OnGetIntPara(version_key, &version);
  std::printf("VERSION ret=%ld value=%ld\n", version_ret, version);

  char torque_key[30] = "R.A0.L0.BASIC.TorqueMax";
  long torque = -1;
  const long torque_ret = OnGetIntPara(torque_key, &torque);
  std::printf("TorqueMax ret=%ld value=%ld\n", torque_ret, torque);

  char sensor_key[30] = "R.A1.L0.BASIC.SensorK";
  double sensor_k = 0.0;
  const long sensor_ret = OnGetFloatPara(sensor_key, &sensor_k);
  std::printf("SensorK ret=%ld value=%lf\n", sensor_ret, sensor_k);

  OnRelease();

  if (version_ret != 0 || version <= 0)
  {
    std::fprintf(stderr, "[FAILED] VERSION read\n");
    return 1;
  }
  std::printf("[PASSED] param_read (no set/save)\n");
  return 0;
}
