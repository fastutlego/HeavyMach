# Instruments 시뮬레이터 검증 — 2026-10-24

요청에 따라 실제 Instruments 기록을 수집하고 분석했다. 앱·게임 소스는 이번 검증에서 변경하지 않았다. 이전 보고의 “Instruments trace 미수집”은 이전 작업 범위이며, 이 문서가 후속 결과다.

## 실행 조건

Xcode 27.1 / iPhone Air iOS 26.5 Simulator / Mac M4 Pro. 이전 검증과 같은 Debug 바이너리를 test-without-building으로 실행했다. HMFramePerformanceTests/testOnscreenOpenGLCadenceAudioAndResume를 각 조건에서 4회 반복했다. 매회 타이틀·슬롯 메뉴·실제 사격/음향을 실행하고 각 장면 30프레임 준비 뒤 180프레임을 측정했다. 각 조건 2,160개 측정 프레임이다. 저장 쓰기는 기존 테스트 하위 클래스에서 차단한다.

- Time Profiler: 55초, 전체 프로세스 기록. 4회 검사 통과.
- Allocations + Leaks: 앱 PID에 연결, 48.674초 기록. 4회 검사 통과. 메모리 기록은 마지막 반복이 끝나기 전에 종료됐다.
- Metal System Trace: 앱 PID에 연결, 앱 종료까지 기록. 포함된 Time Profiler도 분석했다. 4회 검사 통과.
- 추적기를 끈 대조 실행: 같은 검사 4회 반복. 결과는 아래 표와 control-tests.log 참조.

## 프레임 측정

p95는 각 반복의 p95 중 최댓값이다. Hz는 CADisplayLink 콜백 평균이며 실기기 scanout FPS가 아니다. 프레임 처리시간이 짧아도 콜백 지연을 별도로 기록한다.

| 조건 | 장면 | 측정 프레임 | 처리 p95 ms | 처리 최대 ms | 16.67ms 초과 프레임 | 최저 평균 콜백 Hz | 최대 콜백 간격 ms |
|---|---|---:|---:|---:|---:|---:|---:|
| cpu-tests | title_animation | 720 | 10.179 | 20.677 | 4 | 59.35 | 33.362 |
| cpu-tests | slot_preview | 720 | 11.706 | 28.445 | 3 | 59.34 | 33.360 |
| cpu-tests | stage01_firing | 720 | 10.848 | 13.782 | 0 | 59.67 | 33.067 |
| memory-tests | title_animation | 720 | 9.932 | 14.557 | 0 | 59.67 | 33.368 |
| memory-tests | slot_preview | 720 | 14.171 | 22.103 | 2 | 59.68 | 32.601 |
| memory-tests | stage01_firing | 720 | 11.601 | 24.250 | 1 | 17.91 | 7024.501 |
| gpu-tests | title_animation | 720 | 10.219 | 13.431 | 0 | 59.66 | 33.271 |
| gpu-tests | slot_preview | 720 | 11.175 | 11.504 | 0 | 59.67 | 33.343 |
| gpu-tests | stage01_firing | 720 | 10.945 | 12.855 | 0 | 59.67 | 33.356 |
| control-tests | title_animation | 720 | 9.847 | 13.364 | 0 | 59.66 | 33.356 |
| control-tests | slot_preview | 720 | 11.307 | 13.571 | 0 | 59.67 | 32.939 |
| control-tests | stage01_firing | 720 | 11.051 | 11.365 | 0 | 59.67 | 33.345 |

약 33ms의 콜백 간격은 추적기를 끈 대조 실행에도 있었다. 따라서 모든 지연을 Instruments 탓으로 단정하지 않는다. Allocations+Leaks 실행에는 최대 7.025초 콜백 간격도 있었다. 이 조건의 콜백 Hz를 일반 플레이 성능으로 해석하지 않는다. 검사는 처리시간 p95 기준을 통과했지만 콜백 최대 간격까지 무드랍을 보장하는 검사는 아니다.

## CPU 분석

처음 전체 프로세스 Time Profiler 기록에서 앱 샘플 30,362개를 얻었으나 앱 주소 상당수가 미심볼화 상태였다. 기존 바이너리로 dSYM을 생성하고 xctrace symbolicate도 시도했으나 해당 trace와 관련 dSYM을 연결하지 못했다. 실패를 보존했다. 대신 앱에 직접 연결한 Metal System Trace 안의 Time Profiler에서 심볼이 확인된 앱 샘플 29,029개를 분석했다.

- `HMGLRenderer presentToLayer:viewport:`를 포함한 샘플: 20,602 / 29,029 = 70.97%.
- `HMGameView drawLogicalFrame`: 18.74%.
- `GameSession::tick`: 0.31%.
- 최상위 실행 함수 중 OpenGL 픽셀 변환 `glgConvertTo_32<GLGConverter_ABGR8_ARGB8...>`: 12.53%.

이 비율은 앱 CPU 샘플의 호출 스택 포함 비율이다. 서로 포함되는 항목을 더하거나 전체 시스템 CPU 사용률이라고 해석하지 않는다. 일부 드라이버/JIT 주소는 여전히 미심볼화다. 현재 시뮬레이터에서는 게임 규칙 계산보다 OpenGL 화면 확대·표시 쪽 비용이 큰 것으로 관측됐다. GL_RENDERER는 Apple Software Renderer다. 이 결과만으로 아이폰 GPU 병목을 확정하지 않는다.

## 메모리 분석

Instruments GUI에서 memory.trace를 열어 Allocations 통계와 Leaks 트랙을 직접 확인했다.

- 전체 기록의 Created & Persistent: Heap + Anonymous VM 127.62 MiB. Heap 62.53 MiB, Anonymous VM 65.09 MiB.
- 누적 할당량 1.76 GiB, 살아 있는 할당 93,071개, 해제된 할당 1,619,562개. 누적 할당량은 현재 메모리 사용량이나 누수량이 아니다.
- 초기 증가 후 반복 구간의 그래프는 일시적 봉우리 뒤 비슷한 수준으로 돌아왔다. 이 짧은 구간에서 지속 상승하는 형태는 보이지 않았다.
- Leaks 검사 표시 2개가 모두 녹색이고 Leaks 결과 목록이 비어 있었다. 해당 검사에서 검출된 누수는 0건이다.

테스트는 장면마다 뷰·미디어·컨텍스트를 재생성하고 화면 첨부도 만들므로 누적 할당을 순수 사격 1프레임 비용으로 계산하지 않는다. 프로세스 시작 후 attach했으며 전체 미션 장시간 누수 검사도 아니다. VM Tracker 자동 snapshot은 꺼져 있었으므로 별도 RSS/dirty/peak 측정값을 주장하지 않는다.

## GPU 수집 결과

Metal System Trace를 실제 실행해 metal-gpu-intervals 31,212개를 수집했다. SimMetalHost 15,604개, SimRenderServer 2,602개와 WindowServer 등 호스트 이벤트가 포함됐다. HeavyMach 앱 이름으로 귀속된 Metal GPU interval은 0개였다. 앱 CAMetalDrawable present 요청, GPU counter 값, GPU performance state interval도 0개였다.

즉 시뮬레이터의 화면 합성에 관련된 GPU 이벤트는 관측됐다. 앱의 OpenGL ES1.1 렌더 시간을 별도의 GPU 시간으로 분리하거나 GPU 성능 상태를 바꾼 검증은 하지 못했다. 수집되지 않은 카운터를 0% GPU 사용률로 해석하지 않는다. Time Profiler/Metal trace의 “known input source가 없는 table 1개” 경고도 원본 로그에 남겼다.

## 판정과 증거

실제 Instruments를 사용한 CPU·메모리·GPU 추적 및 반복 실행을 완료했다. 짧은 반복에서 누수는 검출되지 않았고 프레임 처리 p95 기준은 통과했다. 간헐적인 약 33ms 콜백 간격은 남았으므로 무드랍으로 판정하지 않는다. 실제 아이폰 성능 확인은 별도다.

증거 디렉터리: `HeavyMach/build/instruments-20261024/`.

- `cpu.trace`, `memory.trace`, `gpu.trace`: 원본 Instruments 기록.
- `*-tests.xcresult`, `*-tests.log`: 실행과 판정, 프레임별 집계.
- `frame-summary.json`, `gpu-cpu-analysis.json`, `gpu-analysis.json`, `memory-ui-observations.json`: 분석 결과.
- `cpu-samples.xml`, `gpu-cpu-samples.xml`, `gpu-data.xml`, `*-toc.xml`: Instruments export.
- `manifest.json`: 환경·바이너리/소스 해시·범위.
- `run_attached.py`: attach와 반복 실행 재현 스크립트. 결과 경로가 존재하면 새 이름으로 실행해 기존 증거를 보존한다.

이전 제품 수정·원본 IPA·자원·사용자 저장은 변경하지 않았다. 새 커밋·push·실기기 설치는 하지 않았다.
