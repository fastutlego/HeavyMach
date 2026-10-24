# HP 막대의 좌표와 표시 등록

2026-10-24. 원본 REA 함수와 복원 소스를 대조해 적 HP 막대의 좌표·화면 판정을 수정했다. 플레이어와 적 모두 tick에서 각 막대의 표시 여부를 등록하고 draw에서 현재 위치·HP 비율을 읽도록 연결했다.

## 원본 근거와 차이

- `GobHvM_Enemy::makeMachFromName:` 0x14918은 배경 frame1을 적에게, 채움 frame0을 배경에게 붙인다. 두 이미지 모두 fixedPos1이다. HP500 이하는 `HPGaugeEnemy.png` 32×6의 두 행, HP501 이상은 `HPGaugeBoss.png` 100×12의 두 행이다. gaugeY는40/80이고 비행형은−10이다. layer2, blend0을 유지한다.
- `setHPGauge:` 0x14778은 채움 X를 `−(size−fraction×size)`, scaleX를 fraction으로 설정한다. scale 변경0x8234는 culling bounds를 줄이지 않는다.
- `QobBase::tick` 0x7830과 `isInCam` 0x7c78은 부모 본체의 화면 판정과 별개로 각 자식 이미지를 등록한다. 배경이 화면 밖이어도 채움 자식은 검사한다. fixedPos 좌표0x7a28/0x7b00은 quake를 뺀 뒤 `int(value+0.5f)`로 만든다. 음수에 floor/round를 대신 적용하지 않는다.
- 원본 draw0x8cf0은 등록된 이미지의 현재 좌표·scale을 읽는다. tick 이후 탄환이 HP를 바꿨다고 표시 여부를 다시 판정하지 않는다. 이미 등록된 막대는 같은 프레임에 적이 죽어도 남고 다음 tick에서 제거된다.

수정 전 `GameSession::machineDraw`는 적 본체가 화면 밖이면 HP 막대도 생략했다. 적 막대는 fixedPos 변환과 독립 culling 없이 소수 좌표로 그렸다. 플레이어 막대는 독립 culling을 했지만 draw 수집 때 다시 판정했다.

수정 후 `Machine::gaugeQueued`에 두 이미지의 tick 판정을 보존한다. `TickChildren`에서 등록하고, 다음 tick 시작 때 이전 등록을 비운다. draw에서는 현재 좌표를 계산하며 호스트의 전체 quake 이동을 상쇄하는 기존 좌표 계약을 사용한다. 원본 posY0x79d4처럼 배경의 world Y를 먼저 계산한 뒤 채움의 local Y=1을 더한다. HP 계산·적 피격·타깃 선택·사망 시점·저장 규칙은 수정하지 않았다. 등록 전에 이미 사망한 적은 자식 등록을 건너뛰며, 등록 뒤 사망한 적의 기존 큐는 유지한다.

## 실행한 검사

증거 디렉터리는 `build/gauge-registration/`다. 변경 전 소스와 테스트 사본도 보존했다.

- 적 회귀 최초68조건 중22개가 수정 전에 실패했다(`enemy-red.log`). 플레이어 회귀는 tick 뒤 HP 변경에서 실패했다(`player-red.log`, exit134).
- 수정 후 일반 적·보스·비행형의 full/half/zero, quake, 음수 경계, 독립 자식 culling, tick/draw 사이 HP 변경, 사망 프레임과 다음 tick 제거가 통과했다.
- 최초 수정 후 CTest16/16이 통과했다(`core-tests.log`). 실제 tick의 숨김·재표시 회귀를 추가했다. 독립 검토에서 찾은 등록 전 추락사망·재귀 좌표 합산 순서도 수정 전 실패를 확보했다(`review-red.log`). 최종 적73조건과 플레이어 테스트를 포함한 전체 CTest16/16이 통과했다(`review-green.log`). 최종 두 테스트의 ASan/UBSan도 통과했다(`sanitizer-review-green.log`).
- 추가 회귀 첫 실행은 이동 중인 camera follow 때문에 마지막 재표시 조건이 실패했다. 카메라를 원래 clamp된 추적 목표에 먼저 놓도록 fixture를 고쳤다. 이 실패는 `final-gauge-tests.log`에 보존했다. 이때 제품 코드는 변경하지 않았다.
- 일반 scheme의 iphoneos ARM64 및 iOS Simulator 빌드가 각각 성공했다(`iphoneos-final-build.log`, `simulator-final-build.log`). 서명은 제외했다. 설치·실기기 실행 검사가 아니다.

## 렌더 비교

현재 Core에서 Stage01/28/35의 tick180 draw 목록을 새로 만들고, 동일 목록을 실제 CGL과 CPU/CoreGraphics 양쪽에서 합성했다. 앱과 같은 `-ffp-contract=off -fno-fast-math`를 사용했다. 최종 결과는 `scenes-final/comparison.jsonl`이다. 독립 검토의 두 수정 전후15개 RGBA/isolated/prefix 산출물은 byte 단위로 동일했다. `manifest.json`에 소스·바이너리·증거 해시와 실행 범위를 남겼다. 원본 파생 이미지695개의 해시도 기존 목록과 모두 일치했다.

| 장면 | Sprite / Trail | 최대 RGB 차이 | 차이1 초과 채널 | RNG 호출 |
| --- | --- | --- | --- | --- |
| Stage01 | 41 / 0 | 2 | 11 | 416 |
| Stage28 | 30 / 7 | 2 | 41 | 31423 |
| Stage35 | 72 / 4 | 2 | 81 | 8691 |

이전 장면은 최대 차이2/2/3이었다. Stage35의 화면 밖 막대 두 개가 새 목록에서 제외됐다. 세 표본의 RNG 호출 수는 이전과 같다. **픽셀 일치는 아직 실패다.** 같은 native draw 목록의 두 renderer 비교이며 원본 게임의 실제 draw 목록·전체 화면과 일치한다는 증거가 아니다. 이전156 draw 표본과 새154 draw 표본을 섞지 않는다.

물리 iPhone은 사용하지 않았다. Mac 잠금으로 이번 수정의 UI 조작 검사는 하지 않았다. 이전 35맵 여정23클리어·12사망은 이 표시 수정 전 증거이며, 이번에 전체 여정을 다시 실행한 것으로 보고하지 않는다.
