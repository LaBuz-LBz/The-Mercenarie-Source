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
 int x,y,w,h,font,align;std::string kind,text,texture;Colour color;IntCoord crop;bool enabled,selected,visible;Event eventMouseButtonClick,eventWindowButtonPressed,eventMouseSetFocus,eventMouseLostFocus;
 Widget():x(0),y(0),w(0),h(0),font(16),align(0),color(),enabled(true),selected(false),visible(true){}
 template<class T>T* createWidget(const char* skin,int xx,int yy,int ww,int hh,int,const char* layer="",const char* name=""){T* t=new T;t->kind=skin;t->x=x+xx;t->y=y+yy;t->w=ww;t->h=hh;widgets.push_back(t);return t;}
 void setCaption(const std::string& s){text=s;}void setFontHeight(int n){font=n;}void setTextColour(Colour c){color=c;}void setTextAlign(int n){align=n;}void setNeedMouseFocus(bool){}void setColour(Colour c){color=c;}void setImageTexture(const char* s){texture=s;}void setImageCoord(IntCoord r){crop=r;}void setEnabled(bool b){enabled=b;}void setStateSelected(bool b){selected=b;}void setVisible(bool b){visible=b;}
 int getWidth(){return w;} int getHeight(){return h;} bool getVisible(){return visible;} Widget* getClientWidget(){return this;}
};
struct TextBox:Widget{};struct ImageBox:Widget{};struct Button:Widget{};struct Window:Widget{};typedef Widget* WidgetPtr;
struct Gui:Widget {static Gui& getInstance(){static Gui g;return g;}static Gui* getInstancePtr(){return &getInstance();}};
struct RenderManager {IntSize view;static RenderManager& getInstance(){static RenderManager r;return r;}const IntSize& getViewSize(){return view;}};
struct ResourceManager {static ResourceManager& getInstance(){static ResourceManager r;return r;}void load(const char*){}};
}
namespace Ogre {struct ResourceGroupManager {static ResourceGroupManager& getSingleton(){static ResourceGroupManager r;return r;}bool resourceLocationExists(const char*,const char*){return true;}void addResourceLocation(const char*,const char*,const char*){}};}
bool gMercenarieEnglish=false;
MyGUI::Widget* mercenarieLauncherMenu=0;
bool routeTestEnabled(){return false;}
void closeMercenarieLauncher(MyGUI::Widget*){}void openMercenarieQuests(MyGUI::Widget*){}void openMercenarieAutopilot(MyGUI::Widget*){}void toggleGuildManagement(MyGUI::Widget*){}void openRouteTest(MyGUI::Widget*){}void prepareRoadTestContract(MyGUI::Widget*){}
#include "LauncherView.h"
int main(int argc,char**){gMercenarieEnglish=argc>1;MyGUI::RenderManager::getInstance().view=MyGUI::IntSize(900,800);buildMercenarieLauncher(MyGUI::Gui::getInstancePtr());launcherFocus(launcherCards[0],0);
 std::ofstream out("report-preview.tsv");
 for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];std::string text=t->text;for(size_t j=0;j<text.size();++j)if(text[j]=='\n')text[j]='~';out<<t->kind<<'\t'<<t->x<<'\t'<<t->y<<'\t'<<t->w<<'\t'<<t->h<<'\t'<<t->font<<'\t'<<t->align<<'\t'<<(int)(t->color.r*255)<<'\t'<<(int)(t->color.g*255)<<'\t'<<(int)(t->color.b*255)<<'\t'<<t->texture<<'\t'<<t->crop.left<<'\t'<<text<<'\n';}
}
