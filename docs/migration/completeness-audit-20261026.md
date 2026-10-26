# HeavyMach 저장소 전수검사 — 2026-10-26

분리 전 `90a7fc684571130e4209c86fe83f8f0b17250f3a`와 독립 저장소를 대조했다. 게임 소스·테스트·iOS 구성·아트는 v0.9와 같다. Git 추적 대상 595개에 누락은 없었다. 경로를 바꾼 문서·도구·설정은 별도로 대조했다.

## 발견과 수정

| 발견 | 수정·확인 |
|---|---|
| 기존 일반 Git 대상 595개 | 595개 모두 존재. 제품 파일의 Git blob 또는 LFS 원본 SHA-256과 실행 권한을 대조 |
| OpenAL resources의 원본 소스 4개가 Git 제외됨 | 원래 폴더에서 복구. Resources 제외 규칙을 저장소 루트로 한정하여 vendor 소스를 추적. 원본의 CRLF·공백을 포함해 바이트 그대로 보존 |
| 로컬 Analysis가 새 폴더에 없음 | 3,571개 분석 자료 복사·SHA-256 대조 |
| 로컬 호환 실행기 작업 소스가 새 폴더에 없음 | 파일·심볼릭 링크 5,119개 복사·대조. 고정 upstream과 두 패치의 역적용 검사를 통과 |
| 상위 iOS README·작업 지침이 새 저장소에 없음 | 원문 2개를 legacy-ios에 보존하고 Compatibility/AGENTS.md에 현행 경로·보존 규칙을 적용 |
| 문서 링크 5개와 읽기 명령이 옛 경로 사용 | 새 독립 저장소 경로로 수정. 읽기 도구는 실제 절대 경로 출력 |
| 공통 스킬이 Unity 아트 절차·다른 보고 사이트를 가리킴 | HeavyMach 아트 정책과 기존 HeavyMach 보고 사이트로 수정. Unity 절차는 보존 참조로 분리 |
| 독립 폴더의 공통 정책이 없는 harness를 안내 | 실제 document_contract 명령으로 수정. CLAUDE 진입과 로컬 스킬 연결 추가 |

## 검사 범위

Resources 1,600개, Analysis 3,571개, 호환 소스 5,119개, originals 2개를 합한 10,292항목의 파일 해시 또는 링크 값을 원본과 대조했다. 원래 허브 docs에 있던 관련 문서 3개도 확인했다. 이 항목들은 개인 원본·분석·실행기 준비 폴더이므로 기존 Git 제외 정책을 유지한다.

REA 보고 부록 6개를 별도 출력 폴더에서 재생성해 기존 CSV/JSON과 바이트 일치를 확인했다. 원작 함수 목록 1,133개, Assault 함수 목록 1,802개, 증거 ID 1,872개, JSON 자료 2,067개가 그대로 재현됐다. 과거 실패를 삭제하지 않았다. 분석 자체를 다시 실행한 결과와는 구분한다.

문서 연결, Python 도구 구문, 셸 빌드 스크립트 구문을 검사했다. 게임 소스는 수정하지 않았으며 앞선 코어17/17·일반/치트 arm64 Simulator 빌드 결과를 재사용한다. 이번 감사에서 새 폰 설치나 플레이 검사는 하지 않았다.

## 보존한 로컬 증거

분리 전 빌드·DerivedData 85,848항목, 11,243,954,093바이트는 `/Users/sokol/Git/ProjectGameDev`의 기존 위치에 보존했다. 이 범위는 경로·종류·크기를 전수 열거했으며 모든 캐시·trace의 내용 해시를 새로 계산한 것은 아니다. 과거 문서의 build/ 경로는 별도 표시가 없으면 당시 기존 HeavyMach 폴더 기준이다. 새 폴더의 build/와 섞거나 보존 원본에 새 출력을 덮어쓰지 않는다.

호환판의 toolchain과 target은 원래 검증 캐시 위치를 가리키는 링크를 유지했다. 재분석·빌드 시 원본을 변경하지 말고 별도 출력 경로를 사용한다. IPA·Resources·Analysis·캐시·서명 키·사용자 저장을 원격에 올리지 않는다.

재실행:

```sh
python3 tools/audit_repository_migration.py --legacy-root /Users/sokol/Git/ProjectGameDev --output build/migration-audit --site-root build/sites/heavymach-report/dist
```

파일별 목록·해시·모드·변경 목록·보존 위치는 Git 제외 `build/migration-audit/`에 있다. 이 검사는 저장소 분리의 내용 보존을 확인한다. 전체 게임의 원작 동등성이나 모든 기기 성능을 인증하지 않는다.
