#!/usr/bin/env bash
set -euo pipefail
shopt -s nullglob

repo_root="$(pwd)"
packages=("${repo_root}"/dist/linux-64/libmarvin-*.conda)
package_version="$(
  sed -n 's/^  version: "\([^"]*\)"/\1/p' recipe/recipe.yaml | head -n 1
)"

if (( ${#packages[@]} != 1 )); then
  echo "Expected exactly one package in dist/linux-64/, found ${#packages[@]}." >&2
  exit 1
fi

channel_dir="$(mktemp -d /tmp/libmarvin-local-channel.XXXXXX)"
project_dir="$(mktemp -d /tmp/libmarvin-consumer-smoke.XXXXXX)"
cache_dir="$(mktemp -d /tmp/libmarvin-pixi-cache.XXXXXX)"
rattler_cache_dir="$(mktemp -d /tmp/libmarvin-rattler-cache.XXXXXX)"

mkdir -p "${channel_dir}/noarch" "${channel_dir}/linux-64"
printf '%s\n' '{"packages":{},"packages.conda":{},"repodata_version":1}' \
  > "${channel_dir}/noarch/repodata.json"
printf '%s\n' '{"packages":{},"packages.conda":{},"repodata_version":1}' \
  > "${channel_dir}/linux-64/repodata.json"

rattler-build publish \
  --to "${channel_dir}" \
  "${packages[0]}" \
  --force

cat > "${project_dir}/pixi.toml" <<EOF
[workspace]
name = "libmarvin-consumer-smoke"
version = "0.1.0"
channels = [
  "file://${channel_dir}",
  "conda-forge",
  "https://prefix.dev/robostack-jazzy"
]
platforms = ["linux-64"]

[dependencies]
cmake = "*"
ninja = "*"
cxx-compiler = "*"
sysroot_linux-64 = ">=2.39"
ros-jazzy-ros2-control-cmake = "*"
libmarvin = "==${package_version}"

[tasks]
configure = "cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=\$CONDA_PREFIX"
build = "cmake --build build --parallel"
run = "./build/smoke"
EOF

cat > "${project_dir}/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.16)
project(libmarvin_consumer_smoke LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(libmarvin CONFIG REQUIRED)

add_executable(smoke main.cpp)
target_link_libraries(smoke PRIVATE libmarvin::libmarvin)
target_compile_definitions(smoke PRIVATE CMPL_LIN)
EOF

cat > "${project_dir}/main.cpp" <<'EOF'
#include <iostream>

#include "FxType.h"

int main()
{
  std::cout << "libmarvin consumer smoke test passed: sizeof(FX_DOUBLE)="
            << sizeof(FX_DOUBLE) << '\n';
  return 0;
}
EOF

(
  cd "${project_dir}"
  PIXI_CACHE_DIR="${cache_dir}" RATTLER_CACHE_DIR="${rattler_cache_dir}" pixi install
  PIXI_CACHE_DIR="${cache_dir}" RATTLER_CACHE_DIR="${rattler_cache_dir}" pixi run configure
  PIXI_CACHE_DIR="${cache_dir}" RATTLER_CACHE_DIR="${rattler_cache_dir}" pixi run build
  PIXI_CACHE_DIR="${cache_dir}" RATTLER_CACHE_DIR="${rattler_cache_dir}" pixi run run
)

echo "Smoke project: ${project_dir}"
