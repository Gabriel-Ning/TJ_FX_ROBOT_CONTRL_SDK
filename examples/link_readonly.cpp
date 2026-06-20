#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

#include "MarvinSDK.h"

namespace
{

bool parse_ip_octets(int argc, char ** argv, unsigned char & ip1, unsigned char & ip2,
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
  if (!parse_ip_octets(argc, argv, ip1, ip2, ip3, ip4))
  {
    std::cerr << "Usage: " << argv[0] << " [ip1 ip2 ip3 ip4]\n"
              << "  or set MARVIN_IP=10.19.0.191\n";
    return 1;
  }

  std::cout << "Connecting read-only to " << static_cast<int>(ip1) << '.'
            << static_cast<int>(ip2) << '.' << static_cast<int>(ip3) << '.'
            << static_cast<int>(ip4) << " (no clear/send)\n";

  if (!OnLinkTo(ip1, ip2, ip3, ip4))
  {
    std::cerr << "[FAILED] OnLinkTo failed (port occupied or unreachable)\n";
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
      std::cerr << "[FAILED] OnGetBuf() returned false\n";
      OnRelease();
      return 1;
    }

    const int serial = dcss.m_Out[0].m_OutFrameSerial;
    std::cout << "frame serial=" << serial
              << " armA state=" << dcss.m_State[0].m_CurState
              << " armB state=" << dcss.m_State[1].m_CurState << '\n';

    if (serial != 0 && serial != last_serial)
    {
      ++updates;
      last_serial = serial;
    }
    sleep(1);
  }

  if (updates == 0)
  {
    std::cerr << "[FAILED] frame serial did not update (check network/firewall)\n";
    OnRelease();
    return 1;
  }

  std::cout << "[PASSED] read-only link validated (" << updates << " frame updates)\n";
  OnRelease();
  return 0;
}
