# libmarvin branch strategy

Two long-lived branches publish **different conda versions** of the same package name (`libmarvin`) to [gabriel-robotics on prefix.dev](https://prefix.dev/gabriel-robotics). Only one version line is valid per controller generation — use semver to tell them apart.

| Branch | SDK source | `SDK_VERSION` | **Conda version** | Publish from |
|--------|------------|---------------|-------------------|--------------|
| `hardware-1003` | `contrlSDK/` | `1003` | **`0.1.x` only** | `hardware-1003` branch only |
| `master` | `contrlSDK100343/` | `100343007` | **`0.2.x` only** | `master` branch only |

**Do not** publish `0.1.x` from `master` or `0.2.x` from `hardware-1003`. After clearing the channel, republish exactly one artifact per version line.

## ros2_workspace pinning

```toml
# Field hardware — 1003 protocol (hardware-1003 branch packages)
libmarvin = ">=0.1.0,<0.2"

# Controllers on 100343+ protocol (master branch packages)
libmarvin = ">=0.2.0,<0.3"
```

Verify what you installed:

```bash
grep SDK_VERSION .pixi/envs/default/include/marvin/Robot.h
# 1003      → correct for field hardware
# 100343007 → 100343-line package (master / 0.2.x)
```

## `hardware-1003`

- Build and publish **`libmarvin=0.1.0`** only.
- Validation: `pixi run validate-offline`, `pixi run validate-hardware`.

## `master`

- Build and publish **`libmarvin=0.2.0`** only.
- For controllers upgraded to vendor **100343+** protocol.

See [LIBMARVIN.md](LIBMARVIN.md) for build/upload commands.
