# TJ_FX_ROBOT_CONTRL_SDK — `hardware-1003` branch

Field-validated **1003-line** Marvin control SDK, packaged as **`libmarvin` 0.1.x** for ros2 and C++ integration.

| | |
|---|---|
| **Branch** | `hardware-1003` (do not use `master` for current robots) |
| **Control SDK** | `contrlSDK/` · `SDK_VERSION = 1003` |
| **Conda package** | `libmarvin` **`0.1.0`** on [prefix.dev/gabriel-robotics](https://prefix.dev/gabriel-robotics) |
| **Controllers** | 1003 protocol (e.g. `OnGetIntPara("VERSION")` → `100391`) |
| **Other branch** | `master` → `libmarvin` **0.2.x** / `contrlSDK100343` (100343+ protocol only) |

详细分支策略见 [docs/BRANCHES.md](docs/BRANCHES.md)，打包说明见 [docs/LIBMARVIN.md](docs/LIBMARVIN.md)。

---

## Quick start

```bash
git checkout hardware-1003
pixi install
pixi run build
pixi run validate-offline
```

真机验证（可选，默认 IP `10.19.0.191`，或 `export MARVIN_IP=10.19.0.191`）：

```bash
pixi run validate-hardware
```

确认安装的是 1003 线 SDK：

```bash
grep SDK_VERSION .pixi/envs/default/include/marvin/Robot.h   # 应为 1003
```

---

## Build `libmarvin` (conda)

本分支 **只发布** `libmarvin` **0.1.x**。不要在 `hardware-1003` 上发布 0.2.x。

```bash
pixi run version-check          # CMake / pixi / recipe 均为 0.1.0
pixi run conda-build            # → dist/linux-64/libmarvin-0.1.0-*.conda
pixi run conda-smoke            # 本地 consumer 测试
pixi run conda-upload           # 上传到 gabriel-robotics
```

CMake 编译与 vendor `contrlSDK/makefile` 相同的 10 个源文件，输出 `libMarvinSDK.so`，导入目标为 `libmarvin::libmarvin`。

---

## ros2_workspace

```toml
# pixi.toml
libmarvin = ">=0.1.0,<0.2"
```

```bash
pixi update libmarvin
pixi run colcon build --packages-select marvin_hardware_interface
ldd install/marvin_hardware_interface/lib/libmarvin_hardware_interface.so | grep -i marvin
```

应加载 pixi 环境中的 `libMarvinSDK.so.0`，且 header 中 `SDK_VERSION` 为 **1003**。

控制器升级到 vendor **100343+** 协议后，改用 `master` 分支与 `libmarvin >=0.2.0,<0.3`。

---

## SDK 源码布局

```
contrlSDK/          # 1003-line control SDK（本分支唯一控制 SDK 源码）
  MarvinSDK.h       # C API
  Robot.h           # SDK_VERSION 1003
  makefile          # vendor 参考编译命令
examples/           # libmarvin 验证程序（link_readonly, pkg_probe, …）
kinematicsSDK/      # 运动学 SDK（未打入 libmarvin conda 包）
DEMO_C++/           # vendor 示例（需自行改 IP / 链接 libMarvinSDK）
DEMO_PYTHON/
```

读取控制器参数前需按 vendor 顺序清错并发送（见 `examples/pkg_probe.cpp` 或 `DEMO_C++/showcase_get_set_param_demo.cpp`）：

`OnLinkTo` → `OnClearSet` → `OnClearErr_A/B` → `OnSetSend` → `OnGetIntPara(...)`

仅订阅 UDP 状态（`OnGetBuf`）时可只连接，不保证参数通道可用。

---

## Vendor 文档与示例

本仓库仍包含天机 Marvin 上游文档与 demo，使用时请以 **`contrlSDK/`（1003 线）** 为准，不要链接 `contrlSDK100343/`（该目录仅存在于 `master` 分支）。

| 文档 | 路径 |
|------|------|
| C++ 控制 SDK | [c++_doc_contrl.md](c++_doc_contrl.md) |
| Python 控制 SDK | [python_doc_contrl.md](python_doc_contrl.md) |
| C++ 运动学 SDK | [c++_doc_kine.md](c++_doc_kine.md) |
| Python 运动学 SDK | [python_doc_kine.md](python_doc_kine.md) |
| C++ Demo | [DEMO_C++/readme.md](DEMO_C++/readme.md) |
| Python Demo | [DEMO_PYTHON/readme.md](DEMO_PYTHON/readme.md) |

Linux 下手动编译控制 SDK（与 conda 包同源）：

```bash
cd contrlSDK
g++ MarvinSDK.cpp Robot.cpp ACB.cpp FXDG.cpp PointSet.cpp \
    FileOP.cpp FilePortal.cpp Parser.cpp TCPAgent.cpp TCPFileClient.cpp \
    -Wall -O2 -fPIC -shared -o libMarvinSDK.so -lpthread -lrt -DCMPL_LIN
```

---

## 注意事项

1. 先熟悉 MarvinPlatform / 上位机操作逻辑，再写生产代码。
2. Demo 中的速度、sleep、刚度等参数为测试参考值，现场需自行调整。
3. 任务结束调用 `OnRelease()`，避免占用 UDP 端口。
4. **`master` 分支的 `libmarvin` 0.2.x 与当前现场 1003 控制器不兼容**；混用会出现参数读失败或版本告警。

---

## License

Apache-2.0 — see [LICENSE](LICENSE).
