#!/usr/bin/env python3
"""Generate declaration-only ObjC interfaces from original runtime encodings."""
import json,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];DATA=json.load(open(ROOT/'Analysis/objc-layouts.json'));OUT=ROOT/'Sources/OriginalInterfaces';OUT.mkdir(parents=True,exist_ok=True)
structs={}
class Parser:
 def __init__(self,s):self.s=s;self.i=0
 def quote(self):
  assert self.s[self.i]=='"';self.i+=1;j=self.s.index('"',self.i);s=self.s[self.i:j];self.i=j+1;return s
 def type(self):
  s=self.s
  while s[self.i] in 'rnNoORV':self.i+=1
  t=s[self.i];self.i+=1
  if t=='@':
   name=self.quote() if self.i<len(s) and s[self.i]=='"' else None
   return ('object',name)
  if t=='^':return ('pointer',self.type())
  if t=='[':
   j=self.i
   while s[self.i].isdigit():self.i+=1
   n=int(s[j:self.i]);node=self.type();assert s[self.i]==']';self.i+=1;return ('array',n,node)
  if t=='{':
   j=self.i
   while s[self.i] not in '=}':self.i+=1
   name=s[j:self.i];fields=[]
   if s[self.i]=='=':
    self.i+=1
    while s[self.i]!='}':
     field=self.quote() if s[self.i]=='"' else None
     fields.append((field,self.type()))
   self.i+=1
   if fields:
    old=structs.get(name,[])
    if len(fields)>len(old) or sum(n is not None for n,t in fields)>sum(n is not None for n,t in old):structs[name]=fields
   return ('struct',name)
  if t in 'cislqCISLQfdBv*#:':return ('primitive',t)
  raise ValueError((s,self.i,t))
 def all(self):
  a=[]
  while self.i<len(self.s):
   while self.i<len(self.s) and (self.s[self.i].isdigit() or self.s[self.i] in '+-'):self.i+=1
   if self.i<len(self.s):a.append(self.type())
  return a
for c in DATA['classes']:
 for v in c['ivars']:v['node']=Parser(v['type_encoding']).all()[0]
 for m in c['methods']+c['class_methods']:m['nodes']=Parser(m['type_encoding']).all()
primitive={'c':'int8_t','i':'int32_t','s':'int16_t','l':'int32_t','q':'int64_t','C':'uint8_t','I':'uint32_t','S':'uint16_t','L':'uint32_t','Q':'uint64_t','f':'float','d':'double','B':'bool','v':'void','*':'char *','#':'Class',':':'SEL'}
platform={'CGPoint','CGSize','CGRect'}
def name_type(n,disk=False):
 kind=n[0]
 if kind=='object':return 'uint32_t' if disk else (('id'+n[1]) if n[1] and n[1].startswith('<') else ((n[1]+' *') if n[1] else 'id'))
 if kind=='primitive':return 'uint32_t' if disk and n[1] in '*#:' else primitive[n[1]]
 if kind=='struct':return ('HM32' if disk else '')+n[1]
 if kind=='pointer':return 'uint32_t' if disk else name_type(n[1])+' *'
 raise ValueError(n)
def decl(n,name,disk=False):
 if n[0]=='array':return decl(n[2],f'{name}[{n[1]}]',disk)
 return f'{name_type(n,disk)} {name}'
def deps(n):
 if n[0]=='struct':return {n[1]}
 if n[0]=='array':return deps(n[2])
 return set()
def ordered_structs():
 pending=set(structs);done=set();a=[]
 while pending:
  good=sorted(k for k in pending if set().union(*(deps(t) for _,t in structs[k])).issubset(done))
  if not good:raise ValueError('Recursive value structs')
  for k in good:done.add(k);pending.remove(k);a.append(k)
 return a
order=ordered_structs()
header='// Generated from original ARMv6 Objective-C metadata. Declarations only; no implementations.\n#pragma once\n'
lines=[header,'#import <UIKit/UIKit.h>','#import <QuartzCore/QuartzCore.h>','#import <OpenGLES/EAGL.h>','#include <stdint.h>','#include <stdbool.h>','']
lines+=['@protocol EAGLViewDelegate;','@class '+', '.join(c['name'] for c in DATA['classes'])+';','']
for name in order:
 if name not in platform:lines.append(f'typedef struct {name} {name};')
lines+=['']
for name in order:
 if name in platform:continue
 lines.append(f'struct {name} {{')
 for i,(field,t) in enumerate(structs[name]):lines.append('    '+decl(t,field or f'field{i}')+';'+(' // Field name absent from runtime encoding.' if field is None else ''))
 lines+=['};','']
(OUT/'OriginalTypes.h').write_text('\n'.join(lines)+'\n')
# Fixed-width ARM32 evidence representations, including pointer addresses as integers.
def layout(n):
 k=n[0]
 if k in ('object','pointer'):return (4,4)
 if k=='primitive':
  c=n[1];size={'c':1,'C':1,'B':1,'s':2,'S':2,'q':8,'Q':8,'d':8,'v':0}.get(c,4);return size,min(size,4)
 if k=='array':sz,al=layout(n[2]);return sz*n[1],al
 if k=='struct':
  pos=0;maxal=1
  for _,t in structs[n[1]]:
   sz,al=layout(t);pos=(pos+al-1)//al*al;pos+=sz;maxal=max(maxal,al)
  return (pos+maxal-1)//maxal*maxal,maxal
lines=[header,'// Disk/address evidence only. HM32 pointer fields are uint32_t addresses, never native pointers.','#include <stdint.h>','#include <stdbool.h>','#include <stddef.h>','#pragma pack(push, 4)','']
for name in order:lines.append(f'typedef struct HM32{name} HM32{name};')
for name in order:
 lines+=['',f'struct HM32{name} {{']
 for i,(field,t) in enumerate(structs[name]):lines.append('    '+decl(t,field or f'field{i}',True)+';')
 lines+=['};']
lines+=['','#pragma pack(pop)','#if defined(__cplusplus)','#define HM32_ASSERT static_assert','#else','#define HM32_ASSERT _Static_assert','#endif']
for name in order:
 sz,al=layout(('struct',name));lines.append(f'HM32_ASSERT(sizeof(HM32{name}) == {sz}, "Original ARM32 {name} size");')
lines+=['#undef HM32_ASSERT']
(OUT/'Arm32DiskLayouts.h').write_text('\n'.join(lines)+'\n')
# Superclasses before subclasses; native ivars use normal current ABI alignment.
classes={c['name']:c for c in DATA['classes']};done=set();ordered=[]
while len(done)<len(classes):
 for name,c in classes.items():
  if name not in done and (c['superclass'] not in classes or c['superclass'] in done):ordered.append(c);done.add(name)
lines=[header,'#import "OriginalTypes.h"','']
mapping=[]
for c in ordered:
 assert c['superclass'],c['name']
 lines += [f'// Original class {c["address"]}; ARM32 instance size {c["instance_size"]}.',f'@interface {c["name"]} : {c["superclass"]} {{','@public']
 for v in c['ivars']:lines.append('    '+decl(v['node'],v['name'])+f'; // ARM32 offset {v["offset"]}, size {v["size"]}.')
 lines+=['}']
 for kind,methods in [('-',c['methods']),('+',c['class_methods'])]:
  for m in methods:
   nodes=m['nodes'];ret=name_type(nodes[0]);args=nodes[3:];sel=m['selector'];nargs=sel.count(':');assert nargs==len(args),(c['name'],sel,nodes)
   sig=f'{kind} ({ret})'
   if not args:sig+=sel
   else:sig+=' '.join(f'{s}:({name_type(t)})arg{i}' for i,(s,t) in enumerate(zip(sel.split(':')[:-1],args)))
   lines.append(f'// Original {m["address"]} ({m["implementation_raw"]} raw IMP); {m["type_encoding"]}')
   lines.append(sig+';')
   mapping.append({'class':c['name'],'kind':kind,'selector':sel,'address':m['address'],'implementation_raw':m['implementation_raw'],'type_encoding':m['type_encoding'],'native_declaration':sig+';','executable_implementation':False})
 lines+=['@end','']
(OUT/'OriginalClasses.h').write_text('\n'.join(lines)+'\n')
(OUT/'method-address-map.json').write_text(json.dumps(mapping,indent=2)+'\n')
print('classes',len(classes),'methods',len(mapping),'ivars',sum(len(c['ivars']) for c in ordered),'structs',order)
