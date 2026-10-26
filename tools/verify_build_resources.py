#!/usr/bin/env python3
"""Verify the materialized assets needed to build and test the iOS apps."""
import argparse
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def verify(root, manifest):
    errors = []
    expected = {item['path']: item for item in manifest['files']}
    actual = set()
    for directory in manifest['roots']:
        folder = root / directory
        if not folder.is_dir() or folder.is_symlink():
            errors.append(f'Missing or linked asset directory: {directory}')
            continue
        for path in folder.rglob('*'):
            if path.is_symlink():
                errors.append(f'Linked asset: {path.relative_to(root)}')
            elif path.is_file() and path.suffix != '.log':
                actual.add(path.relative_to(root).as_posix())
    for name in sorted(set(expected) - actual):
        errors.append(f'Missing: {name}')
    for name in sorted(actual - set(expected)):
        errors.append(f'Not in manifest: {name}')
    for name in sorted(actual & set(expected)):
        data = (root / name).read_bytes()
        if data.startswith(b'version https://git-lfs.github.com/spec/v1\n'):
            errors.append(f'Git LFS pointer instead of asset: {name}; run git lfs pull')
        elif len(data) != expected[name]['bytes'] or hashlib.sha256(data).hexdigest() != expected[name]['sha256']:
            errors.append(f'Content mismatch: {name}')
    return errors


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--manifest', type=Path, default=ROOT / 'docs/build-resources.json')
    args = parser.parse_args()
    manifest = json.loads(args.manifest.read_text())
    errors = verify(args.root.resolve(), manifest)
    if errors:
        for message in errors[:20]:
            print(message)
        print(f'FAIL: {len(errors)} asset errors')
        return 1
    print(f"PASS: {len(manifest['files'])} build/test assets; all SHA-256 hashes match")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
