# libmarvin branch strategy

Two long-lived branches maintain different Marvin control SDK generations.

| Branch | SDK source | `SDK_VERSION` | Package version | Use when |
|--------|------------|---------------|-----------------|----------|
| `hardware-1003` | `contrlSDK/` | `1003` | `0.1.x` | **Current field hardware** on the 1003 protocol line (e.g. controller `VERSION=100391`). |
| `master` | `contrlSDK100343/` | `100343007` | `0.2.x` | Controller firmware is **100343+** (new protocol). Track upstream updates here. |

## `hardware-1003` (default for ros2 today)

- `contrlSDK/` is the field-validated 1003-line SDK (formerly kept under a local `tested/` snapshot; that tree is now merged into `contrlSDK/` and removed).
- Build and publish **`libmarvin` 0.1.x** from this branch only.
- Validation: `pixi run validate-offline` and `pixi run validate-hardware` (robot IP via `MARVIN_IP` or default `10.19.0.191`).

## ros2_workspace pinning

```toml
# Field hardware (this branch)
libmarvin = ">=0.1.0,<0.2"

# After controller upgrade to 100343+ (master branch packages)
libmarvin = ">=0.2.0,<0.3"
```

## Refreshing `contrlSDK/` from vendor

When vendor ships an updated 1003-line SDK, replace sources under `contrlSDK/` on `hardware-1003`, keeping the same 10-file build set as `contrlSDK/makefile`, then:

```bash
git checkout hardware-1003
pixi run build
pixi run validate-offline
pixi run validate-hardware   # optional, needs robot
pixi run conda-build
```

See [LIBMARVIN.md](LIBMARVIN.md) for conda/pixi commands.
