# 수직이착륙 공중 적 3종 시안·배치안

실제 기체 사진과 야크141 도면을 참조해 **야크141·해리어 GR3·F35B의 두 번째 시안**을 만들었다. 길이와 높이의 비례, 기수·흡입구·날개·꼬리 형태를 실물에 맞춰 다시 그렸다. 기존 게임 그림은 색조·질감·명암의 참고로 사용했다. 기체는 투명 PNG이며 불꽃은 넣지 않았다. 게임 적용 전 시안이다.

## 기체 시안

### 야크141 — 긴 동체·각진 흡입구·두 수직 꼬리날개

실물 사진과 삼면도에서 긴 동체, 낮은 조종석, 측면 흡입구와 높은 주익의 위치를 확인했다. 첫 시안의 두꺼운 장갑판과 갈색 도장을 빼고 회색 금속 표면으로 다시 그렸다.

<figure class="vtol-art"><a href="media/vtol/v2/yak141-concept.png"><img src="media/vtol/v2/yak141-concept.png" alt="실물 형상을 참조해 다시 그린 회색 야크141 측면 시안" style="width:100%;height:auto"></a><figcaption>야크141 · 두 번째 시안 · 클릭하면 투명 PNG 원본</figcaption></figure>

### 해리어 GR3 — 긴 기수·둥근 흡입구·단일 수직 꼬리날개

GR3 사진의 기수, 큰 흡입구와 낮게 내려가는 주익을 참고했다. 올리브색과 짙은 회색 위장 도장을 사용했다. 생성본의 중복 배기구를 수정해 보이는 측면에 앞·뒤 두 노즐을 남겼다.

<figure class="vtol-art"><a href="media/vtol/v2/harrier-concept.png"><img src="media/vtol/v2/harrier-concept.png" alt="실물 형상을 참조해 다시 그린 올리브색 해리어 GR3 측면 시안" style="width:100%;height:auto"></a><figcaption>해리어 GR3 · 두 번째 시안 · 클릭하면 투명 PNG 원본</figcaption></figure>

### F35B — 각진 기수·흡입구·두 수직 꼬리날개

미 해병대 F35B 측면 사진을 참조했다. 기수에서 동체로 이어지는 윤곽, 조종석, 각진 흡입구와 꼬리 배치를 다시 그렸다. 랜딩기어와 리프트팬 덮개를 닫은 기체 시안이다. 호버링 때 열리는 덮개와 배기 효과는 구현할 때 별도로 만든다.

<figure class="vtol-art"><a href="media/vtol/v2/f35b-concept.png"><img src="media/vtol/v2/f35b-concept.png" alt="실물 형상을 참조해 다시 그린 회색 F35B 측면 시안" style="width:100%;height:auto"></a><figcaption>F35B · 두 번째 시안 · 클릭하면 투명 PNG 원본</figcaption></figure>

## 기존 유닛과 작은 크기 비교

아래는 브라우저에서 폭을 줄여 보는 비교다. 실제 게임 캡처는 아니다. 원본 비율을 유지했다. 신규 기체의 폭140/132/148은 첫 적용 후보이며 충돌 크기와 함께 검사한 뒤 정한다.

<div class="vtol-small" style="display:flex;flex-wrap:wrap;align-items:end;gap:24px;padding:20px;background:#34362e;color:#f3efe3"><figure><img src="media/vtol/E_Heli_Body_01.png" alt="기존 Heli001 몸체" style="width:100px;height:auto"><figcaption>기존 Heli001</figcaption></figure><figure><img src="media/vtol/E_Heli_Body_06.png" alt="기존 Heli006 몸체" style="width:121px;height:auto"><figcaption>기존 Heli006</figcaption></figure><figure><img src="media/vtol/E_0604_Body.png" alt="기존 E0604 헬기 몸체" style="width:140px;height:auto"><figcaption>기존 E0604</figcaption></figure><figure><img src="media/vtol/v2/yak141-concept.png" alt="작게 표시한 야크" style="width:140px;height:auto"><figcaption>야크141 / 140</figcaption></figure><figure><img src="media/vtol/v2/harrier-concept.png" alt="작게 표시한 해리어" style="width:132px;height:auto"><figcaption>해리어 / 132</figcaption></figure><figure><img src="media/vtol/v2/f35b-concept.png" alt="작게 표시한 F35B" style="width:148px;height:auto"><figcaption>F35B / 148</figcaption></figure></div>

## 무장 차이

일반 Heli001~010은 모두 같은 단발 미사일을 사용한다. 추가 조사에서 회전날개가 있는 헬기 보스와E0604의 다른 무기를 확인했다. 다음 세 정의를 그대로 재사용한다. 새 무기·탄환 계산을 복제하지 않는다.

| 기체 | 무장안·재사용 정의 | 기존 사용 유닛 | 발당 피해 / 한 번에 준비하는 탄수 | 차이 |
|---|---|---|---|---|
| 야크141 | 단발 미사일 `E_Wpn_ASM` | Heli001~010 | 25 / 1발 | 낮은 위력의 단발 공격. 미션2부터 등장 |
| 해리어 | 2발 기관포 `E_Boss_Wpn_01` | 헬기 보스Boss04·Boss06 | 50 / 2발 | 미사일과 다른 기관포 탄환. 미션3부터 등장 |
| F35B | 2발 미사일 `E_0604_Wpn` | 후반 헬기E0604 | 100 / 2발 | 기존 후반 헬기의 강한 연속 공격. 미션6부터 등장 |

표의 피해·탄수는 실제 원본 데이터 값이다. 세 정의의delay는 모두5지만 AI 조준 대기·발사·재장전이 함께 작동하므로 ‘정확히5초마다 발사’라고 해석하지 않는다. 2발은 연사 묶음이며 두 발이 동시에 나간다는 뜻이 아니다. type6 탄환에 발사 후 추적 기능은 없어 유도 미사일이라고 부르지 않는다. HP·등장 밀도와 합친 난도는 치트 앱에서 비교할 항목이다.

## 미션별 추가 배치

**기존 적을 모두 유지하고 아래14개 서브미션에 일반 헬기 평균을 기준으로 추가한다.** 사용자의 수량 수정 지시를 반영해 최초의 각1대안을 바꿨다. 새 안은 야크30대·해리어41대·F35B42대, 총113대다. 보스 플래그는 붙이지 않는다. 미션1은 원래 구성으로 유지한다. 후기 미션에서도 모든 기체가 무조건 한 단계씩 교체되는 방식은 쓰지 않는다.

| 기체 | 최초 등장 | 추가할 미션·서브미션 |
|---|---|---|
| 야크141 | 2-2 | 2-2, 2-4, 3-2, 4-2, 5-2 |
| 해리어 | 3-3 | 3-3, 4-3, 5-3, 6-3, 7-2 |
| F35B | 6-2 | 6-2, 6-4, 7-3, 7-4 |

### 헬기 수와 추가 수량 기준

41개 스테이지의 원래 적 정의를 읽었다. 이름이Heli로 시작하는 유닛만 세지 않고, 재귀 부품에 헬기 회전날개가 있는E0603·E0604 등도 포함했다. 보스 플래그가 있는 헬기는 별도로 세고 평균에서 제외했다. 회전날개 없는 항공기는 제외했다. 각 미션의 모든 서브미션을 분모로 쓰며0대인 스테이지도 포함한다.

`추가 수량 = 미션 내 일반 헬기 총수 ÷ 서브미션 수`를 소수 첫째 자리에서 반올림한다. 기존에 선택한 서브미션마다 해당 수량을 더한다. 한 장소나 한 번에 그 수량을 전부 생성하는 뜻은 아니다.

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

미션1은 원래 구성을 유지한다. 미션8의 현재 맵 데이터는 적 항목이0개여서 이번안에서 제외했다. 이 값으로 실제 앱의 미션8 전체 동작까지 판정하지 않는다. 원래 헬기형 보스는2-5의1대이며 수량 평균에 섞지 않았다.

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

처음 조사한 좌표는 한 대안의 후보이므로113대를 그14곳에 몰아서 넣지 않는다. 다음 배치 작업에서 원래 적 사이의 여러 간격에 나누고, 시작·출구·보스 전투를 피한다. Y는 지형과 비행 높이를 보고 정한다. 모든 개체의 최종 좌표는 아직 정하지 않았다. 특히 미션7-2는 원래60대에11대가 더해지므로 중첩 전투·탄환 수·프레임을 우선 검사한다. 무기별 원래 수치는 유지한 채 배치 간격과 동시 진입 수로 난도를 확인한다. Survival은 이번 캠페인 배치에 포함하지 않았다.

[41개 스테이지 헬기 수·평균 JSON](../evidence/vtol-helicopter-density-20261027.json), [수정 배치와 이전 한 대안](../evidence/vtol-art-plan-20261027.json).

## 자세·엔진 효과 적용안

전후 이동은 기존 비행 코드의 속도 기반 기울기를 참고한다. 새3종에만 전후 목표각 약±8도와 상승·하강 보조각 약±3도를 초기 후보로 적용하고, 급격히 꺾이지 않게 보간한다. 이 각도는 아직 적용하지 않은 제안이다. 기체 회전과 같은 변환으로 노즐·불꽃·발사점을 이동시킨다.

상승할 때 배기를 길게, 정지 비행에서는 짧게, 하강할 때는 더 약하게 표시한다. 새 시안에는 불꽃이 없다. 구현할 때 별도 배기 효과를 노즐에 연결하고 애니메이션한다. 불꽃 길이를 기체 충돌 사각형에 포함하지 않는다. 기존 적의 이동 상수·무기 수치·스폰 테이블은 유지한다.

## 제작·조사 근거

내장 imagegen으로 두 번째 시안 3종을 만들었다. 생성 후 사진·도면과 기수, 동체 비례, 흡입구, 날개·꼬리를 대조했고 해리어의 중복 노즐을 한 번 수정했다. 그림은 원본 크기로 확인했다. 아래 축소 표시는 브라우저의 비율 유지 축소이며 실제 게임 캡처가 아니다.

- 형상 참고: [야크141 삼면도](https://www.aviastar.org/air/russia/yak-141.php), [야크141 실물 사진](https://pesawattempurdaffa.blogspot.com/2012/06/yak-141-freestyle.html), [해리어 GR3 사진 — Kev Slade / Air-Britain](https://abpic.co.uk/pictures/view/1199591), [F35B 사진 — 미 해병대 / DVIDS](https://www.dvidshub.net/image/8085647/distributed-stovl-operation).
- 색조·질감 참고: 기존 게임의 `E_Heli_Body_06.png`.
- 새 원본: `Art/VTOL/20261027-v2/`. [파일 해시·참조 목록](media/vtol/v2/manifest.json), [생성·수정 프롬프트](media/vtol/v2/prompts.json).
- 첫 시안은 `Art/VTOL/20261027/`에 보존했다. [첫 제작 명세](media/vtol/manifest.json).
- [무기·배치 조사](../evidence/vtol-art-plan-20261027.json), [헬기 수량·평균](../evidence/vtol-helicopter-density-20261027.json).

기존 무장·113대 추가 배치안은 유지했다. 게임 코드·맵·설치본은 변경하지 않았다. 호버링 부품과 배기 효과, 게임 내 크기·충돌·발사점, 난도·성능 검사는 구현 단계에 남아 있다.

<details>
<summary>첫 시안 3종 보기 — 미채택</summary>

<h3>야크141 — 갈색 금속판·단발 미사일</h3>

좁고 긴 기수와 두 꼬리날개, 갈색·회색 금속판으로 구분했다. 기존 초기형 헬기의 색을 참고했다. 작은 하부 발사대와 아래로 향한 배기 두 곳을 배치했다. 첫 생성본의 윗면 배기 불꽃은 수정했다.

<figure class="vtol-art"><a href="media/vtol/yak141-concept.png"><img src="media/vtol/yak141-concept.png" alt="왼쪽을 향한 갈색 야크141 공중 적 시안" style="width:100%;height:auto"></a><figcaption>야크141 시안 · 투명 PNG1944×809 · 클릭하면 원본</figcaption></figure>

<h3>해리어 — 둥근 흡입구·올리브색·2발 기관포</h3>

둥근 기수와 큰 측면 흡입구, 하나의 꼬리날개를 사용했다. 올리브색과 밝은 회색 도장으로 야크와 구분한다. 하부의 작은 포드에서 기관포를 발사하고 측면 노즐은 아래로 향한다.

<figure class="vtol-art"><a href="media/vtol/harrier-concept.png"><img src="media/vtol/harrier-concept.png" alt="왼쪽을 향한 올리브색 해리어 공중 적 시안" style="width:100%;height:auto"></a><figcaption>해리어 시안 · 투명 PNG1774×887 · 클릭하면 원본</figcaption></figure>

<h3>F35B — 각진 회색 기체·2발 미사일</h3>

짧은 쐐기형 기수, 각진 흡입구와 두 꼬리날개, 열린 상부 리프트팬 덮개로 구분했다. 회색 도장과 작은 황토색 표시를 사용한다. 후반 헬기의2발 미사일을 하부 발사구에 연결하는 안이다. 시안의 배기는 게임용 표현이며 실물 기체의 배기 온도·화염을 재현한 그림은 아니다.

<figure class="vtol-art"><a href="media/vtol/f35b-concept.png"><img src="media/vtol/f35b-concept.png" alt="왼쪽을 향한 회색 F35B 공중 적 시안" style="width:100%;height:auto"></a><figcaption>F35B 시안 · 투명 PNG1962×801 · 클릭하면 원본</figcaption></figure>


</details>
