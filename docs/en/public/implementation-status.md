<a id="구현-상태"></a>

# Implementation status

<!-- language-nav:start -->
[한국어](../../public/implementation-status.md) | [English](implementation-status.md)
<!-- language-nav:end -->

As of October 30, 2026, the game is **v0.17**, which adds regular and cheat Android apps. v0.16 changed Lux Lunae horizontal and vertical entry spacing. It passed 23 core tests and three functional tests each on iPhone and iPad. The following work was completed in v0.15: text-free app icons and removal of SET GAME UI from mission selection. HUD editing opens from the combat pause menu. Three tests each on iPhone and iPad Simulators and two native Mac tests passed; regular and cheat apps were installed and launched wirelessly on both devices. [Screens and tests](../../public/app-icon-and-media-20261029.md).

<a id="구현된-기능"></a>

## Implemented features

| Feature | Current behavior | Checks and evidence |
|---|---|---|
| Combat, progression and missions | Movement, jumping, weapons, special attacks, enemies, bosses, experience, upgrades and saves | My full-mission playthrough; core and app tests |
| Graphics, audio and timing | OpenGL, OpenAL reuse, one update per display callback, event-based saves | Calculation, resource, save and audio tests; some display-time failures remain |
| Wider screens | Visible world follows aspect ratio; expanded-area touch; result background | iPhone, iPad and Mac display/input tests |
| Menus and doors | Connected central and side artwork; full-screen doors | Aspect-ratio, transition and sound tests |
| Landscape and lifecycle | Both landscape orientations; pause on lock, background or 120 seconds idle; manual resume | Device tests and my checks |
| Apple modules | Shared C++ core, Apple runtime and resource bundle; AppKit Mac | Builds and launch on three platforms |
| Keyboard | Full combat/menu control, Shift fine adjustment, remapping, iPad touch coexistence | API/OS key input checked; physical-keyboard tests remain |
| Auto-aim | Toggle, target cycling, barrel adjustment, HUD indicators; original projectile rules | v0.11; 25,200 manual-firing frames matched after C removal |
| Three VTOLs | 113 additions in 14 sub-missions; existing placements preserved | v0.10; movement, weapons, collision and device checks |
| Three fighters | 72 reinforcements over 14 events; gold glass, grouped missiles, off-screen entry | v0.12; core and three-platform tests and my play |
| Two ground enemies | Shilka and Tunguska; 152 vehicles across 35 stages; independent radar; 6/4 gun rounds and 2 missiles | v0.13; [final artwork and tests](../../public/ground-aa-art-20261028.md#두-포탑-높이-맞춤) |
| HUD editing | Move buttons, gauges and decorations; preview, cancel, apply and reset | v0.14; 13 tests each on physical iPhone/iPad; v0.15 entry-change tests |
| App icons | Text-free shared Apple asset catalog; Android PNG prepared | v0.15; four Apple builds passed. [Artwork and material](../../public/app-icon-and-media-20261029.md) |
| Cheat app | Set mission, weapon and upgrade conditions; saves separate from regular app | iPhone, iPad and Mac |
| Wireless development | iPhone/iPad installation, launch and LLDB | Reboot repetitions and full iPhone profiling remain |

[Feature details](features.md) · [Measurements and conditions](validation.md)

<a id="이번-문서-작업"></a>

## Documentation work

The README, features, controls, installation, architecture, troubleshooting, development history, validation, status tables, changelog and credits have been written. Eight gameplay images, two GIFs and three MP4s are linked. [Writing material](../../public/publication-inventory.md) is organized for the SaaS session to prepare Apps and Mags.

GitHub documents and gameplay media have been merged into main. The game release tag is v0.17.

<a id="남은-기능과-연구"></a>

## Remaining features and research

| Task | Stage | Work |
|---|---|---|
| Added-enemy validation | Full-campaign and long-session testing remain | Track difficulty, heat and memory with new ground and air enemies |
| Build warnings | Full audit and fixes planned | Deprecated APIs are a suspected cause. Collect platform, regular/cheat and configuration logs; investigate alongside feature development |
| Build/run script | Examples researched; implementation planned | Combine resource download/checks, dependency/app builds, Mac launch and iPhone/iPad installation/signing guidance |
| Auto-aim indicators | [Display plan](../../public/auto-aim-visibility-plan-20261028.md); not implemented | Show no target, off-screen and underground targets while retaining the aiming algorithm |
| Tutorial | Research pending | Keep existing missions; add top package 0-0 through 0-4 covering tilt, weapons, switching and auto-aim using original UI, art and clear-condition conventions |
| Android | Regular and cheat apps implemented; Galaxy USB/wireless installation and gameplay checked | Full campaign completion, extended thermal tests, physical tablets and distribution signing |
| Folded screen | Concept recorded | Explore a game screen above controls and the associated input layout |
| Enemy AI and fixed coordinates | Follow-up preventive review | Check aircraft movement and activation ranges affected by screen width |

Remaining tests cover display timing, long-session heat/memory, physical keyboards, added-enemy difficulty and repeated wireless work. I saw no abnormal behavior in the installed iPhone and iPad apps. CI builds passed for all four Apple apps. [All remaining tasks](../../public/ground-units-research-20261028.md). [Work records and detailed requirements](../../usability-todo.md)

<a id="배포와-버전"></a>

## Distribution and versions

The repository became public on October 28, 2026. The unified report site requires owner login. Git tags run from v0.9 through v0.17; v0.17 Releases offers a source-and-resource archive. Build and install iPhone, iPad and Mac apps through Xcode. A fresh clone builds using Git/LFS resources. v0.9 predates resource registration and needs additional resource preparation to build that historical code. [Installation](build-and-install.md) · [Version restoration](../../repository-management.md)

<a id="android-구현--2026-10-30"></a>

## Android implementation · 2026-10-30

The [Android development account](../../public/android-development-20261030.md) covers the regular and cheat APKs, Galaxy USB and wireless installation, the shared core, the Android host, tests and fixes. The implementation was developed on `feat/android-app` and merged through develop into main. Existing Apple sources were preserved. Build Android from the v0.17 source. [Android installation](build-and-install.md#android-빌드와-설치), [Galaxy gameplay gallery](../../public/android-gameplay-20261030.md).
