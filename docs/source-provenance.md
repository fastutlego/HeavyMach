# 원본과 증거 출처

2026-10-24 기준. 외부 페이지 내용의 새 검증이 아니라 사용자 제공 링크와 기존 확보 증거를 기록한다.

## 입력 경로

- 사용자 제공 검색: [Internet Archive heavy mach 검색](https://archive.org/search?query=heavy+mach&tab=all).
- 검색에서 확인한 모음: [Heavy MACH IPA Archive](https://archive.org/details/heavy-mach-archive_202406).
- 복원 대상: [Heavy Mach 1.13, iOS 2.1](https://archive.org/details/Heavy_Mach_1.13_ios_2.1).
- 비교 대상: [Heavy Mach Assault](https://archive.org/details/com.wemadeent.heavymachassault-ios4.3-clutch-2.0.4).

파일 보관 루트 `REA=/Users/sokol/Documents/Codex/2026-10-24/rea-ready`. `REA`는 이 문서의 경로 약칭이며 환경 변수 설정이 아니다.

| 대상 | 식별 | SHA-256 |
| --- | --- | --- |
| `REA/Heavy_Mach_1.13_ios_2.1.ipa` | 원작 1.13 | `477c672f92110c6020be3f744dfa7a3456739e6f970c66715e3b7b3cd32b7776` |
| `REA/HeavyMach-1.13-extracted/Payload/HeavyMach.app/HeavyMach` | ARMv6, `kr.connect.touch.HeavyMach.game` | `97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb` |
| `REA/HeavyMachAssault.ipa` | Assault 1.0.3 | `71368f0167061e629deb2db2d7a53b7338e03239740caa2e5771a9ac77fb00a8` |
| `REA/Assault-extracted/Payload/HeavyMach.app/HeavyMach` | ARMv7, `com.wemadeent.HeavyMachAssault` | `37827b677f739c48a55b6d4c5362f61fccbe92bf713727e9f0526e4eff779762` |

Clutch-2.0.4는 아카이브 이름의 도구 표기다. Assault 앱 버전은 Info.plist의 1.0.3이다. 두 Mach-O의 cryptid는 0이었다. 해시 유지 근거는 `REA/comparison/final-preservation.json`이다. 원본 개발 소스는 확보하지 못했다.

## REA 분석 증거

도구는 rea-agents 6.7.0, Ghidra 12.1.4였다. 출처는 `REA/comparison/tool-provenance.json`, 기존 CLI는 `/Users/sokol/.local/share/rea/rea`다.

| 증거 | 위치 또는 ID |
| --- | --- |
| 원작 inventory | `ev_19b1c9ea3e13378cf42f18b2eced6aaae122368c274a20fdeebd0e8c03f1ea68` |
| 원작 extract | `ev_2b5227cc4c34d2148e73a7de81382e791fd5e14128b8d7c146a7907d499164f1` |
| 함수·주소·Evidence 대응 | `REA/comparison/core/evidence-index.json`, `original-function-index.json`, `original-procedures.json` |
| 원작 대표 함수 | `REA/comparison/core/original-function-00..20.json`와 읽기용 `.c` |
| 전체 리소스 비교 | `REA/comparison/assets/resource-comparison.json`, `FINDINGS.ko.md` |
| 원시 구조·offset·payload | `REA/comparison/assets/parsed-game-data.json` |
| 데이터 비교 재현 | `REA/comparison/assets/compare_resources.py`, `decoder-validation.json` |
| 타이틀·조작 추가 분석 | `/Users/sokol/Documents/Codex/2026-10-24/heavymach-recovery/title-analysis/`의 JSON·C·findings |

`.c`는 디컴파일 의사 코드다. 원본 작성자의 소스, 완전한 타입 복원, 컴파일 가능성을 보장하지 않는다. 새 함수 복원은 입력 SHA, 주소, 원본 Evidence, 해석 근거, 미확인을 함께 남긴다.

## 자원 보존

원작 일반 파일 776개 중 이미지 695개, WAV 31개, 맵 43개와 MCH 3개를 확인했다. 두 판본의 같은 경로·바이트 일치 파일은 744개다. 원작 이미지 674개와 WAV 31개는 Assault와 동일하지만 Stage01·Stage36~41과 Boss08 부품 등에는 차이가 있다. 원작 복원에 Assault 파일을 일괄 대체하지 않는다.

기존 상세 보고: [판본 분석](heavy-mach-113-assault-analysis-20261024.md), [분석 인계](handoff-20261024-rea-install.md), [호환 실행 검증](heavymach-recovery-20261024.md). 과거 보고의 미착수 표기는 당시 상태다. 현재 판정은 [current-status](current-status.md)를 따른다.
