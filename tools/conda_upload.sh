#!/usr/bin/env bash
set -euo pipefail

channel="${PREFIX_CHANNEL:-gabriel-robotics}"
packages=(dist/linux-64/libmarvin-*.conda)

if (( ${#packages[@]} == 0 )); then
  echo "No package found under dist/linux-64/. Run 'pixi run conda-build' first." >&2
  exit 1
fi

if (( ${#packages[@]} > 1 )); then
  echo "Expected exactly one libmarvin package, found ${#packages[@]}:" >&2
  printf '  %s\n' "${packages[@]}" >&2
  echo "Run 'pixi run conda-build' to recreate a clean dist/ directory." >&2
  exit 1
fi

rattler-build upload prefix \
  --channel "${channel}" \
  "${packages[0]}"
