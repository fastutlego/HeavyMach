# GitHub 공개·배포 라이선스와 README 배지

나는 코드와 원작 에셋을 함께 공개해, 다른 사람이 저장소를 받아 직접 빌드하고 플레이할 수 있게 하려 한다. 개발 과정과 수정 기록도 계속 남기고 싶다. 이 복원판으로 돈을 벌 생각은 없다. 원작자와 이해관계자들이 앱스토어 배포를 허락하더라도 무료로 제공할 생각이다.

이 목적에는 **새 코드의 라이선스, 이 복원판에 원작 자원을 포함해 공개하는 허락, 외부 라이브러리의 배포 조건**이 필요하다. 이번 작업에서 새 코드 기여분에 MIT를 적용하고, 원작 자원의 조건은 별도로 기록했다. [코드 적용 범위](../../LICENSE-SCOPE.md), [자원 조건](../../ASSET-LICENSES.md), [외부 고지](../../THIRD_PARTY_NOTICES.md). 누구나 빌드·플레이하게 하는 데 원작 그림을 다른 상품에 재사용할 권리까지 허용할 필요는 없다. 에셋을 제외하고 각자 구하게 하는 방식은 이번 공개 목표에 맞지 않는다.

버전·OS를 작은 칩 모양으로 표시하는 것은 **배지(badge)**다. 필수는 아니지만 README에서 실행 환경을 빠르게 찾는 데 도움이 된다. HeavyMach에는 제목 아래에 버전·플랫폼·아키텍처·언어를 표시하고, 실제 검사 환경은 본문의 표로 설명하는 안을 제안한다.

Codex가 2026-10-28에 저장소 파일과 GitHub·GNU·Creative Commons·Apple 공식 자료를 대조했다. 아래는 조사 결과와 적용 내역이다. README에 기술 배지4개와 코드 라이선스 배지1개, 최소 OS·실제 검사 환경 표를 추가했다. [GitHub 구성 점검·후속 계획](github-repository-plan-20261028.md).

## 현재 저장소에서 확인한 내용

| 항목 | 확인한 상태 | 근거 |
|---|---|---|
| 공개 상태·라이선스 | 비공개 저장소. 새 코드용 MIT와 별도 적용 범위를 이번 문서 변경에 추가 | GitHub 저장소 조회, [제작자·자원](../../CREDITS.md) |
| 원작 자원 | 기존 이미지·WAV·맵·데이터와 앱 아이콘이 Git/LFS에 포함됨 | [빌드 자원 목록](../build-resources.json) |
| 원작 배포 허락 | 개발자의 변에는 향후 배포를 허락받는 조건이 적혀 있음. 확인한 원고·자료에 허락받았다는 기록은 없음 | [개발자의 변](../RESTORATION.md) |
| 새 그림 | 메뉴 확장, VTOL, 전투기. 원작·실물·편대 도색을 참조한 제작 기록이 있음 | [VTOL](vtol-art-and-loadout-20261027.md), [전투기](lux-lunae-design-20261027.md) |
| OpenAL Soft | 1.22.2. 주된 소스 헤더는 LGPL-2.0-or-later. 정적 라이브러리로 빌드해 앱에 링크 | [COPYING](../../vendor/openal-soft/COPYING), [소스 헤더](../../vendor/openal-soft/al/source.cpp), [빌드 스크립트](../../tools/build_openal.sh) |
| stb | MIT 또는 Public Domain 중 선택하는 이중 라이선스 | [LICENSE](../../vendor/stb/LICENSE) |
| 자동화 | GitHub Actions에는 main 병합 출발 브랜치를 확인하는 작업만 있음 | [branch-flow.yml](../../.github/workflows/branch-flow.yml) |
| 버전 | Git 태그 v0.12. 앱 번들 표시는 1.13, 빌드 번호 1 | [변경 이력](../../CHANGELOG.md), [iOS 명세](../../iOS/project.yml), [Mac 명세](../../macOS/project.yml) |

## 공개 저장소와 자유 재배포

GitHub 공개 저장소는 다른 사람이 내용을 보고 fork할 수 있게 한다. 코드의 수정·재배포를 허용하는 조건은 라이선스가 정한다. GitHub도 라이선스가 없는 저장소와 오픈소스 라이선스를 제공하는 저장소를 구분한다. [GitHub 라이선스 안내](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/licensing-a-repository)

원작 자원은 권리자가 허락한 이용 방법과 조건에 따라 쓸 수 있다. 무료 배포라는 이유만으로 이용허락을 대신할 수는 없다. HeavyMach를 그림·음원까지 포함해 공개하려면 해당 자원의 현재 저작재산권자와 공개 범위를 확인해야 한다. 제작자 이름이 알려져 있다는 사실만으로 현재 권리 귀속까지 확정되지는 않는다. [저작권법 제46조](https://law.go.kr/LSW/lsSideInfoP.do?docCls=jo&joBrNo=00&joNo=0046&lsiSeq=283335&urlMode=lsScJoRltInfoR)

허락 문건에는 다음 범위를 구체적으로 적는 것이 좋다.

- 대상: 게임 코드·이미지·음원·맵·텍스트·제목·로고 중 허용하는 항목과 버전.
- 게시: GitHub 공개 저장소와 이력, fork·clone·다운로드, SaaS의 이미지·영상 소개.
- 배포: 수정된 게임의 소스와 실행 파일, iPhone·iPad·Mac·향후 Android, 배포 지역과 기간.
- 재이용: 제3자의 수정·재배포·재허락, 상업 이용 허용 여부, 제작자 표시 방법.

이번에 확보하려는 자원 이용 범위는 GitHub에서 코드와 에셋을 함께 보관·공개하고, 방문자가 clone·fork한 뒤 자신의 기기에서 빌드·플레이하는 것이다. 수정본 공유도 허용하려면 그 범위를 함께 정한다. 앱스토어의 무료 배포는 별도 항목으로 둔다. 원작 에셋의 단독 판매나 다른 게임에서의 재사용까지 허용하는 조건은 목표에 포함하지 않는다.

무료 배포 의향은 개발 목적을 설명한다. 원작 자원을 포함한 공개의 근거는 권리자가 허락한 이용 범위에서 확인한다. 코드용 MIT·BSD 파일이나 무료 배포 문구 자체가 그 허락을 대신하지는 않는다. [저작권법 제46조](https://law.go.kr/LSW/lsSideInfoP.do?docCls=jo&joBrNo=00&joNo=0046&lsiSeq=283335&urlMode=lsScJoRltInfoR)

새 C++ 구현도 파일별 출처를 확인해야 한다. 독자적으로 작성한 부분과 원작 표현·외부 코드에서 옮긴 부분을 구분한 뒤 적용 라이선스를 정한다. 동작이 같다는 사실이나 소스를 새로 작성했다는 사실 하나만으로 저장소 전체의 권리를 판정할 수는 없다.

## 필요한 문건

| 문건·위치 | 담을 내용 | 현재 제안 |
|---|---|---|
| 루트 `LICENSE` | 권리를 보유하거나 허락받은 새 코드의 표준 라이선스 원문 | 추가 완료. 새 코드 기여분과 제외 대상은 LICENSE-SCOPE.md에 명시 |
| 루트 `ASSET-LICENSES.md` | 원작·추가·참조 이미지, 음원, 맵, 아이콘, 스크린샷의 출처와 이용 조건 | 추가 완료. 자원 경로별 출처·이용허락 확인 상태 기록 |
| 루트 `THIRD_PARTY_NOTICES.md` | 라이브러리명·버전·출처·라이선스·수정 여부와 고지 | 추가 완료. OpenAL Soft·stb와 외부 도구·스킬의 고지·확인할 범위 연결 |
| `LICENSES/` | 적용되는 외부 라이선스의 원문 | OpenAL Soft·stb·외부 스킬 MIT의 원문 사본 추가 |
| `docs/rights-manifest.json` | 파일/경로·출처·권리자·허락 범위·근거 문건·배포 가능 여부 | 기존 자원 SHA-256 목록과 연결 |
| 기존 `CREDITS.md` | 원작 제작자, 기획·구현·미술 담당과 출처 | 저작권 이용허락 문건과 함께 읽도록 연결 |
| README의 라이선스 절 | 코드 라이선스와 자원 조건의 요약·링크 | README에 추가. LICENSE-SCOPE·ASSET-LICENSES·외부 고지로 연결 |
| 배포 묶음의 고지·소스 | 앱에 포함된 라이브러리의 고지, 해당 버전 소스와 재빌드 자료 | Git 태그·자원·바이너리 해시를 연결 |

`LICENSE`에는 표준 원문을 유지하고, 파일별 예외와 설명은 별도 문건에 둔다. GitHub의 자동 라이선스 인식도 이 구성을 권한다. [GitHub의 라이선스 위치·인식](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/licensing-a-repository)

### 새 코드의 후보

| 후보 | 허용 범위와 조건 | HeavyMach에서 선택할 이유 |
|---|---|---|
| MIT | 수정·재배포·상업 이용 허용. 저작권·라이선스 고지 유지 | 간단한 재사용·배포가 목표일 때 우선 후보 |
| BSD-3-Clause | 수정·재배포·상업 이용 허용. 고지 유지와 원저작자 이름을 이용한 홍보 제한 | Mesabloo가 코드에 적용한 방식과 비교할 후보 |
| Apache-2.0 | 수정·재배포·상업 이용 허용. 특허 허여와 변경·고지 조건 포함 | 특허 조건을 명시할 필요가 있을 때 비교 후보 |
| CC BY 4.0 | 출처·라이선스·변경 표시를 조건으로 공유·수정·상업 이용 허용 | 권리를 확보한 문서·별도 그림 후보. 게임 코드에는 소프트웨어 라이선스 사용 |

MIT와 Apache-2.0은 다른 사람의 상업 이용도 허용한다. 개발자가 직접 수익을 낼 생각이 없다는 의향과, 다른 사람의 상업 이용을 금지하는 조건은 별개다. [MIT](https://choosealicense.com/licenses/mit/), [Apache-2.0](https://choosealicense.com/licenses/apache-2.0/)

Creative Commons는 소프트웨어에는 전용 소프트웨어 라이선스를 권하고, 별도 문서·게임 그림·음악에는 CC를 사용할 수 있다고 설명한다. 그림·문서에 CC BY를 적용할 때도 해당 부분의 권리를 먼저 확보해야 한다. [CC의 소프트웨어 안내](https://creativecommons.org/faq/#can-i-apply-a-creative-commons-license-to-software), [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/)

### OpenAL Soft 정적 링크

현재 `build_openal.sh`는 `LIBTYPE=STATIC`으로 `libopenal.a`를 만들고, iOS·Mac 프로젝트가 이를 링크한다. 저장소의 LGPL v2 제6절은 라이브러리 고지·라이선스와 함께, 수정한 라이브러리로 앱을 다시 링크할 수 있는 소스 또는 오브젝트 등의 제공 조건을 정한다. 앱 전체를 반드시 GPL로 바꾸라는 조건과는 다르다. [현재 사용한 라이선스 제6절](../../vendor/openal-soft/COPYING), [GNU 정적·동적 링크 설명](https://www.gnu.org/licenses/gpl-faq.en.html#LGPLStaticVsDynamic)

배포 버전의 OpenAL 소스·수정분·빌드 설정, 앱 소스 또는 재링크 가능한 오브젝트, 연결에 필요한 자료를 같은 릴리스에서 받을 수 있도록 구성한다. 새 clone에서 라이브러리를 수정해 다시 빌드·링크하는 검사도 필요하다. 라이브러리를 이용한 부분의 수정과 그 디버깅을 위한 역분석을 배포 조건으로 금지하지 않아야 한다. 2026-10-28에 자원 포함 v0.12 압축본으로 라이브러리를 수정해 Simulator·Mac 앱에 다시 링크했고, 앱 코드에서 변경한 표식을 확인했다. [재링크 결과](github-followup-20261028.md).

추가 확인 대상은 `Compatibility/patches`, 외부 구현을 참조한 `HMCompatibilityPixels.hpp`, `.agents/skills`의 복사 원문이다. 예를 들어 touchHLE의 공식 라이선스는 MPL-2.0이므로 실제로 옮긴 코드가 있다면 해당 파일의 고지와 소스 제공 조건을 대조해야 한다. 참조 주석만으로 코드 복제 여부를 단정하지 않는다. [touchHLE 저장소](https://github.com/touchHLE/touchHLE)

## 원작 자원을 함께 배포하는 방법

현재처럼 자원이 포함된 저장소를 유지하려면 원작 자원과 추가 그림의 공개·재배포 범위를 확보하는 방법이 맞다. 추가 그림에도 원작 에셋에서 이어 붙인 부분, 다른 작품의 도색·표식, 참고 이미지의 표현이 포함됐는지 확인한다. README의 게임 캡처와 영상에도 같은 게임 그림이 들어 있다.

**Mesabloo의 공개 사례**

Mesabloo의 `hm-defense` README는 어린 시절 즐겼던 게임이 앱스토어에서 사라져 복원을 시작했다고 설명한다. 원작 스튜디오에 알렸으며 문제가 있으면 저장소를 삭제하거나 비우겠다고 적었다. 수익을 받지 않는다는 설명과 코드의 BSD-3-Clause 라이선스도 있다. 공개 `main`의 `core/assets`에는 그림·음원 파일이 포함돼 있다. README에는 스튜디오로부터 자원 공개 허락을 받았다는 내용은 적혀 있지 않다. [README](https://github.com/Mesabloo/hm-defense#additional-notes), [코드 라이선스](https://github.com/Mesabloo/hm-defense/blob/main/LICENSE), [에셋 폴더](https://github.com/Mesabloo/hm-defense/tree/main/core/assets)

나는 문제가 생기면 프로젝트 전체를 없애는 방식으로 끝내고 싶지 않다. 복원판을 무료로 제공하고 개발 기록도 유지하려 한다. 따라서 HeavyMach는 코드·에셋을 함께 공개하는 범위와 조건을 정리한다. Mesabloo의 사례는 복원 동기와 공개 구성의 참고이며, HeavyMach 에셋에 대한 이용허락 근거로 쓰지는 않는다.

필요한 문건은 원작 자원의 대상 경로·버전, GitHub 보관·공개·clone·fork, 개인 빌드·플레이, 수정본 공유 범위, 제작자 표시, 허락 기간·종료 조건을 명시한 이용허락이다. 무료 앱스토어 배포와 소개용 캡처·영상 사용은 각각 포함 여부를 정한다. 이 내용을 `ASSET-LICENSES.md`에 요약하고 허락 근거와 연결한다. 권리 확인 목록에서 각 항목의 답변 여부를 관리한다.

GitHub는 신고가 저장소 일부를 대상으로 하면 수정 기회를 줄 수 있지만, 전체를 대상으로 하면 전체 접근을 비활성화할 수 있다고 설명한다. 그러므로 “문제가 생겨도 저장소는 반드시 유지된다”는 보장을 LICENSE 문구로 만들 수는 없다. 이번 준비는 공개 대상과 허락 범위를 먼저 확인해 게임과 기록을 계속 공개할 근거를 갖추는 데 목적이 있다. [GitHub 처리 절차](https://docs.github.com/en/site-policy/content-removal-policies/dmca-takedown-policy#a-how-does-this-actually-work)

파일을 공개 대상에서 빼야 한다면 현재 폴더만 지워서는 부족하다. 과거 Git 커밋·태그·LFS에도 남는지 검사하고, 기존 비공개 복원 자료를 보존한 상태에서 공개할 범위를 별도로 준비한다. 이번 조사에서는 자원·이력·공개 범위를 변경하지 않았다.

## 플랫폼별 앱 배포

소스 이용허락과 기기 설치 방식은 별도로 준비한다. 현재 iPhone·iPad 설치본은 개발 서명 앱이다. 개발 프로파일은 앱 ID·인증서·등록 기기를 포함하므로 그대로 올린 앱이 모든 아이폰에서 설치되는 것은 아니다. 소스 공개 시에는 각자가 Xcode에서 본인 개발 팀으로 빌드하는 방법을 제공하고, 일반 이용자용 앱 배포는 별도의 배포 채널을 정한다. [Apple 개발 프로파일](https://developer.apple.com/help/account/provisioning-profiles/create-a-development-provisioning-profile/)

Mac 다운로드판은 Developer ID 서명·공증과 새 Mac에서의 실행 검사를 준비한다. Apple은 Mac App Store 밖 배포에 이 절차를 제공한다. Android는 아직 APK가 없으므로 현재 다운로드 항목에 넣을 단계가 아니다. [Apple Developer ID 배포](https://developer.apple.com/developer-id/), [현재 구현 범위](implementation-status.md)

## README 배지의 내용과 위치

배지는 선택 사항이다. SDL은 짧은 소개와 설치·라이선스 링크를 쓰며, OpenRCT2는 게임 이미지 다음 Download 절에 릴리스·개발 버전 배지를 배치한다. 모든 프로젝트가 같은 배지 구성을 써야 하는 규칙은 없다. [SDL README](https://github.com/libsdl-org/SDL/blob/main/README.md), [OpenRCT2 README](https://github.com/OpenRCT2/OpenRCT2/blob/develop/readme.md)

HeavyMach는 **제목 → 게임 소개 → 배지 → 개발 동기·대표 게임 화면 → 설치·조작 링크** 순서로 배치했다. 기술 배지4개와 코드 라이선스 배지1개를 각각 상세 문서에 연결했다.

| 배지 문구 | 연결 위치 | 표시 근거 |
|---|---|---|
| Git tag · v0.12 | v0.12 태그 또는 CHANGELOG | 실제 Git 태그. 앱 번들 1.13과 구분 |
| Platforms · iOS / iPadOS / macOS | 기능·검증 문서 | 구현된 Apple 세 플랫폼 |
| Arch · arm64 | 설치 문서 | 현재 빌드 스크립트와 검사 아키텍처 |
| Code · C++20 / Objective-C++ | 구조 문서 | 현재 소스 구성 |
| Code license · MIT | LICENSE-SCOPE.md | 새 코드 기여분의 MIT. 원작 자원·외부 코드 제외 |

OS 숫자는 README의 플랫폼 표와 설치 문서에서 나눠 쓴다.

| 플랫폼 | 프로젝트의 최소 설정 | 실제 검사 기록 | 남은 범위 |
|---|---|---|---|
| iPhone·iPad | iOS/iPadOS 15.0 | iPhone Air·iPad Air 13형 M2, 27.0.1 | 15.0부터 모든 OS에서 실행한 결과는 아님 |
| Mac | macOS 12.0 | Mac mini M4 Pro, macOS 27.0.1 | 12.0 실기기·Intel 검사 없음 |
| Android | 개발 계획 | C++ 코어 NDK 컴파일 | 앱·APK·실기기 플레이 미구현 |

최소 설정을 “검증 완료 OS” 배지로 쓰지 않는다. iPhone·iPad의 일부 표시시간 실패는 검증 문서에 유지하고, 전체 통과를 뜻하는 초록색 Tests passing 배지는 붙이지 않는다. 현재 Actions의 Branch flow는 빌드 검사가 아니므로 Build passing으로 이름을 바꾸지 않는다.

GitHub의 workflow 배지는 실제 작업 결과를 표시하며 `branch`로 기준 브랜치를 지정할 수 있다. 비공개 저장소의 배지는 외부 사이트에서 접근되지 않는다. 현재 검수 사이트에는 비공개 API 토큰 없이 표시할 수 있는 정적 배지가 적합하다. [GitHub 상태 배지](https://docs.github.com/en/actions/how-tos/monitor-workflows/add-a-status-badge)

### 적용할 Markdown 예시

아래는 최초 조사에서 비교한 Shields 형식이다. 실제 README에는 같은 문구의 저장소 내 SVG를 사용했다. 외부 이미지 서비스 없이 표시되며 링크는 버전·설치·구조·라이선스 범위로 연결된다.

```markdown
[![Git tag v0.12](https://img.shields.io/badge/Git_tag-v0.12-48664d)](https://github.com/gelb2/HeavyMach/tree/v0.12)
[![Platforms iOS iPadOS macOS](https://img.shields.io/badge/Platforms-iOS%20%7C%20iPadOS%20%7C%20macOS-526577)](docs/public/validation.md)
[![Architecture arm64](https://img.shields.io/badge/Arch-arm64-526577)](docs/public/build-and-install.md)
[![C++20 and Objective-C++](https://img.shields.io/badge/Code-C%2B%2B20%20%7C%20Objective--C%2B%2B-526577)](docs/public/architecture.md)
```

Shields의 정적 배지는 문구와 색을 URL에 넣는다. 현재는 같은 목적의 로컬 SVG5개를 사용하며 `Code license · MIT`는 적용 범위 문서로 연결된다. 릴리스 때 Git 태그 배지를 갱신하고, 실제 빌드 CI가 생기면 그 workflow의 배지를 추가한다. [Shields 정적 배지 형식](https://shields.io/badges/static-badge)

## 다음 작업

1. 코드·에셋을 함께 공개하고 누구나 clone·fork·빌드·플레이할 수 있는 범위를 기준으로 원작·추가 자원의 출처와 허락 근거를 정리한다. 무료 앱스토어 배포와 수정본 공유 조건은 별도 항목으로 확인한다.
2. 추가한 LICENSE·LICENSE-SCOPE·ASSET-LICENSES·THIRD_PARTY_NOTICES의 경로별 조건을 유지하고, 원작·외부 코드의 확인 결과를 보강한다.
3. OpenAL 소스·빌드 설정과 재링크 도구를 준비하고 Simulator·Mac 앱에서 검사를 완료했다.
4. 공개할 Git 이력·LFS·문서·미디어 범위를 확인하고, 소스 공개와 플랫폼별 앱 배포를 각각 준비한다.
5. README에 추가한 배지5개와 환경 표를 버전 변경 때 갱신한다. 실제 빌드 CI·다운로드가 준비되면 해당 배지를 추가한다.

새 코드 기여분의 MIT, 적용 범위·자원·외부 고지 문건과 README 배지를 추가했다. 저장소 공개 전환과 앱의 외부 배포는 아직 실행하지 않았다. [투두](../usability-todo.md)
