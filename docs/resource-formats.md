# HeavyMach 1.13 리소스 복원

원본 `HeavyMach.app`의 776개 파일을 SHA-256으로 기록했다. `Resources/inventory.json`은 원본 경로·크기·해시를 보존한다. 내보내기 종료 시 원본 전부를 다시 해시해 변경이 없음을 확인한다. 실행 파일 해시는 `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`이다.

`Resources/Images`는 이미지 695개, `Audio`는 원본 WAV 31개, `RawData`는 원본 데이터 46개를 담는다. `Data`는 46개 파일의 구조·오프셋·원시 payload hex·해석한 필드를 JSON으로 보존한다. `sprites.json`은 부품 294개의 소켓·이미지·프레임 좌표를 연결한다. 정확히 대응하는 이미지가 원본에 없는 34개는 null로 남긴다.

## 바이너리 구조

모든 정수와 float32는 little endian이다. 노드는 `u32 declared_size, u32 type, u32 data_size, payload[data_size], u32 child_count, child nodes` 순서다. 일부 declared_size는 실제 크기와 맞지 않는다. 원본처럼 payload 길이와 자식 개수로 순회한다. 46개 파일 모두 끝까지 소비한다. 고정 문자열은 NUL까지 해석하고 뒤의 잔여 바이트는 JSON hex에 보존한다.

### WeaponInfo.mch

160개 type1 노드, payload 156바이트. 원본 `GlobalInfo readWeaponChunk` 0x10e00을 근거로 했다.

|오프셋|필드|형식|
|---|---|---|
|0|name|char[24]|
|24,26|wpnType,dmg|int16|
|28,29|pierce,shotCnt|uint8|
|30,34,38,42,46,50|shotDly,delay,spd,spdRand,aimRand,g|float32|
|54|charge|int16|
|56|liveTm|float32|
|60,61|rotWpn,uiTile|uint8|
|62|bulletTile|char[24]|
|86|bulletTileNo|int16|
|88|effectType|char[16]|
|104|effectTile|char[24]|
|128,130|effectTileNo,shell|int16|
|132|fireSFX|char[24]|

effectType 문자열은 Normal=1, Flame=2, Smoke=3, Trail=4, Laser=5, 그 외=0이다. effectTileNo는 디컴파일 표현에서 누락되지만 `weapon-reader-assembly.json`의 0x10eae–0x10eb2 load와 0x110b8 이후 호출 인수에서 확인했다. 자산 이름은 호스트 포인터나 사운드 핸들로 바꾸지 않고 문자열로 제공한다.

### PartsInfo.mch / MachInfo.mch

Parts 294개 type1: name char[24], partsType uint8@24, tileCnt uint8@25, socketCnt uint8@26, socketCnt개의 float32(x,y)@27. 원본 0x10a54가 근거다.

Mach 720개 type3: name char[24], hpMax int32@24, giveExp int32@28, machType uint8@32. 자식 type4는 name char[24], baseSocket uint8@24, zOrder uint8@25이며 재귀적인 부품 트리를 이룬다. 이름과 필드는 원본 `GobHvM_Enemy decodeChunk:Data:Pos:` 0x147e8의 setter와 대조했다. 부모·자식 구조를 C++ `MachPartDefinition.children`에 그대로 보존한다.

### 지도 43개

Stage01–Stage42, Survival이다. type2는 left/top/right/bottom float32 4개(0xda80), type9는 배경 이미지 문자열(0xdfa8)이다. type3 아래 type5 경로가 있고 type6의 float32(x,y)가 관절이다(0xdbfc). 선분은 현재 관절을 origin, 이전 관절을 second로 사용한다. C++ `collisionVectors()`는 경로 안의 prev/next만 연결한다. 반환 벡터를 복사하거나 재할당하면 내부 포인터를 다시 만들어야 한다.

type7 아래 type8 오브젝트: u32 문자열 길이, 그 길이의 이름, float32 x/y, uint8 layer flag. 원본은 이름에 `.png`를 붙인다(0xde40). flag=0이면 실제 렌더 layer1, 그 외 layer2다(0xcd00). JSON은 flag와 layer를 함께 보존하며 C++은 실제 layer를 제공한다.

type10 아래 type11 적 배치: name char[24], float32 x/y@24/28. 원본 reader 0xe1b4처럼 Player 마커를 소비하되 적 목록에서는 제외한다. Boss/MBoss 접두사 또는 위치2/3의 MB 문자열은 boss 표시다. 게임 좌표는 원본 값을 유지한다.

## 이미지와 atlas

CgBI PNG는 원본 DEFLATE와 PNG filter를 풀고 BGRA를 RGBA로 바꾼다. premultiplied RGB는 alpha로 나눠 표준 PNG를 만든다. 부분 투명 픽셀의 원래 RGB는 정수 반올림 때문에 완전 역복원할 수 없다. 일반 JPEG는 macOS sips로 같은 크기의 PNG로 변환한다. 원본 `Icon.png`의 실제 형식은 JPEG512이며 같은 그림을 PNG1024로 변환해 AppIcon에 사용한다.

`GlobalInfo getTileOfParts` 0x11380과 `makePartsFromName` 0x11554: 일반 부품은 name+.png, tileCnt0은 PartsDummy.png. 열 수는 max(tileCnt,1), 행은1이다. socket0은 중심 보정값이며 bounds는 `[-w/2-sx,-h/2-sy,w/2-sx,h/2-sy]`다. `QTile tileSplitX:splitY:` 0x6d9c와 draw 0x6f28은 행 우선 인덱스를 사용한다. `atlases.json`은 PNG 왼쪽 위 기준 각 프레임의 x/y/w/h를 기록한다. 무기 bullet atlas는 EnemyBullet0 접두사4×4, EnemyMissile 접두사2×4, 그 외4×8이다.

## 재생성과 확인

```sh
python3 tools/export_resources.py --source /path/to/original/HeavyMach.app
```

`--data-only`는 기존 이미지 파생물을 유지하고 데이터 해석을 갱신한다. REA 증거는 `Resources/Evidence/*.json`에 procedure 주소·원본 해시·Ghidra 결과·evidence ID를 함께 보존했다. 별도 subprocess 세션으로 수집했으며 공유 활성 target은 변경하지 않았다.

`GameData::loadDirectory(path)`는 C++20 표준 라이브러리만 사용한다. `Tests/GameDataTests.cpp`의 `runGameDataTests()`를 ASan+UBSan, `-Wall -Wextra -Werror`로 실행했다. 원본46개 로드, 레코드 개수와 대표 값, 선분 연결, 322개 잘림·길이 오류·추가 바이트 거부가 통과했다. 이 검사는 데이터 복원을 확인하며 전체 게임 진행의 동일성을 확인하지 않는다.
