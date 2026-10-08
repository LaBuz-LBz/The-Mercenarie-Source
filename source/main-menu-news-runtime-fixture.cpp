#include "MainMenuNewsRules.h"
#include <cassert>
#include <vector>
#include <stdexcept>
#include <iostream>
typedef unsigned long DWORD;
typedef long LONG;
DWORD clockMs=10000;
DWORD GetTickCount(){return clockMs;}
void DebugLog(const std::string&){}
void ErrorLog(const std::string&){}
namespace MyGUI {
struct Widget {
    std::string name;bool visible,button,selected;Widget* parent;std::vector<Widget*> children;
    Widget(const std::string& n="",bool b=false):name(n),visible(true),button(b),selected(false),parent(0){}
    const std::string& getName(){return name;}
    bool getInheritedVisible(){return visible&&(!parent||parent->getInheritedVisible());}
    size_t getChildCount(){return children.size();}
    Widget* getChildAt(size_t i){return children[i];}
    Widget* getParent(){return parent;}
    template<class T> T* castType(bool){return button?reinterpret_cast<T*>(this):0;}
    void setVisible(bool v){visible=v;}
    bool getStateSelected(){return selected;}
    void setStateSelected(bool v){selected=v;}
    void add(Widget* w){children.push_back(w);w->parent=this;}
};
typedef Widget Button;
struct EnumeratorWidgetPtr {
    std::vector<Widget*> roots;size_t nextIndex;
    EnumeratorWidgetPtr(const std::vector<Widget*>& r):roots(r),nextIndex(0){}
    bool next(){return nextIndex<roots.size()?++nextIndex,true:false;}
    Widget* current(){return roots[nextIndex-1];}
};
struct Gui {
    std::vector<Widget*> roots;
    static Gui& getInstance(){static Gui g;return g;}
    static Gui* getInstancePtr(){return &getInstance();}
    EnumeratorWidgetPtr getEnumerator(){return EnumeratorWidgetPtr(roots);}
};
struct IntSize {int width,height;IntSize():width(1920),height(1080){} };
struct RenderManager {
    static RenderManager& getInstance(){static RenderManager r;return r;}
    IntSize getViewSize(){return IntSize();}
};
}
using namespace MyGUI;
Widget modal,manual,check;
Widget *modalRoot=0,*manualButton=0,*dismissCheck=0;
bool pendingClose=false,automaticHandled=false,guiReadyLogged=false,menuReadyLogged=false,decisionLogged=false,creationFailureLogged=false;
DWORD nextCreationAttempt=0;
int lastWidth=1920,lastHeight=1080,created=0;
bool failCreation=false;
int selectedVersion=0;bool pendingNavigation=false;
std::string preference;
std::string dismissedVersion(){return preference;}
void destroyModal(){modalRoot=0;dismissCheck=0;pendingClose=false;}
void destroyManual(){manualButton=0;}
void cleanup(){destroyModal();destroyManual();}
void ensureResources(){}
void ensureManual(){manualButton=&manual;lastWidth=1920;lastHeight=1080;}
void createModal(){if(failCreation)throw std::runtime_error("fixture failure");modalRoot=&modal;dismissCheck=&check;++created;}
inline MyGUI::Widget* deepFind(MyGUI::Widget* w,const char* name){if(!w||!w->getInheritedVisible())return 0;if(MainMenuNewsRules::matchesLayoutName(w->getName(),name))return w;for(size_t i=0;i<w->getChildCount();++i){MyGUI::Widget* found=deepFind(w->getChildAt(i),name);if(found)return found;}return 0;}
inline MyGUI::Widget* findRootChild(const char* name){MyGUI::EnumeratorWidgetPtr roots=MyGUI::Gui::getInstance().getEnumerator();while(roots.next()){MyGUI::Widget* found=deepFind(roots.current(),name);if(found)return found;}return 0;}
inline bool mainMenuReady(){
    // Import + Credits identify the title menu, unlike the in-game pause menu.
    // Continue may be hidden when no save exists. All required buttons must
    // belong to the same native menu container (never combine separate menus).
    MyGUI::Widget* importButton=findRootChild("ImportGameButton");
    if(!importButton||!importButton->castType<MyGUI::Button>(false))return false;
    MyGUI::Widget* parent=importButton->getParent();if(!parent)return false;
    const char* names[]={"NewGameButton","LoadGameButton","OptionsButton","CreditsButton","ExitButton"};
    for(int i=0;i<5;++i){MyGUI::Widget* w=deepFind(parent,names[i]);if(!w||!w->castType<MyGUI::Button>(false))return false;}return true;
}

inline bool tryCreateModal(){
    try{selectedVersion=MainMenuNewsRules::versionCount()-1;createModal();automaticHandled=true;creationFailureLogged=false;return true;}
    catch(const std::exception& e){if(!creationFailureLogged)ErrorLog(std::string("Mercenarie news: popup creation failed: ")+e.what());}
    catch(...){if(!creationFailureLogged)ErrorLog("Mercenarie news: popup creation failed (unknown exception)");}
    creationFailureLogged=true;destroyModal();nextCreationAttempt=GetTickCount()+5000;return false;
}

inline void update(){
    if(!MyGUI::Gui::getInstancePtr())return;
    if(!guiReadyLogged){DebugLog("Mercenarie news: MyGUI frame ready; waiting for visible main menu");guiReadyLogged=true;}
    if(pendingClose){pendingNavigation=false;destroyModal();}
    if(pendingNavigation&&modalRoot){bool checked=dismissCheck&&dismissCheck->getStateSelected();pendingNavigation=false;destroyModal();try{createModal();dismissCheck->setStateSelected(checked);}catch(...){destroyModal();}}
    const bool menu=mainMenuReady();if(!menu){cleanup();return;}
    if(!menuReadyLogged){DebugLog("Mercenarie news: main menu detected (BaseLayout prefix supported)");menuReadyLogged=true;}
    if(nextCreationAttempt&&static_cast<LONG>(GetTickCount()-nextCreationAttempt)<0)return;
    try{
    ensureResources();ensureManual();if(modalRoot)manualButton->setVisible(false);else manualButton->setVisible(true);
    const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();if((modalRoot||manualButton)&&(v.width!=lastWidth||v.height!=lastHeight)){bool wasModal=modalRoot!=0,checked=dismissCheck&&dismissCheck->getStateSelected();destroyModal();destroyManual();lastWidth=v.width;lastHeight=v.height;ensureManual();if(wasModal){createModal();dismissCheck->setStateSelected(checked);manualButton->setVisible(false);}else manualButton->setVisible(true);}
    const std::string dismissed=dismissedVersion();
    const bool show=MainMenuNewsRules::shouldShowAutomatically(dismissed,MainMenuNewsRules::CurrentNewsVersion,automaticHandled);
    if(!decisionLogged){DebugLog(std::string("Mercenarie news: content-version=")+MainMenuNewsRules::CurrentNewsVersion+" dismissed="+(dismissed.empty()?"<absent>":dismissed)+" decision="+(show?"show":automaticHandled?"skip/session-shown":"skip/version-dismissed"));decisionLogged=true;}
    if(!modalRoot&&show&&tryCreateModal())manualButton->setVisible(false);
    }catch(const std::exception& e){if(!creationFailureLogged)ErrorLog(std::string("Mercenarie news: UI setup failed: ")+e.what());creationFailureLogged=true;cleanup();nextCreationAttempt=GetTickCount()+5000;}
    catch(...){if(!creationFailureLogged)ErrorLog("Mercenarie news: UI setup failed (unknown exception)");creationFailureLogged=true;cleanup();nextCreationAttempt=GetTickCount()+5000;}
}

void reset(const std::string& dismissed){cleanup();preference=dismissed;automaticHandled=false;decisionLogged=false;created=0;nextCreationAttempt=0;failCreation=false;}
int main(){
    Widget hiddenRoot("old_Root"),hiddenImport("old_ImportGameButton",true);
    hiddenRoot.add(&hiddenImport);hiddenRoot.visible=false;
    Widget root("000ABC_Root"),container;
    root.add(&container);
    const char* names[]={"ImportGameButton","NewGameButton","LoadGameButton","OptionsButton","CreditsButton","ExitButton"};
    Widget buttons[6];
    for(int i=0;i<6;++i){buttons[i].name=std::string("000ABC_")+names[i];buttons[i].button=true;container.add(&buttons[i]);}
    Gui::getInstance().roots.push_back(&hiddenRoot);Gui::getInstance().roots.push_back(&root);
    assert(mainMenuReady()); // Prefix and no Continue button (fresh install).
    reset("");update();assert(created==1&&modalRoot&&automaticHandled);
    update();assert(created==1);
    pendingClose=true;update();assert(!modalRoot&&manualButton->visible&&created==1);
    reset("");update();assert(created==1); // New process, unchecked close.
    reset("V8");update();assert(created==1&&modalRoot&&selectedVersion==1);
    check.setStateSelected(true);selectedVersion=0;pendingNavigation=true;update();assert(created==2&&selectedVersion==0&&check.getStateSelected()&&preference=="V8");
    selectedVersion=1;pendingNavigation=true;update();assert(created==3&&selectedVersion==1&&preference=="V8");
    reset("V9");update();assert(created==0&&!modalRoot);
    reset("V7");update();assert(created==1);
    assert(MainMenuNewsRules::shouldShowAutomatically("V8","V8.1",false));
    reset("");failCreation=true;update();assert(!automaticHandled&&!modalRoot&&created==0);
    failCreation=false;clockMs+=5001;update();assert(automaticHandled&&created==1);
    reset("");buttons[4].visible=false;update();assert(!modalRoot&&created==0); // Pause/options/incomplete menu.
    buttons[4].visible=true;update();assert(created==1);
    root.visible=false;update();assert(!modalRoot&&!manualButton);
    std::cout<<"PASS: production menu detector + update: prefixed/hidden widgets, absent Continue, current/dismissed/session, close cleanup, retry after creation failure.\n";
}