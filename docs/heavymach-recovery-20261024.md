# Heavy Mach 1.13 복구 · 2026-10-24

사용자가 확인한 GameDev 초기 기획의 참고 원본이다. 원본 소스는 확보하지 못했다. 보유 ARMv6 IPA를 no-JIT CPU 인터프리터 실행기로 구동하며 게임 바이너리·에셋은 수정하지 않는다.

## 작업 위치

- 기능 프로젝트: [iOS Proj/HeavyMachRecovery](../Compatibility/README.md)
- Xcode: `iOS Proj/HeavyMachRecovery/ios/HeavyMachRecovery.xcodeproj`
- Unity: [Unity 진입](https://github.com/gelb2/ProjectGameDevUnity). 실제 기존 소스는 `game/`에 보존.
- 복구 조사와 실행 증거: `/Users/sokol/Documents/Codex/2026-10-24/heavymach-recovery`
- 원본: `/Users/sokol/Documents/Codex/2026-10-24/rea-ready/Heavy_Mach_1.13_ios_2.1.ipa`
- 원본 SHA-256: `477c672f92110c6020be3f744dfa7a3456739e6f970c66715e3b7b3cd32b7776`

첫 기능 변경 인계의 조사·복구 증거와 현재 HEAD `678266df1679931d45c9639d757d6751b08dffe3`를 대조했다. 기존 Unity 기능을 revert/reset하지 않았다. 새 복구는 별도 기능 폴더와 원본 조사 폴더에서 수행했다.

## 확인 결과 · 진행 중

| 검사 | 실제 결과 | 근거 |
| --- | --- | --- |
| MCP 시작 | 최소 GUI PATH에서 node 누락 재현. global MCP env PATH 수정 후 initialize 및 tools/list 29개 성공 | `mcp/before-minimal-path.*`, `mcp/after.json`, `mcp/result.txt` |
| 현재 대화 MCP 재접속 | 별도 확인 안 함. 프로젝트의 enabled=false 기존 설정 보존 | global 설정과 세션 연결 구별 |
| 원본 무결성 | 인계의 IPA/실행 파일 해시와 일치 | `original-sha256.json` |
| 실행기 CPU 일반 경로 | 게임별 하드코딩 PC 우회 제거 회귀 2개 통과 | `cpu-tests.log` |
| 시뮬레이터 Xcode 빌드 | SDK 27.1 arm64 빌드 성공. 새 GameDev 경로에서도 성공 | `workspace-simulator-build.log` |
| iPhone Air 시뮬레이터 | iOS 26.5 타이틀→저장 슬롯→시나리오→Mission 1 Stage 1 진입 | 앱 화면·touchHLE_log.txt |
| iOS 27.1 시뮬레이터 | 공식 런타임 설치. 지원 타입 iPhone Duo에 앱 설치·launch 명령 성공. 플레이 미확인 | sim UDID `A7F1189A-6F0C-4D48-A554-1F02D396D647` |
| 실기기 ARM64 빌드·서명 | Xcode GUI 빌드 및 codesign strict 검증 성공 | Xcode Build Succeeded, 서명 검사 |
| iPhone Air 설치 | iOS 27.0.1 설치 성공. 사용자 개발자 프로필 신뢰 완료 | devicectl install |
| iPhone Air 실행 | 초기 UIScene 미채택 SIGTRAP 수정. 최종 수정본 재설치 후 타이틀·슬롯·시나리오·미션 선택 화면 및 프로세스 유지 확인 | 초기 `device-crash-112331.ips`, 수정 후 `device-final-console.log` |
| 한 스테이지 클리어·레벨업·재실행 저장 | 아직 미완료. 직접 플레이 검사 진행 중 | 완료 결과와 구분 |

시뮬레이터 창이 180도 반대 방향을 표시한 경우 앱 프레임 자체는 정상 방향이었다. Device Hub Rotate Left 두 번으로 표시 방향을 맞췄다. 코드의 iOS 방향 변경 시 SDL 방향 힌트 갱신 누락도 수정했다.

## 조작 기준

원작은 가속도 Y 입력으로 이동하고 화면의 오른쪽 먼 지점을 누르고 유지하면 조준·사격한다. 화면 회전과 기울기 센서 입력을 구별한다. 시뮬레이터 방향키 틸트 코드를 추가했다. 실제 함수의 매핑·해제 회귀 검사 2개가 통과했다(`simulator-keyboard-tilt-tests.log`). Device Hub의 Capture Keyboard를 켜고 방향키로 이동, 화면 누르기로 사격, 왼쪽 아래 부스터로 점프하는 동작을 직접 확인했다. Stage 1 첫 시도는 출구 근처에서 HP가 소진되어 MISSION FAIL이었다. 클리어·레벨업 성공으로 집계하지 않는다. 이후 사용자가 시뮬레이터를 종료하여 실기기 확인을 우선한다. 게임의 수치·EXP·세이브를 편집해 검증하지 않는다.

원본 분석에서 오른쪽 끝 도달 및 남은 보스 0이면 다음 스테이지로 전환한다. 일반 적 전멸은 필수 조건이 아니다. EXP 임계치에 도달하면 레벨과 업그레이드 점수가 증가하며 다음 스테이지 전환에서 `HvM.dat`를 저장한다. 이는 정적 분석이며 직접 플레이 성공 증거를 대신하지 않는다. 근거는 `title-analysis/gameplay-findings.txt`에 보존했다.

## 실행 오류 후속 수정

SDL에 UIScene 연결과 전경·배경 이벤트 처리를 추가했다. iOS 27에서는 UIApplication의 전역 방향 값이 Unknown이고 실제 UIWindowScene은 Landscape인 경우가 있어, 화면 크기 계산이 scene의 방향을 우선하도록 수정했다.

Xcode 증분 빌드에서 패키징한 실행 파일의 과거 수정 시각 때문에 서명 단계가 생략되는 실패를 설치 오류 `0xe800801c (No code signature found.)`로 확인했다. 패키징 직후 실행 파일 시각을 갱신하도록 수정하고 Clean Build로 재서명한다. 최종 설치·실행 결과는 아래에 추가한다.

최종 실기기 수정본: 11:43 설치 성공, codesign --verify --deep --strict 통과. iPhone Air의 PID 55753과 렌더 프레임 증가를 확인했고, Device Hub에서 타이틀·저장 슬롯·시나리오·미션 선택을 직접 터치했다. 원본 실패 로그는 보존했다.

11:45 실기기 Mission 1 Stage 1 진입과 화면 터치 사격, 메뉴 일시정지를 직접 확인했다. 기기 틸트는 원격으로 물리 조작할 수 없어 미검증이다. 아이폰은 첫 스테이지 일시정지 메뉴에 두었다. 사용자가 종료한 시뮬레이터는 다시 시작하지 않았다. 클리어·레벨업·저장 재로드 검증은 남아 있다.

## 사용자 실기기 플레이 확인

사용자가 설치된 iPhone 앱을 직접 조작한 뒤 다음 동작이 정상이라고 보고했다.

- 다음 미션으로 전환.
- 낙사 후 미션 실패 처리.
- 아이템 획득.
- 획득한 새 무기 사용.

이는 사용자 실기기 플레이 결과이며 에이전트의 직접 클리어 증거와 구별한다. 레벨업과 앱 재실행 후 저장 유지 여부는 아직 확인하지 않았다. 앞선 미완료 기록은 당시 상태이며, 미션 전환·실패·아이템·무기 기능의 현재 확인 범위는 이 사용자 보고로 갱신한다.

## 커밋 시점의 범위 정정

사용자가 재실행 후 저장 유지를 실기기에서 확인했다. 레벨업은 아직 보지 못했다고 보고했다. 따라서 저장 유지 검증은 완료, 레벨업은 미확인이다.

사용자 요청은 REA를 사용한 게임 핵심 코드 복원이다. 현재 구현은 원본 IPA를 호환 실행기로 구동하는 단계이며, 원본 게임 코드 복원 완료가 아니다. 기존 실행본은 후속 소스 복원의 동작 비교 기준으로 보존한다. Xcode에는 빌드용 Stub만 표시되며 실행기 소스는 compat-source에 있다. 원본 아이콘은 찾았지만 앱 아이콘 연결은 아직 적용하지 않았다. 코드 복원과 아이콘 적용은 후속 작업으로 남아 있다.
