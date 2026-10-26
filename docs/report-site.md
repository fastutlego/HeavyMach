# HeavyMach 보고 사이트

## 최신 게시 — 2026-10-26 Mac 객체 해제·일반 앱 추적

`/apple-platforms.html`에 게임 객체 해제24개 확인, XCTest 없는 일반 앱의 메모리 측정, 잠금 상태로 건너뛴 전체 화면 검사와 원시 로그 해시를 추가했다. 실제 제품 코드는 앞선 `58ade7ae`와 같고 추가 검사·문서 커밋은 `d39caa98`다.

- Sites 소스: `e0ec9bfee732b6963da38e37da3bb15b560f868e`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_5cefc2b4c9948191b20da4bd9dd78e02`.
- 배포: `appgdep_6adf5996e5e48191a0c80be31f986e78`, `succeeded`, 2026-10-26T13:46:12.972069+00:00. 소유자 전용 유지.
- 로컬 링크141개, 원고 다운로드 일치, 반복 생성 시 결과 불변을 확인했다. CSS 중복 누적을 제거했다. 기존13개 이미지를 보존했다.
- Mac 잠금 상태여서 새 브라우저 화면 검사는 수행하지 못했다. 전체 화면 검사1개를 통과 수에 포함하지 않았다.


## 최신 게시 — 2026-10-26 Apple 모듈·키보드·성능 검사

`/apple-platforms.html`에 공용 모듈, 키보드, Mac·iPad·iPhone 검사, Instruments CPU·메모리 결과와 실패 범위를 게시했다. `/auto-aim.html`에 후속 개발 계획을 게시했다. 기존13개 이미지를 유지했다.

- 제품 커밋: `58ade7ae` (`feat/apple-workspace-modules`). main·v0.9 변경 없음.
- Sites 소스: `b190878370a67e873ecb8ea4632b54f7274ba97a`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_6841e3631ff88191af66f80fa8e3185c`.
- 배포: `appgdep_6adf500612f8819197f7cdb649d1c552`, `succeeded`, 2026-10-26T13:05:23.936746+00:00. 소유자 전용 유지.
- 생성된 최상위 HTML의 로컬 링크141개와 새 원고2개의 다운로드 일치를 확인했다. Mac 잠금으로 새 브라우저 화면 검사는 수행하지 못했다.
- 최종 커밋을 별도 export에 반영하고 기존 Git 폴더·Codex 원본 폴더·IPA 읽기를 차단해 Mac/iOS Simulator 치트 앱을 다시 빌드했다. 모두 `BUILD SUCCEEDED`. 로그는 `/private/tmp/heavymach-apple-clean.0bWLXm/final-mac.log`, `final-ios.log`다. 앞선 cold-cache 빌드와 구분하며 이번 재검사는 기존 export의 빌드 캐시를 사용했다.


## 최신 게시 — 2026-10-26 새 원격 clone 빌드

`/clone-build.html`에 자원 Git 누락 수정, GitHub clone·LFS 다운로드, IPA 읽기 차단 상태의 빌드4개·코어17·iOS14·두 앱 Simulator 실행 결과를 게시했다. 설치 가이드에서 IPA 추출·자원 수동 복사 요구를 제거했다.

- 소스: `716bf64e9aca2aaeca413893bf1a6a9c93934ac7`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_8327a5c46b948191b6fb26c8fc69a6df`.
- 배포: `appgdep_6adf2db5902c8191ab0d2d7f612a92bb`, `succeeded`, 2026-10-26T10:38:59.379457+00:00. 소유자 전용 유지.
- 로컬 링크246개, 기존 앵커·이미지, 수정 원고5개의 본문·다운로드 일치를 확인했다. 실행 화면2개와 빌드·앱·검사JSON을 게시했다.
- 새 웹페이지의 브라우저 화면 검사는 수행하지 않았다. 게임의 새 Simulator 실행·자동 검사는 직접 수행했다.


## 최신 게시 — 2026-10-26 IPA 없는 빌드·REA 구현 검증

`/independence.html`에 원본 읽기 차단·4개 빌드·앱 자원 해시·Simulator 실행·Core17/iOS14 검사와 Git 자원 상태를 게시했다. `/rea-implementation.html`에 25함수군·증거140건·산출물2,067개·REA 함수 재조회 결과를 연결했다. 설치 문서의 IPA 필수 표현을 수정했다.

- 소스: `a8240d47748ed67d8a63b80950906f5252a182c9`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_c61cacdce1f48191a953becc27710c04`.
- 배포: `appgdep_6adf27b486c48191a2cacdd71f37495b`, `succeeded`, 2026-10-26T10:13:22.069962+00:00. 소유자 전용 유지.
- 새 보고·REA 상세·설치 원고와 웹 본문/다운로드 일치, 로컬 링크226개, 기존 앵커·이미지 보존 검사 통과. 새 실행 화면2개와 증거JSON을 함께 게시했다.
- 게임 빌드와 Simulator 검사는 실제 실행했다. 새 웹페이지의 브라우저 화면 검사는 수행하지 않았다.

## 최신 게시 — 2026-10-26 집필 자료·설치 안내·영상 준비

`/publication.html`에 SaaS 담당 세션의 읽기 순서·주제별 근거·미확보 자료를 정리했다. `/installation.html`에는 GitHub용 설치 가이드, `/implementation.html`에는 구현·검증 범위, `/media-preparation.html`에는 후속 영상의 추출·README 배치 절차를 게시했다. SaaS Apps·Mags 본문은 이번 게시 대상이 아니다.

- 소스: `32275a10020d6ccd55e9fe99b6c11f6cf6f5a7cd`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_9c3a4ad5f30c819199a8818f214da063`.
- 배포: `appgdep_6adf213956248191ba802dcfeaabd47f`, `succeeded`, 2026-10-26T09:45:44.242032+00:00. 소유자 전용 유지.
- 새 원고 4개·웹 본문·다운로드 일치, 로컬 링크 196개, 기존 앵커·이미지 보존 검사 통과. 기존 이미지 13개의 저장소 사본과 해시도 일치했다.
- 설치 가이드의 이미지 복사 코드를 임시 출력에 실행해 원본 695개와 해시를 대조했다. 전체 새 clone 빌드·기기 설치와 영상 편집은 실행하지 않았다. 영상은 수신 전이다.
- 문서 연결 11업무·65참조 통과. CUA 목록의 브라우저 연결은 없었고, 이번 새 페이지의 브라우저 화면 검사는 실행하지 않았다.

## 최신 게시 — 2026-10-26 지인 발언 표현 정정

사용자가 지정한 문장으로 `/restoration.html`과 `RESTORATION.md`를 수정했다.

- 소스: `72e77e1e20b13cb1675840c61f7ebc71650d30ae`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_2f548dbbda10819194213103c1246014`.
- 배포: `appgdep_6adf1bca065081919c1014a24a317da2`, `succeeded`, 2026-10-26T09:22:33.852592+00:00. 소유자 전용 유지.
- 원고·다운로드·HTML 본문 11문단 일치와 수정 문장 반영을 확인했다. 로컬 링크 161개와 기존 이미지·앵커 검사를 통과했다. 새 브라우저 화면 검사는 하지 않았다.

## 최신 게시 — 2026-10-26 개발자의 변 후속 문단

`/restoration.html`과 `RESTORATION.md`에 후속 글감을 반영했다. 본문은 총 11문단이다. 이전 게시 기록의 다섯 문단·후속 대기는 이번 반영 전 상태다.

- 소스: `042e15b882b565b5ed518597878a8d6c3c866e23`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_476e016d3bb48191bccb342fcb93c930`.
- 배포: `appgdep_6adf199c9ab481918c818f2d85db2b71`, `succeeded`, 2026-10-26T09:13:16.170031+00:00. 소유자 전용 유지.
- 원고·다운로드·HTML 본문 11문단 일치, 로컬 링크 161개, 기존 앵커·이미지 13개 보존을 확인했다.
- 이번에는 새 브라우저 화면 검사를 실행하지 않았다. 직전 Chrome 창 접근 실패 기록을 유지한다.

## 최신 게시 — 2026-10-26 개발자의 변

`/#developer-story`와 `/restoration.html`에 이번 사용자 설명까지 작성한 다섯 문단을 추가했다. 2026년에 다시 복원을 택한 이유는 다음 사용자 문단을 받기 전까지 쓰지 않는다. 동일한 `RESTORATION.md`를 다운로드로 제공한다.

- 소스: `2128ce15005902c517e9dce81f9fc9397b5bdece`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_6b6ef773cfec819183c906e4dc14fd32`.
- 배포: `appgdep_6adf1507b99481918d99c434949e0517`, `succeeded`, 2026-10-26T08:53:46.008989+00:00. 소유자 전용 유지.
- 로컬 링크 161개, 기존 앵커·이미지 13개 보존, 원고/다운로드 바이트 일치를 확인했다.
- Chrome 접근은 `cgWindowNotFound`로 실패했다. 새 원고의 브라우저 화면 검사와 인증 후 게시 화면 검사는 미실행이다. 배포 API 성공과 구분한다.

## 최신 게시 — 2026-10-26 iOS 코드·문서 전수검사

`/#ios-audit`와 `/ios-audit.html`에 소스·Xcode 등록·문서·미커밋 변경 검사 결과를 추가했다. 같은 Markdown을 다운로드로 제공한다.

- 소스: `ce2f1deae96136d13ee5019c5c2d48b8b0ec4f76`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_5774121bfd288191b08e2c35cc2c48be`.
- 배포: `appgdep_6aded6230ea4819182b7b00779784dac`, `succeeded`, 2026-10-26T04:25:19.614754+00:00. 소유자 전용 유지.
- Chrome 로컬 미리보기에서 본문·목차 이동·좁은 화면 표를 확인했다. 로컬 링크 156개, 기존 앵커·이미지 13개, 원고와 다운로드 일치 검사를 통과했다.
- 배포 API 성공을 확인했다. 인증 후 게시 화면은 별도로 검사하지 않았다. 게임 코드·설치본은 변경하지 않았다.

## 최신 게시 — 2026-10-26 후속 타이틀·Android 계획

`#android-plan`에 두 조사 요약을 추가하고 `/related-titles.html`, `/android-plan.html`에 전문과 동일 Markdown 다운로드를 게시했다. 원래 Archive 목록, 중단한 Defense 포크, Android 코어 컴파일 결과, 개발 단계·스킬·APK 검증 절차를 기록했다.

- 소스: `a9215736a6ebe6b4b9f7a146b1ea03b8d0293e8a`.
- 버전: `appgprj_6addbf55aea081918d3d182d7e496e88~appgver_60aac3caa8c4819181e60788788d7c1f`.
- 배포: `appgdep_6aded2ef195081918ead09f9b8382c8c`, `succeeded`, 2026-10-26T04:11:40.055557+00:00. 소유자 전용 유지.
- 로컬 링크 145개, 기존 앵커·이미지 13개·REA 부록, 두 신규 원고/다운로드 일치를 확인했다. Chrome의 좁은 창에서 두 전문, 목차 이동, 단계 표와 긴 경로 줄바꿈을 확인했다.
- 문서 연결 9개 업무·48개 참조와 Android 스킬 형식 검사를 통과했다. 배포 API 성공을 확인했으며 인증 후 게시 화면은 별도 검사하지 않았다. Android APK 생성 완료를 뜻하지 않는다.

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
