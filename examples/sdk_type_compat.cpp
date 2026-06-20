#include <cstdlib>
#include <iostream>

#include "MarvinSDK.h"

int main(int argc, char ** argv)
{
  bool connect_robot = false;
  for (int i = 1; i < argc; ++i)
  {
    if (std::string(argv[i]) == "--connect")
    {
      connect_robot = true;
    }
  }

  int byte_order = -1;
  const int ret = CheckSDKTypeCompat(&byte_order);
  if (ret < 0)
  {
    std::cerr << "[FAILED] SDK type compatibility check failed, mask=0x"
              << std::hex << (-ret) << std::dec << '\n';
    return 1;
  }

  std::cout << "[PASSED] FxType.h sizes and pack(4) alignment match libmarvin\n";
  std::cout << "[INFO]  byte order: "
            << (byte_order == 0 ? "little-endian" : "big-endian") << '\n';

  if (!connect_robot)
  {
    return 0;
  }

  const char * ip_env = std::getenv("MARVIN_IP");
  unsigned char ip1 = 10, ip2 = 19, ip3 = 0, ip4 = 191;
  if (ip_env != nullptr)
  {
    unsigned a = 0, b = 0, c = 0, d = 0;
    if (std::sscanf(ip_env, "%u.%u.%u.%u", &a, &b, &c, &d) == 4)
    {
      ip1 = static_cast<unsigned char>(a);
      ip2 = static_cast<unsigned char>(b);
      ip3 = static_cast<unsigned char>(c);
      ip4 = static_cast<unsigned char>(d);
    }
  }

  if (!OnLinkTo(ip1, ip2, ip3, ip4))
  {
    std::cerr << "[FAILED] OnLinkTo(" << static_cast<int>(ip1) << '.'
              << static_cast<int>(ip2) << '.' << static_cast<int>(ip3) << '.'
              << static_cast<int>(ip4) << ") failed\n";
    return 1;
  }

  DCSS dcss{};
  if (!OnGetBuf(&dcss))
  {
    std::cerr << "[FAILED] OnGetBuf() failed after connect\n";
    OnRelease();
    return 1;
  }

  std::cout << "[PASSED] connected and received DCSS frame "
            << dcss.m_Out[0].m_OutFrameSerial << '\n';
  OnRelease();
  return 0;
}
