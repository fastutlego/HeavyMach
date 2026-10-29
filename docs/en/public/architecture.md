<a id="heavymach-코드-구조"></a>

# HeavyMach code structure

<!-- language-nav:start -->
[한국어](../../public/architecture.md) | [English](architecture.md)
<!-- language-nav:end -->

Game rules are implemented in the C++20 core. Objective-C++ connects them to Apple platforms. One workspace manages the shared modules and the iOS/iPadOS and Mac project dependencies. The apps use the same core and resources.

<a id="workspace와-프로젝트"></a>

## Workspace and projects

```text
HeavyMach.xcworkspace
├── Modules/HeavyMachModules.xcodeproj
│   ├── HeavyMachCore / HeavyMachValidationCore
│   ├── HeavyMachAppleRuntime / HeavyMachValidationRuntime
│   └── HeavyMachAssets.bundle
├── iOS/HeavyMachRestored.xcodeproj
│   ├── HeavyMachRestored       (iPhone + iPad)
│   └── HeavyMachValidation     (iPhone + iPad 치트)
└── macOS/HeavyMachMac.xcodeproj
    ├── HeavyMachMac
    └── HeavyMachMacValidation
```

These names describe module roles. Actual targets have `_iOS` or `_macOS` SDK suffixes. The regular and cheat cores build separately with `HM_VALIDATION` matching their apps. Cheat code and saves are separate from the regular app.

[Shared project settings](../../../Modules/project.yml) · [iOS settings](../../../iOS/project.yml) · [Mac settings](../../../macOS/project.yml)

<a id="모듈화를-지시한-이유"></a>

## Why I requested shared modules

I asked for shared code and assets that each app could import, rather than separate copies of the game for iPhone, iPad and Mac. Copying movement, ballistics, damage and enemy placement into each platform would make it easy to miss a fix on one platform. Linking the same core puts weapon and calculation changes into all three builds.

The workspace groups projects and build dependencies. Static frameworks share code; a bundle supplies resources to each app. This works without SPM. Apple's [guide to managing multiple projects](https://developer.apple.com/documentation/xcode/managing-multiple-projects-and-their-dependencies) describes project dependencies and shared build products.

iPhone and iPad use device families 1 and 2 of the same iOS target. Mac runs as an AppKit app, without Catalyst. Shared code compiles separately for each platform SDK.

<a id="소스와-자원"></a>

## Source and resources

| Location | Responsibility |
|---|---|
| [Sources/Core](../../../Sources/Core) | Movement, world, enemies, projectiles, damage, progression, save format, auto-aim and additional enemies |
| [Sources/iOS](../../../Sources/iOS) | Game view, HUD, menus, OpenGL and OpenAL. This directory also contains code used by the shared Apple runtime |
| [Sources/Apple](../../../Sources/Apple) | UIKit/AppKit and graphics-context differences; shared input connections |
| [Sources/Mac](../../../Sources/Mac) | AppKit window, app lifecycle and Mac entry point |
| [Resources](../../../Resources) | Game images, audio, maps, weapons, part data and test fixtures |
| [Art/Extensions](../../../Art/Extensions) | New artwork, including menu extensions and additional aircraft |
| [Tests](../../../Tests) | C++ core tests and Apple input, display, audio and save tests |

`HeavyMachAssets.bundle` packages OriginalImages, Images, Audio, RawData and extension artwork. The [build resource manifest](../../build-resources.json) records file lists and hashes. Git LFS downloads large resources.

<a id="한-프레임의-처리"></a>

## One frame

1. Touch, tilt and keyboard input update the game input state. The viewport and camera convert screen coordinates to world coordinates.
2. `GameSession` updates the player, enemies, weapons, projectiles, items and world. Auto-aim adjusts the barrel using the selected enemy's position and weapon values.
3. Game events reach the HUD. OpenGL draws textures, parts, projectiles, effects and menus.
4. Sound events use prepared OpenAL buffers and sources.
5. Save events write progress to a file. Projectile updates and individual key events do not write files.

The game updates once per display callback. The time accumulator that ran several catch-up ticks after a delay was removed. Pausing or entering the background stops game time, held-fire input and sound effects.

<a id="그래픽과-좌표"></a>

## Graphics and coordinates

OpenGL renders the game: OpenGL ES 1.1 on iOS, and desktop OpenGL compatibility on Mac. Textures are prepared at load time. The CPU does not recreate images every frame.

The canvas shows more of the world according to the device's aspect ratio. World, camera, UI and physical-pixel coordinates have separate roles. Buttons use the same layout calculation for their artwork and hit areas. Menus retain their central artwork with added side backgrounds. Doors cover the full canvas.

[Coordinate policy](../../coordinate-policy.md) · [Rendering policy](../../rendering-policy.md) · [Timing, saves and audio](../../runtime-policy.md) · [Damage and progression calculations](../../game-calculation-policy.md)

<a id="android-계획"></a>

## Android plan

The plan is to build the shared C++ core with CMake and the NDK, then add Android window, input, audio and graphics connections. There is no Android app or APK yet. [Development stages and tests](../../android-port-plan-20261026.md)
