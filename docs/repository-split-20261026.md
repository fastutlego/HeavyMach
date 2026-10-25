# HeavyMach 독립 저장소 분리

2026-10-26 사용자 지시에 따라 GameDev 허브 아래 독립 저장소를 구성했다. 기존 `/Users/sokol/Git/ProjectGameDev`는 미커밋 작업·원본·빌드·검사 증거로 유지한다. 새 작업 위치는 `/Users/sokol/Git/ProjectGameDev-Hub/HeavyMach`다.

원본 제작 HEAD는 `90a7fc684571130e4209c86fe83f8f0b17250f3a`다. 파일별 이력을 git-filter-repo 2.47.0의 source/target 방식으로 별도 저장소에 추출했다. 원본 저장소의 이력은 다시 쓰지 않았다. 경로가 바뀌어 커밋 SHA가 달라지며 `docs/migration/commit-map.txt`에 대응표를 보존했다. 제품 트리 대조는 `docs/migration/product-tree-proof.json`에 있다.

브랜치와 복원은 [저장소 관리](repository-management.md)를 따른다. 서버 branch protection은 현재 비공개 저장소 요금제에서 사용할 수 없으며 로컬 main 경로 검사와 PR 검사를 추가했다. 실제 Git 기록을 사용하는 독립 테스트 8개가 통과했다.
