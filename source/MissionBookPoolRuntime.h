// Read-only presentation adapter. Filtering never opens a board or rotates offers.
Character* missionBookTownBountyIssuer();
bool missionBookSelectionValid(){
    if(!missionBookPoolReady||missionBookSelectedId.empty()||selectedOffer<0||selectedOffer>=6)return false;
    for(size_t i=0;i<missionBookPool.size();++i){const MissionBookPoolEntry& e=missionBookPool[i];if(e.id!=missionBookSelectedId)continue;
        if(e.issuer.isNull()||!missionBookSameTown(e.issuer.getCharacter()))return false;
        if(e.security){
            std::map<std::string,MercenarieV5::BountyBoard>::const_iterator b=bountyWorld.boards.find(e.issuer.toString());
            if(b==bountyWorld.boards.end()||b->second.refreshDue(currentGameHours)||bountyWorld.suspended)return false;
            for(size_t j=0;j<b->second.offers.size();++j)if(b->second.offers[j].id==e.id)return true;
            return false;
        }
        std::map<std::string,CityContractBoard>::const_iterator b=savedContractBoards.find(e.boardKey);
        if(b==savedContractBoards.end()||b->second.expiresAt<=currentGameHours||!b->second.offers[e.sourceIndex].available)return false;
        const BoardOffer& live=b->second.offers[e.sourceIndex];std::ostringstream id;id<<e.boardKey<<"#"<<e.sourceIndex<<"#"<<b->second.expiresAt<<"#"<<live.townId<<"#"<<live.missionType;
        return id.str()==e.id;
    }
    return false;
}
bool missionBookPersonalAvailable(){
    if(!missionBookSelectionValid())return false;
    return missionBookTab!=1||(activeQuestCount()<maximumActiveQuests&&GuildProgression::bountyUnlocked(guildLevel()));
}
void initializeMissionBookPool(){
    missionBookLegalBoardKey=currentBoardKey;missionBookLegalIssuer=contractBarmanHandle;
    missionBookClassicSources.clear();missionBookClassicSources[missionBookLegalBoardKey]=missionBookLegalIssuer;
    missionBookSecurityIssuer.setNull();
    if(missionBookTownId.empty()&&contractOriginTown&&contractOriginTown->getGameData())missionBookTownId=contractOriginTown->getGameData()->stringID;
    collectMissionBookSecurityIssuers();
    if(GuildProgression::bountyUnlocked(guildLevel())&&!bountyWorld.suspended)
        for(size_t i=0;i<missionBookSecurityIssuers.size();++i){Character* actor=missionBookSecurityIssuers[i].getCharacter();MercenarieV5::IssuerFaction kind;if(actor&&MercenarieV5::bountyIssuer(actor,kind))synchronizeBountySource(actor,kind);}
    missionBookPoolReady=true;
}
void refreshMissionBookOfferCounts(){
    if(!missionBookPoolReady||!contractsRefreshText)return;
    std::ostringstream footer;footer<<missionBookFiltered.size()<<" "<<Loc::text("ui.offers");
    double nextRotation=0;
    for(size_t i=0;i<missionBookFiltered.size();++i){const MissionBookPoolEntry& e=missionBookPool[missionBookFiltered[i]];double deadline=0;
        if(e.security){std::map<std::string,MercenarieV5::BountyBoard>::const_iterator b=bountyWorld.boards.find(e.issuer.toString());if(b!=bountyWorld.boards.end())deadline=b->second.nextRefreshHour;}
        else{std::map<std::string,CityContractBoard>::const_iterator b=savedContractBoards.find(e.boardKey);if(b!=savedContractBoards.end())deadline=b->second.expiresAt;}
        if(deadline>currentGameHours&&(nextRotation==0||deadline<nextRotation))nextRotation=deadline;
    }
    if(nextRotation>0){int hours=std::max(0,(int)(nextRotation-currentGameHours));footer<<" - "<<Loc::text("ui.next_rotation_in")<<" "<<hours/24<<" "<<Loc::text("ui.days")<<" "<<hours%24<<Loc::text("ui.h");}
    MercenarieFonts::caption(contractsRefreshText,footer.str());
}
void selectMissionBookEntry(size_t index){
    if(index>=missionBookPool.size())return;
    const MissionBookPoolEntry& e=missionBookPool[index];
    if(missionBookSelectedId!=e.id){contractAcceptArmed=false;delegationFinalArmed=false;delegationConfirming=false;}
    missionBookSelectedId=e.id;missionBookTab=e.security?1:0;
    contractBarman=e.issuer.isNull()?0:e.issuer.getCharacter();contractBarmanHandle=e.issuer;
    contractOriginTown=contractBarman?contractBarman->getCurrentTownLocation():0;
    originCity=contractOriginTown?frenchPlaceName(contractOriginTown->getName()):missionBookOfficeCity;
    if(e.security){
        // The native adapter needs one selected slot; the visible book has its own seven rows.
        selectedOffer=0;boardOffers[0]=e.offer;missionBookSecurityOfferIds[0]=e.id;
        missionBookSecurityAreaNames[0]=e.areaName;missionBookSecurityAreaRadii[0]=e.radius;
        Town* area=shou&&shou->townList?shou->townList->getTownBySID(e.offer.townId):0;
        missionBookSecurityAreaCenters[0]=area?area->getPosition():Ogre::Vector3::ZERO;
        bountyViewingIssuer=e.issuer;
    }else{
        currentBoardKey=e.boardKey;selectedOffer=e.sourceIndex;
        const CityContractBoard& board=savedContractBoards.find(e.boardKey)->second;
        for(int i=0;i<6;++i)boardOffers[i]=board.offers[i];
    }
}
void missionBookOfferRowClicked(MyGUI::WidgetPtr sender){
    size_t row=(size_t)atoi(sender->getUserString("bookRow").c_str());
    if(row>=missionBookPageEntries.size())return;
    selectMissionBookEntry(missionBookPageEntries[row]);
    contractOfferClicked(0); // Existing route framing, without changing a source slot.
}
void refreshMissionBookContractHub(){
    if(!contractsWindow||!missionBookDelegationContext||!missionBookPoolReady)return;
    const std::vector<MissionBookPoolEntry> previous=missionBookPool;missionBookPool.clear();
    missionBookClassicSources.clear();missionBookClassicSources[missionBookLegalBoardKey]=missionBookLegalIssuer;
    // Existing visitor boards are independent sources; tavern givers share their legacy board.
    for(size_t i=0;i<guildVisitors.size();++i){Character* actor=guildVisitors[i].leader;if(missionBookSameTown(actor)&&savedContractBoards.count(guildVisitors[i].offerKey))missionBookClassicSources[guildVisitors[i].offerKey]=actor->getHandle();}
    for(std::map<std::string,hand>::const_iterator source=missionBookClassicSources.begin();source!=missionBookClassicSources.end();++source){
    if(source->second.isNull()||!missionBookSameTown(source->second.getCharacter()))continue;
    std::map<std::string,CityContractBoard>::const_iterator board=savedContractBoards.find(source->first);
    if(board!=savedContractBoards.end()&&board->second.expiresAt>currentGameHours){
        for(int i=0;i<6;++i){const BoardOffer& offer=board->second.offers[i];
            if(!offer.available||!shou||!shou->townList||!shou->townList->getTownBySID(offer.townId))continue;
            MissionBookPoolEntry e;e.offer=offer;e.sourceIndex=i;e.boardKey=source->first;e.issuer=source->second;
            std::ostringstream id;id<<e.boardKey<<"#"<<i<<"#"<<board->second.expiresAt<<"#"<<offer.townId<<"#"<<offer.missionType;e.id=id.str();missionBookPool.push_back(e);
        }
    }
    }
    std::set<std::string> bountyIds;
    for(size_t giver=0;giver<missionBookSecurityIssuers.size();++giver){
    const hand issuerHandle=missionBookSecurityIssuers[giver];Character* issuer=issuerHandle.isNull()?0:issuerHandle.getCharacter();
    if(missionBookSameTown(issuer)&&GuildProgression::bountyUnlocked(guildLevel())&&!bountyWorld.suspended){
        std::map<std::string,MercenarieV5::BountyBoard>::const_iterator b=bountyWorld.boards.find(issuerHandle.toString());
        if(b!=bountyWorld.boards.end()&&!b->second.refreshDue(currentGameHours))for(size_t i=0;i<b->second.offers.size();++i){
            const MercenarieV5::BountyOffer& source=b->second.offers[i];Town* area=shou&&shou->townList?shou->townList->getTownBySID(source.areaId):0;if(!area||source.targetName.empty()||!bountyIds.insert(source.id).second)continue;
            MissionBookPoolEntry e;e.security=true;e.sourceIndex=(int)i;e.id=source.id;e.issuer=issuerHandle;
            BoardOffer& view=e.offer;view.available=true;view.missionType=3;view.townId=source.areaId;view.townName=source.targetName;view.profile=source.targetName;view.story=Loc::text("ui.bounty_hunt");view.estimatedPay=ContractReroll::bountyReward(source.amount,source.id);view.groupSize=source.guards+1;view.rarity=MercenarieV5::bountyDifficulty(source);view.rarityName=bountyDifficultyName(source);view.dangerLevel=1+2*MercenarieV5::bountyDifficulty(source);
            bool cached=false;for(size_t old=0;old<previous.size();++old)if(previous[old].security&&previous[old].id==e.id&&previous[old].issuer==e.issuer){view.distance=previous[old].offer.distance;view.danger=previous[old].offer.danger;view.routeRegions=previous[old].offer.routeRegions;cached=true;break;}
            if(!cached)analyseContractRoute(issuer->getPosition(),area->getPosition(),view);if(ContractReroll::bountyMarked(source.id))ContractReroll::mark(view.routeRegions,source.amount,0);e.areaName=frenchPlaceName(area->getName());e.radius=bountySearchRadius(source,area);missionBookPool.push_back(e);
        }
    }
    }
    std::vector<MissionBookContracts::Offer> offers;
    for(size_t i=0;i<missionBookPool.size();++i){const MissionBookPoolEntry& e=missionBookPool[i];offers.push_back(MissionBookContracts::Offer(e.id,MissionBookContracts::categoryFor(e.offer.missionType,e.security),true));}
    missionBookFiltered=MissionBookContracts::filtered(offers,(MissionBookContracts::Category)missionBookCategoryFilter);
    missionBookOfferPage=MissionBookContracts::clampPage(missionBookOfferPage,missionBookFiltered.size());
    missionBookPageEntries=MissionBookContracts::page(missionBookFiltered,missionBookOfferPage);
    size_t chosen=missionBookPool.size();
    for(size_t i=0;i<missionBookPageEntries.size();++i)if(missionBookPool[missionBookPageEntries[i]].id==missionBookSelectedId)chosen=missionBookPageEntries[i];
    if(chosen==missionBookPool.size()&&!missionBookPageEntries.empty())chosen=missionBookPageEntries[0];
    const std::string previousSelection=missionBookSelectedId;
    if(chosen<missionBookPool.size())selectMissionBookEntry(chosen);
    else {missionBookSelectedId.clear();missionBookTab=0;selectedOffer=0;boardOffers[0]=BoardOffer();contractAcceptArmed=false;delegationFinalArmed=false;}
    for(int i=0;i<6;++i)missionBookBoardTabs[i]->setVisible(false);
    missionBookBoardPageText->setVisible(false);contractsListPanel->setVisible(true);contractsMapPanel->setVisible(true);contractsAccept->setVisible(true);contractsRefreshText->setVisible(true);
    if(chosen<missionBookPool.size()&&previousSelection!=missionBookSelectedId)contractOfferClicked(0);else updateContractsBoard();refreshMissionBookOfferCounts();
}

void tickMissionBookPool(){
    if(!missionBookPoolReady)return;
    loadContractBoards();
    std::map<std::string,CityContractBoard>::const_iterator b=savedContractBoards.find(missionBookLegalBoardKey);
    Character* giver=missionBookLegalIssuer.isNull()?0:missionBookLegalIssuer.getCharacter();
    if(b!=savedContractBoards.end()&&b->second.expiresAt<=currentGameHours&&giver&&!missionActive&&!missionPending){openContractsBoard(giver,false,false);return;}
    collectMissionBookSecurityIssuers();
    if(!bountyWorld.suspended&&GuildProgression::bountyUnlocked(guildLevel()))for(size_t i=0;i<missionBookSecurityIssuers.size();++i){
        Character* actor=missionBookSecurityIssuers[i].getCharacter();MercenarieV5::IssuerFaction kind;
        if(actor&&MercenarieV5::bountyIssuer(actor,kind))synchronizeBountySource(actor,kind);
    }
    refreshMissionBookContractHub();
}

std::string missionBookBountyDossier(){
    std::map<std::string,MercenarieV5::BountyBoard>::const_iterator board=bountyWorld.boards.find(bountyViewingIssuer.toString());if(board==bountyWorld.boards.end())return std::string();
    for(size_t i=0;i<board->second.offers.size();++i){const MercenarieV5::BountyOffer& o=board->second.offers[i];if(o.id!=missionBookSelectedId)continue;
        Character* giver=bountyViewingIssuer.getCharacter();Town* area=shou&&shou->townList?shou->townList->getTownBySID(o.areaId):0;
        std::ostringstream text;text<<(Loc::text("v8.literal.117"))<<(giver?giver->getName():"")<<"\n"<<Loc::text("ui.faction")<<bountyTargetFactionName(o)<<" | "<<Loc::text("ui.difficulty")<<bountyDifficultyName(o)<<"\n"<<Loc::text("ui.guards")<<o.guards<<" | "<<Loc::text("ui.estimated_combat")<<o.combatMin<<" - "<<o.combatMax<<"\n";
        const char* reasons[]={Loc::text("ui.caravan_raids_and_murders_of_travellers_near"),Loc::text("ui.armed_robberies_and_kidnappings_around"),Loc::text("ui.murders_and_theft_of_trade_goods_near")};unsigned int variant=0;for(size_t j=0;j<o.id.size();++j)variant=variant*33+(unsigned char)o.id[j];text<<reasons[variant%3]<<(area?frenchPlaceName(area->getName()):o.areaId)<<".";return text.str();
    }return std::string();
}
