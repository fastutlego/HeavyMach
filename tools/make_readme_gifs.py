#!/usr/bin/env python3
"""Create or verify README GIFs from the selected MP4 clips. Keeps originals intact."""
import hashlib
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DEST = ROOT / 'docs/public/media/gameplay-20261027'

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    manifest = DEST / 'media-scenes.json'
    data = json.loads(manifest.read_text())
    scenes = {scene['id']: scene for scene in data['scenes']}
    records = []
    for scene_id, offset, duration in [('01-auto-aim', .5, 5), ('08-jump-clip', 0, 6)]:
        scene = scenes[scene_id]
        source = DEST / (scene_id + '.mp4')
        expected = next(item for item in scene['output_metadata'] if item['path'] == source.name)
        assert sha(source) == expected['sha256'], f'MP4 hash mismatch: {source.name}'
        dest = DEST / (scene_id + '.gif')
        filters = '[0:v]fps=12,scale=768:-1:flags=lanczos,split[a][b];[a]palettegen=max_colors=128:stats_mode=diff[p];[b][p]paletteuse=dither=bayer:bayer_scale=3'
        if not dest.exists():
            subprocess.run(['ffmpeg', '-hide_banner', '-loglevel', 'error', '-n', '-ss', str(offset), '-t', str(duration), '-i', str(source), '-filter_complex', filters, '-loop', '0', str(dest)], check=True)
        probe = json.loads(subprocess.check_output(['ffprobe', '-v', 'error', '-show_streams', '-show_format', '-of', 'json', str(dest)]))
        stream = probe['streams'][0]
        assert stream['width'] == 768 and stream['height'] == 354
        assert abs(float(probe['format']['duration']) - duration) < .1
        assert dest.stat().st_size < 10 * 1024 * 1024
        subprocess.run(['ffmpeg', '-v', 'error', '-i', str(dest), '-f', 'null', '-'], check=True)
        previous = next((a for a in data.get('animations', []) if a['path'] == dest.name), None)
        if previous:
            assert sha(dest) == previous['sha256'], f'Existing GIF hash mismatch: {dest.name}'
        records.append(dict(path=dest.name, scene_id=scene_id, source_clip=source.name, source_id=scene['source_id'], source_start_seconds=scene['start_seconds'] + offset, source_end_seconds=scene['start_seconds'] + offset + duration, duration_seconds=float(probe['format']['duration']), fps=12, width=768, height=354, speed=1, audio=False, filter=filters, sha256=sha(dest), bytes=dest.stat().st_size, full_decode='passed'))
        print(f'{dest.name}: {dest.stat().st_size} bytes, verified')
    data['animations'] = records
    data['status'] = 'selected_for_github'
    manifest.write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n')

if __name__ == '__main__':
    main()
