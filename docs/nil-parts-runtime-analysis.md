# nil BaseParts runtime buffer analysis

## Result and evidence boundary

The three nil calls in `build/nil-parts-runtime-second.jsonl` contain nonzero stack bytes, preserved across both struct-return getters. **Their values are nevertheless predicted to produce exactly the same root-part accumulated bounds as native's zero fallback: (-20,0,20,40).** Nonzero buffer contents alone do not demonstrate a final collision-bounds difference.

This is a static calculation using actually observed input bytes. The supplied log has only four getter phases per call,12 rows total. It does not observe the final bounds store at0x4778, whole-machine completion, or machine names. The `machine_bounds_hex` field is zero in all12 records and is the bounds before this calculation completes. Do not call it final bounds.

No UI, debugger connection, application or device operation was performed for this analysis. Original executable, observer, product sources and other documents were not modified.

## Observed inputs and calculation

All samples have r10=0, SP=0xfffffbe4, threadQC1, and a different self pointer. All six buffer words are unchanged across rot_before/rot_after/bounds_before/bounds_after within each sample. These bytes prove behavior of this wrapper execution, not every historical Apple ARM32 runtime or every subsequent invocation.

The original0x428c calculation and native `Sources/Core/ItemBox.cpp::accumulateMachBounds` use:

```text
Lcandidate = rotX + .5 * rectLeft
Rcandidate = rotX + .5 * rectRight
Tcandidate = rotY + .5 * rectTop
L = min(previousL, Lcandidate, -20)
B = 0
R = max(previousR, Rcandidate, 20)
T = max(previousT, Tcandidate, 40)
```

These simplified min/max expressions apply to the observed finite values. Original/native NaN handling must not be replaced by these expressions for arbitrary data. rectBottom is not used. Initial previous bounds are observed as all zero. Binary32 operations were calculated with rounding after each multiply and add.

| Sample | self | Observed rotX,rotY | Observed rectLeft,rectRight,rectTop | Calculated L/R/T candidates | Predicted bounds |
| --- | --- | --- | --- | --- | --- |
|0|0x30051ea0|9,4.4841551e-44|4.6793502e-10,2.2420775e-44,28|9,9,14|(-20,0,20,40)|
|1|0x30055510|16.5,23|9.0794331e-41,-10.5,-1|16.5,11.25,22.5|(-20,0,20,40)|
|2|0x30055200|1.7936620e-43,1.7936620e-43|128,2.3035105e-40,4.7644148e-44|64,1.1535489e-40,2.0318828e-43|(-20,0,20,40)|

The large128 in sample2 is **left**, so it cannot extend the negative left boundary. The lower clamp wins. Tiny subnormal values also cannot defeat any clamp; whether those tiny intermediate results flush to zero does not change this conclusion.

Native zero fallback yields candidates0,0,0 and the same bounds. If the rest of the assembly follows the observed nil getter behavior, later child additions start from the same accumulated bounds. These samples therefore supply no evidence that the nil fallback changes the final whole-machine bounds. They do not prove that all other stack patterns are harmless.

The original repeatedly calls the nil getters while selecting clamp branches. Its same sp+0x50..0x67 return area is not initialized between those calls; copies go to separate local areas. The observation proves the first pair's preservation. Continued preservation through all subsequent calls is a static expectation from the nil dispatcher and caller, until the proposed final sample confirms it.

## Observe this addParts completion

Original assembly:

```text
0x4776  41 46  mov r1,r8
0x4778  c8 60  str r0,[r1,#0xc]  ; final top store
0x477a  10 98  ldr r0,[sp,#0x40] ; first instruction after all four stores
0x477c  4a b0  add sp,#0x128
```

Stop **before0x477a executes**. The full frame still exists:

- r10 is BaseParts; require0 for the selected nil call.
- `u32[sp+0x30]` is self. Match thread, SP and self to the getter sample.
- `self+0x34` is the16-byte boundRect to read.
- r1 and r8 should both equal self+0x34 at this exact boundary; use this as a consistency check, not a replacement for reading self.
- r0 still contains the just-stored top float bits. It is not self. The next ldr will replace it with the returned part pointer.
- `u32[sp+0x40]` identifies that returned part. The24 bytes atsp+0x50 can be reread to check continued buffer preservation.

Predicted little-endian bounds bytes for all three observed inputs:

```text
0000a0c1000000000000a04100002042
```

This is completion of one `addParts:BaseParts:BaseSocket:` call. At a nil root it precedes recursive child-part additions. It is not whole-machine final bounds. Parent's new observer `--final` targets this boundary; its new run is not included in the supplied12-row evidence.

## Link names and observe the full machine

Use these separately verified original instruction boundaries:

| Stop address / original bytes | Registers and meaning |
| --- | --- |
|0x14918 / `f0b5`|`GobHvM_Enemy makeMachFromName:` entry, before push. r0=self, r2=const char* machine name. Record both and current thread/call sequence.|
|0x14992 / `301c`|Immediately after recursive decodeChunk returns at0x1498e. r8=self, r10=original char* name, r6=temporary copied MachInfo chunk. The next call at0x14994 frees r6. Read self+0x34 here for bounds after **all** decoded parts.|
|0x14ae6 / `05b0`|Common makeMach return epilogue, before stack/high-register restore. r8=self, r10=name pointer, r0=success bool (1 successful path,0 failure). Read self+0x34 for end-of-method confirmation.|

The name is a UTF-8 C string, not NSString:0x1492e saves incoming r2 inr10;0x14938 passes it to stringWithUTF8String;0x14998..0x1499c passes it to strcmp("Heli001"). Read bounded chunks until NUL (the authored machine names fit their24-byte field); log raw bytes and pointer as well as decoded text. Do not invoke Objective-C methods from the debugger just to obtain the name.

Prefer the0x14918 entry record plus0x14992 and0x14ae6 confirmation. Match self **within the same live call sequence/thread**, not by address alone across app launches or object reuse. The existing three self addresses cannot be assigned machine names from the supplied log; guessing Stage01's first enemies would be unsupported, especially if menu/model construction preceded them.

An alternative root-chunk cross-check is0x14818, before type comparison: r2=chunk type, r4=payload pointer, r8=self. When r2==3, the payload begins with the24-byte machine name. This avoids interpreting Objective-C objects but must not mistake type4 part-name payloads for machine names.

## Minimal remote sequence for the debugger operator

This is a proposed procedure, not an executed connection. The existing observer's framing/checksum/greeting/ACK handling and instruction restoration should be reused.

1. Validate original bytes with `m14918,2`, `m477a,2`, `m14992,2`, `m14ae6,2` before patching.
2. Install one Thumb trap at a time (payload `M14918,2:00be`, then `c`). RequireT05, PC0x14918 and Thumb CPSR. Restore `M14918,2:f0b5` immediately while stopped. Read `g`, `qC`, and name memory at r2, and retain r0=self.
3. Continue the same invocation through the existing four getter stops. For its nil call, retain SP/self and original input bytes. Install/stop/restore0x477a (`M477a,2:00be` / restore`:1098`), then read `m<self+34hex>,10` (RSP lengths/addresses are hexadecimal) and `m<sp+50hex>,18`.
4. Install/stop/restore0x14992 (`:00be` /`:301c`). Require r8=self; record r10 name string and16 bytes atself+0x34. Let recursive children execute normally between root477a and14992; do not treat their absence from the log as absence from the game.
5. Install/stop/restore0x14ae6 (`:00be` /`:05b0`), verify same self/name and successr0. Record final bounds and continue. Restore any outstanding trap before disconnecting.

A machine missing from the resource dictionary can return through0x14ae6 without reaching0x14992; a robust operator should retain an error/timeout path rather than assume every call succeeds. The three current nonzero-buffer samples came from successful entry into addParts, but their whole-method return status was not recorded.

Software breakpoints change live guest instructions temporarily. They are runtime instrumentation, even though the immutable IPA file is not edited. A stop in another thread/frame or unexpected opcode must be reported, not silently attached to the current sample. These observations would validate wrapper runtime outputs; they still would not establish arbitrary historical iPhone stack contents.

## Follow-up: actual root accumulation observed

Parent subsequently supplied `build/nil-parts-runtime-root.jsonl`:15 rows, three nil calls with the four getter phases followed by `root_accumulated` at0x477a. Parent reports observer exit0 and eight observer self-tests passing. This reviewer read the log and independently recalculated each sample's binary32 candidates; no debugger or tests were rerun.

All three observed root results equal the predicted `(-20,0,20,40)`, bytes `0000a0c1000000000000a04100002042`. This upgrades **these three calls' root accumulation** from prediction to actual wrapper observation. It does not retroactively observe the prior run's final values.

| New sample | self | Calculated L/R/T candidates from this run's before buffer | Actual0x477a bounds | Match |
| --- | --- | --- | --- | --- |
|0|0x30055620|7.2560002451e-10,7.2731820566e-10,3.5432952709e-40|(-20,0,20,40)|yes|
|1|0x30055310|16.5,11.25,22.5|(-20,0,20,40)|yes|
|2|0x300554a0|64,1.1535488958e-40,2.0318827733e-43|(-20,0,20,40)|yes|

Within each new sample all24 buffer bytes remain identical through all five phases, including0x477a. The observed initial bounds are zero and the final clamp result matches native zero fallback. The before buffers differ from the earlier run:

```text
sample0 c0690430001f0530905b05308c840200b03e053074990200
sample1 19fd0000001f0530000028c1000080bf000084410000b841
sample2 00000043c086013020820200220000008000000080000000
```

In particular sample0 has entirely different small values; sample1/2 also have different unused bottom words. This demonstrates why one run's stack bytes must not be hardcoded as a universal nil return. The two runs have independent self addresses and no machine-name mapping; sample numbering alone does not establish machine identity across them.

Current remaining boundary: root bounds were observed for these three calls, but neither log records the machine names, whole-machine0x14992 bounds, or successful final0x14ae6 result. The exact name/full-machine observation procedure above remains proposed. These finite inputs show no root-bounds difference from native zero fallback; they do not prove every possible stale stack pattern is harmless or reproduce a historical physical iPhone runtime.

## Named whole-machine comparison preparation

The native inventory now contains720 constructions through the actual GameSession enemyParts path. See [named machine bounds](named-machine-bounds.md). The new original collector validates the entry-to-postdecode stack delta0x34, owner, thread and raw C name, with a return sentinel for resource failures. Six host tests and five strict comparator tests pass. No new original capture was made because the Mac console was locked. This does not change the three observed root results or establish a name for them.
