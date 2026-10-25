# 버그 수정·사용성 개선 과제

기준: 2026-10-25 사용자 요청.

최신 실기기 결과: [iPhone Air 자체 검사와 후속 범위](physical-self-test-20261025.md). 치트 앱 설치·기능8/8, 전체20무기360조건·결과 이미지45조건 통과. 아래 “실기기 새 수정본 확인은 남음”은 자동 검사에는 해소됐으며, 수동 전체 화면·모든 충돌/피해·출력음·잠금·물리 회전·발열 범위는 보고서에서 따로 남긴다.

## 최신 사용자 보고와 진행 과제

| 과제 | 사용자 관측·가설 | 처리 상태와 완료 조건 |
|---|---|---|
| 온보딩·메뉴 등 미대응 화면의 전체 화면 처리 | 원본 에셋 재배치와 화풍에 맞는 좌우 그림 추가 | **부분 구현.** [타이틀·슬롯 첫 적용](menu-art-extension-20261025.md): imagegen 후보A/B·금속 배경 제작, 원본 보존 합성·슬롯 입력 검사 통과. 치트 앱 설치, 잠금 해제 후 실기기 확인 대기. 나머지 화면의 디자인 감사·검수는 유지. |
| 전체 화면과 적 AI 고정값의 간섭 | 적 이동·사거리·전장·카메라의 고정값이 넓어진 시야와 충돌하는지 전체 조사 요청. | **미완료.** [개발 자료 조사·보정 후보·완료 조건](canvas-ai-and-rotation-research-20261025.md) 등록. 모든 적 계열·보스·Survival의 사용처 감사와 비교 후 보정 결정. 아래 보스 활성 수정만으로 완료하지 않는다. |
| OS를 따르는 양방향 가로 플레이 | iPhone을 왼쪽·오른쪽 어느 쪽으로 눕혀도 가로 플레이. 세로 미지원. | **미완료.** [현재 설정·Apple API·구현/검증안](canvas-ai-and-rotation-research-20261025.md) 등록. 양방향 허용은 이미 코드에 있으나 실제 OS 전환·틸트·터치·HUD 연결을 검사하고 필요한 경로를 수정한다. |
| 미션3-5 항공기 접근·우측 이동 | 확장 뒤 탱크 쪽으로 빨리 접근하고 우측 끝에 잘 가지 않음. | **부분 수정.** 보스 선행 생성이 이동·공격도 앞당긴 원인 수정. 전체 고정값 간섭 과제는 위 별도 TODO로 남김. 원작 접근 조건에서 AI 시작, 이동 상수·전투 경계 유지. 코어17/17·iOS2/2 통과. [증거와 외형 정정](boss-motion-fix-20261025.md). 물리폰 새 수정본 확인은 남음. |
| 클리어 일러스트 전체 화면 | 일러스트가화면 전체를 덮지 않아 월드가 노출됨. | 수정·시뮬레이터 검사 완료. 결과 이미지15개×화면크기3종, 바깥 영역 단색 완전 일치와 결과 버튼 검사 통과. 실기기 새 수정본 확인은 남음. |
| 필살기3종 탄착 위치 | 초기 지급 포탄·미사일·하늘 레이저의 상당수가 터치 위치 대신 탱크 뒤로 떨어짐. | 가짜 해제 좌표가 남은 연사 목표점을 덮는 원인 확인·수정. 전체20종×3크기×6입력360조건 통과. 원작 분산·탄도 유지. 실기기 새 수정본 확인은 남음. |
| 치트 앱 전체 무기 검사 | 기본 무기만 검사해서 전체 무기 정상으로 판정하지 않는다. | [전체 무기 검증 절차](weapon-validation-procedure.md) 제정·AGENTS 의무화. 20종 연사·해제·탄약 결과 기록. 모든 위치의 실제 충돌/피해·출력음·폰 검사는 별도 후속 범위. |

일반·치트 최신 앱은 실제 폰에 설치됐다. 직전 가장자리 실기기 검사는 기본 무기의 일부 입력만 확인했다. 이번 보고의 필살기와 클리어 화면 문제를 포함한 전체 정상 판정이 아니었다. 아래 과거 조사·미설치 설명보다 [현재 상태](current-status.md)의 최신 기록을 우선한다.

최신 실행: 무기 전환음·비활성/잠금·foreground 무입력120초 정책을 구현하고 검증 앱 iOS21/21, Core17/17을 통과했다. 상세 현행 정책은 [실행 정책](runtime-policy.md), 증거는 [현재 상태](current-status.md). 아래 조사는 당시 근거와 발열 후속 계획으로 보존한다. 캔버스도 검증 앱의 확장 검사17/17 후 일반 앱에 반영했다. 전체iOS 실행72중71통과·기존 표시시간 기준1실패, 최종보스/시작연결 영향 검사9/9 통과이며 물리폰 새 버전 설치/잠금 조작 및 발열 계측 완료는 아직 아니다.
 이 문서는 조사 결과와 구현 계획이다. 아래 항목의 제품 수정·장시간 폰 검사 완료를 뜻하지 않는다. 사용자는 폰 연결을 해제하고 문서 정리를 계속하도록 지시했다. 이 조사에서 추가 폰 작업은 하지 않았다. 현재 폰에 설치한 틸트 수정 빌드는 아래 무기 전환음 수정이나 생명주기 정책을 포함하지 않는다. 현행 계약은 [정본](canonical.md), [현재 상태](current-status.md), [시간·저장·음향](runtime-policy.md)을 따른다. 최신 사용자 요청에 따라 원본과 달라지는 사용성 정책은 별도 표시한다.

## 조사 결과와 우선순위

| 과제 | 확인한 사실 | 다음 작업 |
|---|---|---|
| 무기 전환음 | 원본 버튼1100의 `sfxID:4` 호출이 새 앱의 버튼 처리에서 빠졌다. 음원은 초기 로드 목록에 있다. | 버튼 release 경로에 원본 호출을 복원하고 실제 출력까지 검사한다. |
| 앱 전환·잠금 | 현재 background에서 프레임/샘플 타이머를 멈추지만 게임 pause와 메뉴를 설정하지 않는다. resign-active 처리도 없다. | inactive 진입에서 게임 정지·입력 해제·메뉴 대기를 한 번 처리한다. 복귀 후 명시적 재개를 요구한다. |
| 장시간 미사용 | 원본 HeavyMach delegate 두 함수는 즉시 반환한다. 확인한 코드에 별도 미사용 시간 정책은 없다. | OS inactive와 foreground 무입력을 구분한다. 오래 지나도 진행을 몰아서 계산하지 않는다. |
| 발열 | 기존 iPhone Air 기록127.77초 전체가 Serious였다. 테스트 옵션으로 유도한 상태는 아니었다. | 같은 초기 열 상태에서 호환판/복원판의 전력·열·CPU·GPU·표시를 비교한다. |

좌우 잘림과 넓은 게임 캔버스는 좌표·렌더 변경을 동반하는 별도 과제다. 관련 조사·계획은 [전체 캔버스 계획](wide-canvas-plan.md)과 [좌표 정책](coordinate-policy.md), [렌더 정책](rendering-policy.md)에 연결한다. 이 문서의 생명주기·음향 조사로 넓은 캔버스 구현을 완료 처리하지 않는다.

## 원본과 호환 실행본의 근거

원본 실행파일 SHA-256은 `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`다. 아래는 REA/Ghidra 보존 결과이며 개발자의 원본 소스가 아니다.

| 함수 | 보존 근거 | 확인한 동작 |
|---|---|---|
| `GuiGame.handleButton` 0x17c4c | `Analysis/functions/0x17c4c.pseudo.c`, `.asm`; Evidence `ev_8a46958ab40e9493b5d9ae3bc838ca3f2ec1d349c8c17f8e7b302aca7a077481` | 버튼1100 release에서 `changeWeapon:1` 뒤 효과음4. 버튼100은 world pause1과 메뉴 열기,101은 pause0과 메뉴 닫기. |
| `GlobalInfo.init` 0xfd44 | `Analysis/functions/0xfd44.pseudo.c`; Evidence `ev_ac6bdd9fa8807d842fdad1cfd3dfd1c453cc2288c92a6af4ffb3925b78773e16` | 효과음 배열4번이 `Game_ChangeWeapon`이다. |
| `applicationWillResignActive:` 0x225c | `Analysis/functions/0x225c.asm`의 `bx lr`; Evidence `ev_c6e8e2c3326b0b24251ece649542a40c20c4b7c24d3f9487ad618e3f6a064bc6` | 함수 자체는 아무 처리 없이 반환한다. |
| `applicationDidBecomeActive:` 0x225e | `Analysis/functions/0x225e.asm`의 `bx lr`; Evidence `ev_12284fa16b1145e57421ff631a42a74d237dfc18afb30288f17edb2ea0aa28a7` | 함수 자체는 아무 처리 없이 반환한다. |

호환 실행본 루트는 `iOS Proj/HeavyMachRecovery/compat-source/fresh-port/20-touchHLE-src/touchHLE`다. 이 루트의 다음 코드를 비교했다.

- `src/frameworks/uikit/ui_application.rs`: iOS `did_enter_background`는 먼저 `window.set_backgrounded(true)`로 표시를 막고 delegate/notification을 전달한다. `will_enter_foreground`와 `did_become_active`는 이 차단을 해제한다. HeavyMach 전용 자동 일시정지 메뉴를 추가하지 않는다.
- `src/window.rs`: `AppWillResignActive`, `AppDidEnterBackground`, `AppWillEnterForeground`, `AppDidBecomeActive`를 구분한다. 실제 background/terminate 이벤트가 resign보다 우선한다.
- `src/environment.rs`: 긴 guest 프레임 도중 background 이벤트를 발견하면 guest 콜백을 기다리지 않고 host에서 표시 차단 상태를 설정한다.
- `ui_application.rs`의 `suspend_app`·`mute_game_audio_for_suspend` 설명에는 Android 또는 `iMoleVillageAppDelegate` 전용 조건이 있다. 해당 게임의 저장·음소거 정책을 HeavyMach iOS에 복사하지 않는다.
- `isIdleTimerDisabled`/`setIdleTimerDisabled`의 호환 구현은 있다. 이것만으로 HeavyMach가 자동잠금을 금지했다고 판단하지 않는다. 이번 `Analysis/functions/*.pseudo.c`와 현재 `Sources/` 검색에서 HeavyMach의 해당 호출이나 thermal 정책은 발견하지 못했다. 이 검색은 원작 전체 실행에서 기능이 없다는 증명이 아니다.

## 1. 무기 전환음 복원

현재 `HMGameView.mm`의 `gameButton:state:` case1100은 `_session->cycleWeapon()`만 호출한다. `GameSession.cpp`의 `select`는 무기 생성과 `WeaponInventory` UI 이벤트를 보내고 효과음4를 보내지 않는다. 따라서 버튼 경로의 재생 요청 누락은 정적 비교로 확정했다. 음원 파일과 OpenAL 출력장치 자체가 정상인지 여부는 별도 실행 검사다.

구현은 버튼1100 release 처리에서 원본 순서대로 `cycleWeapon` 다음 `Game_ChangeWeapon`을 한 번 요청한다. 공통 `select`에 효과음을 넣으면 시작·치트 초기 선택·탄약 소진 뒤 기본 무기 교체에서도 추가음이 날 수 있으므로 원본의 버튼 호출 위치를 보존한다. 원본은 선택 결과가 바뀌지 않아도 해당 버튼 처리 뒤 효과음4를 요청한다. 무기 하나인 경우도 임의로 무음 처리하지 않는다. 정지 중 차단 규칙과 누름/뗌 구분을 유지한다.

완료 조건:

- 무기1개/여러 개에서 버튼 release 한 번당 실제 `HMOpenALAudio`의 해당 source 시작 횟수가1 증가한다. press/cancel·정지 중 클릭은 증가하지 않는다.
- 초기 무기 선택, 아이템 획득, 탄약 소진의 기존 효과음·순서는 변하지 않는다.
- 시뮬레이터와 폰에서 전환을10회 반복하며 귀로 출력과 소스 진단을 대조한다. 진단 카운터만으로 들리는 소리까지 통과 처리하지 않는다.
- 사격 중·잠금 복귀 후에도 전환음이 들리고 사격 source와 동시 재생한다. 추가 파일 로드나 저장이 생기지 않는다.

## 2. 앱 전환·폰 잠금

현재 `Application.mm`은 `sceneDidEnterBackground`에서 비활성으로, `sceneWillEnterForeground`에서 활성으로 바꾼다. `HMGameView.setApplicationActive`는 타이머와 버튼 상태를 정리하지만 world pause·`_gameMenu`·HUD 메뉴를 변경하지 않는다. `CMMotionManager.stopAccelerometerUpdates`는 현재 dealloc에서만 호출한다. OS가 센서 전달을 멈추는 것과 앱이 명시적으로 생산을 중단하는 것은 구별한다.

사용자 요청 정책은 다음과 같다. 이는 원본의 빈 delegate를 그대로 복사하는 범위를 넘어선 사용성 변경이다. 게임 메뉴의 모양과 명시적 재개 동작은 원본 버튼100/101 구현을 재사용한다.

1. `sceneWillResignActive`에서 프레임 예약·센서 샘플·센서 생산을 중단한다. 키보드·검증 방향·진행 중 터치·연속 사격을 취소한다. background가 되기 전에 필요한 GL 작업을 끝내고 이후 draw/present를 허용하지 않는다.
2. 진행 중 전투는 world를 멈추고 일시정지 메뉴를 대기 상태로 만든다. 비활성 상태에서 메뉴 애니메이션을 그리기 위해 타이머를 다시 켜지 않는다.
3. 업그레이드·게임오버·스테이지 결과 등 이미 진행을 멈춘 화면은 기존 상태를 보존한다. 일반 pause 메뉴가 그 화면을 덮거나 복귀 시 unpause하지 않게 한다.
4. `sceneDidBecomeActive`에서 화면/입력 자원을 복구한다. 전투는 메뉴를 띄운 상태로 유지하고 사용자가 재개를 눌러야 움직인다. `sceneWillEnterForeground`만으로 전투를 재개하지 않는다. 제어센터 등 foreground 안의 일시적 inactive에도 동일하게 적용한다.
5. 재개 시 타이머 기한과 틸트 필터를 초기 입력에 맞춘다. 비활성 시간만큼 tick·탄약·피해·패시브 시간을 보충하지 않는다. 들고 있던 사격 입력을 자동 복원하지 않는다.
6. 일시정지 진입만으로 `HvM.dat` 저장을 추가하지 않는다. 기존 네 저장 지점을 유지한다. 프로세스 종료 후 전투 전체 이어하기는 별도 저장 설계가 필요한 기능으로 남긴다.

음향은 inactive에서 진행 중 게임 source를 정리하고 복귀 시 오래된 발사음을 재생하지 않는 방향으로 검증한다. 사용자 음량/무음·출력 경로 설정은 보존한다. 이 항목은 최신 요청에 따른 정책 변경 후보이며 원본의 OpenAL 동작이 이미 그 처리를 한다고 기록하지 않는다. 실제 iOS interruption·route 변경을 같이 확인한 뒤 `runtime-policy.md`와 구현을 함께 바꾼다.

완료 조건: 사격/이동 중 앱 전환·수동 잠금·자동 잠금·제어센터를 각각 반복한다. 비활성 구간의 world time/HP/탄약/EXP가 그대로이고 draw/present·반복 저장이0인지 확인한다. 복귀 화면은 메뉴 또는 기존 결과/업그레이드 화면이어야 한다. 재개 전 움직임·사격이 없어야 한다. 1초·30초·5분 뒤 복귀와 연속 전환20회에서 종료·중복 타이머·복귀 뒤 무음·틸트 반전이 없어야 한다.

## 3. 오래 사용하지 않은 상태

OS inactive, foreground에서 조작하지 않는 상태, OS가 앱을 종료한 뒤 다시 실행한 상태를 분리한다.

| 상태 | 정책/계획 |
|---|---|
| background 또는 잠금 상태로 오래 경과 | 경과 시간과 무관하게 정지 상태를 유지한다. 프로세스가 살아 있으면 같은 메뉴에서 재개한다. 시간 경과만으로 스테이지 초기화·게임오버·저장을 만들지 않는다. |
| 메뉴를 열어 두고 미사용 | 전투는 정지한다. 메뉴가 안정된 뒤 불필요한 연속 렌더·센서 생산을 멈추는 방식을 계측한다. OS 자동잠금을 허용한다. |
| foreground 전투에서 무입력 | 새 정책안: 유효 조작이120초 없으면 기존 일시정지 메뉴를 연다. 이120초는 원작 값이 아닌 검증용 제안이다. 터치/키/방향 버튼, 유지 중 연속 사격과 실제 이동을 명령하는 필터 후 틸트는 유효 조작에 포함한다. raw 센서 잡음만으로 시간을 갱신하지 않는다. 임계값은 현재 이동 dead zone을 사용하며 별도 센서 상수를 중복 도입하지 않는다. |
| OS가 종료한 뒤 재실행 | 정상 `HvM.dat` 슬롯/선택 화면으로 복귀한다. 메모리에만 있던 전투 위치·HP·탄환 상태를 복원했다고 표시하지 않는다. 기존 저장을 미사용 기간만으로 삭제·덮어쓰기하지 않는다. |

foreground 타임아웃의 시계는 단조 증가하는 실제 시간으로 계산한다. 게임 프레임 시간이나 기기 날짜 변경을 사용하지 않는다. raw 센서 노이즈·책상 위 기울어진 자세·터치 없이 틸트로만 주행·누른 채 사격하는 경우를 먼저 비교한다. 놓아둔 폰이 계속 이동 명령을 내리는 경우와 실제 사용을 센서만으로 완벽히 구별할 수는 없다. 자동잠금과 OS inactive 정지를 함께 검사하며 과도하게 짧은 임계값으로 정상 플레이를 끊지 않는다.

완료 조건: 타임아웃 직전/직후, 연속 누름, 틸트만 사용, 센서 미지원 시뮬레이터, 날짜 변경, background 장기 복귀를 검사한다. 최초 pause만 한 번 발생하고 저장·데미지·EXP 값에 추가 변화가 없어야 한다.120초 채택 여부는 이 사용성 검사 결과와 함께 확정 기록한다.

## 4. 발열 측정과 대책

기존 사실: `build/device-test-20261025/physical-latest-game-performance.trace` Run2와 `latest-display-summary.json`에서 iPhone Air/iOS27.0.1의127.77초 동안 thermal state가 Serious였다. 주 화면 표시59.71회/초, 간격p9516.67ms였지만 이 기록은 장시간 발열 통과가 아니다. 시작 전 열 상태·충전·이전 작업의 영향이 통제되지 않았으므로 현재 게임이 Serious의 단독 원인이라고 확정하지 않는다.

측정 방법:

1. 앱 시작 시 `NSProcessInfo.thermalState`를 먼저 읽고 변화 알림을 등록한다. 상태 전환 시각·현재 화면·일시정지 여부·프레임 통계를 묶어 기록한다. 프레임마다 파일에 쓰지 않는다. 이 API 값은 시스템 열 압력 등급이며 섭씨 온도가 아니다. [Apple thermal state 알림](https://developer.apple.com/documentation/foundation/processinfo/thermalstatedidchangenotification).
2. 실기기 Instruments Power Profiler에서 시스템 전력, 밝기, 충전, thermal state를 함께 기록한다. Time Profiler/Allocations와 Game Performance의 CPU·메모리·표시 간격을 같은 구간과 연결한다. 연결한 Xcode 자체가 전력 관측에 영향을 줄 수 있으므로 동일 연결 조건 비교와 분리 수집을 구분한다. [Apple Power Profiler](https://developer.apple.com/documentation/xcode/measuring-your-app-s-power-use-with-power-profiler).
3. 호환판과 복원판을 같은 폰, 밝기, 음량, 충전 여부, 배터리 범위, 시작 열 상태에서 번갈아 실행한다. 정상 열 상태로 돌아온 다음 다음 측정을 시작한다. 측정 순서를 바꿔 후속 앱이 이미 뜨거운 조건을 받는 편향을 줄인다.
4. 타이틀 정지2분, 전투10분, 메뉴 정지2분, background2분을 각각 관측한다. 전투는 동일 미션/무기/적 조건을 기록하고 치트 사용 여부를 명시한다. 장시간 목표 검사는30분으로 확대한다. thermal 등급 변화 시각, Serious/Critical 체류, 초반/후반 표시 p95·p99·최대, CPU/GPU와 메모리 증가량을 비교한다.
5. 시뮬레이터에서는 열 등급을 주입하는 테스트로 정책 분기, 중복 타이머, 렌더 중단/복구를 검사한다. 시뮬레이터 측정치를 iPhone 온도·배터리 소비 수치로 환산하지 않는다.

대책 순서:

- 먼저 비활성·정지 메뉴의 불필요한 프레임/센서/음향 작업과 중복 타이머를 제거한다. 원본 시뮬레이션 속도·수식은 바꾸지 않는다.
- 전투 중 병목은 실제 trace의 CPU 함수·GPU/표시 비용으로 좁힌다. 매 프레임 객체 생성·텍스처 업로드·파일 읽기/쓰기가 다시 생겼는지 확인한다. 기존 GPU 업로드 후 CPU 이미지 해제와 buffer/source 재사용을 보존한다.
- Serious에서 게임 tick을30Hz로 낮추는 방식은 금지한다. 현재 원본 규칙이1프레임1tick이라 게임 속도까지 달라진다. 표시만 줄이는 대책도 tick/draw 결합을 먼저 분리·검증해야 하므로 즉시 적용하지 않는다.
- Critical에서 일시정지 메뉴와 냉각 안내로 멈추고 자동 재개하지 않는 정책을 후보로 검증한다. Serious에 들어왔다는 이유만으로 즉시 강제 메뉴를 반복해서 열지 않는다. 상태 경계에서 안내/정지 반복을 막는 조건을 둔다.
- 정지 메뉴 렌더 중단, 넓어진 캔버스 픽셀 수 증가, 최종 확대 패스 변경은 각각 따로 계측한다. 여러 변경을 한 번에 묶어 개선 원인을 잃지 않는다.

완료 조건: 정상 열 상태에서 시작한 반복 측정3회에서 비교판과 복원판의 열 전환·지속 전력·후반 프레임 분포를 제시한다. background에서 게임 tick/render·지속 센서 생산이 멈추고 메뉴 정지 시 CPU 작업이 줄어야 한다. Critical 보호 정책은 상태 주입 테스트와 복귀 테스트를 통과해야 한다. 숫자 없이 '발열 개선'이나 '장시간 안정'으로 완료 처리하지 않는다.

## 실행 순서와 기록

1. 무기 전환음의 누락 호출을 먼저 복원한다.
2. 생명주기 정지·입력 정리·메뉴 복귀를 구현한다. 원본 pause UI와 최신 사용성 요구를 연결한다.
3. 열 관측을 추가하고 위 조건으로 호환판/복원판을 비교한다. foreground 미사용 정책과 메뉴 렌더 절감을 검증한다.
4. 각 변경의 원본 근거, 실패 재현, 수정 검사, 실기기 관측을 `current-status.md`에 기록한다. 실제 채택한 정책만 `runtime-policy.md`에 반영한다.

Apple의 scene 기반 앱에서는 `sceneWillResignActive`가 일시적 중단과 background 진입 전의 정지 지점이다. 이번 앱도 scene delegate를 사용하므로 기존 앱 delegate만 추가하는 수정은 부족하다. [Apple sceneWillResignActive](https://developer.apple.com/documentation/uikit/uiscenedelegate/scenewillresignactive(_:)).
