# HeavyMach 1.13 source coverage audit

## 후속 치트 검증

사용자 요청으로 남은10개 맵을 치트 조건에서 검증해 **10/10클리어·보스9/9처치·저장후일반코어진입10/10**을 확인했다. 다음 맵8개·미션 메뉴 요청2개다. 무적·무한탄약과 낙하복귀1회를 사용했다. [조건·로그·한계](cheat-stage-check.md)를 따른다. 비치트25/35와 조건을 합산하지 않는다.

## 최신 확인 — HUD 이벤트 순서

[HUD 이벤트 순서](hud-event-order.md)를 원본과 맞췄다. 실제 iOS 수정 전3개 검사에서7개 assertion 실패를 확보했고, 수정 후 관련9/9·CTest17/17·일반 iPhone 서명 제외 빌드를 통과했다. 같은35맵 재검사는 **25클리어·10사망·0시간초과**다. 다음 맵 진입20개와 미션 메뉴 요청5개이며, 독립 초기 조건 검사다. 이전23/35 기록을 현재 결과와 합산하지 않는다. 원본 전체 동등성과 연속 캠페인 성공은 미검증이다.

실기기 FPS 급락은 사용자 보고이며 미해결이다. 후속 지시에 따라 성능 수정을 보류하고 기능 검증 결과를 마감한다. 재진입 HUD 표시 초기화 차이까지 후속 수정했고 관련iOS10/10·일반iPhone빌드를 통과했다. 최신근거는 `build/hud-persistence/manifest.json`이다. 아래의 이전 검증 기록은 각 당시 소스로 한정한다.

## 이전 전투·화면 전환 수명 수정

[무기 소진·시간제 효과](session-lifecycle.md): 새 탄약 재획득 삭제와 강화 효과 전환 소실을 수정했다. 최종CTest17/17, 관련ASan/UBSan3/3, 실제iOS GameView 연결1개, 일반iphoneos서명제외빌드가 통과했다. 같은35맵 재검사는23클리어·12사망이며 이전 JSON 결과와 CSV trace를 모두 재현했다. 물리기기·실제UI조작·연속캠페인·원본전체동등성 검사는 아니다. 아래 HP 막대 및 이전 증거는 각 당시 범위를 유지한다.


## 이전 HP 막대 수정

[원본 근거와 회귀 검사](gauge-registration.md): 적 막대의 fixedPos·독립 culling과 양쪽 막대의 tick 등록/draw 갱신을 복원했다. 등록 전 추락사망과 등록 후 피격사망, binary32 재귀 좌표 순서도 검사했다. 최종 CTest16/16, 관련 ASan/UBSan, 일반 iPhone·Simulator 서명 제외 빌드가 통과했다. 실제 CGL 비교3프레임은 최대 RGB 차이2/2/2이며 픽셀 일치는 실패다. 이번 수정 뒤 UI·실기기·35맵 전체 여정은 재실행하지 않았다. 아래 이전 증거는 각 실행 당시 소스로 한정한다.


## 이전 확인 — 렌더·음향·UI와 35개 맵 여정

[렌더 iOS 연결](sprite-native-integration.md): 원본 GL_REPLACE, 전체 텍스처 보간·POT 패딩·RGB565 정규화와 Trail UV를 복원했다. 480×320 프레임을 먼저 합성한 뒤 Air 화면에 확대한다. Mac 성능 실패를 근거로 두 raster 파일에만 O2를 적용했다. 후속 [음원 사전 준비](audio-preload.md)까지 포함한 최신 결과는 **CTest15/15, 실제 시뮬레이터 iOS 검사7/7(음향3+렌더4), 일반 iphoneos 서명 제외 빌드 통과**다. [최신 일반 앱 UI](normal-fidelity-ui-check.md)에서 Stage03 기본 전투·저장·재실행도 확인했다. 최신 물리 기기 추가 설치는 하지 않았다.

[최신 맵 여정](stage-journeys.md)은 앱의 카메라 흔들림과 HUD 처리 순서를 연결한35개 독립 시작에서 **23클리어·12사망·0시간초과, exit1**이다. 다음 맵 전환18개와 미션 메뉴 전환5개를 확인했다. 이전27/35는 카메라 미반영 조건이며 대조 검사에서 기존 JSON/CSV 값이 모두 재현됐다. 초기 등급6·원본 유한 탄약과 기존 조작 전략을 유지했다. 제품 소스는 변경하지 않았다. 연속 캠페인 성공과 전체 UIKit 타이밍 일치는 미검증이다.

기존 보스 전투 **48/48 처치**는 별도 전투 조건의 결과다. 보스 검사는 Trail UV/렌더 변경 전의 실행 증거다. 최신 여정은 현재 소스로 재실행했다. 함수 분석·선언 수나 개별 검사 통과 수를 전체 복원 완료율로 계산하지 않는다.

2026-10-24. 원본 SHA-256: `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`.
이 보고서는 현재 편집 가능한 코드와 원본 함수 목록을 대조한다. 코드 변경이나 테스트 재실행은 하지 않았다. 후속 CameraShake, atlas frame clamp, LoadingTransition, HMButtonState와 Air 시뮬레이터 실행 증거를 반영했다. 정확한 소스 해시와 함수별 분류는 [source-coverage.json](../Analysis/source-coverage.json)에 있다.

## 범위와 분모

| 목록 | 수 | 의미 |
|---|---:|---|
| 원본 파일에 있는 함수 본문 | 658 | 게임 코드 외에 SoundEngine, C++ 라이브러리, 컴파일러 보조 코드 포함 |
| 원본 파일에 있는 thunk | 186 | import/branch 전달 코드 |
| 분석기가 만든 ObjC thunk | 56 | 실제 파일 영역 밖의 합성 함수 |
| local 합계 | 900 | 위 세 종류 합계 |
| external import | 233 | 시스템·라이브러리 외부 심볼 |
| 전체 분석 목록 | 1133 | local + external |

900개 pseudocode와 844개 assembly export가 있다. 합성 thunk 56개에는 실제 원본 assembly가 없다. pseudocode export는 컴파일 가능한 구현을 뜻하지 않는다. `OriginalInterfaces`의 37개 클래스, 553개 메서드, 354개 ivar는 선언과 주소 매핑이며 구현체가 아니다. 이 수로 복원 완료율을 계산하지 않는다. 특히 658개를 모두 게임 기능이라고 부르지 않는다.

## 함수군 대조

`구현`은 실행 가능한 소스가 있다는 뜻이다. `연결`은 소비자 호출 경로를 확인했다는 뜻이다. 테스트가 있어도 원본의 모든 메서드·입력·장면과 같은 동작을 보증하지 않는다. JSON은 658개 본문을 25개 함수군에 모두 배정하고, 1133개 전체 주소의 분류와 선언 여부를 보존한다.

| 함수군 | 구현과 연결 | 확인 범위 및 남은 점 |
|---|---|---|
| Vector2 / Collider / ColVector | Geometry → Motion/World/Visuals/ItemBox | host 단위 검사. 모든 퇴화 입력은 미인증 |
| WeaponInfo / PartsInfo / XML | GameData 원본 필드·reader → Session | 실제 RawData 검사. ObjC accessor ABI는 선언만 보존 |
| 저장 / EXP / 인벤토리 | SaveData ARM32 disk layout, GlobalInfo → native 저장 | layout·진행 규칙 검사. Air 정상 레벨업·공격 강화 뒤 동일 슬롯 재실행 확인 |
| 기체 이동 / 부품 | Motion/Visuals → Session 부모·자식 그래프 | 지형·공중·booster·preview 검사. nil BaseParts struct-return 미확정 |
| 플레이어 | Player → Session → HUD/저장 이벤트 | 피격·아이템·EXP·입력 검사; 검증 조건의 18회 실제 강화·외형·저장 복원 확인 |
| 적 | Enemy → Session → World | 정의·AI·피격·드롭·EXP 검사; 최신 v6 단일48/48 지정 보스 처치. 초기 조건·유한 탄약·고정 입력 사용; 모든 미션 여정·원본 동등성은 미인증 |
| 무기 / 탄환 | Combat/BulletVisual → Session | 발사·reload·AI·effect 1..5 및 무기20종 검사. 화면 밖 총알 draw/RNG 회귀 수정 전 실패·수정 후 통과. 원본 출력 비교 미완 |
| 파티클 / trail / item | Visuals/Trail/TrailRaster/ItemBox → Session → HMMedia | 수명·bounce·mesh·raster·pickup 검사. 원본 GL fixture RGB 차이0. 전체 장면 비교는 미완 |
| 월드 | World → Session | 맵·카메라 follow·regen·경계·결과·survival 검사. 전체 stage/mission 여정 미인증 |
| 실제 게임 루프 | GameSession → HMGameView | 실제 자산 stage loop, pause/release, 무기·trail, 적 처치 EXP, HP0 검사 |
| 타이틀 / 메뉴 / HUD | TitleState/HMMenus/HMGameHUD → HMGameView | 타이틀 조건부 RNG·preview 생성시점·보존된 World 시간/pause 복원. 전체 UI 경로 실검증 미완 |
| 카메라 흔들림 | CameraShake → title/player/world 이벤트 → HMMedia raster offset | host 회귀와 UIKit 분수 offset 검사 통과. 최신 전체 화면 직접 비교는 미완 |
| 화면 전환 | native 화면 선택·시작·결과 경로 | LoadingTransition 원본 상태0..6 구현·native 연결; Air 패널 닫힘·열림 확인 |
| QobButton | native hit/action 경로 | HMButtonState 시간·눌림·release 변위 구현, native 메뉴/HUD 연결; 전체 원본 입력 비교는 남음 |
| QobBase / 그룹 / 글꼴 | native 상태·stable layer·sprite·숫자 glyph로 부분 대체 | 일반 receiver 재진입·전체 text/font 메서드는 별도 구현되지 않음 |
| 그래픽 / 음향 / 앱 | CoreGraphics/AVAudioPlayer/UIScene/CoreMotion | 단일 음원 재시작·거리 gain 복원. 전체 blending·음향 출력 동등성 미인증 |

GameSession의 최근 ASan/UBSan 검사에서는 실제 RawData를 사용한 stage 1 loop, stage 2 regen, pause 중 release 후 발사 중단, RaderLsr trail, 적 처치와 EXP, 적 공격에 의한 HP0를 확인했다. 처치/피격 검사는 적 위치만 바꾼 테스트 fixture이며 실제 플레이 난이도 검증은 아니다. 각각의 자세한 계약과 증거는 `core-*.md` 및 해당 `Tests/*.cpp`에 있다. 이번 audit는 테스트 결과를 새로 만든 것으로 보고하지 않는다.

## 구현된 후속 계약과 남은 작업

- LoadingTransition은 원본 GameMain 상태0..6, 닫기/열기 효과음과 동기 화면 교체를 구현했다. Air 시뮬레이터의 타이틀→슬롯→모드→미션→게임 전환을 확인했다. HMButtonState의 눌림·release 시간/변위도 메뉴/HUD에 연결했다.
- TitleState는 0xf464의 조건에 따라 random()을 0/1/2회 호출한다. 슬롯 preview의 weapon 생성 RNG는 실제 showSlots 시점에 소비하며, 원본의 숨겨진 World 시간과 pause를 유지한다. [타이틀 근거](core-title.md). 전체 Core child/render 순서와 난수 stream 동등성은 별도 비교 대상이다.
- 메뉴103은 원본 문자열 `http://www.indiean.com`으로 현대 openURL을 호출한다. 목적지 미구현 상태는 해소됐다. 실제 웹 접속·현재 사이트 응답은 검사하지 않았다.
- HMMedia는 이름당 단일 voice를 준비상태를 유지하며 위치0으로 재시작하고, 원본 source 위치에 따른 거리 gain과 로드 순서별 maxDistance를 구현했다. [음향 근거](audio-fidelity.md). AVAudioPlayer stereo pan 곡선은 여전히 근사다. 구형 원본과 실제 출력·latency·합성을 비교해야 한다. playBGM0x13d2c의 직접 게임 caller는 미확정이며 API 존재만으로 음악 누락을 단정하지 않는다.

1. **P1 — 남은 native 여정.** 전체 mission 연속 진행, 물리 기기 성장·입력·생명주기·저장 검증. 보스48개 처치와 서바이벌 실패/복귀의 확인 범위는 아래에 기록했다. 이미 확인한 Air 정상 레벨업/재실행과 검증 조건의 성공 범위를 아래에 분리했다. 모든 조건을 통과했다고 확대하지 않는다.
2. **P2 — 원본 출력 비교.** 전체 장면과 모든 UI 입력 시간, stereo 공간 배치·음향 출력·latency 및 같은 조건의 영상/수치 비교.
3. **P3 — 미확정 경계.** nil BaseParts struct-return, 전체 게임의 RNG 소비·child/render 순서. 확인하지 않은 동작을 추정값으로 채워 완료 처리하지 않는다.

별도 attract/demo 모드는 조사한 원본에서 확인하지 못했다. `GobHvMach::processModel`의 메뉴 기체 preview는 구현되어 HMMenus가 사용한다. 근거 없이 “demo 미구현”을 추가하지 않는다. 기존 `core-player.md`의 level 66 예외 관련 설명은 후속 VFP 변환 포화 복원 이전 기록이다. 현재 GlobalInfo와 `Tests/HighLevelEvidence.md`를 우선한다.

## 플랫폼 코드 제외 기준

233개 외부 import, 242개 thunk와 ARM switch/entry 보조 함수는 그 개수만큼 게임 기능을 의미하지 않는다. 구형 EAGL/OpenAL·Objective-C 소유권 처리·컴파일러/STL 내부는 현대 UIKit/UIScene/CoreGraphics/AVAudioPlayer 및 C++ 런타임으로 대체할 수 있다. 원본 주소와 evidence는 삭제하지 않는다. 입력 좌표, 시간, 저장 format, 자산, 음향·그리기 결과 등 게임이 관찰하는 계약은 계속 검증한다.

QobCamera 흔들림, QobButton 시간 동작, GameMain loading 전환은 게임 고유 동작이므로 이 제외 기준에 넣지 않는다. 현재 CameraShake, loading 전환, HMButtonState는 구현·연결됐다. 원본과의 전체 시각·입력 동등성 검증은 별도 범위다. 헤더 선언이나 platform shim만으로 해당 기능을 복원했다고 집계하지 않는다.

## 후속 실행 증거 갱신

[verification.md](verification.md)와 [validation-mode.md](validation-mode.md)의 실행 기록을 재사용한다. 아래 Air 결과는 **iPhone Air iOS26.5 시뮬레이터** 증거이며 물리 기기 결과로 확대하지 않는다. 이번 문서 갱신에서 빌드나 테스트를 다시 실행하지 않았다.

| 조건 | 확인한 결과 | 한계 |
|---|---|---|
| 일반 앱 정상 전투 | Stage1/2 클리어, EXP304/internal level1, 공격 강화·weapon grade1. 종료·재실행 후 **동일 슬롯 Stage3**에서 LV02/공격02/EXP304 유지, 이동·사격 | 모든 미션·보스 성공을 뜻하지 않음. `air-native-levelup-relaunch.log` |
| 별도 검증 앱 레벨업 | EXP299 조건→실제 addExperience(1)→LV02/UPGRADE→이동 강화·차체 변경→저장→종료·재실행→조건 재주입 없는 이어하기 | 일반 난이도 획득 검사와 구분. 일반/검증 Documents 분리 |
| 강화 자동 검사 | 이동·방어·공격 각각0..5→다음 등급, 총18회 실제 레벨업·강화·외형 tile·저장 encode/decode·일반 start 복원. 이동 tick, HP, 충돌 피해, 등급6 선택 거절 확인 | host 검사이며 모든 native 화면 직접 확인과 다름 |
| 무기 자동 검사 | 원본20종 선택·발사·효과·저장 복원 후 재선택·재사격. 19종 탄약 감소, 기본 Gun01 무제한99 유지; trail8종 | 초기 장착 index 보존 또는 실제 원본 음향/영상 동등성을 뜻하지 않음 |
| 첫 보스 native 검증 | body0/top6/weapon0·Canon03 조건으로 MBoss01 실제 처치→Stage3 clear→Stage4→저장·종료·재실행→조건 재주입 없이 Stage4/EXP215 재개. Canon01 재선택·사격10→9 | 한 보스·해당 fixture 성공. 전체48보스 통과가 아님 |
| 맵·보스 사전 검사 | 원본41맵 각3tick, 48개 보스 배치의 접근 지형 계산 | 로드/지형 확인을 모든 맵 클리어·보스 처치로 세지 않음 |

[보스 v5 기록](boss-battles-v5.md)은 단일 실행 **33/48 처치, 플레이어 사망14, 접근 미완료1**이다. Stage25의 이전 보스를 실제로 처치하고 같은 세션에서 두 번째 보스까지 전진한 별도 순차 검사가 접근 미완료1개를 해소했다. 당시 합친 확인 범위는 **34/48**, 남은14개는 미통과였다. 전체48개를 한 실행에서 통과한 결과가 아니며 보스 처치를 stage/mission 전체 클리어와 동일시하지 않는다. v1의8 killed/40 player_died 등 이전 실패 기록도 유지한다. 이 전투 기록을 최신 총알 수정 뒤 전체 보스 재실행 결과로 주장하지 않는다.

[Enemy 독립 대조](core-enemy.md)는 남은14개 배치의 AI 유형, 원본 정의, HP·피해·파괴 및 무기 AI 호출 경계를 확인했다. 이 범위에서 수정할 구체적 차이를 찾지 못했고 기존 Enemy 회귀가 ASan/UBSan exit0이었다. 자동 조작 패배를 곧바로 게임 코드 결함으로 판단하지 않는다. 모든 무기 분기·충돌·원본 실기기 동시 실행까지 검증한 결과는 아니다.

후속 fidelity Core CTest9개 통과 기록은 `build/fidelity-core-ctest.log`에 있다. 회귀검사 통과와 별도 보스 전투 runner 결과를 합산하지 않는다. 검증용 HM_VALIDATION fixture/API는 원본에 없는 검사 도구이며 원본 함수 복원 수에 더하지 않는다.

## 화면 밖 총알 draw 후속 수정

`GobBullet::isInCam`0xab00은 visual 등록에 항상 true를 반환한다. 충돌 tick0xa444의 **super** isInCam 검사와 별개다. GameSession collect에서 잘못 적용한 화면 영역 draw 조건을 제거하고 show/remove/info 조건을 유지했다. 화면 밖 Smoke의 파티클·난수4개, Flame 반전, Trail/Laser 위치 기록이 원본 draw 경로대로 계속 실행된다. 화면 clip과 충돌 판정은 유지한다. [원본·회귀 근거](core-bullet-visual.md).

분리된 테스트 데이터와 실제 Session 발사/tick/collect 경로에서 수정 전 화면 밖 본체 assertion 실패를 재현했다. 수정 후 같은 회귀가 Debug 및 ASan/UBSan에서 통과했다. `build/offscreen-bullet-ctest.log`에서 Core CTest9개 통과, `build/offscreen-bullet-simulator-build.log`에서 simulator BUILD SUCCEEDED를 확인했다. 이 문서 갱신에서는 검사나 앱 조작을 다시 실행하지 않았다. 전체 RNG stream 동등성·모든 보스 통과·원본 영상/음향 동등성 인증으로 확대하지 않는다.

## 서바이벌·카메라·HP 막대 후속

[서바이벌 직접 검사](survival-native-check.md)에서 원본·복원 모드 진입, 복원 사격/피격/일시정지/HP0 후 슬롯 복귀와 EXP306 저장을 확인했다. [누적 시간 수정](survival-audit.md)은 게임→메뉴→게임에서 원본 World 시간을 유지하며, 별도 SurvivalTests는 스폰 RNG 분기·동기 생성 소비·seed 하한·pause 및 실제 적 공격에 의한 실패/복귀와 진행 저장값 보존을 검사한다.

[플레이어 HP 막대](player-gauge.md) 누락을 원본 자식 이미지 생성과 연결해 복원했다. 수정 앱에서 노란 막대 표시를 직접 확인했다. [카메라 연결](camera-integration.md)은 quake를 화면 판정·AI gate·충돌 대상 gate·터치 기준점·고정 지도 이미지에 반영한다. 실제 월드 충돌 좌표와 아이템의 이전 tick 가시성 사용 순서를 보존한다.

최종 CTest12개 통과(`build/camera-integration-ctest.log`), simulator 및 일반 iphoneos 서명 제외 빌드 성공. 물리 Air에는 추가 설치하지 않았다. 이 당시 일반 앱 UI 재검사는 남아 있었다. 후속 최신 기본 UI 검사는 아래 기록과 구분한다.

[nil BaseParts 경계](nil-parts-boundary.md)는 정상 enemy 데이터에서도 발생한다. 원본 caller/ARM32 런타임의 반환 버퍼 미초기화를 확인했지만 준비 당시 실행 중 stack값은 미관측이었다. 후속 실행값은 아래 기록에 있다. 원본이 항상0을 반환한다고 확정하지 않는다.

## 이전 보스·맵 여정 검사

[보스 v6](boss-battles-v6.md)의 최신 코어 단일 실행에서 **48/48 지정 보스 처치, exit0**을 확인했다. 초기 부품/무기 조건과 전투 전에 정한 입력을 사용했다. 전투 중 무적·탄약 보충·적 삭제는 없다. 첫 전체47/48과 Stage16 사망 기록도 보존했다. `build/boss-v6/hm-boss-v6-full2-final.jsonl` 및 실행 전후 동일 해시가 근거다. 모든 스테이지의 출구·연속 캠페인·원본 출력 동등성을 뜻하지 않는다.

[맵 여정](stage-journeys.md)은 원본 왼쪽 시작점부터41개 맵을 각각 진행했다. 최초 전체 결과는 **19클리어·21사망·1시간초과, exit1**이다. 클리어19개는 다음 스테이지15개와 미션 메뉴4개 전환을 포함한다. 실패22개는 성공 여정 미검증으로 유지한다. 초기 등급6/원본 무기 지급을 사용하며 실행 중 회복·보충·워프는 없다. `build/stage-journeys/`에 /tmp 원본 결과와 SHA256 목록을 보존했다.

41개 중 원본 일반 진행으로 도달하는 맵은35개다. 최초 여정 검사의 일반 진행35개는19클리어·15사망·1시간초과다. 나머지6개는 데이터에 있지만 일반 nextStage가 연결하지 않는 mission index7의 직접 지정 검사이며 모두 사망했다.

## RGB565·빌드 구성 후속

알파 채널이 없는 원본 이미지16개의 RGB565 색 변환을 이미지 로딩 경로에 복원했다. 원본 RGBA679개는 새 변환을 건너뛴다. 695개 원본/파생 형식·해시는 `Resources/texture-formats.json`에 있다. 원본 및 파생 이미지 파일은 수정하지 않았다. [렌더 검토](render-fidelity-audit.md).

변환65536개 값의 검사와 독립 이미지 방향·채널·버퍼 수명 검사(ASan/UBSan)가 통과했다. 전체 CTest는 **13개 통과**(`build/texture-format-ctest.log`), simulator 검증 앱과 일반 iphoneos 서명 제외 빌드도 성공했다(`build/texture-format-simulator-build.log`, `build/texture-format-device-unsigned-build-retry.log`). 이 시점에 남았던 UIKit 보간/atlas 차이는 후속 [렌더 연결](sprite-native-integration.md)에서 수정·표본 검증했다. 전체 장면의 원본 출력 일치는 여전히 미검증이다. 실기기 추가 설치와 최신 UI 비교는 하지 않았다.

xcodegen 재생성 뒤 일반 앱 scheme이 없어 첫 iphoneos 빌드는65로 실패했다. 실패 로그 `build/texture-format-device-unsigned-build.log`를 보존했다. `iOS/project.yml`에 일반/검증 scheme을 명시하고 재생성해 두 target 분리를 확인한 뒤 재빌드했다. 일반 앱에는 HM_VALIDATION=0, 검증 앱에는1을 유지한다.

## 맵 여정 후속 원인 구분

Stage18의 첫 정지는 뒤에 남은 보스를 제외한 자동 조작과 원본 보스 이동 경계가 만든 결과였다. 후속 전환 정지는 하네스의 즉시 업그레이드 닫힘 호출에서 발생했다. 원본 닫힘 지연을 적용한 분리 검사가 전환을 통과했으므로 게임 코어를 변경하지 않았다.

최초19/41 결과는 같은 전략으로 모든 tick/EXP/RNG/저장값을 재현했다. 당시 실제 닫힘 지연을 사용하는 41맵 전체 전략은 **18클리어/23사망, exit1**이었다. 후속 selected-35 결과는 문서 상단에 별도로 기록했다. 분리된 Stage18 성공을 이 결과에 합산하지 않았다. 전체 미션 연속 성공과 원본 UI 동등성은 미검증이다. [증거와 전략별 한계](stage-journeys.md).

## blend 활성 경로 후속 조사

[원본 호출·자산 대조](blend-reachability.md)에서 setter25호출은 모두0/2, Trail 생성은2로 고정됐다. 원본 reader에는 blend 입력 필드가 없고 map의1/2는 layer 선택이다. Trail30무기record가 참조하는3이미지는 모두RGBA다. 따라서 미지원 blend1·상태상속 분기를 현재 게임의 누락 기능으로 단정하지 않는다. 범용 renderer의 임의 입력 지원과 전체 dynamic dispatch 도달성 증명은 별개다.

## 실제 게임 프레임 렌더 후속

작은CGLfixture0~1byte와 별도로 실제 이미지가 겹친3프레임은 최대 RGB 차이2/2/3이었다. source/CG/각draw/prefix를 나눠 차이를 추적했으며156draw의CPU→CG변환은RGB차이0이었다. 일부 단일 이미지의GL대비2~3차이는남아있다. 실제앱의부동소수점flags를맞춰도출력동일이었다. 전체픽셀동등성통과로집계하지않는다. [원본조건·측정·정확한한계](scene-render-comparison.md).

## 음원 준비·전이 계약 최신 통합

원본31음원의초기요청목록·순서를복원해HMMedia생성시decode/prepare한다. 같은voice는준비를유지하며재생위치0으로돌아간다. 실제iOS음향3+렌더4=7개,CTest15개,일반iphoneos서명제외빌드가통과했다. [수정전실패·수정후증거](audio-preload.md).

일반35맵과직접6맵의Motion/World/GlobalInfo출구·전이조건820개와sanitizer를통과했다. [전이계약](stage-transition-contract.md)은전투·연속캠페인성공으로세지않는다. [렌더진단](texture-sampling-diagnosis.md)은실제GL의UV보간·샘플/출력양자화영향을분리했으나정확한내부정밀도는미확정이다.

## nil 경계 실제 관측과 최신 일반 앱 UI 후속

[원본 ARM guest 관측](nil-parts-observation.md)은 두 번 실행했다. 첫12행은3개 nil 호출의 getter 전후 버퍼 보존을 확인했고, 추가15행은3개 호출의 root 누적 bounds(-20,0,20,40)를 실제로 읽었다. 이 표본에서는 현행 복원 결과와 일치했다. 기체 이름·자식 포함 최종 경계·모든 생성 경로·당시 Apple ARM32 OS는 미검증이다. 연결 인사를 수정한 관측 도구8개 검사가 통과했으며 제품 Core는 변경하지 않았다.

[최신 일반 앱 UI](normal-fidelity-ui-check.md)는 Air/iOS26.5의 Device Hub를 CUA로 조작했다. 기존 LV02 슬롯의 Stage03 이동·터치 사격·피격·일시정지·저장과 프로세스 종료 후 재실행을 확인했다. EXP304→311, HP256→198, 진행1/3·부품1/1/2·무기5종 유지와 재실행 전후 저장 파일 해시 일치를 기록했다. 최신 물리 기기 설치, 새 Stage03 클리어, 음향 청취와 원본 전체 화면 동등성은 포함하지 않는다.

## 이름별 기체 경계 수집 후속

[기체 생성 경계](named-machine-bounds.md): 현재 GameSession의 실제 생성 경로로720종을 각각 새 세션에서 생성해 경계 binary32를 수집했다. 오류0·Unsupported0이며 원본 데이터와 제품 Sources를 변경하지 않았다. 원본 이름별 최종값 관측은 Mac 잠금으로 미실행이다. 앞선 이름 없는 root3개와 이 목록을 임의로 매칭하지 않는다.

## HUD 처리 순서와 맵 여정 후속

검사기의 업그레이드 즉시 확인·위치 리셋·session tick 전 닫힘과 화면 밖 결과 버튼 호출을 수정했다. 실제 `HMGameHUD`와 테스트 모델을 비교한 iOS 검사3개가 통과했다. 제품 소스는 변경하지 않았다. 기존 카메라 반영 조건의 대조 실행은 이전 JSON/CSV 값을 모두 재현했다.

최신 카메라·HUD 순서35맵 결과는23클리어(다음맵18·미션메뉴요청5)·12사망, exit1이다. Stage11이 성공으로, Stage06이 실패로 바뀌었다. 최신 실패는Stage06/16/17/21/22/23/24/31/32/33/34/35다. 결과를 서로 합쳐 전체 성공으로 세지 않는다. [조건·실제 HUD 검사·재현 근거](stage-journeys.md)를 따른다. 한tick당draw1회 모델, 직접API입력, 메뉴요청까지의 관측과 연속캠페인 미검증이라는 제한은 남아 있다.
