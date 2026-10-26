# HeavyMach 보고 사이트

## 최신 게시 — 2026-10-26 전수검사·REA 실행

`#migration-audit`에 독립 저장소 전수검사, 누락 복구, 현재 세션의 REA 디컴파일 실행 결과와 상세 문서 링크를 추가했다.

- 소스: `8181a42ac7fa10f8c43f451fe5484e879ed15b78`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_087130b040f481919dfe9f9b1e0fcf00`.
- 배포: `appgdep_6adece26053c8191a7c283afbeb7d306`, `succeeded`, 2026-10-26T03:51:15.013100+00:00. 소유자 전용 유지.
- 로컬 링크 126개, 기존 앵커·이미지 13개, REA 목록 수, 원고·다운로드 일치, 글쓰기 원본 14개 해시 검사를 통과했다. Chrome의 좁은 창에서 새 절의 본문·링크·줄바꿈을 확인했다. 배포 API 성공을 확인했으며 인증 후 게시 화면은 별도 검사하지 않았다.
- 현행 상세 문서: [전수검사](migration/completeness-audit-20261026.md), [REA 실행 확인](rea-availability-20261026.md).

## 최신 게시 — 2026-10-26 독립 저장소·v0.9

첫 화면 `#repositories`에 허브·HeavyMach·Unity 분리, feature→develop→main, v0.9, 보존 및 검사 결과, 복원 방법을 추가했다.

- 소스: `d0a7126cd0aeb0caac58e4fe0728a5e7863020c6`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_9e7ed69bcd948191975f7ba21b11fb2e`.
- 배포: `appgdep_6ade333d0df48191b79bba969b1dbdaf`, `succeeded`, 2026-10-25T16:50:17.525926+00:00. 소유자 전용 유지.
- Chrome 로컬 미리보기에서 본문·링크·좁은 화면 표시를 확인했다. 로컬 링크 125개, 기존 앵커·이미지, REA 목록 수, 원고와 다운로드 일치, 글쓰기 원본 14개 해시 검사를 통과했다. 배포 API 성공을 확인했으며 인증 후 게시 화면은 별도 검사하지 않았다.
- 현행 Sites checkout은 `build/sites/heavymach-report/`다. 아래 과거 경로와 커밋은 당시 게시 기록이다.

## 최신 게시 — 2026-10-26 IPA 포팅·네이티브 앱 기술 블로그

`/port-and-native.html`에 호환 실행판 제작부터 네이티브 앱의 성능 문제, 비교·수정 순서, 치트·일반 앱과 아이폰 설치까지 설명한 원고를 추가했다. 첫 화면에 진입 링크를 추가하고 같은 Markdown을 다운로드로 제공한다.

- 소스: `8aa294f27cc32d35a4bab48d920550a29576db6d`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_eee3d3ab5d5c81919096e2ab2e210717`.
- 배포: `appgdep_6ade29cb097c8191a5feb1729924d2b9`, `succeeded`, 2026-10-25T16:09:57.787643+00:00. 소유자 전용 유지.
- Chrome 로컬 미리보기에서 좁은 화면의 본문·목차 이동·근거 표와 긴 경로 줄바꿈을 확인했다. 로컬 링크 124개, 기존 앵커·이미지 13개 보존, 원고/다운로드 바이트 일치, 문서 구조 검사를 통과했다.
- 배포 API 성공을 확인했다. 인증 후 게시 화면은 별도로 검사하지 않았다. 기존 REA 전문·회고·문서 보고 URL을 유지했다.

## 최신 게시 — 2026-10-26 과장된 표현 수정

보고서 4개와 조사·TODO 문서의 제목·본문을 검토하고 표현 46곳을 수정했다. “가로 화면 회전 지원”, “수정 내역”, “REA 활용 내역”처럼 기능·작업명을 직접 썼다. 글쓰기 스킬에 작은 기능을 서사·교훈·선언으로 포장하지 않는 기준과 예시를 추가했다.

- 소스: `ddc24fcba37b9a0e92f67b90217660a05443e4c0`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_b79bef91643c8191b7eb841a9f4843eb`.
- 배포: `appgdep_6ade27c83e5481919d863cc1037641ed`, `succeeded`, 2026-10-25T16:01:24.728249+00:00. 소유자 전용 유지.
- 수치·인라인 코드 보존, 원고/다운로드 일치, 로컬 링크 108개, 기존 앵커·이미지, 스킬 형식과 문서 구조 검사 통과. 레이아웃 변경 없이 문구만 갱신했다. 이번 게시본의 브라우저 검사는 별도로 실행하지 않았다.


## 최신 게시 — 2026-10-25 REA 기록과 보고서 전면 개정

기존 첫 화면의 모든 보고 절, 기술 회고, 문서 체계 보고를 `write-project-text` 스킬로 개정했다. REA 복원 전문과 8개 메타데이터 부록을 추가했다. 현재 설치·사용자 플레이 결과를 먼저 두고 과거 계측·대기 상태와 구분했다. 기존 URL·절 앵커와 이미지 13개를 보존했다.

- 소스: `c2e59324507f3467ad3783d9e069171294a97778`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_40ff16f5c2e0819180a060b8bee4a631`.
- 배포: `appgdep_6addff1e90288191ba025c2a748bd98f`, `succeeded`, 2026-10-25T13:07:56.316275+00:00. 소유자 전용 유지.
- 전문: `/retrospective.html`, `/documentation.html`, `/rea.html`. Markdown 다운로드는 저장소 원고와 바이트 일치.
- 정적 검사: 로컬 링크 108개, 기존 앵커, 이미지 바이트, 원고/다운로드, REA 목록 수, 복사 스킬 원본 14개 해시 통과.
- Chrome 좁은 창에서 첫 화면·REA 전문·표를 확인했다. Markdown 표에 thead/tbody가 생기면서 첫 데이터 행까지 숨던 기존 CSS를 고치고 실제 표시를 확인했다.
- 공식 배포 API가 성공과 URL을 반환했다. 게시 후 Chrome 이동은 `cgWindowNotFound`로 실패했다. 이전 Chrome의 게시 주소는 로그인 화면까지 확인한 상태이며 이번 인증 후 게시 화면은 직접 확인하지 못했다.
- 제작 도구: `tools/build_report_site.py` (Python Markdown·beautifulsoup4 필요). 기존 Sites checkout의 이미지와 스타일을 재사용한다. 증거 부록은 `tools/export_rea_public_report.py`로 별도 생성한다.


## 최신 게시 — 2026-10-25 문서·코드·앱 일치 체계

`/documentation.html`에 발견·보완·업무별 읽기·실제 대조 표본·검사 범위를 게시했다. 기획/위계/연결표 다운로드도 제공한다. 문구와 링크 구조를 검사했고 기존 기사 레이아웃을 재사용했다.

- 소스: `e970c927b4cdc0e79619e28457c6bed8e16f14f9`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_1bf41dcb82b48191a00c2b5641c4085f`.
- 배포: `appgdep_6addfa403b54819193e89b502c36d55e`, `succeeded`, 2026-10-25 12:47:07 UTC. 소유자 전용 유지.

## 최신 게시 — 2026-10-25 사용자 전체 미션 플레이 결과

사용자가 현재 아이폰 설치본의 모든 미션을 플레이했고 크래시·오동작·적 이동 이상은 없는 것 같다고 보고한 결과를 회고 전문·Markdown 다운로드·보고 첫 부분에 반영했다. AI 전수 조사는 예방 검토로 구분했다. 문구 변경만 수행했으며 기존 레이아웃을 유지하고 원고/웹/다운로드 내용 일치를 검사했다.

- 사이트 소스: `8649e88aed531891e4aef95cdb30b6e17326915a`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_9af2bca2a7988191a7e5562d7b5dcfbb`.
- 배포: `appgdep_6addf6d575108191a9aaeeedf2fe5669`, API `succeeded`, 2026-10-25 12:32:34 UTC. 소유자 전용 유지.

## 최신 게시 — 2026-10-25 19:19 기술 회고·두 앱 설치

- `/retrospective.html`: 지시·지적별 타당성, 잘못된 구현, 수정과 결과, 미완료 범위의 전문.
- `/engineering-retrospective.md`: GitHub·외부 서비스 게시용 Markdown 원고. 저장소 원고와 내용 일치 검사 통과.
- `#retrospective`: 두 앱 재설치·실행 확인 및 전문/다운로드 진입 링크.
- 사이트 소스: `e0881a9fcda1c083c4ee6b33efb4edc0f808cab9`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_f49a3fa7541c8191bd7dfb94a41cf635`.
- 배포: `appgdep_6addd792e9b081918f93e8c2302b2678`, API `succeeded`, 2026-10-25 10:19:11 UTC.
- 소유자 전용 유지. 로컬 Chrome에서 목차와 좁은 화면의 표/본문 배치를 확인했다. 공개 GitHub·외부 서비스 게시를 수행한 것은 아니다.
- [설치 증거](release-review-20261025.md). 전체 화면 단일 문 전환은 구현 전으로 표시했다.

## 최신 게시 — 2026-10-25 18:57 배경 연결·검은줄 수정

- `#menu-joins`: 타이틀·모드·미션 수정 전후 실제 GL 이미지, 검은 POT 여백 재현과 수정, 물리 iPhone Air/Simulator26개 통과를 게시했다. 소유자 전용 유지.
- 사이트 소스: `fc0de67a67bda94112171b898ec96f82a5500b83`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_8be97137a5288191a14a01a1a4772f68`.
- 배포: `appgdep_6addd29490dc8191afb1976f6b13f8fb`, API `succeeded`.
- 로컬 Chrome 최신 페이지의 문구·전후 이미지·반응형 배치를 직접 확인했다. 게시 주소의 기존 Chrome 로그인 게이트는 유지하며 접근 범위를 넓히지 않았다.
- 게임 변경 상세는 [모드·미션 연결과 검은줄 수정](menu-joins-20261025.md). 아래 설치 대기/미검사 문장은 과거 게시 당시 기록이다.

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

## 2026-10-25 17:58 실기기 보고 갱신

- `#phone`에 물리 iPhone Air 기능8/8, 20무기360조건·1,080발, 화면 관측과 자동 검사의 차이, TODO별 검사 방법, 메뉴 재배치+배경 좌우 확장 권고를 게시했다.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_3dee2ea59b588191868eb1f21e711ecd`
- 배포: `appgdep_6addc49b037c8191a0f5c2f2cabffcfd`, API `succeeded`.
- 소스 커밋: `4a7df000385b52836a9dd8fe09248c27b5017508`.
- 로컬 Chrome 실제 화면에서 새 검사 결과의 글자·배치·수치와 접근성 트리를 확인했다. 인증 후 게시 화면 확인 한계는 위와 같다. 소유자 전용을 유지했다.

## 2026-10-25 18:03 메뉴·이미지 제작 계획 갱신

- `#asset-plan`: 메뉴 기준점/터치 동시 재배치, 내장 imagegen의 원본 참조 확장, 원본 중앙과 생성된 좌우 분리 합성, 치트 앱 우선 적용 절차 게시. 이미지 생성·제품 구현 전이다.
- 사이트 소스: `32c458ac0657156dfa9d072ec39dec9a4bb01f13`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_12b03a40fb2c819196e6440a7b021623`.
- 배포: `appgdep_6addc5d8fe5481918ce33c096ecc38fa`, API `succeeded`. HTML 파싱·절 식별자·원문 파일 확인. 이번 문구 추가의 새 브라우저 시각 검수는 하지 않았다.

## 2026-10-25 18:37 메뉴·배경 첫 적용 보고

- `#menu-result`: 타이틀·슬롯·모드·미션의 실제 GL 캡처, 생성 후보A/B, 원본 보존, 터치 검사, 자원 비용을 게시했다. 첫 적용과 나머지 화면의 TODO를 구분했다.
- 사이트 소스: `51e0d4ef3e7470248f729170df014daacdf354a7`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_2036c2456db0819198df71b88048b4f3`.
- 배포: `appgdep_6addcdb603a48191855e52d73a06dbf6`, API `succeeded`, 소유자 전용 유지.
- 관련27개 검사와 후속 경로·타이머 검사 통과를 기록했다. Simulator의 타이틀·슬롯 약27~30ms는 60Hz 통과로 표시하지 않았다. 새 치트 앱은 아이폰 설치 성공, 잠금 때문에 실행 검사 대기다.
- HTML 파싱과 이미지 참조 파일을 확인했다. 새 보고서의 브라우저 시각 검수는 `cgWindowNotFound`로 완료하지 못했다. 현재 앱 목록 조회는 성공했고 서비스/CLI 버전·서명 일치 및 최근 인증 거절0을 확인했다. 근거 없이 서버를 재시작하지 않았다. 진단은 `build/menu-extension-20261025/cua-window-diagnostic.json`에 남겼다.
