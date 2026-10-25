# Original Objective-C declarations

`Sources/OriginalInterfaces/OriginalClasses.h` declares all 37 classes, 553 methods and 354 ivars recovered from Heavy Mach 1.13 ARMv6 runtime metadata. These are declarations, not executable implementations. No generated no-op method bodies were added.

`Analysis/objc-layouts.json` preserves original addresses, type encodings, instance sizes and ivar offsets. `Analysis/relocations-otool.txt` resolves external superclass references: EAGLView inherits UIView; the other external superclasses resolve to NSObject. `method-address-map.json` links each declaration to its original method implementation address, including the raw Thumb-bit address.

`OriginalTypes.h` uses current UIKit/Foundation geometry and object types. Named fields come from runtime encodings. Unnamed fields retain neutral `fieldN` names rather than invented meanings. EAGLViewDelegate is forward-declared because a protocol-qualified ivar identifies it. WeaponInfo and PartsInfo are original Objective-C classes, not invented C structs.

`Arm32DiskLayouts.h` separately describes original ARM32 records using fixed-width integers and floats. Pointer-bearing evidence records use uint32_t address fields; they are not native object pointers and must not be dereferenced. The original save record has no pointers: HM32TWeaponItem is 28 bytes, HM32TSaveSlotData is 1208 bytes, and HM32TDataFile is 3624 (0xe28) bytes. Static assertions verify the generated record sizes. Native Objective-C ivar offsets must never be compared directly with the original ARM32 offsets.

Validation: iOS Simulator 27.1 SDK, arm64 target, clang Objective-C syntax check of `Analysis/headers-syntax-check.m` passed. The original EAGLContext declaration produces one expected SDK deprecation warning. This check does not establish working game behavior or complete method implementations.

Regenerate with `python3 tools/export-rea-metadata.py`, then `python3 tools/export-rea-headers.py`. Metadata extraction reads the supplied original binary; it does not change it.

## Level experience calculation (0x105f8)

Evidence: `Analysis/functions/0x105f8.pseudo.c` and `.asm`, method `GlobalInfo::calcLevelUpExp`.

Observed flow:

1. Set levelUpExp to zero and a float32 growth base to 1.5f.
2. Iterate i from zero through the saved level, inclusive.
3. Copy the previous levelUpExp into levelExp.
4. Set levelUpExp to the signed-int conversion of `double(levelUpExp) + 300.0 * pow(double(base), double(i))`.
5. If base is greater than 1.25f, subtract 0.01f with float32 rounding.

The float32 arithmetic matters. Repeated subtraction reaches approximately 1.250000238, which remains greater than 1.25f, so it subtracts once more and stabilizes near 1.240000248. Replacing this with a mathematical clamp at exactly 1.25 changes the original progression.

Host calculation with explicit float32 base rounding first exceeds INT32_MAX at level 66 (previous total 1831618501, pre-conversion sum approximately 2271117820.61). The shipped code calls external `___fixdfsivfp`; its out-of-range conversion result is not established by this method alone. A native C++ out-of-range cast must not be treated as a verified reproduction. Keep overflow behavior explicitly unresolved until the shipped helper/runtime result is established.
