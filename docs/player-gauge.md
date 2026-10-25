# 플레이어 머리 위 HP 막대

2026-10-24. 원본 survival 화면과 native 화면의 차이를 원본 함수로 확인했다. native HUD의 HP 값은 존재했지만, 플레이어 자식 이미지 두 개가 session renderer에 빠져 있었다.

## 원본 근거

- `GobHvM_Player::init` 0x1bb84는 GameMain.screen==6일 때만 `HPGauge.png`를1×2로 나눈다. 별도 player draw override가 아니라 QobImage 자식들이다. scenario/survival gameplay 모두 포함하고 메뉴 preview는 제외한다.
- 배경frame1은 player상대(0,60), fixedPos1이다. 채움frame0은 배경의 자식이며 배경상대(0,1), fixedPos1이다. 추출 PNG는42×10이므로 각 frame은42×5다.
- `setHPGauge:` 0x1d278은 채움 scaleX=fraction, X=−(fraction×−21+21)를 설정한다. Y는 초기값1을 유지한다. 같은 fraction을 GuiGame HUD에도 전달한다.
- `QobBase::init` 0x76a0의 기본 layer는2이다. player는 생성 후 `GobWorld::addMyMachX:Y:` 0xd474에서 layer1로 바뀐다. `setLayer:` 0x8294는 자신의 layer만 변경하므로 이미 만들어진 막대 자식들의 layer2는 유지된다. QobImage init0x8b08의 blend는0이다.
- `QobBase::tick` 0x7830은 부모 show가 false면 자식을 처리하지 않는다. 부모가 카메라 밖이라는 이유만으로 자식 tick을 건너뛰지 않는다. 두 막대는 각각 QobBase.isInCam0x7c78로 검사한다.
- fixedPos는 screenPosX0x7a28/screenPosY0x7b00에서 `int(value+0.5f)`로 변환한다. 음수에 일반 round를 적용하는 것과 다르다. 이미지 bounds는 원본42×5이며 scaleX가0이어도 culling bounds를0으로 바꾸지 않는다.

## 변경

GameSession은 기존 PlayerEvent.HPGauge에서 playerGraph.gauge를 갱신한다. machineDraw는 player가 보이고 hasHPGauge가 참이면 두 이미지를 생성한다. playerGraph.inScreen 검사 전에 독립적인 자식 화면 판정을 수행한다. 좌표에는 player 위치와 카메라 변환을 적용하고 fixedPos 정수 변환을 거친다. 회전이나 engineBounce를 막대에 추가하지 않는다. pause에서도 그리기를 유지한다. 기존 HUD 이벤트 전달과 HP 계산, 저장, game-over 규칙은 변경하지 않았다.

## 검사

`Tests/PlayerGaugeTests.cpp`는 실제 GameSession 구현을 포함하는 내부 연결 회귀다. 원본 raw data를 읽고 테스트에만 고정 이미지 크기를 제공한다. 자산과 실제 저장 파일은 쓰지 않는다.

- 수정 전 전체 HP에서 막대2개 존재 assertion이 실패했다(exit134).
- 수정 후 full/half/zero의 frame, 좌표, scale, layer2, blend0을 확인했다.
- pause 유지, parent.visible=false와 hasHPGauge=false에서 숨김을 확인했다.
- 부모는 화면 밖이지만 막대는 화면 안인 경우와 그 반대를 확인했다.
- 음수 fixedPos와 각 자식의 독립 culling으로 채움만 보이는 경계를 확인했다.

일반 Debug와 AddressSanitizer/UndefinedBehaviorSanitizer 독립 실행이 모두 exit0으로 통과했다. 앱 build/install/launch와 실제 화면 비교는 이 검사에서 수행하지 않았다.

## 후속: quake와 fixedPos 순서

독립 검토에서 원본0x7a28/0x7b00은 camera.offset을 뺀 뒤 fixedPos 정수 변환을 수행한다는 차이를 확인했다. 최초 native 구현은 정수 변환 뒤 UIKit의 fractional quake 이동을 적용했다. 예를 들어 y=180.25, offsetY=.75일 때 원본은 int(180.25−.75+.5)=180이고 기존 결과는180−.75=179.25였다.

GameSession.setCameraOffset은 tick 직전에 host의 현재 +Y-up quake offset을 받는다. collect는 기존 cameraScreenPosition helper로 offset 차감→fixedPos→개별 이미지 culling을 수행한다. render position에는 offset을 다시 더해 저장한다. host가 전체 world에 적용하는 −offset 이동을 상쇄하는 좌표 계약이며, quake를 두 번 적용하지 않는다. collect를 추가로 호출하지 않아 draw-time RNG 소비 횟수는 바뀌지 않는다. host의 CameraShake.tick은 session.tick 앞에 있고, tick 뒤 event의 setQuake는 시작 시각/크기만 변경하며 현재 offset을 즉시 변경하지 않는다.

ASan/UBSan PlayerGaugeTests에 fractional offset의 최종 정수 좌표, quake 때문에 화면 밖으로 나가는 경우와 안으로 들어오는 경우를 추가했고 exit0을 확인했다. 이 수정은 HP 자식 이미지에 한정한다. 다른 world sprite의 카메라 판정은 아직 quake 없는 좌표로 수행되는 경로가 남아 있으며, 이번 검사를 전체 화면 culling 동등성으로 확대하지 않는다.

## 후속: tick 등록과 재귀 좌표

위 collect 시점 culling은 후속 [HP 막대 등록 수정](gauge-registration.md)에서 tick 시점 등록으로 바꿨다. draw에서는 현재 좌표와 HP 비율만 다시 읽는다. `ownerY + (60 + 1)` 대신 `(ownerY + 60) + 1` 순서로 계산한다. 특정 소수 좌표에서 나던1픽셀 차이와 tick/draw 사이 HP 변경 회귀의 수정 전 실패·수정 후 통과를 기록했다. 이전 검사의 범위를 전체 원본 동등성으로 확대하지 않는다.
