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
texts={
'title':['REAL ESTATE','IMMOBILIER','NIERUCHOMOŚCI','НЕДВИЖИМОСТЬ'],
'for_sale':['FOR SALE','À VENDRE','NA SPRZEDAŻ','ПРОДАЁТСЯ'],
'for_rent':['FOR RENT','À LOUER','DO WYNAJĘCIA','СДАЁТСЯ'],
'rented':['RENTED','LOUÉ','WYNAJĘTE','АРЕНДОВАНО'],
'rent':['RENT','LOYER','CZYNSZ','АРЕНДНАЯ ПЛАТА'],
'weekly':['{amount} Cats / 7 days','{amount} Cats / 7 jours','{amount} Cats / 7 dni','{amount} катов / 7 дней'],
'due':['NEXT PAYMENT','PROCHAINE ÉCHÉANCE','NASTĘPNA PŁATNOŚĆ','СЛЕДУЮЩИЙ ПЛАТЁЖ'],
'hours':['{hours} h','{hours} h','{hours} godz.','{hours} ч'],
'buy':['BUY','ACHETER','KUP','КУПИТЬ'],
'terminate':['TERMINATE LEASE','RÉSILIER LE BAIL','ZAKOŃCZ NAJEM','РАСТОРГНУТЬ АРЕНДУ'],
'pay':['PAY ONE UNPAID RENT','PAYER UN LOYER IMPAYÉ','ZAPŁAĆ ZALEGŁY CZYNSZ','ПОГАСИТЬ ОДИН ПЛАТЁЖ'],
'debt':['REAL ESTATE DEBT','DETTE IMMOBILIÈRE','DŁUG ZA NAJEM','ДОЛГ ПО АРЕНДЕ'],
'unpaid':['UNPAID RENT','LOYER IMPAYÉ','ZALEGŁY CZYNSZ','ПРОСРОЧЕННАЯ АРЕНДА'],
'suspended':['IDENTITY UNRESOLVED — PAYMENTS SUSPENDED','IDENTITÉ NON RÉSOLUE — OPÉRATIONS SUSPENDUES','NIEUSTALONA TOŻSAMOŚĆ — OPERACJE WSTRZYMANE','ОБЪЕКТ НЕ ОПРЕДЕЛЁН — ОПЕРАЦИИ ПРИОСТАНОВЛЕНЫ'],
'closed':['LEASE ENDED','BAIL TERMINÉ','NAJEM ZAKOŃCZONY','АРЕНДА ЗАВЕРШЕНА'],
'owned':['OWNED','ACHETÉ','WŁASNOŚĆ','СОБСТВЕННОСТЬ'],
'insufficient':['Insufficient funds. No payment was taken.','Fonds insuffisants. Aucun paiement effectué.','Brak środków. Nic nie pobrano.','Недостаточно средств. Деньги не списаны.'],
'unavailable':['Real estate operations are unavailable. No action was completed.','Opérations immobilières indisponibles. Aucune action effectuée.','Operacje dotyczące nieruchomości są niedostępne. Nic nie wykonano.','Операции с недвижимостью недоступны. Действие не выполнено.'],
'choice':['Several rents are due and your funds cannot cover them all. Open Finances → Real Estate to choose which rent to pay. Your buildings remain available.','Vos fonds ne couvrent pas tous les loyers dus. Ouvrez Finances → Immobilier pour choisir les loyers à payer. Vos bâtiments restent disponibles.','Brakuje środków na należne czynsze. Otwórz Finanse → Nieruchomości i wybierz płatności. Nadal możesz korzystać z budynków.','Средств не хватает на все платежи. Откройте Финансы → Недвижимость и выберите платежи. Здания остаются доступны.'],
'purchase_blocked_debt.title':['PURCHASE BLOCKED — REAL ESTATE DEBT','ACHAT IMPOSSIBLE — DETTE IMMOBILIÈRE','ZAKUP NIEMOŻLIWY — DŁUG ZA NAJEM','ПОКУПКА НЕВОЗМОЖНА — ДОЛГ ПО АРЕНДЕ'],
'purchase_blocked_debt.body':['You must repay your real estate debt of {amount} Cats before acquiring another building.','Vous devez régler votre dette immobilière de {amount} Cats avant de pouvoir acquérir un nouveau bâtiment.','Przed zakupem budynku musisz spłacić dług za najem wynoszący {amount} Cats.','Перед покупкой здания необходимо погасить долг по аренде в размере {amount} катов.'],
'summary':['Rented: {count} | Weekly: {weekly} | Next 7 days: {forecast} | Debt: {debt} | Funds: {funds}', 'Locations : {count} | Par semaine : {weekly} | Prévision 7 jours : {forecast} | Dette : {debt} | Fonds : {funds}', 'Najmy: {count} | Tygodniowo: {weekly} | Następne 7 dni: {forecast} | Dług: {debt} | Środki: {funds}', 'Арендовано: {count} | В неделю: {weekly} | За 7 дней: {forecast} | Долг: {debt} | Средства: {funds}'],
'empty':['No leases or real estate debt.','Aucun bail ni dette immobilière.','Brak najmu i długu za najem.','Нет аренды и долгов.'],
'owner':['Owner','Propriétaire','Właściciel','Владелец'],
'rent_payment':['Rent payment','Paiement de loyer','Opłata czynszu','Арендный платёж'],
'purchase_payment':['Building purchase','Achat de bâtiment','Zakup budynku','Покупка здания'],
'signature':['Lease signed. The first full weekly rent was paid.','Bail signé. Le premier loyer hebdomadaire complet est payé.','Najem rozpoczęty. Pierwszy tygodniowy czynsz został opłacony.','Аренда оформлена. Первая неделя оплачена полностью.'],
'terminated':['Lease terminated. Existing debt remains due.','Bail résilié. La dette existante reste due.','Najem zakończony. Dotychczasowy dług pozostaje.','Аренда расторгнута. Имеющийся долг сохраняется.'],
'terminate_confirm':['End this lease immediately, without a refund? Existing debt remains due.','Résilier immédiatement ce bail sans remboursement ? La dette existante reste due.','Zakończyć najem natychmiast, bez zwrotu? Dotychczasowy dług pozostaje.','Расторгнуть аренду немедленно без возврата денег? Имеющийся долг сохраняется.'],
}
for index,lang in enumerate(['en','fr','pl','ru']):
    path=ROOT/'Localization'/f'{lang}.json';obj=json.loads(path.read_text(encoding='utf-8-sig'))
    for key,value in texts.items():obj['estate.'+key]=value[index]
    obj['finance.rents']=['Rent','Loyers','Czynsze','Аренда'][index]
    path.write_text(json.dumps(obj,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('Estate baseline and four language catalogues generated.')
