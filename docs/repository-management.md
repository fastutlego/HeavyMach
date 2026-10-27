# 저장소와 브랜치 관리

2026-10-26에 정한 규칙이다. 허브·HeavyMach·Unity 저장소에 공통 적용한다.

1. 개발은 각 저장소의 `develop`에서 분기한 `feat/*`, `fix/*`, `docs/*` 등 작업 브랜치에서 수행한다.
2. 작업 브랜치는 `develop`에 병합한다. `main`에 직접 병합하지 않는다.
3. `main`에는 `develop`만 병합한다. 릴리스에는 병합 커밋이 남는 merge 방식을 사용한다.
4. 릴리스 태그는 main의 해당 병합 커밋에 붙인다. 기존 태그를 이동하거나 덮어쓰지 않는다.
5. HeavyMach 최초 정상 플레이 기준 버전은 `v0.9`다. Unity 또는 허브의 게임 플레이 완료를 뜻하지 않는다.
6. 게임 코드와 함께 빌드 구성·의존성 버전·에셋 해시·검사 근거를 보존한다. 플레이 저장·서명 키·로컬 빌드는 Git에 올리지 않는다.

## 현재 태그에서 다시 빌드

v0.12는 main의 `eedf245f`다. v0.10 이후 태그는 게임 자원을 Git/LFS에 포함한다. 별도 폴더에서 이전 버전을 빌드할 수 있다.

```sh
git fetch origin --tags
git worktree add --detach ../HeavyMach-v0.12 v0.12
cd ../HeavyMach-v0.12
git lfs pull
python3 tools/verify_build_resources.py
open HeavyMach.xcworkspace
```

재설치 전 현재 앱 컨테이너를 백업한다. 태그는 코드·자원을 복원하고, 앱의 진행 상태는 별도 저장 데이터에서 복원한다. [빌드·설치](public/build-and-install.md)

## v0.9 복원

`git worktree add --detach ../HeavyMach-v0.9 v0.9`로 별도 폴더에 그 버전을 꺼낸다. `v0.9`는 자원을 Git에 넣기 전 태그다. 태그는 이동하지 않는다. 해당 worktree를 빌드하려면 현재 main에서 `git lfs pull`로 받은 `Resources`를 복사하고 현재 자원 목록의 SHA-256과 대조한다. 현재 main의 새 clone은 별도 복사 없이 빌드한다. iPhone 재설치는 해당 소스를 빌드·서명하고 현재 저장을 백업한 뒤 진행한다. Git 태그만으로 폰 설치본이나 저장 데이터가 되돌아가지는 않는다.

후속 변경을 제품에서 취소할 때는 작업 브랜치에서 필요한 revert 커밋을 만들고 develop→main으로 전달한다. 공유 main을 reset하거나 force-push하지 않는다.

## 허브 연결

허브는 `.gitmodules`와 gitlink에 하위 저장소 URL·정확한 커밋을 기록한다. 하위 저장소의 커밋을 먼저 원격에 올린 뒤 허브 포인터를 갱신한다. 과거 허브 커밋을 checkout하면 당시 두 게임의 연결 버전을 확인할 수 있다.

## 검사와 한계

`.githooks/pre-push`는 main의 두 번째 부모가 develop인지 검사하고 Git LFS 업로드를 실행한다. PR 검사도 main의 source branch가 develop인지 확인한다. 로컬 훅을 사용하려면 clone마다 `git config core.hooksPath .githooks`를 설정한다.

현재 GitHub 계정의 비공개 저장소 branch protection API는 요금제 제한으로 403을 반환했다. 서버 branch protection은 설정되지 않았다. 로컬 훅과 PR 검사, 저장소 지침을 적용하며 관리자에 의한 우회까지 차단한 상태는 아니다.

## v0.10 — 2026-10-27

VTOL 크기·외곽선·불꽃 수정 `6404c999`까지 작업 브랜치→develop(`63afa4f5`)→main(`76cbd21c`)으로 병합했다. main 병합 커밋에 annotated tag `v0.10`을 붙였고 develop·main·태그의 원격 반영을 확인했다. v0.9는 이동하지 않았다. v0.10은 Git LFS 자원을 포함한다.

이후 전체 화면 도어 수정은 develop에서 분기한 `fix/fullscreen-transitions`에서 진행한다. v0.10에는 포함되지 않는다. [변경·검사](public/fullscreen-transitions-20261027.md).

## 자동조준 개발 브랜치 — 2026-10-27

`develop`의 `8fdeb629`에서 `feat/auto-aim-ipad-validation`을 분기했다. 내 지시의 dev는 이 저장소의 develop을 가리킨다. 자동조준 개발과 iPad 치트 앱 검증은 이 브랜치에서 진행한다. 완성 전 develop/main 병합과 릴리스 태그 생성은 하지 않는다.

## v0.11 — 2026-10-27

내가 자동조준 작업의 develop→main 병합과 새 버전 태그를 지시했다. `15968772`까지의 자동조준·C 삭제와 이전 전체 화면 도어 변경을 포함한다. `v0.11`은 main 병합 커밋에 붙이며 기존 v0.9/v0.10은 유지한다. C 삭제 검사는 코어20개, 시뮬레이터·iPhone·iPad 각4개, 수동 사격25,200프레임의 기준 버전 일치다. 이전 프레임 계측과 장시간 검사는 남은 과제로 보존한다. 후속 신규 전투기3종은 별도 작업 브랜치에서 진행한다.

실행 결과: develop `e9376910`, main `f0c4432f`, 주석 태그 `v0.11`을 원격에 반영했다. 태그는 main 병합 커밋을 가리킨다. 신규 기획은 develop에서 분기한 `feat/lux-lunae-fighters`에서 진행한다.

## 럭스루나에 develop 반영 — 2026-10-27

Codex가 내 지시에 따라 커밋·dev 병합·브랜치 정리를 진행했다. `feat/lux-lunae-fighters`의 `bd801119`를 develop에 merge commit으로 병합했다. 제품 구현은 `45befedc`이며 화면 밖 진입·연사10% 수정본을 두 기기의 일반/치트 앱으로 설치·실행했다. 병합 충돌은 없었다. main 릴리스와 새 태그는 이번 요청 범위에 포함하지 않는다.

브랜치 정리는 develop의 조상인 작업 브랜치만 대상으로 한다. 로컬은 `git branch -d`, origin은 해당 작업 ref 삭제를 사용한다. main·develop·기존 태그·migration-source 보존 원격은 유지한다. 병합되지 않은 브랜치나 다른 worktree에서 사용 중인 브랜치는 삭제하지 않는다.

## v0.12 — 2026-10-27

내 지시에 따라 남은 문서 작업을 develop에 병합하고, develop을 main에 병합한 커밋에 주석 태그 `v0.12`를 붙인다. 포함 범위는 럭스루나에 전투기3종·금색 조종석·묶음 연사·화면 밖 진입·연사속도10% 조정과 후속 투두/조사 문서다. 아이콘·영상 제작·자동조준 시각 안내·튜토리얼 패키지는 계획/과제이며 구현된 기능으로 세지 않는다.

제품 소스·에셋은 두 기기에 설치·검사한 `45befedc`와 같다. 기존 코어21개·시뮬레이터·실기기·Mac의 유효 검사를 재사용하고 문서 연결·자원 해시·병합 결과를 확인한다. 기존 v0.9/v0.10/v0.11 태그를 이동하지 않는다. 이번 버전은 Git 릴리스 태그이며 앱 번들 버전·새 설치 작업은 포함하지 않는다.
