# 서바이벌 시뮬레이터 직접 검사

2026-10-24, iPhone Air iOS26.5 시뮬레이터. 실기기는 사용하지 않았다.

동일 저장 비교에 쓴 LV02, 이동06·방어07·공격06 저장으로 원본 비교 앱과 복원 검증 앱에서 각각 슬롯→모드→SURVIVAL MODE→OK를 눌렀다. 검증 패널로 모드를 강제하지 않았다.

원본 실행본에서 전장 배경·지면·기체·HUD·적 생성, 3초 터치 사격, MENU 일시정지 화면을 직접 관측했다. `build/original-survival-pause.png`는 원본 CUA 화면이다. 원본은 해당 일시정지 상태로 보존했다.

복원 앱도 같은 전장과 모드로 진입했다. 터치 사격 뒤 EXP300→306, 적 생성·피격, MENU 일시정지·RESUME을 확인했다. 이후 조작하지 않고 적 공격을 받아 HP0/gameState3/paused1이 된 것을 로그에서 확인했다. CUA에서는 자동으로 돌아온 슬롯 화면과 LV02·강화 표시를 확인했다. 실패 문구 애니메이션 자체의 순간 화면은 캡처하지 못했다.

저장 파일을 읽어 EXP306/internal level1/weapon5·top6·body5, 원래 mission0/stage0 유지를 확인했다. 별도 시나리오 진행으로 덮이지 않았다. 저장3624bytes SHA256 `1a0ed43a8e5ef868a57b63eeddda1c950eb43ab5081da7c4ac4ffafeb2e1f6e4`.

## 새로 찾은 차이

원본에는 기체 머리 위 노란 HP 막대가 있으나 복원 앱에는 없었다. 원본 생성0x1bb84와 갱신0x1d278을 대조해 실제 누락을 확인했다. [HP 막대 복원](player-gauge.md)에서 수정·검사를 별도로 기록한다. 이 문서의 최초 전장 캡처는 수정 전 화면이다.

World 누적 시간이 모드 재진입 때 초기화되는 차이도 정적 분석·회귀 검사에서 발견했다. [시계·서바이벌 근거](survival-audit.md)를 따른다. 최초 앱 실행의 서바이벌 진입 관측만으로 그 재진입 차이가 해결됐다고 판단하지 않았다.

## 증거와 한계

- `build/native-survival-console.log`: 실제 mode 진입 mission4/stage4, EXP·HP·적·탄환·실패 상태. t186~189에서 HP0/gameState3.
- `build/native-survival-pause.png`, `build/native-survival-return-slot.png`: CUA 직접 화면.
- `build/native-survival-save-after-failure.dat/.json`: 종료 이후 저장 원본과 읽은 값.

두 실행의 난수 seed·입력 시간·적 구성이 같지 않다. 이 결과는 서바이벌 진입과 생명주기의 직접 검사이며 모든 웨이브·보스·화면/음향 동등성 검사가 아니다.

## HP 막대 수정 후 확인

HP 막대와 누적 시간 수정 후 CTest11개, 시뮬레이터 검증 앱 빌드, 일반 앱 iphoneos ARM64 서명 제외 빌드가 성공했다. Air 시뮬레이터에 수정 앱을 설치·종료 후 실행했다. 검증 패널의 ‘저장 이어하기(초기 조건 적용 안 함)’로 재진입해 EXP306/LV02를 유지하고 머리 위 노란 HP 막대가 실제 화면에 표시되는 것을 CUA에서 확인했다. `build/gauge-quake-native-console.log`에 이어하기와 후속 상태를 남겼다. 물리 iPhone 설치·서명 검사로 확대하지 않는다.

이후 추가 화면 저장 시 CUA가 `noWindowsAvailable`, 재연결에서 `cgWindowNotFound`를 반환했다. 수정 후 HP 막대는 앞선 CUA tool screenshot에서 직접 확인했으나 별도 PNG 파일 저장은 실패했다. 기존 수정 전 비교 PNG는 보존했다. 실패 원인을 기기나 앱 crash로 단정하지 않는다. 이후 UI 검사는 창이 다시 제공될 때 재개한다.
