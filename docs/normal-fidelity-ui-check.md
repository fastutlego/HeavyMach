# 최신 일반 빌드의 Air 시뮬레이터 UI 검사

2026-10-24 15:04–15:11 KST. 물리 iPhone을 사용하지 않았다. 최신 음원 사전 준비와 소프트웨어 렌더가 포함된 일반 `HeavyMachRestored` 빌드로 검사했다. HM_VALIDATION=0이며 저장·HP·탄약 조건을 주입하지 않았다.

## 빌드와 설치

- Air/iOS26.5: `97CA5646-36BB-47A9-919A-D78DD632A439`.
- 실행 파일 SHA256: `2e8fb33e27ff281134fa7b7f8e7e53a38f1cca85ad39347e2315cda6f398a1e0`.
- 빌드: `build/normal-fidelity-simulator-build.log`, source/산출물 해시: `build/normal-fidelity-simulator-manifest.json`.
- 설치 전 Documents를 `build/normal-fidelity-simulator-save-before/`로 백업했다. 설치 직후 HvM.dat 해시가 같음을 확인했다.
- CUA의 `com.apple.dt.Devices`(Xcode27 Device Hub)로 시뮬레이터 UI를 조작했다. 예전 Simulator.app 경로를 찾지 못한 상태는 UI 제어 불가를 뜻하지 않았다. 이 경로에서 창과 키보드 입력을 확인했다.

## 직접 확인한 화면과 동작

1. 홈 화면의 원본 게임 아이콘, 타이틀, 슬롯 선택, 모드 선택, 미션 선택을 확인했다.
2. 기존 첫 슬롯은 LV02, MOV01/DEF01/ATK02였다. Scenario의 Mission01 Stage03을 선택했다.
3. 오른쪽 키 2.5초와 4초 입력으로 경사로를 내려가고 카메라가 이동하는 화면을 확인했다. 손으로 기울이는 검사는 아니다.
4. 게임 영역의 2초·5초 터치 입력 뒤 탄환, 적, 체력 막대가 표시됐다. 뒤 화면에서 가까운 적이 사라졌고 저장 EXP가304→311로 증가했다. 플레이어 HP는256→198이었다.
5. MENU → Save & Exit Game으로 슬롯에 복귀했다. 마지막 접근 시간이15:07:47로 갱신됐다.
6. 프로세스를 종료하고 홈 아이콘으로 다시 실행했다. LV02와 MOV01/DEF01/ATK02, 같은 마지막 접근 시간의 슬롯을 다시 확인했다. 저장 파일 SHA256도 종료 전과 같았다.

관찰한 게임 화면의 480×320 영역과 상·하단 HUD는 화면 안에 표시됐으며 양옆에는 검은 여백이 있었다. 스크린샷은 CUA 대화 결과에서 검토했다. 파일로 저장한 연속 영상이나 프레임 계측은 없다.

## 저장 증거와 한계

`build/normal-fidelity-simulator-ui-save.json`에 전후 해시와3개 슬롯의 읽기 전용 해석을 보존했다. 전체 파일은3624바이트다.

| 항목 | 설치 전 | 플레이 후 및 재실행 |
|---|---:|---:|
| EXP |304|311|
| 표시 레벨 |2|2|
| MOV/DEF/ATK 표시 등급 |1/1/2|1/1/2|
| Mission/Stage 표시 |1/3|1/3|
| 무기 수 |5|5|
| HP |256|198|

플레이 후 SHA256은 `c0cc13656c62761df98716f3f971a5487b9c3e57872699317e911061d400db5c`이며 cold restart 후 동일했다. 추가 두 슬롯은 비어 있었다. 시작 전 저장을 복사해 되돌리지 않았다.

이번 검사는 최신 일반 앱의 기본 동작 확인이다. Stage03 클리어, 새 레벨업, 모든 무기 조합, 물리 기기 성능, 음향을 실제로 들은 결과, 원본과 동일 시점의 전체 화면 비교를 뜻하지 않는다. 시작 로그의 CoreMotion plist 없음은 시뮬레이터 시스템 로그로 보존했다. 기존 실제 AVAudioPlayer decode/prepare 검사는 [음향 검사](audio-preload.md)와 구분한다.
