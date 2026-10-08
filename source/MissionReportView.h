#include "Localization.h"
#pragma once
// Presentation coordinates follow the supplied 1488 x 1060 reference.
float reportScaleX=1,reportScaleY=1;
bool reportReadOnly=false;
MyGUI::TextBox* reportTotalLabel=0;
MyGUI::TextBox* reportFields[7]={0};
MyGUI::TextBox* reportGains[7]={0};
MyGUI::TextBox* reportCount=0;
MyGUI::TextBox* reportBefore=0;
MyGUI::TextBox* reportAfter=0;
MyGUI::TextBox* reportHint=0;
MyGUI::TextBox* reportType=0;
MyGUI::ImageBox* reportMissionIcon=0;
MyGUI::ImageBox* reportSkulls[5]={0};
MyGUI::TextBox* reportChecks[MissionBonuses::Count]={0};
MyGUI::TextBox* reportAmounts[MissionBonuses::Count]={0};
MyGUI::TextBox* reportNames[MissionBonuses::Count]={0};
MyGUI::TextBox* reportDescriptions[MissionBonuses::Count]={0};
MyGUI::ImageBox* reportIcons[MissionBonuses::Count]={0};
MyGUI::Widget* reportEdges[MissionBonuses::Count][4]={{0}};
int rx(int n){return (int)(n*reportScaleX+.5f);} int ry(int n){return (int)(n*reportScaleY+.5f);}
int rf(int n){return std::max(10,(int)(n*std::min(reportScaleX,reportScaleY)+.5f));}
std::string reportSigned(double n){std::ostringstream s;s<<std::showpos<<n;return s.str();}
MyGUI::TextBox* reportText(MyGUI::Widget* p,int x,int y,int w,int h,int font,const std::string& text,const MyGUI::Colour& color){return registerText(p,rx(x),ry(y),rx(w),ry(h),rf(font),text,color);}
MyGUI::Widget* reportPanel(MyGUI::Widget* p,int x,int y,int w,int h){return boardPanelV6(p,rx(x),ry(y),rx(w),ry(h));}
MyGUI::ImageBox* reportIcon(MyGUI::Widget* p,int slot,int x,int y,int size){
    MyGUI::ImageBox* i=p->createWidget<MyGUI::ImageBox>("ImageBox",rx(x)+(rx(size)-rf(size))/2,ry(y)+(ry(size)-rf(size))/2,rf(size),rf(size),MyGUI::Align::Default);
    i->setImageTexture("MissionReportIcons.png");i->setImageCoord(MyGUI::IntCoord(slot*96,0,96,96));i->setColour(MyGUI::Colour(.88f,.68f,.39f));i->setNeedMouseFocus(false);return i;
}
const char* reportDescription(int i){
    const char* fr[]={Loc::text("ui.mission_completed_faster_than_expected"),Loc::text("ui.the_client_was_never_knocked_out"),Loc::text("ui.the_client_arrived_in_good_health"),Loc::text("ui.you_survived_an_ambush"),Loc::text("ui.you_followed_a_more_dangerous_route"),Loc::text("ui.the_client_stayed_close_to_your_troops"),Loc::text("ui.no_cargo_was_lost"),Loc::text("ui.a_place_or_item_of_interest_was_discovered")};
    const char* en[]={Loc::text("ui.mission_completed_faster_than_expected"),Loc::text("ui.the_client_was_never_knocked_out"),Loc::text("ui.the_client_arrived_in_good_health"),Loc::text("ui.you_survived_an_ambush"),Loc::text("ui.you_followed_a_more_dangerous_route"),Loc::text("ui.the_client_stayed_close_to_your_troops"),Loc::text("ui.no_cargo_was_lost"),Loc::text("ui.a_place_or_item_of_interest_was_discovered")};
    return (gMercenarieEnglish?en:fr)[i];
}
void finalReportClose(MyGUI::WidgetPtr){finalWindowButtonPressed(finalWindow,"close");}

MyGUI::Widget* reportBonusArea88=0;
MyGUI::Button* reportUnavailable88=0;
MyGUI::TextBox *reportPay88=0,*reportPenalty88=0,*reportTip88=0,*reportBottom88=0,*reportStatus88=0;
bool reportExpanded88=false;
std::string r88(const char* fr,const char* en){return Loc::engine().language=="fr"?fr:en;}
void r88Font(MyGUI::TextBox* t,int size,bool sans=false){float previous=board77Scale;board77Scale=reportScaleX;board77Font(t,size,sans);board77Scale=previous;}
void r88Fit(MyGUI::TextBox* t,int size,bool sans=false){r88Font(t,size,sans);float previous=board77Scale;board77Scale=reportScaleX;board77Fit(t);board77Scale=previous;}
void r88Coord(MyGUI::Widget* w,int x,int y,int width,int height){w->setCoord(rx(x),ry(y),rx(width),ry(height));}
MyGUI::TextBox* r88Text(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text,bool gold=false,bool sans=false){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Board77Text",rx(x),ry(y),rx(w),ry(h),MyGUI::Align::Default);t->setNeedMouseFocus(false);t->setCaption(text);t->setTextColour(gold?board77Gold:board77Ink);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);r88Fit(t,size,sans);return t;
}
MyGUI::Widget* r88Panel(MyGUI::Widget* p,int x,int y,int w,int h){MyGUI::Widget* v=p->createWidget<MyGUI::Widget>("Board77Panel",rx(x),ry(y),rx(w),ry(h),MyGUI::Align::Default);registerSolid(v,0,0,rx(w),1,board77Edge);registerSolid(v,0,ry(h)-1,rx(w),1,board77Edge);registerSolid(v,0,0,1,ry(h),board77Edge);registerSolid(v,rx(w)-1,0,1,ry(h),board77Edge);return v;}
MyGUI::Button* r88Button(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& text,bool gold=false){MyGUI::Button* b=p->createWidget<MyGUI::Button>(gold?"Board77Gold":"Board77Card",rx(x),ry(y),rx(w),ry(h),MyGUI::Align::Default);b->setCaption(text);b->setTextColour(gold?MyGUI::Colour(.09f,.08f,.04f):board77Ink);b->setTextAlign(MyGUI::Align::Center);r88Fit(b,23);return b;}
void reportBonusSelection92(int i,bool selected){
 MyGUI::TextBox* box=reportChecks[i];
 box->changeWidgetSkin(selected?"Board77Gold":"Board77Card");
 box->setNeedMouseFocus(false);box->setTextAlign(MyGUI::Align::Center);
 box->setTextColour(selected?MyGUI::Colour(.09f,.08f,.04f):board77Gold);
 box->setCaption(selected?"X":"");r88Font(box,23,true);
}
void layoutReportCards88(){
 if(!reportBonusArea88)return;
 std::vector<int> order;
 for(int i=0;i<MissionBonuses::Count;++i)if(!reportReadOnly&&finalBonusChoice.amounts[i]>0)order.push_back(i);
 int earned=(int)order.size();
 if(reportExpanded88&&!reportReadOnly)for(int i=0;i<MissionBonuses::Count;++i)if(finalBonusChoice.amounts[i]<=0)order.push_back(i);
 for(int i=0;i<MissionBonuses::Count;++i)finalBonusButtons[i]->setVisible(false);
 int rows=std::max(2,((int)order.size()+1)/2),height=rows>2?224/rows:92;
 for(size_t k=0;k<order.size();++k){int i=order[k];MyGUI::Button* b=finalBonusButtons[i];b->setVisible(true);r88Coord(b,20+(k%2)*734,84+(k/2)*(height+6),716,height);
  r88Coord(reportChecks[i],16,(height-30)/2,32,30);r88Coord(reportIcons[i],62,(height-44)/2,44,44);
  r88Coord(reportNames[i],126,rows>2?2:10,380,32);r88Fit(reportNames[i],rows>2?20:24);
  reportDescriptions[i]->setVisible(rows<=2);r88Coord(reportDescriptions[i],126,47,385,28);r88Fit(reportDescriptions[i],16);
  r88Coord(reportAmounts[i],511,(height-32)/2,184,32);r88Fit(reportAmounts[i],22);
  r88Coord(reportEdges[i][0],0,0,716,1);r88Coord(reportEdges[i][1],0,height-1,716,1);r88Coord(reportEdges[i][2],0,0,1,height);r88Coord(reportEdges[i][3],715,0,1,height);
 }
 reportUnavailable88->setVisible(!reportReadOnly&&earned<MissionBonuses::Count);
 reportUnavailable88->setCaption(r88("Primes non acquises (","Unearned bonuses (")+registerNumber(MissionBonuses::Count-earned)+(reportExpanded88?")  -":")  +"));
 r88Fit(reportUnavailable88,19);
}
void toggleReportUnavailable88(MyGUI::Widget*){reportExpanded88=!reportExpanded88;layoutReportCards88();}
void refreshReport88(int payable,int extra,int penalty){
 layoutReportCards88();
 reportPay88->setCaption(reportGains[2]->getCaption());
 reportBottom88->setCaption(reportGains[2]->getCaption());
 reportPenalty88->setCaption("-"+registerNumber(penalty)+" Cats");
 reportTip88->setCaption("+"+registerNumber(finalClientTip)+" Cats");
 reportStatus88->setCaption(reportReadOnly?r88("VERSEMENT EFFECTU\303\211","PAYMENT RECEIVED"):extra?r88("SI TOUTES LES PRIMES SONT ACCORD\303\211ES","IF ALL BONUSES ARE GRANTED"):r88("VERSEMENT SANS PRIME","PAYMENT WITHOUT BONUSES"));
 r88Fit(reportPay88,54);r88Fit(reportBottom88,36);r88Fit(reportStatus88,15,true);
 r88Fit(reportType,20,true);reportType->setTextColour(board77Gold);
 r88Fit(reportFields[0],24);for(int i=1;i<7;++i)r88Fit(reportFields[i],i>=3?22:18);
 for(int i=0;i<7;++i)r88Fit(reportGains[i],i==1?30:22);
 r88Fit(reportCount,17);r88Fit(reportHint,16);r88Fit(finalCashButton,23);r88Fit(finalReputationButton,23);r88Fit(finalHalfCashButton,18);
 for(int i=0;i<5;++i)reportSkulls[i]->setVisible(false);
}
void buildFinalReport(){
 if(finalWindow||!MyGUI::Gui::getInstancePtr())return;
 const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
 reportScaleX=reportScaleY=std::min((view.width-32)/1536.0f,(view.height-48)/1024.0f)*.90f;
 int w=rx(1536),h=ry(1024);
 Ogre::ResourceGroupManager& resources=Ogre::ResourceGroupManager::getSingleton();
 if(!resources.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))resources.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");
 MyGUI::ResourceManager::getInstance().load("ContractBoardV6.xml");MyGUI::ResourceManager::getInstance().load("MercenarieBoard77.xml");
 finalWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("ContractBoardWindowV6",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","EscortFinalNegotiation");finalWindow->eventWindowButtonPressed+=MyGUI::newDelegate(finalWindowButtonPressed);
 MyGUI::Widget* c=finalWindow->getClientWidget();MyGUI::Widget* frame=c->createWidget<MyGUI::Widget>("Board77Frame",0,0,w,h,MyGUI::Align::Default);frame->setNeedMouseFocus(false);
 MyGUI::Widget* hidden=c->createWidget<MyGUI::Widget>("PanelEmpty",0,0,1,1,MyGUI::Align::Default);hidden->setVisible(false);
 reportBefore=r88Text(hidden,0,0,1,1,14,"");reportAfter=r88Text(hidden,0,0,1,1,14,"");finalBonusPreview=r88Text(hidden,0,0,1,1,14,"");finalSummaryText=reportBefore;
 reportMissionIcon=reportIcon(hidden,0,0,0,1);for(int i=0;i<5;++i)reportSkulls[i]=reportIcon(hidden,1,0,0,1);
 reportGains[2]=r88Text(hidden,0,0,300,40,20,"");reportTotalLabel=r88Text(hidden,0,0,300,40,20,"");
 MyGUI::ImageBox* logo=c->createWidget<MyGUI::ImageBox>("ImageBox",rx(36),ry(20),rx(54),ry(64),MyGUI::Align::Default);logo->setImageTexture("MercenarieNegotiationPlate74.png");logo->setImageCoord(MyGUI::IntCoord(48,52,76,94));logo->setNeedMouseFocus(false);
 r88Text(c,110,24,380,40,20,"T H E   M E R C E N A R I E",false,true);
 r88Text(c,460,12,615,56,42,r88("Bilan de mission","Mission report"))->setTextAlign(MyGUI::Align::Center);
 reportType=r88Text(c,460,66,615,30,20,"",true,true);reportType->setTextAlign(MyGUI::Align::Center);
 MyGUI::Button* close=r88Button(c,1460,24,48,48,"X");close->eventMouseButtonClick+=MyGUI::newDelegate(finalReportClose);
 MyGUI::Widget* hero=r88Panel(c,24,104,974,398);
 MyGUI::ImageBox* scene=hero->createWidget<MyGUI::ImageBox>("ImageBox",rx(3),ry(3),rx(968),ry(307),MyGUI::Align::Default);scene->setImageTexture("MissionReportPanorama88.png");scene->setImageCoord(MyGUI::IntCoord(0,0,2172,724));scene->setNeedMouseFocus(false);
 MyGUI::Widget* shade=hero->createWidget<MyGUI::Widget>("WhiteSkin",rx(3),ry(211),rx(968),ry(99),MyGUI::Align::Default);shade->setColour(MyGUI::Colour(0,0,0));shade->setAlpha(.12f);shade->setNeedMouseFocus(false);
 r88Text(hero,24,219,910,44,35,r88("Contrat accompli","Contract completed"));reportFields[0]=r88Text(hero,24,264,918,36,24,"");
 const char* factKeys[]={"ui.distance_travelled","ui.mission_duration","ui.estimated_danger","ui.clients_involved"};
 for(int i=0;i<4;++i){reportFields[i+3]=r88Text(hero,20+i*240,326,224,33,22,"");r88Text(hero,20+i*240,365,224,24,12,Loc::text(factKeys[i]),false,true);}
 MyGUI::Widget* gains=r88Panel(c,1010,104,502,398);
 r88Text(gains,22,12,458,30,20,r88("V O T R E   R \303\211 C O M P E N S E","Y O U R   R E W A R D"),true,true)->setTextAlign(MyGUI::Align::Center);
 reportPay88=r88Text(gains,20,48,462,70,54, "",true);reportPay88->setTextAlign(MyGUI::Align::Center);
 reportStatus88=r88Text(gains,20,118,462,30,15,"",false,true);reportStatus88->setTextAlign(MyGUI::Align::Center);
 const char* labels[]={"ui.payment_before_bonuses","ui.forced_pace_penalty","ui.advance_already_received"};
 for(int i=0;i<3;++i)r88Text(gains,20,157+i*29,292,28,16,Loc::text(labels[i]));
 reportGains[0]=r88Text(gains,313,157,169,28,18,"",true);reportPenalty88=r88Text(gains,313,186,169,28,18,"",true);reportFields[2]=r88Text(gains,313,215,169,28,18,"");reportFields[1]=r88Text(hidden,0,0,300,30,18,"");
 r88Text(gains,20,244,292,28,16,r88("Pourboire","Tip"));reportTip88=r88Text(gains,313,244,169,28,18,"",true);
 MyGUI::TextBox* amounts[]={reportGains[0],reportPenalty88,reportFields[2],reportTip88};for(int i=0;i<4;++i)amounts[i]->setTextAlign(MyGUI::Align::Right|MyGUI::Align::VCenter);
 const char* progression[]={"ui.guild_xp_4df4069","ui.local_reputation_b4893c4","ui.global_reputation_e746d94","common.prestige"};
 for(int i=0;i<4;++i){reportIcon(gains,i==0?9:i==3?10:5,20,279+i*28,25);reportGains[i+3]=r88Text(gains,60,278+i*28,95,28,21,"",true);r88Text(gains,160,278+i*28,320,28,17,Loc::text(progression[i]));}
 reportBonusArea88=r88Panel(c,24,514,1488,332);MyGUI::Widget* bonuses=reportBonusArea88;
 r88Text(bonuses,22,10,760,42,31,r88("Primes \303\240 demander","Bonuses to request"));reportHint=r88Text(bonuses,24,51,990,26,16,"");reportCount=r88Text(bonuses,976,17,258,38,17,"");
 finalHalfCashButton=r88Button(bonuses,1242,14,226,44,r88("Tout cocher","Select all"));finalHalfCashButton->eventMouseButtonClick+=MyGUI::newDelegate(finalHalfCashClicked);
 for(int i=0;i<MissionBonuses::Count;++i){MyGUI::Button* b=r88Button(bonuses,20,84,716,92,"");finalBonusButtons[i]=b;b->eventMouseButtonClick+=MyGUI::newDelegate(toggleFinalBonus);
  reportIcons[i]=reportIcon(b,i,62,24,44);reportChecks[i]=r88Text(b,16,30,32,30,23,"");reportBonusSelection92(i,false);
  reportNames[i]=r88Text(b,126,10,380,32,24,MissionBonuses::name(i,gMercenarieEnglish));reportDescriptions[i]=r88Text(b,126,47,385,28,16,reportDescription(i));reportAmounts[i]=r88Text(b,511,30,184,32,22,"",true);reportAmounts[i]->setTextAlign(MyGUI::Align::Right|MyGUI::Align::VCenter);
  reportEdges[i][0]=registerSolid(b,0,0,rx(716),1,board77Edge);reportEdges[i][1]=registerSolid(b,0,ry(91),rx(716),1,board77Edge);reportEdges[i][2]=registerSolid(b,0,0,1,ry(92),board77Edge);reportEdges[i][3]=registerSolid(b,rx(715),0,1,ry(92),board77Edge);
 }
 reportUnavailable88=r88Button(c,44,854,1448,38,"");reportUnavailable88->eventMouseButtonClick+=MyGUI::newDelegate(toggleReportUnavailable88);reportExpanded88=false;
 MyGUI::Widget* summary=r88Panel(c,24,904,1488,64);
 r88Text(summary,22,3,430,24,15,r88("Primes s\303\251lectionn\303\251es","Selected bonuses"));reportGains[1]=r88Text(summary,22,25,430,36,30,"",true);
 r88Text(summary,470,4,572,55,14,r88("Primes soumises \303\240 l\342\200\231accord du client. XP conserv\303\251e.\nChaque prime accord\303\251e r\303\251duit le gain de r\303\251putation locale de 1.","Bonuses require client approval. XP is preserved.\nEach granted bonus reduces local reputation gain by 1."))->setTextAlign(MyGUI::Align::Center);
 r88Text(summary,1060,2,403,24,15,r88("Versement pr\303\251vu","Expected payment"))->setTextAlign(MyGUI::Align::Right|MyGUI::Align::VCenter);reportBottom88=r88Text(summary,1060,25,403,36,36,"",true);reportBottom88->setTextAlign(MyGUI::Align::Right|MyGUI::Align::VCenter);
 finalReputationButton=r88Button(c,24,976,708,38,Loc::text("ui.validate_without_bonuses"));finalReputationButton->eventMouseButtonClick+=MyGUI::newDelegate(finalReputationClicked);
 finalCashButton=r88Button(c,772,976,740,38,Loc::text("ui.request_selected_bonuses"),true);finalCashButton->eventMouseButtonClick+=MyGUI::newDelegate(finalCashClicked);
 finalWindow->setVisible(false);
}
