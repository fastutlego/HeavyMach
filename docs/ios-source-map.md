# iOS 소스와 문서 연결

2026-10-26 전수검사 기준. 현재 구현의 파일별 안내다. 요구·승인 범위는 [기획](game-design.md), 구현 규칙은 각 분야 정책을 따른다. 과거 함수 복원 과정은 [이전 대응표](source-coverage.md)에 남긴다.

현재 앱은 C++ 코어와 Objective-C++ Apple 연결로 구성된다. Swift 소스는 없다. 공용 코드·자원은 Modules 프로젝트의 정적 프레임워크와 자원 번들로 빌드한다. iOS/iPadOS 앱과 native macOS 앱은 각 진입점만 컴파일하고 공용 프레임워크를 연결한다. `HM_VALIDATION`으로 일반·치트 기능을 분리한다. 원본 인터페이스 자료는 참조용이다.

## 소스 파일

표는 `Sources/`의 모든 파일을 열거한다. 검사 열에는 해당 소스를 다루는 검사 파일을 연결했다.

| 역할 | 파일 | 구현 규칙 | 관련 검사 |
|---|---|---|---|
| 앱 시작·화면 방향·생명주기 | `Sources/iOS/Application.mm` | [runtime-policy.md](runtime-policy.md) | Tests/iOS/HMPassiveLifecycleTests.mm |
| 게임 프레임·터치·틸트·화면 전환 | `Sources/iOS/HMGameView.h`, `Sources/iOS/HMGameView.mm` | [runtime-policy.md](runtime-policy.md) | Tests/iOS/HMFramePerformanceTests.mm |
| OpenGL ES 표시·호환 색상 | `Sources/iOS/HMGLRenderer.h`, `Sources/iOS/HMGLRenderer.mm`, `Sources/iOS/HMCompatibilityPixels.hpp` | [rendering-policy.md](rendering-policy.md) | Tests/iOS/HMGLReferenceTests.mm; Tests/iOS/HMCompatibilityPixelsTests.mm |
| 텍스처·음원 자원 관리 | `Sources/iOS/HMMedia.h`, `Sources/iOS/HMMedia.mm` | [rendering-policy.md](rendering-policy.md) | Tests/iOS/HMResourceLifecycleTests.mm; Tests/iOS/HMMediaTests.mm; Tests/iOS/HMMediaParityTests.mm |
| OpenAL 재생 | `Sources/iOS/HMOpenALAudio.h`, `Sources/iOS/HMOpenALAudio.mm` | [runtime-policy.md](runtime-policy.md) | Tests/iOS/HMOpenALAudioTests.mm; Tests/iOS/HMAudioTests.mm |
| 메뉴·HUD·버튼 상태 | `Sources/iOS/HMMenus.h`, `Sources/iOS/HMMenus.mm`, `Sources/iOS/HMGameHUD.h`, `Sources/iOS/HMGameHUD.mm`, `Sources/iOS/HMButtonState.hpp` | [coordinate-policy.md](coordinate-policy.md) | Tests/iOS/HMMenuParityTests.mm |
| 치트 앱 조건 패널·방향 버튼 | `Sources/iOS/HMValidationPanel.h`, `Sources/iOS/HMValidationPanel.mm`, `Sources/iOS/HMValidationControls.h`, `Sources/iOS/HMValidationControls.mm` | [weapon-validation-procedure.md](weapon-validation-procedure.md) | Tests/iOS/HMFramePerformanceTests.mm |
| 전투 진행·입력·이벤트 | `Sources/Core/GameSession.cpp`, `Sources/Core/GameSession.hpp` | [game-design.md](game-design.md) | Tests/GameSessionTests.cpp |
| 원본 데이터 읽기 | `Sources/Core/GameData.cpp`, `Sources/Core/GameData.hpp` | [design.md](design.md) | Tests/CoreTests.cpp |
| 저장·강화·소지품 | `Sources/Core/SaveData.cpp`, `Sources/Core/SaveData.hpp`, `Sources/Core/GlobalInfo.cpp`, `Sources/Core/GlobalInfo.hpp` | [runtime-policy.md](runtime-policy.md) | Tests/iOS/HMSaveParityTests.mm; Tests/CoreTests.cpp |
| 플레이어·적·월드·아이템 | `Sources/Core/Player.cpp`, `Sources/Core/Player.hpp`, `Sources/Core/Enemy.cpp`, `Sources/Core/Enemy.hpp`, `Sources/Core/World.cpp`, `Sources/Core/World.hpp`, `Sources/Core/ItemBox.cpp`, `Sources/Core/ItemBox.hpp` | [game-calculation-policy.md](game-calculation-policy.md) | Tests/PlayerTests.cpp; Tests/WorldTests.cpp; Tests/GameSessionTests.cpp |
| 전투·거리·이동 | `Sources/Core/Combat.cpp`, `Sources/Core/Combat.hpp`, `Sources/Core/Geometry.cpp`, `Sources/Core/Geometry.hpp`, `Sources/Core/Motion.cpp`, `Sources/Core/Motion.hpp` | [game-calculation-policy.md](game-calculation-policy.md) | Tests/CombatTests.cpp; Tests/CoreTests.cpp |
| 프레임·탄환·궤적 표시 | `Sources/Core/Visuals.cpp`, `Sources/Core/Visuals.hpp`, `Sources/Core/BulletVisual.cpp`, `Sources/Core/BulletVisual.hpp`, `Sources/Core/Trail.cpp`, `Sources/Core/Trail.hpp` | [rendering-policy.md](rendering-policy.md) | Tests/CoreTests.cpp |
| CPU 비교 렌더·텍스처 변환 | `Sources/Core/SpriteRaster.cpp`, `Sources/Core/SpriteRaster.hpp`, `Sources/Core/TrailRaster.cpp`, `Sources/Core/TrailRaster.hpp`, `Sources/Core/TextureFormat.hpp` | [rendering-policy.md](rendering-policy.md) | Tests/iOS/HMGLReferenceTests.mm; Tests/iOS/HMPresentationComparisonTests.mm |
| 타이틀·문 전환 | `Sources/Core/TitleState.cpp`, `Sources/Core/TitleState.hpp`, `Sources/Core/LoadingTransition.cpp`, `Sources/Core/LoadingTransition.hpp` | [rendering-policy.md](rendering-policy.md) | Tests/TitleStateTests.cpp; Tests/LoadingTransitionTests.cpp |
| 카메라 흔들림 | `Sources/Core/CameraShake.cpp`, `Sources/Core/CameraShake.hpp` | [coordinate-policy.md](coordinate-policy.md) | Tests/CameraShakeTests.cpp |
| 화면·월드 좌표·틸트 | `Sources/Core/CanvasGeometry.hpp`, `Sources/Core/Viewport.hpp`, `Sources/Core/TiltState.hpp` | [coordinate-policy.md](coordinate-policy.md) | Tests/iOS/HMFramePerformanceTests.mm |
| 음향 계약 | `Sources/Core/AudioContract.hpp` | [runtime-policy.md](runtime-policy.md) | Tests/AudioContractTests.cpp |
| 검증 조건 구성 | `Sources/Core/ValidationScenario.cpp`, `Sources/Core/ValidationScenario.hpp` | [weapon-validation-procedure.md](weapon-validation-procedure.md) | Tests/iOS/HMFramePerformanceTests.mm |
| 원본 클래스·메서드 주소·디스크 구조 | `Sources/OriginalInterfaces/OriginalClasses.h`, `Sources/OriginalInterfaces/OriginalTypes.h`, `Sources/OriginalInterfaces/Arm32DiskLayouts.h`, `Sources/OriginalInterfaces/method-address-map.json` | [source-provenance.md](source-provenance.md) | 원본 분석 참조. 앱 컴파일 대상에서 제외 |

## 추가 공용 기능

| 기능 | 소스 | 문서·검사 |
|---|---|---|
| 자동조준·상태 표지 | `Sources/Core/AutoAim.hpp`, `Sources/iOS/HMAimOverlay.hpp` | [자동조준](auto-aim-plan-20261026.md), Tests/AutoAimTests.cpp |
| 수직이착륙기 | `Sources/Core/VTOL.cpp`, `Sources/Core/VTOL.hpp` | [VTOL](vtol-expansion.md), Tests/VTOLTests.cpp |
| 럭스루나에 증원 | `Sources/Core/Fighters.cpp`, `Sources/Core/Fighters.hpp` | [기체·무기·배치](lux-lunae-plan-20261027.md), Tests/FightersTests.cpp |
| 쉴카·퉁구스카 | `Sources/Core/GroundAA.cpp`, `Sources/Core/GroundAA.hpp`, `Sources/Core/GroundAAArt.inc` | [조립·무장·배치](ground-aa-expansion.md), Tests/GroundAATests.cpp·GroundAARuntimeTests.cpp |

## 빌드와 자원

- `Modules/project.yml`, `iOS/project.yml`, `macOS/project.yml`이 Xcode 생성 입력이며 각 하위 project.pbxproj가 등록 결과다. 구성 변경 시 둘을 함께 검토한다.
- iOS·macOS 일반/치트 앱은 공용 코어 22개와 각 플랫폼 런타임을 연결한다. 현재 대상별 파일 수는 `tools/audit_ios_sources.py` 결과를 따른다.
- `vendor/openal-soft/`와 `vendor/stb/`의 소스·라이선스를 보존한다. OpenAL 빌드 명령과 arm64 대상은 [README](../README.md)에 있다.
- `Resources/`와 `Art/Extensions/`는 저장소에 포함된 빌드 입력이다. `Analysis/`, 원본 IPA와 호환판 checkout은 복원 조사 자료이며 새 앱 빌드에 필요하지 않다. Git 제외는 누락 여부와 별도로 판정한다. 출처·해시는 [출처 문서](source-provenance.md), 호환 소스 재구성은 [호환판 안내](../Compatibility/README.md)를 따른다.
- Git 태그 `v0.9`는 정상 플레이 기준 릴리스다. iOS plist의 `CFBundleShortVersionString=1.13`, `CFBundleVersion=1`과 별개다. 이번 검사는 버전 값을 변경하지 않았다.

## 재검사

```sh
python3 tools/audit_ios_sources.py --output build/ios-source-audit.json
python3 tools/document_contract.py check
```

첫 명령은 소스 목록·Xcode 등록·로컬 include·이 문서의 파일 목록·기준 태그의 제품 tree·호환판 패치를 검사한다. 패치는 임시 폴더에서만 적용한다. 새 제품 변경으로 v0.9와 달라지면 실패를 보고하므로 변경 사유를 검토한다. 자동으로 코드를 되돌리지 않는다.

이번 전체 검사 결과는 [iOS 전수검사](ios-completeness-audit-20261026.md)에 있다.

## Apple 플랫폼 확장

| 역할 | 파일 | 관련 검사 |
|---|---|---|
| 키 매핑·가상 기울기·조준 | `Sources/Core/KeyboardInput.hpp` | Tests/KeyboardInputTests.cpp, Tests/Apple/HMAppleInputTests.mm |
| 플랫폼 이미지·뷰·좌표 값 | `Sources/Apple/HMPlatform.h`, `Sources/Apple/HMPlatform.mm` | Apple 공통 입력·렌더 검사 |
| ES/desktop GL 컨텍스트 | `Sources/Apple/HMGLPlatform.h`, `Sources/Apple/HMGLPlatform.mm` | Mac 앱 실행·iOS GL 비교 |
| 공용 자원 번들 조회 | `Sources/Apple/HMResources.h`, `Sources/Apple/HMResources.mm` | 이미지·음원·게임 데이터 로드 검사 |
| native Mac 앱·창·생명주기 | `Sources/Mac/Application.mm` | Mac 치트 앱 실행 |
| Mac 검증 방향 입력 | `Sources/Mac/HMValidationControlsMac.mm` | HMAppleInputTests |
| Mac 검증 조건 창 | `Sources/Mac/HMValidationPanelMac.mm` | Mac 검증 패널·공통 시나리오 검사 |

공통 Apple 구현 파일은 기존 `Sources/iOS` 경로를 유지한다. 경로 이름만으로 UIKit 전용이라고 판단하지 않는다. 실제 컴파일 대상과 AppKit/UIKit 분기는 `Modules/project.yml`에서 확인한다.
