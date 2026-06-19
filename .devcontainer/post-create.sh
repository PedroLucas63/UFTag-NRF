#!/usr/bin/env bash

set -euo pipefail

repo_url="https://github.com/ICantMakeThings/Nicenano-NRF52-Supermini-PlatformIO-Support.git"
tmp_dir="$(mktemp -d)"

cleanup() {
  rm -rf "$tmp_dir"
}

trap cleanup EXIT

git clone --depth 1 "$repo_url" "$tmp_dir/repo"
mkdir -p "$HOME/.platformio"
cp -a "$tmp_dir/repo/.platformio/." "$HOME/.platformio/"