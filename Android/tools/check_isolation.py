#!/usr/bin/env python3
from pathlib import Path
import hashlib,json,os,subprocess,sys
ROOT=Path(__file__).resolve().parents[2]
baseline=json.loads((ROOT/'Android/protected-baseline.json').read_text())
errors=[]
documentation_changes=[]
# README status updates do not enter any Apple build target.
editorial={"README.md","README.en.md"}
for entry in baseline['files']:
    name,digest=entry['path'],entry['sha256']
    p=ROOT/name
    try:data=os.readlink(p).encode() if p.is_symlink() else p.read_bytes()
    except OSError:errors.append(f'Missing: {name}');continue
    if hashlib.sha256(data).hexdigest()!=digest:
        if name in editorial:documentation_changes.append(name)
        else:errors.append(f'Changed: {name}')
# Check additions as well as the baseline, including untracked build inputs.
changed=subprocess.check_output(['git','diff','--name-only',baseline['base']],cwd=ROOT,text=True).splitlines()
changed+=subprocess.check_output(['git','ls-files','--others','--exclude-standard'],cwd=ROOT,text=True).splitlines()
allowed=('Android/','docs/','.agents/')
for p in changed:
    if not p.startswith(allowed) and p not in editorial and p!='AGENTS.md':errors.append(f'Outside Android/docs/skills: {p}')
result={'base':baseline['base'],'protected_files':len(baseline['files']),'unchanged_protected_files':len(baseline['files'])-len(documentation_changes),'documentation_only_changes':documentation_changes,'errors':sorted(set(errors))}
print(json.dumps(result,indent=2));sys.exit(bool(errors))
