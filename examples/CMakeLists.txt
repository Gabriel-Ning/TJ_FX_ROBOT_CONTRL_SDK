set(MARVIN_KINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../kinematicsSDK")

set(MARVIN_KINE_SOURCES
  ${MARVIN_KINE_DIR}/AxisPln.cpp
  ${MARVIN_KINE_DIR}/fxrobot_cpi.cpp
  ${MARVIN_KINE_DIR}/FXDG.cpp
  ${MARVIN_KINE_DIR}/FXMath.cpp
  ${MARVIN_KINE_DIR}/FXMatrix.cpp
  ${MARVIN_KINE_DIR}/FxRobot.cpp
  ${MARVIN_KINE_DIR}/LoadIdenPub.cpp
  ${MARVIN_KINE_DIR}/O3Polynorm.cpp
  ${MARVIN_KINE_DIR}/PointSet.cpp
)

add_library(marvin_kine_validate STATIC ${MARVIN_KINE_SOURCES})
target_include_directories(marvin_kine_validate PUBLIC ${MARVIN_KINE_DIR})
target_link_libraries(marvin_kine_validate PRIVATE Threads::Threads rt)

macro(add_marvin_control_example target source)
  add_executable(${target} ${source})
  target_link_libraries(${target} PRIVATE libmarvin)
  target_compile_definitions(${target} PRIVATE CMPL_LIN)
endmacro()

add_marvin_control_example(marvin_validate_link_readonly link_readonly.cpp)
add_marvin_control_example(marvin_validate_pkg_probe pkg_probe.cpp)

add_executable(marvin_validate_kine_ccs_m6_40 kine_ccs_m6_40.cpp)
target_link_libraries(marvin_validate_kine_ccs_m6_40 PRIVATE marvin_kine_validate)

set(MARVIN_CFG_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../DEMO_C++/ccs_m6_40.MvKDCfg")
configure_file(${MARVIN_CFG_SRC} ${CMAKE_CURRENT_BINARY_DIR}/ccs_m6_40.MvKDCfg COPYONLY)

if(BUILD_TESTING)
  add_test(NAME marvin_validate_kine_ccs_m6_40
    COMMAND marvin_validate_kine_ccs_m6_40
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
