# HeavyMach v0.9

정상 플레이가 확인된 HeavyMach를 독립 저장소의 main 병합 커밋으로 고정하는 릴리스다. Git 릴리스 버전은 0.9이며 원본 게임의 1.13 표기와 구분한다. 앱 코드·번들 설정을 바꾸지 않고 기존 설치본의 소스와 같은 버전을 관리한다.

- 분리 전 제품: `72e8c68e57f0a5c51b31481a21528c73f8a48069`.
- 분리 전 문서: `90a7fc684571130e4209c86fe83f8f0b17250f3a`.
- 분리된 제품 이력: `9c0d7639b2988c94d12d2a7e6220d8d62b596c48`.
- 실제 일반·치트 설치와 코드 해시: [설치 기록](release-review-20261025.md).
- 사용자는 설치본의 전체 미션을 플레이하고 크래시·오동작·적 이동 이상은 없는 것 같다고 보고했다.
- Sources·iOS·Art·vendor·Tests의 Git tree ID가 기존 제품과 같다. [내용 대조](migration/product-tree-proof.json), [이력 대응](migration/commit-map.txt).
- 새 경로에서 CMake 코어 검사 17/17과 문서 연결 검사가 통과했다. 새 경로의 arm64 Simulator 일반·치트 Xcode 빌드도 성공했다. 첫 generic 실행 실패와 준비 명령은 개편 기록에 보존했다.

개발은 feature→develop→main 순서로 병합한다. `v0.9`는 main의 병합 커밋을 가리키며 이후 이동하지 않는다. 실제 태그 SHA는 `git rev-parse v0.9^{}`로 조회한다. [복원 절차](repository-management.md)를 따른다.

전체 화면 단일 문·물리 회전 통합 검사·장시간 발열 등 후속 TODO는 유지한다. 원본 전체 동등성이나 모든 조건의 무결함을 선언하는 버전은 아니다. 이번 저장소 분리는 폰 앱 재설치나 사용자 저장 변경을 포함하지 않는다.
