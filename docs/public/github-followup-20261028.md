# GitHub 구성과 v0.12 소스 릴리스

README와 개발자의 변에 복원 시작 이야기를 같은 문장으로 실었다. README의 첫 본문에는 개발자의 변에서 “요즘 나는 내게 남은 시간이”로 시작하는 문단을 그대로 옮겼다. 꿈에서 아이팟터치를 처음 쓰던 나를 보았다는 이야기, Mesabloo보다 시작점이 나았다는 설명, 원작에서 벗어난 Codex의 구현을 지적하며 고친 경험을 담았다. [README](../../README.md) · [개발자의 변](../RESTORATION.md)

## 저장소 소개와 기여 안내

About를 현재 iPhone·iPad·Mac 앱과 공용 C++ 코어·입력·자동조준 소개로 바꿨다. 장르와 실제 구현을 나타내는 Topics 10개를 등록했다. [기여 안내](../../CONTRIBUTING.md), 버그·기능 제안 양식과 PR 양식을 추가했다. 개발 PR은 `develop`을 대상으로 한다.

공유 카드는 메인 화면의 전차 일러스트와 기존 HEAVY MACH 로고로 바꿨다. 일러스트를 같은 비율로 확대해 카드 전체를 채우고, 위아래를 잘라 2:1 비율로 맞췄다. 검은 여백과 플랫폼 문구는 삭제했다. 1280×640 PNG다. SVG 배치 파일을 CairoSVG로 렌더링했으며 게임 에셋은 수정하지 않았다.

![메인 화면 전차 일러스트와 기존 HEAVY MACH 로고로 만든 OG 카드](media/github-social-preview.png)

[PNG 원본](media/github-social-preview.png) · [SVG 배치 파일](media/github-social-preview.svg) · [이전 플레이 캡처 카드](media/github-social-preview-gameplay.png)

2026-10-28 저장소를 공개하고 이 카드를 GitHub Social preview에 등록했다. 로그인 없는 저장소 페이지의 OG 이미지가 등록한 PNG와 일치한다. [공개 저장소](https://github.com/gelb2/HeavyMach) · [확인 기록](../evidence/public-repository-20261028.json)

### Xcode 파일 제외 점검

개인별 `xcuserdata`·`xcuserstate`, DerivedData와 빌드 폴더는 이미 제외돼 있었다. 아카이브·검사 결과·디버그 심볼·헤더 인덱스·SPM 빌드 캐시, 구형 Xcode 개인 설정과 추가 서명 파일 형식을 제외 규칙에 넣었다.

| 구분 | 처리 |
|---|---|
| `*.xcarchive`, `*.xcresult`, `*.dSYM`, `*.dSYM.zip`, `*.hmap`, `.build` | 추가 제외 |
| `*.pbxuser`, `*.mode1v3`, `*.mode2v3`, `*.perspectivev3`, `*.xccheckout`, `*.xcscmblueprint` | 추가 제외. `default.*` 공유 설정은 유지 |
| `*.p8`, `*.pfx`, `*.provisionprofile` | 기존 `.p12`·`.mobileprovision` 제외에 추가 |
| `project.pbxproj`, workspace 구성, 공유 scheme, `project.yml`, `Info.plist`, 공용 `.xcconfig`, `Package.resolved` | Git 포함 유지 |

실제 `git check-ignore`로 제외 대상19개와 공유 설정12개를 확인했다. 이미 추적 중인 파일 중 제외 규칙에 걸리는 파일은 0개다. [검사 기록](../evidence/xcode-ignore-audit-20261028.json) · [.gitignore](../../.gitignore)

## 자동 검사

| 작업 | 실행 내용 | 현재 결과 |
|---|---|---|
| Documentation and resources | 문서 연결, 게시 원고의 로컬 링크, 두 도입문 일치, 보존 원문 해시, 자원 1,482개 | 로컬 통과. GitHub 실행은 계정 결제·사용 한도로 시작 전 차단 |
| Core and Apple builds | CMake·CTest, arm64 Simulator 일반·치트, native Mac 일반·치트 빌드 | 압축본에서 로컬 통과. GitHub 실행은 같은 계정 제한으로 차단 |
| Branch flow | main PR의 출발 브랜치가 같은 저장소의 develop인지 확인 | 기존 규칙 유지 |

문서만 바꾼 커밋은 Apple 빌드를 시작하지 않는다. 코드·자원·프로젝트·빌드 workflow를 바꾸면 빌드 작업이 실행된다. GitHub 빌드 성공 배지는 아직 추가하지 않았다. [문서 실행](https://github.com/gelb2/HeavyMach/actions/runs/40542759494) · [빌드 실행](https://github.com/gelb2/HeavyMach/actions/runs/40542759541)

GitHub가 반환한 사유는 계정의 최근 결제 실패 또는 사용 한도다. 워크플로 코드는 실행되지 않았다. 결제·한도를 해결한 뒤 **Actions → Core and Apple builds → Run workflow**에서 `main`을 선택하면 다시 검사할 수 있다. macOS 작업은 [GitHub의 arm64 macos-15 runner](https://docs.github.com/en/actions/reference/runners/github-hosted-runners)를 사용한다.

## v0.12 소스와 자원

[v0.12 Releases](https://github.com/gelb2/HeavyMach/releases/tag/v0.12)에 자원 포함 소스 압축본, 라이선스·고지와 재링크 도구 묶음, `SHA256SUMS.txt`를 등록했다. 업로드한 소스·고지 파일을 다시 내려받아 SHA-256이 같은 것을 확인했다. GitHub 자동 소스 압축에도 LFS 실파일을 포함하도록 설정했다.

`v0.12`는 `eedf245f859a5224adf8f1c4a4b010eda1b5a5bb`를 가리킨다. 태그를 옮기지 않고 해당 커밋의 추적 파일 2,366개를 묶었다. 이미지·음원은 LFS 포인터 대신 실제 파일을 넣었다.

| 검사 | 결과 |
|---|---|
| 압축본 | `HeavyMach-v0.12-source-with-assets.tar.gz`, 146,187,445바이트 |
| SHA-256 | `06ef5e0f0fb31e7fe4f716293030a9d76dee4d3d0c45ac0d56f7ed4f672d5296` |
| 자원 | 1,482개 크기·해시 일치 |
| 공용 코어 | 21개 통과, 실패 0개 |
| Simulator | HeavyMachRestored·HeavyMachValidation 빌드 통과 |
| Mac | HeavyMachMac·HeavyMachMacValidation 빌드 통과 |
| 도구 | Apple Silicon, Xcode 27.1, Debug·서명 없음 |

검사는 `.git`과 기존 빌드 결과가 없는 새 폴더에 압축을 풀어 실행했다. 컴파일·링크는 위 표의 네 앱에서 통과했다. 기기 플레이 결과는 [기존 검증 기록](validation.md)에 있다. [릴리스 검사 근거](../evidence/source-release-v0.12-20261028.json)

## OpenAL 수정 후 재링크

같은 압축본의 OpenAL 1.22.2 버전 문자열에 검사 표식을 넣고 라이브러리와 앱을 다시 빌드했다. iOS Simulator와 Mac 앱의 `debug.dylib`에서 표식을 확인했다. 검사가 끝난 뒤 소스의 버전 문자열을 원래대로 복구했다. Xcode Debug 앱은 실행 진입 파일과 실제 앱 코드가 들어가는 dylib가 나뉘므로 후자도 검사한다.

새 폴더에 푼 소스와 현재 main의 검사 도구로 재현할 수 있다.

```sh
python3 tools/verify_openal_relink.py --source /path/to/unpacked/HeavyMach-v0.12
```

OpenAL 소스는 압축본의 `vendor/openal-soft`에 있고, 빌드 설정은 `tools/build_openal.sh`에 있다. 소스를 수정한 라이브러리로 다시 링크할 수 있음을 확인했다. [재링크 결과](../evidence/openal-relink-20261028.json) · [LGPL 원문](../../LICENSES/OpenAL-Soft-LGPL-2.0.txt)

## 저장소 공개

[자원 목록](../rights-manifest.json)에 원작 그림·음원·데이터, 추가 그림, 캡처·영상의 출처와 확보할 공개 범위를 정리했다. 현재 자료에는 원작 자원의 공개·재배포 허락 문건이 없다.

외부 구현의 참조 커밋과 실제 파일을 대조하고 touchHLE MPL-2.0, rust-sdl2 MIT 원문을 보관했다. `HMCompatibilityPixels.hpp`에는 MPL-2.0을 표시했다. [외부 코드 고지](../../THIRD_PARTY_NOTICES.md)

전체 브랜치·태그의 145개 커밋과 연결된 파일 내용 3,054개, 현재 추적 파일 2,439개, LFS 실파일 1,524개, v0.12 릴리스 첨부 파일 3개를 검사했다. Gitleaks 8.30.1에서 탐지한 비밀값은 0건이다. 개인 키·프로비저닝·환경변수 파일명도 Git 이력에서 발견되지 않았다. 검사 오류와 LFS 누락은 없었다. [전면 검사 기록](../evidence/secret-audit-20261028.json)

Git 차이 검사는 병합 등을 제외한 99개 커밋을 처리했다. 누락 여부를 확인하려고 전체 참조에 연결된 파일 내용도 따로 꺼내 검사했다. 2026-10-28 저장소를 공개로 전환했다. 로컬 경로·개발 팀 ID가 있는 기존 문서·설정 49개는 [기존 목록](../evidence/publication-source-audit-20261028.json)에 남아 있다.

공유 이미지 등록은 완료했다. GitHub Actions의 기존 실행 제한과 원작·참조 그림의 이용 조건 확인 기록은 남아 있다. 앱 설치 파일의 일반 배포에는 플랫폼별 서명·설치 검사가 추가로 필요하다.

[전체 문서](../README.md) · [GitHub 구성 계획](github-repository-plan-20261028.md) · [빌드·설치](build-and-install.md)


## 빌드·실행 스크립트 조사 — 2026-10-28

| 프로젝트 | 실제 설치·실행 방식 |
|---|---|
| [Mesabloo/hm-defense](https://github.com/Mesabloo/hm-defense#installation) | Gradle Wrapper의 `./gradlew :desktop:dist`로 JAR을 만든 뒤 Java로 실행한다. 실행용 wrapper는 README의 TODO다. |
| [DevilutionX](https://github.com/diasurgical/devilutionX/blob/master/docs/installing.md) | 운영체제별 실행 파일에 원작 데이터를 넣어 실행한다. GameShell에는 빌드·업데이트를 처리하는 실행 도구가 있다. |
| [OpenRCT2](https://github.com/OpenRCT2/OpenRCT2#2-downloading-the-game-pre-built) | 설치 프로그램·휴대용 빌드를 제공하고 원작 게임 파일의 위치를 지정한다. |

HeavyMach에는 자원 다운로드·검사, OpenAL과 앱 빌드, Mac 실행 또는 iPhone·iPad 설치를 묶는 스크립트를 추가할 예정이다. 필요한 도구와 SDK, 개발 팀·서명 설정을 안내하고 일반·치트 앱을 선택하게 한다. 스크립트는 아직 구현하지 않았다. [내부 TODO](../usability-todo.md#hm-build-launcher--다운로드빌드실행-절차-통합).

## 브랜치 정리 — 2026-10-28

origin을 갱신한 뒤 main·develop의 로컬/원격 차이가 각각 0/0임을 확인했다. 두 브랜치의 파일 내용도 같았다. develop·main에 모두 병합된 기존 문서 브랜치 로컬 10개와 origin 4개를 정리했다. 삭제한 브랜치의 마지막 커밋과 병합 여부는 [정리 기록](../evidence/branch-cleanup-20261028.json)에 있다. v0.9~v0.12 태그와 릴리스 worktree, migration-source는 유지했다.


## 원작 제작자·위메이드 표기 검토 — 2026-10-28

위메이드의 역할을 보강할 근거가 있다. 변해준·박재철은 원작 제작자로, 위메이드 크리에이티브는 2011년 IP 인수 주체로 구분해 적는 안이다. 현재 README와 개발자의 변에는 수익 목적이 없다는 의향과 게시 중단 요청 시 삭제한다는 문장이 이미 있다.

### 확인한 이력

| 시점·자료 | 확인한 내용 |
|---|---|
| [2009년 개발자 인터뷰](https://www.koreajoongangdaily.com/lifestyle/korean-game-developers-shoot-for-success/10531072) | HeavyMach의 개발자 변해준과 그래픽 디자이너 박재철을 소개했다. |
| [2009년 출시 기사](https://toucharcade.com/2009/02/10/first-look-at-heavy-mach-side-scrolling-tank-shooter/) | 출시 주체를 IndieAn으로 표기했다. |
| [2011년 5월 인수 보도](https://zdnet.co.kr/view/?no=20110512162801) | 위메이드 엔터테인먼트의 자회사 위메이드 크리에이티브가 변해준을 영입하고 HeavyMach·HeavyMach 2 IP를 포함한 자산을 양수했다고 보도했다. [같은 시기 인디앱스 인수 보도](https://www.inews24.com/view/572510)도 있다. |
| [위메이드가 배포한 2011년 10월 보도자료](https://www.newswire.co.kr/newsRead.php?no=575013) | 위메이드 엔터테인먼트가 Assault·Battle·Defense의 무료화 리뉴얼을 발표했다. |
| [Mesabloo README](https://github.com/Mesabloo/hm-defense#credits--license) | Heavy MACH: Defense를 대상으로 We Made Entertainment에 크레딧을 표기했다. 추가 설명에는 스튜디오 통지, 문제 발생 시 삭제·비우기, 금전 수령 거절을 적었다. |

우리 복원 대상은 2009년 HeavyMach다. Mesabloo가 다루는 Defense와 판본이 다르지만, 2011년 인수 보도에는 HeavyMach와 HeavyMach 2도 포함돼 있다. 따라서 위메이드와의 관계는 Defense에만 해당하지 않는다. 2011년 이후의 권리 이전과 현재 계약 주체는 이번 자료로 확인되지 않았다.

### Mesabloo 문장과 현재 문서 비교

| 항목 | 현재 HeavyMach 문서 | 보강안 |
|---|---|---|
| 원작 크레딧 | CREDITS에 변해준·박재철 표기 | IndieAn 출시 이력과 위메이드 크리에이티브의 IP 인수 이력 추가 |
| 수익 목적 | README·개발자의 변에 수익 목적 없음, 향후 허락받은 앱스토어 배포도 무료라는 의향 | 기존 문장 유지. Mesabloo의 후원금까지 거절한다는 약속은 별도 선택 사항 |
| 게시 중단 | 원작자·다른 이해관계자 요청 시 저장소 삭제 | 기존 문장 유지. 위메이드도 인수 이력과 함께 이름을 명시 |
| 스튜디오 통지 | 박재철 본인의 것으로 보이는 프로필에 문의했다는 작성자 기록 | 위메이드에 알렸다는 기록은 없다. Mesabloo의 통지 사실을 이 프로젝트의 사실로 옮기지 않음 |
| 새 코드와 원작 자료 | MIT 적용 범위와 자원 출처를 별도 문서로 관리 | 자원 문서에도 같은 인수 근거 연결 |
| 위법 가능성 문장 | 원작 자원의 허락 문건 수령 여부를 자원 문서에 기록 | 법적 결론을 새로 쓰기보다 확인한 제작·인수 이력과 기존 게시 중단 의사를 사용 |

비영리 의향, 제작자 표기, 이용허락은 각각 다른 내용이다. Mesabloo의 README는 그가 정한 공개 방침이며, HeavyMach의 이용허락 자료는 아니다.

### 넣을 문장안

README의 ‘라이선스와 자원’ 절에는 다음 두 문장을 추가하는 안이다.

> HeavyMach 원작은 변해준이 개발하고 박재철이 그림을 맡았다. 2011년에는 위메이드 엔터테인먼트의 자회사 위메이드 크리에이티브가 HeavyMach·HeavyMach 2의 IP를 포함한 인디앱스 자산을 인수했다.

바로 뒤에 인수 기사와 CREDITS 링크를 둔다. 개인적인 복원 동기를 다룬 첫 문단은 유지한다.

CREDITS의 원작 항목은 다음 역할로 나눈다.

- 게임 개발: 변해준
- 일러스트: 박재철
- 2009년 출시 표기: IndieAn
- 2011년 IP 인수: 위메이드 크리에이티브 — 당시 위메이드 엔터테인먼트 자회사
- 2011년 시리즈 리뉴얼 발표: 위메이드 엔터테인먼트

개발자의 변에는 두 제작자에 관한 개인적인 문단을 유지하고, 회사의 인수 이력은 CREDITS로 연결하는 안이다. 자원 문서·권리 목록·SaaS 집필 자료에도 같은 출처를 연결한다. 새 코드의 MIT 라이선스와 원작 자원 구분은 유지한다.

현재 단계는 검토안 작성이다. README·개발자의 변·CREDITS·라이선스 본문은 이번 검토에서 변경하지 않았다.
