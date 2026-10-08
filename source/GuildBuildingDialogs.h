// Native building assignment dialogs. The existing office registry remains authoritative.
#include "GuildBuildingTypes.h"
MyGUI::Window* guildTypeWindow=0;
MyGUI::Window* guildManageWindow=0;
MyGUI::Window* guildAbandonWindow=0;
MyGUI::TextBox* guildTypeHint=0;
MyGUI::Widget* guildTypeTooltip=0;MyGUI::TextBox* guildTypeTooltipText=0;
bool guildNameIsRename=false;
void refreshOfficesView(bool force);
void removeGuildVisitor(Character*,bool);
void departFiscalParty();
void abandonGuildOffice(const std::string& key,Building* building);
void requestGuildHouseName();
void confirmGuildHouseName(MyGUI::WidgetPtr);
void gbNameKey(MyGUI::Widget*,MyGUI::KeyCode key,MyGUI::Char){if(key==MyGUI::KeyCode::Return)confirmGuildHouseName(0);}
Building* selectedPlayerBuilding();
std::string gbText(const char* key){return Loc::text(key);}
bool gbLive(MyGUI::Window* window){if(!window||!MyGUI::Gui::getInstancePtr())return false;MyGUI::EnumeratorWidgetPtr it=MyGUI::Gui::getInstance().getEnumerator();while(it.next())if(it.current()==window)return true;return false;}
void gbHide(MyGUI::Window* window){if(gbLive(window)){MyGUI::InputManager::getInstance().removeWidgetModal(window);window->setVisible(false);}}
void gbClose(MyGUI::Window*,const std::string&){gbHide(guildTypeWindow);gbHide(guildManageWindow);gbHide(guildAbandonWindow);gbHide(guildNameWindow);pendingGuildHouseKey.clear();pendingGuildHouseHandle.setNull();guildNameIsRename=false;}
void gbCancel(MyGUI::Widget*){gbClose(0,"");}
void gbWrap(MyGUI::TextBox* t,const std::string& text){std::istringstream lines(text);std::string line,out;while(std::getline(lines,line)){std::istringstream words(line);std::string word,row;while(words>>word){std::string next=row.empty()?word:row+" "+word;MercenarieFonts::caption(t,next);if(!row.empty()&&t->getTextSize().width>t->getWidth()){out+=row+"\n";row=word;}else row=next;}out+=row+"\n";}MercenarieFonts::caption(t,out);}
MyGUI::TextBox* gbLabel(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& caption,int font=18){MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",x,y,w,h,MyGUI::Align::Default);t->setFontHeight(font);t->setTextColour(MyGUI::Colour(.87f,.84f,.71f));t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setNeedMouseFocus(false);gbWrap(t,caption);return t;}
MyGUI::ImageBox* gbIcon(MyGUI::Widget* p,int slot,int x,int y,int size){MyGUI::ImageBox* icon=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);icon->setImageTexture("MercenarieBuildingIcons.png");icon->setImageCoord(MyGUI::IntCoord(slot*128,0,128,128));icon->setNeedMouseFocus(false);return icon;}
MyGUI::Button* gbButton(MyGUI::Widget* p,int x,int y,int w,int h,const char* key,void (*callback)(MyGUI::Widget*),bool danger=false){MyGUI::Button* b=p->createWidget<MyGUI::Button>("Kenshi_Button1",x,y,w,h,MyGUI::Align::Default);b->setFontHeight(17);MercenarieFonts::caption(b,Loc::text(key));b->setTextColour(danger?MyGUI::Colour(1,.28f,.18f):MyGUI::Colour(1,.65f,.20f));b->eventMouseButtonClick+=MyGUI::newDelegate(callback);if(danger){b->setColour(MyGUI::Colour(.8f,.18f,.12f));gbIcon(b,3,6,(h-24)/2,24);}return b;}
MyGUI::Window* gbWindow(MyGUI::Window*& window,int width,int height,const char* title){
 if(gbLive(window)){gbHide(window);MyGUI::Gui::getInstance().destroyWidget(window);}window=0;
 const MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();width=std::min(width,v.width-24);height=std::min(height,v.height-24);
 window=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(v.width-width)/2,(v.height-height)/2,width,height,MyGUI::Align::Default,"Popup");
 MercenarieFonts::caption(window,Loc::text(title));window->eventWindowButtonPressed+=MyGUI::newDelegate(gbClose);applyMercenarieFrame(window->getClientWidget(),false);MyGUI::InputManager::getInstance().addWidgetModal(window);return window;
}
std::string gbReason(Building* b){
 if(!b||b->isDestroyed()||!b->isThePlayer())return Loc::text("guild.pages.select_building");
 if(guildHouseNames.count(guildHouseKey(b)))return Loc::text("guild.pages.registered");
 int maximum=GuildBuildingTypes::limit(guildLevel());if(!maximum)return Loc::text("guild.pages.level3");
 if(guildHouseNames.size()>=size_t(maximum)){std::ostringstream s;s<<Loc::text("guild.pages.limit")<<" : "<<guildHouseNames.size()<<"/"<<maximum;return s.str();}
 TownBase* town=b->getCurrentTownLocation();if(!town||!town->isTown())return Loc::text("guild.pages.invalid_city");
 const std::string city=ReputationIdentity::key(frenchPlaceName(town->getName()));
 for(std::map<std::string,std::string>::const_iterator i=guildHouseCities.begin();i!=guildHouseCities.end();++i)if(guildHouseNames.count(i->first)&&ReputationIdentity::key(i->second)==city)return Loc::text("guild.pages.occupied");return "";
}
void gbSelectType(MyGUI::Widget*);
#include "GuildBuildingPickerView.h"
#include "GuildOfficeNameView.h"
void gbSelectType(MyGUI::Widget* sender){
 if(!sender||sender->getUserString("type")!="office")return;
 Building* b=pendingGuildHouseHandle.isNull()?0:pendingGuildHouseHandle.getBuilding();std::string reason=gbReason(b);if(!reason.empty()){if(gbpDescriptionBody)gbpWrap(gbpDescriptionBody,reason);return;}
 gbHide(guildTypeWindow);guildNameIsRename=false;requestGuildHouseName();
}
void requestGuildBuildingType(){
 Building* b=selectedPlayerBuilding();if(!b||guildHouseNames.count(guildHouseKey(b)))return;
 gbClose(0,"");pendingGuildHouseKey=guildHouseKey(b);pendingGuildHouseHandle=b;gbpEnsure();
 if(gbLive(guildTypeWindow))MyGUI::Gui::getInstance().destroyWidget(guildTypeWindow);
 const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();GuildBuildingPicker::Layout l(view.width,view.height);
 guildTypeWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("MercenarieBuildingWindow",(view.width-l.w)/2,(view.height-l.h)/2,l.w,l.h,MyGUI::Align::Default,"Popup");
 gbpDraw(guildTypeWindow->getClientWidget(),l,gbReason(b));MyGUI::InputManager::getInstance().addWidgetModal(guildTypeWindow);
}
void gbRename(MyGUI::Widget*){gbHide(guildManageWindow);guildNameIsRename=true;requestGuildHouseName();}
void gbConfirmAbandon(MyGUI::Widget*){const std::string key=pendingGuildHouseKey;Building* b=pendingGuildHouseHandle.isNull()?0:pendingGuildHouseHandle.getBuilding();gbClose(0,"");if(!key.empty()&&guildHouseNames.count(key))abandonGuildOffice(key,b);}
void gbAskAbandon(MyGUI::Widget*);
void gbActivate(MyGUI::Widget*){Building* b=pendingGuildHouseHandle.isNull()?0:pendingGuildHouseHandle.getBuilding();if(!b||b->isDestroyed()||!b->isThePlayer()||!guildHouseNames.count(pendingGuildHouseKey))return;designatedGuildHouseKey=pendingGuildHouseKey;designatedGuildHouseHandle=b;saveReputations();gbClose(0,"");refreshGuildFurniture(false);refreshOfficesView(true);}
#include "GuildOfficeManageView.h"
#include "GuildOfficeAbandonView.h"
void gbAskAbandon(MyGUI::Widget*){gbHide(guildManageWindow);gbpEnsure();if(gbLive(guildAbandonWindow)){gbHide(guildAbandonWindow);MyGUI::Gui::getInstance().destroyWidget(guildAbandonWindow);}const MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();GuildAbandonLayout l(v.width,v.height);guildAbandonWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("MercenarieBuildingWindow",(v.width-l.w)/2,(v.height-l.h)/2,l.w,l.h,MyGUI::Align::Default,"Popup");gbaDraw(guildAbandonWindow->getClientWidget(),l);MyGUI::InputManager::getInstance().addWidgetModal(guildAbandonWindow);}
void requestGuildOfficeManagement(){gbpEnsure();if(gbLive(guildManageWindow)){gbHide(guildManageWindow);MyGUI::Gui::getInstance().destroyWidget(guildManageWindow);}const MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();GuildManageLayout l(v.width,v.height);guildManageWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("MercenarieBuildingWindow",(v.width-l.w)/2,(v.height-l.h)/2,l.w,l.h,MyGUI::Align::Default,"Popup");gbmDraw(guildManageWindow->getClientWidget(),l,guildHouseNames[pendingGuildHouseKey],pendingGuildHouseKey==designatedGuildHouseKey);MyGUI::InputManager::getInstance().addWidgetModal(guildManageWindow);}

bool gbModalOpen(){return (gbLive(guildTypeWindow)&&guildTypeWindow->getVisible())||(gbLive(guildNameWindow)&&guildNameWindow->getVisible())||(gbLive(guildManageWindow)&&guildManageWindow->getVisible())||(gbLive(guildAbandonWindow)&&guildAbandonWindow->getVisible());}
bool gbKeyboard(){static bool wasEscape=false;if(!key||!key->keyboard)return false;bool consumed=false;bool down=key->keyboard->isKeyDown(OIS::KC_ESCAPE);if(down&&!wasEscape&&gbModalOpen()){gbClose(0,"");consumed=true;}wasEscape=down;return consumed;}

void gbRecenter(){static int width=0,height=0;const MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();if(width==v.width&&height==v.height)return;width=v.width;height=v.height;if(gbLive(guildTypeWindow)&&guildTypeWindow->getVisible()){GuildBuildingPicker::Layout l(width,height);guildTypeWindow->setSize(l.w,l.h);MyGUI::Widget* p=guildTypeWindow->getClientWidget();while(p->getChildCount())MyGUI::Gui::getInstance().destroyWidget(p->getChildAt(0));gbpDraw(p,l,gbReason(pendingGuildHouseHandle.isNull()?0:pendingGuildHouseHandle.getBuilding()));}if(gbLive(guildNameWindow)&&guildNameWindow->getVisible()&&guildNameEdit){std::string draft=guildNameEdit->getCaption();size_t cursor=guildNameEdit->getTextCursor();GuildNameLayout l(width,height);guildNameWindow->setSize(l.w,l.h);MyGUI::Widget* p=guildNameWindow->getClientWidget();gbnFieldFrame=0;while(p->getChildCount())MyGUI::Gui::getInstance().destroyWidget(p->getChildAt(0));gbnDraw(p,l,guildNameIsRename);guildNameEdit->setCaption(draft);guildNameEdit->setTextCursor(cursor);MyGUI::InputManager::getInstance().setKeyFocusWidget(guildNameEdit);}if(gbLive(guildManageWindow)&&guildManageWindow->getVisible()){GuildManageLayout l(width,height);guildManageWindow->setSize(l.w,l.h);MyGUI::Widget* p=guildManageWindow->getClientWidget();while(p->getChildCount())MyGUI::Gui::getInstance().destroyWidget(p->getChildAt(0));gbmDraw(p,l,guildHouseNames[pendingGuildHouseKey],pendingGuildHouseKey==designatedGuildHouseKey);}MyGUI::Window* windows[]={guildTypeWindow,guildNameWindow,guildManageWindow,guildAbandonWindow};for(int i=0;i<4;++i)if(gbLive(windows[i]))windows[i]->setPosition(std::max(0,(width-windows[i]->getWidth())/2),std::max(0,(height-windows[i]->getHeight())/2));}
