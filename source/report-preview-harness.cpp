#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include "GuildProgression.h"
#include "MissionBonuses.h"
namespace MyGUI {
struct Colour {float r,g,b;Colour(float x=1,float y=1,float z=1):r(x),g(y),b(z){}};
struct IntCoord {int left,top,width,height;IntCoord(int x=0,int y=0,int w=0,int h=0):left(x),top(y),width(w),height(h){}};
struct IntSize {int width,height;IntSize(int w=1488,int h=1060):width(w),height(h){}};
struct Align {enum {Default=0,Left=0,Top=0,Right=1,Center=2};};
struct Event {void operator+=(int){}};
template<class T>int newDelegate(T){return 0;}
struct Widget;std::vector<Widget*> widgets;
struct Widget {
 int x,y,w,h,font,align;std::string kind,text,texture;Colour color;IntCoord crop;bool enabled,selected,visible;Event eventMouseButtonClick,eventWindowButtonPressed;
 Widget():x(0),y(0),w(0),h(0),font(16),align(0),color(),enabled(true),selected(false),visible(true){}
 template<class T>T* createWidget(const char* skin,int xx,int yy,int ww,int hh,int,const char* layer="",const char* name=""){T* t=new T;t->kind=skin;t->x=x+xx;t->y=y+yy;t->w=ww;t->h=hh;widgets.push_back(t);return t;}
 void setCaption(const std::string& s){text=s;}void setFontHeight(int n){font=n;}void setTextColour(Colour c){color=c;}void setTextAlign(int n){align=n;}void setNeedMouseFocus(bool){}void setColour(Colour c){color=c;}void setImageTexture(const char* s){texture=s;}void setImageCoord(IntCoord r){crop=r;}void setEnabled(bool b){enabled=b;}void setStateSelected(bool b){selected=b;}void setVisible(bool b){visible=b;}
 bool getVisible(){return visible;} Widget* getClientWidget(){return this;}
};
struct TextBox:Widget{};struct ImageBox:Widget{};struct Button:Widget{};struct Window:Widget{};typedef Widget* WidgetPtr;
struct Gui:Widget {static Gui& getInstance(){static Gui g;return g;}static Gui* getInstancePtr(){return &getInstance();}};
struct RenderManager {IntSize view;static RenderManager& getInstance(){static RenderManager r;return r;}const IntSize& getViewSize(){return view;}};
struct ResourceManager {static ResourceManager& getInstance(){static ResourceManager r;return r;}void load(const char*){}};
}
namespace Ogre {struct ResourceGroupManager {static ResourceGroupManager& getSingleton(){static ResourceGroupManager r;return r;}bool resourceLocationExists(const char*,const char*){return true;}void addResourceLocation(const char*,const char*,const char*){}};}
bool gMercenarieEnglish=false;MyGUI::Colour registerAmber(1,.61f,.14f),registerIvory(.88f,.86f,.80f);
std::string registerLanguage(const char* fr,const char* en){return gMercenarieEnglish?en:fr;}
std::string registerNumber(int n){std::ostringstream s;s<<n;return s.str();}
std::string mercenarieLocalize(const std::string& s){return s;}
MyGUI::TextBox* registerText(MyGUI::Widget* p,int x,int y,int w,int h,int f,const std::string& s,const MyGUI::Colour& c){MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Text",x,y,w,h,0);t->font=f;t->text=s;t->color=c;return t;}
MyGUI::Widget* registerSolid(MyGUI::Widget* p,int x,int y,int w,int h,const MyGUI::Colour& c){MyGUI::Widget* t=p->createWidget<MyGUI::Widget>("Solid",x,y,w,h,0);t->color=c;return t;}
MyGUI::Widget* boardPanelV6(MyGUI::Widget* p,int x,int y,int w,int h){return p->createWidget<MyGUI::Widget>("Panel",x,y,w,h,0);}
MyGUI::Colour missionColourV6(int n){return n==2?MyGUI::Colour(.22f,.80f,.92f):MyGUI::Colour(.22f,.82f,.21f);}
std::string missionLabelV6(int n){return n==2?registerLanguage("EXPEDITION SCIENTIFIQUE","SCIENTIFIC EXPEDITION"):registerLanguage("ESCORTE","ESCORT");}
void setMissionIconV6(MyGUI::ImageBox* i,int n){i->texture="ContractIconsV6.png";i->crop=MyGUI::IntCoord(n*96,0,96,96);i->color=missionColourV6(n);}
MyGUI::ImageBox* boardIconV6(MyGUI::Widget* p,int n,int x,int y,int size,const MyGUI::Colour& c){MyGUI::ImageBox* t=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,0);setMissionIconV6(t,n);t->color=c;return t;}
void fitRegisterText(MyGUI::TextBox* t,int n){t->font=n;}
MyGUI::Window* finalWindow=0;MyGUI::TextBox* finalBonusPreview=0;MyGUI::TextBox* finalSummaryText=0;MyGUI::Button* finalBonusButtons[8]={0};MyGUI::Button* finalCashButton=0;MyGUI::Button* finalHalfCashButton=0;MyGUI::Button* finalReputationButton=0;
int settlements=0,requestedFinalBonusPercent=0;bool negotiationWasPaused=false;struct MockWorld{void userPause(bool){}}mockWorld;MockWorld* ou=&mockWorld;void settleSuccessfulContract(bool){++settlements;}void finalCashClicked(MyGUI::WidgetPtr);void finalHalfCashClicked(MyGUI::WidgetPtr);void finalReputationClicked(MyGUI::WidgetPtr);void finalWindowButtonPressed(MyGUI::Window*,const std::string&);void toggleFinalBonus(MyGUI::WidgetPtr);
int guildPoints=600,guildPrestige=0,pendingGuildXp=131,missionReward=23989,advancePaid=0,finalClientTip=0,progressReportLocal=6,progressReportGlobal=1;
float escortReputation=0;int guildLevel(){return GuildProgression::level(guildPoints);}float localReputation(){return 0;}
namespace UtilityT {float random(float,float){return 0;}}
MissionBonuses::Choice finalBonusChoice;
struct Contract{int type,dangerLevel,groupSize;Contract():type(0),dangerLevel(3),groupSize(2){}}currentContract;
struct Journey{float distanceTravelled;Journey():distanceTravelled(259225){}}journeyData;
std::string originCity="Le Hub",destinationName="Village Flotsam";bool caravanMission=false,scientificMission=false,progressRosterKnown=true;std::vector<int> progressMembers(2);float missionElapsed=1000;double reportStartHour=10,reportEndHour=96;
#include "MissionReportView.h"
#pragma once
float rollFinalBonus(){return UtilityT::random(0.0f,1.0f);}
void refreshFinalBonuses(){
    if(!finalWindow||!reportBefore)return;
    reportReadOnly=false;finalReputationButton->setEnabled(true);reportTotalLabel->setCaption(registerLanguage("TOTAL PREVU","PROJECTED TOTAL"));
    const bool unlocked=guildLevel()>=1;
    const unsigned int mask=unlocked?finalBonusChoice.selected:0;
    const int count=finalBonusChoice.count(mask),cost=MissionBonuses::xpCost(pendingGuildXp,count),extra=finalBonusChoice.total(mask);
    int previewXp=guildPoints,previewPrestige=guildPrestige;GuildProgression::award(previewXp,previewPrestige,pendingGuildXp-cost);
    int baseXp=guildPoints,basePrestige=guildPrestige;GuildProgression::award(baseXp,basePrestige,pendingGuildXp);
    double local=GuildProgression::clampRep(localReputation()+progressReportLocal-count)-localReputation();
    double global=GuildProgression::clampRep(escortReputation+progressReportGlobal)-escortReputation;
    int payable=missionReward-advancePaid+finalClientTip+extra;
    setMissionIconV6(reportMissionIcon,(int)currentContract.type);reportType->setCaption(missionLabelV6((int)currentContract.type));reportType->setTextColour(missionColourV6((int)currentContract.type));
    reportFields[0]->setCaption(mercenarieLocalize(originCity)+" > "+mercenarieLocalize(destinationName)+(caravanMission||scientificMission?" > "+mercenarieLocalize(originCity):""));
    reportFields[1]->setCaption(registerNumber(missionReward)+" Cats");reportFields[2]->setCaption(registerNumber(advancePaid)+" Cats");
    std::ostringstream distance;distance.precision(3);distance<<std::fixed<<journeyData.distanceTravelled/1000.0f<<" km";std::string km=distance.str();if(!gMercenarieEnglish)std::replace(km.begin(),km.end(),'.',',');reportFields[3]->setCaption(km);
    int minutes=std::max(0,(int)(reportStartHour>=0&&reportEndHour>=reportStartHour?(reportEndHour-reportStartHour)*60:missionElapsed/60));std::ostringstream duration;duration<<minutes/1440<<registerLanguage(" j "," d ")<<(minutes/60)%24<<" h "<<minutes%60<<" min";if(reportStartHour<0)duration<<registerLanguage(" (suivie)"," (tracked)");reportFields[4]->setCaption(duration.str());
    reportFields[5]->setCaption(registerNumber(currentContract.dangerLevel)+"/5");
    int members=progressRosterKnown?(int)progressMembers.size():currentContract.groupSize;reportFields[6]->setCaption(registerNumber(members)+registerLanguage(" (dont 1 chef)"," (including 1 leader)"));
    for(int i=0;i<5;++i)reportSkulls[i]->setVisible(true);
    for(int i=0;i<5;++i)reportSkulls[i]->setColour(i<currentContract.dangerLevel?MyGUI::Colour(.8f,.08f,.17f):MyGUI::Colour(.28f,.32f,.36f));
    reportGains[0]->setCaption(registerNumber(missionReward)+" Cats");reportGains[1]->setCaption(registerNumber(extra)+" Cats");reportGains[2]->setCaption(registerNumber(payable)+" CATS");
    reportGains[3]->setCaption(reportSigned(previewXp-guildPoints));reportGains[4]->setCaption(reportSigned(local));reportGains[5]->setCaption(reportSigned(global));reportGains[6]->setCaption(reportSigned(previewPrestige-guildPrestige));
    reportCount->setCaption(registerLanguage("Primes selectionnees : ","Bonuses selected: ")+registerNumber(count)+"/"+registerNumber(finalBonusChoice.count(255)));
    reportHint->setCaption(unlocked?registerLanguage("Selectionnez les primes que vous souhaitez demander.","Select the bonuses you wish to request."):registerLanguage("Demandes de primes debloquees au niveau 1 de Guilde.","Bonus requests unlock at Guild level 1."));
    for(int i=0;i<MissionBonuses::Count;++i){
        bool earned=finalBonusChoice.amounts[i]>0,available=earned&&unlocked,selected=(mask&(1u<<i))!=0;
        finalBonusButtons[i]->setCaption("");finalBonusButtons[i]->setEnabled(available);finalBonusButtons[i]->setStateSelected(selected);
        reportChecks[i]->setCaption(selected?"X":"");
        const MyGUI::Colour color=available?MyGUI::Colour(.88f,.68f,.39f):MyGUI::Colour(.44f,.47f,.49f);
        reportIcons[i]->setColour(color);reportNames[i]->setTextColour(available?registerIvory:MyGUI::Colour(.58f,.61f,.64f));reportDescriptions[i]->setTextColour(available?MyGUI::Colour(.73f,.76f,.78f):MyGUI::Colour(.5f,.53f,.56f));
        reportAmounts[i]->setCaption(earned?(unlocked?"+ "+registerNumber(finalBonusChoice.amounts[i])+" CATS":registerLanguage("Niveau 1 requis","Level 1 required")):registerLanguage("Prime non acquise","Not earned"));reportAmounts[i]->setTextColour(available?registerAmber:MyGUI::Colour(.58f,.61f,.64f));
        for(int e=0;e<4;++e)reportEdges[i][e]->setColour(available?registerAmber:MyGUI::Colour(.38f,.41f,.43f));
        fitRegisterText(reportNames[i],rf(18));fitRegisterText(reportDescriptions[i],rf(14));fitRegisterText(reportAmounts[i],rf(available?19:16));
    }
    std::ostringstream before;before<<registerLanguage("Avant demandes de primes :","Before bonus requests:")<<"\n"<<registerLanguage("XP de Guilde : ","Guild XP: ")<<reportSigned(baseXp-guildPoints)<<"\n"<<registerLanguage("Reputation locale : ","Local reputation: ")<<reportSigned(GuildProgression::clampRep(localReputation()+progressReportLocal)-localReputation())<<"\n"<<registerLanguage("Reputation globale : ","Global reputation: ")<<reportSigned(global);reportBefore->setCaption(before.str());
    std::ostringstream preview;preview<<registerLanguage("Si toutes les primes sont accordees : ","If all bonuses are granted: ")<<"+"<<registerNumber(extra)<<" Cats\n"<<registerLanguage("XP apres primes : ","XP after bonuses: ")<<reportSigned(previewXp-guildPoints)<<registerLanguage(" | Reputation locale : "," | Local reputation: ")<<reportSigned(local)<<"\n"<<registerLanguage("Reputation globale : ","Global reputation: ")<<reportSigned(global)<<"\n"<<registerLanguage("Acompte deduit ; pourboire inclus : ","Advance deducted; included tip: ")<<registerNumber(finalClientTip)<<" Cats";finalBonusPreview->setCaption(preview.str());
    std::ostringstream after;after<<registerLanguage("Versement : ","Payout: ")<<registerNumber(payable)<<" Cats\n"<<registerLanguage("XP : ","XP: ")<<reportSigned(previewXp-guildPoints)<<registerLanguage(" | Prestige : "," | Prestige: ")<<reportSigned(previewPrestige-guildPrestige)<<"\n"<<registerLanguage("Rep. locale : ","Local rep.: ")<<reportSigned(local)<<registerLanguage(" | Globale : "," | Global: ")<<reportSigned(global);reportAfter->setCaption(after.str());
    for(int i=0;i<7;++i){fitRegisterText(reportFields[i],rf(17));fitRegisterText(reportGains[i],rf(i==2?26:18));}
    finalCashButton->setEnabled(unlocked&&count>0);finalCashButton->setCaption(registerLanguage("DEMANDER LES PRIMES CHOISIES","REQUEST SELECTED BONUSES"));
    finalHalfCashButton->setEnabled(unlocked&&finalBonusChoice.count(255)>0);finalHalfCashButton->setCaption(registerLanguage("TOUT SELECTIONNER / RETIRER","SELECT ALL / CLEAR ALL"));
    finalReputationButton->setCaption(registerLanguage("VALIDER SANS PRIME","VALIDATE WITHOUT BONUSES"));
}
void toggleFinalBonus(MyGUI::WidgetPtr sender){
    if(guildLevel()<1)return;
    for(int i=0;i<MissionBonuses::Count;++i)if(sender==finalBonusButtons[i]&&finalBonusChoice.amounts[i]>0)finalBonusChoice.selected^=1u<<i;
    refreshFinalBonuses();
}
    void finalCashClicked(MyGUI::WidgetPtr){if(!finalWindow||!finalWindow->getVisible())return;if(reportReadOnly){finalWindow->setVisible(false);reportReadOnly=false;if(!negotiationWasPaused)ou->userPause(false);return;}requestedFinalBonusPercent=100;settleSuccessfulContract(true);}
    void finalHalfCashClicked(MyGUI::WidgetPtr){if(reportReadOnly||!finalWindow||!finalWindow->getVisible()||guildLevel()<1)return;unsigned int available=0;for(int i=0;i<MissionBonuses::Count;++i)if(finalBonusChoice.amounts[i]>0)available|=1u<<i;finalBonusChoice.selected=finalBonusChoice.selected==available?0:available;refreshFinalBonuses();}
    void finalReputationClicked(MyGUI::WidgetPtr){if(reportReadOnly||!finalWindow||!finalWindow->getVisible())return;settleSuccessfulContract(false);}
    void finalWindowButtonPressed(MyGUI::Window*,const std::string&){if(reportReadOnly){finalWindow->setVisible(false);reportReadOnly=false;if(!negotiationWasPaused)ou->userPause(false);return;}settleSuccessfulContract(false);}


int main(int argc,char** argv){
 gMercenarieEnglish=argc>1&&std::string(argv[1])=="en";if(argc>2){MyGUI::RenderManager::getInstance().view=MyGUI::IntSize(1280,720);}
 buildFinalReport();finalBonusChoice.amounts[0]=2998;finalBonusChoice.amounts[1]=2398;finalBonusChoice.amounts[2]=3427;finalBonusChoice.amounts[5]=1999;finalBonusChoice.amounts[6]=300;finalBonusChoice.cap(11994);refreshFinalBonuses();
 assert(reportCount->text.find("0/5")!=std::string::npos&&!finalCashButton->enabled&&!finalBonusButtons[3]->enabled);
 toggleFinalBonus(finalBonusButtons[0]);assert(finalCashButton->enabled&&reportChecks[0]->text=="X");assert(reportGains[2]->text=="26987 CATS");toggleFinalBonus(finalBonusButtons[3]);assert(finalBonusChoice.selected==1);
 guildPoints=0;refreshFinalBonuses();assert(!finalCashButton->enabled&&!finalBonusButtons[0]->enabled);guildPoints=600;refreshFinalBonuses();
 finalWindow->setVisible(true);finalHalfCashClicked(0);assert(finalBonusChoice.count(finalBonusChoice.selected)==5);finalHalfCashClicked(0);assert(finalBonusChoice.selected==0);toggleFinalBonus(finalBonusButtons[0]);reportReadOnly=true;finalReputationClicked(0);assert(settlements==0);finalCashClicked(0);finalCashClicked(0);assert(settlements==0&&!finalWindow->getVisible());reportReadOnly=false;
 std::ofstream out("report-preview.tsv");
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];std::string text=t->text;for(size_t j=0;j<text.size();++j)if(text[j]=='\n')text[j]='~';out<<t->kind<<'\t'<<t->x<<'\t'<<t->y<<'\t'<<t->w<<'\t'<<t->h<<'\t'<<t->font<<'\t'<<t->align<<'\t'<<(int)(t->color.r*255)<<'\t'<<(int)(t->color.g*255)<<'\t'<<(int)(t->color.b*255)<<'\t'<<t->texture<<'\t'<<t->crop.left<<'\t'<<text<<'\n';}
 std::cout<<"PASS: report layout creation and production refresh, selection, disabled cards, level lock, projected payment\n";
}
