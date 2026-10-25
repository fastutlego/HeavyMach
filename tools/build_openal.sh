#!/bin/bash
# Build the exact OpenAL Soft source preserved from the working IPA host.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SDK="${1:-iphonesimulator}"
case "$SDK" in iphonesimulator|iphoneos) ;; *) echo 'Usage: build_openal.sh [iphonesimulator|iphoneos]' >&2; exit 2;; esac
OUT="$ROOT/build/openal/$SDK"
# Xcode launched from Finder does not inherit the login shell's Homebrew PATH.
CMAKE="$(command -v cmake || true)"
if [[ -z "$CMAKE" ]]; then
  for candidate in /opt/homebrew/bin/cmake /usr/local/bin/cmake /Applications/CMake.app/Contents/bin/cmake; do
    if [[ -x "$candidate" ]]; then CMAKE="$candidate"; break; fi
  done
fi
if [[ -z "$CMAKE" ]]; then echo 'CMake is required to build preserved OpenAL Soft.' >&2; exit 1; fi
"$CMAKE" -S "$ROOT/vendor/openal-soft" -B "$OUT" \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT="$SDK" -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=15.0 -DCMAKE_BUILD_TYPE=Release \
  -DLIBTYPE=STATIC -DALSOFT_UTILS=OFF -DALSOFT_EXAMPLES=OFF \
  -DALSOFT_NO_CONFIG_UTIL=ON -DALSOFT_BACKEND_WAVE=OFF \
  -DALSOFT_REQUIRE_COREAUDIO=ON -DALSOFT_INSTALL=OFF
"$CMAKE" --build "$OUT" --target OpenAL --parallel 4
printf '%s\n' "$OUT/libopenal.a"
