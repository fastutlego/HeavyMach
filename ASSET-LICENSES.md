# 그림·음원·게임 데이터의 출처와 이용 조건

코드와 에셋을 함께 제공한다. 저장소를 받아 빌드하고 플레이하는 방법은 빌드 문서에 정리했다. 아래 자료에는 루트의 코드용 MIT가 적용되지 않는다.

## 자원별 상태

| 자료·경로 | 출처 | 현재 이용 조건·확인할 사항 |
|---|---|---|
| `Resources/OriginalImages`, `Resources/Images`의 원작 그림, `Resources/Audio` | HeavyMach 원작 | 원작 권리 유지. 이 복원판에 포함한 GitHub 공개·복제·빌드·플레이 허락 문건은 아직 연결되지 않음 |
| `Resources/RawData`, `Resources/Data`의 원작 유래 맵·게임 데이터 | 원작 데이터와 변환 결과 | 원작 자료와 새 구현을 구분하며, 공개할 파일·버전의 범위 확인 필요 |
| `Resources/Assets.xcassets`, 플랫폼별 아이콘 | 원작 그림을 포함한 앱 아이콘 | 코드용 MIT에서 제외. 원작 그림 이용 범위에 함께 포함 |
| `Art/Extensions`, 메뉴 확장 그림 | 기존 그림을 참조·연장해 제작 | 원작에 이어지는 표현을 포함. 제작 기록과 원작 자원 허락 범위를 함께 확인 |
| `Art/VTOL`, `Art/LuxLunae`, `Art/GroundAA`, `Art/AutoAim`, `Art/Candidates` 및 런타임 파생 그림 | imagegen 생성·수정, 실물·게임 그림·도색 참조 | 프롬프트·참조 출처 보존. 원작·타 작품의 표현과 표식이 포함된 부분의 이용 범위 확인 필요 |
| `docs/public/media`의 게임 화면·GIF·MP4, 비교·검사 캡처 | 직접 플레이 녹화와 앱 검사에서 추출 | 게임 그림을 포함. GitHub·소개 글·영상에서의 게시 범위를 원작 자원 허락에 포함 |
| `docs/public/media/badges/*.svg` | 이번 문서 작업에서 작성 | 원작 그림을 사용하지 않은 SVG 코드. [MIT](LICENSE) 적용 |

원작 개발자는 변해준, 일러스트레이터는 박재철이다. 2011년에는 위메이드 엔터테인먼트의 자회사 위메이드 크리에이티브가 HeavyMach·HeavyMach 2의 IP를 포함한 인디앱스 자산을 인수했다. [당시 인수 보도](https://zdnet.co.kr/view/?no=20110512162801). 이후 권리 이전과 현재 이용허락 창구는 확인되지 않았다. [제작자](CREDITS.md), [빌드 자원과 SHA-256](docs/build-resources.json), [그림 제작 기록](docs/public/media/README.md).

## 확보할 공개 범위

- 코드와 에셋을 같은 GitHub 저장소·이력·LFS에 보관하고 공개한다.
- 방문자가 clone·fork·다운로드한 뒤 자신의 기기에서 빌드하고 플레이한다.
- 수정본을 공유할 수 있는 범위와 제작자 표시 방법을 정한다.
- 개발 과정 소개에 게임 캡처·GIF·영상을 사용한다.
- 앱스토어 무료 배포는 별도 허락 항목으로 둔다.

원작 에셋을 다른 게임에 사용하거나 단독 판매하는 권리를 이 문서에서 부여하지 않는다. 답변을 받으면 대상 파일·버전, 허락한 주체, 허락 일자·기간·조건과 근거를 이 문서에 연결한다. [공개·배포 준비](docs/public/github-licenses-and-badges-20261028.md). 경로별 상태는 [공개 범위 목록](docs/rights-manifest.json)에 있다.
