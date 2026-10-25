# 좌표계와 입력 정책

정본: [canonical](canonical.md). 구현: `Viewport.hpp`, `World.cpp`, `HMGameView.mm`, `HMMedia.mm`, `HMGLRenderer.mm`.

2026-10-25 사용자 승인에 따라 게임 플레이는 전체 화면 비율로 시야를 확장한다. 검증 앱에서 먼저 검사한 뒤 일반 앱의 동일 경로를 활성화했다. 타이틀/선택 화면은 원본480×320 비율 맞춤을 유지한다.

## 공간과 단위

- 게임 월드: 원본 맵 단위, +X 오른쪽·+Y 위쪽. 높이는320, 폭은 `max(480, 320×drawableWidth/drawableHeight)`다. 320은 논리 높이이며 폰의 픽셀 높이 제한이 아니다.
- 화면 위치: `world−camera+(W/2,160)`. 탱크·지형·탄환 크기는 유지한다. 표시 카메라 반폭만W/2로 제한하며 물리 backLine/frontLine과 보스 접근 규칙은 별도다.
- HUD: 원본480×320 묶음이다. 가운데 배치하고 실제 safeArea에 들어가지 않으면 HUD만 균등 축소한다. 입력은 동일 offset/scale을 역변환한다. 원본480 비교 모드는 기존 배치를 그대로 쓴다.
- UIKit 터치: `gx=(px−viewport.x)/scale`, `gy=320−(py−viewport.y)/verticalScale`. [0,W]×[0,320] 외부는 좌표 없음이다. drawable과 터치가 같은 Viewport를 사용한다.

확장 모드에서는 처음 누른 HUD/월드가 해당 터치를 끝까지 맡는다. 모달·로딩·정지 상태는 바깥 추가 영역에서도 월드 입력을 막는다. 화면 밖에서 종료/취소해도 발사를 해제하며 마지막 월드 터치가 끝날 때 해제한다. 원본480 모드는 기존 버튼 등록·이벤트별 역순 검사 규칙을 보존한다.

월드 터치의 정상 해제는 실제 해제 좌표를 원작 `setTap`에 전달한다. `(0,0)` 같은 가짜 좌표를 보내지 않는다. 취소·화면 밖 해제·정지·키 해제는 `releaseFire`로 새 연사만 중단하고 진행 중 연사의 월드 목표점을 유지한다. 필살기3종은 해제 뒤에도 원작 연사를 마치므로 목표점을 덮어쓰면 남은 탄환이 잘못된 곳으로 간다. [전체 무기 절차](weapon-validation-procedure.md)를 적용한다.

검증 패널에서 확장을 끌 수 있다. 정지 중 변경은 선택만 보관하고 재개 후 정상 tick 직전에 적용한다. 정지 그림을 다시 수집하여 stateful draw를 중복 실행하지 않는다. layout은 기하만 계산하며 tick/draw를 실행하지 않는다.

기하와 원본 모드 비교는 CanvasGeometry/Viewport, CoreTests/CameraIntegrationTests/WorldTests에서 검사한다. 실제 확장 GL과 safeArea는 `HMFramePerformanceTests.testExpandedAirAndProMaxCanvasPreservesHeightAndKeepsHUDInsideSafeArea`가 Air912×420·Pro Max956×440 입력과 좌우 안전영역을 각각 검사한다. 기기 입력값을 이용한 시뮬레이터 검사이며 물리 Pro Max 검사로 표시하지 않는다.

## 틸트 방향

`UIInterfaceOrientation`을 사용한다. 이름이 반대인 `UIDeviceOrientation`을 섞지 않는다. 현재 화면 방향은 `window.windowScene.interfaceOrientation`에서 얻는다.

| 실제 UI 방향 | 본체 +Y가 향하는 화면 쪽 | 원본 기준 입력 |
|---|---|---|
| LandscapeLeft (Home 왼쪽, 카메라 오른쪽) | 오른쪽 | x,y 유지 |
| LandscapeRight (Home 오른쪽, 카메라 왼쪽) | 왼쪽 | x,y 부호 반전; z 유지 |

어느 방향으로 잡아도 화면 오른쪽 가장자리를 내리면 오른쪽 이동, 왼쪽을 내리면 왼쪽 이동이다. CoreMotion g→SDL float m/s²→guest g의 기존 정밀도 변환 뒤 실제 센서 X/Y를 위 표대로 변환하고, 원본 50Hz 저역통과 필터에 넣는다. 필터는 `new=float(sample×double(0.1f)+old×(1−double(0.1f)))`다. 월드 입력은 `abs(y)>0.03`이면 `clamp(y×15,−1,1)`, 아니면0이다.

치트 방향 버튼·시뮬레이터 키는 이미 화면 기준 ±0.2g 입력이다. 센서 방향 변환 이후 이 값을 대입하며 두 번 반전하지 않는다. 일반 폰 앱에는 치트 방향 입력을 넣지 않는다. 메뉴/백그라운드에서 수동 입력이 남지 않게 한다.

2026-10-25 결함: 포팅판의 고정 방향을 근거로 기존 LandscapeRight 보정을 제거했으나 새 앱은 양방향 회전을 계속 허용했다. 사용자 실기기에서 반전이 발견됐다. 이 차이를 근거 없이 다시 제거하지 않는다.

검사: `HMFramePerformanceTests.testPhysicalTiltMovesTowardLowerScreenEdgeInBothLandscapeOrientations`는 두 방향의 CoreMotion 표본으로 실제 탱크 이동과 수동 입력 부호를 검사한다. 방향 버튼·키 해제 검사도 함께 유지한다. 물리폰을 손으로 기울인 결과는 별도로 기록한다.

방향 정의: [Apple UIInterfaceOrientation](https://developer.apple.com/documentation/uikit/uiinterfaceorientation). 게임 입력 근거: 원본 `processMyMach 0xc830`, 필터 `0xb55c`.
