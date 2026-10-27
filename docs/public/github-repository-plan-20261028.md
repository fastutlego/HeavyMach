# HeavyMach GitHub 구성 점검과 보완 계획

현재 README에는 개발 동기, 실제 플레이 이미지 8장·GIF 2개·영상 3개, 기능·설치·트러블슈팅·검증 문서가 연결돼 있다. 2026-10-28에 문서 브랜치를 develop `0a2c0e33`을 거쳐 main `db634ce6`으로 병합했다. GitHub 기본 화면의 README도 갱신됐다. 게임 코드와 자원은 v0.12와 같다.

이번 후속 작업에는 새 코드용 MIT와 적용 범위, 자원별 조건, 외부 라이브러리 고지, 기술 배지 5개를 추가했다. 저장소 공개 범위와 게임 기능은 바꾸지 않았다.

## 다른 게임 저장소에서 참고한 구성

| 프로젝트 | 실제 구성 | HeavyMach에 적용할 부분 |
|---|---|---|
| [Mindustry](https://github.com/Anuken/Mindustry) | 로고·실제 테스트 배지, 기여 안내, 플랫폼별 빌드, 빌드 산출물·다운로드 경로 | 빌드 안내와 실행 파일 다운로드를 구분. CI가 생기면 실제 결과를 표시 |
| [VVVVVV](https://github.com/TerryCavanagh/VVVVVV) | 로고, 게임·제작자 소개, 공개 배경 글, 소스 위치와 개인 빌드·배포 조건, 크레딧 | 개발자의 변과 제작자 이름을 앞에서 찾게 하고 코드·배포 조건을 연결 |
| [Endless Sky](https://github.com/endless-sky/endless-sky) | 게임 소개, 설치·요구 환경, 소스 빌드, 기여·로드맵, 코드와 미술의 라이선스 구분 | 최소 환경·검사 환경 분리, 코드·자원별 조건, 다운로드할 버전 구분 |
| [Mesabloo/hm-defense](https://github.com/Mesabloo/hm-defense) | 복원 동기, 빌드 명령, 원작 화면, 무수익 목적, 코드 BSD-3-Clause | 개인적인 복원 이유와 빌드 가능한 프로젝트를 함께 설명 |

HeavyMach는 내가 요청한 플레이 자료 전체를 README에 유지한다.

## 현재 점검 결과

| 항목 | 확인한 상태 | 처리 |
|---|---|---|
| GitHub 기본 화면 | 새 README를 main에 병합했고 서버 렌더 확인 | 완료 |
| 개발 배경·기여 구분 | 개발자의 변, 나의 기획·플레이와 Codex의 구현·검사 구분 | 유지 |
| 플레이 소개 | 이미지 8장·GIF 2개·MP4 3개, 장면·시간·해시 기록 | 유지. 원본 녹화 파일은 Git 제외 |
| 기능·기술 문서 | 기능·조작·빌드·구조·트러블슈팅17건·개발 과정·검증·상태·변경 이력 | 새 기능·버전 변경 시 해당 문서 갱신 |
| 라이선스 | 이번 작업에서 MIT·코드 적용 범위·자원 조건·외부 고지 추가 | 원작 공개 허락과 OpenAL 재링크 자료는 후속 확인 |
| 기술 배지 | Git tag v0.12, Apple 플랫폼, arm64, C++20·Objective-C++, Code license MIT | 저장소 안 SVG 사용. MIT 배지는 적용 범위 문서로 연결 |
| 최소 OS·실제 검증 | 설치 문서에 최소 설정, 검증 문서에 기기별 결과 | README에도 최소 설정·실제 검사 환경을 나란히 표시 |
| About | 이전 소개가 남아 있고 홈페이지 링크 없음 | 아래 제안 문구로 갱신 |
| Topics | 없음 | 실제 구현과 장르를 나타내는 태그 추가 |
| Releases | v0.9~v0.12 Git 태그는 있으나 Release 항목 없음 | 소스 릴리스부터 작성. 설치 파일은 서명·배포 조건 준비 뒤 추가 |
| Actions | main의 병합 출발 브랜치만 검사 | 문서·자원 검사부터 자동화하고 실제 빌드 CI 추가 |
| 외부 기여 안내 | CONTRIBUTING·이슈·PR 템플릿 없음 | 재현 정보와 develop 대상 PR 절차 작성 |
| 검색·공유 미리보기 | 별도 이미지 설정 확인 전 | 기존 플레이 캡처로 공유 이미지 준비·설정 확인 |
| 공개 범위 | 저장소는 비공개 | 코드·에셋 포함 공개 조건과 외부 자료 범위 정리 후 공개 준비 |

GitHub Community Profile은 기본 브랜치의 안내 파일 존재를 확인하는 자료로 사용했다. 게임 완성도나 문서의 정확성을 평가하는 점수로 사용하지 않았다.

## README 배치

제목과 게임 소개 바로 아래에 기술 배지를 두었다. 다음에는 개발 동기, 대표 게임 화면, 버전·설치·조작 링크가 이어진다. 전투·성장·새 적을 보여주는 이미지와 영상은 각 기능 설명 옆에 유지한다. 뒤에는 지원 환경, 빌드, 트러블슈팅, 구조·버전, 라이선스를 둔다.

배지는 외부 서비스 요청 없이 저장소의 SVG를 표시한다. 플랫폼 배지는 구현된 플랫폼을 뜻하며, 최소 OS의 모든 버전에서 검사했다는 뜻은 아니다. Game tag v0.12와 앱 번들 버전 1.13·빌드 번호 1은 서로 다른 값이다. 게임 태그가 바뀌면 버전 SVG와 README·CHANGELOG를 함께 갱신한다.

## 다음 작업 순서

| 순서 | 작업 | 완료를 확인할 방법 |
|---|---|---|
| 1 | 라이선스·배지·환경 표를 문서 검사 뒤 develop→main에 반영 | GitHub 기본 README의 배지5개, 링크, 라이선스 범위, 기존 이미지 유지 확인 |
| 2 | About·Topics·공유 이미지 정리 | 저장소 첫 화면의 소개·태그와 링크를 공유했을 때 표시되는 이미지 확인 |
| 3 | CONTRIBUTING과 버그·기능 제안·PR 양식 | 미션·플랫폼·버전·재현 순서·예상/실제 동작·화면 자료 입력. PR 기본 대상 develop, 기존 데이터·게임 규칙 보존 안내 |
| 4 | 문서·자원 CI | Markdown 링크·원고 대응·LFS 자원 해시 검사. 문서만 바뀐 PR과 소스가 바뀐 PR을 구분 |
| 5 | 코어·Apple 빌드 CI | 공용 코어 검사 후 macOS runner에서 Simulator 빌드. 서명 키 없이 가능한 범위부터 실행. 실제 성공 뒤 빌드 배지 연결 |
| 6 | v0.12 소스 릴리스 정리 | 태그·변경 내용·플랫폼·빌드법·알려진 한계·자원 포함 여부를 기록. 새 폴더에 받은 릴리스 자료로 자원·빌드 검사 |
| 7 | 공개·앱 배포 준비 | 원작 자원 이용 범위, 외부 코드 조건, LGPL 재링크, 개인정보·서명 자료, 공개 이력 확인. 실행 파일은 플랫폼별 배포 조건과 검사 결과를 붙임 |

1번은 이번 작업 범위다. 2~7번은 후속 계획이며 실행한 항목만 상태를 갱신한다. 앱 빌드·실기기·성능 검사는 이번 정적 문서 작업에서 다시 실행하지 않았다.

### About와 Topics 제안

About: `HeavyMach rebuilt for iPhone, iPad and native Mac — shared C++ core, touch and keyboard controls, auto-aim, and development notes.`

Topics: `heavymach`, `game-preservation`, `2d-game`, `side-scroller`, `ios`, `ipados`, `macos`, `cpp`, `objective-cpp`, `opengl`.

홈페이지는 공개된 프로젝트 소개 페이지가 준비되면 연결한다. 현재 통합 검수 사이트는 소유자 로그인용이므로 공개 홈페이지 링크로 쓰지 않는다. Android는 구현 후 지원 플랫폼에 추가한다. [GitHub Topics 안내](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/classifying-your-repository-with-topics)

### 릴리스와 배포 자료

Git 태그는 코드의 특정 시점을 가리키고, Release는 그 태그의 설명·다운로드 파일을 제공한다. v0.12 태그를 옮기지 않고 기존 태그의 설명부터 작성한다. GitHub의 자동 소스 ZIP에 LFS 실파일이 포함되는지 설정과 실제 다운로드를 확인한다. [GitHub Releases](https://docs.github.com/en/repositories/releasing-projects-on-github/about-releases)

현재 iPhone·iPad 개발 서명 앱을 모든 기기에 설치할 수 있는 파일로 안내하지 않는다. 직접 빌드하는 경로와 향후 일반 이용자용 배포 경로를 구분한다. Mac도 설치 파일을 올리기 전에 서명·공증과 새 환경에서의 실행을 확인한다. Android APK는 개발 후 제공한다.

공유 미리보기는 기존 플레이 화면으로 준비한다. 원본 캡처는 보존하고 별도 배치 파일을 만든다. 게임 이름·전투 장면이 작은 카드에서도 읽히는지 확인한다. [GitHub 공유 이미지 안내](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/customizing-your-repositorys-social-media-preview)

[전체 문서](../README.md) · [라이선스·배지 조사](github-licenses-and-badges-20261028.md) · [남은 과제](../usability-todo.md)
