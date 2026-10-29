# HeavyMach 코드 구조

<!-- language-nav:start -->
[한국어](architecture.md) | [English](../en/public/architecture.md)
<!-- language-nav:end -->

게임 규칙은 C++20 코어에, Apple 플랫폼 연결은 Objective-C++에 구현했다. 하나의 workspace가 공용 모듈과 iOS/iPadOS·Mac 프로젝트의 의존 관계를 관리한다. 앱은 같은 코어와 자원을 사용한다.

## Workspace와 프로젝트

```text
HeavyMach.xcworkspace
├── Modules/HeavyMachModules.xcodeproj
│   ├── HeavyMachCore / HeavyMachValidationCore
│   ├── HeavyMachAppleRuntime / HeavyMachValidationRuntime
│   └── HeavyMachAssets.bundle
├── iOS/HeavyMachRestored.xcodeproj
│   ├── HeavyMachRestored       (iPhone + iPad)
│   └── HeavyMachValidation     (iPhone + iPad 치트)
└── macOS/HeavyMachMac.xcodeproj
    ├── HeavyMachMac
    └── HeavyMachMacValidation
```

위 이름은 모듈 역할을 나타낸다. 실제 target에는 `_iOS`·`_macOS` SDK 구분이 붙는다. 일반·치트 코어는 `HM_VALIDATION` 값을 앱과 일치시켜 따로 빌드한다. 치트 코드와 저장은 일반 앱에서 분리된다.

[공용 프로젝트 설정](../../Modules/project.yml) · [iOS 설정](../../iOS/project.yml) · [Mac 설정](../../macOS/project.yml)

## 모듈화를 지시한 이유

나는 iPhone·iPad·Mac용 게임을 각각 복사해 만드는 대신 공용 코드와 에셋을 모듈로 두고 앱에서 가져오도록 요구했다. 이동·탄도·데미지·적 배치를 플랫폼마다 복사하면 한쪽의 수정이 다른 쪽에 빠질 수 있다. 같은 코어를 연결하면 무기 수치와 계산 변경이 세 플랫폼의 빌드에 함께 들어간다.

Workspace는 여러 프로젝트와 빌드 의존 관계를 묶는다. 코드 공유는 정적 framework를 링크하는 방식으로, 자원 공유는 bundle을 앱에 포함하는 방식으로 구현했다. SPM 없이도 이 구성이 가능하다. Apple의 [복수 프로젝트 관리](https://developer.apple.com/documentation/xcode/managing-multiple-projects-and-their-dependencies)도 프로젝트 간 의존 관계와 공용 산출물을 다룬다.

iPhone·iPad는 같은 iOS target의 기기군 1·2를 사용하며, Mac은 AppKit 앱으로 실행한다. Catalyst는 사용하지 않는다. 공용 코드는 플랫폼별 SDK로 각각 컴파일한다.

## 소스와 자원

| 위치 | 책임 |
|---|---|
| [Sources/Core](../../Sources/Core) | 이동·월드·적·탄환·피해·성장·저장 형식·자동조준·추가 적 |
| [Sources/iOS](../../Sources/iOS) | 게임 뷰·HUD·메뉴·OpenGL·OpenAL. Apple 공용 런타임에 포함되는 코드도 이 경로에 있음 |
| [Sources/Apple](../../Sources/Apple) | UIKit/AppKit 및 그래픽 컨텍스트 차이, 공용 입력 연결 |
| [Sources/Mac](../../Sources/Mac) | AppKit 창·앱 수명주기·Mac 진입점 |
| [Resources](../../Resources) | 게임 이미지·음원·맵·무기·부품 데이터·검사 자료 |
| [Art/Extensions](../../Art/Extensions) | 메뉴 확장 배경·추가 항공기 등 새 그림 |
| [Tests](../../Tests) | C++ 코어 검사, Apple 입력·화면·음향·저장 검사 |

`HeavyMachAssets.bundle`은 OriginalImages·Images·Audio·RawData와 확장 아트를 묶는다. 코드와 자원의 파일 목록·해시는 [빌드 자원 목록](../build-resources.json)에 있다. Git LFS가 큰 자원을 내려받는다.

## 한 프레임의 처리

1. 터치·기울기·키보드를 게임 입력 상태로 바꾼다. 화면 좌표는 viewport와 카메라를 통해 월드 좌표로 변환한다.
2. `GameSession`이 플레이어·적·무기·탄환·아이템·월드를 갱신한다. 자동조준은 선택한 적의 위치와 무기 값을 이용해 포신을 조정한다.
3. 게임 이벤트를 HUD에 전달하고 OpenGL에서 텍스처·부품·탄환·효과·메뉴를 그린다.
4. 효과음 이벤트는 미리 준비한 OpenAL 버퍼·소스를 사용한다.
5. 저장 이벤트가 발생하면 진행 상태를 파일에 쓴다. 탄환 갱신·키 입력마다 파일을 쓰는 경로는 없다.

게임 진행은 표시 호출에 맞춰 한 번 갱신한다. 지연 뒤 여러 tick을 한꺼번에 따라잡던 누적 시간 계산은 제거했다. 일시정지와 백그라운드에서는 게임 시간·발사 유지 입력·효과음을 멈춘다.

## 그래픽과 좌표

게임 그림은 OpenGL로 그린다. iOS는 OpenGL ES 1.1 경로, Mac은 desktop OpenGL 호환 경로를 사용한다. 텍스처는 로드 때 준비하고 매 프레임 CPU에서 이미지를 재생성하지 않는다.

캔버스는 기기 비율에 따라 더 넓은 월드를 표시한다. 월드 좌표, 카메라 좌표, UI 좌표, 실제 픽셀을 구분한다. 버튼은 그림과 입력 영역을 같은 배치 계산으로 옮긴다. 메뉴의 중앙 에셋은 유지하고 좌우 배경을 덧붙였으며, 도어는 전체 캔버스를 덮는다.

[좌표 정책](../coordinate-policy.md) · [렌더링 정책](../rendering-policy.md) · [시간·저장·음향](../runtime-policy.md) · [데미지·성장 계산](../game-calculation-policy.md)

## Android 계획

공용 C++ 코어는 CMake·NDK로 빌드하고 Android의 창·입력·음향·그래픽 연결을 추가하는 계획이다. 현재 Android 앱과 APK는 없다. [개발 단계와 필요한 검사](../android-port-plan-20261026.md)
