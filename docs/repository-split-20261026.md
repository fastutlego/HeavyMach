# HeavyMach 독립 저장소 분리

2026-10-26 사용자 지시에 따라 GameDev 허브 아래 독립 저장소를 구성했다. 기존 `/Users/sokol/Git/ProjectGameDev`는 미커밋 작업·원본·빌드·검사 증거로 유지한다. 새 작업 위치는 `/Users/sokol/Git/ProjectGameDev-Hub/HeavyMach`다.

원본 제작 HEAD는 `90a7fc684571130e4209c86fe83f8f0b17250f3a`다. 파일별 이력을 git-filter-repo 2.47.0의 source/target 방식으로 별도 저장소에 추출했다. 원본 저장소의 이력은 다시 쓰지 않았다. 경로가 바뀌어 커밋 SHA가 달라지며 `docs/migration/commit-map.txt`에 대응표를 보존했다. 제품 트리 대조는 `docs/migration/product-tree-proof.json`에 있다.

브랜치와 복원은 [저장소 관리](repository-management.md)를 따른다. 서버 branch protection은 현재 비공개 저장소 요금제에서 사용할 수 없으며 로컬 main 경로 검사와 PR 검사를 추가했다. 실제 Git 기록을 사용하는 독립 테스트 8개가 통과했다.

## 새 경로 검사

- CMake Debug 코어 17/17 통과.
- arm64 Simulator의 일반·치트 Xcode 빌드 성공. 서명·재설치·실기기 플레이는 새로 실행하지 않았다.
- 처음 generic Simulator 빌드는 OpenAL 정적 라이브러리 입력 누락으로 실패했다. 당시 script는 라이브러리 생성을 출력했으므로 로그를 보존했다. 라이브러리 준비 후 기존 스크립트가 지원하는 arm64를 지정한 두 scheme은 성공했다. README에 준비 명령을 추가했고 제품 소스·프로젝트 구성은 바꾸지 않았다.
- 로그는 build/repository-split-normal.log, repository-split-normal-arm64.log, repository-split-cheat-arm64.log와 repository-split-core/Testing/Temporary/LastTest.log에 보존한다.
