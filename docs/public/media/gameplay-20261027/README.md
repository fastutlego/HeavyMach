# 2026-10-27 플레이 영상 후보

[후보 이미지·클립과 배치안](../../gameplay-media-20261027.md) · [장면 명세](media-scenes.json)

사용자가 제공한 두 녹화본에서 추출한 이미지 8장과 H.264/AAC 클립 3개다. 공개용 최종 선택 전 후보이며 게임 화면을 재생성하거나 합성하지 않았다. PNG는 원본 회전을 반영한 2736×1260이다. 각 장면의 실제 PNG 프레임 PTS, 클립 구간, SHA-256, 캡션·대체 텍스트·사용 위치를 명세에서 찾을 수 있다.

원본은 저장소의 `build/media/user-gameplay-20261027/original/`에 있고 Git에서 제외한다. Downloads 원본도 보존했다. 재추출에는 원본 두 파일과 FFmpeg/ffprobe가 필요하다. 게임 앱의 빌드에는 이 영상이 필요하지 않다.

```sh
python3 tools/extract_gameplay_media.py
```

기존 산출물은 덮어쓰지 않고 검사한다. 새로 추출하려면 별도 checkout이나 출력 백업 후 실행한다. 원본 해시가 명세와 다르면 중단한다. 검토용 프레임 목록과 로그는 `build/media/user-gameplay-20261027/review/`에 있다.

GitHub에서는 대표 이미지와 MP4 파일 링크를 사용한다. 보고 사이트에는 동일 파일을 올려 영상 재생기를 제공한다. Apps·Mags 원고와 GitHub 최종 배치는 별도 게시 작업이다.
