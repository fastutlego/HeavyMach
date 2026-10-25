# GobBullet draw 복원

`Sources/Core/BulletVisual.hpp/.cpp`의 `drawBullet`은 원본 GobBullet draw0xab04를 옮겼다. 반환하는 DrawSprite는 world 좌표다. Session이 카메라 좌표로 한 번 변환한다. 화면에 보이는 총알마다 원본 draw1회에 대응하여 호출한다. 같은 프레임에 collect와 renderer가 각각 호출하면 flame 토글·연기·난수 소비가 두 번 발생하므로 호출자는 한 번만 실행해야 한다.

## 원본 분기와 출력

- 본체: bulletTile가 있으면 지정된 bulletTileNo를 normal blend/alpha1로 그린다. 회전은 bullet.rotate다. alpha 감소는 본체에 적용하지 않는다. atlas는 WeaponInfo reader0x10e00처럼 EnemyBullet0 접두사4×4, EnemyMissile 접두사2×4, 그 외4×8이다.
- Normal(1),0xab98: effectTile/effectTileNo를 blend2, bullet.alpha, scale1, bullet.rotate로 그린다. 기본 기관총은 bulletTile가 none이어도 이 효과로 보인다.
- Flame(2),0xac00: flameShow일 때만 Normal과 같은 효과를 그리고 매 draw마다 flag를 반전한다. physics tick이나 pause 여부로 바꾸지 않는다.
- Smoke(3),0xac82: worldTime이 float(effectRegenTime+effectDelay)보다 클 때만 생성한다. regenTime을 먼저 갱신하고 pool을 요청한다. 풀이 가득 차면 난수를 소비하지 않고 종료한다.
- Trail/Laser(4/5),0xafc4: 기존 trail이 있으면 world 위치를 addTrailPosX:Y로 전달한다. draw에 pause 검사는 없다. trail의 geometry와 수명은 별도 Trail 구현이 소유한다.

Smoke의 파티클 frame은 WeaponInfo.effectTileNo가 아닌 Qob의 inherited tileNo다. `BulletState.tileNo`가 이를 보존한다. pool은 reset된 ParticleState를 돌려주며 start는 globalTime을 기록한다. 생성 지연은 worldTime을 비교하므로 두 시간을 혼용하지 않는다.

|Smoke 값|플레이어 총알|적 총알(toPlayer)|
|---|---|---|
|liveTime|0.6|0.5|
|scale|random×0.4+0.2|random×0.2+0.1|
|scaleVelocity|random×1.5+0.5|random×0.5+0.5|
|rotation|floatπ×random을 두 번 더함|동일|
|rotationVelocity|random×5−2.5|random×3−1.5|

각 random은 원본 random31을 float로 변환한 뒤2⁻³¹을 곱한다. 생성 성공마다 위 순서대로4개를 소비한다. 원본의 float 연산 순서와 FMA 비사용을 보존했다.

## 증거 복구 방법

원본 SHA-256은 `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`이다. REA의 원본 pseudo `Resources/Evidence/bullet-visual-0xab04.json`은 Thumb switch16 이후 Normal 분기만 표시한다. 이것을 나머지 분기가 없다는 증거로 사용하지 않았다.

`bullet-original-assembly.txt`는 불변 원본 전체 llvm-objdump에서 draw 영역을 추출한 것이다. switch table0xab8a의 case2/3/4·5 목적지는0xac00/0xac82/0xafc4다. 별도 `/tmp` 복사본에서 switch 호출4바이트만 직접 Thumb branch+NOP로 치환했다. `bullet-case-provenance.json`은 원본/파생 해시, 오프셋, 교체 전후 바이트를 기록한다. case2/3/4 pseudo는 이 **분석용 파생본**에서 얻었으며 원본 소스라고 주장하지 않는다. 분기 내부 명령은 원본 assembly와 대조했다. 원본 파일과 공유 REA active target은 변경하지 않았다.

```sh
python3 tools/prepare_bullet_switch_cases.py --source /path/to/HeavyMach.app/HeavyMach
python3 tools/collect_resource_evidence.py tools/bullet-case-scenario.json
```

## 검사

`Tests/BulletVisualTests.cpp`의 runBulletVisualTests는 Normal의 무본체 총알, 본체/effect alpha 분리, atlas 규칙, Flame 연속 토글, Smoke 엄격한 시간 경계, 풀 실패 시 시간 갱신과 난수0회, 성공 시 난수4회와 양 진영의 파티클 값, world/global 시간 분리, Trail/Laser 콜백, 숨긴/삭제된 총알을 검사한다. ASan/UBSan과 `-Wall -Wextra -Werror`를 사용한다. 이 검사는 화면 렌더의 실제 픽셀 비교를 대신하지 않는다.

## 화면 밖 총알의 draw 등록 — 2026-10-24 독립 대조

`GobBullet::isInCam` 0xab00은 `movs r0,#1; bx lr`로 항상 참을 반환한다. `QobBase::tick` 0x7830은 이 가상 메서드를 호출해 살아 있는 총알을 visual에 등록한다. 반면 `GobBullet::tick` 0xa444는 충돌 검사에서 **super** `isInCam`을 직접 호출한다. 두 판정을 같은 화면 영역 검사로 통합하면 원본 동작이 바뀐다.

GameSession collect의 화면 영역 조건을 제거했다. show/remove/info 검사는 유지했다. 충돌용 `inCamera`는 변경하지 않았다. 화면 밖에서도 Smoke의 파티클 할당과 성공 시 난수4개, Flame의 매 draw 반전, Trail/Laser의 위치 기록이 실행된다. host 화면 clip은 그대로다.

`GameSessionTests`에 적이 없는 분리된 데이터 복사본과 장수명 고속 탄환을 사용했다. 실제 session 발사·tick·collect 경로에서 화면 밖 본체, Smoke의 추가 난수4개 단위 소비, Flame의12회 draw 중6회 출력, Trail/Laser mesh 생성을 검사한다. 수정 전 화면 밖 본체 assertion 실패를 확인했고 수정 후 통과했다. 원본 자산 파일과 저장 파일은 변경하지 않았다.

이번 순서 대조에서 `openMap` 0xe3f4의 자식 등록(적 목록→입자64개→플레이어→후속 동적 객체), `QobBase::tick` 0x7830의 순방향 자식 순회, `GobHvMach::tick` 0x479c의 숨김 return 및 pause 뒤 자식 tick 유지, `addParts` 0x4214와 `decodeChunk` 0x147e8의 생성 순서는 현재 session 연결과 맞았다. 이 확인을 모든 실행 경로의 RNG 동등성 증명으로 확대하지 않는다.

동일 GameSessionTests를 Debug 및 AddressSanitizer/UndefinedBehaviorSanitizer 구성으로 실행했고 모두 exit0을 확인했다.
