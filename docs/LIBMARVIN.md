# libmarvin (master branch)

Conda/pixi packaging for the **100343-line** Marvin control SDK (`contrlSDK100343/` → `libMarvinSDK.so`).

| Item | Value |
|------|--------|
| Branch | **`master`** |
| Conda package | `libmarvin` |
| **Version** | **`0.2.0`** (only this line on this branch) |
| Channel | [prefix.dev/gabriel-robotics](https://prefix.dev/gabriel-robotics) |
| Controller | **100343+** protocol (not backward compatible with 1003 line) |

The **1003-line** field SDK is **`libmarvin` 0.1.x** from branch `hardware-1003`.

## Quick start

```bash
git checkout master
pixi install
pixi run build
pixi run validate-offline
```

## Publish

```bash
pixi run version-check    # must print 0.2.0
pixi run conda-build      # dist/linux-64/libmarvin-0.2.0-*.conda
pixi run conda-upload
```

## ros2_workspace (after controller upgrade)

```toml
libmarvin = ">=0.2.0,<0.3"
```

Confirm installed SDK line:

```bash
grep SDK_VERSION .pixi/envs/default/include/marvin/Robot.h   # expect 100343007
```

See [BRANCHES.md](BRANCHES.md) for the full two-branch policy.
