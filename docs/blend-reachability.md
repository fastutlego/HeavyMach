# blend1·상태 상속의 실제 도달 경로 조사

2026-10-24. **현재 원본 패키지와 복원 앱에서 blend1 또는 상태 상속 mode로 들어가는 활성 경로를 찾지 못했다.** 원본 renderer에는 이 분기가 있지만, 확인한 생성·설정·draw 경로는 mode0 또는2를 사용한다. 현재 HeavyMach1 게임 동작을 복원하는 데 이 두 미지원 분기가 필수라는 근거는 없다. 범용 Tile2D/Trail API에서 blend1·상태 상속 mode를 입력하면 미지원 오류를 반환한다.

코드·자산·UI는 변경하지 않았다. 이 문서만 작성했다. 새 REA 서버 세션을 열지 않고 보존된 원본 pseudo/assembly와 RawData를 재사용했다.

## 조사 범위

- `Analysis/export-manifest.json`: 원본SHA256 `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`, inventory1133개, local function export900개.
- 위 함수의 `setBlendType:`, `_blendType`, 전체 Tile2D draw selector, 직접 GL blend 호출을 검색했다. ivar의 초기화·setter·reader와 생성 경로를 대조했다.
- `Resources/RawData`: WeaponInfo/PartsInfo/MachInfo3개와 map43개를 실제 chunk parser로 읽었다. map43개 모두 group type2/7/3/9/10으로 구성됐다. 이는 RawData 전체 파일 수이며 플레이 가능한 stage 개수로 바꾸어 말하지 않는다.
- 현재 Core의 DrawSprite/CombatEvent/TrailMesh 생산자와 iOS의 HMMedia 호출 경로를 대조했다.

## 설정 값과 저장 위치

원본 export의 `setBlendType:` 호출은 총25개다. 인자는mode0이6개, mode2가19개이며 변수 인자나mode1/3/범위 밖 인자는 없다.

| 값 | 호출 함수 주소 | 경로 |
|---|---|---|
|0|0x11e64(2회),0xa198,0x14418,0x1c408,0x1c64c|무기 부수 효과·충돌 폭발·적 파괴·획득 효과|
|2|0xa444,0x9dc4|피해 flash·bullet trail|
|2|0xb8f0,0xbf74|로딩 이미지 효과|
|2|0x151a0(8회),0x18158,0x186d8(6회)|게임 HUD·숫자 font·슬롯 선택|

단순 문자열 검색 결과만으로 끝내지 않고 저장 경로도 확인했다.

- QobImage initWithTile `0x8b08`: `_blendType=0`.
- QobImageFont initWithTile `0x8db4`: `_blendType=0`.
- QobParticle reset `0x92b4`: 상속받은 blend field를2로 다시 설정한다. assembly `0x9374 movs r2,#2`→`0x937c str r2,[r4,r3]`도 일치한다. pool 재사용이 이전 임의 mode를 유지하는 경로가 아니다.
- QobImage setter `0x8da8`과 QobImageFont setter `0x91ec`는 전달된r2를 해당 ivar에 저장한다. metadata의 ivar offset은 각각84와352다. setter 자체는 범위 검사하지 않지만 위 게임 호출은0/2다.
- GobTrailEffect init `0x1ab20`은 super init 후 wind/diffuse를 설정한다. 실제 생성자 호출 `0x9dc4`는 initWithTile 뒤 setBlendType2를 명시한다.

## draw까지 전달되는 값

- QobImage draw `0x8cf0`, QobParticle draw `0x95dc`, QobImageFont draw `0x9054`는 위 field를 그대로 전달한다.
- Parts draw `0x850c`는 mode0을 직접 전달한다.
- GobBullet draw `0xab04`는 mode2를 직접 전달한다.
- Tile2D 편의 wrapper `0x7134`, `0x719c`, `0x71d0`, `0x7208`은 mode0을 전달한다. `0x7134.asm`의stack 인자 `0x7142..0x7144`도0이다.
- `0x7168`의 명시적 blend wrapper는 임의 인자를 받을 수 있다. 그러나 export 전체에서 짧은 `drawTile:x:y:blendType:alpha:` selector를 호출하는 게임 함수는 찾지 못했다. wrapper가 존재한다는 사실만으로 mode1 활성 경로로 분류하지 않았다.
- full Tile2D draw `0x6f28` 및 Trail draw `0x1b8dc`의 switch에는mode1 SRC_COLOR와 기본 상태 상속 분기가 있다. 이는 소비자 지원 범위이며 입력 생산자가 해당 값을 만든다는 증거는 아니다.

현재 Core의 DrawSprite 기본값은0, particle reset은2, 일반 parts는0, bullet은2다. GameSession은 Combat/Enemy 이벤트의 blend 값을 전달한다. 해당 생산자는0/2를 쓴다. iOS HUD·메뉴의 literal blend도0/2이고 HMGameView는 sprite 값을 전달한다. 사용자 touch·무기 선택·업그레이드·저장값에서 blend 정수를 받는 공개 경로는 확인하지 못했다.

## RawData가 blend 값을 공급하는지

원본 reader까지 추적했다.

- Weapon reader `0x10e00`: 피해·속도·effectType·effectTile 등 필드를 WeaponInfo setter로 옮긴다. blend 필드는 없다. 실제160개 record도 해당156-byte payload 구조다.
- Parts reader `0x10a54`: name/type/tile count/socket count/좌표를 읽는다. blend 설정은 없다.
- Mach reader `0x10c58`은 chunk를 NSData로 보존하고, enemy load `0x14918`→decodeChunk `0x147e8`가 parts/socket/zOrder를 구성한다. zOrder를 blend mode로 읽는 경로는 없다.
- map object reader `0xde40`은 마지막 byte를 regen record offset0x10에 저장한다. 소비자 `0xcd00`은 이를 **layer1/2** 선택에 사용한다. blend1/2를 의미하는 byte가 아니다. 생성하는 QobImage의 blend는 기본0이다.

RawData에서 정수1을 발견하는 것만으로 blend1을 활성으로 판정할 수 없다. 위 reader/consumer와 연결되는 필드만 판단에 사용했다.

## Trail texture의 실제 형식

원본 `0x9dc4`는 effectType4(Trail) 또는5(Laser)일 때만 GobTrailEffect를 만들고 mode2를 설정한다. WeaponInfo160개를 실제 RawData에서 읽은 결과 이 조건에 맞는 record는30개다.

| effectType·asset | record 수 | 원본/파생 형식 |
|---|---:|---|
|Trail / SmokeTrail2.png|5|RGBA8 / RGBA8|
|Laser / LaserTrail.png|2|RGBA8 / RGBA8|
|Laser / SmokeTrail2.png|1|RGBA8 / RGBA8|
|Trail / SmokeTrail1.png|22|RGBA8 / RGBA8|

플레이어 이름은Missile01/02/03,RaderBomb,RaderMsl,Laser,RaderLsr,ElShock이다. 나머지22개는 적 무기다. 이 표는 데이터상 선택 가능한 record 전부를 포함하며, 모든 적 무기가 실제 stage에서 발사됐다는 runtime 기록으로 확대하지 않는다.

세 asset 모두40×64이며 원본PNG colorType6, 파생PNG colorType6, 기존 ImageIO 검사 alphaInfo3(last)다. `Resources/texture-formats.json`에 원본/파생SHA256과 전수 형식 대조가 남아 있다. RGB565 원본16개는 로딩JPEG·StageClear·Icon이며 Trail asset과 겹치지 않는다.

따라서 현재 HMMedia drawTrail의 기본RGBA texture format은 원본에서 가능한 이 세 Trail asset에 맞는다. RGB565 Trail의 활성 경로는 없다. 미래에 다른 asset을 연결할 경우 format 전달을 추가해야 하지만, 현재 패키지에서 요구되는 수정으로 보고하지 않는다.

## 판단과 남은 한계

확인한 정적 경로와 원본 데이터에서는0/2로 닫힌 값 집합이다. blend1/상태 상속을 구현하지 않아 현재 원본 게임 기능이 누락됐다는 근거는 발견하지 못했다. 추후 구체적인 원본 호출·asset 필드·실행 로그가 나오면 그 경로를 다시 추적해야 한다.

900개 local export에 대한 검색·원본 reader/ivar/소비자 대조는 수행했지만, 모든 명령에 대한 형식 검증이나 전체 앱 dynamic dispatch 도달성 증명은 아니다. decompiler가 이름을 복원하지 못한 별칭 쓰기·잘못된 데이터·메모리 손상까지 불가능하다고 주장하지 않는다. 정상 원본 패키지의 기능 범위를 판단한 결과다. 실기기·UI 조작 및 새 전투 실행은 하지 않았다.
