# HeavyMach iOS 코드·문서 전수검사

2026-10-26에 Codex가 iOS 소스와 Xcode 등록을 검사했다. 누락은 발견하지 못했다. 검사 시작 시 HeavyMach 저장소의 미커밋·미추적 파일은 0개였다. 호환판 내부에는 수정 24개가 있지만 모두 커밋된 패치로 재구성할 수 있다.

이번에 추가한 것은 파일별 문서 안내와 재검사 도구다. 게임 코드·아이폰 설치본·플레이 저장·v0.9 태그는 바꾸지 않았다.

## 검사 기준

- 검사 시작 main/원격 main: `e4c30ad8d6c35e76995ac1d932f122181ef4ee78`.
- 분리 전 보존 저장소: `90a7fc684571130e4209c86fe83f8f0b17250f3a`.
- 정상 플레이 기준 태그 v0.9: `4e17f5efd786095dfcd74a7334229908954da930`.
- 문서·검사 도구 작업은 develop `223b731542345d288f538473070fc767042e8792`에서 분기했다. 시작 main과 제품 파일은 같다.

## 파일과 빌드 구성

| 검사 | 결과 |
|---|---|
| 분리 전 Git 대상 595개 | 누락 0개. 578개 동일, 문서·도구·설정 17개는 이전 개편 변경으로 분류 |
| 현재 Sources 전체 | 68개. 모두 Git 추적. 파일별 SHA-256 기록 |
| 일반 앱 HeavyMachRestored | C++ 20개·Objective-C++ 9개, Xcode 등록 누락·중복 0개 |
| 치트 앱 HeavyMachValidation | 같은 29개. HM_VALIDATION으로 검증 기능 구분 |
| iOS XCTest | Tests/iOS의 12개 소스가 모두 등록됨 |
| Xcode 로컬 파일 참조 | 실제 경로 누락 0개. SDK 프레임워크·빌드 산출물은 별도 대상 |
| 따옴표 include/import | Sources와 iOS 검사에서 152개 경로 해결 |
| 제품 기준 비교 | Sources·iOS·Art·Tests의 Git tree가 v0.9와 동일 |
| Git LFS | git lfs fsck --objects 통과 |
| Git 제외 파일 | Sources·Tests·iOS·docs·tools·vendor·.agents의 소유 코드/문서 누락 후보 없음. 캐시·로컬 환경 설정 제외 |
| 별도 보존 입력 | Resources 1,600개·Analysis 3,571개·호환판 5,119개·원본 2개, 합계 10,292항목 동일 |

이전 검사에서 복구한 OpenAL 소스 4개도 현재 Git에 있다. 이번에 새로 발견한 누락으로 세지 않았다. 과거 빌드·trace 등 85,848항목, 11,243,954,093바이트는 기존 위치에서 존재·종류·크기를 확인했다. 이 캐시 전체의 내용 해시를 새로 계산한 것은 아니다.

## 미커밋 변경

| 위치 | 검사 시작 상태 | 보존 여부 |
|---|---|---|
| HeavyMach 저장소 | 미커밋·미추적 0개, 원격 main과 동일 | 커밋·원격 반영됨 |
| Compatibility/compat-source | 추적 파일 23개 수정, 내부 rust-sdl2 checkout 표시 | heavymach.patch가 git diff --binary HEAD와 바이트 일치. 임시 적용 결과도 일치 |
| 내부 rust-sdl2 | sdl2-sys/build.rs 1개 수정 | rust-sdl2.patch 임시 적용 결과가 현재 파일과 일치 |
| 기존 ProjectGameDev의 HeavyMach·iOS Proj | 해당 범위에 미커밋·미추적 변경 없음 | 이전 작업·원본 보존 |
| 허브의 Unity | 기존 수정 상태 유지 | 이 iOS 검사에서 수정하거나 커밋하지 않음 |

호환판은 upstream checkout에 패치를 적용한 구성이다. 내부 git status에 수정이 보이는 것이 예상 상태다. 패치를 삭제하거나 내부 checkout을 reset하지 않았다. 복구 기준은 Compatibility/upstream.json과 두 패치다.

## 문서 보완

기획·정본·설계·좌표·렌더·게임 계산·실행 정책·검증 절차 문서는 모두 존재한다. 검사 시작 시 문서 101개의 로컬 링크 398개와 사이트 링크 20개에서 끊긴 연결은 없었다.

기존 업무 연결표는 대표 파일 위주였다. 전체 소스 68개를 열거한 `docs/ios-source-map.md`를 추가하고 정책·관련 검사와 연결했다. 재검사 도구는 새 소스가 이 목록에서 빠지면 실패한다. 이 검사는 파일 목록의 누락을 잡으며 문서 내용의 정확성까지 자동 판정하지 않는다.

README의 source-coverage 안내를 과거 복원 기록으로 표시했다. 해당 문서의 CoreGraphics·AVAudioPlayer 설명을 현행 OpenGL ES·OpenAL 구현으로 오해하지 않도록 현재 소스 안내를 옆에 연결했다. TODO에서 이미 끝난 폰 자동 검사를 아직 미실행으로 읽을 수 있던 문구도 수정했다. 당시 실패·미설치 기록은 과거 이력으로 남겼다.

Git 릴리스 v0.9와 앱의 표시 버전 1.13/build 1도 구분해 적었다. 이번에는 앱 버전을 바꾸지 않았다.

## 남아 있는 작업

파일 누락 여부와 요청한 기능의 완료 여부는 따로 확인했다. 전체 화면 단일 문 전환은 아직 구현 전이다. 온보딩·메뉴의 나머지 화면 검토, 적 AI 고정값 전수 조사, 실제 OS 가로 회전과 틸트·터치 연결 검사, 장시간 발열 비교, 전체 무기의 수동 충돌·피해·출력음 확인도 남아 있다. 접히는 기기의 화면·조작부 분할은 구상 단계다. 현행 상태는 `docs/usability-todo.md`를 따른다.

모든 미션을 직접 플레이했을 때 크래시·오동작·적 이동 이상은 느끼지 못했다. 이번 파일 검사를 새 실기기 플레이 결과로 바꾸어 기록하지 않는다. 제품 변경이 없어 이전 새 경로의 코어 17/17·일반/치트 arm64 Simulator 빌드 증거를 재사용했다. 이번에는 앱 빌드·설치·플레이를 다시 실행하지 않았다.

## 검사 자료와 재실행

Git에 저장한 요약·소스 해시·대상별 파일 목록은 `docs/research/ios-completeness-evidence-20261026.json`에 있다. 전체 이전 파일 대조와 로컬 자료 목록은 `build/ios-completeness-20261026/`에 보존했다. 공개용 보고서에는 IPA·추출 자원·플레이 저장을 포함하지 않았다.

```sh
python3 tools/audit_repository_migration.py --legacy-root /Users/sokol/Git/ProjectGameDev --output build/ios-completeness-20261026/migration --site-root build/sites/heavymach-report/dist
python3 tools/audit_ios_sources.py --output build/ios-completeness-20261026/ios-source-audit.json
python3 tools/document_contract.py check
git lfs fsck --objects
```
