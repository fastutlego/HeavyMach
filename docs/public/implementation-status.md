# HeavyMach 구현과 검증 범위

HeavyMach 1.13을 현재 아이폰에서 실행하는 호환판을 만들고, 이어 수정·빌드 가능한 네이티브 복원판을 작성했다. 복원판의 일반 앱과 치트 앱을 iPhone Air에 설치했다. 사용자는 설치본의 모든 미션을 플레이한 뒤 크래시·오동작·적 이동 이상은 없는 것 같다고 보고했다.

정상 플레이 기준은 `v0.9`다. 현재 저장소에는 앱 빌드·검사 자원도 포함한다. `v0.9`는 자원을 포함하기 전 태그이며 재지정하지 않는다. 2026년 10월 26일의 Apple 확장은 `feat/apple-workspace-modules`에서 진행한다. main/v0.9의 사용자 플레이 확인과 새 브랜치의 Mac·Simulator 검사 결과를 구분한다. [설치 방법](https://github.com/gelb2/HeavyMach/blob/main/docs/public/build-and-install.md)과 [소스 저장소](https://github.com/gelb2/HeavyMach)에서 확인할 수 있다.

## 두 판본의 구조

| 판본 | 실행하는 코드 | 역할 |
|---|---|---|
| IPA 호환 실행판 | 원본 ARMv6 바이너리를 touchHLE 계열 Rust 인터프리터로 실행 | 원작 동작 비교 기준 |
| 네이티브 복원판 | C++20 게임 코어와 Objective-C++ Apple 연결 코드 | 수정·빌드·테스트할 수 있는 현재 제품 |

원본 개발 프로젝트를 확보한 것은 아니다. IPA의 실행파일·자원과 REA 분석 결과에서 규칙을 확인해 새 코드를 작성했다. 복원판은 Swift로 작성하지 않았다.

게임의 이동·전투·진행·저장은 `Sources/Core`, 창·입력·화면·음향은 `Sources/iOS`에 있다. 게임 그래픽은 OpenGL ES 1.1, 음향은 기존 호환판에서 보존한 OpenAL Soft 1.22.2를 사용한다. UIKit은 창·입력·검증 UI에 사용한다. [소스별 설명](https://github.com/gelb2/HeavyMach/blob/main/docs/ios-source-map.md)에 각 파일과 정책·검사를 연결했다.

## 구현한 기능

| 영역 | 현재 동작 |
|---|---|
| 플레이 | 전차 이동·점프·터치 사격, 무기 전환, 경험치·부품 강화, 미션 진행과 저장 |
| 일반·치트 앱 | 같은 코어를 사용한다. 치트 앱에는 미션·무기·부품·EXP 설정과 방향 버튼이 있고 저장 공간을 분리한다. |
| 렌더링·음향 | 원본 텍스처·블렌딩·그리기 순서와 WAV를 사용한다. GPU 업로드 뒤 CPU 이미지 메모리를 해제하고 음향 자원을 재사용한다. |
| 시간·저장 | 원작의 한 번 갱신 규칙과 저장 호출 위치를 복원했다. 발사할 때마다 파일을 저장하던 경로를 제거했다. |
| 게임 화면 | 전투 오브젝트 크기를 유지하면서 가로 시야를 넓혔다. 확장 영역의 터치 좌표와 필살기 해제 입력을 수정했다. |
| 결과·메뉴 | 미션 결과 일러스트를 복구하고 뒤쪽 월드를 덮도록 수정했다. 타이틀·슬롯·모드·미션에 좌우 배경을 추가하고 연결부를 수정했다. |
| 중단·재개 | 비활성·잠금 시 게임과 입력·음향을 멈춘다. 복귀 후 직접 재개한다. 전투 중 유효 입력이 120초 없을 때도 일시정지한다. |
| 적 활성 | 확장 화면에 보스를 미리 표시하는 것과 이동·공격 시작을 분리했다. 미션 3-5 항공기의 선행 접근을 수정했다. |

전체 화면, 중단·재개와 검증 UI는 사용자가 요청한 확장이다. 원작 그대로인 부분과 추가한 동작은 [게임 기획](https://github.com/gelb2/HeavyMach/blob/main/docs/game-design.md)에 구분했다.

## 확인한 결과

| 확인 방법 | 결과 | 범위 |
|---|---|---|
| 사용자 플레이 | 설치본의 모든 미션에서 크래시·오동작·적 이동 이상을 발견하지 못했다고 보고 | 일반·치트 중 어느 앱인지와 플레이 시간은 별도로 기록되지 않음 |
| 아이폰 설치 | 일반·치트 앱 서명·설치·실행 성공, 타이틀의 치트 버튼 유무 확인 | 일반 앱 Documents의 설치 전후 해시 일치 |
| 물리 iPhone 자동 검사 | 기능 8개, 무기 20종·360조건·1,080발 검사 통과 | 이동·점프·사격·전환과 목표·연사·탄약 검사. 모든 장면의 피해·출력음을 전수 검사한 결과는 아님 |
| 메뉴 확장 검사 | Simulator와 iPhone Air 각각 26개 통과 | 배경 연결·경계 수정 범위 |
| 새 원격 clone | 일반·치트 × Simulator·iPhone 빌드4개, 새 Simulator 두 앱 실행, 코어17·iOS14 통과 | IPA·기존 프로젝트 읽기 차단. iPhone 빌드는 서명 없음. [검사 기록](clone-build-20261026.md) |
| 코어 검사 | CTest 17개 통과 | 코어 조건 검사. 전체 미션 사용자 플레이와 별개 |
| 아이폰 성능 기록 | 127.77초 동안 주 화면 갱신 59.71회/초, 간격 p95 16.67ms | 대부분 정지 전투 구간. 열 상태 Serious에서 기록 |

각 행은 서로 다른 실행이다. 검사 수를 합쳐 하나의 통과율로 표시하지 않는다. Simulator에는 16.67ms 표시시간 기준을 넘긴 실패 기록도 남아 있다. [현재 상태](https://github.com/gelb2/HeavyMach/blob/main/docs/current-status.md), [실기기 검사](https://github.com/gelb2/HeavyMach/blob/main/docs/physical-self-test-20261025.md), [설치 기록](https://github.com/gelb2/HeavyMach/blob/main/docs/release-review-20261025.md)에서 조건을 확인할 수 있다.

## 남은 작업

장시간 전투의 발열·전력 비교, 실제 OS 가로 회전과 입력의 연결 검증, 첫 온보딩과 나머지 화면의 전체 화면 검수가 남아 있다. 메뉴 전환을 화면 전체의 큰 문 하나로 바꾸는 요청도 아직 구현하지 않았다. 전체 적 AI의 고정값 감사는 예방 조사로 남겼다. 사용자의 전체 미션 플레이에서 적 이동 이상이 없었다는 결과는 유지한다.

Android는 C++ 코어의 ARM64 컴파일까지 확인했다. Android 앱·APK는 아직 만들지 않았다. 접히는 기기의 화면·조작부 분리도 검토 단계다. 세부 작업은 [TODO](https://github.com/gelb2/HeavyMach/blob/main/docs/usability-todo.md)와 [Android 계획](https://github.com/gelb2/HeavyMach/blob/main/docs/android-port-plan-20261026.md)에 있다.

## 버전과 배포

`v0.9`는 Git 태그이며 앱 Info.plist의 표시 버전 `1.13`과 구분한다. 작업 브랜치는 `develop`에 병합하고, `main`에는 `develop`만 병합한다. 이전 코드를 따로 열 때는 `git worktree add --detach ../HeavyMach-v0.9 v0.9`를 사용한다. 아이폰 앱·저장 데이터의 복원은 해당 소스의 재빌드와 저장 백업이 별도로 필요하다.

현재 앱은 개발 서명으로 아이폰에 설치한 상태다. App Store·TestFlight 배포나 GitHub Release 설치 파일 제공은 진행하지 않았다.

## Apple 확장 브랜치

[Apple 플랫폼 구조](../apple-platforms.md)에서 workspace·프레임워크·자원 번들과 native Mac을 설명한다. iPhone과 iPad는 하나의 iOS 앱을 공유한다. [키보드 입력](../keyboard-input-policy.md)은 이동·점프·조준·사격·무기 전환·메뉴·키 재지정을 포함한다. [현재 상태](../current-status.md)의 후속 검사 범위를 확인한다. 자동조준과 HUD 배치 편집은 TODO의 연구·계획 단계다.
