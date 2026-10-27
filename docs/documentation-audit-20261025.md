# HeavyMach 문서 점검·수정 내역

2026-10-25. 점검 시작 기준 `af927c32`, 설치된 제품 코드 `72e8c68e`.

## 문서 구성과 읽기 목록 보완

기존에는 정본·분야 정책을 읽고 코드와 대조하라는 지침은 있었으나, 게임 기획, 문서 위계, 업무별 연결, 과거 기록의 지위가 충분히 정리되지 않았다. 초기 UIKit 설명과 미해결 성능 기록이 최신 상태처럼 남아 있었다.

나는 문서·소스코드·앱의 기능이 서로 모순 없이 설명돼야 한다고 요구했다. Codex는 문서별 역할과 변경 절차를 작업 규칙에 반영했다. 이번 변경은 문서·업무 안내·연결 검사 도구다. 게임 코드·수식·현재 아이폰 설치본은 변경하지 않았다.

## 발견과 보완

| 발견 | 실제 근거 | 이번 보완 |
|---|---|---|
| 별도 게임 기획 부재 | design은 C++/iOS 모듈·원본 함수·렌더 구조 중심이며 플레이 의도와 승인/미구현 기능은 여러 문서에 흩어져 있었다 | game-design에 플레이 흐름·기능 요구 ID·화면/아트 기준·승인/미구현 범위를 정리 |
| 읽기 순서와 문서 위계가 혼재 | AGENTS/canonical에 필수 읽기·개발 요구 우선 규칙은 있으나 문서별 소유 내용과 변경 전파가 부족했다 | document-governance에 상위 요구→설계/정책→코드/검사→실행 근거의 역할과 수정 절차 명시 |
| 루트 진입과 업무별 선택이 느슨함 | HeavyMach/AGENTS에만 분야별 규칙이 있고 범용 harness plan은 node/python 검사 안내만 반환했다 | 루트 AGENTS에 HeavyMach 진입 추가, 8개 업무별 문서·소스·검사·증거 연결표와 plan 명령 추가 |
| 과거 성능 설명이 최신처럼 남음 | verification/source-coverage 상단에 FPS 급락 미해결과 성능 수정 보류가 남아 현재 내 확인과 충돌 | 당시 기록임을 명시하고 최신 상태로 연결. 원시 실패·계측 결과 보존 |
| 초기 메뉴 설명이 현행처럼 남음 | native-menus는 UIKit/CoreGraphics·변환 PNG로 설명. 실제 HMMedia는 OriginalImages/Extensions→HMGLRenderer 경로 | 초기 이력 표시와 현행 경로 설명·정책 링크 추가 |
| 로딩 문서의 적용 범위 불명확 | core-loading-transition은 원본480의 입력 gate 없음 설명. 확장 앱에는 월드 입력 차단과 메뉴 3영역 셔터가 존재 | 원본 코어 이력과 현행 호스트 정책을 구분하고 rendering-policy에 실제 분기 기록 |
| 연결 누락 검사 부재 | Git pre-commit은 gitleaks이며 범용 harness에 문서 계약 검사가 없었다 | document_contract.py로 문서/소스/검사 경로와 로컬 링크 검사. AGENTS 마감 규칙에 연결 |

## 현재 위계

최신 내 지시·제공 원본
→ 게임 기획 + 복원 계약
→ 기술 설계 + 좌표/렌더/수치/실행 정책
→ 실제 소비 코드 + 검사
→ 빌드·설치·앱 관측·내 확인.

정본에는 필수 문서 목록과 보존 규칙을 둔다. current-status는 최신 실행 상태, TODO는 남은 작업, 보고 사이트와 회고는 파생 보고다. 하위 코드에 맞춰 상위 요구를 임의 변경하지 않으며, 실행 실패나 새 내 관측은 요구 해석을 재검토하는 근거로 올린다.

기획 변경은 영향받는 하위 설계·코드·검사를 함께 바꾼다. 구현 오류는 요구를 보존하며 수정한다. 문서 오류는 원본·승인·실행 증거로 바로잡는다. 어느 경우든 요구 ID, 변경 전후, 소비 코드, 실행 근거, 남은 범위를 남긴다.

## 업무별 읽기

모든 업무는 HeavyMach/AGENTS, canonical, document-governance, game-design, restoration-contract, current-status, design, TODO로 진입한다. 추가 문서는 document-map.json이 소유한다.

| 업무 | 추가 필수 문서의 중심 |
|---|---|
| 기획·진행 | 수치 정책·스테이지 전환 계약 |
| 입력·회전 | 좌표·실행 정책·전체 무기 절차 |
| 그래픽·아트 | 렌더·좌표 정책·전체 무기 절차 |
| 메뉴·전환 | 좌표·렌더·실행 정책 |
| 무기·적·수치 | 수치·좌표 정책·전체 무기 절차 |
| 음향·저장·생명주기·성능 | 실행·렌더·수치 정책 |
| 빌드·실기기 | 전체 무기 절차·실행 정책·원본 출처 |
| 문서·보고 | 보고 사이트 절차 |

여러 업무나 공통 파일은 합집합을 읽는다. 연결되지 않은 새 소스는 자동 정상으로 넘기지 않고 요구를 분류한다. 목록 출력이 실제 읽기를 증명하지 않는다.

## 코드와 대조한 표본

| 계약 | 확인한 실제 소비자/검사 | 판정 |
|---|---|---|
| 높이320·가변폭·터치 역변환 | CanvasGeometry.hpp, Viewport.hpp, HMGameView layout / 확장 Air·Pro Max 검사 | 문서와 코드의 식·변환 일치 |
| 한 프레임 한 tick·NSTimer 기한 예약 | HMGameView scheduleOriginalTimer/frame / testOriginalRenderSceneAdvancesExactlyOncePerFrame | 계약과 호출 경로 일치 |
| 데미지 정수 나눗셈·Attack/Defense | Combat.cpp tickBullet / CombatTests의 강화·1.5배·방어 단언 | 문서 수식과 코드·기존 검사 일치 |
| 전환음·무입력120초 | HMGameView handleButton1100/checkInactivity / HMFramePerformanceTests | 효과음 호출·정지 조건 일치 |
| 원본 배경·생성 좌우·테두리 | HMGameView/HMMenus/HMMedia/HMGLRenderer / HMGLReferenceTests | 현행 정책과 경로 일치. 타이틀 경계20, 모드/미션24의 적용 범위 구분 |
| 전체 화면 단일 문 | HMGameView drawLoading | 현재 미구현. 3영역 반복 구현을 기획의 승인·미구현 항목으로 명시 |

대조 범위는 위 표의 여섯 계약이다. 현재 앱의 모든 미션에서 크래시·오동작·적 이동 이상이 없었던 것 같다는 내 플레이 기록과 기존 자동 검사/설치 증거를 재사용했다. 이번 문서 정비 때문에 게임 재빌드·재설치·전체 플레이를 다시 실행하지 않았다.

## 실행한 문서 검사

- document_contract.py check: 업무8종, 참조 파일41개, 현행 문서 로컬 링크 통과.
- Combat.cpp 입력에서 전투 정책·검사·증거 선택 확인.
- HMGameView 공통 파일에서 입력·메뉴·실행 업무 합집합 선택 확인.
- 존재하지 않는 업무·미분류 파일 거부 확인.
- 메모리 안의 별도 연결표에 없는 검사 파일/소스 패턴을 넣었을 때 오류 검출 확인. 실제 연결표나 제품 파일을 손상시키지 않았다.
- git diff --check 통과.

현재 도구는 구조를 검사한다. 문서의 자연어 의미·실제 앱 기능은 정책과 소비 코드를 읽고 관련 실행 근거로 확인해야 한다. Git 훅은 변경하지 않았으므로 이 도구가 커밋마다 자동 실행된다고 보고하지 않는다. 마감 시 실행은 AGENTS 규칙이다.

## 산출물

- 게임 기획: `docs/game-design.md`
- 위계·변경·일치 규칙: `docs/document-governance.md`
- 업무별 연결: `docs/document-map.json`
- 읽기 안내/구조 검사: `tools/document_contract.py`
- 진입: 루트 `AGENTS.md`, `HeavyMach/AGENTS.md`, `docs/canonical.md`

작업자는 요구 ID에서 관련 정책·코드·검사로 이동하고, 변경 후 같은 연결을 다시 확인한다. 글쓰기와 답변은 프로젝트의 `write-project-text` 스킬을 함께 적용한다.
