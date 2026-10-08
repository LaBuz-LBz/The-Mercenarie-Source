// Presentation adapter only: never writes payroll or journal data.
#include "src/FinancePresentation.h"
MyGUI::Colour payrollGrey(.62f,.66f,.66f),payrollEdge(.28f,.32f,.32f),payrollRed(1,.20f,.16f);
std::string payrollQuery[4],payrollSelected[4],payrollShell;
int payrollFilter[4]={0},payrollSort[4]={0};
MyGUI::ScrollView* payrollFooter=0;
int payrollHistoryPage=0;
bool payrollUiDirty=false,payrollResetScroll=false;
std::vector<int> payrollCategoryMap;
std::string pv(const char* k){return Loc::text((std::string("payroll.ui.")+k).c_str());}
std::string payrollFold(const std::string& s){MyGUI::UString u(s);for(size_t i=0;i<u.size();++i){unsigned int c=u[i];if(c>='A'&&c<='Z')c+=32;else if((c>=0xc0&&c<=0xd6)||(c>=0xd8&&c<=0xde)||(c>=0x410&&c<=0x42f))c+=32;else if(c==0x401)c=0x451;else if(c==0x141)c=0x142;else if(c==0x104||c==0x106||c==0x118||c==0x143||c==0x15a||c==0x179||c==0x17b)c++;u[i]=c;}return u.asUTF8();}
bool payrollMatches(const std::string& s){return payrollFold(s).find(payrollFold(payrollQuery[payrollPage]))!=std::string::npos;}
MyGUI::TextBox* pvText(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& text,const MyGUI::Colour& c,int font=16){
 MyGUI::TextBox* t=registerText(p,x,y,w,h,font,"",c);
 for(int f=font;f>=12;--f){t->setFontHeight(f);if(popupWrap(t,text,w)<=h)break;}if(t->getTextSize().height>h){MyGUI::UString shortText(text);while(shortText.size()>0){shortText.erase(shortText.size()-1,1);if(popupWrap(t,shortText.asUTF8()+"...",w)<=h)break;}}return t;
}
MyGUI::Widget* pvPanel(MyGUI::Widget* p,int x,int y,int w,int h){MyGUI::Widget* b=p->createWidget<MyGUI::Widget>("PanelEmpty",x,y,w,h,MyGUI::Align::Default);registerSolid(b,0,0,w,h,MyGUI::Colour(.025f,.038f,.043f));popupBorder(b,w,h,payrollEdge);return b;}
void pvIcon(MyGUI::Widget* p,int id,int x,int y,int size){MyGUI::ImageBox* i=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);i->setImageTexture("MercenariePayrollIcons.png");i->setImageCoord(MyGUI::IntCoord(id*64,0,64,64));i->setNeedMouseFocus(false);}
void pvCard(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& title,const std::string& value,int icon,const MyGUI::Colour& c){MyGUI::Widget* b=pvPanel(p,x,y,w,h);pvIcon(b,icon,10,(h-32)/2,32);pvText(b,50,8,w-58,32,title,payrollGrey,14);pvText(b,50,42,w-58,h-46,value,c,20);}
int pvInfo(MyGUI::Widget* p,int x,int y,int w,const std::string& body,int minimum=0){
 MyGUI::TextBox* measure=registerText(p,0,0,w-24,2000,16,"",registerIvory);int h=popupWrap(measure,body,w-24)+58;MyGUI::Gui::getInstance().destroyWidget(measure);h=std::max(h,minimum);
 MyGUI::Widget* b=pvPanel(p,x,y,w,h);pvIcon(b,10,10,10,22);pvText(b,40,10,w-50,28,pv("information"),registerAmber);pvText(b,12,48,w-24,h-52,body,registerIvory);return h;
}
void pvEmpty(MyGUI::Widget* p,int y,int w,const std::string& heading){MyGUI::Widget* b=pvPanel(p,0,y,w,150);pvIcon(b,10,w/2-16,18,32);MyGUI::TextBox* t=pvText(b,20,60,w-40,38,heading,registerAmber,20);t->setTextAlign(MyGUI::Align::Center);t=pvText(b,20,106,w-40,32,pv("empty_hint"),payrollGrey);t->setTextAlign(MyGUI::Align::Center);}
void pvPortrait(MyGUI::Widget* p,const GuildPayroll::Member& m,int x,int y,int size){Character* a=payrollCharacter(m.id);MyGUI::ImageBox* i=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);i->setNeedMouseFocus(false);if(a&&PortraitManager::getInstance())PortraitManager::getInstance()->setImageWidget(a->getHandle(),i,true);else{i->setImageTexture("TheMercenarieBookIcons.png");i->setImageCoord(MyGUI::IntCoord(9*64,0,64,64));i->setColour(registerIvory);}}
GuildPayroll::Money pvForecast(const GuildPayroll::Member& m){return m.former||m.robot||!guildPayroll.enabled?0:GuildPayroll::rounded(m.accrued()+std::max(0.0,guildPayroll.next-guildPayroll.now)*m.rate/24);}
int pvDetail(MyGUI::Widget* p,int y,int w,const GuildPayroll::Member& m){
 Loc::Catalogue a;a["rate"]=payrollMoney(m.former?0:m.rate);a["period"]=payrollMoney(pvForecast(m));a["wages"]=payrollMoney(guildPayroll.debt(GuildPayroll::Salary,m.id));a["limbs"]=payrollMoney(guildPayroll.debt(GuildPayroll::Limb,m.id));a["income"]=payrollMoney(m.income);a["paid"]=payrollMoney(m.wages);a["compensation"]=payrollMoney(m.compensation);a["cost"]=payrollMoney(m.wages+m.compensation);a["profit"]=payrollMoney(m.income-m.wages-m.compensation);a["contracts"]=payrollNumber(m.contracts);a["joined"]=payrollDate(m.joined);a["ended"]=m.former?payrollDate(m.ended):Loc::text("payroll.active");
 return pvInfo(p,0,y,w,m.name+"\n"+Loc::format("payroll.member_detail",a))+y+8;
}
void pvSearch(MyGUI::EditBox* e){payrollQuery[payrollPage]=e->getCaption().asUTF8();payrollHistoryPage=0;payrollUiDirty=payrollResetScroll=true;}
void pvFilter(MyGUI::ComboBox*,size_t n){payrollFilter[payrollPage]=(int)n;payrollHistoryPage=0;payrollUiDirty=payrollResetScroll=true;}
void pvSort(MyGUI::ComboBox*,size_t n){payrollSort[payrollPage]=(int)n;payrollHistoryPage=0;payrollUiDirty=payrollResetScroll=true;}
MyGUI::ComboBox* pvCombo(MyGUI::Widget* p,int x,int y,int w){MyGUI::ComboBox* b=p->createWidget<MyGUI::ComboBox>("MercenarieContractSort",x,y,w,34,MyGUI::Align::Default);b->setComboModeDrop(true);b->setFontHeight(14);b->setTextColour(registerIvory);return b;}
struct PvMemberOrder{bool operator()(const GuildPayroll::Member* a,const GuildPayroll::Member* b)const{int sort=payrollSort[payrollPage];if(payrollPage==2){if(sort<2&&a->ended!=b->ended)return sort==0?a->ended>b->ended:a->ended<b->ended;return payrollFold(a->name)!=payrollFold(b->name)?payrollFold(a->name)<payrollFold(b->name):a->id<b->id;}if(sort==1&&a->rate!=b->rate)return a->rate>b->rate;if(sort==2&&a->joined!=b->joined)return a->joined>b->joined;return payrollFold(a->name)!=payrollFold(b->name)?payrollFold(a->name)<payrollFold(b->name):a->id<b->id;}};
void pvToolbar(MyGUI::Widget* p,int w){
 int fw=(w-36)/4,sw=(w-36)/3;MyGUI::ComboBox* f=pvCombo(p,12,148,fw);f->addItem(pv("all"));
 if(payrollPage==1)for(int r=0;r<5;++r)f->addItem(Loc::text(payrollRankKey(r)));
 if(payrollPage==2)f->addItem(pv("unknown"));
 if(payrollPage==3){payrollCategoryMap.clear();payrollCategoryMap.push_back(-1);bool present[Finance::Count]={false};std::vector<Finance::Entry> entries=Finance::displayEntries(fiscalLedger.cash,currentGameHours,0);for(size_t i=0;i<entries.size();++i)present[entries[i].category]=true;for(int c=0;c<Finance::Count;++c)if(present[c]){payrollCategoryMap.push_back(c);f->addItem(fnText(fnCategories[c]));}}
 if(payrollPage==3&&payrollFilter[3]>=(int)payrollCategoryMap.size())payrollFilter[3]=0;
 f->setIndexSelected(payrollFilter[payrollPage]);f->eventComboChangePosition+=MyGUI::newDelegate(pvFilter);
 MyGUI::EditBox* e=p->createWidget<MyGUI::EditBox>("TheMercenarie_Input",20+fw,148,sw,34,MyGUI::Align::Default);e->setFontHeight(16);e->setTextColour(registerIvory);MercenarieFonts::caption(e,payrollQuery[payrollPage]);e->eventEditTextChange+=MyGUI::newDelegate(pvSearch);
 pvText(p,20+fw,124,sw,24,pv("search"),payrollGrey,13);
 MyGUI::ComboBox* sort=pvCombo(p,28+fw+sw,148,w-40-fw-sw);sort->addItem(pv(payrollPage>=2?"newest":"name"));sort->addItem(pv(payrollPage>=2?"oldest":"rate"));if(payrollPage!=3)sort->addItem(pv(payrollPage==2?"name":"newest"));sort->setIndexSelected(payrollSort[payrollPage]);sort->eventComboChangePosition+=MyGUI::newDelegate(pvSort);
}
void pvRates(MyGUI::Widget* s,int w,int& y){
 int cw=(w-24)/4;const char* titles[]={"treasury","next","estimate","debt"};std::string values[]={payrollMoney(financeBalance()),guildPayroll.enabled?payrollDate(guildPayroll.next):Loc::text("payroll.suspended"),payrollMoney(guildPayroll.estimate()),payrollMoney(guildPayroll.debt())};
 for(int i=0;i<4;++i)pvCard(s,i*(cw+8),y,cw,108,pv(titles[i]),values[i],5+i,i>=2?payrollRed:registerIvory);y+=120;
 bool wide=w>=950;int tableW=wide?w*72/100:w;MyGUI::Widget* table=pvPanel(s,0,y,tableW,350);pvText(table,10,8,tableW-20,28,pv("rates"),registerAmber);
 const char* headers[]={"rank","rate","count","daily","weekly"};int edges[]={0,29,53,64,81,100};for(int c=0;c<5;++c)pvText(table,tableW*edges[c]/100+8,44,tableW*(edges[c+1]-edges[c])/100-16,44,pv(headers[c]),payrollGrey,14);
 long long daily=0;int total=0;for(std::map<std::string,GuildPayroll::Member>::const_iterator i=guildPayroll.members.begin();i!=guildPayroll.members.end();++i)if(!i->second.former)++total;
 for(int r=0;r<5;++r){int count=0;for(std::map<std::string,GuildPayroll::Member>::const_iterator i=guildPayroll.members.begin();i!=guildPayroll.members.end();++i)if(!i->second.former&&!i->second.robot&&i->second.rank==r)++count;int rate=guildPayroll.rates[r];long long cost=(long long)rate*count;daily+=cost;int ry=92+r*50;
 registerSolid(table,8,ry-4,tableW-16,1,payrollEdge);pvIcon(table,r,10,ry+4,28);pvText(table,46,ry,tableW*29/100-54,46,Loc::text(payrollRankKey(r)),registerAmber,15);
 int x=tableW*29/100+6,rw=tableW*24/100-12;MyGUI::Button* minus=payrollUiButton(table,x,ry+3,28,32,"-","rate-"+payrollNumber(r),registerAmber);minus->setEnabled(rate>GuildPayroll::minimum(r));rerollButtonPaint(minus);pvText(table,x+30,ry+5,rw-60,32,payrollNumber(rate),registerIvory,18);MyGUI::Button* plus=payrollUiButton(table,x+rw-28,ry+3,28,32,"+","rate+"+payrollNumber(r),registerAmber);plus->setEnabled(rate<200);rerollButtonPaint(plus);
 pvText(table,tableW*53/100+8,ry+5,tableW*11/100-12,35,payrollNumber(count),registerIvory);pvText(table,tableW*64/100+8,ry+5,tableW*17/100-12,40,payrollMoney(cost),registerIvory);pvText(table,tableW*81/100+8,ry+5,tableW*19/100-16,40,payrollMoney(cost*7),registerIvory);
 }
 int infoH=0;if(wide)infoH=pvInfo(s,tableW+8,y,w-tableW-8,pv("rates_info"),350);y+=std::max(350,infoH)+10;
 if(!wide)y+=pvInfo(s,0,y,w,pv("rates_info"))+10;
 int bw=(w-16)/3;pvCard(s,0,y,bw,96,pv("headcount"),payrollNumber(total),9,registerIvory);pvCard(s,bw+8,y,bw,96,pv("daily"),payrollMoney(daily),5,registerIvory);pvCard(s,2*(bw+8),y,bw,96,pv("weekly"),payrollMoney(daily*7),6,registerIvory);y+=106;
}
void pvMembers(MyGUI::Widget* s,int w,int& y){
 std::vector<const GuildPayroll::Member*> rows;int total=0;long long compensation=0;bool former=payrollPage==2;
 for(std::map<std::string,GuildPayroll::Member>::const_iterator i=guildPayroll.members.begin();i!=guildPayroll.members.end();++i){const GuildPayroll::Member& m=i->second;if(m.former!=former)continue;++total;compensation+=m.compensation;if(!former&&payrollFilter[1]&&m.rank!=payrollFilter[1]-1)continue;if(payrollMatches(m.name))rows.push_back(&m);}
 std::sort(rows.begin(),rows.end(),PvMemberOrder());pvText(s,8,y,w-16,28,pv("headcount")+" : "+payrollNumber(rows.size())+" / "+payrollNumber(total),payrollGrey);y+=34;
 int columns[]={0,22,36,47,60,70,85,94,100};const char* heads[]={"name","rank","status","date","service","cause","compensation","details"};
 if(former){MyGUI::Widget* h=pvPanel(s,0,y,w,76);for(int c=0;c<8;++c)pvText(h,w*columns[c]/100+5,6,w*(columns[c+1]-columns[c])/100-10,66,pv(heads[c]),payrollGrey,13);y+=76;}
 for(size_t i=0;i<rows.size();++i){const GuildPayroll::Member& m=*rows[i];MyGUI::Widget* row=pvPanel(s,0,y,w,former?86:108);
 if(former){pvPortrait(row,m,6,14,40);pvText(row,52,8,w*22/100-58,68,m.name,registerIvory);std::string values[]={Loc::text(payrollRankKey(m.rank)),pv("unknown"),m.ended>=0?payrollDate(m.ended):pv("unknown"),m.ended>=m.joined?payrollNumber((long long)((m.ended-m.joined)/24))+" "+pv("days"):pv("unknown"),pv("unknown"),payrollMoney(m.compensation)};for(int c=1;c<7;++c)pvText(row,w*columns[c]/100+5,8,w*(columns[c+1]-columns[c])/100-10,70,values[c-1],c==1?registerAmber:registerIvory,14);
 }else{pvPortrait(row,m,8,12,78);int a=w*37/100,b=w*65/100;pvIcon(row,m.rank,94,55,26);pvText(row,94,10,a-102,40,m.name,registerIvory,19);pvText(row,124,54,a-132,48,Loc::text(payrollRankKey(m.rank)),registerAmber,15);pvText(row,a+10,10,b-a-18,42,pv("rate")+" : "+payrollMoney(m.rate),registerAmber);pvText(row,a+10,56,b-a-18,44,pv("next")+" : "+payrollMoney(pvForecast(m)),registerIvory,14);pvText(row,b+10,8,w-b-60,94,pv("contracts")+" : "+payrollNumber(m.contracts)+"\n"+pv("income")+" : "+payrollMoney(m.income)+"\n"+pv("cost")+" : "+payrollMoney(m.wages+m.compensation),registerIvory,14);}
 payrollUiButton(row,w-38,former?25:38,30,32,"=","detail"+m.id,registerAmber);y+=(former?86:108)+6;if(payrollSelected[payrollPage]==m.id)y=pvDetail(s,y,w,m);
 }
 if(rows.empty()){pvEmpty(s,y,w,pv(former?"empty_former":"empty_members"));y+=160;}
 if(former){int left=w/3;MyGUI::Widget* stats=pvPanel(payrollFooter,0,0,left,174);pvText(stats,10,8,left-20,28,pv("statistics"),registerAmber);pvText(stats,10,44,left-20,120,pv("headcount")+" : "+payrollNumber(total)+"\n"+pv("deceased")+" : "+pv("unknown")+"\n"+pv("departed")+" : "+pv("unknown")+"\n"+pv("compensation")+" : "+payrollMoney(compensation),registerIvory,15);int height=std::max(174,pvInfo(payrollFooter,left+8,0,w-left-8,pv("former_info"),174));payrollFooter->setCanvasSize(w,height+4);}

}
void pvHistory(MyGUI::Widget* s,int w,int& y){
 std::vector<Finance::Entry> all=Finance::displayEntries(fiscalLedger.cash,currentGameHours,0),rows;int filter=payrollFilter[3]<(int)payrollCategoryMap.size()?payrollCategoryMap[payrollFilter[3]]:-1;
 for(size_t i=0;i<all.size();++i)if((filter<0||all[i].category==filter)&&payrollMatches(std::string(Loc::text(all[i].description.c_str()))+" "+fnText(fnCategories[all[i].category])))rows.push_back(all[i]);std::sort(rows.begin(),rows.end(),Finance::NewestFirst());if(payrollSort[3])std::reverse(rows.begin(),rows.end());
 int pages=std::max(1,((int)rows.size()+39)/40);payrollHistoryPage=std::max(0,std::min(payrollHistoryPage,pages-1));
 pvText(s,8,y,w-160,28,pv("transactions")+" : "+payrollNumber(rows.size())+"  |  "+payrollNumber(payrollHistoryPage+1)+" / "+payrollNumber(pages),payrollGrey);MyGUI::Button* prev=payrollUiButton(s,w-82,y,34,28,"<","previous",registerAmber);prev->setEnabled(payrollHistoryPage>0);rerollButtonPaint(prev);MyGUI::Button* next=payrollUiButton(s,w-40,y,34,28,">","next",registerAmber);next->setEnabled(payrollHistoryPage+1<pages);rerollButtonPaint(next);y+=38;
 int edges[]={0,15,30,65,80,94,100};const char* titles[]={"date","type","description","amount","balance","details"};MyGUI::Widget* head=pvPanel(s,0,y,w,76);for(int c=0;c<6;++c)pvText(head,w*edges[c]/100+5,5,w*(edges[c+1]-edges[c])/100-10,66,pv(titles[c]),payrollGrey,13);y+=76;long long income=0,expense=0;
 for(size_t i=0;i<rows.size();++i){const Finance::Entry& e=rows[i];if(e.amount>0)income+=e.amount;else expense-=e.amount;if(i<(size_t)payrollHistoryPage*40||i>=(size_t)(payrollHistoryPage+1)*40)continue;MyGUI::Widget* row=pvPanel(s,0,y,w,68);std::string values[]={payrollDate(e.hour),fnText(fnCategories[e.category]),Loc::text(e.description.c_str()),(e.amount>0?"+":"")+payrollMoney(e.amount),e.balance<0?pv("unknown"):payrollMoney(e.balance)};for(int c=0;c<5;++c)pvText(row,w*edges[c]/100+5,7,w*(edges[c+1]-edges[c])/100-10,54,values[c],c==3?(e.amount>0?registerGreen:e.amount<0?payrollRed:registerIvory):registerIvory,14);
 std::string id=payrollNumber(e.id)+":"+payrollNumber((long long)e.hour)+":"+payrollNumber(e.category)+":"+payrollNumber(e.amount);payrollUiButton(row,w-38,17,30,32,"=","detail"+id,registerAmber);y+=68;if(payrollSelected[3]==id)y+=pvInfo(s,0,y,w,values[0]+"\n"+values[1]+"\n"+values[2]+"\n"+pv("amount")+" : "+values[3]+"\n"+pv("balance")+" : "+values[4])+8;
 }
 if(rows.empty()){pvEmpty(s,y,w,pv("empty_history"));y+=160;}int cw=(w-16)/4;MyGUI::Widget* summary=pvPanel(payrollFooter,0,0,cw,162);pvText(summary,12,8,cw-24,28,pv("statistics"),registerAmber);pvText(summary,12,48,cw-24,44,pv("income")+" : "+payrollMoney(income),registerGreen);pvText(summary,12,100,cw-24,44,pv("expenses")+" : "+payrollMoney(-expense),payrollRed);pvCard(payrollFooter,cw+8,0,cw,162,pv("period_balance"),payrollMoney(income-expense),8,income>=expense?registerGreen:payrollRed);int height=pvInfo(payrollFooter,2*(cw+8),0,w-2*(cw+8),pv("history_info"),162);payrollFooter->setCanvasSize(w,std::max(162,height)+4);

}
void buildPayrollManagement(){
 MyGUI::IntPoint previousOffset;if(v9WidgetLive(payrollManagementWindow)&&payrollManagementScroll&&payrollRenderedPage==payrollPage&&!payrollResetScroll)previousOffset=payrollManagementScroll->getViewOffset();
 MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();std::string shell=payrollNumber(v.width)+":"+payrollNumber(v.height)+":"+payrollNumber(payrollPage)+":"+Loc::engine().language;
 if(payrollPage==3){bool present[Finance::Count]={false};std::vector<Finance::Entry> entries=Finance::displayEntries(fiscalLedger.cash,currentGameHours,0);for(size_t i=0;i<entries.size();++i)present[entries[i].category]=true;for(int c=0;c<Finance::Count;++c)shell+=present[c]?"1":"0";}
 if(!v9WidgetLive(payrollManagementWindow)||shell!=payrollShell){payrollClose(payrollManagementWindow);payrollManagementScroll=0;payrollFooter=0;payrollManagementWindow=payrollWindow("payroll.page");payrollShell=shell;MyGUI::Widget* p=payrollManagementWindow->getClientWidget();int w=p->getWidth();
 const char* tabs[]={"payroll.rates","payroll.members","payroll.former","payroll.history"};int tw=(w-24)/4;for(int i=0;i<4;++i){MyGUI::Button* b=payrollUiButton(p,12+i*tw,78,tw-4,42,Loc::text(tabs[i]),"tab"+payrollNumber(i),i==payrollPage?registerAmber:payrollEdge);if(i==payrollPage){registerSolid(b,1,39,tw-6,2,registerAmber);b->setAlpha(1);}}
 if(payrollPage)pvToolbar(p,w);MyGUI::InputManager::getInstance().addWidgetModal(payrollManagementWindow);
 }else if(payrollManagementScroll){MyGUI::Gui::getInstance().destroyWidget(payrollManagementScroll);payrollManagementScroll=0;}
 if(payrollFooter){MyGUI::Gui::getInstance().destroyWidget(payrollFooter);payrollFooter=0;}
 MyGUI::ScrollView* s=payrollScroll(payrollManagementWindow,payrollPage?192:132,payrollPage>=2?198:12);int w=payrollPage==0?s->getWidth()-20:std::max(1100,s->getWidth()-20),y=4;s->setVisibleHScroll(w>s->getWidth()-20);if(payrollPage>=2){payrollFooter=payrollScroll(payrollManagementWindow,payrollManagementWindow->getClientWidget()->getHeight()-184,12);payrollFooter->setVisibleHScroll(w>payrollFooter->getWidth()-20);}
 if(importedDomainsSuspended&&payrollPage==0){y=payrollLine(s,y,w,Loc::text("import.payroll.help"));bool ready=!payrollRecoverySignature().empty();for(std::map<std::string,GuildPayroll::Member>::const_iterator i=guildPayroll.members.begin();i!=guildPayroll.members.end();++i)if(!i->second.former)y=payrollLine(s,y,w,i->second.name);if(!ready)y=payrollLine(s,y,w,Loc::text("import.payroll.ambiguous"));MyGUI::Button* resume=payrollUiButton(s,12,y,w-24,56,Loc::text(payrollRecoveryConfirm.empty()?"import.payroll.review":"import.confirm"),"recover_payroll",registerAmber);resume->setEnabled(ready);y+=72;}
 if(payrollPage==0)pvRates(s,w,y);else if(payrollPage==1||payrollPage==2)pvMembers(s,w,y);else pvHistory(s,w,y);s->setCanvasSize(w,y+12);s->setViewOffset(previousOffset);payrollManagementScroll=s;payrollRenderedPage=payrollPage;payrollUiDirty=payrollResetScroll=false;
}
