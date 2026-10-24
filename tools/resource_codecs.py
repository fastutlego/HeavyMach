"""Reused from the authenticated 2026-10-24 package comparison; no source writes."""
import hashlib,struct,zlib
from pathlib import Path
sha=lambda b:hashlib.sha256(b).hexdigest()

def category(n):
    if n=='HeavyMach': return 'executable'
    if n=='CodeResources' or n.startswith('_CodeSignature/'):return 'signature'
    s=Path(n).suffix.lower()
    if s in ('.png','.jpg','.jpeg') or n.endswith('.png.orig'):return 'image'
    if s=='.wav':return 'audio'
    if s in ('.map','.mch'):return 'game_data'
    return 'package_metadata_or_shader'

def pngdecode(b):
    assert b[:8]==b'\x89PNG\r\n\x1a\n'
    off=8; chunks=[]
    while off<len(b):
        size=struct.unpack_from('>I',b,off)[0];t=b[off+4:off+8];payload=b[off+8:off+8+size]
        assert zlib.crc32(t+payload)&0xffffffff==struct.unpack_from('>I',b,off+8+size)[0]
        chunks.append((t,payload));off+=size+12
    hd=next(d for t,d in chunks if t==b'IHDR');w,h,depth,color,co,fi,inter=struct.unpack('>IIBBBBB',hd)
    assert depth==8 and color in (2,6) and inter==0,(depth,color,inter)
    cg=any(t==b'CgBI' for t,d in chunks);channels=3 if color==2 else 4;stride=w*channels
    raw=zlib.decompress(b''.join(d for t,d in chunks if t==b'IDAT'),-15 if cg else 15)
    assert len(raw)==h*(stride+1)
    prev=bytearray(stride);pix=bytearray();cursor=0
    for row in range(h):
        filt=raw[cursor];cur=bytearray(raw[cursor+1:cursor+1+stride]);cursor+=1+stride
        for i in range(stride):
            left=cur[i-channels] if i>=channels else 0;up=prev[i];ul=prev[i-channels] if i>=channels else 0
            if filt==1:cur[i]=(cur[i]+left)&255
            elif filt==2:cur[i]=(cur[i]+up)&255
            elif filt==3:cur[i]=(cur[i]+((left+up)//2))&255
            elif filt==4:
                pr=left+up-ul;pa=abs(pr-left);pb=abs(pr-up);pc=abs(pr-ul)
                cur[i]=(cur[i]+(left if pa<=pb and pa<=pc else up if pb<=pc else ul))&255
            else:assert filt==0
        if cg:
            for i in range(0,stride,channels):cur[i],cur[i+2]=cur[i+2],cur[i]
        if channels==3:
            for i in range(0,stride,3):pix.extend(cur[i:i+3]);pix.append(255)
        else:pix.extend(cur)
        # PNG prediction uses stored BGRA values, before our channel swap.
        if cg:
            for i in range(0,stride,channels):cur[i],cur[i+2]=cur[i+2],cur[i]
        prev=cur
    return {'width':w,'height':h,'bit_depth':depth,'color_type':color,'cgbi':cg,'pixel_convention':'RGBA; CgBI RGB remains premultiplied','pixel_sha256':sha(pix)},bytes(pix)

def parse_node(b,off=0,depth=0):
    assert depth<20 and off+16<=len(b)
    size,typ,ds=struct.unpack_from('<3I',b,off);end=off+12+ds;assert end+4<=len(b)
    count=struct.unpack_from('<I',b,end)[0];assert count<20000
    data=b[off+12:end];node={'offset':off,'declared_size':size,'type':typ,'data_size':ds,'data_hex':data.hex(),'data_sha256':sha(data),'children':[]}
    if data:
        prefix=data.split(b'\0',1)[0]
        if prefix and all(32<=c<127 for c in prefix):node['name_prefix']=prefix.decode()
    end+=4
    for i in range(count):
        child,end=parse_node(b,end,depth+1);node['children'].append(child)
    node['actual_size']=end-off;return node,end

def walk(n):
    yield n
    for c in n['children']:yield from walk(c)
