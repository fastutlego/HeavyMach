# 수직이착륙 공중 적 3종 — 그림·배치·설치 결과

Codex는 실제 기체 사진과 야크141 도면을 참조해 **야크141·해리어 GR3·F35B의 두 번째 시안**을 만들었다. 길이와 높이의 비례, 기수·흡입구·날개·꼬리 형태를 실물에 맞춰 다시 그렸다. 기존 게임 그림은 색조·질감·명암의 참고로 사용했다. 기체는 투명 PNG이며 불꽃은 넣지 않았다. 내가 채택한 그림을 Codex가 게임에 적용했다. 이후 외곽선·크기·불꽃을 수정했다. 최신 기기 캡처와 설치 결과는 마지막 절에 있다.

**첫 시안과 수정 지시**

나는 실제 야크·해리어·F35를 지정하고 기존 유닛의 색조·질감을 따르도록 했다. Codex는 첫 프롬프트에 실물에서 영감을 받은 기체와 과장된 장갑 패널 등의 조건을 넣었다. 참조에는 기존 게임 그림만 있었고 실물 사진·도면은 없었다. 내가 요청하지 않은 외형 재해석이었다.

나는 실제 모델과 다른 이유를 물었고, 사실적인 그림을 위한 별도 스킬이 필요한지도 물었다. Codex는 기존 `integrate-game-art` 스킬을 보완해 실물 형상을 보존하고 사진·도면과 대조하도록 했다. 두 번째 시안을 채택하면서 나는 앞으로 현실의 유닛을 추가할 때 이 절차를 쓰라고 지시했다. 기존 게임 그림은 화풍 참조로, 실물 사진·도면은 형태 참조로 구분하는 규칙이 여기에 포함됐다. [첫 프롬프트](https://github.com/gelb2/HeavyMach/blob/docs/github-publication/Art/VTOL/20261027/manifest.json), [수정 프롬프트](https://github.com/gelb2/HeavyMach/blob/docs/github-publication/Art/VTOL/20261027-v2/prompts.json), [트러블슈팅](troubleshooting.md)

## 기체 시안

### 야크141 — 긴 동체·각진 흡입구·두 수직 꼬리날개

실물 사진과 삼면도에서 긴 동체, 낮은 조종석, 측면 흡입구와 높은 주익의 위치를 확인했다. 첫 시안의 두꺼운 장갑판과 갈색 도장을 빼고 회색 금속 표면으로 다시 그렸다.

<figure class="vtol-art"><a href="media/vtol/v2/yak141-concept.png"><img src="media/vtol/v2/yak141-concept.png" alt="실물 형상을 참조해 다시 그린 회색 야크141 측면 시안" style="width:100%;height:auto"></a><figcaption>야크141 · 두 번째 시안 · 클릭하면 투명 PNG 원본</figcaption></figure>

### 해리어 GR3 — 긴 기수·둥근 흡입구·단일 수직 꼬리날개

GR3 사진의 기수, 큰 흡입구와 낮게 내려가는 주익을 참고했다. 올리브색과 짙은 회색 위장 도장을 사용했다. 생성본의 중복 배기구를 수정해 보이는 측면에 앞·뒤 두 노즐을 남겼다.

<figure class="vtol-art"><a href="media/vtol/v2/harrier-concept.png"><img src="media/vtol/v2/harrier-concept.png" alt="실물 형상을 참조해 다시 그린 올리브색 해리어 GR3 측면 시안" style="width:100%;height:auto"></a><figcaption>해리어 GR3 · 두 번째 시안 · 클릭하면 투명 PNG 원본</figcaption></figure>

### F35B — 각진 기수·흡입구·두 수직 꼬리날개

미 해병대 F35B 측면 사진을 참조했다. 기수에서 동체로 이어지는 윤곽, 조종석, 각진 흡입구와 꼬리 배치를 다시 그렸다. 랜딩기어와 리프트팬 덮개를 닫은 기체 시안이다. 게임에서는 이 기체에 별도 배기 효과를 붙였다. 덮개가 열리는 애니메이션은 구현하지 않았다.

<figure class="vtol-art"><a href="media/vtol/v2/f35b-concept.png"><img src="media/vtol/v2/f35b-concept.png" alt="실물 형상을 참조해 다시 그린 회색 F35B 측면 시안" style="width:100%;height:auto"></a><figcaption>F35B · 두 번째 시안 · 클릭하면 투명 PNG 원본</figcaption></figure>

## 기존 유닛과 작은 크기 비교

첫 적용 크기의 축소 시안이다. 당시 폭은 야크140·해리어132·F35B148이었다. 후속 지시로 현재 폭은112·108·116이다.

<div class="vtol-small" style="display:flex;flex-wrap:wrap;align-items:end;gap:24px;padding:20px;background:#34362e;color:#f3efe3"><figure><img src="media/vtol/E_Heli_Body_01.png" alt="기존 Heli001 몸체" style="width:100px;height:auto"><figcaption>기존 Heli001</figcaption></figure><figure><img src="media/vtol/E_Heli_Body_06.png" alt="기존 Heli006 몸체" style="width:121px;height:auto"><figcaption>기존 Heli006</figcaption></figure><figure><img src="media/vtol/E_0604_Body.png" alt="기존 E0604 헬기 몸체" style="width:140px;height:auto"><figcaption>기존 E0604</figcaption></figure><figure><img src="media/vtol/v2/yak141-concept.png" alt="작게 표시한 야크" style="width:140px;height:auto"><figcaption>야크141 / 140</figcaption></figure><figure><img src="media/vtol/v2/harrier-concept.png" alt="작게 표시한 해리어" style="width:132px;height:auto"><figcaption>해리어 / 132</figcaption></figure><figure><img src="media/vtol/v2/f35b-concept.png" alt="작게 표시한 F35B" style="width:148px;height:auto"><figcaption>F35B / 148</figcaption></figure></div>

## 무장 차이

일반 Heli001~010은 모두 같은 단발 미사일을 사용한다. 추가 조사에서 회전날개가 있는 헬기 보스와E0604의 다른 무기를 확인했다. 다음 세 무기 정의와 탄환 계산을 재사용한다.

| 기체 | 무장·재사용 정의 | 기존 사용 유닛 | 발당 피해 / 한 번에 준비하는 탄수 | 차이 |
|---|---|---|---|---|
| 야크141 | 단발 미사일 `E_Wpn_ASM` | Heli001~010 | 25 / 1발 | 낮은 위력의 단발 공격. 미션2부터 등장 |
| 해리어 | 2발 기관포 `E_Boss_Wpn_01` | 헬기 보스Boss04·Boss06 | 50 / 2발 | 미사일과 다른 기관포 탄환. 미션3부터 등장 |
| F35B | 2발 미사일 `E_0604_Wpn` | 후반 헬기E0604 | 100 / 2발 | 기존 후반 헬기의 강한 연속 공격. 미션6부터 등장 |

표의 피해·탄수는 실제 원본 데이터 값이다. 세 정의의 delay는 모두5다. 실제 발사 간격에는 AI 조준 대기와 재장전 시간이 더해진다. 2발 묶음은 차례로 발사하며, type6 미사일은 발사 방향으로 비행한다. HP는 야크210·해리어400·F35B900이다. 추가 밀도에 따른 전체 미션 난이도는 후속 플레이 검사에 남겼다.

## 미션별 추가 배치

**기존 적을 모두 유지하고 아래14개 서브미션에 일반 헬기 평균을 기준으로 추가한다.** 내 수량 수정 지시를 반영해 최초의 각1대안을 바꿨다. 적용 수량은 야크30대·해리어41대·F35B42대, 총113대다. 보스 플래그는 붙이지 않는다. 미션1은 원래 구성으로 유지한다. 후반 미션에도 초기형 기체가 함께 등장한다.

| 기체 | 최초 등장 | 추가한 미션·서브미션 |
|---|---|---|
| 야크141 | 2-2 | 2-2, 2-4, 3-2, 4-2, 5-2 |
| 해리어 | 3-3 | 3-3, 4-3, 5-3, 6-3, 7-2 |
| F35B | 6-2 | 6-2, 6-4, 7-3, 7-4 |

### 헬기 수와 추가 수량 기준

41개 스테이지의 원래 적 정의를 읽었다. 이름이Heli로 시작하는 유닛만 세지 않고, 재귀 부품에 헬기 회전날개가 있는E0603·E0604 등도 포함했다. 보스 플래그가 있는 헬기는 별도로 세고 평균에서 제외했다. 회전날개 없는 항공기는 제외했다. 각 미션의 모든 서브미션을 분모로 쓰며0대인 스테이지도 포함한다.

`추가 수량 = 미션 내 일반 헬기 총수 ÷ 서브미션 수`를 소수 첫째 자리에서 반올림한다. 기존에 선택한 서브미션마다 해당 수량을 더한다. 추가 기체는 스테이지 안의 여러 지점에 나누어 배치한다.

| 미션 | 스테이지별 일반 헬기 수 | 합계 / 스테이지 수 | 평균 | 선택한 스테이지당 추가 수량 |
|---|---|---|---|---|
| 1 | 2, 2, 4, 3 | 11 / 4 | 2.75 | 이번 배치 제외 |
| 2 | 7, 4, 7, 3, 5 | 26 / 5 | 5.20 | 5 |
| 3 | 6, 7, 8, 13, 3 | 37 / 5 | 7.40 | 7 |
| 4 | 5, 3, 4, 3, 4 | 19 / 5 | 3.80 | 4 |
| 5 | 9, 9, 13, 9, 5 | 45 / 5 | 9.00 | 9 |
| 6 | 14, 12, 10, 10, 5 | 51 / 5 | 10.20 | 10 |
| 7 | 8, 19, 8, 6, 17, 6 | 64 / 6 | 10.67 | 11 |
| 8 | 0, 0, 0, 0, 0, 0 | 0 / 6 | 0.00 | 이번 배치 제외 |

미션1은 원래 구성을 유지한다. 미션8의 현재 맵 데이터는 적 항목이0개여서 이번안에서 제외했다. 원래 헬기형 보스는2-5의1대이며 수량 평균에 섞지 않았다.

### 수정한 배치표

| 미션-서브미션 | 맵 | 추가 기체 | 기존 적 수 | 추가 수량 | 추가 후 전체 적 수 |
|---|---|---|---|---|---|
| 2-2 | Stage06 | yak141 | 39 | +5 | 44 |
| 2-4 | Stage08 | yak141 | 44 | +5 | 49 |
| 3-2 | Stage11 | yak141 | 41 | +7 | 48 |
| 4-2 | Stage26 | yak141 | 32 | +4 | 36 |
| 5-2 | Stage16 | yak141 | 46 | +9 | 55 |
| 3-3 | Stage12 | harrier | 23 | +7 | 30 |
| 4-3 | Stage27 | harrier | 41 | +4 | 45 |
| 5-3 | Stage17 | harrier | 43 | +9 | 52 |
| 6-3 | Stage22 | harrier | 29 | +10 | 39 |
| 7-2 | Stage31 | harrier | 60 | +11 | 71 |
| 6-2 | Stage21 | f35b | 36 | +10 | 46 |
| 6-4 | Stage23 | f35b | 22 | +10 | 32 |
| 7-3 | Stage32 | f35b | 45 | +11 | 56 |
| 7-4 | Stage33 | f35b | 47 | +11 | 58 |

맵 번호는`GlobalInfo::mapNumber`의 진행표와 대조했다. 미션4는Stage25~29다. 원래 적의 이름·좌표·수량·생성 조건을 유지하고 새로운 배치 목록만 합친다.

113개의 최종 좌표를 정해 원래 적 사이에 분산했다. 맵 양끝300과 보스 주변600을 피하고, 지형 높이에140을 더해 생성한다. 원래 적1,182개 항목의 이름·좌표·순서·보스 플래그가 그대로인지 자동 대조했다. 미션7-2는 원래60대에11대를 추가했다. 전체 미션 난이도와 장시간 성능은 후속 검사다. Survival의 원래 생성 규칙은 유지했다. [113개 좌표와 검사 결과](../evidence/vtol-implementation-20261027.json).

[41개 스테이지 헬기 수·평균 JSON](../evidence/vtol-helicopter-density-20261027.json), [수정 배치와 이전 한 대안](../evidence/vtol-art-plan-20261027.json).

## 자세·엔진 효과

기존 헬기 비행을 재사용했다. 새3종에만 수평 속도에 따른 최대8도와 수직 속도에 따른 최대3도를 더한 목표각을 적용하고, 각도를 서서히 바꾼다. 노즐·불꽃·발사점·피격 범위가 기체 회전을 따른다.

기존 배기 그림을 후방 노즐 하나에 붙였다. 앞쪽 노즐의 불꽃은 후속 지시로 제거했다. 상승할 때 길게, 하강할 때 짧게 표시하고 일시정지 때 멈춘다. 불꽃은 기체 충돌 사각형에 포함하지 않는다. 원래 적의 이동 상수·무기 수치·스폰 항목은 유지했다.

## 제작·조사 근거

내장 imagegen으로 두 번째 시안 3종을 만들었다. 생성 후 사진·도면과 기수, 동체 비례, 흡입구, 날개·꼬리를 대조했고 해리어의 중복 노즐을 한 번 수정했다. 원본과 축소 시안에서 기체 비례를 확인했다.

- 형상 참고: [야크141 삼면도](https://www.aviastar.org/air/russia/yak-141.php), [야크141 실물 사진](https://pesawattempurdaffa.blogspot.com/2012/06/yak-141-freestyle.html), [해리어 GR3 사진 — Kev Slade / Air-Britain](https://abpic.co.uk/pictures/view/1199591), [F35B 사진 — 미 해병대 / DVIDS](https://www.dvidshub.net/image/8085647/distributed-stovl-operation).
- 색조·질감 참고: 기존 게임의 `E_Heli_Body_06.png`.
- 새 원본: `Art/VTOL/20261027-v2/`. [파일 해시·참조 목록](media/vtol/v2/manifest.json), [생성·수정 프롬프트](media/vtol/v2/prompts.json).
- 첫 시안은 `Art/VTOL/20261027/`에 보존했다. [첫 제작 명세](media/vtol/manifest.json).
- [무기·배치 조사](../evidence/vtol-art-plan-20261027.json), [헬기 수량·평균](../evidence/vtol-helicopter-density-20261027.json).

앞으로 현실 기체를 추가할 때도 사진·도면으로 형태를 확인하고 기존 게임 그림에서 색조·질감을 참고하도록 `integrate-game-art` 스킬에 기록했다. 후속 외곽선 수정본을 자원에 넣고, 로드할 때만 각 기체의 게임 표시 폭으로 고품질 축소해 GPU에서 사용한다. 준비한 텍스처는 다음 프레임에서도 재사용한다.

<details>
<summary>첫 시안 3종 보기 — 미채택</summary>

<h3>야크141 — 갈색 금속판·단발 미사일</h3>

좁고 긴 기수와 두 꼬리날개, 갈색·회색 금속판으로 구분했다. 기존 초기형 헬기의 색을 참고했다. 작은 하부 발사대와 아래로 향한 배기 두 곳을 배치했다. 첫 생성본의 윗면 배기 불꽃은 수정했다.

<figure class="vtol-art"><a href="media/vtol/yak141-concept.png"><img src="media/vtol/yak141-concept.png" alt="왼쪽을 향한 갈색 야크141 공중 적 시안" style="width:100%;height:auto"></a><figcaption>야크141 시안 · 투명 PNG1944×809 · 클릭하면 원본</figcaption></figure>

<h3>해리어 — 둥근 흡입구·올리브색·2발 기관포</h3>

둥근 기수와 큰 측면 흡입구, 하나의 꼬리날개를 사용했다. 올리브색과 밝은 회색 도장으로 야크와 구분한다. 하부의 작은 포드에서 기관포를 발사하고 측면 노즐은 아래로 향한다.

<figure class="vtol-art"><a href="media/vtol/harrier-concept.png"><img src="media/vtol/harrier-concept.png" alt="왼쪽을 향한 올리브색 해리어 공중 적 시안" style="width:100%;height:auto"></a><figcaption>해리어 시안 · 투명 PNG1774×887 · 클릭하면 원본</figcaption></figure>

<h3>F35B — 각진 회색 기체·2발 미사일</h3>

짧은 쐐기형 기수, 각진 흡입구와 두 꼬리날개, 열린 상부 리프트팬 덮개로 구분했다. 회색 도장과 작은 황토색 표시를 사용한다. 후반 헬기의2발 미사일을 하부 발사구에 연결하는 안이다. 첫 시안에는 아래쪽 배기 불꽃을 넣었다.

<figure class="vtol-art"><a href="media/vtol/f35b-concept.png"><img src="media/vtol/f35b-concept.png" alt="왼쪽을 향한 회색 F35B 공중 적 시안" style="width:100%;height:auto"></a><figcaption>F35B 시안 · 투명 PNG1962×801 · 클릭하면 원본</figcaption></figure>


</details>

## 첫 적용·설치 결과

iPhone Air와 iPad Air 13형 M2에 치트 앱과 정식 앱을 모두 업데이트 설치하고 실행했다. 기존 앱을 삭제하거나 저장을 초기화하지 않았다. iPhone은 무선, iPad는 USB 연결을 사용했다.

- 코어 자동 검사19개 통과. 원래 적1,182개 항목 보존,113개 추가, 이동·자세·발사·정지·파괴를 확인했다.
- 플레이어 무기20종×새 기체3종의 피격60건 통과. 탄환을 피격 범위 안에서 시작해 충돌·피해 처리를 확인하는 검사다.
- iPhone·iPad 각각 입력 검사8개 통과. 터치·키보드와20종 무기 발사, 새3종의 생성·렌더가 포함된다.
- 각 기기에서 중립 이동 상태로 새 기체 화면을 다시 캡처했다. 이 추가 검사도 각각1개 통과했다.

F35B와 기존 헬기가 함께 비행한다.

<figure><a href="media/vtol/runtime/iphone-vtol_yak141.png"><img src="media/vtol/runtime/iphone-vtol_yak141.png" alt="iPhone에서 렌더한 야크141 전투 화면" style="width:100%;height:auto"></a><figcaption>iPhone · 야크141</figcaption></figure>

<figure><a href="media/vtol/runtime/iphone-vtol_harrier.png"><img src="media/vtol/runtime/iphone-vtol_harrier.png" alt="iPhone에서 렌더한 해리어 GR3 전투 화면" style="width:100%;height:auto"></a><figcaption>iPhone · 해리어 GR3</figcaption></figure>

<figure><a href="media/vtol/runtime/iphone-vtol_f35b.png"><img src="media/vtol/runtime/iphone-vtol_f35b.png" alt="iPhone에서 렌더한 F35B 전투 화면" style="width:100%;height:auto"></a><figcaption>iPhone · F35B</figcaption></figure>

<figure><a href="media/vtol/runtime/ipad-vtol_yak141.png"><img src="media/vtol/runtime/ipad-vtol_yak141.png" alt="iPad에서 렌더한 야크141 전투 화면" style="width:100%;height:auto"></a><figcaption>iPad · 야크141</figcaption></figure>

<figure><a href="media/vtol/runtime/ipad-vtol_harrier.png"><img src="media/vtol/runtime/ipad-vtol_harrier.png" alt="iPad에서 렌더한 해리어 GR3 전투 화면" style="width:100%;height:auto"></a><figcaption>iPad · 해리어 GR3</figcaption></figure>

<figure><a href="media/vtol/runtime/ipad-vtol_f35b.png"><img src="media/vtol/runtime/ipad-vtol_f35b.png" alt="iPad에서 렌더한 F35B 전투 화면" style="width:100%;height:auto"></a><figcaption>iPad · F35B</figcaption></figure>

전체 미션의 난이도 플레이, 장시간 발열과 Mac의 신규 유닛 성능 측정은 남았다. [설치·검사·캡처 해시 기록](../evidence/vtol-implementation-20261027.json).

## 외곽선·크기·불꽃 수정

내가 설치본을 확인한 뒤 앞쪽 불꽃 제거, 외곽선 정리, Heli001과 Heli006 사이의 길이를 요청했다. Codex는 후방 노즐 하나만 남기고 기체·피격 범위·발사점·노즐 위치를 함께 줄였다. 무기와 미션 배치는 유지했다.

| 기체 | 이전 길이 | 수정 길이 × 높이 |
|---|---:|---:|
| Heli001 몸체 — 비교 기준 | 99 | 99 × 47 |
| 야크141 | 140 | 112 × 37.3 |
| 해리어 GR3 | 132 | 108 × 41.0 |
| F35B | 148 | 116 × 44.1 |
| Heli006 몸체 — 비교 기준 | 126 | 126 × 48 |

수치는 게임의 논리 좌표이며 기체 이미지 기준이다. 회전·불꽃은 제외했다. 높이는 이미지 비율을 유지했다.

기체 모양과 도장을 유지한 채 이미지 편집으로 외곽선을 다듬었다. 게임에서는512픽셀 텍스처를 작은 화면에 다시 축소하던 처리를 바꿨다. 처음 로드할 때112/108/116픽셀로 고품질 축소해 가는 선과 점이 축소 과정에서 튀는 현상을 줄였다. 기존 헬기의 이미지와 GL 렌더 규칙은 유지했다.

위에서부터 Heli001·Heli006·야크·해리어·F35B다. 왼쪽은 수평, 오른쪽은 기울어진 자세다.

<figure><a href="media/vtol/refined/ipad-vtol_size_edges.png"><img src="media/vtol/refined/ipad-vtol_size_edges.png" alt="기존 헬기2종과 VTOL3종의 크기·외곽선 비교" style="width:100%;height:auto"></a><figcaption>iPad · 기체 크기와 외곽선 비교</figcaption></figure>

<figure><a href="media/vtol/refined/iphone-vtol_yak141.png"><img src="media/vtol/refined/iphone-vtol_yak141.png" alt="수정 후 iPhone의 야크141 전투 화면" style="width:100%;height:auto"></a><figcaption>iPhone · 야크141</figcaption></figure>

<figure><a href="media/vtol/refined/iphone-vtol_harrier.png"><img src="media/vtol/refined/iphone-vtol_harrier.png" alt="수정 후 iPhone의 해리어 GR3 전투 화면" style="width:100%;height:auto"></a><figcaption>iPhone · 해리어 GR3</figcaption></figure>

<figure><a href="media/vtol/refined/iphone-vtol_f35b.png"><img src="media/vtol/refined/iphone-vtol_f35b.png" alt="수정 후 iPhone의 F35B 전투 화면" style="width:100%;height:auto"></a><figcaption>iPhone · F35B</figcaption></figure>

기존 헬기가 F35B 앞을 지나며 동체 일부를 가린다.

[야크 수정 PNG](media/vtol/refined/VTOL_Yak141.png) · [해리어 수정 PNG](media/vtol/refined/VTOL_Harrier.png) · [F35B 수정 PNG](media/vtol/refined/VTOL_F35B.png) · [제작 기록](media/vtol/refined/manifest.json) · [편집 프롬프트](media/vtol/refined/prompts.json).

수정 후 VTOL 회귀 검사와20종 무기×3기체 피격60건이 통과했다. iPhone·iPad에서 전투 렌더 검사와 단독 비교 검사도 각각 통과했다. 두 기기에 수정된 치트 앱과 정식 앱을 업데이트 설치하고 실행했다. [검사·설치·파일 해시](../evidence/vtol-refinement-20261027.json). 전체 미션 난이도와 장시간 발열 검사는 별도로 남아 있다.
