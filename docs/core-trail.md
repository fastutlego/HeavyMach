# GobTrailEffect 복원

2026-10-24. Sources/Core/Trail.hpp/.cpp 및 Tests/TrailTests.cpp를 작성했다. REA의 원본 export를 재사용하고, switch로 잘린 draw 분기는 원본 바이너리를 LLVM objdump로 추가 확인했다. 별도 그래픽 효과를 설계하지 않았다.

| 원본 주소 | API | 복원 범위 |
|---|---|---|
|0x1ab20|생성자|wind/diffuse 기본 true|
|0x1ab60|configure|폭/간격/블록수/지연/감쇠, last0, odd false|
|0x1acd0|addValidPos|좌표·법선·색·UV, 버퍼 밀기, 곡선 폭 RNG|
|0x1b438|addTrailPos|world→local, 길이별 보간, 이동 끝점 갱신|
|0x1b5c0|tick|float 생성시각, alpha, 확산/바람 RNG, 제거|
|0x1b8d8|host 계약|항상 camera 안으로 취급|
|0x1b8dc|draw|GL_TRIANGLE_STRIP에 해당하는 mesh|
|0x9dc4|CombatEvent 생성자|effect4 폭8/간격24, effect5 폭4/간격32,128blocks,delay.1,fade.8,blend2|

## 좌표와 기하

TrailMesh.vertices는 **local 좌표**다. translation은 생성 당시 trail.position이며 host가 world→screen 변환한다. 원본 GL draw는 screenPos로 translate하고 vertex 배열을 그대로 제출한다. host가 local y축 변환을 적용할 경우 원본 렌더링 좌표계와 함께 맞춰야 한다. mesh의 vertices를 다시 world 좌표로 오인해 카메라를 두 번 빼지 않는다. UV의 maxS/maxT는 원본 texture의 유효 영역 비율을 host가 전달한다(기본1).

`lastPosition`은 고정점 뒤의 임시 끝점 인덱스다. 고정점 간 거리가 blockLen 이상이면 sqrt(double(float(dx²+dy²)))/double(blockLen)을 float로 줄인 횟수만큼 보간한다. 마지막에 IsAdd=false로 실제 끝점을 기록한다. UV는 매 고정점마다 v=maxT와0을 교대한다. 각 점의 두 vertex는 center±normal이다. normal=(dy,−dx)/sqrtf(dx²+dy²)×width×.5.

IsAdd=false이거나 last<2이면 원본처럼 점0의 normal/vertices도 현재 normal로 갱신한다. 그 외에는 중간점에서 이전/다음 방향을 각각 width 길이로 정규화하고 cross>.1일 때만 random을1회 호출한다. width에 randomUnit×6을 더하고 두 방향 합을 정규화하여 중간점의 반폭 normal을 갱신한다. 반대쪽 회전에는 이 보정과 RNG가 없다.

첫 addTrailPos는 동일한 점을 고정점/끝점으로 기록하므로 0/0 normal이 생긴다. 원본 수학을 보존하며 이를 임의의 수직선으로 바꾸지 않았다. last>1일 때만 mesh를 그린다. 끝점이 정확한 간격 배수인 경우에도 중복점이 가능하다. host는 비유한 primitive만 생략해야 하며 이를 RNG 호출이나 core 상태 변경으로 연결하지 않는다.

## 노화와 RNG

생성시각은 float(time), 경과시간은 float(double worldTime−double storedTime)다. elapsed>fadeDelay일 때만 alpha를 갱신한다. fade 구간이면1−(elapsed−delay)/fadeTime, 아니면0이다. 점0의 alpha는 이 경로에서 항상0이다. 지연 중에는 기존 alpha를 유지한다.

살아있는 각 점마다 diffuse 먼저 random1회, wind 다음 random1회다. diffuse는 randomUnit×.05×normal을 한쪽 vertex에 더하고 다른 쪽에서 뺀다. wind는 sinf(30×storedTime)/(randomUnit×10+20)×normal을 양쪽에 더한다. elapsed나 현재 시간을 sin 인자로 바꾸지 않는다. alpha0인 점은 RNG를 소비하지 않는다. 전부 죽고 last>0이면 Remove 이벤트 뒤 TickChildren 이벤트가 발생한다. paused이면 노화/RNG를 멈추고 TickChildren만 발생한다.

## Bullet에서의 실제 호출 위치 — export 누락 보완

REA `0xab04.pseudo.c`와 `.asm`는 switch16의 첫 경로만 포함했다. 따라서 이 export만 보고 trail 갱신 위치를 tick이라고 추정하면 잘못된다. 원본 파일에 아래 읽기 전용 명령을 실행했다:

```
xcrun llvm-objdump --disassemble --triple=thumbv6-apple-ios \
 --start-address=0xab80 --stop-address=0xb034 ORIGINAL_HEAVYMACH
```

결과는 `/tmp/hm-bullet-draw.asm`에 저장했다. effect4/5 분기는0xafc4에서 smokeTrail이 null이면 반환한다.0xafd0..0xafec은 world time getter를 호출한다. 그 뒤0xaff0..0xb024에서 bullet의 world x/y를 읽고 trail의 `addTrailPosX:Y:`를 호출한다. 실제 literal pointer를 원본 파일에서 확인했다:

| literal VA | 값/selector ref | selector 문자열 |
|---|---|---|
|0xb098|0x2cfc8→0x21854|getWorld|
|0xb09c|0x2d05c→0x21fb0|time|
|0xb0a0|0x2d1b4→0x23304|addTrailPosX:Y:|
|0xb054|0x2cff4→0x22070|x|
|0xb058|0x2d020→0x22028|y|

이 분기에는 pause 검사가 없다. **Bullet draw가 실행될 때** addTrailPos를 호출한다. BaseBulletTick 콜백에 넣는 방식은 원본 호출 위치를 보존하지 않는다. Bullet setFire에서 trail을 objectBase에 추가하므로 trail 생성 순서도 유지해야 한다. Session 담당자에게 이 근거와 API를 전달했다.

Trail draw의 switch8도 raw byte를 확인했다.0x1b91e부터 table bytes는04 03 05 09 09 0d다. blend0는 SRC_ALPHA/ONE_MINUS_SRC_ALPHA, blend1 SRC_COLOR/ONE_MINUS_SRC_ALPHA, blend2/3 SRC_ALPHA/ONE, 나머지는 glBlendFunc를 새로 설정하지 않는다. TrailMesh는 이 factor metadata를 제공한다. topology는 연속 triangle strip이고 vertex count=2×last+2다. host는 원본 GL_REPLACE 규칙과 additive blend를 적용한다. RGBA texture는 vertex color/alpha를 무시하고 RGB565는 vertex alpha를 사용한다. Trail의 alpha 감쇠·RNG·제거 규칙은 유지한다. 후속 [렌더 연결](sprite-native-integration.md)에서 실제 GL fixture와 native 합성을 검사했다.

## 안전한 저장과 동등성 경계

원본 posArray 할당은 `blocks*20+1`바이트이며 blocks+1개 점을 쓴다. 중첩 memcpy도 존재한다. portable 코어는 충분한 vector 저장 공간과 순방향 복사로 같은 논리 점 순서를 보존한다. 최초 점 기록 전의 tick은 원본 미초기화 malloc 내용을 읽는 영역이므로, 이식 코어는 TickChildren만 실행한다. blocks<1, 비양수/비유한 blockLen, float 보간 nonprogress는 예외로 거절한다. 원본 undefined memory 동작은 재현 대상이 아니다.

## 실행한 검사

`clang++ -std=c++20 -ffp-contract=off -fno-fast-math -fsanitize=address,undefined`로 Trail.cpp/TrailTests.cpp와 임시 main을 링크하고 `/tmp/hm-trail-tests`를 실행했다. exit0, sanitizer 진단 없음.

검사 범위: 25픽셀 보간점0/10/20/25, 좌표/반폭/UV 교대, buffer shift, 정방향 corner RNG1회, diffuse→wind 점당2회, pause·alpha0 RNG0회, alpha 감쇠/제거 순서, 초기 중복점NaN, additive factor. 실제 native mesh rasterization과 원본 스크린샷 비교는 이 검사로 확인하지 않았다.
