# libmarvin branch strategy

Two long-lived branches maintain different Marvin control SDK generations.

| Branch | SDK source | `SDK_VERSION` | Package version | Use when |
|--------|------------|---------------|-----------------|----------|
| `master` | `contrlSDK100343/` | `100343007` | `0.2.x` | Controller firmware is **100343+** (new protocol). Track upstream updates here. |
| `hardware-1003` | `contrlSDK/` (pinned to `tested/TJ_FX_ROBOT_CONTRL_SDK-master`) | `1003` | `0.1.x` | Current field hardware on the **1003** line (e.g. controller `VERSION=100391`). |

## ros2_workspace pinning

```toml
# Field hardware (1003 line)
libmarvin = ">=0.1.0,<0.2"

# After controller upgrade to 100343+
libmarvin = ">=0.2.0,<0.3"
```

## Refresh hardware SDK snapshot

When re-syncing from the tested vendor tree:

```bash
git checkout hardware-1003
rsync -a --exclude='*.so' --exclude='*.dll' \
  tested/TJ_FX_ROBOT_CONTRL_SDK-master/contrlSDK/ contrlSDK/
pixi run build && pixi run validate-offline
```
