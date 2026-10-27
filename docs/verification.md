# 검증 기준과 현재 증거

> **과거 실행·분석 이력.** 이 문서의 “최신”·“미해결”은 아래 기록 당시를 뜻한다. 현행 구현과 설치/사용자 결과는 [현재 상태](current-status.md), 현재 검사 선택은 [업무 연결표](document-map.json)와 분야 정책을 따른다. 후속 사용자 전체 미션 플레이에서 크래시·오동작·적 이동 이상은 관측되지 않았다. 과거 실패/계측 원문은 보존한다.

## 사용자 실기기 관측 보충

사용자가 실제 iPhone에서 소리 정상 출력을 확인했다. 프레임 급락 조건은 주인공 탱크 사격과 메뉴 화면 애니메이션 재생이다. 심할 때 한 자릿수FPS라고 보고했다. 이는 사용자 관측이며 계측값은 아니다. 원본과의 음향 동일성 비교와 구분한다. 이미 제공된 재현 조건을 다시 묻지 않는다.

## 후속 치트 검증

사용자 요청으로 남은10개 맵을 치트 조건에서 검증해 **10/10클리어·보스9/9처치·저장후일반코어진입10/10**을 확인했다. 다음 맵8개·미션 메뉴 요청2개다. 무적·무한탄약과 낙하복귀1회를 사용했다. [조건·로그·한계](cheat-stage-check.md)를 따른다. 비치트25/35와 조건을 합산하지 않는다.

## 당시 최신 확인 — HUD 이벤트 순서

[HUD 이벤트 순서](hud-event-order.md)를 원본과 맞췄다. 실제 iOS 수정 전3개 검사에서7개 assertion 실패를 확보했고, 수정 후 관련9/9·CTest17/17·일반 iPhone 서명 제외 빌드를 통과했다. 같은35맵 재검사는 **25클리어·10사망·0시간초과**다. 다음 맵 진입20개와 미션 메뉴 요청5개이며, 독립 초기 조건 검사다. 이전23/35 기록을 현재 결과와 합산하지 않는다. 원본 전체 동등성과 연속 캠페인 성공은 미검증이다.

당시에는 실기기 FPS 급락이 사용자 보고상 미해결이었으며, 성능 수정을 보류하고 기능 검증 결과를 마감했다. 이후 성능 수정과 사용자 확인은 current-status의 후속 기록을 따른다. 재진입 HUD 표시 초기화 차이까지 후속 수정했고 관련iOS10/10·일반iPhone빌드를 통과했다. 최신근거는 `build/hud-persistence/manifest.json`이다. 아래의 이전 검증 기록은 각 당시 소스로 한정한다.

## 이전 전투·화면 전환 수명 수정

[무기 소진·시간제 효과](session-lifecycle.md): 새 탄약 재획득 삭제와 강화 효과 전환 소실을 수정했다. 최종CTest17/17, 관련ASan/UBSan3/3, 실제iOS GameView 연결1개, 일반iphoneos서명제외빌드가 통과했다. 같은35맵 재검사는23클리어·12사망이며 이전 JSON 결과와 CSV trace를 모두 재현했다. 물리기기·실제UI조작·연속캠페인·원본전체동등성 검사는 아니다. 아래 HP 막대 및 이전 증거는 각 당시 범위를 유지한다.


## 이전 HP 막대 수정

[원본 근거와 회귀 검사](gauge-registration.md): 적 막대의 fixedPos·독립 culling과 양쪽 막대의 tick 등록/draw 갱신을 복원했다. 등록 전 추락사망과 등록 후 피격사망, binary32 재귀 좌표 순서도 검사했다. 최종 CTest16/16, 관련 ASan/UBSan, 일반 iPhone·Simulator 서명 제외 빌드가 통과했다. 실제 CGL 비교3프레임은 최대 RGB 차이2/2/2이며 픽셀 일치는 실패다. 이번 수정 뒤 UI·실기기·35맵 전체 여정은 재실행하지 않았다. 아래 이전 증거는 각 실행 당시 소스로 한정한다.


## 이전 통합 — 음원 준비·전이 계약

원본31음원 사전준비를복원했다. 수정전 실제iOS검사3개중2개실패(파일decode1개통과), 수정후음향3+렌더4=7개모두통과했다. CTest15개와 일반iphoneos서명제외빌드도통과했다. [음향근거·로그](audio-preload.md)를따른다. 물리기기에추가설치하지않았다.

[StageTransition](stage-transition-contract.md)의 일반35/직접6 초기상태계약·820조건은통과했다. 최신 실제전투여정23/35와별도다. [GL샘플링차이진단](texture-sampling-diagnosis.md)은차이를블렌딩전단계까지좁혔으며픽셀일치통과는아니다.

## 이전 확인 — 렌더 연결과 35개 맵 여정

[렌더 iOS 연결](sprite-native-integration.md): 원본 GL_REPLACE, 전체 텍스처 보간·POT 패딩·RGB565 정규화와 Trail UV를 복원했다. 480×320 프레임을 먼저 합성한 뒤 Air 화면에 확대한다. **CTest 14/14, 실제 시뮬레이터 UIKit 검사 4/4, 일반 iphoneos 서명 제외 빌드가 통과**했다. Mac 성능 실패를 근거로 두 raster 파일에만 O2를 적용했다. 물리 기기 추가 설치·최신 앱 화면 직접 검사는 하지 않았다.

[최신 맵 여정](stage-journeys.md)은 앱의 카메라 흔들림을 연결한35개 독립 시작에서 **23클리어·12사망·0시간초과, exit1**이다. 다음 맵 전환18개와 미션 메뉴 전환5개를 확인했다. 이전27/35는 카메라 미반영 조건이며 대조 검사에서 기존 JSON/CSV 값이 모두 재현됐다. 초기 등급6·원본 유한 탄약과 기존 조작 전략을 유지했다. 제품 소스는 변경하지 않았다. 연속 캠페인 성공과 전체 UIKit 타이밍 일치는 미검증이다.

기존 보스 전투 **48/48 처치**는 별도 전투 조건의 결과다. 보스 검사는 Trail UV/렌더 변경 전의 실행 증거다. 최신 여정은 현재 소스로 재실행했다. 함수 분석·선언 수나 개별 검사 통과 수를 전체 복원 완료율로 계산하지 않는다.

검증 대상은 새 복원 소스 빌드다. 기존 호환 실행 결과는 비교 기준으로만 재사용한다. 현재 상태는 [current-status](current-status.md)에 기록한다.

## 기록 단위

검사마다 소스 버전·빌드 식별·원본 SHA·기기/OS·명령·입력·기대/실제·로그/화면 위치를 남긴다. 직접 관측과 사용자 보고를 구분한다. 실패 원본과 성공 후속 기록을 함께 보존한다. 기존 사용자 저장은 복사해 보존하고 별도 테스트 저장으로 회귀 검사를 한다.

## 검사 순서

| 범위 | 확인 방법 | 통과 기준 |
| --- | --- | --- |
| 원본 보호 | IPA·실행 파일 SHA 비교, Git 제외 확인 | 원본 SHA 유지, 원본·비밀·산출물 유입 없음 |
| 데이터 reader | 기존 46개 원작 MAP/MCH의 원시 offset·개수 대조 | 전 파일 경계 준수, 미해석 필드 명시, Assault 데이터 혼입 없음 |
| 산술·입력 | 원작 경계값과 동작을 함수 단위 비교 | 임계치·부호·clamp·터치 해제·회전 좌표 일치 |
| 소스 빌드 | iOS simulator ARM64, device ARM64 분리 | 현재 소스에서 빌드 성공; 원작 ARM 해석기로 게임 실행을 대체하지 않음 |
| 서명·설치 | 서명 검증·실기기 설치·콜드 실행 | 해당 빌드의 실기기 실행 확인 |
| 화면 | 타이틀→슬롯→시나리오→미션→HUD 캡처 비교 | 배치·비율·타일·알파·방향·아이콘 확인 |
| 조작·전투 | 기울기/대체입력·사격유지/해제·부스터·피격·벽/계단·낙사 | 정상 경로, 입력을 떼면 동작 종료, 원작 규칙 유지 |
| 진행·성장 | 오른쪽 끝·보스 조건, 아이템·새 무기·EXP 축적 | 스테이지 전환과 정상 레벨업·업그레이드 |
| 저장 | 정상 저장→종료→재실행→동일 슬롯 | 진행·무기·레벨 유지, 원본 사용자 저장 무변경 |
| 생명주기·음향 | 일시정지·백그라운드·복귀·효과음 | 상태 보존, 중복 루프·멈춤·입력 잔류 없음 |

사용자 지시에 따라 별도 검증 앱에서 레벨업 직전 EXP·무기·부품·미션의 시작 조건을 지정할 수 있다. 이후 실제 경험치 획득·강화·전투·저장 코드를 실행해 검증한다. 조건 설정과 실행 결과를 구분하고 일반 앱의 사용자 저장은 보존한다. 일반 적 전멸은 원작 클리어 필수 조건이 아니므로 검증 기준에 임의 추가하지 않는다.

## 기존 비교 증거의 한계

호환 실행본에서 사용자는 다음 미션·낙사 실패·아이템·새 무기·재실행 저장을 확인했다. 레벨업은 미확인이다. 에이전트의 첫 시뮬레이터 Stage1 시도는 HP 소진으로 실패했다. 이 기록은 새 소스 빌드에서 같은 항목을 이미 통과했다는 뜻이 아니다.

전체 메서드 목록, 대표19쌍 분석, 데이터92파일 파싱은 각각 범위가 다르다. 자동 검사 통과 뒤에도 실제 화면·기기 조작·사용자 저장 결과를 별도로 확인한다.

초기 사용자 보고에서 호환 실행본의 iPhone Air 좌우 화면 잘림을 확인했다. 후속 소스 앱의 짧은 Air 검사에서는 좌우·하단 HUD 전체를 확인했다. 이후 변경된 빌드는 실기기에 추가 설치하지 않았으므로 최신 전체 화면·회전·터치 좌표 검증은 남아 있다. 사용자가 확인한 기존 적 피격·애니메이션 효과는 호환 실행본의 비교 근거로 구분한다.


## 2026-10-24 소스 앱 Air 검사

- 기기 우선순위: 사용자 지시에 따라 Air 먼저, Duo 검사 중단.
- `HeavyMachRestored.xcodeproj`, bundle `com.gelb2.heavymach.restored`, ARM64 simulator build 성공.
- Air iOS 26.5에 별도 설치했다. 기존 호환판 bundle과 저장을 덮어쓰지 않았다.
- CUA로 홈 화면 원본 아이콘, 가로 타이틀 전체, 슬롯 선택/생성, 시나리오 모드, Mission01/잠긴 Mission02 표시를 직접 확인했다.
- 발견한 결함: 슬롯 선택 이미지가 검은 사각형으로 아래 화면을 가렸다. 원본 `setBlendType:2`와 ARM switch case(SRC_ALPHA,ONE)를 확인하여 UIKit `PlusLighter`로 수정했다. Air에서 검은 사각형이 사라진 결과를 확인했다.
- 원본 480×320을 safe area 안에 비율 유지 배치한다. 타이틀/메뉴는 양끝과 하단 버튼이 보였다. 기존 실기기 호환판의 좌우 잘림과 새 게임 화면의 실기기 검증은 남아 있다.
- 이 실행본에서는 게임루프 연결 전까지 검사했다. 스테이지 클리어/레벨업/적 피격 성공으로 확대하지 않는다.
- 후속 GameSession/HUD/CoreMotion 연결은 현재 구현 중이다. 위 성공은 그 변경 이후 빌드·실행을 대신하지 않는다.

코어 재현: `cmake -S HeavyMach -B HeavyMach/build/core -DCMAKE_BUILD_TYPE=Debug`, `cmake --build HeavyMach/build/core -j 4`, `ctest --test-dir HeavyMach/build/core --output-on-failure`. 현재 Core/Combat/Visuals/World 4개 통과. Core에는 Player와 원본 고레벨 VFP 변환 경계도 포함한다.

## 2026-10-24 Air 게임루프 후속 검사

기존 메뉴 전용 빌드 기록 이후 GameSession/HUD/CoreMotion/CameraShake를 연결한 네이티브 앱을 빌드·설치했다. CUA 키보드 방향키(기울기 대체), Shift/F 사격, Space 부스터와 화면 터치만 사용했다. HP/EXP/저장/게임 규칙을 변경하지 않았다.

- 첫 시도: 적 출현·피격·HP 감소·HP 0 게임오버 확인. `build/air-native-play-first.log`.
- 두 번째 시도: 적 처치·아이템 획득·지형 이동 후 Stage1 클리어. CUA 화면에 STAGE CLEAR와 두 결과 버튼 표시. `build/air-native-play-second.log` 12:40:44 기준 mission0 stage1, HP13, EXP44, level0, gameState1 paused1. stage1은 다음 스테이지의 0-based 값이다.
- NEXT STAGE를 눌러 MISSION 1:2와 다른 지형·적을 확인했다.
- MENU → SAVE & EXIT GAME으로 저장한 뒤 `simctl terminate`/`launch`로 앱 프로세스를 재시작했다. 동일 슬롯에서 Stage02가 선택되며 다시 MISSION 1:2에 진입했다. `build/air-native-save-relaunch.log` 12:42:29 EXP44/level0 보존. 정상 스테이지 시작 시 HP256으로 초기화됨.
- Air 게임 화면에서 원작 480×320 전체와 HUD가 보였다. 실기기 화면·틸트 검증은 아직 남음.
- device ARM64 코드 빌드: `xcodebuild -project HeavyMach/iOS/HeavyMachRestored.xcodeproj -scheme HeavyMachRestored -destination 'generic/platform=iOS' -derivedDataPath HeavyMach/DerivedDataDevice CODE_SIGNING_ALLOWED=NO build` 성공. 로그 `build/native-device-unsigned.log`. 이 빌드는 서명 없이 컴파일·링크를 확인했다.
- 첫 레벨업, 보스, 새 무기 획득 후 재실행, 전체 영상·음향 동등성은 미확인이다. 현재 함수군별 복원 범위는 [source-coverage](source-coverage.md)를 따른다.

위 실행본 기준 CTest 6개(Core/Combat/Visuals/World/GameSession/AmmoSync)가 통과했다. 탄약 pickup 후 cache 동기화 회귀와 원본 고레벨 VFP 변환 경계를 포함한다. 이후 변경 검사는 별도로 기록한다.

## Air 후속 진행·레벨업 (12:47–12:53)

- Stage2 직접 클리어: air-native-save-relaunch.log 12:47:26, 다음 stage2(0-based), HP3 EXP205. Stage3 진입 후 낙사 실패까지 관측.
- LoadingTransition와 HMButtonState, PartsDummy 수정 포함 simulator build 성공. 로그 native-loading-simulator.log. 새 앱 설치 후 타이틀→슬롯→모드→미션→게임 패널 닫힘·열림을 CUA로 확인.
- 완료한 Stage2를 정상 메뉴에서 다시 선택해 전투. air-native-loading-play.log에서 EXP229→289→304, internal level0→1. CUA HUD LV02와 UPGRADE 화면 확인.
- 공격 선택 상태에서 OK를 눌렀고 게임으로 복귀했다. MENU→SAVE & EXIT GAME 후 원본 경로인 슬롯 화면으로 전환. LV02, 공격02 표시 확인.
- 저장 파일을 읽기만 하여 3624bytes, EXP304, internal level1, weapon grade1, farthest2 확인. SHA256 7f77ee0b663795a4f890fbdd5b1039ac90440f3a0dfd6ee6f0a744a87d5e3e6d. 세부 JSON build/air-levelup-save-evidence.json.
- 이후 프로세스 종료·재실행하고 같은 슬롯에서 Stage3에 재진입했다. LV02/공격02/EXP304가 유지됐으며 이동·사격 후 일시정지했다. 로그 air-native-levelup-relaunch.log.
- 변경 후 CTest 7개 통과(LoadingTransition 포함 Core 및 HMButtonState 추가). 새 실행 로그에서 PartsDummy 누락 요청은 관측되지 않음.

## 별도 검증 앱 (사용자 승인 조건 설정)

HM Validation의 패널→EXP299 시작→실제 addExperience(1)→LV02/UPGRADE→이동 강화→차체 외형 변경→SAVE & EXIT→프로세스 종료·재실행→기존 저장 재개를 Air iOS 26.5에서 직접 확인했다. 재개 시 조건을 다시 주입하지 않았다. 일반 앱과 검증 앱은 별도 bundle/Documents다. CTest 8개 통과와 18종 강화 자동 검사, 전체 41맵 단기 로드, 48보스 접근 지형의 정확한 범위는 [검증 모드](validation-mode.md)에 기록했다.

## 무기·첫 보스 플레이·원본 계약 수정

검증 조건에서 원본 무기20종 사격·탄약·저장복원 자동 검사 및 Air의MBoss01 처치→Stage3 클리어→Stage4→프로세스 재실행→저장된Canon01 재선택·사격을 확인했다. 상세 조건과 로그는 [검증 모드](validation-mode.md)에 있다. 48보스 자동 전투 결과는8성공/40플레이어사망으로 보존했다.

원본 주소를 재검토하여 [타이틀·슬롯 난수](core-title.md) 호출 시점과 [음향](audio-fidelity.md)의 동일 효과음 재시작·거리 감쇠를 수정했다. 웹 버튼은 원본 문자열 `http://www.indiean.com`에 연결했다. 실제 웹 접속은 수행하지 않았다. 후속 Core CTest9개 통과(`build/fidelity-core-ctest.log`); 보스 runner의40실패는 이9개 회귀검사와 별도이며 숨기지 않는다.

## 13:21 일반 복원 앱 실기기 서명 및 설치 시도

Xcode의 HeavyMachRestored scheme과 실제 PaleMirror/Air 대상에서 Build Succeeded를 확인했다. DerivedData/HeavyMachRestored-cweztugewwwviqfjypghnhdbyzfg/Build/Products/Debug-iphoneos/HeavyMachRestored.app의 strict/deep codesign 검증은 성공했다. CLI 서명은 앞서 errSecInternalComponent로 실패했으며 해당 로그를 덮어쓰지 않았다.

`devicectl device install app`은 무료 개발 프로필의 설치 앱3개 제한으로 실패했다. 오류 원본: `build/native-fidelity-device-install.json`. 기존 앱을 삭제하지 않았다. 현재 새 소스 앱의 물리 Air 설치·실행·화면·틸트 검증은 미완료다.

## 기존 앱 제거 승인 이후 설치·저장 이전

사용자 “전부 다 삭제” 승인 후 기존3앱 제거 성공. HeavyMach Documents 전체를 Mac에 먼저 백업했다. 일반 복원 앱의 Air 설치 및 기존 HvM.dat 복사는 성공했다. 최초 launch는 iOS Security(서명·entitlements·프로필 신뢰) 오류로 거절됐다. 설치 성공과 실행 성공을 구분한다. 관련 JSON·로그와 저장 SHA는 [현재 상태](current-status.md)의 후속 기록을 따른다.

최신 지면 조건 수정과 타이틀·음향 구현을 포함한 simulator build 및 CTest9개 통과: `build/validation-ground-simulator.log`, `build/validation-ground-core-ctest.log`. 검증 앱을 Air 시뮬레이터에 재설치·실행했다.

## 13:27–13:32 물리 Air 짧은 검사

신뢰 완료 후 일반 복원 앱의 cold launch→기존 두번째슬롯 LV07/부품03·03·03→Scenario→Mission03 Stage01→실제게임을 CUA로 확인했다. 키보드 대체 이동·터치 사격·일시정지·SAVE & EXIT를 실행했다. 좌우와 하단 HUD가 모두 표시됐다. 이후 저장 읽기에서 EXP6654/internal level6/부품grade2·2·2/mission2 stage0/무기12종 유지 확인. `build/air-native-device-relaunch.log`, `build/air-native-save-after-play.json`. 실제 틸트 방향·감도, 장시간 플레이, 홈 아이콘은 이번 검사에 포함하지 않았다.

검증 앱 실기기 서명·설치·실행·패널·EXP299 조건 시작 성공. 버튼으로 경험치를 얻은 결과를 확인하기 전에 기기 잠금으로 중단했다. 실기기 레벨업·외형 변경 확인은 남았다. 로그 `build/air-validation-device-play.log`. 사용자의10분 이내 제한에 따라 실기기 조작을 종료했다.

시뮬레이터의 추가 성장 검사에서는 body/top/weapon grade5·5·5,EXP299로 시작→실제 EXP1 획득→LV02→DEFENSE 선택/OK→상체 외형 변경·저장을 확인했다. `build/air-validation-ground-relaunch.log`. 전체18개 외형 화면 비교는 미완료다.

## 이전 host 전투·여정 검사

카메라 수정 후 CTest12개, simulator 검증 앱 빌드, 일반 iphoneos 서명 제외 빌드가 통과했다. 물리 Air 설치/실행 범위는13:32 종료 당시 빌드로 한정한다.

[보스 v6](boss-battles-v6.md): 최신 코어 단일48개 전투 모두 지정 보스HP0/removed 확인, exit0. [맵 여정](stage-journeys.md): 첫41개 독립 시작19클리어/21사망/1시간초과, exit1. 보스 성공을 전체 스테이지 클리어로 계산하지 않는다. 초기 검증 조건·유한 탄약·실제 입력을 사용하며 원본 규칙을 바꾸지 않았다. 로그는 `build/boss-v6/`, `build/stage-journeys/`에 있다.

41개 중 원본 일반 진행으로 도달하는 맵은35개다. 최초 여정 검사의 일반 진행35개는19클리어·15사망·1시간초과다. 나머지6개는 데이터에 있지만 일반 nextStage가 연결하지 않는 mission index7의 직접 지정 검사이며 모두 사망했다.

## RGB565·빌드 구성 후속

알파 채널이 없는 원본 이미지16개의 RGB565 색 변환을 이미지 로딩 경로에 복원했다. 원본 RGBA679개는 새 변환을 건너뛴다. 695개 원본/파생 형식·해시는 `Resources/texture-formats.json`에 있다. 원본 및 파생 이미지 파일은 수정하지 않았다. [렌더 검토](render-fidelity-audit.md).

변환65536개 값의 검사와 독립 이미지 방향·채널·버퍼 수명 검사(ASan/UBSan)가 통과했다. 전체 CTest는 **13개 통과**(`build/texture-format-ctest.log`), simulator 검증 앱과 일반 iphoneos 서명 제외 빌드도 성공했다(`build/texture-format-simulator-build.log`, `build/texture-format-device-unsigned-build-retry.log`). 이 시점에 남았던 UIKit 보간/atlas 차이는 후속 [렌더 연결](sprite-native-integration.md)에서 수정·표본 검증했다. 전체 장면의 원본 출력 일치는 여전히 미검증이다. 실기기 추가 설치와 최신 UI 비교는 하지 않았다.

xcodegen 재생성 뒤 일반 앱 scheme이 없어 첫 iphoneos 빌드는65로 실패했다. 실패 로그 `build/texture-format-device-unsigned-build.log`를 보존했다. `iOS/project.yml`에 일반/검증 scheme을 명시하고 재생성해 두 target 분리를 확인한 뒤 재빌드했다. 일반 앱에는 HM_VALIDATION=0, 검증 앱에는1을 유지한다.

## 맵 여정 후속 원인 구분

Stage18의 첫 정지는 뒤에 남은 보스를 제외한 자동 조작과 원본 보스 이동 경계가 만든 결과였다. 후속 전환 정지는 하네스의 즉시 업그레이드 닫힘 호출에서 발생했다. 원본 닫힘 지연을 적용한 분리 검사가 전환을 통과했으므로 게임 코어를 변경하지 않았다.

최초19/41 결과는 같은 전략으로 모든 tick/EXP/RNG/저장값을 재현했다. 당시 실제 닫힘 지연을 사용하는 41맵 전체 전략은 **18클리어/23사망, exit1**이었다. 후속 selected-35 결과는 문서 상단에 별도로 기록했다. 분리된 Stage18 성공을 이 결과에 합산하지 않았다. 전체 미션 연속 성공과 원본 UI 동등성은 미검증이다. [증거와 전략별 한계](stage-journeys.md).

## 남은 8맵 원인별 후속 검사

기존 trace에서 selected-35의 Stage23은 낙사, 나머지7맵은 map.bottom 위에서 누적 피격으로 사망했다. 거리·홉·조준주기·접근·보스 처치 후 출구 입력을 좁혀21회 유효 검사를 실행했으나 모두 실패했다. 당시 새 성공이 없어27/35를 유지했고 전체35를 다시 실행하지 않았다. 이후 카메라 반영35맵 재검사는 위의23/35 결과로 구분한다. 초기 exitdash 하네스의 substring 분기 오류와 수정 후 별도 실패도 보존했다. [맵 여정의 조건·원인·증거](stage-journeys.md)를 따른다. 원본 규칙 불일치는 확인하지 못했고 게임 코어를 변경하지 않았다.

## 실제 게임 프레임의 렌더 차이

Stage01/28/35의 같은 native draw list를 실제 CGL과 CPU+CG에 각각 그렸다. 최대 RGB 차이2/2/3이며1초과 채널11/41/86이다. 전체156draw의 CPU출력→CG변환 차이0, 일부 단일 asset의GL차이2~3과 합성 누적 차이를 분리했다. 앱 부동소수점 flags를 맞춘 별도 실행에서도 결과 파일15개가 같았다. **픽셀 동등성 통과가 아니다.** [측정·첫 차이·원인 미확정 범위](scene-render-comparison.md)에 기록했다.

## nil 경계 실제 관측과 최신 일반 앱 UI 후속

[원본 ARM guest 관측](nil-parts-observation.md)은 두 번 실행했다. 첫12행은3개 nil 호출의 getter 전후 버퍼 보존을 확인했고, 추가15행은3개 호출의 root 누적 bounds(-20,0,20,40)를 실제로 읽었다. 이 표본에서는 현행 복원 결과와 일치했다. 기체 이름·자식 포함 최종 경계·모든 생성 경로·당시 Apple ARM32 OS는 미검증이다. 연결 인사를 수정한 관측 도구8개 검사가 통과했으며 제품 Core는 변경하지 않았다.

[최신 일반 앱 UI](normal-fidelity-ui-check.md)는 Air/iOS26.5의 Device Hub를 CUA로 조작했다. 기존 LV02 슬롯의 Stage03 이동·터치 사격·피격·일시정지·저장과 프로세스 종료 후 재실행을 확인했다. EXP304→311, HP256→198, 진행1/3·부품1/1/2·무기5종 유지와 재실행 전후 저장 파일 해시 일치를 기록했다. 최신 물리 기기 설치, 새 Stage03 클리어, 음향 청취와 원본 전체 화면 동등성은 포함하지 않는다.

## 이름별 기체 경계 수집 후속

[기체 생성 경계](named-machine-bounds.md): 현재 GameSession의 실제 생성 경로로720종을 각각 새 세션에서 생성해 경계 binary32를 수집했다. 오류0·Unsupported0이며 원본 데이터와 제품 Sources를 변경하지 않았다. 원본 이름별 최종값 관측은 Mac 잠금으로 미실행이다. 앞선 이름 없는 root3개와 이 목록을 임의로 매칭하지 않는다.

## HUD 처리 순서와 맵 여정 후속

검사기의 업그레이드 즉시 확인·위치 리셋·session tick 전 닫힘과 화면 밖 결과 버튼 호출을 수정했다. 실제 `HMGameHUD`와 테스트 모델을 비교한 iOS 검사3개가 통과했다. 제품 소스는 변경하지 않았다. 기존 카메라 반영 조건의 대조 실행은 이전 JSON/CSV 값을 모두 재현했다.

최신 카메라·HUD 순서35맵 결과는23클리어(다음맵18·미션메뉴요청5)·12사망, exit1이다. Stage11이 성공으로, Stage06이 실패로 바뀌었다. 최신 실패는Stage06/16/17/21/22/23/24/31/32/33/34/35다. [조건·실제 HUD 검사·재현 근거](stage-journeys.md)를 따른다. 한tick당draw1회 모델, 직접API입력, 메뉴요청까지의 관측과 연속캠페인 미검증이라는 제한은 남아 있다.
