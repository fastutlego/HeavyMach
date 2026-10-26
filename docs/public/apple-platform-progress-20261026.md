# Apple 공용 모듈·키보드 검사

2026년 10월 26일 `feat/apple-workspace-modules`의 검사 기록이다. Mac 일반·치트 앱을 개발 인증서로 서명한 뒤 두 앱 모두 메모리 누수0건을 확인했다. 잠금을 푼 Mac에서 기능·전체 화면·성능 검사33개가 통과했다. 여섯 장면의 프레임 처리 p95는10.86~14.29ms, 평균 호출 간격은16.67~16.69ms였다. 수정 전 실패와 수정 내용은 문서 끝의 후속 검사에 기록했다.

공용 모듈과 키보드 입력은 구현했다. iPhone/iPad Simulator 기능 검사는 통과했지만 같은 환경의 프레임 검사는 변경 전 main과 작업본 모두 실패했다. 2026-10-27 실제 iPhone/iPad 검사 결과는 문서 끝에 추가했다. Mac 전체 화면의 창·뷰·그리기 영역 검사는 통과했다. 2026-10-27 macOS 자체 캡처에서 게임이 화면 끝까지 표시됨을 확인했다. CUA 창 캡처의 흰 여백은 캡처 경로의 현상으로 분류했다. main·v0.9에는 이번 변경을 반영하지 않았다.

## 프로젝트 구성

`HeavyMach.xcworkspace` 아래에 Modules, iOS, macOS 프로젝트를 뒀다. iPhone과 iPad는 같은 iOS target을 사용한다. Mac은 AppKit 앱이다. Catalyst는 사용하지 않는다.

공용 C++ 코어, Apple 실행 코드, 이미지·소리·맵 자원을 모듈과 번들로 묶었다. iOS는 기존 OpenGL ES 1.1, Mac은 desktop OpenGL 컨텍스트로 같은 스프라이트·블렌드 연산을 실행한다. Mac OpenGL은 Apple이 deprecated한 API다. 이번 작업은 기존 렌더 연산을 재사용하며 Metal 이식은 별도 과제로 남긴다.

게임도 framework나 SPM으로 코드·자원을 공유할 수 있다. 이번에는 요청한 여러 프로젝트 구성과 기존 Objective-C++ 코드를 연결하기 위해 정적 framework를 사용했다. [Apple의 프로젝트 간 의존 관계](https://developer.apple.com/documentation/xcode/managing-multiple-projects-and-their-dependencies), [SPM 자원 번들](https://developer.apple.com/documentation/xcode/bundling-resources-with-a-swift-package).

## 키보드 조작

| 기능 | 기본 키 |
|---|---|
| 전진·후진 | D·A |
| 조준 위치 이동 | 방향키 |
| 미세 이동·조준 | Shift와 함께 누르기 |
| 점프 | Space |
| 사격 | F |
| 이전·다음 무기 | Q·E |
| 메뉴 항목 이동·선택 | Tab·Enter |
| 메뉴·일시정지·뒤로 | Esc |
| 키 설정 | F2 |

키 설정에서 기능을 선택하고 새 키를 누르면 배치를 바꾼다. 중복 키는 서로 교환하고 기본 배치 복원을 제공한다. iPad에서는 키보드와 터치를 함께 사용할 수 있다. 이동 키를 놓으면 키보드가 만든 기울기 값을 지우고 실제 틸트 입력으로 돌아간다. 앱 비활성화·키보드 분리 때 누른 키와 사격 상태를 해제한다.

키보드 접근성의 재지정·단독 조작 기준을 조사하고 `game-keyboard-input` 스킬에 검사 순서를 기록했다. [Xbox XAG 107](https://learn.microsoft.com/en-us/gaming/accessibility/xbox-accessibility-guidelines/107), [Game Accessibility Guidelines](https://gameaccessibilityguidelines.com/allow-controls-to-be-remapped-reconfigured/), [Apple 키보드 지침](https://developer.apple.com/design/human-interface-guidelines/keyboards).

## 실행한 검사

| 대상 | 결과 | 검사 범위 |
|---|---|---|
| C++ 코어 | 18/18 통과 | 원래 게임 규칙과 키 입력 |
| native Mac | 33/33 통과 · 건너뜀0 | 이동·점프·조준·사격·20종 무기·메뉴·이미지·GL·음향 |
| Mac 치트 무기 선택 검사(위33개에 포함) | 1/1 통과 | 등급0~6, 전체 무기 지급 켜기/끄기, 무기20종의 280조합 |
| iPad Simulator | 22/22 통과 | 키보드·터치 병용, 화면비, 위쪽 터치, 양방향 틸트, GL·이미지 |
| iPhone Simulator | 53/53 통과 | 키보드·20종 무기·캔버스·GL·이미지·음향 기능 |
| iPhone SDK | 일반·치트 빌드 성공 | arm64 앱 컴파일·링크 |
| Mac SDK | 일반·치트 빌드 성공 | native arm64 앱 컴파일·링크 |
| 독립 export | Mac·iOS Simulator 치트 빌드 성공 | IPA·기존 Git 폴더 읽기를 차단하고 새 OpenAL 캐시에서 빌드 |

원본 이미지695개의 픽셀 해시를 비교했다. 미션3의 5번째 서브미션 항공기 보스는 논리 높이320과360에서 같은 난수·입력으로 360프레임 동안 위치를 대조했다. 화면 높이를 늘려도 이 보스의 이동 결과는 같았다.

Mac 실제 창에서 치트 게임을 시작하고 이동·점프·조준·사격·무기 전환 키를 입력했다. 전체 화면 캡처의 상단·오른쪽 흰 여백을 처음에는 게임 문제로 분류했다. 이후 창·뷰·내용 영역이 모두1920×1080이고 원점이0,0임을 확인했다. 게임과 OpenGL 없이 파란 배경과 빨간 테두리만 그리는 AppKit 비교 앱에서도 같은 여백을 재현했다.

비교 앱의 네 테두리는 모두 남아 있었다. 3840×2160 캡처 안에서 내용이 높이2050에 맞춰 너비 약3645로 축소되고 상단110·오른쪽 약195픽셀에 흰색이 남았다. 게임 장면의 캔버스 계산과 별도로 발생하는 창 캡처 현상으로 분리했다. 게임 크기에 역보정을 넣지 않았다. 실제 디스플레이와 캡처의 최종 대조는 남아 있다. 비교 소스는 `build/window-probe.m`, 창 크기 기록은 `build/mac-fullscreen-geometry.log`다. 진단용 로그와 임시 바탕색 변경은 제품 코드에서 제거했다.

현재 물리 iPhone·iPad는 개발 도구에서 unavailable로 표시됐다. 위 iPhone·iPad 결과는 Simulator 검사이며 실제 기기의 블루투스·유선 키보드 검사와 구분한다.

## 키 입력 중 저장 검사

이동·점프·조준·사격·일시정지/재개와 무기20종 연속 사격에 대해 저장 함수의 호출 횟수를 검사했다. 두 검사 모두 통과했고 전투 입력 중 게임 저장과 키 배치 저장 호출은0회였다. 코드에서도 일반 피격·발사·탄환 충돌은 저장 요청을 만들지 않는다.

키 배치는 새 키를 지정하거나 기본값으로 복원할 때 저장한다. 게임 진행은 진행 저장 이벤트·게임 오버·명시적인 저장 종료 등 지정된 경로에서 저장한다. 검사 로그는 `build/mac-no-input-save-tests.log`다.

## Simulator 성능 검사

16.67ms 기준은 통과하지 못했다. 같은 iPhone Simulator의 Apple Software Renderer에서 변경 전 main과 비교했다.

| 장면 | 변경 전 main p95 | 작업본 p95 |
|---|---:|---:|
| 타이틀 | 27.731ms | 27.688ms |
| 슬롯 미리보기 | 29.556ms | 29.672ms |
| 스테이지1 사격 | 28.471ms | 28.647ms |

양쪽 모두 화면 확대용 그리기에 약22.3ms가 걸렸다. 이 비교에서 모듈화 뒤 새로 늘어난 큰 지연은 확인하지 못했다. 물리 iPhone의 프레임 시간은 이번 검사로 측정하지 않았다.

사용자가 여러 Simulator의 동시 실행 영향을 지적한 뒤, 21:37에 실행 상태를 확인했다. iPhone과 iPad 두 대가 켜져 있었다. iPad를 종료하고 iPhone 한 대만 남긴 상태에서 병렬 테스트를 끄고 같은 성능 검사를 다시 실행했다. 각 장면은 준비30프레임 뒤180프레임을 측정했다.

| 장면 | 앞선 작업본 p95 | iPhone 한 대 p95 |
|---|---:|---:|
| 타이틀 | 27.688ms | 27.528ms |
| 슬롯 미리보기 | 29.672ms | 29.540ms |
| 스테이지1 사격 | 28.647ms | 28.646ms |

한 대에서도 세 장면 모두16.67ms 기준을 넘었다. 두 대 동시 실행이 지연의 주원인이라는 결과는 나오지 않았다. 검사 중 렌더러는 Apple Software Renderer였고 마지막 프레임의 화면 확대 그리기는22.24~22.50ms였다. 최초 보고 전에 동시 실행 조건을 통제하지 않은 것은 검증 누락이다. Mac의 다른 앱 부하까지 제거한 실험은 아니며, 이 결과를 실제 iPhone 성능으로 해석하지 않는다.

원시 기록: `build/iphone-single-simulator-environment.log`, `build/iphone-single-simulator-perf.log`. 테스트 코드는 변경하지 않았다. 같은 시점에 물리 iPhone·iPad는 `devicectl`에서 unavailable로 확인됐다.


## Mac·iPad 성능과 Instruments 결과

아래는 Mac 서명·표시 경로 수정 전의 측정 기록이다. 최종 Mac 결과는 문서 끝의 후속 검사에 있다.

시뮬레이터는 한 대씩 실행했다. Mac 검사에서는 시뮬레이터를 종료했다. 장면별 준비30프레임 뒤180프레임을 측정했다. 아래는 추적기를 붙이지 않은 프레임 처리시간 p95다. Mac은 실제 앱의 NSTimer를 사용했고 iPad는 기존 CADisplayLink 검사 경로를 사용했다. 서로의 타이머 간격을 같은 측정으로 취급하지 않는다.

| 장면 | Mac 창1200×800pt | Mac 화면 크기1920×1080pt | iPad 작업본 | iPad 변경 전 main |
|---|---:|---:|---:|---:|
| 타이틀 | 5.206ms | 6.584ms | 44.407ms | 40.469ms |
| 슬롯 메뉴 | 10.308ms | 3.470ms | 48.133ms | 40.580ms |
| 사격 | 5.212ms | 7.393ms | 44.311ms | 39.972ms |

Mac은 2배 backing scale, M4 Pro의 `OpenGL 2.1 Metal` 드라이버를 사용했다. 두 크기의 세 장면 모두16.67ms 처리 예산을 통과했다. 사격 장면은 실제 탄환 최대8개와 발사음23회를 확인했다. 저장 호출은0회였다. 표의 ‘화면 크기’는 화면을 덮는 borderless 검사 창이며 macOS 전체 화면 전환 애니메이션의 시각 검사를 대신하지 않는다.

Mac Time Profiler는 CPU 표본7,459개를 수집했다. 실제 `HMGameView frame:`을 포함한4,033개 중 표시 경로를 포함한 표본은2,102개다. 호출은 `NSOpenGLContext flushBuffer`, Metal 드라이버 제출, 스프라이트 그리기로 연결됐다. 이 CPU 표본을 GPU 실행시간이라고 쓰지 않는다.

iPad Simulator는 Apple Software Renderer다. 변경 전 main(`0a80922d`)도16.67ms 기준을 넘었다. 작업본의 논리 높이는320→360으로 늘었고, 이전의 세로 여백까지 표시한다. 마지막 측정 프레임의 확대 그리기는 변경 전33.29~33.48ms, 작업본37.43~37.74ms였다. 추가 면적 비율12.5%와 비슷한 증가다. iPad CPU 추적에서는 게임 프레임 표본26,147개 중23,880개(91.33%)가 최종 표시 경로를 포함했다. 소프트웨어 삼각형 채우기 함수 `gldRenderFillTriangles`가22,964개였다. 이는 Simulator의 픽셀 처리 병목이며 실제 iPad GPU 시간으로 환산하지 않는다. 화면을 다시 줄이거나 필터를 바꿔 통과시키지 않았다.

Allocations와 Leaks 기록을 Mac·iPad에서 각각 수집했다. 별도의 `leaks` 명령으로 iPad는 누수0건, 그 시점의 physical footprint493.2MB를 확인했다. Mac은417개·20,016바이트가 검출됐다. 모두 `com.apple.linkd.autoShortcut`의 `LNDaemonApplicationInterface` 연결 세 개에 연결된 참조 순환이었다. Mac footprint는555.1MB였다. 이 수치는 XCTest·도구·화면별 뷰 재생성을 포함한 짧은 검사 과정의 값이며 일반 앱의 장시간 메모리 상한이 아니다.

Mac의 첫 Allocations+Leaks 기록은 저장됐다. 추가 heap 수집 실행에서는 종료 시 Instruments Leaks가 메모리 그래프 생성 오류를 냈고, 별도 `leaks` 출력은 보존했다. 게임 없는 AppKit 비교 앱의 Instruments 연결은 OS 권한 오류(-60007)로 실패했다. 당시에는 Mac 누수 원인과 수정 효과를 확인하지 못했다.

첫 iPad 전체 프로세스 Time Profiler는 Simulator 라이브러리 중첩 오류로 결과 저장이 끝나지 않았다. 해당 실패를 보존하고, Simulator UDID와 게임 PID를 지정한 연결로 재실행해 분석 가능한 기록을 저장했다. 이후에는 `tools/profile_apple.py`를 사용한다.

Mac은 후속 확인 당시 잠금 상태였다. CUA는 `cgWindowNotFound`를 반환했고 버전·서명·인증 오류는 없었다. 화면 확인을 위해 보안 설정을 바꾸지 않았다. 자동 검사와 Instruments 결과는 유효하지만 최종 화면 캡처·기존 흰 여백 대조는 완료하지 않았다.

[프레임·CPU 집계 JSON](../evidence/apple-platform-performance-20261026.json)에 측정값과 원시 로그 해시를 보존했다. 로컬 원시 기록은 `build/apple-performance-20261026/`의 `mac-cpu`, `mac-memory`, `mac-memory-with-heap`, `ipad-cpu-device`, `ipad-memory`다. 시작 실패나 기록 오류가 난 trace를 성공 표본에 합치지 않았다.

재실행은 대상 앱을 먼저 빌드하고 다른 Simulator를 종료한 뒤 수행한다.

```sh
python3 tools/profile_apple.py --platform mac --template 'Time Profiler' --output build/mac-profile-new
python3 tools/profile_apple.py --platform ipad --template Allocations --output build/ipad-memory-new
```

## Mac 객체 해제·일반 앱 메모리 후속 검사

실제 게임 뷰로 타이틀·슬롯·사격을 두 창 크기에서 실행한 뒤, 뷰와 미디어·GL 렌더러·OpenAL 음향 객체의 해제를 검사했다. 여섯 조건의 24개 해제 확인을 모두 통과했다. 사격 조건에서는 실제 탄환과 발사음을 확인했고 게임 저장 호출은0회였다. 이 검사는 게임 객체의 참조가 남는 문제를 검사하며 모든 C/C++ 메모리 누수를 배제하지는 않는다.

같은 실행의 프레임 처리 p95는 창 크기1200×800에서5.271/8.064/10.078ms, 화면 크기1920×1080에서8.342/10.909/5.274ms였다. 순서는 타이틀·슬롯·사격이다. 여섯 조건 모두16.67ms 처리 기준을 통과했다. 프레임 호출 간격과 처리시간은 JSON에 따로 기록했다.

XCTest 없이 일반 Mac 게임 앱도 실행했다. 이 측정용 Debug 빌드에만 `com.apple.security.get-task-allow`를 지정했다. 두 차례 `leaks` 결과는416개·19,968바이트와417개·20,016바이트였고 physical footprint는 모두313.3MB였다. 앞선 XCTest 실행과 같은 자동 단축어 연결 세 개의 참조 순환이 검출됐다. 이 시점에는 게임 객체 해제 검사만 통과했고 연결의 발생 원인은 확정하지 못했다.

일반 앱의 Allocations 기록은35.748초를 담았고 목차 내보내기도 성공했다. 기록 종료 사유는 시간 제한이며 대상 프로세스 종료는 SIGKILL로 남았다. `xctrace` 종료 코드는54이므로 정상 종료 코드0으로 기록하지 않는다. 게임 없는 비교 앱의 추적 실패와 별개로 실제 게임 앱의 메모리 자료를 확보했다.

macOS 전체 화면 진입·복귀와 그리기 영역을 확인하는 검사도 추가했다. 처음 실행은 잠긴 데스크톱에서 전체 화면 진입 알림을 받지 못해 실패했다. 잠금 상태를 명시적으로 확인하도록 보완한 재실행에서는 해당 검사1개를 건너뛰고, 객체 해제·프레임 검사1개를 통과했다. 전체 화면 검사를 통과로 세지 않는다. 원시 기록은 `mac-lifetime-fullscreen.log`, `mac-lifetime-final.log`, `mac-app-direct/`이며 모두 `build/apple-performance-20261026/` 아래에 있다.

## 추가 요청: 자동조준

[자동조준 개발 계획](../auto-aim-plan-20261026.md)을 작성했다. 대상 선택·유지·교체, 실제 탄도 계산, 명중 보정 후보, 20종 무기 검사, 버튼 제작과 세 플랫폼 적용 순서를 담았다. 자동조준과 HUD 편집은 아직 구현하지 않았다.

- 플레이어 앞의 가장 가까운 적을 선택한다. 적이 없으면 표준 포신 각도를 사용한다.
- 무기 특성·포물선·플레이어 이동과 점프 높이를 계산해 포신 각도를 조절한다. 요구 목표는 발사 버튼을 누른 시점의 명중이다. 적의 후속 이동·비행시간·차폐·사거리·산탄·유도 동작을 조사해 성립 조건을 정한다.
- 별도 버튼으로 다른 적을 선택한다. 대상이 죽거나 플레이어 뒤로 이동하면 다른 앞쪽 적을 선택한다.
- 대상이 화면 밖으로 나갔다는 이유만으로 선택을 해제하지 않는다.
- 버튼은 기존 게임의 질감·화풍·색조에 맞춘다.

## 추가 요청: HUD 배치 편집

게임 시작 직전에 HUD 위치를 편집하는 옵션 화면을 제공한다. 실제 게임 캡처 위에 HUD와 버튼 표시 영역을 보여 주고 드래그 앤 드롭으로 옮긴다. 플레이 중 어떤 위치에 보일지 미리 확인하는 화면이다.

HUD 에셋은 이동할 요소별로 나눈다. 원본 이미지에서 필요한 영역만 참조하는 방식과 파생 이미지를 분할하는 방식을 비교한다. 원본 픽셀·화풍·레이어 순서를 유지한다.

확정한 배치는 실제 게임에 적용한다. 그리는 위치와 터치 판정에 같은 배치 데이터를 사용한다. 옛 위치에 보이지 않는 버튼 판정이 남아서는 안 된다. 이동·점프·사격·무기 전환을 방해하는 배치, 버튼 겹침, 화면 밖 배치를 검사한다.

‘기본 배치로 복원’을 제공한다. 기본값은 현재 초기 버전의 HUD 배치다. 편집 취소는 기존 배치를 유지하고, 확정할 때만 저장한다. 드래그 매 프레임이나 전투 중 사격 때 파일을 저장하지 않는다.

폰·패드의 두 가로 방향과 터치, Mac 마우스·트랙패드, 키보드 병용, 창 크기 변경을 검사한다. 자동조준 버튼을 추가할 때도 배치 편집을 적용한다. 현재는 투두에 요구를 기록한 단계다.

## 검사 기록 위치

저장소의 `build/` 아래에 실행 로그가 있다.

- `apple-core-final-tests.log`
- `mac-final-tests5.log`, `mac-panel-selection-tests.log`, `mac-no-input-save-tests.log`
- `ipad-final-tests.log`, `iphone-final-functional.log`, `iphone-final-keyboard-save2.log`
- `iphone-main-baseline-perf.log`, `iphone-final-input-perf.log`
- `iphone-normal-build.log`, `iphone-cheat-build.log`, `mac-normal-build.log`, `mac-panel-build.log`

독립 export의 경로는 `/private/tmp/heavymach-apple-clean.0bWLXm`이며 `fresh-mac.log`와 `fresh-ios.log`에 빌드 결과를 보존했다. 이 export 뒤에 Mac 치트 창의 포커스·무기 선택을 수정했고 추가 무기 선택 검사를 실행했다.

## Mac 누수 원인·서명 수정 후속

LLDB로 일반 Mac 앱의 연결 생성 지점에 중단점을 걸었다. 시작점은 `Sources/Mac/Application.mm:21`의 `NSApplication.sharedApplication`이다. AppKit의 `NSApplication init`이 `_registerApplicationWithUIIntelligence`를 호출하고, AppIntents의 `LNProcessInstanceRegistryClient`가 현재 앱 프로세스를 등록하려고 `com.apple.linkd.autoShortcut` 연결을 만든다. 게임 뷰·렌더러를 만들기 전 실행되는 경로다.

App Intents는 앱의 기능·콘텐츠를 Siri·Spotlight·단축어 등 시스템 기능과 연결하는 프레임워크다. XPC는 프로세스 사이의 통신 수단이다. 이번 연결은 AppKit 초기화의 시스템 등록 경로이며 게임의 사격·이동을 처리하지 않는다. [Apple App Intents 설명](https://developer.apple.com/documentation/appintents), [Apple XPC 설명](https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPSystemStartup/Chapters/CreatingXPCServices.html).

서비스 쪽 `linkd` 로그에서 `Unable to get teamId from com.gelb2.heavymach.mac.restored`와 `Rejecting invalid client due to requiresValidatedBundle`을 확인했다. Mac 프로젝트의 `CODE_SIGN_IDENTITY: '-'`가 만든 ad-hoc 서명에는 개발 팀 정보가 없었다. 서비스가 앱 등록을 거절하고 AppIntents가 재시도했다. 연결 생성7회·활성화4회를 추적했으며, 메모리 기록에는 해당 서비스 연결3개의 참조 순환이 남았다. LaunchServices로 일반 앱을 직접 실행해도417개·20,016바이트가 검출됐다. 디버거나 XCTest 실행에만 한정된 현상이 아니었다.

Mac 프로젝트를 기존 iOS와 같은 개발 팀의 `Apple Development` 자동 서명으로 수정했다. 일반·치트·테스트 target에 적용한다. 이 설정으로 빌드를 시도했으나 잠긴 키체인에서 `errSecInternalComponent`로 서명에 실패했다. 키체인 조회도 `User interaction is not allowed`를 반환했다. 이 시점에는 서명 설정만 수정했고 정상 서명본의 누수 검사를 실행하지 못했다. 이후 잠금을 해제해 실행한 결과는 다음 절에 기록했다.

그동안 실제 Mac 치트 앱의 기능32개를 다시 통과했다. 잠긴 데스크톱의 전체 화면 전환1개는 건너뛰었다. 타이틀·슬롯·사격을 두 창 크기에서 측정한 프레임 처리 p95는4.682~6.393ms였다. 각 사격 조건에서 최대 탄환8개와 게임 저장 호출0회를 확인했다. 객체 해제24개 확인도 통과했다. 이 실행은 기능·성능 검사용 ad-hoc 서명을 명시했으므로 서명 수정 후 누수 검사로 세지 않는다.

원시 기록은 `build/apple-performance-20261026/mac-xpc-origin/`, `mac-launchservices-memory/`, `mac-functional-recheck.log`에 있다. 다른 프로세스가 포함된 시스템 로그 원문은 사이트에 올리지 않고 해당 오류 문구·호출 경로·파일 해시만 [집계 JSON](../evidence/apple-platform-performance-20261026.json)에 기록했다.


## 잠금 해제 후 Mac 누수·표시 대기 수정 결과

정식 앱과 치트 앱을 Xcode에서 Apple Development 인증서로 빌드했다. 서명의 팀 ID와 `codesign --verify --deep --strict` 성공을 확인했다. 정식 앱은 최종 수정본에서 누수0건·0바이트였고 physical footprint는289.2MB였다. 치트 앱은 키보드로 이동·조준·무기 전환·사격한 뒤 누수0건·0바이트, footprint321.9MB였다. Instruments Allocations+Leaks의35초 기록도 종료 코드0으로 저장했다. 장시간 메모리 증가 검사는 이 결과에 포함하지 않는다.

이 결과로 기존417개·20,016바이트 누수의 수정 효과를 확인했다. 원인은 명시적 ad-hoc 서명으로 팀 ID가 빠진 앱의 시스템 등록 거절과 재시도였다. Apple Intelligence 설정은 변경하지 않았다. 게임 코드에 단축어·XPC 기능을 추가하지 않았다.

화면을 켠 상태에서는 기존 Mac 표시 경로의 지연도 재현됐다. 수정 전 여섯 장면의 처리 p95는23.90~35.15ms였다. Mac의 수직동기화 대기와 기존 제한기가 함께 작동했고, 표시 영역 연결을 매 프레임 갱신했으며, Retina 픽셀 좌표를 쓰면서 고해상도 GL 표면을 명시하지 않았다.

Mac에 다음 변경을 적용했다. iPhone/iPad의 전처리 분기는 그대로 유지했다.

- 버퍼 교체의 추가 수직동기화 대기를 끄고 기존60Hz 게임 타이머를 유지했다.
- 프레임 끝에서 기다릴 때 렌더링에 이미 쓴 시간을 차감했다.
- 창·뷰·화면 위치·픽셀 크기가 바뀔 때 GL 표면을 갱신한다.
- `wantsBestResolutionOpenGLSurface`를 켜서 GL 표면과 Retina 픽셀 좌표를 맞췄다.

Apple은 swap interval0을 가능한 즉시 교체하는 설정으로 설명한다. 드라이버의 모든 대기가 없어지는 옵션은 아니다. [버퍼 교체](https://developer.apple.com/documentation/appkit/nsopenglcontext/flushbuffer%28%29), [컨텍스트 갱신과 표시 설정](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_contexts/opengl_contexts.html).

잠금 해제·개발 서명·계측기 미연결 조건의 최종 XCTest 결과는33개 통과, 실패0, 건너뜀0이다. 각 장면은30프레임 준비 후180프레임을 측정했다. 기존16.67ms 처리 기준은 바꾸지 않았다.

| 화면 크기 | 장면 | 처리 p95 | 평균 호출 간격 |
|---|---|---:|---:|
| 1200×800 | 타이틀 | 12.391ms | 16.667ms |
| 1200×800 | 슬롯 | 10.859ms | 16.667ms |
| 1200×800 | 사격 | 13.876ms | 16.667ms |
| 1920×1080 | 타이틀 | 11.989ms | 16.670ms |
| 1920×1080 | 슬롯 | 14.287ms | 16.685ms |
| 1920×1080 | 사격 | 11.656ms | 16.668ms |

프레임 호출 간격 p95는17.00~17.60ms였다. 순간 최댓값은29.62ms였으므로 모든 프레임이16.67ms 이내라고 표현하지 않는다. 실제 사격·음향과 전투 중 저장 호출0회를 확인했고, 게임 뷰·미디어·렌더러·음향 객체 해제24개 확인도 통과했다. 20종 무기의 키 선택·발사와 등급별 시작 조건 검사를 같은 suite에서 통과했다.

CUA로 치트 앱의 전진·후진·포신 조절·무기 전환·발사·일시정지 메뉴·전체 화면 왕복을 직접 확인했다. 점프 키도 입력했고 점프 동작은 자동 검사에서 확인했다. 정식 앱은 최종 빌드와 실행·메뉴 진입을 확인했다. 전체 화면 자동 검사의 창·뷰는1920×1080, backing은3840×2160이었다. 이 시점에 남겼던 CUA 캡처 여백 대조는 다음 절의2026-10-27 검사로 완료했다.

원시 결과는 `build/apple-performance-20261026/mac-development-signed/`, `mac-signed-pacing-profile/`, `mac-signed-play-memory/`에 있다. [증거 JSON](../evidence/apple-platform-performance-20261026.json)에 측정값·파일 해시·이전 실패를 보존했다. 이번 수정은 Mac 전용이며 iPhone/iPad를 재설치하거나 새로 측정하지 않았다.


## Mac 전체 화면 캡처 대조

2026-10-27 정식 Mac 앱의 전체 화면을 macOS 미리보기의 ‘스크린샷 찍기 → 전체 화면’으로 캡처했다. 원본 PNG는3840×2160이며 게임이 네 가장자리까지 표시됐다. CUA 창 캡처에서 보이던 상단 약110픽셀·오른쪽 약195픽셀의 흰 여백은 없었다.

이미지를 RGBA8로 읽어 RGB 각 값이245 이상인 픽셀을 셌다. 위·아래110픽셀 영역은 각각422,400개, 오른쪽195픽셀 영역은421,200개 픽셀을 검사했고 흰색 픽셀은 모두0개였다. 기존 자동 검사에서 확인한 창·뷰1920×1080, backing3840×2160과 함께 대조했다.

흰 여백은 CUA 창 캡처 경로에서 나타나는 현상으로 분류했다. 게임 화면에 역보정을 추가하지 않았다. CUA 내부에서 여백을 만드는 정확한 구현은 이번 검사 범위에 포함하지 않았다.

원본 캡처에는 시스템 캡처 도구와 바탕화면 알림이 함께 들어 있어 로컬에 보존한다. [검사 JSON](../evidence/mac-fullscreen-capture-20261027.json)에 원본 경로·SHA-256·크기·가장자리 측정값을 기록했다. 이번 대조 대상은 정식 앱이며 치트 앱의 전체 화면 전환·크기 검사는 앞선33개 검사에 포함돼 있다. 게임 소스는 변경하지 않았다.


## iPhone·iPad 실제 기기 검사 — 2026-10-27

공용 모듈을 적용한 치트 앱을 iPhone Air와 iPad Air 13형 M2에 설치했다. 두 기기 모두 iOS/iPadOS27.0.1이며, 각각85개 중84개 통과·1개 실패·건너뜀0개다. 실패한 항목은 `testOnscreenOpenGLCadenceAudioAndResume`의16.67ms 처리시간 기준이다. 전체 통과로 판정하지 않았다. 제품 소스와 성능 기준은 변경하지 않았다.

기기별로 순서대로 실행했고 Simulator는 모두 껐다. 프레임 측정 중 Instruments를 연결하지 않았다. 앱이 사용하는 NSTimer와 비교 검사에 쓰는 CADisplayLink를 따로 측정했다. 아래는30프레임 준비 뒤180프레임의 처리시간 p95다.

| 장면 | iPhone 첫 검사 | iPad | iPhone 후속 검사 |
|---|---:|---:|---:|
| 타이틀 | 8.069ms | 9.627ms | 8.090ms |
| 메뉴 | 14.810ms | 15.056ms | 17.309ms |
| 원본 폭 사격 | 12.498ms | 10.570ms | 8.237ms |
| 확장 폭 사격 | 14.672ms | 11.395ms | 9.505ms |
| 배경 없는 메뉴 | 11.547ms | 14.598ms | 17.629ms |

실제 게임 타이머의 평균 호출 간격은 세 검사에서16.663~16.671ms였다. 약60회/초에 해당한다. 후속 iPhone 메뉴 두 조건은 처리 p95가16.67ms를 넘었으며, 호출 간격 최댓값은19.046ms였다. 처리시간에는 렌더러의 의도적인 프레임 대기도 포함된다. 평균 호출 속도만으로 모든 프레임이 일정하다고 판정하지 않는다.

CADisplayLink 비교 검사는 게임의 NSTimer를 중단하고 별도 화면 동기화 호출을 넣는다. 첫 사격 p95는 iPhone19.093ms·iPad18.897ms, 후속 iPhone16.922ms였다. iPad 타이틀도17.888ms였다. `HMGLRenderer`의 iOS60Hz 대기는 그대로 남아 있다. 두 대기의 영향을 분리하는 검사는 남았으므로, 실패 원인을 확정하거나 기준을 완화하지 않았다. 원시 로그에 남은 `simulator` 문구는 검사에 고정된 문자열이다. 실제 대상은 xcresult와 GPU 이름으로 확인했다.

통과한84개에는 자원·렌더 비교·생명주기·음향 호출·입력 검사가 포함된다. 전체20종 무기의 키 선택·발사, 키보드 이동과 터치 사격 병용, 전투 중 저장 호출0회도 확인했다. 이 항목들은 실제 기기에서 실행한 API 주입 검사다.

Device Hub로 OS 키 입력과 터치를 전달해 별도로 조작했다. 두 기기에서 이동·점프·조준·사격·무기 전환을 확인했다. iPad에서는 키보드만으로 타이틀에서 전투까지 진입했고 필살기3종도 발사했다. 양쪽 기기에서 가로 방향을180도 바꾼 뒤 화면과 터치 사격을 확인했다. 홈으로 나갔다 돌아오면 게임이 멈추고 Resume Game 메뉴가 표시됐다. 실제 Bluetooth/USB 키보드와 손으로 기기를 기울이는 검사는 수행하지 않았다.

| Instruments 기록 | 길이 | 누수 검사 횟수 | 검출 누수 | 종료 시 남은 Heap+Anonymous VM |
|---|---:|---:|---:|---:|
| iPhone 타이틀·슬롯 | 56.038초 | 5 | 0건 | 56.08MiB |
| iPhone 패널·짧은 전투 | 101.019초 | 9 | 0건 | 62.40MiB |
| iPad 패널·짧은 전투·이동·점프 | 151.836초 | 13 | 0건 | 75.37MiB |

위 메모리 값은 Allocations의 살아 있는 할당 합계이며 앱 전체 물리 메모리 값과 다르다. 회전·복귀와 대부분의 무기 수동 검사는 기록 종료 뒤 수행했다. 장시간 발열·메모리 증가 검사는 남아 있다. xctrace의 dyld 영역 중첩 경고도 원시 로그에 보존했다.

사용자가 iPad 전원이 꺼졌다 켜졌다고 알렸다. 기기에는02:05:17 저전력 로그와 `Capacity: 1`, `Voltage: 3620 mV`가 기록돼 있었다. 조회한 목록에는 같은 시각의 HeavyMach 크래시·커널 패닉 파일이 없었다. 앞선 검사·메모리 기록은 종료 전에 저장돼 있다.

[측정값·검사 결과·파일 해시](../evidence/physical-apple-20261027.json)를 보존했다. 일반 앱과 일반 저장 파일은 변경하지 않았다. 이번 결과는 치트 앱의 검사이며 main·v0.9 병합은 수행하지 않았다.

후속 과제로 iPhone·iPad 무선 설치·디버깅 절차를 등록했다. Apple은 Mac과 기기를 페어링한 뒤 네트워크에서 실행하는 방법을 제공한다. 빌드는 Mac에서 수행한다. 케이블 없이 설치·실행·중단점·XCTest·Instruments 연결과 재부팅 후 재연결까지 확인하고 절차로 남긴다. [Apple 무선 실행 안내](https://help.apple.com/xcode/mac/current/en.lproj/dev3e2f4ee6d.html), [Device Hub 안내](https://developer.apple.com/documentation/xcode/managing-your-simulated-and-physical-devices-in-device-hub).


## iPhone·iPad 무선 설치·디버깅 — 2026-10-27

iPhone에 치트 기능이 없는 일반 앱을 설치했다. 케이블을 뺀 뒤 `localNetwork` 연결에서 다시 설치하고 실행했다. iPad에도 일반·치트 앱을 무선으로 설치했다. 두 기기에서 Xcode 중단점 정지, 게임 객체 조회, 실행 재개를 확인했다. 빌드는 Mac의 Xcode27.1에서 수행했고 앱 전송·디버깅은 네트워크를 사용했다. 설치한 일반 앱은 개발 서명된 Debug 구성이다.

| 항목 | iPhone Air | iPad Air 13형 M2 |
|---|---|---|
| 무선 연결 | localNetwork / connected | localNetwork / connected |
| 일반 앱 설치·실행 | 성공 | 성공 |
| LLDB 중단점·변수·재개 | 확인 | 확인 |
| 이번 무선 XCTest | 미실행 | 8개 통과·실패0·건너뜀0 |
| 이번 무선 Instruments | 미실행 | Time Profiler21.716초·CPU 표본10,202개 내보내기 성공 |

iPad의8개 검사는 전체20종 무기 선택·발사, 이동·점프·조준·해제, 키보드와 터치 병용, 메뉴·설정, 화면 크기별 좌표 변환, 항공기 보스 이동 보존, 실제 타이머와 전체 화면 터치를 포함한다. 실제 기기의 입력 API를 호출하는 검사다. 별도 Bluetooth·USB 키보드를 연결한 결과는 아니다. Device Hub로 기존 치트 저장을 이어서 터치 사격, 이동·정지와 일시정지 메뉴도 확인했다. 수동 점프는 화면으로 확정하지 않았으며 자동 검사에서 통과했다.

CPU 기록은 시작·타이틀 구간이다. 게임 프레임 함수가 표본에 포함됨을 확인했다. 이번 기록으로 전투 성능·장시간 발열·누수까지 판정하지 않는다. 앞 절의85개 검사 중 남은 표시시간 실패와 iPhone 메뉴 p95 초과는 그대로 남아 있다. 제품 코드는 변경하지 않았다.

CLI 빌드는 키체인 서명 오류로 실패했고 같은 구성을 Xcode GUI에서 빌드해 성공했다. 첫 무선 XCTest는 Run 빌드에 테스트 묶음이 없어 시작하지 못했다. Build For Testing으로 묶음을 만든 뒤8개가 통과했다. 실패한 로그도 보존했다. iPad는 사용자가 알린 재부팅 이후 무선으로 연결됐으며, 이번 작업에서 추가 재부팅을 반복하지 않았다.

Sentry MobileBuildMCP의 공식 스킬과 기기 설치·실행 소스를 조사했다. 기존 도구로 이번 검증을 수행했고, 실제 명령·연결 확인·서명 오류·테스트 묶음 복구를 프로젝트 전용 스킬로 만들었다. [무선 작업 절차와 조사 출처](../wireless-device-workflow.md), [검사 결과·원시 파일 해시](../evidence/wireless-apple-20261027.json)에 기록했다.


## 수직이착륙 공중 적 3종 추가 — 2026-10-27

상태: **요구 등록·구현 가능성 확인 완료, 제품 구현 전.** 사용자 추가 지시: 기존 스폰 테이블 일부를 대체하지 않는다. 기존 테이블을 유지하고 그 사이에 새 기체의 등장 항목을 추가한다.

- [ ] 야크·해리어·F35 수직이착륙기 계열의 공중 적을 각각1종, 총3종 추가한다. 세부 형식·실루엣은 제작 단계에서 정하고 선택 근거를 남긴다.
- [ ] 기존 헬기들이 사용하는 무기를 재사용한다. 무기·탄환·발사음·피해 계산을 별도 복제하지 않는다.
- [ ] 헬기의 전후·상하 비행을 참고한다. 새 기체에 엔진 불꽃과 전후·상승·하강 동작에 따른 기체 기울기를 구현한다. 불꽃·무기 장착점도 기체 회전을 따라야 한다.
- [ ] 미션별 기존 헬기 배치·능력치를 읽어 새3종의 등장 미션과 빈도·좌표·HP·경험치를 데이터로 정한다. 기존 적 목록의 항목을 삭제·교체하거나 좌표·수량·생성 조건을 변경하지 않는다. 기존 등장 지점 사이에 별도 추가 항목을 넣는다.
- [ ] 원본 맵 파일과 스폰 테이블을 보존한다. 새 기체 배치는 별도 추가 데이터로 읽고 월드에서 함께 생성한다. 기존 적의 식별자·보스 연결·클리어 조건이 추가 항목 때문에 어긋나지 않게 한다.
- [ ] 추가 기체의 선택·행동 난수는 기존 공유 난수 소비 순서를 바꾸지 않도록 분리한다. 동일 입력·기존 난수에서 원래 적 생성 목록이 유지되는지 대조한다. 새 적과의 전투로 생기는 플레이 결과 변화는 별도로 평가한다.
- [ ] Survival을 적용 범위에 포함할 때도 기존4분류 추첨과 Heli001 보너스 생성을 보존하고 독립된 추가 생성 규칙을 사용한다. 기존 추첨 범위를 늘려 원래 적의 등장 확률을 낮추지 않는다.
- [ ] 원래 유닛과 같은 색조·톤·금속 질감·외곽선·명암·디테일 밀도로 제작한다. 실제 기존 기체를 이미지 생성 참조로 사용하고 투명 배경의 별도 에셋으로 보존한다. 엔진 불꽃이 배경·탄환과 혼동되지 않는지 확인한다.
- [ ] 치트 앱에서3종 강제 등장, 전후·상하 이동과 자세, 불꽃·발사점, 전체20종 플레이어 무기의 피격·파괴, 일시정지·재개, 화면 밖 진입·이탈을 검사한다. 추가 적의 충돌 크기는 그림에 맞춘다.
- [ ] 모든 미션에서 원래 적 목록·좌표·수량·보스가 유지되고 새 항목만 추가됐는지 자동 대조한다. 추가 적 때문에 동시 적·탄환·이펙트 수가 증가하는 조건에서 iPhone·iPad·Mac의 프레임·메모리를 측정한다.

### 현재 코드·데이터에서 확인한 사실

`GameData::loadDirectory`로 실제 RawData를 읽었다. Heli001~010은 모두 machType1이며 무기는 `E_Wpn_ASM`(type6, damage25)이다. HP는150~450, 경험치는5~30이다. Heli001에는 고정 아이템2개 드롭 예외가 있어 새 기체에 이름만 바꿔 복사하지 않는다.

캠페인은 `MapDefinition.enemies`의 기체 이름·좌표를 `WorldState::processRegen`이 읽는다. 예를 들어 Stage01은Heli001 두 대, Stage13은Heli001·004·005·006·008·009·010, Stage35는Heli001 다섯 대와Heli010 한 대다. 후반에도 초기형이 등장하므로 단순한 미션 번호→헬기 티어 계산으로 해석하지 않는다.

Survival은 `World.cpp`의4분류 이름 형식과 개수315/200/10/10, Heli001 추가 생성 분기를 사용한다. 이 부분은 코드에 고정돼 있다. 비행 AI는 `Enemy.cpp`의machType1, 이동과 좌우 속도 기반 기울기는 `Motion.cpp::processFly`에 있다. 헬기 부품에는 몸체·회전날개·무기가 있으며 제트 엔진 불꽃은 새로 연결해야 한다. 상하 속도에 따른 자세 변화도 새3종 전용으로 추가한다.

### 단독 작업 가능 범위

기존 코드·원본 데이터·참조 이미지·내장 이미지 생성 도구가 있어 데이터 작성, 그림 제작, 비행/효과 구현, 공용 모듈 연결, 치트 앱 자동·화면 검사를 진행할 수 있다. 이미지 생성은 제작 실행 때 imagegen 스킬, 아트 연결은 integrate-game-art 스킬을 적용한다. 현재 단계에서는 새 그림·제품 코드·기기 설치본을 변경하지 않았다.

조사 도구와 원시 결과는 `build/vtol-feasibility-20261027/`, 추적 가능한 집계는 [헬기 배치 조사](../evidence/vtol-feasibility-20261027.json)에 보존한다.
