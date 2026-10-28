# 글자 없는 앱 아이콘

현재 iPhone·iPad 아이콘의 Episode 1과 HEAVY MACH 글자를 제거했다. 내장 image_gen으로 글자에 가려진 전차·궤도·지면을 복원했다. 원본은 before.png, 생성 결과는 master.png, 지시문은 prompt.json이다.

`python3 tools/prepare_app_icons.py`는 이 원본에서 sips로 크기만 바꿔 Apple 공용 Assets.xcassets와 Android PNG를 만든다. iOS·iPadOS는1024px, Mac은16~1024px를 사용한다. Android의48/72/96/144/192px와 스토어512px는 APK 구현 때 연결할 파일이다. adaptive icon의 분리 레이어·마스크 검사는 Android 작업에 남는다.

게임 타이틀 화면·원본 Icon.png는 유지한다. 정식·치트 앱 모두 같은 새 그림을 사용하며 앱 이름으로 구별한다.
