#!/usr/bin/env python3
"""Check owned iOS sources, Xcode membership, includes, docs and compatibility patches.
Read-only except --output; compatibility patches are applied only in a temporary folder.
Requires macOS plutil and Git. Does not run the game or certify behavioral parity.
"""
import argparse
import hashlib
import json
import re
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXTENSIONS = {'.cpp', '.mm', '.m', '.c'}


def run(*args, cwd=ROOT):
    return subprocess.check_output(args, cwd=cwd)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output', type=Path, required=True)
    args = ap.parse_args()
    errors = []
    project = json.loads(run('plutil', '-convert', 'json', '-o', '-',
                             'iOS/HeavyMachRestored.xcodeproj/project.pbxproj'))
    objects = project['objects']
    refs = {}

    def visit(oid, parent):
        obj = objects[oid]
        tree = obj.get('sourceTree', '<group>')
        if tree in ('SDKROOT', 'BUILT_PRODUCTS_DIR', 'DEVELOPER_DIR'):
            return
        base = ROOT / 'iOS' if tree == 'SOURCE_ROOT' else parent
        path = (base / obj.get('path', '')).resolve()
        if obj['isa'] == 'PBXFileReference':
            refs[oid] = path
            if not path.exists():
                errors.append(f'Missing Xcode file: {path}')
        for child in obj.get('children', []):
            visit(child, path)

    visit(objects[project['rootObject']]['mainGroup'], ROOT / 'iOS')
    source_files = sorted(p for p in (ROOT / 'Sources').rglob('*') if p.is_file())
    compile_files = {p for p in source_files if p.suffix in EXTENSIONS}
    test_files = {p for p in (ROOT / 'Tests/iOS').rglob('*') if p.suffix in EXTENSIONS}
    targets = {}
    for obj in objects.values():
        if obj['isa'] != 'PBXNativeTarget':
            continue
        actual = []
        for phase_id in obj['buildPhases']:
            phase = objects[phase_id]
            if phase['isa'] == 'PBXSourcesBuildPhase':
                actual += [refs[objects[f]['fileRef']] for f in phase['files']]
        expected = test_files if obj['name'] == 'HeavyMachRenderTests' else compile_files
        missing, extra = expected - set(actual), set(actual) - expected
        duplicates = len(actual) - len(set(actual))
        if missing or extra or duplicates:
            errors.append(f"Target {obj['name']}: missing={missing}, extra={extra}, duplicates={duplicates}")
        targets[obj['name']] = [str(p.relative_to(ROOT)) for p in sorted(actual)]
    if set(targets) != {'HeavyMachRestored', 'HeavyMachValidation', 'HeavyMachRenderTests'}:
        errors.append('Unexpected Xcode target set')
    include_count = 0
    for path in source_files + sorted(test_files):
        if path.suffix not in EXTENSIONS | {'.h', '.hpp'}:
            continue
        for name in re.findall(r'^\s*#\s*(?:include|import)\s*"([^"]+)"', path.read_text(), re.M):
            include_count += 1
            candidates = [path.parent / name, ROOT / 'Sources/Core' / name, ROOT / 'Sources/iOS' / name]
            if not any(p.is_file() for p in candidates):
                errors.append(f'Unresolved quoted include: {path.relative_to(ROOT)}: {name}')
    source_map = (ROOT / 'docs/ios-source-map.md').read_text()
    undocumented = [str(p.relative_to(ROOT)) for p in source_files
                    if f'`{p.relative_to(ROOT)}`' not in source_map]
    if undocumented:
        errors.append(f'Sources absent from document map: {undocumented}')
    tracked = set(run('git', 'ls-files', '-z').decode().split('\0'))
    untracked_sources = [str(p.relative_to(ROOT)) for p in source_files if str(p.relative_to(ROOT)) not in tracked]
    if untracked_sources:
        errors.append(f'Untracked source files: {untracked_sources}')
    preserved_trees = {}
    for folder in ('Sources', 'iOS', 'Art', 'Tests'):
        baseline = run('git', 'rev-parse', f'v0.9:{folder}').decode().strip()
        current = run('git', 'rev-parse', f'HEAD:{folder}').decode().strip()
        preserved_trees[folder] = {'v0.9': baseline, 'HEAD': current, 'equal': baseline == current}
        if baseline != current:
            errors.append(f'{folder} differs from v0.9: review before updating audit baseline')
    compat = ROOT / 'Compatibility/compat-source'
    patch_results = []
    for repo, patch in ((compat, 'heavymach.patch'),
                        (compat / 'fresh-port/20-touchHLE-src/touchHLE/vendor/rust-sdl2', 'rust-sdl2.patch')):
        patch_path = ROOT / 'Compatibility/patches' / patch
        status = run('git', 'status', '--porcelain=v1', '--untracked-files=all', cwd=repo).decode().splitlines()
        changed = run('git', 'diff', '--name-only', '-z', 'HEAD', cwd=repo).decode().strip('\0').split('\0')
        changed = [x for x in changed if x]
        with tempfile.TemporaryDirectory(prefix='hm-compat-audit-') as temp:
            tmp = Path(temp)
            for name in changed:
                dest = tmp / name
                dest.parent.mkdir(parents=True, exist_ok=True)
                dest.write_bytes(run('git', 'show', f'HEAD:{name}', cwd=repo))
            run('git', 'apply', str(patch_path), cwd=tmp)
            mismatches = [name for name in changed if not (tmp / name).exists()
                          or (tmp / name).read_bytes() != (repo / name).read_bytes()]
        if mismatches:
            errors.append(f'Compatibility patch mismatch: {mismatches}')
        patch_results.append({'repository': str(repo.relative_to(ROOT)),
                              'head': run('git', 'rev-parse', 'HEAD', cwd=repo).decode().strip(),
                              'status': status, 'modified_files': changed,
                              'patch': str(patch_path.relative_to(ROOT)),
                              'patch_sha256': sha(patch_path.read_bytes()),
                              'patched_contents_match_checkout': not mismatches})
    report = {'head': run('git', 'rev-parse', 'HEAD').decode().strip(),
              'status': run('git', 'status', '--porcelain=v1', '--untracked-files=all').decode().splitlines(),
              'source_count': len(source_files), 'targets': targets,
              'quoted_includes_resolved': include_count,
              'sources': [{'path': str(p.relative_to(ROOT)), 'sha256': sha(p.read_bytes())} for p in source_files],
              'product_trees': preserved_trees, 'compatibility': patch_results, 'errors': errors}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n')
    print(json.dumps({'sources': len(source_files), 'targets': {k: len(v) for k, v in targets.items()},
                      'includes': include_count, 'errors': errors}, ensure_ascii=False))
    raise SystemExit(bool(errors))


if __name__ == '__main__':
    main()
