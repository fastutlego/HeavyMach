# 플레이 영상에서 고른 이미지와 클립

Codex가 내 플레이 영상 두 개에서 이미지 8장과 클립 3개를 추출했다. README는 **02 점프와 사격**으로 원작의 틸트·터치 전투를 먼저 소개한다. **01 자동조준 표적**과 **08 점프·사격**은 추가 항공기 소개 다음의 자동조준 절에 배치했다.

![탱크가 공중의 적을 향해 사격한다. 녹색 표적 표시와 LOCKED 문구가 보인다.](media/gameplay-20261027/01-auto-aim.png)

## GitHub README 구성 {#section-0}

후속 지시를 반영한 [GitHub 문서 전체 작성 계획](github-documentation-plan-20261027.md)에서 트러블슈팅·추가 기능·상태표·문서별 범위를 정했다.

README에는 이미지 8장을 모두 사용한다. 원작의 전투·무기·성장 장면을 먼저 보여주고, 내가 추가한 항공기·자동조준 장면을 그다음에 보여준다.

| 순서 | 내용 | 사용할 이미지 |
|---|---|---|
| 1 | 원작의 틸트·터치·점프·사격 | 02 점프와 사격 |
| 2 | 원작의 무기 선택과 필살기 | 03 무기 선택, 04 상공 공격 |
| 3 | 원작의 성장과 미션 진행 | 05 업그레이드, 06 스테이지 클리어 |
| 4 | 내가 추가한 항공기 | 07 항공기와 전투 |
| 5 | 내가 추가한 자동조준 | 01 표적 표시, 08 자동조준 중 점프·사격·무기 목록 |
| 6 | 지원 플랫폼·조작·설치 | 현재 구현과 빌드·설치 문서 연결 |
| 7 | 구현 내용·개발 과정 | 공용 코어, 렌더링, 화면 확장, 입력, 저장·음향 수정 |
| 8 | 버전·남은 작업·제작자 | 릴리스, TODO, 원작 제작자와 사용 라이브러리 정보 |

소개에는 개발 동기, 현재 기능, 구현과 수정 과정, 설치 방법을 담는다.

[README](../../README.md)와 [구현 상태](implementation-status.md)를 v0.12에 맞춰 갱신했다.

## GIF와 영상 {#gif-video}

현재 파일은 **PNG 8장·GIF 2개·MP4 3개**다. MP4는 각각 7초·13초·7초이며 원래 재생 속도와 소리를 유지한다.

01 자동조준은 원본 V2 9.5~14.5초의 5초, 08 점프는 37~43초의 6초로 GIF를 만들었다. 768×354·12fps·128색이며 원래 속도와 전체 화면 비율을 유지했다. 용량은 각각 3,320,299·3,223,384바이트다. [GIF와 재추출 방법](media/gameplay-20261027/README.md).

각 GIF 아래에 소리가 있는 MP4 링크를 둔다. 07 항공기 클립은 여러 기체의 진입과 전투를 보여주므로 13초 MP4 전체를 연결한다. SaaS 웹에서는 제공된 MP4와 대표 이미지를 영상 플레이어에 사용할 수 있다.

GitHub는 첨부 이미지·GIF를 10MB까지, 동영상은 무료 플랜 저장소 10MB·유료 플랜 저장소 100MB까지 받으며 H.264를 권장한다. 이는 첨부 업로드 기준이다. README에는 이미지·GIF와 MP4 링크를 배치한다. 인라인 영상은 GitHub 재생 확인 후 적용한다. [GitHub 첨부 문서](https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/attaching-files).

## 개인 프로젝트의 GitHub 운영 절차 {#github-workflow}

README는 프로젝트가 하는 일, 사용 방법, 도움을 받을 곳과 제작자를 안내한다. 저장소 안의 문서와 이미지는 상대 경로로 연결할 수 있다. [GitHub README 문서](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/about-readmes).

아래는 GitHub 공식 문서와 현재 저장소 규칙을 HeavyMach에 적용한 순서다.

| 단계 | HeavyMach에서 할 일 | 확인할 결과 |
|---|---|---|
| 저장소 소개 | About에 게임 설명·플랫폼·프로젝트 링크를 적고 관련 topics를 붙인다 | 첫 방문자가 게임과 지원 환경을 알 수 있음 |
| README 작성 | 개발 동기, 이미지 8장, 짧은 GIF, 조작, 설치, 구현 내용, 제작자 순으로 작성한다 | 화면과 설명이 대응하고 링크가 열림 |
| 설치 재현 | clone·Git LFS·자원 검사·workspace·스킴·서명 순서를 현행 구성과 대조한다 | 일반 앱과 치트 앱의 시작 방법을 구분함 |
| 작업 관리 | TODO를 기능·버그별 Issue로 옮기고 필요한 항목만 milestone으로 묶는다 | 증상·완료 조건·관련 문서와 버전이 연결됨 |
| 변경과 검토 | develop에서 작업 브랜치를 만들고 PR을 develop 대상으로 연다 | 변경 이유와 관련 검사 결과를 검토할 수 있음 |
| 릴리스 | develop만 main에 병합하고 해당 병합 커밋에 새 버전 태그를 단다 | 기존 태그를 옮기지 않고 이전 버전으로 돌아갈 수 있음 |
| 릴리스 안내 | 태그의 변경점·설치 방법·알려진 문제를 GitHub Release에 적는다 | 소스 태그와 실제 배포 파일의 범위가 분명함 |
| 유지보수 | 버그 수정 후 기능·설치 문서와 스크린샷을 함께 갱신한다 | README가 현재 앱을 설명함 |

태그와 GitHub Release는 별개다. Release는 기존 태그에 설명과 파일을 붙일 수 있다. LFS 자원이 필요한 프로젝트이므로 자동 생성 ZIP에 자원이 들어가는지도 확인한다. [GitHub 릴리스 관리 문서](https://docs.github.com/en/repositories/releasing-projects-on-github/managing-releases-in-a-repository).

## 조사한 학습 자료와 에이전트 스킬 {#github-skills}

GitHub Skills는 연습 저장소에서 브랜치·PR·릴리스를 따라 만드는 강좌다. 아래 표에는 강좌와 에이전트 스킬을 함께 모았다.

| 자료 | 다루는 범위 | HeavyMach 적용 |
|---|---|---|
| [GitHub Skills: Introduction to GitHub](https://github.com/skills/introduction-to-github) | 브랜치, 커밋, PR, 병합 | 저장소 작업 절차의 기초 참고 |
| [GitHub Skills: Release-based workflow](https://github.com/skills/release-based-workflow) | 태그, 릴리스 노트, 릴리스·핫픽스 흐름 | develop→main 규칙을 유지하며 릴리스 안내에 적용 |
| [crafting-effective-readmes](https://github.com/softaworks/agent-toolkit/blob/main/skills/crafting-effective-readmes/SKILL.md) | 독자·프로젝트 종류별 README 구성, 오래된 설명 점검 | 개인 프로젝트 구성과 누락 점검에 참고. 내가 제공한 원문과 글쓰기 규칙을 우선 |
| [gh-address-comments](https://github.com/openai/skills/blob/main/skills/.curated/gh-address-comments/SKILL.md) | PR 의견 수집·수정 | 실제 검토 의견이 생겼을 때 사용 후보 |
| [gh-fix-ci](https://github.com/openai/skills/blob/main/skills/.curated/gh-fix-ci/SKILL.md) | GitHub Actions 실패 로그 조사·수정 | 검사 실패의 원인을 찾을 때 사용 후보 |
| [yeet](https://github.com/openai/skills/blob/main/skills/.curated/yeet/SKILL.md) | 변경 추가·커밋·push·PR 생성 | 현재 프로젝트에 그대로 채택하지 않음. 전체 변경 추가와 기본 브랜치 처리 규칙을 이 저장소 규칙에 맞춰야 함 |

현재 적용하는 스킬은 저장소의 `write-project-text`와 `present-game-artifacts`다. 외부 후보는 본문을 읽어 조사했으며 설치하지 않았다. 조사한 범위에는 README·미디어·설치·PR·릴리스를 모두 맡는 단일 스킬이 없었다. HeavyMach에서는 이 문서의 순서를 작업 절차로 삼고, 각 단계에 필요한 도구와 스킬을 적용한다.

## SaaS 전달 {#sass-delivery}

Apps·Mags를 쓰는 세션에는 이미지 8장과 MP4 3개, 장면 명세와 이 배치안을 함께 전달한다. Apps는 기술 설명, Mags는 매거진 인터뷰 형식으로 구성하며 두 곳 모두 현재 기능과 시정 과정에 맞춰 배치한다.

## 채택한 이미지·클립 {#section-1}

### 01-auto-aim · 자동조준 표적

**V2 · 00:11.500 부근**

탱크가 공중의 적을 향해 사격한다. 녹색 표적 표시와 LOCKED 문구가 보인다.

선정 이유: 탱크, 공중 적, 조준 표지와 하단 버튼이 함께 보여 자동조준 설명에 적합하다.

사용 위치: GitHub 대표 이미지 · Sass Apps 기능 소개.

클립: 00:09.000–00:16.000 · 7초. [MP4 열기](media/gameplay-20261027/01-auto-aim.mp4)

<video controls playsinline preload="none" poster="media/gameplay-20261027/01-auto-aim.png" aria-label="자동조준 표적"><source src="media/gameplay-20261027/01-auto-aim.mp4" type="video/mp4"></video>

### 02-jump · 점프와 사격

**V1 · 00:56.000 부근**

![탱크가 경사로 위로 뛰어오른 상태에서 오른쪽 적을 향해 사격한다.](media/gameplay-20261027/02-jump.png)

탱크가 경사로 위로 뛰어오른 상태에서 오른쪽 적을 향해 사격한다.

선정 이유: 탱크와 바닥 사이 간격이 뚜렷하고 적과 탄환도 구분된다.

사용 위치: GitHub 조작 설명 · Sass Apps 플레이 소개.

### 03-weapon · 무기 선택과 발사

**V1 · 00:44.015 부근**

![상단 무기 목록에서 청록색 테두리가 선택한 무기를 가리킨다. 흰 궤적이 전장을 가로지른다.](media/gameplay-20261027/03-weapon.png)

상단 무기 목록에서 청록색 테두리가 선택한 무기를 가리킨다. 흰 궤적이 전장을 가로지른다.

선정 이유: 선택 목록, 하단 무기 아이콘, 발사 효과를 한 화면에서 볼 수 있다.

사용 위치: GitHub 무기 소개 · Sass Apps 기능 소개.

### 04-special · 상공에서 내려오는 공격

**V1 · 02:44.060 부근**

![여러 줄의 푸른 광선이 상공에서 지상의 적 쪽으로 내려온다.](media/gameplay-20261027/04-special.png)

여러 줄의 푸른 광선이 상공에서 지상의 적 쪽으로 내려온다.

선정 이유: 일반 사격과 다른 공격 효과를 보여준다.

사용 위치: GitHub 무기 소개 보조 · Sass Apps 기능 소개.

### 05-upgrade · 업그레이드

**V1 · 01:52.040 부근**

![UPGRADE 창에 MOVEMENT, DEFENSE, ATTACK 세 항목이 표시된다.](media/gameplay-20261027/05-upgrade.png)

UPGRADE 창에 MOVEMENT, DEFENSE, ATTACK 세 항목이 표시된다.

선정 이유: 전투 외 성장 요소를 보여주며 항목과 선택 상태를 읽을 수 있다.

사용 위치: Sass Apps 성장 요소 · GitHub 상세 소개.

### 06-stage-clear · 스테이지 클리어

**V1 · 02:52.060 부근**

![STAGE CLEAR 문구와 전차 일러스트, 미션 선택 및 다음 스테이지 버튼이 표시된다.](media/gameplay-20261027/06-stage-clear.png)

STAGE CLEAR 문구와 전차 일러스트, 미션 선택 및 다음 스테이지 버튼이 표시된다.

선정 이유: 전투에서 결과 화면으로 이어지는 구성을 보여준다.

사용 위치: Sass Mags 복원 화면 · GitHub 상세 소개.

### 07-fighters · 추가 항공기와 전투

**V2 · 01:40.750 부근**

![검은 항공기 여러 대가 화면 오른쪽에 보이고 탱크의 광선이 그쪽으로 뻗는다.](media/gameplay-20261027/07-fighters.png)

검은 항공기 여러 대가 화면 오른쪽에 보이고 탱크의 광선이 그쪽으로 뻗는다.

선정 이유: 기체 외형과 전투를 함께 볼 수 있다. 클립으로 이동과 무기 전환도 확인할 수 있다.

사용 위치: GitHub 추가 유닛 소개 · Sass Apps 추가 기능.

클립: 01:30.000–01:43.000 · 13초. [MP4 열기](media/gameplay-20261027/07-fighters.mp4)

<video controls playsinline preload="none" poster="media/gameplay-20261027/07-fighters.png" aria-label="추가 항공기와 전투"><source src="media/gameplay-20261027/07-fighters.mp4" type="video/mp4"></video>

### 08-jump-clip · 점프·사격·무기 목록

**V2 · 00:39.500 부근**

![공중 적과 싸우며 점프하는 탱크.](media/gameplay-20261027/08-jump-clip.png)

사격 중 점프와 착지, 무기 목록 표시.

선정 이유: 사격, 점프, 착지와 무기 목록 표시가 이어진다.

사용 위치: GitHub 조작 클립 · Sass Apps 플레이 소개.

클립: 00:37.000–00:44.000 · 7초. [MP4 열기](media/gameplay-20261027/08-jump-clip.mp4)

<video controls playsinline preload="none" poster="media/gameplay-20261027/08-jump-clip.png" aria-label="점프·사격·무기 목록"><source src="media/gameplay-20261027/08-jump-clip.mp4" type="video/mp4"></video>

## 원본과 판독 범위 {#section-2}

| 원본 | 길이 | 영상에서 확인한 내용 |
|---|---|---|
| V1 · 10월 25일 19:38:32 | 2분 58.853초 | 미션 5-3 전투, 무기 목록, 업그레이드, 클리어, 5-4 시작 |
| V2 · 10월 27일 19:39:44 | 2분 4.050초 | 미션 3-4 전투, 자동조준 상태 표시, 항공기 편대 |

전체 구간을 약 4초 간격의 76개 프레임으로 훑고, 후보 6개 구간은 0.5초 간격으로 다시 봤다. 이미지는 회전 정보를 적용한 2736×1260 전체 화면이다. 왼쪽 검은 타원은 원본 영상에 들어 있다. 클립은 원래 속도와 음향을 유지한 H.264/AAC다.

촬영 기종·앱 종류·빌드 번호는 미확인이다. 두 영상에는 서로 다른 UI와 미션이 담겨 있다. 자동조준 ON/OFF 조작, 전체 무기, iPad·Mac 조작은 추가 촬영 항목이다. 성능 수치는 별도의 계측 기록에 있다.

[장면별 시각·원본 해시·파일 명세](media/gameplay-20261027/media-scenes.json)

## 고르지 않은 장면 {#section-3}

- 미션 선택 직후와 클리어 직전의 전환 중 화면: 글자와 그림이 겹치는 순간이 있어 대표 컷에서 제외했다.
- 폭발이 탱크나 기체를 크게 가리는 순간: 공격 효과용 후보와 구분했다. 기체 소개에는 외형을 읽을 수 있는 컷을 쓴다.
- 같은 무기·배경이 반복되는 전투: 이미 고른 컷과 겹치면 제외했다.
- 10월 27일 영상의 마지막 구간: 클리어 화면이 나오기 전에 녹화가 끝나므로 클리어 장면으로 쓰지 않았다.

## Sass 인계 {#section-4}

이미지 ID, 캡션, 대체 텍스트, 타임코드는 `media-scenes.json`에 함께 있다. Sass 세션은 필요한 파일과 해당 항목을 가져가면 된다. 원본 영상은 Git에서 제외했고 파생 이미지·클립과 출처 명세를 저장소에 보관한다. 원본 자료의 위치와 다시 추출하는 명령은 미디어 폴더의 README에 있다.
