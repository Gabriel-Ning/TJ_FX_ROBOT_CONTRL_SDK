#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

#include "MarvinSDK.h"

namespace
{

bool parse_ip(
  int argc, char ** argv, unsigned char & ip1, unsigned char & ip2,
  unsigned char & ip3, unsigned char & ip4)
{
  unsigned a = 10;
  unsigned b = 19;
  unsigned c = 0;
  unsigned d = 191;

  if (argc == 5)
  {
    if (std::sscanf(argv[1], "%u", &a) != 1 ||
      std::sscanf(argv[2], "%u", &b) != 1 ||
      std::sscanf(argv[3], "%u", &c) != 1 ||
      std::sscanf(argv[4], "%u", &d) != 1)
    {
      return false;
    }
  }
  else if (argc == 1)
  {
    const char * ip_env = std::getenv("MARVIN_IP");
    if (ip_env != nullptr &&
      std::sscanf(ip_env, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  if (a > 255 || b > 255 || c > 255 || d > 255)
  {
    return false;
  }

  ip1 = static_cast<unsigned char>(a);
  ip2 = static_cast<unsigned char>(b);
  ip3 = static_cast<unsigned char>(c);
  ip4 = static_cast<unsigned char>(d);
  return true;
}

void print_servo_errors(const char * arm, const std::array<long, 7> & errors)
{
  std::cout << arm << " servo errors: [";
  for (std::size_t i = 0; i < errors.size(); ++i)
  {
    if (i != 0)
    {
      std::cout << ", ";
    }
    std::cout << errors[i];
  }
  std::cout << "]\n";
}

}  // namespace

int main(int argc, char ** argv)
{
  unsigned char ip1 = 0;
  unsigned char ip2 = 0;
  unsigned char ip3 = 0;
  unsigned char ip4 = 0;
  if (!parse_ip(argc, argv, ip1, ip2, ip3, ip4))
  {
    std::cerr << "Usage: " << argv[0] << " [ip1 ip2 ip3 ip4]\n"
              << "  or set MARVIN_IP=10.19.0.191\n";
    return 1;
  }

  std::cout << "Connecting to " << static_cast<int>(ip1) << '.'
            << static_cast<int>(ip2) << '.' << static_cast<int>(ip3) << '.'
            << static_cast<int>(ip4) << "\n";

  if (!OnLinkTo(ip1, ip2, ip3, ip4))
  {
    std::cerr << "[FAILED] OnLinkTo failed (controller unreachable or another client is connected)\n";
    return 1;
  }

  usleep(500000);

  DCSS before{};
  if (!OnGetBuf(&before))
  {
    std::cerr << "[FAILED] Could not read controller state\n";
    OnRelease();
    return 1;
  }

  std::array<long, 7> servo_a{};
  std::array<long, 7> servo_b{};
  OnGetServoErr_A(servo_a.data());
  OnGetServoErr_B(servo_b.data());

  std::cout << "Before clear: arm A state=" << before.m_State[0].m_CurState
            << " error=" << before.m_State[0].m_ERRCode
            << "; arm B state=" << before.m_State[1].m_CurState
            << " error=" << before.m_State[1].m_ERRCode << '\n';
  print_servo_errors("Arm A", servo_a);
  print_servo_errors("Arm B", servo_b);

  std::cout << "\nRelease the physical E-stop and verify the safety area is clear.\n"
            << "Type CLEAR to clear errors on both arms: ";
  std::string confirmation;
  std::getline(std::cin, confirmation);
  if (confirmation != "CLEAR")
  {
    std::cout << "[CANCELLED] No command was sent\n";
    OnRelease();
    return 2;
  }

  if (!OnClearSet())
  {
    std::cerr << "[FAILED] OnClearSet failed\n";
    OnRelease();
    return 1;
  }
  OnClearErr_A();
  OnClearErr_B();
  if (!OnSetSend())
  {
    std::cerr << "[FAILED] OnSetSend failed\n";
    OnRelease();
    return 1;
  }

  usleep(500000);

  DCSS after{};
  if (!OnGetBuf(&after))
  {
    std::cerr << "[FAILED] Clear command sent, but verification read failed\n";
    OnRelease();
    return 1;
  }

  servo_a.fill(0);
  servo_b.fill(0);
  OnGetServoErr_A(servo_a.data());
  OnGetServoErr_B(servo_b.data());

  std::cout << "After clear: arm A state=" << after.m_State[0].m_CurState
            << " error=" << after.m_State[0].m_ERRCode
            << "; arm B state=" << after.m_State[1].m_CurState
            << " error=" << after.m_State[1].m_ERRCode << '\n';
  print_servo_errors("Arm A", servo_a);
  print_servo_errors("Arm B", servo_b);

  const bool cleared =
    after.m_State[0].m_ERRCode == 0 && after.m_State[1].m_ERRCode == 0;
  OnRelease();

  if (!cleared)
  {
    std::cerr << "[FAILED] Arm-level error remains. Error 13 means the E-stop is still active.\n";
    return 1;
  }

  std::cout << "[PASSED] Both arm-level errors are clear. No motion command was sent.\n";
  return 0;
}
