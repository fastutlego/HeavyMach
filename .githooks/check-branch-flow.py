#!/usr/bin/env python3
"""Check main merge parent and immutable release tags in pre-push input."""
import subprocess, sys
ZERO = '0' * 40

def git(*args):
    return subprocess.check_output(['git', *args], text=True, stderr=subprocess.DEVNULL).strip()

def check(rows):
    updates = {r[2]: r[1] for r in rows}
    errors = []
    for local_ref, new, remote_ref, old in rows:
        if remote_ref in ('refs/heads/main', 'refs/heads/develop') and new == ZERO:
            errors.append('main/develop 삭제 금지')
            continue
        if remote_ref == 'refs/heads/main':
            parents = git('rev-list', '--parents', '-n', '1', new).split()[1:]
            # A new repository can publish an empty root before its first PR.
            bootstrap = old == ZERO and not parents and not git('ls-tree', new)
            if bootstrap:
                continue
            develop = updates.get('refs/heads/develop') or git('rev-parse', 'refs/heads/develop')
            if len(parents) != 2 or parents[1] != develop:
                errors.append('main에는 현재 develop의 merge commit만 push할 수 있습니다')
            if old != ZERO and subprocess.call(['git','merge-base','--is-ancestor',old,new], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL):
                errors.append('공유 main의 이력을 다시 쓰지 않습니다')
        if remote_ref.startswith('refs/tags/v'):
            if new == ZERO or (old != ZERO and old != new):
                errors.append('릴리스 태그 삭제·재지정 금지')
                continue
            main = updates.get('refs/heads/main') or git('rev-parse','refs/heads/main')
            commit = git('rev-parse', new + '^{}')
            parents = git('rev-list','--parents','-n','1',commit).split()[1:]
            if commit != main or len(parents) != 2:
                errors.append('새 릴리스 태그는 main의 현재 병합 커밋을 가리켜야 합니다')
    return errors

if __name__ == '__main__':
    try:
        rows = [line.split() for line in sys.stdin if line.strip()]
        if any(len(r) != 4 for r in rows):
            raise ValueError('Invalid pre-push input')
        errors = check(rows)
    except (ValueError, subprocess.CalledProcessError) as e:
        errors = ['브랜치 기준을 확인할 수 없습니다: ' + str(e)]
    for error in errors:
        print(error, file=sys.stderr)
    sys.exit(bool(errors))
