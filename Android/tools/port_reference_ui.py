"""One-time syntax translation of the frozen Apple HUD/menu into Android C++.
Outputs are Android-owned. Never writes Sources/. Review semantic changes separately.
"""
from pathlib import Path
import re
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'Android/app/src/main/cpp'
def messages(s):
 out='';i=0
 while i<len(s):
  if s[i]=='"':
   j=i+1
   while j<len(s):
    if s[j]=='\\':j+=2;continue
    if s[j]=='"':break
    j+=1
   out+=s[i:j+1];i=j+1;continue
  if s[i:i+2] in ('//','/*'):
   end=s.find('\n' if s[i:i+2]=='//' else '*/',i+2)
   end=len(s) if end<0 else end+(0 if s[i:i+2]=='//' else 2)
   out+=s[i:end];i=end;continue
  array=s[i:i+2]=='@['
  if s[i]!='[' and not array:
   out+=s[i];i+=1;continue
  start=i+1+int(array);j=start;depth=1;quote=False
  while j<len(s) and depth:
   c=s[j]
   if c=='\\' and quote:j+=2;continue
   if c=='"':quote=not quote
   if not quote:
    if c=='[':depth+=1
    if c==']':depth-=1
   j+=1
  raw=s[start:j-1];inner=messages(raw)
  if array:out+='{'+inner+'}';i=j;continue
  match=re.match(r'^(self|_media|NSString|text|name)\s+(\w+)(.*)$',inner,re.S)
  if not match:out+='['+inner+']';i=j;continue
  obj,method,tail=match.groups();args=[]
  if tail.startswith(':'):
   tail=tail[1:];depth=0;quote=False;cuts=[];k=0
   while k<len(tail):
    c=tail[k]
    if c=='\\' and quote:k+=2;continue
    if c=='"':quote=not quote
    if not quote:
     if c in '([{':depth+=1
     if c in ')]}':depth-=1
     if depth==0 and c.isspace():
      m=re.match(r'\s+\w+:(?!:)',tail[k:])
      if m:cuts.append((k,k+len(m[0])));k+=len(m[0]);continue
    k+=1
   st=0
   for a,b in cuts:args.append(tail[st:a].strip());st=b
   args.append(tail[st:].strip())
  if obj=='NSString':expr=('fmt('+','.join(args)+')') if method=='stringWithFormat' else ('std::string('+','.join(args)+')')
  elif method=='isEqualToString':expr='('+obj+'=='+args[0]+')'
  elif method=='characterAtIndex':expr=obj+'.at('+args[0]+')'
  else:expr=('this->' if obj=='self' else obj+'->')+method+'('+','.join(args)+')'
  out+=expr;i=j
 return out

def convert(s):
 s=messages(s).replace('@"','"').replace('self.','this->').replace('[self]','[this]')
 s=re.sub(r'NSString\s*\*', 'std::string ',s)
 s=re.sub(r'NSArray<std::string\s*>\s*\*','std::vector<std::string> ',s)
 s=s.replace('NSMutableArray<NSArray<std::string > *> *_slotTexts','std::array<std::vector<std::string>,3> _slotTexts')
 s=re.sub(r'\b(BOOL|NSUInteger|unichar)\b',lambda m:{'BOOL':'bool','NSUInteger':'size_t','unichar':'char'}[m[0]],s)
 s=re.sub(r'\bYES\b','true',s);s=re.sub(r'\bNO\b','false',s)
 s=s.replace('text.length','text.size()').replace('names[j].UTF8String','names[j].c_str()').replace('_media.resourceScope','_media->resourceScope')
 s=re.sub(r'(names\[\d+\])\.UTF8String',r'\1.c_str()',s)
 return s

def run(name):
 src=(ROOT/'Sources/iOS'/f'{name}.mm').read_text()
 ns=src[src.index('namespace {')+len('namespace {'):src.index('@implementation')].rstrip()
 ns=ns[:ns.rfind('}')]
 a=ns.index('bool originalButtonContains');b=ns.index('\n}',a)+2;ns=ns[:a]+ns[b:]
 start=src.index('{',src.index('@implementation'));end=src.index('\n}',start)
 state=src[start+1:end]
 ns=convert(ns).replace('int glyph(', 'inline int glyph(')
 state=state.replace('NSMutableArray<NSArray<NSString *> *> *_slotTexts;', 'std::array<std::vector<std::string>,3> _slotTexts;')
 state=convert(state).replace('HMMedia','Media')
 # ObjC instance storage starts zeroed. Preserve that initialization explicitly.
 state=re.sub(r';', '{};',state)
 # Multiple declarators cannot receive a single trailing initializer.
 lines=[]
 for line in state.splitlines():
  m=re.match(r'(\s*)(int|bool|double|float)\s+([^;]+)\{\};',line)
  if m and ',' in m[3]:line=m[1]+m[2]+' '+', '.join(v.strip()+'{}' for v in m[3].split(','))+';'
  lines.append(line)
 state='\n'.join(lines).replace('_positions, _targets{}','_positions{}, _targets{}')
 body=src[end+2:src.index('@end')]
 methods=[]
 pattern=re.compile(r'-\s*\(([^)]+)\)\s*(\w+)([^\{]*)\{')
 matches=list(pattern.finditer(body))
 for idx,m in enumerate(matches):
  ret,method,signature=m.groups();params=re.findall(r':\s*\(([^)]+)\)\s*(\w+)',signature)
  text=body[m.end()-1:matches[idx+1].start() if idx+1<len(matches) else len(body)].rstrip()
  # Comments before the next method remain within the previous textual block.
  if method=='initWithMedia':continue
  ret=convert(ret).replace('NSNumber *','std::optional<int>')
  args=', '.join(convert(t)+' '+n for t,n in params)
  text=convert(text)
  if method=='changeWeaponForSlot':text=text.replace('return nil','return std::nullopt').replace('return @(itemSlot->i32(chargeOffset));','return itemSlot->i32(chargeOffset);')
  methods.append(ret+' '+method+'('+args+') '+text)
 cls='HUD' if name=='HMGameHUD' else 'Menus'
 if cls=='HUD':
  ctor='HUD(Media* media):_media(media){_layoutGroup=-1;_pressed=-1;_startTime=_reloadTime=_weaponTime=-1;_menuX.fill(-240);_hpDest=0;_upgradeX=-240;}\nstd::function<void(int,int)> button;std::function<void()> upgradeClosed;'
  snap=(ROOT/'Sources/iOS/HMGameHUD.h').read_text();snap=snap[snap.index('struct HMGameHUDSnapshot'):snap.index('@interface')];ns+=convert(snap)
 else:ctor='Menus(Media* media,heavymach::SaveData* save,const heavymach::GameData* data):_media(media),_save(save),_data(data){_canvasWidth=480;_safeRect=CGRectMake(0,0,480,320);_pressed=-1;}\nstd::function<int32_t()> random31; std::function<void(int)> changeScreen,slotSelected;std::function<void(int,int,int,int)> startGame;std::function<void()> saveChanged,previewPlayerCreated;'
 out='#pragma once\n#include "Media.hpp"\n#include "HMButtonState.hpp"\n#include "GameSession.hpp"\n#include "HUDConfiguration.hpp"\n#include "GlobalInfo.hpp"\n#include "Visuals.hpp"\n#include "Motion.hpp"\n#include <map>\n#include <array>\n#include <vector>\n#include <numbers>\n#include <ctime>\nnamespace hm {\n'+ns+'\nclass '+cls+' {\npublic:\n'+state+'\n'+ctor+'\n'+'\n'.join(methods)+'\n};\n}\n'
 (OUT/(cls+'.hpp')).write_text(out)
run('HMGameHUD');run('HMMenus')
