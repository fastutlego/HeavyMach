# SaaS Apps·Mags 집필 자료

HeavyMach의 사용자 원문·기술 설명·수정 과정·이미지를 이 저장소에 모았다. Apps는 기술 설명, Mags는 매거진 인터뷰 형식이다. 두 곳 모두 현재 기능과 시정 과정을 다룬다. SaaS 담당 세션이 이 자료로 원고를 작성하고 서비스에 반영한다.

## 읽는 순서

1. [개발자의 변](../RESTORATION.md)과 [개발 동기 자료](../developer-background.md): 사용자 원문, 연도 정정, 잡지사·필리핀 근무와 개발자 전업, HeavyMach를 선택한 이유.
2. [README](../../README.md), [기능](features.md), [구현 상태](implementation-status.md): v0.12 게임과 추가 기능, 완료·남은 과제.
3. [개발 과정](development-history.md), [트러블슈팅](troubleshooting.md): 분석·비교 순서, 사용자 질문과 시정 지시, 기술 원인과 결과.
4. [구조](architecture.md), [설치](build-and-install.md), [검증](validation.md): 기술 구성, 실제 사용 절차, 날짜·기기·조건별 수치.
5. [미디어 명세](media/gameplay-20261027/media-scenes.json), [문장 스킬](../../.agents/skills/write-project-text/SKILL.md): 장면·캡션·원본 시간과 집필 규칙.

## Apps 기술 설명에 연결할 자료

| 주제 | 원고 | 세부 근거 |
|---|---|---|
| 게임 기능과 조작 | [기능](features.md), [조작](gameplay-guide.md) | 게임 기획, 입력·계산 정책 |
| 자동조준 | [기능의 자동조준 절](features.md) | AutoAim.hpp·GameSession.cpp, C 삭제와 수동 사격 25,200프레임 기록 |
| VTOL·전투기 | [기능의 항공기 절](features.md) | VTOL.cpp·Fighters.cpp, 수량·무장·배치·실기기 검사 |
| workspace·모듈 | [구조](architecture.md) | Modules/iOS/macOS 프로젝트 설정, 공용 코어·자원 bundle |
| 성능·버그 수정 | [트러블슈팅](troubleshooting.md) | 각 사건의 정책·코드·원시 검사 링크 |
| 설치·운영 | [설치](build-and-install.md), [버전 관리](../repository-management.md) | clone·LFS·서명·무선·태그 |
| 현재 범위 | [상태](implementation-status.md), [검증](validation.md) | 사용자 플레이, 자동 검사, 미구현·남은 검사 |

## Mags 인터뷰 자료

| 이야기 | 사용자 원문·사건 | 기술 설명 |
|---|---|---|
| 왜 HeavyMach인가 | [개발자의 변](../RESTORATION.md), [개인 경험 원문 목록](../developer-background.md) | 2009년의 플레이와 2017년 7월 전업, 2026년 복원 |
| 원작과 무엇이 달랐나 | 사격마다 저장, UIKit 렌더, 시간 누적에 대한 사용자 질문 | [트러블슈팅 1~5절](troubleshooting.md) |
| 직접 해보고 무엇을 발견했나 | 틸트 반전, 필살기 후방 낙하, 보스 움직임, 그림 누락·검은줄 | [트러블슈팅 6~11절](troubleshooting.md) |
| 어떤 게임을 더 만들고 싶었나 | 넓은 화면, 키보드, 자동조준, 실제 기체·도색·무장 조건 | [기능](features.md) |
| 구현자가 틀렸을 때 어떻게 고쳤나 | 원본 비교 요구, C 삭제, 전투기 진입·연사 수정 | [개발 과정](development-history.md), [트러블슈팅](troubleshooting.md) |
| 어디까지 됐나 | 사용자 전체 미션 및 iPhone·iPad 플레이 확인 | [검증](validation.md), [남은 과제](implementation-status.md) |

개인적인 답변의 원문은 `docs/references/developer-background/`에 있다. 필리핀 근무는 2016년, 개발자 전업은 2017년 7월이다. HeavyMach의 영향 약 35%는 사용자의 회고다. 원문·수정 문장·출처 해시는 [자료 목록](../references/developer-background/source-manifest.json)에 보존했다.

## 사용할 이미지와 영상

| 파일 | 보여주는 내용 | 배치 예 |
|---|---|---|
| 01-auto-aim.png / gif / mp4 | 표적·탱크·HUD와 자동조준 사격 | 대표 화면·자동조준 |
| 02-jump.png | 경사로 위 점프와 사격 | 기본 조작 |
| 03-weapon.png | 무기 목록과 발사 궤적 | 무기 전환 |
| 04-special.png | 상공에서 내려오는 푸른 광선 | 공중 지원 공격 |
| 05-upgrade.png | 이동·방어·공격 강화 창 | 성장 |
| 06-stage-clear.png | 결과 일러스트와 다음 미션 버튼 | 미션·일러스트 복원 |
| 07-fighters.png / mp4 | 추가 항공기 편대와 전투 | 신규 유닛 |
| 08-jump-clip.png / gif / mp4 | 사격·점프·착지·무기 목록 | 조작 영상 |

모든 파일은 [playback 자료 폴더](media/gameplay-20261027/README.md)에 있다. PNG는 2736×1260, GIF는 원래 속도의 짧은 미리보기, MP4는 소리를 포함한다. 촬영 기종·앱 종류·빌드 번호는 미확인이다. 파일별 원본 시간·SHA-256·캡션·대체 텍스트를 함께 전달한다.

기존 [그림 색인](media/README.md), [VTOL 시안과 실제 게임](vtol-art-and-loadout-20261027.md), [전투기 시안·실제 게임](lux-lunae-design-20261027.md)에는 미술 비교 자료도 있다.

## 버전과 남은 근거

GitHub 게임 태그는 v0.12이며 이 원고는 `docs/github-publication`에서 작성했다. [변경 이력](../../CHANGELOG.md)과 [전체 문서 목차](../README.md)로 연결된다.

추가로 필요한 자료는 장시간 발열·전력 비교, 추가 적의 전체 미션 난도 검사, 실제 Bluetooth·USB 키보드 조작, 촬영 환경 정보, 원작자 답변이다. 현재의 실행 수치와 실패는 [검증](validation.md)에 모았다.
