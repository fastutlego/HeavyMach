---
name: integrate-game-art
description: HeavyMach 아트 변경에서 원본 에셋·생성 배경·실제 게임 합성을 대조한다.
---

# 게임 아트 연결

HeavyMach 아트는 `docs/canonical.md`, `docs/game-design.md`, `docs/rendering-policy.md`, `docs/coordinate-policy.md`와 `Art/generation-20261025.json`·`Art/menu-joins-20261025.json`를 읽고 원본 중앙 에셋·좌우 확장·실제 앱 합성을 대조한다. 이미지 생성이 필요하면 현재 제공된 imagegen 스킬을 사용한다.

원본 이미지는 수정하지 않고 별도 파생 에셋을 만든다. 변경한 메뉴·게임 화면의 실제 합성과 터치 위치를 검증하고, 결과는 이 저장소의 present-game-artifacts와 docs/report-site.md를 따른다.

분리 전 Unity 절차는 [보존 원문](references/unity-original.md)에 있다.
