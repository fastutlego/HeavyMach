#!/usr/bin/env python3
"""Export immutable HeavyMach 1.13 resources into native-build inputs.

Unknown record fields remain offset-labelled. Never invent gameplay meanings.
"""
import argparse
import collections
import json
import plistlib
import shutil
import struct
import subprocess
import zlib
from pathlib import Path
from resource_codecs import category, parse_node, pngdecode, sha, walk


def dump(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + '\n')


def pngencode(width, height, pixels):
    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff)
    raw = b''.join(b'\0' + pixels[y * width * 4:(y + 1) * width * 4] for y in range(height))
    return b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0)) + chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b'')


def cstring(data):
    return data.split(b'\0', 1)[0].decode('ascii')


def export_record(node, filename):
    b = bytes.fromhex(node['data_hex'])
    record = dict(node)
    record['children'] = [export_record(child, filename) for child in node['children']]
    fields = {}
    if filename == 'PartsInfo.mch' and node['type'] == 1:
        assert len(b) == 27 + b[26] * 8
        fields = {'name': cstring(b[:24]), 'partsType': b[24], 'tileCnt': b[25],
                  'socketCnt': b[26], 'sockets': [list(struct.unpack_from('<2f', b, 27 + i * 8)) for i in range(b[26])]}
    elif filename == 'WeaponInfo.mch' and node['type'] == 1:
        assert len(b) == 156
        fields = {'name': cstring(b[:24]), 'wpnType': struct.unpack_from('<h', b, 24)[0],
                  'dmg': struct.unpack_from('<h', b, 26)[0], 'pierce': b[28], 'shotCnt': b[29],
                  **{name: struct.unpack_from('<f', b, offset)[0] for name, offset in
                     [('shotDly',30), ('delay',34), ('spd',38), ('spdRand',42), ('aimRand',46), ('g',50), ('liveTm',56)]},
                  'charge': struct.unpack_from('<h', b, 54)[0], 'rotWpn': b[60], 'uiTile': b[61],
                  'bulletTile': cstring(b[62:86]), 'bulletTileNo': struct.unpack_from('<h', b, 86)[0],
                  'effectType': cstring(b[88:104]), 'effectTile': cstring(b[104:128]),
                  'effectTileNo': struct.unpack_from('<h', b, 128)[0],
                  'shell': struct.unpack_from('<h', b, 130)[0], 'fireSFX': cstring(b[132:156])}
    elif filename == 'MachInfo.mch' and node['type'] == 3:
        assert len(b) == 33
        fields = {'name': cstring(b[:24]), 'hpMax': struct.unpack_from('<i', b, 24)[0],
                  'giveExp': struct.unpack_from('<i', b, 28)[0], 'machType': b[32]}
    elif filename == 'MachInfo.mch' and node['type'] == 4:
        fields = {'name': cstring(b[:24]), 'baseSocket': b[24], 'zOrder': b[25]}
    elif filename.endswith('.map'):
        if node['type'] == 2:
            assert len(b) == 16
            fields = dict(zip(['left','top','right','bottom'], struct.unpack('<4f', b)))
        elif node['type'] == 6:
            assert len(b) == 8
            fields = {'position': list(struct.unpack('<2f', b))}
        elif node['type'] == 8:
            length = struct.unpack_from('<I', b)[0]
            assert len(b) == 4 + length + 8 + 1
            fields = {'name_byte_length': length, 'name': cstring(b[4:4 + length]),
                      'position': list(struct.unpack_from('<2f', b, 4 + length)), 'layer_flag': b[-1], 'layer': 1 if b[-1] == 0 else 2}
        elif node['type'] == 9:
            fields = {'background_image': cstring(b)}
        elif node['type'] == 11:
            assert len(b) == 32
            fields = {'name': cstring(b[:24]), 'is_player_marker_ignored_by_original_reader': cstring(b[:24]) == 'Player', 'position': list(struct.unpack_from('<2f', b, 24))}
    if fields:
        record['fields'] = fields
    return record


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--data-only', action='store_true', help='Reuse validated image/audio derivatives while enriching JSON schemas')
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--output', type=Path, default=Path(__file__).resolve().parent.parent / 'Resources')
    args = parser.parse_args()
    source, out = args.source.resolve(), args.output.resolve()
    assert source != out and source not in out.parents and out not in source.parents
    for sub in ['Images', 'Audio', 'Data', 'RawData', 'Assets.xcassets/AppIcon.appiconset']:
        (out / sub).mkdir(parents=True, exist_ok=True)
    inventory, images, data = [], {}, {}
    previous = {}
    if args.data_only:
        images = json.loads((out / 'images.json').read_text())
        previous = {e['path']: e for e in json.loads((out / 'inventory.json').read_text())['files']}
    for path in sorted(source.rglob('*')):
        if not path.is_file():
            continue
        relative = str(path.relative_to(source))
        blob = path.read_bytes()
        entry = {'path': relative, 'size': len(blob), 'sha256': sha(blob), 'category': category(relative)}
        if args.data_only and entry['category'] == 'image':
            prior = previous[relative]
            assert prior['sha256'] == entry['sha256']
            assert sha((out / prior['derived']).read_bytes()) == prior['derived_sha256']
            inventory.append(prior)
            continue
        if entry['category'] == 'image':
            target = out / 'Images' / (relative if relative.endswith('.png') else relative + '.png')
            target.parent.mkdir(parents=True, exist_ok=True)
            if blob.startswith(b'\x89PNG\r\n\x1a\n'):
                meta, rgba = pngdecode(blob)
                if meta['cgbi']:
                    straight = bytearray(rgba)
                    for i in range(0, len(straight), 4):
                        alpha = straight[i + 3]
                        for k in range(3):
                            straight[i + k] = min(255, (straight[i + k] * 255 + alpha // 2) // alpha) if alpha else 0
                    rgba = bytes(straight)
                target.write_bytes(pngencode(meta['width'], meta['height'], rgba))
            else:
                subprocess.run(['/usr/bin/sips', '-s', 'format', 'png', str(path), '--out', str(target)], check=True, stdout=subprocess.DEVNULL)
                meta, rgba = pngdecode(target.read_bytes())
            check, check_pixels = pngdecode(target.read_bytes())
            assert (check['width'], check['height']) == (meta['width'], meta['height'])
            entry['derived'] = str(target.relative_to(out))
            entry['derived_sha256'] = sha(target.read_bytes())
            images[relative] = {'path': entry['derived'], 'width': meta['width'], 'height': meta['height'],
                                'source_sha256': entry['sha256'], 'rgba_sha256': sha(check_pixels),
                                'source_cgbi': meta['cgbi'], 'alpha': 'straight RGBA',
                                'full_rect_xywh': [0, 0, meta['width'], meta['height']]}
        elif entry['category'] == 'audio':
            shutil.copyfile(path, out / 'Audio' / relative)
            entry['derived'] = 'Audio/' + relative
            assert sha((out / entry['derived']).read_bytes()) == entry['sha256']
        elif entry['category'] == 'game_data':
            node, end = parse_node(blob)
            assert end == len(blob)
            exported = export_record(node, relative)
            result = {'source': relative, 'source_sha256': entry['sha256'], 'source_size': len(blob),
                      'consumed_bytes': end, 'root': exported}
            if relative.endswith('.map'):
                nodes = list(walk(exported))
                result['bounds'] = next(n['fields'] for n in nodes if n['type'] == 2)
                result['background'] = next(n['fields']['background_image'] for n in nodes if n['type'] == 9)
                result['objects'] = [n['fields'] for n in nodes if n['type'] == 8]
                result['collision_paths'] = [[c['fields']['position'] for c in n['children'] if c['type'] == 6] for n in nodes if n['type'] == 5 and n['children']]
                result['enemies'] = [n['fields'] for n in nodes if n['type'] == 11 and not n['fields']['is_player_marker_ignored_by_original_reader']]
            else:
                result['records'] = [n['fields'] for n in exported['children']]
            data[relative] = result
            dump(out / 'Data' / (path.stem + '.json'), result)
            shutil.copyfile(path, out / 'RawData' / relative)
            entry['derived'] = 'Data/' + path.stem + '.json'
        inventory.append(entry)
    # Every source file is hashed again after processing to prove preservation.
    for entry in inventory:
        assert sha((source / entry['path']).read_bytes()) == entry['sha256'], entry['path']
    parts = data['PartsInfo.mch']['root']['children']
    sprites = []
    atlases = {}
    def atlas(name, columns, rows, evidence):
        if name not in images:
            return None
        image = images[name]
        width, height = image['width'] / columns, image['height'] / rows
        frames = [{'index': i, 'rect_xywh_top_left': [(i % columns) * width, (i // columns) * height, width, height]}
                  for i in range(columns * rows)]
        key = f'{name}:{columns}x{rows}'
        atlases[key] = {'image': image['path'], 'columns': columns, 'rows': rows, 'frames': frames, 'evidence': evidence}
        return key
    for part in parts:
        fields = part['fields']
        name = fields['name'] + '.png' if fields['tileCnt'] else 'PartsDummy.png'
        image = images.get(name)
        key = atlas(name, max(fields['tileCnt'], 1), 1, 'Evidence/parts-sprite.json')
        bounds = None
        if image:
            width, height = image['width'] / max(fields['tileCnt'], 1), image['height']
            sx, sy = fields['sockets'][0]
            bounds = [-width/2-sx, -height/2-sy, width/2-sx, height/2-sy]
        sprites.append({'name': fields['name'], 'parts_record_offset': part['offset'], 'image': image,
                        'sockets': fields['sockets'], 'partsType': fields['partsType'], 'tileCnt': fields['tileCnt'],
                        'atlas': key, 'local_bounds_ltrb': bounds,
                        'frame_rectangles': atlases[key]['frames'] if key else None,
                        'frame_status': 'native-reader-confirmed' if key else 'referenced image absent in original package'})
    for record in data['WeaponInfo.mch']['records']:
        name = record['bulletTile']
        if name != 'none':
            cols, rows = (4,4) if name.startswith('EnemyBullet0') else (2,4) if name.startswith('EnemyMissile') else (4,8)
            atlas(name, cols, rows, 'Evidence/weapon-reader.json')
    dump(out / 'atlases.json', atlases)
    dump(out / 'images.json', images)
    dump(out / 'sprites.json', sprites)
    dump(out / 'inventory.json', {'source_bundle': str(source), 'files': inventory,
                                  'counts': dict(collections.Counter(e['category'] for e in inventory)),
                                  'source_unchanged_after_export': True})
    dump(out / 'Assets.xcassets/Contents.json', {'info': {'author': 'xcode', 'version': 1}})
    icon = out / 'Assets.xcassets/AppIcon.appiconset/AppIcon1024.png'
    subprocess.run(['/usr/bin/sips', '-z', '1024', '1024', str(out / 'Images/Icon.png'), '--out', str(icon)], check=True, stdout=subprocess.DEVNULL)
    dump(icon.parent / 'Contents.json', {'images': [{'filename': icon.name, 'idiom': 'universal', 'platform': 'ios', 'size': '1024x1024'}], 'info': {'author': 'xcode', 'version': 1}})
    metadata = plistlib.loads((source / 'Info.plist').read_bytes())
    dump(out / 'provenance.json', {'bundle_identifier': metadata['CFBundleIdentifier'], 'version': metadata.get('CFBundleVersion'),
         'source_executable_sha256': sha((source / 'HeavyMach').read_bytes()),
         'resource_inventory': 'inventory.json', 'original_modified': False,
         'codec_provenance': '2026-10-24 rea-ready/comparison/assets/compare_resources.py pngdecode/parse_node reused',
         'transformations': ['CgBI raw DEFLATE and PNG filter decoding', 'BGRA to RGBA', 'CgBI premultiplied RGB to straight alpha with integer rounding',
                             'JPEG decoded by macOS sips; dimensions unchanged', 'Original Icon.png JPEG512 decoded and resized to PNG1024 for the app icon'],
         'limitations': ['Unknown semantic fields remain offset-labelled.', 'Exact original CgBI RGB before premultiplication cannot be recovered losslessly at partial alpha.',
                        'Resource export alone does not establish restored gameplay.']})
    print(json.dumps({'files': len(inventory), 'images': len(images), 'data_files': len(data), 'parts': len(sprites),
                      'parts_without_matching_image': sum(s['image'] is None for s in sprites), 'source_unchanged': True}))


if __name__ == '__main__':
    main()
