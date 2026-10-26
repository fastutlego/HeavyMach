# HeavyMach 빌드와 설치

HeavyMach 1.13 복원 소스로 일반 앱과 치트 검증 앱을 빌드하는 방법이다. 두 앱은 같은 게임 코어를 쓰며 별도 앱 ID와 저장 공간을 사용한다.

## 준비물

Apple Silicon Mac, iOS SDK와 Simulator 런타임을 설치한 Xcode, Python 3, CMake 3.20 이상, Git LFS가 필요하다. Xcode 프로젝트는 저장소에 포함돼 있다. `iOS/project.yml`을 변경할 때만 XcodeGen으로 다시 생성한다.

2026년 10월 26일 작성 환경은 macOS 27.0.1, Xcode 27.1, CMake 4.4.4, Python 3.14.6, XcodeGen 2.45.4다. 앱의 deployment target은 iOS 15.0이다. 현재 OpenAL 빌드 스크립트는 arm64를 대상으로 한다.

원본 HeavyMach 1.13 IPA도 필요하다. 이미지·음원·맵은 저장소에 포함하지 않는다. 사용한 IPA의 SHA-256은 `477c672f92110c6020be3f744dfa7a3456739e6f970c66715e3b7b3cd32b7776`이다. [원본 출처와 식별 정보](https://github.com/gelb2/HeavyMach/blob/main/docs/source-provenance.md)에서 대조한다.

## 소스 받기

```sh
git clone https://github.com/gelb2/HeavyMach.git
cd HeavyMach
git lfs install --local
git lfs pull
```

현재 저장소는 비공개이므로 접근 가능한 GitHub 계정으로 인증해야 한다. 재현 기준을 고정하려면 clone 직후 `git switch --detach v0.9`를 실행한다. `v0.9`는 정상 플레이를 확인한 제품 코드의 태그이며, `main`에는 그 뒤의 문서·저장소 정리가 추가돼 있다. GitHub Releases에 설치 파일은 아직 등록하지 않았다.

## 원본 자원 준비

이 절은 `Resources`가 없는 새 checkout에서 실행한다. `HM_IPA`에는 보관한 IPA의 실제 경로를 지정한다. 원본은 읽기만 하고 추출·변환 결과는 별도 폴더에 만든다.

```sh
export HM_IPA='/absolute/path/Heavy_Mach_1.13_ios_2.1.ipa'
shasum -a 256 "$HM_IPA"
mkdir -p build/original-113
unzip -q "$HM_IPA" -d build/original-113
python3 tools/export_resources.py \
  --source build/original-113/Payload/HeavyMach.app \
  --output Resources
```

해시가 위 값과 같을 때 이 절차를 진행한다. 추출 명령은 `Images`, `Audio`, `RawData`, 앱 아이콘 등을 만든다. 현재 게임 렌더러는 원본 이미지 바이트를 `OriginalImages`에서 읽으므로 다음 복사도 필요하다.

```sh
python3 - <<'PY'
from pathlib import Path
import hashlib, shutil, sys
sys.path.insert(0, 'tools')
from resource_codecs import category
source = Path('build/original-113/Payload/HeavyMach.app')
output = Path('Resources/OriginalImages')
output.mkdir(parents=True, exist_ok=False)
count = 0
for path in sorted(source.rglob('*')):
    if not path.is_file():
        continue
    relative = path.relative_to(source)
    if category(str(relative)) != 'image':
        continue
    target = output / relative
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(path, target)
    assert hashlib.sha256(path.read_bytes()).digest() == hashlib.sha256(target.read_bytes()).digest()
    count += 1
Path('Resources/Validation').mkdir(exist_ok=True)
print(f'OriginalImages: {count} files copied and verified')
PY
```

같은 원본의 이미지는 695개다. `Resources/Validation`은 검증 앱의 추가 자료 폴더다. 게임 빌드에는 빈 폴더를 준비할 수 있지만, 이미지 디코딩 비교 테스트에는 별도로 `ImageDecodeParity.json`이 필요하다. 이 비교 자료는 로컬에 보관돼 있다.

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
| 그림이 표시되지 않음 | `Resources/OriginalImages`에 원본 이미지가 있고 앱 번들에도 복사됐는지 확인한다. |
| 맵·무기 데이터를 찾지 못함 | `Resources/RawData`를 확인하고 같은 1.13 IPA에서 자원을 준비한다. |
| 서명 실패 | Xcode의 팀·인증서·기기 등록 오류를 확인한다. 기존 작업에서는 CLI 서명 실패 후 Xcode GUI 빌드로 설치했다. |
| 이미지 비교 테스트에서 자료 없음 | 로컬 비교 자료 `Resources/Validation/ImageDecodeParity.json`이 필요하다. 앱 빌드 성공과 이 테스트의 준비 상태를 구분한다. |

코어 검사는 원본 자원을 준비한 뒤 실행한다.

```sh
cmake -S . -B build/core -DCMAKE_BUILD_TYPE=Debug
cmake --build build/core -j 4
ctest --test-dir build/core --output-on-failure
```

기존 분리 작업에서 일반·치트 arm64 Simulator 빌드와 코어 17개 검사를 통과했다. 이 문서 작성에서는 스크립트·프로젝트 설정·명령 도움말을 대조하고, 위 이미지 복사 코드를 별도 임시 폴더에서 실행해 695개 파일의 원본 해시 일치를 확인했다. 외부 사용자의 새 clone에서 위 순서 전체를 실행한 기록은 아직 없다.
