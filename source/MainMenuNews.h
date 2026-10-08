#pragma once
#include "MainMenuNewsRules.h"
#include "Localization.h"
#include "src/UI/ClientOptionsFile.h"
#include <mygui/MyGUI_Gui.h>
#include <mygui/MyGUI_Button.h>
#include <mygui/MyGUI_ImageBox.h>
#include <mygui/MyGUI_ScrollView.h>
#include <mygui/MyGUI_TextBox.h>
#include <mygui/MyGUI_EditBox.h>
#include <mygui/MyGUI_RenderManager.h>
#include <mygui/MyGUI_ResourceManager.h>
#include <mygui/MyGUI_InputManager.h>
#include <Debug.h>
#include <ogre/OgreResourceGroupManager.h>
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

namespace MainMenuNews {

struct NewsItem { const char* id; int icon; const char* title; const char* shortText; const char* tooltipTitle; const char* tooltipText; };
static const NewsItem Items[6]={
    {"contracts",0,"news.card.contracts.title","news.card.contracts.short","news.card.contracts.title","news.card.contracts.tooltip"},
    {"delegation",1,"news.card.delegation.title","news.card.delegation.short","news.card.delegation.title","news.card.delegation.tooltip"},
    {"finance",2,"news.card.finance.title","news.card.finance.short","news.card.finance.title","news.card.finance.tooltip"},
    {"options",3,"news.card.options.title","news.card.options.short","news.card.options.title","news.card.options.tooltip"},
    {"localization",4,"news.card.localization.title","news.card.localization.short","news.card.localization.title","news.card.localization.tooltip"},
    {"fixes",5,"news.card.fixes.title","news.card.fixes.short","news.card.fixes.title","news.card.fixes.tooltip"}
};

static const NewsItem V9Items[6]={
    {"reroll",0,"news.v9.reroll.title","news.v9.reroll.short","news.v9.reroll.title","news.v9.reroll.short"},
    {"translations",1,"news.card.localization.title","news.v9.translations.short","news.card.localization.title","news.v9.translations.short"},
    {"payroll",2,"news.v9.payroll.title","news.v9.payroll.short","news.v9.payroll.title","news.v9.payroll.short"},
    {"artisan",3,"news.v9.artisan.title","news.v9.artisan.short","news.v9.artisan.title","news.v9.artisan.short"},
    {"armour",4,"news.v9.armour.title","news.v9.armour.short","news.v9.armour.title","news.v9.armour.short"},
    {"buildings",5,"news.v9.buildings.title","news.v9.buildings.short","news.v9.buildings.title","news.v9.buildings.short"},
};
static int selectedVersion=MainMenuNewsRules::versionCount()-1;
static bool pendingNavigation=false;
inline const NewsItem* pageItems(){return selectedVersion==0?Items:V9Items;}
inline int pageItemCount(){return 6;}
inline const char* pageVersion(){return MainMenuNewsRules::Versions[selectedVersion];}
inline const char* pageChangelog(){return selectedVersion==0?"news.changelog_body":"news.v9.changelog_body";}
inline const char* pageChangelogTitle(){return selectedVersion==0?"news.changelog_title":"news.v9.changelog_title";}
inline std::string navigationCaption(int delta){Loc::Catalogue args;args["version"]=MainMenuNewsRules::Versions[MainMenuNewsRules::navigate(selectedVersion,delta,MainMenuNewsRules::versionCount())];return Loc::format(delta<0?"news.previous_version":"news.next_version",args);}

static MyGUI::Widget* modalRoot=0;
static MyGUI::Widget* popup=0;
static MyGUI::Widget* summaryPanel=0;
static MyGUI::Widget* changelogPanel=0;
static MyGUI::Widget* tooltip=0;
static MyGUI::Button* dismissCheck=0;
static MyGUI::Button* continueButton=0;
static MyGUI::Button* changelogButton=0;
static MyGUI::Button* returnButton=0;
static MyGUI::Button* manualButton=0;
static bool automaticHandled=false;
static bool pendingClose=false;
static bool resourcesLoaded=false;
static bool guiReadyLogged=false;
static bool menuReadyLogged=false;
static bool decisionLogged=false;
static bool creationFailureLogged=false;
static DWORD nextCreationAttempt=0;
static int lastWidth=0,lastHeight=0;

inline void update();
inline void cleanup();
inline void guiShuttingDown(){cleanup();resourcesLoaded=false;guiReadyLogged=false;menuReadyLogged=false;}

inline std::string trim(const std::string& s){size_t a=0,b=s.size();while(a<b&&(s[a]==' '||s[a]=='\t'||s[a]=='\r'))++a;while(b>a&&(s[b-1]==' '||s[b-1]=='\t'||s[b-1]=='\r'))--b;return s.substr(a,b-a);}
inline std::string dismissedVersion(){
    ClientOptionsFile::Failure failure;std::wstring file=ClientOptionsFile::path(failure);std::string bytes;
    if(file.empty()||!ClientOptionsFile::read(file,bytes,failure))return "";
    return MainMenuNewsRules::dismissedPreference(bytes);
}
inline bool persistDismissed(){
    ClientOptionsFile::Failure failure;std::wstring file=ClientOptionsFile::path(failure);std::string bytes;
    if(file.empty()||(!ClientOptionsFile::read(file,bytes,failure)&&failure.code!=ERROR_FILE_NOT_FOUND))return false;
    std::istringstream in(bytes);std::ostringstream out;std::string line;
    while(std::getline(in,line)){size_t p=line.find('=');if(p!=std::string::npos&&trim(line.substr(0,p))=="lastDismissedNewsVersion")continue;out<<line<<"\n";}
    out<<"lastDismissedNewsVersion="<<MainMenuNewsRules::CurrentNewsVersion<<"\n";
    return ClientOptionsFile::write(file,out.str(),failure);
}

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
inline void ensureResources(){if(resourcesLoaded)return;Ogre::ResourceGroupManager& r=Ogre::ResourceGroupManager::getSingleton();if(!r.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))r.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");MyGUI::ResourceManager::getInstance().load("GuildRegisterSkins.xml");MyGUI::ResourceManager::getInstance().load("MercenarieNewsSkins.xml");resourcesLoaded=true;}
inline MyGUI::TextBox* text(MyGUI::Widget* parent,int x,int y,int w,int h,int size,const char* value,const MyGUI::Colour& colour){MyGUI::TextBox* t=parent->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",x,y,w,h,MyGUI::Align::Default);MercenarieFonts::caption(t,value);t->setFontHeight(size);t->setTextColour(colour);t->setNeedMouseFocus(false);return t;}
inline MyGUI::EditBox* wrappedText(MyGUI::Widget* parent,int x,int y,int w,int h,int size,const char* value,const MyGUI::Colour& colour){
    // MyGUI TextBox does not implement WordWrap. Kenshi's native wrapping widget is an EditBox.
    MyGUI::EditBox* t=parent->createWidget<MyGUI::EditBox>("Kenshi_WordWrapEmpty",x,y,w,h,MyGUI::Align::Default);
    t->setEditStatic(true);t->setEditReadOnly(true);t->setEditMultiLine(true);t->setEditWordWrap(true);
    t->setFontName(MercenarieFonts::newsFont("MercenarieNewsBody"));t->setFontHeight(size);
    t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setTextColour(colour);
    t->setNeedMouseFocus(false);t->setNeedKeyFocus(false);MercenarieFonts::caption(t,value);return t;
}
inline void border(MyGUI::Widget* parent,const MyGUI::Colour& colour){MyGUI::Widget* top=parent->createWidget<MyGUI::Widget>("WhiteSkin",0,0,parent->getWidth(),1,MyGUI::Align::HStretch|MyGUI::Align::Top);MyGUI::Widget* bottom=parent->createWidget<MyGUI::Widget>("WhiteSkin",0,parent->getHeight()-1,parent->getWidth(),1,MyGUI::Align::HStretch|MyGUI::Align::Bottom);MyGUI::Widget* left=parent->createWidget<MyGUI::Widget>("WhiteSkin",0,0,1,parent->getHeight(),MyGUI::Align::Left|MyGUI::Align::VStretch);MyGUI::Widget* right=parent->createWidget<MyGUI::Widget>("WhiteSkin",parent->getWidth()-1,0,1,parent->getHeight(),MyGUI::Align::Right|MyGUI::Align::VStretch);top->setColour(colour);bottom->setColour(colour);left->setColour(colour);right->setColour(colour);top->setNeedMouseFocus(false);bottom->setNeedMouseFocus(false);left->setNeedMouseFocus(false);right->setNeedMouseFocus(false);}
inline void destroyTooltip(){if(tooltip&&MyGUI::Gui::getInstancePtr())MyGUI::Gui::getInstance().destroyWidget(tooltip);tooltip=0;}
inline void destroyModal(){destroyTooltip();if(modalRoot&&MyGUI::Gui::getInstancePtr()){if(MyGUI::InputManager::getInstancePtr())MyGUI::InputManager::getInstance().removeWidgetModal(modalRoot);MyGUI::Gui::getInstance().destroyWidget(modalRoot);}modalRoot=popup=summaryPanel=changelogPanel=0;dismissCheck=continueButton=changelogButton=returnButton=0;pendingClose=false;}
inline void destroyManual(){if(manualButton&&MyGUI::Gui::getInstancePtr())MyGUI::Gui::getInstance().destroyWidget(manualButton);manualButton=0;}
inline void cleanup(){destroyModal();destroyManual();lastWidth=lastHeight=0;}

inline void closePressed(MyGUI::Widget*){if(dismissCheck&&dismissCheck->getStateSelected()){if(!persistDismissed())ErrorLog("Mercenarie news: could not persist dismissed version");}destroyTooltip();if(modalRoot)modalRoot->setVisible(false);pendingClose=true;}
inline void previousVersion(MyGUI::Widget*){selectedVersion=MainMenuNewsRules::navigate(selectedVersion,-1,MainMenuNewsRules::versionCount());pendingNavigation=true;}
inline void nextVersion(MyGUI::Widget*){selectedVersion=MainMenuNewsRules::navigate(selectedVersion,1,MainMenuNewsRules::versionCount());pendingNavigation=true;}
inline void checkPressed(MyGUI::Widget* sender){MyGUI::Button* b=sender->castType<MyGUI::Button>(false);if(b)b->setStateSelected(!b->getStateSelected());}
inline void showSummary(MyGUI::Widget*){destroyTooltip();if(summaryPanel)summaryPanel->setVisible(true);if(changelogPanel)changelogPanel->setVisible(false);if(dismissCheck)dismissCheck->setVisible(true);if(continueButton)continueButton->setVisible(true);if(changelogButton)changelogButton->setVisible(true);if(returnButton)returnButton->setVisible(false);}
inline void showChangelog(MyGUI::Widget*){destroyTooltip();if(summaryPanel)summaryPanel->setVisible(false);if(changelogPanel)changelogPanel->setVisible(true);if(dismissCheck)dismissCheck->setVisible(false);if(continueButton)continueButton->setVisible(false);if(changelogButton)changelogButton->setVisible(false);if(returnButton)returnButton->setVisible(true);}
inline void hideTooltip(MyGUI::Widget*,MyGUI::Widget*){destroyTooltip();}
inline void showTooltip(MyGUI::Widget* sender,MyGUI::Widget*){
    destroyTooltip();int index=std::atoi(sender->getUserString("newsIndex").c_str());if(index<0||index>=pageItemCount())return;
    const MyGUI::IntSize& screen=MyGUI::RenderManager::getInstance().getViewSize();int width=std::min(390,screen.width-24);std::string description=Loc::text(pageItems()[index].tooltipText);int lines=3+(int)description.size()/48,height=std::min(250,92+lines*18);
    MyGUI::IntCoord a=sender->getAbsoluteCoord();MainMenuNewsRules::Rect placed=MainMenuNewsRules::placeTooltip(MainMenuNewsRules::Rect(a.left,a.top,a.width,a.height),width,height,screen.width,screen.height);
    tooltip=MyGUI::Gui::getInstance().createWidget<MyGUI::Widget>("TheMercenarie_Panel",placed.x,placed.y,placed.w,placed.h,MyGUI::Align::Default,"ToolTip","MercenarieNewsTooltip");tooltip->setNeedMouseFocus(false);border(tooltip,MyGUI::Colour(.90f,.43f,.08f));
    MyGUI::TextBox* title=text(tooltip,18,12,width-36,32,20,Loc::text(pageItems()[index].tooltipTitle),MyGUI::Colour(.95f,.55f,.15f));title->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
    MyGUI::EditBox* body=wrappedText(tooltip,18,50,width-36,height-62,17,description.c_str(),MyGUI::Colour(.91f,.87f,.77f));body->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
}

#include "MainMenuNewsView.h"

inline bool tryCreateModal(){
    try{selectedVersion=MainMenuNewsRules::versionCount()-1;createModal();automaticHandled=true;creationFailureLogged=false;return true;}
    catch(const std::exception& e){if(!creationFailureLogged)ErrorLog(std::string("Mercenarie news: popup creation failed: ")+e.what());}
    catch(...){if(!creationFailureLogged)ErrorLog("Mercenarie news: popup creation failed (unknown exception)");}
    creationFailureLogged=true;destroyModal();nextCreationAttempt=GetTickCount()+5000;return false;
}
inline void manualPressed(MyGUI::Widget*){if(modalRoot)return;if(tryCreateModal()&&manualButton)manualButton->setVisible(false);}
inline void ensureManual(){if(manualButton)return;const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();lastWidth=v.width;lastHeight=v.height;manualButton=MyGUI::Gui::getInstance().createWidget<MyGUI::Button>("TheMercenarie_Button",18,v.height-50,300,34,MyGUI::Align::Left|MyGUI::Align::Bottom,"Info","MercenarieNewsManual");MercenarieFonts::caption(manualButton,Loc::text("news.manual_open"));manualButton->setFontHeight(15);manualButton->eventMouseButtonClick+=MyGUI::newDelegate(manualPressed);}

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

}
