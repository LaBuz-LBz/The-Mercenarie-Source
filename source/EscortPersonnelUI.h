#pragma once
#include <kenshi/gui/OrdersPanel.h>
MyGUI::ImageBox* personnelDesignTick=0;
MyGUI::Button* personnelOption=0;MyGUI::Button* personnelBasicOption=0;
MyGUI::Window* personnelPicker=0;std::vector<hand> personnelCandidates;std::vector<MyGUI::Button*> personnelRows;std::vector<bool> personnelChosen;
int personnelReward=0,personnelQuest=-1;bool personnelGoodwill=false,personnelRequested=false;std::string personnelMission;
void resetPersonnelChoice(){personnelRequested=false;if(contractDecisionWindow&&personnelBasicOption)personnelBasicOption->setStateSelected(false);if(contractDecisionWindow&&basicContractTick)basicContractTick->setVisible(false);}
bool personnelDesignCheckbox=false;
void personnelOptionClicked(MyGUI::Widget* sender){personnelRequested=!personnelRequested;static_cast<MyGUI::Button*>(sender)->setStateSelected(personnelRequested);if(personnelDesignCheckbox&&sender==personnelOption&&personnelDesignTick)personnelDesignTick->setVisible(personnelRequested);}
void addPersonnelOption(MyGUI::Widget* parent,int y,int width,bool basic){MyGUI::Button* b=parent->createWidget<MyGUI::Button>("Kenshi_Button1",12,y,width-24,32,MyGUI::Align::Default);
 MercenarieFonts::caption(b,Loc::text("staff.assign"));b->setStateSelected(personnelRequested);b->eventMouseButtonClick+=MyGUI::newDelegate(personnelOptionClicked);if(basic)personnelBasicOption=b;else personnelOption=b;}
void basicPersonnelClicked(MyGUI::Widget* sender){personnelOptionClicked(sender);if(basicContractTick)basicContractTick->setVisible(personnelRequested);}
void addBasicPersonnelDesign(MyGUI::Widget* parent){
 personnelBasicOption=bcButton(parent,96,691,52,53,26,"",negotiationGold);personnelBasicOption->setStateSelected(personnelRequested);personnelBasicOption->eventMouseButtonClick+=MyGUI::newDelegate(basicPersonnelClicked);
 basicContractTick=personnelBasicOption->createWidget<MyGUI::ImageBox>("ImageBox",0,0,personnelBasicOption->getWidth(),personnelBasicOption->getHeight(),MyGUI::Align::Stretch);basicContractTick->setImageTexture("MercenarieBasicReference76.png");basicContractTick->setImageCoord(MyGUI::IntCoord(96,691,52,53));basicContractTick->setNeedMouseFocus(false);basicContractTick->setVisible(personnelRequested);
}
void personnelClosePicker(){if(personnelPicker)personnelPicker->setVisible(false);}
void personnelPickerCancel(MyGUI::Widget*){personnelClosePicker();if(negotiationWindow)negotiationWindow->setEnabled(true);if(contractDecisionWindow)contractDecisionWindow->setEnabled(true);}
void closePersonnelDialog(){personnelPickerCancel(0);}
void personnelPickerWindowClose(MyGUI::Window*,const std::string&){personnelPickerCancel(0);}
void personnelRowClicked(MyGUI::Widget* sender);
void personnelPickerConfirm(MyGUI::Widget* sender);
#include "EscortPersonnelPickerView.h"
void personnelRowClicked(MyGUI::Widget* sender){for(size_t i=0;i<personnelRows.size();++i)if(personnelRows[i]==sender){personnelChosen[i]=!personnelChosen[i];EscortPersonnel::choose(personnelSelectionOrder,i,personnelChosen[i]);personnelRefreshPicker();break;}}
void personnelPickerConfirm(MyGUI::Widget*){
 if(selectedEscortQuest!=personnelQuest||currentMissionFiscalId!=personnelMission||!missionPending||!negotiationOpen){personnelPickerCancel(0);return;}
 std::vector<EscortPersonnel::Member> assigned;
 for(size_t slot=0;slot<personnelSelectionOrder.size();++slot){size_t i=personnelSelectionOrder[slot];Character* c=personnelCandidates[i].getCharacter();if(!personnelSameTown(c)||personnelAssignment(c)>=0){ou->showPlayerAMessage(Loc::text("staff.changed"),true);return;}assigned.push_back(EscortPersonnel::Member(c->getHandle().toString()));}
 if(assigned.empty()){ou->showPlayerAMessage(Loc::text("staff.empty"),true);return;}
 personnelPickerCancel(0);EscortPersonnel::write(currentContract.routeRegions,assigned);EscortPersonnel::writeFormation(currentContract.routeRegions,personnelFormationId);acceptNegotiatedContractFinal(personnelReward,personnelGoodwill);
 if(missionActive)for(size_t i=0;i<assigned.size();++i)personnelSetGuard(personnelPlayer(assigned[i].id),true);
}
bool requestPersonnelSelection(int reward,bool goodwill){
 if(!personnelRequested)return false;
 if(!caravanNativeFormationReady){ou->showPlayerAMessage(Loc::text("staff.unavailable"),true);return true;}
 if(personnelPicker&&personnelPicker->getVisible())return true;
 if(personnelPicker)mercenarieDestroyLiveWidget(personnelPicker);personnelPicker=0;personnelCandidates.clear();personnelRows.clear();personnelChosen.clear();
 personnelReward=reward;personnelGoodwill=goodwill;personnelQuest=selectedEscortQuest;personnelMission=currentMissionFiscalId;
 personnelBuildPicker();
 if(negotiationWindow)negotiationWindow->setEnabled(false);if(contractDecisionWindow)contractDecisionWindow->setEnabled(false);return true;
}
struct PersonnelPanel{MyGUI::Button* button;MyGUI::IntCoord original;hand actor;bool split;int fontHeight;std::string fontName;PersonnelPanel():button(0),split(false),fontHeight(0){}};
std::map<OrdersPanel*,PersonnelPanel> personnelPanels;
void personnelGuardClicked(MyGUI::Widget* sender){for(std::map<OrdersPanel*,PersonnelPanel>::iterator i=personnelPanels.begin();i!=personnelPanels.end();++i)if(i->second.button==sender){Character* c=i->second.actor.getCharacter();if(!c)return;bool enabled=!i->second.button->getStateSelected();personnelSetGuard(c,enabled);i->second.button->setStateSelected(enabled);return;}}
void (*personnelPanelOriginal)(OrdersPanel*,Character*)=0;
void personnelPanelHook(OrdersPanel* panel,Character* c){
 personnelPanelOriginal(panel,c);personnelSyncGuard(c);if(!panel||!panel->tauntCheckBox)return;PersonnelPanel& state=personnelPanels[panel];MyGUI::Button* taunt=panel->tauntCheckBox;
 bool visible=!MercenarieCleanup::disabled&&!missionWorldChanging&&!missionRestorePending&&personnelAssignment(c)>=0;
 if(!visible){if(state.split){std::string caption=taunt->getCaption().asUTF8();bool checked=taunt->getStateSelected();taunt->changeWidgetSkin("Kenshi_TickButton1Skin");taunt->setFontName(state.fontName);taunt->setFontHeight(state.fontHeight);taunt->setCaption(caption);taunt->setStateSelected(checked);taunt->setTextAlign(MyGUI::Align::Center);taunt->setCoord(state.original);state.split=false;}if(state.button)state.button->setVisible(false);return;}
 if(!state.split){state.original=taunt->getCoord();state.fontName=taunt->getFontName();state.fontHeight=taunt->getFontHeight();std::string caption=taunt->getCaption().asUTF8();bool checked=taunt->getStateSelected();taunt->changeWidgetSkin("Kenshi_Button1Skin");taunt->setFontName(state.fontName);taunt->setCaption(caption);taunt->setStateSelected(checked);taunt->setTextAlign(MyGUI::Align::Center);state.split=true;}
 if(!state.button){state.button=taunt->getParent()->createWidget<MyGUI::Button>("Kenshi_Button1",state.original,MyGUI::Align::Default);MercenarieFonts::caption(state.button,Loc::text("staff.guard"));state.button->eventMouseButtonClick+=MyGUI::newDelegate(personnelGuardClicked);}
 int half=(state.original.width-2)/2;taunt->setCoord(state.original.left,state.original.top,half,state.original.height);state.button->setCoord(state.original.left+half+2,state.original.top,state.original.width-half-2,state.original.height);state.button->setVisible(taunt->getVisible());state.actor=c->getHandle();
 int font=std::max(8,state.fontHeight-1);taunt->setFontHeight(font);
 while(font>8&&taunt->getTextSize().width>taunt->getTextRegion().width){--font;taunt->setFontHeight(font);}
 std::vector<EscortPersonnel::Member> list=EscortPersonnel::read(personnelMetadata(personnelAssignment(c)));bool on=false;for(size_t i=0;i<list.size();++i)if(list[i].id==c->getHandle().toString())on=list[i].enabled;state.button->setStateSelected(on);
}
void (*personnelPanelDestroyOriginal)(OrdersPanel*)=0;
void personnelPanelDestroyHook(OrdersPanel* panel){personnelPanels.erase(panel);personnelPanelDestroyOriginal(panel);}
void personnelManualSelected(PlayerInterface* player){
 if(!player)return;for(unsigned int i=0;i<player->playerCharacters.size();++i){Character* c=player->playerCharacters[i];if(c&&player->isObjectSelected(c))personnelManualOrder(c);}
}
void (*personnelInputOrderOriginal)(PlayerInterface*,Building*,TaskType,RootObject*,bool,bool,const Ogre::Vector3&)=0;
void personnelInputOrderHook(PlayerInterface* player,Building* building,TaskType type,RootObject* subject,bool shift,bool add,const Ogre::Vector3& point){
 personnelManualSelected(player);personnelInputOrderOriginal(player,building,type,subject,shift,add,point);
}
void (*personnelInputMoveOriginal)(Character*,Building*,RootObject*,const Ogre::Vector3&)=0;
void personnelInputMoveHook(Character* c,Building* b,RootObject* target,const Ogre::Vector3& point){personnelManualOrder(c);personnelInputMoveOriginal(c,b,target,point);}
void (*personnelInputStopOriginal)(PlayerInterface*)=0;
void personnelInputStopHook(PlayerInterface* player){personnelManualSelected(player);personnelInputStopOriginal(player);}
void (*personnelInputStandingOriginal)(PlayerInterface*,MessageForB::StandingOrder)=0;
void personnelInputStandingHook(PlayerInterface* player,MessageForB::StandingOrder order){
 if(order==MessageForB::M_SET_ORDER_HOLD||order==MessageForB::M_SET_ORDER_PASSIVE||order==MessageForB::M_SET_ORDER_CHASE)personnelManualSelected(player);
 personnelInputStandingOriginal(player,order);
}
void installPersonnelUI(){
 if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PlayerInterface::addOrderSelectedCharacters),&personnelInputOrderHook,&personnelInputOrderOriginal))ErrorLog("PERSONNEL player order hook unavailable");
 if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::_NV_playerMoveOrderDefault),&personnelInputMoveHook,&personnelInputMoveOriginal))ErrorLog("PERSONNEL player move hook unavailable");
 if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PlayerInterface::stopCharactersMovement),&personnelInputStopHook,&personnelInputStopOriginal))ErrorLog("PERSONNEL player stop hook unavailable");
 if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PlayerInterface::setOrderSelectedCharacters),&personnelInputStandingHook,&personnelInputStandingOriginal))ErrorLog("PERSONNEL player standing order hook unavailable");
 if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&OrdersPanel::update),&personnelPanelHook,&personnelPanelOriginal))ErrorLog("PERSONNEL orders panel hook unavailable");
 if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&OrdersPanel::_DESTRUCTOR),&personnelPanelDestroyHook,&personnelPanelDestroyOriginal))ErrorLog("PERSONNEL orders panel destructor hook unavailable");
}

void addPersonnelDesign(MyGUI::Widget* parent,int width){
 personnelDesignCheckbox=true;
 personnelOption=negotiationAction(parent,74,759,42,43,24,"",negotiationGold);
 personnelOption->setStateSelected(personnelRequested);personnelOption->eventMouseButtonClick+=MyGUI::newDelegate(personnelOptionClicked);
 personnelDesignTick=personnelOption->createWidget<MyGUI::ImageBox>("ImageBox",0,0,personnelOption->getWidth(),personnelOption->getHeight(),MyGUI::Align::Stretch);
 personnelDesignTick->setImageTexture("MercenarieNegotiationReference74.png");personnelDesignTick->setImageCoord(MyGUI::IntCoord(74,759,42,43));personnelDesignTick->setNeedMouseFocus(false);personnelDesignTick->setVisible(personnelRequested);
 negotiationLabel(parent,148,757,1039,43,28,Loc::text("staff.assign"),negotiationInk);
 negotiationLabel(parent,149,806,1038,37,22,Loc::text("negotiation.design.staff_hint"),negotiationInk);
}
