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
 void setPosition(int a,int b){int px=parent<0?0:widgets[parent]->x,py=parent<0?0:widgets[parent]->y;int dx=px+a-x,dy=py+b-y;x+=dx;y+=dy;for(size_t i=0;i<widgets.size();++i){int ancestor=widgets[i]->parent;while(ancestor>=0&&ancestor!=id)ancestor=widgets[ancestor]->parent;if(ancestor==id){widgets[i]->x+=dx;widgets[i]->y+=dy;}}}void setSize(int a,int b){assert(a>0&&b>0);w=a;h=b;}int getHeight(){return h;}int getWidth(){return w;}int getTop(){return y;}int getFontHeight(){return font;}
 void setFontHeight(int f){font=f;}void setFontName(const std::string& s){fontName=s;}void setTextAlign(int a){align=a;}void setCaption(const std::string& s){text=s;}
 IntSize getTextSize(){double width=0,line=0;int lines=1;for(size_t i=0;i<text.size();){unsigned char c=text[i++];unsigned int point=c;int extra=0;if(c>=240){point=c&7;extra=3;}else if(c>=224){point=c&15;extra=2;}else if(c>=192){point=c&31;extra=1;}while(extra--&&i<text.size())point=(point<<6)|((unsigned char)text[i++]&63);if(point==10){width=std::max(width,line);line=0;++lines;}else{double adv=advances[fontName][point];line+=adv?adv:font*.55;}}return IntSize((int)std::ceil(std::max(width,line)),lines*(font+2));}
 void setAlpha(float a){alpha=a;}void setColour(Colour c){color=c;}void setTextColour(Colour c){color=c;}void setNeedMouseFocus(bool b){props["mouseOff"]=b?"0":"1";}void setInheritsPick(bool b){props["inheritsPick"]=b?"1":"0";}void setStateSelected(bool b){selected=b;}void setImageCoord(IntCoord c){crop=c;}void setImageTexture(const char* s){texture=s;}
 int getLeft(){return x;}bool getVisible(){return visible;}Widget* getClientWidget(){return this;}void setVisible(bool b){visible=b;}void setEnabled(bool b){enabled=b;}void setUserString(const char* k,const std::string& v){props[k]=v;}void setComboModeDrop(bool){}void addItem(const std::string& s){if(text.empty())text=s;}void setMaxListLength(int){}void setIndexSelected(int){}
};
struct Window:Widget{};typedef Widget* WidgetPtr;struct TextBox:Widget{};struct Button:TextBox{};struct ImageBox:Widget{};struct ScrollView:Widget{};struct ComboBox:TextBox{};struct EditBox:TextBox{};
struct Gui:Widget{static Gui& getInstance(){static Gui x;return x;}int getEnumerator(){return 0;}};
struct RenderManager{IntSize v;static RenderManager& getInstance(){static RenderManager x;return x;}IntSize getViewSize(){return v;}};
struct ResourceManager{static ResourceManager& getInstance(){static ResourceManager x;return x;}bool isExist(const char*){return true;}void load(const char*){}};
}
namespace Ogre{struct ResourceGroupManager{static ResourceGroupManager& getSingleton(){static ResourceGroupManager r;return r;}bool resourceLocationExists(const char*,const char*){return true;}void addResourceLocation(const char*,const char*,const char*){}};}

#include "Localization.h"
#include "DelegatedMissionTiming.h"
namespace MercenarieFonts{void caption(MyGUI::Widget* w,const std::string& s){w->setCaption(s);}}
namespace MercenarieNativeInput{void bind(MyGUI::Widget*){}}
void mercenarieDestroyLiveWidget(MyGUI::Widget*){for(size_t i=0;i<MyGUI::widgets.size();++i)delete MyGUI::widgets[i];MyGUI::widgets.clear();}
std::string mercenarieLocalize(const std::string& s){return s;}
MyGUI::Window* trackerWindow=0;MyGUI::Widget* mercenarieLauncherMenu=0;
void closeMercenarieInterface(MyGUI::Window*,const std::string&){}
#include "quest-launcher-v9.generated.h"
    struct DelegatedMissionState {
        int version,kind,reward,difficulty,sent,returned,injured,amputations,dead,guildXp,reputationDelta,paymentState;
        bool resultRolled,success,completed,reportShown;
        float successChance,successRoll;
        double completedAt;
        std::string groupId,offerIdentity,issuerIdentity,boardKey,title,origin,destination,officeKey,clientName,outcomeCode;
        DelegatedMissionTiming::State timing;
        DelegatedMissionState():version(4),kind(0),reward(0),difficulty(0),sent(0),returned(0),injured(0),amputations(0),dead(0),guildXp(0),reputationDelta(0),paymentState(0),resultRolled(false),success(false),completed(false),reportShown(false),successChance(0),successRoll(0),completedAt(0){}
        template<class Archive> void archive(Archive& a){a.field(version);a.field(kind);a.field(reward);a.field(offerIdentity);a.field(issuerIdentity);a.field(boardKey);a.field(title);a.field(origin);a.field(destination);timing.archive(a);if(!a.reading||version>=2){a.field(resultRolled);a.field(success);}else if(a.reading){resultRolled=true;success=true;}if(!a.reading||version>=3)a.field(groupId);else if(a.reading)groupId="legacy";if(!a.reading||version>=4){a.field(difficulty);a.field(sent);a.field(returned);a.field(injured);a.field(amputations);a.field(dead);a.field(guildXp);a.field(reputationDelta);a.field(paymentState);a.field(completed);a.field(reportShown);a.field(successChance);a.field(successRoll);a.field(completedAt);a.field(officeKey);a.field(clientName);a.field(outcomeCode);}else if(a.reading){difficulty=0;sent=returned=injured=amputations=dead=0;guildXp=35;reputationDelta=success?1:-1;paymentState=completed=reportShown=0;successChance=successRoll=0;completedAt=0;version=4;}if(a.reading&&((version<1||version>4)||kind<0||kind>2||reward<0||paymentState<0||paymentState>3||(timing.active&&!resultRolled)))throw std::runtime_error("invalid delegated mission");}
        void clear(){*this=DelegatedMissionState();}
    } delegatedMission;
    std::vector<DelegatedMissionState> delegatedMissions;

double currentGameHours=100;
namespace Ogre {struct Vector3 {float x;Vector3(float n=0):x(n){}float squaredDistance(const Vector3& b){return (x-b.x)*(x-b.x);}};struct Math{static float Sqrt(float n){return (float)sqrt(n);}};}
struct Character {Ogre::Vector3 pos;bool dead;Character():dead(false){}bool isDead(){return dead;}bool isInCombatMode(bool,bool){return false;}Ogre::Vector3 getPosition(){return pos;}std::string getName(){return "Soldat";}};
struct hand {Character* p;hand(Character* c=0):p(c){}bool isNull(){return !p;}Character* getCharacter(){return p;}};
struct Waiting {bool returning;};std::vector<Waiting> waitingHere;
struct Contract {int type;bool settlementPaid;std::string destination;Contract():type(0),settlementPaid(false),destination("Mastoc"){}}currentContract;
Character actor;Character* escort=&actor;hand escortHandle(&actor),missionFollowTarget;
Ogre::Vector3 destination(141200);
bool missionActive=false,missionPending=true,scientificReturning=false,caravanReturning=false,missionPaused=false,waitingForPlayer=false,scientificResearching=false,missionFollowing=false;
std::string originCity="Le Hub",destinationName="Mastoc";
int clicks=0,clickedSlot=-1;bool locked=false;

void trackerEntryClicked(MyGUI::WidgetPtr){++clicks;}
bool questPanelLocked(){return locked;}
void selectTrackerQuest(int slot,bool){clickedSlot=slot;}
void appendAllQuestItems();int syntheticCount=0;

bool gMercenarieEnglish=false;
std::string registerLanguage(const char* fr,const char* en){return gMercenarieEnglish?en:fr;}
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

#include "QuestTrackerView.h"
void appendBountyTrackerItem(){}void appendMailTrackerItems(){}
void appendAllQuestItems(){for(int i=0;i<syntheticCount;++i){currentContract.type=i%6;size_t old=questTrackerItems.size();appendEscortTrackerItem();if(questTrackerItems.size()>old){questTrackerItems.back().slot=i;questTrackerItems.back().location=currentContract.destination;}}}
std::string escaped(const std::string& s){std::string o;for(size_t i=0;i<s.size();++i){if(s[i]=='\n')o+="\\n";else if(s[i]=='\t')o+=' ';else o+=s[i];}return o;}

int main(int argc,char** argv){assert(argc>=7);int w=atoi(argv[1]),h=atoi(argv[2]),count=atoi(argv[4]);Loc::configure("Localization",argv[3]);gMercenarieEnglish=std::string(argv[3])=="en";
std::ifstream metrics(argv[6]);std::string name;unsigned int point;double advance;while(metrics>>name>>point>>advance)MyGUI::advances[name][point]=advance;
MyGUI::RenderManager::getInstance().v=MyGUI::IntSize(w,h);syntheticCount=count;currentGameHours=124;
for(int i=0;i<count;++i){DelegatedMissionState d;d.title=missionLabelV6(i%5);d.origin="Squin";d.destination="Bad Teeth";d.groupId="group"+std::string(i+1,'x');d.timing=DelegatedMissionTiming::start(DelegatedMissionTiming::ActivityMailDelivery,(48+i*12)*3,100);d.timing.activityType=(DelegatedMissionTiming::ActivityType)(1+i%5);d.title=missionLabelV6(QuestDelegatedProgress::missionType(d.timing.activityType));d.resultRolled=true;delegatedMissions.push_back(d);}
// Reverse the input to prove sorting never changes the saved mission order.
std::reverse(delegatedMissions.begin(),delegatedMissions.end());
createDynamicTracker();MyGUI::Window* original=trackerWindow;createDynamicTracker();assert(original==trackerWindow);trackerWindow->setVisible(true);refreshDynamicTracker();assert((int)questTrackerItems.size()==count);
for(int i=0;i<count;++i){assert(!questTrackerItems[i].delegated);assert(!questTrackerCards[i].bar->visible&&!questTrackerCards[i].percent->visible);}
if(count){questTrackerClick(questTrackerCards[count-1].row);assert(clicks==1&&clickedSlot==count-1);locked=true;questTrackerClick(questTrackerCards[0].row);assert(clicks==1);locked=false;}
size_t widgetCount=MyGUI::widgets.size();refreshDynamicTracker();assert(widgetCount==MyGUI::widgets.size());
questTrackerChooseTab(questTrackerTabs[1]);assert(trackerWindow==original&&questTrackerTab==1&&(int)questTrackerItems.size()==count);
for(int i=0;i<count;++i){assert(questTrackerItems[i].delegated&&questTrackerCards[i].state->text.empty());assert(questTrackerCards[i].bar->visible&&questTrackerCards[i].percent->visible);if(i)assert(questTrackerItems[i-1].deadline<=questTrackerItems[i].deadline);}
if(count){assert(questTrackerItems[0].percent==50);int previousClicks=clicks;questTrackerClick(questTrackerCards[0].row);assert(clicks==previousClicks);delegatedMissions.back().completed=true;delegatedMissions.back().timing.active=false;refreshDynamicTracker();assert((int)questTrackerItems.size()==count-1);delegatedMissions.back().completed=false;delegatedMissions.back().timing.active=true;}
currentGameHours=100;refreshDynamicTracker();if(count)assert(questTrackerItems[0].percent==0&&!questTrackerCards[0].fill->visible);
currentGameHours=112;refreshDynamicTracker();if(count)assert(questTrackerItems[0].percent==25);
currentGameHours=136;refreshDynamicTracker();if(count)assert(questTrackerItems[0].percent==75);
currentGameHours=148;refreshDynamicTracker();if(count)assert(questTrackerItems[0].percent==100);
currentGameHours=124;refreshDynamicTracker();
if(count>4){
 int low=questTrackerScroll->h-questTrackerScroll->canvasH;assert(low<0);questTrackerScroll->setViewOffset(MyGUI::IntPoint(0,low));
 questTrackerChooseTab(questTrackerTabs[0]);questTrackerChooseTab(questTrackerTabs[1]);assert(questTrackerScroll->getViewOffset().top==low);
 QuestTrackerCard& last=questTrackerCards[count-1];assert(last.row->y+last.row->h+low<=questTrackerScroll->y+questTrackerScroll->h);
 std::vector<DelegatedMissionState> saved=delegatedMissions;delegatedMissions.clear();refreshDynamicTracker();assert(questTrackerEmpty->visible&&questTrackerScroll->getViewOffset().top==0);
 delegatedMissions=saved;refreshDynamicTracker();
}
// Reset cached UI references like save/load; timing data is untouched.
mercenarieDestroyLiveWidget(trackerWindow);trackerWindow=0;resetQuestTrackerView();assert(questTrackerCards.empty()&&!questTrackerTabs[0]&&!questTrackerScroll);
createDynamicTracker();trackerWindow->setVisible(true);refreshDynamicTracker();questTrackerChooseTab(questTrackerTabs[1]);if(count)assert(questTrackerItems[0].percent==50);
// Production viewport rebuild preserves the active tab and keeps the frame on screen.
MyGUI::RenderManager::getInstance().v=MyGUI::IntSize(800,600);refreshDynamicTracker();assert(questTrackerTab==1&&trackerWindow->getVisible());
MyGUI::RenderManager::getInstance().v=MyGUI::IntSize(w,h);refreshDynamicTracker();
if(std::string(argv[5])=="player")questTrackerChooseTab(questTrackerTabs[0]);
assert(questTrackerEmpty->visible==(count==0));
// Geometry/text assertions exclude children intentionally clipped by the scroll viewport.
for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];if(!t->visible)continue;bool hidden=false;for(int ancestor=t->parent;ancestor>=0;ancestor=MyGUI::widgets[ancestor]->parent)if(!MyGUI::widgets[ancestor]->visible)hidden=true;if(hidden)continue;
if(!t->text.empty()){MyGUI::IntSize measured=t->getTextSize();if(measured.width>t->w+2||measured.height>t->h+2){std::cerr<<"Text overflow: "<<t->text<<" / "<<measured.width<<","<<measured.height<<" in "<<t->w<<","<<t->h;return 2;}}
if(t->parent>=0&&MyGUI::widgets[t->parent]!=questTrackerScroll){MyGUI::Widget* p=MyGUI::widgets[t->parent];if(t->x<p->x||t->y<p->y||t->x+t->w>p->x+p->w||t->y+t->h>p->y+p->h){std::cerr<<"Parent overflow: "<<t->kind<<" "<<t->text;return 3;}}
}
assert(trackerWindow->x>=0&&trackerWindow->y>=0&&trackerWindow->x+trackerWindow->w<=w&&trackerWindow->y+trackerWindow->h<=h);
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];std::cout<<t->id<<'\t'<<t->parent<<'\t'<<t->kind<<'\t'<<t->x<<'\t'<<t->y<<'\t'<<t->w<<'\t'<<t->h<<'\t'<<t->font<<'\t'<<t->color.r<<'\t'<<t->color.g<<'\t'<<t->color.b<<'\t'<<t->scroll<<'\t'<<t->selected<<'\t'<<escaped(t->text)<<'\t'<<t->texture<<'\t'<<t->fontName<<'\t'<<t->alpha<<'\t'<<t->crop.left<<'\t'<<t->crop.top<<'\t'<<t->crop.width<<'\t'<<t->crop.height<<'\t'<<t->align<<'\t'<<t->enabled<<'\t'<<t->visible<<'\t'<<t->canvasW<<'\t'<<t->canvasH<<'\t'<<t->hscroll<<'\n';}
}
