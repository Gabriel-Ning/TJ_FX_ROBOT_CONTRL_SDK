# libmarvin (hardware-1003 branch)

Conda/pixi packaging for the **1003-line** Marvin control SDK (`contrlSDK/` → `libMarvinSDK.so`).

Use this branch for current field controllers (1003 protocol, e.g. `OnGetIntPara("VERSION")` → `100391`).

## Quick start

```bash
git checkout hardware-1003
pixi install
pixi run build
pixi run validate-offline
```

With robot connected (optional):

```bash
export MARVIN_IP=10.19.0.191   # optional
pixi run validate-hardware
```

## Conda package

| Item | Value |
|------|--------|
| Branch | `hardware-1003` |
| Package | `libmarvin` |
| Version | `0.1.x` |
| CMake target | `libmarvin::libmarvin` |
| Library | `$PREFIX/lib/libMarvinSDK.so` |
| Headers | `$PREFIX/include/marvin/` |

Build locally:

```bash
pixi run version-check
pixi run conda-build          # output: dist/linux-64/libmarvin-0.1.0-*.conda
pixi run conda-smoke          # build + consumer install test
```

Publish to prefix.dev (optional):

```bash
pixi run conda-upload         # channel: gabriel-robotics
# or
pixi run conda-release        # build + upload
```

## ros2_workspace

Pin the 1003-line package from this branch:

```toml
[pypi-dependencies]  # or [dependencies] depending on your pixi layout
# in [dependencies]:
libmarvin = ">=0.1.0,<0.2"
```

In `marvin_hardware_interface/CMakeLists.txt`:

```cmake
find_package(libmarvin CONFIG REQUIRED)
target_link_libraries(... libmarvin::libmarvin)
```

After `pixi update`, confirm runtime loads the pixi env library:

```bash
ldd install/marvin_hardware_interface/lib/libmarvin_hardware_interface.so | grep -i marvin
```

## SDK source layout

CMake compiles the same sources as `contrlSDK/makefile`:

- `MarvinSDK.cpp`, `Robot.cpp`, `ACB.cpp`, `FXDG.cpp`, `PointSet.cpp`
- `FileOP.cpp`, `FilePortal.cpp`, `Parser.cpp`, `TCPAgent.cpp`, `TCPFileClient.cpp`

`SDK_VERSION` in `contrlSDK/Robot.h` is `1003`.

Parameter reads require vendor setup after connect (`OnClearSet`, `OnClearErr_*`, `OnSetSend`) — see `examples/pkg_probe.cpp`.

## Other branch

For controllers on the **100343+** protocol, use `master` and `libmarvin` **0.2.x** — see [BRANCHES.md](BRANCHES.md).
