# GobWorld native state

World.hpp/.cpp is executable C++20 reconstructed from HeavyMach 1.13. Raw source-address evidence remains in Analysis/functions, including both pseudocode and disassembly. This is a state/controller module. UIKit, OpenGL, persistent saves, object graph ownership and full gameplay host integration remain outside it.

| Original | Native |
|---|---|
| openMap 0xe3f4, readMapSize 0xda80, readJoint 0xdbfc | openMap |
| readBGName 0xdfa8, processCamera 0xc8b8 | bgScale and processCamera |
| getColListFromX 0xd8dc, getMapHeight 0xd958 | collisionCandidates, mapHeight |
| processObjectRegen 0xcd00, readObject 0xde40 | processRegen |
| addEnemyMach 0xd6b8, killBoss 0xd848 | spawnEnemy, killBoss |
| checkBulletCollide 0xd3c8, checkEnemyBulletCollide 0xd44c | worldTargetAt |
| getFreeParticle 0xd898 | freeParticle |
| removeTrashObject 0xd2f4, clearAllObject 0xd27c | removeTrash, clearObjects |
| setNextStage 0xb8f0, processStageClear 0xc228, handleButton 0xe7e0 | nextStage, processStageClear, resultButton |
| setSurvival 0xb840, processSurvivalMode 0xd06a | configureSurvival, processSurvival |
| tick 0xb658, processMyMach 0xc830 | tick, moveInput |

Map parsing uses GameData. World stores a copy; ground segments expose stable indices and ColVector links. World cannot be copied because those links refer to its own ground storage. Moving is permitted. Mutating/resizing ground after opening invalidates links and spans. The host should keep WorldState and its parsed assets alive while child objects use them.

The original 100-unit collision bins use `[binLeft-50,binRight]` inclusion tests against current/previous joint X, without sorting endpoints. The asymmetry and original joint order are retained. Height tests cast down from Y=4000 by -10000 and return the first collision in bin order, including Geometry's +0.5 normal correction. No-hit sentinel is10000; player starts at left+20 (survival right-480) and height+10, or180 if no hit.

Camera approaches player+(140,70) by one third, then applies sequential lower/upper clamps (240 horizontal and160 vertical). Background position is camera+(mapCenter-camera)*bgScale. bgScale is the smaller original image/map travel ratio; map dimensions equal to the viewport retain original IEEE division behavior. Draw conversion remains host-owned.

Object regeneration runs only after player X increases strictly beyond the last trigger+10. Object left edge uses actual tile width/2 and must be strictly within500 of player; enemy center uses the same500 threshold. Spawn callbacks run once in original object list then enemy list order. Host supplies actual tile dimensions and constructs machines in state2, layer1; player engine bounce is enabled. Enemy/Motion/Player integration uses motionWorld(x,globalTime,RNG), EnemyFrame and PlayerHost without introducing a second physics loop.

Target checks are ordered: first in-screen enemy with original point containment wins; player-targeted shots do not require inScreen. contains callbacks must use the original Qob bounds behavior; this module does not substitute circular hitboxes. removeTrash delegates the original condition boundary>objectX+fullTileWidth, and advances backLine monotonically. Particle pool has64 entries and returns/reset the first hidden entry. Callers configure and start it synchronously before requesting another.

Boss count audit: binary ivar slot0x29970 has exactly five code literal references, at0xb3c8(init),0xd888(kill),0xe1a4(readEnemyList),0xe988(getter),0xe994(setter). setBossCount selector string0x239c8 only appears in method metadata; no selector-reference call exists. readEnemy assembly0xe29a writes the boss byte, then increments enemyRegenCnt, not bossCount. Thus original bossCount starts0 and kill decrements it. spawn increments bossEncounter and clamps frontLine to bossX+150/right-10; killing the final encountered boss restores right+10. The stage exit remains guarded by frontLine in Motion. No inferred boss-count repair was added. Reproducible raw-pointer matches are in Analysis/boss-count-pointer-audit.json.

Next-stage increments mission/stage using GlobalInfo::stageCount, creates presentation state and emits SaveProgress after the original mission6 clamp. The host must update/save the slot. Result steps preserve scale/rotation animation, alpha fade, button gates, object clear, resource clear, map-open request and unpause ordering. Quake event corresponds to amplitude10,count4,delay.3; TitleBang/TitleIn/ButtonSound are host audio. OpenStage must synchronously open the selected map before the next tick. ChangeScreen4 is the original scenario menu route,3 the survival route. Loading images and result text can consume StagePresentation; their child UI layout remains host-owned.

World tick advances gameTime by double(float(1/60)), runs regen/survival only in gameState0, runs result steps only while paused in states1..3, then updates camera. Host must apply moveInput(accelY) before this prelude and tick child machines/bullets/particles afterward. The original scenario-mode handler0xd068 is empty. Survival requires configureSurvival(internalLevel); original seed=max(100,400-level*10), gradually decreasing to50. It preserves original random draw order and machine name ranges.

Remaining scope: game-over setup0xbf74, touch/accelerometer registration, generic child traversal, loading UI child creation, changeStateTime, timeItemRegen, resource-manager teardown, and rendering/audio execution are not reconstructed here. Original malformed-map allocations and absent Player list slots are not emulated: GameData supplies validated semantic records. No claim is made that every GobWorld selector is complete.

Validation: WorldTests compiled and ran with clang++ C++20, -Wall -Wextra -Werror -ffp-contract=off. Tests cover original height correction, bin construction, strict regeneration thresholds, boss borders/count, camera clamp, background ratio, ordered target filtering, first-free pool, result transitions, pause/game clock, survival configuration and map reopen. Visual equivalence and complete game integration require host testing.
