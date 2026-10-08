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
struct MouseButton{enum{Left=0};int value;MouseButton(int v=0):value(v){}bool operator!=(int v){return value!=v;}};
struct Align{enum{Default=0,Left=0,Top=0,Stretch=0,Center=1,VCenter=2};};
struct Event{void operator+=(int){}};template<class T>int newDelegate(T){return 0;}
std::map<std::string,std::map<unsigned int,double> > advances;
struct Widget;std::vector<Widget*> widgets;
struct Widget{
 IntPoint viewOffset;int x,y,w,h,font,id,parent,align,canvasW,canvasH;float alpha;bool scroll,selected,enabled,visible,hscroll;std::string kind,text,texture,fontName;Colour color;IntCoord crop;std::map<std::string,std::string> props;Event eventMouseWheel,eventMouseDrag,eventWindowButtonPressed,eventComboChangePosition,eventEditTextChange,eventMouseButtonClick,eventMouseSetFocus,eventMouseLostFocus,eventMouseButtonPressed,eventMouseButtonReleased;
 Widget():x(0),y(0),w(0),h(0),font(18),id(-1),parent(-1),align(0),canvasW(0),canvasH(0),alpha(1),scroll(false),selected(false),enabled(true),visible(true),hscroll(false),fontName("ArtisanBody18"){}
 template<class T>T* createWidget(const char* skin,int xx,int yy,int ww,int hh,int,const char* layer="",const char* name=""){if(ww<=0||hh<=0){std::cerr<<"Invalid "<<skin<<" "<<xx<<","<<yy<<","<<ww<<","<<hh<<"\n";abort();}T* t=new T;t->parent=id;t->id=(int)widgets.size();t->x=x+xx;t->y=y+yy;t->w=ww;t->h=hh;t->kind=skin;widgets.push_back(t);return t;}
 template<class T>T* createWidget(const char* skin,IntCoord r,int a,const char* layer,const char* name){return createWidget<T>(skin,r.left,r.top,r.width,r.height,a,layer,name);}
 template<class T>T* castType(){return (T*)this;}size_t getChildCount(){size_t n=0;for(size_t i=0;i<widgets.size();++i)if(widgets[i]->parent==id)++n;return n;}
Widget* getChildAt(size_t n){for(size_t i=0;i<widgets.size();++i)if(widgets[i]->parent==id){if(n==0)return widgets[i];--n;}return 0;}
std::string getUserString(const char* k){return props[k];}
void setCanvasAlign(int){}void setVisibleVScroll(bool b){props["vscroll"]=b?"1":"0";}void setVisibleHScroll(bool b){hscroll=b;}void setCanvasSize(int a,int b){assert(a>0&&b>0);scroll=true;canvasW=a;canvasH=b;}void setViewOffset(IntPoint p){viewOffset=p;}IntPoint getViewOffset(){return viewOffset;}
 int getAbsoluteLeft(){return x;}int getAbsoluteTop(){return y;}void setCoord(int a,int b,int c,int d){setPosition(a,b);setSize(c,d);}void setPosition(int a,int b){int px=parent<0?0:widgets[parent]->x,py=parent<0?0:widgets[parent]->y;int dx=px+a-x,dy=py+b-y;x+=dx;y+=dy;for(size_t i=0;i<widgets.size();++i){int ancestor=widgets[i]->parent;while(ancestor>=0&&ancestor!=id)ancestor=widgets[ancestor]->parent;if(ancestor==id){widgets[i]->x+=dx;widgets[i]->y+=dy;}}}void setSize(int a,int b){assert(a>0&&b>0);w=a;h=b;}int getHeight(){return h;}int getWidth(){return w;}int getTop(){return y;}int getFontHeight(){return font;}
 void setFontHeight(int f){font=f;}void setFontName(const std::string& s){fontName=s;}void setTextAlign(int a){align=a;}void setCaption(const std::string& s){text=s;}
 IntSize getTextSize(){double width=0,line=0;int lines=1;for(size_t i=0;i<text.size();){unsigned char c=text[i++];unsigned int point=c;int extra=0;if(c>=240){point=c&7;extra=3;}else if(c>=224){point=c&15;extra=2;}else if(c>=192){point=c&31;extra=1;}while(extra--&&i<text.size())point=(point<<6)|((unsigned char)text[i++]&63);if(point==10){width=std::max(width,line);line=0;++lines;}else{double adv=advances[fontName][point];line+=adv?adv:font*.55;}}return IntSize((int)std::ceil(std::max(width,line)),lines*(font+2));}
 void setAlpha(float a){alpha=a;}void setColour(Colour c){color=c;}void setTextColour(Colour c){color=c;}void setNeedMouseFocus(bool b){props["mouseOff"]=b?"0":"1";}void setInheritsPick(bool b){props["inheritsPick"]=b?"1":"0";}void setStateSelected(bool b){selected=b;}void setImageCoord(IntCoord c){crop=c;}void setImageTexture(const std::string& s){texture=s;}void setImageInfo(const std::string& s,IntCoord c,IntSize){texture=s;crop=c;}void setImageIndex(int){}
 int getLeft(){return x;}bool getVisible(){return visible;}Widget* getClientWidget(){return this;}void setVisible(bool b){visible=b;}void setEnabled(bool b){enabled=b;}void setUserString(const char* k,const std::string& v){props[k]=v;}void setComboModeDrop(bool){}void addItem(const std::string& s){if(text.empty())text=s;}void setMaxListLength(int){}void setIndexSelected(int){}
};
struct Window:Widget{};typedef Widget* WidgetPtr;struct TextBox:Widget{};struct Button:TextBox{};struct ImageBox:Widget{};struct ScrollView:Widget{};struct ComboBox:TextBox{};struct EditBox:TextBox{struct Caption{std::string s;std::string asUTF8(){return s;}};Caption getCaption(){Caption c;c.s=text;return c;}void setEditMultiLine(bool){}};
struct Gui:Widget{static Gui& getInstance(){static Gui x;return x;}static Gui* getInstancePtr(){return &getInstance();}void destroyWidget(Widget* root){root->visible=false;for(size_t i=0;i<widgets.size();++i){int a=widgets[i]->parent;while(a>=0&&a!=root->id)a=widgets[a]->parent;if(a==root->id)widgets[i]->visible=false;}}int getEnumerator(){return 0;}};
struct RenderManager{IntSize v;static RenderManager& getInstance(){static RenderManager x;return x;}IntSize getViewSize(){return v;}};
struct InputManager{static InputManager& getInstance(){static InputManager i;return i;}IntPoint getMousePosition(){return IntPoint(400,300);}};
struct IndexImage{std::string name;std::vector<IntPoint> frames;};struct GroupImage{std::string name,texture;IntSize size;std::vector<IndexImage> indexes;};
std::vector<GroupImage> nativeGroups;
struct EnumeratorGroupImage{size_t at;EnumeratorGroupImage():at(0){}bool next(){return at++<nativeGroups.size();}const GroupImage& current(){return nativeGroups[at-1];}};
struct IResource{template<class T>T* castType(bool){return (T*)this;}};struct ResourceImageSet:IResource{EnumeratorGroupImage getEnumerator(){return EnumeratorGroupImage();}};
struct ResourceManager{IResource* getByName(const char*,bool){static ResourceImageSet r;return &r;}static ResourceManager& getInstance(){static ResourceManager x;return x;}bool isExist(const char*){return true;}void load(const char*){}};
}
namespace Ogre{struct ResourceGroupManager{static ResourceGroupManager& getSingleton(){static ResourceGroupManager r;return r;}bool resourceLocationExists(const char*,const char*){return true;}void addResourceLocation(const char*,const char*,const char*){}};}


#include "Localization.h"
namespace MercenarieFonts{void caption(MyGUI::Widget* w,const std::string& s){w->setCaption(s);}}
namespace MercenarieNativeInput{void bind(MyGUI::Widget*){}void wheel(MyGUI::Widget*,int){}}
MyGUI::Window* mercenarieAutopilotWindow=0;MyGUI::Widget* mercenarieLauncherMenu=0;
void closeMercenarieInterface(MyGUI::Window* w,const std::string&){w->setVisible(false);}
#include "quest-launcher-v9.generated.h"
namespace Ogre {struct Vector3 {float x,y,z;Vector3(float a=0,float b=0,float c=0):x(a),y(b),z(c){}float squaredDistance(const Vector3& p)const{return (x-p.x)*(x-p.x)+(y-p.y)*(y-p.y)+(z-p.z)*(z-p.z);}};struct Math{static float Sqrt(float n){return std::sqrt(n);}};}
struct hand {std::string id;bool isNull()const{return id.empty();}std::string toString()const{return id;}bool operator!=(const hand& h)const{return id!=h.id;}bool operator==(const hand& h)const{return id==h.id;}};
struct GameData {std::string stringID;};struct Faction {std::string name;GameData* data;Faction():data(0){}};
struct RootObject{virtual ~RootObject(){}};
enum {TOWN_NEST,TOWN_OUTPOST,TOWN_TOWN,TOWN_VILLAGE,TOWN_RUINS,TOWN_SLAVE_CAMP,TOWN_MILITARY,TOWN_PRISON,TOWN_NEST_MARKER,TOWN_POI,TOWN_NULL};
struct TownBase:RootObject {bool discovered;GameData data;std::string name,marker;hand handle;Ogre::Vector3 pos;Faction* owner;int townType;
 TownBase():discovered(true),owner(0),townType(TOWN_TOWN){}bool isDiscovered(){return discovered;}GameData* getGameData(){return &data;}std::string getName(){return name;}std::string getMapMarker(){return marker;}hand getHandle(){return handle;}const Ogre::Vector3& getPosition(){return pos;}Faction* getFaction(){return owner;}};
template<class T>struct lektor:std::vector<T>{};
struct TownList{lektor<RootObject*> towns,nests;lektor<RootObject*>& getAllTowns(){return towns;}};
struct iVector2{int x,y;iVector2(int a,int b):x(a),y(b){}};struct Zone {iVector2 getMapSector(const Ogre::Vector3& v){return iVector2((int)(v.x/1000),(int)(v.z/1000));}};
struct Movement{Ogre::Vector3 destination;float preference;int roadCalls,halts;Movement():preference(0),roadCalls(0),halts(0){}void setRoadPreference(float p){preference=p;}void setRoadDestination(const Ogre::Vector3& v){destination=v;++roadCalls;}Ogre::Vector3 getDestination(){return destination;}void halt(){++halts;}};
enum {MOVE_CUS_ORDERED=9};
struct Character{hand handle;Faction* faction;std::string name;bool selected,dead,carried;int orders,removals;Movement movement;Ogre::Vector3 pos;
 Character():faction(0),selected(false),dead(false),carried(false),orders(0),removals(0){}Faction* getFaction(){return faction;}hand getHandle(){return handle;}bool isDead(){return dead;}bool isBeingCarried(){return carried;}std::string getName(){return name;}const Ogre::Vector3& getPosition(){return pos;}Movement* getMovement(){return &movement;}void addOrder(int,int order,int,bool,bool,const Ogre::Vector3& dest){assert(order==MOVE_CUS_ORDERED);++orders;movement.destination=dest;}void removeJob(int order){assert(order==MOVE_CUS_ORDERED);++removals;}};
struct Player{std::vector<Character*> playerCharacters;Faction faction;Faction* getFaction(){return &faction;}bool isObjectSelected(Character* c){return c->selected;}};
struct World{Player* player;Zone* zoneMgr;void showPlayerAMessage(const char*,bool){}};
struct Shared{TownList* townList;};World* ou=0;Shared* shou=0;bool gMercenarieEnglish=false;

#include "PricingRoads.h"
#include "PlayerAutopilot.h"

#include "tests/autopilot-v9-cases.h"
