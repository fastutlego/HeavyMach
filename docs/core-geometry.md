# Vector2·ColVector 소스 복원

구현: `Sources/Core/Geometry.hpp/.cpp`. C++20 namespace `heavymach`의 `Vector2`, `ColVector`는 원작 클래스 이름을 유지한다. Objective-C 객체 소유·autorelease는 값과 `std::optional<Vector2>`로 대체했다. `draw` 렌더링은 이 코어 범위에 포함하지 않는다.

## 함수 대응과 근거

기준 실행 SHA는 [source-provenance](source-provenance.md)의 원작 1.13이다. 정적 근거는 `Analysis/functions/<주소>.pseudo.c`와 `.asm`, 원시 Evidence는 `Analysis/evidence/`에 보존되어 있다. 전체 export 완료 전에 시작한 별도 CLI 출력 6개는 `Analysis/geometry/`에 남겼다.

| 원작 주소·선택자 | C++ 대응 |
| --- | --- |
| 0x7250 init, 0x7294 initFromVector:, 0x7634 copyFrom: | zero/value 초기화, copyFrom |
| 0x72bc setX:Y:, 0x72d4 setAngle:Len: | setXY, setAngleLen |
| 0x731c add:, 0x7378 sub:, 0x73d4 mul:, 0x7408 div:, 0x743c reverse | 같은 메서드 이름 |
| 0x7460 dotProduct:, 0x74c8 crossProduct: | dotProduct, crossProduct |
| 0x7530 length, 0x7568 squereLength, 0x759c angle, 0x75d4 normalize | 같은 메서드 이름, 원작 squere 철자 유지 |
| 0x5874 init, 0x5918 initWithOrigin:Second:, 0x59a0 setOrigin:Second: | 생성자, setOriginSecond |
| 0x5a70 setPrev:, 0x5a7c setNext: | setPrev, setNext 비소유 링크 |
| 0x5a88 checkCollide:ColRadius: | checkCollideColRadius |
| 0x5c34 checkCollide:Vel: | checkCollideVel |
| 0x5e18 checkCollide:Dir:ColRadius: | checkCollideDirColRadius |
| 0x6010 checkCollide:Vel:ColRadius: | checkCollideVelColRadius |

0x59a0 어셈블리에서 첫 copyFrom의 r2는 원점 인수이며, 0x5e18 첫 dotProduct의 r2는 상대 좌표 인수다. 의사 코드에서 생략된 인수를 호출 레지스터로 확인했다.

## 분기·부작용

- normal=(dir.y, −dir.x). setOriginSecond는 normalize를 호출하지 않고 length로 직접 나눈다.
- normalize는 정확히 float bit pattern `0x3727c5ac`보다 짧으면 (0,0), 같거나 크면 나눈다. angle은 double atan2 후 float 변환이다. 나머지 기본 산술은 float이며 FMA 결합을 금지한다.
- radius 근접 검사는 선분 진행축의 [−radius,length+radius] 양끝을 포함하고, 수직거리 abs(distance)<radius를 요구한다. 입력 점의 원점 빼기/더하기를 실제 수행하므로 복원 과정의 반올림도 남는다.
- velocity 교차는 normal 쪽으로 멀어지는 속도를 거부한다. before>=0, after<=0일 때 표면에서0.5 떨어지게 보정한다. 진행축 [−3,length+3]을 포함한다. 실패 시 원래 입력값을 정확히 복원한다.
- directional radius는 원점 기준 상대 좌표를 받는다. 진행축 [0,length], 방향 dot normal<=0, abs(distance)<radius에서 입력 상대 점을 변경하고 world contact를 반환한다.
- velocity radius는 caller 입력을 복사하고 radius씩 전진해 directional 검사를 수행한다. 남은 길이가 radius 이하면 원래 시작점+전체 속도로 최종 검사한다.

## 퇴화 입력과 한계

원작에는 zero segment 나눗셈, parallel direction 나눗셈, zero velocity 정규화의0 나눗셈 방어가 없다. 복원은 그 IEEE NaN/Inf 경로를 보존하며 가짜 충돌 없음으로 고치지 않았다. 이는 퇴화 기하의 유효한 접촉이라는 뜻이 아니다. float bit 동일성은 모든 플랫폼 libm에 대해 검증하지 않았다.

sweep의 radius<=0·비유한 radius 또는 float 감산이 진전하지 않는 경우 원작은 루프가 끝나지 않을 수 있다. 복원 API는 이를 명시적으로 예외 처리한다. 이 입력 범위는 원작 동작 동등성 주장 범위 밖이다. caller는 유한 양의 radius와 정상 게임 좌표를 공급해야 한다. 큰 반경/좌표에서 NaN을 정상 geometry로 해석하지 않는다.

## 실행 검사

`Tests/GeometryTests.cpp`는 `void runGeometryTests()`를 제공한다. 임시 main에서 이를 호출하여 아래 명령을 실행했다.

```sh
clang++ -std=c++20 -Wall -Wextra -Werror -ffp-contract=off -fno-fast-math \
  -fsanitize=address,undefined Sources/Core/Geometry.cpp Tests/GeometryTests.cpp \
  /tmp/heavymach-geometry-main.cpp -o /tmp/heavymach-geometry-tests
/tmp/heavymach-geometry-tests
```

2026-10-24 결과: 종료0, assertion 실패·sanitizer 진단 없음. 수직/대각선, radius 경계, 선분 양끝, 단방향 교차, 입력 보존, normalize 임계치, signed zero, 퇴화 NaN, 비정상 radius 거부를 검사했다. 원본 바이너리와 자동 differential 실행은 아직 하지 않았다. 이 검사로 전체 게임 충돌 동등성을 확정하지 않는다.
