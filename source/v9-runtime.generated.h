std::string serializeContractBoards()
    {
        std::ostringstream out;
        out.imbue(std::locale::classic());out<<std::setprecision(17)<<"@version|3\n";
        out<<"@rerolls|"<<guildRerolls.encode()<<'\n';
        for(std::map<std::string,CityContractBoard>::const_iterator it=savedContractBoards.begin();it!=savedContractBoards.end();++it){out<<"@board|"<<cleanBoardField(it->first)<<'|'<<it->second.expiresAt<<'\n';for(int i=0;i<6;++i){const BoardOffer& o=it->second.offers[i];out<<"@offer|"<<cleanBoardField(it->first)<<'|'<<i<<'|'<<o.missionType<<'|'<<o.tierIndex<<'|'<<cleanBoardField(o.tier)<<'|'<<cleanBoardField(o.townId)<<'|'<<cleanBoardField(o.townName)<<'|'<<cleanBoardField(o.profile)<<'|'<<cleanBoardField(o.story)<<'|'<<cleanBoardField(o.squadId)<<'|'<<cleanBoardField(o.rarityName)<<'|'<<o.distance<<'|'<<o.danger<<'|'<<o.estimatedPay<<'|'<<o.rarity<<'|'<<o.groupSize<<'|'<<(o.available?1:0)<<'|'<<(o.prestigious?1:0)<<'|'<<o.source<<'|'<<o.dangerLevel<<'|'<<o.environmentTags<<'|'<<o.caravanSize<<'|'<<o.cargoClass<<'|'<<o.studyClass<<'|'<<o.studyDuration<<'|'<<cleanBoardField(o.routeRegions)<<'\n';}}
        if(!out.good())throw std::runtime_error("boards serialization failed");SaveText::boards(out.str());return out.str();
    }
std::string cleanBoardField(std::string value){std::replace(value.begin(),value.end(),'|','/');std::replace(value.begin(),value.end(),'\n',' ');return value;}
void saveContractBoards(){if(savePreparing||MercenarieCleanup::disabled||contractBoardsFile.empty())return;atomicMissionFile(contractBoardsFile,serializeContractBoards());}
void loadContractBoards()
    {
        try{
        if(contractBoardsLoaded)return;contractBoardsLoaded=true;savedContractBoards.clear();guildRerolls=ContractReroll::Charges();std::string bytes=readMissionFile(contractBoardsFile);SaveText::boards(bytes);std::istringstream in(bytes);std::string line;
        while(std::getline(in,line)){std::stringstream s(line);std::string tag,city,v;std::getline(s,tag,'|');std::getline(s,city,'|');if(tag=="@rerolls"){std::string rest;std::getline(s,rest);if(!guildRerolls.decode(city+"|"+rest)){guildRerolls.remaining=0;guildRerolls.started=currentGameHours;guildRerolls.ends=currentGameHours+24;ErrorLog("V9 invalid reroll state: fail closed");}continue;}if(tag=="@version")continue;if(tag=="@board"){std::getline(s,v,'|');savedContractBoards[city].expiresAt=atof(v.c_str());continue;}if(tag!="@offer")continue;std::string fields[25];for(int i=0;i<25;++i)std::getline(s,fields[i],'|');int index=atoi(fields[0].c_str());if(index<0||index>=6)continue;BoardOffer& o=savedContractBoards[city].offers[index];o.missionType=atoi(fields[1].c_str());o.tierIndex=atoi(fields[2].c_str());o.tier=fields[3];o.townId=fields[4];o.townName=fields[5];o.profile=fields[6];o.story=fields[7];o.squadId=fields[8];o.rarityName=fields[9];o.distance=std::max(0.0f,static_cast<float>(atof(fields[10].c_str())));o.danger=static_cast<float>(atof(fields[11].c_str()));o.estimatedPay=std::max(0,atoi(fields[12].c_str()));o.rarity=std::max(0,std::min(2,atoi(fields[13].c_str())));o.groupSize=std::max(1,std::min(EscortConfig::MaxCaravanMembers,atoi(fields[14].c_str())));o.available=atoi(fields[15].c_str())!=0;o.prestigious=atoi(fields[16].c_str())!=0;if(!fields[17].empty()){o.source=atoi(fields[17].c_str());o.dangerLevel=std::max(1,std::min(5,atoi(fields[18].c_str())));o.environmentTags=atoi(fields[19].c_str());o.caravanSize=atoi(fields[20].c_str());o.cargoClass=atoi(fields[21].c_str());o.studyClass=atoi(fields[22].c_str());o.studyDuration=atoi(fields[23].c_str());o.routeRegions=fields[24];}else{o.source=city.find("#GUILD_HOUSE")!=std::string::npos?MCS_GUILD_HOUSE:MCS_TAVERN;o.dangerLevel=o.danger>=1.8f?5:o.danger>=1.48f?4:o.danger>=1.25f?3:o.danger>=1.08f?2:1;o.rarity=o.rarity>1?MCR_EPIC:o.rarity>0?MCR_RARE:MCR_COMMON;}if(o.rarity==MCR_LEGENDARY)o.rarity=MCR_EPIC;
            refreshOfferRegions(o);
            // Expire invalid unaccepted legacy offers without touching active quests.
            if(o.available&&(ContractDestinationRules::blocked(o.townId)||(o.missionType==MCT_SCIENCE&&!ContractDestinationRules::scientific(o.townId)))){o.available=false;savedContractBoards[city].expiresAt=0;}
        }
    
        }catch(const std::exception& e){progressWriteBlocked=true;progressLoadFault=true;reportPersistenceException("LOAD",e,contractBoardsFile);}
    }
void resetContractBoardWorld(){
        savedContractBoards.clear();contractBoardsLoaded=false;guildRerolls=ContractReroll::Charges();
        currentBoardKey.clear();for(int i=0;i<6;++i)boardOffers[i]=BoardOffer();
        currentGameHours=0;developerTimeOffsetHours=0;resetV9PendingActions();
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