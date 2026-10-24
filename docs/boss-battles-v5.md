# 보스 실제 전투 v5 — 2026-10-24

48개 원본 배치 중 33개를 실제 사격으로 처치했다. 14개는 플레이어가 먼저 사망했고, Stage25의 두 번째 보스는 이전 보스의 진행 경계 때문에 접근하지 못했다. runner는 실패를 유지하며 exit1을 반환했다. 보스 처치는 스테이지 클리어·전체 미션 진행 성공과 구분한다.

검사 조건은 internal level0, 차체·상체·무기 grade6, 원본 무기와 기본 지급 탄약, seed11이다. 원본 HP·적 위치·지형·명중 판정은 바꾸지 않았다. 같은 객체 ID와 원본 spawn 좌표로 목표를 식별하고, HP0 및 removed=true를 관측한 경우만 성공 처리했다. 적이 사라지기만 한 경우는 성공으로 처리하지 않는다.

## 실제 검사와 입력 전략

- 실제 GameSession 선택·touch·tick·이동·부스터를 호출했다. 적 bounds 중앙을 겨냥하고, 원본 중력과 속도로 낮은 탄도를 계산했다. 산포·탄환 충돌·피해는 원본 코드가 처리한다.
- Laser/ElShock, 미사일, 레이더, 대포 순으로 보유 무기를 선택한다. 탄약 고갈 시 원본 inventory 삭제를 반영해 이름으로 다시 찾는다. 탄약을 전투 중 보충하지 않는다.
- 거리 유지 입력과 부스터를 사용한다. 레벨업으로 정지한 경우 실제 confirmUpgrade→finishUpgradeSlide를 호출한다. 강제 unpause나 승패 지정은 사용하지 않는다.
- 가까운 시작점에서 안정 지면이 없으면 원본 맵 전체에서 왼쪽 지면을 검색한다. 실제 World collisionCandidates와 120tick 착지를 검사한다. 먼 위치는 실제 이동으로 접근한다.

## 원인 조사

원본 REA 0xcd00의 재생성 조건은 enemyX-playerX<500이며 뒤쪽 하한이 없다. 보스 직전으로 위치를 주입하면 지나온 적도 살아서 생성된다. 0xd2f4의 정리는 listMapObject만 대상으로 한다. 2차 계측에서 46/48배치에 240거리 이상 뒤쪽 적이 있었고 최대76대였다. 16개 배치에는 이전 보스도 남았다. 이는 정상 진행의 적·RNG·AI 이력과 다르다. 어떤 적이 피해를 얼마나 줬는지까지 이 통계로 단정하지 않는다.

기존 지면 계산은 전체 collision vector를 검색하여 실제 x별 충돌 후보와 달랐다. 기존48개 중15개 위치는 runtime mapHeight가 지면 없음10000이었다. 기존40실패 중9개는 2차 실행에서 맵 아래 추락을 확인했다. fixture만 수정했으며 v3 이후 정지 전투에서 해당 추락은0건이다.

기존 조준은 적의 발 위치를 겨냥했다. 적 bounds의 bottom은0, top은최소40이며 탄환은 지형 충돌을 먼저 검사한다. 대포 속도500·중력10/tick은180거리에서 약39만큼 낙하하므로 단순 직선 조준으로 지면에 먼저 맞을 수 있다. 실제 무기 회전 원점을 사용하고 적 중앙과 탄도를 반영했다. 2차 검사에서 뒤쪽 조준/근접 터치 거절 프레임은0이었고, 모든 탄약을 소진한 배치도 없었다.

Stage25 boss1은 시작x6601, 목표x7221이나 이전 boss0가 frontLine1286·bossEncounter1을 설정한다. 목표는500범위 밖이라 아직 생성되지 않고, 원본 이동 규칙이 전진을 막는다. 이 경계를 제거하지 않았다. v5에서는 이전 보스 이력이 없어 막혔다. 아래 후속 순차 검사는 이전 보스를 실제로 처치하여 이 차이를 해소했다.

## 반복 결과 보존

| 실행 | 변경 | 처치/48 | 나머지 결과 |
|---|---|---:|---|
| v1 | 기존 정지 조준 | 8 | 사망40 |
| v2 | 실제 무기 원점 계측 | 4 | 사망44 |
| v3 | runtime 안정 지면·근거리 검색 | 5 | 사망35, 미생성4, 지면거절2, 시간초과1, 레벨업정지1 |
| v4 | 전체 맵 지면 검색·실제 접근 | 6 | 사망38, 레벨업정지2, 접근미완료1, 시간초과1 |
| v5 | 중앙·탄도 조준, 무기·이동 전략 | 33 | 사망14, 접근미완료1 |

v1의 8개 성공은 모두 v5에도 포함된다. 여러 전략 성공의 합집합도33/48이다. 아직 모든 보스를 통과하지 않았다.

## 배치별 v5 결과

| 맵 | 보스/배치 index | 초기→마지막 HP | 플레이어 HP | 전투 tick | 접근 tick | 결과 |
|---|---|---:|---:|---:|---:|---|
| Stage03 | MBoss01/0 | 2000→0 | 702 | 195 | 0 | killed |
| Stage04 | Boss02/0 | 10000→0 | 595 | 881 | 0 | killed |
| Stage05 | MBoss02/0 | 2000→0 | 650 | 258 | 0 | killed |
| Stage06 | MBoss02/0 | 2000→0 | 720 | 318 | 0 | killed |
| Stage06 | MBoss03/1 | 2500→0 | 670 | 319 | 0 | killed |
| Stage07 | MBoss02/0 | 2000→0 | 840 | 197 | 148 | killed |
| Stage08 | MBoss04/0 | 2500→0 | 620 | 320 | 0 | killed |
| Stage09 | Boss04/0 | 12000→0 | 295 | 1238 | 0 | killed |
| Stage10 | MBoss04/0 | 2500→0 | 790 | 321 | 0 | killed |
| Stage10 | MBoss05/1 | 3000→0 | 660 | 382 | 0 | killed |
| Stage11 | MBoss04/0 | 2500→0 | 590 | 382 | 0 | killed |
| Stage11 | MBoss03/1 | 2500→0 | 780 | 323 | 0 | killed |
| Stage11 | MBoss05/2 | 3000→0 | 703 | 382 | 193 | killed |
| Stage12 | MBoss06/0 | 3000→0 | 585 | 446 | 0 | killed |
| Stage12 | MBoss04/1 | 2500→0 | 630 | 322 | 0 | killed |
| Stage13 | MBoss05/0 | 3000→0 | 840 | 382 | 0 | killed |
| Stage13 | MBoss07/1 | 3500→0 | 755 | 386 | 0 | killed |
| Stage14 | Boss05/0 | 15000→0 | 180 | 1136 | 0 | killed |
| Stage25 | MBoss08/0 | 3500→0 | 660 | 382 | 192 | killed |
| Stage25 | MBoss07/1 | -1→-1 | 840 | 0 | 3600 | target_not_spawned_after_approach |
| Stage26 | MBoss08/0 | 3500→0 | 840 | 444 | 0 | killed |
| Stage26 | MBoss09/1 | 4000→0 | 840 | 444 | 0 | killed |
| Stage27 | MBoss09/0 | 4000→0 | 735 | 446 | 265 | killed |
| Stage27 | MBoss09/1 | 4000→0 | 638 | 510 | 0 | killed |
| Stage28 | MBoss09/0 | 4000→0 | 494 | 508 | 136 | killed |
| Stage28 | MBoss08/1 | 3500→0 | 514 | 509 | 0 | killed |
| Stage28 | MBoss08/2 | 3500→0 | 240 | 444 | 0 | killed |
| Stage28 | MBoss09/3 | 4000→0 | 630 | 509 | 0 | killed |
| Stage28 | MBoss10/4 | 4000→0 | 760 | 449 | 0 | killed |
| Stage29 | Boss03/0 | 20000→824 | 0 | 2386 | 0 | player_died |
| Stage15 | MBoss0501/0 | 8000→0 | 700 | 859 | 0 | killed |
| Stage16 | E05MB02/0 | 9000→0 | 660 | 1303 | 0 | killed |
| Stage17 | E05MB03/0 | 10000→5184 | 0 | 563 | 0 | player_died |
| Stage17 | E05MB04/1 | 11000→4198 | 0 | 899 | 0 | player_died |
| Stage18 | E05MB05/0 | 12000→3612 | 0 | 980 | 0 | player_died |
| Stage18 | E05MB02/1 | 9000→1236 | 0 | 1217 | 0 | player_died |
| Stage19 | Boss01/0 | 30000→17930 | 0 | 992 | 0 | player_died |
| Stage20 | E06MB01/0 | 10000→0 | 355 | 1504 | 0 | killed |
| Stage20 | MBoss0601/1 | 11000→1332 | 0 | 1089 | 0 | player_died |
| Stage21 | MBoss0701/0 | 12000→1128 | 0 | 1473 | 0 | player_died |
| Stage22 | MBoss0801/0 | 13000→0 | 105 | 1576 | 0 | killed |
| Stage23 | E06MB04/0 | 15000→9044 | 0 | 608 | 0 | player_died |
| Stage24 | Boss06/0 | 50000→38580 | 0 | 849 | 0 | player_died |
| Stage30 | E07MB01/0 | 15000→7268 | 0 | 847 | 0 | player_died |
| Stage31 | MBoss08/0 | 3500→0 | 285 | 511 | 0 | killed |
| Stage32 | E07MB02/0 | 15000→7708 | 0 | 838 | 0 | player_died |
| Stage34 | E07MB03/0 | 15000→11216 | 0 | 330 | 0 | player_died |
| Stage35 | Boss07/0 | 60000→53808 | 0 | 367 | 0 | player_died |

## 재현·근거

`cmake --build /tmp/hm-validation-build --target HeavyMachValidationScenarioTests HeavyMachValidationBossTests -j4`

`/tmp/hm-validation-build/HeavyMachValidationScenarioTests`는18강화·레벨/EXP/능력치 저장 복원·등급상한3건·무기20종 실제 사격 및 저장 후 재사격·맵41개·안정 보스 시작점48개를 통과했다. 합성 지면 없음 조건에서 fixture 실패가 기존 슬롯을 바꾸지 않는 검사도 통과했다. 로그는 `build/boss-v5-scenario-regression.log`다.

`/tmp/hm-validation-build/HeavyMachValidationBossTests output.jsonl`은 보스 배치를 실제 전투로 실행하고 하나라도 실패하면 exit1을 반환한다. 세 번째 인자에 `Stage29:0,Stage17:0`처럼 맵:보스index 목록을 주면 해당 배치만 실행한다.

원본 결과는 `build/boss-battles-v1.jsonl`에 보존한다. 후속 결과는 `build/boss-battles-v2.jsonl`부터 `build/boss-battles-v5.jsonl` 및 같은 이름 `.log`다. 지면·경계는 `build/boss-fixture-original.csv`, `build/boss-fixture-runtime.csv`, 원본 무기 수치는 `build/boss-original-weapon-fields.txt`다. build 산출물은 Git 제외 경로다.

## 남은14개 사망 원인 계측

14개만 같은 입력으로 재실행하여 사망 tick·목표의 마지막 HP·판정이 기존 v5와 모두 같음을 확인했다. 전투 중 무기별 실제 탄약 감소, 선택 중 발생한 목표 HP 감소, 플레이어 피해 tick을 추가 계측했다. 이 검사는 0/14 성공으로 exit1을 유지한다.

- 14개 모두 목표에 실제 피해를 줬다. 사망 시 유한 탄약이712~753 남았다. 전체 무기 탄약을 다 써서 실패한 사례는 없다. 원본 기본 총의 무제한 탄약은 합계에서 제외한다. 실제 아이템 획득으로 재고가 늘 수 있으므로 남은 수량을 초기 수량−소모량으로 대체하지 않았다.
- Stage17 첫 보스, Stage34, Stage35는 각각9.4·5.5·6.1초 만에 사망했다. Stage34의 최대 한 tick HP 감소는275다. 회피·접근을 포함한 입력 전략의 생존 한계이며, 이 값으로 원본 피해 계산 결함을 단정하지 않는다.
- Stage29, Stage18 두 번째 보스, Stage20 두 번째 보스, Stage21은 목표 HP가 각각824/20000,1236/9000,1332/11000,1128/12000 남았다. 처치 직전의 생존·사격 지속 실패로 분류한다.
- Stage24와 Stage35는 HP50000·60000 중38580·53808이 남았다. 현재 무기 순서와 회피 입력으로 처리한 피해가 부족했다. 적 HP를 줄이거나 무적을 추가하지 않았다.

`damageWhileSelected`는 해당 무기를 선택한 동안 실제 목표 HP가 감소한 양이다. 이전 무기의 비행 중 탄환도 맞을 수 있으므로 무기별 독립 피해량이라고 해석하지 않는다. `maxHPDrop`도 한 tick 누적 감소량이며 단일 탄환 피해라고 부르지 않는다. 접근 전투의 별도 통계는 후속 순차 runner에 추가했다.

| 맵/보스 index | 유효 피해/초기 HP | 생존 초 | 실제 탄약 소모 | 남은 유한 탄약 | 최대 tick HP 감소 |
|---|---:|---:|---:|---:|---:|
| Stage29/0 | 19176/20000 | 39.8 | 60 | 719 | 80 |
| Stage17/0 | 4816/10000 | 9.4 | 10 | 749 | 100 |
| Stage17/1 | 6802/11000 | 15.0 | 41 | 718 | 120 |
| Stage18/0 | 8388/12000 | 16.3 | 42 | 717 | 150 |
| Stage18/1 | 7764/9000 | 20.3 | 45 | 714 | 100 |
| Stage19/0 | 12070/30000 | 16.5 | 42 | 717 | 105 |
| Stage20/1 | 9668/11000 | 18.1 | 43 | 716 | 80 |
| Stage21/0 | 10872/12000 | 24.6 | 47 | 712 | 80 |
| Stage23/0 | 5956/15000 | 10.1 | 15 | 744 | 200 |
| Stage24/0 | 11420/50000 | 14.2 | 40 | 719 | 80 |
| Stage30/0 | 7732/15000 | 14.1 | 40 | 719 | 200 |
| Stage32/0 | 7292/15000 | 14.0 | 39 | 720 | 200 |
| Stage34/0 | 3784/15000 | 5.5 | 6 | 753 | 275 |
| Stage35/0 | 6192/60000 | 6.1 | 6 | 753 | 150 |

원본별 무기 소모·선택 중 피해는 `build/boss-v5-failure-diagnostics.jsonl`, 실행 결과는 같은 이름 `.log`다.

## Stage25 순차 전투 후속 성공

두 번째 보스 위치로 바로 이동하는 대신 첫 보스 MBoss08 앞의 안정 지면x411에서 시작했다. 같은 GameSession을 유지하고 오른쪽 이동·부스터·실제 무기 선택·사격·레벨업 확인을 실행했다. 추가 위치 주입, 적 삭제, 경계 변경, HP 재설정은 하지 않았다.

- 이전 보스 MBoss08: 원본 spawnX1136, 객체ID6, HP0·removed=true를 관측했다. 원본 KillBoss 경로가 진행 경계를 열었다.
- 목표 MBoss07: 원본 HP3500→0·제거 상태 확인. 최종 플레이어x7000.5/HP238,3571tick(약59.5초). 첫 시작점부터 목표까지6810 거리였다.
- 실제 유한 탄약 소모는 Laser10, ElShock27이다. 이동 중 기본 총을 선택한 시간도2864tick이며 원본 기본 총 탄약은 감소하지 않았다. 접근 중 사격·피해 통계를 별도로 기록했다.
- 단일 v5 결과는33/48로 유지한다. 이 별도 순차 성공을 합친 확인 범위는34/48이며, 남은14개는 위 사망 사례다. 전체48개를 한 실행에서 통과한 결과가 아니다.

`/tmp/hm-validation-build/HeavyMachValidationBossTests output.jsonl Stage25:1 sequential`로 재현한다. `sequential`은 이 배치에서 첫 보스부터 시작하는 검증 입력 전략이며 원본 게임 로직을 교체하지 않는다. 결과는 `build/boss-stage25-sequential.jsonl`과 `.log`다. 명시적 이전 보스 사망 계측을 넣은 재실행도 동일한3571tick/HP238/목표 사망으로 exit0을 반환했다.
