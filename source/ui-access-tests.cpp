#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <iostream>
namespace MyGUI {
struct Widget;std::map<std::string,Widget*> registry;
namespace Align{enum{Default,Stretch};}
struct Event{int count;Event():count(0){}void operator+=(int){++count;}};
template<class T> int newDelegate(T){return 1;}
struct Widget{
 bool live,visible;int w,h;std::string name;Widget* parent;Event eventWindowButtonPressed,eventMouseButtonClick;
 Widget():live(true),visible(false),w(600),h(200),parent(0){}
 int getWidth(){assert(live);return w;}int getHeight(){assert(live);return h;}
 Widget* getClientWidget(){assert(live);return this;}
 void setVisible(bool b){assert(live);visible=b;}
 void setEditStatic(bool){}void setEditReadOnly(bool){}void setEditMultiLine(bool){}void setEditWordWrap(bool){}void setFontHeight(int){}
 template<class T>T* createWidget(const char*,int,int,int width,int height,int,const std::string& name){assert(!registry.count(name));T* p=new T;p->w=width;p->h=height;p->name=name;p->parent=this;registry[name]=p;return p;}
 template<class T>T* createWidget(const char* s,int x,int y,int width,int height,int a,const std::string&,const std::string& name){T* p=createWidget<T>(s,x,y,width,height,a,name);p->parent=0;return p;}
};
typedef Widget Window;typedef Widget Button;typedef Widget EditBox;
struct Gui:Widget{
 static Gui* instance;static Gui* getInstancePtr(){return instance;}
 template<class T>T* findWidget(const std::string& n,bool){return registry.count(n)?static_cast<T*>(registry[n]):0;}
 void destroyWidget(Widget* p){assert(p->live);std::vector<Widget*> children;for(std::map<std::string,Widget*>::iterator i=registry.begin();i!=registry.end();++i)if(i->second->parent==p)children.push_back(i->second);for(size_t i=0;i<children.size();++i)destroyWidget(children[i]);registry.erase(p->name);p->live=false;}
};Gui* Gui::instance=0;
struct IntSize{int width,height;IntSize():width(1280),height(720){}};
struct RenderManager{static RenderManager& getInstance(){static RenderManager r;return r;}IntSize getViewSize(){return IntSize();}};
}
namespace Loc{const char* text(const char* s){return s;}}
namespace MercenarieFonts{void caption(MyGUI::Widget* w,const char*){assert(w->live);}}
namespace MainMenuNews{bool title=false;bool mainMenuReady(){return title;}}
struct GameWorld{void* player;bool loading;GameWorld():player(this),loading(false){}bool isLoadingFromASaveGame(){return loading;}} world;GameWorld* ou=&world;
namespace OIS{typedef int KeyCode;}
namespace ClientOptions{enum{OpenGuildManagement};}
struct Settings{int bindings[1];Settings(){bindings[0]=36;}}clientOptions;
struct Keyboard{int pressed;Keyboard():pressed(0){}bool isKeyDown(int k){return pressed==k;}}keyboard;
struct Input{Keyboard* keyboard;}input={&keyboard};Input* key=&input;
bool missionWorldChanging=false,missionRestorePending=false,progressLoadFault=false,progressWriteBlocked=false,guildKeyCapture=false,negotiationOpen=false,jWasDown=false;
std::string activeMercenarieSaveSlot="test";
MyGUI::Widget *trackerIcon=0,*mercenarieLauncherMenu=0,*trackerWindow=0,*trackerEntry=0,*bountyTrackerEntry=0,*guildWindow=0;
unsigned long mercenarieUISession=0;
int polls=0,opened=0,resets=0;namespace {bool mercenarieGameplayUnavailable();void showMercenarieUnavailable();}
void DebugLog(const std::string&){}void loadClientOptions(){}void launcherEnsureUi(){}void launcherLogo(MyGUI::Widget*,int,int,int){}
void closeMercenarieInterface(MyGUI::Window*,const std::string&){}void trackerIconClicked(MyGUI::Widget*){}
void resetQuestTrackerView(){++resets;}
void rebuildGuildMenuForViewport(){guildWindow=0;++resets;}
void buildMercenarieLauncher(MyGUI::Gui* g){if(!mercenarieLauncherMenu)mercenarieLauncherMenu=g->createWidget<MyGUI::Widget>("",0,0,500,300,0,"Window","MercenarieLauncherMenu");}
void toggleGuildManagement(int){++opened;if(mercenarieGameplayUnavailable())showMercenarieUnavailable();}
bool gbModalOpen(){return false;}
#include "ui-access-init.generated.h"
#include "ui-access-hotkey.generated.h"
#include "MissionUIAccess.h"
int main(){
 MyGUI::Gui gui;MyGUI::Gui::instance=&gui;
 for(int cycle=0;cycle<100;++cycle){++mercenarieUISession;
  missionRestorePending=cycle%3==0;progressWriteBlocked=cycle%3==1;progressLoadFault=cycle%3==2;
  keyboard.pressed=0;updateMercenarieUIShell(&world);assert(trackerIcon&&trackerIcon->visible);
  int before=opened;keyboard.pressed=clientOptions.bindings[0];updateMercenarieUIShell(&world);assert(opened==before+1);
  for(int i=0;i<5;++i)updateMercenarieUIShell(&world);assert(opened==before+1);assert(trackerIcon->eventMouseButtonClick.count==1);
  assert(gui.findWidget<MyGUI::Widget>("MercenarieUnavailable",false));
  gui.destroyWidget(trackerIcon); // cached pointer remains stale
  gui.destroyWidget(mercenarieLauncherMenu);updateMercenarieUIShell(&world);assert(trackerIcon->live&&trackerIcon->eventMouseButtonClick.count==1);
  trackerWindow=gui.createWidget<MyGUI::Widget>("",0,0,10,10,0,"Window","GuildEscortTrackerWindow");gui.destroyWidget(trackerWindow);
  guildWindow=gui.createWidget<MyGUI::Widget>("",0,0,10,10,0,"Window","GuildContractsMenu");gui.destroyWidget(guildWindow);updateMercenarieUIShell(&world);assert(!trackerWindow&&!guildWindow);
  MainMenuNews::title=true;before=polls;updateMercenarieUIShell(&world);assert(!trackerIcon->visible&&polls==before);MainMenuNews::title=false;
  missionWorldChanging=true;updateMercenarieUIShell(&world);assert(polls==before);missionWorldChanging=false;
  world.loading=true;updateMercenarieUIShell(&world);assert(polls==before);world.loading=false;
  clientOptions.bindings[0]=cycle%2?36:37;
 }
 missionRestorePending=progressWriteBlocked=progressLoadFault=false;keyboard.pressed=0;updateMercenarieUIShell(&world);
 assert(!gui.findWidget<MyGUI::Widget>("MercenarieUnavailable",false));
 MyGUI::Gui::instance=0;int before=polls;updateMercenarieUIShell(&world);assert(polls==before);
 std::cout<<"PASS: production UI shell/init/hotkey, 100 lifecycle cycles, blocked restore/progress, custom key, edge polling, stale roots, one click delegate, title/loading/null GUI\n";
}

