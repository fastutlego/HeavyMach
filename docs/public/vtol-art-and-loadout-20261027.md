# 수직이착륙 공중 적 3종 시안·배치안

야크141·해리어·F35B를 바탕으로 게임용 시안3종을 만들었다. 기존 적 그림을 참조해 낡은 금속판, 낮은 채도, 밝은 윗면과 어두운 아랫면을 사용했다. 아래 이미지는 **제작한 시안**이며, 게임에는 아직 넣지 않았다. 불꽃은 시안에 함께 그렸고 실제 구현에서는 기체·노즐·불꽃을 분리한다.

## 기체 시안

### 야크141 — 갈색 금속판·단발 미사일

좁고 긴 기수와 두 꼬리날개, 갈색·회색 금속판으로 구분했다. 기존 초기형 헬기의 색을 참고했다. 작은 하부 발사대와 아래로 향한 배기 두 곳을 배치했다. 첫 생성본의 윗면 배기 불꽃은 수정했다.

<figure class="vtol-art"><a href="media/vtol/yak141-concept.png"><img src="media/vtol/yak141-concept.png" alt="왼쪽을 향한 갈색 야크141 공중 적 시안" style="width:100%;height:auto"></a><figcaption>야크141 시안 · 투명 PNG1944×809 · 클릭하면 원본</figcaption></figure>

### 해리어 — 둥근 흡입구·올리브색·2발 기관포

둥근 기수와 큰 측면 흡입구, 하나의 꼬리날개를 사용했다. 올리브색과 밝은 회색 도장으로 야크와 구분한다. 하부의 작은 포드에서 기관포를 발사하고 측면 노즐은 아래로 향한다.

<figure class="vtol-art"><a href="media/vtol/harrier-concept.png"><img src="media/vtol/harrier-concept.png" alt="왼쪽을 향한 올리브색 해리어 공중 적 시안" style="width:100%;height:auto"></a><figcaption>해리어 시안 · 투명 PNG1774×887 · 클릭하면 원본</figcaption></figure>

### F35B — 각진 회색 기체·2발 미사일

짧은 쐐기형 기수, 각진 흡입구와 두 꼬리날개, 열린 상부 리프트팬 덮개로 구분했다. 회색 도장과 작은 황토색 표시를 사용한다. 후반 헬기의2발 미사일을 하부 발사구에 연결하는 안이다. 시안의 배기는 게임용 표현이며 실물 기체의 배기 온도·화염을 재현한 그림은 아니다.

<figure class="vtol-art"><a href="media/vtol/f35b-concept.png"><img src="media/vtol/f35b-concept.png" alt="왼쪽을 향한 회색 F35B 공중 적 시안" style="width:100%;height:auto"></a><figcaption>F35B 시안 · 투명 PNG1962×801 · 클릭하면 원본</figcaption></figure>

## 기존 유닛과 작은 크기 비교

아래는 브라우저에서 폭을 줄여 보는 비교다. 실제 게임 캡처는 아니다. 원본 비율을 유지했다. 신규 기체의 폭140/132/148은 첫 적용 후보이며 충돌 크기와 함께 검사한 뒤 정한다.

<div class="vtol-small" style="display:flex;flex-wrap:wrap;align-items:end;gap:24px;padding:20px;background:#34362e;color:#f3efe3"><figure><img src="media/vtol/E_Heli_Body_01.png" alt="기존 Heli001 몸체" style="width:100px;height:auto"><figcaption>기존 Heli001</figcaption></figure><figure><img src="media/vtol/E_Heli_Body_06.png" alt="기존 Heli006 몸체" style="width:121px;height:auto"><figcaption>기존 Heli006</figcaption></figure><figure><img src="media/vtol/E_0604_Body.png" alt="기존 E0604 헬기 몸체" style="width:140px;height:auto"><figcaption>기존 E0604</figcaption></figure><figure><img src="media/vtol/yak141-concept.png" alt="작게 표시한 야크" style="width:140px;height:auto"><figcaption>야크141 / 140</figcaption></figure><figure><img src="media/vtol/harrier-concept.png" alt="작게 표시한 해리어" style="width:132px;height:auto"><figcaption>해리어 / 132</figcaption></figure><figure><img src="media/vtol/f35b-concept.png" alt="작게 표시한 F35B" style="width:148px;height:auto"><figcaption>F35B / 148</figcaption></figure></div>

## 무장 차이

일반 Heli001~010은 모두 같은 단발 미사일을 사용한다. 추가 조사에서 회전날개가 있는 헬기 보스와E0604의 다른 무기를 확인했다. 다음 세 정의를 그대로 재사용한다. 새 무기·탄환 계산을 복제하지 않는다.

| 기체 | 무장안·재사용 정의 | 기존 사용 유닛 | 발당 피해 / 한 번에 준비하는 탄수 | 차이 |
|---|---|---|---|---|
| 야크141 | 단발 미사일 `E_Wpn_ASM` | Heli001~010 | 25 / 1발 | 낮은 위력의 단발 공격. 미션2부터 등장 |
| 해리어 | 2발 기관포 `E_Boss_Wpn_01` | 헬기 보스Boss04·Boss06 | 50 / 2발 | 미사일과 다른 기관포 탄환. 미션3부터 등장 |
| F35B | 2발 미사일 `E_0604_Wpn` | 후반 헬기E0604 | 100 / 2발 | 기존 후반 헬기의 강한 연속 공격. 미션6부터 등장 |

표의 피해·탄수는 실제 원본 데이터 값이다. 세 정의의delay는 모두5지만 AI 조준 대기·발사·재장전이 함께 작동하므로 ‘정확히5초마다 발사’라고 해석하지 않는다. 2발은 연사 묶음이며 두 발이 동시에 나간다는 뜻이 아니다. type6 탄환에 발사 후 추적 기능은 없어 유도 미사일이라고 부르지 않는다. HP·등장 밀도와 합친 난도는 치트 앱에서 비교할 항목이다.

## 미션별 추가 배치

**기존 적을 모두 유지하고 아래14개 서브미션에 각1대씩 추가하는 안이다.** 야크5대·해리어5대·F35B4대다. 보스 플래그는 붙이지 않는다. 미션1은 원래 구성으로 유지한다. 후기 미션에서도 모든 기체가 무조건 한 단계씩 교체되는 방식은 쓰지 않는다.

| 기체 | 최초 등장 | 추가할 미션·서브미션 |
|---|---|---|
| 야크141 | 2-2 | 2-2, 2-4, 3-2, 4-2, 5-2 |
| 해리어 | 3-3 | 3-3, 4-3, 5-3, 6-3, 7-2 |
| F35B | 6-2 | 6-2, 6-4, 7-3, 7-4 |

맵 번호를 미션 번호로 잘못 쓰지 않도록 `GlobalInfo::mapNumber`의 실제 진행표와 대조했다. 미션4는Stage25~29를 사용한다. 각 맵에서 원래 적을X좌표순으로 읽고, 시작·끝에서300 이상 떨어지며 보스 배치와600 이상 떨어진 빈 간격의 중간을 골랐다. 간격243~697은 기존 두 생성 위치 사이의 거리다. 적이 이동한 뒤까지 서로 겹치지 않는다는 검증은 아직 아니다.

| 미션-서브미션 | 맵 | 추가 기체 | 기존 적 수 → 추가 후 | 후보 월드X |
|---|---|---|---|---|
| 2-2 | Stage06 | yak141 | 39 → 40 | 730 |
| 2-4 | Stage08 | yak141 | 44 → 45 | -291 |
| 3-2 | Stage11 | yak141 | 41 → 42 | 4590.5 |
| 4-2 | Stage26 | yak141 | 32 → 33 | 3364.5 |
| 5-2 | Stage16 | yak141 | 46 → 47 | 5190 |
| 3-3 | Stage12 | harrier | 23 → 24 | 258 |
| 4-3 | Stage27 | harrier | 41 → 42 | 1529.5 |
| 5-3 | Stage17 | harrier | 43 → 44 | 665.5 |
| 6-3 | Stage22 | harrier | 29 → 30 | 1941.5 |
| 7-2 | Stage31 | harrier | 60 → 61 | 1969.5 |
| 6-2 | Stage21 | f35b | 36 → 37 | 3307 |
| 6-4 | Stage23 | f35b | 22 → 23 | 3316.5 |
| 7-3 | Stage32 | f35b | 45 → 46 | 2243.5 |
| 7-4 | Stage33 | f35b | 47 → 48 | 2736.5 |

높이Y는 해당 지점의 지형 높이와 기존 헬기의 비행 높이를 대조해 정한다. 지형·화면 위쪽·보스 전투를 침범하면 새 항목의 위치를 조절하며 원래 항목은 옮기지 않는다. 미션8의 검토 맵Stage37/39는 현재 reader 결과에 기존 적 항목이0개여서 이번 ‘사이사이 추가’ 후보에서 제외했다. Survival도 이번14개 캠페인 배치에 포함하지 않았다.

## 자세·엔진 효과 적용안

전후 이동은 기존 비행 코드의 속도 기반 기울기를 참고한다. 새3종에만 전후 목표각 약±8도와 상승·하강 보조각 약±3도를 초기 후보로 적용하고, 급격히 꺾이지 않게 보간한다. 이 각도는 아직 적용하지 않은 제안이다. 기체 회전과 같은 변환으로 노즐·불꽃·발사점을 이동시킨다.

상승할 때 배기를 길게, 정지 비행에서는 짧게, 하강할 때는 더 약하게 표시한다. 실제 제작 때 기체와 불꽃을 분리하고 불꽃만 애니메이션한다. 현재PNG 전체를 흔들거나 불꽃 길이까지 포함한 충돌 사각형을 사용하지 않는다. 기존 적의 이동 상수·무기 수치·스폰 테이블은 유지한다.

## 제작·조사 근거

내장 imagegen으로3종을 생성하고 야크의 배기 위치를 한 번 수정했다. PNG3개 모두 알파 채널이 있다. 이미지 원본·참조·프롬프트·해시는 `Art/VTOL/20261027/`와 [제작 명세](media/vtol/manifest.json)에 보존했다. [배치·무기 조사 JSON](../evidence/vtol-art-plan-20261027.json)은 실제 데이터 값과 제안 좌표를 구분한다.

기체 형식은 야크141, 해리어 계열, F35의수직이착륙형인B를 시안 기준으로 골랐다. 실물 제원과 게임 능력치를 연결하지 않는다. [야크141 전시 설명](https://muscom.ru/en//expositions/yak-141/), [RAF Museum 해리어](https://www.rafmuseum.org.uk/research/collections/hawker-siddeley-harrier-gr3/), [Lockheed Martin F35B](https://www.lockheedmartin.com/f35/news-and-features/Choose-Your-Fighter-F35A-vs-F35B.html).

이번 작업은 그림 제작과 무장·배치안 작성이다. 게임 코드·맵·설치본은 변경하지 않았다. 기체·불꽃 분리, 실제 합성, 충돌·발사점, 추가 적 난도·성능 검사는 구현 단계에 남아 있다.
