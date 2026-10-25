# nil BaseParts 런타임 관측

2026-10-24. `Tools/observe_nil_parts.py`는 Python 표준 라이브러리만 사용하는 touchHLE GDB remote protocol(RSP) client다. GNU GDB 설치를 요구하지 않는다. 별도 관측 앱에서 원본 ARM guest를 실행해 두 차례 관측했다. 원본 IPA와 복원 앱 Sources는 변경하지 않았다.

## 확인한 현재 wrapper 경로

기준 source는 `iOS Proj/HeavyMachRecovery/compat-source/fresh-port/20-touchHLE-src/touchHLE`이다.

1. `src/lib.rs` ios_entry는 SDL base_path의 `HeavyMach_options.txt`를 읽는다. 빈 줄과 # 주석을 제외한 **각 줄 전체**를 한 개의 CLI 인자로 전달한다. 환경 변수나 Documents의 같은 이름 파일을 읽는 경로가 아니다.
2. `src/options.rs`의 `--gdb=주소:포트`가 gdb_listen_addrs에 저장된다.
3. `src/environment.rs`는 guest 로딩·late linking 뒤 TcpListener를 만들고 accept에서 대기한다. 최초 guest 실행 전에 연결할 수 있다. 기준 config에는 이 옵션이 없다. 별도 관측 복사본에만 추가했다.
4. `src/gdb.rs`는 `?`, `g`, `p`, `m`, `M`, `c`, `qC`를 지원한다. M은 guest 메모리를 쓰고 CPU instruction cache를 무효화한다. Z breakpoint 요청은 지원하지 않는다. s는 thread 선택을 요구하므로 이 도구는 single-step을 사용하지 않는다.
5. `src/objc/messages.rs`의 stret wrapper는 반환 버퍼를 쓰지 않는다. nil dispatcher는 r0/r1만0으로 바꾼다. 실제 반환 버퍼 값은 guest stack을 관측해야 한다.

## 재현 절차

아래 절차로 Air/iOS26.5 시뮬레이터에서 실행했다. UI 조작은 CUA의 Device Hub(`com.apple.dt.Devices`)를 사용했다.

1. 비교 앱의 **별도 관측용 복사본**에 기존 orientation/device-family 옵션을 유지하고 `HeavyMach_options.txt`에 `--gdb=127.0.0.1:24681` 한 줄을 추가한다. 원본 IPA와 native 복원 앱은 바꾸지 않는다. 이미 서명된 실제 기기의 bundle 파일을 직접 바꾸는 절차로 해석하지 않는다. 부모의 기존 패키징/서명·설치 절차를 사용한다. simulator의 localhost 연결을 우선한다. 실제 기기에는 별도 접근 가능한 주소/전달 경로가 필요하며 이번에 검증하지 않았다.
2. 관측용 비교 앱을 시작해 `Waiting for debugger connection` 로그를 확인한다. 도구 자체는 앱을 시작하지 않는다.
3. 별도 터미널에서 실행한다.

```sh
cd /Users/sokol/Git/ProjectGameDev/HeavyMach
python3 Tools/observe_nil_parts.py --run --host 127.0.0.1 --port 24681 --calls 3 --output /tmp/heavymach-nil-parts-observation.jsonl
```

4. 연결 후 앱을 정상 조작해 Stage01에 진입한다. 도구는 nil BaseParts 호출3개를 기록하고 모든 자신 소유 trap을 복구한 뒤 guest를 continue한다. 그 뒤 socket을 닫는다. 기존 파일에는 덮어쓰지 않는다. 다른 observer/debugger를 동시에 연결하지 않는다.
5. JSONL의 각 sample에 rot_before/rot_after/bounds_before/bounds_after4개가 있는지 확인한다. `buffer.hex`와 u32가 원시 증거다. float 해석은 보조 값이다. 기록이 생겼다는 이유만으로 원본 Apple 런타임의 값이라고 부르지 않는다.

`--run`이 없으면 host-side 검사만 실행한다. 포트가 열려 있거나 앱이 실행 중이어도 연결하지 않는다.

## 런타임 변경과 복구

**소프트웨어 breakpoint는 실행 중 guest 코드 변경이다.** 도구는 불변 IPA 파일을 수정하지 않지만, guest 메모리에 Thumb BKPT #0(`00 be`)를 잠시 쓴다. 원본 주소의 명령 바이트를 먼저 대조한다. 하나라도 맞지 않으면 trap을 설치하지 않는다.

| 주소 | phase | 검증하는 원본 바이트 |
|---|---|---|
| 0x42dc | rot_before | 1c f0 b6 ea |
| 0x42e0 | rot_after | b1 49 |
| 0x42ee | bounds_before | 1c f0 ae ea |
| 0x42f2 | bounds_after | 32 1c |

`--final`을 지정하면 0x477a(`10 98`)의 `root_accumulated`를 추가한다. 이 위치는 root part의 경계 누적 직후이며 자식 part 전체 생성 완료가 아니다.

근거는 `build/original-assembly.txt`와 `Analysis/functions/0x428c.asm`이다. 실행 시 한 주소에만 trap을 두고, 멈추면 즉시 원래2바이트를 되돌린다. 다음 주소에 trap을 설치하고 continue하므로 원본 getter 호출을 건너뛰거나 single-step하지 않는다. 처음과 마지막 주소 사이의 SP/thread가 바뀌면 실패로 보고한다. non-nil 호출은 같은 순서로 통과하되 기록하지 않는다.

현재 interpreter의 debug_cpu_error 경로는 Thumb trap에서 PC를2바이트 되감는다. 도구는 report된 PC가 정확히 해당 주소이고 CPSR Thumb bit가 설정돼 있는지 검사한다. 임의로 PC를 보정하지 않는다.

원본 바이트 검증을 통과한 뒤 정상 완료 및 멈춘 상태의 예외에서는 현재 설치한 trap을 원복하고 continue한다. 실행 중 연결 손실·프로세스 강제 종료·Ctrl-C는 복구를 보장하지 않는다. 이 경우 도구가 주소 복구를 완료했다고 주장하지 않으며, 비교 앱을 종료·재시작한 뒤 재사용해야 한다. 원본 파일에서 다시 로드되므로 남은 trap을 다음 조사 증거로 섞지 않는다. 이 source의 서버는 최초 연결 이후 재accept하는 흐름이 아니므로 자동 reconnect를 시도하지 않는다.

## 기록 형식과 한계

각 줄은 sample index, phase, pc, thread, sp, r10, self, r0..r15, 반환 버퍼24바이트, 현재 self+52의 bounds16바이트를 담는다. r10==0인 호출만 기록한다. 반환 버퍼는 sp+0x50 CGRect16바이트 뒤 sp+0x60 CGPoint8바이트다. float NaN/Infinity는 문자열로 기록하고 u32/hex는 payload와 signed zero까지 보존한다.

machine_bounds_hex는 **그 phase 시점** 값이다. 0x42f2는 함수 종료가 아니므로 최종 누적 bounds라고 부르면 안 된다. 기체 이름은 이 지점의 인자에 보존되어 있지 않아 도구가 추측하지 않는다. self와 생성 순서로 표본을 식별한다. `--final`은 root 누적값만 추가하며 이름과 자식 포함 최종값은 별도 관측이 필요하다.

## 실행 결과

- 관측 앱: `/tmp/heavymach-nil-observation/HeavyMachObserve.app`, bundle `com.gelb2.heavymach.observation`. IPA SHA256은 원본 `477c672f…7776`과 같다. `build/nil-parts-observation-app.json`에 패키징 근거를 보존했다.
- 첫 연결은 RSP 시작 `+` 인사를 보내지 않아 ConnectionResetError로 실패했다. 서버 `GdbServer::new`의 인사 요구에 맞춰 수정했다. 실패 파일은 그대로 보존했다. 서버가 다시 연결받지 않으므로 관측 앱만 종료·재실행했다.
- 수정 후 첫 실행: `build/nil-parts-runtime-second.jsonl`, nil 호출3개×4단계=12행. getter 전후 반환 버퍼24바이트는 각 표본에서 동일했다. 버퍼에는 0 이외 값이 남았다.
- `--final` 실행: `build/nil-parts-runtime-root.jsonl`, nil 호출3개×5단계=15행. 실제 root 누적 bounds는 세 표본 모두 `(-20,0,20,40)`이었다. 원본 계산식의 binary32 추적 및 현재 복원 코드의 zero 대체 결과와 이 표본에서 일치했다.
- 두 실행 모두 Stage01을 정상 UI로 시작해 오른쪽 이동으로 적을 생성했다. 저장·HP·탄약을 주입하지 않았다. 도구가 모든 소유 trap을 원복하고 마지막 continue를 보낸 뒤 종료한 로그를 확보했다.
- 실행·관측 파일 해시는 `build/nil-parts-runtime-manifest.json`, 수치 분석은 [실행값 분석](nil-parts-runtime-analysis.md)에 있다.

이 결과는 touchHLE에서 실행한 원본 ARM 코드의 제한된 표본이다. nil이 zero geometry를 반환한다는 뜻이 아니다. 당시 Apple ARM32 OS, 720종 기체 전체, 이름 대응 및 자식 포함 최종 bounds 일치는 미검증이다. 현재 표본에는 Core 변경을 정당화할 차이가 없어 기존 대체 규칙을 유지한다.

## host-side 검사

`python3 Tools/observe_nil_parts.py`:8개 검사 통과(exit0).

- RSP 시작 인사 성공/거부, checksum 생성/검증과1바이트씩 분할된 응답 수신
- ARM32 little-endian word 및 CGRect+CGPoint 해석
- 잘못된 길이와 remote memory error 거부
- NaN payload/signed zero를 보존하는 JSON 직렬화
- 모의 remote에서 non-nil 호출 제외, nil4단계 기록, 모든 원본 명령 복구, 마지막 continue

모의 remote는 실제 앱 연결 성공을 대신하지 않는다.
