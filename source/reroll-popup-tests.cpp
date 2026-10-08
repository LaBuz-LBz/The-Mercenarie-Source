#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "reroll-popup-fixtures.generated.h"
namespace MyGUI {
struct Widget;struct ToolTipInfo{};struct MouseButton{};
struct IntSize{int width,height;IntSize(int w=0,int h=0):width(w),height(h){}};
struct IntCoord{int left,top,width,height;IntCoord(int x=0,int y=0,int w=0,int h=0):left(x),top(y),width(w),height(h){}};
struct Colour{float r,g,b;Colour(float x=1,float y=1,float z=1):r(x),g(y),b(z){}};
struct Align{enum{Default=0,Left=1,Top=2,Center=4};};
template<class T>T newDelegate(T v){return v;}
struct Event{template<class T>void operator+=(T){}};
struct Click{void(*callback)(Widget*);Click():callback(0){}void operator+=(void(*f)(Widget*)){callback=f;}};
struct Widget{
 Widget* parent;std::vector<Widget*> children;std::map<std::string,std::string> data;
 int x,y,w,h;float alpha;bool visible,enabled,mouse;Colour colour;std::string skin;
 Click eventMouseButtonClick;Event eventToolTip,eventMouseSetFocus,eventMouseLostFocus,eventMouseButtonPressed,eventMouseButtonReleased,eventWindowButtonPressed;
 Widget():parent(0),x(0),y(0),w(0),h(0),alpha(1),visible(true),enabled(true),mouse(true){}
 virtual ~Widget(){for(size_t i=0;i<children.size();++i)delete children[i];}
 virtual const char* type(){return "widget";}
 template<class T>T* createWidget(const char* s,int a,int b,int c,int d,int,const char* =0){T* t=new T;t->skin=s;t->parent=this;t->setCoord(a,b,c,d);children.push_back(t);return t;}
 void setCoord(int a,int b,int c,int d){x=a;y=b;w=c;h=d;}int getWidth(){return w;}int getHeight(){return h;}
 Widget* getParent(){return parent;}size_t getChildCount(){return children.size();}Widget* getChildAt(size_t i){return children[i];}
 void setVisible(bool v){visible=v;}bool getVisible(){return visible;}void setEnabled(bool v){enabled=v;}bool getEnabled(){return enabled;}
 void setAlpha(float v){alpha=v;}void setColour(Colour c){colour=c;}void setNeedMouseFocus(bool v){mouse=v;}void setNeedToolTip(bool){}
 void setUserString(const char* k,const std::string& s){data[k]=s;}std::string getUserString(const char* k){return data[k];}
 Widget* hit(int a,int b){if(!visible)return 0;for(size_t i=children.size();i>0;--i){Widget* c=children[i-1];if(a>=c->x&&a<c->x+c->w&&b>=c->y&&b<c->y+c->h){Widget* found=c->hit(a-c->x,b-c->y);if(found)return found;}}return mouse&&enabled?this:0;}
};
struct TextBox:Widget{
 std::string caption;int font,align;TextBox():font(24),align(0){}const char* type(){return "text";}
 void setFontHeight(int n){font=n;}int getFontHeight(){return font;}void setTextAlign(int a){align=a;}
 IntSize getTextSize(){double width=0,maxw=0;int lines=1;for(size_t i=0;i<caption.size();){unsigned char c=caption[i++];
  if(c=='#'&&i+6<=caption.size()){i+=6;continue;}if(c=='\n'){maxw=std::max(maxw,width);width=0;++lines;continue;}
  unsigned int cp=c;int extra=0;if(c>=240){cp=c&7;extra=3;}else if(c>=224){cp=c&15;extra=2;}else if(c>=192){cp=c&31;extra=1;}
  while(extra--&&i<caption.size())cp=(cp<<6)|((unsigned char)caption[i++]&63);
  width+=glyphWidth(cp)*font;
 }return IntSize((int)ceil(std::max(maxw,width)),lines*(font+3));}
};
struct Button:TextBox{const char* type(){return "button";}};
struct ImageBox:Widget{std::string texture;IntCoord uv;const char* type(){return "image";}void setImageTexture(const char* s){texture=s;}void setImageCoord(IntCoord c){uv=c;}};
struct Window:TextBox{Widget* getClientWidget(){return this;}};
struct ScrollView:Widget{IntSize canvas;const char* type(){return "scroll";}void setCanvasAlign(int){}void setVisibleHScroll(bool){}void setCanvasSize(int w,int h){canvas=IntSize(w,h);}};
struct Gui:Widget{static Gui& getInstance(){static Gui g;return g;}void destroyWidget(Widget* w){if(w->parent){std::vector<Widget*>& v=w->parent->children;v.erase(std::find(v.begin(),v.end(),w));}delete w;}};
struct RenderManager{IntSize size;static RenderManager& getInstance(){static RenderManager r;return r;}IntSize getViewSize(){return size;}};
struct InputManager{Widget* modal;InputManager():modal(0){}static InputManager& getInstance(){static InputManager i;return i;}void removeWidgetModal(Widget*){modal=0;}void addWidgetModal(Widget* w){modal=w;}};
struct ResourceManager{bool loaded;ResourceManager():loaded(false){}static ResourceManager& getInstance(){static ResourceManager r;return r;}bool isExist(const char*){return loaded;}void load(const char* file){assert(std::string(file)=="MercenariePopup.xml");loaded=true;}};
}
namespace Loc{struct Engine{std::string language;};Engine& engine(){static Engine e;return e;}std::map<std::string,std::string> strings;std::string text(const char* k){return strings[k];}}
void setCatalogue(int n){Loc::engine().language=fixtureLanguages[n];for(int i=0;i<4;++i)Loc::strings[fixtureKeys[i]]=fixtureText[n][i];}
namespace MercenarieFonts{template<class T>void caption(T* p,const std::string& s){p->caption=s;}}
MyGUI::Colour registerAmber(1,.67f,.23f),registerIvory(.90f,.87f,.78f);
MyGUI::TextBox* registerText(MyGUI::Widget* p,int x,int y,int w,int h,int font,const std::string& s,const MyGUI::Colour& c){MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("text",x,y,w,h,0);t->font=font;t->caption=s;t->colour=c;t->mouse=false;return t;}
MyGUI::Widget* registerSolid(MyGUI::Widget* p,int x,int y,int w,int h,const MyGUI::Colour& c){MyGUI::Widget* t=p->createWidget<MyGUI::Widget>("WhiteSkin",x,y,w,h,0);t->colour=c;t->mouse=false;return t;}
void applyMercenarieFrame(MyGUI::Widget*,bool){}
void bookWrap(MyGUI::TextBox* p,const std::string& s){p->caption=s;}
bool v9WidgetLive(MyGUI::Widget* p){return p!=0;}
void mercenarieDestroyLiveWidget(MyGUI::Widget* p){if(p)MyGUI::Gui::getInstance().destroyWidget(p);}
std::string registerNumber(int i){std::ostringstream s;s<<i;return s.str();}
void rerollBookRow(MyGUI::Widget*){}void rerollClassicRow(MyGUI::Widget*){}void bookRerollTooltip(MyGUI::Widget*,const MyGUI::ToolTipInfo&){}
struct Charges{int remaining;}guildRerolls;
#include "ContractRerollView.h"
MyGUI::Window* v9Confirmation=0;MyGUI::Widget* bookRerollTip=0;
void(*v9ConfirmationCallback)(int)=0;
int confirms=0,cancels=0;
void confirmReroll(int answer){if(answer){++confirms;--guildRerolls.remaining;}else ++cancels;}
#include "reroll-popup-dispatch.generated.h"
#include "tests/NativeInputLayoutStub.h"
#include "RerollPopupView.h"
#include "reroll-popup-create.generated.h"
std::string hex(const std::string& s){const char* h="0123456789abcdef";std::string r;for(size_t i=0;i<s.size();++i){unsigned char c=s[i];r+=h[c>>4];r+=h[c&15];}return r;}
void dump(MyGUI::Widget* w,std::ofstream& f,int x=0,int y=0,float opacity=1){
 x+=w->x;y+=w->y;opacity*=w->alpha;MyGUI::TextBox* t=dynamic_cast<MyGUI::TextBox*>(w);MyGUI::ImageBox* icon=dynamic_cast<MyGUI::ImageBox*>(w);
 f<<w->type()<<"\t"<<x<<"\t"<<y<<"\t"<<w->w<<"\t"<<w->h<<"\t"<<w->colour.r<<"\t"<<w->colour.g<<"\t"<<w->colour.b<<"\t"<<opacity<<"\t"<<(t?t->font:0)<<"\t"<<(t?t->align:0)<<"\t"<<(icon?icon->uv.left:0)<<"\t"<<(t?hex(t->caption):"")<<"\n";
 for(size_t i=0;i<w->children.size();++i)dump(w->children[i],f,x,y,opacity);
}
void verify(MyGUI::Widget* w,bool inScroll=false){
 for(size_t i=0;i<w->children.size();++i){MyGUI::Widget* c=w->children[i];assert(c->w>0&&c->h>0);assert(c->x>=0&&c->y>=0);
  if(!inScroll){assert(c->x+c->w<=w->w);assert(c->y+c->h<=w->h);}
  MyGUI::TextBox* t=dynamic_cast<MyGUI::TextBox*>(c);if(t){assert(t->getTextSize().width<=c->w);assert(t->getTextSize().height<=c->h);}
  MyGUI::ImageBox* icon=dynamic_cast<MyGUI::ImageBox*>(c);if(icon)assert(icon->w==icon->h&&icon->texture=="MercenariePopupIcons.png");
  verify(c,inScroll||dynamic_cast<MyGUI::ScrollView*>(c)!=0);
 }
}
int main(){
 const int widths[]={800,1024,1280,1920,2560},heights[]={600,768,720,1080,1440};
 for(int lang=0;lang<7;++lang){setCatalogue(lang);for(int r=0;r<5;++r){
  MyGUI::RenderManager::getInstance().size=MyGUI::IntSize(widths[r],heights[r]);guildRerolls.remaining=2;
  v9Confirm(Loc::text("v9.reroll.confirm_title"),Loc::text("v9.reroll.confirm_body"),"v9.confirm",confirmReroll);
  assert(v9Confirmation->skin=="MercenariePopupWindow"&&MyGUI::InputManager::getInstance().modal==v9Confirmation);
  assert(v9Confirmation->x>=0&&v9Confirmation->y>=0&&v9Confirmation->x+v9Confirmation->w<=widths[r]&&v9Confirmation->y+v9Confirmation->h<=heights[r]);verify(v9Confirmation);
  assert(rerollPopupBody->caption.find("#F5AD3625")!=std::string::npos);
  std::vector<MyGUI::Button*> buttons;for(size_t i=0;i<v9Confirmation->children.size();++i)if(MyGUI::Button* b=dynamic_cast<MyGUI::Button*>(v9Confirmation->children[i]))buttons.push_back(b);
  assert(buttons.size()==3&&buttons[1]->w==buttons[2]->w&&buttons[1]->h==buttons[2]->h);
  for(int i=0;i<3;++i){v9Confirmation->setVisible(true);MyGUI::Button* b=buttons[i];assert(b->eventMouseButtonClick.callback==answerV9Confirmation);assert(v9Confirmation->hit(b->x+b->w/2,b->y+b->h/2)==b);
   rerollButtonHover(b,0);assert(b->alpha==1);rerollButtonPress(b,0,0,MyGUI::MouseButton());assert(b->alpha<1);rerollButtonRelease(b,0,0,MyGUI::MouseButton());assert(b->alpha==1);
   v9Confirmation->setVisible(true);v9ConfirmationCallback=confirmReroll;b->eventMouseButtonClick.callback(b);assert(!v9ConfirmationCallback&&!v9Confirmation->getVisible());
   assert(guildRerolls.remaining==(i==0?2:1));
  }
  if(lang<4&&r==3){v9Confirmation->setVisible(true);std::ostringstream path;path<<"reports/popup-"<<Loc::engine().language<<".tsv";std::ofstream out(path.str().c_str());dump(v9Confirmation,out);}
  v9Confirmation->setVisible(true);MyGUI::RenderManager::getInstance().size=MyGUI::IntSize(1024,768);updateRerollPopupLayout();verify(v9Confirmation);
 }}
 // Long, unspaced translation: all content remains in the scroll canvas.
 setCatalogue(4);std::string longText;for(int i=0;i<30;++i)longText+=Loc::text("v9.reroll.confirm_body");Loc::strings["v9.reroll.confirm_body"]=longText;
 layoutRerollPopup();verify(v9Confirmation);assert(rerollPopupScroll->canvas.height>rerollPopupScroll->h);
 // Changing a community language while open refreshes text at the same size.
 setCatalogue(1);updateRerollPopupLayout();assert(rerollPopupTitle->caption==Loc::text("v9.reroll.confirm_title"));verify(v9Confirmation);
 Loc::strings["v9.confirm"]="Confirm this replacement contract and its reduced reward";layoutRerollPopup();verify(v9Confirmation);
 assert(confirms==35&&cancels==70);
 mercenarieDestroyLiveWidget(v9Confirmation);v9Confirmation=0;updateRerollPopupLayout();
 std::cout<<"PASS production popup creation, 7 languages x 5 resolutions, modal/real buttons/hit-testing, cancel/X/confirm dispatcher, states, resources, wrapping and overflow, live resize\n";
}
