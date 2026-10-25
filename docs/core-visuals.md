# Native particle and machine-part visuals

These are executable portable C++20 reconstructions. Raw pseudocode and ARM/Thumb listings remain in `Analysis/functions/0xADDRESS.{pseudo.c,asm}`. They are not generated stubs. Native renderer integration and original-device pixel comparison are separate work.

| Source entry | Native entry |
|---|---|
| QobParticle reset 0x92b4, start 0x942c | resetParticle, startParticle |
| processFly 0x968c, tick 0x94a8 | tickParticle |
| setBound 0x9a14 | bounceParticle |
| draw 0x95dc | ParticleState.sprite (submit only while show) |
| GobMachParts init 0x114d8, setPartsInfo 0x11554 | PartsState, partsSprite |
| getSocketPos 0x11818, setBaseParts 0x11798 | partsSocketPosition, attachParts |
| refreshRotationPos 0x118fc, draw 0x11a48 | refreshPartsRotation, drawParts |
| GobMachWeapon refreshRotationPos 0x13530, draw 0x13874 | refreshWeaponVisual, drawWeapon |
| Tile2D draw wrapper 0x7208, full draw 0x6f28 | DrawSprite renderer contract |

Particles integrate velocity at fixed float32 1/60. Gravity is acceleration per second, unlike GobBullet gravity per tick. Surface contact returns contact plus normal*radius. Original bounce is `(normal*abs(dot) + .7*(velocity + normal*abs(dot)))*.3`, not a generic reflection. Flat-ground bounce is Y*-.3, X*.7 and spin*.5. Surface spin uses double *.4 then float, followed by signed `<.1` clamp, so negative spin becomes zero. VelocityAdd multiplies velocity per tick after movement. Lifetime is based on global double time minus stored float time, converted to float; expiration occurs at equality. Tile animation uses `(1-alpha)*frames+.5`, truncation to integer. The original reset/start do not reset alpha; this behavior is retained. The host owns object pooling, removal and superclass child traversal; a callback supplies the latter, including on the expiration tick.

Parts use original PartsInfo asset names and horizontal `tileCnt × 1` atlases, falling back to PartsDummy.png for zero tiles. Socket zero returns the sprite-center offset; other sockets are parent attachment plus local socket minus socket0. Rotation initially equals sentinel 999, forcing first refresh. Parent rotation uses atan2, float angle rounding, sqrtf and double sin/cos, preserving original arithmetic order. Track animation increments once after strictly more than three world X units, or uses abs(low signed16(speed*time*50))%2 while flying. Types 3 and 4 use abs(low signed16(time*200))%tileCnt and half engine bounce. Type 4 additionally subtracts fmod(time*.5,2π) from rotation.

Weapon parent anchor and aiming have separate update gates. `drawWeapon` calls aim smoothing once internally; callers must not call Combat::refreshWeaponAim again for the same visual update. Fixed enemy weapons retain unrotated socket offsets. Other weapon offsets rotate by aim, with π subtraction for machType != 3. Weapon drawing adds recoil and half engine bounce, then decays recoil by adding -offset/5 once per draw.

DrawSprite uses world coordinates with +Y up and rotation radians. Position is the sprite center. Host applies camera and screen Y conversion. The original transform order is translate, scale, rotate (T*S*R); do not reverse scale and rotation for nonuniform particles. Atlas indexing is row-major. Original Tile2D clamps frames >= total to total-1; renderer must preserve that upper clamp rather than wrap animation frames. Parts use blend 0 and alpha/scale 1. Particle blend is the stored original integer (default 2); exact CoreGraphics mapping of all GL blend modes remains host work. Z order is supplied by the host graph; no new layer scheme is inferred.

Limitations: this module does not implement generic Qob child traversal, Tile2D GPU calls/texture padding, part collision bound rectangles, full world camera, trail geometry, or all effect creation sites. Missing socket indices return the original no-change result. Zero-count animated type3/4 and float-to-int32 overflow throw explicitly rather than invoke undefined C++ behavior; original runtime behavior for these invalid/overflow cases is unresolved. State must remain owned and stable while PartsState.base pointers refer to it; attachment graphs must be acyclic.

Validation: VisualsTests covers lifetime equality/removal, callback timing, pool reset alpha, fixed-step motion, ground and surface bounce, socket chains, rotated anchor, strict movement threshold, engine bounce, type3 frames, weapon aim and recoil. Built and executed using clang++ C++20 with -Wall -Wextra -Werror -ffp-contract=off, linking Visuals, Combat, GameData and Geometry. No visual equivalence claim follows from those host tests.
