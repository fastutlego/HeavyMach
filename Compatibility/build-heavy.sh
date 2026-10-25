#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
export CARGO_HOME="$ROOT/toolchain/cargo" RUSTUP_HOME="$ROOT/toolchain/rustup"
export PATH="$CARGO_HOME/bin:/opt/homebrew/bin:/usr/bin:/bin:/usr/sbin:/sbin"
export GIT_CONFIG_GLOBAL=/dev/null CARGO_NET_GIT_FETCH_WITH_CLI=true
export IPHONEOS_DEPLOYMENT_TARGET=15.0 CMAKE_POLICY_VERSION_MINIMUM=3.5
cd "$ROOT/compat-source/fresh-port/20-touchHLE-src/touchHLE"
cargo +1.99.1 build --locked --release --target "${1:-aarch64-apple-ios-sim}" --no-default-features --features static,cpu_interpreter --bin touchHLE
