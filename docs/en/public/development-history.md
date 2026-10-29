<a id="heavymach-개발-과정"></a>

# HeavyMach development history

<!-- language-nav:start -->
[한국어](../../public/development-history.md) | [English](development-history.md)
<!-- language-nav:end -->

I started this project to play HeavyMach, a game I played in college, on a current iPhone. I think it contributed about 35% to my decision to become a developer in July 2017. In 2026, I instructed Codex to analyze the original behavior and implement it again from source. [Developer's statement](../RESTORATION.md)

<a id="게임-규칙을-소스로-옮기기"></a>

## Turning game rules into source code

Codex first inventoried functions, strings and data, separating movement, weapons, enemies, progression, display and audio responsibilities. It compared pseudocode from REA and Ghidra with assembly. Arguments omitted from pseudocode, structure returns and Objective-C type information were checked against ARM calling conventions and raw metadata.

Codex implemented game rules in C++20, with display, input, audio and operating-system connections in Objective-C++. This remains the current structure. Weapon, aircraft, part and map readers came first, followed by movement, collision, projectiles, damage, experience, progression, saves, menus and HUD connected to actual data.

Matching values could still produce different results when operation order changed. Damage upgrades retain integer division order in `D + (D / 8) * G`. Attack's 1.5 multiplier uses floating-point calculation followed by truncation; Defense applies integer halving. Experience checks covered integer saturation, 32-bit wrap and at most one level gain per call. Floating-point calculations use intermediate rounding and `-ffp-contract=off`.

<a id="비교한-순서"></a>

## Comparison order

Codex compared code and runtime results in this order. I played installed apps and reported the conditions where differences appeared.

1. **Definitions and calculations:** Compare the same weapon, aircraft and map values and boundary inputs. This established data readers and damage and progression formulas.
2. **Identical game states:** Match position, weapon, upgrades and random state, then compare movement, firing, enemy AI, items and camera behavior. Check off-screen objects and random-number consumption in draw calls.
3. **Events and saves:** Check that World events reach the HUD before its tick, and that level-ups, reloading, menu returns and saves occur in the same order.
4. **Actual app paths:** Test touch and sensors through `HMGameView`, the core, HUD and OpenGL output. Fix input conversion and resource loading separately even after core tests pass.
5. **Performance:** Measure rendering, audio, file writes, timers and presentation separately. Compare causes of the frame drops and delays I reported.
6. **Play:** Repeat all weapons and mission conditions in the cheat app, then install the regular app. I played every mission and saw no crashes, malfunctions or abnormal enemy movement.

<a id="초기-구현에서-다시-고친-부분"></a>

## Reworking the initial implementation

While reproducing game results, the initial Codex implementation also changed how they ran. It wrote files for memory changes, generated images on the CPU and restarted effects through expensive calls. Its time accumulator changed how many times the game advanced per displayed frame.

I required direct comparisons with the version I had played successfully. I asked, “Why did you design it separately?” and “Why was the time accumulator added?” Codex separated save events, reused OpenGL and OpenAL resources and changed updates to once per display callback. [Problems and fixes](troubleshooting.md)

<a id="화면을-넓힌-뒤의-수정"></a>

## Fixes after widening the screen

I asked for more visible world instead of enlarged assets. After expanding the canvas, Codex tested side-area touches, special-attack impact coordinates, boss activation, HUD and result artwork together. Menus gained side backgrounds connected to the existing central artwork. Codex fixed the joins and black lines I found, and made door transitions fill the screen.

Filling the screen with artwork and extending game coordinates required different code. Codex checked whether earlier enemy visibility also advanced combat activation, and corrected the aircraft boss in mission 3-5.

<a id="공용-모듈과-ipadmac"></a>

## Shared modules, iPad and Mac

I requested one workspace for Apple platforms, with shared code and resources in modules. Codex expanded the iOS app to iPhone and iPad device families and built Mac with AppKit. Keyboard controls cover movement, jumping, firing, barrel adjustment, weapon selection and menus.

Codex's Mac tests were delayed by presentation waits and system connection leaks. Measurements and system logs showed rejected app registration and connection retries under ad-hoc signing without development-team information. Regular and cheat apps with development signing had zero leaks. After display-path fixes, 33 Mac tests passed. iPhone and iPad each passed 84 of 85 tests, with one display-timing failure remaining. [Platform evidence](validation.md)

<a id="자동조준과-항공기-추가"></a>

## Auto-aim and additional aircraft

Codex developed auto-aim on a separate branch using the iPad cheat app. I required forward target selection, off-screen tracking, switching after kills and barrel adjustment for tank position. Codex added an ON/OFF switch between touch firing and auto-aim, plus a dedicated fire button.

The C correction Codex added to improve hits also changed projectile speed and spread. I instructed its removal. Codex compared 25,200 frames of firing with auto-aim OFF against the pre-feature code, confirmed matching results and included the change in v0.11.

I asked for VTOL additions based on average helicopter counts per mission while preserving existing placements. Codex calculated 113 additions and implemented the aircraft and placement code. Initial artwork that differed from the real models was redrawn, then outlines, sizes and flames were corrected. F-22, Su-57 and MiG-31 fighters followed as reinforcements in boss fights and preceding sections. Codex added gold glass, grouped missile bursts and off-screen entry. I later reduced the firing-rate increase to 10%, and Codex changed the values. This entered v0.12. [Intent and implementation](features.md)

[Why Lux Lunae?](lux-lunae-intent-20261028.md) describes the personal significance of both games, why I wanted strong enemies, what I was testing through text instructions and the actual production results.

<a id="사용한-도구와-스킬"></a>

## Tools and skills used

| Tool or skill | Use and results |
|---|---|
| reverse-engineer-anything | Native analysis, function/string/metadata inspection, comparison of static analysis with runtime observations |
| connection-and-recovery | Separately checked skill installation, server registration, session connection and engine execution; prepared CLI and separate MCP-client paths |
| native-and-artifacts | ARM/Thumb functions, Objective-C classes, types and assembly |
| evidence-workflows | Connected input hashes, Evidence IDs, analysis files and derived results |
| runtime-observation | Observed runtime samples in matched states; checked registers and return buffers through a separate GDB tool |
| REA CLI and Ghidra | Preserved unique Evidence for 951 pseudocode and 902 assembly results, including two versions and trial analyses |
| Python, otool and byte parsers | Worked around old metadata-tool errors; data readers, resource hashes and parity checks |
| engineering-debug / test / review | Reproduction, cause isolation, core/app regression tests and implementation review |
| CMake, Xcode and XCTest | Core, regular/cheat apps and platform input, graphics and save tests |
| Instruments and leaks | CPU, display, allocation and leak measurements; comparisons before and after signing changes |
| Device Hub and CUA | Device installation, key/touch input, menus, combat, windows and full-screen checks |
| repair-computer-use | Diagnosed and repaired screen-control connection and authentication errors |
| game-keyboard-input | Key layout, held input, focus loss, touch coexistence and remapping |
| apple-wireless-device | Wireless iPhone/iPad installation, launch and LLDB procedures and results |
| imagegen / integrate-game-art | Menu extensions and aircraft artwork; real shapes and game colors; resource-to-game integration |
| write-project-text | Developer source text, implementation/test results, GitHub prose and captions; includes SaaS and Candidate writing rules |
| present-game-artifacts / Sites | Gathered approved artwork and manuscripts in the existing report site and maintained document/image links |
| FFmpeg and ffprobe | Frames, clips and GIFs from actual play recordings, with source timestamps and hashes |

The REA operation record includes `inspect_artifact`, `inventory_artifact`, `extract_artifact`, `inspect_plist`, `binary_overview`, `read_binary_inventory_summary`, `list_procedures`, `list_strings`, `search_procedures`, `search_strings`, `analyze_function`, `get_objc_classes`, `get_objc_protocols`, `procedure_pseudo_code` and `procedure_assembly`. These record successive inspection from file structure and metadata to function code.

REA analysis was grouped into 25 function families: geometry, data, saves, movement, player, enemies, combat, particles, trails, items, world, title, menus, HUD, transitions, camera, basic visual elements, buttons, containers, graphics resources, audio, platform, runtime, compiler connections and unnamed fragments. [Source coverage](../../source-coverage.md) and the [iOS source map](../../ios-source-map.md) connect the analysis inventory to implementation.

Ghidra, otool and byte parsers worked around Mach-O inspection errors in old tools. `_rea_probe` symbol-name errors, omitted structure arguments in pseudocode and floating-point/integer conversion differences were checked through commands, types and runtime samples. The analyzer's 56 external thunks were classified as entries without bodies in the file.

<a id="현재-버전과-다음-작업"></a>

## Current version and next work

v0.9 is the baseline I confirmed through play. v0.10 added shared modules, Apple platforms and VTOLs; v0.11 added doors, auto-aim and C removal; v0.12 added fighters and their entry and firing-rate fixes. v0.13 added Shilka and Tunguska, v0.14 HUD editing, v0.15 icons and the pause-menu entry, and v0.16 fighter entry spacing. [Changelog](../../../CHANGELOG.en.md)

GitHub contains current features, development history and gameplay media. Next game tasks include auto-aim status, tutorials and Android. Display timing, long-session heat and additional-enemy difficulty still need testing. [Current status](implementation-status.md)
