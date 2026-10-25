# 무기 소진과 시간제 효과의 수명

2026-10-24. REA 원본 호출 순서를 대조해 같은 프레임의 무기 재획득과 스테이지·메뉴 전환의 시간제 효과 소실을 수정했다. 실기기는 사용하지 않았다.

## 마지막 탄약과 같은 프레임의 아이템 획득

원본 무기 tick0x12800..1282a는 마지막 burst에서 탄약을 소진하면 `GobHvM_Player::removeCurWeapon` 0x1c408을 호출한다. 원본은 버린 무기 파티클 생성→재고 삭제→기본 무기 선택·생성→효과음5 순서로 처리한다. 아이템 tick보다 먼저 실행한다.

기존 GameSession은 삭제를 동적 객체 처리 뒤로 미뤘다. RaderBomb 탄약1을 쏜 프레임에 같은 무기 상자를 주우면, 탄약0인 기존 슬롯에 새 탄약5가 더해진 뒤 프레임 끝에서 슬롯 전체가 삭제됐다. 새 탄약을 잃는 실제 실행 차이다. 버린 무기 파티클과 효과음5도 빠져 있었다.

수정 후 기존 `tickWeapon`이 반환한 직후 재고 삭제와 기본 무기 교체를 끝낸다. 현재 실행 중인 객체를 해제하지 않도록 이 반환 경계까지만 처리를 미룬다. 원본 플레이어 weapon tail0x129c6..129f8은 이 사이에 AI·RNG·super tick을 실행하지 않는다. 새 무기도 같은 프레임에 tick한다. 원본 QobBase0x7830은 childArray 길이를 매번 읽어 새로 추가된 무기도 방문한다. 기본 무기 초기화의 RNG1회는 아이템 추첨보다 먼저 발생한다.

원본 discard 파티클은 해당 무기 PNG/frame0, blend0, life2, scale1, player+(0,40), velocity(−50,150), gravity250, rotationVelocity15다. 렌더에는 새 무기만 남는다. `refreshMyMachLayer` 0x1be0c가 전용 부품 목록을 갱신하기 때문이다.

`AmmoSyncTests`는 실제 GameSession.tick에서 마지막 burst와 착지한 상자 획득을 함께 실행한다. 수정 전 신규6조건 실패를 `build/weapon-exhaustion/red.log`에 보존했다. 수정 후 재획득 RaderBomb이 마지막 슬롯에 탄약5로 남고, 다른 무기 순서·기본 무기 선택·저장 encode/decode·효과음1회·파티클 속성이 통과했다. 상수 RNG fixture이므로 소비 순서는 별도의 원본/소스 대조 근거이며 이 테스트가 가변 RNG 순서를 증명한다고 주장하지 않는다.

## 스테이지·메뉴 사이의 강화 효과

원본 Attack·Defense·Speed·Booster 슬롯은 플레이어가 아닌 GuiGame 소유다. GameMain0x2ecc가 한 번 생성하고, 메뉴 전환0x3778은 숨기기만 한다. GuiGame.tick0x161d0은 show·world.pause와 무관하게 processPassiveSlot0x176b8을 호출한다. next-stage clear/openMap과 setGameStart0x169b0에는 초기화가 없다. 확인한 passiveSlot 참조에서 초기화는 GuiGame 생성, 추가는0x1778c, 감소·만료는0x176b8이다.

기존 코드는 플레이어를 교체할 때 네 효과도 버렸고, 플레이어가 없는 전환 프레임에는 시간을 줄이지 않았다. 수정 후 앱 GameView가 효과 상태를 소유하고 GameSession과 플레이어가 참조한다. 스테이지·메뉴·슬롯 재진입에서도 남은 시간을 이어받는다. 메뉴와 타이틀에서도 tick당 원본 float 상수0x3c888889만큼 감소한다. 저장 파일에 임시 효과를 새로 기록하지 않는다. 새 앱 소유자는 빈 상태로 시작한다.

기본 Core 소비자는 GameSession 자체가 효과 상태를 소유한다. 외부 상태를 전달할 때는 소유자가 세션보다 오래 살아야 한다. 세션 이동은 heap Impl을 그대로 이전한다. 플레이어 객체의 잘못된 값 복사로 내부 참조가 남지 않도록 복사·이동은 금지했다.

별도 검증 앱의 **초기 조건 재설정**은 일반 재진입과 구분한다. `startValidation`만 효과를 비운다. 이전 검증 조건의 효과가 다음 치트 fixture로 새어 들어가는 실패를 `fixture-reset-red.log`에 재현한 뒤 수정했다.

## 실행 근거와 한계

- `PassiveContinuityTests` 수정 전 실제 next-stage 전환에서, 플레이어 없는 tick의 갱신 이벤트는 예상4개/실제0개였다. 새 플레이어의 네 효과는 예상 잔여0.96초/실제0초였다. `build/passive-continuity/red-*`에 명령·로그·바이너리·해시를 보존했다.
- 수정 후 실제 Motion 출구→결과 버튼→clear→openMap, 두 플레이어 없는 tick, 서로 다른 슬롯·세션 파괴/이동·메뉴 만료·새 소유자·저장 바이트 불변을 검사했다.
- 최종 CTest17/17과 관련 ASan/UBSan3/3이 통과했다. `build/session-lifecycle/final-core-tests.log`, `sanitizer-tests.log`.
- 실제 Air 시뮬레이터 iOS 검사1개가 통과했다. 실제 HMGameView.frame, 메뉴·타이틀 라우팅, 세션 생성, 씬 활성 상태 연결에서 시간 감소·재진입·만료를 확인했다. 테스트용 뷰는 저장 쓰기와 음향/이벤트 출력을 차단했다. UI를 사람이 누른 검사나 실제 백그라운드 전환 관측은 아니다. `passive-ios-final.log/.xcresult`.
- 일반 iphoneos ARM64 서명 제외 빌드가 통과했다. `iphoneos-build.log`. 실제 기기 설치·실행은 하지 않았다.

이번 변경은 전투·RNG에 영향을 주므로 이전35맵 결과를 최신 통과로 간주하지 않는다. 같은 입력 전략의 후속 전체 결과와 제한은 [맵 여정](stage-journeys.md)에 기록한다. 모든 미션·연속 캠페인·원본 전체 화면 동등성은 별도 검증 항목이다.

## 35맵 후속 결과

최종 제품 소스로 같은 selected-35·카메라·HUD 조건을 다시 실행했다.23클리어·12사망·시간초과0, exit1이다. 이전35행 JSON 결과와 CSV trace 전체가 동일했다. 두 경계 결함의 수정 효과는 별도 RED/GREEN 검사로 확인했고, 이 전투 여정은 기존 진행 결과의 회귀 검사로 구분한다. `build/stage-journeys/session-lifecycle/manifest.json` 및 `build/session-lifecycle/manifest.json`에 명령·소스·실행 바이너리·결과 해시를 기록했다.
