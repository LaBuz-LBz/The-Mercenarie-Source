#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cmath>



namespace MyGUI {
struct Colour{float r,g,b;Colour(float R=1,float G=1,float B=1):r(R),g(G),b(B){}};
struct IntPoint{int left,top;IntPoint(int x=0,int y=0):left(x),top(y){}};
struct IntSize{int width,height;IntSize(int w=1400,int h=940):width(w),height(h){}bool operator==(const IntSize& o)const{return width==o.width&&height==o.height;}};
struct IntCoord{int left,top,width,height;IntCoord(int x=0,int y=0,int w=0,int h=0):left(x),top(y),width(w),height(h){}};
struct MouseButton{};
struct Align{enum{Default=0,Left=0,Top=0,Center=1};};
struct Event{void operator+=(int){}};template<class T>int newDelegate(T){return 0;}
std::map<std::string,std::map<unsigned int,double> > advances;
struct Widget;std::vector<Widget*> widgets;
struct Widget{
 IntPoint viewOffset;int x,y,w,h,font,id,parent,align,canvasW,canvasH;float alpha;bool scroll,selected,enabled,visible,hscroll;std::string kind,text,texture,fontName;Colour color;IntCoord crop;std::map<std::string,std::string> props;Event eventWindowButtonPressed,eventComboChangePosition,eventEditTextChange,eventMouseButtonClick,eventMouseSetFocus,eventMouseLostFocus,eventMouseButtonPressed,eventMouseButtonReleased;
 Widget():x(0),y(0),w(0),h(0),font(18),id(-1),parent(-1),align(0),canvasW(0),canvasH(0),alpha(1),scroll(false),selected(false),enabled(true),visible(true),hscroll(false),fontName("ArtisanBody18"){}
 template<class T>T* createWidget(const char* skin,int xx,int yy,int ww,int hh,int,const char* layer="",const char* name=""){if(ww<=0||hh<=0){std::cerr<<"Invalid "<<skin<<" "<<xx<<","<<yy<<","<<ww<<","<<hh<<"\n";abort();}T* t=new T;t->parent=id;t->id=(int)widgets.size();t->x=x+xx;t->y=y+yy;t->w=ww;t->h=hh;t->kind=skin;widgets.push_back(t);return t;}
 template<class T>T* createWidget(const char* skin,IntCoord r,int a,const char* layer,const char* name){return createWidget<T>(skin,r.left,r.top,r.width,r.height,a,layer,name);}
 size_t getChildCount(){size_t n=0;for(size_t i=0;i<widgets.size();++i)if(widgets[i]->parent==id)++n;return n;}
Widget* getChildAt(size_t n){for(size_t i=0;i<widgets.size();++i)if(widgets[i]->parent==id){if(n==0)return widgets[i];--n;}return 0;}
std::string getUserString(const char* k){return props[k];}
void setCanvasAlign(int){}void setVisibleVScroll(bool b){props["vscroll"]=b?"1":"0";}void setVisibleHScroll(bool b){hscroll=b;}void setCanvasSize(int a,int b){assert(a>0&&b>0);scroll=true;canvasW=a;canvasH=b;}void setViewOffset(IntPoint p){viewOffset=p;}IntPoint getViewOffset(){return viewOffset;}
 void setCoord(int a,int b,int c,int d){setPosition(a,b);setSize(c,d);}void setPosition(int a,int b){int px=parent<0?0:widgets[parent]->x,py=parent<0?0:widgets[parent]->y;int dx=px+a-x,dy=py+b-y;x+=dx;y+=dy;for(size_t i=0;i<widgets.size();++i){int ancestor=widgets[i]->parent;while(ancestor>=0&&ancestor!=id)ancestor=widgets[ancestor]->parent;if(ancestor==id){widgets[i]->x+=dx;widgets[i]->y+=dy;}}}void setSize(int a,int b){assert(a>0&&b>0);w=a;h=b;}int getHeight(){return h;}int getWidth(){return w;}int getTop(){return parent<0?y:y-widgets[parent]->y;}int getFontHeight(){return font;}
 void setFontHeight(int f){font=f;}void setFontName(const std::string& s){fontName=s;}void setTextAlign(int a){align=a;}struct Caption{std::string value;std::string asUTF8(){return value;}};Caption getCaption(){Caption c;c.value=text;return c;}bool getStateSelected(){return selected;}void setCaption(const std::string& s){text=s;}
 IntSize getTextSize(){double width=0,line=0;int lines=1;for(size_t i=0;i<text.size();){unsigned char c=text[i++];unsigned int point=c;int extra=0;if(c>=240){point=c&7;extra=3;}else if(c>=224){point=c&15;extra=2;}else if(c>=192){point=c&31;extra=1;}while(extra--&&i<text.size())point=(point<<6)|((unsigned char)text[i++]&63);if(point==10){width=std::max(width,line);line=0;++lines;}else{double adv=advances[fontName][point];line+=adv?adv:font*.55;}}return IntSize((int)std::ceil(std::max(width,line)),lines*(font+2));}
 void setAlpha(float a){alpha=a;}void setColour(Colour c){color=c;}void setTextColour(Colour c){color=c;}void setNeedMouseFocus(bool b){props["mouseOff"]=b?"0":"1";}void setInheritsPick(bool b){props["inheritsPick"]=b?"1":"0";}void setStateSelected(bool b){selected=b;}void setImageCoord(IntCoord c){crop=c;}void setImageTexture(const char* s){texture=s;}
 int getLeft(){return x;}bool getVisible(){return visible;}Widget* getClientWidget(){return this;}void setVisible(bool b){visible=b;}void setEnabled(bool b){enabled=b;}void setUserString(const char* k,const std::string& v){props[k]=v;}void setComboModeDrop(bool){}void addItem(const std::string& s){if(text.empty())text=s;}void setMaxListLength(int){}void setIndexSelected(int){}
};
struct ScrollBar:Widget{size_t position,range;Event eventScrollChangePosition;ScrollBar():position(0),range(46){}void setTrackSize(int){}void setMoveToClick(bool){}void setScrollRange(size_t n){range=n;}void setScrollPosition(size_t n){position=n;}size_t getScrollPosition(){return position;}void setScrollPage(int){}};struct Window:Widget{};typedef Widget* WidgetPtr;struct TextBox:Widget{};struct Button:TextBox{};struct ImageBox:Widget{};struct ScrollView:Widget{};struct ComboBox:TextBox{};struct EditBox:TextBox{};
struct Gui:Widget{static Gui& getInstance(){static Gui x;return x;}static Gui* getInstancePtr(){return &getInstance();}int getEnumerator(){return 0;}};
struct RenderManager{IntSize v;static RenderManager& getInstance(){static RenderManager x;return x;}IntSize getViewSize(){return v;}};
struct ResourceManager{static ResourceManager& getInstance(){static ResourceManager x;return x;}bool isExist(const char*){return true;}void load(const char*){}};
}
namespace Ogre{struct ResourceGroupManager{static ResourceGroupManager& getSingleton(){static ResourceGroupManager r;return r;}bool resourceLocationExists(const char*,const char*){return true;}void addResourceLocation(const char*,const char*,const char*){}};}


#include <iomanip>
#include "Localization.h"
#include "src/Contracts/EstimatedPricing.h"
using ContractPricing::calculateEstimatedContract;
namespace MercenarieFonts{void caption(MyGUI::Widget* w,const std::string& s){w->setCaption(s);}}
namespace MercenarieNativeInput{void bind(MyGUI::Widget*){}}
void mercenarieDestroyLiveWidget(MyGUI::Widget*){for(size_t i=0;i<MyGUI::widgets.size();++i)delete MyGUI::widgets[i];MyGUI::widgets.clear();}
std::string mercenarieLocalize(const std::string& s){return s;}
std::string registerLanguage(const char* a,const char*){return a;}
MyGUI::Widget* mercenarieLauncherMenu=0;
#include "quest-launcher-v9.generated.h"
struct MissionVisualV6 { int slot; unsigned char r,g,b; const char* fr; const char* en; };
const MissionVisualV6& missionVisualV6(int type){
    static MissionVisualV6 visuals[]={
        {0,55,210,54,Loc::text("ui.escort"),Loc::text("ui.escort")},
        {1,205,146,65,Loc::text("ui.caravan"),Loc::text("ui.caravan")},
        {2,57,205,235,Loc::text("ui.scientific_expedition"),Loc::text("ui.scientific_expedition")},
        {3,206,76,83,Loc::text("ui.bounty_hunt"),Loc::text("ui.bounty_hunt")},
        {4,210,183,98,Loc::text("ui.message_delivery"),Loc::text("ui.message_delivery")},
        {5,160,126,191,Loc::text("ui.long_distance"),Loc::text("ui.long_distance")}
    };
    static MissionVisualV6 unknown={6,153,163,166,Loc::text("common.mission"),Loc::text("common.mission")};
    const char* keys[]={"ui.escort","ui.caravan","ui.scientific_expedition","ui.bounty_hunt","ui.message_delivery","ui.long_distance"};
    for(int i=0;i<6;++i)visuals[i].fr=visuals[i].en=Loc::text(keys[i]);
    unknown.fr=unknown.en=Loc::text("common.mission");
    return type>=0&&type<static_cast<int>(sizeof(visuals)/sizeof(visuals[0]))?visuals[type]:unknown;
}
MyGUI::Colour missionColourV6(int type){
    const MissionVisualV6& v=missionVisualV6(type);return MyGUI::Colour(v.r/255.0f,v.g/255.0f,v.b/255.0f);
}
std::string missionLabelV6(int type){
    const MissionVisualV6& v=missionVisualV6(type);return registerLanguage(v.fr,v.en);
}
void setMissionIconV6(MyGUI::ImageBox* icon,int type){
    icon->setImageTexture("ContractIconsV6.png");
    icon->setImageCoord(MyGUI::IntCoord(missionVisualV6(type).slot*96,0,96,96));
    icon->setColour(missionColourV6(type));
}
std::string registerNumber(long long n){std::stringstream out;out<<n;std::string s=out.str();for(int i=(int)s.size()-3;i>(n<0?1:0);i-=3)s.insert(i," ");return s;}

MyGUI::Window* negotiationWindow=0;
MyGUI::Widget* negotiationSliderPanel=0;
MyGUI::Button *negotiationSliderTrack=0,*sliderMinusButton=0,*sliderPlusButton=0,*bonusButtons[6]={0},*proposeButton=0,*counterButton=0,*returnButton=0,*refuseButton=0;
MyGUI::TextBox *negotiationSliderTitle=0,*proposalText=0,*sliderPercentText=0,*paymentBreakdownText=0,*reactionText=0,*offerText=0;
MyGUI::ScrollBar* priceSlider=0;
EscortContractData currentContract;int proposedReward=0,negotiatedAdvancePercent=0,baseReward=0;
bool counterOfferActive=false,scientificMission=false,caravanMission=false;
std::string originCity="Le Hub",destinationName="Village-Ruche";
struct Actor{std::string name;std::string getName(){return name;}void sayALine(const char*,bool){}}actor;
Actor* escort=&actor;
int suspended=0,accepted=0,agreed=0;
void suspendNegotiation(){++suspended;}
void negotiationWindowButtonPressed(MyGUI::Window*,const std::string&){suspendNegotiation();}
void refuseNegotiationClicked(MyGUI::WidgetPtr){}
void returnToContractsClicked(MyGUI::WidgetPtr){}
void acceptNegotiatedContract(int reward,bool){++accepted;agreed=reward;}
bool negotiationOpen=true,rareContract=false;int negotiationInsistence=0,counterOffer=0;float clientBudgetMultiplier=1.1f,escortReputation=0;
int localReputation(){return 0;}
namespace GuildProgression{float chanceBonus(int,float){return 0;}}
namespace UtilityT{float roll=0;float random(float,float){return roll;}int randomInt(int,int){return 0;}}
struct CityMemory{int abuses,recovery;CityMemory():abuses(0),recovery(0){}};std::map<std::string,CityMemory> cityMemories;
namespace ReputationIdentity{std::string key(const std::string& s){return s;}}
void layoutNegotiationButtons(bool);
#include "NegotiationView.h"
    void updateNegotiationPrice(size_t position)
    {
        int percent = static_cast<int>(position) - 20;
        calculateEstimatedContract(currentContract,percent);
        proposedReward=currentContract.totalPay;
        currentContract.bonusPay=0;
        currentContract.advance=currentContract.totalPay*negotiatedAdvancePercent/100;
        currentContract.finalPay=currentContract.totalPay-currentContract.advance;
        refreshNegotiationV9Price(percent);
        if(reactionText&&!counterOfferActive){float repEffect=std::max(-5.0f,std::min(5.0f,-percent*0.25f));char consequence[4096];sprintf_s(consequence,mercenarieLocalize(Loc::text("ui.expected_reaction_s_consequences_d_cats_estimated_reputation_1f")).c_str(),mercenarieLocalize(percent<=0?Loc::text("v8.mood.0"):percent<=10?Loc::text("v8.mood.1"):percent<=25?Loc::text("v8.mood.2"):Loc::text("v8.mood.3")).c_str(),currentContract.totalPay,repEffect);negotiationReaction(consequence);}
    }
    void bonusClicked(MyGUI::WidgetPtr sender)
    {
        const int flags[]={EB_HEALTHY,EB_HALF_NOW,EB_FAST,EB_NO_KO,EB_DANGER,EB_SUPPLIES};
        if(sender==bonusButtons[1]){if(negotiatedAdvancePercent==0)negotiatedAdvancePercent=10;else if(negotiatedAdvancePercent==10)negotiatedAdvancePercent=25;else if(negotiatedAdvancePercent==25)negotiatedAdvancePercent=50;else negotiatedAdvancePercent=0;if(negotiatedAdvancePercent)currentContract.selectedBonuses|=EB_HALF_NOW;else currentContract.selectedBonuses&=~EB_HALF_NOW;char advanceLabel[80];sprintf_s(advanceLabel,mercenarieLocalize(Loc::text("ui.advance_d")).c_str(),negotiatedAdvancePercent);MercenarieFonts::caption(bonusButtons[1],advanceLabel);bonusButtons[1]->setStateSelected(negotiatedAdvancePercent>0);updateNegotiationPrice(priceSlider?priceSlider->getScrollPosition():20);return;}
        for(int i=0;i<6;++i)if(sender==bonusButtons[i]){bool selected=!bonusButtons[i]->getStateSelected();bonusButtons[i]->setStateSelected(selected);if(selected)currentContract.selectedBonuses|=flags[i];else currentContract.selectedBonuses&=~flags[i];break;}
        updateNegotiationPrice(priceSlider?priceSlider->getScrollPosition():20);
    }
    void negotiationSliderChanged(MyGUI::ScrollBar*, size_t position)
    {
        updateNegotiationPrice(position);
    }
    void negotiationMinusClicked(MyGUI::WidgetPtr)
    {
        if(!priceSlider) return;
        size_t position=priceSlider->getScrollPosition();
        position=position<5?0:position-5;
        priceSlider->setScrollPosition(position);
        updateNegotiationPrice(position);
    }
    void negotiationPlusClicked(MyGUI::WidgetPtr)
    {
        if(!priceSlider) return;
        size_t position=priceSlider->getScrollPosition();
        position=std::min<size_t>(45,position+5);
        priceSlider->setScrollPosition(position);
        updateNegotiationPrice(position);
    }
    void proposePriceClicked(MyGUI::WidgetPtr)
    {
        if (!negotiationOpen) return;
        if(counterOfferActive){++negotiationInsistence;if(negotiationInsistence>2){if(escort)escort->sayALine(Loc::text("ui.i_have_already_made_two_efforts_the_discussion_is"),true);suspendNegotiation();return;}}
        int percent = baseReward ? ((proposedReward - baseReward) * 100 / baseReward) : 0;
        bool withinBudget = proposedReward <= static_cast<int>(baseReward * clientBudgetMultiplier);
        int bonusCount=0;for(int mask=currentContract.selectedBonuses;mask;mask>>=1)bonusCount+=mask&1;
        float chance = std::max(.03f,std::min(.97f,EscortEconomy::acceptance(percent,bonusCount,0,0,currentContract.wealth,currentContract.personality,currentContract.danger,currentContract.prestigious)+GuildProgression::chanceBonus(localReputation(),escortReputation)));
        if (counterOfferActive) chance *= 0.45f;
        if (percent <= 0 || (withinBudget && UtilityT::random(0.0f, 1.0f) <= chance))
        {
            const char* accepted[] = {
                Loc::text("ui.deal_concluded_you_know_how_to_negotiate"), Loc::text("ui.agreed_but_you_will_have_to_earn_that_amount"),
                Loc::text("ui.i_accept_do_not_make_me_regret_this_concession")
            };
            if (escort) escort->sayALine(accepted[UtilityT::randomInt(0, 2)], true);
            acceptNegotiatedContract(proposedReward, percent <= 0);
            return;
        }

        CityMemory& memory = cityMemories[ReputationIdentity::key(originCity)];
        if (percent >= 25) { ++memory.abuses; memory.recovery = 3; }
        counterOffer = std::min(static_cast<int>(baseReward * clientBudgetMultiplier),
            baseReward + (proposedReward - baseReward) / 2);
        counterOfferActive = true;
        char text[4096];
        sprintf_s(text, mercenarieLocalize(Loc::text("ui.d_cats_i_can_offer_d_cats_attempts_used")).c_str(), proposedReward, counterOffer,negotiationInsistence,negotiationInsistence>=2?mercenarieLocalize(Loc::text("ui.further_insistence_will_void_the_contract")).c_str():"");
        if (reactionText) negotiationReaction(text);
        if (counterButton) { MercenarieFonts::caption(counterButton,Loc::text("ui.accept_the_counteroffer")); layoutNegotiationButtons(true); }
        if (proposeButton) { MercenarieFonts::caption(proposeButton,Loc::text("ui.insist_risk_cancellation")); negotiationFitButton(proposeButton); }
    }
    void acceptCounterClicked(MyGUI::WidgetPtr)
    {
        if (counterOfferActive) acceptNegotiatedContract(counterOffer, false);
    }
    void layoutNegotiationButtons(bool showCounter)
    {
        if(!negotiationWindow||!proposeButton||!counterButton||!returnButton||!refuseButton)return;
        MyGUI::Widget* c=negotiationWindow->getClientWidget();int inner=c->getWidth()-32,y=c->getHeight()-58,gap=8,w=(inner-2*gap)/3;
        proposeButton->setCoord(16,y,w,46);returnButton->setCoord(16+w+gap,y,w,46);refuseButton->setCoord(16+2*(w+gap),y,w,46);
        counterButton->setCoord(16,y-52,inner,46);counterButton->setVisible(showCounter);
        negotiationContent->setSize(inner,y-negotiationContent->getTop()-(showCounter?60:8));
        negotiationFitButton(proposeButton);negotiationFitButton(returnButton);negotiationFitButton(refuseButton);negotiationFitButton(counterButton);
    }

#include "NegotiationBuildView.h"

std::string escaped(std::string s){size_t i=0;while((i=s.find('\n',i))!=std::string::npos){s.replace(i,1,"\\n");i+=2;}return s;}
int main(int argc,char** argv){assert(argc>=7);int w=atoi(argv[1]),h=atoi(argv[2]),type=atoi(argv[4]),mode=atoi(argv[5]);Loc::configure("Localization",argv[3]);
std::ifstream metrics(argv[6]);std::string name;unsigned int point;double advance;while(metrics>>name>>point>>advance)MyGUI::advances[name][point]=advance;
MyGUI::RenderManager::getInstance().v=MyGUI::IntSize(w,h);
currentContract.type=(MercContractType)type;currentContract.distanceKm=154.4f;currentContract.dangerLevel=3;currentContract.basePay=21422;currentContract.routeRegions="V6EST:1.25:ROAD";
caravanMission=type==1;scientificMission=type==2;actor.name="Soto";
if(mode==1){actor.name="Soto le voyageur des terres septentrionales et des montagnes";originCity="La grande forteresse des voyageurs des montagnes grises";destinationName="Le village des chercheurs de la frontiere occidentale";currentContract.basePay=EscortConfig::AbsoluteContractCap/2;}
if(mode==2)currentContract.distanceKm=0;
calculateEstimatedContract(currentContract,0);baseReward=currentContract.basePay;
createNegotiationUI();updateNegotiationPrice(20);layoutNegotiationButtons(false);negotiationWindow->setVisible(true);
// Reopening rebuilds only the view and preserves the existing proposal/advance.
size_t nodesBefore=MyGUI::widgets.size();priceSlider->setScrollPosition(31);negotiatedAdvancePercent=25;updateNegotiationPrice(31);int savedTotal=proposedReward;
createNegotiationUI();assert(MyGUI::widgets.size()==nodesBefore);assert(priceSlider->getScrollPosition()==31&&bonusButtons[1]->getStateSelected());updateNegotiationPrice(31);assert(proposedReward==savedTotal&&negotiatedAdvancePercent==25);
negotiatedAdvancePercent=0;priceSlider->setScrollPosition(20);updateNegotiationPrice(20);layoutNegotiationButtons(false);negotiationWindow->setVisible(true);
for(int i=0;i<6;++i)assert(missionVisualV6(i).slot==i);assert(missionVisualV6(-1).slot==6);
assert(offerText->text.find(actor.name.substr(0,4))!=std::string::npos);
bool found=false;for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* n=MyGUI::widgets[i];if(n->texture=="ContractIconsV6.png"){assert(n->crop.left==missionVisualV6(type).slot*96);found=true;}}assert(found);
for(int pos=0;pos<=45;++pos){priceSlider->setScrollPosition(pos);updateNegotiationPrice(pos);EscortContractData expected=currentContract;calculateEstimatedContract(expected,pos-20);assert(proposedReward==expected.totalPay);assert(currentContract.advance+currentContract.finalPay==proposedReward);assert(sliderMinusButton->enabled==(pos>0));assert(sliderPlusButton->enabled==(pos<45));}
priceSlider->setScrollPosition(0);negotiationMinusClicked(0);assert(priceSlider->getScrollPosition()==0);negotiationPlusClicked(0);assert(priceSlider->getScrollPosition()==5);
priceSlider->setScrollPosition(43);negotiationPlusClicked(0);assert(priceSlider->getScrollPosition()==45);negotiationPlusClicked(0);assert(priceSlider->getScrollPosition()==45);negotiationMinusClicked(0);assert(priceSlider->getScrollPosition()==40);
int stages[]={10,25,50,0};for(int i=0;i<4;++i){bonusClicked(bonusButtons[1]);assert(negotiatedAdvancePercent==stages[i]);assert(currentContract.advance==currentContract.totalPay*stages[i]/100);assert(currentContract.finalPay+currentContract.advance==currentContract.totalPay);}
priceSlider->setScrollPosition(20);updateNegotiationPrice(20);proposePriceClicked(0);assert(accepted==1&&agreed==proposedReward);
priceSlider->setScrollPosition(45);updateNegotiationPrice(45);UtilityT::roll=1;proposePriceClicked(0);assert(counterOfferActive&&counterButton->visible);acceptCounterClicked(0);assert(accepted==2&&agreed==counterOffer);
counterOfferActive=false;priceSlider->setScrollPosition(20);updateNegotiationPrice(20);MercenarieFonts::caption(proposeButton,Loc::text("ui.validate_and_propose"));layoutNegotiationButtons(mode==3);
negotiationReaction(mode==3?Loc::text("ui.further_insistence_will_void_the_contract"):Loc::text("ui.move_the_slider_from_20_to_25_the_client"));
negotiationCloseV9(0);assert(suspended==1);
for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];if(!t->visible)continue;
if(!t->text.empty()){MyGUI::IntSize z=t->getTextSize();int pad=t->kind=="NegotiationV9Button"?16:0;if(z.width>t->w-pad+2||z.height>t->h+2){std::cerr<<"Text overflow: "<<t->text<<" / "<<z.width<<","<<z.height<<" in "<<t->w<<","<<t->h;return 2;}}
if(t->parent>=0&&MyGUI::widgets[t->parent]!=negotiationContent){MyGUI::Widget* owner=MyGUI::widgets[t->parent];if(t->x<owner->x||t->y<owner->y||t->x+t->w>owner->x+owner->w||t->y+t->h>owner->y+owner->h){std::cerr<<"Parent overflow: "<<t->kind<<" "<<t->text;return 3;}}}
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];std::cout<<t->id<<'\t'<<t->parent<<'\t'<<t->kind<<'\t'<<t->x<<'\t'<<t->y<<'\t'<<t->w<<'\t'<<t->h<<'\t'<<t->font<<'\t'<<t->color.r<<'\t'<<t->color.g<<'\t'<<t->color.b<<'\t'<<t->scroll<<'\t'<<t->selected<<'\t'<<escaped(t->text)<<'\t'<<t->texture<<'\t'<<t->fontName<<'\t'<<t->alpha<<'\t'<<t->crop.left<<'\t'<<t->crop.top<<'\t'<<t->crop.width<<'\t'<<t->crop.height<<'\t'<<t->align<<'\t'<<t->enabled<<'\t'<<t->visible<<'\t'<<t->canvasW<<'\t'<<t->canvasH<<'\t'<<t->hscroll<<'\n';}
}
