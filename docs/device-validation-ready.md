# 실제 iPhone 검증 준비

2026-10-25 02:29 KST 갱신: 최신 검증 앱의 Xcode GUI 서명 빌드·서명 검사·실기기 설치를 완료했다. 오른쪽·정지·왼쪽 버튼, 연속 터치 사격, EXP310→315→322, 일시정지·재개를 실제 iPhone Air에서 확인했다. 설치 전 Documents를 백업했다. Game Performance 127.77초 기록의 주 화면 갱신은 초당59.71회, 최대 간격50.02ms였다. 대부분 정지 전투 구간이다. 손으로 기울이기·연속 이동과 사격의 성능·장시간 검사는 남아 있다. 현재 서명 앱은 `/Users/sokol/Library/Developer/Xcode/DerivedData/HeavyMachRestored-cweztugewwwviqfjypghnhdbyzfg/Build/Products/Debug-iphoneos/HeavyMachValidation.app`이다. CLI 서명은 `errSecInternalComponent`가 발생했으나 GUI 빌드는 성공했다. 아래 CLI 재빌드 절차가 성공한 것으로 확대하지 않는다.

연결 대상은 Air `00008150-000808EC36D8401C` 하나다. 검증 앱 ID는 `com.gelb2.heavymach.validation`이다. 일반 앱과 저장은 삭제하지 않는다.

저장소 루트에서 실행한다. 사용자가 Mac 로그인 키체인을 정상적으로 잠금 해제한 뒤 서명 빌드를 실행한다. 스크립트는 비밀번호를 받거나 보안 설정을 바꾸지 않는다.

```sh
python3 HeavyMach/tools/prepare_device_validation.py build
python3 HeavyMach/tools/prepare_device_validation.py --dry-run
```

`build`는 `HeavyMachValidation`을 `/tmp/heavymach-direction-device`에 다시 빌드한다. 서명 검사가 통과하면 `HeavyMach/build/direction-controls-20261024/iphoneos/HeavyMachValidation.app`에 보존한다. 기존 산출물도 실행 기록 폴더로 옮겨 보존한다. 기기 연결 없이 실행할 수 있다.

Air를 연결하고 잠금을 풀어 Mac을 신뢰한다. 개발자 모드가 켜져 있어야 한다. 다음 명령은 기기 종류·UDID, 앱 ID, ARM64, 실제 서명, 프로파일의 대상 기기·만료일을 확인한 뒤 설치한다.

```sh
python3 HeavyMach/tools/prepare_device_validation.py install
```

기존 검증 앱이 있으면 `Documents`를 먼저 Mac에 백업한다. 백업 실패나 설치 실패 시 후속 작업을 중단한다. 검증 앱이 없으면 신규 설치로 기록한다. 설치 성공 후에만 실행한다. 앱 삭제나 저장 초기화는 하지 않는다.

재설치 없이 실행하거나, 검사 후 저장을 복사할 수 있다.

```sh
python3 HeavyMach/tools/prepare_device_validation.py launch
python3 HeavyMach/tools/prepare_device_validation.py export
```

매 실행의 JSON·명령 로그·저장 백업은 `HeavyMach/build/device-validation-runs/<UTC시각>/`에 남는다. `summary.json`에는 실행 파일·Debug.dylib·프레임워크 등 Mach-O 코드별 SHA-256과 실제 성공한 설치·실행 여부가 들어간다. `--dry-run`은 읽기 검사만 한다. 미연결은 종료 코드 2, 검사 실패는 1이다. 자동 대기·재시도는 하지 않는다.

## 폰에서 검사

1. `검증` 패널에서 미션·무기·부품·EXP 조건을 정해 시작한다. 방향 조작부의 `← / 정지 / → / 기울기`는 이동 방식을 고른다. Air에서는 게임 화면 왼쪽 여백에 세로로 배치한다. 공간이 부족할 때만 화면 상단에 표시한다. 기본값은 기울기다. 방향 선택은 유지되며 `정지`로 멈춘다. 선택한 초기 조건과 실제 얻은 EXP·레벨업을 구분해 기록한다.
2. 같은 스테이지에서 좌우 이동, 터치 조준·연속 사격, 부스터를 각각 30초 이상 확인한다. 적과 효과가 많은 구간도 같은 조건으로 반복한다. 조작 반응·프레임 끊김·음향을 기록한다.
3. 메뉴를 열고 닫고, 홈 화면에 갔다가 돌아온다. 방향 입력이 남지 않는지 확인한다. 저장 후 앱을 다시 열어 `검증` 패널의 이어하기로 저장된 진행을 확인한다.
4. Mac의 Instruments에서 **실제 Air의 HM Validation 프로세스**를 선택한다. `Game Performance` 또는 `Metal System Trace`로 프레임 간격·긴 프레임을 기록하고, 별도 실행에서 `Game Memory` 또는 `Allocations`로 메모리와 스테이지 반복 시 증가량을 기록한다. 해당 템플릿들은 현재 Xcode에서 존재함을 확인했다. 실기기 수집 가능 여부와 수치는 연결 후 확인한다.
5. 기기·OS·실행 파일 SHA·시작 조건·검사 시간과 `.trace`를 같은 실행 기록 폴더에 보관한다. 시뮬레이터 결과를 실제 FPS·메모리 결과로 대신하지 않는다.

## 30초 성능 기록 명령

앱을 실행한 상태에서 저장소 루트에서 실행한다. `--attach`에는 앱 ID가 아닌 실행 프로세스 이름을 사용한다. 다음 옵션은 현재 `xcrun xctrace help record`로 확인했다. 실제 Air의 Game Performance 수집은 GUI에서 완료했다. 아래 CLI 연결은 당시 프로세스 탐색 실패가 있었으므로, 검증된 GUI 수집 결과와 구분한다. Allocations 수집 완료로 보고하지 않는다.

```sh
mkdir -p HeavyMach/build/device-validation-traces
xcrun xctrace record --device 00008150-000808EC36D8401C --template 'Game Performance' --attach HeavyMachValidation --time-limit 30s --output HeavyMach/build/device-validation-traces
xcrun xctrace record --device 00008150-000808EC36D8401C --template 'Allocations' --attach HeavyMachValidation --time-limit 30s --output HeavyMach/build/device-validation-traces
```

출력은 기존 `.trace`를 덮어쓰지 않고 디렉터리에 고유 이름으로 생성된다. 각 기록에서 같은 전투 조건을 재현한다. 수집 실패 시 오류를 보존하고 성공한 trace로 보고하지 않는다. `Game Performance`에 필요한 FPS 트랙이 나오지 않으면 Instruments의 `Metal System Trace`로 같은 프로세스를 조사한다.
