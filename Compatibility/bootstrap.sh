#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
export GIT_CONFIG_GLOBAL=/dev/null
cd "$ROOT"
# Pin the external interpreter source. Never overwrite an existing checkout.
if [ ! -d compat-source ]; then
  git clone --filter=blob:none --no-checkout https://github.com/moleworld-dev/MoleWorld-5.5.0-touchHLE-offline.git compat-source
  git -C compat-source sparse-checkout set fresh-port/20-touchHLE-src/touchHLE
  git -C compat-source checkout --detach ebf9433e2bfd32071545bf49484b1345d15522a3
  git -C compat-source apply --check "$ROOT/patches/heavymach.patch"
  git -C compat-source apply "$ROOT/patches/heavymach.patch"
fi
test "$(git -C compat-source rev-parse HEAD)" = ebf9433e2bfd32071545bf49484b1345d15522a3 || { echo 'Existing source commit differs; preserve and inspect it.' >&2; exit 1; }
git -C compat-source apply --reverse --check "$ROOT/patches/heavymach.patch" || { echo 'Existing source does not match the recovery patch; inspect it without resetting.' >&2; exit 1; }
SDL_SOURCE="$ROOT/compat-source/fresh-port/20-touchHLE-src/touchHLE/vendor/rust-sdl2"
if [ ! -d "$SDL_SOURCE" ]; then
  git clone --no-checkout https://github.com/touchHLE/rust-sdl2.git "$SDL_SOURCE"
  git -C "$SDL_SOURCE" checkout --detach b67f98fe6a147773b6f05d50ae775d9fe16561e7
  git -C "$SDL_SOURCE" apply "$ROOT/patches/rust-sdl2.patch"
fi
test -d "$SDL_SOURCE/.git" || { echo 'SDL source has no checkout metadata; inspect before replacing.' >&2; exit 1; }
test "$(git -C "$SDL_SOURCE" rev-parse HEAD)" = b67f98fe6a147773b6f05d50ae775d9fe16561e7 || { echo 'SDL source commit differs.' >&2; exit 1; }
git -C "$SDL_SOURCE" apply --reverse --check "$ROOT/patches/rust-sdl2.patch"
export CARGO_HOME="$ROOT/toolchain/cargo" RUSTUP_HOME="$ROOT/toolchain/rustup"
export PATH="$CARGO_HOME/bin:/opt/homebrew/bin:/usr/bin:/bin:/usr/sbin:/sbin"
if [ ! -x "$CARGO_HOME/bin/rustup" ]; then
  mkdir -p "$ROOT/toolchain"
  curl --fail --location https://sh.rustup.rs -o "$ROOT/toolchain/rustup-init.sh"
  sh "$ROOT/toolchain/rustup-init.sh" -y --no-modify-path --default-toolchain 1.99.1
fi
rustup toolchain install 1.99.1 --profile minimal
rustup target add --toolchain 1.99.1 aarch64-apple-ios aarch64-apple-ios-sim
command -v cmake >/dev/null || { echo 'Install CMake from Homebrew: brew install cmake' >&2; exit 1; }
python3 - <<'PY'
import hashlib,json
from pathlib import Path
p=Path('originals/Heavy_Mach_1.13_ios_2.1.ipa')
if not p.exists(): raise SystemExit('Place the preserved original IPA in originals/; see originals/README.md')
assert hashlib.sha256(p.read_bytes()).hexdigest()==json.loads(Path('upstream.json').read_text())['ipa_sha256'], 'IPA hash mismatch'
PY
printf '%s\n' 'Ready: open ios/HeavyMachRecovery.xcodeproj'
