# 좌표계와 입력 정책

정본: [canonical](canonical.md). 구현: `Viewport.hpp`, `World.cpp`, `HMGameView.mm`, `HMMedia.mm`, `HMGLRenderer.mm`.

2026-10-25 사용자 승인에 따라 게임 플레이는 전체 화면 비율로 시야를 확장한다. 검증 앱에서 먼저 검사한 뒤 일반 앱의 동일 경로를 활성화했다. 후속 승인으로 타이틀/선택 화면도 가변폭을 사용한다. 원본480 비교 모드는 유지한다. 메뉴는 전투 월드와 분리된 표시·터치 배치를 사용한다.

## 공간과 단위

- 게임 월드: 원본 맵 단위, +X 오른쪽·+Y 위쪽. 폭은 `max(480, 320×drawableWidth/drawableHeight)`, 높이는 `max(320, 480×drawableHeight/drawableWidth)`다. 320은 논리 높이이며 폰의 픽셀 높이 제한이 아니다.
- 화면 위치: `world−camera+(W/2,160)`. 높이가 늘면 기존 바닥 기준을 유지하고 위쪽을 더 표시한다. 탱크·지형·탄환 크기는 유지한다. 표시 카메라 반폭만W/2로 제한하며 물리 backLine/frontLine과 보스 접근 규칙은 별도다.
- HUD: 원본480×320 묶음이다. 가운데 배치하고 실제 safeArea에 들어가지 않으면 HUD만 균등 축소한다. 입력은 동일 offset/scale을 역변환한다. 원본480 비교 모드는 기존 배치를 그대로 쓴다.
- UIKit 터치: `gx=(px−viewport.x)/scale`, `gy=H−(py−viewport.y)/verticalScale`. [0,W]×[0,H] 외부는 좌표 없음이다. drawable과 터치가 같은 Viewport를 사용한다.

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

## 보스 접근과 표시 좌표

확장 시야의 보스 선행 생성은 표시·피격 준비다. 이동과 무기AI는 원래 배치가 player+500 안으로 들어오는10단위 진행 샘플에서 시작한다. 화면 위치·이동한 보스 위치로 발동점을 바꾸지 않는다. Stage14 항공기는 map.right1459와 전투 frontLine1330을 두 폭에서 동일하게 사용한다. 화면 오른쪽 끝을 AI 목표점으로 새로 넣지 않는다. [실행 증거](boss-motion-fix-20261025.md).

## 타이틀·메뉴 확장

타이틀 배경 중앙은 캔버스 중앙, 로고·문구는 안전영역 중앙이다. 슬롯 카드 틀과 내용은 같은 이동량을 적용하고 버튼 hit bounds에도 그 이동량을 더한다. 카드 하단과 확인/삭제 버튼은 safeArea.bottom을 피한다. 모드·미션은 원본 묶음을 안전영역 중앙에 두며 미션 확인 버튼의 하단을 보정한다. layout은 tick을 진행하지 않는다. [현재 적용과 검사](menu-art-extension-20261025.md).

## 세로 표시 범위와 키보드

iPad 4:3 화면은480×360이다. HUD 원본480×320 묶음은 안전영역 안에서 가운데 배치하고 화면 입력은 같은 offset/scale을 역변환한다. 메뉴 주변은 기존 배경 픽셀로 채우며 원본 에셋을 덮어쓰지 않는다. 결과 일러스트는 W×H 전체를 덮는다. 표시 높이가 늘어도 원작 카메라 Y, AI의320높이 활성 조건, 맵 경계·보스 발동 조건은 유지한다. 새로 보이는 영역의 지형·적 렌더와 탄환/적 충돌 판정에는 표시 높이를 사용한다.

창 크기 변경은 게임 tick을 추가하지 않는다. 정지 중에는 캐시의 표시 X를 새 폭에 맞춰 옮기고 배경을 채운다. 원본480 비교 모드 전환은 기존대로 재개 직전 적용한다. 키보드 가상 기울기·조준·메뉴 규칙은 [입력 정책](keyboard-input-policy.md)을 따른다.

## VTOL 추가 기체

새3종의 그림·무기 장착점·노즐은 기체 중심 기준 +Y 위쪽 좌표를 공유한다. 회전행렬 하나로 장착점을 월드에 변환한다. 원래 무기 수식을 바꾸지 않고 새 기체의 CombatFrame에 변환된 발사점을 전달한다. 기체 사각형 네 꼭짓점의 회전 결과로 피격 범위를 갱신하며 불꽃은 제외한다. 기존 적의 좌표·충돌 경로는 유지한다. [설정](../Sources/Core/VTOL.cpp).

VTOL 후속 크기 수정: 야크112·해리어108·F35B116. 높이는 이미지 비율을 유지하고, 발사점·후방 노즐·피격 범위도 이전 크기에 대한 비율로 함께 줄인다. 원래 적과 맵 배치 좌표는 유지한다.

## 전체 화면 문

`LoadingTransition`의 원작480×320 이동 상태는 유지한다. `expandedPanels(W,H)`가 화면 중심 이동, 외곽 철판 확장량, 화면 밖으로 열리는 이동 거리를 계산한다. 닫힌 순간의 표시 좌표만 소수점 틈 없이 맞추며 tick이나 이벤트 시점은 변경하지 않는다. `HMGameView.drawLoading`은 카메라·HUD 이동·확대·clip을 초기화하고 문을 그린다. 버튼·월드·터치 좌표에는 이 변환을 적용하지 않는다. [검사](public/fullscreen-transitions-20261027.md).

## 자동조준 HUD 버튼

A·다음 표적·사격은 기존480×320 HUD 좌표에 배치한다. 버튼 위치에도 `_hudOffset`과 `_hudScale`을 적용하고, 같은 사각형을 터치 판정에 쓴다. 기기의 화면 끝을 기준으로 별도 배치하지 않는다. 기존 점프·무기·게이지 영역 오른쪽에 붙이며 M·C의 터치 영역은 제거한다.
