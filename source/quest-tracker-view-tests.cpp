#include "tests/LocalizedCaptionStub.h"
// Compile and exercise the actual tracker view; native MyGUI is mocked.
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cmath>
#include "Localization.h"
#include "UI/ClientOptions.h"

bool testValidGuildKey(int key){return key==36||key==59;}
void testClientOptions(){
 ClientOptions::Settings settings(true,36);
 std::istringstream input("ignored=1\nshowClientTracker=0\nguildManagementHotkey=999\nguildManagementHotkey=59\n");
 ClientOptions::read(input,settings,testValidGuildKey);
 assert(!settings.showClientTracker&&settings.guildManagementHotkey==59);
 std::ostringstream output;ClientOptions::write(output,settings.showClientTracker,settings.guildManagementHotkey);
 assert(output.str()=="showClientTracker=0\nguildManagementHotkey=59\n");
 std::istringstream legacy("showClientTracker=1\nguildManagementHotkey=not-a-number\n");
 ClientOptions::read(legacy,settings,testValidGuildKey);
 assert(settings.showClientTracker&&settings.guildManagementHotkey==59);
}
namespace MyGUI {
struct Colour {float r,g,b;Colour(float x=1,float y=1,float z=1):r(x),g(y),b(z){}};
struct IntCoord {int left,top,width,height;IntCoord(int x=0,int y=0,int w=0,int h=0):left(x),top(y),width(w),height(h){}};
struct IntSize {int width,height;IntSize(int w=1920,int h=1080):width(w),height(h){}};
struct IntPoint {int left,top;IntPoint(int x=0,int y=0):left(x),top(y){}};
struct Align {enum {Default=0,Left=0,Top=0,Right=1,Center=2};};
struct Event {void operator+=(int){}};template<class T>int newDelegate(T){return 0;}
struct Widget;std::vector<Widget*> widgets;
struct Widget {
 Widget* parent;int x,y,w,h,font,align;std::string kind,text,texture;Colour color;IntCoord crop;bool visible,focus;Event eventMouseButtonClick,eventWindowButtonPressed;
 Widget():parent(0),x(0),y(0),w(0),h(0),font(16),align(0),visible(true),focus(false){}
 template<class T>T* createWidget(const char* skin,int xx,int yy,int ww,int hh,int,const char* layer="",const char* name=""){T* t=new T;t->parent=this;t->kind=skin;t->x=xx;t->y=yy;t->w=ww;t->h=hh;widgets.push_back(t);return t;}
 void setCaption(const std::string& s){text=s;}void setFontHeight(int n){font=n;}void setTextColour(Colour c){color=c;}void setTextAlign(int n){align=n;}void setNeedMouseFocus(bool b){focus=b;}void setColour(Colour c){color=c;}void setImageTexture(const char* s){texture=s;}void setImageCoord(IntCoord r){crop=r;}void setVisible(bool b){visible=b;}
 int getWidth(){return w;}int getHeight(){return h;}int getTop(){return y;}bool getVisible(){return visible;}Widget* getClientWidget(){return this;}
 void setSize(int ww,int hh){w=ww;h=hh;}void setPosition(int xx,int yy){x=xx;y=yy;}int ax(){return x+(parent?parent->ax():0);}int ay(){return y+(parent?parent->ay():0);}bool shown(){return visible&&(!parent||parent->shown());}
};
struct TextBox:Widget{};struct ImageBox:Widget{};struct Button:Widget{};struct Window:Widget{};typedef Widget* WidgetPtr;
struct ScrollView:Widget {bool vertical;IntPoint offset;IntSize canvas;void setVisibleHScroll(bool){}void setCanvasAlign(int){}void setVisibleVScroll(bool b){vertical=b;}void setCanvasSize(int w,int h){canvas=IntSize(w,h);}IntPoint getViewOffset(){return offset;}void setViewOffset(IntPoint p){offset=p;}};
struct Gui:Widget {static Gui& getInstance(){static Gui g;return g;}static Gui* getInstancePtr(){return &getInstance();}};
struct RenderManager {IntSize view;static RenderManager& getInstance(){static RenderManager r;return r;}const IntSize& getViewSize(){return view;}};
struct ResourceManager {static ResourceManager& getInstance(){static ResourceManager r;return r;}void load(const char*){}};
}
bool gMercenarieEnglish=false;
std::string registerLanguage(const char* fr,const char* en){return gMercenarieEnglish?en:fr;}
std::string registerNumber(int n){std::ostringstream s;s<<n;return s.str();}
std::string mercenarieLocalize(const std::string& s){return s;}
void fitRegisterText(MyGUI::TextBox* t,int n){t->font=n;while(t->font>10&&t->text.size()*t->font*.52>t->w)--t->font;}
#include "quest-test-visuals.generated.h"
#include "quest-test-launcher.generated.h"
namespace Ogre {struct Vector3 {float x;Vector3(float n=0):x(n){}float squaredDistance(const Vector3& b){return (x-b.x)*(x-b.x);}};struct Math{static float Sqrt(float n){return (float)sqrt(n);}};}
struct Character {Ogre::Vector3 pos;bool dead;Character():dead(false){}bool isDead(){return dead;}bool isInCombatMode(bool,bool){return false;}Ogre::Vector3 getPosition(){return pos;}std::string getName(){return "Soldat";}};
struct hand {Character* p;hand(Character* c=0):p(c){}bool isNull(){return !p;}Character* getCharacter(){return p;}};
struct Waiting {bool returning;};std::vector<Waiting> waitingHere;
struct Contract {int type;bool settlementPaid;std::string destination;Contract():type(0),settlementPaid(false),destination("Mastoc"){}}currentContract;
Character actor;Character* escort=&actor;hand escortHandle(&actor),missionFollowTarget;
Ogre::Vector3 destination(141200);
bool missionActive=false,missionPending=true,scientificReturning=false,caravanReturning=false,missionPaused=false,waitingForPlayer=false,scientificResearching=false,missionFollowing=false;
std::string originCity="Le Hub",destinationName="Mastoc";
MyGUI::Window* trackerWindow=0;int clicks=0,clickedSlot=-1;bool locked=false;
void closeMercenarieInterface(MyGUI::Window*,const std::string&){}
void trackerEntryClicked(MyGUI::WidgetPtr){++clicks;}
bool questPanelLocked(){return locked;}
void selectTrackerQuest(int slot,bool){clickedSlot=slot;}
void appendAllQuestItems();int syntheticCount=0;
#include "tests/NativeInputLayoutStub.h"
#include "QuestTrackerView.h"
void appendBountyTrackerItem(){}
void appendMailTrackerItems(){}
void appendAllQuestItems(){for(int i=0;i<syntheticCount;++i){currentContract.type=i%6;size_t old=questTrackerItems.size();appendEscortTrackerItem();if(questTrackerItems.size()>old)questTrackerItems.back().slot=i;}}
void dump(int count){std::ostringstream path;path<<"tracker-"<<(gMercenarieEnglish?"EN":"FR")<<"-"<<count<<".tsv";std::ofstream out(path.str().c_str());
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];if(!t->shown())continue;bool clipped=false;for(MyGUI::Widget* p=t->parent;p;p=p->parent)if(p==questTrackerScroll&&(t->ay()+t->h>p->ay()+p->h||t->ay()<p->ay()))clipped=true;if(clipped)continue;
 std::string text=t->text;std::replace(text.begin(),text.end(),'\n','~');out<<t->kind<<'\t'<<t->ax()-trackerWindow->ax()<<'\t'<<t->ay()-trackerWindow->ay()<<'\t'<<t->w<<'\t'<<t->h<<'\t'<<t->font<<'\t'<<t->align<<'\t'<<(int)(t->color.r*255)<<'\t'<<(int)(t->color.g*255)<<'\t'<<(int)(t->color.b*255)<<'\t'<<t->texture<<'\t'<<t->crop.left<<'\t'<<text<<'\n';}}
int main(){testClientOptions();Loc::configure("Localization","fr");createDynamicTracker();trackerWindow->setVisible(true);
 for(int lang=0;lang<2;++lang){gMercenarieEnglish=lang!=0;Loc::select(lang?"en":"fr");
  for(int n=0;n<10;++n){syntheticCount=n;refreshDynamicTracker();assert(questTrackerItems.size()==(size_t)n);assert(trackerWindow->h==(n==0?200:108+104*std::min(n,5)));assert(questTrackerScroll->vertical==(n>5));assert(questTrackerEmpty->visible==(n==0));assert(questTrackerCount->text==QuestTrackerText::count(n,gMercenarieEnglish));
   for(int i=0;i<n;++i){QuestTrackerCard& c=questTrackerCards[i];assert(c.row->visible&&c.row->y==i*104);assert(c.state->y>c.objective->y);assert(c.objective->x+c.objective->w<c.location->x);assert(c.location->x+c.location->w<c.distance->x);assert(c.distance->x+c.distance->w<c.row->w);assert(c.type->color.r==missionColourV6(i%6).r);assert(c.icon->crop.left==missionVisualV6(i%6).slot*96);assert(c.distance->text==(lang?"141.2 km":"141,2 km"));}
   assert(questTrackerTitle->text==(lang?"QUEST TRACKER":"SUIVI DES QU\xC3\x8ATES"));assert(questTrackerEmpty->text.find(lang?"NO ACTIVE QUEST":"AUCUNE")!=std::string::npos);
   if(n==0||n==1||n==2||n==3||n==5||n==6)dump(n);
  }
  questTrackerScroll->setViewOffset(MyGUI::IntPoint(0,-416));
  for(int n=5;n>=0;--n){syntheticCount=n;refreshDynamicTracker();assert(trackerWindow->h==(n==0?200:108+104*n));assert(questTrackerScroll->offset.top==0);for(size_t i=n;i<questTrackerCards.size();++i)assert(!questTrackerCards[i].row->visible);}
 }
 Loc::select("fr");assert(QuestTrackerText::count(0,false)=="0 qu\xC3\xAAte active");assert(QuestTrackerText::count(1,false)=="1 qu\xC3\xAAte active");assert(QuestTrackerText::count(2,false)=="2 qu\xC3\xAAtes actives");Loc::select("en");assert(QuestTrackerText::count(0,true)=="0 active quests");assert(QuestTrackerText::count(1,true)=="1 active quest");assert(QuestTrackerText::count(2,true)=="2 active quests");
 syntheticCount=3;refreshDynamicTracker();questTrackerClick(questTrackerCards[2].row);assert(clicks==1&&clickedSlot==2);locked=true;questTrackerClick(questTrackerCards[0].row);assert(clicks==1);locked=false;
 for(int outcome=0;outcome<3;++outcome){missionActive=missionPending=false;refreshDynamicTracker();assert(questTrackerItems.empty()&&trackerWindow->h==200);}missionPending=true;
 actor.dead=true;refreshDynamicTracker();assert(questTrackerItems.empty());actor.dead=false;escortHandle.p=0;refreshDynamicTracker();assert(questTrackerItems.size()==3&&!questTrackerItems[0].clickable&&questTrackerItems[0].distance.empty()&&!questTrackerCards[0].location->visible);escortHandle.p=&actor;
 MyGUI::RenderManager::getInstance().view=MyGUI::IntSize(1024,600);syntheticCount=5;refreshDynamicTracker();assert(questTrackerScroll->vertical&&trackerWindow->h<600);
 std::cout<<"PASS: actual QuestTrackerView FR/EN: 0/1/2/3/5/6/9 cards, 5-to-0 shrink, scrollbar and offset reset, per-card information bounds, official icons/colors, distance, plurals, unavailable/dead actors, callbacks, small screen\n";
}
