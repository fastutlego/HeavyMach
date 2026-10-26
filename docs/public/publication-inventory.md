# SaaS 집필용 HeavyMach 문서 안내

SaaS 담당 세션이 HeavyMach의 자료를 읽고 Apps·Mags를 작성할 때 쓰는 안내다. 이 저장소는 개인 경험 원문, 개발 과정, 구현 범위, 실행 근거와 이미지를 보관한다. Apps·Mags 본문 작성과 번역·서비스 반영은 SaaS 세션에서 한다.

## 읽는 순서

1. [개발자의 변](https://github.com/gelb2/HeavyMach/blob/main/docs/RESTORATION.md)과 [개발 동기 자료](https://github.com/gelb2/HeavyMach/blob/main/docs/developer-background.md)를 읽는다. 후자의 사용자 원문·연도 정정·MachDefense·Unity 초기 기획·SaaS 발췌까지 따라간다.
2. [정본](https://github.com/gelb2/HeavyMach/blob/main/docs/canonical.md), [게임 기획](https://github.com/gelb2/HeavyMach/blob/main/docs/game-design.md), [현재 상태](https://github.com/gelb2/HeavyMach/blob/main/docs/current-status.md), [구현과 검증 범위](https://github.com/gelb2/HeavyMach/blob/main/docs/public/implementation-status.md)에서 현재 제품과 남은 작업을 확인한다.
3. [IPA 포팅과 네이티브 복원 과정](https://github.com/gelb2/HeavyMach/blob/main/docs/public/ipa-port-and-native-app-20261026.md), [기술 회고](https://github.com/gelb2/HeavyMach/blob/main/docs/public/engineering-retrospective-20261025.md), [REA 활용 내역](https://github.com/gelb2/HeavyMach/blob/main/docs/public/rea-restoration-method-20261025.md)을 읽는다. REA 문서의 함수·도구·증거 부록을 포함한다.
4. 아래 표에서 쓸 주제의 원인·수정·검사 기록을 확인한다. 과거 기록의 ‘미설치’·‘실패’를 현재 상태로 옮기지 않는다.
5. [이미지 색인](https://github.com/gelb2/HeavyMach/blob/main/docs/public/media/README.md)과 [글쓰기 스킬](https://github.com/gelb2/HeavyMach/blob/main/.agents/skills/write-project-text/SKILL.md)을 읽는다. 문장별 사실·수치·이미지를 출처와 대조한 뒤 SaaS의 현행 콘텐츠 구조에 맞춰 집필한다.

저장소에서는 `python3 tools/document_contract.py plan --task developer_story`로 필수 문서 경로를 출력할 수 있다. 이 안내에서 빠진 기술 세부사항은 [문서·소스 연결표](https://github.com/gelb2/HeavyMach/blob/main/docs/document-map.json)로 찾는다.

## 주제별 근거

| 집필 주제 | 먼저 읽을 자료 | 확인할 내용 |
|---|---|---|
| 왜 HeavyMach인가 | RESTORATION, developer-background와 연결된 원문 | 2015년 잡지사·2016년 필리핀·2017년 7월 전업, 본인이 평가한 영향 35%, 2026년 복원 이유 |
| 원본과 두 판본 | source-provenance, Compatibility/README, public/ipa-port-and-native-app-20261026 | IPA 바이너리 호환 실행과 C++·Objective-C++ 소스 복원 구분. Swift로 만든 앱이라고 쓰지 않음 |
| 성능 문제와 잘못된 구현 | compat-source-comparison-20261024, opengl-migration-20261024, runtime-policy | UIKit 게임 렌더, 음향 재시작, 누적 시간 보충, 잦은 저장을 원작·호환판과 대조해 수정한 과정 |
| 전체 화면과 입력 | coordinate-policy, weapon-result-fix-20261025, boss-motion-fix-20261025 | 넓어진 시야, 필살기 해제 좌표, 보스의 표시 준비와 전투 활성 분리 |
| 일러스트와 메뉴 아트 | clear-illustration-fix-20261025, menu-art-extension-20261025, menu-joins-20261025 | 원본 이미지 복구, 좌우 배경 생성과 합성, 연결부·검은줄 수정 |
| 사용자와 코덱스의 역할 | 기술 회고, 사용자 원문, current-status | 사용자의 비교 기준·수정 지시·실제 폰 플레이와 코덱스의 분석·구현·검사를 구분 |
| 실제 동작·성능 | physical-self-test-20261025, release-review-20261025, instruments-validation-20261024, current-status | 전체 미션 사용자 보고, 자동 검사, 127.77초 계측을 서로 바꾸어 설명하지 않음 |
| 설치와 현재 완료 범위 | public/build-and-install, public/implementation-status | 자원·서명 준비, 일반·치트 앱, v0.9, 미완료 항목 |
| 코드·문서 보존 | ios-source-map, ios-completeness-audit-20261026, migration/completeness-audit-20261026, repository-management | 독립 저장소 분리, 누락 검사, develop→main과 태그 |
| 다음 개발 | usability-todo, android-port-plan-20261026, related-titles-20261026 | 발열·회전·나머지 화면·단일 문 전환, Android APK 미구현 |

표의 경로는 특별한 표시가 없으면 저장소 `docs/` 아래 Markdown 파일이다. 각 보고의 코드·정책·실행 증거 링크를 따라갈 수 있다. 당시 로컬 `build/` 계측 원본과 IPA는 Git에 포함하지 않으며, 그 근거와 보존 위치는 해당 보고에 기록돼 있다.

## GitHub용 기술 문서

[빌드와 설치](https://github.com/gelb2/HeavyMach/blob/main/docs/public/build-and-install.md)는 clone·Git LFS 자원 받기, 일반·치트 앱 빌드, Simulator 실행, 본인 서명으로 아이폰에 설치하는 순서를 설명한다. 빌드·검사 자원은 저장소에 포함한다. IPA 추출과 기존 Mac에서 자원을 복사하는 단계는 필요하지 않다.

[구현과 검증 범위](https://github.com/gelb2/HeavyMach/blob/main/docs/public/implementation-status.md)는 만든 기능, 사용한 기술, 실행 결과, 남은 작업과 배포 상태를 설명한다. README에서 두 문서로 바로 이동한다.

## 모은 이미지와 앞으로 받을 영상

기존 보고 이미지 13개를 저장소 `docs/public/media/`에 보존했다. 타이틀·모드·미션의 수정 전후, 슬롯 배치, 생성 후보가 포함돼 있다. [이미지 색인과 해시](https://github.com/gelb2/HeavyMach/blob/main/docs/public/media/README.md)에서 실제 게임 캡처와 생성 후보, Simulator와 아이폰 자료를 구분한다.

사용자가 제공할 플레이 영상은 아직 도착하지 않았다. 도착하면 [영상·README 작업 절차](https://github.com/gelb2/HeavyMach/blob/main/docs/media-publication.md)에 따라 원본을 보존하고 장면별 이미지·클립·캡션을 만든다. SaaS 세션은 같은 파생 파일과 타임코드를 사용할 수 있다.

## 아직 없는 근거

| 자료 | 현재 상태 | 이후 문서에 추가할 내용 |
|---|---|---|
| 새 clone 빌드·실행 | 원격 clone의 빌드4개, 코어17·iOS14, 새 Simulator 두 앱 실행 통과 | 새 clone으로 실제 아이폰 서명·재설치는 이번 검사에서 미실행 |
| 이미지 비교 테스트 입력 | ImageDecodeParity.json을 Git에 포함. 새 clone에서 이미지695개 디코딩 비교 통과 | 추가 입력 준비 없음 |
| 장시간 발열·전력 비교 | 127.77초 실기기 계측, 전체 미션 사용자 플레이 보고 | 동일 조건의 호환판·복원판 장시간 비교 결과 |
| 원작자 답변과 배포 조건 | 사용자 문의 기록과 무료 배포 의향 | 실제 답변·조건을 받으면 원문과 날짜 추가 |
| 게시용 플레이 영상 | 사용자 후속 제공 예정 | 촬영 앱·기기·버전, 장면 타임코드, 이미지·클립과 설명 |

GitHub Release 등록·SaaS 편집·번역·대표 컷 선정은 집필·게시 작업이다. 위의 미확보 실행 근거와 구분한다. 현재 GitHub Release는 없으며, 저장소와 검수 사이트는 비공개다.

## SaaS 담당 세션의 적용 범위

조회한 SaaS 웹 소스에는 HeavyMach Apps·Mags 항목이 없었다. Apps는 `src/entities/content/apps.ts`와 `src/shared/i18n/canonical/apps.tsx`, 언어별 콘텐츠를 사용한다. Mags는 `src/entities/content/issues.ts`의 호 등록과 본문·번역·공유 카드·챗봇 자료가 연결된다. 집필·게시는 SaaS 저장소의 현행 지침을 읽고 진행한다.

개발자의 변과 사용자 정정은 개인 서사의 기준이다. 설치·구현·검사 수치는 기술 문서와 실행 기록을 기준으로 쓴다. 메타카피·자기합리화·근거 없는 성과를 추가하지 않고, 실제로 없었던 인터뷰 질문·답변도 만들지 않는다. 이번 작업은 HeavyMach 내부 자료 정리이며 SaaS 본문·운영 데이터는 변경하지 않았다.

[새 clone 빌드 검증](clone-build-20261026.md)은 자원 누락 수정과 원격에서 받은 파일만으로 수행한 빌드·실행 결과다. 앞선 로컬 복사 검사와 구분해 인용한다.
