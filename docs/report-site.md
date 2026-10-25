# HeavyMach 보고 사이트

2026-10-25 사용자 요청으로 Sites 스킬을 사용해 생성했다. 이후 보고는 이 사이트를 갱신하고 URL을 제공한다. 원본 정본과 TODO는 저장소에 유지한다. 새 사이트를 매번 만들지 않는다.

- URL: https://heavymach-dev-report-20261025.hiphopxy.chatgpt.site
- project_id: `appgprj_6addbf55aea081918d3d182d7e496e88`
- 공개 범위: 생성 기본값인 소유자 전용. 임의로 공개하지 않는다.
- 최초 게시 상태: Sites API `succeeded`, 2026-10-25 08:42:27 UTC.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_c786469b52f0819182693a4b26e25cae`
- 배포: `appgdep_6addc0e58a4481919e708d9dc1c526d6`
- 사이트 소스 커밋: `f7620d33e8b4a4a1ccdd3d956dc9f843627b3d1a`.
- 로컬 checkout: `HeavyMach/build/sites/heavymach-report-20261025/` (별도 Sites 소스 저장소, 게임 Git에서는 제외).
- 소스가 없거나 다음 작업에서 갱신할 때 Sites `get_site`와 소스 credential을 받아 스킬의 source helper로 연다. 토큰은 파일·문서·로그에 저장하지 않는다.

최초 내용은 적 AI 고정값·확장 화면 간섭, 양방향 가로 회전, 온보딩·메뉴 전체 화면 처리의 세 TODO다. 세 번째 항목은 원본 무변형 배치와 화풍·톤에 맞는 주변 부가 에셋 연결을 비교한다. 세 과제 모두 구현 완료로 표시하지 않았다.

로컬 HTTP200과 Chrome 접근성 트리에서 세 과제와 원문 링크 표시를 확인했다. 세부 UI 조작 중 CUA 시간초과가 발생해 전체 클릭·시각·모바일 검수 완료로 처리하지 않는다. Sites 배포 성공은 API 반환값으로 확인했다.

게시 URL을 Chrome의 새 탭에 열었고 Sites 로그인 게이트에서 OpenAI 로그인 화면으로 이동했다. 이 Chrome에는 로그인 세션이 없어 게시본의 인증 후 화면 검증은 남았다. 접근 범위를 공개로 바꾸지 않았다.
