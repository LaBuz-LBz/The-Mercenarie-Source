#pragma once
MyGUI::Widget* estatePanel=0;
MyGUI::TextBox *estateSummary=0,*estateEmpty=0,*estatePageLabel=0;
MyGUI::Button *estateOpenButton=0,*estateBackButton=0,*estatePrev=0,*estateNext=0;
struct EstateRow {MyGUI::Widget* panel;MyGUI::TextBox *title,*terms,*balance;MyGUI::Button *pay,*buy,*end;RealEstate::Money id,seq;EstateRow():panel(0),title(0),terms(0),balance(0),pay(0),buy(0),end(0),id(0),seq(0){} };
std::vector<EstateRow> estateRows;
int estatePage=0;
void estateResetView(){estatePanel=0;estateSummary=estateEmpty=estatePageLabel=0;estateOpenButton=estateBackButton=estatePrev=estateNext=0;estateRows.clear();estatePage=0;}
void estateViewClick(MyGUI::Widget* sender){
    if(!estateEnabled())return;
    if(sender==estateBackButton){estatePanel->setVisible(false);fnDashboard->setVisible(true);return;}
    if(sender==estatePrev){estatePage=std::max(0,estatePage-1);estateRefreshView();return;}if(sender==estateNext){++estatePage;estateRefreshView();return;}
    for(size_t i=0;i<estateRows.size();++i){EstateRow r=estateRows[i];if(!r.id)continue;if(sender==r.pay){estatePay(r.id,r.seq);return;}if(sender==r.end){estateTerminate(r.id);return;}if(sender==r.buy){std::map<RealEstate::Money,RealEstate::Lease>::iterator l=estateState.leases.find(r.id);if(l!=estateState.leases.end()){Building* b=estateResolve(l->second);if(b)estateBuy(b);}return;}}
}
MyGUI::Button* estateButton(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& text){MyGUI::Button* b=overviewButton(p,x,y,w,h,text);b->eventMouseButtonClick+=MyGUI::newDelegate(estateViewClick);return b;}
bool estateViewIsOpen(){return estatePanel&&estatePanel->getVisible();}
void estateShowView(MyGUI::Widget*){if(!estateEnabled()||!estatePanel)return;fnDashboard->setVisible(false);fnHistory->setVisible(false);fnTax->setVisible(false);estatePanel->setVisible(true);estateRefreshView();refreshRegisterNavigation();}
void estateRefreshView(){
    if(!estatePanel||!estatePanel->getVisible())return;
    if(!estateEnabled()){estatePanel->setVisible(false);if(fnDashboard)fnDashboard->setVisible(true);return;}
    std::vector<RealEstate::Money> ids;RealEstate::Money weekly=0;int count=0;
    for(std::map<RealEstate::Money,RealEstate::Lease>::const_iterator i=estateState.leases.begin();i!=estateState.leases.end();++i){const RealEstate::Lease& l=i->second;if(l.status==RealEstate::Active||l.debt())ids.push_back(i->first);if(l.status==RealEstate::Active){++count;if(!l.suspended)weekly=RealEstate::add(weekly,l.rent);}}
    Loc::Catalogue args;args["count"]=estateNumber(count);args["weekly"]=estateNumber(weekly);args["forecast"]=estateNumber(estateState.forecast(currentGameHours));args["debt"]=estateNumber(estateState.debt());args["funds"]=estateNumber(std::max(0,financeBalance()));
    MercenarieFonts::caption(estateSummary,estateState.available?Loc::format("estate.summary",args):estateText("unavailable"));
    int pages=std::max(1,((int)ids.size()+(int)estateRows.size()-1)/(int)estateRows.size());estatePage=std::max(0,std::min(estatePage,pages-1));MercenarieFonts::caption(estatePageLabel,estateNumber(estatePage+1)+" / "+estateNumber(pages));estatePrev->setEnabled(estatePage>0);estateNext->setEnabled(estatePage+1<pages);estateEmpty->setVisible(ids.empty());
    for(size_t n=0;n<estateRows.size();++n){EstateRow& row=estateRows[n];size_t idx=estatePage*estateRows.size()+n;row.panel->setVisible(idx<ids.size());row.id=0;if(idx>=ids.size())continue;const RealEstate::Lease& l=estateState.leases.find(ids[idx])->second;row.id=l.id;row.seq=l.oldest();
        MercenarieFonts::caption(row.title,l.identity.name+" — "+l.identity.city+" | "+estateText("owner")+" : "+l.identity.ownerName);
        MercenarieFonts::caption(row.terms,estateText(l.suspended?"suspended":l.status!=RealEstate::Active?"closed":l.debt()?"unpaid":"rented")+" | "+estateWeekly(l.rent)+" | "+estateText("due")+" : "+(l.status==RealEstate::Active?estateDue(l):"—"));
        MercenarieFonts::caption(row.balance,estateText("debt")+" : "+estateNumber(l.debt())+Loc::text("ui.cats")+" | "+estateText("buy")+" : "+estateNumber(l.buy)+Loc::text("ui.cats"));
        bool ready=estateState.available&&!l.suspended;row.pay->setEnabled(ready&&l.debt()>0);row.buy->setEnabled(ready&&l.status==RealEstate::Active);row.end->setEnabled(ready&&l.status==RealEstate::Active);
    }
}
void estateBuildView(MyGUI::Widget* parent){
    int w=parent->getWidth(),h=parent->getHeight();estatePanel=fnPanel(parent,0,0,w,h,estateText("title"));estateBackButton=estateButton(estatePanel,w-160,4,150,25,fnText("back"));
    estateSummary=fnLabel(estatePanel,12,38,w-24,66,"");estateSummary->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    estateEmpty=fnLabel(estatePanel,12,108,w-24,45,estateText("empty"));int capacity=std::max(1,(h-155)/145);estateRows.resize(capacity);
    for(int i=0;i<capacity;++i){EstateRow& row=estateRows[i];row.panel=overviewPanel(estatePanel,GuildResponsive::Rect(8,108+i*145,w-16,137));int rw=w-16;row.title=fnLabel(row.panel,8,5,rw-16,27,"");row.terms=fnLabel(row.panel,8,33,rw-16,34,"");row.balance=fnLabel(row.panel,8,68,rw-16,26,"");int bw=(rw-32)/3;row.pay=estateButton(row.panel,8,101,bw,28,estateText("pay"));row.buy=estateButton(row.panel,16+bw,101,bw,28,estateText("buy"));row.end=estateButton(row.panel,24+2*bw,101,bw,28,estateText("terminate"));}
    estatePrev=estateButton(estatePanel,8,h-35,115,28,fnText("previous"));estateNext=estateButton(estatePanel,w-123,h-35,115,28,fnText("next"));estatePageLabel=fnLabel(estatePanel,135,h-35,w-270,28,"");estatePanel->setVisible(false);
}
