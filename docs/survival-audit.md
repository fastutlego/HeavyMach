# Survival 원본·소비자 독립 대조

2026-10-24. 기존 REA export, World/GameSession/HMMenus/HUD/Motion/Enemy와 WorldTests를 읽었다. 최초 대조는 읽기 전용으로 진행했다. 이후 승인된 clock 수정과 host 검사를 수행했다. UI·설치·기기 조작은 하지 않았다. 스폰 알고리즘과 모드 진입의 구체적 누락은 찾지 못했다. **새 GameSession 생성 때 원본 World의 누적 시간이0으로 바뀌는 차이**를 발견해 수정했다. survival 전체 스폰 분기의 회귀 검사는 아직 남아 있다.

## 원본과 일치하는 경로

| 동작 | 원본 근거 | 현재 소비자 |
|---|---|---|
| 모드 선택 | GuiSelectMode handleButton0x1a900: mode1 선택; 시작 시 mission4/stage4, changeScreen6 | HMMenus mode 버튼1002, 시작1000 → _mission/_stage4 → screen6 → startSelectedGame |
| 게임 진입 | GameMain setScreen_Game0x3a1c: world/UI 표시, mode1이면 setSurvival, 마지막 pause0 | HMGameView startSlot → GameSession.start(1,4,4), 새 World paused=false |
| seed | setSurvival0xb840: max(100,400−internalLevel×10) | configureSurvival(slot.level()); 정상 레벨 범위에서 일치 |
| 자산 | setSurvival0xb840: Survival.map | GameSession.open이 mode1에서 Survival을 직접 선택 |
| 플레이어 위치 | openMap0xe3f4: right−480, getMapHeight+10, no-ground이면180 | World.openMap 동일 |
| tick | GobWorld0xb658: show 체크; pause 아님·gameState0일 때 object regen 다음 survival | World.tick → processRegen → processSurvival → camera, Session child ticks |
| 스폰 게이트 | processSurvivalMode0xd06a: random%seed==0; bossEncounter가 있으면 seed×5 | World.processSurvival 동일 |
| 일반 적 | category random%4 → ArmoredCar001..315/Tank001..200/Truck001..010/Heli001..010 | 이름 범위·RNG 호출 순서 일치; spawnEnemy callback은 동기 |
| 스폰 위치 | x=right+30, y=height+1 또는50; Heli는 y+200 | 동일, 실제 World.mapHeight와 collisionCandidates 사용 |
| 추가 적 | 기본 적 생성 후 random%80==0이면 MBoss01..10; 아니면 새 random%120==0이면 Boss01..06; 아니면 새 random%20==0이면 Heli001 | else-if 구조와 추가 난수 순서 동일 |
| 난이도 변화 | seed>50인 경우에만 마지막 random%10==0이면 seed−1 | 동일. 게이트 실패 tick에는 감소 없음 |
| 보스 경계 | addEnemyMach0xd6b8, killBoss0xd848 | bossEncounter/frontLine 적용 및 최종 보스 사망 시 해제 |
| EXP | Enemy onDestroy0x14418의 mode1 분기 | Enemy::onDestroy에서 giveExp/2, 실제 Player.addExperience로 전달 |
| 종료·표시 | setGameOver0xbf74: ResultTypo frame3; processStageClear0xc228에서 survival→screen3 | Session GameOver mode1 frame3, World 결과 종료 screen3 → 슬롯 |
| 우측 경계 | GobHvMach tick0x479c의 mode1에서는 다음 stage를 시작하지 않음 | Motion.tick mode1에서 전진 속도0, NextStage 이벤트 없음 |
| 미션 글자 | GuiGame setGameStart0x169b0은 mode0에서만 미션 글자 설정 | HUD drawMissionIntro mode!=0 return |

MachInfo.mch 원시 문자열도 읽어 위 생성 범위551개 이름(315+200+10+10+10+6)이 모두 존재함을 확인했다. 이것은 각 정의가 전투에서 끝까지 정상 동작함을 증명하지 않는다. 원본과 같은 스폰 이름을 임의의 다른 적으로 대체하는 누락은 보이지 않았다.

## 구체적 차이: 누적 World 시간

원본 World는 GameMain과 함께 한 번 생성된다. gameTime=0 대입은 init0xb0a4에 있다. clearAllScreen0x3778은 객체를 지우고 World를 숨기며 시간을 초기화하지 않는다. setSurvival0xb840, setMission:Stage:0xb780, openMap0xe3f4도 누적 gameTime을 유지한다. openMap은 오히려 이 시간을 timeItemRegen에 복사한다. 숨겨진 World는 tick0xb658에서 즉시 return하므로 메뉴 동안은 멈춘다.

수정 전 GameSession::Impl::start는 `world=WorldState{}`로 gameTime을0으로 만들었다. 수정 전 HMGameView는 이전 세션 시간을 HMMenus preview에만 전달하고 다음 GameSession으로 전달하지 않았다. **이전 플레이→메뉴→survival** 및 다시 시작 경로에서 원본 시간 기준이 달라진다. 첫 앱 실행에서 바로 survival로 들어가는 경우에는 이 차이가 나타나지 않는다.

새 무기의 resetTime 등 상대 타이머도 새 기준으로 만들어지므로 이를 즉시 체감되는 AI 발사 오류라고 단정하지 않는다. 그러나 double 누적 시계와 float timestamp의 반올림 경계, world-time 기반 trail/AI 상태를 원본과 동등하다고 볼 수 없다. 후속 수정은 이전 World.gameTime을 새 세션 초기화에 전달한다. processSurvival의 확률 자체는 시간을 사용하지 않는다.

## 현재 검사 범위와 추가 설계

최초 조사 당시 WorldTests의 survival 검사는 configureSurvival(0)→400, configureSurvival(50)→100 두 assertion뿐이다. 최초 전체 Tests 검색에서 실제 mode1 GameSession 경로를 실행하는 검사를 찾지 못했다. 후속 GameSessionTests에 mode1 clock 경로를 추가했다. 다른 모드의 보스 전투 성공은 survival 통합 검사를 대신하지 않는다.

다음 검사는 원본 자산·수치를 바꾸지 않고 실제 실패를 잡을 수 있다.

1. **분기와 호출 순서:** World+기록 Host와 미리 정한 RNG 시퀀스로 spawn gate 실패1회, 네 일반 적 범위 양끝, MBoss/Boss/추가Heli 분기, bossEncounter×5, seed51→50 및50에서 감소 RNG 생략을 검사한다. Host.spawnEnemy가 생성 중 RNG를 소비하는 경우도 넣어 이후 boss gate가 소비하는 값의 위치를 확인한다. 생성 callback을 일괄 지연하면 실패해야 한다.
2. **실제 mode1 연결:** 실제 RawData와 원본 tile 크기로 GameSession.start(1,4,4). Survival.map 경계·right−480 spawn·level별 seed, 실제 enemy 생성·sprites·움직임·피격을 확인한다. 고정 seed로 충분히 긴 tick을 진행하되 생존/처치를 임의로 강제하지 않는다. 요청된 적이 없어 throw하면 실제 연결 결함으로 기록한다.
3. **pause와 종료:** pause 동안 spawn RNG와 gameTime이 멈추는지, mode1 적 처치 EXP가 절반인지, 실제 HP0에서 frame3→결과 종료→ChangeScreen3가 전달되는지 확인한다. 기존 slot 진행 mission/stage를 survival이 nextStage로 갱신하지 않는지도 검사한다.
4. **누적 시간 회귀:** scenario에서 시간을 진행하고 메뉴를 거쳐 survival을 시작했을 때 원본 World 시간을 이어받는지 확인한다. 현행 fresh-session0 기준은 이 기대를 만족하지 못한다. host 계약 수정 시 이 검사를 함께 넣는다.

시간 차이를 부모에게 보고한 뒤 승인 범위에서 아래와 같이 수정했다. 원본/native survival 화면 직접 비교는 부모 작업이며 여기의 정적 대조나 제안 검사를 실제 UI 통과로 표시하지 않는다.

## 누적 시간 수정과 실패 재현

GameSession.start(mode,mission,stage)는 현재 World의 시간을 유지한다. 새 세션 객체로 교체하는 native host는 추가 인자 retainedWorldTime을 전달한다. HMGameView는 화면 전환에서 session을 파괴하기 직전에 시간을 저장하고 다음 start에 전달한다. 기존 preview 시간/pause 전달도 유지했다. 메뉴 동안 world tick이나 wall-clock 경과를 더하지 않는다. 새 앱 프로세스의 최초값은0이며 새 게임은 원본 setScreen_Game처럼 pause=false로 시작한다. 명시적인 검증 fixture 초기화는 기존처럼 시간0을 사용한다.

관련 소비자는 GobMachWeapon.init0x11d70의 float(world.time) resetTime, 무기 AI/reload 시간 비교, world-time 기반 trail 기록/소멸이다. 새 객체들의 상대 시각을 함께 옮겨도 큰 누적 시간에서 float 변환 경계가 달라질 수 있으므로 원본 시계의 유지 자체를 복원했다. 이 수정이 실제 오디오·영상에서 특정 차이를 없앴다는 비교 결과는 아직 없다.

실제 RawData Session에서 scenario12tick→pause tick→survival start의 시간 보존 assertion을 먼저 추가했다. 수정 전 `/tmp/hm-survival-clock-before.log`에서 해당 assertion 실패, exit134를 확인했다. 수정 후 `/tmp/hm-survival-clock-after.log` Debug 통과, `/tmp/hm-survival-clock-asan.log` ASan/UBSan 통과. 새 session으로 retainedTime 전달, 메뉴가 지난 뒤 globalTime500에서 world에는 한 tick만 증가, cold session0도 검사한다. 정상·검증 HMGameView simulator 문법 검사(HM_VALIDATION0/1)는 exit0이다. 전체 survival 확률 분기/장기 생존 검사는 위 설계대로 별도 필요하다.

## SurvivalTests 구현·실행

후속 승인으로 `Tests/SurvivalTests.cpp`를 추가했다. 원본 RawData를 읽으며 원본 적 정의·HP·피해·지형·저장 자산을 변경하지 않는다.

- World 분기 검사는 gate 실패1회 소비, bossEncounter의 seed×5, 네 일반 적 이름 상한, MBoss10/Boss06/추가Heli001, spawn X/Y, seed51→50 및50에서 감소 RNG 생략을 확인한다. 이름의 하한은 Tank001/Truck001/ArmoredCar001 분기에서도 확인한다.
- 기록 Host의 동기 spawn callback이 RNG 두 개를 소비하도록 했다. 일반 적 생성 직후41/42, 추가 보스 생성 직후43/44가 소비되고 나머지 gate/name/seed RNG 위치가 맞는지 검사한다. 이 검사는 실제 weapon 구성에 필요한 RNG 개수를2로 가정하는 것이 아니라 callback과 다음 결정의 호출 순서를 검사한다.
- World pause와 show=false에서 gameTime 및 survival RNG가 진행하지 않는지 확인한다. 실제 GameSession pause도 같은 조건을 검사한다.
- 원본 ArmoredCar001 정의로 Enemy 객체를 만들고 HP와 같은 피해를 onDamage에 전달해 파괴 이벤트의 mode1 EXP가 원본 giveExp/2=1임을 확인한다. 이는 단위 피해 callback 검사이며 플레이어의 실제 총탄 처치 성공으로 보고하지 않는다.
- 별도 실제 GameSession.start(1,4,4)는 원본 Survival.map/right−480/seed400을 사용한다. 원본 grade0 플레이어를 정지시키고 seed17 LCG 테스트 난수로 실제 적 생성·이동·공격·충돌·피격을 진행한다. 실제 적 공격으로 tick1791에 HP0, gameState3/paused/frame3를 관측했다. 계속 정상 tick하여 ChangeScreen3를 확인했다. 슬롯 mission/stage/farthest 값은 변하지 않았다. 무적·피해/HP 변경·강제 승패·적 위치 변경은 없다.

Debug 독립 실행 `/tmp/heavymach-survival-tests` exit0, 결과 `/tmp/heavymach-survival-tests.log`. 동일 최종 테스트의 ASan/UBSan 실행 `/tmp/heavymach-survival-asan`도 exit0이며 `/tmp/heavymach-survival-asan.log`에 기록했다. 부모가 연결한 CMake target도 재구성 후 빌드·실행 exit0을 확인했다. 이 테스트는 survival 진입·기본 스폰·사망 복귀 계약을 확인하며 장기 생존 성공이나 원본 RNG 전체 stream 동등성을 주장하지 않는다.
