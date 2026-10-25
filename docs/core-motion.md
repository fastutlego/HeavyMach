# GobHvMach 이동·점프 소스 복원

`Sources/Core/Motion.hpp/.cpp`, `Tests/MotionTests.cpp`를 작성했다. `GobHvMach` 클래스는 원작 이름을 유지하며 Geometry의 단방향 선분 충돌을 소비한다. 근거는 `Analysis/functions/<주소>.pseudo.c`와 `.asm`이다. 이 코어는 렌더·음향·세계 스케줄러를 실행하지 않는다.

## 원작 함수 대응

| 주소·함수 | 복원 범위 |
| --- | --- |
| 0x3fd0 init | state0, type3, HP256, moveSpeed1.4, boosterVel230, 벡터·링크·bounce 초기값. 시각 레이어 생성은 host 책임 |
| 0x4ab0 setCurrentCol: | prev/current 갱신, atan2(-normal.x,normal.y) 후 float 회전. nil이면0성분 |
| 0x5704 setMoveVel: | 이전 dash0·speed>=0·input<−0.5일 때 dash−2.5, input×moveSpeed, 플레이어 passive3의1.3배 |
| 0x4c10 processLand | 경사±0.7 분기, 선분 이동·prev/next·플레이어/적 끝점 처리, 회전·엔진 bounce |
| 0x5240 processFly | 중력·고정 timestep·순서 있는 후보 충돌·착지·낙사 damage 호출 이벤트·회전 |
| 0x479c tick | hidden/paused, dash 감쇠, 세계 경계, 다음 스테이지 조건, 상태별 함수, 상위 tick 이벤트 |
| 0x4b5c processModel | idle/moving bounce, random%60 무기 조준 이벤트. host RNG가 없으면 요구 이벤트 |
| 0x1cc70 GobHvM_Player setBooster: | 지상 점프·등급별 공중 추가 점프·passive2의1.3배·연기 생성 요청 |

## 산술·상태

부동소수는 float 순서를 유지하고 `-ffp-contract=off -fno-fast-math`로 빌드한다. 비행 중력은 원작 비트 `0x40e55556`, timestep은 `0x3c888889`다. velocityTick은 velocity×dt 후 x만 speed로 대체한다. machType1은 중력과 지면 검사를 건너뛴다. 충돌 후보 순서를 유지하고 첫 성공에서 멈춘다. 후보 목록이 비어 있으면 검사하지 않는다.

지상 정상 경사에서는 선분 dir×−speed만큼 이동한다. 진행방향 기준 경사>0.7이면 speed를 ±1.5로 바꾸고 dir×speed로 미끄러진다. 경사<−0.7이면 비행 state2·velocity.y30으로 전환한다. along<0은 next, along>length는 prev로 옮긴다. 선분이 끝난 플레이어는 currentCol=-1을 유지하며 다음 land 호출에서 비행으로 전환한다. 적은 이전 선분과 끝점으로 복귀한다.

낙사는 원작 `onDamage:hp`에 해당하는 Damage 이벤트다. 이 코어가 HP를0으로 단정하지 않는다. 실제 damage·방어·사망 처리는 수신자가 실행해야 한다. NextStage 이벤트와 state3 전환은 클리어 요청을 의미하며 저장·미션 UI는 세계의 setNextStage 소비자가 수행한다.

## 누락 인수의 확인

processLand의 엔진 bounce 의사 코드에서 double 곱셈 인수가 생략되어 있었다. 어셈블리0x517e/0x5182 및0x51c0/0x51c4는 r2=0, r3=[0x5234]를 설정한다. 원본 Mach-O `__TEXT vmaddr=0x1000,fileoff=0`로 변환한 파일 offset0x4234의 u32는 `0x40490000`이다. 따라서 곱셈 상수는 double50.0이다.

원작은 time×50→int32 truncate→signed low16→remainder10→float 변환 후 idle0.04 또는 moving0.15를 곱한다. 모델 모드는 globalTime×50에0.1을 곱한다. 복원은 이 low16 wrap을 유지한다. int32 범위를 벗어나거나 비유한 time은 명시적으로 예외 처리하며 원작 변환의 범위 밖 동등성을 주장하지 않는다.

## host 계약과 미지원

- GroundSegment의 prev/next는 인덱스다. −1만 nil이며 잘못된 인덱스는 예외다. host는 원작 getColListFromX와 같은 순서의 collisionCandidates를 제공한다.
- MotionWorld time은 세계 시간, globalTime은 앱 시간이다. tick이 시간을 증가시키지 않는다. paused라도 원작처럼 TickChildren 이벤트가 나오며 hidden에서는 나오지 않는다.
- ModelAim은 host의 원작 random() 계약(nonnegative31bit) 입력을 사용한다. RNG가 없으면 ModelRandomRequired를 반환한다. 무작위값을 임의 고정하지 않는다.
- BoosterSmoke는 free particle 요청이다. 원작의 풀 할당 성공 후 random4회·scale/rotate 효과 설정은 아직 host에 연결하지 않았다. 이 이벤트만으로 원작 연기 렌더 동등성을 주장하지 않는다.
- 부스터의 원작 BOOL 인수는 함수 본문에서 검사되지 않는다. 복원 API는 효과 없는 bool 인수를 제거했다. grade0 지상 점프, grade1~4 공중1회, grade5 이상 공중2회 경로를 보존한다.
- 상위 QobBase tick, 시각 레이어, 부품 객체·피격 처리·미션 전환·저장·연기 표시·사운드는 별도 소비자다. 이 코어만으로 전체 게임 실행을 완료하지 않는다.

## 검사

임시 main은 runGeometryTests와 runMotionTests를 호출했다.

```sh
clang++ -std=c++20 -Wall -Wextra -Werror -ffp-contract=off -fno-fast-math \
  -fsanitize=address,undefined Sources/Core/Geometry.cpp Sources/Core/Motion.cpp \
  Tests/GeometryTests.cpp Tests/MotionTests.cpp /tmp/heavymach-motion-main.cpp \
  -o /tmp/heavymach-motion-tests
/tmp/heavymach-motion-tests
```

2026-10-24 종료0, assertion 실패·sanitizer 진단 없음. 초기값·이동·dash·양방향 선분 전이·적 끝점·경사 미끄럼/비행·착지·무중력형·부스터 등급·낙사 요청·보스/생존모드 클리어 제한·pause/hidden·bounce·RNG 요구를 확인했다. 원작 runtime과 자동 differential 검사는 미실행이다. host 통합·실제 iPhone 플레이 검증은 별도로 남는다.
