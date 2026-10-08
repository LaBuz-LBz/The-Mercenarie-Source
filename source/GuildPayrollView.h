// Separate payroll surfaces; the existing reroll popup is unchanged.
MyGUI::Window *payrollSheetWindow=0,*payrollManagementWindow=0;
bool payrollManagementIsOpen(){return v9WidgetLive(payrollManagementWindow);}
MyGUI::ScrollView* payrollManagementScroll=0;
int payrollRenderedPage=-1;
GuildPayroll::Money payrollDisplayedBill=0;
bool payrollWasPaused=false;int payrollPage=0;std::string payrollViewStamp;
MyGUI::IntSize payrollSheetViewport;
std::string payrollSheetLanguage;
void payrollUiClick(MyGUI::Widget*);
std::string payrollText(const char* key){return Loc::text(key);}
std::string payrollMoney(GuildPayroll::Money n){return payrollNumber(n)+Loc::text("ui.cats");}
std::string payrollDate(double h){Loc::Catalogue a;a["day"]=payrollNumber((long long)std::floor(h/24)+1);a["hour"]=payrollNumber((int)h%24);return Loc::format("payroll.date",a);}
int payrollLine(MyGUI::Widget* p,int y,int w,const std::string& text,bool title=false,int x=12){
    MyGUI::TextBox* label=registerText(p,x,y,w-x-12,2000,title?23:18,"",title?registerAmber:registerIvory);
    int h=popupWrap(label,text,label->getWidth());label->setCoord(x,y,label->getWidth(),h);return y+h+10;
}
MyGUI::Button* payrollUiButton(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& text,const std::string& action,const MyGUI::Colour& colour){
    MyGUI::Button* b=p->createWidget<MyGUI::Button>("PanelEmpty",x,y,w,h,MyGUI::Align::Default);
    registerSolid(b,0,0,w,h,MyGUI::Colour(.055f,.065f,.07f));popupBorder(b,w,h,colour);
    MyGUI::TextBox* label=registerText(b,6,4,w-12,h-8,18,"",registerIvory);
    for(int font=18;font>=12;--font){label->setFontHeight(font);if(popupWrap(label,text,w-12)<=h-8)break;}
    label->setTextAlign(MyGUI::Align::Center);
    b->setUserString("payrollAction",action);b->eventMouseButtonClick+=MyGUI::newDelegate(payrollUiClick);
    b->eventMouseSetFocus+=MyGUI::newDelegate(rerollButtonHover);b->eventMouseLostFocus+=MyGUI::newDelegate(rerollButtonLeave);
    b->eventMouseButtonPressed+=MyGUI::newDelegate(rerollButtonPress);b->eventMouseButtonReleased+=MyGUI::newDelegate(rerollButtonRelease);rerollButtonPaint(b);return b;
}
MyGUI::Window* payrollWindow(const char* title){
    if(!MyGUI::ResourceManager::getInstance().isExist("MercenariePopupWindow"))MyGUI::ResourceManager::getInstance().load("MercenariePopup.xml");
    MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();int w=std::min(1280,v.width-40),h=std::min(820,v.height-40);
    MyGUI::Window* window=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("MercenariePopupWindow",(v.width-w)/2,(v.height-h)/2,w,h,MyGUI::Align::Default,"Popup");
    MyGUI::Widget* p=window->getClientWidget();registerSolid(p,0,0,w,h,MyGUI::Colour(.04f,.052f,.059f));popupBorder(p,w,h,registerAmber);
    if(std::string(title)=="payroll.page"){registerText(p,18,8,w-80,25,17,Loc::text("payroll.ui.header"),registerAmber);registerText(p,18,40,w-80,30,22,Loc::text(title),registerAmber);}else registerText(p,18,12,w-80,38,24,Loc::text(title),registerAmber);registerSolid(p,1,std::string(title)=="payroll.page"?35:53,w-2,1,MyGUI::Colour(.35f,.34f,.28f));
    MyGUI::Button* close=payrollUiButton(p,w-44,12,28,28,"",std::string(title)=="payroll.sheet"?"refuse":"close",registerAmber);popupIcon(close,2,5,5,18,registerIvory);
    return window;
}
MyGUI::ScrollView* payrollScroll(MyGUI::Window* window,int top,int bottom){MyGUI::Widget* p=window->getClientWidget();MyGUI::ScrollView* s=p->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",10,top,p->getWidth()-20,p->getHeight()-top-bottom,MyGUI::Align::Default);MercenarieNativeInput::bind(s);s->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);s->setVisibleHScroll(false);return s;}
void payrollClose(MyGUI::Window*& w){if(v9WidgetLive(w)){MyGUI::InputManager::getInstance().removeWidgetModal(w);mercenarieDestroyLiveWidget(w);}w=0;refreshRegisterNavigation();}
void closePayrollWindowsForWorld(){payrollClose(payrollSheetWindow);payrollClose(payrollManagementWindow);payrollDisplayedBill=0;payrollViewStamp.clear();}
#include "PayrollSheetView.h"
#include "PayrollManagementView.h"
void openPayrollFinances(MyGUI::Widget*){payrollSync();payrollPage=0;buildPayrollManagement();payrollViewStamp.clear();refreshRegisterNavigation();}
void payrollUiClick(MyGUI::Widget* sender){
    std::string action=sender->getUserString("payrollAction");
    if(action=="recover_payroll"&&importedDomainsSuspended){std::string signature=payrollRecoverySignature();if(signature.empty()){payrollRecoveryConfirm.clear();return;}if(payrollRecoveryConfirm!=signature){payrollRecoveryConfirm=signature;buildPayrollManagement();return;}if(!financeReady()||progressWriteBlocked)return;GuildPayroll::State recovered=ImportRecovery::resumePayroll(guildPayroll,payrollRecoveryPeople(),currentGameHours);guildPayroll=recovered;importedDomainsSuspended=false;payrollRecoveryConfirm.clear();buildPayrollManagement();return;}
    if(importedDomainsSuspended&&(action=="pay"||action=="refuse"||action.compare(0,4,"rate")==0)){if(ou)ou->showPlayerAMessage(Loc::text("save.import.suspended"),true);return;}
    if(action=="sheet_details"){payrollSheetDetails=!payrollSheetDetails;buildPayrollSheet();return;}
    if(action=="pay"||action=="refuse"){
        if(!payrollDisplayedBill||!guildPayroll.answer(payrollDisplayedBill))return;
        if(action=="pay"){GuildPayroll::Money paid=payrollDebit(guildPayroll.debt(),"payroll.salary_payment");guildPayroll.repay(paid,currentGameHours,"payroll.salary_payment");}
        payrollDisplayedBill=0;payrollClose(payrollSheetWindow);if(ou&&!payrollWasPaused)ou->userPause(false);return;
    }
    if(action=="previous"||action=="next"){payrollHistoryPage+=action=="next"?1:-1;payrollResetScroll=true;}
    if(action.compare(0,6,"detail")==0){std::string id=action.substr(6);payrollSelected[payrollPage]=payrollSelected[payrollPage]==id?"":id;}
    if(action=="close"){payrollClose(payrollManagementWindow);return;}
    if(action.compare(0,3,"tab")==0)payrollPage=std::max(0,std::min(3,atoi(action.substr(3).c_str())));
    if(action.compare(0,4,"rate")==0){int rank=atoi(action.substr(5).c_str());if(rank>=0&&rank<5){payrollSync();guildPayroll.rate(rank,guildPayroll.rates[rank]+(action[4]=='+'?5:-5),currentGameHours);}}
    buildPayrollManagement();payrollViewStamp.clear();
}
void updatePayrollWindows(){
    if(!MyGUI::Gui::getInstancePtr()||!ou||progressWriteBlocked)return;
    MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
    if(v9WidgetLive(payrollSheetWindow)&&(view.width!=payrollSheetViewport.width||view.height!=payrollSheetViewport.height||payrollSheetLanguage!=Loc::engine().language))buildPayrollSheet();
    if(!importedDomainsSuspended&&!v9WidgetLive(payrollSheetWindow)&&guildPayroll.pending()&&!MyGUI::InputManager::getInstance().isModalAny()){
        payrollWasPaused=ou->isPaused();ou->userPause(true);buildPayrollSheet();
    }
    if(v9WidgetLive(payrollManagementWindow)){
        std::ostringstream stamp;stamp<<view.width<<":"<<view.height<<":"<<(long long)(currentGameHours*4)<<":"<<financeBalance()<<":"<<guildPayroll.debt()<<":"<<Loc::engine().language;
        stamp<<":"<<importedDomainsSuspended<<":"<<fiscalLedger.cash.revision;for(std::map<std::string,GuildPayroll::Member>::const_iterator it=guildPayroll.members.begin();it!=guildPayroll.members.end();++it){const GuildPayroll::Member& m=it->second;stamp<<":"<<m.id<<":"<<m.name<<":"<<m.rank<<":"<<m.rate<<":"<<m.former<<":"<<m.income<<":"<<m.wages<<":"<<m.compensation<<":"<<m.contracts;}
        if(payrollUiDirty||stamp.str()!=payrollViewStamp){payrollViewStamp=stamp.str();buildPayrollManagement();}
    }
}
