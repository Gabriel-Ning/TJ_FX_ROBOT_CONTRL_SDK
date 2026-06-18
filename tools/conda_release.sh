#!/usr/bin/env bash
set -euo pipefail

bash tools/check_versions.sh

channel="${PREFIX_CHANNEL:-gabriel-robotics}"

rm -rf dist
rattler-build publish \
  -r recipe \
  --channel conda-forge \
  --channel https://prefix.dev/robostack-jazzy \
  --to "https://prefix.dev/${channel}" \
  --build-number +1 \
  --output-dir dist
