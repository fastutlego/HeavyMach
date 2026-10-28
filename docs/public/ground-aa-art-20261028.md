# 쉴카·퉁구스카 도안과 구현

<a id="integrated"></a>

## 새 파츠와 레이더 적용

차체·포탑·포신·미사일에 새 시안의 색과 질감을 적용했다. 원작 중형 전차에 맞춘 차체 폭 80·85와 짧게 줄인 포신을 유지했다. 레이더 받침은 포탑에 고정하고, 위쪽 회전부를 8방향에서 24방향으로 늘렸다. 덜컹거림·무장·미션별 배치는 그대로다.

### 아이폰·아이패드 게임 화면

아이폰에서 원작 Tank001, 쉴카, 퉁구스카, 원작 Tank030을 나란히 표시했다.

![아이폰의 원작 전차와 새 파츠 비교](media/ground-aa/integrated/iphone-comparison.png)

아이패드에서 경사면을 이동하며 사격하는 쉴카와 퉁구스카다.

![아이패드 쉴카](media/ground-aa/integrated/ipad-shilka.png)

![아이패드 퉁구스카](media/ground-aa/integrated/ipad-tunguska.png)

[쉴카 이동·레이더 회전 영상](media/ground-aa/integrated/shilka-motion.mp4) · [퉁구스카 이동·레이더 회전 영상](media/ground-aa/integrated/tunguska-motion.mp4)

### 연결한 파츠

쉴카는 차체 2장·포탑·포신, 퉁구스카는 여기에 미사일 발사관을 더해 조립했다. 고정 받침은 포탑 뒤쪽에 붙어 있다.

![쉴카 파츠](media/ground-aa/integrated/shilka-parts.png)

![퉁구스카 파츠](media/ground-aa/integrated/tunguska-parts.png)

![쉴카 레이더 24방향](media/ground-aa/integrated/shilka-radar.png)

![퉁구스카 레이더 24방향](media/ground-aa/integrated/tunguska-radar.png)

### 검사와 설치

| 대상 | 결과 |
|---|---|
| 코어 |23개 통과. 기존 배치·무장, 152좌표 착지·이동, 20무기×2차량 피격·파괴 검사|
| 아이패드 시뮬레이터 |조립2개·전투 회귀4개 통과|
| 아이폰 Air |무선 실기기 검사5개 통과. 일반·치트 앱 업데이트|
| 아이패드 Air 13형 M2 |무선 실기기 검사5개 통과. 일반·치트 앱 업데이트|

실기기 검사에서 레이더 24프레임·차체 2프레임, 사격 중 저장 0회, 무기 20종 전환·발사와 자동조준·이동·점프·터치 병용을 확인했다. 일반 앱은 업데이트 후 실행했다. 설치 전후 무선 연결도 확인했다. 두 회전 영상은 아이패드 시뮬레이터에서 촬영했다.

`feat/ground-aa-refinement`에 작업했다. develop/main 병합은 남아 있다. [구현·배치표](../ground-aa-expansion.md) · [검사 기록](../evidence/ground-aa-integrated-20261028.json) · [파츠와 연결점](../../Art/GroundAA/20261028-integrated/manifest.json) · [생성 지시문](../../Art/GroundAA/20261028-integrated/prompts.json).

---

차체·포탑·포신·발사관·레이더를 모두 새로 그렸다. 위쪽은 조립된 모습, 아래쪽은 분리할 파츠다. 원작 지상 유닛 5종의 파츠 연결점·겹침 순서·회전 설정을 확인해 조립안을 작성했다.

## 현재 에셋으로 다시 그린 두 시안

현재 게임의 해리어 v3와 F-22 파일을 생성 도구에 직접 넣었다. 실차 사진은 차체·포탑·레이더 형태를, 공중 유닛은 도장·금속 명암·표면 묘사를 참조했다. 이전 지상 시안에 색만 덧칠하는 대신 두 차량을 새로 그렸다.

### 새 쉴카

![현재 공중 유닛을 참조해 새로 그린 쉴카](media/ground-aa/runtime-style/shilka.png)

낮은 차체, 바퀴 6개, 두 쌍으로 배치한 포신 4개와 뒤쪽 원형 레이더를 그렸다. 포탑 아래는 차체 지붕과 겹치고 레이더 받침은 포탑에 붙인다. 회녹색·회색 도장과 면을 따라 달라지는 금속 명암을 적용했다.

### 새 퉁구스카

![현재 공중 유닛을 참조해 새로 그린 퉁구스카](media/ground-aa/runtime-style/tunguska.png)

첫 생성 결과가 쉴카의 긴 상자형 포탑까지 따라 그려 포탑과 무장 배치를 다시 수정했다. 앞의 원형 추적 레이더와 뒤의 탐색 레이더, 측면 발사관을 구분했다. 짧은 포신과 바퀴 6개, 붙어 있는 포탑 받침을 유지한다.

이 두 장을 바탕으로 차체·포탑·포신·레이더를 나눠 그렸다. 적용 결과는 이 문서 첫 부분에 있다.

### 생성에 사용한 현재 공중 유닛

<div class="gaa-art-pair">
<figure><img src="media/vtol/refined/VTOL_Harrier.png" data-runtime-asset="Art/Extensions/VTOL_Harrier.png" alt="이번 생성 입력인 현재 해리어 v3"><figcaption>해리어 v3 · 9a523efede9a</figcaption></figure>
<figure><img src="media/lux-lunae/F22-gold.png" data-runtime-asset="Art/Extensions/Lux_F22.png" alt="이번 생성 입력인 현재 F22"><figcaption>F-22 · 8cf228dafd36</figcaption></figure>
</div>

<div class="gaa-reference-strip">
<figure><img src="media/ground-aa/runtime-style/shilka.png" width="86" alt="작게 표시한 새 쉴카"><figcaption>쉴카</figcaption></figure>
<figure><img src="media/ground-aa/runtime-style/tunguska.png" width="90" alt="작게 표시한 새 퉁구스카"><figcaption>퉁구스카</figcaption></figure>
<figure><img src="media/vtol/refined/VTOL_Harrier.png" width="114" data-runtime-asset="Art/Extensions/VTOL_Harrier.png" alt="작게 표시한 현재 해리어"><figcaption>해리어</figcaption></figure>
</div>

[생성 입력·수정 프롬프트](../../Art/GroundAA/20261028-runtime-style/prompts.json) · [참조와 결과 파일의 해시](../../Art/GroundAA/20261028-runtime-style/manifest.json)

## GitHub main 독립 빌드 재검사

그림을 만들기 전에 GitHub의 `main` 커밋 `dc41705e`를 새 임시 폴더에 clone했다. Git LFS로 자원을 받았으며 기존 프로젝트의 파일은 복사하지 않았다. 기존 Git 작업 폴더·분석 폴더와 `.ipa` 파일 읽기를 차단한 상태에서 빌드했다. 접근 차단 검사도 실제 읽기 실패를 반환했다.

| 검사 | 결과 |
|---|---|
| GitHub에서 받은 빌드·검사 자원 | 1,482개 해시 일치 |
| 일반 앱·치트 앱 | 둘 다 새 빌드 성공 |
| 앱에 들어간 게임 자원 | 앱마다 1,478개 해시 일치 |
| 새 iPhone 시뮬레이터 | 두 앱 설치·실행 성공 |
| 코어 검사 | 21개 통과 |
| 이미지·음향·무기·확장 화면 입력 검사 | 10개 통과 |

현재 GitHub 소스와 자원으로 빌드하고 실행할 수 있다. 필요한 이미지·소리·맵은 앱의 `HeavyMachAssets.bundle`에 들어간다. 이번에 검사한 원격 `main`에는 공중 유닛 6종이 포함돼 있다. 쉴카·퉁구스카는 아직 로컬 `feat/ground-aa` 작업이다.

<details><summary>새 clone으로 만든 일반·치트 앱의 첫 실행 화면</summary>
<img src="media/ground-aa/runtime-style/HeavyMachRestored-fresh-clone.png" alt="새 clone 일반 앱 실행">
<img src="media/ground-aa/runtime-style/HeavyMachValidation-fresh-clone.png" alt="새 clone 치트 앱 실행">
</details>

[명령·커밋·검사 결과](../evidence/independence-main-20261028.json)

## 해리어 참조 오류 정정

보고서에서 ‘채택된 해리어’라고 보여준 그림은 탈락한 첫 시안이었다. 생성 도구에는 두 번째 시안을 넣었고, 현재 앱은 외곽선을 정리한 세 번째 파일을 사용한다. 세 파일을 대조하지 않아 생성 참조와 보고서 그림이 달라졌다. 아래 지상 유닛 시안은 보류했다.

| 구분 | 확인한 파일 | SHA-256 앞 12자리 |
|---|---|---|
| 보고서에 잘못 표시한 탈락 시안 | `Art/VTOL/20261027/harrier-concept.png` | `aaa54c606b50` |
| 지상 시안 생성에 실제 사용한 파일 | `Art/VTOL/20261027-v2/harrier-concept.png` | `b40e1e5b9cae` |
| 현재 앱의 해리어 | `Art/Extensions/VTOL_Harrier.png` — v3와 동일 | `9a523efede9a` |

### 현재 앱에 포함된 해리어

<img src="media/vtol/refined/VTOL_Harrier.png" data-runtime-asset="Art/Extensions/VTOL_Harrier.png" alt="현재 앱의 해리어 v3">

야크141·해리어·F35B·F22·Su57·MiG31 전체 6종에 대해 제작 원본, 런타임 자원, 보고서의 최신 이미지, 로컬 일반·치트 빌드의 파일 해시를 대조했다. 여섯 종 모두 일치했다. 이번 오류로 앱의 공중 유닛 자원은 바뀌지 않았다.

<details><summary>보고서에서 채택본으로 잘못 표시했던 탈락 시안</summary>
<img src="media/vtol/harrier-concept.png" alt="탈락한 해리어 첫 시안 — 참조 사용 금지">
</details>

최신 참조 그림에는 실제 런타임 경로를 붙였다. 두 파일의 해시가 다르면 보고서 생성을 중단한다. 생성 입력은 프롬프트와 파일 해시를 함께 기록한다. [파일별 대조 결과](../../Art/GroundAA/20261028-aircraft-style/reference-audit.json).

## 공중 유닛 화풍을 참조한 수정 시안

아래는 해리어 v2와 F-22 금색 조종석 버전을 입력해 만든 지상 시안이다. 해리어의 최신 v3 참조로 다시 검토하기 전까지 보류한다. 이전 시안에서 튀던 밝은 모서리와 판금 경계의 대비를 낮추고, 표면에 부드러운 금속 명암을 넣었다. 첫 수정에서 늘어난 잔무늬는 두 번째 수정에서 줄였다. 아래는 새 조립 시안이며 게임 파츠 교체는 아직 하지 않았다.

### 대조를 위해 바로잡은 현재 공중 유닛 이미지

<div class="gaa-art-pair">
<figure><img src="media/vtol/refined/VTOL_Harrier.png" data-runtime-asset="Art/Extensions/VTOL_Harrier.png" alt="현재 앱의 해리어 v3"><figcaption>현재 해리어 v3 — 생성 입력 v2의 외곽선 수정본</figcaption></figure>
<figure><img src="media/lux-lunae/F22-gold.png" data-runtime-asset="Art/Extensions/Lux_F22.png" alt="현재 앱과 생성 입력이 같은 F22"><figcaption>채택된 F-22 — 면을 따라 변하는 밝기·얇은 표면 묘사</figcaption></figure>
</div>

이전에는 원작 전차·헬기만 참조하고 추가 공중 유닛을 빠뜨렸다. 생성 시 해리어 v2와 F-22를 입력했지만, 보고서에는 해리어 v1을 잘못 연결했다. 전투기의 금색 조종석·문양·도색은 차량에 옮기지 않았다.

### 쉴카 수정 전후

<div class="gaa-art-pair">
<figure><img src="media/ground-aa/texture-proposal/shilka.png" alt="이전 쉴카 색 시안"><figcaption>이전 시안</figcaption></figure>
<figure><img src="media/ground-aa/aircraft-style/shilka.png" alt="공중 유닛 화풍을 참조한 쉴카"><figcaption>수정 시안</figcaption></figure>
</div>

밝은 회백색으로 둘렀던 포탑 패널·차체 점검판의 경계를 줄였다. 큰 철판 면은 밝기가 조금씩 달라지는 회색 금속으로 바꿨다. 포신·레이더·바퀴의 배치와 차량 비례는 유지했다.

### 퉁구스카 수정 전후

<div class="gaa-art-pair">
<figure><img src="media/ground-aa/texture-proposal/tunguska.png" alt="이전 퉁구스카 색 시안"><figcaption>이전 시안</figcaption></figure>
<figure><img src="media/ground-aa/aircraft-style/tunguska.png" alt="공중 유닛 화풍을 참조한 퉁구스카"><figcaption>수정 시안</figcaption></figure>
</div>

포탑과 발사관의 밝은 테두리를 누그러뜨리고 차체·레이더 표면의 명암을 연결했다. 점검판·볼트가 각각 튀어나와 보이던 대비도 낮췄다. 레이더와 발사관의 형태는 유지했다.

### 축소 비교

<div class="gaa-reference-strip">
<figure><img src="media/ground-aa/aircraft-style/shilka.png" alt="축소한 새 쉴카 시안" width="92"><figcaption>쉴카</figcaption></figure>
<figure><img src="media/ground-aa/aircraft-style/tunguska.png" alt="축소한 새 퉁구스카 시안" width="90"><figcaption>퉁구스카</figcaption></figure>
<figure><img src="media/vtol/refined/VTOL_Harrier.png" data-runtime-asset="Art/Extensions/VTOL_Harrier.png" alt="비교용 현재 해리어 v3" width="126"><figcaption>해리어</figcaption></figure>
<figure><img src="media/lux-lunae/F22-gold.png" alt="비교용 채택 F22" width="126"><figcaption>F-22</figcaption></figure>
</div>

큰 그림에서는 차량의 점검판·바퀴 테두리가 여전히 항공기보다 두드러진다. 파츠 적용 때는 축소 화면에서 이 부분이 밝은 띠로 이어지는지 확인하고, 도드라지는 선만 더 줄인다. 덜컹거림·무장·크기·장착점은 현재 값을 유지한다.

[이번 생성·수정 프롬프트](../../Art/GroundAA/20261028-aircraft-style/prompts.json) · [최종 시안과 참조 파일 기록](../../Art/GroundAA/20261028-aircraft-style/manifest.json)

## 색·질감 수정 시안

아래는 공중 유닛과의 화풍 차이가 지적된 이전 시안이다.

쉴카·퉁구스카를 원작 전차와 헬기의 회녹색·회백색 철판에 맞춰 다시 칠하는 안이다. 갈색 기운과 금색 테두리를 줄이고, 윗면·옆면·궤도의 밝기를 나눈 시안 2장을 만들었다. 이번 그림은 색·질감 제안이며 앱에 적용한 파츠는 아니다. 덜컹거림은 사용자 지시에 따라 현재 상태를 유지한다.

### 현재 화면과 원작 참조

![현재 앱의 원작 전차와 쉴카·퉁구스카](media/ground-aa/short-guns-attached.png)

현재 화면의 세 번째와 네 번째 차량이 쉴카와 퉁구스카다. 두 차량은 원작 전차보다 어둡고 갈색 기운이 강하다. 작은 긁힘과 밝은 테두리가 여러 부품에 반복돼 축소하면 표면이 복잡해 보인다. 원작 전차·헬기는 밝은 철판과 어두운 바퀴·창·접합부가 큰 덩어리로 구분된다.

<div class="gaa-reference-strip">
<figure><img src="media/ground-aa/texture-proposal/original-tank-body.png" alt="원작 전차 차체 2프레임" width="200"><figcaption>원작 전차 차체 2프레임</figcaption></figure>
<figure><img src="media/ground-aa/texture-proposal/original-heli-03.png" alt="원작 헬기 03 동체" width="99"><figcaption>원작 헬기 03 동체</figcaption></figure>
<figure><img src="media/ground-aa/texture-proposal/original-heli-06.png" alt="원작 헬기 06 동체" width="126"><figcaption>원작 헬기 06 동체</figcaption></figure>
</div>

위 참조는 원작 이미지 파일을 그대로 표시했다. 전차는 차체 그림 2장이 한 파일에 들어 있다. 헬기에서는 밝은 외판, 어두운 창과 기계 부분의 명암 차이를 참고한다.

### 제안한 쉴카

![회녹색과 밝은 철판 면으로 바꾼 쉴카 시안](media/ground-aa/texture-proposal/shilka.png)

갈색 도장을 회녹색으로 바꾸고 포탑 윗면과 차체 앞쪽 경사면을 밝게 했다. 궤도는 짙은 회색으로 묶었다. 작은 흠집 대신 큰 철판 경계와 짧은 마모 자국으로 형태를 구분한다. 작은 적갈색 패널은 원작 전차의 측면 색을 참고한 추가 제안이다.

### 제안한 퉁구스카

![같은 색과 명암으로 정리한 퉁구스카 시안](media/ground-aa/texture-proposal/tunguska.png)

쉴카와 같은 도장·광원을 적용했다. 원형 레이더, 미사일 발사관, 포탑 옆면의 밝기를 나눠 작은 화면에서도 부품이 구분되게 했다. 궤도와 포탑 밑은 어둡게 두고, 전면 레이더와 발사관 위쪽에는 넓은 밝은 면을 남겼다.

### 작게 표시했을 때

<div class="gaa-reference-strip gaa-proposal-small">
<figure><img src="media/ground-aa/texture-proposal/shilka.png" alt="축소한 쉴카 색 시안" width="92"><figcaption>쉴카 축소 시안</figcaption></figure>
<figure><img src="media/ground-aa/texture-proposal/tunguska.png" alt="축소한 퉁구스카 색 시안" width="90"><figcaption>퉁구스카 축소 시안</figcaption></figure>
<figure><img src="media/ground-aa/texture-proposal/original-heli-03.png" alt="같이 비교할 원작 헬기 동체" width="99"><figcaption>원작 헬기 동체</figcaption></figure>
</div>

시안의 차체가 약 80·85 CSS 픽셀이 되도록 축소했다. 파츠에 적용할 때는 실제 치트 앱의 같은 장면에서 다시 비교한다. 큰 그림에서 좋아 보이는 흠집도 이 크기에서 부품을 흐리게 하면 줄인다.

### 적용할 변경

| 항목 | 변경 방법 | 확인할 모습 |
|---|---|---|
| 도장 | 갈색·금색을 줄이고 회녹색을 기본색으로 사용 | 원작 전차·헬기 옆에서 두 차량만 누렇게 보이지 않음 |
| 명암 | 윗면은 회백색, 옆면은 회녹색, 궤도·접합부는 짙은 회색 | 차체·포탑·무장이 작은 화면에서도 구분됨 |
| 표면 | 작은 긁힘·리벳의 대비를 줄이고 큰 철판 면을 유지 | 축소했을 때 점무늬나 얼룩으로 뭉치지 않음 |
| 외곽선 | 전체를 두른 밝은 테두리를 줄이고 빛을 받는 모서리에만 짧게 표시 | 검거나 금빛인 띠가 유닛을 둘러싸지 않음 |
| 파츠 경계 | 각 부품의 광원을 통일하고 맞닿는 면에만 좁은 그림자 추가 | 포탑·레이더가 같은 차량의 부품으로 보임 |
| 작은 색 강조 | 쉴카 포탑·퉁구스카 차체에 작은 적갈색 패널 | 두 차량에 공통 도장 특징을 주되 무장보다 튀지 않음 |

게임 화면 크기에 맞춰 세부 묘사를 고르고, 복잡한 부분 옆에 단순한 면을 두는 방법은 Valve의 [캐릭터 아트 가이드](https://help.steampowered.com/en/faqs/view/0688-7692-4D5A-1935)에서도 제시한다. 여기서는 원작 에셋을 색·화풍 기준으로 삼고 이 방법을 적용했다.

### 파츠 교체 순서와 추가 제안

1. 현재 11개 파츠의 외곽 형태·투명 영역·프레임 배치를 고정한 채 색과 명암을 수정한다. 차체 폭 80·85, 포 부품 폭 32·37과 장착점은 유지한다. 조립 시안을 한 장짜리 게임 유닛으로 교체하지 않는다.
2. 같은 색 기준을 차체·포탑·포·발사관·레이더 전체에 적용한다. 레이더는 방향마다 다른 색이 번쩍이지 않게 같은 광원에서 만든다. 고정 받침과 24방향 회전도 함께 적용했다.
3. 현재 차체 2프레임의 자세와 움직임을 보존한다. 전진·후진·정지·경사에서 색과 연결면만 비교한다.
4. 밝은 하늘과 어두운 구조물 앞에서 원작 전차·헬기와 나란히 검사한다. 포신과 레이더가 배경에 묻히면 해당 면의 명암부터 조정한다.
5. 실제 게임 크기에서 외곽선과 부품이 읽히는 것을 확인한 뒤 아이폰·아이패드 치트 앱에서 조준·연사·피격을 확인한다.

추천하는 조합은 **회녹색 도장 + 넓은 명암 면 + 적은 마모 자국 + 작은 적갈색 패널**이다.

[생성 프롬프트](../../Art/GroundAA/20261028-texture-proposal/prompts.json) · [시안·원작 참조 파일과 해시](../../Art/GroundAA/20261028-texture-proposal/manifest.json)

### 함께 기록한 버그

알라미 같은 알람 앱으로 인해 게임이 백그라운드로 갔다 돌아오면 소리가 나지 않는다는 사용자 보고를 `HM-AUDIO-ALARM-RETURN`에 등록했다. 외부 오디오 중단과 게임 복귀 순서, 오디오 세션·OpenAL 출력 복구를 조사할 예정이다. 아직 재현·수정 전이다.

## 포신 단축·접합부 재수정과 외형 개선 계획

![포신과 연결점을 다시 수정한 쉴카·퉁구스카](media/ground-aa/short-guns-attached.png)

포 부품 전체 길이를 쉴카 38→32, 퉁구스카 40→37로 줄였다. 차체 폭 80·85는 유지하고 포신 높이·회전축·포구를 함께 조정했다. 쉴카의 포탑은 차체와 4픽셀 겹치게 내렸다.

레이더 장착 위치에서 포탑의 실제 표면 높이는 쉴카3.46·퉁구스카3.74다. 기존 장착점은 각각7·10으로 잡혀 받침 아래가 떠 있었다. 두 장착점을2로 내려 받침이 포탑 표면 안으로 약1.4·1.7픽셀 들어가도록 했다. 레이더 회전 프레임8장의 받침 위치와 평지·경사에서의 연결을 검사했다.

### 실차 길이와 대조

쉴카의 차체 길이는 박물관 자료에6,495mm로 나온다. 2A7/2A14용 포신 제조사 카탈로그의 길이 상한은1,900mm다. 차체 폭80에 적용하면 포신 부분은 약23.4픽셀이다. 이전 그림에서 고정부 앞의 관 부분을 읽으면 약27.4픽셀이었다. 포 부품 폭을32로 줄이면 같은 부분이 약23.1픽셀이 된다. 관과 고정부의 경계는 그림에서 읽은 근삿값이다. [박물관 제원·사진](https://www.kskdivniy.ru/museum/eksponaty/zsu-23-4-shilka/) · [제조사 카탈로그 40–41쪽](https://docs.tuyap.online/FDOCS/90009.pdf#page=21).

퉁구스카의 1991년 교범은 차량 길이7,880mm, 2A38 기관포 전체 길이3,434mm를 제시한다. 차체 폭85에 적용하면 기관포 전체는 약37픽셀이다. 이 값으로 현재40을37로 줄였다. 3,434mm에는 기관부가 포함된다. [교범 11·160쪽](https://ukr.bulletpicker.com/pdf/%D0%A3%D1%87%D0%B5%D0%B1%D0%BD%D0%B8%D0%BA%20%D0%97%D0%A0%D0%9F%D0%9A%20%D0%A2%D1%83%D0%BD%D0%B3%D1%83%D1%81%D0%BA%D0%B0%20%281991%29.pdf#page=161).

### 덜컹거림 조사

평지에서 같은 속도±0.6으로120프레임씩 움직여 비교했다. 원작 Tank001·Tank030·E0505는 차체 좌표의 추가 상하 이동이0이고 차체 그림2장을 바꾼다. 새 차량은 상하0~1.35픽셀 이동과 그림2장 전환을 한다. 차체와 포탑의 상하 이동량이 같아 둘 사이의 흔들림은 없다. 원작처럼 보이려면 전체 차량을 흔드는 것 외에 차체 그림의 자세 차이가 필요하다.

현재 새 차체는 각 프레임을 개별 영역으로 잘라 같은 크기로 맞춘다. 프레임 사이 자세 차이도 작아 궤도·차체가 눌렸다 돌아오는 모습이 약하다. 사용자가 이 조사 후 덜컹거림을 현재 수준으로 유지하도록 지정했다. 눌림·복원 자세 재제작은 취소했다.

### 레이더 회전 개선 계획

수정 전에는 2초 동안 8장을 보여줬다. 0.25초마다 45도씩 건너뛰었고, 그림마다 본체의 두께와 앞뒤 형태도 달랐다. 이 문제를 줄이려고 회전부를 24방향으로 다시 그렸다.

레이더의 받침은 고정 파츠로 분리한다. 회전하는 본체만 단순한3D 형상으로 만들고, 같은 카메라·조명·재질에서15도 간격24방향을 그린다. 설치된 Blender로 제작할 수 있다. 게임에는2D 스프라이트만 넣고12fps로 전환해 현재2초 회전 주기를 유지한다. 실제 표시 크기에서 형태가 튀는지, 마지막 프레임에서 첫 프레임으로 자연스럽게 이어지는지 확인한다.

### 질감 개선 계획

새 차량은 갈색·금색 테두리와 작은 긁힘이 많다. 원작 전차는 회색·올리브색 철판, 넓은 명암 면과 짧은 밝은 선으로 형태를 구분한다. 고해상도 그림을 줄이는 것만으로 이 차이가 없어지지 않았다.

1. Tank001·Tank030·E0505를 같은 크기와 배경에 놓고 색·외곽선·밝은 면을 기준으로 정한다.
2. 쉴카부터 차체·포탑·포·레이더의 광원과 색을 통일한다. 황금색 테두리와 미세한 긁힘을 줄이고 원작의 철판 면·굵은 명암 구분을 따라 다시 그린다. 실차의 비례와 부품 수는 유지한다.
3. 실제80픽셀 차체 크기에서 원작 전차와 비교한다. 포탑 접합면, 레이더 받침과 포 축도 함께 확인한 뒤 퉁구스카에 적용한다.
4. 차체 2프레임의 현재 자세·접지점·전환 간격과 덜컹거림은 유지한다. 색·질감을 고친 뒤에도 연결면이 움직이는 동안 겹치는지 확인한다.
5. 치트 앱에서 평지 좌우 이동·방향 전환·경사·조준·연사·정지/재개를 녹화한다. 정지 캡처와 반복 재생을 함께 보고 외형을 판단한다.

이전 수정에서는 포신 단축과 연결점 수정을 반영했다. 차체 자세 재작업은 사용자 지시로 취소했다. 당시 남겨둔 레이더 24방향과 질감 재작업은 이 문서 첫 부분의 수정본에 적용했다. 코어 검사 2개와 iPad 시뮬레이터·iPhone·iPad 실기기 검사 2개씩 통과했다. 두 기기의 일반·치트 앱을 업데이트했고 일반 앱도 실행했다.

## 크기·연결부·레이더·포신 수정

![원작 전차와 쉴카·퉁구스카 크기 비교](media/ground-aa/medium-comparison.png)

왼쪽부터 플레이어, 원작 Tank001, 쉴카, 퉁구스카, 원작 Tank030이다. 평지 검사 장면에서 같은 배율로 그렸다. 차체 폭을 쉴카 104→80, 퉁구스카 112→85로 줄였다. 기준인 Tank001의 차체 한 프레임은 85×30이다. 두 차량의 포탑·포신·레이더도 함께 줄였다.

아래는 첫 연결점 수정 기록이다. 이후 실제 플레이에서 쉴카의 틈이 다시 지적돼 위와 같이 추가 수정했다. 포탑과 차체 사이의 틈은 그림 사각형의 끝을 연결점으로 사용하면서 생겼다. 차체 지붕의 투명 영역을 고려하지 않아 포탑이 떠 보였다. 실제 접합면이 2픽셀 겹치도록 연결점을 내렸다.

![크기를 줄인 퉁구스카의 경사면 사격](media/ground-aa/tunguska-medium-slope.png)

![크기를 줄인 쉴카의 경사 이동](media/ground-aa/shilka-medium-slope.png)

포신과 레이더도 실차 사진을 다시 대조해 교체했다. 쉴카의 네 포신은 좌우 두 쌍으로 배치하고 굵은 포구를 줄였다. 레이더는 짧은 받침 위의 두꺼운 원형 본체로 바꿨다. 퉁구스카는 가는 기관포 두 관과 냉각부를 구분하고, 후방 탐색 레이더의 높은 받침과 위쪽 돌출물을 없앴다. 회전축은 프레임마다 같은 장착점에 놓았다.

![수정한 쉴카 포신](media/ground-aa/shilka-gun-v2.png)

![수정한 쉴카 레이더 8방향](media/ground-aa/shilka-radar-v2.png)

![수정한 퉁구스카 포신](media/ground-aa/tunguska-gun-v2.png)

![수정한 퉁구스카 탐색 레이더 8방향](media/ground-aa/tunguska-radar-v2.png)

![iPad 실기기의 원작 전차·쉴카·퉁구스카 비교](media/ground-aa/medium-ipad.png)

수정본은 코어 23개 검사와 iPad 시뮬레이터·Mac·iPhone·iPad 실기기의 조립·전투 검사 2개씩을 통과했다. iPhone·iPad의 일반·치트 앱을 수정본으로 업데이트했다.

[수정 프롬프트](../../Art/GroundAA/20261028-refinement/prompts.json) · [교체 전후 파일 기록](../../Art/GroundAA/20261028-refinement/changes.json). 아래 도안과 초기 게임 캡처는 수정 전 기록이다.

## 쉴카

![쉴카 조립도와 차체·포탑·4연장 기관포·레이더 분해도](media/ground-aa/shilka-concept-v1.png)

낮고 긴 궤도 차체, 네 포신, 뒤쪽 원형 레이더를 유지했다. 분해도 A의 큰 바퀴 수를 조립도와 같은 6개로 수정했고, 처음 생성된 불필요한 차체 앞 총구를 삭제했다. 흰색 총탄 6발을 연속 발사하도록 구현했다.

| 파츠 | 연결 | 움직임 |
|---|---|---|
| A 차체·궤도 | 지면 기준 루트 | 전진·후진, 경사에 따른 기울기, 궤도 프레임 전환 |
| B 포탑 몸체 | A 위의 포탑 연결점 | 차체 기울기를 따르고 원작과 같은 파츠 흔들림 적용 |
| C 기관포 묶음 | B 앞쪽 회전축 | 표적 높이에 따른 상하 조준, 발사 반동 |
| D 레이더 | B 뒤쪽 장착점 | 포탑 장착점에서 독립적으로 회전 |

네 포신의 그림과 기관포 무기 1종을 연결한다. 파츠 C를 회전시키고 포구 위치도 함께 변환한다.

## 퉁구스카

![퉁구스카 조립도와 차체·포탑·기관포·미사일 발사관·레이더 분해도](media/ground-aa/tunguska-concept-v1.png)

긴 궤도 차체 위에 기관포와 미사일 발사관을 배치했다. 앞쪽 원형 추적 레이더와 뒤쪽 탐색 레이더를 구분했다. 분해도 D의 발사관을 3개에서 4개로 수정했다. 흰색 총탄 4발과 미사일 2발을 각각 연속 발사하도록 구현했다.

| 파츠 | 연결 | 움직임 |
|---|---|---|
| A 차체·궤도 | 지면 기준 루트 | 전진·후진, 경사에 따른 기울기, 궤도 프레임 전환 |
| B 포탑·전방 레이더 | A 위의 포탑 연결점 | 차체를 따라 이동·기울어짐 |
| C 기관포 | B의 포 장착점 | 상하 조준·발사 반동 |
| D 미사일 발사관 | B의 별도 무기 장착점 | 기관포와 별도의 조준 상태·발사점 사용 |
| E 탐색 레이더 | B 뒤쪽 연결점 | 포탑 장착점에서 독립적으로 회전 |

조립도는 측면에서 보이는 무장과 파츠를 중심으로 그렸다. 차체·포탑·레이더·기관포·발사관의 겹침 순서를 파츠 정의에 지정했다.

## 확인한 원작 조립 구조

`MachInfo`의 부모·자식 연결, `PartsInfo`의 소켓과 타일 수, `WeaponInfo`의 회전 설정을 함께 읽었다. `Tank001`, `Tank030`, `E0505`, `MBoss0501`, `MBoss0601`의 결과를 [조립 데이터](../evidence/ground-original-assembly-20261028.json)에 저장했다.

| 원작 유닛 | 실제 연결 | 새 차량에 적용할 구조 |
|---|---|---|
| Tank001 | 차체 소켓1 → 포탑, 포탑 소켓1 → 포신 | 쉴카의 차체·포탑·기관포 연결 |
| Tank030 | 같은 3단 연결, 차체·포탑·포신의 중심점은 다른 값 | 새 그림의 크기에 맞춰 연결점 지정 |
| E0505 | 2프레임 차체 소켓1 → 회전 가능한 무기 | 궤도 변화와 포신 회전을 별도로 처리 |
| MBoss0501 | 차체 → 포탑, 포탑 소켓2 → 포, 소켓1 → 미사일 | 퉁구스카의 무기별 독립 연결·조준·발사 |
| MBoss0601 | 차체 → 포탑 → 포·미사일·박격포 | 여러 무기 상태를 따로 갱신하는 기존 구조 참고 |

위 예제의 포·미사일에는 `rotWpn = 1`이 설정되어 있다. `refreshWeaponAim`은 목표 각도 차이의 절반씩 현재 각도를 바꾸고, `refreshWeaponVisual`은 포신의 회전축과 위치를 계산한다. 발사점은 파츠의 소켓0·소켓1에서 계산한다. 새 파츠도 이 경로에 연결한다.

## 덜컹거림과 파츠 조립

궤도는 이동 거리에 따라 그림 프레임을 바꾸고, 차체는 지면 경사를 따라 기울어진다. 포신은 차체와 별도로 조준하고 발사 반동을 받는다. 새 차체·포탑·포신을 각각 그린 이유도 이 움직임을 따로 연결하기 위해서다.

새 차량에 기존 `useEngineBounce`를 켜 차체·포탑과 포신이 서로 다른 폭으로 흔들리게 했다. 원작 적의 설정은 유지했다. 기존 지상 적과 나란히 놓은 치트 앱에서 경사 이동·접지·포신 회전·반동을 확인했다.

## 제작 자료

- 형태 참조: [쉴카 실차 사진](https://www.kskdivniy.ru/museum/eksponaty/zsu-23-4-shilka/), [퉁구스카 실차 사진](https://www.kskdivniy.ru/museum/eksponaty/tunguska/).
- 화풍 참조: 원작 `E_0505_Body.png`, `E_Tank_Body_03.png`의 올리브·회색·갈색 철판과 명암.
- 제작: 내장 `image_gen`. [프롬프트](../../Art/GroundAA/20261028-v1/prompts.json) · [파일·해시·수정 기록](../../Art/GroundAA/20261028-v1/manifest.json).
- [개발 조사와 남은 TODO](ground-units-research-20261028.md).

## 초기 게임 화면과 무장

![경사를 따라 이동하는 쉴카와 기존 지상 적](media/ground-aa/shilka-game-240.png)

쉴카는 네 포신으로 흰색 총탄 6발을 연사한다. 탄환 사이 간격은 0.2초다. 뒤쪽 원형 레이더는 포신과 별도로 2초에 한 바퀴 돈다.

![기관포와 미사일을 발사하는 퉁구스카](media/ground-aa/tunguska-game-120.png)

퉁구스카는 흰색 총탄 4발을 0.2초 간격으로, 미사일 2발을 0.5초 간격으로 발사한다. 두 무기는 원작의 별도 무기 상태와 발사점을 사용한다. 전방 원형 레이더는 포탑에 고정했고 후방 사각 레이더가 회전한다.

![경사에서 차체가 기울어진 퉁구스카](media/ground-aa/tunguska-game-240.png)

차체·포탑·포신·발사관·레이더를 투명 파츠로 나눴다. 궤도 2프레임과 레이더 8방향을 사용한다. 원작의 지상 이동·지형 충돌·무기 조준·피격·폭발 코드를 연결했다.

## 미션별 수량

원작 Tank 계열은 7개 미션 패키지에 모두 등장한다. 패키지별 등장 수의 약 1.3배를 두 신규 유닛의 합계로 추가했다. 35개 서브미션에 적어도 한 대씩 들어가며, 패키지마다 두 종류를 모두 볼 수 있다.

| 미션 | 원작 Tank 계열 | 쉴카 | 퉁구스카 | 추가 합계 |
|---|---:|---:|---:|---:|
| 1 | 4 | 3 | 2 | 5 |
| 2 | 31 | 20 | 20 | 40 |
| 3 | 4 | 3 | 2 | 5 |
| 4 | 21 | 13 | 14 | 27 |
| 5 | 26 | 17 | 17 | 34 |
| 6 | 11 | 7 | 7 | 14 |
| 7 | 21 | 14 | 13 | 27 |

원작 적 1,182항목, VTOL 113항목과 기존 전투기 증원을 유지했다. 추가 차량 152대는 실제 착지 코드로 확인한 지점에 배치했다. [서브미션별 수량·기술 설명](../ground-aa-expansion.md).

## 구현 검사 결과

코어 테스트 23개를 모두 통과했다. 추가 위치 152곳의 착지·좌우 이동, 두 차량에 대한 플레이어 20무기의 충돌·피해·파괴 40조건을 확인했다. iPhone Air·iPad Pro 13형 시뮬레이터의 치트 앱에서 레이더 8방향·궤도 2프레임·사격·일시정지와 저장 호출을 검사했다. 발사로 인한 저장 호출은 0회였다.

검사 중 지면 높이 조회와 실제 낙하가 다른 지점을 찾았다. 실제 낙하 코드를 120회 실행해 안정적으로 착지하는 좌표만 배치 후보로 채택하도록 수정했다. 기존 맵과 이동 코드는 유지했다.

## 초기 실기기 검사

![iPhone Air에서 경사를 오르는 쉴카](media/ground-aa/shilka-iphone.png)

iPhone Air의 쉴카. 독립 레이더 회전과 포신 조준, 경사 이동을 검사했다.

![iPad Air에서 기관포와 미사일을 발사하는 퉁구스카](media/ground-aa/tunguska-ipad.png)

iPad Air 13형의 퉁구스카. 두 무기가 각자의 발사점에서 공격한다.

Mac·iPhone Air·iPad Air 13형 치트 앱에서 신규 유닛, 20종 무기, 자동조준, 키보드와 터치, 기존 전투기 증원 검사 5개씩 통과했다. iPhone·iPad의 일반·치트 앱은 무선으로 업데이트 설치했다. 게임 시작 전에 새 파츠를 GPU에 준비하며 레이더 방향 전환 때 이미지 파일을 읽지 않는다. 전체 캠페인 난이도와 장시간 발열 검사는 후속 과제다.
