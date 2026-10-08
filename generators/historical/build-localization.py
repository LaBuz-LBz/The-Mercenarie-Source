"""Generate the fallback and translator template from the authoritative JSON catalogues."""
import json,pathlib,re,sys
ROOT=pathlib.Path(__file__).parent;LANG=ROOT/'Localization'
def unique(pairs):
    d={}
    for k,v in pairs:
        if k in d:raise ValueError('Duplicate key: '+k)
        if not isinstance(v,str):raise ValueError('All entries must be strings: '+k)
        d[k]=v
    return d
def load(p):return json.loads(p.read_text(encoding='utf-8-sig'),object_pairs_hook=unique)
en=load(LANG/'en.json');fr=load(LANG/'fr.json');messages={k:v for k,v in en.items() if not k.startswith('_')}
catalogues={lang:load(LANG/(lang+'.json')) for lang in ('en','fr','pl','ru')}
for lang,cat in catalogues.items():
    if set(en)!=set(cat):raise ValueError(lang+' keys differ')
    if any(not v for v in cat.values()):raise ValueError(lang+' has empty values')
def quote(s):return '"'+''.join('\\x%02X'%b for b in s.encode('utf-8'))+'"'
aliases=load(ROOT/'localization-aliases.json')
for cat in catalogues.values():
    for key,value in cat.items():
        if not key.startswith('_') and value and '{' not in value and '%' not in value:
            aliases.setdefault(value,key)
parts=['#pragma once','// Generated from Localization/en.json and the legacy source index. Do not edit.','struct LocalizationDefault {const char* key;const char* value;};','static const LocalizationDefault kLocalizationEnglish[]={']
for k,v in sorted(messages.items()):parts.append('{'+json.dumps(k)+','+quote(v)+'},')
parts+=['};','static const LocalizationDefault kLocalizationAliases[]={']
for v,k in sorted(aliases.items()):
    if v and k in messages:parts.append('{'+json.dumps(k)+','+quote(v)+'},')
parts+=['};']
parts+=['static const LocalizationDefault kLocalizationTemplates[]={']
for cat in catalogues.values():
    for k,v in cat.items():
        if '{destination}' in v:
            parts.append('{'+json.dumps(k)+','+quote(v)+'},')
parts+=['};']
(ROOT/'LocalizationDefaults.generated.h').write_text('\n'.join(parts)+'\n',encoding='ascii')
template=dict(en,_language='xx',_name='Your language',_author='Your name')
(LANG/'Translation_Template.json').write_text(json.dumps(template,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
pack=ROOT/'Translation_Template/Localization/xx';pack.mkdir(parents=True,exist_ok=True)
(pack/'messages.json').write_text(json.dumps(messages,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print(len(messages),'English defaults and translator entries generated')
