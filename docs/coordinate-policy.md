# 좌표계와 입력 정책

정본: [canonical](canonical.md). 구현: `Viewport.hpp`, `World.cpp`, `HMGameView.mm`, `HMMedia.mm`, `HMGLRenderer.mm`.

사용자의 2026-10-25 전체 캔버스·시야 확장 요청은 [확장 계획](wide-canvas-plan.md)에 따른다. 아래 480×320 규칙은 현재 제품 기준이며 확장 구현을 금지하는 조건이 아니다. 확장을 구현할 때 관련 정책·입력·검사를 함께 갱신한다.

## 공간과 단위

| 공간 | 원점·축·단위 | 변환 책임 |
|---|---|---|
| 게임 논리 화면 | 480×320, 왼쪽 아래 원점, +X 오른쪽, +Y 위쪽 | 게임/HUD는 이 공간을 사용 |
| 월드 | 맵 데이터 원점, +X 오른쪽, +Y 위쪽; 음수 좌표 허용 | World/Geometry/Motion |
| 카메라 | 월드 좌표의 화면 중심 | 화면 위치는 월드−카메라+(240,160), 흔들림은 기존 CameraShake 경로로 적용 |
| UIKit 터치 | 뷰의 왼쪽 위 원점, +Y 아래, point | Viewport.logical에서 한 번만 게임 좌표로 변환 |
| drawable | 실제 픽셀, 화면 크기와 nativeScale 반영 | Viewport.drawable/GLRenderer |
| CoreMotion | 화면 회전과 무관한 본체 portrait 축, g 단위 | HMGameView.sampleOriginalAcceleration |

화면 비율은 3:2로 유지한다. 전체 drawable에 맞춘 뒤 남는 공간을 검정 여백으로 둔다. safeArea를 새 게임 캔버스로 사용하거나 맵·HUD 좌표를 화면 크기에 따라 다시 설계하지 않는다. 정수 픽셀 반올림과 홀수 여백은 `Viewport::drawable` 계산을 사용한다. X/Y 유효 배율은 픽셀 반올림 때문에 미세하게 다를 수 있다.

UIKit point `(px,py)`에서 `gx=(px−viewport.x)/scale`, `gy=320−(py−viewport.y)/verticalScale`이다. [0,480]×[0,320] 밖은 게임 좌표 없음으로 처리한다. 여백에서 터치를 끝내도 기존 발사를 해제할 수 있어야 한다. renderer와 입력에서 서로 다른 viewport를 계산하지 않는다.

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
