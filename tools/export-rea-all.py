#!/usr/bin/env python3
"""Export REA analysis evidence, not executable source. Resumable; original immutable."""
import argparse,hashlib,json,os,queue,subprocess,threading,time,tomllib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]; OUT=ROOT/'Analysis'
ORIGINAL=Path('/Users/sokol/Documents/Codex/2026-10-24/rea-ready/HeavyMach-1.13-extracted/Payload/HeavyMach.app/HeavyMach')
CORE=Path('/Users/sokol/Documents/Codex/2026-10-24/rea-ready/comparison/core')
TITLE=CORE.parents[2]/'heavymach-recovery/title-analysis'
SHA='97166198d701dd929243196a85e81182d7716915cdd50dec2acdc1918d10deeb'
def write(path,value):
 path.parent.mkdir(parents=True,exist_ok=True);tmp=path.with_suffix(path.suffix+'.tmp');tmp.write_text(json.dumps(value,indent=2,ensure_ascii=False)+'\n');tmp.replace(path)
def evidence(raw):
 if 'evidence_id' in raw:return raw
 if 'structuredContent' in raw and 'evidence_id' in raw['structuredContent']:return raw['structuredContent']
 for c in raw.get('content',[]):
  if c.get('type')=='text':
   try:
    v=json.loads(c['text'])
    if isinstance(v,dict):return v
   except ValueError:pass
 raise ValueError('No JSON Evidence')
def filename(addr):return addr.replace(':','_')
def extract(raw,op,addr):
 d=evidence(raw)
 if d.get('subject',{}).get('digest',{}).get('sha256')!=SHA:raise ValueError('Source digest mismatch')
 value=d.get('normalized_result');value=value if isinstance(value,str) else d.get('raw_result',{}).get('value')
 if not isinstance(value,str) or not value.strip():raise ValueError('No code text')
 suffix='.pseudo.c' if op=='procedure_pseudo_code' else '.asm'
 prefix='/* REA DECOMPILER PSEUDOCODE: not compilable reconstructed source. */\n' if suffix=='.pseudo.c' else '; REA disassembly of original ARM/Thumb code.\n'
 (OUT/'functions'/f'{filename(addr)}{suffix}').write_text(prefix+value)
 return d
class Client:
 def __init__(self):
  cfg=tomllib.loads(Path('/Users/sokol/.codex/config.toml').read_text())['mcp_servers']['rea'];env=os.environ.copy();env.update(cfg.get('env',{}));self.seq=0;self.q=queue.Queue()
  self.p=subprocess.Popen([cfg['command'],*cfg.get('args',[])],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(OUT/'export.stderr').open('a'),text=True,bufsize=1,env=env)
  def reader():
   for line in self.p.stdout:
    try:self.q.put(json.loads(line))
    except ValueError:continue
   self.q.put(None)
  threading.Thread(target=reader,daemon=True).start()
  self.request('initialize',{'protocolVersion':'2024-11-05','capabilities':{},'clientInfo':{'name':'heavymach-original-export','version':'1'}})
  self.send({'jsonrpc':'2.0','method':'notifications/initialized'})
 def send(self,v):self.p.stdin.write(json.dumps(v)+'\n');self.p.stdin.flush()
 def request(self,method,params,timeout=450):
  self.seq+=1;i=self.seq;self.send({'jsonrpc':'2.0','id':i,'method':method,'params':params});end=time.monotonic()+timeout
  while True:
   v=self.q.get(timeout=max(.01,end-time.monotonic()))
   if v is None:raise RuntimeError('MCP closed')
   if v.get('id')==i:
    if 'error' in v:raise RuntimeError(v['error'])
    return v['result']
 def call(self,tool,args={}):return self.request('tools/call',{'name':tool,'arguments':args})
 def close(self):
  self.p.stdin.close()
  try:self.p.wait(timeout=15)
  except subprocess.TimeoutExpired:self.p.terminate();self.p.wait(timeout=10)
def main():
 assert hashlib.sha256(ORIGINAL.read_bytes()).hexdigest()==SHA
 for d in ['functions','evidence','metadata']:(OUT/d).mkdir(parents=True,exist_ok=True)
 invraw=json.load(open(CORE/'original-procedures.json'));inv=evidence(invraw);procs=inv['raw_result'];write(OUT/'inventory-evidence.json',invraw)
 write(OUT/'functions.json',{'source_sha256':SHA,'inventory_evidence_id':inv['evidence_id'],'count':len(procs),'procedures':procs})
 # Reuse only matching original Evidence, never the different Assault artifact.
 reused=0
 for folder in [CORE,TITLE]:
  if not folder.exists():continue
  for p in folder.glob('*.json'):
   try:
    raw=json.load(open(p));d=evidence(raw);op=d.get('operation');addr=d.get('parameters',{}).get('procedure','')
    if op not in ['procedure_pseudo_code','procedure_assembly'] or not addr.startswith('0x'):continue
    kind='pseudo' if op=='procedure_pseudo_code' else 'assembly';extract(raw,op,addr)
    write(OUT/'evidence'/f'{filename(addr)}.{kind}.json',raw);reused+=1
   except (ValueError,KeyError,AttributeError,TypeError):pass
 print(f'inventory={len(procs)} reused={reused}',flush=True)
 c=Client();failures=[];completed=0
 try:
  catalog=c.request('tools/list',{});write(OUT/'tools-input-schemas.json',{t['name']:t['inputSchema'] for t in catalog['tools']})
  r=c.call('open_binary',{'path':str(ORIGINAL),'provider_id':'ghidra','snapshot_path':str(CORE/'original.snapshot.json')});write(OUT/'open.json',r)
  if r.get('isError'):raise RuntimeError('open_binary failed; see open.json')
  for name in ['get_objc_classes','get_objc_protocols','inspect_macho']:
   if name not in [t['name'] for t in catalog['tools']]:continue
   try:
    dest=OUT/'metadata'/f'{name}.json'
    if not dest.exists():write(dest,c.call(name,{}))
   except Exception as e:failures.append({'operation':name,'error':str(e)})
  # Export real guest bodies first, imports have no body in this executable.
  ordered=sorted(procs,key=lambda p:(p['procedure']['external'],p['procedure']['thunk'],p['address']))
  records=[]
  for p in ordered:
   rec=dict(p);addr=p['address'];rec['executable_source']=False
   if p['procedure']['external']:
    rec['status']='external_import_no_body';records.append(rec);continue
   rec['exports']={}
   for op,kind in [('procedure_pseudo_code','pseudo'),('procedure_assembly','assembly')]:
    path=OUT/'evidence'/f'{filename(addr)}.{kind}.json'
    try:
     if path.exists():raw=json.load(open(path))
     else:raw=c.call(op,{'procedure':addr});write(path,raw)
     d=extract(raw,op,addr);rec['exports'][kind]={'evidence_id':d['evidence_id'],'limitations':d.get('limitations',[]),'status':'exported'}
    except Exception as e:
     rec['exports'][kind]={'status':'failed','error':str(e)};failures.append({'address':addr,'name':p['value'],'operation':op,'error':str(e)})
   records.append(rec);completed+=1
   write(OUT/'export-manifest.json',{'source_sha256':SHA,'inventory_count':len(procs),'completed_local_functions':completed,'executable_source':False,'functions':records,'failures':failures})
   if completed%25==0:print(f'exported local={completed}/900 failures={len(failures)}',flush=True)
  write(OUT/'export-manifest.json',{'source_sha256':SHA,'inventory_count':len(procs),'completed_local_functions':completed,'executable_source':False,'functions':records,'failures':failures})
  write(OUT/'failures.json',failures)
 finally:
  try:write(OUT/'close.json',c.call('close_binary',{}))
  finally:c.close()
 print(f'Done local={completed}, failures={len(failures)}',flush=True)
if __name__=='__main__':main()
