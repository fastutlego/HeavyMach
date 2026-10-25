# Heavy Mach 1.13 · Assault 1.0.3 분석

2026-10-24. 사용자가 지정한 두 IPA를 **morluto/rea의 rea-agents 6.7.0과 Ghidra 12.1.4로 정적 분석**했다. 이미지·음향·게임 데이터는 REA 추출 결과에 Python 해시·픽셀·구조 비교를 추가했다. 앱 실행과 모든 기능의 동등성 검증은 수행하지 않았다.

## 결론

두 판본은 대부분의 자원과 주요 게임 코드 구조를 공유한다. Assault에는 Life 구매가 추가됐다. 수집한 파일에서는 후반 스테이지 적 배치, Boss08 부품, 터치 처리, Game Center·광고 코드에도 차이가 확인됐다. 따라서 “모든 에셋·기능이 동일하고 결제만 추가됐다”는 설명은 이 두 패키지의 전체 차이를 설명하지 않는다. 출시 순서·원작 종료 이유는 이번 바이너리 분석으로 확정하지 않았다.

## 분석 대상

| 항목 | Heavy Mach | HM:Assault |
|---|---|---|
| 실제 앱 버전 | 1.13 | 1.0.3 |
| Bundle ID | kr.connect.touch.HeavyMach.game | com.wemadeent.HeavyMachAssault |
| 최소 OS 선언 | iOS 2.1 | iOS 4.3 |
| 실행 파일 | ARMv6, 32비트 | ARMv7, 32비트 |
| 앱 내부 일반 파일 | 776 | 807 |
| Ghidra 함수 목록 항목 | 1,133 | 1,802 |

Assault 아카이브 이름의 `Clutch-2.0.4`는 앱 버전이 아니다. 앱 버전은 Info.plist에서 읽었다. 함수 수는 import·thunk를 포함한 도구의 목록 항목 수이며 기능 수가 아니다. 양쪽 Mach-O의 `cryptid`는 0이다.

입력 출처: [Heavy Mach 1.13](https://archive.org/details/Heavy_Mach_1.13_ios_2.1), [Heavy Mach Assault](https://archive.org/details/com.wemadeent.heavymachassault-ios4.3-clutch-2.0.4).

원본·증거 루트는 `/Users/sokol/Documents/Codex/2026-10-24/rea-ready`다. 아래 경로는 이 루트를 기준으로 한다.

| 입력 | SHA-256 |
|---|---|
| Heavy_Mach_1.13_ios_2.1.ipa | `477c672f92110c6020be3f744dfa7a3456739e6f970c66715e3b7b3cd32b7776` |
| HeavyMachAssault.ipa | `71368f0167061e629deb2db2d7a53b7338e03239740caa2e5771a9ac77fb00a8` |
| 원작 HeavyMach 실행 파일 | `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb` |
| Assault HeavyMach 실행 파일 | `37827b677f739c48a55b6d4c5362f61fccbe92bf713727e9f0526e4eff779762` |

다운로드 당시 두 IPA의 SHA-1도 Internet Archive 메타데이터와 대조했다. 분석 종료 시 위 네 SHA-256이 유지됐다. 근거: `comparison/final-preservation.json`.

## 에셋과 게임 데이터

- 같은 상대 경로의 파일 744개가 바이트까지 같다. 15개는 변경됐고, 원작에만 17개, Assault에만 48개가 있다.
- 이미지 695→720개 중 674개가 같은 경로·바이트다. 같은 이름의 이미지 4개는 픽셀 또는 크기가 변경됐다.
- Assault 전용 이미지 경로 42개 중 15개는 원작 JPG에 대응하는 PNG다. 크기는 같고 채널 평균 차이는 0~255 척도에서 0.115~0.546이다. 변환본으로 추정하지만 픽셀 동일은 아니다.
- WAV 31개는 모두 바이트까지 같다.
- `WeaponInfo.mch` 160개 레코드와 `PartsInfo.mch` 294개 레코드는 파일 전체가 같다.
- 맵 43개 중 36개는 같다. `Stage01`, `Stage36~41`이 변경됐다. Stage36~41의 적 배치 레코드는 합계 0→487개다.
- `MachInfo.mch`는 각 720개 레코드다. NUL 종료 이름 뒤의 패딩을 무시하면 719개가 같고, Boss08에는 부품 7개가 추가됐다. 패딩 무시는 비교를 위한 해석이며 원시 바이트도 보존했다.

맵·MCH 총 92개를 파일 끝까지 재귀 파싱했다. 잔여 바이트는 0개다. Assault의 REA `readChunk:Pos:`와 `readEnemyList:Data:Size:Cnt:`를 대조해 type10·11이 적 목록·적 레코드에 연결됨을 확인했다. 원작 reader의 switch 복원이 불완전하므로 원작 타입 의미는 동일 파일 구조에 근거한 대응이다. 파일에 있는 스테이지의 실제 해금·선택 가능성은 확인하지 않았다.

상세 근거: [리소스 보고서](/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/assets/FINDINGS.ko.md), [전체 파일 비교](/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/assets/resource-comparison.json), [원시 데이터와 오프셋](/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/assets/parsed-game-data.json).

## 코드와 기능

Objective-C 메서드 이름을 맞췄을 때 511개 이름이 대응한다. 원작은 이름이 있는 Objective-C 메서드 557개, Assault는 내부 namespace 항목 902개를 비교했다. 두 목록의 탐지 방식이 다르므로 나머지 이름 수를 기능 추가·삭제 수로 해석하지 않는다.

대표 공통 함수 19쌍을 선택해 이동·비행, 무기 발사·교체·업그레이드, 피격, 탄환, 스테이지 전환, 생존 모드, 기울기 입력, 레벨 경험치, 충돌, 저장을 읽었다. 다음은 확인한 범위의 결론이다.

| 범위 | 코드에서 확인한 내용 | 근거 파일 (`comparison/core/`) |
|---|---|---|
| 무기 발사 | 양쪽 모두 재장전 가능 여부 확인→발사 시간 갱신→포구 갱신→발사 횟수·지연 설정 흐름. 플레이어의 비기본 무기는 잔량 표시를 줄인다. | original/assault-function-08.json |
| 저장 | 양쪽 모두 Documents/HvM.dat에 3,624바이트를 원자적으로 쓴다. 저장 호환성은 미확인이다. | original/assault-function-00.json |
| 터치 | Assault는 TapID 인수를 받아 사격 터치를 `_shootTapID`로 구분하고 해제 시 초기화한다. 원작은 인수가 두 개다. | original/assault-function-19.json |
| 스테이지 전환·점수 | Assault의 setNextStage에 reportScore 호출이 있다. GameCenterManager의 점수 보고는 GKScore와 완료 콜백을 사용한다. | assault-function-13.json, -21.json, -22.json |
| 광고·다른 게임 | 배너의 로드·표시 검사와 프레임 애니메이션, 다른 게임 버튼·타일 생성 코드가 있다. 서비스 동작은 미확인이다. | assault-function-23.json, -24.json |
| 경험치 | 의사 코드의 차이를 어셈블리로 재확인했다. 양쪽에서 초기 배수 1.5, 1.25와의 비교, 조건부 0.01 감소와 300×pow 항을 확인했다. 이 항목을 난이도 변경으로 판정하지 않았다. | original/assault-level-assembly.json |

원작 발사 Evidence: `ev_47881b28d7c619f4223326f2606d3a9cdb6b2a0be8a24cf35a448d1318744730`. Assault 발사 Evidence: `ev_c7257682edb1c65523ca98cd76121248ea679c977c2d15a79002df1a6c402485`.

전체 주소·함수·Evidence ID 대응은 [코드 증거 색인](/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/core/evidence-index.json)에 있다. `*-function-index.json`은 함수 이름과 주소를 제공한다. `.c` 파일은 읽기용 디컴파일 사본이며 원본 소스가 아니다.

Assault `GobBullet::setFire:angle:x:y:`는 Ghidra의 `Overlapping input varnodes` 오류로 의사 코드를 얻지 못했다. 실패 결과와 `assault-function-10-assembly.json`을 함께 보존했다. 해당 함수의 완전한 동등성은 판단하지 않았다. `checkMachCollide` 메서드의 존재와 본문은 확인했지만 실제 호출 경로를 확정하지 않아 새 충돌 동작으로 결론 내리지 않았다.

## Assault 구매·Life

코드상 흐름은 **Life 구매 버튼→확인→StoreKit 결제 큐→구매 완료 콜백→Life 20 추가→저장**이다. 계속하기 함수는 Life 1을 쓰고 기체를 회복·재배치한다.

- 상품 ID는 `wemade.havymach.Assault1`이다. `havymach`는 파일에 있는 철자다.
- 구매 버튼은 현재 Life가 6 미만일 때 확인 창을 연다. 문구의 5는 총 구매 횟수 제한이 아니다.
- `Life 20 for $0.99`는 고정 UI 문구다. 실제 당시·현재 판매 가격을 검증한 것이 아니다.
- 구매 성공은 `val=1`, 실패는 `-1`, 복원 콜백은 `2`다. tick의 `val=1`만 Life 20을 추가한다. 복원 UI 진입 경로와 실행 결과는 미확인이다.
- 원작의 전체 함수·정의 문자열 목록에서 지정 StoreKit/IAP 패턴 일치는 0건이었다. 지정 패턴 범위의 결과다.

구매 지급 Evidence: `ev_90f41c243fbe4a80c07ecf4c7ac72dc9174ed2a4838fe7cb4b7940773c9ed78d`.
상세 근거와 미확인 경로: [구매·Life 보고서](/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/iap/IAP-REPORT.md), [판단별 Evidence](/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/iap/IAP-FINDINGS.json).

## 도구·완료 범위·남은 일

설치 패키지의 repository는 `git+https://github.com/morluto/rea.git`, 버전은 6.7.0이다. [설치 출처 기록](/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/tool-provenance.json)에 실제 package.json 경로를 남겼다. REA MCP의 `open_binary`, `list_procedures`, `procedure_pseudo_code`, `procedure_assembly` 등을 실행하고 Ghidra Evidence를 보관했다. 현재 대화에 직접 노출된 REA 도구 대신 등록 설정을 읽는 별도 MCP 클라이언트와 CLI를 사용했다. 열린 분석 세션은 닫았다.

| 질문 | 상태 |
|---|---|
| 공식 REA 설치 및 실제 앱 분석 가능 여부 | 검증 완료 |
| 두 IPA 식별·추출·전체 파일 비교 | 완료 |
| 맵·MCH 구조와 주요 차이 | 파일 파싱 완료, 일부 타입 의미·사용 조건 미확인 |
| 주요 게임 로직의 공통 구조·차이 | 대표 함수 정적 비교 완료, 전체 동등성 미검증 |
| Assault 구매→Life 지급·소비 | 주요 정적 경로 확인, 실제 구매·복원 미검증 |
| 최신 iOS 시뮬레이터·실기기 이식 및 실행 | 다음 과제, 미착수 |

**현재 두 실행 파일은 32비트다.** Apple은 iOS 11부터 32비트 앱이 실행되지 않는다고 명시한다. 따라서 이 IPA의 재서명이나 최소 OS 선언 변경만으로 최신 iOS 실행을 해결할 수 없다. 다음 작업은 소스 확보 여부와 64비트 재빌드·재구현 또는 호환 실행 경로의 가능성을 먼저 검증해야 한다. 최신 iOS 구동 성공을 약속한 상태가 아니다. [Apple 32비트 지원 안내](https://developer.apple.com/news/upcoming-requirements/?id=06062022a)

다음 작업의 범위·검증 기준은 [핸드오버](handoff-20261024-rea-install.md)의 최상단을 따른다. 이번에는 게임 제품 코드, IPA, 사용자 저장 파일을 수정하지 않았다. 커밋·배포·앱 실행·실제 결제를 하지 않았다.
