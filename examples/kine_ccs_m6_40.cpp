#include <cstdlib>
#include <iostream>
#include <string>

#include "FxRobot.h"

namespace
{

const char * cfg_path()
{
  if (const char * env = std::getenv("MARVIN_KINE_CFG"); env != nullptr && env[0] != '\0')
  {
    return env;
  }
  return "ccs_m6_40.MvKDCfg";
}

}  // namespace

int main()
{
  FX_INT32L TYPE[2]{};
  FX_DOUBLE GRV[2][3]{};
  FX_DOUBLE DH[2][8][4]{};
  FX_DOUBLE PNVA[2][7][4]{};
  FX_DOUBLE BD[2][4][3]{};
  FX_DOUBLE Mass[2][7]{};
  FX_DOUBLE MCP[2][7][3]{};
  FX_DOUBLE I[2][7][6]{};

  const char * path = cfg_path();
  if (LOADMvCfg(const_cast<char *>(path), TYPE, GRV, DH, PNVA, BD, Mass, MCP, I) != FX_TRUE)
  {
    std::cerr << "[FAILED] LOADMvCfg(" << path << ")\n";
    return 1;
  }
  std::cout << "[PASSED] LOADMvCfg(" << path << ")\n";

  if (FX_Robot_Init_Type(0, TYPE[0]) != FX_TRUE ||
    FX_Robot_Init_Kine(0, DH[0]) != FX_TRUE)
  {
    std::cerr << "[FAILED] left-arm kinematics init\n";
    return 1;
  }

  FX_DOUBLE joints[7] = {0, 0, 0, 0, 0, 0, 0};
  Matrix4 kine_pg{};
  if (FX_Robot_Kine_FK(0, joints, kine_pg) != FX_TRUE)
  {
    std::cerr << "[FAILED] FX_Robot_Kine_FK at zero joints\n";
    return 1;
  }

  Vect6 xyzabc{};
  if (FX_Matrix42XYZABCDEG(kine_pg, xyzabc) != FX_TRUE)
  {
    std::cerr << "[FAILED] FX_Matrix42XYZABCDEG\n";
    return 1;
  }

  std::cout << "[PASSED] FX_Robot_Kine_FK zero pose xyzabc ["
            << xyzabc[0] << ", " << xyzabc[1] << ", " << xyzabc[2] << ", "
            << xyzabc[3] << ", " << xyzabc[4] << ", " << xyzabc[5] << "]\n";
  return 0;
}
