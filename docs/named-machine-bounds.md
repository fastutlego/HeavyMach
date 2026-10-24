# 이름별 기체 생성 경계 비교

2026-10-24. 복원 코드에서 720종 기체의 자식 포함 생성 경계를 수집했다. 원본 이름별 최종값 관측은 아직 실행하지 않았다. 앞선 원본 nil root 표본3개와 이 native 목록을 임의로 연결하지 않는다.

## 실행한 native 수집

`Tools/native_enemy_bounds.cpp`가 현재 Core를 링크하고 실제 `GameSession.start → World.openMap/processRegen → spawnEnemy → makeMachFromDefinition → PartsDefinition → enemyParts/addPart → validationEnemies` 경로를 실행했다. 별도로 복제한 경계 계산식을 사용하지 않았다.

각 기체마다 새 세션을 만들었다. 메모리상의 Stage01 적 목록만 해당 기체1개로 지정하고 원래 map.left+200, y180에 배치했다. 원본 Mach/Parts/Weapon 정의, 지형과 이미지 파일은 바꾸지 않았다. 실제 소비 PNG247개의 크기를 읽었다. 시작 직후 tick·입력 없이 생성 결과를 읽었다. RNG는 이 host fixture의 고정 LCG이며 원본 난수 stream 동등성을 뜻하지 않는다.

- 720/720 생성 성공, 오류0, Unsupported 이벤트0, exit0.
- 서로 다른 binary32 경계 조합125개.
- `build/native-enemy-bounds.json`: 이름·경계 순서·float 값·binary32 비트·생성 위치·HP.
- `build/native-enemy-bounds-manifest.json`: 실제 빌드/실행 명령, Core·정적 라이브러리·도구·원본 데이터·소비 이미지·결과 해시. 빌드/실행 중 Core 해시 변경0.
- 결과 JSON SHA256: `99b5476db264bcc503e8c8fdd9cbdfcebe023b1769aedda2281dc6f195cd429d`.

| 이름 | native 자식 포함 bounds(left,bottom,right,top) |
|---|---|
|ArmoredCar015|(-20,0,20,41.5)|
|ArmoredCar114|(-20,0,20,43.5)|
|ArmoredCar016|(-20,0,20,41.5)|

이는 native 생성 결과다. 원본이 같은 값을 낸다고 아직 주장하지 않는다. root(-20,0,20,40)에 자식이 누적된 결과이므로 이전 root 관측값과 바로 비교해서 차이라고 판정하면 안 된다.

## 원본 관측 도구

`Tools/observe_machine_bounds.py`는 기존 RSP transport를 재사용한다. 기본 실행은 host 검사6개만 수행하며 원본에 연결하지 않는다. `--run`은 별도 관측 앱의 guest 명령에 잠시 Thumb breakpoint를 쓴다. IPA와 저장 파일은 수정하지 않는다.

| 주소 | 원본 바이트 | 의미 |
|---|---|---|
|0x14918|f0 b5|makeMachFromName 진입; r0=self, r2=NUL 종료 C 문자열|
|0x14992|30 1c|재귀 decodeChunk 완료; r8=self, r10=name|
|0x14ae6|05 b0|공통 반환; r0=성공 여부, r8=self, r10=name|

독립 검토에서 entry SP와 후속 frame 연결 검사가 부족함을 찾았다. 원본 prologue 크기에 맞춰 후속 SP==entry SP−0x34 검사를 추가했고 잘못된 SP 거부 검사가 통과했다. 같은 thread/self/name pointer/문자열 바이트도 검사한다. bounds는 self+0x34의16바이트다. 누락 리소스는 decode 완료를 건너뛸 수 있으므로 decode와 return에 대체 정지점을 함께 설치하고 첫 정지에서 둘 다 복구한다. 실패 반환은 그대로 기록하며 성공으로 바꾸지 않는다.

이번에는 이전 관측 앱만 종료했다. 재실행 전 CUA의 Device Hub와 Xcode가 모두 `cgWindowNotFound`를 반환했고, 현행 콘솔 잠금 상태를 확인했다. 잠금 해제나 설정 변경을 시도하지 않았다. 따라서 새 debugger 연결과 이름별 원본 관측은 아직 없다. `build/named-machine-bounds-preparation.json`에 이 경계와 도구 해시를 기록했다.

## 잠금 해제 후 실행할 비교

CUA에서 별도 `HM Nil Observe` 앱을 열고 아래 collector를 연결한 뒤 정상 UI로 Stage01에 진입한다. 실기기는 사용하지 않는다. 기존 파일을 덮어쓰지 않도록 출력 파일은 배타 생성한다.

```sh
python3 HeavyMach/Tools/observe_machine_bounds.py --run --calls 3 --output HeavyMach/build/original-named-machine-bounds.jsonl
python3 HeavyMach/Tools/compare_machine_bounds.py --original HeavyMach/build/original-named-machine-bounds.jsonl --native HeavyMach/build/native-enemy-bounds.json --output HeavyMach/build/named-machine-bounds-comparison.json
```

비교 도구는 같은 이름의 native raw binary32와 원본 decoded/returned 값을 비교한다. 빈 입력·불완전 호출·실패 반환·중복 phase·잘못된 호출 frame은 오류다. 한 비트 차이도 불일치로 남긴다. 비교기 host 검사5개를 통과했다. 위 실제 비교 명령은 아직 실행하지 않았다.

이 검사만으로720종 원본 전체, 모든 생성 경로, 게임 진행 중 bounds 변화나 당시 Apple ARM32 OS를 검증하지 않는다. 현재 native 전체 목록은 후속 원본 표본을 이름으로 대조할 기준이다.
