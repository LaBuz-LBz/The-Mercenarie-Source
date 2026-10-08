#pragma once
#include "Localization.h"
#include "AutopilotRuntime.h"
#include "QuestTrackerText.h"
hand autopilotActor;
std::map<std::string,Ogre::Vector3> autopilotTrips;
std::vector<std::string> autopilotTowns; // Retained for save/load cleanup compatibility; UI uses value snapshots.
AutopilotDestinations::Model autopilotModel;
AutopilotDestinations::View autopilotView;
void launcherEnsureUi();
void launcherLogo(MyGUI::Widget*,int,int,int);
MyGUI::TextBox* launcherV9Text(MyGUI::Widget*,int,int,int,int,int,const std::string&,const MyGUI::Colour&,bool);
MyGUI::Widget* launcherV9Solid(MyGUI::Widget*,int,int,int,int,const MyGUI::Colour&);
int launcherWrap(MyGUI::TextBox*,const std::string&);
Character* selectedAutopilotActor(){
    if(!ou||!ou->player)return 0;
    Character* selected=0;
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){
        Character* c=ou->player->playerCharacters[i];
        if(c&&c->getFaction()==ou->player->getFaction()&&ou->player->isObjectSelected(c)){
            if(selected)return 0;selected=c;
        }
    }
    return selected;
}
void cancelAutopilot(MyGUI::Widget*){
    Character* actor=selectedAutopilotActor();if(!actor)return;
    std::map<std::string,Ogre::Vector3>::iterator trip=autopilotTrips.find(actor->getHandle().toString());
    if(trip==autopilotTrips.end())return;
    if(actor->getMovement()->getDestination().squaredDistance(trip->second)>100.0f){autopilotTrips.erase(trip);return;}
    actor->removeJob(MOVE_CUS_ORDERED);actor->getMovement()->halt();
    autopilotTrips.erase(trip);
    if(mercenarieAutopilotWindow)mercenarieAutopilotWindow->setVisible(false);
}

void closeAutopilot(MyGUI::Widget*){if(mercenarieAutopilotWindow)mercenarieAutopilotWindow->setVisible(false);}
void launchAutopilot(MyGUI::Widget*){
 Character* actor=selectedAutopilotActor();
 if(!actor||actor->getHandle()!=autopilotActor||actor->isDead()||actor->isBeingCarried()){
  if(ou)ou->showPlayerAMessage(Loc::text("ui.select_the_same_single_squad_member_and_reopen_autopilot"),true);return;
 }
 TownBase* town=resolveAutopilotPlace(autopilotModel.selected);
 if(!town){autopilotModel.selected.clear();return;}
 actor->addOrder(0,MOVE_CUS_ORDERED,0,false,true,town->getPosition());
 actor->getMovement()->setRoadPreference(1.0f);
 actor->getMovement()->setRoadDestination(town->getPosition());
 autopilotTrips[actor->getHandle().toString()]=town->getPosition();
 mercenarieAutopilotWindow->setVisible(false);
}
#include "AutopilotView.h"
