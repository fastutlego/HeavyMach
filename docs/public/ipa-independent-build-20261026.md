# IPA 없이 iOS 복원판 빌드·실행 검증

**원본 IPA 없이 현재 HeavyMach iOS 복원판을 빌드할 수 있다.** Codex는 IPA와 포팅판을 넣지 않은 별도 복사본에서 일반·치트 앱의 Simulator용·iPhone용 빌드 4개를 모두 통과했다. 새 Simulator에 두 앱을 설치해 실행했고, 코어 17개·iOS 14개 검사도 통과했다.

검증 소스는 `264a410e432adb4e7c3f1dc040fbfedbe1ffcfa6`이다. 게임 코드를 변경하지 않았다. iPhone용 빌드는 서명을 제외한 컴파일·링크·번들 생성이며, 이번에 실제 아이폰 앱을 재설치하지는 않았다.

## IPA에 접근할 수 없는 빌드

새 폴더에 `Sources`, `iOS`, `Art`, `Resources`, `vendor`, `Tests`, `CMakeLists.txt`와 OpenAL 빌드 스크립트만 복사했다. 모든 입력 파일의 해시를 현재 프로젝트와 대조했다. 심볼릭 링크는 없었고 IPA·원작 실행파일·Compatibility·Analysis·기존 빌드 캐시는 복사하지 않았다.

macOS sandbox로 기존 Git 작업 폴더, 원본·분석 자료 보관 폴더, `.ipa` 파일 읽기를 차단했다. 실제 원본 IPA와 기존 소스 읽기는 `Operation not permitted`로 실패했고 복사본 소스 읽기는 성공했다. 원본 폴더 밖의 임시 `.ipa` 파일도 읽기 실패를 확인했다. 원본 파일을 삭제하거나 옮기지 않았다.

OpenAL부터 새로 빌드하고, 별도 DerivedData에 네 앱을 만들었다. 각 앱의 빌드 로그에서 실제 소스 컴파일 29개를 확인했다. 기존 프로젝트 경로의 소스를 컴파일한 항목은 0개다.

| SDK | 일반 앱 | 치트 앱 |
|---|---|---|
| iOS Simulator · arm64 | BUILD SUCCEEDED | BUILD SUCCEEDED |
| iPhone · arm64 · 서명 제외 | BUILD SUCCEEDED | BUILD SUCCEEDED |

첫 실행도 원본 폴더 차단 상태에서 성공했다. 이후 원본 폴더 밖의 임시 `.ipa`로 차단 규칙을 검사하면서 확장자 정규식 오류를 발견했다. 규칙을 수정하고 새 입력 폴더·새 DerivedData에서 위 빌드와 검사를 다시 수행했다. 이 문서의 최종 결과는 수정된 차단 규칙으로 확인한 실행이다. 첫 기록도 보존했다.

## 소스와 자원이 들어 있는 위치

| 항목 | 현재 프로젝트 | 생성된 앱 |
|---|---|---|
| 게임·iOS 소스 | `Sources` 68파일. 헤더·인터페이스 포함 | 앱별 29개 컴파일 단위가 arm64 코드로 포함 |
| 원본 게임 이미지 | `Resources/OriginalImages` 695개 | 695개, 파일별 SHA-256 일치 |
| 변환 이미지 | `Resources/Images` 695개 | 695개, 파일별 SHA-256 일치 |
| 음원 | `Resources/Audio` WAV 31개 | 31개, 파일별 SHA-256 일치 |
| 맵·무기·기체·부품 데이터 | `Resources/RawData` 46개 | 43개 MAP·3개 MCH, 파일별 SHA-256 일치 |
| 추가 배경 그림 | `Art/Extensions` 4개 | 4개, 파일별 SHA-256 일치 |
| 원본 IPA·ARMv6 실행파일 | 빌드 입력에 없음 | 없음 |

네 앱 모두 자원 누락·해시 불일치가 0개다. `GameSession`, `Player`, `World`, `HMGLRenderer`, `HMOpenALAudio`가 새 arm64 바이너리에 컴파일된 것도 확인했다. 원작 ARMv6 실행파일과 같은 해시의 파일은 없으며, 링크 의존성에도 touchHLE·HeavyMachRecovery가 없다.

실제 코드는 번들의 `OriginalImages`에서 이미지를, `Audio`에서 WAV를, `RawData`에서 게임 데이터를 읽는다. `Compatibility`는 비교용 별도 프로젝트다. 새 앱의 빌드 대상이나 실행 입력이 아니다.

## 새 앱 실행과 검사

빈 iPhone Air Simulator를 새로 만들고 이번 빌드의 일반·치트 앱을 각각 설치·실행했다. 아래는 각 앱의 실행 직후 화면이다.

![일반 앱 실행 직후 화면](independence/HeavyMachRestored-title.png)

![치트 앱 실행 직후 화면. 검증 버튼이 표시된다.](independence/HeavyMachValidation-title.png)

| 검사 | 결과 |
|---|---|
| CTest 코어 | 17통과·0실패 |
| 이미지 디코딩 | 이미지 695개의 크기·디코딩 픽셀 해시·캐시 재사용 검사 통과 |
| OpenAL | 재생·자원 재사용·비활성 처리 등 7개 검사 통과 |
| 게임 입력·동작 | 전체 무기 목표·연사, 확장 영역 사격, 무기 전환음 호출, 방향 버튼 이동, 원본 프레임 갱신·난수 검사 통과 |
| iOS XCTest 합계 | 14통과·0실패·0건너뜀 |

음원 파일의 포함·무결성과 OpenAL 동작은 확인했다. 이번 검사가 31개 소리를 사람이 각각 듣고 확인한 결과라는 뜻은 아니다. 내가 직접 한 전체 미션 플레이 결과는 그대로 유지하며 이번 검사와 구분한다.

## REA 분석과 새 소스 구현

REA 분석 기록 2,067개를 실제 파일 해시와 전수 대조했고 누락·불일치는 없었다. 25함수군의 구현·테스트 파일을 확인하고, 대표 주소 70항목의 의사코드·어셈블리 Evidence 140건을 원본 SHA·주소·현재 코드와 대조했다.

이번 세션에서도 REA/Ghidra로 `GlobalInfo.addExp(0x106e0)`를 재조회했다. 기존 의사코드와 일치했으며, 경험치 누적·레벨 99 상한·호출당 한 단계 증가·업그레이드·음향·HUD까지 현재 코드에 연결돼 있다. 조회 후 REA 세션을 닫았고 원본 해시는 유지됐다.

게임 규칙은 C++로, iOS 연결은 Objective-C++로 구현돼 있다. 원본 실행파일에 계산을 맡기거나 IPA를 실행하는 경로는 발견하지 않았다. 검사한 게임 경로에서 빈 구현으로 기능을 건너뛰는 코드도 찾지 못했다. 원작의 모든 Objective-C 메서드를 같은 이름으로 복제한 구조는 아니다. 범용 컨테이너·런타임·진입점은 C++와 현재 iOS 구현을 사용한다.

[25함수군·주소·소비 코드 상세](rea-implementation-audit-20261026.md)와 [기계 판독 증거](independence/rea-provenance.json)에 대조 결과를 기록했다.

## 현재 폴더와 Git 저장소

현재 Mac의 iOS 프로젝트에는 위 소스와 자원이 모두 들어 있다. `.ipa` 파일만 삭제해도 이 자원 폴더와 설치된 앱은 그대로 남으며, 이번 빌드는 IPA 접근 없이 성공했다.

이 검사 당시 Git에는 `Resources`가 제외돼 있었으므로 새 clone에 자원이 빠졌다. 이후 빌드·검사 자원 1,471개를 Git/LFS에 추가했다. 현재 설치 절차는 [빌드와 설치](build-and-install.md)를 따른다. 기존 로컬 프로젝트의 빌드·실행에 IPA가 필요하다고 설명한 것은 잘못이다.

## 증거 파일

[빌드·검사 요약](independence/summary.json), [앱별 자원·바이너리 해시](independence/bundle-audit.json), [REA 대조](independence/rea-provenance.json)를 제공한다. 원시 빌드 로그·전체 입력 해시·차단 규칙·XCTest 결과·재조회 원문은 로컬 `build/no-ipa-audit-20261026/`에 보존했다. 최종 실행은 그 아래 `final/`, 최초 실행은 상위 폴더다.
