#!/usr/bin/env python3
"""Prepare tagged HeavyMach sources/resources and publish a verified GitHub release."""
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import posixpath
import re
import subprocess
import tarfile
import tempfile
from urllib.parse import quote, unquote, urlsplit
import zipfile

from verify_build_resources import verify as verify_resources

ROOT = Path(__file__).resolve().parents[1]
POINTER = b'version https://git-lfs.github.com/spec/v1\n'
NOTICES = ['LICENSE', 'LICENSE-SCOPE.md', 'ASSET-LICENSES.md',
           'THIRD_PARTY_NOTICES.md', 'CREDITS.md']


class ReleaseError(RuntimeError):
    pass


def run(*args, cwd=ROOT):
    result = subprocess.run(args, cwd=cwd, capture_output=True)
    if result.returncode:
        # Keep credentials/URLs out of failures. The command can be rerun locally.
        raise ReleaseError(f'{args[0]} {args[1]} failed (exit {result.returncode})')
    return result.stdout


def git(root, *args):
    return run('git', *args, cwd=root).decode().strip()


def sha(path):
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def write_json(path, value):
    temporary = path.with_suffix('.tmp')
    temporary.write_text(json.dumps(value, indent=2, ensure_ascii=False) + '\n')
    temporary.replace(path)


def tag_info(root, version):
    if not re.fullmatch(r'v\d+\.\d+(?:\.\d+)?', version):
        raise ReleaseError('Version must look like v0.17')
    ref = 'refs/tags/' + version
    if git(root, 'cat-file', '-t', ref) != 'tag':
        raise ReleaseError('An annotated release tag is required')
    commit = git(root, 'rev-parse', ref + '^{commit}')
    git(root, 'merge-base', '--is-ancestor', commit, 'refs/remotes/origin/main')
    if len(git(root, 'rev-list', '--parents', '-n', '1', commit).split()) != 3:
        raise ReleaseError('Release tag must identify a main merge commit')
    origin = git(root, 'remote', 'get-url', 'origin')
    match = re.fullmatch(r'(?:git@github.com:|https://github.com/)([\w.-]+/[\w.-]+?)(?:\.git)?', origin)
    if not match:
        raise ReleaseError('origin must be a GitHub repository URL')
    return commit, match[1]


def safe_name(name):
    path = PurePosixPath(name)
    if path.is_absolute() or '..' in path.parts or not path.parts:
        raise ReleaseError(f'Unsafe archive path: {name}')
    return path


def extract_notes(changelog, version, repo, paths):
    heading = re.search(r'^## ' + re.escape(version) + r'(?=\s|$)[^\n]*\n', changelog, re.M)
    if not heading:
        raise ReleaseError(f'CHANGELOG has no section for {version}')
    body = re.split(r'^## ', changelog[heading.end():], maxsplit=1, flags=re.M)[0]
    body = re.sub(r'\s*<a id="[^"]+"></a>\s*$', '', body).strip()
    if not body:
        raise ReleaseError('Release notes are empty')
    links = []

    def convert(match):
        target = match[1]
        parsed = urlsplit(target)
        if parsed.scheme or parsed.netloc:
            return match[0]
        path = posixpath.normpath(unquote(parsed.path) or 'CHANGELOG.md')
        safe_name(path)
        if path not in paths:
            raise ReleaseError(f'Release note link missing at {version}: {path}')
        links.append(path)
        url = f'https://github.com/{repo}/blob/{version}/{quote(path, safe="/")}'
        if parsed.query:
            url += '?' + parsed.query
        if parsed.fragment:
            url += '#' + parsed.fragment
        return '](' + url + ')'

    body = re.sub(r'\]\(([^\s)]+)\)', convert, body)
    # Reference-style definitions use the same tag conversion.
    body = re.sub(r'^(\s*\[[^\]]+\]:\s*)(\S+)',
                  lambda m: m[1] + re.sub(r'\]\(([^\s)]+)\)', convert, '](' + m[2] + ')')[2:-1],
                  body, flags=re.M)
    return body + '\n', sorted(set(links))


def export_tag(root, commit, destination):
    entries = run('git', 'ls-tree', '-rz', '--full-tree', commit, cwd=root).split(b'\0')
    media = None
    records = {}
    process = subprocess.Popen(['git', 'cat-file', '--batch'], cwd=root,
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    try:
        for entry in filter(None, entries):
            metadata, encoded_name = entry.split(b'\t', 1)
            mode, kind, oid = metadata.decode().split()
            name = encoded_name.decode()
            safe_name(name)
            if kind != 'blob' or mode not in ('100644', '100755', '120000'):
                raise ReleaseError(f'Unsupported tracked entry: {name}')
            process.stdin.write(oid.encode() + b'\n')
            process.stdin.flush()
            header = process.stdout.readline().split()
            if len(header) != 3 or header[1] != b'blob':
                raise ReleaseError(f'Cannot read tagged blob: {name}')
            data = process.stdout.read(int(header[2]))
            if process.stdout.read(1) != b'\n':
                raise ReleaseError('Truncated Git blob stream')
            path = destination / name
            path.parent.mkdir(parents=True, exist_ok=True)
            if mode == '120000':
                target = data.decode()
                resolved = posixpath.normpath(posixpath.join(posixpath.dirname(name), target))
                if PurePosixPath(target).is_absolute():
                    raise ReleaseError(f'Absolute symlink: {name}')
                safe_name(resolved)
                path.symlink_to(target)
                records[name] = {'mode': mode, 'target': target}
                continue
            if data.startswith(POINTER):
                pointer = re.fullmatch(POINTER + rb'oid sha256:([a-f0-9]{64})\nsize (\d+)\n', data)
                if not pointer:
                    raise ReleaseError(f'Invalid LFS pointer: {name}')
                digest, size = pointer[1].decode(), int(pointer[2])
                if media is None:
                    env = git(root, 'lfs', 'env')
                    media = Path(re.search(r'^LocalMediaDir=(.+)$', env, re.M)[1])
                obj = media / digest[:2] / digest[2:4] / digest
                if not obj.is_file():
                    raise ReleaseError('Missing LFS object; run git lfs fetch origin <version>, then prepare again')
                data = obj.read_bytes()
                if len(data) != size or hashlib.sha256(data).hexdigest() != digest:
                    raise ReleaseError(f'LFS content mismatch: {name}')
            path.write_bytes(data)
            path.chmod(int(mode[-3:], 8))
            records[name] = {'mode': mode, 'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
    finally:
        process.stdin.close()
        process.stdout.close()
        process.wait()
    for name, record in records.items():
        if 'target' in record and not (destination / name).exists():
            raise ReleaseError(f'Broken tagged symlink: {name}')
    return records


def verify_extracted(root, records):
    actual = {str(p.relative_to(root)) for p in root.rglob('*') if p.is_file() or p.is_symlink()}
    if actual != set(records):
        raise ReleaseError('Archive file list differs from tagged file list')
    for name, record in records.items():
        path = root / name
        if 'target' in record:
            if not path.is_symlink() or os.readlink(path) != record['target']:
                raise ReleaseError(f'Symlink mismatch: {name}')
        elif (path.is_symlink() or path.stat().st_size != record['bytes'] or
              sha(path) != record['sha256'] or path.stat().st_mode & 0o777 != int(record['mode'][-3:], 8)):
            raise ReleaseError(f'Archive content/mode mismatch: {name}')
        else:
            with path.open('rb') as stream:
                if stream.read(len(POINTER)) == POINTER:
                    raise ReleaseError(f'LFS pointer left in archive: {name}')
    manifest = json.loads((root / 'docs/build-resources.json').read_text())
    errors = verify_resources(root, manifest)
    if errors:
        raise ReleaseError('; '.join(errors[:5]))
    return len(manifest['files'])


def prepare(root, version, output):
    commit, repo = tag_info(root, version)
    if output.exists():
        raise ReleaseError(f'Output already exists: {output}')
    output.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='.release-', dir=output.parent) as temp:
        staging = Path(temp)
        source = staging / ('HeavyMach-' + version)
        source.mkdir()
        print(f'Exporting {version} ({commit[:12]})', flush=True)
        records = export_tag(root, commit, source)
        body, links = extract_notes((source / 'CHANGELOG.md').read_text(), version, repo, records)
        install = 'docs/public/build-and-install.md'
        if install not in records:
            raise ReleaseError('Tagged installation guide missing')
        body += f'\n[설치 방법](https://github.com/{repo}/blob/{version}/{install})\n\n'
        body += '첨부: 소스·자원 압축본, 고지 묶음, SHA256SUMS.txt.\n'
        versions = []
        current = tuple(map(int, version[1:].split('.')))
        for tag in git(root, 'tag', '--merged', commit).splitlines():
            if re.fullmatch(r'v\d+\.\d+(?:\.\d+)?', tag) and tuple(map(int, tag[1:].split('.'))) < current:
                versions.append(tag)
        if versions:
            previous = max(versions, key=lambda tag: tuple(map(int, tag[1:].split('.'))))
            body += f'\n[이전 버전과 비교](https://github.com/{repo}/compare/{previous}...{version})\n'
        result = staging / 'output'
        result.mkdir()
        (result / 'release-notes.md').write_text(body)
        archive = result / f'HeavyMach-{version}-source-with-assets.tar.gz'
        with tarfile.open(archive, 'w:gz') as tar:
            tar.add(source, arcname=source.name)
        notices = NOTICES + sorted(n for n in records if n.startswith('LICENSES/'))
        with zipfile.ZipFile(result / f'HeavyMach-{version}-notices.zip', 'w', zipfile.ZIP_DEFLATED) as z:
            for name in notices:
                z.write(source / name, arcname=name)
        unpack = staging / 'unpack'
        with tarfile.open(archive) as tar:
            tar.extractall(unpack, filter='data')
        count = verify_extracted(unpack / source.name, records)
        with zipfile.ZipFile(result / f'HeavyMach-{version}-notices.zip') as z:
            if set(z.namelist()) != set(notices) or any(z.read(n) != (source / n).read_bytes() for n in notices):
                raise ReleaseError('Notices archive mismatch')
        files = sorted(result.glob('*.gz')) + sorted(result.glob('*.zip'))
        (result / 'SHA256SUMS.txt').write_text(''.join(f'{sha(p)}  {p.name}\n' for p in files))
        files.append(result / 'SHA256SUMS.txt')
        manifest = {'schema_version': 1, 'version': version, 'commit': commit, 'repository': repo,
                    'notes_sha256': sha(result / 'release-notes.md'),
                    'artifacts': {p.name: {'sha256': sha(p), 'bytes': p.stat().st_size} for p in files},
                    'checks': {'tracked_files': len(records), 'build_assets': count, 'note_links': links,
                               'unpacked_hashes': True, 'notices_content': True, 'lfs_pointers': 0}}
        write_json(result / 'manifest.json', manifest)
        result.rename(output)
    print(f'Prepared {output}: {len(records)} files, {count} build assets', flush=True)
    return manifest


class GitHub:
    def __init__(self, root, repo):
        self.root, self.repo = root, repo

    def cli(self, *args):
        return run('gh', *args, cwd=self.root)

    def remote_commit(self, version):
        refs = git(self.root, 'ls-remote', 'origin', f'refs/tags/{version}', f'refs/tags/{version}^{{}}')
        values = dict(line.split()[::-1] for line in refs.splitlines())
        return values.get(f'refs/tags/{version}^{{}}')

    def find(self, version):
        # Listing keeps authentication/network failures distinct from absence (404).
        pages = json.loads(self.cli('api', '--paginate', '--slurp', f'repos/{self.repo}/releases?per_page=100'))
        return next((r for page in pages for r in page if r['tag_name'] == version), None)

    def create(self, version, notes):
        self.cli('release', 'create', version, '--repo', self.repo, '--verify-tag', '--draft',
                 '--title', 'HeavyMach ' + version, '--notes-file', str(notes))

    def upload(self, version, paths):
        self.cli('release', 'upload', version, *map(str, paths), '--repo', self.repo)

    def publish(self, version):
        self.cli('release', 'edit', version, '--repo', self.repo, '--draft=false', '--latest')


def verify_remote(release, manifest, body, draft):
    if not release or release['tag_name'] != manifest['version'] or release['draft'] != draft:
        raise ReleaseError('Remote release state/tag mismatch')
    if (release.get('name') != 'HeavyMach ' + manifest['version'] or
            release.get('prerelease') or release['body'].strip() != body.strip()):
        raise ReleaseError('Remote release body/prerelease mismatch')
    assets = {a['name']: a for a in release['assets']}
    if set(assets) != set(manifest['artifacts']):
        raise ReleaseError('Remote release attachment list mismatch')
    for name, info in manifest['artifacts'].items():
        asset = assets[name]
        if asset['size'] != info['bytes'] or asset.get('digest') != 'sha256:' + info['sha256'] or asset.get('state') != 'uploaded':
            raise ReleaseError(f'Remote attachment checksum/state mismatch: {name}')


def publish(root, version, output, check_only=False, resume=False, github=None):
    manifest = json.loads((output / 'manifest.json').read_text())
    commit, repo = tag_info(root, version)
    if (manifest['version'], manifest['commit'], manifest['repository']) != (version, commit, repo):
        raise ReleaseError('Prepared version/commit/repository differs from current tag')
    notes = output / 'release-notes.md'
    if sha(notes) != manifest['notes_sha256']:
        raise ReleaseError('Prepared notes changed; prepare again in a new output directory')
    expected = {f'HeavyMach-{version}-source-with-assets.tar.gz',
                f'HeavyMach-{version}-notices.zip', 'SHA256SUMS.txt'}
    if set(manifest['artifacts']) != expected:
        raise ReleaseError('Prepared attachment list is incomplete')
    for name, info in manifest['artifacts'].items():
        if safe_name(name).name != name:
            raise ReleaseError('Artifact must be a direct child of output directory')
        if (output / name).stat().st_size != info['bytes'] or sha(output / name) != info['sha256']:
            raise ReleaseError(f'Prepared attachment changed: {name}')
    api = github or GitHub(root, repo)
    if api.remote_commit(version) != commit:
        raise ReleaseError('Remote tag differs from prepared commit')
    existing = api.find(version)
    receipt_path = output / 'publish-result.json'
    receipt = {'version': version, 'commit': commit, 'stage': 'preflight', 'published': False}
    body = notes.read_text()
    if existing and not existing['draft']:
        raise ReleaseError('This version already has a published Release; it will not be changed')
    if existing:
        prior = json.loads(receipt_path.read_text()) if receipt_path.exists() else {}
        if not resume or prior.get('release_id') != existing['id'] or prior.get('commit') != commit:
            raise ReleaseError('Existing draft: resume only with its original output directory and --resume')
        if existing['body'].strip() != body.strip():
            raise ReleaseError('Draft notes changed remotely')
        known = manifest['artifacts']
        for asset in existing['assets']:
            info = known.get(asset['name'])
            if not info or asset.get('digest') != 'sha256:' + info['sha256'] or asset['size'] != info['bytes'] or asset.get('state') != 'uploaded':
                raise ReleaseError('Draft contains an unexpected/incomplete attachment; inspect the draft')
    if check_only:
        print('Publish preflight passed; no GitHub changes')
        return receipt
    try:
        if not existing:
            receipt['stage'] = 'create-draft'
            write_json(receipt_path, receipt)
            api.create(version, notes)
            existing = api.find(version)
            if not existing or not existing['draft']:
                raise ReleaseError('Created draft not found')
        receipt.update(release_id=existing['id'], stage='upload')
        write_json(receipt_path, receipt)
        uploaded = {a['name'] for a in existing['assets']}
        pending = [output / n for n in manifest['artifacts'] if n not in uploaded]
        if pending:
            api.upload(version, pending)
        receipt['stage'] = 'verify-draft'
        write_json(receipt_path, receipt)
        verify_remote(api.find(version), manifest, body, True)
        if api.remote_commit(version) != commit:
            raise ReleaseError('Remote tag changed during upload')
        receipt.update(stage='publish', published=None)
        write_json(receipt_path, receipt)
        api.publish(version)
        receipt['stage'] = 'verify-public'
        write_json(receipt_path, receipt)
        public = api.find(version)
        verify_remote(public, manifest, body, False)
        if api.remote_commit(version) != commit:
            raise ReleaseError('Remote tag changed during publication')
        receipt.update(stage='complete', published=True, url=public['html_url'])
        write_json(receipt_path, receipt)
        print(public['html_url'])
        return receipt
    except Exception as error:
        receipt['error'] = str(error)
        write_json(receipt_path, receipt)
        raise


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=['prepare', 'publish'])
    parser.add_argument('version')
    parser.add_argument('--output', type=Path)
    parser.add_argument('--check', action='store_true', help='publish preflight only; no remote writes')
    parser.add_argument('--resume', action='store_true', help='resume the draft recorded in this output directory')
    args = parser.parse_args()
    output = (args.output or ROOT / 'build/releases' / args.version).resolve()
    try:
        if args.command == 'prepare':
            if args.check or args.resume:
                parser.error('--check and --resume apply to publish')
            prepare(ROOT, args.version, output)
        else:
            publish(ROOT, args.version, output, args.check, args.resume)
    except (ReleaseError, OSError, ValueError, KeyError) as error:
        parser.exit(1, f'Release failed: {error}\n')


if __name__ == '__main__':
    main()
