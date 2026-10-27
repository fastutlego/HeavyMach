# 화면 표시 차이 A/B 검증 — 2026-10-24

정상 IPA 호환판과 복원판의 표시 영역·확대 필터 차이를 실제 복원판에 하나씩 적용했다. 변경은 테스트 전용이며 제품 표시 코드·자원·저장은 그대로다. xcodegen으로 테스트 파일을 프로젝트에 등록했다.

## 조건

iPhone Air / iOS26.5 Simulator, Xcode27.1. 실제 UIWindow·CAEAGLLayer·OpenGL ES1.1 표시와 기존 사격/음향 코드. 각 조건30프레임 준비 뒤360프레임. 메뉴는 정순, 사격은 역순으로 실행했다. 모든 조건에서 logical480×320과 기존 게임 좌표를 유지했다.

- baseline: 현재 제품 표시 방식(safeArea, NEAREST).
- full_viewport: 호환판처럼 전체 bounds에480:320 비율로 맞춤. 화면이 더 커지며 game coordinate를 늘리지는 않는다.
- linear_filter: 호환판의 화면 확대 LINEAR 적용. 원래 sprite 텍스처 필터는 양쪽 모두 LINEAR이므로 그대로다.
- full_plus_linear: 위 두 표시 차이를 함께 적용.
- scale1_load_probe: 표시 buffer만912×420으로 줄여 픽셀 부하를 분리. 호환판 기본 설정이라고 주장하지 않는다.
- no_render_control: 같은 표시 타이머를 사용하되 frame 작업(게임·음향·렌더)을 수행하지 않는 대조 조건.

타이머 실제 호출 시각(CACurrentMediaTime)과 CADisplayLink.timestamp를 따로 수집했다. 25ms 초과를 한 화면 주기 이상 건너뛴 후보로 집계했다.

## 최종 실행 결과

| 장면 | 조건 | 표시 buffer | 처리 p95 ms | 처리 최대 ms | 화면 timestamp 간격 최대 ms | 25ms 초과 간격 /360 |
|---|---|---|---:|---:|---:|---:|
| slot_preview | baseline | 2736×1260 | 10.557 | 11.646 | 16.667 | 0 |
| slot_preview | full_viewport | 2736×1260 | 11.311 | 14.462 | 33.333 | 1 |
| slot_preview | linear_filter | 2736×1260 | 18.977 | 19.412 | 35.557 | 360 |
| slot_preview | full_plus_linear | 2736×1260 | 20.403 | 21.979 | 36.980 | 175 |
| slot_preview | scale1_load_probe | 912×420 | 5.440 | 5.657 | 16.667 | 0 |
| slot_preview | no_render_control | 2736×1260 | 0.002 | 0.007 | 16.667 | 0 |
| stage01_firing | no_render_control | 2736×1260 | 0.001 | 0.028 | 16.667 | 0 |
| stage01_firing | scale1_load_probe | 912×420 | 5.436 | 6.051 | 16.667 | 0 |
| stage01_firing | full_plus_linear | 2736×1260 | 20.125 | 21.417 | 36.885 | 283 |
| stage01_firing | linear_filter | 2736×1260 | 19.014 | 19.231 | 35.041 | 250 |
| stage01_firing | full_viewport | 2736×1260 | 11.148 | 11.324 | 16.667 | 0 |
| stage01_firing | baseline | 2736×1260 | 10.351 | 11.464 | 16.667 | 0 |

아래 실제 호출 간격은 위의 화면 timestamp 간격과 별도로 집계했다.

| 장면 | 조건 | 실제 호출 간격 최대 ms | 25ms 초과 /360 |
|---|---|---:|---:|
| slot_preview | baseline | 18.286 | 0 |
| slot_preview | full_viewport | 18.318 | 0 |
| slot_preview | linear_filter | 33.402 | 360 |
| slot_preview | full_plus_linear | 35.881 | 126 |
| slot_preview | scale1_load_probe | 33.242 | 1 |
| slot_preview | no_render_control | 33.362 | 1 |
| stage01_firing | no_render_control | 31.633 | 1 |
| stage01_firing | scale1_load_probe | 33.368 | 1 |
| stage01_firing | full_plus_linear | 35.954 | 263 |
| stage01_firing | linear_filter | 33.381 | 235 |
| stage01_firing | full_viewport | 32.959 | 1 |
| stage01_firing | baseline | 33.258 | 1 |

## 판정

화면 확대 필터는 큰 비용 차이를 만들었다. 호환판의 LINEAR를 복원판에 적용하면 시뮬레이터에서 처리시간이 약19~21ms로 늘고 약33ms 화면 간격이 반복됐다. 현재 복원판의 NEAREST가 이 조건에서는 더 빠르다. 따라서 현재 복원판의 간헐적32ms 간격을 “호환판과 다른 필터라서 발생”이라고 결론낼 근거는 없다.

전체 화면 영역으로 바꾼 경우 비용 변화는 작았다. 좌표 변환 차이가 있다는 사실과 프레임 지연 원인이라는 판정은 구분한다. 출력 해상도를1배로 줄이면 처리비용은 감소했으나 호환판도 기본 HiDPI이므로 해상도를 낮춰야 원본과 같아진다는 결론은 아니다.

아무것도 그리지 않는 대조 조건에서도 실제 호출 간격이 메뉴33.362ms, 사격31.633ms로 각각1회 벌어졌다. 이때 게임/음향/렌더 작업은 모두 생략했고 화면 timestamp 간격은16.667ms였다. 두 경우 모두 frame30(첫 측정 표본, 약0.5초 준비 직후)에 발생했다. 따라서 이 간헐적 호출 지연을 캔버스·좌표 변환·확대 렌더 작업만으로 설명할 수 없다. 호출 스케줄/검사 시작 구간 영향은 추가 구분이 필요하다. 현재 baseline 사격에서는 실제 호출 간격33.258ms가1회 관측됐다. 해당 순간 CADisplayLink.timestamp 간격은16.667ms였고 직전 frame 작업은10.492ms였다. 직전 그리기 처리시간과 다음 호출까지의 대기시간에 차이가 있었다. 이12개 구간에서 재현한 간헐적 호출 지연의 원인은 추가 조사한다.

이 시험은 호환 실행기 전체를 복제한 것이 아니다. 원본 renderbuffer→CopyTexSubImage2D 경로와 복원판의 직접 texture-FBO 경로 차이, NSTimer/호환 루프와 CADisplayLink 차이는 남아 있다. 원본 파일을 덮거나 실기기 FPS를 시뮬레이터 값으로 대체하지 않았다.

## 기록 품질과 보존

첫 실행 `comparison.xcresult/log`도 성공했지만 LINEAR 조건의 긴 late_events JSON은 NSLog32KiB 한도로 잘렸다. `initial-results.json`은 잘리기 전의 frame_work/display timestamp 집계만 복원했으며 잘린 항목을 표시했다. 또 최초 scale1 요청이 UIKit에 의해 다시3배가 된 것을 실제 drawable 크기에서 확인했다. 해당 최초 조건은 해상도 비교로 사용하지 않는다.

후속 `corrected.xcresult/log`는 표시 직전에 layer.contentsScale을 적용하고 실제912×420 결과를 확인했다. 로그는 최초8개 지연 이벤트와 전체 지연 횟수를 분리해 잘림을 방지했다. 이 보고서의 표는 후속 실행만 사용한다. 합격 판정은 실험 완료·렌더 오류 없음·사격 workload 존재에 대한 것이며 느린 LINEAR 조건까지60Hz 성능 통과했다는 뜻이 아니다.

증거: `HeavyMach/build/presentation-ab-20261024/`의 corrected.xcresult/log, results.json, manifest.json. 검사 소스: `HeavyMach/Tests/iOS/HMPresentationComparisonTests.mm`. 일반 앱 코드는 이번 작업에서 변경하지 않았다.
