#include "Localization.h"
#pragma once
namespace {
MyGUI::Button* developerFinishRows[5]={0};
int developerFinishSlots[5]={0};
std::string developerFinishIds[5];
int developerFinishCount=0;
bool developerFinishEligible(){return !missionPending&&!currentContract.settlementPaid&&(missionActive||contractLifecycle==CONTRACT_ACTIVE);}
void closeDeveloperFinishPicker(MyGUI::WidgetPtr){if(developerFinishPicker)developerFinishPicker->setVisible(false);}
void closeDeveloperFinishPickerWindow(MyGUI::Window*,const std::string&){closeDeveloperFinishPicker(0);}
void developerFinishPicked(MyGUI::WidgetPtr sender){
    if(!clientOptions.developerMode)return;
    if(!developerFinishPicker||!developerFinishPicker->getVisible())return;
    for(int i=0;i<developerFinishCount;++i)if(sender==developerFinishRows[i]){
        const int previous=selectedEscortQuest;
        selectEscortQuest(developerFinishSlots[i]);
        if(currentMissionFiscalId!=developerFinishIds[i]||!developerFinishEligible()){
            selectEscortQuest(previous);closeDeveloperFinishPicker(0);showDeveloperFinishPicker();return;
        }
        // CONTRACT_ACTIVE is the persisted lifecycle truth. Repair older saves
        // whose transient boolean was not synchronized before invoking the cheat.
        if(!missionActive&&contractLifecycle==CONTRACT_ACTIVE)missionActive=true;
        if(!escort){selectEscortQuest(previous);ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.this_group_is_not_loaded_move_closer_to_it"):Loc::text("ui.this_group_is_not_loaded_move_closer_to_it"),true);return;}
        closeDeveloperFinishPicker(0);developerFinishMission();return;
    }
}
void showDeveloperFinishPicker(){
    if(!clientOptions.developerMode)return;
    if(!ou||!MyGUI::Gui::getInstancePtr())return;
    // Never change the owner of an open payment/negotiation panel.
    if(questPanelLocked()){ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.close_the_contract_or_report_window_first"):Loc::text("ui.close_the_contract_or_report_window_first"),true);return;}
    closeDeveloperFinishPicker(0);
    const int previous=selectedEscortQuest;developerFinishCount=0;
    std::string labels[5];
    for(int slot=0;slot<maximumActiveQuests;++slot){
        selectEscortQuest(slot);
        if(!developerFinishEligible())continue;
        int row=developerFinishCount++;developerFinishSlots[row]=slot;developerFinishIds[row]=currentMissionFiscalId;
        std::ostringstream label;label<<row+1<<". "<<(scientificMission?(gMercenarieEnglish?Loc::text("ui.expedition_c7bfee1"):Loc::text("ui.expedition_c7bfee1")):caravanMission?(gMercenarieEnglish?Loc::text("ui.caravan_879cb28"):Loc::text("ui.caravan_879cb28")):(gMercenarieEnglish?Loc::text("ui.escort_2e246e4"):Loc::text("ui.escort_2e246e4")))<<" | "<<originCity<<" > "<<destinationName;
        if(escort)label<<" | "<<escort->getName();labels[row]=label.str();
    }
    selectEscortQuest(previous);
    if(!developerFinishCount){ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.no_accepted_escort_caravan_or_expedition_to_finish"):Loc::text("ui.no_accepted_escort_caravan_or_expedition_to_finish"),true);return;}
    if(developerFinishCount==1){selectEscortQuest(developerFinishSlots[0]);developerFinishMission();return;}
    if(!developerFinishPicker){
        const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();int w=std::min(1000,view.width-30),h=400;
        developerFinishPicker=MyGUI::Gui::getInstancePtr()->createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","MercenarieFinishPicker");
        developerFinishPicker->eventWindowButtonPressed+=MyGUI::newDelegate(closeDeveloperFinishPickerWindow);
        MyGUI::Widget* parent=developerFinishPicker->getClientWidget();
        for(int i=0;i<5;++i){developerFinishRows[i]=parent->createWidget<MyGUI::Button>("Kenshi_Button1",16,20+i*54,w-50,46,MyGUI::Align::Default);developerFinishRows[i]->setFontHeight(16);developerFinishRows[i]->eventMouseButtonClick+=MyGUI::newDelegate(developerFinishPicked);}
        MyGUI::Button* cancel=parent->createWidget<MyGUI::Button>("Kenshi_Button1",16,300,w-50,40,MyGUI::Align::Default);MercenarieFonts::caption(cancel,gMercenarieEnglish?Loc::text("ui.cancel"):Loc::text("ui.cancel"));cancel->eventMouseButtonClick+=MyGUI::newDelegate(closeDeveloperFinishPicker);
    }
    MercenarieFonts::caption(developerFinishPicker,gMercenarieEnglish?Loc::text("ui.cheat_choose_the_mission_to_finish"):Loc::text("ui.cheat_choose_the_mission_to_finish"));
    for(int i=0;i<5;++i){developerFinishRows[i]->setVisible(i<developerFinishCount);if(i<developerFinishCount)MercenarieFonts::caption(developerFinishRows[i],labels[i]);}
    developerFinishPicker->setVisible(true);
}
}
