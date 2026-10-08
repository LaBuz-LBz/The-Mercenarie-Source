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
#include "ArtisanOrders.h"
#include "ArtisanLayout.h"
#include "RerollPopupLayout.h"
namespace MyGUI {
struct Colour{float r,g,b;Colour(float R=1,float G=1,float B=1):r(R),g(G),b(B){}};
struct IntPoint{int left,top;IntPoint(int x=0,int y=0):left(x),top(y){}};
struct IntSize{int width,height;IntSize(int w=1400,int h=940):width(w),height(h){}bool operator==(const IntSize& o)const{return width==o.width&&height==o.height;}};
struct IntCoord{int left,top,width,height;IntCoord(int x=0,int y=0,int w=0,int h=0):left(x),top(y),width(w),height(h){}};
struct Align{enum{Default=0,Left=0,Top=0,Center=1};};
struct Event{void operator+=(int){}};template<class T>int newDelegate(T){return 0;}
std::map<std::string,std::map<unsigned int,double> > advances;
struct Widget;std::vector<Widget*> widgets;
struct Widget{
 int x,y,w,h,font,id,parent,align,canvasW,canvasH;float alpha;bool scroll,selected,enabled,visible,hscroll;std::string kind,text,texture,fontName;Colour color;IntCoord crop;std::map<std::string,std::string> props;Event eventComboChangePosition,eventEditTextChange,eventMouseButtonClick;
 Widget():x(0),y(0),w(0),h(0),font(18),id(-1),parent(-1),align(0),canvasW(0),canvasH(0),alpha(1),scroll(false),selected(false),enabled(true),visible(true),hscroll(false),fontName("ArtisanBody18"){}
 template<class T>T* createWidget(const char* skin,int xx,int yy,int ww,int hh,int,const char* layer="",const char* name=""){if(ww<=0||hh<=0){std::cerr<<"Invalid "<<skin<<" "<<xx<<","<<yy<<","<<ww<<","<<hh<<"\n";abort();}T* t=new T;t->parent=id;t->id=(int)widgets.size();t->x=x+xx;t->y=y+yy;t->w=ww;t->h=hh;t->kind=skin;widgets.push_back(t);return t;}
 template<class T>T* createWidget(const char* skin,IntCoord r,int a,const char* layer,const char* name){return createWidget<T>(skin,r.left,r.top,r.width,r.height,a,layer,name);}
 void setCanvasAlign(int){}void setVisibleHScroll(bool b){hscroll=b;}void setCanvasSize(int a,int b){assert(a>0&&b>0);scroll=true;canvasW=a;canvasH=b;}void setViewOffset(IntPoint){}IntPoint getViewOffset(){return IntPoint();}
 void setPosition(int a,int b){int px=parent<0?0:widgets[parent]->x,py=parent<0?0:widgets[parent]->y;x=px+a;y=py+b;}void setSize(int a,int b){assert(a>0&&b>0);w=a;h=b;}int getHeight(){return h;}int getWidth(){return w;}int getTop(){return y;}int getFontHeight(){return font;}
 void setFontHeight(int f){font=f;}void setFontName(const std::string& s){fontName=s;}void setTextAlign(int a){align=a;}void setCaption(const std::string& s){text=s;}
 IntSize getTextSize(){double width=0,line=0;int lines=1;for(size_t i=0;i<text.size();){unsigned char c=text[i++];unsigned int point=c;int extra=0;if(c>=240){point=c&7;extra=3;}else if(c>=224){point=c&15;extra=2;}else if(c>=192){point=c&31;extra=1;}while(extra--&&i<text.size())point=(point<<6)|((unsigned char)text[i++]&63);if(point==10){width=std::max(width,line);line=0;++lines;}else{double adv=advances[fontName][point];line+=adv?adv:font*.55;}}return IntSize((int)std::ceil(std::max(width,line)),lines*(font+2));}
 void setAlpha(float a){alpha=a;}void setColour(Colour c){color=c;}void setTextColour(Colour c){color=c;}void setNeedMouseFocus(bool b){props["mouseOff"]=b?"0":"1";}void setInheritsPick(bool b){props["inheritsPick"]=b?"1":"0";}void setStateSelected(bool b){selected=b;}void setImageCoord(IntCoord c){crop=c;}void setImageTexture(const char* s){texture=s;}
 void setVisible(bool b){visible=b;}void setEnabled(bool b){enabled=b;}void setUserString(const char* k,const std::string& v){props[k]=v;}void setComboModeDrop(bool){}void addItem(const std::string& s){if(text.empty())text=s;}void setMaxListLength(int){}void setIndexSelected(int){}
};
struct TextBox:Widget{};struct Button:TextBox{};struct ImageBox:Widget{};struct ScrollView:Widget{};struct ComboBox:TextBox{};struct EditBox:TextBox{};
struct Gui:Widget{static Gui& getInstance(){static Gui x;return x;}int getEnumerator(){return 0;}};
struct RenderManager{IntSize v;static RenderManager& getInstance(){static RenderManager x;return x;}IntSize getViewSize(){return v;}};
struct ResourceManager{static ResourceManager& getInstance(){static ResourceManager x;return x;}bool isExist(const char*){return true;}void load(const char*){}};
}
namespace Ogre{struct ResourceGroupManager{static ResourceGroupManager& getSingleton(){static ResourceGroupManager r;return r;}bool resourceLocationExists(const char*,const char*){return true;}void addResourceLocation(const char*,const char*,const char*){}};}
namespace Loc{std::map<std::string,std::string> strings;struct Engine{std::string language;}e;Engine& engine(){return e;}const char* text(const char* k){return strings[k].c_str();}struct LanguagePack{bool official;LanguagePack():official(true){}};struct Registry{LanguagePack p;LanguagePack* find(const std::string&){return &p;}};Registry& registry(){static Registry r;return r;}}
namespace MercenarieNativeInput{void bind(MyGUI::Widget*){}}
namespace MercenarieFonts{void prepare(MyGUI::Widget*){}void caption(MyGUI::Widget* w,const std::string& s){w->text=s;}}
std::string mercenarieLocalize(const std::string& s){return s;}
MyGUI::Colour registerAmber(1,.61f,.14f),registerIvory(.88f,.86f,.80f);
MyGUI::Widget* registerSolid(MyGUI::Widget* p,int x,int y,int w,int h,MyGUI::Colour c){MyGUI::Widget* t=p->createWidget<MyGUI::Widget>("solid",x,y,w,h,0);t->color=c;return t;}
void popupBorder(MyGUI::Widget* p,int w,int h,MyGUI::Colour c){registerSolid(p,0,0,w,1,c);registerSolid(p,0,h-1,w,1,c);registerSolid(p,0,0,1,h,c);registerSolid(p,w-1,0,1,h,c);}
std::string artisanNumber(long long n){std::ostringstream s;s<<n;return s.str();}
#include "ArtisanTypography.h"
void artisanClick(MyGUI::Widget*){}
#include "artisan-preview-primitives.generated.h"
std::string artisanTime(double h){long n=(long)std::ceil(std::max(0.,h)*60);return (n>=1440?artisanNumber(n/1440)+" "+Loc::text("artisan.days")+" ":"")+artisanNumber(n/60%24)+" h"+(n%60?" "+artisanNumber(n%60)+" min":"");}
double artisanNow(){return 150;}
std::string artisanItemName(const ArtisanOrders::Line& l){return Loc::text(("fixture."+l.item).c_str());}
std::string artisanGradeName(const ArtisanOrders::Line& l){return Loc::text("artisan.grade.2");}
void artisanIcon(MyGUI::Widget* p,const ArtisanOrders::Line& l,int x,int y,int boxW=48,int boxH=60){MyGUI::Widget* b=p->createWidget<MyGUI::Widget>("native_icon",x,y,boxW,boxH,0);b->texture=l.item;}
struct ArtisanGrade{std::string name;};std::vector<ArtisanGrade> artisanWeaponGrades(3),artisanArmourGrades(3);
int guildLevel(){return 3;}void artisanGradeChanged(MyGUI::ComboBox*,size_t){}void artisanSearchChanged(MyGUI::EditBox*){}
struct GameData{int slot;};template<class T>struct lektor:std::vector<T>{};struct World{struct Data{void getDataOfType(lektor<GameData*>& a,int){static GameData d[2];d[0].slot=7;d[1].slot=8;a.push_back(&d[0]);a.push_back(&d[1]);}}gamedata;}world;World* ou=&world;
int artisanItemCategory(GameData* d){return d->slot;}
bool artisanContext(){return true;}void artisanClose(){}void artisanGrades(){}void artisanClearPreviews(){}void artisanFontTree(MyGUI::Widget*){}
bool mercenarieFindLiveWidget(int,MyGUI::Widget*){return false;}void mercenarieDestroyLiveWidget(MyGUI::Widget*){}
ArtisanOrders::Ledger artisanLedger;std::string artisanKey="smith",artisanName="Marchand Armures Ronin",artisanSearch,artisanNotice,artisanLanguage;
int artisanKind=0,artisanCategory=0,artisanPage=0,artisanDrawnPage=-1,artisanOrderFilter=0;unsigned long artisanDetailOrder=1;bool artisanRefresh=false,artisanResetScroll=false;
MyGUI::Widget* artisanWindow=0;MyGUI::Widget* artisanBody=0;MyGUI::Button* artisanBasketButton=0;MyGUI::ScrollView* artisanScroll=0;MyGUI::IntSize artisanViewSize;
std::map<size_t,MyGUI::TextBox*> artisanDetailLabels;std::vector<ArtisanOrders::Line> artisanShown;std::map<std::string,int> artisanRowGrades;
std::map<unsigned long,MyGUI::Widget*> artisanProgressWidgets;std::map<unsigned long,MyGUI::TextBox*> artisanTimeWidgets,artisanPercentWidgets;
std::vector<ArtisanOrders::Line> catalogue;
void artisanCatalogue(){artisanShown=catalogue;}
#include "artisan-preview-view.generated.h"
std::string escaped(const std::string& s){std::string o;for(size_t i=0;i<s.size();++i){if(s[i]=='\n')o+="\\n";else if(s[i]=='\t')o+=' ';else o+=s[i];}return o;}
bool inputReachable(MyGUI::Widget* w){
 // MyGUI_WidgetInput.h: false NeedMouseFocus gates descendants unless
 // InheritsPick is true. Defaults are NeedMouseFocus=true, InheritsPick=false.
 if(w->props["mouseOff"]=="1")return false;
 for(int p=w->parent;p>=0;p=MyGUI::widgets[p]->parent){MyGUI::Widget* a=MyGUI::widgets[p];if(!a->visible||!a->enabled||(a->props["mouseOff"]=="1"&&a->props["inheritsPick"]!="1"))return false;}
 return true;
}
void verifyArtisanPicking(){
 int actions=0,descendants=0;
 assert(artisanBody->props["mouseOff"]=="1");
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* w=MyGUI::widgets[i];
  if(w->props["action"].empty()&&w->kind!="ArtisanCombo"&&w->kind!="Kenshi_EditBox")continue;
  if(!w->enabled)continue;
  assert(inputReachable(w));++actions;
  bool child=false;for(int p=w->parent;p>=0;p=MyGUI::widgets[p]->parent)if(p==artisanBody->id)child=true;
  if(child){std::string old=artisanBody->props["inheritsPick"];artisanBody->setInheritsPick(false);assert(!inputReachable(w));artisanBody->props["inheritsPick"]=old;++descendants;}
 }
 assert(actions>0&&descendants>0);
 std::cerr<<"PASS Artisan picking: page "<<artisanPage<<", "<<actions<<" controls reachable; "<<descendants<<" reproduce the old blocked subtree\n";
}
int main(int argc,char** argv){
 assert(argc>=6);int w=atoi(argv[1]),h=atoi(argv[2]);artisanPage=atoi(argv[3]);std::ifstream lang(argv[4]);std::string line;while(std::getline(lang,line)){size_t p=line.find('\t');if(p!=std::string::npos)Loc::strings[line.substr(0,p)]=line.substr(p+1);}Loc::e.language=Loc::strings["_language"];
 std::ifstream metrics(argv[5]);std::string name;unsigned int point;double advance;while(metrics>>name>>point>>advance)MyGUI::advances[name][point]=advance;
 MyGUI::RenderManager::getInstance().v=MyGUI::IntSize(w,h);for(size_t i=0;i<artisanArmourGrades.size();++i)artisanArmourGrades[i].name=Loc::text("artisan.grade.2");
 const char* ids[]={"holy_mercenary_plastron_icon.png","holy_mercenary_paladin_helmet_icon.png","holy_mercenary_samurai_pants_icon.png","guild_chainmail_icon.png","guild_service_vest_icon.png","holy_mercenary_plated_boots_icon.png","guild_martial_bindings_icon.png","guild_mask_black_icon.png"};
 for(int i=0;i<8;++i){ArtisanOrders::Line l;l.item=ids[i];l.quality="armour:40";l.base=i==0?5200:i==1?3600:i==2?980:1200;l.unit=ArtisanOrders::price(l.base);l.quantity=l.remaining=i==2?5:10;l.hours=ArtisanOrders::series(l.base,l.quantity);catalogue.push_back(l);if(i<3)artisanLedger.baskets[artisanKey].push_back(l);}
 for(int i=0;i<4;++i){ArtisanOrders::Order o;o.id=i+1;o.artisan=artisanKey;o.lines=artisanLedger.baskets[artisanKey];o.paid=ArtisanOrders::total(o.lines);o.start=i==0?0:i==1?ArtisanOrders::duration(o.lines):0;o.end=o.start+ArtisanOrders::duration(o.lines);o.status=i==0?ArtisanOrders::Making:i==1?ArtisanOrders::Waiting:i==2?ArtisanOrders::Ready:ArtisanOrders::Lost;artisanLedger.orders.push_back(o);}
 if(argc>6){std::string state=argv[6];if(state=="empty"){artisanLedger=ArtisanOrders::Ledger();catalogue.clear();artisanDetailOrder=0;}if(state=="complete")artisanOrderFilter=1;if(state=="all")artisanOrderFilter=2;}
 artisanDraw();
 verifyArtisanPicking();
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];std::cout<<t->id<<'\t'<<t->parent<<'\t'<<t->kind<<'\t'<<t->x<<'\t'<<t->y<<'\t'<<t->w<<'\t'<<t->h<<'\t'<<t->font<<'\t'<<t->color.r<<'\t'<<t->color.g<<'\t'<<t->color.b<<'\t'<<t->scroll<<'\t'<<t->selected<<'\t'<<escaped(t->text)<<'\t'<<t->texture<<'\t'<<t->fontName<<'\t'<<t->alpha<<'\t'<<t->crop.left<<'\t'<<t->crop.top<<'\t'<<t->crop.width<<'\t'<<t->crop.height<<'\t'<<t->align<<'\t'<<t->enabled<<'\t'<<t->visible<<'\t'<<t->canvasW<<'\t'<<t->canvasH<<'\t'<<t->hscroll<<'\n';}
}
