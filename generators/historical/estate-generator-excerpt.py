"""Generate the vanilla reference from the read-only FCS audit, and translations."""
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
data=json.loads((ROOT/'docs/real-estate-v9-fcs-audit.json').read_text(encoding='utf-8-sig'))
quote=lambda s:json.dumps(s,ensure_ascii=True)
rows=['#pragma once','// Generated from unmodified FCS base data; IDs are data keys, not a name blacklist.','namespace EstateBaseline {',
      'struct BuildingRow {const char* id;int materials;};','static const BuildingRow buildings[]={']
for x in data:
    if x['Type']=='BUILDING' and x['References'].get('interior'):
        mats=x['Fields'].get('build materials',0)
        if x['ReferenceValues'].get('construction'):mats=sum(v['v0'] for v in x['ReferenceValues']['construction'])
        if mats>0:rows.append('{%s,%d},'%(quote(x['Id']),mats))
rows+=['};','struct FactionRow {const char* id;float multiplier;int type;};','static const FactionRow factions[]={']
for x in data:
    if x['Type']=='FACTION':rows.append('{%s,%.6ff,%d},'%(quote(x['Id']),x['Fields'].get('building cost mult',0),x['Fields'].get('fundamental type',0)))
rows+=['};','struct TownRow {const char* id;int type;bool publicTown;};','static const TownRow towns[]={']
for x in data:
    if x['Type']=='TOWN':rows.append('{%s,%d,%s},'%(quote(x['Id']),x['Fields'].get('type',-1),str(bool(x['Fields'].get('is public',False))).lower()))
rows+=['};','static const char* sellHome[]={']
rows += [quote(x['Id'])+',' for x in data if x['Type']=='SQUAD_TEMPLATE' and x['Fields'].get('sell home')]
rows+=['};','}']
(ROOT/'RealEstateBaseline.generated.h').write_text('\n'.join(rows)+'\n',encoding='utf-8')
