<a id="남은-과제"></a>

# Roadmap

<!-- language-nav:start -->
[한국어](../../public/roadmap.md) | [English](roadmap.md)
<!-- language-nav:end -->

This describes game v0.16. I saw no abnormal behavior while playing the installed iPhone and iPad apps. The following fixes and tests remain.

<a id="버그와-성능"></a>

## Bugs and performance

| Task | Status and next work |
|---|---|
| No sound after an external alarm | Sound reportedly disappears after an alarm app sends HeavyMach to the background and it returns. Record interruption/resume, audio-session and OpenAL output state together, then test menu and weapon sound recovery. The cause is not established. |
| iPad fighter entry timing | v0.16 entry callback-interval p95 was 21.39ms, above 20ms. Compare against the same pre-fix conditions. iPhone entry/combat and iPad combat passed. |
| Earlier display-time failures | Reproduce and isolate outstanding iPhone menu and shared-module timing failures by condition. |
| Long sessions | Measure heat, power and memory during movement, jumping, firing and mission changes. Investigate Mac HUD-editing processing time too. |
| Build warnings | Full audit pending. Deprecated APIs are a suspected cause. Collect warnings by platform, regular/cheat and Debug/Release configuration, then diagnose and fix. |

<a id="조작과-게임-기능"></a>

## Controls and game features

| Task | Plan |
|---|---|
| Auto-aim status | Distinguish no target, off-screen targets and terrain-hidden targets visually. Retain current target selection, barrel and projectile calculations. |
| Tutorial | Research adding five missions, 0-0 through 0-4, above existing packages while preserving missions and saves. Teach tilt, movement, jumping, weapons, reloads, special attacks and auto-aim through actual input. |
| Added-enemy difficulty | Check ammunition, damage, special-attack use and completion difficulty across the full campaign with new ground and air enemies. |
| Formation combat behavior | Spawn and entry spacing shipped in v0.16. Post-entry formation keeping, evasion and leaving/re-entering the screen are not implemented. |
| Physical keyboards | Test Bluetooth/USB connection, reconnection, held keys and combined touch input. |
| Enemy AI and wider screens | Review fixed movement/activation ranges affected by screen width. |

<a id="빌드와-플랫폼"></a>

## Builds and platforms

- A script connecting download, resource checks, OpenAL preparation, build and launch. Choose Mac launch or iPhone/iPad installation, regular or cheat app, with development-team/signing guidance.
- Full wireless XCTest/Instruments runs on iPhone and repeated installation/debugging after reboot on both devices.
- Android regular and cheat APKs run on Galaxy. Full campaign completion, extended thermal tests, physical tablet tests and distribution signing remain. [Development account](../../public/android-development-20261030.md).
- Explore folded-screen layouts with the game above and controls below.
- Record the release tool's actual GitHub upload/publication in the next new version. Existing v0.9–v0.16 releases are organized.

[Implementation status](implementation-status.md) · [Validation](validation.md) · [Changelog](../../../CHANGELOG.en.md) · [Detailed requirements and work records](../../usability-todo.md)
