cmake_minimum_required(VERSION 3.16)
project(libmarvin VERSION 0.1.0 LANGUAGES CXX)

if(POLICY CMP0087)
  cmake_policy(SET CMP0087 NEW)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)

set(MARVIN_CONTROL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/contrlSDK100343")

set(MARVIN_CONTROL_SOURCES
  ${MARVIN_CONTROL_DIR}/ACB.cpp
  ${MARVIN_CONTROL_DIR}/CAxisSpPln.cpp
  ${MARVIN_CONTROL_DIR}/FileOP.cpp
  ${MARVIN_CONTROL_DIR}/FilePortal.cpp
  ${MARVIN_CONTROL_DIR}/FXDG.cpp
  ${MARVIN_CONTROL_DIR}/MarvinSDK.cpp
  ${MARVIN_CONTROL_DIR}/Parser.cpp
  ${MARVIN_CONTROL_DIR}/PointSet.cpp
  ${MARVIN_CONTROL_DIR}/Robot.cpp
  ${MARVIN_CONTROL_DIR}/ShMem.cpp
  ${MARVIN_CONTROL_DIR}/TCPAgent.cpp
  ${MARVIN_CONTROL_DIR}/TCPFileClient.cpp
)

add_library(${PROJECT_NAME} SHARED ${MARVIN_CONTROL_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  OUTPUT_NAME MarvinSDK
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  INSTALL_REMOVE_ENVIRONMENT_RPATH TRUE
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${MARVIN_CONTROL_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/marvin>
)

target_compile_definitions(${PROJECT_NAME} PRIVATE CMPL_LIN)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads rt)

add_library(libmarvin::libmarvin ALIAS ${PROJECT_NAME})

if(BUILD_TESTING)
  add_executable(libmarvin_smoke tests/smoke.cpp)
  target_link_libraries(libmarvin_smoke PRIVATE ${PROJECT_NAME})
  add_test(NAME libmarvin_smoke COMMAND libmarvin_smoke)
endif()

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY ${MARVIN_CONTROL_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/marvin
  FILES_MATCHING PATTERN "*.h"
)

install(
  EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_package_config_file(
  cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
