# nil BaseParts와 ARM32 구조체 반환 경계

2026-10-24. 원본 REA assembly·메타데이터·추출 데이터를 읽기 전용으로 대조했다. 앱 실행, 실기기 조작, Sources 변경은 하지 않았다. 원본 실행 파일 SHA-256은 `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`이다.

**결론:** nil BaseParts는 실제 적 생성의 정상 경로다. 원본 caller는 구조체 반환 버퍼를 초기화하지 않는다. 확인한 Apple 공개 ARM32 런타임은 nil 수신자의 반환 버퍼를 쓰지 않는다. 따라서 native의 `nil → zero geometry`는 결정적인 대체 규칙이며, 원본이 항상0을 반환한다는 복원 근거는 없다. 당시 Apple OS의 값은 미관측이다. 후속 실행에서 root 경계3개가 복원 결과와 일치했다. 나머지 기체 경계는 비교가 남아 있다.

## 실제 진입과 데이터

- `GobHvM_Enemy::makeMachFromName:` 0x14918은 `decodeChunk:Data:Pos:`에 BaseParts=0, Pos=0을 전달한다.
- `decodeChunk` 0x147e8은 type3 기체 chunk에서 자식에게 넘길 base를0으로 둔다(0x14840–0x14842). type4의 PartsInfo가 있으면 현재 base를 `addParts:BaseParts:BaseSocket:`으로 전달한다(0x14888–0x14894). 생성된 part는 후속 자식들의 base가 된다.
- `Resources/Data/MachInfo.json` 전체720개 기체는 각각 최상위 part1개를 갖는다. 모든 part 이름을 `PartsInfo.json`과 대조한 결과 누락0, 존재하는 부품에 대한 nil base 진입720개다. 이는 데이터 목록의 잠재 호출 수이며 실제 한 플레이의 실행 횟수가 아니다.
- 지도 JSON 전체의 enemy 항목1182개는155종 기체 이름을 참조한다. Stage01의 첫 항목들은 아래와 같다.

| 지도 기체 | MachInfo machine offset | nil base root part | part offset |
|---|---:|---|---:|
| ArmoredCar015 | 62981 | E_ArmoCar_01 | 63030 |
| ArmoredCar114 | 80306 | E_ArmoCar_02 | 80355 |
| ArmoredCar016 | 63156 | E_ArmoCar_02 | 63205 |

위 offset은 원본 MachInfo.mch 기준이다. 최초 enemy가 카메라에 들어오는지와 무관하게 기체 생성 시 이 경계에 진입한다.

## 원본 caller의 버퍼

`Analysis/functions/0x428c.asm`:

| 명령 위치 | 의미 |
|---|---|
| 0x4298 | stack frame0x128바이트 확보 |
| 0x429c | BaseParts 인자 r3를 r10에 보존 |
| 0x42ac–0x42b0 | nil이면 setChildParts 호출만 건너뛰고0x42be로 이동 |
| 0x42c6,0x42d8,0x42dc | r1=nil receiver, r0=sp+0x60, rotPos를 objc_msgSend_stret로 호출 |
| 0x42e8,0x42ee | r1=nil receiver, r0=sp+0x50, boundRect를 objc_msgSend_stret로 호출 |
| 0x42ea,0x42f4–0x42f8 | 반환용 stack 영역의 값을 읽어 bounds 계산에 사용 |

sp+0x60의 CGPoint8바이트와 sp+0x50의 CGRect16바이트는 첫 해당 호출 전에 초기화되지 않는다. nil 분기를 지난 뒤에도 getter들을 조건 없이 반복 호출한다. 일반 메시지의 r0/r1 반환을0으로 만드는 동작과 구조체 반환용 메모리 초기화는 별개다.

메타데이터 `Sources/OriginalInterfaces/method-address-map.json`은 rotPos0x11d34의 `{CGPoint=ff}`와 boundRect0x81c4의 `{CGRect={CGPoint=ff}{CGSize=ff}}` 반환형을 보존한다. 현재 ARM64 UIKit typedef 크기로 원본 ARM32 호출 ABI를 추정하지 않았다.

## 런타임 근거와 제한

Apple 공개 [objc4-437.1 ARM messenger](https://github.com/apple-oss-distributions/objc4/blob/objc4-437.1/runtime/Messengers.subproj/objc-msg-arm.s)의 `objc_msgSend_stret` 진입은 receiver(a2)를0과 비교한 뒤 nil이면 lr로 복귀한다. a1의 반환 버퍼를 쓰는 명령은 없다. [493.9](https://github.com/apple-oss-distributions/objc4/blob/objc4-493.9/runtime/Messengers.subproj/objc-msg-arm.s)와 [532.2](https://github.com/apple-oss-distributions/objc4/blob/objc4-532.2/runtime/Messengers.subproj/objc-msg-arm.s)도 같은 반환 버퍼 보존 동작을 확인했다.

이 소스들은 ARM32 nil 구조체 반환에0을 가정할 수 없다는 직접 근거다. IPA에 함께 배포된 런타임 바이너리나 사용자의 당시 OS 버전으로 확인한 것은 아니다. 그러므로 원본 장치의 실제6개 float 값까지 이 공개 코드로 확정하지 않는다.

현재 비교용 touchHLE의 `src/objc/messages.rs`도 objc_msgSend_stret가 공통 dispatcher를 호출하고 nil에서는 CPU r0/r1만0으로 만들며 반환 버퍼 메모리는 쓰지 않는다. 따라서 비교용 호환 실행에서 보이는 값도 guest stack 내용의 관측이 필요하다. 이 구현을 원본 Apple 런타임과 완전히 같다고 간주하지 않는다.

## native의 영향 범위

`GameSession::addPart`는 base가 없으면 Vector2{}와 ObjectBounds{}를 `accumulateMachBounds`로 전달한다. 결과는 최소 bounds(-20,0,20,40)이다. 원본 식의 clamp는 임의 stack 값을 모두 없애지 않는다. 예를 들어 rotPos.x=0, boundRect.left=-100이면 왼쪽 경계가-50으로 남을 수 있고, 뒤에 자식 부품을 누적해도 이전의 더 넓은 경계가 보존될 수 있다. 이것은 영향 설명용 입력이며 원본에서 관측한 값이 아니다. NaN도 원본 비교식의 별도 경계다.

영향은 적 bounds와 그에 따른 카메라 포함/충돌 판정이다. 일반 플레이어 부품은 `setPartsToMyMach` 0x1be84가 단일인자 addParts0x4214를 사용하고 끝에 bounds(-20,0,20,50)를 덮어쓰므로 이 nil bounds 누적 경로와 구분한다.

`getSocketPos:` 0x11818은 자기 `_baseParts`가 nil일 때 시작 좌표를 명시적으로0으로 둔다. 이 메서드의 정상 nil-base 처리와 nil 수신자에게 구조체 getter를 보내는0x428c를 혼동하지 않는다. `setBaseParts:Socket:` 0x11798의 마지막 getter도 구조체 반환이지만 확인한 caller0x11748은 존재하는 부모를 전달한다. 이번 데이터에서는 누락 부품으로 부모가 끊긴 추가 경로는 발견하지 못했다.

## 남은 관측과 변경 판단

호출 전후 반환 버퍼와 root 누적값은 후속 실행에서 관측했다. 다음에는 자식 포함 기체 bounds를 기체 이름/생성 순서에 묶어야 한다. 최소한 첫 Stage01 생성과 같은 기체의 후속 생성, 다른 직전 호출 경로를 구분해야 한다. 호환 실행의 값은 호환 실행 증거로만 표시하고, 정확한 Apple 런타임을 가진 ARM32 환경의 값과 구분한다.

이번에는 소스 변경을 제안하지 않는다. native C++에 미초기화 읽기를 도입하면 원본 런타임을 재현하지 못한 채 새 미정의 동작을 만든다. 특정 관측값을 모든 기체에 하드코딩하는 것도 근거가 부족하다. 현행 zero 대체는 명시된 미확정 경계로 유지하며, 실제 관측 없이는 정확한 원본 bounds라고 주장하지 않는다.

## 현재 iOS Proj wrapper의 관측 수단

문서 작성 후 실제 `iOS Proj/HeavyMachRecovery/compat-source/fresh-port/20-touchHLE-src/touchHLE/src/objc/messages.rs`를 별도로 확인했다. objc_msgSend_stret(1341행)은 `_stret` 인자를 쓰지 않고 공통 dispatcher로 간다. nil 분기393–397행은 CPU r0/r1만0으로 만든 뒤 반환한다. 반환 버퍼를 지우는 코드는 없다. 앞서 확인한 Documents 복사본만을 근거로 현재 wrapper를 추정하지 않았다.

현재 source의 options.rs282행에는 `--gdb=주소`가 있고 gdb.rs는 레지스터·메모리 읽기 및 소프트웨어 breakpoint 경로를 제공한다. 따라서 새 trace 빌드 없이 기존 debugger 활성화로 관측할 가능성이 있다. 현재 옵션 파일에는 landscape/device-family만 있고 debugger는 활성화되지 않았다. 이번에는 옵션 변경·연결·재시작을 하지 않았으므로 현 산출물에서 debugger 작동을 검증한 것은 아니다.

후속 관측 지점은 0x42dc와0x42e0,0x42ee와0x42f2(호출 전후),0x477a(root part bounds 기록 직후; 자식 포함 최종값은 아님)다. r10==0인 호출만 수집하고 sp+0x30의 self 및 self+52의 CGRect, 반환 버퍼24바이트를 함께 기록하면 guest의 실제 bounds를 좁힐 수 있다. debugger가 software trap을 guest 메모리에 심는 과정은 원본 파일 수정과 다르지만 실행 상태를 바꾸므로 단순 파일 읽기 작업으로 취급하지 않는다. 새 trace 빌드도 수행하지 않았다.

## 후속 실제 관측

CUA에서 Xcode27의 Device Hub(`com.apple.dt.Devices`)를 선택해 Air 시뮬레이터 창을 제어했다. 별도 관측 앱의 Stage01에서 getter 전후3개 호출을 읽었고, 재실행 후 추가3개 호출의 root 누적값도 읽었다. 버퍼는 getter 전후 변하지 않았고 0 이외 값이 남았다. 두 번째 실행의 실제 root bounds는 세 표본 모두 `(-20,0,20,40)`으로 현행 복원 결과와 일치했다.

[관측 절차와 로그](nil-parts-observation.md), [수치 분석](nil-parts-runtime-analysis.md)을 따른다. RSP 시작 인사를 수정했고 host 검사8개를 통과했다. 각 실행은 원본 명령 복구와 continue 후 종료했다. 이 표본에서 Core 수정 근거는 없었다. 기체 이름 대응, 자식 포함 최종값, 모든 경로 및 당시 Apple ARM32 OS는 남은 범위다. 앞 절의 미실행 서술은 준비 단계 이력이다.
