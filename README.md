# HeavyMach 1.13 소스 복원

원본 IPA를 REA로 분석하여 게임 코드를 소스로 복원하고, 최신 iPhone과 iOS 시뮬레이터에서 빌드·실행하는 프로젝트다. 2026-10-24 사용자는 HeavyMach 1을 GameDev 초기 기획의 참고 원본이라고 확인했다. Unity는 별도 ProjectGameDevUnity 저장소에서 관리한다.

일반 앱과 치트 검증 앱을 iPhone Air에 설치했고, 사용자는 설치본의 모든 미션에서 크래시·오동작·적 이동 이상은 없는 것 같다고 보고했다. 이 제품 코드를 `v0.9`로 보존했다. 복원판은 C++20·Objective-C++로 작성했다.

- [IPA 없는 빌드·실행 검증](docs/public/ipa-independent-build-20261026.md): 원본 접근 차단 상태에서 네 앱 빌드, 자원·REA 구현 대조.
- [빌드와 설치](docs/public/build-and-install.md): 원본 자원 준비, 두 앱 빌드, Simulator·아이폰 설치.
- [구현과 검증 범위](docs/public/implementation-status.md): 구현 기능, 실제 확인한 결과, 남은 작업과 배포 상태.
- [SaaS 집필용 문서 안내](docs/public/publication-inventory.md): 읽기 순서·주제별 출처·이미지와 아직 없는 근거.

기존 [HeavyMachRecovery](Compatibility/README.md)는 원본 ARMv6 바이너리를 인터프리터로 실행한다. 그 실행본은 동작 비교 기준이며, 소스 복원 완료를 뜻하지 않는다. 이 프로젝트의 완료 여부는 소스로 빌드한 게임의 증거로 판단한다.

[개발자의 변](docs/RESTORATION.md)에 HeavyMach를 선택한 개인적인 배경을 기록한다. 소개·회고·SaaS Apps·Mags를 작성할 때는 [원문과 출처](docs/developer-background.md)를 함께 읽는다.

| 문서 | 내용 |
| --- | --- |
| [game-design](docs/game-design.md) | 게임 기획·화면 의도·요구 ID·승인/미구현 범위 |
| [document-governance](docs/document-governance.md) | 문서 위계·업무별 읽기·변경 전파·문서/코드/앱 대조 |
| [document-map](docs/document-map.json) | 현행 문서와 업무별 소스·검사·증거 연결 |
| [source-provenance](docs/source-provenance.md) | 사용자 제공 링크, 원본 식별·해시, REA 증거 위치 |
| [current-status](docs/current-status.md) | 정적 분석·사용자 관측·남은 검증 |
| [restoration-contract](docs/restoration-contract.md) | 보존 요구와 소스 복원 완료 기준 |
| [design](docs/design.md) | 확인한 함수·데이터와 복원 설계 |
| [verification](docs/verification.md) | 과거 빌드·동작 비교·저장 실행 이력. 현행은 current-status와 분야 정책 |
| [validation-mode](docs/validation-mode.md) | 조건 설정 패널·실제 레벨업·외형·저장 검사 결과 |
| [source-coverage](docs/source-coverage.md) | 과거 원본 함수 대응·당시 검사 범위 |
| [ios-source-map](docs/ios-source-map.md) | 현행 iOS 소스 68개·정책·검사 연결 |
| [iOS 전수검사](docs/ios-completeness-audit-20261026.md) | 파일 누락·Xcode 등록·문서·미커밋 변경 확인 |
| [boss-battles-v6](docs/boss-battles-v6.md) | 최신 코어의 보스 48개 실제 처치 검사 |
| [runtime-policy](docs/runtime-policy.md) | 시간·저장·OpenAL 음향 정책 |
| [stage-transition-contract](docs/stage-transition-contract.md) | 전체맵의 출구·미션말·최종clamp 조건검사 |
| [rendering-policy](docs/rendering-policy.md) | 현행 OpenGL 렌더링·자원 수명 |
| [coordinate-policy](docs/coordinate-policy.md) | 월드·화면·터치·틸트 좌표 |
| [game-calculation-policy](docs/game-calculation-policy.md) | 데미지·HP·EXP·강화·난수 계산 |
| [usability-todo](docs/usability-todo.md) | 발열·전환음·잠금·미사용 후속 과제 |
| [wide-canvas-plan](docs/wide-canvas-plan.md) | 디바이스 전체 캔버스의 시야 확장 조사·계획 |
| [stage-journeys](docs/stage-journeys.md) | 정상 시작 위치부터 진행한 맵별 성공·실패 |
| [hud-event-order](docs/hud-event-order.md) | HP·강화·메뉴 이벤트 수정과 최종 빌드·검사 |

게임 핵심 소스는 `Sources/Core/`, iOS 화면·입력·음향 연결은 `Sources/iOS/`, Xcode 프로젝트는 `iOS/HeavyMachRestored.xcodeproj`다. Xcode에서 일반 실행은 `HeavyMachRestored`, 조건별 검사는 `HeavyMachValidation` scheme을 선택한다. 검증 앱은 별도 저장을 사용한다.

이 저장소 루트에서 코어 검사:

```sh
cmake -S . -B build/core -DCMAKE_BUILD_TYPE=Debug
cmake --build build/core -j 4
ctest --test-dir build/core --output-on-failure
```

프로젝트 구성 변경 후 생성은 `xcodegen generate --spec iOS/project.yml`이다. 원본 IPA·추출본·사용자 저장·검증된 호환 실행본을 덮어쓰지 않는다. 최신 빌드·실행 성공 범위와 미확인 항목은 위 문서에서 확인한다.

원본과 추출 자원 `Resources/`, 로컬 분석 `Analysis/`, `build/`는 Git에서 제외한다. 새 checkout에는 `Resources`가 없으므로 기존 프로젝트의 준비된 자원 폴더를 함께 가져온다. 준비된 자원이 있으면 IPA는 빌드·실행에 필요하지 않다. 자원을 다시 추출할 때는 [출처](docs/source-provenance.md)의 동일 SHA 원본을 사용하고 `python3 tools/export_resources.py --source <원본-추출-앱-폴더> --output Resources`로 파생 자원을 준비한다. 현재 렌더러에는 `OriginalImages`도 필요하다. [설치 가이드](docs/public/build-and-install.md)의 원본 이미지 복사 단계까지 수행한다. 기존 자원과 사용자 저장을 덮어쓰지 않는다. vendor의 라이선스를 유지한다.

업무별 읽기 목록과 문서 연결 검사:

```sh
python3 tools/document_contract.py plan --task rendering
python3 tools/document_contract.py check
```

이 명령은 문서 연결을 검사한다. 기능 의미와 앱 실행의 정상 여부는 관련 정책·코드·실행 증거를 대조한다.

- [REA 분석·복원 방법과 전체 증거 색인](docs/public/rea-restoration-method-20261025.md): 사용한 스킬·도구, 25개 함수군, 실패 수정, 구현·검증 기록.
- [IPA 포팅과 네이티브 앱 복원](docs/public/ipa-port-and-native-app-20261026.md): 호환판 제작, 새 앱의 성능 문제와 비교·수정 순서, 치트·일반 앱 설치를 설명한 기술 블로그 원고.

독립 저장소의 브랜치·태그·롤백은 [저장소 관리](docs/repository-management.md)를 따른다. 분리 전 경로와 커밋 해시는 당시 증거이며 [이력 대응](docs/migration/commit-map.txt)에서 새 해시를 찾는다.

Apple Silicon 시뮬레이터에서 새 checkout을 빌드할 때는 먼저 음향 라이브러리를 준비한다.

```sh
bash tools/build_openal.sh iphonesimulator
xcodebuild -project iOS/HeavyMachRestored.xcodeproj -scheme HeavyMachRestored -sdk iphonesimulator -destination 'generic/platform=iOS Simulator' ARCHS=arm64 ONLY_ACTIVE_ARCH=YES CODE_SIGNING_ALLOWED=NO build
```

치트 앱은 같은 명령의 scheme을 HeavyMachValidation으로 바꾼다. 현재 OpenAL 빌드 스크립트의 대상은 arm64다. 최초 일반 generic 명령의 라이브러리 입력 누락 실패는 개편 기록에 보존했다.

[분리 후 전수검사](docs/migration/completeness-audit-20261026.md)에 파일 보존·복구·경로 수정과 로컬 증거 위치를 기록했다. 과거 build/ 증거는 기존 제작 폴더에 보존하며 새 빌드 출력과 구분한다.

코드를 수정할 때는 [AGENTS.md](AGENTS.md), [정본](docs/canonical.md), [현재 상태](docs/current-status.md)와 관련 정책을 읽는다.

기존 화면 자료는 [이미지 색인](docs/public/media/README.md), 후속 플레이 영상의 추출·배치는 [영상·README 작업 절차](docs/media-publication.md)를 따른다.
