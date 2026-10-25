# 그래픽 렌더링 정책

정본: [canonical](canonical.md). 제품 경로는 `HMGameView → HMMedia → HMGLRenderer`다. 이전 CPU raster/UIImage 합성은 비교·검사 경로로 남아 있으며 일반 게임의 정상 렌더 경로로 복귀시키지 않는다.

2026-10-25 전체 캔버스 확장을 구현했다. 원본480 비교 모드와 확장 게임플레이 모드를 같은 GL 경로로 처리한다. 현재 구현과 검사는 [캔버스 기록](wide-canvas-plan.md)을 따른다.

## 고정할 경로

- 게임 장면은 OpenGL ES1.1. UIKit은 창·수명주기·검증 조작부를 담당한다.
- 논리W×320, 할당ceil(W)×320 RGBA8 renderbuffer에 원본 순서로 그린다. 기본·원본 비교 모드는W=480이다. 크기가 바뀔 때만 재할당하고 투영·복사·readback이 같은 치수를 사용한다. 장면을 재사용 화면 텍스처로 복사하고 drawable에 최종 확대한다.
- 원본 자원의 제품 소비자는 `OriginalImages`의 원본 파일명과 확장자를 사용한다. 미션/스테이지 클리어의 `GameLoading_XX[E].jpg`, 게임오버의 `StageClear.jpg`에 파생본 접미사 `.png`를 붙이지 않는다. `Images/*.jpg.png`는 변환·비교용 자원이며 현재 제품 로더의 이름 계약이 아니다.
- 원본 이미지695개를 보존된 stb 코드로 해석한다. POT 패딩, premultiplied 픽셀, RGB565 변환, UV와 타일 순서를 임의 보정하지 않는다.
- sprite 텍스처는 LINEAR/REPEAT, 최종 화면 텍스처는 LINEAR/CLAMP_TO_EDGE다. 두 텍스처의 규칙을 섞지 않는다.
- drawable은 nativeScale과 RGB565 요청을 사용한다. 실제 드라이버 보고 형식은 별도 기록한다. 요청이 실제 RGB565 할당을 증명하지 않는다.
- 마지막 확대는 전체 화면 비율 맞춤 NDC quad, DITHER 비활성·blend 비활성 후 원래 상태 복원이다. 원본480 모드에서는 검정 여백을 유지하고 확장 게임 화면은 전체 drawable을 채운다. viewport와 터치는 [좌표 정책](coordinate-policy.md)을 함께 따른다.

## 합성과 순서

| Sprite blend 값 | GL source | GL destination |
|---|---|---|
| 0,3 | SRC_ALPHA | ONE_MINUS_SRC_ALPHA |
| 1 | SRC_COLOR | ONE_MINUS_SRC_ALPHA |
| 2 | SRC_ALPHA | ONE |
| 그 외 | 기존 상태 유지 | 기존 상태 유지 |

텍스처 환경은 원본 GL_REPLACE를 보존한다. Trail의 blend3는 sprite와 같다고 가정하지 않는다. `Trail.cpp`가 만든 setBlend/sourceFactor/destinationFactor를 renderer가 소비한다. 원본의 계층·등록·z순서·숨김 상태를 유지하고 최적화를 이유로 투명 sprite 순서를 임의 재배치하지 않는다.

## 자원 수명과 프레임 비용

공유·화면·월드 scope가 소유자다. GPU 업로드 후 CPU 이미지 픽셀을 해제하고 크기/형식 정보를 보관한다. 새 화면 준비 뒤 이전 전용 scope를 해제하며 맵 ClearTileResources에서 월드 텍스처를 해제한다. 동일 이름도 원본 소유자가 다르면 분리한다.

프레임마다 파일 읽기·이미지 재해석·UIImage 생성·부품 벡터 정렬을 추가하지 않는다. layoutSubviews/drawRect에서 게임 tick/draw를 중복 호출하지 않는다. 수명이 끝난 탄흔은 그 프레임에 그리지 않는다. 새 자원의 최초 준비 비용과 반복 프레임 비용을 따로 측정한다.

## 검사와 변경 판정

GL 오류, 논리 화면 픽셀, 최종 표시 배치, 자원 해제, 실제 표시 간격을 각각 확인한다. 완전 픽셀 일치가 실패하면 차이를 보존한다. 평균 FPS만으로 긴 프레임을 지우지 않는다. 장면·기기·OS·빌드·계측 구간·열 상태를 기록한다.

시뮬레이터 Apple Software Renderer의 수치를 아이폰 GPU 성능으로 환산하지 않는다. 같은 환경 포팅판과의 비교는 회귀 분리에 사용한다. 16.67ms 기존 검사 실패 기준을 낮추지 않는다. API 변경·저해상도화·확대 필터 변경을 성능 해결의 기본 처방으로 삼지 않는다.

원본/호환판 주소와 최종 비교 근거: [직접 비교](compat-source-comparison-20261024.md), [OpenGL 전환](opengl-migration-20261024.md), `HMGLRenderer.mm`, `HMCompatibilityPixels.hpp`, `SpriteRaster.cpp`, `Trail.cpp`.


## 확장 장면·중앙 HUD

월드 그림은 전체 폭, HUD/업그레이드/결과 글자·버튼은 원본480 영역에 같은 transform과 clip을 적용한다. **결과 일러스트는 별도로 전체 캔버스를 덮는다.** 원본 이미지 비율을 유지해 중앙 확대하고 높이 방향의 초과분을 자른다. 확장 모드의 scale은 `max(W/(imageWidth−1),320/(imageHeight−1))`이며 LINEAR/REPEAT가 원본 이미지 가장자리 밖의 검정 POT 여백을 섞지 않게 반 texel 여유를 확보한다. 원본480 모드의 일러스트 배치·크기는 유지한다. 그림을 그린 뒤 HUD 변환·clip을 복원하므로 결과 버튼의 위치와 입력은 유지된다. 퇴장 중인 패널이 추가 월드 영역에 다시 보이지 않게 한다. 로딩 중에는 확장 월드를 숨기고 중앙 원본 셔터를 표시한다. 중단된 결과 전환의 재개 메뉴는 일러스트보다 위에 그린다.

일부 원본 배경은 새 폭보다 작다. 확장 모드에서 배경만 `max(1,W/imageWidth,320/imageHeight)`로 균등 확대하고 끝부분 위치를 제한한다. 비율 왜곡·새 에셋·근거 없는 반복 타일은 없다. 탱크·지형·효과 크기는 바뀌지 않는다. 원본480은 배경 크기·parallax 수치도 유지한다.

CPU raster는 원본480 비교 전용이다. 확장 offscreen 진단은 실제GL 버퍼 snapshot을 사용한다. 제품 onscreen 경로는 계속 GL 직접 present이며 readback/UIImage를 매 프레임 만들지 않는다. `HMGLReferenceTests`의 원본8개와 확장/resize/clip4개를 함께 유지한다.

## 생성된 화면 확장 자원

모드·미션은 각 원본별 `ModeWide`/`MissionWide` 생성 배경의 바깥 사분면을 연결한다. 공통 MenuMetal은 슬롯에만 쓴다. 확장 타이틀·모드·미션의 원본 배경은 반 texel 테두리 UV를 유효 색에 고정하여 검정 POT 여백 혼입을 막는다. 중앙 UV와 원본480/sprite 경로는 유지한다. 화면별 자원 수명과 원본 정책을 우회하지 않는다. [검은줄 재현·검사](menu-joins-20261025.md).

사용자 승인으로 `Art/Extensions`의 별도 생성 이미지를 번들 `Extensions`에서 읽는다. 타이틀은 생성 이미지의 바깥 두 사분면만 원본과 같은 이동·회전·확대 변환으로 그린다. 원본 사각형 바깥20단위에만 가장자리 반사/알파 전환을 적용한다. 이 보조 draw에서만 GL_MODULATE를 쓰고 즉시 GL_REPLACE로 복원한다. 원본 sprite와 전투 렌더 규칙은 유지한다. 메뉴 원본 부분은 텍스처 clip으로 재배치하며 CPU bitmap을 만들지 않는다. 생성 자원도 화면 scope의 업로드·해제 규칙을 따른다. [적용·포맷 비용·증거](menu-art-extension-20261025.md).
