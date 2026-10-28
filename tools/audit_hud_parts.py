#!/usr/bin/env python3
"""Inventory actual HMGameHUD draw submissions and display source regions in SVG.
Input: xcresulttool-exported attachments from testHUDAssetOwnershipAudit.
PNG attachments are copied byte-for-byte. Pixel reads check coverage and bounds.
"""
import argparse,base64,hashlib,html,json,math,shutil
from pathlib import Path
import numpy as np
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
COLORS=['#e4bb56','#53c4c7','#f4896f','#a898df','#7aca81','#e191c7']
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--attachments',type=Path,required=True);args=ap.parse_args()
 out=ROOT/'docs/public/media/hud-audit';out.mkdir(parents=True,exist_ok=True)
 manifest=json.loads((args.attachments/'manifest.json').read_text());files={}
 for test in manifest:
  for a in test['attachments']:
   name=a['suggestedHumanReadableName'].split('_0_')[0]
   if not name.startswith('HUD_'):continue
   src=args.attachments/a['exportedFileName'];dest=out/(name+src.suffix);shutil.copyfile(src,dest);files[name]=dest
 inventory=json.loads(files['HUD_draw_inventory'].read_text())
 assets={a['asset']:a for a in inventory['assets']};urls={};images={};evidence={}
 for name,a in assets.items():
  src=ROOT/('Art/'+name if name.startswith('Extensions/') else 'Resources/OriginalImages/'+name)
  pic=files['HUD_asset_'+Path(name).stem];im=Image.open(pic).convert('RGBA');images[name]=np.array(im)
  assert im.size==(a['width'],a['height'])
  urls[name]='data:image/png;base64,'+base64.b64encode(pic.read_bytes()).decode()
  evidence[name]={'runtime_source':src.relative_to(ROOT).as_posix(),'source_sha256':sha(src),'decoded_png':pic.relative_to(ROOT).as_posix(),'decoded_sha256':sha(pic),'runtime_size':list(im.size)}
 def text(x,y,s,size=16,color='#e8ecdf'):return f'<text x="{x}" y="{y}" font-size="{size}" fill="{color}" font-family="Arial,sans-serif">{html.escape(s)}</text>'
 def image(name,x,y,w=None,h=None):
  a=assets[name];return f'<image href="{urls[name]}" x="{x}" y="{y}" width="{w or a["width"]}" height="{h or a["height"]}"/>'
 def svg(body,w,h):return f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}"><rect width="100%" height="100%" fill="#293333"/>{body}</svg>'
 def region(name,rect,x,y,scale=1):
  xx,yy,w,h=rect
  return f'<svg x="{x}" y="{y}" width="{w*scale}" height="{h*scale}" viewBox="{xx} {yy} {w} {h}" overflow="hidden">{image(name,0,0)}</svg>'
 # Atlas frames retain the original GPU sheet order, not the HUD button order.
 sheets=[('GameMenu.png',1,2,'Menu: normal / pressed'),('Btn_Booster.png',2,1,'Jump: normal / pressed'),('Btn_Weapon.png',2,1,'Weapon: normal / pressed'),('Extensions/AutoAimControls.png',3,2,'Sheet columns: A / FIRE / NEXT; rows: idle / active'),('GameUI_Gauge.png',1,1,'Gauge needle (used twice)'),('GameUI_BlueLight.png',1,1,'Reload light'),('GameUI_RedLight.png',1,1,'Warning light'),('GameUI_Gear.png',1,1,'Rotating gear'),('GameUI_Exp.png',1,1,'EXP bar'),('NumSet01.png',16,1,'Level glyphs: 16 x 16'),('NumSet02b.png',16,1,'Ammo / passive glyphs: 8 x 8'),('WeaponTile.png',7,4,'Weapon icons: 35 x 26'),('ItemTile.png',7,4,'Passive icons: 35 x 26, drawn at 0.5'),('SelWeapon.png',1,1,'Selected weapon outline: 36 x 26')]
 body='';y=38;frames={}
 for name,cols,rows,label in sheets:
  a=assets[name];w=a['width']/cols;h=a['height']/rows;scale=min(3,880/a['width']);body+=text(22,y,label+' — '+name,16);y+=16
  body+=image(name,24,y,a['width']*scale,a['height']*scale)
  frames[name]=[]
  for tile in range(cols*rows):
   tx=(tile%cols)*w;ty=(tile//cols)*h;body+=f'<rect x="{24+tx*scale}" y="{y+ty*scale}" width="{w*scale}" height="{h*scale}" fill="none" stroke="#e4bb56" stroke-width="0.6"/>'
   alpha=images[name][int(ty):int(ty+h),int(tx):int(tx+w),3];ys,xs=np.where(alpha>0)
   frames[name].append({'tile':tile,'source_rect':[tx,ty,w,h],'alpha_bbox':None if not len(xs) else [int(xs.min()),int(ys.min()),int(xs.max()+1),int(ys.max()+1)]})
  y+=a['height']*scale+40
 (out/'atlas.svg').write_text(svg(body,960,y))
 # Divisions shown over the unchanged backgrounds; right rail stays complete.
 body=text(24,32,'Background ownership / cut review',24);y=60;seams=[]
 for name,edges,title in [('GameUI_Top.png',[0,76,102,480],'TOP: menu and LV/EXP overlap in x; split by y too'),('GameUI_Btm.png',[0,72,160,334,480],'BOTTOM: jump / weapon / gauges / shared aim rail')]:
  body+=text(24,y,title);y+=20;body+=image(name,24,y,960,128)
  for i,x in enumerate(edges):
   start,extent=(27,37) if name=='GameUI_Top.png' and x==76 else (0,27) if name=='GameUI_Top.png' and x==102 else (0,64)
   body+=f'<path d="M {24+x*2} {y+start*2} v {extent*2}" stroke="{COLORS[i%6]}" stroke-width="2"/>'+text(24+x*2,y+151,str(x),14,COLORS[i%6])
   if 0<x<480:
    a=images[name][start:start+extent,:,3];n=int(np.count_nonzero((a[:,x-1]>16)&(a[:,x]>16)));seams.append({'asset':name,'x':x,'connected_alpha_rows':n})
  if name=='GameUI_Top.png':body+=f'<path d="M 24 {y+54} h 204" stroke="#f4896f" stroke-width="2"/>'+text(230,y+50,'y = 27',13)
  else:
   for x in [200,382,430]:body+=f'<path d="M {24+x*2} {y} v 128" stroke="#ff6b6b" stroke-width="2" stroke-dasharray="5 3"/>'+text(24+x*2,y+174,'old '+str(x),12,'#ff9999')
  y+=208
 body+=text(24,y,'Right rail: one 146 x 64 background; three independent actions.',18);y+=24
 body+=region('GameUI_Btm.png',[334,0,146,64],24,y,4);y+=280
 (out/'cuts.svg').write_text(svg(body,1020,y))
 def group(c):
  n=c['asset']
  if n in ['GameUI_Top.png','GameUI_Btm.png']:return 'background'
  if n=='GameMenu.png':return 'menu'
  if n in ['GameUI_Exp.png','NumSet01.png']:return 'progress'
  if n=='Btn_Booster.png':return 'jump'
  if n=='Btn_Weapon.png':return 'weapon'
  if n=='WeaponTile.png':return 'weapon' if c['y']==35 else 'preview'
  if n=='NumSet02b.png':return 'weapon' if c['y']==26 else 'passives'
  if n in ['GameUI_Gauge.png','GameUI_BlueLight.png','GameUI_RedLight.png','GameUI_Gear.png']:return 'gauges'
  if n=='Extensions/AutoAimControls.png':return 'aim'
  if n=='ItemTile.png':return 'passives'
  if n=='SelWeapon.png':return 'preview'
  raise ValueError('Unassigned HUD draw '+n)
 group_defs=[('menu','MENU', [0,256,76,39],('GameUI_Top.png',[0,27,76,37]),[6,263,64,32]),('progress','LEVEL / EXP',[0,293,102,27],('GameUI_Top.png',[0,0,102,27]),None),('jump','JUMP',[0,0,72,68],('GameUI_Btm.png',[0,0,72,64]),[0,0,68,68]),('weapon','WEAPON / AMMO',[67,0,93,64],('GameUI_Btm.png',[72,0,88,64]),[67,0,72,62]),('gauges','HP / RELOAD / LIGHTS / GEAR',[160,0,174,64],('GameUI_Btm.png',[160,0,174,64]),None),('aim','A / NEXT / FIRE + RAIL',[334,0,146,64],('GameUI_Btm.png',[334,0,146,64]),None),('passives','PASSIVE ITEMS',[0,190,50,68],None,None),('preview','WEAPON PREVIEW',[170,270,224,40],None,None)]
 groups={};body='';y=38
 calls=inventory['states']['extras']
 for gid,title,bounds,bg,hit in group_defs:
  selected=[c for c in calls if group(c)==gid];groups[gid]={'title':title,'candidate_bounds':bounds,'draws':selected,'hit_rect':hit,'background':bg}
  body+=text(24,y,title,21);y+=16;xx,yy,w,h=bounds;s=min(3,800/w)
  body+=f'<svg x="24" y="{y}" width="{w*s}" height="{h*s}" viewBox="{xx} {320-yy-h} {w} {h}" overflow="hidden">'
  native=files['HUD_group_'+gid];native_url='data:image/png;base64,'+base64.b64encode(native.read_bytes()).decode()
  body+=f'<image href="{native_url}" x="0" y="0" width="480" height="320"/>'
  if hit:
   hx,hy,hw,hh=hit;body+=f'<rect x="{hx}" y="{320-hy-hh}" width="{hw}" height="{hh}" fill="none" stroke="#56e7ee" stroke-width="0.5"/>'
  if gid=='aim':
   for ax in [334,382,430]:body+=f'<rect x="{ax}" y="264" width="46" height="46" fill="none" stroke="#56e7ee" stroke-width="0.5"/>'
  body+='</svg>';y+=h*s+36
 (out/'groups.svg').write_text(svg(body,960,y))
 # Every submitted sprite is assigned; shared assets may serve different groups.
 assigned={state:[group(c) for c in arr] for state,arr in inventory['states'].items()}
 a=np.array(Image.open(files['HUD_normal']).convert('RGBA'));b=np.array(Image.open(files['HUD_partitioned']).convert('RGBA'))
 difference=int(np.count_nonzero(np.any(a!=b,axis=2)));assert difference==0
 audit={'runtime_assets':evidence,'atlas_frames':frames,'groups':groups,'state_draw_counts':{k:len(v) for k,v in assigned.items()},'unassigned_draws':0,'reconstruction_different_pixels':difference,'seams':seams,'note':'Source regions are candidates. Connected rail cut ends still need finishing before independent relocation.'}
 (out/'audit.json').write_text(json.dumps(audit,ensure_ascii=False,indent=2)+'\n')
 print(json.dumps({'assets':len(assets),'states':len(assigned),'draws':sum(map(len,assigned.values())),'unassigned':0,'reconstruction_diff_pixels':difference,'output':str(out)},ensure_ascii=False))
if __name__=='__main__':main()
