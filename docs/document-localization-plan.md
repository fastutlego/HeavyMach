# GitHub 방문자용 국문·영문 문서 계획

2026-10-29. 최신 사용자 지시에 따라 번역 범위를 GitHub 방문자용 설명 문서로 한정한다. 한국어를 기본으로 두고 영어판을 추가한다. [번역 대상 목록](public/document-translation-scope.json).

## 번역할 내용

방문자가 프로젝트를 이해하고, 빌드·설치·플레이하고, 개발 동기와 기획 의도를 읽는 데 필요한 문서를 번역한다.

| 방문자가 찾는 내용 | 기존 문서 또는 작성할 공개 문서 |
|---|---|
| 프로젝트 소개·게임 화면 | `README.md` |
| 빌드·설치·조작 | `docs/public/build-and-install.md`, `gameplay-guide.md` |
| 현재 기능·완료 범위 | `docs/public/features.md`, `implementation-status.md` |
| 기술 구조·개발 과정·문제 해결 | `docs/public/architecture.md`, `development-history.md`, `troubleshooting.md`, `validation.md` |
| 개발자의 변·복원 동기 | `docs/RESTORATION.md` |
| 럭스루나에의 기획 의도 | `docs/public/lux-lunae-intent-20261028.md` |
| 자동조준·추가 유닛·화면 확장·HUD의 기획 의도 | 새 공개 문서 `docs/public/design-intent.md` |
| 버전별 변경 | `CHANGELOG.md` |
| 방문자용 남은 과제 | 새 공개 문서 `docs/public/roadmap.md` |
| 제작자·기여·라이선스 안내 | `CREDITS.md`, `CONTRIBUTING.md`, `LICENSE-SCOPE.md`, `ASSET-LICENSES.md`, `THIRD_PARTY_NOTICES.md` |
| 방문자용 문서 목차 | 새 공개 문서 `docs/public/index.md` |

명시한 기존 문서는 17개이고 새 공개 문서는 3개다. 새 문서 3개는 아직 작성 전이다. 원본 라이선스·이미지·영상·소스코드·명령 출력은 공용 파일을 연결한다. 이미지의 캡션과 대체 텍스트는 영어로 작성한다.

`docs/public/`에 있다는 이유만으로 전부 번역하지 않는다. 그 안에도 작업 계획·검수 보고·SaaS 담당 세션용 집필 안내가 섞여 있으므로 위 목록의 파일만 대상으로 삼는다.

## 개발 기준과 공개 설명의 관계

개발은 지금 사용하는 사용자 명령 원문, 국문 정본·기획서·정책, 기존 개발 스킬을 계속 따른다. 공개 설명에 필요한 내용만 확인해 별도 원고로 쓴다.

```text
사용자 명령 원문 · 개발 정본 · 내부 기획서 · 정책 · 코드
                         ↓ 내용 확인
              GitHub 방문자용 한국어 설명
                         ↓ 번역
              GitHub 방문자용 영어 설명
```

영어 설명을 수정했다고 개발 기준이나 게임 코드에 그 내용을 역으로 적용하지 않는다. 문서가 실제 동작과 다르면 코드·정본·사용자 지시를 확인해 공개 설명을 고친다. 새 기능 요구는 기존 개발 절차로 다룬다.

다음 파일은 이번 번역의 대상과 수정 경로에서 제외한다.

- `AGENTS.md`, `CLAUDE.md`, `.agents/`, `.claude/`와 개발 스킬.
- `docs/canonical.md`, `game-design.md`, `design.md`, 문서 위계·업무 연결표.
- 좌표·렌더링·데미지·저장·생명주기 등 개발 정책과 내부 기능 기획서.
- 사용자 명령 원문, 보존된 대화·인용 원본, 인계와 검사 증거.
- `docs/usability-todo.md`, `current-status.md` 등 내부 업무 기록. 이번 번역 과제의 상태 기록만 해당 TODO에 갱신한다.
- `docs/public/publication-inventory.md` 등 다른 작업 세션에 주는 집필 안내.

`docs/README.md`는 방문자 안내와 내부 개발 문서를 함께 연결한다. 이 파일 전체를 번역하는 대신, 방문자용 목차 `docs/public/index.md`를 새로 만든다. 내부 개발 문서를 방문자에게 설명할 필요가 생기면 `architecture.md`나 `design-intent.md`에 필요한 내용을 작성한다.

## 언어 선택과 문서 경로

| 한국어 | 영어 |
|---|---|
| `README.md` | `README.en.md` |
| `CHANGELOG.md`, `CREDITS.md` 등 대상 루트 문서 | `CHANGELOG.en.md`, `CREDITS.en.md` 등 |
| `docs/RESTORATION.md` | `docs/en/RESTORATION.md` |
| `docs/public/index.md` | `docs/en/public/index.md` |
| 대상 `docs/public/*.md` | 대응하는 `docs/en/public/*.md` |

README와 대상 문서의 제목 아래에 `한국어 | English`를 둔다. 영어 README에서 설치·기능·기획 의도를 누르면 각각 영어 문서로 이동한다. 언어를 바꾸면 같은 문서의 다른 언어판으로 이동한다.

국문 파일의 기존 주소와 절 링크는 유지한다. 번역된 제목의 절 주소는 별도로 대조한다. 이미지·영상은 기존 파일을 공유하고 캡션·표·대체 텍스트를 번역한다. 문서 본문에 번역 검사 상태나 작성자용 지침을 붙이지 않는다.

## 문체와 번역 절차

기존 `write-project-text`를 이 문서 작업에 적용한다. 개발 스킬의 본문이나 자동 호출 범위는 변경하지 않는다. 번역에 필요한 세부 규칙과 용어집은 `docs/translations/editorial-guide.md`에 두고, 공개 문서 번역 작업에서만 읽는다.

작업 순서는 국문 문체 점검, 영어 번역, 원문 대조, 영어판 단독 읽기, 링크·그림 확인이다.

- 국문과 영문 모두 메타카피·불필요한 면책·자기합리화·과장된 표현을 제거한다. 실제 동작·원인·수치·결과를 쓴다.
- 개발자의 변과 기획 의도는 작성자의 인칭·정서·비유·비판의 강도를 보존한다. 작성자가 직접 고친 문장은 최신 main과 대조한다.
- 기술문서는 행위자·조건·시제·단위·수치를 확인한다. 완료·계획·실패·삭제된 기능을 서로 바꾸지 않는다.
- 명령·코드 식별자·파일명·키 매핑은 실제 값과 맞춘다. 자동조준을 자동 발사로 바꾸거나 연사속도와 미사일 이동 속도를 혼동하지 않는다.
- 금지어 검색에 이어 문단 전체를 읽는다. 숫자가 같아도 값이 붙은 대상이나 인과관계가 달라졌는지 대조한다.

[Google의 기술문서 지침](https://developers.google.com/style/translation)은 짧은 문장과 용어 일관성에 참고한다. 개인 원고의 문화적 경험과 비유는 원문에 따라 번역한다. [Python의 번역 지침](https://devguide.python.org/documentation/translations/translating/)에서 의미·용어집·식별자 보존 방식을 참고한다.

## 구현할 문서와 도구

| 대상 | 작업 |
|---|---|
| `docs/public/document-translation-scope.json` | 번역할 공개 문서의 경로와 새 원고 작성 여부를 명시. 현재 작성 완료 |
| `docs/translations/editorial-guide.md` | 공개 문서 번역용 문체·용어·대조 절차 작성 |
| `docs/translations/manifest.json` | 목록에 있는 국문·영문 경로, 검토한 원문 해시, 영어판 해시와 상태 기록 |
| `tools/check_translations.py` | 대상 문서의 누락·원문 변경·언어 링크·앵커·코드·수치 후보 검사 |
| `tools/tests/test_translations.py` | 검사 실패 사례와 제외 경계 검사 |
| `tools/check_publication.py` | 대상 영문 원고 검사와 README 언어 링크 인식 추가. 기존 저자 본문 보호 유지 |
| 문서 전용 CI | 대상 공개 원고·번역 도구가 변경됐을 때 실행 |

검사 대상은 명시 목록으로 제한한다. 디렉터리 전체를 재귀적으로 번역하거나 새 국문 파일을 자동 등록하지 않는다. 제외한 개발 문서가 번역 목록에 들어오면 실패하도록 검사한다.

국문 공개 원고가 바뀌면 대응 번역을 갱신 대상으로 표시한다. 국문 정본이나 내부 기획서 수정만으로 영어 번역 완료를 게임 버그 수정의 병합 조건으로 걸지 않는다. 공개 설명이 바뀌는 릴리스에서 해당 원고와 영어판을 함께 갱신한다.

기존 `docs/document-map.json`의 개발 읽기 경로는 유지한다. 번역 계획을 그 연결표에 추가했던 이전 변경은 이번 수정에서 제거했다. 새로운 번역 절차를 게임 작업의 필수 읽기 경로로 등록하지 않는다.

## 구현 순서

| 단계 | 작업 | 확인할 결과 |
|---|---|---|
| 1 | 대상 17개 확인, 새 공개 목차·기획 의도·남은 과제 원고 작성 | 방문자가 읽을 내용과 내부 개발 기준이 분리됨 |
| 2 | 번역 지침·용어집·문서 대응 목록과 검사 구현 | 제외 경계, 원문 갱신, 링크·코드 오류 검사가 동작함 |
| 3 | README·설치·기획 의도에서 먼저 시험 | 기술 설명·명령·개인 어조·언어 이동을 확인함 |
| 4 | 나머지 대상 공개 문서 번역과 국영문 문체 검수 | 목록의 모든 공개 문서에 대응 영어판이 있음 |
| 5 | GitHub 렌더·언어 이동·그림·절 링크 검사 | 영어 문서끼리 이동하고 같은 국문 문서로 돌아갈 수 있음 |
| 6 | 최신 main 직접 수정 대조 후 develop→main 병합 | 저자 원고·개발 기준·게임 파일 보존, 공개 문서 검사 통과 |

번역은 별도 문서 브랜치에서 진행한다. 병합 전에는 게임 소스·에셋·프로젝트 설정·AGENTS·스킬·정본·정책·사용자 명령 원문이 번역 변경에 섞이지 않았는지 diff로 검사한다.

완료 기준은 대상 공개 문서의 번역, 국영문 문체·의미 대조, 언어 선택과 내부 링크 동작이다. 게임 기능 테스트는 게임 코드 변경 없이 문서 번역만 진행한 경우 새로 수행하지 않는다.

## 해외 사례에서 가져올 방법

| 사례 | 이번 작업에 쓸 방법 |
|---|---|
| [RustDesk](https://github.com/rustdesk/rustdesk/blob/master/README.md) | README 상단에서 언어별 Markdown으로 이동 |
| [Kubernetes](https://kubernetes.io/docs/contribute/localization/) | 언어별 디렉터리에서 대응하는 문서 경로 유지 |
| [Python](https://devguide.python.org/documentation/translations/translating/) | 의미 대조·용어집·링크와 식별자 보존 |
| [Godot](https://docs.godotengine.org/en/4.4/contributing/documentation/editor_and_docs_localization.html) | 본문·마크업·이미지 설명을 각각 확인 |
| [Weblate](https://docs.weblate.org/en/latest/workflows.html) | 원문 변경 시 번역 갱신 상태 관리 |
| [Diátaxis](https://diataxis.fr/start-here/) | 설치 절차와 구조·기획 설명의 목적에 맞춰 작성 |

현재 Markdown과 기존 보고 사이트를 사용한다. Vale는 공개 영문 원고의 반복 문구를 검사하는 후보로 남겨 두며, 도입 여부는 대표 문서 시험에서 정한다.

## 이전 전수 목록의 처리

이전 조사에서는 저장소 문서209개를 집계했다. 그 수는 저장소 규모이며 이번 번역 목표가 아니다. 당시 [전수 목록](evidence/document-localization-inventory-20261029.json)은 조사 기록으로 보존하고, 모든 항목의 번역 제안은 철회한 것으로 표시했다. 실제 번역 대상은 새 공개 문서 목록에서만 읽는다.

이번 수정에서 완료한 것은 범위 확정·계획·목록·TODO·보고서 갱신이다. 영어 원고와 번역 도구 구현은 위 단계에 따라 진행한다.
