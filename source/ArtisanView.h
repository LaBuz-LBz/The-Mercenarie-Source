// Included inside the artisan adapter namespace. Transactions remain in ArtisanNative.h.
MyGUI::ScrollView* artisanList(MyGUI::Widget* parent,int x,int y,int w,int h){
 MyGUI::ScrollView* s=parent->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",x,y,w,h,MyGUI::Align::Default);
 s->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);s->setVisibleHScroll(false);MercenarieNativeInput::bind(s);return s;
}
void artisanAccent(MyGUI::Button* b,bool active){
 if(active){b->setTextColour(registerAmber);popupBorder(b,b->getWidth(),b->getHeight(),registerAmber);}
}
void artisanDivider(MyGUI::Widget* p,int y,int w){registerSolid(p,0,y,w,1,MyGUI::Colour(.25f,.24f,.20f));}
std::string artisanMoney(long long n);
std::string artisanBasketCaption(){
 const std::vector<ArtisanOrders::Line>& b=artisanLedger.baskets[artisanKey];
 return std::string(Loc::text("artisan.basket"))+" ("+artisanNumber(b.size())+")\n"+artisanMoney(ArtisanOrders::total(b));
}
struct ArtisanAudit {
 int loaded,finished,enabled,armour,weapons,known,profession,category,quality,creation,search,shown;
 ArtisanAudit():loaded(0),finished(0),enabled(0),armour(0),weapons(0),known(0),profession(0),category(0),quality(0),creation(0),search(0),shown(0){}
 void write(){
  std::ostringstream s;s<<"Artisan catalogue audit: kind="<<artisanKind<<" category="<<artisanCategory<<" guild="<<guildLevel()
   <<" relevant_research="<<loaded<<" finished="<<finished<<" enabledObjects="<<enabled
   <<" ARMOUR="<<armour<<" WEAPON="<<weapons<<" known="<<known<<" rejected_profession="<<profession
   <<" rejected_category="<<category<<" rejected_quality="<<quality<<" rejected_creation_or_quote="<<creation
   <<" rejected_search="<<search<<" FINAL="<<shown;DebugLog(s.str());
 }
};
void artisanCatalogue(){
 ArtisanAudit audit;Research* r=ou->player->technology;
 if(r){audit.finished=(int)r->finished.size();audit.enabled=(int)r->enabledObjects.size();}
 if(artisanAuditOpening){
  lektor<GameData*> tech;ou->gamedata.getDataOfType(tech,RESEARCH);
  const char* fields[]={"enable armour","enable weapon type","enable weapon model"};
  for(size_t i=0;i<tech.size();++i){bool relevant=false;for(int f=0;f<3;++f){const Ogre::vector<GameDataReference>::type* refs=tech[i]->getReferenceListIfExists(fields[f]);if(refs&&!refs->empty())relevant=true;}if(relevant)++audit.loaded;}
 }
 const std::vector<ArtisanGrade>& grades=artisanKind?artisanWeaponGrades:artisanArmourGrades;
 size_t n=std::min(grades.size(),(size_t)std::max(0,guildLevel()));
 for(int kind=0;kind<2;++kind){lektor<GameData*> items;ou->gamedata.getDataOfType(items,kind?WEAPON:ARMOUR);
  if(kind)audit.weapons=(int)items.size();else audit.armour=(int)items.size();
  for(size_t i=0;i<items.size();++i){GameData* d=items[i];if(!artisanKnown(d))continue;++audit.known;
   if(kind!=artisanKind){++audit.profession;continue;}
   if(!kind&&artisanCategory&&artisanItemCategory(d)!=artisanCategory){++audit.category;continue;}
   if(!artisanSearch.empty()){
    std::wstring name=MyGUI::UString(d->name).asWStr(),query=MyGUI::UString(artisanSearch).asWStr();
    if(!name.empty())CharLowerBuffW(&name[0],(DWORD)name.size());
    if(!query.empty())CharLowerBuffW(&query[0],(DWORD)query.size());
    bool found=name.find(query)!=std::wstring::npos;
    if(!found){++audit.search;continue;}
   }
   if(!n){++audit.quality;continue;}
   int& grade=artisanRowGrades[d->stringID];if(grade<0||(size_t)grade>=n)grade=0;
   ArtisanOrders::Line l;l.item=d->stringID;l.kind=kind;l.quality=grades[grade].id;
   int chosen=artisanQuantities[l.item+"|"+l.quality];l.quantity=l.remaining=chosen?chosen:1;
   if(!artisanQuote(l,true)){++audit.creation;if(artisanAuditOpening)DebugLog("Artisan rejected item="+l.item+" grade="+l.quality+" reason="+artisanMakeError);continue;}
   artisanShown.push_back(l);++audit.shown;
  }
 }
 if(artisanAuditOpening){
  if(r){std::ostringstream detail;detail<<"Artisan native known table:";
   const itemType types[]={ARMOUR,WEAPON,MATERIAL_SPECS_WEAPON};const char* names[]={" ARMOUR="," WEAPON="," WEAPON_MODEL="};
   for(int i=0;i<3;++i){Ogre::map<itemType,lektor<GameData*> >::type::const_iterator entry=r->knownObjectsByType.find(types[i]);detail<<names[i]<<(entry==r->knownObjectsByType.end()?0:entry->second.size());}
   DebugLog(detail.str());
  }
  audit.write();artisanAuditOpening=false;
 }
}
void artisanQuality(MyGUI::Widget* p,const ArtisanOrders::Line& l,int x,int y,int width){
 MyGUI::ComboBox* cb=p->createWidget<MyGUI::ComboBox>("ArtisanCombo",x,y,width,40,MyGUI::Align::Default);
 artisanSetFont(cb,16);cb->setComboModeDrop(true);cb->setUserString("item",l.item);
 const std::vector<ArtisanGrade>& gs=l.kind?artisanWeaponGrades:artisanArmourGrades;
 size_t n=std::min(gs.size(),(size_t)std::max(0,guildLevel()));
 for(size_t i=0;i<n;++i)cb->addItem(gs[i].name);
 cb->setMaxListLength(200);if(n)cb->setIndexSelected(artisanRowGrades[l.item]);cb->eventComboChangePosition+=MyGUI::newDelegate(artisanGradeChanged);
 for(int f=16;f>=12;f-=2){artisanSetFont(cb,f);if(cb->getTextSize().width<=width-50)break;}
}
void artisanQuantity(MyGUI::Widget* p,int x,int y,const ArtisanOrders::Line& l,int row,bool catalog){
 artisanButton(p,x,y,30,"-",catalog?"pickminus":"minus",row);
 artisanText(p,x+32,y+6,28,26,18,artisanNumber(l.quantity),registerIvory);
 artisanButton(p,x+62,y,30,"+",catalog?"pickplus":"plus",row);
}
#include "ArtisanStyle.h"
int artisanRows(MyGUI::ScrollView* s,int width,bool wide){
 if(artisanShown.empty())return artisanEmpty(s,width,s->getHeight(),artisanPage==0?"artisan.empty_catalog_title":"artisan.empty_cart_title",artisanPage==0?"artisan.empty_catalog":"artisan.empty_cart_help",artisanPage==0?1:0);
 int cy=0;
 if(artisanPage==0){
  ArtisanLayout::Row c(width,wide);std::vector<int> edges;edges.push_back(0);edges.push_back(c.qualityX-8);edges.push_back(c.priceX-8);edges.push_back(c.quantityX-8);edges.push_back(c.addX-8);edges.push_back(width-1);
  if(wide){const char* titles[]={"artisan.object","artisan.available_quality","artisan.price","artisan.quantity","artisan.actions"};
   for(int j=0;j<5;++j)cy=std::max(cy,artisanCell(s,edges[j],0,edges[j+1]-edges[j],Loc::text(titles[j]),true));artisanGrid(s,edges,0,cy);
  }
  for(size_t i=0;i<artisanShown.size();++i){const ArtisanOrders::Line& l=artisanShown[i];int top=cy;
   artisanIcon(s,l,8,top+8);
   MyGUI::TextBox* name=artisanText(s,68,top+13,wide?c.qualityX-86:width-84,40,18,artisanItemName(l),registerIvory);
   int controls=wide?top+14:top+std::max(74,name->getHeight()+24);
   artisanQuality(s,l,c.qualityX,controls,c.qualityWidth);
   MyGUI::TextBox* price=artisanText(s,c.priceX,controls,c.priceWidth-10,42,16,artisanMoney(l.unit)+"\n"+Loc::text("artisan.base")+" "+artisanNumber(l.base),registerIvory);
   artisanQuantity(s,c.quantityX,controls+2,l,(int)i,true);
   MyGUI::Button* add=artisanAction(s,c.addX,controls,c.addWidth,42,"    "+std::string(Loc::text("artisan.add")),"add",true,(int)i);artisanMark(add,8,9,24,0,registerAmber);
   cy=std::max(top+std::max(80,name->getHeight()+22),controls+std::max(44,price->getHeight())+10);
   if(wide)artisanGrid(s,edges,top,cy-top);else artisanDivider(s,cy,width);
  }return cy;
 }
 const bool basket=artisanPage==1;std::vector<int> e;e.push_back(0);
 const int ratesCart[]={30,43,53,64,75,85,95,100},ratesConfirm[]={32,49,57,72,87,100};
 int columns=basket?8:6;for(int j=0;j<columns;++j)e.push_back(width*(basket?ratesCart[j]:ratesConfirm[j])/100-(j==columns-1?1:0));
 const char* headers[]={"artisan.object","artisan.quality","artisan.quantity","artisan.unit_price","artisan.subtotal","artisan.unit_time","artisan.total_time","artisan.actions"};
 for(int j=0;j<columns;++j)cy=std::max(cy,artisanCell(s,e[j],0,e[j+1]-e[j],Loc::text(!basket&&j==5?"artisan.total_time":!basket&&j==2?"artisan.short_quantity":headers[j]),true));artisanGrid(s,e,0,cy);
 for(size_t i=0;i<artisanShown.size();++i){const ArtisanOrders::Line& l=artisanShown[i];int top=cy,rh=80;artisanIcon(s,l,8,top+8);
  MyGUI::TextBox* name=artisanText(s,68,top+12,e[1]-82,44,18,artisanItemName(l),registerIvory);rh=std::max(rh,name->getHeight()+24);
  rh=std::max(rh,artisanCell(s,e[1],top,e[2]-e[1],artisanGradeName(l)));
  if(basket)artisanQuantity(s,e[2]+8,top+18,l,(int)i,false);else artisanCell(s,e[2],top,e[3]-e[2],artisanNumber(l.quantity));
  rh=std::max(rh,artisanCell(s,e[3],top,e[4]-e[3],artisanMoney(l.unit)));
  rh=std::max(rh,artisanCell(s,e[4],top,e[5]-e[4],artisanMoney(l.unit*l.quantity)));
  if(basket){rh=std::max(rh,artisanCell(s,e[5],top,e[6]-e[5],artisanTime(ArtisanOrders::unitHours(l.base))));rh=std::max(rh,artisanCell(s,e[6],top,e[7]-e[6],artisanTime(l.hours)));
   MyGUI::Button* b=artisanAction(s,e[7]+8,top+14,e[8]-e[7]-16,44,"","remove",false,(int)i);artisanMark(b,(b->getWidth()-28)/2,8,28,2,registerIvory);b->setUserString("ToolTip",Loc::text("artisan.remove"));
  }else rh=std::max(rh,artisanCell(s,e[5],top,e[6]-e[5],artisanTime(l.hours)));
  cy+=rh;artisanGrid(s,e,top,rh);
 }return cy;
}
const char* artisanStateKey(int status){const char* states[]={"artisan.waiting","artisan.making","artisan.ready","artisan.collected","artisan.lost"};return states[status];}
bool artisanOrderVisible(const ArtisanOrders::Order& o){
 if(o.artisan!=artisanKey)return false;
 if(artisanOrderFilter==0)return o.status==ArtisanOrders::Making||o.status==ArtisanOrders::Waiting;
 if(artisanOrderFilter==1)return o.status==ArtisanOrders::Ready||o.status==ArtisanOrders::Collected;
 return true;
}
int artisanOrderRows(MyGUI::ScrollView* s,int width){
 int count=0;for(size_t i=0;i<artisanLedger.orders.size();++i)if(artisanOrderVisible(artisanLedger.orders[i]))++count;
 if(!count)return artisanEmpty(s,width,s->getHeight(),"artisan.empty_orders_title","artisan.empty_orders_help",21);
 int cy=0;std::vector<int> e;e.push_back(0);const int rates[]={6,27,37,60,73,87,100};for(int j=0;j<7;++j)e.push_back(width*rates[j]/100-(j==6?1:0));
 const char* heads[]={"artisan.number","artisan.object","artisan.total_quantity","artisan.progress","artisan.remaining","artisan.status","artisan.actions"};
 for(int j=0;j<7;++j)cy=std::max(cy,artisanCell(s,e[j],0,e[j+1]-e[j],Loc::text(heads[j]),true));artisanGrid(s,e,0,cy);
 const int sections[]={ArtisanOrders::Making,ArtisanOrders::Waiting,ArtisanOrders::Ready,ArtisanOrders::Collected,ArtisanOrders::Lost};
 for(int section=0;section<5;++section)for(size_t i=0;i<artisanLedger.orders.size();++i){const ArtisanOrders::Order& o=artisanLedger.orders[i];if(!artisanOrderVisible(o)||o.status!=sections[section])continue;
  int top=cy,rh=o.status==ArtisanOrders::Ready?110:96;
  MyGUI::Colour color=o.status==ArtisanOrders::Ready?MyGUI::Colour(.48f,.85f,.48f):(o.status==ArtisanOrders::Lost?MyGUI::Colour(.95f,.32f,.22f):o.status==ArtisanOrders::Collected?registerIvory:o.status==ArtisanOrders::Waiting?MyGUI::Colour(.60f,.62f,.60f):registerAmber);
  artisanCell(s,e[0],top,e[1]-e[0],"#"+artisanNumber(o.id));int quantity=0,shown=std::min(std::max(1,std::min(4,(e[2]-e[1]-54)/64)),(int)o.lines.size());
  for(size_t j=0;j<o.lines.size();++j){quantity+=o.lines[j].quantity;if((int)j<shown)artisanIcon(s,o.lines[j],e[1]+8+(int)j*64,top+12,56,68);}
  if((int)o.lines.size()>shown)artisanText(s,e[1]+shown*64+10,top+30,e[2]-e[1]-shown*64-16,26,16,"+"+artisanNumber(o.lines.size()-shown),registerAmber);
  artisanCell(s,e[2],top,e[3]-e[2],artisanNumber(quantity));
  int barW=e[4]-e[3]-84;double ratio=artisanRatio(o);MyGUI::Widget* trough=artisanPanel(s,e[3]+10,top+29,barW,34);
  MyGUI::Widget* bar=registerSolid(trough,3,3,std::max(1,(int)((barW-6)*ratio)),28,color);bar->setUserString("Width",artisanNumber(barW-6));bar->setVisible(ratio>0);artisanProgressWidgets[o.id]=bar;
  artisanPercentWidgets[o.id]=artisanText(s,e[4]-66,top+29,60,32,20,artisanNumber((int)(ratio*100))+" %",registerIvory);
  artisanTimeWidgets[o.id]=artisanText(s,e[4]+10,top+26,e[5]-e[4]-20,40,20,(o.status==ArtisanOrders::Making||o.status==ArtisanOrders::Waiting)?artisanTime(o.end-artisanNow()):"-",registerIvory);rh=std::max(rh,artisanTimeWidgets[o.id]->getHeight()+32);
  int mark=o.status==ArtisanOrders::Making?3:o.status==ArtisanOrders::Waiting?11:o.status==ArtisanOrders::Lost?9:5;artisanMark(s,e[5]+8,top+25,36,mark,color);
  MyGUI::TextBox* status=artisanText(s,e[5]+52,top+20,e[6]-e[5]-60,48,18,Loc::text(artisanStateKey(o.status)),color);rh=std::max(rh,status->getHeight()+28);
  int aw=e[7]-e[6]-20;MyGUI::Button* view=artisanAction(s,e[6]+10,top+(o.status==ArtisanOrders::Ready?6:22),aw,o.status==ArtisanOrders::Ready?44:50,Loc::text("artisan.view"),"view");view->setUserString("order",artisanNumber(o.id));
  if(o.status==ArtisanOrders::Ready){MyGUI::Button* take=artisanAction(s,e[6]+10,top+56,aw,44,Loc::text("artisan.collect"),"collect");take->setUserString("order",artisanNumber(o.id));}
  cy+=rh;artisanGrid(s,e,top,rh);
 }return cy;
}
std::string artisanLineStatus(const ArtisanOrders::Order& o,size_t index){
 const ArtisanOrders::Line& l=o.lines[index];double start=o.start;for(size_t j=0;j<index;++j)start+=o.lines[j].hours;
 if(o.status==ArtisanOrders::Making){
  if(artisanNow()>=start+l.hours)return Loc::text("artisan.fabricated");
  if(artisanNow()<start)return Loc::text("artisan.waiting");
  int percent=(int)(100*std::max(0.0,std::min(1.0,(artisanNow()-start)/l.hours)));
  return std::string(Loc::text("artisan.making"))+" ("+artisanNumber(percent)+" %)";
 }
 return std::string(Loc::text(artisanStateKey(o.status)))+(o.status==ArtisanOrders::Ready?" | "+std::string(Loc::text("artisan.stored"))+" "+artisanNumber(l.remaining):"");
}
void artisanUpdateDetails(){
 for(size_t i=0;i<artisanLedger.orders.size();++i){const ArtisanOrders::Order& o=artisanLedger.orders[i];if(o.artisan!=artisanKey||o.id!=artisanDetailOrder)continue;
  for(std::map<size_t,MyGUI::TextBox*>::iterator it=artisanDetailLabels.begin();it!=artisanDetailLabels.end();++it)if(it->first<o.lines.size())artisanWrap(it->second,artisanLineStatus(o,it->first),it->second->getWidth());
 }
}
void artisanDetails(MyGUI::Widget* parent,int w,int h){
 MyGUI::ScrollView* s=artisanList(parent,12,10,w-24,h-20);int cy=0;const ArtisanOrders::Order* chosen=0;
 for(size_t i=0;i<artisanLedger.orders.size();++i)if(artisanLedger.orders[i].artisan==artisanKey&&artisanLedger.orders[i].id==artisanDetailOrder)chosen=&artisanLedger.orders[i];
 MyGUI::TextBox* heading=artisanHeading(s,4,0,w-52,34,22,std::string(Loc::text("artisan.details"))+(chosen?" #"+artisanNumber(chosen->id):""));cy=heading->getHeight()+14;artisanDivider(s,cy-4,w-48);
 if(chosen){for(size_t i=0;i<chosen->lines.size();++i){const ArtisanOrders::Line& l=chosen->lines[i];artisanIcon(s,l,4,cy+6,56,68);bool wide=w>=680;int statusW=wide?200:0;
  MyGUI::TextBox* name=artisanText(s,76,cy+10,w-124-statusW,46,18,artisanItemName(l)+" - "+artisanGradeName(l)+" x"+artisanNumber(l.quantity),registerIvory);
  int sx=wide?w-240:76,sy=wide?cy+12:cy+name->getHeight()+14,sw=wide?190:w-124;
  MyGUI::TextBox* status=artisanText(s,sx,sy,sw,50,16,artisanLineStatus(*chosen,i),registerAmber);artisanDetailLabels[i]=status;
  cy=std::max(cy+90,std::max(cy+name->getHeight()+20,sy+status->getHeight()+12));artisanDivider(s,cy,w-48);
 }}else {MyGUI::Widget* empty=s->createWidget<MyGUI::Widget>("PanelEmpty",0,cy,w-48,std::max(180,h-cy-32),MyGUI::Align::Default);empty->setNeedMouseFocus(false);cy+=artisanEmpty(empty,w-48,std::max(180,h-cy-32),"artisan.details","artisan.select_order",21);}
 s->setCanvasSize(w-48,cy+8);
}
void artisanInformation(MyGUI::Widget* p,int w,int h,bool confirmation){
 MyGUI::ScrollView* s=artisanList(p,14,12,w-28,h-24);int cy=0;
 MyGUI::TextBox* title=artisanHeading(s,4,0,w-60,34,22,Loc::text(confirmation?"artisan.important":"artisan.reminder"));cy=title->getHeight()+18;artisanDivider(s,cy-8,w-54);
 const char* confirm[]={"artisan.info.payment","artisan.info.cancel","artisan.info.refund","artisan.info.queue_short","artisan.info.series","artisan.info.group"};
 const char* reminder[]={"artisan.info.visit","artisan.info.notify","artisan.info.storage","artisan.info.death"};const int marks[]={8,9,10,3,1,6};
 for(int i=0;i<(confirmation?6:4);++i){artisanMark(s,4,cy+2,40,confirmation?marks[i]:4,confirmation&&(i==1||i==2)?MyGUI::Colour(.95f,.3f,.22f):registerAmber);MyGUI::TextBox* t=artisanText(s,60,cy,w-118,40,18,Loc::text(confirmation?confirm[i]:reminder[i]),registerIvory);cy+=std::max(48,t->getHeight())+18;}
 s->setCanvasSize(w-52,cy+4);
}
void artisanTotals(MyGUI::Widget* p,int w,int h){
 const std::vector<ArtisanOrders::Line>& b=artisanLedger.baskets[artisanKey];int count=0;long long base=0,total=ArtisanOrders::total(b);for(size_t i=0;i<b.size();++i){count+=b[i].quantity;base+=b[i].base*b[i].quantity;}
 MyGUI::ScrollView* s=artisanList(p,12,10,w-24,h-20);int cy=2;const char* keys[]={"artisan.count","artisan.total","artisan.duration"};std::string values[]={artisanNumber(count),artisanMoney(total),artisanTime(ArtisanOrders::duration(b))};
 for(int i=0;i<3;++i){int labelW=(w-60)*53/100;MyGUI::TextBox* a=artisanText(s,6,cy,labelW,30,18,Loc::text(keys[i]),registerIvory);MyGUI::TextBox* v=artisanText(s,labelW+16,cy,w-70-labelW,34,i?28:22,values[i],i?registerAmber:registerIvory);cy+=std::max(a->getHeight(),v->getHeight())+6;}
 MyGUI::TextBox* fee=artisanText(s,6,cy+6,w-60,26,14,std::string(Loc::text("artisan.base"))+" "+artisanMoney(base)+"  |  "+Loc::text("artisan.service")+" "+artisanMoney(total-base),MyGUI::Colour(.65f,.66f,.60f));cy+=fee->getHeight()+18;s->setCanvasSize(w-48,cy);
}
void artisanDraw(){
 if(!artisanContext()){artisanClose();return;}artisanEnsureUi();
 const MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();ArtisanLayout::Layout layout(v.width,v.height);
 bool live=artisanWindow&&mercenarieFindLiveWidget(MyGUI::Gui::getInstance().getEnumerator(),artisanWindow);
 bool reuse=live&&artisanPage==0&&artisanDrawnPage==0&&v==artisanViewSize&&artisanLanguage==Loc::engine().language;
 MyGUI::IntPoint offset;if(live&&artisanScroll&&artisanDrawnPage==artisanPage)offset=artisanScroll->getViewOffset();if(artisanLanguage!=Loc::engine().language)artisanGrades();
 artisanProgressWidgets.clear();artisanTimeWidgets.clear();artisanPercentWidgets.clear();artisanDetailLabels.clear();
 if(reuse){if(artisanBody)mercenarieDestroyLiveWidget(artisanBody);}else{if(artisanWindow)mercenarieDestroyLiveWidget(artisanWindow);artisanWindow=0;artisanBasketButton=0;}
 artisanBody=0;artisanScroll=0;artisanClearPreviews();artisanShown.clear();artisanViewSize=v;artisanLanguage=Loc::engine().language;
 int w=layout.width,h=layout.height,pad=w<1000?12:20,header=w<1000?102:92,bw=w-2*pad,bh=h-header-88;
 int sidebar=layout.side;
 if(!reuse){
  artisanWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Widget>("ArtisanFrame",MyGUI::IntCoord((v.width-w)/2,(v.height-h)/2,w,h),MyGUI::Align::Default,"Popup","MercenarieArtisan");
  artisanImage(artisanWindow,8,8,w-16,h-16,"MercenarieArtisanSurface.png",.52f);
  const char* titles[]={"artisan.final.catalog","artisan.final.cart","artisan.final.confirm","artisan.final.orders"};const char* subtitles[]={"artisan.subtitle.catalog","artisan.subtitle.basket","artisan.subtitle.confirm","artisan.subtitle.orders"};
  int titleW=w>=1200?w*56/100:w-84;artisanHeading(artisanWindow,pad+4,18,titleW,42,w<1000?28:34,Loc::text(titles[std::min(3,artisanPage)]));
  if(w>=1200)artisanText(artisanWindow,titleW+24,20,w-titleW-100,48,18,Loc::text(subtitles[std::min(3,artisanPage)]),registerIvory);else artisanText(artisanWindow,pad+4,60,w-2*pad-12,34,14,Loc::text(subtitles[std::min(3,artisanPage)]),registerIvory);
  MyGUI::Button* close=artisanAction(artisanWindow,w-pad-38,18,38,38,"","close");artisanMark(close,9,9,20,22,registerIvory);
  if(artisanPage==0){int sx=pad+sidebar+14,cartW=w<1000?198:290,sw=w-pad-cartW-14-sx;
   artisanText(artisanWindow,sx,header+2,sw,24,14,Loc::text("artisan.search_hint"),registerIvory);
   MyGUI::EditBox* search=artisanWindow->createWidget<MyGUI::EditBox>("Kenshi_EditBox",sx,header+28,sw,40,MyGUI::Align::Default);artisanSetFont(search,18);artisanCaption(search,artisanSearch);search->eventEditTextChange+=MyGUI::newDelegate(artisanSearchChanged);
   artisanBasketButton=artisanAction(artisanWindow,w-pad-cartW,header,cartW,72,artisanBasketCaption(),"basket",true);artisanMark(artisanBasketButton,10,22,30,0,registerAmber);artisanMark(artisanBasketButton,cartW-27,26,20,20,registerAmber);
  }
 }
 if(artisanBasketButton){for(int f=22;f>=14;f-=2){artisanSetFont(artisanBasketButton,f);if(artisanWrap(artisanBasketButton,artisanBasketCaption(),artisanBasketButton->getWidth()-68)<=60)break;}}
 artisanBody=artisanWindow->createWidget<MyGUI::Widget>("PanelEmpty",pad,header,bw,bh+76,MyGUI::Align::Default);
 artisanBody->setNeedMouseFocus(false);
 // MyGUI otherwise excludes this entire subtree from mouse picking.
 // Keep the body transparent over search/cart, but allow its children to react.
 artisanBody->setInheritsPick(true);
 MyGUI::ScrollView* list=0;int contentW=0,cy=0;
 if(artisanPage==0){
  MyGUI::Widget* panel=artisanPanel(artisanBody,0,0,sidebar,bh);if(sidebar>=340)artisanMark(panel,14,14,42,1,registerIvory);artisanHeading(panel,sidebar>=340?66:12,16,sidebar>=340?sidebar-78:sidebar-24,40,sidebar>=340?24:20,Loc::text(artisanKind?"artisan.trade.weapon":"artisan.trade.armour"));artisanDivider(panel,68,sidebar);
  bool split=sidebar>=340;int identity=split?sidebar*52/100:0,catW=sidebar-identity-24;
  if(split){artisanImage(panel,8,76,identity-12,bh-88,"MercenarieArtisanWorkshop.png");artisanMark(panel,(identity-112)/2,124,112,1,registerAmber);
   artisanText(panel,20,266,identity-40,60,22,artisanName,registerIvory);MyGUI::TextBox* quote=artisanText(panel,20,std::max(346,bh-174),identity-40,90,18,Loc::text("artisan.flavour"),registerIvory);
  }
  MyGUI::ScrollView* side=artisanList(panel,identity+8,80,catW+8,bh-90);int sy=0;
  if(!split){MyGUI::TextBox* name=artisanText(side,6,0,catW-16,40,16,artisanName,registerAmber);sy=name->getHeight()+14;}
  const int categories[]={0,2,4,1,7,5,8,3,6},icons[]={7,12,13,14,15,16,17,18,19};lektor<GameData*> armour;ou->gamedata.getDataOfType(armour,ARMOUR);std::set<int> supported;for(size_t i=0;i<armour.size();++i)supported.insert(artisanItemCategory(armour[i]));
  for(int i=0;i<(artisanKind?1:9);++i){int c=categories[i];if((c==7||c==8)&&supported.find(c)==supported.end())continue;
   int buttonW=catW-12;MyGUI::Button* b=artisanAction(side,2,sy,buttonW,52,"    "+std::string(Loc::text(("artisan.category."+artisanNumber(c)).c_str())),"category",artisanCategory==c,c);artisanMark(b,8,14,24,icons[i],artisanCategory==c?registerAmber:registerIvory);sy+=58;
  }side->setCanvasSize(catW-16,sy+4);
  int lx=sidebar+14,lw=bw-lx;artisanPanel(artisanBody,lx,82,lw,bh-82);list=artisanList(artisanBody,lx+5,87,lw-10,bh-92);contentW=lw-32;artisanCatalogue();cy=artisanRows(list,contentW,layout.wideRows);
 }else if(artisanPage==1){
  artisanShown=artisanLedger.baskets[artisanKey];int listH=std::min(350,bh*46/100),lowerY=listH+16,lowerH=bh-lowerY;
  artisanPanel(artisanBody,0,0,bw,listH);list=artisanList(artisanBody,5,5,bw-10,listH-10);contentW=std::max(1160,bw-32);list->setVisibleHScroll(true);cy=artisanRows(list,contentW,true);
  int left=bw*37/100,sw=bw-left-18;MyGUI::Button* clear=artisanAction(artisanBody,0,lowerY+8,std::min(left-10,300),50,"    "+std::string(Loc::text("artisan.clear")),"clear");artisanMark(clear,12,12,28,2,registerIvory);
  if(lowerH>100)artisanImage(artisanBody,0,lowerY+72,left-10,lowerH-72,"MercenarieArtisanWorkshop.png",.8f);
  int noteH=lowerH<260?54:76,summaryH=lowerH-noteH-8;MyGUI::Widget* summary=artisanPanel(artisanBody,left+18,lowerY,sw,summaryH);artisanTotals(summary,sw,summaryH-80);
  MyGUI::Button* proceed=artisanAction(summary,16,summaryH-74,sw-32,62,Loc::text("artisan.review"),"confirm",true);proceed->setEnabled(!artisanShown.empty());
  MyGUI::ScrollView* note=artisanList(artisanBody,left+18,lowerY+summaryH+8,sw,noteH);artisanMark(note,10,4,32,4,registerAmber);MyGUI::TextBox* t=artisanText(note,54,2,sw-86,48,18,Loc::text("artisan.info.queue_short"),registerIvory);note->setCanvasSize(sw-24,t->getHeight()+8);
 }else if(artisanPage==2){
  artisanShown=artisanLedger.baskets[artisanKey];int left=(bw-16)*63/100,right=bw-left-16;int tableH=std::min(std::max(120,bh-306),std::max(170,60+(int)artisanShown.size()*80));int totalsH=std::min(210,bh-tableH-128),frameH=tableH+totalsH+48;
  MyGUI::Widget* frame=artisanPanel(artisanBody,0,0,left,frameH);artisanHeading(frame,16,10,left-32,34,24,Loc::text("artisan.summary"));artisanDivider(frame,48,left);
  list=artisanList(frame,5,54,left-10,tableH-6);contentW=std::max(920,left-32);list->setVisibleHScroll(true);cy=artisanRows(list,contentW,true);
  MyGUI::Widget* totals=frame->createWidget<MyGUI::Widget>("PanelEmpty",6,tableH+50,left-12,totalsH-6,MyGUI::Align::Default);artisanTotals(totals,left-12,totalsH-6);
  int backW=(left-16)*43/100;artisanAction(artisanBody,0,frameH+16,backW,64,Loc::text("artisan.back_basket"),"basket");
  MyGUI::Button* pay=artisanAction(artisanBody,backW+16,frameH+16,left-backW-16,64,std::string(Loc::text("artisan.confirm_order"))+"\n("+artisanMoney(ArtisanOrders::total(artisanShown))+")","pay",true);pay->setEnabled(!artisanShown.empty());
  MyGUI::Widget* info=artisanPanel(artisanBody,left+16,0,right,std::min(bh,frameH+80));artisanInformation(info,right,std::min(bh,frameH+80),true);
 }else if(artisanPage==4){
  list=artisanList(artisanBody,0,0,bw,bh);contentW=bw-24;MyGUI::TextBox* info=artisanText(list,8,8,contentW-16,40,18,Loc::text("import.artisan.help"),registerIvory);cy=info->getHeight()+24;
  for(size_t k=0;k<importedArtisans.size();++k){const std::string& key=importedArtisans[k];std::string name;unsigned int count=0;long long paid=0;for(size_t j=0;j<artisanLedger.orders.size();++j){const ArtisanOrders::Order& o=artisanLedger.orders[j];if(o.artisan==key){name=o.name;++count;paid+=o.paid;}}
   MyGUI::TextBox* label=artisanText(list,8,cy,contentW-16,40,18,name+" | "+artisanNumber(count)+" | "+artisanMoney(paid),registerIvory);cy+=label->getHeight()+8;
   bool selected=artisanRecoveryChoice==key;if(selected){MyGUI::TextBox* notice=artisanText(list,8,cy,contentW-16,40,18,std::string(Loc::text("import.artisan.confirm"))+" "+artisanName,registerAmber);cy+=notice->getHeight()+8;}
   MyGUI::Button* recover=artisanAction(list,8,cy,contentW-16,52,Loc::text(selected?"import.confirm":"import.artisan.transfer"),selected?"recover_apply":"recover_select");recover->setUserString("source",key);cy+=72;
  }
 }else{
  int active=0,complete=0,history=0,visible=0;for(size_t i=0;i<artisanLedger.orders.size();++i){const ArtisanOrders::Order& o=artisanLedger.orders[i];if(o.artisan!=artisanKey)continue;++history;if(artisanOrderVisible(o))++visible;if(o.status==ArtisanOrders::Ready||o.status==ArtisanOrders::Collected)++complete;else if(o.status==ArtisanOrders::Making||o.status==ArtisanOrders::Waiting)++active;}
  const char* keys[]={"artisan.in_progress","artisan.completed","artisan.all_orders"};int counts[]={active,complete,history};int tabW=(bw-180)/2;tabW=std::min(450,tabW);
  for(int i=0;i<3;++i){int x=i==2?bw-160:i*(tabW+12),tw=i==2?160:tabW;MyGUI::Button* tab=artisanAction(artisanBody,x,0,tw,54,std::string(Loc::text(keys[i]))+" ("+artisanNumber(counts[i])+")","order_filter",artisanOrderFilter==i,i);}
  int listH=std::min((bh-76)*(bh>=600?58:43)/100,std::max(182,60+visible*100)),lowerY=66+listH+12,lowerH=bh-lowerY,left=(bw-16)*56/100;
  artisanPanel(artisanBody,0,66,bw,listH);list=artisanList(artisanBody,5,71,bw-10,listH-10);contentW=std::max(1120,bw-32);list->setVisibleHScroll(true);cy=artisanOrderRows(list,contentW);
  MyGUI::Widget* detail=artisanPanel(artisanBody,0,lowerY,left,lowerH);artisanDetails(detail,left,lowerH);MyGUI::Widget* reminder=artisanPanel(artisanBody,left+16,lowerY,bw-left-16,lowerH);artisanInformation(reminder,bw-left-16,lowerH,false);
 }
 if(list){list->setCanvasSize(contentW,std::max(1,cy));list->setViewOffset(artisanResetScroll?MyGUI::IntPoint():offset);artisanResetScroll=false;artisanScroll=list;}
 artisanDrawnPage=artisanPage;
 if(artisanPage==3&&!importedArtisans.empty())artisanAction(artisanBody,8,bh+14,std::min(400,bw-280),54,Loc::text("import.artisan.open"),"recover_orders");
 else if(artisanNotice.empty())artisanBrand(artisanBody,4,bh+10,bw-430);else{MyGUI::ScrollView* note=artisanList(artisanBody,4,bh+10,bw-420,64);MyGUI::TextBox* t=artisanText(note,4,2,bw-456,40,16,artisanNotice,MyGUI::Colour(1,.4f,.3f));note->setCanvasSize(bw-444,t->getHeight()+8);}
 if(artisanPage==3)artisanAction(artisanBody,bw-260,bh+14,260,54,Loc::text("artisan.close"),"close");
 else if(artisanPage!=2){artisanAction(artisanBody,bw-390,bh+14,190,48,Loc::text(artisanPage==0?"artisan.orders":"artisan.catalog"),artisanPage==0?"orders":"catalog");if(artisanPage==1)artisanAction(artisanBody,bw-190,bh+14,190,48,Loc::text("artisan.orders"),"orders");}
 artisanFontTree(artisanWindow);artisanRefresh=false;
}
