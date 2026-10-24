# Native title and original website action

`TitleState` implements GuiTitle processTitle0xf464 and onTap0xf934. Existing
intro easing, scales, sound timing and tap-to-skip behavior remain intact.

The RNG-taking tick overload now accepts a callable rather than precomputed values.
The original processTitle consumes no random numbers before `startTime+6`, or
outside the blink phase `float(fmod(double(float(time-begin)),2)) < 0.2f`.
Inside that phase it consumes one `random()%10` gate; only gate0..5 consumes a
second value for `(~random())&1`. A rejected gate retains the previous visibility.
The ARM calls are at0xf880 and0xf898, with the `cmp5/bgt` between them. Evidence is
`Analysis/functions/0xf464.pseudo.c` and `build/original-assembly.txt`.

HMGameView supplies the existing process-wide `random()` stream lazily. It no
longer calls arc4random twice every title frame. A value-based adapter remains for
existing deterministic core fixtures; production uses the callable overload.
Tests exercise zero/one/two-call branches, strict six-second and blink boundaries,
retained visibility, a new blink cycle, and first-tap skip without RNG consumption.
`Tests/TitleStateTests.cpp` exports `runTitleStateTests()` and also supports the
standalone `HEAVYMACH_TITLE_TEST_MAIN` define.

Website button103 is observed in GuiGame handleButton0x17c4c, in the paused PopButton
branch. It plays sfxID0 (UI_Click), constructs NSURL from the original CFString,
and calls UIApplication.openURL. Exact immutable original bytes at file0x24964
are the NUL-terminated ASCII `http://www.indiean.com` (no trailing slash).
The original executable SHA256 is
`97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`.
The host now uses the current UIApplication `openURL:options:completionHandler:`
API with those exact bytes, retains the paused menu state, and logs OS dispatch
failure. No substitute URL, inferred redirect, or network request was used to
establish the destination. Current website content/availability is unverified.

Validation: standalone TitleState tests passed with clang C++20, warnings as
errors, FP contraction disabled, ASan and UBSan. The HMGameView change passed
simulator SDK Objective-C++ strict syntax checking with HM_VALIDATION=1. No app
install/launch or browser-opening test was performed in this pass.

## Shared native RNG caller audit

The iOS caller inventory was compared with the original constructors and tick
branches, not only with RNG function names:

| Caller | Original evidence | Consumption/timing |
|---|---|---|
| App seed | GameMain.init0x2ecc | One srandom(low32(mach_absolute_time)); no screen-change reseed |
| Title | processTitle0xf464 | Conditional0/1/2 draws as above |
| Slot preview construction | refreshSlot0x186d8 → setParts0x1be84 → addParts0x4214 → weapon.init0x11d70 | One draw for each occupied slot's weapon, in slot0/1/2 order, when the slot screen is created |
| Slot model tick | GobHvMach.tick0x479c → processModel0x4b5c | Paused/stationary0; moving gate1; gate%60==0 and weapon present adds1 aim draw |
| Slot model weapon child | tick0x11e64 → epilogue0x129c6 | Not firing, machType3, so no weapon-AI random draws |
| Gameplay | GameSession host callback | Same process random() stream; Core combat/visual ordering is a separate audit |
| HUD/loading/drawing slot models | Current host source inventory | No extra random-source calls |

HMMenus construction used to create slot previews before the title screen and
omitted the weapon-constructor RNG entirely. Construction now waits for actual
showSlots (original setScreen_SelectSlot0x3994), and each created weapon stores
`float(random())*2^-31+.5`. Preview ticking uses actual preview existence, not the
mutable occupied save flag: creating a new save during slot-confirmation does not
create an extra model during shutter closure. Deleting a slot clears its preview,
matching refreshSlot after deletion0x18fc0. Model draws use shared random(), with
the existing exact processModel gate; they no longer use arc4random.

The original World survives screen changes. clearAllScreen0x3778 clears objects
and hides it, without resetting its clock or pause flag. HMMenus therefore receives
that clock/pause immediately before the native session is destroyed. Constructor
RNG still runs even if paused; model-tick RNG does not. In particular, returning
from game-over to slots keeps previews stationary while the retained World pause
is true. The pause-menu return button102 is different: original0x17c4c unpauses,
closes its submenu and plays UI_Click before saving/requesting screen3. The host
now preserves those actions, so a normal return does not inherit game-over pause.

Validation: an isolated executable used the actual Motion.cpp processModel/tick
and a counted RNG callback. Stationary0, rejected gate1, accepted gate2, missing
weapon1, and paused tick0 all passed with ASan/UBSan and warnings as errors.
The updated HMMenus/HMGameView passed simulator Objective-C++ syntax checking.
No UI install or visual verification was performed. This audits native RNG source
selection and preview lifetimes; it does not claim identical full-game streams
until the separate Core child-order/render-time audit is complete.
