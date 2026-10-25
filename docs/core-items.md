# ItemBox와 객체 획득 경계

2026-10-24. `Sources/Core/ItemBox.hpp/.cpp`는 원본 REA export를 소비한 C++20 복원이다. `Tests/ItemBoxTests.cpp`는 `runItemBoxTests()`를 제공한다. GameSession 연결은 별도 작업이다.

## 주소와 확인 근거

| 주소 | 원본 함수 | 복원 |
|---|---|---|
| 0x1d538 | GobItemBox init | 초기 velocity RNG 2회, ItemBox.png tile0 |
| 0x1d6c8 | GobItemBox tick | 낙하·충돌·반사·획득·survival 유인 |
| 0x8cf0 | QobImage draw | DrawSprite 반환, frame0, blend0 |
| 0x76a0 | QobBase init | show/inScreen true, alpha/scale1, rotation0, zLayer2, bounds0 |
| 0x8b08 | QobImage initWithTile | imageBounds ±tileWidth/2, ±tileHeight/2 |
| 0x7d78 | QobBase isInPtX:Y: | 경계 포함 AABB, ordered `>` 거절 순서 |
| 0x798c/0x79d4 | QobBase posX/Y | 부모 위치를 더한 세계좌표; helper의 worldPosition 입력 계약 |
| 0x11554 | GobMachParts setPartsInfo | tile 크기의 절반에서 socket0을 뺀 partsBounds |
| 0x428c | GobHvMach addParts:BaseParts:BaseSocket: | base part로 기체 bounds 누적 |
| 0x1be84 | Player setPartsToMyMach | player 고정 bounds(-20,0,20,50) |

## 상자 규칙

생성 시 random31을 x,y 순서로 두 번 호출한다. 각각 먼저 float로 변환하고 `2^-31`(bits0x30000000)을 곱한다. vx는 그 값×200−100, vy는×100+150이다. 최대 random31이 float에서2147483648로 반올림되는 원본 특성도 유지한다. 생성자의 RNG가 Enemy drop 처리 사이에서 동기적으로 발생해야 한다.

show=false이면 아무 tick도 하지 않는다. paused이면 이동·획득 검사를 멈추지만 기본 child tick 이벤트는 발생한다. 착지 전에는 매 tick vy에서10을 빼고 velocity×float1/60(bits0x3c888889)을 계산한다. 현재 x로 호스트가 구한 collisionCandidates 순서대로 Geometry의 checkCollideVel을 호출한다. 첫 hit에서 멈춘다. hit이면 Geometry가 수정한 position을 유지하고 이동량을 별도로 더하지 않는다. vy<0일 때만 vy×−.3f(bits0xbe99999a)로 반사하며, 반사 후 vy<30이면 landed=true다. 정확히30은 미착지다. vx는 줄이지 않는다. hit이 없을 때만 position+=velocityTick이다.

획득은 inScreen=true일 때만 검사한다. `(box.x,box.y+12)`가 player 세계좌표 bounds 안이면 Pickup, Remove, TickChildren 순서다. Pickup 이벤트 안에서 Player.pickupItem을 동기적으로 호출해야 원본 RNG 순서를 보존한다. Remove에서는 제거 예약만 하고 객체를 즉시 해제하지 않는다.

획득하지 않은 착지 상자는 gameMode1에서 player로 이동한다. x+=(player.x−x)/25, y+=(player.y+24−y)/25. 이 동작도 화면 안에서만 발생하며 그 tick의 획득 검사 뒤에 수행한다. player가 없는 원본 nil getter의 좌표는0으로 취급한다. 존재하지 않는 player에게는 획득하지 않는다.

원본 GobItemBox에는 수명, 시간 기반 alpha, 회전, particle 생성, random 추가 호출이 없다. 장시간 지난 이유만으로 제거하지 않았다. World의 객체 제거/맵 교체는 별도 책임이다. 이미 있는 Visuals::DrawSprite를 반환하며 별도 렌더러·파티클 시스템을 추가하지 않았다. 호스트는 show/removed와 camera 판정으로 제출 여부를 결정한다.

## 기체 bounds

ObjectBounds는 왼쪽/아래/오른쪽/위의 local min/max이다. 원본 CGRect라는 타입명과 달리 width/height가 아니다. pointInObject는 worldPosition+각 edge를 순서대로 비교한다. 경계를 포함한다. 회전·scale·alpha·hp·show는 이 함수에서 확인하지 않는다. NaN도 원본 `>` 비교가 모두 false가 되는 동작을 유지한다. Combat 호스트가 살아있는 후보인지 먼저 판단한다.

Player는 parts 교체 후(-20,0,20,50)으로 덮어쓴다. Enemy는 고정 player bounds를 쓰면 안 된다. base Qob bounds는0에서 시작한다. addParts overload0x428c는 **새 child가 아닌 BaseParts 인자**의 rotPos와 bounds를 사용한다. `.asm`에서 entry r3→r10 저장 후 구조체 getter receiver로 r10을 전달하는 것을 확인했다.

누적 계산은 left=min(oldLeft,baseRot.x+.5*baseLeft,−20), bottom0, right=max(oldRight,baseRot.x+.5*baseRight,20), top=max(oldTop,baseRot.y+.5*baseTop,40)이다. 구현은 원본 unordered 비교까지 보존한다. partsBounds는 실제 tile 분할 후 크기와 socket0이 필요하다. host는 원본 parts 생성 순서로 이 함수를 호출해야 한다.

BaseParts=nil일 때 원본 objc_msgSend_stret의 반환 버퍼 초기화가 여기서는 확정되지 않았다. 임의 크기를 정확한 원본 메타데이터로 주장하지 않는다. `minimumMachBounds()`는 식의 정상값 하한(-20,0,20,40)이며, Enemy 모든 종류의 최종 bounds가 아니다. 실제 asset 크기와 연결 그래프에 따른 Enemy 최종 bounds 검증이 남아 있다.

## 검사 결과

다음 명령으로 standalone 검사 실행, exit0 및 sanitizer 진단 없음:

```
clang++ -std=c++20 -ffp-contract=off -fno-fast-math -fsanitize=address,undefined \
 HeavyMach/Sources/Core/{ItemBox,Geometry}.cpp HeavyMach/Tests/ItemBoxTests.cpp \
 /tmp/hm-items-main.cpp -o /tmp/hm-items-tests
/tmp/hm-items-tests
```

검사: 초기 RNG 두 번과 최대값 float 반올림, pause/hidden, 중력·반사 경계, 착지 후5000tick 생존, y+12/포함 경계 획득, 이벤트 순서, survival 유인/화면 밖 정지, DrawSprite, AABB NaN, socket 보정과 기체 bounds 누적. GameSession 연결·실기기 아이템 회수는 이 검사로 확인하지 않았다.
