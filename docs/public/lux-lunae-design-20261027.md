# 럭스루나에 전투기 3종

Codex는 F-22·Su-57·MiG-31을 구현하고 아이폰·아이패드에 치트·정식 앱을 설치했다. 금색 조종석 유리, HP765, 3발 연사와 2발씩 3묶음 발사, 14개 구간의 72대 증원을 적용했다. 나는 설치본에서 기체가 갑자기 나타나는 문제를 발견했다. Codex가 화면 밖에서 비행 진입하도록 고치고, 연사속도 증가는 10%로 낮췄다.

기존 완성분은 develop→main 병합과 **v0.11** 태그 푸시를 끝냈다. 새 전투기 작업은 `feat/lux-lunae-fighters`에서 분리했다. [구현 계획 원문](lux-lunae-plan-20261027.md).

## 도색과 기체 형상

[반다이남코 공식 소개](https://www.bandainamcoent.com/news/ace-combat-8-wings-of-theve-game-overview)의 럭스루나에 55부대 이미지를 참조했다. 짙은 회색·올리브색 동체, 가는 금색 선, 꼬리의 방패 문양과 흰색 055를 적용했다. 공식 이미지에서 보이는 MiG-31 도색을 F-22·Su-57에도 옮긴 시안이다.

실제 기체 사진은 형태 참조로, HeavyMach 헬기는 색조·금속 질감 참조로 사용했다. 기체 비례를 줄이거나 다른 날개를 붙이는 재설계는 하지 않는다. 생성된 MiG-31의 여분 꼬리날개와 배경 번짐은 다시 그려 수정했다. 기체에 불꽃은 그리지 않았다. 게임에서는 뒤쪽 엔진 위치에 별도로 붙인다.

## F-22

![럭스루나에 도색 F-22 시안](media/lux-lunae/F22-gold.png)

단좌 조종석, 각진 흡입구, 쌍수직미익과 납작한 배기구를 유지했다. 기존 헬기의 단발 미사일을 3발씩 연사한다. [실기체 형태 참조](https://www.dvidshub.net/image/9081653/f-22-raptor-right-orthographic-view).

## Su-57

![럭스루나에 도색 Su-57 시안](media/lux-lunae/Su57-gold.png)

길고 낮은 동체, 주익과 동체의 연결, 분리된 엔진과 원형 배기구를 유지했다. F-22와 같은 미사일을 3발씩 연사한다. [실기체 형태 참조](https://defence24.pl/przemysl/rosja-rosnie-flota-su-57).

## MiG-31

![럭스루나에 도색 MiG-31 시안](media/lux-lunae/MiG31-gold.png)

길고 뾰족한 기수, 2인승 조종석, 큰 사각 흡입구, 두 수직미익을 유지했다. 기존 F35B의 미사일을 2발씩 3묶음, 총6발 발사한다. [실기체 형태 참조](https://www.mk.ru/politics/2022/12/12/minoborony-mig31-soprovodil-nad-barencevym-morem-r3s-orion-vvs-norvegii.html).

## 표시 크기 비교

게임 표시 길이는 F-22 112, Su-57 116, MiG-31 120 논리 단위다. 높이는 비례 조절한다. 아래는 웹에서 축소한 그림이며 실제 게임 렌더 결과는 아니다. 두 기기에서 원본 헬기와 함께 그려 크기와 외곽선을 확인했다. 실제 GPU 캡처는 아래에 있다.

<div class="lux-small">
<figure><img src="media/lux-lunae/heli001.png" style="width:99px" alt="원본 Heli001"><figcaption>Heli001 · 원본</figcaption></figure>
<figure><img src="media/lux-lunae/F22-gold.png" style="width:112px" alt="F-22 축소 시안"><figcaption>F-22 · 112</figcaption></figure>
<figure><img src="media/lux-lunae/Su57-gold.png" style="width:116px" alt="Su-57 축소 시안"><figcaption>Su-57 · 116</figcaption></figure>
<figure><img src="media/lux-lunae/MiG31-gold.png" style="width:120px" alt="MiG-31 축소 시안"><figcaption>MiG-31 · 120</figcaption></figure>
<figure><img src="media/lux-lunae/heli006.png" style="width:126px" alt="원본 Heli006"><figcaption>Heli006 · 원본</figcaption></figure>
</div>

## 내구력과 미사일

원본 헬기의 최대 HP는 Heli009·Heli010의 450이다. 세 기체 모두 **765 = 450 × 1.7**로 설정한다.

| 기체 | 재사용 무기 | 피해 / 탄 수 | 비행 속도 | 재발사 간격 | 연속탄 간격 |
|---|---|---|---|---|---|
| F-22 | 헬기·Yak141의 E_Wpn_ASM | 25 / 3발 | 160 → 200 | 5 → 약4.545초 | 약0.455초 |
| Su-57 | 헬기·Yak141의 E_Wpn_ASM | 25 / 3발 | 160 → 200 | 5 → 약4.545초 | 약0.455초 |
| MiG-31 | F35B의 E_0604_Wpn | 100 / 6발 | 160 → 200 | 5 → 약4.545초 | 약0.455초 / 묶음 사이 약0.909초 |

연사속도는 최초25%에서 내 후속 지시로 원본 대비10% 증가로 낮췄다. 발사·AI 대기를1.1로 나눈다. 비행 속도는25% 증가를 유지한다. 발당 피해·분산·중력·수명·음향은 원래 무기 값을 사용한다. 기존 헬기·VTOL의 무기 데이터는 변경하지 않는다.

## 미사일을 묶어서 쏘는 구성

F-22·Su-57은 약0.455초 간격으로3발 발사한다. MiG-31은 약0.455초 간격으로2발을 쏘고 약0.909초 기다린 뒤 다음 묶음을 발사한다. 총3묶음,6발이다. 60Hz 갱신에서 발사 시각의 최대1프레임 차이를 검사했다.

기존 미사일의 발당 피해·그림·비행·충돌을 재사용한다. 신규 기체 전용 설정이므로 원래 헬기·VTOL에는 적용되지 않는다. 발사 처리는 기존 게임 갱신 안에서 하며 발사마다 파일을 저장하지 않는다.

## 등장 미션과 수량

대형 보스전을 기준으로 한 실제 배치다. 일반 서브미션의 중간보스는 제외한다. 수량은 세 기체를 합친 값이다. 기존 적을 빼거나 교체하지 않는다.

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

## 구현·검증 결과

코어21개, 시뮬레이터4개, iPhone4개, iPad4개, native Mac5개 검사가 통과했다. 3종 각각에 플레이어20종 무기의 충돌·피해60건을 확인했다. 기존 수동 사격25,200프레임은 이전 버전과 바이트 단위로 같았다. 내가 첫 정식 앱을 플레이하고 수정을 요구한 뒤 Codex가 진입 수정·연사속도10% 조건으로 코어21개, 시뮬레이터2개, 두 실기기·Mac 각1개를 추가 검사했다.

아이폰과 아이패드에 일반·치트 앱을 수정본으로 다시 설치하고 실행했다. 기존 앱이나 저장을 지우지 않았다. 아이폰은 무선, 아이패드는 유선으로 설치했다. 작업 브랜치는 `feat/lux-lunae-fighters`이며 develop/main에 병합하지 않았다.

## 원본 파일과 조사 자료

- [F-22 PNG](media/lux-lunae/F22-gold.png), [Su-57 PNG](media/lux-lunae/Su57-gold.png), [MiG-31 PNG](media/lux-lunae/MiG31-gold.png)
- [내장 imagegen 프롬프트](media/lux-lunae/prompts.json), [참조·파일 해시](media/lux-lunae/manifest.json)
- [원본 헬기·무기·맵 수치](evidence/lux-lunae-inventory-20261027.txt), [추출 코드](evidence/lux-lunae-inventory-20261027.cpp)
- [세부 구현 계획](lux-lunae-plan-20261027.md)

[금색 유리 편집 프롬프트](media/lux-lunae/gold-prompt.txt), [최종 PNG 해시](media/lux-lunae/gold-manifest.json), [실행 증거](evidence/lux-lunae-implementation-20261027.json). 위 큰 그림은 제품 PNG이며 아래는 실제 기기 GPU 캡처다.

## 실제 기기 화면과 진입 수정

처음에는 플레이어 앞 화면 안에 증원했기 때문에 기체가 갑자기 나타났다. 생성 위치를 오른쪽 화면 밖으로 옮겼다. 비행으로 화면 안에 들어온 뒤 기존 이동·공격을 시작한다. 진입 중에는 사격하지 않고, 일시정지하면 기체도 멈춘다. 보스전 경계에서 기체 위치를 안쪽으로 강제 이동시키지 않는다.

![아이패드 기존 헬기와 신규3종 크기·금색 유리 비교](media/lux-lunae/ipad-size.png)

위부터 Heli001, Heli006, F-22, Su-57, MiG-31이다. 각 기체의 수평·기울어진 상태를 같은 GPU 화면에 표시했다.

![아이패드 화면 밖 진입 첫 구간](media/lux-lunae/ipad-entry-030.png)

![아이패드 화면 밖 진입 후속 구간](media/lux-lunae/ipad-entry-060.png)

![아이패드 보스와 증원 전투](media/lux-lunae/ipad-combat.png)

![아이폰 보스와 증원 전투](media/lux-lunae/iphone-combat.png)

검사는8대 증원과 보스가 함께 있는 조건이다. 갱신·GL 처리시간은420회 표본으로 기록했다. 표시 주기와 대기를 포함한 실제 FPS 측정은 아니며, 수치는 실행 증거에 보존했다. 사격 구간의 게임 저장 호출은0회였다. 장시간 발열·메모리 추적과 추가 적을 포함한 정식 난이도 전체 캠페인 검사는 남아 있다.
