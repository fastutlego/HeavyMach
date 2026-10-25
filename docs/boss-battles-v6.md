# 보스 전투 v6 — 남은14개 배치 검증

2026-10-24. 최신 offscreen draw 수정이 포함된 코어를 독립 빌드했다. 이전 v5에서 플레이어가 사망했던14개 원본 보스 배치를, 고정한 입력 전략으로 한 번에 실행하여 **14/14 실제 처치, exit0**을 확인했다. 보스 HP·적 배치·지형·탄약 수량·명중·승패 판정은 수정하지 않았다.

이 첫 결과는 해당14개 배치의 전투 검사다. 아래 후속 전체 검사는 첫 실행47/48을 보존하고, Stage16의 입력 전략을 추가한 두 번째 단일 실행에서48/48을 기록했다. 모든 미션 클리어·스테이지 출구 통과·실기기/UI 조작 완료로 확대하지 않는다. 기존 v1~v5 결과와 v6의 모든 실패는 보존한다.

## 고정 조건과 판정

- 원본 mission/stage와 보스 spawn 위치, internal level0, body/top/weapon grade6, 원본 플레이어 무기 및 초기 지급 탄약, RNG seed11. 전투 중 탄약 보충·HP 재설정·무적·적 이동·적 삭제를 하지 않았다. 실제 아이템 획득은 원본 게임이 처리한다.
- target는 원본 이름·spawn 좌표로 찾은 같은 객체 ID다. 그 객체의 HP0 및 removed=true를 관측한 경우에만 성공한다. 사라지기만 하거나 시간제한에 도달한 경우는 실패다. 빈 필터가0건을 실행한 경우도 exit1이다.
- 조작은 GameSession의 touch, 이동 tilt, booster, selectWeapon, 실제 upgrade 확인 API를 사용한다. 최종 v6 입력 좌표는 논리 화면x1..479/y1..319로 제한한다. 레이더의 실제 타격 지점은 touch의 무기 원점 오프셋을 고려해서 지정한다.
- 원본 유한 탄약 감소와 inventory 제거를 그대로 따른다. 성공을 위해 기본 무기를 무한 유한무기로 바꾸거나 탄약 감소를 피하지 않았다. 원본 기본 총만 기존 무제한 규칙을 유지한다.

## 입력에서 확인한 원인

최신 코어에 기존 v5 입력을 그대로 적용하면3/14가 통과했다(Stage18 index1, Stage20 index1, Stage21 index0). 따라서 예전 실패 수를 최신 코어 결과로 재사용하지 않았다.

거리 왕복·연속 부스터만 추가한 전략은0/14였다. 레이더 무기를 먼저 선택하고 조준점 오프셋을 보정한 전략은12/14였다. 화면 범위를 강제한 재실행도12/14로 같았다. 실패를 통과로 바꾸지 않았고 각 결과를 별도 JSONL에 남겼다.

고정 방향 레이더 무기에서 매tick 새 touch move를 보내는 입력은 발사를 방해할 수 있다. 원본 복원 `0x13530` refreshRotationPos는 non-rotating player weapon의 aimDirDest를 parent rotation으로 되돌리고, `0x11e64` tick은 aim 차이가0.1 미만일 때 fireWeapon을 실행한다. 다음 tick 직전 다시 임의 조준각을 쓰면 이 조건을 반복해서 벗어난다. 일부 정지 전략에서는 유한 탄약을 한 발도 쓰지 못한 채 사망했다. 코어를 수정하지 않고, 버튼을 누른 상태를 유지하며2 또는6tick 간격으로 손가락 이동을 전달했다.

최종 입력은 전투 전에 맵별로 고정한다. 실행 중 실패하면 조건을 바꾸거나 다시 시작하는 분기는 없다.

| 대상 | 고정 입력 |
|---|---|
| 아래12개 일반 대상 | v6-radar: 레이더 우선,160~330거리 왕복, 착지/하강 시 원본 부스터 입력 |
| Stage23 index0 | v6-radar-still-hold: 정지 사격,6tick마다 touch move, 누른 상태 유지 |
| Stage35 index0 | v6-radar-still-hop210-hold2: 정지 사격,2tick마다 touch move,210tick에 부스터1회 |

Boss07의 마지막 전략은 실제 처치426tick/플레이어HP440을 기록했다. 비교한120·150·180·270·300tick 부스터 시점은 실패했고,240·330tick도 성공했다. 모든 시도의 결과를 보존했다. 유효한 입력 시점을 찾은 것이며 적 HP나 성공 기준을 바꾼 것이 아니다.

## 최종 단일 실행 결과

| 맵/index | 보스 | 원본 HP→마지막 | 플레이어 HP | 전투 tick | 유한 탄약 소모 | 결과 |
|---|---|---:|---:|---:|---|---|
| Stage29/0 | Boss03 | 20000→0 | 655 | 1313 | RaderLsr:5, RaderMsl:4 | killed |
| Stage17/0 | E05MB03 | 10000→0 | 240 | 310 | RaderLsr:2 | killed |
| Stage17/1 | E05MB04 | 11000→0 | 675 | 278 | RaderLsr:2 | killed |
| Stage18/0 | E05MB05 | 12000→0 | 680 | 338 | RaderLsr:2 | killed |
| Stage18/1 | E05MB02 | 9000→0 | 840 | 154 | RaderLsr:1 | killed |
| Stage19/0 | Boss01 | 30000→0 | 735 | 324 | RaderLsr:2 | killed |
| Stage20/1 | MBoss0601 | 11000→0 | 840 | 448 | RaderLsr:3 | killed |
| Stage21/0 | MBoss0701 | 12000→0 | 770 | 316 | RaderLsr:3 | killed |
| Stage23/0 | E06MB04 | 15000→0 | 815 | 258 | RaderLsr:2 | killed |
| Stage24/0 | Boss06 | 50000→0 | 545 | 665 | RaderLsr:5 | killed |
| Stage30/0 | E07MB01 | 15000→0 | 840 | 335 | RaderLsr:2 | killed |
| Stage32/0 | E07MB02 | 15000→0 | 595 | 287 | RaderLsr:2 | killed |
| Stage34/0 | E07MB03 | 15000→0 | 655 | 269 | RaderLsr:2 | killed |
| Stage35/0 | Boss07 | 60000→0 | 440 | 426 | RaderLsr:4 | killed |

## 재현

```sh
cmake -S HeavyMach -B /tmp/hm-boss-v6-build -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/hm-boss-v6-build --target HeavyMachValidationBossTests -j4
/tmp/hm-boss-v6-build/HeavyMachValidationBossTests /tmp/hm-boss-v6-final.jsonl 'Stage29:0,Stage17:0,Stage17:1,Stage18:0,Stage18:1,Stage19:0,Stage20:1,Stage21:0,Stage23:0,Stage24:0,Stage30:0,Stage32:0,Stage34:0,Stage35:0' v6-verified
```

최종 CMake 빌드와 위 실행 모두 exit0이다. 최초 독립 빌드의 코어에 최신 runner만 연결한 실행도14/14였으며, 마지막에는 CMake target을 다시 빌드하여 최신 소스 조합으로14/14를 재확인했다. 중간에 다른 작업자가 CMake에 등록 중이던 SurvivalTests 파일이 아직 없어서 일시적으로 재생성이 실패한 로그도 보존했다. 그 동안 입력 비교는 이미 빌드한 같은 코어에 runner만 연결했으며, 최종 CMake 빌드는 해당 파일 생성 후 성공했다.

산출물은 Git 제외 경로 `build/boss-v6/`에 보존한다.

- `hm-boss-v6-baseline.jsonl/.log`: 최신 코어의 기존14실패 재실행,3/14.
- `hm-boss-v6-strafe.jsonl/.log`: 회피만 변경,0/14.
- `hm-boss-v6-radar-clamped.jsonl/.log`: 화면 내 레이더 입력,12/14.
- `hm-boss-v6-radar-still-hold.jsonl/.log`: Stage23 성공을 포함한 hold 비교.
- `hm-boss-v6-hop-*.jsonl/.log`: Boss07 실제 부스터 시점 비교, 실패 포함.
- `hm-boss-v6-final.jsonl/.log`: 최종14/14 단일 실행.
- `hm-boss-v6-final-build.log`: 최종 CMake target 빌드.
- `ValidationBossTests.cpp`, `binary-hashes.json`: 최종 runner 사본과 빌드된 코어/실행 파일의 SHA256.

조작 중 다른 적도 살아 있고 원본 입력으로 교전한다. 보스 접근 fixture의 과거 적 생성 이력 차이는 [v5 보고서](boss-battles-v5.md)에 남아 있다. v6는 그 차이를 적 삭제로 없애지 않았다. 물리 기기·시뮬레이터 UI는 이번 작업에서 조작하지 않았다.


## 최신 코어 전체48 단일 실행

2026-10-24 후속. camera offset 기본값0 및 map rounding 변경이 포함된 현재 소스로 독립 CMake target을 다시 빌드했다. `full-verified`는 기존14개에 위 v6 입력을 선택하고, 나머지34개에는 기존 baseline 입력을 선택한다. Stage25 index1은 index0부터 같은 세션에서 실제 이동·전투를 수행하는 순차 접근을 사용한다. 이 선택은 전투 전에 고정하며 재시도하지 않는다.

**결과:47/48, exit1.** 기존14개는14/14, 나머지는33/34다. 이전에 성공했던 Stage16 index0 E05MB02는 이번 단일 실행에서 실패했다. 원본HP9000→364,1107tick 후 플레이어HP0, 유한 탄약716이 남았다. Laser10·ElShock30·Missile03 3회분을 실제 소비했다. 이 기록은 전체 탄약 소진을 원인으로 지지하지 않는다. 현재 입력에서의 사망이며, 이번 검사만으로 원본 코드 차이 또는 특정 최신 수정의 인과관계를 확정하지 않는다. 이전 성공 결과를 삭제하거나 이번 실패로 덮어쓰지 않았다.

Stage25 index1은 먼저 MBoss08의HP0/removed를 확인한 뒤 같은 세션으로 이동하여 MBoss07도 처치했다. 접근3410tick+교전332tick, 플레이어HP10, Laser10·ElShock28회분 소비다. 모든 미션을 처음부터 끝까지 클리어했다는 검사가 아니다.48개 원본 보스 배치에서 조건을 준비하고 지정 보스를 실제 전투로 처치하는 검사다.

```sh
cmake --build /tmp/hm-boss-v6-build --target HeavyMachValidationBossTests -j4
/tmp/hm-boss-v6-build/HeavyMachValidationBossTests /tmp/hm-boss-v6-full-final.jsonl '' full-verified
```

`build/boss-v6/hm-boss-v6-full-final.jsonl/.log`에48개 전체 결과를 보존했다. `hm-boss-v6-full-build.log`는 빌드 로그, `hm-boss-v6-full-hashes.json`은 실행 당시 Sources/Core 파일·runner·CMakeLists·코어 정적 라이브러리·실행 파일 SHA256이다. `ValidationBossTests-full.cpp`는 사용한 runner 사본이다. 앞선14개 검사 파일과 해시는 별도 이름으로 보존했다.

| 맵 | index | 원본 보스 | 입력 | 결과 | 남은 보스HP | 플레이어HP |
|---|---:|---|---|---|---:|---:|
| Stage03 | 0 | MBoss01 | baseline | 처치 | 0 | 757 |
| Stage04 | 0 | Boss02 | baseline | 처치 | 0 | 595 |
| Stage05 | 0 | MBoss02 | baseline | 처치 | 0 | 710 |
| Stage06 | 0 | MBoss02 | baseline | 처치 | 0 | 780 |
| Stage06 | 1 | MBoss03 | baseline | 처치 | 0 | 695 |
| Stage07 | 0 | MBoss02 | baseline | 처치 | 0 | 840 |
| Stage08 | 0 | MBoss04 | baseline | 처치 | 0 | 695 |
| Stage09 | 0 | Boss04 | baseline | 처치 | 0 | 375 |
| Stage10 | 0 | MBoss04 | baseline | 처치 | 0 | 565 |
| Stage10 | 1 | MBoss05 | baseline | 처치 | 0 | 720 |
| Stage11 | 0 | MBoss04 | baseline | 처치 | 0 | 590 |
| Stage11 | 1 | MBoss03 | baseline | 처치 | 0 | 720 |
| Stage11 | 2 | MBoss05 | baseline | 처치 | 0 | 643 |
| Stage12 | 0 | MBoss06 | baseline | 처치 | 0 | 695 |
| Stage12 | 1 | MBoss04 | baseline | 처치 | 0 | 780 |
| Stage13 | 0 | MBoss05 | baseline | 처치 | 0 | 840 |
| Stage13 | 1 | MBoss07 | baseline | 처치 | 0 | 780 |
| Stage14 | 0 | Boss05 | baseline | 처치 | 0 | 105 |
| Stage25 | 0 | MBoss08 | baseline | 처치 | 0 | 660 |
| Stage25 | 1 | MBoss07 | sequential + baseline | 처치 | 0 | 10 |
| Stage26 | 0 | MBoss08 | baseline | 처치 | 0 | 840 |
| Stage26 | 1 | MBoss09 | baseline | 처치 | 0 | 840 |
| Stage27 | 0 | MBoss09 | baseline | 처치 | 0 | 655 |
| Stage27 | 1 | MBoss09 | baseline | 처치 | 0 | 528 |
| Stage28 | 0 | MBoss09 | baseline | 처치 | 0 | 574 |
| Stage28 | 1 | MBoss08 | baseline | 처치 | 0 | 514 |
| Stage28 | 2 | MBoss08 | baseline | 처치 | 0 | 480 |
| Stage28 | 3 | MBoss09 | baseline | 처치 | 0 | 630 |
| Stage28 | 4 | MBoss10 | baseline | 처치 | 0 | 575 |
| Stage29 | 0 | Boss03 | v6-radar | 처치 | 0 | 655 |
| Stage15 | 0 | MBoss0501 | baseline | 처치 | 0 | 640 |
| Stage16 | 0 | E05MB02 | baseline | player_died | 364 | 0 |
| Stage17 | 0 | E05MB03 | v6-radar | 처치 | 0 | 240 |
| Stage17 | 1 | E05MB04 | v6-radar | 처치 | 0 | 675 |
| Stage18 | 0 | E05MB05 | v6-radar | 처치 | 0 | 680 |
| Stage18 | 1 | E05MB02 | v6-radar | 처치 | 0 | 840 |
| Stage19 | 0 | Boss01 | v6-radar | 처치 | 0 | 735 |
| Stage20 | 0 | E06MB01 | baseline | 처치 | 0 | 130 |
| Stage20 | 1 | MBoss0601 | v6-radar | 처치 | 0 | 840 |
| Stage21 | 0 | MBoss0701 | v6-radar | 처치 | 0 | 770 |
| Stage22 | 0 | MBoss0801 | baseline | 처치 | 0 | 235 |
| Stage23 | 0 | E06MB04 | v6-radar-still-hold | 처치 | 0 | 815 |
| Stage24 | 0 | Boss06 | v6-radar | 처치 | 0 | 545 |
| Stage30 | 0 | E07MB01 | v6-radar | 처치 | 0 | 840 |
| Stage31 | 0 | MBoss08 | baseline | 처치 | 0 | 695 |
| Stage32 | 0 | E07MB02 | v6-radar | 처치 | 0 | 595 |
| Stage34 | 0 | E07MB03 | v6-radar | 처치 | 0 | 655 |
| Stage35 | 0 | Boss07 | v6-radar-still-hop210-hold2 | 처치 | 0 | 440 |


## 최종 전체48 단일 실행 —48/48

첫 전체 실행의 Stage16 index0 실패를 보존한 뒤 해당 배치만 `v6-radar`로 검사했다.174tick에 원본HP9000→0/removed=true, 플레이어HP815, RaderLsr1회분 소비로 성공했다. 이 입력을 `full-verified`의 Stage16 분기에 고정했다. 코어·원본 데이터·HP·적 배치·탄약·판정은 변경하지 않았다.

그 상태로 최신 코어 전체48개를 다시 한 번 실행했다. **48/48 실제 지정 보스 처치, exit0**이다. 시작 전 고정된15개 배치에 v6 입력,33개에 기존 baseline 입력을 사용하며 Stage25 index1은 이전 보스부터 같은 세션에서 순차 전투한다. 실행 중 실패를 감지해 전략을 바꾸거나 재시작하지 않는다. 앞선47/48 실행은 그대로 남겼다.

```sh
cmake --build /tmp/hm-boss-v6-build --target HeavyMachValidationBossTests -j4
/tmp/hm-boss-v6-build/HeavyMachValidationBossTests /tmp/hm-boss-v6-full2-final.jsonl '' full-verified
```

최종 증거는 `build/boss-v6/hm-boss-v6-full2-final.jsonl/.log`, 빌드는 `hm-boss-v6-full2-build.log`, SHA256은 `hm-boss-v6-full2-hashes.json`, runner 사본은 `ValidationBossTests-full2.cpp`다. 시작 시 기록한 모든 코어 소스·CMake·runner·정적 라이브러리·실행 파일의 해시가 완료 후에도 같음을 확인했다. Stage16 단독 비교는 `hm-boss-v6-stage16-radar.jsonl/.log`다.

이 결과는48개 원본 보스 배치의 실제 입력 전투 검사다. 원본 전체 미션을 연속 클리어하거나 모든 출구·실기기/UI 화면을 검사했다는 의미는 아니다. 원본 유한 탄약과 실제 충돌/피해/사망 경로를 사용했다.

| 맵 | index | 원본 보스 | 결과 | 남은 보스HP | 플레이어HP |
|---|---:|---|---|---:|---:|
| Stage03 | 0 | MBoss01 | 처치 | 0 | 757 |
| Stage04 | 0 | Boss02 | 처치 | 0 | 595 |
| Stage05 | 0 | MBoss02 | 처치 | 0 | 710 |
| Stage06 | 0 | MBoss02 | 처치 | 0 | 780 |
| Stage06 | 1 | MBoss03 | 처치 | 0 | 695 |
| Stage07 | 0 | MBoss02 | 처치 | 0 | 840 |
| Stage08 | 0 | MBoss04 | 처치 | 0 | 695 |
| Stage09 | 0 | Boss04 | 처치 | 0 | 375 |
| Stage10 | 0 | MBoss04 | 처치 | 0 | 565 |
| Stage10 | 1 | MBoss05 | 처치 | 0 | 720 |
| Stage11 | 0 | MBoss04 | 처치 | 0 | 590 |
| Stage11 | 1 | MBoss03 | 처치 | 0 | 720 |
| Stage11 | 2 | MBoss05 | 처치 | 0 | 643 |
| Stage12 | 0 | MBoss06 | 처치 | 0 | 695 |
| Stage12 | 1 | MBoss04 | 처치 | 0 | 780 |
| Stage13 | 0 | MBoss05 | 처치 | 0 | 840 |
| Stage13 | 1 | MBoss07 | 처치 | 0 | 780 |
| Stage14 | 0 | Boss05 | 처치 | 0 | 105 |
| Stage25 | 0 | MBoss08 | 처치 | 0 | 660 |
| Stage25 | 1 | MBoss07 | 처치 | 0 | 10 |
| Stage26 | 0 | MBoss08 | 처치 | 0 | 840 |
| Stage26 | 1 | MBoss09 | 처치 | 0 | 840 |
| Stage27 | 0 | MBoss09 | 처치 | 0 | 655 |
| Stage27 | 1 | MBoss09 | 처치 | 0 | 528 |
| Stage28 | 0 | MBoss09 | 처치 | 0 | 574 |
| Stage28 | 1 | MBoss08 | 처치 | 0 | 514 |
| Stage28 | 2 | MBoss08 | 처치 | 0 | 480 |
| Stage28 | 3 | MBoss09 | 처치 | 0 | 630 |
| Stage28 | 4 | MBoss10 | 처치 | 0 | 575 |
| Stage29 | 0 | Boss03 | 처치 | 0 | 655 |
| Stage15 | 0 | MBoss0501 | 처치 | 0 | 640 |
| Stage16 | 0 | E05MB02 | 처치 | 0 | 815 |
| Stage17 | 0 | E05MB03 | 처치 | 0 | 240 |
| Stage17 | 1 | E05MB04 | 처치 | 0 | 675 |
| Stage18 | 0 | E05MB05 | 처치 | 0 | 680 |
| Stage18 | 1 | E05MB02 | 처치 | 0 | 840 |
| Stage19 | 0 | Boss01 | 처치 | 0 | 735 |
| Stage20 | 0 | E06MB01 | 처치 | 0 | 130 |
| Stage20 | 1 | MBoss0601 | 처치 | 0 | 840 |
| Stage21 | 0 | MBoss0701 | 처치 | 0 | 770 |
| Stage22 | 0 | MBoss0801 | 처치 | 0 | 235 |
| Stage23 | 0 | E06MB04 | 처치 | 0 | 815 |
| Stage24 | 0 | Boss06 | 처치 | 0 | 545 |
| Stage30 | 0 | E07MB01 | 처치 | 0 | 840 |
| Stage31 | 0 | MBoss08 | 처치 | 0 | 695 |
| Stage32 | 0 | E07MB02 | 처치 | 0 | 595 |
| Stage34 | 0 | E07MB03 | 처치 | 0 | 655 |
| Stage35 | 0 | Boss07 | 처치 | 0 | 440 |
