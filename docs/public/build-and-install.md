# HeavyMach 빌드와 설치

HeavyMach 1.13 복원 소스로 일반 앱과 치트 검증 앱을 빌드하는 방법이다. 두 앱은 같은 게임 코어를 쓰며 별도 앱 ID와 저장 공간을 사용한다.

## 준비물

Apple Silicon Mac, iOS SDK와 Simulator 런타임을 설치한 Xcode, Python 3, CMake 3.20 이상, Git LFS가 필요하다. Xcode 프로젝트는 저장소에 포함돼 있다. `iOS/project.yml`을 변경할 때만 XcodeGen으로 다시 생성한다.

2026년 10월 26일 작성 환경은 macOS 27.0.1, Xcode 27.1, CMake 4.4.4, Python 3.14.6, XcodeGen 2.45.4다. 앱의 deployment target은 iOS 15.0이다. 현재 OpenAL 빌드 스크립트는 arm64를 대상으로 한다.

## 소스와 자원 받기

```sh
git clone https://github.com/gelb2/HeavyMach.git
cd HeavyMach
git lfs install --local
git lfs pull
```

이미지·음원·맵·앱 아이콘·이미지 비교 자료는 저장소에 포함돼 있다. 원본 IPA, 포팅판, 기존 Mac의 자원 폴더는 필요하지 않다.

현재 저장소는 비공개이므로 접근 가능한 GitHub 계정으로 인증한다. `v0.9`는 자원을 Git에 넣기 전의 제품 코드 태그다. 새로 빌드할 때는 자원이 포함된 현재 `main`을 사용한다. GitHub Releases에는 설치 파일을 등록하지 않았다.

## 자원 검사

```sh
python3 tools/verify_build_resources.py
```

빌드·검사 자원 1,475개의 크기와 SHA-256을 확인한다. 누락·손상·다운로드되지 않은 LFS 파일이 있으면 종료 코드 1을 반환한다. LFS 포인터가 남았다면 `git lfs pull`을 실행하고 다시 검사한다.

## 시뮬레이터 빌드

저장소 루트에서 음향 라이브러리를 먼저 준비한다. 이어 일반 앱을 빌드한다.

```sh
bash tools/build_openal.sh iphonesimulator
xcodebuild -project iOS/HeavyMachRestored.xcodeproj \
  -scheme HeavyMachRestored -configuration Debug \
  -sdk iphonesimulator -destination 'generic/platform=iOS Simulator' \
  -derivedDataPath build/ios-simulator \
  ARCHS=arm64 ONLY_ACTIVE_ARCH=YES CODE_SIGNING_ALLOWED=NO build
```

산출물은 `build/ios-simulator/Build/Products/Debug-iphonesimulator/HeavyMachRestored.app`이다. 치트 앱은 같은 명령에서 scheme을 `HeavyMachValidation`으로 바꾼다. 산출물 이름도 `HeavyMachValidation.app`으로 바뀐다.

Simulator에서 iPhone 한 대를 부팅한 뒤 설치하고 실행한다.

```sh
xcrun simctl install booted build/ios-simulator/Build/Products/Debug-iphonesimulator/HeavyMachRestored.app
xcrun simctl launch booted com.gelb2.heavymach.restored
```

여러 Simulator가 부팅돼 있으면 `xcrun simctl list devices booted`에서 기기 ID를 확인하고 두 명령의 `booted`를 해당 ID로 바꾼다. 치트 앱은 경로의 앱 이름과 앱 ID를 각각 `HeavyMachValidation.app`, `com.gelb2.heavymach.validation`으로 바꾼다.

## 아이폰 설치

Xcode에서 `iOS/HeavyMachRestored.xcodeproj`를 연다. 사용할 target의 **Signing & Capabilities**에서 본인의 개발 팀을 선택한다. 저장소에 기록된 팀 ID는 작성자의 설정이다. 앱 ID 등록 충돌이 발생하면 본인 팀에서 사용할 고유한 Bundle Identifier를 지정한다.

아이폰을 연결하고 Mac을 신뢰한 뒤 개발자 모드를 켠다. 실행 대상을 연결한 아이폰으로 바꾸고 일반 앱은 `HeavyMachRestored`, 치트 앱은 `HeavyMachValidation` scheme으로 Run한다. Xcode의 빌드 단계가 `iphoneos`용 OpenAL을 준비한다.

기존 설치를 갱신할 때는 먼저 Xcode의 **Devices and Simulators**에서 해당 앱의 컨테이너를 내려받아 저장 데이터를 보관한다. 같은 앱 ID를 유지하면 기존 앱을 업데이트한다. 앱 ID를 바꾸면 별도 앱으로 설치되며 이전 저장이 자동으로 옮겨지지 않는다.

`tools/prepare_device_validation.py`는 작성자의 검증용 아이폰과 앱 ID에 고정돼 있다. 다른 아이폰의 설치에는 위 Xcode 경로를 사용한다.

## 실행 확인과 오류 해결

일반 앱은 타이틀→슬롯→모드·미션→전투 순서로 진입한다. 치트 앱은 타이틀의 검증 패널에서 시작 조건을 지정한다. 이동·점프·사격·무기 전환, 메뉴 복귀 후 진행 복구를 확인한다. 전체 무기 검사는 [검증 절차](https://github.com/gelb2/HeavyMach/blob/main/docs/weapon-validation-procedure.md)를 따른다.

| 증상 | 확인할 항목 |
|---|---|
| `libopenal.a`를 찾지 못함 | 선택한 SDK에 맞게 `build_openal.sh`를 먼저 실행했는지 확인한다. |
| x86_64 링크 실패 | 현재 라이브러리는 arm64다. 위 명령의 `ARCHS=arm64`와 Apple Silicon 실행 대상을 확인한다. |
| 그림이 표시되지 않음 | `git lfs pull`과 자원 검사를 실행하고 앱 번들의 `OriginalImages`를 확인한다. |
| 맵·무기 데이터를 찾지 못함 | `git lfs pull`과 자원 검사를 실행해 `Resources/RawData`를 확인한다. |
| 서명 실패 | Xcode의 팀·인증서·기기 등록 오류를 확인한다. 기존 작업에서는 CLI 서명 실패 후 Xcode GUI 빌드로 설치했다. |
| 이미지 비교 테스트에서 자료 없음 | 저장소에 포함된 `Resources/Validation/ImageDecodeParity.json`을 자원 검사로 확인한다. |

코어 검사는 자원 검사를 통과한 뒤 실행한다.

```sh
cmake -S . -B build/core -DCMAKE_BUILD_TYPE=Debug
cmake --build build/core -j 4
ctest --test-dir build/core --output-on-failure
```

기존 [IPA 없는 빌드 검사](ipa-independent-build-20261026.md)는 로컬 자원을 복사한 검사였다. [새 원격 clone 검사](clone-build-20261026.md)에서는 GitHub에서 소스·LFS 자원을 받아 빌드 4개, 코어17·iOS14 검사, 새 Simulator의 두 앱 실행을 통과했다.
