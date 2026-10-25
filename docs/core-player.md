# Player 규칙 복원

2026-10-24. 원본 1.13 ARMv6 REA export `Analysis/functions/`의 pseudocode와 기존 typed core를 근거로 복원했다. `Player.hpp/.cpp`는 portable C++20이고 `runPlayerTests()`는 독립 검사 진입점이다. 이 검사는 iPhone 플레이 검증을 대신하지 않는다.

| 원본 | 복원 소비자 | 확인한 동작 |
|---|---|---|
| `0xfd44` GlobalInfo init | originalLootTable | 22개 순서·signed short weight·원본 이름 |
| `0x1c64c` pickupItem | pickupItem | 해금/가중 추첨, HP/EXP/패시브/무기, 획득 이미지, sound3 |
| `0x1778c`, `0x176b8`, `0x17870` | PlayerPassives | 60초 누적, float 1/60 감소, 정수 표시, 만료 |
| `0x161d0` GuiGame tick | host 계약 | pause 검사 없이 processPassiveSlot 호출 |
| `0x1bfdc`, `0x1cf58`, `0x1d0dc` | upgradeParts/refresh | 등급6 상한, top 강화 HP 전량 회복, body 이동속도 |
| `0x16a98`, `0x17c4c` | open/select/confirmUpgrade | random 초기 선택, 최대 등급 건너뜀, 확인/사운드 |
| `0x106e0` | GlobalInfo + addExperience | 호출당 최대1레벨, UI 열기 RNG 뒤 sound6, EXP gauge |
| `0x54b8`, `0x1d140`, `0x1d230`, `0x1d278` | onDamage/setHPGauge | 사망 후 피격음, 게이지, quake |
| `0x1d300` | setTap | 근접 dash, 오른쪽 조준·발사, release |

## 아이템과 RNG

`SaveSlot`의 `0x3b4` signed16 loot step을 읽는다. step<22이면 먼저 `random()%50`을 호출한다. 결과0이면 현재 step 아이템을 지급하고 step을1 증가시킨다. 이 경로의 비무기 표시 tile은0이다. 실패하면 별도 random으로 해금된 항목의 weight 합을 나눈다. step22에서는 해금용 random을 호출하지 않는다. 누적 weight를 차례로 빼고 최초 음수가 되는 항목을 고른다.

테이블은 HP_Half100, HP_Max10, EXP_Qurter4, EXP_Half2, EXP_Max1, Defense/Attack/Booster/Speed 각각50, 이어서 원본 무기13개 각각50이다. 오자 `EXP_Qurter`, `Canon`, `Rader`도 보존했다. EXP_Max는 다음 레벨 필요 구간을 float로 변환해1.5f로 나눈 후 정수 절삭한다. 이미 보유한 무기의 합산과 상한은 GlobalInfo를 재사용한다. 없는 무기 정의는 효과·particle 없이 sound3만 재생하며, 이미 올린 step을 되돌리지 않는다.

EXP 아이템이 레벨업을 일으키면 `openUpgrade`의 random 호출이 particle 획득과 sound3보다 먼저 발생한다. 호스트 RNG는 Enemy/Combat/World와 같은 스트림이어야 한다. 콜백에서 발생하는 원본 RNG도 동기적으로 소비해야 한다.

## 강화와 패시브

`0x2c`는 GlobalInfo addExp에서 레벨업마다 증가한다. 전체 exported pseudocode의 이 오프셋 접근과 upgradeParts/GuiGame handleButton을 확인했으며 강화 차감은 없다. 따라서 이를 소비 가능한 포인트로 가정하여 차감하지 않았다. 등급은 body0x2f/top0x2e/weapon0x2d, 0..6이다. 이벤트 Parts의 index는 body0/top1/weapon2, amount는 이미지 번호 grade+1이다.

openUpgrade는 random%3에서 시작하며 body 최대이면 선택0→1, top 최대이면1→2, weapon 최대이면2→마지막 가능한 body/top을 적용한다. 모두 최대이면 원본 sentinel100을 유지한다. confirm은 closing step2, upgradeParts, sound1 순서다. 원본 UI는 closing slide 종료 뒤 pause를 해제한다. `UpgradeClose`는 즉시 unpause 명령이 아니다.

패시브 index0 Defense,1 Attack,2 Booster,3 Speed다. 알 수 없는 이름은 원본처럼 Defense다. active 항목은 매 GuiGame tick마다 float bit pattern0x3c888889를 빼며, 표시 숫자는 정수 절삭한다. pause 중에도 GUI tick을 호출해야 한다. CombatFrame의 shield/damageBoost, Motion의 booster/speed 플래그에 호스트가 연결한다.

## 피해와 입력

HP 뺄셈은 원본32bit wrap을 유지한다. hp<1이면0으로 만들고 sound14, GameOver, random%3+8 hit sound, gauge 순서다. damage>10이면 quake amplitude=max(float(damage)/5,10), count4, delay.3f다. 원본에 없는 방어 보정을 이 경로에 추가하지 않았다.

setTap은 원본 touch state0 down,1 move,2 up과 원본480×320 화면 좌표를 받는다. player screen position+weapon rotPos를 뺀 dx/dy를 계산한다. abs(dx)<100,abs(dy)<60인 down에서 bodygrade>2, dashVel0이면 ±5 dash다. release는 위치와 상관없이 firing을 끈다. 그 외 dx>0이고 abs(dx)>100 또는 abs(dy)>100이어야 조준/발사한다. 정확히100인 경계는 제외한다. Combat의 setWeaponAim/setWeaponFire를 소비한다.

## 호스트 계약과 남은 범위

- 생성자는 선택된 SaveSlot 참조를 보관하고 HP0x18과 이동속도를 읽는 편의 초기화다. 원본 init 전체의 영상 객체 생성 복원이 아니다. 새 게임/mission HP 시작 정책과 SaveSlot의 현재 HP 동기화는 World가 원본 시작·저장 함수에 따라 처리해야 한다.
- hasHPGauge는 gameplay screen6일 때 true, 메뉴 preview는 false로 설정한다. 게이지 이벤트 value는 fraction, position.x는 `-(fraction*-21+21)`다.
- PickupParticle 이벤트 name은 ItemTile.png/WeaponTile.png, split7×4, blend0, life2, scaleVelocity.5, velocity(0,30), position은 player y+16이다. acquireParticle은 asset 존재와 원본 pool 가용성을 반영해야 한다.
- Parts 이름과 소켓/Visuals 교체, GuiGame 레이아웃·slide animation, pause, sound, save persistence는 host 이벤트 소비 범위다. `upgradeStep`의 closing 완료는 호스트가0으로 바꾼다.
- 잘못된 loot step<0/>22, 가중합0, 음수 random은 예외로 거절한다. 원본 잘못된 메모리 접근·0 나눗셈 영역은 동등성 주장 밖이다. passive active의 범위 밖 index도 false로 제한한다.
- GlobalInfo thresholds의 높은 레벨 double→int 범위 초과는 기존 checkedInt 예외를 그대로 전파한다. 원본 ARM 변환 결과와 level66 이후 동등성은 미해결이다.
- 실제 World/앱 연결과 실기기 레벨업은 이 단위 검사로 확인하지 않았다.

## 실행한 검사

`clang++ -std=c++20 -ffp-contract=off -fno-fast-math -fsanitize=address,undefined`로 Player, Motion, Geometry, Combat, SaveData, GlobalInfo, GameData와 PlayerTests 및 임시 main을 링크했다. `/tmp/hm-player-tests` exit0, sanitizer 진단 없음.

검사 범위: step22 draw수, 이전 step 실패 후2회 draw, unlock 뒤 missing weapon, weight 경계100/117/317, 무기 charge/tile, 패시브 누적·절삭·만료, 레벨업 선택 RNG와 사운드 순서, 포인트 미차감, top 전량회복/등급상한, 모두최대 sentinel100, dash/100픽셀 경계/release, 치명 피해 이벤트 순서와 quake 하한.
