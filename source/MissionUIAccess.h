#pragma once
// Included after persistence: shell access must precede gameplay restore gates.
namespace {
bool mercenarieUIKeyCaptureAtFrameStart=false;
bool mercenarieGameplayUnavailable(){
    return missionWorldChanging||missionRestorePending||progressLoadFault||progressWriteBlocked||activeMercenarieSaveSlot.empty();
}
void showMercenarieUnavailable(){
    MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();if(!gui)return;
    MyGUI::Window* window=gui->findWidget<MyGUI::Window>("MercenarieUnavailable",false);
    if(!window){
        const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
        const int width=std::min(620,view.width-24);
        window=gui->createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-width)/2,80,width,230,MyGUI::Align::Default,"Window","MercenarieUnavailable");
        window->eventWindowButtonPressed+=MyGUI::newDelegate(closeMercenarieInterface);
        MercenarieFonts::caption(window,Loc::text("ui.the_mercenarie_guild_management"));
        MyGUI::Widget* client=window->getClientWidget();
        MyGUI::EditBox* text=client->createWidget<MyGUI::EditBox>("Kenshi_WordWrapEmpty",18,18,client->getWidth()-36,client->getHeight()-36,MyGUI::Align::Stretch,"MercenarieUnavailableText");
        text->setEditStatic(true);text->setEditReadOnly(true);text->setEditMultiLine(true);text->setEditWordWrap(true);text->setFontHeight(18);
    }
    MyGUI::EditBox* text=gui->findWidget<MyGUI::EditBox>("MercenarieUnavailableText",false);
    if(text)MercenarieFonts::caption(text,Loc::text(missionRestorePending?
        "ui.guild_mission_waiting_for_the_saved_group_to_load":
        "ui.guild_progress_unavailable_guild_actions_paused_to_protect_your"));
    window->setVisible(true);
}
void updateMercenarieUIShell(GameWorld* world){
    MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();
    if(!world||!ou||!ou->player||!gui||missionWorldChanging||world->isLoadingFromASaveGame())return;
    // Named lookup does not dereference a cached widget or add a second delegate.
    trackerIcon=gui->findWidget<MyGUI::Button>("GuildEscortTrackerIcon",false);
    mercenarieLauncherMenu=gui->findWidget<MyGUI::Widget>("MercenarieLauncherMenu",false);
    if(trackerWindow&&gui->findWidget<MyGUI::Widget>("GuildEscortTrackerWindow",false)!=trackerWindow){
        trackerWindow=0;trackerEntry=0;bountyTrackerEntry=0;resetQuestTrackerView();
    }
    if(guildWindow&&gui->findWidget<MyGUI::Widget>("GuildContractsMenu",false)!=guildWindow)rebuildGuildMenuForViewport();
    if(MainMenuNews::mainMenuReady()){
        if(trackerIcon)trackerIcon->setVisible(false);
        if(mercenarieLauncherMenu)mercenarieLauncherMenu->setVisible(false);
        return;
    }
    const bool rebuilt=!trackerIcon||!mercenarieLauncherMenu;
    initialiseTrackerUI();
    mercenarieUIKeyCaptureAtFrameStart=guildKeyCapture;
    updateGuildMenuHotkey();
    const bool blocked=mercenarieGameplayUnavailable();
    MyGUI::Window* notice=gui->findWidget<MyGUI::Window>("MercenarieUnavailable",false);
    if(notice&&!blocked)gui->destroyWidget(notice);
    static int previous=-1;static unsigned long previousSession=~0UL;
    const int state=(missionRestorePending?1:0)|(progressWriteBlocked?2:0)|(progressLoadFault?4:0)|(trackerIcon?8:0)|(key&&key->keyboard?16:0);
    if(state!=previous||rebuilt||previousSession!=mercenarieUISession){previous=state;previousSession=mercenarieUISession;std::ostringstream out;
        out<<"MercenarieUI lifecycle: event=shell gameState=in-game myguiReady=1 launcherExpected=1 launcherActuallyExists="<<(trackerIcon!=0)
            <<" hotkeyMode=poll hotkeyAvailable="<<(key&&key->keyboard)<<" restorePending="<<missionRestorePending
            <<" sessionGeneration="<<mercenarieUISession<<" rebuilt="<<rebuilt<<" slot="<<activeMercenarieSaveSlot
            <<" progressWriteBlocked="<<progressWriteBlocked<<" progressLoadFault="<<progressLoadFault;
        DebugLog(out.str());
    }
}
}
