# 럭스루나에 전투기 시안과 추가 계획

F-22·Su-57·MiG-31을 추가할 수 있다. 기존 공중 적과 미사일 코드를 재사용하고, 기체 그림·무기 복사본·증원 조건을 추가한다. 현재는 조사와 시안 3종을 만들었다. 게임 적용은 다음 단계다.

기존 완성분은 develop→main 병합과 **v0.11** 태그 푸시를 끝냈다. 새 전투기 작업은 `feat/lux-lunae-fighters`에서 분리했다. [구현 계획 원문](lux-lunae-plan-20261027.md).

## 도색과 기체 형상

[반다이남코 공식 소개](https://www.bandainamcoent.com/news/ace-combat-8-wings-of-theve-game-overview)의 럭스루나에 55부대 이미지를 참조했다. 짙은 회색·올리브색 동체, 가는 금색 선, 꼬리의 방패 문양과 흰색 055를 적용했다. 공식 이미지에서 보이는 MiG-31 도색을 F-22·Su-57에도 옮긴 시안이다.

실제 기체 사진은 형태 참조로, HeavyMach 헬기는 색조·금속 질감 참조로 사용했다. 기체 비례를 줄이거나 다른 날개를 붙이는 재설계는 하지 않는다. 생성된 MiG-31의 여분 꼬리날개와 배경 번짐은 다시 그려 수정했다. 기체에 불꽃은 그리지 않았다. 게임에서는 뒤쪽 엔진 위치에 별도로 붙인다.

## F-22

![럭스루나에 도색 F-22 시안](media/lux-lunae/f22-concept.png)

단좌 조종석, 각진 흡입구, 쌍수직미익과 납작한 배기구를 유지했다. 무장은 기존 헬기의 단발 미사일을 제안한다. [실기체 형태 참조](https://www.dvidshub.net/image/9081653/f-22-raptor-right-orthographic-view).

## Su-57

![럭스루나에 도색 Su-57 시안](media/lux-lunae/su57-concept.png)

길고 낮은 동체, 주익과 동체의 연결, 분리된 엔진과 원형 배기구를 유지했다. 무장은 F-22와 같은 단발 미사일을 제안한다. [실기체 형태 참조](https://defence24.pl/przemysl/rosja-rosnie-flota-su-57).

## MiG-31

![럭스루나에 도색 MiG-31 시안](media/lux-lunae/mig31-concept.png)

길고 뾰족한 기수, 2인승 조종석, 큰 사각 흡입구, 두 수직미익을 유지했다. 기존 F35B가 쓰는 2발 연속 미사일을 제안한다. [실기체 형태 참조](https://www.mk.ru/politics/2022/12/12/minoborony-mig31-soprovodil-nad-barencevym-morem-r3s-orion-vvs-norvegii.html).

## 표시 크기 비교

첫 비교용 길이는 F-22 112, Su-57 116, MiG-31 120 논리 단위다. 높이는 비례 조절한다. 아래는 웹에서 축소한 그림이며 실제 게임 렌더 결과는 아니다. 구현 때 원본 헬기와 같은 화면에 그려 외곽선·발사점·피격 범위를 확인한다.

<div class="lux-small">
<figure><img src="media/lux-lunae/heli001.png" style="width:99px" alt="원본 Heli001"><figcaption>Heli001 · 원본</figcaption></figure>
<figure><img src="media/lux-lunae/f22-concept.png" style="width:112px" alt="F-22 축소 시안"><figcaption>F-22 · 112</figcaption></figure>
<figure><img src="media/lux-lunae/su57-concept.png" style="width:116px" alt="Su-57 축소 시안"><figcaption>Su-57 · 116</figcaption></figure>
<figure><img src="media/lux-lunae/mig31-concept.png" style="width:120px" alt="MiG-31 축소 시안"><figcaption>MiG-31 · 120</figcaption></figure>
<figure><img src="media/lux-lunae/heli006.png" style="width:126px" alt="원본 Heli006"><figcaption>Heli006 · 원본</figcaption></figure>
</div>

## 내구력과 미사일

원본 헬기의 최대 HP는 Heli009·Heli010의 450이다. 세 기체 모두 **765 = 450 × 1.7**로 설정한다.

| 기체 | 재사용 무기 | 피해 / 탄 수 | 비행 속도 | 재발사 간격 | 연속탄 간격 |
|---|---|---|---|---|---|
| F-22 | 헬기·Yak141의 E_Wpn_ASM | 25 / 1발 | 160 → 200 | 5 → 4초 | 없음 |
| Su-57 | 헬기·Yak141의 E_Wpn_ASM | 25 / 1발 | 160 → 200 | 5 → 4초 | 없음 |
| MiG-31 | F35B의 E_0604_Wpn | 100 / 2발 | 160 → 200 | 5 → 4초 | 0.5 → 0.4초 |

25% 빠른 연사는 간격을 0.8배로 줄이는 값이다. 적 AI의 조준·발사 준비·다음 공격 대기도 신규 기체에 한해 0.8배로 처리하고 실제 발사 시각으로 검사한다. 피해·탄 수·분산·중력·수명·음향은 원래 무기 값을 유지한다. 새 기체 전용 무기 복사본에만 속도와 간격을 적용한다.

## 미사일을 묶어서 쏘는 구성

후속 질문의 두 방식 모두 가능하다. 단발 미사일은 총 3발 연사로 만들 수 있다. 기존 2발 발사는 세 묶음 반복하면 총 6발이다.

3연사는 현재 탄 수·간격 설정을 이용한다. 2발씩 끊어 세 번 쏘려면 묶음 사이의 대기를 추가한다. 예를 들어 2발 사이를 0.4초로 유지하면서, 다음 묶음까지는 더 길게 기다리게 할 수 있다. `6발`만 설정하면 일정한 간격으로 여섯 발이 나가므로 두 동작을 구별해 구현한다. 이 구성은 아직 특정 기체에 확정하지 않았다.

기존 미사일의 발당 피해·그림·비행·충돌을 재사용한다. 신규 기체 전용 설정이므로 원래 헬기·VTOL에는 적용되지 않는다. 발사 처리는 기존 게임 갱신 안에서 하며 발사마다 파일을 저장하지 않는다.

## 등장 미션과 수량

대형 보스전을 기준으로 한 배치안이다. 일반 서브미션의 중간보스는 제외한다. 수량은 세 기체를 합친 값이다. 기존 적을 빼거나 교체하지 않는다.

| 보스전 | 합계 | F-22 / Su-57 / MiG-31 | 바로 직전 서브미션 | 마지막 구간 편성 |
|---|---:|---|---|---|
| 미션 1-4 | 2 | 1 / 1 / 0 | 1-3 | 2 / 1 / 1 = 4대 |
| 미션 2-5 | 4 | 1 / 1 / 2 | 2-4 | 1 / 2 / 1 = 4대 |
| 미션 3-5 | 6 | 2 / 2 / 2 | 3-4 | 1 / 1 / 2 = 4대 |
| 미션 4-5 | 8 | 3 / 3 / 2 | 4-4 | 2 / 1 / 1 = 4대 |
| 미션 5-5 | 8 | 3 / 2 / 3 | 5-4 | 1 / 2 / 1 = 4대 |
| 미션 6-5 | 8 | 2 / 3 / 3 | 6-4 | 1 / 1 / 2 = 4대 |
| 미션 7-6 | 8 | 3 / 3 / 2 | 7-5 | 2 / 1 / 1 = 4대 |

보스전 44대와 직전 구간 28대를 합쳐 72대를 추가한다. 기존 원본 적 1,182항목과 VTOL 113항목은 유지한다. 보스전 시작과 마지막 월드 구간 진입을 기준으로 한 번씩 생성한다. 화면이 넓어졌다는 이유로 먼저 등장하거나, 일시정지 후 돌아왔을 때 중복 생성되지 않게 한다. 보스를 처치했을 때의 기존 클리어 조건도 유지한다.

## 구현·검증할 내용

기체와 무기 정의를 추가한 뒤 14개 대상 구간의 생성 수·시점·재시작을 검사한다. 치트 앱에서는 3종 단독과 혼합 8대를 띄우고 이동·점프·수동 사격·자동조준·표적 전환·전체 무기 20종의 피격을 확인한다. 8대의 미사일과 보스 공격이 겹치는 장면에서 프레임·메모리도 측정한다. iPhone·iPad·Mac의 같은 공용 코어에 적용한다.

## 원본 파일과 조사 자료

- [F-22 PNG](media/lux-lunae/f22-concept.png), [Su-57 PNG](media/lux-lunae/su57-concept.png), [MiG-31 PNG](media/lux-lunae/mig31-concept.png)
- [내장 imagegen 프롬프트](media/lux-lunae/prompts.json), [참조·파일 해시](media/lux-lunae/manifest.json)
- [원본 헬기·무기·맵 수치](evidence/lux-lunae-inventory-20261027.txt), [추출 코드](evidence/lux-lunae-inventory-20261027.cpp)
- [세부 구현 계획](lux-lunae-plan-20261027.md)

게임용 PNG는 현재 시안에서 표시 크기·투명 여백·외곽선을 확인한 후 별도로 확정한다. 이 페이지의 이미지는 설치본 캡처가 아니다.
