#!/usr/bin/env bash
set -euo pipefail

bash tools/check_versions.sh

rm -rf dist
rattler-build build \
  -r recipe \
  --channel conda-forge \
  --channel https://prefix.dev/robostack-jazzy \
  --output-dir dist
