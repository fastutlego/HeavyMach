#!/usr/bin/env python3
"""Render approved HeavyMach report manuscripts into the existing Sites checkout.
Requires Python packages Markdown and beautifulsoup4. Does not deploy.
"""
import argparse,json,re,shutil,hashlib
from pathlib import Path
from bs4 import BeautifulSoup
import markdown
ROOT=Path(__file__).resolve().parents[1]

def render(text):
    return BeautifulSoup(markdown.markdown(text,extensions=['tables','fenced_code','attr_list']), 'html.parser')

def tables(soup):
    for table in soup.select('table'):
        headers=[h.get_text(' ',strip=True) for h in table.select('th')]
        for tr in table.select('tr'):
            for i,td in enumerate(tr.select('td')):
                if i<len(headers): td['data-label']=headers[i]
        wrapper=soup.new_tag('div',attrs={'class':'tablewrap table-scroll'});table.wrap(wrapper)

def article(source,dest,download,template):
    raw=source.read_text(); content=render(raw); title=content.h1.get_text() if content.h1 else source.stem
    old=BeautifulSoup(template,'html.parser')
    if old.h1 and old.h1.get('id') and content.h1: content.h1['id']=old.h1['id']
    ids=[x.get('id') for x in old.select('h2')]
    nav=BeautifulSoup('<nav aria-label="본문 목차"><ol></ol></nav>','html.parser')
    for i,h in enumerate(content.select('h2')):
        h['id']=ids[i] if i<len(ids) and ids[i] else f'section-{i}'
        li=nav.new_tag('li');a=nav.new_tag('a',href='#'+h['id']);a.string=h.get_text();li.append(a);nav.ol.append(li)
    tables(content)
    page=BeautifulSoup(template,'html.parser');main=page.main;main.clear()
    links=BeautifulSoup('<div class="top"><a href="index.html">← 개발 보고</a> · <a href="'+download+'">Markdown 원고</a></div>','html.parser')
    main.append(links)
    for child in list(content.contents):
        if getattr(child,'name',None)=='h2' and nav.parent is None: main.append(nav)
        main.append(child)
    footer=page.new_tag('footer');footer.string='HeavyMach · 제품 코드 72e8c68e';main.append(footer)
    page.title.string=title+' | HeavyMach'
    # Keep long hashes, code, and tables within narrow mobile screens.
    style=page.new_tag('style');style.string='main{min-width:0}code,a{overflow-wrap:anywhere}pre{white-space:pre-wrap}nav{display:block}nav ol{padding-left:0;list-style:none;columns:2}nav li{break-inside:avoid}@media(max-width:700px){table thead{display:none}table tbody tr:first-child{display:block}nav ol{columns:1}}';page.head.append(style)
    dest.write_text(str(page));shutil.copyfile(source,dest.parent/download)

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--checkout',type=Path,required=True);args=ap.parse_args();dist=args.checkout/'dist'
    original=(dist/'index.html').read_text();old=BeautifulSoup(original,'html.parser');images={}
    for sid in ('menu-joins','menu-result'):
        gallery=BeautifulSoup('<div class="grid"></div>','html.parser')
        for img in old.select(f'#{sid} img'):
            fig=gallery.new_tag('figure');a=gallery.new_tag('a',href=img['src']);a.append(img.__copy__());fig.append(a)
            cap=gallery.new_tag('figcaption');cap.string=img.get('alt','');fig.append(cap);gallery.div.append(fig)
        images[sid]=str(gallery)
    raw=(ROOT/'docs/public/report-overview-20261025.md').read_text()
    for sid,gallery in images.items():raw=raw.replace('{{'+sid+'-gallery}}',gallery)
    content=render(raw);tables(content)
    # Wrap each h2 and its following content into stable existing anchor sections.
    main=old.main;main.clear();current=main
    for child in list(content.contents):
        if getattr(child,'name',None)=='h2':
            sid=child.get('id');child.attrs.pop('id',None);current=old.new_tag('section',id=sid);main.append(current)
        current.append(child)
    nav=old.select_one('aside nav');nav.clear()
    for section in main.select('section'):
        a=old.new_tag('a',href='#'+section['id']);a.string={'ios-audit':'iOS 코드·문서 검사', 'android-plan':'타이틀·Android', 'migration-audit':'전수검사·REA', 'repositories':'저장소·v0.9','port-and-native':'포팅·복원 과정','retrospective':'기술 회고','rea':'REA 활용','documentation':'문서 체계','menu-joins':'배경 연결 수정','menu-result':'메뉴 확장','phone':'아이폰 검사','ai':'적 이동','rotation':'가로 회전','screens':'남은 화면','evidence':'남은 작업'}[section['id']];nav.append(a)
    old.select_one('.sidefoot').string='2026.10.26 · 개발·검증 기록'
    old.title.string='HeavyMach 복원과 아이폰 검증'
    old.select_one('meta[name="description"]')['content']='HeavyMach 원본 분석, 사용자 지시별 수정, 아이폰 검증과 남은 작업'
    style=old.new_tag('style');style.string='figure{margin:0;min-width:0}figure img{width:100%;height:auto;display:block}figcaption{font-size:12px;color:var(--muted);margin-top:6px}aside{overflow:auto}nav{margin:24px 0}nav a{line-height:1.4}code,a{overflow-wrap:anywhere}@media(max-width:540px){table thead{display:none}table tr{display:block;border-top:1px solid var(--line);padding:10px 0}table td{display:block;border:0;padding:5px 0}table td:before{content:attr(data-label);display:block;font-size:11px;color:var(--muted)}}';old.head.append(style)
    (dist/'index.html').write_text(str(old))
    retro=(dist/'retrospective.html').read_text();doc=(dist/'documentation.html').read_text()
    article(ROOT/'docs/public/engineering-retrospective-20261025.md',dist/'retrospective.html','engineering-retrospective.md',retro)
    article(ROOT/'docs/documentation-audit-20261025.md',dist/'documentation.html','documentation-audit-20261025.md',doc)
    article(ROOT/'docs/public/rea-restoration-method-20261025.md',dist/'rea.html','rea-restoration-method-20261025.md',retro)
    article(ROOT/'docs/public/ipa-port-and-native-app-20261026.md',dist/'port-and-native.html','ipa-port-and-native-app-20261026.md',retro)
    article(ROOT/'docs/related-titles-20261026.md',dist/'related-titles.html','related-titles-20261026.md',retro)
    article(ROOT/'docs/android-port-plan-20261026.md',dist/'android-plan.html','android-port-plan-20261026.md',retro)
    article(ROOT/'docs/ios-completeness-audit-20261026.md',dist/'ios-audit.html','ios-completeness-audit-20261026.md',retro)
    shutil.copytree(ROOT/'docs/public/rea',dist/'rea',dirs_exist_ok=True)
    for name in ['game-design.md','document-governance.md','document-map.json']:
        shutil.copyfile(ROOT/'docs'/name,dist/name)
    report={'pages':['index.html','retrospective.html','documentation.html','rea.html','port-and-native.html','related-titles.html','android-plan.html','ios-audit.html'],'manuscripts':['report-overview-20261025.md','engineering-retrospective-20261025.md','rea-restoration-method-20261025.md','ipa-port-and-native-app-20261026.md','related-titles-20261026.md','android-port-plan-20261026.md','ios-completeness-audit-20261026.md'],'image_count_before':len(BeautifulSoup(original,'html.parser').select('img')),'image_count_after':len(old.select('img'))}
    assert report['image_count_before']==report['image_count_after']
    print(json.dumps(report,ensure_ascii=False))
if __name__=='__main__':main()
