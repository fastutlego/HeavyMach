# World camera offset 연결 복원

2026-10-24. 일반 world의 화면 판정과 터치 조준 기준점에서 quake offset이 빠진 차이를 원본 함수와 대조해 수정했다. 앞서 완료한 player HP 막대의 fixedPos 복원은 유지했다. 부모 작업에서 확보한 HP 막대 native 화면 확인과 이번 source/회귀 검사는 구분한다. 이번 작업자는 앱을 설치하거나 실행하지 않았다.

## 원본 계약과 누락

| 원본 | 확인한 동작 | native 소비자 |
|---|---|---|
| QobBase screenPosX0x7a28, screenPosY0x7b00 | world position−camera position+halfScreen−camera.offset; fixedPos일 때만 마지막 int(v+.5) | cameraPoint / 고정 지도 이미지 |
| QobBase isInCam0x7c78 | 위 screenPos와 원본 bounds로 좌0/우540/하0/상320 검사 | inView |
| QobBase tick0x7830 | 현재 isInScreen 갱신 후 자식 tick | motionEvents TickChildren |
| GobBullet tick0xa444 | **super** isInCam으로 충돌 가능 여부 검사 | CombatHost.inCamera |
| GobMachWeapon0x129c6 및0x128ec tail | owner.isInScreen인 경우에만 적 AI 실행 | CombatFrame.ownerInScreen |
| GobWorld checkBulletCollide0xd3c8 | 적 목록 순서대로 isInScreen+world point 포함 검사 | targetAt(false) |
| GobWorld checkEnemyBulletCollide0xd44c | 플레이어 world point 포함 검사; isInScreen gate 없음 | targetAt(true) |
| GobItemBox tick0x1d6c8 | 기존 isInScreen으로 pickup/끌림을 처리하고 마지막 base tick에서 갱신 | box.tick 뒤 box.inScreen 갱신 |
| processObjectRegen0xcd00 | 지도 QobImage에 fixedPos1 지정 | mapSprites collect |
| GobHvM_Player setTap0x1d300 | player.screenPos+weapon.rotPos 기준으로 touch delta 계산 | GameSession.touch |

기존 screen()은 quake 없는 렌더 좌표였다. CGContext의 world 이동으로 이미지가 흔들려 보이더라도, inView에서 이를 사용하면 화면 경계의 AI·충돌·아이템 판정은 원본과 달랐다. 예를 들어 screen y=.25인 점과 offsetY1은 원본에서 y=−.75로 화면 밖이다. 기존 native는 화면 안으로 판정했다.

## 변경 범위

GameSession.cpp에 cameraPoint()를 추가해 기존 cameraScreenPosition helper와 이미 연결된 cameraOffset을 사용한다. inView는 cameraPoint를 사용한다. 기체의 inScreen 갱신, 그 값을 받는 AI와 적 명중 gate, 총알 충돌용 super camera 검사, 아이템·입자 화면 검사에 같은 offset이 전달된다.

기존 screen()은 렌더용으로 남겼다. 일반 sprite와 trail은 host의 −offset 이동을 한 번만 받는다. fixedPos가 원본에서 확인된 mapSprites는 quake를 뺀 좌표를 정수 변환한 뒤 cull하고, render 좌표에는 offset을 더해 기존 host 이동을 상쇄한다. HP 막대도 이미 같은 계약을 사용한다. 추가 collect 호출은 없다.

터치 조준 기준점과 검증용 validationAimOrigin은 cameraPoint를 사용한다. 월드 위치, 뒤쪽 이동 경계, 지형 충돌, pointInObject에는 offset을 더하지 않는다. 적탄의 플레이어 target에 새 isInScreen gate를 추가하지 않는다. 아이템의 이전 tick 화면 판정 사용 순서도 변경하지 않는다. GobBullet.isInCam0xab00과 GobTrailEffect.isInCam0x1b8d8의 무조건 참인 draw 등록은 유지한다.

start/누적 world clock, 피해량, RNG 생성기, 원본 자산, 저장 데이터는 변경하지 않았다.

## 회귀 검사

새 Tests/CameraIntegrationTests.cpp는 실제 GameSession 구현과 RawData를 사용한다. 검사 전용 객체 위치/범위와 일정한 난수 입력으로 화면 경계만 분리하며 파일은 쓰지 않는다.

- 수정 전 offsetY1 경계의 inCamera assertion 실패(exit134)를 확보했다.
- 수정 후 상하·좌우 camera 경계, graph.inScreen 갱신 뒤 적 AI 난수 소비 및 enemy target gate를 검사했다.
- 화면 밖 플레이어도 적탄 target의 world 포함 검사는 유지함을 검사했다.
- touch delta에 quake가 반영되어 수평 aim이 되는 입력을 검사했다.
- 지도 fixedPos의 fractional quake 반올림과 화면 아래 경계 culling을 검사했다.
- 아이템이 첫 tick에는 이전 inScreen으로 끌리고, 갱신된 화면 밖 상태를 두 번째 tick에 사용함을 검사했다.

Debug와 ASan/UBSan 독립 실행은 모두 exit0이다. 전체 원본/runtime RNG 순서나 픽셀 동일성을 이 검사로 증명하지 않는다.
