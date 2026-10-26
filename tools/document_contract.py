#!/usr/bin/env python3
"""Print HeavyMach reading routes and check their structural references.

Does not execute tests, read documents on behalf of an agent, or prove semantics.
"""
import argparse
import fnmatch
import json
import re
import sys
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / 'docs/document-map.json'


def plan(data, tasks, files):
    selected = list(dict.fromkeys(tasks))
    errors = []
    for task in selected:
        if task not in data['domains']:
            errors.append(f'Unknown task: {task}')
    for name in files:
        path = Path(name)
        if path.is_absolute():
            try:
                name = str(path.relative_to(ROOT))
            except ValueError:
                errors.append(f'Outside HeavyMach: {name}')
                continue
        elif name.startswith('HeavyMach/'):
            name = name[len('HeavyMach/'):]
        matched = [key for key, value in data['domains'].items()
                   if any(fnmatch.fnmatchcase(name, pattern) for pattern in value['sources'])]
        if not matched:
            errors.append(f'Unmapped file; classify its requirements: {name}')
        selected.extend(key for key in matched if key not in selected)
    if errors:
        return errors
    print('업무:', ', '.join(selected) or '공통 진입 (업무별 --task 또는 --files 지정)')
    docs = list(data['base_read'])
    for key in selected:
        docs.extend(data['domains'][key]['read'])
    print('\n실제로 읽을 문서:')
    for name in dict.fromkeys(docs):
        print('  ' + str(ROOT / name))
    for key in selected:
        domain = data['domains'][key]
        print('\n' + domain['label'])
        for field in ('sources', 'tests', 'evidence'):
            print(' ', field + ':', ', '.join(domain[field]) or '없음')
    print('\n출력은 읽기·검사 실행 또는 앱 동작 확인을 뜻하지 않습니다.')
    return []


def check(data):
    errors = []
    files = set(data['current_documents'] + data['base_read'])
    for key, domain in data['domains'].items():
        files.update(domain['read'] + domain['tests'] + domain['evidence'])
        for pattern in domain['sources']:
            if not any(p.is_file() for p in ROOT.glob(pattern)):
                errors.append(f'{key}: no source matched {pattern}')
    for name in sorted(files):
        if not (ROOT / name).is_file():
            errors.append('Missing reference: ' + name)
    for name in data['current_documents']:
        path = ROOT / name
        if not path.is_file():
            continue
        for target in re.findall(r'(?<!!)\[[^\]]*\]\(([^)]+)\)', path.read_text()):
            url = urlsplit(target.strip('<>'))
            if url.scheme or url.netloc or not url.path:
                continue
            if not (path.parent / unquote(url.path)).exists():
                errors.append(f'{name}: broken local link {target}')
    if not errors:
        print(f'PASS: {len(data["domains"])} task routes, {len(files)} referenced files, current-document links')
        print('Structural check only; document/code/app semantics require evidence review.')
    return errors


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='command', required=True)
    route = sub.add_parser('plan')
    route.add_argument('--task', action='append', default=[])
    route.add_argument('--files', nargs='+', default=[])
    sub.add_parser('check')
    args = parser.parse_args()
    try:
        data = json.loads(MAP.read_text())
        errors = check(data) if args.command == 'check' else plan(data, args.task, args.files)
    except (OSError, ValueError, KeyError, TypeError) as error:
        errors = [str(error)]
    for error in errors:
        print('ERROR: ' + error, file=sys.stderr)
    return bool(errors)


if __name__ == '__main__':
    sys.exit(main())
