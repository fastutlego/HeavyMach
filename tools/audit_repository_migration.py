#!/usr/bin/env python3
"""Compare every migrated file with the preserved monorepo; never modify inputs.

Private per-file inventories go to the explicitly selected output directory.
Build caches remain at their original paths and are inventoried, not imported.
"""
import argparse
import hashlib
import json
import os
import re
import stat
import subprocess
from collections import Counter
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]


def git(root, *args):
    return subprocess.check_output(['git', *args], cwd=root)


def tree(root):
    result = {}
    for row in git(root, 'ls-tree', '-rz', 'HEAD').split(b'\0'):
        if row:
            meta, path = row.split(b'\t', 1)
            mode, kind, oid = meta.decode().split()
            result[path.decode()] = (mode, kind, oid)
    return result


def digest(path):
    if path.is_symlink():
        return hashlib.sha256(os.readlink(path).encode()).hexdigest()
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()


def walk(root):
    for parent, dirs, files in os.walk(root):
        dirs[:] = sorted(d for d in dirs if d != '.git')
        for name in list(dirs):
            p = Path(parent) / name
            if p.is_symlink():
                yield p
                dirs.remove(name)
        for name in sorted(files):
            yield Path(parent) / name


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--legacy-root', type=Path, required=True)
    ap.add_argument('--output', type=Path, required=True)
    ap.add_argument('--site-root', type=Path)
    args = ap.parse_args()
    old, out = args.legacy_root.resolve(), args.output.resolve()
    if out == old or old in out.parents:
        ap.error('Do not write audit output inside the preserved source checkout')
    errors, rows = [], []
    originals = tree(old)
    for name, (mode, kind, oid) in originals.items():
        if name.startswith('HeavyMach/'):
            target = name[len('HeavyMach/'):]
        elif name.startswith('iOS Proj/HeavyMachRecovery/'):
            target = 'Compatibility/' + name[len('iOS Proj/HeavyMachRecovery/'):]
        else:
            continue
        p = ROOT / target
        record = {'source': name, 'target': target, 'original_mode': mode,
                  'original_blob': oid}
        if not p.exists() and not p.is_symlink():
            record['status'] = 'missing'
            errors.append('Missing migrated file: ' + target)
        else:
            data = os.readlink(p).encode() if p.is_symlink() else p.read_bytes()
            actual = hashlib.sha1(b'blob ' + str(len(data)).encode() + b'\0' + data).hexdigest()
            record['current_blob'] = actual
            same = actual == oid
            if not same:
                original = git(old, 'cat-file', 'blob', oid)
                if original.startswith(b'version https://git-lfs.github.com/spec/v1\n'):
                    fields = dict(line.decode().split(' ', 1) for line in original.splitlines())
                    expected = fields['oid'].removeprefix('sha256:')
                    record.update(storage='git-lfs', original_sha256=expected,
                                  current_sha256=hashlib.sha256(data).hexdigest())
                    same = record['current_sha256'] == expected and len(data) == int(fields['size'])
            record['status'] = 'same' if same else 'changed_review_required'
            executable = bool(p.lstat().st_mode & stat.S_IXUSR)
            if mode in ('100755', '100644') and executable != (mode == '100755'):
                errors.append('Executable mode differs: ' + target)
            # Gameplay and platform sources must not change during this audit.
            if target.startswith(('Sources/', 'Tests/', 'iOS/', 'Art/', 'vendor/')) and not same:
                errors.append('Product bytes changed: ' + target)
        rows.append(record)

    local = []
    pairs = [('Resources', old / 'HeavyMach/Resources', ROOT / 'Resources'),
             ('Analysis', old / 'HeavyMach/Analysis', ROOT / 'Analysis'),
             ('compat-source', old / 'iOS Proj/HeavyMachRecovery/compat-source', ROOT / 'Compatibility/compat-source'),
             ('originals', old / 'iOS Proj/HeavyMachRecovery/originals', ROOT / 'Compatibility/originals')]
    for label, source, destination in pairs:
        if not source.is_dir():
            errors.append('Missing original input directory: ' + str(source))
            continue
        for p in walk(source):
            rel = p.relative_to(source)
            q = destination / rel
            record = {'group': label, 'path': str(rel), 'source_sha256': digest(p),
                      'symlink': p.is_symlink()}
            if not q.exists() and not q.is_symlink():
                record['status'] = 'missing'
                errors.append('Missing local input: ' + label + '/' + str(rel))
            else:
                record['current_sha256'] = digest(q)
                record['status'] = 'same' if record['source_sha256'] == record['current_sha256'] and p.is_symlink() == q.is_symlink() else 'different'
                if record['status'] != 'same':
                    errors.append('Local input differs: ' + label + '/' + str(rel))
                if q.is_symlink() and not q.exists():
                    errors.append('Broken local symlink: ' + label + '/' + str(rel))
            local.append(record)

    related = []
    for p in sorted((old / 'docs').glob('*.md')):
        if re.search(r'heavy.?mach|rea', p.name, re.I):
            q = ROOT / 'docs' / p.name
            related.append({'source': str(p.relative_to(old)), 'target': str(q.relative_to(ROOT)),
                            'source_sha256': digest(p), 'current_sha256': digest(q) if q.exists() else None})
            if not q.is_file():
                errors.append('Missing related root document: ' + p.name)
    for name in ('AGENTS.md', 'README.md'):
        p, q = old / 'iOS Proj' / name, ROOT / 'docs/migration/legacy-ios' / (name + '.txt')
        if not q.is_file() or digest(p) != digest(q):
            errors.append('Missing or changed archived iOS parent document: ' + name)

    links, site_links = [], []
    documents = [ROOT / 'AGENTS.md', ROOT / 'README.md', *sorted((ROOT / 'docs').rglob('*.md')),
                 *sorted((ROOT / 'Compatibility').glob('*.md'))]
    for p in documents:
        body = re.sub(r'```.*?```', '', p.read_text(), flags=re.S)
        for href in re.findall(r'(?<!!)\[[^\]]*\]\(([^)]+)\)', body):
            url = urlsplit(href.strip('<>'))
            if url.scheme or url.netloc or not url.path:
                continue
            # The report overview is rendered into the Site root, not docs/public.
            is_site = p.name == 'report-overview-20261025.md'
            base = args.site_root if is_site else p.parent
            item = {'document': str(p.relative_to(ROOT)), 'target': href}
            (site_links if is_site else links).append(item)
            if base is None:
                errors.append('Supply --site-root to verify generated report links')
            elif not (base / unquote(url.path)).exists():
                errors.append('Broken link: ' + str(item))

    archives = []
    for rel in ('HeavyMach/build', 'HeavyMach/DerivedData', 'HeavyMach/DerivedDataDevice',
                'iOS Proj/HeavyMachRecovery/DerivedData'):
        source = old / rel
        for p in walk(source):
            archives.append({'root': rel, 'path': str(p.relative_to(source)),
                             'bytes': p.lstat().st_size, 'symlink': p.is_symlink()})
    summary = {'legacy_head': git(old, 'rev-parse', 'HEAD').decode().strip(),
               'target_head': git(ROOT, 'rev-parse', 'HEAD').decode().strip(),
               'target_scope': 'current working files, including uncommitted audit fixes',
               'tracked_total': len(rows), 'tracked_status': dict(Counter(r['status'] for r in rows)),
               'local_input_counts': dict(Counter(r['group'] for r in local)),
               'local_input_status': dict(Counter(r['status'] for r in local)),
               'related_root_documents': len(related), 'documents_checked': len(documents),
               'local_links': len(links), 'site_links': len(site_links),
               'retained_archive_entries': len(archives),
               'retained_archive_bytes': sum(r['bytes'] for r in archives),
               'archive_check_scope': 'existence, type and size; no full cache/trace content hashing',
               'errors': errors}
    out.mkdir(parents=True, exist_ok=True)
    for name, value in [('summary', summary), ('tracked-files', rows), ('local-inputs', local),
                        ('related-documents', related), ('retained-archives', archives)]:
        (out / (name + '.json')).write_text(json.dumps(value, ensure_ascii=False, indent=2) + '\n')
    print(json.dumps(summary, ensure_ascii=False, indent=2))
    return bool(errors)


if __name__ == '__main__':
    raise SystemExit(main())
