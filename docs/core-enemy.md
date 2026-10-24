# GobHvM_Enemy 소스 복원

`Sources/Core/Enemy.hpp/.cpp`는 `GobHvMach`를 상속한다. 데이터 입력은 GameData의 `MachDefinition`이다. `Tests/EnemyTests.cpp`는 `runEnemyTests()`를 제공한다. 근거는 원작1.13 `Analysis/functions/<주소>.pseudo.c`와 `.asm`이다.

## 주소와 구현

| 원작 함수 | 주소 | 복원 책임 |
| --- | --- | --- |
| init | 0x13d80 | boss=false,type2,giveItem/giveExp/speedDest/gravityDest/gauge 초기화 |
| tick | 0x13e04 | type0 지상 AI, type1 헬기 AI, 상위 이동 tick 호출 |
| onDamage: | 0x143b8 | 상위 HP감소/파괴/피격음 뒤 HP gauge 갱신 |
| onDestroy | 0x14418 | 보스수 감소·아이템·폭발·경험치·remove·상위 파괴음 순서 |
| setHPGauge: | 0x14778 | scaleX=fraction, x=−(gaugeSize−fraction×gaugeSize) |
| decodeChunk:Data:Pos: | 0x147e8 | type3 HP/EXP/machType를 typed definition으로 소비; 재귀 parts는 host 적용 |
| makeMachFromName: | 0x14918 | makeMachFromDefinition: 드롭 설정·HP gauge 선택 |
| 상위 onDamage:/onDestroy | 0x54b8 / 0x55fc | HP 차감·0 clamp·가상 destroy, 랜덤 hit sound8..10, destroy sound14 |

데이터 reader 자체는 GameData 담당 범위다. MachDefinition이 없거나 이름 조회 실패면 host가 생성하지 않아야 한다. typed definition이 제공되면 makeMachFromDefinition은 성공을 반환한다.

## AI 분기

pause이면 적 AI를 건너뛰고 상위 tick으로 간다. 원작 Enemy tick은 visible 검사보다 먼저 AI를 실행한다. 표시 여부는 상위 GobHvMach tick이 검사한다. type2 등은 별도 이동 AI를 추가하지 않는다.

- type0: x>right−30이면 즉시speed−0.6. 그 밖에는 random%50==0일 때 random%3 모드를 읽는다. x<playerX+50이면+0.6. 모드0은 player가앞이면+0.8, playerX<x−400이면−1, 그외0. 모드1은 다음random의짝수−0.6/홀수+0.4. 모드2는0. 모드0의 일부 비교는 앞선 분기 때문에 정상 유한좌표에서는 도달하지 않을 수 있지만 제거하지 않았다.
- type1: x>right−30이면 speedDest−0.5, 아니면 random%60==0에서 다음random짝수−0.5/홀수+0.5. 별도의 random%30==0에서 x<playerX+120이면 speedDest+0.5. y>playerY+130이면 gravityDest−20, y<playerY+80이면+20, 사이면0. speed와 velocity.y를 목표까지1/30씩 보간한다.

공격 활성·시야·무기 조준은 이 tick에 없다. 원작 GobMachWeapon processAI 경로가 담당한다. 이 클래스에 임의 거리 기반 자동 사격을 추가하지 않았다. host는 CombatFrame에 원작 owner/player/camera 상태를 공급하고 무기 tick을 별도로 실행한다.

## 파괴·드롭·경험치

Heli001은 생성시giveItem2이며 랜덤을 소비하지 않는다. 다른 이름은 random%20==0일 때giveItem1, 실패하면 기존 값을 그대로 둔다. hpMax<=500은 gaugeSize16,y40, 그 이상은50,y80이다. type1은 gaugeY−10이다.

보스 파괴는 random%2+2 아이템으로 덮어쓰고 KillBoss를 먼저 발행한다. 각 아이템을 같은 위치에 동기 생성한 뒤 풀에서 particle을 요청한다. 성공하면 Efx_ExplodeNPC.png,16프레임,수명1,scale1,blend0,위치y+28의 폭발 이벤트를 낸다.

시나리오(gameMode0)에서는 internalLevel>mission×4+4일 때 giveExp×((mission×4+4)/internalLevel)를 float 계산 후 int로 절삭한다. 그 외 모드는 giveExp/2다. AddExperience→Remove→파괴음14 순서다. onDamage를 통해 사망했다면 그 뒤 피격음random%3+8과 gauge 갱신까지 이어진다. sound pan=(enemyX−playerX)/100이며 clamp를 임의 추가하지 않는다.

## host 계약과 한계

EnemyHost callback은 동기다. spawnItemBox가 원작 아이템 초기화를 실행하며 공유 RNG를 소비하면 이후 랜덤 순서에 반영되어야 한다. 이벤트를 모두 모은 뒤 나중에 아이템을 만드는 방식은 이 순서를 바꾸므로 동등성을 주장할 수 없다.

Remove 이벤트는 제거 예약이다. 현재 C++메서드가 반환하기 전에 객체를 delete하면 안 된다. 원작처럼 뒤의 사운드와 gauge가 처리된다. 반복 onDestroy를 억제하는 별도 규칙은 추가하지 않았으므로 host는 제거된 객체를 다시 충돌 대상으로 공급하지 않아야 한다.

PartsDefinition 이벤트는 host가 현재 MachDefinition.parts를 원래 재귀 순서대로 부품 객체·레이어에 연결할 지점이다. 부품 렌더·실제 gauge 객체·아이템 생성·풀·음향·경험치 저장은 host 책임이며 코어 테스트만으로 연결 완료를 주장하지 않는다. RNG는0..0x7fffffff 계약을 검사한다. EXP float→int가 비유한/범위초과인 입력은 예외이며 해당 비정상 범위의 원작 동등성은 미검증이다.

## 실행 검사

```sh
clang++ -std=c++20 -Wall -Wextra -Werror -ffp-contract=off -fno-fast-math \
  -fsanitize=address,undefined Sources/Core/Geometry.cpp Sources/Core/Motion.cpp \
  Sources/Core/Enemy.cpp Tests/GeometryTests.cpp Tests/MotionTests.cpp \
  Tests/EnemyTests.cpp /tmp/heavymach-enemy-main.cpp -o /tmp/heavymach-enemy-tests
/tmp/heavymach-enemy-tests
```

임시main에서 세 run 함수 호출. 2026-10-24 종료0, assertion·sanitizer 오류 없음. 초기/typed data, 랜덤 호출 횟수, 지상 모드, 헬기 높이대역·평활, pause, 피격 gauge, 동기아이템 RNG소비, 사망 순서, 보스 드롭, EXP 감소·생존절반을 확인했다. 원작과 자동 differential 실행, 렌더/음향·세계 통합·기기 플레이는 별도 검증 대상이다.

## 남은14개 보스 계열 독립 대조 — 2026-10-24

보스 v5의 플레이어 사망14개 배치를 대상으로 Enemy 코드와 원본 REA를 다시 대조했다. 이 검토에서 수정할 구체적 차이를 찾지 못했다. `Enemy.cpp`와 `EnemyTests.cpp`는 변경하지 않았다. 자동 조작의 패배를 원본 AI·피해 코드 결함으로 바꾸어 설명하지 않는다.

### 원본 분기·피해 대조

- `0x13e04.pseudo.c/.asm`: pause 검사(asm0x13e34), type0의 right−30 비교와 난수%50→%3 순서(0x13e90~0x13ea6), 모드별 속도와 짝홀 분기, type1의 독립%60/%30 검사, speed/velocity.y의 float1/30 보간을 확인했다. 모든 경로는0x1433e에서 상위 tick으로 이어진다. 현행 Enemy의 분기·난수 소비 순서와 일치한다.
- `0x13d80`: boss=false/type2 및 목적 속도·중력·드롭·EXP 초기값이 현행과 같다. 실제 정의의 machType이 이후 이를 덮어쓴다.
- `0x147e8`: type3 chunk에서HP는payload+0x18,EXP+0x1c,machType+0x20이다. HP와HPMax에 같은 원본값을 쓴다. 이번14종에 임의 보스 체력 배율이나 레벨 기반 HP 추가는 없다.
- `0x54b8.asm`:0x54ce의32-bit SUBS 결과를 저장하고 signed HP<=0이면0으로 만든 뒤 가상 onDestroy를 호출한다. 이후 random%3+8 피격음이 난다. `0x143b8`은 상위 피해 처리 후 HP/HPMax로 gauge를 갱신한다. 현행 wrapping subtraction·파괴·피격음·gauge 순서와 일치한다.
- `0x14918`: Heli001 고정 드롭 외에는random%20을 소비한다. HP501부터 큰 gauge이고 type1의 gaugeY는−10이다. 특정 남은 보스에게 새 방어율·무적·피해 제한을 부여하는 분기는 없다.

### 실제 원본 정의가 사용하는 유형과 무기

원본 RawData를 기존 GameData reader로 읽고 각 MachDefinition의 재귀 parts에서 무기 정의를 조회했다. 아래 개수는 장착 무기 객체 수이며, 모든 무기가 같은 순간에 발사된다는 뜻은 아니다. delay는 원본 WeaponInfo 값이고 실제 발사 주기는 아래 AI 단계와 재장전의 영향을 함께 받는다.

| 이름 | machType | HP | 장착 무기 수 | 원본 무기 피해 | 정의 delay 범위 |
|---|---:|---:|---:|---|---|
| Boss03 | 0 | 20000 | 3 | 50,50,80 | 2~5 |
| E05MB03 | 0 | 10000 | 2 | 80,100 | 3 |
| E05MB04 | 0 | 11000 | 3 | 80,120,120 | 2~3 |
| E05MB05 | 0 | 12000 | 2 | 80,150 | 2~3 |
| E05MB02 | 2 | 9000 | 1 | 80 | 3 |
| Boss01 | 0 | 30000 | 4 | 50,70,80,100 | 5 |
| MBoss0601 | 0 | 11000 | 3 | 60,80,70 | 2~3 |
| MBoss0701 | 0 | 12000 | 4 | 80,70,70,70 | 2~3 |
| E06MB04 | 0 | 15000 | 5 | 200×5 | 2 |
| Boss06 | 1 | 50000 | 5 | 50,50,60,60,60 | 2~5 |
| E07MB01 | 0 | 15000 | 2 | 200,200 | 1 |
| E07MB02 | 0 | 15000 | 2 | 80,200 | 1~2 |
| E07MB03 | 0 | 15000 | 3 | 80,200,200 | 1~2 |
| Boss07 | 0 | 60000 | 6 | 120,120,150,150,200,200 | 1~2.5 |

따라서12종은type0 지상 AI, Boss06은type1 헬기 AI, E05MB02는type2로 Enemy 자체 이동 AI가 없다. type2도 상위 tick과 부품 무기 tick을 실행한다. 200피해 type7 무기의 반복 장착과 Boss07의HP60000은 원본 정의에서 확인한 값이다. 이를 임의로 줄이지 않았다.

### 발사 주기 호출 경계

Enemy tick에는 발사 주기 조정이 없다. 원본 무기 tick의 공통 경로 `0x128ec`는 owner machType!=3 및 isInScreen 조건에서 processAI를 부른다. `0x12a40`은 signed dx+dy가0..520 범위일 때 AI 단계별 시간 검사를 한다. 현행 Combat의 같은 게이트와 비교했다.

- 단계0: 엄격한 time>float(resetTime+delayTime) 후 조준, 다음 대기0.3+random×0.5.
- 단계1: 같은 엄격한 시간 검사 후 단계2, 다음 대기0.05+random×0.4.
- 단계2: 기한이 지나면 단계0으로 돌아가 다음 대기0.5+random×원본delay를 정한다. 이 분기에서도 해당 tick의 조준 흔들림과 fireWeapon 호출이 이어진다.
- 원본 `0x128ec` 재장전은 float((time−fireTime)/delay)>1일 때 완료한다. 단순한 고정 delay 타이머 하나로 치환하지 않는다.

확인한 호출 경계와 단계 수식에서 차이를 찾지 못했다. 이번 소유 범위는Enemy이며, 무기 모든 switch 분기·탄환 충돌·실기기 원본 동시 실행까지 재검증한 것은 아니다. 그 범위의 별도 Combat 검토와 구분한다.

### 실행 결과

기존 Enemy 회귀를 `Geometry.cpp`, `Motion.cpp`, `Enemy.cpp`, `EnemyTests.cpp`와 임시main으로 빌드했다. `-Wall -Wextra -Werror -ffp-contract=off -fno-fast-math -fsanitize=address,undefined`를 사용했다. `/tmp/hm-enemy-review-tests` 종료0, assertion 및 sanitizer 오류 없음. 정의 조회 결과는 `/tmp/hm-enemy-original-boss-audit.txt`에 남겼다. 새 결함이 없어 실패 재현 테스트나 수치 변경을 추가하지 않았다.
