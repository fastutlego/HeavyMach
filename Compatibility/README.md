# Heavy Mach 1.13 복구 프로젝트

사용자가 보유한 원본 Heavy Mach 1.13 IPA를 iPhone/iOS 시뮬레이터에서 실행하는 Xcode 프로젝트다. 원본 게임 소스는 확보되지 않았다. 원본 ARMv6 실행 파일을 touchHLE 계열의 CPU 인터프리터로 실행한다. JIT는 사용하지 않는다.

## 열기와 빌드

1. `originals/README.md`대로 보유 원본 IPA를 둔다. 패키징은 SHA-256 일치를 검사한다.
2. 이 폴더에서 `./bootstrap.sh`를 실행한다. macOS Apple Silicon, Xcode, CMake가 필요하다. Rust 1.99.1과 iOS 대상은 프로젝트 범위에 설치한다.
3. `ios/HeavyMachRecovery.xcodeproj`를 연다. HeavyMachRecovery scheme과 시뮬레이터 또는 iPhone을 선택한다.
4. 실기기는 Xcode의 Signing & Capabilities에서 본인 개발팀을 선택한다. 이 Mac에서 검증한 Personal Team 설정이 들어 있다. 다른 Mac에서 기존 팀을 사용할 필요는 없다.
5. Build/Run한다. Xcode 빌드 단계가 Rust 코드도 다시 빌드한 다음 원본을 패키징한다.

CLI 시뮬레이터 빌드:

```sh
xcodebuild -project ios/HeavyMachRecovery.xcodeproj -scheme HeavyMachRecovery \
  -destination 'generic/platform=iOS Simulator' -derivedDataPath DerivedData \
  CODE_SIGNING_ALLOWED=NO build
```

Rust 실행기만 빌드하려면 `./build-heavy.sh aarch64-apple-ios-sim` 또는 `./build-heavy.sh aarch64-apple-ios`를 사용한다. `ios/project.yml`은 XcodeGen 원본이다. 프로젝트 설정 변경 후 `xcodegen generate --spec ios/project.yml`로 재생성한다.

## 소스와 원본 보존

`upstream.json`이 외부 실행기 커밋·Rust 버전·원본 해시를 고정한다. `patches/rust-sdl2.patch`는 버전 고정된 SDL Rust 바인딩이 로컬 SDL 소스를 사용하도록 연결한다. `patches/heavymach.patch`가 Heavy Mach 진입점, iOS/시뮬레이터 링크, 원래 화면 회전, 최신 iOS UIScene 지원, 시뮬레이터 방향키 틸트, MoleWorld 전용 후킹 비활성화 변경을 보존한다. `bootstrap.sh`는 기존 checkout을 덮어쓰지 않는다. 패치를 바꿨다면 기존 compat-source 작업과 비교한 후 적용한다.

`compat-source`, `toolchain`, `originals` 입력, 빌드 산출물은 Git 제외다. 현재 Mac의 toolchain 및 Rust 빌드 캐시는 복구 조사 폴더의 검증된 캐시를 링크해 재사용한다. 새 checkout은 bootstrap으로 독립 준비한다. upstream 소스에 포함된 LICENSE와 third-party 라이선스를 유지한다.

## 조작과 확인

원본 게임의 메뉴·조작·성장 규칙을 보존한다. 시뮬레이터는 Device Hub에서 Capture Keyboard를 켜고 방향키를 누르면 기울기 입력을 대신한다. 키를 놓으면 중립으로 돌아간다. 실기기는 원래 기울기 센서를 사용한다. 시뮬레이터 창 방향이 반대면 Device Hub의 Rotate Left를 사용한다. 앱 프레임 캡처와 창 방향을 구별해서 확인한다.

검증 결과와 남은 문제는 허브의 `docs/heavymach-recovery-20261024.md`에 기록한다. 빌드 성공만으로 플레이 완료를 주장하지 않는다.
