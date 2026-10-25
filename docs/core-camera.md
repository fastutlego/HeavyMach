# QobCamera 흔들림 복원

2026-10-24. `CameraShake.hpp/.cpp`, `Tests/CameraShakeTests.cpp`를 작성했다. 원본 REA `Analysis/functions`의 `0xed80` tick, `0xeef8` setQuake, `0x7a28/0x7b00` screenPos를 근거로 한다. UIKit translate 연결은 별도 host 책임이다.

## 시간과 식

`setQuake(amplitude,count,duration,globalTime)`은 startTime=float(globalTime), delay=duration, size=amplitude, pitch=duration/float(count)를 저장한다. 기존 offset을 초기화하지 않는다. 이후 tick은 world의 정지 가능한 time이 아닌 **global g_time(double)**을 사용한다. 원본 camera tick 안에는 pause나 show 검사가 없다. 실제 객체 트리에서 tick이 호출되는지는 host가 결정한다.

startTime==0은 inactive sentinel이다. 시간0에서 발생한 quake를 보정하여 강제로 활성화하지 않았다. active이면 elapsed=float(globalTime−double(startTime))이다. elapsed<delay일 때:

```
phase = fmod(double(elapsed),double(pitch))/double(pitch)
decay = 1.f-elapsed/delay
offset.y = float((sin(phase*pi)*double(size))*double(decay))
```

pi는 원본 bits0x400921fb54442d18이다. elapsed/delay와1−ratio는 float 연산이고 sin/fmod/size 곱은 double이다. offset.x는 active 경로에서 건드리지 않는다. elapsed가 delay 이상이거나 unordered이면 startTime과 offset.x/y를0으로 만든다. 양수 시간에서 각 pitch는 양의 반사인 한 주기이므로 위아래 부호 교대 sin(2πt)로 바꾸면 안 된다. RNG는 없다.

## 화면 부호와 적용 범위

QobBase screenPosX/Y는 camera가 있을 때 `(parent를 포함한 pos−camera.pos)+halfScreen−camera.offset`이다. 따라서 y-up 화면에서 흔들림은 **−offset.y**, UIKit y-down에서는 **+offset.y**다. 부모별로 같은 offset을 반복해서 빼지 말고 최종 screenPos에서 한 번만 적용한다. `cameraScreenPosition()`은 이 연산과 선택적 fixedPos 처리를 제공한다.

fixedPos는 일반 round가 아니라 `float(ARM_int32_trunc(value+.5f))`다. 음수−1.7→−1,−.7→0을 보존한다. float→int는 VFP 포화와 NaN0을 사용한다. camera 없는 객체는 cameraPosition/halfScreen/offset을 적용하지 않는다.

적용 대상의 근거:

- QobBase init0x76a0은 camera=nil이다. addChild0x800c는 부모 camera가 있으면 child에 setCam을 호출한다. setCam0x7c1c는 기존 자식 전체에 재귀 전달한다.
- GuiTitle init0xefbc는 자신의 camera를 생성하고480×320 screen을 지정한 뒤 자식을 추가한다. 따라서 title 배경/제목/episode 등 해당 트리 시각 객체가 같은 title camera offset을 받는다.
- GobWorld init0xb0a4도 자신의 camera를 생성한다. 이후 추가되는 world background/objectBase/기체/상자/입자 등 자식은 world camera를 받는다.
- GameMain init0x2ecc는 World와 GuiGame를 **별도 sibling**으로 추가한다. GuiGame init0x151a0은 camera를 생성하거나 World camera를 지정하지 않는다. HUD, 하단/상단 UI, 메뉴/강화 UI에 world quake를 적용하면 원본과 다르다.
- QobManager setMainCam0xfb70은 manager의 포인터만 저장하며 GuiGame에 camera를 전파하지 않는다.

원본 QobCamera init0xecf8은 halfScr의 같은 x필드에160,240을 연속 기록한다(.asm도 확인). 임의의 정상 초기값으로 해석하지 않았다. Title/World가 즉시 setScreenWidth:Height:480/320을 호출하므로 실제 host 변환은 halfScreen(240,160)을 전달한다.

## 호출값

GuiTitle tick0xf464: title 충격 size10,count4,delay.3; episode 충격 size6,count4,delay.3. GobWorld processClear0xc228: size10,count4,delay.3. Player onDamage0x1d140: damage>10일 때 size=max(float(damage)/5,10),count4,delay.3. PlayerEvent Quake의 value/amount/delay를 그대로 전달한다. 새 quake가 도착하면 이전 시간·크기를 덮어쓰지만 그 순간 offset은 유지하며 다음 tick이 갱신한다.

## 검사

`clang++ -std=c++20 -ffp-contract=off -fno-fast-math -fsanitize=address,undefined`로 CameraShake.cpp/CameraShakeTests.cpp와 임시 main을 링크했다. `/tmp/hm-camera-tests` exit0, sanitizer 진단 없음.

검사: 시간0 sentinel, size10 첫 반주기 정점8.75, size6 정점5.25, 후속 양의 hump 감쇠, active x 보존/만료 x·y 초기화, 재시작 offset 보존, 큰 시간 float 정밀도, duration0/count0/NaN time, 화면 offset 부호, camera 없는 HUD, 음수 fixedPos 절삭. 실기기 title bang과 피해 흔들림의 시각 비교는 이 검사로 확인하지 않았다.
