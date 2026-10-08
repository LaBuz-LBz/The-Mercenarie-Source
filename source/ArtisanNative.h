#include "ArtisanLayout.h"
#include "ArtisanClock.h"
#pragma comment(lib,"user32.lib")
// Included in the existing game-thread integration after UI and finance helpers.
namespace {
extern bool missionWorldChanging,missionRestorePending;
struct ArtisanGrade {std::string id,name;GameData* maker;GameData* model;int level;ArtisanGrade():maker(0),model(0),level(0){}};
ArtisanOrders::Ledger artisanLedger;

std::vector<ArtisanGrade> artisanArmourGrades,artisanWeaponGrades;
MyGUI::Widget* artisanWindow=0;
MyGUI::ScrollView* artisanScroll=0;
int artisanDrawnPage=-1;
int artisanOrderFilter=0;
std::map<unsigned long,MyGUI::TextBox*> artisanPercentWidgets;
std::map<size_t,MyGUI::TextBox*> artisanDetailLabels;
MyGUI::Widget* artisanBody=0; MyGUI::Button* artisanBasketButton=0;
std::map<std::string,int> artisanRowGrades; unsigned long artisanDetailOrder=0;
bool artisanResetScroll=false; bool artisanAuditOpening=false; std::string artisanMakeError;
std::map<unsigned long,MyGUI::Widget*> artisanProgressWidgets;
std::map<unsigned long,MyGUI::TextBox*> artisanTimeWidgets;
hand artisanSpeaker,artisanReceiver;
std::string artisanKey,artisanName,artisanSearch,artisanNotice,artisanRecoveryChoice;
std::vector<ArtisanOrders::Line> artisanShown;
std::vector<Item*> artisanPreviews;
int artisanPage=0,artisanKind=0,artisanOffset=0,artisanGradeIndex=0,artisanCategory=0;
std::map<std::string,int> artisanQuantities;
bool artisanRefresh=false,artisanBusy=false;
MyGUI::IntSize artisanViewSize;
std::string artisanLanguage;
double artisanNow(){return ou?WorldServiceClock::now(ou->getTimeStamp_inGameHours().getTotalHours(),developerTimeOffsetHours):0;}
std::string artisanNumber(long long n){std::ostringstream s;s<<n;return s.str();}
std::string artisanTime(double h){long minutes=(long)std::ceil(std::max(0.0,h)*60.0);std::ostringstream s;if(minutes>=1440)s<<minutes/1440<<" "<<Loc::text("artisan.days")<<" ";s<<(minutes/60)%24<<" h";if(minutes%60)s<<" "<<minutes%60<<" min";return s.str();}
std::string artisanIdentity(Character* c){return c&&c->getGameData()?c->getHandle().toString()+"|"+c->getGameData()->stringID:"";}
Character* artisanResolve(const std::string& key){size_t p=key.find('|');if(p==std::string::npos)return 0;hand h;h.fromString(key.substr(0,p));Character* c=h.getCharacter();return c&&artisanIdentity(c)==key?c:0;}
bool artisanContext(){Character* s=artisanSpeaker.getCharacter();Character* r=artisanReceiver.getCharacter();return guildLevel()>=1&&!missionWorldChanging&&!missionRestorePending&&!progressWriteBlocked&&s&&!s->isDead()&&r&&r->isPlayerCharacter()&&!r->isDead()&&s->getPosition().squaredDistance(r->getPosition())<=2500.0f&&artisanIdentity(s)==artisanKey;}
bool artisanGradeLess(const ArtisanGrade& a,const ArtisanGrade& b){return a.level==b.level?a.id<b.id:a.level<b.level;}
void artisanGrades(){
 artisanArmourGrades.clear();artisanWeaponGrades.clear();
 const int levels[]={5,20,40,60,80,100};
 for(int i=0;i<6;++i){ArtisanGrade g;g.level=levels[i];g.id="armour:"+artisanNumber(g.level);g.name=Loc::text(("artisan.grade."+artisanNumber(i)).c_str());artisanArmourGrades.push_back(g);}
 if(!ou)return;
 // The native player manufacturer defines the ordered craft models. This is a
 // system record, not an equipment whitelist. Modded entries are discovered.
 GameData* maker=ou->gamedata.getData("PLAYER_WEAPONS",WEAPON_MANUFACTURER);if(!maker)return;
 const Ogre::vector<GameDataReference>::type* models=maker->getReferenceListIfExists("weapon models");if(!models)return;
 for(size_t i=0;i<models->size();++i){const GameDataReference& ref=(*models)[i];if(!ref.ptr||ref.ptr->type!=MATERIAL_SPECS_WEAPON||ref.values.value[0]<1||ref.values.value[0]>100)continue;ArtisanGrade g;g.level=ref.values.value[0];g.maker=maker;g.model=ref.ptr;g.name=ref.ptr->name;g.id=maker->stringID+":"+g.model->stringID+":"+artisanNumber(g.level);artisanWeaponGrades.push_back(g);}
 int craftMax=0;std::set<std::string> modelsSeen;for(size_t i=0;i<artisanWeaponGrades.size();++i){craftMax=std::max(craftMax,artisanWeaponGrades[i].level);modelsSeen.insert(artisanWeaponGrades[i].model->stringID);}
 lektor<GameData*> makers;ou->gamedata.getDataOfType(makers,WEAPON_MANUFACTURER);
 std::map<std::string,GameData*> orderedMakers;for(size_t m=0;m<makers.size();++m)if(makers[m])orderedMakers[makers[m]->stringID]=makers[m];
 for(std::map<std::string,GameData*>::const_iterator makerIt=orderedMakers.begin();makerIt!=orderedMakers.end();++makerIt){GameData* source=makerIt->second;const Ogre::vector<GameDataReference>::type* extra=source->getReferenceListIfExists("weapon models");if(!extra)continue;
  for(size_t i=0;i<extra->size();++i){const GameDataReference& ref=(*extra)[i];if(!ref.ptr||ref.ptr->type!=MATERIAL_SPECS_WEAPON||ref.values.value[0]<=craftMax||ref.values.value[0]>100||!modelsSeen.insert(ref.ptr->stringID).second)continue;ArtisanGrade g;g.level=ref.values.value[0];g.maker=source;g.model=ref.ptr;g.name=ref.ptr->name;g.id=source->stringID+":"+g.model->stringID+":"+artisanNumber(g.level);artisanWeaponGrades.push_back(g);}
 }
 std::sort(artisanWeaponGrades.begin(),artisanWeaponGrades.end(),artisanGradeLess);
}
const ArtisanGrade* artisanGrade(const ArtisanOrders::Line& l){const std::vector<ArtisanGrade>& gs=l.kind?artisanWeaponGrades:artisanArmourGrades;for(size_t i=0;i<gs.size();++i)if(gs[i].id==l.quality)return &gs[i];return 0;}
bool artisanKnown(GameData* data){
 if(!data||!ou||!ou->player||!ou->player->technology)return false;
 return ArtisanLayout::known(ou->player->technology->knownObjectsByType,data);
}
int artisanVendorRole(GameData* data){
 if(!data)return 0;int roles=0;
 const Ogre::vector<GameDataReference>::type* vendors=data->getReferenceListIfExists("vendors");if(!vendors)return 0;
 for(size_t i=0;i<vendors->size();++i){GameData* v=(*vendors)[i].ptr;if(!v)continue;
  const Ogre::vector<GameDataReference>::type* items=v->getReferenceListIfExists("items");if(items)for(size_t j=0;j<items->size();++j){GameData* d=(*items)[j].ptr;if(d&&d->type==ARMOUR)roles|=1;if(d&&d->type==WEAPON)roles|=2;}
  const Ogre::vector<GameDataReference>::type* weapons=v->getReferenceListIfExists("weapons");if(weapons&&!weapons->empty())roles|=2;
  const Ogre::vector<GameDataReference>::type* armour=v->getReferenceListIfExists("clothing");if(armour&&!armour->empty())roles|=1;
 }
 return roles;
}
int artisanRoles(Character* c){MercenariePerf::Phase perf("eligibility-artisan");if(!c||c->isPlayerCharacter()||c->isDead())return 0;GameData* d=c->getGameData();if(!d)return 0;ActivePlatoon* p=c->getPlatoon();GameData* squad=p&&p->me?p->me->squadTemplate:0;bool trader=(d->bdata.find("is trader")!=d->bdata.end()&&d->bdata.find("is trader")->second)||(squad&&squad->bdata.find("is trader")!=squad->bdata.end()&&squad->bdata.find("is trader")->second);if(!trader)return 0;return artisanVendorRole(d)|artisanVendorRole(squad);}
Item* artisanMake(const ArtisanOrders::Line& l){
 try {artisanMakeError="factory unavailable";if(!ou||!ou->theFactory)return 0;GameData* d=ou->gamedata.getData(l.item,l.kind?WEAPON:ARMOUR);const ArtisanGrade* g=artisanGrade(l);if(!d||!g){artisanMakeError="missing data or grade";return 0;}
 Item* it=ou->theFactory->createItem(l.kind?g->maker:d,hand(),l.kind?d:0,g->model,g->level,0);
 if(!it){artisanMakeError="native factory returned null";return 0;} if(it->getLevel()!=g->level||!it->getGameData()||it->getGameData()->stringID!=l.item){artisanMakeError="native mismatch level="+artisanNumber(it->getLevel())+" expected="+artisanNumber(g->level)+" data="+(it->getGameData()?it->getGameData()->stringID:"null");delete it;return 0;}artisanMakeError.clear();return it;}catch(const std::exception& e){ErrorLog(std::string("Artisan item unavailable: ")+e.what());return 0;}
}
bool artisanQuote(ArtisanOrders::Line& l,bool knowledge){
 GameData* d=ou?ou->gamedata.getData(l.item,l.kind?WEAPON:ARMOUR):0;if(!d||(knowledge&&!artisanKnown(d)))return false;
 const std::vector<ArtisanGrade>& gs=l.kind?artisanWeaponGrades:artisanArmourGrades;bool unlocked=false;
 for(size_t i=0;i<gs.size()&&i<(size_t)std::max(0,guildLevel());++i)if(gs[i].id==l.quality)unlocked=true;if(knowledge&&!unlocked)return false;
 Item* it=artisanMake(l);if(!it)return false;int value=it->getValueSingle(false);delete it;
 if(value<0||value>1717986916){artisanMakeError="invalid native value="+artisanNumber(value);return false;}l.base=value;l.unit=ArtisanOrders::price(value);l.hours=ArtisanOrders::series(value,l.quantity);l.remaining=l.quantity;return true;
}
void artisanClearPreviews(){for(size_t i=0;i<artisanPreviews.size();++i)delete artisanPreviews[i];artisanPreviews.clear();}
void artisanClose(){artisanProgressWidgets.clear();artisanTimeWidgets.clear();artisanPercentWidgets.clear();artisanDetailLabels.clear();if(artisanWindow){mercenarieDestroyLiveWidget(artisanWindow);artisanWindow=0;}artisanBody=0;artisanBasketButton=0;artisanClearPreviews();artisanScroll=0;artisanRefresh=false;artisanShown.clear();artisanSpeaker.setNull();artisanReceiver.setNull();}
void artisanReset(){artisanRecoveryChoice.clear();artisanClose();artisanLedger=ArtisanOrders::Ledger();artisanArmourGrades.clear();artisanWeaponGrades.clear();artisanBusy=false;}
void artisanIcon(MyGUI::Widget* p,const ArtisanOrders::Line& l,int x,int y,int boxW=48,int boxH=60){Item* it=artisanMake(l);if(!it)return;artisanPreviews.push_back(it);std::string texture;iVector2 size;InventoryIcon::createIconImage(it,texture,size);if(texture.empty())return;float scale=std::min((float)boxW/std::max(1,size.x),(float)boxH/std::max(1,size.y));int iw=std::max(1,(int)(size.x*scale)),ih=std::max(1,(int)(size.y*scale));MyGUI::ImageBox* icon=p->createWidget<MyGUI::ImageBox>("ImageBox",x+(boxW-iw)/2,y+(boxH-ih)/2,iw,ih,MyGUI::Align::Default);icon->setImageTexture(texture);icon->setNeedMouseFocus(false);}
std::string artisanItemName(const ArtisanOrders::Line& l){GameData* d=ou?ou->gamedata.getData(l.item,l.kind?WEAPON:ARMOUR):0;return d?d->name:std::string(Loc::text("artisan.missing"))+" "+l.item;}
std::string artisanGradeName(const ArtisanOrders::Line& l){const ArtisanGrade* g=artisanGrade(l);return g?g->name:l.quality;}
#include "ArtisanTypography.h"
MyGUI::TextBox* artisanText(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text,const MyGUI::Colour& colour){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("ArtisanText",x,y,w,h,MyGUI::Align::Default);artisanSetFont(t,size);t->setTextColour(colour);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setNeedMouseFocus(false);int height=artisanWrap(t,text,w);t->setSize(w,std::max(h,height));return t;
}
void artisanDraw();
void artisanClick(MyGUI::Widget* sender){
 if(artisanBusy)return;std::string action=sender->getUserString("action");
 if(action=="close"){if(artisanWindow)artisanWindow->setVisible(false);return;}
 if(!artisanContext())return;
 if(action=="recover_orders"){artisanPage=4;artisanRecoveryChoice.clear();artisanDraw();return;}
 if(action=="recover_select"){artisanRecoveryChoice=sender->getUserString("source");artisanDraw();return;}
 if(action=="recover_apply"){
  const std::string from=sender->getUserString("source");if(from!=artisanRecoveryChoice||!ImportRecovery::held(importedArtisans,from))return;
  try{ArtisanOrders::Ledger recovered=ImportRecovery::resumeArtisan(artisanLedger,from,artisanKey,artisanName,artisanRoles(artisanSpeaker.getCharacter()),artisanNow(),importedArtisanHour);artisanLedger.swap(recovered);importedArtisans.erase(std::remove(importedArtisans.begin(),importedArtisans.end(),from),importedArtisans.end());artisanRecoveryChoice.clear();artisanNotice=Loc::text("import.artisan.done");artisanPage=3;}catch(const std::exception&){artisanNotice=Loc::text("import.artisan.unavailable");}
  artisanDraw();return;
 }
 if(ImportRecovery::held(importedArtisans,artisanKey)&&(action=="pay"||action=="collect"||action=="confirm"||action=="add"||action=="plus"||action=="minus"||action=="remove"||action=="clear")){artisanNotice=Loc::text("import.artisan.help");artisanDraw();return;}

 // Ignore stale callbacks after a successful payment or a changed quote.
 if((action=="pay"&&artisanPage!=2)||(action=="confirm"&&artisanPage!=1))return;
 if(action=="catalog"){artisanPage=0;artisanOffset=0;artisanGrades();artisanAuditOpening=true;}
 if(action=="basket")artisanPage=1;
 if(action=="orders")artisanPage=3;
 if(action=="order_filter"){artisanOrderFilter=atoi(sender->getUserString("row").c_str());artisanResetScroll=true;}
 if(action=="category"){artisanResetScroll=true;artisanCategory=atoi(sender->getUserString("row").c_str());artisanAuditOpening=true;}
 if(action=="view"){unsigned long id=strtoul(sender->getUserString("order").c_str(),0,10);artisanDetailOrder=artisanDetailOrder==id?0:id;}
 
 if(action=="clear")artisanLedger.baskets[artisanKey].clear();
 int row=atoi(sender->getUserString("row").c_str());
 if((action=="add"||action=="plus"||action=="minus"||action=="remove")&&row>=0&&(size_t)row<artisanShown.size()){
  ArtisanOrders::Line l=artisanShown[row];int quantity=l.quantity;
  if(action=="add"){quantity=l.quantity;const std::vector<ArtisanOrders::Line>& b=artisanLedger.baskets[artisanKey];for(size_t i=0;i<b.size();++i)if(b[i].item==l.item&&b[i].quality==l.quality)quantity=b[i].quantity+l.quantity;}
  else quantity=action=="remove"?0:quantity+(action=="plus"?1:-1);
  artisanLedger.put(artisanKey,l,quantity);
 }
 if((action=="pickplus"||action=="pickminus")&&row>=0&&(size_t)row<artisanShown.size()){const ArtisanOrders::Line& l=artisanShown[row];artisanQuantities[l.item+"|"+l.quality]=std::max(1,std::min(10,l.quantity+(action=="pickplus"?1:-1)));}
 if(action=="confirm"||action=="pay"){
  std::vector<ArtisanOrders::Line> quoted=artisanLedger.baskets[artisanKey];bool valid=!quoted.empty(),changed=false;
  for(size_t i=0;i<quoted.size();++i){ArtisanOrders::Line before=quoted[i];if(!(artisanRoles(artisanSpeaker.getCharacter())&(1<<quoted[i].kind))||!artisanQuote(quoted[i],true))valid=false;else if(before.unit!=quoted[i].unit||before.base!=quoted[i].base||before.hours!=quoted[i].hours)changed=true;}
  if(!valid){artisanNotice=Loc::text("artisan.invalid");artisanPage=1;}
  else {artisanLedger.baskets[artisanKey]=quoted;artisanPage=2;
   if(action=="pay"&&!changed){artisanBusy=true;try{ArtisanOrders::Ledger prepared=artisanLedger;int before=financeBalance();unsigned long id=prepared.confirm(artisanKey,artisanName,artisanNow(),before);
    if(id&&ArtisanClock::save(prepared,developerTimeOffsetHours).size()>8*1024*1024)id=0;
    if(!id)artisanNotice=Loc::text("artisan.funds");
    else {
     int debit=(int)prepared.orders.back().paid;bool track=financeReady();
     Finance::Journal journal=fiscalLedger.cash;
     if(track){journal.observe(before,currentGameHours);if(debit&&!journal.record(-debit,before-debit,currentGameHours,Finance::Equipment,"artisan.payment","artisan:"+artisanNumber(id)+":"+artisanKey))throw std::runtime_error("Equipment expense refused");}
     // All allocations precede the native debit; both commits below only swap.
     ou->player->participant->factionOwnerships->takeMoney(debit);int after=financeBalance();
     if(after==before-debit){artisanLedger.swap(prepared);if(track)fiscalLedger.cash.swap(journal);artisanPage=3;artisanNotice.clear();}
     else {if(after>=0&&after<before)ou->player->participant->factionOwnerships->addMoney(before-after);artisanNotice=Loc::text("artisan.invalid");}
    }
   }catch(const std::exception& e){ErrorLog(e.what());artisanNotice=Loc::text("artisan.invalid");}artisanBusy=false;}
   else if(changed){artisanNotice=Loc::text("artisan.changed");if(action=="pay")artisanPage=1;}
  }
 }
 if(action=="collect"){
  unsigned long id=strtoul(sender->getUserString("order").c_str(),0,10);Character* receiver=artisanReceiver.getCharacter();artisanBusy=true;
  for(size_t i=0;i<artisanLedger.orders.size();++i){ArtisanOrders::Order& o=artisanLedger.orders[i];if(o.id!=id||o.artisan!=artisanKey||o.status!=ArtisanOrders::Ready)continue;
   try{for(size_t j=0;j<o.lines.size();++j){while(o.lines[j].remaining>0){Item* it=artisanMake(o.lines[j]);if(!it){artisanNotice=Loc::text("artisan.missing");break;}if(!receiver->giveItem(it,false,false)){delete it;artisanNotice=Loc::text("artisan.partial");break;}artisanLedger.take(id,artisanKey,j);}}}catch(const std::exception& e){ErrorLog(e.what());artisanNotice=Loc::text("artisan.invalid");}
  }artisanBusy=false;
 }
 artisanRefresh=true;
}
MyGUI::Button* artisanButton(MyGUI::Widget* p,int x,int y,int w,const std::string& text,const char* action,int row=-1,const char* skin="ArtisanButton"){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>(skin,x,y,w,36,MyGUI::Align::Default);artisanSetFont(b,16);artisanWrap(b,text,w-16);b->setTextAlign(MyGUI::Align::Center);b->setUserString("action",action);b->setUserString("row",artisanNumber(row));b->eventMouseButtonClick+=MyGUI::newDelegate(artisanClick);return b;
}
int artisanItemCategory(GameData* d){int slot=ATTACH_NONE;if(d&&d->idata.find("slot")!=d->idata.end())slot=d->idata.find("slot")->second;return ArtisanLayout::category(slot);}
void artisanCategoryChanged(MyGUI::ComboBox*,size_t index){artisanCategory=(int)index;artisanOffset=0;artisanRefresh=true;}
void artisanFontTree(MyGUI::Widget* w){if(!w)return;if(artisanCommunityFont())MercenarieFonts::prepare(w);for(size_t i=0;i<w->getChildCount();++i)artisanFontTree(w->getChildAt(i));}
void artisanGradeChanged(MyGUI::ComboBox* cb,size_t index){artisanRowGrades[cb->getUserString("item")]=(int)index;artisanAuditOpening=true;artisanRefresh=true;}
void artisanSearchChanged(MyGUI::EditBox* e){artisanSearch=e->getOnlyText().asUTF8();artisanResetScroll=true;artisanOffset=0;artisanRefresh=true;}
#include "ArtisanView.h"
bool artisanReply(Dialogue* d,const std::string& id){if(id!="artisan.armour"&&id!="artisan.weapon"&&id!="artisan.track")return false;if(guildLevel()<1||!d)return true;Character* c=d->getCharacter();int roles=artisanRoles(c);if(!roles||(id=="artisan.armour"&&!(roles&1))||(id=="artisan.weapon"&&!(roles&2)))return true;Character* receiver=d->getConversationTarget().getCharacter();if(!receiver||!receiver->isPlayerCharacter())return true;artisanClose();artisanSpeaker=c->getHandle();artisanReceiver=receiver->getHandle();artisanKey=artisanIdentity(c);artisanName=c->getName();artisanKind=id=="artisan.weapon"||(id=="artisan.track"&&!(roles&1))?1:0;artisanPage=id=="artisan.track"?3:0;artisanOffset=artisanGradeIndex=artisanCategory=0;artisanSearch.clear();artisanRecoveryChoice.clear();artisanNotice=importedArtisans.empty()?"":Loc::text("import.artisan.help");artisanAuditOpening=true;artisanRowGrades.clear();artisanGrades();d->endDialogue(true);artisanDraw();return true;}
void artisanReplies(Dialogue* d){if(guildLevel()<1||!d)return;int roles=artisanRoles(d->getCharacter());if(!roles)return;const char* ids[]={"artisan.armour","artisan.weapon","artisan.track"};for(int i=0;i<3;++i){if(i<2&&!(roles&(1<<i)))continue;bool found=false;for(size_t j=0;j<d->replyIds.size();++j)if(d->replyIds[j]==ids[i])found=true;if(!found){d->replyIds.push_back(ids[i]);d->responses.push_back(Loc::text(ids[i]));}}}
void artisanTick(){
 if(missionWorldChanging||missionRestorePending||progressWriteBlocked||artisanBusy)return;
 static DWORD last=0;DWORD now=GetTickCount();if(!artisanRefresh&&now-last<250)return;last=now;double hour=artisanNow();for(size_t i=0;i<artisanLedger.orders.size();++i){const ArtisanOrders::Order& o=artisanLedger.orders[i];if((o.status==ArtisanOrders::Waiting&&hour>=o.start)||(o.status==ArtisanOrders::Making&&hour>=o.end))artisanRefresh=true;}ImportRecovery::tickArtisans(artisanLedger,importedArtisans,hour);
 std::set<std::string> checked;
 for(size_t i=0;i<artisanLedger.orders.size();++i){ArtisanOrders::Order& o=artisanLedger.orders[i];if(ImportRecovery::held(importedArtisans,o.artisan))continue;if(checked.insert(o.artisan).second){Character* c=artisanResolve(o.artisan);if(c&&c->isDead()&&artisanLedger.dead(o.artisan)){ou->showPlayerAMessage(std::string(Loc::text("artisan.death"))+" "+o.name,true);artisanRefresh=true;}}
  if(o.status==ArtisanOrders::Ready&&!o.notified){o.notified=true;ou->showPlayerAMessage(Loc::named("artisan.notification","artisan",o.name),true);artisanRefresh=true;}
 }
 if(!MyGUI::Gui::getInstancePtr()){artisanWindow=0;artisanScroll=0;return;}
 if(artisanWindow&&!mercenarieFindLiveWidget(MyGUI::Gui::getInstance().getEnumerator(),artisanWindow)){artisanWindow=0;artisanScroll=0;artisanProgressWidgets.clear();artisanTimeWidgets.clear();artisanPercentWidgets.clear();artisanDetailLabels.clear();artisanClearPreviews();}
 if(artisanWindow&&artisanWindow->getVisible()&&!artisanContext()){artisanClose();return;}
 if(artisanWindow&&artisanWindow->getVisible()){
  for(size_t i=0;i<artisanLedger.orders.size();++i){const ArtisanOrders::Order& o=artisanLedger.orders[i];std::map<unsigned long,MyGUI::TextBox*>::iterator time=artisanTimeWidgets.find(o.id);if(time!=artisanTimeWidgets.end())artisanWrap(time->second,(o.status==ArtisanOrders::Waiting||o.status==ArtisanOrders::Making)?artisanTime(o.end-hour):"-",time->second->getWidth());
   std::map<unsigned long,MyGUI::Widget*>::iterator bar=artisanProgressWidgets.find(o.id);if(bar!=artisanProgressWidgets.end()&&o.status==ArtisanOrders::Making){double ratio=std::max(0.0,std::min(1.0,(hour-o.start)/(o.end-o.start)));bar->second->setVisible(ratio>0);bar->second->setSize(std::max(1,(int)(atoi(bar->second->getUserString("Width").c_str())*ratio)),bar->second->getHeight());
    std::map<unsigned long,MyGUI::TextBox*>::iterator percent=artisanPercentWidgets.find(o.id);if(percent!=artisanPercentWidgets.end())artisanCaption(percent->second,artisanNumber((int)(ratio*100))+" %");}}
  artisanUpdateDetails();artisanFontTree(artisanWindow);const MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();if(artisanRefresh||v!=artisanViewSize||artisanLanguage!=Loc::engine().language)artisanDraw();}else artisanRefresh=false;
}


}
