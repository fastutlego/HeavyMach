# REA 설치와 Internet Archive 접속 인계

기록일: 2026-10-24, Asia/Seoul.

## 최신 인계 — 두 판본 분석과 다음 iOS 과제

2026-10-24 사용자 지정: **Heavy Mach 1.13과 Assault 둘 다 분석**. 최종 후속 지시: “분석 다되면 핸드오버 작성해. 다음 과제는 heavy mach을 최신 ios, 시뮬레이터에서 구동 가능하게 수정 + 실제 최신 ios 탑재된 폰에서 구동 가능하게 할 수 있는지 검증하기”.

이 절이 아래 설치·준비 당시 기록보다 최신이다. 아래의 “분석 미실행”, “버전 답변 없음”, “다음 작업에서 버전 선택”은 과거 기록이다.

### 이번에 완료한 범위

- **morluto/rea의 rea-agents 6.7.0 + Ghidra 12.1.4**로 원작 1.13과 Assault **1.0.3**을 실제 정적 분석했다. Clutch 2.0.4는 앱 버전이 아니다.
- [종합 분석 보고서](heavy-mach-113-assault-analysis-20261024.md)에 대상 SHA, 비교 결과, 함수 주소·Evidence, 실패·미확인 범위를 정리했다.
- 이미지 674개와 WAV 31개는 바이트까지 같다. WeaponInfo·PartsInfo도 같다. 맵 7개, Boss08 부품, 터치 처리, Game Center·광고 코드에는 차이가 있다.
- Assault의 StoreKit 구매→Life 20 추가→저장, 계속하기의 Life 1 소비를 코드로 연결했다. 실제 구매·복원은 실행하지 않았다.
- 두 IPA와 실행 파일의 최종 SHA-256이 최초 확보 값과 같았다. 근거: `/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/final-preservation.json`.
- REA 스킬·엔진 설치와 별도 MCP 통신, 패키지 추출·함수 디컴파일은 검증했다. 현재 대화에 직접 REA 도구가 노출됐다는 뜻은 아니다. CLI/별도 MCP로 다음 분석을 계속할 수 있다.
- 게임 전체 기능 동등성, 최신 iOS 빌드, 시뮬레이터·폰 실행은 미검증이다. 이번 단계는 확보한 파일의 정적 비교 결과다.

### 다음 과제: 최신 iOS 시뮬레이터·실기기 구동 검증

목표는 Heavy Mach를 최신 iOS 시뮬레이터와 최신 iOS가 설치된 실제 iPhone에서 구동할 수 있게 수정하고 가능성을 실제 실행으로 판단하는 것이다. 지금은 다음 과제를 인계했으며 이식 코드를 작성하거나 앱을 실행하지 않았다. 1.13과 Assault의 증거를 모두 재사용한다. 사용자가 둘 중 하나를 폐기하라고 지시하지 않았다.

**확인된 제약:** 원작은 ARMv6, Assault는 ARMv7이며 둘 다 32비트 실행 파일만 있다. iOS 11 이후는 32비트 앱을 실행하지 않는다. 재서명·Info.plist 변경만으로 해결할 수 없다. [Apple 공식 안내](https://developer.apple.com/news/upcoming-requirements/?id=06062022a)

**아직 모르는 사항:** 원본 빌드 가능한 소스의 확보 여부, 최신 Xcode/SDK·시뮬레이터 설치 상태, 사용할 실제 iPhone의 모델·iOS 버전·연결·서명 상태, 전체 포팅 비용과 실행 가능성. 디컴파일 `.c`를 바로 빌드 가능한 원본 소스로 간주하지 않는다. 최신 버전 번호는 다음 작업 시 Apple 공식 자료와 실제 설치 환경에서 확인한다.

다음 실행 순서:

1. 기존 분석·원본을 재사용하고 Xcode, SDK, 시뮬레이터 런타임, 연결 기기를 읽기 전용으로 확인한다. 실제 iOS 버전과 도구 버전을 기록한다. 기존 로그인·서명 설정이 있으면 확인하며 인증 정보를 문서에 복사하지 않는다.
2. 해당 게임 소스가 있는지 확인한다. 소스가 있으면 64비트·현재 SDK로 빌드 가능한지 조사한다. 소스가 없으면 복원·재구현 또는 호환 실행 경로의 기술 타당성을 최소 구동 시제품으로 검증한다. 바이너리 몇 바이트 변경만으로 64비트 변환이 된다고 가정하지 않는다.
3. 별도 작업 디렉터리에서 원본 에셋·데이터를 읽는 실행 경로를 만든다. 원본 IPA·추출본·Evidence와 현재 Unity 게임 작업을 보존한다. 현재 ProjectGameDev 게임이 원본 Heavy Mach 소스라고 가정하지 않는다.
4. 렌더링, 오디오, 터치·기울기, 맵 로딩, 게임 루프, 저장을 각각 현재 플랫폼에 연결한다. OpenGLES/OpenAL, 과거 UIKit API, libstdc++/libgcc 및 Assault의 iAd·GameKit·StoreKit 의존성을 실제 SDK와 대조한다. 결제·광고·외부 서비스의 재현 여부는 별도 항목으로 보고한다.
5. **시뮬레이터용 빌드**와 **실기기용 빌드·서명**을 구분해 만든다. Apple의 시뮬레이터 실행 절차는 [Xcode 안내](https://help.apple.com/xcode/mac/current/en.lproj/devfd9e28378.html)를 참고한다. 에뮬레이션·호환층을 썼다면 이를 네이티브 포팅과 구분한다.
6. 두 환경에서 설치·콜드 실행, 타이틀→스테이지 진입, 이동·조준·발사·피격·충돌, 음향, 스테이지 완료, 저장·재실행, 일시정지·복귀를 확인한다. Assault 추가 맵과 Life는 분석 결과를 기준으로 확인한다. 실제 유료 구매 없이 가능한 로컬·테스트 환경을 사용한다.
7. 기기 모델·OS·빌드 식별자·실행 로그·화면 증거와 실패를 보관한다. 빌드 성공, 시뮬레이터 성공, 실제 폰 성공을 따로 판정한다. 실제 폰을 확보하지 못하면 실기기 검증은 미완료로 남긴다.

완료 기준은 “가능할 것 같다”는 설명이 아니라 실제 실행 증거다. 타이틀 화면만 뜬 경우 부분 성공으로 기록한다. 실행이 막히면 실패 조건과 재현 로그, 해결한 범위·남은 조건을 명시한다. App Store 배포는 이번 다음 과제에 포함됐다고 가정하지 않는다.

### 다음 담당자가 바로 읽을 자료

- 종합 결과: [Heavy Mach 두 판본 분석](heavy-mach-113-assault-analysis-20261024.md).
- 증거 루트: `/Users/sokol/Documents/Codex/2026-10-24/rea-ready/`.
- 원작 앱: `HeavyMach-1.13-extracted/Payload/HeavyMach.app/`.
- Assault 앱: `Assault-extracted/Payload/HeavyMach.app/`.
- 데이터 파서·전파일 비교: `comparison/assets/compare_resources.py`, `resource-comparison.json`, `parsed-game-data.json`, `FINDINGS.ko.md`.
- 함수 목록·스냅샷·대표 함수: `comparison/core/`; ID 색인은 `evidence-index.json`. Assault 탄환 함수 하나는 디컴파일 실패와 어셈블리 대체 증거를 함께 보존했다.
- 구매 경로: `comparison/iap/IAP-REPORT.md`, `IAP-FINDINGS.json`.
- 공식 도구 출처: `comparison/tool-provenance.json`. CLI 실행기는 `/Users/sokol/.local/share/rea/rea`.
- MCP 재현기는 각 분석 디렉터리의 `mcp_check.py`와 scenario JSON이다. 기존 출력 덮어쓰기를 피하려면 새 출력 디렉터리·이름을 사용한다. 설치를 반복하지 않는다.

프로젝트 HEAD `678266df1679931d45c9639d757d6751b08dffe3`와 기존 dirty 파일을 보존했다. 이번 저장소 문서는 이 인계와 종합 분석 보고서다. 커밋·push·배포·Unity 변경은 하지 않았다.

---

## 후속 완료 — 실제 앱 분석 준비

사용자 후속 지시: “rea로 앱 분석 가능하게 준비 다 하라고”. 아래 내용이 기존 설치 기록보다 최신이다. **Heavy Mach 1.13 IPA의 구조 확인·추출·네이티브 코드 디컴파일을 실제로 검증했다.** 게임 실행과 전체 기능 분석은 수행하지 않았다.

### 설치와 연결

- REA 6.7.0에 Ghidra 12.1.4와 Homebrew JDK 21.0.12.1을 연결했다.
- Ghidra 공식 배포 파일 SHA-256 `ddac49f903da9d5bac833e5cc79395098b9c33cfd3279be5f31bd00387d2d4db`를 확인한 뒤 `/Users/sokol/.local/share/rea/ghidra_12.1.4_PUBLIC`에 풀었다.
- 배포본에는 Mac ARM64 디컴파일러가 없어 포함된 소스로 `make -j6 ghidra_opt 'ARCH_TYPE=-arch arm64' OSDIR=mac_arm_64`를 실행했다. 결과를 Ghidra의 `Ghidra/Features/Decompiler/os/mac_arm_64/decompile`에 설치했다. REA doctor가 호스트와 일치하는 Mach-O arm64 실행 파일로 확인했다.
- JDK 설치 명령은 `HOMEBREW_NO_AUTO_UPDATE=1 brew install openjdk@21`이다. Homebrew가 관련 의존성도 설치·갱신했다. 기존 JBR 17과 시스템 Java 선택은 변경하지 않았다.
- 사용자 MCP 설정은 setup으로 백업 후 갱신했다. 최종 command는 `/Users/sokol/.nvm/versions/node/v22.19.0/bin/rea`, args는 `["mcp"]`다. 기존 30초 시작 제한·450초 도구 제한을 유지했다.
- REA 서버 환경에 `GHIDRA_INSTALL_DIR`과 `/opt/homebrew/Cellar/openjdk@21/21.0.12.1/libexec/openjdk.jdk/Contents/Home`의 `JAVA_HOME`을 등록했다. Node를 찾도록 REA 항목에만 PATH를 추가했다. GUI 실행 환경을 가정한 최소 PATH에서도 MCP 초기화와 도구 목록 조회를 통과했다.
- 추가 설정 백업: `/Users/sokol/.codex/config.toml.rea-ready-20261024-104231.backup`. setup의 추가 백업은 `config.toml.rea.backup.2d9a2b4e0ef2675c734b52c98eb90e82c4560d92c6303261c59014a8d93f9d41.recovery`다. 최초 백업과 비교해 REA 외 설정 값이 같음을 확인했다.
- 환경 변수를 포함한 CLI 실행기: `/Users/sokol/.local/share/rea/rea`. 이 대화에서도 사용 가능하다. 현재 에이전트의 REA 직접 도구 노출은 미확인이며, 검증은 CLI와 등록 설정을 읽은 별도 MCP 클라이언트로 수행했다. 재시작을 분석 작업의 선행 조건으로 요구하지 않는다.

### 실제 대상과 검증 근거

증거·원본 디렉터리: `/Users/sokol/Documents/Codex/2026-10-24/rea-ready/`.
최종 요약: [RESULT.json](/Users/sokol/Documents/Codex/2026-10-24/rea-ready/RESULT.json).

버전 선택을 비동기로 물었으나 답변이 없어서 원작 1.13을 준비 검증 대상으로 사용한다고 알린 뒤 진행했다. 사용자가 이후 다른 버전을 선택하면 그 선택을 우선한다.

- 원본: `Heavy_Mach_1.13_ios_2.1.ipa`, 30,266,903바이트. Internet Archive 메타데이터의 SHA-1과 다운로드 파일이 일치했다. SHA-256은 `477c672f92110c6020be3f744dfa7a3456739e6f970c66715e3b7b3cd32b7776`.
- REA `inspect-artifact`와 `extract-artifact` 성공. 779개 파일을 추출하고 지속 보관 디렉터리 `HeavyMach-1.13-extracted/`로 복사했다. REA가 기록한 각 파일의 SHA-256과 모두 일치했다.
- 앱 경로: `/Users/sokol/Documents/Codex/2026-10-24/rea-ready/HeavyMach-1.13-extracted/Payload/HeavyMach.app/HeavyMach`.
- 앱 식별자 `kr.connect.touch.HeavyMach.game`, 버전 `1.13`, 최소 iOS `2.1`, 실행 파일 ARMv6 Mach-O. 실행 파일 SHA-256 `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb`.
- `doctor-final.json`: Codex 등록·Ghidra·JDK·스킬 검사 `healthy: true`.
- `probe-decompile-fixed.json`: 직접 만든 ARM64 Mach-O의 분기와 산술 함수 디컴파일 성공. 소스 `probe.c`와 대조했다.
- `heavy-mach-overview.json`: 실제 앱에서 Ghidra가 함수 1,133개, 문자열 5,957개, 세그먼트 24개를 보고했다. 전체 함수 발견이나 전체 기능 이해를 뜻하지 않는다.
- `mcp-heavy-collision-decompile.json`: 등록된 MCP 서버에서 `0x5a88`의 `-[ColVector checkCollide:ColRadius:]` 디컴파일 성공. Evidence ID `ev_9ed3492ecc84a2f712f7304010d3a2dfe0e86f30a25a931a9a036c7aa53f47fa`. 읽기 쉬운 코드 사본은 `heavy-mach-collision-pseudocode.c`다. 디컴파일 결과는 원본 소스가 아니다.
- `mcp-heavy-open.json`, `mcp-heavy-session.json`, `mcp-heavy-close.json`: 실제 대상 열기·세션 조회·닫기 성공. 마지막 원본 IPA와 실행 파일 해시도 유지됐다.
- MCP 검증 재현 스크립트 `mcp_check.py`와 요청 목록 `mcp-heavy-scenario.json`을 보관했다. 재실행하면 동일 이름의 검증 출력이 덮어써지므로 기존 증거를 보존할 별도 디렉터리를 먼저 선택한다.

### 남은 한계와 바로 사용할 경로

- `heavy-mach-macho.json`의 실패를 보존했다. macOS `dyld_info`가 구형 ARMv6 파일에 `unknown platform`을 반환해 REA `inspect_macho`가 실패했다. 이후 Ghidra로 실제 코드 분석을 성공했다. 동일 실패 명령을 조건 변화 없이 반복하지 않는다.
- 최초 검증 함수 선택 `rea_probe`는 Mach-O 심볼 접두사 때문에 실패했다. `nm`으로 실제 이름 `_rea_probe`를 확인한 뒤 성공했다. 최초 실패는 `probe-decompile.json`에 보존했다.
- iOS 앱 실행·화면 조작·동적 분석과 전체 게임 기능 분석은 미실행이다. Android/JADX·펌웨어용 도구의 준비 완료를 주장하지 않는다. 현재 검증 범위는 이 iOS 앱의 패키지·리소스와 네이티브 정적 분석이다.
- 추가 분석 시 기존 원본과 Evidence를 재사용한다. 동일 설치·검증을 처음부터 반복할 필요가 없다. 현재 MCP 도구가 보이면 그 도구를 쓰고, 없으면 아래 실행기로 계속한다.

```sh
/Users/sokol/.local/share/rea/rea decompile \
  /Users/sokol/Documents/Codex/2026-10-24/rea-ready/HeavyMach-1.13-extracted/Payload/HeavyMach.app/HeavyMach \
  --procedure 0x5a88 --provider ghidra --json
```

공식 설치 근거: [REA Ghidra 설치 안내](https://github.com/morluto/rea/blob/main/docs/installation.md#ghidra), [Ghidra 12.1.4 배포](https://github.com/NationalSecurityAgency/ghidra/releases/tag/Ghidra_12.1.4_build).

## 아래는 본체 설치 직후 기록

후속 준비 전 상태를 보존한다. 엔진 미설치·다운로드 미실행 등 아래 과거 상태는 위 완료 기록으로 갱신됐다.

## 현재 상태

REA 6.7.0 CLI를 설치하고 Codex용 MCP 서버를 등록했다. 별도 검증 프로세스에서 서버 초기화, 도구 207개 조회, `binary_session` 정상 응답을 확인했다. **현재 대화에 REA 도구가 연결됐다는 증거는 없다.** Codex 재시작 후 실제 연결 확인이 남았다.

사용자는 REA 도입과 `https://archive.org/search?query=heavy+mach&tab=all` 접속을 요청했다. 처음에는 스킬만 설치했다. 사용자가 REA는 MCP 서버·CLI 도구라고 정정한 뒤 본체와 서버 등록까지 수행했다. Heavy Mach 파일 다운로드·분석·게임 변경은 요청받거나 실행하지 않았다.

## 설치한 항목

- 공식 npm 패키지: `rea-agents@6.7.0`. 저장소: <https://github.com/morluto/rea>.
- 설치 명령: `npm install -g rea-agents@6.7.0`.
- CLI 경로: `/Users/sokol/.nvm/versions/node/v22.19.0/bin/rea`.
- 확인한 실행 환경: Node `v22.19.0`, npm `10.9.3`.
- 등록 명령: `npx -y rea-agents@6.7.0 setup --client codex --skill-client codex --yes --json`에 출력 필터를 붙여 실행했다. 적용 전 같은 범위의 `--dry-run` 계획을 확인했다.
- 수정한 사용자 설정: `/Users/sokol/.codex/config.toml`. 프로젝트의 `.codex/config.toml`과 구분한다.
- 설정 백업: `/Users/sokol/.codex/config.toml.rea.backup`.

등록된 REA 설정:

```toml
[mcp_servers.rea]
command = "npx"
args = ["-y", "rea-agents@6.7.0", "mcp"]
startup_timeout_sec = 30
tool_timeout_sec = 450
```

스킬은 설치된 본체에 맞는 번들 판본 42로 맞췄다. 패키지 버전 6.7.0과 스킬 판본 42는 서로 다른 번호다.

- 공식 setup 설치 위치: `/Users/sokol/.agents/skills/reverse-engineer-anything/`.
- 앞서 설치한 Codex 전용 위치도 같은 번들로 갱신: `/Users/sokol/.codex/skills/reverse-engineer-anything/`.
- 앞서 받은 저장소 main 스킬 판본 46의 백업: `/Users/sokol/.codex/backups/rea-skill-20261024-103557/`.
- 두 위치의 `SKILL.md` 바이트 일치를 확인했다. 현재 스킬 목록에는 같은 이름이 두 번 표시된다. 이번에는 중복 경로를 삭제하지 않았다.

## 실제 검증과 한계

아래 결과는 이번 대화의 도구 실행에서 확인했다. 별도 원본 로그 파일로 저장하지 않았으므로 이 문서가 결과 요약이다.

1. `rea --version`: `6.7.0`. 인계 작성 시 다시 확인했다.
2. setup 결과: `status: ready`, `appliedActions: [configured_codex, installed_skill]`, 해당 범위의 `doctor.healthy: true`.
3. `rea doctor --client codex --json`: Node와 Codex 등록 검사 통과. 스킬 판본 42, 도구 수 207, `state: aligned`.
4. 사용자 설정과 백업을 TOML로 읽고 REA 항목만 제외해 비교했다. 다른 설정의 값이 같았다.
5. 등록된 명령으로 별도 MCP 프로세스를 실행했다. `initialize` 결과 서버 이름 `rea`, 버전 `6.7.0`. `tools/list` 결과 207개. `tools/call`로 `binary_session`을 호출했고 `isError: false`를 확인했다. 검증 후 프로세스를 종료했다.
6. 위 검사는 별도 MCP 통신 검사다. 현재 대화의 도구 연결이나 실제 바이너리 분석 성공을 의미하지 않는다. doctor도 현재 에이전트 연결을 `unknown`으로 보고했다.

Hopper·Ghidra·IDA·JADX·Kuna는 이번에 설치하거나 연결하지 않았다. doctor는 관련 분석 엔진이 없거나 구성되지 않았다고 보고했다. REA 등록 성공과 각 분석 기능의 준비 상태를 구분한다. 브라우저 CDP 설정도 추가하지 않았다.

## Internet Archive 접속 결과

CUA의 내장 브라우저는 `Browser is not available: iab`를 반환했고, 브라우저 연결 목록도 비어 있었다. 네이티브 Chrome 제어로 요청 URL을 열었다. 로드 후 접근성 화면에서 다음을 확인했다.

- 검색어: `heavy mach`, 탭: `All`, 표시된 결과: 707개.
- [Heavy MACH IPA Archive](https://archive.org/details/heavy-mach-archive_202406)
- [Heavy Mach: Assault (iOS)](https://archive.org/details/com.wemadeent.heavymachassault-ios4.3-clutch-2.0.4)
- [Heavy Mach (1.13, iOS 2.1)](https://archive.org/details/Heavy_Mach_1.13_ios_2.1)

개별 항목 열기, 파일 다운로드, 압축 해제, REA 분석은 하지 않았다. 결과 수와 열린 탭 상태는 당시 관측이다. 후속 작업에서 현재 화면을 다시 확인한다.

## 다음 작업

1. Codex를 재시작한 뒤 현재 제공된 도구에서 REA를 찾아 `binary_session`을 호출한다. 도구가 연결되어 있으면 setup을 반복하지 않는다.
2. 등록은 정상인데 도구가 없으면 REA 스킬의 `references/connection-and-recovery.md`를 읽고 실제 연결 오류를 확인한다. 권한 설정을 임의로 넓히지 않는다.
3. 사용자의 후속 요청에 따라 분석할 Heavy Mach 항목과 버전을 정한다. 검색 결과만으로 분석 대상을 임의 선택하지 않는다.
4. 실제 파일 분석을 요청받으면 해당 스킬의 아카이브·Apple 리소스·네이티브 분석 경로를 따른다. 필요한 엔진과 미확인 범위를 구분한다.

## 프로젝트 보존 상태

- 브랜치: `feat/playable-game-20260908`.
- HEAD: `678266df1679931d45c9639d757d6751b08dffe3`. 인계 작성 시 동일함을 확인했다.
- 작업 진입 때 `HANDOVER-FIRST-FUNCTIONAL-CHANGE.md`와 `docs/handoff-20261014-functional-recheck.md`를 읽었다. 최초 개입 `d495d8aa8177a529d20b77974a8f96ced01e8e32`의 부모가 `6935c919fcd120295e02ac6944290653dd70ed3f`임을 Git 객체로 확인했다. 이 부모를 정상 배포로 간주하지 않았다.
- 기존 기능 복구는 완료된 범위를 유지한다. 최신 인계의 전체 정상화 미완료와 남은 실패는 이번 설치로 해결되지 않았다. 복구·게임 검사를 다시 실행하지 않았다.
- 기존 제품·아트·설정·문서의 dirty 변경을 보존했다. 이번 프로젝트 파일 작성 범위는 이 인계 문서다.
- 커밋·push·배포·Unity 검사·모바일 빌드는 하지 않았다.
- 이 문서는 작업 기록이다. 자동 새 채팅 생성·강제 인계·인계 승인 절차를 추가하지 않는다.
