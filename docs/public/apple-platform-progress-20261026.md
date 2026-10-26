# Apple 공용 모듈·키보드 검사

2026년 10월 26일, `feat/apple-workspace-modules` 작업 중 기록이다. 공용 모듈과 키보드 입력을 구현했다. 기능 검사와 Mac 프레임 검사는 통과했고, iPhone/iPad Simulator의 프레임 검사는 실패했다. Mac 전체 화면 캡처에서 보인 여백은 게임 없는 AppKit 비교 앱에서도 재현됐다. 사용자가 외부에서 원격으로 지시한 조건에 따라 실기기 연결을 기다리지 않고 Simulator와 현재 Mac에서 검사했다. 물리 iPhone/iPad 확인은 후속 항목이다. Mac의 최종 화면 캡처 확인은 잠금 상태로 남았다. main·v0.9에는 이 변경을 반영하지 않았다.

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
| native Mac | 기존31개 + 추가 성능1개 통과 | 이동·점프·조준·사격·20종 무기·메뉴·이미지·GL·음향 |
| Mac 치트 무기 선택 검사(위31개에 포함) | 1/1 통과 | 등급0~6, 전체 무기 지급 켜기/끄기, 무기20종의 280조합 |
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

Mac의 첫 Allocations+Leaks 기록은 저장됐다. 추가 heap 수집 실행에서는 종료 시 Instruments Leaks가 메모리 그래프 생성 오류를 냈고, 별도 `leaks` 출력은 보존했다. 게임 없는 AppKit 비교 앱의 Instruments 연결은 OS 권한 오류(-60007)로 실패했다. 따라서 Mac을 누수0건 또는 게임과 무관함이 입증됐다고 표시하지 않는다.

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

XCTest 없이 일반 Mac 게임 앱도 실행했다. 이 측정용 Debug 빌드에만 `com.apple.security.get-task-allow`를 지정했다. 두 차례 `leaks` 결과는416개·19,968바이트와417개·20,016바이트였고 physical footprint는 모두313.3MB였다. 앞선 XCTest 실행과 같은 자동 단축어 연결 세 개의 참조 순환이 검출됐다. 게임 객체 해제 검사는 통과했지만 이 연결의 발생 원인은 아직 확정하지 않았다.

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
