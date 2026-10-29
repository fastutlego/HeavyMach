#!/usr/bin/env python3
"""Export the text-free shared illustration to Apple catalogs and Android PNGs."""
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / 'Art/AppIcon/20261029/master.png'
CATALOG = ROOT / 'Resources/Assets.xcassets/AppIcon.appiconset'


def resize(destination, pixels):
    destination.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(['/usr/bin/sips', '-z', str(pixels), str(pixels), str(MASTER),
                    '--out', str(destination)], check=True, stdout=subprocess.DEVNULL)


def main():
    entries = [{'filename': 'AppIcon1024.png', 'idiom': 'universal',
                'platform': 'ios', 'size': '1024x1024'}]
    for pixels in [16, 32, 64, 128, 256, 512, 1024]:
        resize(CATALOG / f'AppIcon{pixels}.png', pixels)
    for size in [16, 32, 128, 256, 512]:
        for scale in [1, 2]:
            entries.append({'filename': f'AppIcon{size * scale}.png', 'idiom': 'mac',
                            'scale': f'{scale}x', 'size': f'{size}x{size}'})
    (CATALOG / 'Contents.json').write_text(json.dumps(
        {'images': entries, 'info': {'author': 'xcode', 'version': 1}}, indent=2) + '\n')
    for bucket, pixels in [('mdpi', 48), ('hdpi', 72), ('xhdpi', 96),
                           ('xxhdpi', 144), ('xxxhdpi', 192), ('store', 512)]:
        folder = 'mipmap-' + bucket if bucket != 'store' else bucket
        resize(MASTER.parent / 'android' / folder / 'ic_launcher.png', pixels)
    path = ROOT / 'docs/build-resources.json'
    manifest = json.loads(path.read_text())
    entries = {entry['path']: entry for entry in manifest['files']}
    for asset in sorted(CATALOG.rglob('*')):
        if asset.is_file():
            data = asset.read_bytes()
            name = str(asset.relative_to(ROOT))
            entry = {'path': name, 'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
            if name in entries:
                entries[name].update(entry)
            else:
                manifest['files'].append(entry)
    manifest['counts']['Resources/Assets.xcassets'] = sum(
        1 for entry in manifest['files'] if entry['path'].startswith('Resources/Assets.xcassets/'))
    path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n')
    print('Apple app icon catalog and Android PNG exports updated.')


if __name__ == '__main__':
    main()
