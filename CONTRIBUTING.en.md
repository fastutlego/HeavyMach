<a id="heavymach에-기여하기"></a>

# Contributing to HeavyMach

<!-- language-nav:start -->
[한국어](CONTRIBUTING.md) | [English](CONTRIBUTING.en.md)
<!-- language-nav:end -->

See [development status](docs/en/public/implementation-status.md) for current features and remaining work, and [build and installation](docs/en/public/build-and-install.md) to run the game.

<a id="오류-제보"></a>

## Reporting a bug

Include the app type (regular or cheat), version, device and OS, mission, and steps to reproduce. Describe the expected and actual behavior separately. Attach a screen recording and logs if possible. Check save files for account and device identifiers before attaching them.

<a id="코드-변경"></a>

## Changing code

Create a working branch from `develop` and target `develop` in the PR. Only this repository's `develop` branch is merged into `main`.

```sh
git switch develop
git pull --ff-only
git switch -c fix/short-description
git lfs pull
python3 tools/verify_build_resources.py
cmake -S . -B build/core -DCMAKE_BUILD_TYPE=Debug
cmake --build build/core -j 4
ctest --test-dir build/core --output-on-failure
python3 tools/document_contract.py check
python3 tools/check_publication.py
```

Check the requirements, policies, code, and tests for the feature you change. If you change existing mission placements, projectile speed or spread, damage values, or save timing, describe the before/after behavior and reason in the PR. Asset changes also require updates to `docs/build-resources.json` and the relevant art records. Keep logs of failed tests.

For manuscripts, follow the [development background](docs/developer-background.md) and [writing materials](docs/public/publication-inventory.md). The [license scope](LICENSE-SCOPE.en.md) describes the terms for code, original assets, and third-party materials.
