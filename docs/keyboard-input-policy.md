# 키보드 입력

iPhone/iPad의 터치·기울기 조작을 유지한다. iPad에서 키보드를 연결했다고 터치를 끄지 않는다. Mac은 키보드와 마우스/트랙패드 입력을 사용한다. USB와 Bluetooth는 같은 물리 키 코드를 사용한다.

| 기본 키 | 동작 |
|---|---|
| A / D | 좌 / 우 이동 |
| 방향키 | 화면의 조준점 이동 |
| F | 누르는 동안 사격 |
| Space | 점프 |
| Q / E | 이전 / 다음 무기. 끝에서 순환 |
| Shift | 이동·조준 속도를 낮춤 |
| Enter | 타이틀 진행·현재 버튼 선택 |
| Tab / Shift+Tab | 다음 / 이전 버튼 |
| 방향키 | 메뉴에서 버튼 선택 이동 |
| Esc | 전투 일시정지·재개, 메뉴에서 슬롯으로 복귀 |
| F2 | 키 설정 |

키 설정은 ↑↓로 동작 선택, Enter 후 새 키 입력, Backspace로 기본값 복원, Esc로 취소·닫기다. 키가 겹치면 두 동작의 배정을 교환한다. 설정은 확정할 때 UserDefaults에 저장한다. 사격이나 프레임마다 저장하지 않는다. macOS Command 단축키와 iOS 시스템 키 조합은 게임이 가로채지 않는다.

이 배치를 게임업계의 단일 표준이라고 부르지 않는다. A/D 이동과 Space 점프처럼 익숙한 구성을 사용하고, HeavyMach에 필요한 별도 조준·발사·무기 전환을 배정했다. [Xbox XAG107](https://learn.microsoft.com/en-us/xbox/accessibility/xbox-accessibility-guidelines/107)은 아날로그와 메뉴를 포함한 키보드 대체를, [Game Accessibility Guidelines](https://gameaccessibilityguidelines.com/allow-controls-to-be-remapped-reconfigured/)는 키 재지정을 다룬다. [Factorio 공식 조작표](https://wiki.factorio.com/Controls)는 기본값과 재지정을 제공하는 실제 사례다. [프로젝트 스킬](../.agents/skills/game-keyboard-input/SKILL.md)에 적용 절차를 남긴다.

## 입력 처리

`KeyboardInput.hpp`가 물리 HID 키→게임 동작, 가상 기울기와 조준점을 계산한다. HMGameView는 UIKey/NSEvent를 전달한다. OS keyDown 반복을 이동 시간이나 반복 점프로 사용하지 않는다. 이동은 기존 50Hz 필터에 넣고 조준은 기존 게임 tick에서 한 번 갱신한다.

원작은 abs(tilt)>0.03일 때 clamp(tilt×15)를 사용한다. 일반 이동 입력은 0.0301g에서 1/15g까지, Shift는 최대0.04g까지 올린다. 원작 dead zone과 필터는 바꾸지 않는다. 일반 키보드의 압력 감지 기능을 가정하지 않는다.

이동 키가 눌린 동안 키보드 기울기를 사용한다. 해제 시 가상 입력·필터를 비우고 다음 센서 표본부터 기울기를 다시 사용한다. 키보드 이동 중 터치 사격을 허용한다. 포커스 상실·키 취소·키보드 연결 해제·일시정지 시 유지 입력과 발사를 해제한다. 조준·발사·무기 전환은 기존 GameSession의 touch/booster/select 경로로 전달하며 데미지·탄속·중력·연사 규칙을 바꾸지 않는다.

메뉴는 그 프레임에 보이는 실제 버튼 목록을 탐색하고 동일 버튼 동작을 실행한다. 선택 강조와 조준 표시는 GL로 그린다. 키보드 조작 검사는 마우스 클릭으로 빠진 단계를 대신하지 않는다.

## 검사

`Tests/KeyboardInputTests.cpp`: 반대 키·반복 이벤트·속도·키 교환·해제·조준 경계.

`Tests/Apple/HMAppleInputTests.mm`: 실제 공통 뷰와 게임 세션에서 이동·점프·조준·사격, 모든20개 무기 정의(필살기3종 포함), 이전/다음 순환, 슬롯→모드→미션→게임, 설정 변경·복원, 키보드 이동 중 터치 사격, 다양한 화면 비율을 검사한다. 같은 검사를 native Mac, iPhone Simulator, iPad Simulator에서 실행한다. API 주입 검사와 CUA로 실제 OS 키 이벤트를 보낸 관측은 결과에 구분해서 적는다.
