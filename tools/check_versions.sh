#!/usr/bin/env bash
set -euo pipefail

cmake_version="$(
  sed -n 's/^project(libmarvin VERSION \([^ ]*\) LANGUAGES CXX).*/\1/p' CMakeLists.txt
)"
pixi_version="$(
  sed -n 's/^version = "\([^"]*\)"/\1/p' pixi.toml | head -n 1
)"
recipe_version="$(
  sed -n 's/^  version: "\([^"]*\)"/\1/p' recipe/recipe.yaml | head -n 1
)"

if [[ -z "${cmake_version}" || -z "${pixi_version}" || -z "${recipe_version}" ]]; then
  echo "Failed to read one or more versions:" >&2
  echo "  CMakeLists.txt: ${cmake_version:-<missing>}" >&2
  echo "  pixi.toml:      ${pixi_version:-<missing>}" >&2
  echo "  recipe.yaml:    ${recipe_version:-<missing>}" >&2
  exit 1
fi

if [[ "${cmake_version}" != "${pixi_version}" || "${cmake_version}" != "${recipe_version}" ]]; then
  echo "Version mismatch:" >&2
  echo "  CMakeLists.txt: ${cmake_version}" >&2
  echo "  pixi.toml:      ${pixi_version}" >&2
  echo "  recipe.yaml:    ${recipe_version}" >&2
  echo "Keep these versions in sync before packaging." >&2
  exit 1
fi

echo "Version check passed: ${cmake_version}"
