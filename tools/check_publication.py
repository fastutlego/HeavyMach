#!/usr/bin/env python3
"""Check reader-facing local links, the shared introduction, and source copies."""
import hashlib
import json
import re
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
FILES = ['README.md', 'CONTRIBUTING.md', 'LICENSE-SCOPE.md', 'ASSET-LICENSES.md',
         'THIRD_PARTY_NOTICES.md', 'CREDITS.md', 'CHANGELOG.md', 'docs/RESTORATION.md',
         'docs/README.md', 'docs/developer-background.md', 'docs/public/publication-inventory.md',
         'docs/public/features.md', 'docs/public/gameplay-guide.md',
         'docs/public/build-and-install.md', 'docs/public/architecture.md',
         'docs/public/troubleshooting.md', 'docs/public/development-history.md',
         'docs/public/validation.md', 'docs/public/implementation-status.md',
         'docs/public/github-repository-plan-20261028.md']


def check(root):
    errors = []
    links = 0
    for name in FILES:
        path = root / name
        if not path.is_file():
            errors.append(f'Missing document: {name}')
            continue
        # Fence examples can contain sample paths that are not document links.
        content = re.sub(r'^```.*?^```[^\n]*', '', path.read_text(), flags=re.M | re.S)
        for target in re.findall(r'\]\((<[^>]+>|[^\s)]+)(?:\s+"[^"]*")?\)', content):
            url = urlsplit(target.strip('<>'))
            if url.scheme or url.netloc or not url.path:
                continue
            links += 1
            dest = path.parent / unquote(url.path)
            if not dest.exists():
                errors.append(f'{name}: missing local link {target}')
    first = (root / 'README.md').read_text().split('\n\n')[1:4]
    second = (root / 'docs/RESTORATION.md').read_text().split('\n\n')[1:4]
    if len(first) != 3 or first != second or '꿈' not in first[1]:
        errors.append('README and restoration introduction differ')
    sources = root / 'docs/references/developer-background'
    manifest = json.loads((sources / 'source-manifest.json').read_text())
    entries = manifest['sources'] + [manifest['user_statement']] + manifest['user_followups']
    for item in entries:
        expected = item.get('copy_sha256', item.get('sha256'))
        if hashlib.sha256((sources / item['copy']).read_bytes()).hexdigest() != expected:
            errors.append(f'Changed preserved source: {item["copy"]}')
    return errors, links


if __name__ == '__main__':
    errors, count = check(ROOT)
    for error in errors:
        print('FAIL:', error)
    if not errors:
        print(f'PASS: {len(FILES)} reader documents, {count} local links, shared introduction and source hashes')
    raise SystemExit(bool(errors))
