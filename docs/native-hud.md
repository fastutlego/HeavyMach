# Native GuiGame HUD

`Sources/iOS/HMGameHUD.h/.mm`은 원본 GuiGame과 GobWorld 결과 표시를 UIKit/CoreGraphics로 옮긴다. 원본 PNG 파생 자산을 그대로 사용한다. 게임 값은 Player/World/SaveData가 소유하며 HUD는 그 값으로 화면을 그린다.

## 연결

`initWithMedia:` 이후 매60Hz `tick(time)`, 유효한 그리기 context에서 `draw(snapshot)`을 호출한다. Snapshot의 포인터는 draw 중에만 읽는다. HP 목적값·창 표시 상태·업그레이드 시작 등급 등 애니메이션에 필요한 값만 복사한다.

- `setGameStart(time)`: Mission/Stage 문구를 4초간 표시한다. 처음1초 alpha0→0.5, 1–3초0.5, 마지막1초0.5→0.
- `setReloadComplete(time)`: 파란 램프를 0.15초 동안 표시한다.
- `showWeaponChange(time)`: 선택 무기 주변 최대5개를 표시한다. 1초 유지 후1초 동안 사라진다.
- `touch(point,state)`: 480×320 y-up, began0/moved1/ended2/cancelled3. 반환 YES는 게임 조준 입력을 소비한다.
- `button(originalID,state)`: 호스트가 원본 상태 변경과 소리를 실행한다. 버튼 번호는 아래 표를 따른다.
- `upgradeClosed()`: 업그레이드 패널이 x659를 넘은 뒤 호출한다. 호스트는 이때 world pause를 해제한다.

`upgradeVisible`은 `player.upgradeStep==1`에 대응한다. 확정 뒤 false가 되면 퇴장 애니메이션을 시작한다. `menuVisible`도 호스트가 관리한다. passive 타이머는 PlayerPassives가 소유하며 원본처럼 pause 중에도 GuiGame tick마다 갱신해야 한다.

|ID|원본 입력|호스트 동작|
|---|---|---|
|100|release|pause 메뉴 열기|
|101|release|pause 해제, 메뉴 닫기|
|102|release|pause 해제, 저장, 모드 화면|
|103|release|원본 개발사 웹사이트 버튼|
|1000|press|평상시 booster|
|1100|release|무기 변경|
|1200–1202|release|body/top/weapon 업그레이드 선택|
|1300|release|선택 업그레이드 확정|
|1000/1001|release|결과 화면에서 미션 선택/다음 stage|

결과화면의1000과 booster1000은 원본부터 같은 번호다. `presentation.step==2`일 때 결과 처리를 우선한다. HUD는 modal 화면에서 뒤 게임 버튼의 hit 영역을 등록하지 않는다.

## 복원한 배치와 동작

GuiGame init0x151a0: 하단/상단 프레임은 (240,32)/(240,288), menu(38,279), booster(34,34), weapon(103,31)이다. Booster/Weapon은2×1 atlas, GameMenu는1×2, GameMenuSub는1×6이다. WeaponTile/ItemTile은7×4이다. 선택 무기 아이콘은(106,35), 탄약은(120,26)에 오른쪽 정렬한다.

EXP(0x1755c)는 width70, scaleX=gauge, x=27+35×gauge로 왼쪽 끝을 고정한다. 레벨은(40,313), NumSet01 16×1, pitch7, blend2다. HP 목적값(0x17620)은 tick(0x161d0)에서 차이의1/8씩 따라가며 차이가0.01보다 작으면 맞춘다. HP 바늘(239,25)의 회전은 `gauge × -5.183627803521753 − 0.6000000238418579`다. 재장전 바늘(287,22)은 gauge×2π이며 기어 회전속도0.15를 설정한다(0x1762c). 기어는(324,3)에서 매 tick 회전하고 속도가1/20씩 감소한다.

파란 램프는(287,22), 빨간 램프는(265,35), blend2다. 빨간 램프 alpha는 min(1,cos(10×time)+1)이다. passive4칸은 x10/y200+16i, 원본 ItemTile의5+i frame을0.5배로 그린다. 남은 시간은 원본 NumSet02b를 사용한다.

메뉴는 x−240에서 각 버튼 목표x220/240/260으로 이동한다. 각 이동은 차이를7/9/11로 나눈다. 닫힐 때 x720으로 이동하고 마지막 버튼x710 이후 숨긴다. y는220/170/120이다(0x161d0).

업그레이드(0x16a98,0x17280)는 중심y180, 시작x−240, 목표x240, 닫힘 목표x660이다. 매 tick 차이의1/5만큼 이동한다. body/top/weapon 그림은 현재 등급+1 프레임을 미리 보여준다. 최대등급에서 원본 Tile2D와 같이 마지막 프레임으로 제한한다. socket이나 실제 기체 등급은 HUD에서 변경하지 않는다. 등급6 미만인 항목만 선택 버튼을 만든다. 현재 등급 게이지와 선택한 다음 등급 게이지를 원본9픽셀 간격으로 그린다.

결과/실패(0xb8f0,0xbf74)는 `WorldState::presentation`을 소비한다. 로딩 그림은(240,160), ResultTypo1×5는(180,280), blend2다. stage clear frame4, mission clear frame0, 일반 game over frame1, survival game over frame3이다. 결과 버튼1×4의 중심은(120,26+buttonY)/(380,26+buttonY)다. missionEnd이면 오른쪽 버튼을 만들지 않는다. JPEG 파생 파일은 원본 확장자를 유지한 `.jpg.png`로 참조한다.

## 증거와 확인

`Resources/Evidence/hud-*.json`에 GuiGame16개 함수와 World 결과 함수의 REA 결과를 저장했다. 원본 SHA와 procedure 주소가 각 결과에 포함된다. 전역 REA active target은 변경하지 않았다. `Evidence/index.json`에 evidence ID를 연결했다.

simulator arm64와 device arm64 SDK27.1에서 C++20/ARC, `-Wall -Wextra -Werror` 구문 검사를 실행했다. 참조한 HUD 이미지27개가 실제 자산에 존재함을 확인했다. 실제 Air 게임 진행에서 HUD 터치·업그레이드·clear를 확인하는 검사는 앱 통합 담당 범위로 남는다. 컴파일과 자산 존재 검사만으로 그 동작을 통과했다고 보고하지 않는다.

## QobButton pressed motion and input

The renderer now uses shared `Sources/iOS/HMButtonState.hpp`, reconstructed from
QobButton init0x82b8/draw0x850c/onTap0x87f8/releaseAction0x8494. Generic defaults
are depth2 over0.1 seconds, then release sine motion with height42 and duration0.4.
The original y-up draw subtracts displacement while pressed and adds it on release.
Release amplitude is `sinf(float(2*pi*t/delay))*height*(delay-t)`, with the original
float/double conversions; elapsed>=delay clears the release timestamp.

Actual GuiGame constructors override the generic defaults:

| Controls | pushDepth | pushDelay | releaseHeight | releaseDelay | Evidence |
|---|---:|---:|---:|---:|---|
| Menu100, resume101, return102, booster1000, weapon1100 |0|0.1|0|0|0x151a0|
| Web103 |1|0.01|0|0.01|0x151a0|
| Upgrade choices1200..1202 |0|0.1|0|0.1|0x16a98|
| Upgrade OK1300 |1|0.1|1|0.1|0x16a98|
| Stage result1000/1001 |0|0.1|0|0|0xb8f0|

These values affect drawing only, never callback scheduling. Result button1000 and
booster1000 keep separate state despite their shared callback ID. Hit testing uses
the unshifted image rectangle, matching original onTap. Move-out/re-entry restores
the pressed frame and allows inside Pop; cancellation never emits Pop. The host's
per-touch HUD/gameplay routing remains unchanged.

Validation: `Tests/HMButtonStateTests.cpp` passed with ASan/UBSan and warnings as
errors; HMMenus.mm/HMGameHUD.mm passed simulator/device SDK syntax checks. This is source
and timing validation, not a claim of a new Air visual test.
