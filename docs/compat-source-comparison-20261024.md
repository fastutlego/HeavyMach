# 정상 IPA 호환판과 소스 복원판 직접 비교 — 2026-10-24

직접 비교에서 복원판의 시간 누적 방식이 원본과 다르며, 표시할 화면마다 게임을 0회 또는 2회 갱신하는 결함을 확인했다. 원본처럼 한 번씩 갱신하도록 수정했다. 이것을 아이폰에서 보고된 모든 프레임 드랍의 유일한 원인으로 단정하지 않는다.

## 자원 수명과 메뉴·탄흔 추가 수정

원본 함수와 자원 소유자를 다시 대조해 다음 차이를 수정했다. 근거와 현재 소스 SHA는 `build/resource-parity-20261024/manifest.json`에 있다.

- Texture2D 0x69b0처럼 GPU 업로드 후 CPU 이미지 픽셀을 해제한다. 크기·형식만 남기고 같은 이미지를 매 프레임 다시 해석하지 않는다.
- 공유 자원과 화면별 자원, 월드 자원을 분리한다. 원본 0x3778의 이전 화면 제거 순서를 따라 새 화면을 만든 뒤 이전 화면의 전용 텍스처를 해제한다. 지도 그림은 맵을 열 때 준비하고 원본 ClearTileResources에서 해제한다. 이름이 같아도 서로 다른 원본 관리자 소유 자원은 분리한다.
- 시작 때 원본과 같은 셔터 4개와 HUD 18개를 준비한다. 메뉴의 전용 그림과 미리보기 기체의 공유 그림도 원본 소유자로 구분한다.
- 슬롯 재생성 뒤 버튼 입력 순서, HP가 바뀌기 전 바늘의 초기 각도, 업그레이드 버튼 재생성 시 이전 눌림 상태 폐기를 수정했다.
- 원본 0x1b5c0/0x7830처럼 수명이 끝난 탄흔은 그 프레임의 그리기 목록에 넣지 않는다.

검사 결과:

- Core CTest 17/17 통과. 탄흔 제거와 월드 자원 구분은 수정 전 실패와 수정 후 통과를 보존했다.
- 관련 iOS 검사 24/24 통과 (`scoped.xcresult`). 맵→메뉴 6회 반복에서 전용 텍스처 수가 증가하지 않았고 CPU 해석 이미지 잔류 0바이트를 확인했다.
- iOS 전체 53개 중 52개 통과 (`full.xcresult`). 남은 하나는 기존 16.67ms 표시 성능 검사이며 타이틀/메뉴/사격의 세 단언이 실패했다. 합격 기준은 바꾸지 않았다.
- 일반 iPhone 대상 서명 제외 빌드와 시뮬레이터 빌드 성공 (`device.log`, `simulator.log`). 산출물은 이 증거 폴더의 `iphoneos/`, `simulator/`에 복사했다. 실기기에 추가 설치하지 않았다.

타이틀 픽셀 차이의 후속 분석에서 RGB565 한 단계 수준의 차이를 확인했다. 독립 캡처는 같은 프레임의 카메라 상태를 고정하지 않았으므로 원인을 확정하거나 임의 보정하지 않았다. 원본 UV·변환 상수·필터·POT 패딩·565 변환·최종 quad는 대조했다. 동일 상태의 중간 버퍼 직접 비교는 남아 있다.

720종 기체는 최신 native 생성이 모두 성공했다. 원본 관측 앱은 원본 실행파일·Mach·Parts를 유지하고 별도 Stage01 배치만 바꿨다. 타이틀→슬롯 메뉴 입력은 성공했으나 OK 좌표 클릭이 `noWindowsAvailable`로 실패했다. 수집한 이름별 원본 경계 표본은0개다. 원본720종과의 비교는 남아 있다. 종료 후 관측 프로세스와 임시 breakpoint 메모리는 폐기했다. 근거: `build/bounds-720-20261024/result.json`.

## 빈 슬롯 메뉴의 원본 직접 실행 비교

CUA로 원본 타이틀에서 슬롯 메뉴에 진입했다. 빈 슬롯 세 개와 가운데 선택 상태를 화면으로 확인했다. 같은 Air/iOS26.5에서 원본의 마지막 28개 1초 표시 계수 평균은 **48.670회/초**(47.99–49.01)였다. 최신 복원판의 같은 빈 슬롯 조건에서 실제 제품 NSTimer 180프레임 평균은 **50.159회/초**였다. 시간·난수 동기화나 실제 scanout 측정은 아니며 이 메뉴에서 복원판의 성능 악화는 관측되지 않았다. 원본 사격 장면의 직접 측정으로 확대하지 않는다.

`resource-parity-20261024/empty-menu-final.xcresult`에서 타이틀·기체 미리보기 메뉴·사격·빈 슬롯 메뉴의 실제 타이머 검사 1개(장면 4개)가 통과했다. 이것은 표본 수·GL 오류 검사이며 기존 16.67ms 예산 실패를 해소했다는 뜻이 아니다. 초기 화면 캡처는 in-process XCTest에서 UI 검사 권한 오류로 실패했다. 캡처를 소유한 GL 버퍼 읽기로 바꾸는 도중 타입 추론 빌드 오류도 수정했으며 최초 실패와 최종 통과를 모두 보존했다. 정상 앱 코드는 이 캡처 추가로 바뀌지 않았다.

원본 화면 `original-menu.png`, 원본 계수 `compat-menu-fps.json`, 최신 native 계수 `native-menu-metrics.json`, GL 논리 화면 `final-attachments/`를 보존했다. 원본 화면과 native 논리 버퍼는 해상도와 표시 단계가 다르므로 픽셀 일치율로 비교하지 않았다.

## 표시 성능 실패의 원인 분리 — 후속 재검사

`build/perf-failure-20261024/`에서 기존 실패를 그대로 재현하고 표시 경로의 비용을 나눴다. 실패는 크래시·GL 오류가 아닌, `testOnscreenOpenGLCadenceAudioAndResume`의 16.67ms 처리시간 단언 세 건이다. 합격 기준은 변경하지 않았다.

검증 빌드에서만 타이밍을 계측했다. 제품의 OpenGL 명령·해상도·확대 필터·타이머·음향은 바꾸지 않았다. 실제 제품 NSTimer의 장면별 180프레임 평균:

| 장면 | 화면 복사 | 최종 확대 패스(초기화 포함) | 화면 제출 | 상태 복구·프레임 제한 대기 | 전체 처리 |
|---|---:|---:|---:|---:|---:|
| 타이틀 | 0.040ms | 15.613ms | 2.138ms | 0.004ms | 19.195ms |
| 메뉴 | 0.040ms | 15.609ms | 2.092ms | 0.004ms | 20.265ms |
| 사격 | 0.040ms | 15.613ms | 2.091ms | 0.004ms | 20.365ms |

현재 실패를 지배하는 비용은 시뮬레이터의 Apple Software Renderer가 수행하는 최종 확대·제출이다. 복사·프레임 제한 대기가 20ms의 원인이라는 가설은 이 실행에서 배제된다. 이는 현재 시뮬레이터 검사 실패의 설명이며 사용자가 보고한 과거 아이폰 설치본의 원인을 확정한 것이 아니다.

같은 시뮬레이터에서 보존된 포팅판 타이틀을 다시 실행했다. 28개의 1초 집계에서 평균 49.412회/초(48.55–51.76)였다. 복원판 실제 타이머 평균 간격은 19.357ms, 약 51.66회/초였다. 둘 다 화면 제출/호출 계수이며 실제 scanout 측정이 아니다. 포팅판도 60회/초에 도달하지 못하는 현재 환경에서 복원판 60FPS 실패만으로 원본 대비 타이틀 성능 결함을 판단할 수 없다. 메뉴·사격의 원본 실행 비교는 이 결과에 포함하지 않았다.

[Apple의 OpenGL ES Simulator 설명](https://developer.apple.com/library/archive/documentation/OpenGLES/Conceptual/OpenGLESHardwarePlatformGuide_iOS/OpenGLESiniOSSimulator/OpenGLESiniOSSimulator.html)도 시뮬레이터 OpenGL 성능을 실제 기기 성능으로 대응시키지 말라고 명시한다. 환경의 60FPS 실패와 제품의 원본 대비 회귀, 실제 아이폰 성능을 별도로 보고한다. 실패를 삭제하거나 화질을 낮춰 통과시키지 않았다.

증거: `breakdown.xcresult/log`, `metrics.json`, `compat-stderr.log`, `compat-fps.json`. GL 출력 회귀는 `pixels.xcresult/log`에 별도 기록한다.

## 최신 전면 비교·수정 — 포팅판 기준

사용자의 후속 지시로 아래 초기 비교에서 유지했던 차이도 수정했다. 초기 표의 CADisplayLink·NEAREST·safeArea 유지 결정은 더 이상 현행 구현이 아니다. 전체 함수와 캠페인 상황의 동등성 검사는 남아 있다.

- 렌더: OpenGL ES 1.1, 480×320 RGBA8 renderbuffer, 호환판의 화면 복사·LINEAR·CLAMP_TO_EDGE·전체 화면 비율 맞춤·nativeScale·RGB565 drawable 요청을 적용했다. 시뮬레이터가 보고한 실제 drawable 형식은 GL_RGB8이다.
- 시간: 원본 double 상수로 프레임당 한 번 갱신한다. 호환 실행기의 NSTimer 다음 기한 계산과 present 60Hz 제한을 적용했다. native 반복 NSTimer에서 관측한 33ms 주기 대신 같은 기한 계산을 쓰는 일회용 NSTimer를 연결했다. 누적 시간을 여러 tick으로 보충하지 않는다.
- 입력·메뉴: 이벤트별 역순 hit 검사, 버튼 경계, pause 처리, float 계산, 50Hz 센서 필터와 SDL float 변환을 맞췄다. 일반 앱의 추가 사격 키를 제거했다. 메뉴·타이틀·게임이 같은 xorshift32 난수 흐름을 쓴다.
- 이미지·음향: 원본 이미지 695개를 같은 stb 코드로 해석한다. OpenAL Soft 1.22.2와 31개 원본 음향을 재사용하며 position→stop→play 순서를 적용했다. 원본에 없는 수동 오디오 중단·재개 정책을 제거했다.
- 저장: 탄약·피격·EXP 변경의 메모리 통지와 실제 파일 저장을 분리했다. 원본 호출 네 곳(슬롯 선택 0x3b20, 다음 스테이지 0xb8f0, 게임오버 0xbf74, 메뉴 복귀 0x17c4c)에서만 기록한다. 이벤트 전달 전 저장 시점의 슬롯을 복사한다. 슬롯 HP·무기 이름의 남은 바이트를 임의로 덮어쓰던 차이도 제거했다.
- 게임 상태: 기본 무기 탄약 갱신, ARM32 탄약 연산, 무기 삭제 복사 길이, 카메라·흔들림 유지, 시작 시 중복 draw 수집을 수정했다.

현재 증거는 `build/parity-20261024/`와 `build/compat-core-parity-20261024/`에 있다. `save-audio2.xcresult`에서 45개 검사 중 44개 통과, 표시 성능 검사 1개의 세 장면 기준 실패를 보존했다. 실제 임시 저장 파일 검사에서 180프레임 사격 중 추가 쓰기 0회, 메뉴 복귀 쓰기 1회, background 추가 쓰기 0회를 확인했다. 이전 성능 검사는 저장을 막는 대역을 사용했으므로 이 결과와 구분한다.

실제 게임 타이머 180프레임의 평균 호출 간격은 타이틀 19.11ms, 메뉴 20.52ms, 사격 20.29ms였다. 시뮬레이터는 Apple Software Renderer다. 세 장면 모두16.67ms 처리 기준을 넘었다.

일반 앱 시뮬레이터·iPhone 대상 서명 제외 빌드가 성공했다(`normal-save-final.log`, `device-save-final.log`). 사용자 앱 대신 별도 bundle ID에 설치했다. 포팅판과 같은 크기의 타이틀 캡처 비교에서 전체 3,447,360픽셀 중 3,431,729픽셀이 일치했고 RGB 평균 차이 0.00937, 최대 채널 차이 9였다(`title-pixels-final.json`). 완전한 픽셀 일치는 아니다. 이후 발견한 추가 수정의 재검사는 별도 결과로 기록한다.

남은 범위: 모든 원본 함수·모든 캠페인 상황의 동등성, 두 앱의 같은 입력으로 메뉴·사격 직접 비교, 실제 아이폰 재검증. UI 도구가 DeviceHub 창 조작에 noWindowsAvailable을 반환해 메뉴 직접 비교를 끝내지 못했다. 원본 IPA·호환 실행기·사용자 저장은 보존했다.

## 후속 발견의 최종 수정과 재검사

- 기체의 부품 순서는 원본처럼 부품 변경 때만 다시 구성한다. 매 프레임 임시 벡터 할당·정렬을 제거했다.
- 스테이지 전환 후 현재 플레이어를 조회해 카메라를 갱신한다. 삭제한 플레이어 추종과 새 플레이어 추종의 한 프레임 지연을 회귀 검사로 재현·수정했다.
- 슬롯 날짜·레벨·부품·EXP 문자열은 슬롯 생성 때 캐시한다. 선택 모델의 초기 이동 값과 EXP 게이지/모델 표시 순서를 맞췄다. 모델 회전은 원본 setPartsToMyMach 0x1be84의 세 번 감산(-0.03)을 유지한다.
- 숨겨진 HUD도 미리보기 플레이어 생성 때 원본 changeWeapon을 받는다. 선택 슬롯과 기존 무기 항목의 주소를 따로 유지하며 최초 nil 상태와 다른 슬롯 항목을 구분한다. 파일 쓰기는 추가하지 않았다.
- layoutSubviews·화면 drawRect의 추가 게임 draw를 제거했다. 최종 확대에서 DITHER를 끄고 NDC quad를 쓴다. 홀수 여백도 원본의 픽셀 정수 계산으로 배치하며 터치 영역을 맞춘다.

최종 Core CTest **17/17 통과** (`core-final-test.log`). iOS 전체 검사 `complete-pass.xcresult`는 **46개 중 45개 통과**, 기존 표시 성능 검사 하나의 세 장면이 실패했다. 파일명은 성공 판정을 뜻하지 않는다. 이후 숨겨진 HUD 연결·메뉴 수정의 영향 검사 **13/13 통과** (`hidden-hud.xcresult`), 실제 사격 저장 검사도 다시 통과했다. 성능 실패 기준을 삭제하거나 완화하지 않았다.

DITHER·NDC 수정 후 타이틀 재캡처의 픽셀 차이는 앞선 값과 같았다(`title-pixels-dither.json`). 따라서 DITHER가 남은 픽셀 차이의 원인이라는 추정은 입증되지 않았다. 전체 게임의 완전한 일치를 이 캡처로 주장하지 않는다. 마지막 빌드는 `normal-final-current.log`, `device-final-current.log`에 기록한다. 소스 SHA와 수정 목록은 `manifest.json`에 있다.

## 비교 대상과 보존

- 호환판: `iOS Proj/HeavyMachRecovery/compat-source/fresh-port/20-touchHLE-src/touchHLE`. 보존된 IPA의 ARM 명령을 실행하고 GLES/OpenAL 호출을 호스트로 연결한다.
- 복원판: `HeavyMach/Sources/iOS/HMGameView.mm`, `HMGLRenderer.mm`, `HMMedia.mm`, `HMOpenALAudio.mm` 및 Core.
- IPA의 기존 분석: `Analysis/functions/0x20b8.pseudo.c` (시작/타이머), `0x21e0.pseudo.c` (renderScene), `0x2920.pseudo.c` (swapBuffers). 디컴파일 기록을 원본 개발 소스라고 부르지 않는다.
- 호환판 런타임 비교에는 기존 Simulator .app을 별도 `HeavyMachCompare.app`으로 복사했다. 실행 파일은 SHA256 일치. Bundle ID를 `com.gelb2.heavymach.compatcomparison`으로 분리하고 `--print-fps`만 추가했다. 원본 IPA·실행기 소스·기존 앱·저장은 변경하지 않았다.

## 확인한 차이

| 경로 | 정상 IPA 호환판 | 복원판에서 확인한 차이 | 판정/처리 |
|---|---|---|---|
| 게임 갱신 | 원본 renderScene 한 번에 g_time 증가·GameMain tick·drawVisual 각1회 | CADisplayLink timestamp 누적량으로 tick 0/1/2회 이상 결정 | 실제 앱 검사로 불일치 재현. 이번에 frame당1회로 수정 |
| 시간 상수 | 원본 double 비트 `0x3f91111120000000` = 0.01666666753590107 | `1.0/60.0` = 0.016666666666666666 | 이번에 원본 상수 복원. 큰 문제는 상수 미세 차이보다 누적량의 경계 판정 |
| 호출 주체 | 원본 NSTimer + 호환 실행기의 run loop + present 프레임 제한(기본60) | CADisplayLink 60/60/60 요청 | 다름을 유지. 타이머 자체가 단독 원인이라고 입증되지 않았으므로 무작정 교체하지 않음 |
| 게임 장면 렌더 | 원본 GLES1 명령을 호스트 GLES로 전달 | 초기 UIKit/CPU 렌더였으나 이전 수정에서 GLES1로 교체 | 현재 둘 다 GLES1. 원본 CGL 기준8개 검사 유지 |
| 표시 버퍼 | 원본 renderbuffer → 재사용 텍스처로 CopyTexSubImage2D → SDL 표시 buffer | 512×512 POT 텍스처의480×320 영역에 직접 렌더 → CAEAGLLayer 표시 buffer | 구조 차이 존재. 복원판은 매프레임 복사단계가 하나 적음. 복사가 많아서 느리다는 가설은 현재 코드와 맞지 않음 |
| 화면 확대 | LINEAR, 재사용 NPOT 화면 텍스처 | NEAREST, POT 화면 텍스처 | 필터 차이 존재. 이번에 시각 정책을 임의 변경하지 않음 |
| 해상도/배치 | iOS 기본 HiDPI, nativeScale; 전체 화면에 비율 맞춤 | UIScreen.scale; safeArea에 비율 맞춤 | 둘 다 기본 고해상도. “호환판은 무조건 저해상도”라는 추정은 틀림. 실제 캡처의 표시 영역 크기도 다름 |
| 음향 | 원본 OpenAL 호출 + OpenAL Soft1.22.2 | 초기 AVAudioPlayer 호출이었으나 이전 수정에서 같은 OpenAL Soft 소스로 복원 | 이전 음향 지연 증거 유지. 현재31개 정적 buffer/source와 재사용 검사를 통과 |
| 실행 방식 | ARM 인터프리터를 거쳐 원본 코드 실행 | 복원된 ARM64 C++/ObjC++ 직접 실행 | 실행 방식 차이는 있지만 새 버전이 더 느려야 할 이유로 단정할 수 없음 |

호환판 소스 근거: `src/frameworks/opengles/eagl.rs`의 presentRenderbuffer/limit_framerate/present_renderbuffer, `src/gles/present.rs`, `src/window.rs`의 MOLE_HIDPI 기본값, `vendor/SDL/src/video/uikit/SDL_uikitopengles.m`의 nativeScale. 호환판 주석에 남아 있는 다른 게임의 성능 수치를 HeavyMach 측정값으로 사용하지 않았다.

## 갱신 오류의 변경 전/후 검사

XCTest에서 실제 HMGameView.frame을 호출했다. UI 입력이나 저장 쓰기는 하지 않는다. 각 조건600회. 비교 기준은 원본 renderScene의 한 번 갱신이다.

| 입력 | 수정 전 갱신 횟수 불일치 | 수정 후 |
|---|---:|---:|
| 일정한60Hz timestamp | 2/600 | 0/600 |
| timestamp ±0.3ms 흔들림 | 599/600 | 0/600 |
| 매20회마다 한 번의 화면 주기 지연 | 31/600 | 0/600 |

수정 전 `tick-before.xcresult/log`의 실패를 보존했다. `HMGameView.advanceOriginalFrame`으로 한 번의 게임 갱신을 분리하고 timestamp 누적/보충 반복을 제거했다. 비활성 상태에서는 진행하지 않고 재개 후에도 배경 시간을 몰아서 보충하지 않는다. 기존 passive lifecycle 검사의 강제 accumulator 설정은 필요 없어 제거했으며 기능 단언은 유지했다.

독립 검토에서 필수 수정 결함은 발견되지 않았다. 단, CADisplayLink의60Hz 요청은 보장값이 아니다. OS가 지속적으로30Hz만 호출하면 이 방식의 게임 진행도30tick/s가 된다. 그 조건에서 원본 NSTimer와 동등한지는 아직 미검증이다.

## 실제 실행 검증

- 전체 iOS XCTest **35/35 통과** (`after.xcresult`, `after.log`).
- 일반 iPhone용 Debug 서명 제외 빌드 성공 (`device-build.log`). 설치·실기기 실행은 하지 않았다.
- 수정 후 표시·음향 포함 각180프레임: 처리 p95 타이틀9.937ms, 메뉴11.392ms, 사격10.742ms. 측정한 처리시간의16.67ms 초과는0개.
- 사격 화면 호출 최대 간격은31.928ms였다. 따라서 이번 갱신 오류 수정과 별개로 간헐적 표시 호출 지연은 남아 있다.

두 앱을 같은 iPhone Air/iOS26.5 Simulator에서 각각 타이틀 대기 화면으로 실행하고, 각15초 Time Profiler와 화면 캡처를 남겼다. 복원판 검증 버튼·safeArea·확대 필터 차이가 있으므로 두 화면의 픽셀 부하가 완전히 같다고 주장하지 않는다. 사격·메뉴를 포함한 양쪽 동시 기준 FPS 비교는 아직 수행하지 않았다.

호환판의 자체 present 호출 계수는56개1초 집계에서 평균50.15회/초(47.96–53.84)였다. 집계에는 시작·추적 구간이 함께 들어 있다. 호환판은 present 호출, 복원판은 테스트 콜백 호출을 측정했다.

## 증거

`HeavyMach/build/compat-comparison-20261024/`:

- `HMGameView-before.mm`, `HMGameView.diff`, `HMFramePerformanceTests-before.mm`, `HMFramePerformanceTests.diff`.
- `tick-before.xcresult/log`, `after.xcresult/log`, `after-metrics.json`, `device-build.log`.
- `compat-title.trace`, `restored-title.trace`, 각 samples.xml/analysis.json 및 PNG 캡처.
- `compat-fps.json`, `compat-stderr.log`, `manifest.json`.

범위: 갱신 타이밍 결함 수정과 직접 비교. 원본 호환판/게임 자원/사용자 저장은 보존했다. 새 커밋·push는 하지 않았다.
