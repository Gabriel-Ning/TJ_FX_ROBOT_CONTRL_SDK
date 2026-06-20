# libmarvin (hardware-1003 branch)

Conda/pixi packaging for the **1003-line** Marvin control SDK (`contrlSDK/` → `libMarvinSDK.so`).

| Item | Value |
|------|--------|
| Branch | **`hardware-1003`** |
| Conda package | `libmarvin` |
| **Version** | **`0.1.0`** (only this line on this branch) |
| Channel | [prefix.dev/gabriel-robotics](https://prefix.dev/gabriel-robotics) |
| Controller | 1003 protocol (e.g. `VERSION=100391`) |

The **100343-line** SDK is **`libmarvin` 0.2.x** from branch `master` — never mix versions on the same controller.

## Quick start

```bash
git checkout hardware-1003
pixi install
pixi run build
pixi run validate-offline
export MARVIN_IP=10.19.0.191   # optional
pixi run validate-hardware
```

## Publish

```bash
pixi run version-check    # must print 0.1.0
pixi run conda-build      # dist/linux-64/libmarvin-0.1.0-*.conda
pixi run conda-upload
```

## ros2_workspace

```toml
libmarvin = ">=0.1.0,<0.2"
```

After `pixi update libmarvin`, confirm:

```bash
grep SDK_VERSION .pixi/envs/default/include/marvin/Robot.h   # expect 1003
```

See [BRANCHES.md](BRANCHES.md) for the full two-branch policy.
