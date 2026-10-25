# HeavyMach 게임 기획

2026-10-25 현행 요구. HeavyMach 1.13 복원 제품에 적용한다. 별도 Unity 게임 기획과 섞지 않는다. 원본 함수·수치는 [복원 계약](restoration-contract.md)과 분야별 정책이 상세를 소유한다. [문서 위계](document-governance.md)에 따라 기술 설계와 실제 앱에 연결한다.

## 제품 의도와 플레이

원작의 탱크 전투, 이동·점프·조준, 무기 획득·전환, 경험치·부품 강화, 미션 진행을 현재 아이폰에서 플레이하게 한다. 원본 에셋·맵·게임 수식과 저장을 보존하면서 사용자가 승인한 넓은 화면과 중단/재개 편의를 제공한다.

화면 흐름은 타이틀 → 슬롯 → 모드/미션 선택 → 전투 → 결과/다음 스테이지 또는 메뉴다. Scenario와 Survival을 구분한다. 생성·삭제·선택되는 슬롯과 앱 재실행 후 진행 복구는 원본 저장 규칙을 따른다. 원작에 없는 적 레벨 보정·데미지·사거리·성장 곡선을 임의로 넣지 않는다.

일반 앱은 게임용, 치트 앱은 무기·부품·시작 조건과 방향 입력을 설정하는 별도 검증용이다. 같은 게임 코어를 사용하며 저장 공간을 분리한다. 치트 조건에서의 성공을 일반 난이도 완료로 바꾸어 보고하지 않는다.

## 현행 요구와 추적

| ID | 플레이어 관점의 요구 | 정책·설계 | 코드와 검사 연결 |
|---|---|---|---|
| HM-PLAY | 원본 이동·점프·전투·미션 진행과 모드 차이를 보존한다 | restoration-contract, game-calculation-policy | document-map의 combat, planning |
| HM-INPUT | 틸트로 이동하고 터치로 조준·사격한다. 화면에 보인 위치와 입력 좌표가 일치한다. 해제·취소 후 목표점이 가짜 좌표로 바뀌지 않는다 | coordinate-policy | input, combat |
| HM-CANVAS | 전투 오브젝트 크기를 유지하고 전체 가로 화면에 더 넓은 월드를 표시한다. 원본480 비교 모드는 남긴다 | coordinate-policy, rendering-policy | input, rendering |
| HM-MENU | 중앙 원본 에셋을 보존하며 승인된 좌우 배경과 메뉴 재배치를 적용한다. 보이는 버튼과 터치 영역이 일치한다 | coordinate-policy, rendering-policy | menus, rendering |
| HM-RESULT | 클리어/실패 일러스트가 표시되고 확장 화면의 뒤쪽 월드를 덮는다. 결과 버튼은 정상 동작한다 | rendering-policy | menus, rendering |
| HM-WEAPON | 원본 무기·필살기의 연사·분산·탄약·데미지와 전환음을 유지한다 | game-calculation-policy, runtime-policy | combat, runtime |
| HM-PROGRESS | 경험치·강화·정상 진행 저장을 보존한다. 발사·피격·EXP 변경마다 파일을 저장하지 않는다 | game-calculation-policy, runtime-policy | combat, runtime |
| HM-PAUSE | 앱 비활성/잠금에서 게임을 멈추고 복귀 후 명시 재개한다. foreground 무입력120초에도 정지하며 중단 시간을 몰아서 진행하지 않는다 | runtime-policy | runtime, input |
| HM-RENDER | 게임은 원본 GLES 규칙과 OpenAL 음향 경로를 사용한다. 원본 자원과 호출 순서를 보존한다 | rendering-policy, runtime-policy | rendering, runtime |
| HM-VALIDATION | 치트 앱에서 재현 조건을 설정하고 전체 무기를 검사한다. 일반 앱 저장·규칙을 분리한다 | restoration-contract, weapon-validation-procedure | validation |

연결표의 source/test/evidence는 구체적인 저장소 경로다. 파일 연결은 전체 동등성 인증이 아니며 관련 입력과 실제 소비 경로를 함께 확인한다.

## 화면·아트 기준

원본 탱크·지형·효과의 크기와 화풍을 유지한다. 타이틀·선택 배경은 중앙 원본과 좌우 확장 그림이 연결되어 보여야 한다. 생성 이미지의 중앙을 원본 대신 쓰지 않는다. HUD·글자·버튼은 안전영역과 읽기 쉬운 비율을 지키며 배경과 같은 방식으로 무조건 늘이지 않는다. 아트 채택은 원본/생성본 단독 이미지와 실제 앱 합성 화면을 함께 확인한다.

현재 타이틀·슬롯·모드·미션 확장은 적용됐다. 첫 실행 전체 경로와 나머지 화면의 디자인 전수 검수는 TODO다. 코드의 OpenGL 구조를 설명하는 design.md와 이 시각·사용성 요구를 구분한다.

## 승인됐지만 남은 요구와 조사

| ID | 구분 | 요구와 현재 상태 |
|---|---|---|
| HM-DOOR | 승인·미구현 | 화면 전체를 덮는 하나의 문 구조. 원작 4패널 순서·음향·닫힘 후 화면 교체를 유지한다. 현재 메뉴는 중앙/좌/우 반복 셔터다 |
| HM-ROTATE | 승인·검증 남음 | 세로 없이 양방향 OS 가로 회전을 따른다. 방향 허용/센서 변환은 구현됐지만 실제180도 회전·틸트·터치·HUD 통합 검증은 남았다 |
| HM-THERMAL | 조사/검증 | 장시간 발열과 성능을 같은 조건으로 측정하고 근거에 따라 대책을 적용한다 |
| HM-AI | 예방 조사 | 적 고정값과 확장 시야 간섭을 감사한다. 사용자 전체 미션 플레이에서 적 이동 이상이 관측되지 않았으며 새 근거 없이 이동 규칙을 보정하지 않는다 |
| HM-FOLD | 검토 | 접힌 자세의 상단 게임/하단 조작부 구상. 기기 지원 확인·설계·구현 전이다 |

최신 실행 상태와 사용자 확인은 [current-status](current-status.md), 실행할 후속 과제는 [TODO](usability-todo.md)가 소유한다. 현재 설치본은 제품 코드 `72e8c68e`이며 사용자가 모든 미션을 플레이하고 크래시·오동작·적 이동 이상은 없는 것 같다고 보고했다. 문서 정비는 이 앱의 게임 규칙을 바꾸지 않는다.
