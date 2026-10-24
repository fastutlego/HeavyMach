# 원본 메뉴 복원

`Sources/iOS/HMMenus.h/.mm`은 GuiSelectSlot, GuiSelectMode, GuiSelectMission의 원본 데이터·좌표·선택 분기를 UIKit/CoreGraphics로 옮긴다. 원본 실행 파일이나 호환 실행기는 사용하지 않는다. 그림은 원본에서 변환한 PNG, 글자는 원본 NumSet01 16열 atlas다.

호출자는 `initWithMedia:save:data:`에 수명이 유지되는 SaveData/GameData를 전달한다. `showSlots`, 60Hz `tick:`, 그리기 context 안의 `draw`, `touch:state:`를 호출한다. 터치는 480×320 y-up이며 state는 began0/moved1/ended2/cancelled3이다. `startGame(slot,mode,mission,stage)`는 게임 시작을, `saveChanged()`는 슬롯 생성/삭제 저장을 알린다. 화면 배율과 safe-area 변환은 부모 GameView가 소유한다.

## 원본과 대응

|화면/함수|주소|재현한 동작|
|---|---|---|
|Slot init|0x18158|3칸, OK(422,21), DEL(332,20), 초기 선택 강조(83+155i,144)|
|Slot refresh|0x186d8|마지막 사용 시간이 가장 큰 슬롯 선택, 없으면 가운데 슬롯; 생성/수정 날짜, 레벨·부품 숫자, 경험치 게이지|
|Slot button|0x18fc0|슬롯은 down 때 선택; OK/DEL은 release; occupied 슬롯만 삭제 창; YES는 occupied byte만0으로 변경|
|Mode init/handler|0x1a55c/0x1a900|Scenario 기본 선택, Survival 선택 후 OK는 mode1/mission4/stage4|
|Mission init|0x192ec|7개 행, 미션 atlas1×3, stage atlas1×3, 본문1×8, 슬롯1×2|
|Mission tick|0x19ba0|매 tick 현재 좌표에 (목표−현재)/5 추가|
|Mission setTop|0x19ea4|top을 먼저0 이상으로 제한 후 lastMission−1 이하로 제한; 신규 슬롯 top−1 유지|
|Mission stage/handler|0x19d18/0x1a05c|해제된 mission만 선택; lastMission의 stage는 저장된 stage까지; 이전 mission은 stageCount−1까지|
|ImageFont setText/draw|Evidence/menu-font-setText.json /0x9054|숫자0–9, 점10, 슬래시11, 콜론12, 대시13; 공백은 폭만 차지|
|Player part attachment|0x1be84|body→top socket1, top→weapon socket1; 부품 교체마다 rotation−0.01, weapon aim0.1|

슬롯 삭제 확인은 원본 배경(240,140), YES(236,108), NO(307,108)를 사용한다. 확인 창이 보이면 뒤 버튼과 슬롯은 입력을 받지 않는다. 새 슬롯은 `GlobalInfo::selectSlot(unixTime)`으로 초기화한다. 미션 선택은 저장된 해제 진행도를 변경하지 않는다.

미션 행의 기본 위치는 (220,250−96i)다. 행 안의 미션 그림은 (−114,4), 제목은 (18,26), 좌우 버튼은 (78,4)/(140,4), stage 표시는 (−22+35j,−28)이다. 현재 해제 미션의 다음 행은 잠긴 모습으로 보인다. 위로 벗어난 행은 x 방향으로 이동하고 아래 행은 y−50으로 이동한다. 원본의 top−1 분기를 보존하여 신규 슬롯의 첫 행은 최종 y154에 온다.

슬롯 기체는 `Visuals`의 부품/무기 renderer와 `Motion::processModel`을 사용한다. 각 슬롯의 저장된 body/top/weapon 등급을 적용하고 (82+155i,166)에 그린다. 선택된 기체만 이동 프레임·engine bounce·원본 랜덤 조준을 갱신한다. body/top/weapon 순서로 그린다. 선택 테두리와 숫자 글꼴은 additive blend다.

## 확인 범위

REA 별도 세션에서 원본 함수 증거를 `Resources/Evidence/menu-*.json`에 저장했다. shared active target은 변경하지 않았다. simulator arm64와 device arm64 SDK27.1에서 C++20/ARC `-Wall -Wextra -Werror` 구문 검사를 실행했다. 모든 메뉴 이미지·사운드와 동적 미션/스테이지 atlas 이름이 실제 변환 자산에 존재함을 확인했다.

이 단계의 구문/자산 검사는 simulator에서 실제 터치, 화면 배치, 저장 후 재실행 검증을 대신하지 않는다. 앱 통합 후 슬롯 생성, 세 슬롯 선택, 삭제 NO/YES, Scenario stage 제한, Survival 시작, 미션 스크롤을 확인해야 한다.

## QobButton input restoration

`HMButtonState.hpp` restores the shared QobButton behavior from original
`Analysis/functions/0x82b8.pseudo.c` (defaults), `0x850c.pseudo.c` (draw),
`0x87f8.pseudo.c` (onTap) and `0x8494.pseudo.c` (releaseAction).
Push/Pop notifications are synchronous; pushDelay/releaseDelay only affect drawing.
Moving out clears the pressed frame, moving back in restores it without another
Push notification, and ending inside emits Pop even after that re-entry. UIKit
cancellation releases the visual state without activating a button. Hit rectangles
remain at their base positions, independent of the drawn vertical displacement.

The menu constructors at0x18158/0x186d8/0x1a55c/0x192ec explicitly set pushDepth,
releaseHeight and releaseDelay to zero. This restoration preserves those overrides;
it does not add generic default bounce to menu buttons. The existing mission-row
scroll easing remains unchanged. Screen changes clear per-button state so reused
numeric IDs cannot inherit a previous screen's press.

Portable `HMButtonStateTests` checks sine timing, immediate notification, release
expiry, move-out/re-entry/pop, cancellation, zero-motion overrides, and the upgrade
confirmation values. ASan/UBSan and simulator/device Objective-C++ strict syntax checks
passed. Actual Air interaction for this change still requires the host build/run.

## Native loading transitions

Menu confirmations request the original GameMain screen IDs through `changeScreen`;
they no longer replace the current screen immediately. Slot confirmation still
selects and saves the slot immediately, retaining the committed slot independently
of subsequent selection-frame touches. Mode/mission controls remain live during
the shutter animation. At synchronous RefreshScreen6, `startSelectedGame` uses the
current selected mode/mission/stage. Other callbacks remain immediate.

Verified routes are title→3(slot), slot→2(mode), scenario mode→4(mission),
mission/survival→6(game), and world-return→the original event ID (3 slot or4 mission).
Sources: title0xf934, slot0x18fc0, mode0x1a900, mission0x1a05c, HUD0x17c4c.
The original refreshScreen switch at0x38cc dispatches selectors via literals
0x38f8..0x3908, matching1 title/2 mode/3 slot/4 mission/6 game.

HMGameView runs `LoadingTransition::tick` before menu/world/title child ticks,
creates the new screen synchronously in RefreshScreen, and draws all four loading
panels last in logical480×320 coordinates outside camera shake. Loading_Close and
Loading_Open consume module events. The transition adds no input gate and accepts
repeated requests, matching GameMain0x3890. The generic module handles IDs1..7;
unused native routes5/7 remain explicitly unrestored.

Integration validation: Xcode Debug build for generic iOS Simulator succeeded
(`/tmp/heavymach-loading-build.log`, derived app under
`/tmp/heavymach-loading-derived/Build/Products/Debug-iphonesimulator/`).
HMGameView.mm/HMMenus.mm also passed device SDK Objective-C++ syntax checks with
`-Wall -Wextra -Werror`. Air visual and interaction verification is delegated to the
host owner and was not performed by this implementation pass.
