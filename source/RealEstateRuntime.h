#pragma once
namespace {
typedef RealEstate::Money EstateMoney;
int (*estatePriceOriginal)(Building*)=0;
void (*estateBuyOriginal)(Building*,int)=0;
void (*estatePanelOriginal)(Building*,DatapanelGUI*,int)=0;
DWORD estateLastAction=0;std::string estateLastActionKey;
double estateResolveAfter=0;
bool estateOverrideNotified=false;
struct EstateBusyGuard {EstateBusyGuard(){estateBusy=true;}~EstateBusyGuard(){estateBusy=false;}};
bool estateReady(){return estateEnabled()&&estateHooksReady&&!estateBusy&&estateState.available&&financeReady();}
void estateNotice(const char* key){if(ou)ou->showPlayerAMessage(estateText(key),true);}
void estateChanged(){fnDirty=true;estateRefreshView();refreshFinanceView();}
bool estateOnce(const std::string& key){DWORD now=GetTickCount();if(key==estateLastActionKey&&now-estateLastAction<500)return false;estateLastActionKey=key;estateLastAction=now;return true;}
RealEstate::Identity estateIdentity(Building* b){
    RealEstate::Identity id;if(!b)return id;TownBase* town=b->getTown();GameData* data=b->getGameData();Faction* owner=b->getFaction();
    id.uid=b->instanceID.uid;id.layout=b->layoutInstanceID;id.building=data?data->stringID:"";id.town=town&&town->getGameData()?town->getGameData()->stringID:"";id.owner=owner&&owner->getData()?owner->getData()->stringID:"";
    id.name=b->getName();id.city=town?town->getName():"";id.ownerName=owner?owner->name:"";Ogre::Vector3 p=b->getPosition();id.x=p.x;id.y=p.y;id.z=p.z;return id;
}
bool estateRemapMatch(const RealEstate::Identity& a,const RealEstate::Identity& b){
    return a.town==b.town&&a.building==b.building&&std::fabs(a.x-b.x)<0.1&&std::fabs(a.y-b.y)<0.1&&std::fabs(a.z-b.z)<0.1&&
        ((!a.uid.empty()&&a.uid==b.uid&&a.layout==b.layout)||(!a.layout.empty()&&a.layout==b.layout));
}
Building* estateResolve(RealEstate::Lease& l){
    if(!estateEnabled()||!ou||!ou->zoneMgr)return 0;
    std::map<EstateMoney,hand>::iterator cached=estateHandles.find(l.id);
    if(!l.suspended&&cached!=estateHandles.end()){Building* b=cached->second.getBuilding();if(b&&l.identity.matches(estateIdentity(b)))return b;}
    TownBase* town=shou&&shou->townList?shou->townList->getTownBySID(l.identity.town):0;if(!town||!town->isActive())return 0;
    lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,town,0,false,0,0);Building* found=0;unsigned int matches=0;
    for(unsigned int i=0;i<buildings.size();++i){Building* b=buildings[i];if(b&&estateRemapMatch(l.identity,estateIdentity(b))){found=b;++matches;}}
    if(matches!=1||!found->isThePlayer()){if(!l.suspended){l.remaining=l.next-currentGameHours;l.suspended=true;++estateState.revision;}return 0;}
    l.identity.uid=found->instanceID.uid;estateHandles[l.id]=found->getHandle();if(l.suspended)estateState.resume(l.id,currentGameHours);return found;
}
EstateMoney estateLeaseId(Building* b){if(!b)return 0;RealEstate::Identity id=estateIdentity(b);for(std::map<EstateMoney,RealEstate::Lease>::iterator i=estateState.leases.begin();i!=estateState.leases.end();++i)if(i->second.status==RealEstate::Active&&i->second.identity.matches(id))return i->first;return 0;}
// Holy Nation Outlaws are the civilian sellers at the Hub, but FCS labels them
// OT_BANDIT. This explicit civilian exception uses the stable faction record ID;
// town/public/native-sale and hostility checks still apply without exception.
bool estateCivilianOutlaws(const std::string& id){return id=="42022-rebirth.mod";}
bool estateBanditFaction(Faction* f){return f&&f->getFundamentalNPCType()==OT_BANDIT&&(!f->getData()||!estateCivilianOutlaws(f->getData()->stringID));}
bool estateEligible(Building* b,EstateMoney& base){
    base=0;if(!estateEnabled()||!b||b->isThePlayer()||b->isFurnitureOrDoor()||b->isGate()||!b->hasInterior()||!b->isForSale())return false;
    TownBase* t=b->getTown();Faction* f=b->getFaction();Faction* player=ou&&ou->player?ou->player->participant:0;
    if(!t||!f||!player||!t->getFaction()||!t->isPublic()||(t->townType!=TOWN_TOWN&&t->townType!=TOWN_VILLAGE)||!f->relations||f->relations->isEnemy(player)||!t->getFaction()->relations||t->getFaction()->relations->isEnemy(player)||estateBanditFaction(f)||estateBanditFaction(t->getFaction()))return false;
    RealEstate::Identity id=estateIdentity(b);if(!id.valid())return false;
    // Eligibility follows the loaded game's native sale decision, including mods.
    // Vanilla references determine prices, not permission to sell a resident's home.
    int mats=0;for(size_t i=0;i<sizeof(EstateBaseline::buildings)/sizeof(EstateBaseline::buildings[0]);++i)if(id.building==EstateBaseline::buildings[i].id){mats=EstateBaseline::buildings[i].materials;break;}
    float mult=0;for(size_t i=0;i<sizeof(EstateBaseline::factions)/sizeof(EstateBaseline::factions[0]);++i)if(id.owner==EstateBaseline::factions[i].id){if(EstateBaseline::factions[i].type==OT_BANDIT&&!estateCivilianOutlaws(id.owner))return false;mult=EstateBaseline::factions[i].multiplier;break;}
    if(mats>0&&mult>0){float value=(b->isDestroyed()?0.375f:1.0f)*mats;value*=mult;value*=400.0f;if(!(value>0&&value<=214748364.0f))return false;base=(EstateMoney)value;}
    else {
        // Added mod content has no vanilla price record. Use the original native
        // price trampoline, never the V9 hook (which would multiply recursively).
        if(!estatePriceOriginal)return false;base=estatePriceOriginal(b);
    }
    EstateMoney rent=0,buy=0;return RealEstate::prices(base,rent,buy);
}
// The purchase callback is not reused: it clears/reassigns interior actors.
// Use the native ownership operations without deleting furniture or issuing jobs.
bool estateTransfer(Building* b,Faction* target){
    if(!b||!target||!target->factionOwnerships||!b->getFaction()||!b->getFaction()->factionOwnerships)return false;
    std::vector<Building*> affected;affected.push_back(b);lektor<Building*> candidates;
    ou->zoneMgr->findAllBuildings(candidates,b->getTown(),0,false,0,0);
    for(unsigned int i=0;i<candidates.size();++i){Building* part=candidates[i];if(part&&part!=b&&(part->furnitureParentBuilding()==b||part->doorParentBuilding()==b))affected.push_back(part);}
    for(unsigned int i=0;i<b->doors.size();++i)if(b->doors[i]&&std::find(affected.begin(),affected.end(),b->doors[i])==affected.end())affected.push_back(b->doors[i]);
    for(size_t i=0;i<affected.size();++i){Building* part=affected[i];Faction* old=part->getFaction();if(!old)continue;lektor<Platoon*> squads;old->getAllSquadsThatOwn(squads,part);for(unsigned int j=0;j<squads.size();++j)if(squads[j]&&squads[j]->getOwnerships())squads[j]->getOwnerships()->removeOwnedObject(part->getHandle());if(old->factionOwnerships)old->factionOwnerships->removeOwnedObject(part->getHandle());}
    b->setFaction(target,0);
    for(size_t i=0;i<affected.size();++i){Building* part=affected[i];if(part->getFaction()!=target)part->setFaction(target,0);target->factionOwnerships->addOwnedObject(part->getHandle());part->notifyChange();}
    return b->getFaction()==target&&target->factionOwnerships->isOwned(b->getHandle());
}
bool estateDebit(EstateMoney amount){int before=financeBalance();if(amount<0||amount>2147483647LL||before<amount)return false;if(!amount)return true;Ownerships* wallet=ou->player->participant->factionOwnerships;wallet->takeMoney((int)amount);int after=financeBalance();if(after==before-amount)return true;if(after>=0&&after<before)wallet->addMoney(before-after);return false;}
std::string estateEvent(const char* action,EstateMoney id,EstateMoney seq=0){std::ostringstream out;out<<"estate:"<<action<<":"<<id<<":"<<seq;return out.str();}
bool estateJournal(Finance::Journal& j,EstateMoney amount,int balance,int category,const char* key,const std::string& event){return j.record(-amount,balance,currentGameHours,category,key,event);}
void estateTick(bool resolve){
    if(!estateEnabled())return;
    if(!clientOptions.realEstateEnabled&&!estateOverrideNotified&&ou&&financeReady()){ou->showPlayerAMessage(Loc::text("options.real_estate.required_save"),true);estateOverrideNotified=true;optionsRebuildRequested=true;}
    if(!estateReady())return;EstateBusyGuard guard;
    try{
        if(resolve&&currentGameHours>=estateResolveAfter){estateResolveAfter=currentGameHours+0.025;for(std::map<EstateMoney,RealEstate::Lease>::iterator i=estateState.leases.begin();i!=estateState.leases.end();++i){RealEstate::Lease& l=i->second;if(l.status!=RealEstate::Active)continue;Building* b=estateResolve(l);if(b&&!l.suspended){if(!b->isDestroyed()&&l.initialRuin){l.initialRuin=false;++estateState.revision;}if(b->isDestroyed()&&!l.initialRuin){estateState.close(l.id,RealEstate::Destroyed);abandonGuildOffice(guildHouseKey(b),b);}}}}
        RealEstate::State next=estateState;int before=financeBalance();if(before<0)return;EstateMoney oldDebt=next.debt();unsigned long revision=next.revision;
        std::vector<RealEstate::Payment> payments=next.advance(currentGameHours,before);EstateMoney total=0;Finance::Journal journal=fiscalLedger.cash;journal.observe(before,currentGameHours);
        for(size_t i=0;i<payments.size();++i){total=RealEstate::add(total,payments[i].amount);if(!estateJournal(journal,payments[i].amount,before-(int)total,Finance::Rent,"estate.rent_payment",estateEvent("week",payments[i].lease,payments[i].sequence)))throw std::runtime_error("estate duplicate automatic payment");}
        if(!estateDebit(total)){next=estateState;next.advance(currentGameHours,0);journal=fiscalLedger.cash;}
        estateState.swap(next);fiscalLedger.cash.swap(journal);if(revision!=estateState.revision){saveFiscalLedger();estateChanged();if(estateState.debt()>oldDebt)estateNotice("choice");}
    }catch(const std::exception& e){estateState.available=false;estateRootError=e.what();ErrorLog(std::string("Estate tick: ")+e.what());estateNotice("unavailable");}
}
void estatePay(EstateMoney id,EstateMoney seq){
    if(!estateOnce(estateEvent("pay",id))||!estateReady())return;estateTick(false);if(!estateReady())return;EstateBusyGuard guard;
    try{RealEstate::State next=estateState;std::map<EstateMoney,RealEstate::Lease>::iterator it=next.leases.find(id);if(it==next.leases.end())return;EstateMoney amount=it->second.rent;int before=financeBalance();if(!next.pay(id,seq,before)){estateNotice(it->second.suspended?"suspended":"insufficient");return;}
        Finance::Journal journal=fiscalLedger.cash;journal.observe(before,currentGameHours);if(!estateJournal(journal,amount,before-(int)amount,Finance::Rent,"estate.rent_payment",estateEvent("debt",id,seq)))return;if(!estateDebit(amount)){estateNotice("insufficient");return;}estateState.swap(next);fiscalLedger.cash.swap(journal);saveFiscalLedger();estateChanged();
    }catch(const std::exception& e){estateState.available=false;estateRootError=e.what();ErrorLog(std::string("Estate payment: ")+e.what());estateNotice("unavailable");}
}
bool estateDebtGate(){if(!estateEnabled())return true;if(!estateState.available){estateNotice("unavailable");return false;}EstateMoney debt=estateState.debt();if(!debt)return true;Loc::Catalogue args;args["amount"]=estateNumber(debt);ou->showPlayerAMessage(estateText("purchase_blocked_debt.title")+"\n"+Loc::format("estate.purchase_blocked_debt.body",args),true);return false;}
void estateRent(Building* b){
    if(!b||!estateOnce("rent:"+b->instanceID.uid)||!estateReady())return;estateTick(false);if(!estateReady())return;EstateBusyGuard guard;
    try{EstateMoney base=0;if(!estateEligible(b,base)){estateNotice("unavailable");return;}RealEstate::State next=estateState;int before=financeBalance();EstateMoney id=next.sign(estateIdentity(b),base,currentGameHours,before,b->isDestroyed());if(!id){estateNotice("insufficient");return;}EstateMoney cost=next.leases[id].rent;
        Finance::Journal journal=fiscalLedger.cash;journal.observe(before,currentGameHours);if(!estateJournal(journal,cost,before-(int)cost,Finance::Rent,"estate.rent_payment",estateEvent("sign",id)))return;
        Faction* owner=b->getFaction();if(!estateDebit(cost)){estateNotice("insufficient");return;}if(!estateTransfer(b,ou->player->participant)){ou->player->participant->factionOwnerships->addMoney((int)cost);estateTransfer(b,owner);estateNotice("unavailable");return;}
        estateState.swap(next);estateHandles[id]=b->getHandle();fiscalLedger.cash.swap(journal);saveFiscalLedger();estateChanged();estateNotice("signature");
    }catch(const std::exception& e){estateState.available=false;estateRootError=e.what();ErrorLog(std::string("Estate signature: ")+e.what());estateNotice("unavailable");}
}
void estateBuy(Building* b){
    if(!b||!estateOnce("buy:"+b->instanceID.uid)||!estateReady())return;estateTick(false);if(!estateReady()||!estateDebtGate())return;EstateBusyGuard guard;
    try{RealEstate::State next=estateState;EstateMoney id=estateLeaseId(b),cost=0,base=0;bool convert=id!=0;
        if(convert){RealEstate::Lease& l=next.leases[id];if(l.suspended||!b->isThePlayer()||!next.close(id,RealEstate::Purchased)){estateNotice("unavailable");return;}cost=l.buy;}
        else{if(!estateEligible(b,base)){estateNotice("unavailable");return;}EstateMoney rent=0;RealEstate::prices(base,rent,cost);}
        int before=financeBalance();if(before<cost){estateNotice("insufficient");return;}Finance::Journal journal=fiscalLedger.cash;journal.observe(before,currentGameHours);
        std::string event=convert?estateEvent("buy",id):("estate:buy:"+b->instanceID.uid+":"+b->layoutInstanceID);
        if(!estateJournal(journal,cost,before-(int)cost,Finance::Purchase,"estate.purchase_payment",event))return;Faction* owner=b->getFaction();if(!estateDebit(cost)){estateNotice("insufficient");return;}
        if(!convert&&!estateTransfer(b,ou->player->participant)){ou->player->participant->factionOwnerships->addMoney((int)cost);estateTransfer(b,owner);estateNotice("unavailable");return;}
        estateState.swap(next);fiscalLedger.cash.swap(journal);saveFiscalLedger();estateChanged();
    }catch(const std::exception& e){estateState.available=false;estateRootError=e.what();ErrorLog(std::string("Estate purchase: ")+e.what());estateNotice("unavailable");}
}
void estateTerminate(EstateMoney id){
    if(!estateOnce(estateEvent("end",id))||!estateReady())return;estateTick(false);if(!estateReady())return;EstateBusyGuard guard;
    try{std::map<EstateMoney,RealEstate::Lease>::iterator it=estateState.leases.find(id);if(it==estateState.leases.end()||it->second.suspended||it->second.status!=RealEstate::Active)return;Building* b=estateResolve(it->second);Faction* owner=ou->factionMgr->getFactionByStringID(it->second.identity.owner);if(!b||!b->isThePlayer()||!owner){estateNotice("unavailable");return;}
        RealEstate::State next=estateState;if(!next.close(id,RealEstate::Terminated))return;if(!estateTransfer(b,owner)){estateNotice("unavailable");return;}estateState.swap(next);abandonGuildOffice(guildHouseKey(b),b);estateHandles.erase(id);estateChanged();estateNotice("terminated");
    }catch(const std::exception& e){estateState.available=false;estateRootError=e.what();ErrorLog(std::string("Estate termination: ")+e.what());estateNotice("unavailable");}
}
struct EstateLineBinding {EstateMoney id,sequence;hand building;EstateLineBinding():id(0),sequence(0){} };
std::map<DataPanelLine*,EstateLineBinding> estateLines;
void estateRentLine(DataPanelLine* l){estateRent(l?l->getUserData().getBuilding():0);}
void estateBuyLine(DataPanelLine* l){estateBuy(l?l->getUserData().getBuilding():0);}
void estatePayLine(DataPanelLine* l){std::map<DataPanelLine*,EstateLineBinding>::iterator i=estateLines.find(l);if(i==estateLines.end())return;EstateLineBinding b=i->second;estatePay(b.id,b.sequence);}
void estateEndLine(DataPanelLine* l){std::map<DataPanelLine*,EstateLineBinding>::iterator i=estateLines.find(l);if(i!=estateLines.end())estateTerminate(i->second.id);}
void estateCopyTextStyle(MyGUI::TextBox* target,MyGUI::TextBox* source){
    if(!target||!source)return;
    target->setTextColour(source->getTextColour());
    std::string caption=source->getCaption(),tag;
    if(caption.size()>=7&&caption[0]=='#'){
        bool colour=true;for(size_t i=1;i<7;++i)if(!std::isxdigit((unsigned char)caption[i]))colour=false;
        if(colour)tag=caption.substr(0,7);
    }
    if(!tag.empty()){std::string current=target->getCaption();if(current.compare(0,tag.size(),tag)!=0)target->setCaption(tag+current);}
}
void estateLine(DatapanelGUI* p,Building* b,const std::string& label,const std::string& value,void(*cb)(DataPanelLine*),EstateMoney id=0){
    DataPanelLine_Button* nativeStyle=0;
    if(cb==estateRentLine){for(int i=0;i<p->getNumLines(6);++i){DataPanelLine* candidate=p->getLineByNum(6,i);if(candidate&&candidate->classType==DataPanelLine::DPL_BUTTON&&candidate->s1!=label&&!estateLines.count(candidate)){nativeStyle=static_cast<DataPanelLine_Button*>(candidate);break;}}}
    DataPanelLine_Button* l=p->setLineTextButton(label,value,6,0.58f,"Kenshi_Button2");if(!l)return;l->userData=b->getHandle();l->callback=MyGUI::newDelegate(cb);EstateLineBinding bind;bind.building=b->getHandle();bind.id=id;if(id)bind.sequence=estateState.leases[id].oldest();estateLines[l]=bind;
    if(nativeStyle&&nativeStyle!=l){estateCopyTextStyle(l->w1,nativeStyle->button);estateCopyTextStyle(l->button,nativeStyle->button);}
}
void estatePanelHook(Building* b,DatapanelGUI* panel,int category){
    estatePanelOriginal(b,panel,category);
    // Native getGUIData is also called for header/status panels. Only category 6
    // owns the scrolling action panel used by DataPanelLine_Button::createMe.
    // That native method dereferences scrollView and scrollWin without checks.
    if(!estateEnabled()||!estateHooksReady||!panel||!b||category!=6||!panel->scrollView||!panel->scrollWin||!financeReady())return;
    try{estateLines.clear();EstateMoney id=estateLeaseId(b);if(id){const RealEstate::Lease& l=estateState.leases[id];panel->setLine(estateText("rented"),l.suspended?estateText("suspended"):estateWeekly(l.rent),6,false,true);panel->setLine(estateText("due"),estateDue(l),6,false,true);
        if(l.debt())estateLine(panel,b,estateText("unpaid"),estateNumber(l.debt())+Loc::text("ui.cats"),estatePayLine,id);
        estateLine(panel,b,estateText("buy"),estateNumber(l.buy)+Loc::text("ui.cats"),estateBuyLine,id);estateLine(panel,b,estateText("terminate"),estateText("terminate"),estateEndLine,id);
    }else{EstateMoney base=0;if(estateEligible(b,base)){EstateMoney rent=0,buy=0;RealEstate::prices(base,rent,buy);estateLine(panel,b,estateText("for_rent"),estateWeekly(rent),estateRentLine);}}
    }catch(const std::exception& e){ErrorLog(std::string("Estate panel: ")+e.what());}
}
int estatePriceHook(Building* b){if(!estateEnabled()||!estateHooksReady)return estatePriceOriginal(b);EstateMoney base=0;return estateEligible(b,base)?(int)(base*10):estatePriceOriginal(b);}
void estateBuyHook(Building* b,int result){
    if(!estateEnabled()||result!=2){estateBuyOriginal(b,result);return;}
    if(!estateReady()){estateNotice("unavailable");return;}estateTick(false);if(!estateReady()||!estateDebtGate())return;
    EstateMoney base=0;if(estateLeaseId(b)||estateEligible(b,base))estateBuy(b);else estateBuyOriginal(b,result);
}
void estateOptionChanged(){
    estateHandles.clear();estateLines.clear();estateResolveAfter=0;estateLastAction=0;estateLastActionKey.clear();estateOverrideNotified=false;
    estateRefreshView();refreshRegisterNavigation();fnDirty=true;refreshFinanceView();
}
void estateResetWorld(){estateOverrideNotified=false;estateRootError.clear();estateState=RealEstate::State();estateHandles.clear();estateLines.clear();estateResolveAfter=0;estateLastAction=0;estateLastActionKey.clear();estateBusy=false;}
void estateInstallHooks(){
    // Read the global preference before any building callback can run.
    loadClientOptions();
    bool ok=true;ok=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Building::calculateSaleValue),&estatePriceHook,&estatePriceOriginal)==KenshiLib::SUCCESS&&ok;
    ok=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Building::_NV_buyMeCallback),&estateBuyHook,&estateBuyOriginal)==KenshiLib::SUCCESS&&ok;
    ok=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Building::_NV_getGUIData),&estatePanelHook,&estatePanelOriginal)==KenshiLib::SUCCESS&&ok;
    estateHooksReady=ok;if(!ok)ErrorLog("Estate: native hooks incomplete; property actions blocked");else DebugLog("Estate V9: native panel, price and global debt gate installed; no eviction");
}
}
