// Included after both persistent offer adapters.
std::string rerollPendingId;
bool rerollPendingClassic=false;
MissionBookPoolEntry rerollPendingEntry;
double rerollPendingDeadline=0;
unsigned int v9WorldEpoch=0,rerollPendingEpoch=0,warPendingEpoch=0;
int warPendingIndex=-1;bool warPendingPeace=false;
MyGUI::Window* diplomacyWindow=0;
std::string rerollPreservedDestination;
MyGUI::Window* v9Confirmation=0;
void (*v9ConfirmationCallback)(int)=0;
bool v9WidgetLive(MyGUI::Widget* w){MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();return w&&gui&&mercenarieFindLiveWidget(gui->getEnumerator(),w);}
void hideV9Confirmation(){if(v9WidgetLive(v9Confirmation)){MyGUI::InputManager::getInstance().removeWidgetModal(v9Confirmation);v9Confirmation->setVisible(false);}else v9Confirmation=0;v9ConfirmationCallback=0;}
void resetV9PendingActions(){gbClose(0,"");++v9WorldEpoch;rerollPendingId.clear();rerollPendingClassic=false;rerollPendingEntry=MissionBookPoolEntry();warPendingIndex=-1;hideV9Confirmation();if(v9WidgetLive(diplomacyWindow))diplomacyWindow->setVisible(false);else diplomacyWindow=0;}
void v9Message(const char* key){if(ou)ou->showPlayerAMessage(Loc::text(key),true);}
void answerV9Confirmation(MyGUI::Widget* sender){void (*callback)(int)=v9ConfirmationCallback;int answer=sender?atoi(sender->getUserString("answer").c_str()):0;hideV9Confirmation();if(callback)callback(answer);}
void closeV9Confirmation(MyGUI::Window*,const std::string&){answerV9Confirmation(0);}
void confirmReroll(int);
#include "RerollPopupView.h"
void v9Confirm(const std::string& title,const std::string& body,const char* action,void (*callback)(int)){
    if(v9WidgetLive(bookRerollTip))bookRerollTip->setVisible(false);
    hideV9Confirmation();mercenarieDestroyLiveWidget(v9Confirmation);v9Confirmation=0;
    rerollPopupActive=callback==confirmReroll;
    if(rerollPopupActive){
        if(!MyGUI::ResourceManager::getInstance().isExist("MercenariePopupWindow"))MyGUI::ResourceManager::getInstance().load("MercenariePopup.xml");
        v9Confirmation=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("MercenariePopupWindow",0,0,900,460,MyGUI::Align::Default,"Popup");
        layoutRerollPopup();v9ConfirmationCallback=callback;
        MyGUI::InputManager::getInstance().addWidgetModal(v9Confirmation);return;
    }
    const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();int w=std::min(900,view.width-40),h=std::min(body.size()>320?520:340,view.height-40);
    v9Confirmation=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window");
    MercenarieFonts::caption(v9Confirmation,Loc::text("v9.confirm"));v9Confirmation->eventWindowButtonPressed+=MyGUI::newDelegate(closeV9Confirmation);
    MyGUI::Widget* parent=v9Confirmation->getClientWidget();applyMercenarieFrame(parent,true);
    MyGUI::TextBox* text=registerText(parent,20,20,parent->getWidth()-40,parent->getHeight()-100,21,body,registerIvory);text->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);bookWrap(text,title+"\n\n"+body);
    for(int i=0;i<2;++i){MyGUI::Button* button=parent->createWidget<MyGUI::Button>("Kenshi_Button1",20+i*(parent->getWidth()-30)/2,parent->getHeight()-64,(parent->getWidth()-50)/2,48,MyGUI::Align::Default);MercenarieFonts::caption(button,Loc::text(i?"v9.cancel":action));button->setFontHeight(20);button->setUserString("answer",i?"0":"1");button->eventMouseButtonClick+=MyGUI::newDelegate(answerV9Confirmation);}
    v9ConfirmationCallback=callback;MyGUI::InputManager::getInstance().addWidgetModal(v9Confirmation);
}
std::string rerollTooltip(){
    loadContractBoards();guildRerolls.refresh(currentGameHours);
    Loc::Catalogue args;args["remaining"]=registerNumber(guildRerolls.remaining);
    args["hours"]=registerNumber(guildRerolls.hoursLeft(currentGameHours));
    std::string tip=Loc::format("v9.reroll.tooltip",args);
    if(guildRerolls.ends>=0)tip+="\n"+Loc::format("v9.reroll.wait",args);
    return tip;
}
void confirmReroll(int answer){
    const std::string id=rerollPendingId;rerollPendingId.clear();
    const bool classic=rerollPendingClassic;rerollPendingClassic=false;
    if(answer!=1||id.empty()||rerollPendingEpoch!=v9WorldEpoch||(!classic&&!missionBookPoolReady)||!contractsWindow||!contractsWindow->getVisible())return;
    loadContractBoards();guildRerolls.refresh(currentGameHours);
    if(guildRerolls.remaining<=0){v9Message("v9.reroll.empty");return;}
    size_t index=missionBookPool.size();MissionBookPoolEntry entry;
    if(classic){
        entry=rerollPendingEntry;
        std::map<std::string,CityContractBoard>::const_iterator board=savedContractBoards.find(entry.boardKey);
        if(missionBookDelegationContext||currentBoardKey!=entry.boardKey||contractBarmanHandle.toString()!=entry.issuer.toString()||entry.sourceIndex<0||entry.sourceIndex>=6||board==savedContractBoards.end()||board->second.expiresAt!=rerollPendingDeadline||board->second.expiresAt<=currentGameHours){v9Message("v9.reroll.unavailable");return;}
        const BoardOffer& live=board->second.offers[entry.sourceIndex];
        if(!live.available||live.townId!=entry.offer.townId||live.missionType!=entry.offer.missionType||live.routeRegions!=entry.offer.routeRegions){v9Message("v9.reroll.unavailable");return;}
        selectedOffer=entry.sourceIndex;
    }else{
        for(size_t i=0;i<missionBookPool.size();++i)if(missionBookPool[i].id==id){index=i;break;}
        if(index==missionBookPool.size()){v9Message("v9.reroll.unavailable");return;}entry=missionBookPool[index];
    }
    Character* giver=entry.issuer.isNull()?0:entry.issuer.getCharacter();
    if(!giver||(classic?(!contractOriginTown||giver->getCurrentTownLocation()!=contractOriginTown):!missionBookSameTown(giver))){v9Message("v9.reroll.unavailable");return;}
    if(!classic){selectMissionBookEntry(index);if(!missionBookSelectionValid()){v9Message("v9.reroll.unavailable");return;}}
    if(entry.security){
        MercenarieV5::IssuerFaction faction;
        if(!MercenarieV5::bountyIssuer(giver,faction)||bountyWorld.suspended){v9Message("v9.reroll.unavailable");return;}
        std::map<std::string,MercenarieV5::BountyBoard>::iterator board=bountyWorld.boards.find(entry.issuer.toString());
        if(board==bountyWorld.boards.end()||board->second.refreshDue(currentGameHours))return;
        BountyRandom random;
        std::vector<MercenarieV5::BountyOffer> candidates=MercenarieV5::makeBountyOffers(entry.issuer.toString(),MercenarieV5::bountyIdentity(entry.issuer),faction,board->second.rotation,random);
        bool found=false;MercenarieV5::BountyOffer replacement;
        for(size_t i=0;i<candidates.size();++i)if(chooseBountyArea(candidates[i],giver)){replacement=candidates[i];found=true;break;}
        if(!found){v9Message("v9.reroll.unavailable");return;}
        std::ostringstream revision;revision<<":reroll:"<<guildRerolls.sequence+1;replacement.id+=revision.str();
        if(!guildRerolls.consume(currentGameHours))return;
        board->second.offers[entry.sourceIndex]=replacement;
    }else{
        std::map<std::string,CityContractBoard>::iterator board=savedContractBoards.find(entry.boardKey);
        if(board==savedContractBoards.end()||board->second.expiresAt<=currentGameHours)return;
        CityContractBoard candidate;
        // Resolve this giver's real profile; do not inherit the last opened visitor.
        const int oldProfile=visitorOfferProfile;const bool oldUrgent=visitorOfferUrgent,oldVip=visitorOfferVip,oldExceptional=visitorOfferExceptional;
        rerollPreservedDestination=destinationTown?destinationTown:"";const std::string previousName=destinationNameStorage;const float previousDistance=selectedDistance;
        selectGuildVisitorOffer(giver);
        generateContractOffers(giver,candidate,entry.sourceIndex,1,true);
        destinationTown=rerollPreservedDestination.c_str();destinationNameStorage=previousName;destinationName=destinationNameStorage.c_str();selectedDistance=previousDistance;
        visitorOfferProfile=oldProfile;visitorOfferUrgent=oldUrgent;visitorOfferVip=oldVip;visitorOfferExceptional=oldExceptional;
        BoardOffer replacement=candidate.offers[entry.sourceIndex];
        if(!replacement.available){v9Message("v9.reroll.unavailable");return;}
        ContractReroll::mark(replacement.routeRegions,replacement.estimatedPay,guildRerolls.sequence+1);
        replacement.estimatedPay=ContractReroll::reward(replacement.estimatedPay,true);
        if(!guildRerolls.consume(currentGameHours))return;
        board->second.offers[entry.sourceIndex]=replacement;
        if(isGuildVisitor(giver))GuildVisitorOfferRules::remember(recentGuildVisitorOfferTypes,replacement.missionType);
    }
    saveContractBoards();contractAcceptArmed=false;delegationFinalArmed=false;delegationConfirming=false;
    if(classic){for(int i=0;i<6;++i)boardOffers[i]=savedContractBoards[entry.boardKey].offers[i];updateContractsBoard();}
    else{missionBookSelectedId.clear();refreshMissionBookContractHub();}
    v9Message("v9.reroll.success");
}
void rerollClassicRow(MyGUI::Widget* sender){
    if(!sender||missionBookDelegationContext||v9ConfirmationCallback||!contractsWindow||!contractsWindow->getVisible())return;
    const int slot=atoi(sender->getUserString("offerSlot").c_str());if(slot<0||slot>=6)return;
    loadContractBoards();guildRerolls.refresh(currentGameHours);if(guildRerolls.remaining<=0)return;
    std::map<std::string,CityContractBoard>::const_iterator board=savedContractBoards.find(currentBoardKey);
    if(board==savedContractBoards.end()||board->second.expiresAt<=currentGameHours||!board->second.offers[slot].available||contractBarmanHandle.isNull())return;
    rerollPendingEntry=MissionBookPoolEntry();rerollPendingEntry.boardKey=currentBoardKey;rerollPendingEntry.sourceIndex=slot;rerollPendingEntry.issuer=contractBarmanHandle;rerollPendingEntry.offer=board->second.offers[slot];
    rerollPendingDeadline=board->second.expiresAt;rerollPendingClassic=true;rerollPendingId=currentBoardKey;rerollPendingEpoch=v9WorldEpoch;
    v9Confirm(Loc::text("v9.reroll.confirm_title"),Loc::text("v9.reroll.confirm_body"),"v9.confirm",confirmReroll);
}
void rerollBookRow(MyGUI::Widget* sender){
    if(!sender||!missionBookPoolReady||v9ConfirmationCallback)return;
    size_t row=(size_t)atoi(sender->getUserString("bookRow").c_str());
    if(row>=missionBookPageEntries.size())return;
    loadContractBoards();guildRerolls.refresh(currentGameHours);if(guildRerolls.remaining<=0)return;
    rerollPendingClassic=false;rerollPendingId=missionBookPool[missionBookPageEntries[row]].id;rerollPendingEpoch=v9WorldEpoch;
    v9Confirm(Loc::text("v9.reroll.confirm_title"),Loc::text("v9.reroll.confirm_body"),"v9.confirm",confirmReroll);
}
void closeDiplomacy(MyGUI::Widget*){warPendingIndex=-1;if(diplomacyWindow)diplomacyWindow->setVisible(false);}
void closeDiplomacyWindow(MyGUI::Window*,const std::string&){closeDiplomacy(0);}
void confirmDiplomacy(int answer){
    const int index=warPendingIndex;warPendingIndex=-1;
    if(answer!=1||!clientOptions.developerMode||warPendingEpoch!=v9WorldEpoch)return;
    Faction* faction=diplomacyFaction(index);Faction* player=ou&&ou->player?ou->player->participant:0;
    if(!faction||!player||faction==player||!faction->relations||!player->relations){v9Message("v9.war.error");return;}
    // Reject a changed state instead of applying the opposite of the confirmed action.
    if(factionAtWar(faction)!=warPendingPeace){v9Message("v9.war.changed");openFactionDiplomacy(0);return;}
    if(warPendingPeace){faction->relations->setNoLongerEnemies(player);player->relations->setNoLongerEnemies(faction);}
    else{faction->relations->declareWar(player);player->relations->declareWar(faction);}
    bool success=factionAtWar(faction)!=warPendingPeace;
    v9Message(success?"v9.war.success":"v9.war.error");openFactionDiplomacy(0);
}
void chooseDiplomacy(MyGUI::Widget* sender){
    if(!clientOptions.developerMode||!sender||v9ConfirmationCallback)return;
    int index=atoi(sender->getUserString("faction").c_str());Faction* faction=diplomacyFaction(index);if(!faction)return;
    warPendingIndex=index;warPendingPeace=factionAtWar(faction);warPendingEpoch=v9WorldEpoch;
    Loc::Catalogue args;args["faction"]=faction->getName();
    v9Confirm(Loc::format(warPendingPeace?"v9.war.peace_confirm":"v9.war.declare_confirm",args),Loc::text("v9.war.confirm_body"),warPendingPeace?"v9.war.peace":"v9.war.declare",confirmDiplomacy);
}
void openFactionDiplomacy(MyGUI::Widget*){
    if(!clientOptions.developerMode||!ou||!ou->player||!ou->player->participant||!MyGUI::Gui::getInstancePtr())return;
    mercenarieDestroyLiveWidget(diplomacyWindow);diplomacyWindow=0;
    const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
    int width=std::min(900,view.width-30),height=std::min(700,view.height-30);
    diplomacyWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-width)/2,(view.height-height)/2,width,height,MyGUI::Align::Default,"Window");
    MercenarieFonts::caption(diplomacyWindow,Loc::text("v9.war.title"));diplomacyWindow->eventWindowButtonPressed+=MyGUI::newDelegate(closeDiplomacyWindow);
    MyGUI::Widget* parent=diplomacyWindow->getClientWidget();applyMercenarieFrame(parent,true);
    int step=(parent->getHeight()-66)/10;
    for(int i=0;i<10;++i){Faction* faction=diplomacyFaction(i);MyGUI::Button* button=parent->createWidget<MyGUI::Button>("Kenshi_Button1",16,8+i*step,parent->getWidth()-32,step-4,MyGUI::Align::Default);
        button->setFontHeight(20);button->setUserString("faction",registerNumber(i));button->setEnabled(faction&&faction!=ou->player->participant);
        MercenarieFonts::caption(button,faction?faction->getName()+" - "+Loc::text(factionAtWar(faction)?"v9.war.hostile":"v9.war.peaceful"):Loc::text("v9.war.unavailable"));
        button->eventMouseButtonClick+=MyGUI::newDelegate(chooseDiplomacy);
    }
    MyGUI::Button* close=parent->createWidget<MyGUI::Button>("Kenshi_Button1",16,parent->getHeight()-48,parent->getWidth()-32,40,MyGUI::Align::Default);MercenarieFonts::caption(close,Loc::text("v9.cancel"));close->eventMouseButtonClick+=MyGUI::newDelegate(closeDiplomacy);
}
