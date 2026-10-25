# GameMain loading shutter 복원

> **원본480 코어 분석·검사 이력.** 아래 입력 gate 설명은 원본 코어 범위다. 현행 확장 앱은 HMGameView에서 로딩/모달 동안 월드 입력을 막고 메뉴 셔터를 중앙·좌·우로 반복한다. [좌표 정책](coordinate-policy.md), [렌더 정책](rendering-policy.md)을 함께 따른다. 전체 화면 단일 문은 [기획 HM-DOOR](game-design.md)의 승인·미구현 요구다.

`LoadingTransition.hpp/.cpp`는 원본 GameMain의 화면 전환 상태를 실행 가능한 C++로 복원한다. 원본 증거는 `Analysis/functions/0x2ecc.{pseudo.c,asm}`(초기화), `0x3374`(tick), `0x3890`(changeScreen), `0x3778`(clearAllScreen), `0x38b0`(refreshScreen)이다. 화면 생성·삭제는 host의 책임이다. 이 모듈만으로 iOS 연결 완료를 뜻하지 않는다.

## 원본 상태와 수치

좌표는 480×320 화면, +Y 위, 각 이미지 중심이다. QobImage 초기화 0x8b08은 blend0, frame0, 중심 bounds를 설정하고 draw 0x8cf0은 alpha/scale/rotation을 사용한다. GameMain은 네 이미지를 layer6에 Top, Bottom, Left, Right 순으로 붙인다. camera는 지정하지 않는다.

초기에는 state6, loading 숨김, screen1이다. 실제 최초 title는 init의 refreshScreen으로 곧바로 생성한다. `changeScreen(1..7)`는 screen을 기록하고 state0을 설정한다. 범위 검사는 0x3890 assembly의 unsigned `screen-1 <= 6`이다. 진행 중 재요청도 거절하지 않는다.

| 상태 | 한 tick의 연산 | 다음 상태 |
|---|---|---|
| 0 | show=true; top(240,423), bottom(240,-100), left(-100,160), right(580,160); Close sound | 1 |
| 1 | top.y→223, bottom.y→101, 매번 `(target-value)/6` 더함 | bottom.y>70이면 2 + Close sound |
| 2 | 위 연산 계속; left.x→245, right.x→235, /6 | left.x>=240이면 양쪽 x=240, state3 |
| 3 | refreshScreen 동기 호출 | 4 + Open sound |
| 4 | left.x→-110, right.x→590, /10 | left.x<0이면 5 + Open sound |
| 5 | top.y→423, bottom.y→-100, 좌우 계속, 모두 /10 | top.y>422이면 숨김 + 6 |
| 6 | 전환 연산 없음 | 6 |

한 tick은 한 상태 분기만 수행한다. 시간 인자나 임의 duration을 쓰지 않는다. ARM의 float sub/div/add 호출 순서대로 binary32 연산을 분리했다. `-ffp-contract=off` 검증 결과 시작 tick1 → state2 tick12 → state3 tick36 → refresh/state4 tick37 → state5 tick48 → 숨김 tick99이다. 이는 실제 앱의 tick cadence와 결합해야 하며 임의 초 단위 상수로 바꾸지 않는다. 닫기/열기 sound는 각각 두 번 발생한다.

## Host 연결 순서

1. 최초 title는 즉시 표시한다. 다른 화면 요청 시 목적지와 관련 값(slot/mode/mission 등)을 보존하고 `loading.changeScreen(originalScreenID)`를 호출한다. 이때 기존 화면을 즉시 지우지 않는다.
2. 매 게임 tick의 앞부분에서 `loading.tick(sink)`를 호출한다. CloseSound→`Loading_Close`, OpenSound→`Loading_Open`. RefreshScreen에서는 기존 화면 정리와 새 화면 생성을 동기로 수행한다. 모듈은 그 callback이 반환된 뒤 state4 및 OpenSound를 처리한다.
3. 이어 기존 child/menu/world/session tick을 수행한다. 원본 0x3374는 모든 상태에서 마지막에 superclass tick을 호출한다. 전환 중 전체 게임 tick을 정지시키지 않는다. RefreshScreen 당일에는 새 child가 tick된다.
4. 화면을 그린 다음 camera translation을 복구하고 `sprites()`의 네 DrawSprite를 순서대로 그린다. source filename, cols/rows1, frame0, alpha1, scale1, rotation0, blend0, layer6이다. 숨겨진 경우 span은 비어 있다. 반환 좌표를 화면 +Y 위 좌표로 한 번만 변환한다.
5. RefreshScreen handler가 다시 changeScreen을 호출하더라도 원본처럼 이후 state4 대입이 덮어쓴다. 화면 identity 라우팅과 데이터 저장 정책은 기존 host 구현을 유지한다.

별도 전환 input gate는 넣지 않았다. 원본 GameMain에는 onTap override가 없고, QobBase::onTap 0x81b4는 false를 반환한다. Loading layer/image의 생성 경로에는 UI receiver 등록이 없다. QobManager::onTap 0xfc18은 보이는 receiver를 역순으로 호출하며 전환 상태를 검사하지 않는다. 전환 도중 release 전달을 막거나 강제 pause하는 정책은 이 증거로 정당화되지 않는다. 기존 release 보존 처리는 유지한다.

## 검사

`LoadingTransitionTests.cpp`는 초기 숨김·잘못된 screen·첫 위치/float step·상태별 tick 경계·중앙 닫힘 상태의 동기 refresh·sound 순서/횟수·완료 뒤 무동작·진행 중 재요청·sprite 속성을 확인한다.

실행한 명령:

```sh
clang++ -std=c++20 -Wall -Wextra -Werror -ffp-contract=off \
  -fsanitize=address,undefined -DHEAVYMACH_LOADING_TEST_MAIN \
  HeavyMach/Sources/Core/LoadingTransition.cpp \
  HeavyMach/Tests/LoadingTransitionTests.cpp -o /tmp/heavymach-loading-tests
/tmp/heavymach-loading-tests
```

exit0. iOS draw/input/audio 실플레이는 host 연결 후 별도로 확인해야 한다. 후속 소유 확장으로 CMake/CoreTests에 모듈과 테스트를 추가했다. `/tmp/heavymach-loading-cmake` Debug 빌드와 ctest 전체 7개가 통과했다. native 파일은 이 작업에서 수정하지 않았다.
