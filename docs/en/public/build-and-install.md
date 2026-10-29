<a id="heavymach-빌드와-설치"></a>

# Building and installing HeavyMach

<!-- language-nav:start -->
[한국어](../../public/build-and-install.md) | [English](build-and-install.md)
<!-- language-nav:end -->

Build the regular app and cheat validation app from HeavyMach v0.16 source. They share the game core and use separate app IDs and saves.

<a id="준비물"></a>

## Requirements

You need an Apple Silicon Mac, Xcode with the iOS SDK and Simulator runtime, Python 3, CMake 3.20 or later, and Git LFS. Xcode projects are included. Regenerate a project with XcodeGen when changing its `project.yml`.

The environment recorded on October 26, 2026 was macOS 27.0.1, Xcode 27.1, CMake 4.4.4, Python 3.14.6 and XcodeGen 2.45.4. Deployment targets are iOS/iPadOS 15.0 and macOS 12.0. The current OpenAL build script targets arm64.

<a id="소스와-자원-받기"></a>

## Download source and resources

```sh
git clone https://github.com/gelb2/HeavyMach.git
cd HeavyMach
git lfs install --local
git lfs pull
```

Images, audio, maps, app icons and image-comparison fixtures are included in the repository. Runtime resources are packaged in `HeavyMachAssets.bundle`.

The repository is public; downloading source does not require a login. `v0.9` is a product-code tag from before resources were added to Git. Use current `main`, which includes resources, for a new build. [v0.16 Releases](https://github.com/gelb2/HeavyMach/releases/tag/v0.16) provides a source-and-resource archive, notices and SHA-256 checksums. After extracting the archive, you can check resources without Git LFS. Choose your own development team in Xcode to build and install the apps.

<a id="자원-검사"></a>

## Check resources

```sh
python3 tools/verify_build_resources.py
```

This checks the size and SHA-256 of 1,504 build and test resources. Missing, damaged or undownloaded LFS files cause exit code 1. If LFS pointers remain, run `git lfs pull`, then check again.

<a id="시뮬레이터-빌드"></a>

## Simulator build

From the repository root, prepare the audio library, then build the regular app.

```sh
bash tools/build_openal.sh iphonesimulator
xcodebuild -workspace HeavyMach.xcworkspace \
  -scheme HeavyMachRestored -configuration Debug \
  -sdk iphonesimulator -destination 'generic/platform=iOS Simulator' \
  -derivedDataPath build/ios-simulator \
  ARCHS=arm64 ONLY_ACTIVE_ARCH=YES CODE_SIGNING_ALLOWED=NO build
```

The output is `build/ios-simulator/Build/Products/Debug-iphonesimulator/HeavyMachRestored.app`. For the cheat app, change the scheme to `HeavyMachValidation` in the same command. The output name becomes `HeavyMachValidation.app`.

Boot one iPhone in Simulator, then install and launch:

```sh
xcrun simctl install booted build/ios-simulator/Build/Products/Debug-iphonesimulator/HeavyMachRestored.app
xcrun simctl launch booted com.gelb2.heavymach.restored
```

If several Simulators are running, find the device ID with `xcrun simctl list devices booted` and replace `booted` in both commands with that ID. For the cheat app, change the app name and ID to `HeavyMachValidation.app` and `com.gelb2.heavymach.validation`.

<a id="iphoneipad-설치"></a>

## Install on iPhone or iPad

Open `HeavyMach.xcworkspace` in Xcode. Select your development team under **Signing & Capabilities** for the target. The repository contains the previous development team's ID. If registering the app ID causes a conflict, choose a unique Bundle Identifier for your team.

Connect the iPhone, trust the Mac and enable Developer Mode. Choose the connected iPhone or iPad as the destination. Run the `HeavyMachRestored` scheme for the regular app or `HeavyMachValidation` for the cheat app. The Xcode build phase prepares OpenAL for `iphoneos`.

Before updating an existing installation, download its container from Xcode's **Devices and Simulators** to back up saves. Keeping the same app ID updates the installed app. Changing the ID installs a separate app; previous saves do not transfer automatically.

`tools/prepare_device_validation.py` is fixed to the existing validation iPhone and app IDs. Use the Xcode procedure above for another iPhone.

<a id="실행-확인과-오류-해결"></a>

## Check the game and resolve build errors

In the regular app, proceed through title → slot → mode/mission → combat. In the cheat app, set starting conditions in the title-screen validation panel. Check movement, jumping, firing, weapon switching and progress restoration after returning to the menu. The [weapon test procedure](../../weapon-validation-procedure.md) covers all weapons.

| Symptom | Check |
|---|---|
| `libopenal.a` not found | Run `build_openal.sh` for the selected SDK first. |
| x86_64 link failure | The current library is arm64. Check `ARCHS=arm64` and the Apple Silicon destination. |
| Images missing | Run `git lfs pull` and the resource checker; inspect `HeavyMachAssets.bundle/OriginalImages` in the app bundle. |
| Map or weapon data missing | Run `git lfs pull` and the resource checker to check `Resources/RawData`. |
| Signing failure | Check Xcode's team, certificate and device-registration errors. Earlier CLI signing failures were resolved by building in the Xcode GUI. |
| Image-comparison fixture missing | Use the resource checker to verify `Resources/Validation/ImageDecodeParity.json`. |

Run core tests after resource checks pass.

```sh
cmake -S . -B build/core -DCMAKE_BUILD_TYPE=Debug
cmake --build build/core -j 4
ctest --test-dir build/core --output-on-failure
```

The [fresh remote-clone check](../../public/clone-build-20261026.md) passed four builds, 17 core tests, 14 iOS tests and launch checks for both apps in a new Simulator using GitHub source and LFS resources. See [validation results](validation.md) for platform-specific results.

<a id="mac-빌드와-실행"></a>

## Mac build and launch

Open the same workspace in Xcode. Choose `HeavyMachMac` or `HeavyMachMacValidation` and **My Mac**. Set your development team and Apple Development certificate in each app target's Signing & Capabilities, then Run. The AppKit app supports keyboard combat and menus.

For a build-only check:

```sh
bash tools/build_openal.sh macosx
xcodebuild -workspace HeavyMach.xcworkspace \
  -scheme HeavyMachMac -configuration Debug \
  -destination 'platform=macOS,arch=arm64' \
  -derivedDataPath build/mac-documentation \
  CODE_SIGNING_ALLOWED=NO build
```

The output is `build/mac-documentation/Build/Products/Debug/HeavyMachMac.app`. For the cheat app, change the scheme and output name to `HeavyMachMacValidation`. This command does not sign the app. Use an app signed with a development certificate in Xcode for running, debugging and leak tests. The [Mac test record](../../public/apple-platform-progress-20261026.md) documents system-registration retries and connection leaks with ad-hoc signing without a development team.

<a id="scheme과-앱-구분"></a>

## Schemes and apps

| Target | Regular | Cheat |
|---|---|---|
| iPhone, iPad, Simulator | HeavyMachRestored | HeavyMachValidation |
| Native Mac | HeavyMachMac | HeavyMachMacValidation |

Regular and cheat apps have separate IDs and saves. The iOS cheat app ID is `com.gelb2.heavymach.validation`. See [controls](gameplay-guide.md) for key bindings and the [wireless workflow](../../wireless-device-workflow.md) for wireless installation.

<a id="프로젝트-재생성"></a>

## Regenerate projects

When changing source files, module dependencies or build settings, update the YAML and generated project together.

```sh
xcodegen generate --spec Modules/project.yml
xcodegen generate --spec iOS/project.yml
xcodegen generate --spec macOS/project.yml
```

For everyday builds, use the projects included in the repository. [Workspace structure](architecture.md)
