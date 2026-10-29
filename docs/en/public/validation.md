<a id="heavymach-검증-결과"></a>

# HeavyMach validation results

<!-- language-nav:start -->
[한국어](../../public/validation.md) | [English](validation.md)
<!-- language-nav:end -->

The current game version is v0.16. The tables record tests and measurements from their respective versions. Codex ran automated tests and measurements; I played all missions myself.

<a id="플레이와-기능-검사"></a>

## Play and functional tests

| Date or target | Test | Result |
|---|---|---|
| v0.9 personal playthrough | All missions in the installed iPhone app | No observed crashes, malfunctions or abnormal enemy movement |
| October 26 fresh remote clone | Four builds, 17 core tests, 14 iOS tests, regular and cheat launch in a new Simulator | Passed, including decoding comparisons for 695 images at that time |
| October 26–27 native Mac | Development signing, unlocked session, movement, jumping, aiming, firing, 20 weapons, menus, GL, audio and full screen | 33/33 passed, including 280 weapon-selection combinations |
| October 27 iPhone Air | iOS 27.0.1, complete shared-module cheat-app suite | 84/85 passed; one display-time comparison failed |
| October 27 iPad Air 13-inch M2 | iPadOS 27.0.1, same suite | 84/85 passed; one display-time comparison failed |
| C removal `15968772` | 20 core tests; four input tests each on Simulator, iPhone and iPad | Passed |
| Manual firing after C removal | Compared with pre-feature `8fdeb629`: 20 weapons × 3 seeds × 420 frames | 25,200 frames matched byte for byte |
| VTOLs | Preserved 1,182 placements; 113 additions; movement, attacks, attitude, pause, destruction; 60 weapon collision cases | Passed |
| Fighters `45befedc` | 21 core tests; four combat/input tests each on Simulator, iPhone, iPad and Mac, plus HUD tests | Passed, including later off-screen entry tests |
| v0.16 entry placement | 140 conditions, 23 core tests, three functional tests each on iPhone and iPad | Passed. iPad entry p95 of 21.39ms exceeded the 20ms threshold |
| Personal play of the installed apps | Regular iPhone and iPad apps | No observed frame drops or malfunctions |

The 84 passing device tests include selecting and firing 20 weapons by keyboard input, combined touch input, resources, lifecycle, audio and zero save calls during combat. Codex used Device Hub OS key input to check movement, jumping, aiming, firing and weapon switching. Tests with actual Bluetooth and USB keyboards remain.

[Fresh clone](../../public/clone-build-20261026.md) · [Apple tests](../../public/apple-platform-progress-20261026.md) · [Auto-aim](../../public/auto-aim-ipad-20261027.md) · [VTOLs](../../public/vtol-art-and-loadout-20261027.md) · [Fighters](../../public/lux-lunae-design-20261027.md)

<a id="프레임메모리-계측"></a>

## Frame and memory measurements

| Target and conditions | Observation | Result and remaining work |
|---|---|---|
| Native Mac, development signed, unlocked, no profiler attached; 30 warm-up and 180 measured frames | Six scenes at 1200×800 and 1920×1080: p95 10.86–14.29ms, mean callback interval 16.67–16.69ms | Passed the 16.67ms processing threshold for those scenes |
| Mac regular and cheat apps, development certificate | Zero leaks | Resolved 417 connection-leak allocations totaling 20,016 bytes in the earlier ad-hoc app |
| iPad Simulator, Apple Software Renderer | Working build p95 44.311–48.133ms; previous main 39.972–40.580ms | Both failed. Software pixel processing dominated the final enlarged presentation |
| Follow-up iPhone menu measurement | p95 17.309ms, repeated at 17.629ms | Above 16.67ms; cause and reproduction conditions need further checks |
| Initial iPhone combat, 127.77 seconds | Mean 59.71 callbacks/second, p95 interval 16.67ms, maximum 50.02ms, thermal Serious | Mostly firing from a fixed position; no extended moving-combat or power comparison |
| Short iPad measurement | Zero leaks, physical footprint 493.2MB | Includes XCTest and screen recreation; long-session regular-play memory test needed |

Simulator measurements used Apple Software Renderer. Device-specific iPhone/iPad measurements and personal play results appear above.

A 3840×2160 OS capture confirmed that the Mac game reached all four screen edges. White margins visible in CUA window captures were absent from the actual display capture. [Measurement JSON](../../evidence/apple-platform-performance-20261026.json), [full-screen JSON](../../evidence/mac-fullscreen-capture-20261027.json)

<a id="설치무선-디버깅"></a>

## Installation and wireless debugging

Codex installed regular and cheat apps on iPhone and iPad. Device connection, installation, launch and LLDB attachment were also checked with cables disconnected. On iPad, eight wireless tests and 21.716 seconds of startup CPU activity were recorded. Complete wireless XCTest and Instruments runs on iPhone and repeated tests after reboot remain.

[Wireless procedure](../../wireless-device-workflow.md) · [Result JSON](../../evidence/wireless-apple-20261027.json)

<a id="문서미디어-확인"></a>

## Documents and media

The current resource check matched SHA-256 hashes for 1,504 build and test inputs. Codex extracted eight images and three MP4s from my play recordings. Two GIFs use short sections of those same scenes. The [scene manifest](../../public/media/gameplay-20261027/media-scenes.json) records source hashes, times and output hashes. The recording device, app type and build number were not supplied.

<a id="남은-검사"></a>

## Remaining tests

- Display timing: reproduce and isolate the iPhone menu threshold failure and the corresponding iPhone/iPad suite failures.
- Long sessions: heat, power and memory during movement, jumping, combat and mission transitions.
- Input: physical Bluetooth/USB keyboard connection, disconnection, reconnection, held keys and combined touch input.
- Difficulty: ammunition, damage and completion difficulty across all missions with added aircraft.
- Wireless: iPhone XCTest/Instruments and repeated installation/debugging after reboot on both devices.

[All-weapon test procedure](../../weapon-validation-procedure.md) · [Current tasks](implementation-status.md)
