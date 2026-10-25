# 2026-10-25 완료 상태 고정·두 앱 설치·회고 원고

제품 기준은 `72e8c68e`다. 이 요청에서는 게임 소스를 변경하지 않았다. 기존 배경 연결·검은줄 수정의 Simulator26/26·실제 Air26/26 증거를 재사용한다. 문 전환은 아직 반복 셔터이며 전체 화면 단일 구조 변경은 다음 과제다.

## 설치 결과

| 앱 | 결과 | 보존 실행 증거 |
|---|---|---|
| HeavyMachValidation · 치트 | 서명 검증·아이폰 재설치·실행 성공. 실제 타이틀에 검증 버튼 표시 확인 | `build/device-validation-runs/20261025T100459.918202Z/summary.json` |
| HeavyMachRestored · 일반 | Xcode GUI 서명 빌드·재설치·실행 성공. 실제 타이틀에 치트 버튼 없음 확인 | `build/device-validation-runs/20261025T100644.535655Z/summary.json` |

두 앱의 기존 Documents를 설치 전에 각각 백업했다. 일반 앱은 설치 직후 실행 전에 Documents를 다시 내보내 전후 파일 해시 일치를 확인했다. 삭제·초기화하지 않았다. 치트 앱은 이번 설치의 전후 해시 일치 검사까지 수행한 것으로 보고하지 않는다. Device Hub에서 두 앱을 번갈아 확인하고 마지막에 일반 앱을 실행했다. 이번 재설치는 전체 플레이·전체 무기 회귀 검사를 새로 수행한 것은 아니다.

서명된 게임 코드의 SHA-256:

- 치트 `HeavyMachValidation.debug.dylib`: `91a1c9cee19cc63cefa1b845ffa68671d849e358fff61220b373798c197b8670`.
- 일반 `HeavyMachRestored.debug.dylib`: `a66d05dc0c992b2739ccd4e15de827f756f411abf24bb4258be42417773b92a1`.

## 공개용 문서와 근거

[공개용 기술 회고](public/engineering-retrospective-20261025.md)는 GitHub·외부 서비스로 옮길 수 있는 독립 Markdown 원고다. 사용자 지시, 기술적 타당성, 에이전트의 잘못된 접근, 수정, 실제 결과와 미완료 범위를 모두 포함한다. 개인 계정·기기 식별자·서명·사용자 저장·로컬 절대 경로는 원고에서 제외했다. GitHub 공개·외부 서비스 공개는 실행하지 않았다. 기존 소유자 전용 Sites에는 전문과 Markdown 파일을 게시한다.

| 원고 주제 | 대조한 내부 근거 |
|---|---|
| 초기 성능·UIKit·OpenGL·오디오 | [초기 조사](frame-drop-investigation-20261024.md), [GLES 전환](opengl-migration-20261024.md) |
| 정상판 비교·시간·저장 | [소스 비교](compat-source-comparison-20261024.md), [실행 정책](runtime-policy.md) |
| 시뮬레이터·메모리·GPU | [Instruments](instruments-validation-20261024.md) |
| 실제 성능·틸트·생명주기 | [현재 상태](current-status.md), [실행 정책](runtime-policy.md) |
| 일러스트 누락·범위·무기 | [JPEG 수정](clear-illustration-fix-20261025.md), [필살기·결과](weapon-result-fix-20261025.md) |
| 가장자리·물리 기능·전체 무기 | [가장자리 입력](physical-edge-check-20261025.md), [실기기 자체 검사](physical-self-test-20261025.md) |
| 항공기·AI·회전·발열 | [보스 수정](boss-motion-fix-20261025.md), [AI·회전 조사](canvas-ai-and-rotation-research-20261025.md), [TODO](usability-todo.md) |
| 메뉴·생성 에셋·검은줄 | [첫 적용](menu-art-extension-20261025.md), [연결·경계 수정](menu-joins-20261025.md) |
| 계산·좌표·렌더 기준 | [계산 정책](game-calculation-policy.md), [좌표 정책](coordinate-policy.md), [렌더 정책](rendering-policy.md) |

이전 기록의 미설치·도구 장애는 해당 시점의 사실로 보존한다. 현재 결과와 다르면 이 문서와 current-status의 최상단 날짜 기록을 우선한다.
