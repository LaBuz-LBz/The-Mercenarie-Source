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
virtual ~Widget(){}template<class T>T* castType(bool){return dynamic_cast<T*>(this);}
 int x,y,w,h,font,id,parent,align,canvasW,canvasH;float alpha;bool scroll,selected,enabled,visible,hscroll;std::string kind,text,texture,fontName;Colour color;IntCoord crop;std::map<std::string,std::string> props;Event eventComboChangePosition,eventEditTextChange,eventMouseButtonClick,eventMouseSetFocus,eventMouseLostFocus,eventMouseButtonPressed,eventMouseButtonReleased,eventKeyButtonPressed,eventKeySetFocus,eventKeyLostFocus;
 Widget():x(0),y(0),w(0),h(0),font(18),id(-1),parent(-1),align(0),canvasW(0),canvasH(0),alpha(1),scroll(false),selected(false),enabled(true),visible(true),hscroll(false),fontName("ArtisanBody18"){}
 template<class T>T* createWidget(const char* skin,int xx,int yy,int ww,int hh,int,const char* layer="",const char* name=""){if(ww<=0||hh<=0){std::cerr<<"Invalid "<<skin<<" "<<xx<<","<<yy<<","<<ww<<","<<hh<<"\n";abort();}T* t=new T;t->parent=id;t->id=(int)widgets.size();t->x=x+xx;t->y=y+yy;t->w=ww;t->h=hh;t->kind=skin;widgets.push_back(t);return t;}
 template<class T>T* createWidget(const char* skin,IntCoord r,int a,const char* layer,const char* name){return createWidget<T>(skin,r.left,r.top,r.width,r.height,a,layer,name);}
 size_t getChildCount(){size_t n=0;for(size_t i=0;i<widgets.size();++i)if(widgets[i]->parent==id)++n;return n;}
Widget* getChildAt(size_t n){for(size_t i=0;i<widgets.size();++i)if(widgets[i]->parent==id){if(n==0)return widgets[i];--n;}return 0;}
std::string getUserString(const char* k){return props[k];}
void setCanvasAlign(int){}void setVisibleHScroll(bool b){hscroll=b;}void setCanvasSize(int a,int b){assert(a>0&&b>0);scroll=true;canvasW=a;canvasH=b;}void setViewOffset(IntPoint){}IntPoint getViewOffset(){return IntPoint();}
 void setPosition(int a,int b){int px=parent<0?0:widgets[parent]->x,py=parent<0?0:widgets[parent]->y;x=px+a;y=py+b;}void setSize(int a,int b){assert(a>0&&b>0);w=a;h=b;}int getHeight(){return h;}int getWidth(){return w;}int getTop(){return parent<0?y:y-widgets[parent]->y;}int getFontHeight(){return font;}
 void setFontHeight(int f){font=f;}void setFontName(const std::string& s){fontName=s;}void setTextAlign(int a){align=a;}void setCaption(const std::string& s){text=s;}
 IntSize getTextSize(){double width=0,line=0;int lines=1;for(size_t i=0;i<text.size();){unsigned char c=text[i++];unsigned int point=c;int extra=0;if(c>=240){point=c&7;extra=3;}else if(c>=224){point=c&15;extra=2;}else if(c>=192){point=c&31;extra=1;}while(extra--&&i<text.size())point=(point<<6)|((unsigned char)text[i++]&63);if(point==10){width=std::max(width,line);line=0;++lines;}else{double adv=advances[fontName][point];line+=adv?adv:font*.55;}}return IntSize((int)std::ceil(std::max(width,line)),lines*(font+2));}
 void setAlpha(float a){alpha=a;}void setColour(Colour c){color=c;}void setTextColour(Colour c){color=c;}void setNeedMouseFocus(bool b){props["mouseOff"]=b?"0":"1";}void setInheritsPick(bool b){props["inheritsPick"]=b?"1":"0";}void setStateSelected(bool b){selected=b;}void setImageCoord(IntCoord c){crop=c;}void setImageTexture(const char* s){texture=s;}
 void setVisible(bool b){visible=b;}void setEnabled(bool b){enabled=b;}void setUserString(const char* k,const std::string& v){props[k]=v;}void setComboModeDrop(bool){}void addItem(const std::string& s){if(text.empty())text=s;}void setMaxListLength(int){}void setIndexSelected(int){}
};
struct TextBox:Widget{};struct Button:TextBox{};struct ImageBox:Widget{};struct ScrollView:Widget{};struct ComboBox:TextBox{};struct EditBox:TextBox{void setMaxTextLength(int n){std::ostringstream s;s<<n;props["maxLength"]=s.str();}void setEditMultiLine(bool b){props["multiline"]=b?"1":"0";}void setEditWordWrap(bool b){props["wrap"]=b?"1":"0";}};
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

#include "GuildBuildingTypes.h"
void gbCancel(MyGUI::Widget*){}void gbSelectType(MyGUI::Widget*){}
#include "GuildBuildingPickerView.h"
MyGUI::EditBox* guildNameEdit=0;MyGUI::Button* guildNameConfirm=0;void gbNameKey(){}void confirmGuildHouseName(MyGUI::Widget*){}
#include "GuildOfficeNameView.h"
void gbRename(MyGUI::Widget*){}void gbActivate(MyGUI::Widget*){}void gbAskAbandon(MyGUI::Widget*){}
#include "GuildOfficeManageView.h"
void gbConfirmAbandon(MyGUI::Widget*){}
#include "GuildOfficeAbandonView.h"
std::string escaped(const std::string& s){std::string o;for(size_t i=0;i<s.size();++i){if(s[i]=='\n')o+="\\n";else if(s[i]=='\t')o+=' ';else o+=s[i];}return o;}
int main(int argc,char** argv){assert(argc>=6);int w=atoi(argv[1]),h=atoi(argv[2]);std::ifstream lang(argv[3]);std::string line;while(std::getline(lang,line)){size_t p=line.find('\t');if(p!=std::string::npos){std::string value=line.substr(p+1);size_t n=0;while((n=value.find("\\n",n))!=std::string::npos){value.replace(n,2,"\n");++n;}Loc::strings[line.substr(0,p)]=value;}}Loc::e.language=Loc::strings["_language"];
 std::ifstream metrics(argv[4]);std::string name;unsigned int point;double advance;while(metrics>>name>>point>>advance)MyGUI::advances[name][point]=advance;
 MyGUI::RenderManager::getInstance().v=MyGUI::IntSize(w,h);GuildAbandonLayout l(w,h);MyGUI::Widget* p=MyGUI::Gui::getInstance().createWidget<MyGUI::Widget>("PanelEmpty",(w-l.w)/2,(h-l.h)/2,l.w,l.h,0);gbaDraw(p,l);
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];assert(t->x>=p->x&&t->y>=p->y&&t->x+t->w<=p->x+p->w+2&&t->y+t->h<=p->y+p->h+2);if(!t->getUserString("role").empty()){gbmHover(t,0);gbmPress(t,0,0,MyGUI::MouseButton());gbmRelease(t,0,0,MyGUI::MouseButton());gbmLeave(t,0);}}
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];if(!t->text.empty()){MyGUI::IntSize size=t->getTextSize();if(size.width>t->w+2||size.height>t->h+2){std::cerr<<"Text overflow "<<t->fontName<<" "<<size.width<<","<<size.height<<" in "<<t->w<<","<<t->h<<" "<<t->text<<"\n";return 2;}}}
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];std::cout<<t->id<<'\t'<<t->parent<<'\t'<<t->kind<<'\t'<<t->x<<'\t'<<t->y<<'\t'<<t->w<<'\t'<<t->h<<'\t'<<t->font<<'\t'<<t->color.r<<'\t'<<t->color.g<<'\t'<<t->color.b<<'\t'<<t->scroll<<'\t'<<t->selected<<'\t'<<escaped(t->text)<<'\t'<<t->texture<<'\t'<<t->fontName<<'\t'<<t->alpha<<'\t'<<t->crop.left<<'\t'<<t->crop.top<<'\t'<<t->crop.width<<'\t'<<t->crop.height<<'\t'<<t->align<<'\t'<<t->enabled<<'\t'<<t->visible<<'\t'<<t->canvasW<<'\t'<<t->canvasH<<'\t'<<t->hscroll<<'\n';}
}
