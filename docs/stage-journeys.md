# Original map journeys

## 후속 치트 검증

사용자 요청으로 남은10개 맵을 치트 조건에서 검증해 **10/10클리어·보스9/9처치·저장후일반코어진입10/10**을 확인했다. 다음 맵8개·미션 메뉴 요청2개다. 무적·무한탄약과 낙하복귀1회를 사용했다. [조건·로그·한계](cheat-stage-check.md)를 따른다. 비치트25/35와 조건을 합산하지 않는다.

## Latest: original HUD event order

2026-10-24. **25 clears / 10 deaths / 0 timeouts**, exit1, using the same selected-35 controller and initial conditions.20 next-stage openings and5 mission-menu requests. Failures: Stage16/21/22/23/24/31/32/33/34/35. Stage06 and Stage17 now clear. There are34 upgrade confirmations and34 close callbacks;918 opening-wait ticks. This is35 independent starts, not a continuous campaign.

[HUD event correction](hud-event-order.md) changes both the actual iOS HUD bridge and its test-only model: input events precede the next World tick; World events precede the same HUD tick. Drawing no longer opens or closes an upgrade. The prior draw-sync model matched the former native implementation but differed from original GuiGame. The prior results below are historical, not the current behavior. Current iOS9/9 and CTest17/17 passed; the unsigned iPhone build succeeded. Evidence and exact source/output hashes: `build/hud-event-order/manifest.json`.

```sh
HeavyMach/build/core/HeavyMachValidationJourneyTests HeavyMach/build/hud-event-order/journeys.jsonl normal 18000 HeavyMach/build/hud-event-order/journeys.csv selected-35
```

The initial grades6, original finite ammunition, seed11, original spawn and controller strategy remain unchanged. No mid-run invulnerability/refill/warp is used. Deaths do not prove source defects. The legacy flag below reproduces the older camera-only/early-HUD model; it does not reproduce the immediately preceding draw-sync HUD model, whose source snapshot and results remain archived.

## Historical: exhaustion and passive lifetime

The original-order weapon exhaustion and shared passive lifetime changes were rerun with the same selected-35 controller, camera and HUD model. Result: **23 clears / 12 deaths / 0 timeouts**, exit1. All35 JSON result objects and every CSV trace byte match the prior HUD-host run. Failures remain Stage06/16/17/21/22/23/24/31/32/33/34/35. No new campaign success is inferred from this equivalence.

Evidence: `build/stage-journeys/session-lifecycle/after.{jsonl,csv,log}` and `manifest.json`. The manifest records source, runner and artifact hashes. Separate RED/GREEN tests exercise the newly fixed same-frame re-pickup and cross-stage/menu passive lifetime boundaries; this unchanged35-map run is a regression result, not proof those boundaries occurred. [Product changes and exact tests](session-lifecycle.md).


`Tests/ValidationJourneyTests.cpp` drives `GameSession` from each original map's normal left spawn. Each map starts an independent validation save. This does not test one continuous campaign or prove an ordinary grade-zero player can finish every stage.

## Historical: camera and draw-synchronized HUD

2026-10-24. Result at that revision: **23 clears / 12 deaths / 0 timeouts**, exit1, for35 independent natural-progression maps. This includes18 next-stage openings and5 mission-menu **requests**, not native menu rendering. Failures at that revision: **Stage06/16/17/21/22/23/24/31/32/33/34/35**. Stage11 now clears; Stage06 now fails. Do not merge successes from different host conditions to claim all35 succeeded.

The previous runner confirmed immediately, reset the closing panel to−240 and updated the close before the session tick. Actual HMGameHUD opens during tick/draw, retains its current X on confirmation, ticks after GameSession, and detects changed visibility during draw. The test-only `JourneyUpgradeHost.hpp` now follows that order. The controller waits for opening X>239 before confirming; this is a declared controller policy, not an original button restriction. A confirmation frame still ticks the previous opening state; closing begins after draw. World updates resume in the next session tick after the close callback.

The controller also waits for result buttonY>−1 (button centerY>25) before calling the result API. Previously it called as soon as presentation.step reached2, when the result button center could still be−24, outside the screen. Opening/confirmation/result waits change input timing without changing game rules. All-max parts retain the original selection100 and close without adding a grade. The model is not reset at a GameStart event.

Three new actual Air/iOS26.5 app-hosted XCTest cases compare the test model to `HMGameHUD`: first draw,28 opening ticks, confirmation-frame ordering,28 closing ticks, one callback, GameStart state preservation and interrupted hide/reopen. All3 passed. Existing7 audio/render cases were not rerun. Evidence: `build/upgrade-hud-lifecycle.log` and `.xcresult`.

| Execution | Next stage | Mission menu request | Death | Exit |
|---|---:|---:|---:|---:|
| Archived camera-host, early HUD |18|5|12|1|
| New runner, explicit legacy-early-hud |18|5|12|1|
| New runner, actual HUD lifecycle model |18|5|12|1|

Every old JSON field and CSV cell matched the archived camera run in the legacy control. The new default performed33 confirmations and33 close callbacks, with924 opening-wait ticks. Every successful result request occurred at centerY25.088. There were no Core, resource, HP, ammunition or physical-device changes. Source hashes matched the preceding verified build throughout both runs. Raw outputs, command lines and hashes are in `build/stage-journeys/hud-host/manifest.json`.

```sh
HeavyMach/build/core/HeavyMachValidationJourneyTests /tmp/hud-journeys.jsonl normal 18000 /tmp/hud-journeys.csv selected-35
# Reproduce the preceding camera-only run:
HeavyMach/build/core/HeavyMachValidationJourneyTests /tmp/hud-legacy.jsonl normal 18000 /tmp/hud-legacy.csv selected-35 host-camera legacy-early-hud
```

Remaining limits: this models one draw per60Hz tick; real CADisplayLink catch-up can run multiple ticks before one draw. Weapon selection, booster, confirmation and result selection still use public APIs without native button hit-tests or touch-down/up timing. Each fixture begins global time0. Menu success stops at ChangeScreen request. These35 runs do not establish a continuous campaign, full UIKit equivalence or original-runtime gameplay equivalence. Controller deaths alone do not establish product defects.

## Historical camera-only host run

The camera-only runner result was **23 clears / 12 deaths / 0 timeouts**, exit1, across35 independent natural-progression maps. Earlier27/35 results remain valid only for their zero-camera-offset harness condition. They are not the current app-camera result.

`HMGameView` advances `CameraShake` on global time, sets `GameSession` camera offset before its tick, and consumes PlayerUI/WorldUI quake events afterward. The journey runner previously omitted this feedback. Offset affects enemy screen gates, collision eligibility and touch aim, not only drawing. The runner now includes it by default and subtracts the same previous-frame offset from target and dash touch coordinates. No game source, HP, ammo, damage or map changed.

Evidence: `build/stage-journeys/camera-host/manifest.json` and `before`, `legacy-control`, `after` JSONL/CSV/log files. All35 maps emitted quake events and used nonzero offsets, totaling6564ticks. The two existing Core/CameraIntegration CTests passed. Product source hashes still match the preceding audio/iOS verified build.

| Execution | Next stage | Mission menu | Death | Exit |
| --- | ---: | ---: | ---: | ---: |
| Before runner edit |22|5|8|1|
| Edited runner, `legacy-zero-camera` |22|5|8|1|
| Edited runner, camera feedback |18|5|12|1|

Every existing JSON field and CSV cell matched between before and legacy-control. The before JSON also exactly matched archived `selected-35.jsonl`. Thus the camera-disabled reproduction preserves the previous controller result. Stage11/22/32/33 changed from clear to death with camera feedback. The failures in that camera-only run were Stage11/16/17/21/22/23/24/31/32/33/34/35. No failure is classified as a product defect merely because the controller died.

Reproduce from the repository root:

```sh
HeavyMach/build/core/HeavyMachValidationJourneyTests /tmp/camera-journeys.jsonl normal 18000 /tmp/camera-journeys.csv selected-35 host-camera legacy-early-hud
# Historical zero-offset condition only:
HeavyMach/build/core/HeavyMachValidationJourneyTests /tmp/legacy-journeys.jsonl normal 18000 /tmp/legacy-journeys.csv selected-35 legacy-zero-camera legacy-early-hud
```

That camera-only run covered the quake event/tick feedback. Its immediate upgrade confirmation and pre-session close timing are corrected in the current HUD-host run above. The historical outputs remain preserved. Continuous campaign and original runtime equivalence remain unverified. No physical device was used.

## Conditions and input

The fixture is applied once before each map: body/top/weapon grade 6, internal level 0, all 20 original player weapons, each non-basic weapon's original finite `charge`, seed 11. `bossIndex` and `playerPosition` remain unset. The harness verifies the original spawn X equals map.left + 20. The default weapon retains its original ammunition rule. There is no HP/ammo refill, enemy deletion, invulnerability, or position assignment after start. Original item pickups can increase ammunition. Original next-stage opening can restore HP; final HP therefore differs from HP at clear.

The controller uses filtered tilt input 0.1 to advance, stops within 260 units of a selected boss, aims at original enemy bounds, and requests boosters at gaps/steps and periodic intervals. It uses the basic weapon for ordinary targets and selects finite radar/laser/missile/cannon weapons for bosses. Press/move/release use the public touch API. Actual experience triggers level-up; the controller confirms and finishes the upgrade slide through the existing public API. With all grades initially 6 this does not prove lower-grade selection behavior.

On clear, the controller waits for the original result presentation, calls `resultButton(1001)` for next-stage or `1000` for mission-end, and advances the animation. It verifies the next stage, original clamped mission transition, saved mission/stage/farthest, and save encode/decode equality. Mission-end completion records screen 4; it does not construct a native menu. Death waits for the original result screen event and remains a failed journey. Timeout, wrong transition, missing resources, and exceptions also fail the process.

## Reproduce

Root CMake defines `HeavyMachValidationJourneyTests`, linked to the validation-only core. No production app code or original game rule was changed for this runner.

```
cmake --build /tmp/hm-validation-build --target HeavyMachValidationJourneyTests
/tmp/hm-validation-build/HeavyMachValidationJourneyTests /tmp/journeys.jsonl Stage01,Stage03,Stage25 18000
/tmp/hm-validation-build/HeavyMachValidationJourneyTests /tmp/all-journeys.jsonl '' 18000
```

Arguments are output JSONL, optional comma-separated original map keys, and per-map tick limit. Default limit 18,000 is 300 seconds of simulated time. Nonempty failed sets return exit 1; only every selected journey passing returns 0. Output includes independent-start flag, normal spawn, clear tick, outcome, next/saved progress, experience/level, finite ammunition, input/RNG counts. Current telemetry also records initial/minimum/clear HP, separating damage from next-stage HP initialization.

## Executed evidence

Initial harness `/tmp/heavymach-journeys-representative.jsonl` incorrectly labeled the transient player removal during ClearObjects as death after clear. This was a harness defect, not an observed game death. The corrected runner allows the original object replacement and verifies the resulting stage instead. Those earlier logs are retained.

Corrected representative run `/tmp/heavymach-journeys-representative-v2.log` and `.jsonl`: exit 0, 3/3.

| Map | Clear tick | Transition complete tick | Result | Actual level-ups |
| --- | ---: | ---: | --- | ---: |
| Stage01 | 1684 | 1700 | mission 0/stage 1 generated and saved | 0 |
| Stage03 | 3669 | 3685 | mission 0/stage 3 generated and saved | 1 |
| Stage25 | 3766 | 3782 | mission 3/stage 1 generated and saved | 2 |

Full independent-start run `/tmp/heavymach-journeys-all-v2.log` and `.jsonl` completed all 41 original map entries, exit **1**: **19 clears** (15 next-stage openings, 4 mission-menu transitions), **21 deaths**, **1 timeout**. This is not 41 successful journeys. Mission 7 entries are direct-map fixtures; the original normal campaign clamps at mission 6.

The subsequent telemetry-only revision was built and rerun on Stage01/03/25 (`/tmp/heavymach-journeys-representative-v3.log` and `.jsonl`, exit 0). Clear ticks, RNG counts, transitions and saved progress equal v2. Minimum HP was 840 / 698 / 293; clear HP was 840 / 765 / 293, while the original next-stage open restored final HP to 840. The all-map v2 run predates those three HP output fields; input behavior is unchanged.

These failures identify limits of this deterministic controller and/or remaining game behavior. No root cause is assigned without original-runtime comparison. Stage18 retained HP 655 at timeout, X 4141.340; the other 21 failures reached HP 0 and original result screen 4. No failure was changed into a pass by weakening game rules.

| Map | Outcome | Ticks | Saved mission/stage | Final HP |
| --- | --- | ---: | --- | ---: |

| Stage01 | clear_next_stage | 1700 | 0/1 | 840 |
| Stage02 | clear_next_stage | 2928 | 0/2 | 840 |
| Stage03 | clear_next_stage | 3685 | 0/3 | 840 |
| Stage04 | clear_mission_menu | 970 | 1/0 | 0 |
| Stage05 | clear_next_stage | 3181 | 1/1 | 840 |
| Stage06 | clear_next_stage | 3543 | 1/2 | 840 |
| Stage07 | player_died | 3416 | 1/2 | 0 |
| Stage08 | player_died | 3248 | 1/3 | 0 |
| Stage09 | clear_mission_menu | 736 | 2/0 | 0 |
| Stage10 | clear_next_stage | 3609 | 2/1 | 840 |
| Stage11 | clear_next_stage | 3075 | 2/2 | 840 |
| Stage12 | clear_next_stage | 3173 | 2/3 | 840 |
| Stage13 | clear_next_stage | 2928 | 2/4 | 840 |
| Stage14 | clear_mission_menu | 965 | 3/0 | 0 |
| Stage25 | clear_next_stage | 3782 | 3/1 | 840 |
| Stage26 | clear_next_stage | 3671 | 3/2 | 840 |
| Stage27 | player_died | 4532 | 3/2 | 0 |
| Stage28 | player_died | 4126 | 3/3 | 0 |
| Stage29 | clear_mission_menu | 1909 | 4/0 | 0 |
| Stage15 | clear_next_stage | 3113 | 4/1 | 840 |
| Stage16 | player_died | 2822 | 4/1 | 0 |
| Stage17 | clear_next_stage | 3634 | 4/3 | 840 |
| Stage18 | timeout | 18000 | 4/3 | 655 |
| Stage19 | player_died | 2911 | 4/4 | 0 |
| Stage20 | clear_next_stage | 3955 | 5/1 | 840 |
| Stage21 | player_died | 775 | 5/1 | 0 |
| Stage22 | player_died | 1448 | 5/2 | 0 |
| Stage23 | player_died | 1123 | 5/3 | 0 |
| Stage24 | player_died | 5334 | 5/4 | 0 |
| Stage30 | player_died | 2810 | 6/0 | 0 |
| Stage31 | player_died | 1900 | 6/1 | 0 |
| Stage32 | clear_next_stage | 3369 | 6/3 | 840 |
| Stage33 | player_died | 1822 | 6/3 | 0 |
| Stage34 | player_died | 1297 | 6/4 | 0 |
| Stage35 | player_died | 1157 | 6/5 | 0 |
| Stage36 | player_died | 411 | 7/0 | 0 |
| Stage37 | player_died | 403 | 7/1 | 0 |
| Stage38 | player_died | 420 | 7/2 | 0 |
| Stage39 | player_died | 403 | 7/3 | 0 |
| Stage40 | player_died | 403 | 7/4 | 0 |
| Stage41 | player_died | 403 | 7/5 | 0 |

Physical devices and native UI were not operated. Boss-only runner results are a separate experiment. A single continuous campaign, remaining 22 successful map journeys, and comparison of controller failures against the original runtime remain unverified.

## Stage18 timeout investigation

Evidence lives under `build/stage-journeys/`. New optional arguments after tick limit are CSV trace path and a preselected strategy name. `baseline` remains explicitly selectable, preserving earlier evidence. Trace samples every 60 gameplay ticks without consuming RNG. It records player position/velocity/HP/state/booster count, terrain height, world boundaries, boss counters, selected weapon/ammo, aim, tilt, chosen target, every living enemy, and cumulative RNG calls. Result-transition logs additionally record each tick's pause/state/presentation/upgrade phase.

### Stationary position is the original boss boundary

`stage18-baseline.{jsonl,csv,log}` reproduces the 18,000-tick timeout. Player X is 4141.340, ground height -11, HP 655. E05MB02 remains alive at X4020/Y-11 with HP744; bossEncounter is1 and frontLine is4170. The player continues jumping and firing; this is not a frozen tick or missing terrain.

The original `GobWorld::makeEnemy` at0xd6b8 sets boss encounter/frontLine using bossX+150. Literal0xd814 is float150. `GobHvMach::tick` at0x479c, specifically0x48fe..0x491e, obtains frontLine, subtracts float30 (literal0x4a78), compares player X and zeros forward speed. Thus the stop boundary is4140, crossed by one normal movement step to4141.340. Original `killBoss`0xd848 opens the boundary only after encounter count falls below1. Raw binary literals and source references are saved in `stage18-original-boundary-evidence.json`.

The controller caused the deadlock: its target window requires dx > -120. This living boss is now121.340 units behind and permanently excluded. From tick6000 to18000 the controller targets E0511 at4438 instead. Original player `setTap`0x1d300 only accepts forward aiming (dx>0), so changing aim toward a boss behind the player without moving back would not solve it. No original-rule discrepancy was found in this boundary chain.

### Fixed input alternatives and retained failures

- `boss-priority` always selects a living boss within650 units and uses actual negative tilt when closer than180. It died at tick2951, HP0, before clear. Files `stage18-boss-priority.*` retain that failure.
- `boss-recovery` preserves baseline choices until a living boss falls more than100 units behind within650, then enables the same boss priority/retreat policy. It killed the bosses and reached clear at4064 with HP305, EXP1417; saved mission4/stage4. However, its instantaneous upgrade-close shortcut stalled result processing. Files `stage18-boss-recovery.*` and detailed `stage18-transition.*` retain this second failure.
- `boss-recovery-ui` applies the original close-slide equation to every level-up. Stage18 died at3964 (HP0/X5460.918), so this run is not reported as a successful full journey. Files `stage18-slide.*` retain it.
- `boss-recovery-result-ui` isolates the result-processing problem by applying that delay only during the clear transition; earlier gameplay retains the previous shortcut. It passes at4110, with the same clear tick4064, HP305, EXP1417 and total RNG431331 as the stalled run. This proves the result stall came from the harness shortcut; it does not replace the failed all-slide Stage18 run. Files `stage18-result-slide.*`.

### Upgrade callback ordering

At4079, the original next-map `ExperienceZero` call opens a deferred level-up (level2→3), while result step6 is paused. The old harness immediately calls `confirmUpgrade` and `finishUpgradeSlide` at4080, forcing paused=false before step6 fades. `World::tick` processes results while paused, leaving step6 stuck. This is an invalid shortened UI lifecycle in the harness.

Original `GuiGame::tick`0x161d0 moves closing upgrade X toward660 by `(660-x)/5`, and only after X>659 removes the panel and calls `setPause:0`. `HMGameHUD.mm` implements this same equation and deferred callback. The UI strategies advance that closing equation each tick instead of invoking finish immediately. They begin closing at X=-240 after immediate confirmation, so they reproduce the callback gate, not a complete human-timed opening/click interaction. A successful journey now also waits until this close callback finishes. No game-core change was warranted or made.

Representative full-slide regression `representative-slide.{jsonl,csv,log}` passed Stage01/03/25 (completion ticks1700/3715/3855). Previous 19 baseline passes remain evidence for the documented shortcut controller, not proof of full native UI timing. Full 41-map comparisons are recorded below after completion.

```
/tmp/hm-validation-build/HeavyMachValidationJourneyTests \
  HeavyMach/build/stage-journeys/stage18-slide.jsonl Stage18 18000 \
  HeavyMach/build/stage-journeys/stage18-slide.csv boss-recovery-ui
```

`stage18-run-hashes.json` identifies the final runner, linked executable and relevant core source files used for these runs. A strategy is fixed before starting each map. It changes controller decisions and UI timing only; all collisions, damage, boss boundaries, ammunition, drops, RNG calls and save mutations remain GameSession behavior.

### Completed 41-map comparison

Both final full runs finished with exit1:

| Fixed controller | Next-stage clears | Mission-menu clears | Deaths | Timeouts |
| --- | ---: | ---: | ---: | ---: |
| baseline regression | 15 | 4 | 21 | 1 |
| boss-recovery-ui | 14 | 4 | 23 | 0 |

`all-baseline-regression.{jsonl,log}` reproduces every original v2 map's status, tick counts, clear tick, EXP/level, RNG count, final X/HP and saved mission/stage exactly. Thus the earlier19 passes are preserved under their stated controller.

`all-boss-recovery-slide.{jsonl,csv,log}` completes all41 with18 successful journeys and23 deaths. This stronger callback-timing controller is not a universal improvement. Differences from baseline:

| Map | Baseline | Boss recovery + every close slide |
| --- | --- | --- |
| Stage07 | death | next-stage clear |
| Stage10 | next-stage clear | death |
| Stage27 | death | next-stage clear |
| Stage17 | next-stage clear | death |
| Stage18 | timeout | death |
| Stage20 | next-stage clear | death |

All other outcomes match. In particular, Stage18 remains unpassed by the full-slide controller. The isolated result callback test succeeds but must not be pooled into18/41 or reported as full native UI equivalence. No core correction or physical-device experiment was performed.

## Normal campaign map expansion

The latest default strategy is `baseline-ui`; historical `baseline` remains explicitly selectable for reproducing the initial shortened callback timing. All expansion strategies below use the close-slide gate on every level-up. `normal` selects the35 maps reachable through original missions0..6, excluding six direct-map mission7 entries. Every map still starts independently.

Input policies were fixed before each run. `journey-evasive` prioritizes living bosses within650, moves back below160 and forward above330, requests available ground/descending boosters, and updates aim each tick. `armed-hold2` uses finite Gun07→Gun02/Shotgun stocks for ordinary enemies and updates aim every2ticks. `dash` adds real near-player press input every60ticks when no nearby boss and dashVel is zero. `periodic` retains the earlier periodic booster policy. `full` includes the original RaderBomb in the finite boss weapon order. `still` stops within330 and requests a single hop at boss-frame210; `close` maintains210..230 distance. `hold6` updates aim every6ticks. `laser` spends finite Laser/ElShock/cannon/missile stocks on ordinary enemies. These are controller variations, not changes to game rules.

`early-deaths.*` distinguishes falling from incoming damage: continuous boosting fell below Stage08 bottom−160 at tick3042 (HP486→0), and Stage10 bottom−176.5 at422 (HP780→0). The dash strategy subsequently completed Stage08. All failed policies remain saved.

| Evidence file stem | Maps executed | Clears | Deaths | Timeouts |
| --- | ---: | ---: | ---: | ---: |
| journey-evasive | 35 | 18 | 17 | 0 |
| journey-evasive-armed-hold2 | 35 | 18 | 17 | 0 |
| journey-dash | 35 | 16 | 19 | 0 |
| journey-periodic-dash | 35 | 19 | 16 | 0 |
| journey-full-still-hold2 | 14 | 4 | 10 | 0 |
| journey-full-laser-periodic-dash | 12 | 1 | 11 | 0 |
| journey-full-close-hold6 | 9 | 1 | 8 | 0 |
| journey-full-laser-dash-hold2 | 9 | 0 | 9 | 0 |
| baseline-ui | 9 | 0 | 9 | 0 |

The selected policy table is frozen in the runner and `build/stage-journeys/selected-35-policies.json`, with the successful prior evidence for each of27maps. Eight maps without a successful full-slide journey retain `baseline-ui`. `selected-35` is a single process executing these35 independent starts with no retries or policy adjustment from its results. It is not a continuous campaign. Final results follow after completion.

```
/tmp/hm-validation-build/HeavyMachValidationJourneyTests \
  HeavyMach/build/stage-journeys/selected-35.jsonl normal 18000 \
  HeavyMach/build/stage-journeys/selected-35.csv selected-35
```

Final `selected-35.{jsonl,csv,log}` completed all35maps with exit1: **27clears**, **8deaths**, **0timeouts**. This does not establish all35 successful journeys.

| Map | Fixed strategy | Outcome | Ticks | Saved mission/stage |
| --- | --- | --- | ---: | --- |
| Stage01 | boss-recovery-ui | clear_next_stage | 1700 | 0/1 |
| Stage02 | boss-recovery-ui | clear_next_stage | 2928 | 0/2 |
| Stage03 | boss-recovery-ui | clear_next_stage | 3715 | 0/3 |
| Stage04 | boss-recovery-ui | clear_mission_menu | 1000 | 1/0 |
| Stage05 | boss-recovery-ui | clear_next_stage | 3211 | 1/1 |
| Stage06 | boss-recovery-ui | clear_next_stage | 3597 | 1/2 |
| Stage07 | boss-recovery-ui | clear_next_stage | 3739 | 1/3 |
| Stage08 | journey-dash | clear_next_stage | 2800 | 1/4 |
| Stage09 | boss-recovery-ui | clear_mission_menu | 766 | 2/0 |
| Stage10 | journey-full-close-hold6 | clear_next_stage | 3027 | 2/1 |
| Stage11 | boss-recovery-ui | clear_next_stage | 4118 | 2/2 |
| Stage12 | boss-recovery-ui | clear_next_stage | 2939 | 2/3 |
| Stage13 | boss-recovery-ui | clear_next_stage | 3166 | 2/4 |
| Stage14 | boss-recovery-ui | clear_mission_menu | 995 | 3/0 |
| Stage25 | boss-recovery-ui | clear_next_stage | 3855 | 3/1 |
| Stage26 | boss-recovery-ui | clear_next_stage | 3733 | 3/2 |
| Stage27 | boss-recovery-ui | clear_next_stage | 4463 | 3/3 |
| Stage28 | journey-full-still-hold2 | clear_next_stage | 4664 | 3/4 |
| Stage29 | boss-recovery-ui | clear_mission_menu | 1939 | 4/0 |
| Stage15 | boss-recovery-ui | clear_next_stage | 3143 | 4/1 |
| Stage16 | baseline-ui | player_died | 2822 | 4/1 |
| Stage17 | baseline-ui | player_died | 4087 | 4/2 |
| Stage18 | journey-full-still-hold2 | clear_next_stage | 3178 | 4/4 |
| Stage19 | journey-evasive | clear_mission_menu | 1204 | 5/0 |
| Stage20 | journey-periodic-dash | clear_next_stage | 3857 | 5/1 |
| Stage21 | baseline-ui | player_died | 775 | 5/1 |
| Stage22 | journey-evasive-armed-hold2 | clear_next_stage | 2419 | 5/3 |
| Stage23 | baseline-ui | player_died | 1123 | 5/3 |
| Stage24 | baseline-ui | player_died | 5334 | 5/4 |
| Stage30 | journey-evasive | clear_next_stage | 2847 | 6/1 |
| Stage31 | baseline-ui | player_died | 1900 | 6/1 |
| Stage32 | boss-recovery-ui | clear_next_stage | 3429 | 6/3 |
| Stage33 | journey-full-still-hold2 | clear_next_stage | 2274 | 6/4 |
| Stage34 | baseline-ui | player_died | 1297 | 6/4 |
| Stage35 | baseline-ui | player_died | 1157 | 6/5 |

Remaining failed maps: Stage16, Stage17, Stage21, Stage23, Stage24, Stage31, Stage34, Stage35. These are controller-run failures; original/runtime equivalence and an input policy that succeeds remain unverified. The previously successful18 full-slide maps remain successful in this final execution, with9additional full-slide journeys. No core, map, collision, HP, ammunition or physical-device changes were made.

## Trace-directed follow-up on the remaining eight maps

At this historical follow-up, the integrated result remained **27/35** (`selected-35.*`). No new successful journey resulted from this follow-up, and the already successful maps were not rerun. No core/game-rule change or physical-device action was made.

The original falling path is `GobHvMach::processFly`0x5240 (documented in `core-motion.md`): crossing map.bottom requests lethal current-HP damage. The damage traces therefore distinguish an observed bottom crossing from gradual damage while above bottom. They do not identify the individual attacker; a current aim target is not proof that it fired the lethal projectile.

| Map | Selected-35 failure | Closest useful prior trace | Remaining obstacle |
| --- | --- | --- | --- |
| Stage16 | Combat damage: HP10→0 atY49.21, bottom−200.5 | journey-evasive reached E05MB02 HP3324 with RaderMsl10 remaining | Reaches boss with little HP after approach damage |
| Stage17 | Combat damage: HP18→0 atY47.01, bottom−184.5 | full-still-hold2: final E05MB04 HP680; playerHP5, RaderMsl8 | Final boss not finished before player dies |
| Stage21 | Combat damage: HP60→0 atY139.94, bottom−160 | full-laser-dash-hold2 reached MBoss0701 HP7098; RaderLsr3 | Approach depletes HP before boss can be finished |
| Stage23 | Fall: HP740→0 atY−160.479, bottom−160 | evasive-armed-hold2 traversed that terrain and reached E06MB04 HP9453 | Alternative avoids initial fall but dies later in combat |
| Stage24 | Combat damage: HP5→0 atY183.00, bottom−160 | full-still-hold2 reduced Boss06 toHP5624; RaderMsl7 | Low HP during boss fight, not exhausted inventory |
| Stage31 | Combat damage: HP75→0 atY105.34, bottom−160 | full-still-hold2 killed the boss (encounter0), then died atX5015.44 | Post-boss approach to exit still takes damage; attacker identity unlogged |
| Stage34 | Combat damage: HP98→0 atY172.61, bottom−160 | evasive-armed-hold2 reachedX4978.81; Gun05 still57 | Dies during approach before a boss encounter |
| Stage35 | Combat damage: HP40→0 atY19.24, bottom−160 | full-still-hold2 reduced Boss07 toHP18978; RaderLsr2 | Boss damage remains substantial before player dies |

Boss HP/ammo in this table come from the final60-tick CSV sample, not necessarily the exact lethal tick. Finite ammunition remains in every selected-35 failure; the cited closest boss traces also retain ammunition in their active weapon. This does not imply every weapon is equally useful or all firing rays hit.

### Narrow comparisons and outcomes

Each policy was fixed before its start, with the same finite initial fixture, original left spawn and upgrade close-slide gate. The rationale was to change the variable suggested by the trace rather than alter damage, ammo, geometry or enemy state.

| Evidence stem | Maps | Change from prior policy | Result |
| --- | --- | --- | --- |
| followup-near-hop |17/24/35| Maintain170..190 boss distance instead of stopping near330 |3 deaths; worse than the prior closest boss damage |
| followup-nohop |17/24/35| Remove the single boss-frame210 hop |3 deaths; did not improve the closest outcomes |
| followup-cautious |16/21/23/31/34| Retain finite guns, hold140..250 distance from ordinary enemies rather than pass them |5 deaths; more time/ammo spent and earlier approach positions |
| followup-cadence1 |17/24/35| Only update aim every tick instead of every2ticks |3 deaths |
| followup-cadence6 |17/24/35| Only update aim every6ticks instead of every2ticks |3 deaths |
| followup-anchor |17/24/35| Approach to180 then latch a stationary position, matching the boss-only runner's stationary premise without a warp |3 deaths |
| followup-exitdash-fixed |31| Preserve pre-boss inputs; after encounter clears, use actual dash/booster and remaining Laser for the exit approach |1 death atX5053.91, HP15→0; still above bottom |

These are21 failed independent follow-up starts over eight maps, not21 additional map failures. `followup-manifest.json` records results and current runner/core source hashes. All `.csv`, `.jsonl`, and `.log` evidence remains under `build/stage-journeys/`.

The initial `followup-exitdash.*` run was invalid for isolating the exit: substring matching also activated the earlier general dash rule before the boss. That harness defect was corrected by excluding `exitdash` from the general dash condition, and the corrected attempt is separately retained as `followup-exitdash-fixed.*`. Neither is counted as a success.

No concrete mismatch with original gameplay rules was established. Further input-policy search has stopped; remaining work is the same eight maps, and all35 successful journeys or a continuous campaign remain unverified.
