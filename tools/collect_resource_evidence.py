import json,subprocess,selectors,time,tomllib,os,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parent.parent/'Resources/Evidence'
cfg=tomllib.loads(Path('/Users/sokol/.codex/config.toml').read_text())['mcp_servers']['rea']
env=os.environ.copy();env.update(cfg.get('env',{}))
p=subprocess.Popen([cfg['command'],*cfg['args']],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=(ROOT/'mcp-check.stderr').open('w'),text=True,bufsize=1,env=env)
s=selectors.DefaultSelector(); s.register(p.stdout,selectors.EVENT_READ)
def send(obj):
 p.stdin.write(json.dumps(obj)+'\n');p.stdin.flush()
def req(i,method,params,timeout=450):
 send({'jsonrpc':'2.0','id':i,'method':method,'params':params})
 end=time.monotonic()+timeout
 while time.monotonic()<end:
  if not s.select(max(0,end-time.monotonic())):break
  line=p.stdout.readline()
  if not line:raise RuntimeError('Server exited')
  v=json.loads(line)
  if v.get('id')==i:
   if 'error' in v:raise RuntimeError(v['error'])
   return v['result']
 raise TimeoutError(method)
try:
 init=req(1,'initialize',{'protocolVersion':'2024-11-05','capabilities':{},'clientInfo':{'name':'rea-readiness-check','version':'1.0'}})
 send({'jsonrpc':'2.0','method':'notifications/initialized'})
 catalog=req(2,'tools/list',{})
 # The separate stdio session never changes another agent's active target.
 if len(sys.argv)==1:
  for t in catalog['tools']:
   if t['name'] in ['open_binary','procedure_pseudo_code','close_binary','list_procedures','binary_overview','binary_session']:print(json.dumps(t))
 else:
  scenario=json.loads(Path(sys.argv[1]).read_text())
  for i,step in enumerate(scenario,10):
   r=req(i,'tools/call',{'name':step['tool'],'arguments':step.get('arguments',{})})
   (ROOT/step['output']).write_text(json.dumps(r,indent=2)+'\n')
   print(step['tool'],'isError:',r.get('isError',False),flush=True)
   if r.get('isError'):raise RuntimeError('Tool error; inspect '+step['output'])
finally:
 p.stdin.close()
 try:p.wait(timeout=15)
 except subprocess.TimeoutExpired:p.terminate();p.wait(timeout=10)
