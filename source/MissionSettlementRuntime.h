// Runtime settlement orchestration. Included after the mission, finance, UI and
// persistence state declarations that it deliberately coordinates.
void settleSuccessfulContract(bool requestMoney)
{
    if(!missionActive||missionPending||!finalBonusChoice.prepared)return;
    if(currentContract.settlementPaid||rewardedContractIds.count(currentMissionFiscalId)||progressWriteBlocked)return;
    const int reportOldXp=guildPoints,reportOldPrestige=guildPrestige;
    const float reportOldLocal=localReputation(),reportOldGlobal=escortReputation;
    currentContract.settlementPaid=true;
    rewardedContractIds.insert(currentMissionFiscalId);
    int reward=missionReward-advancePaid;
    int acceptedPrimeCount=0;
    float repGain=(float)progressReportLocal;
    int extra=0;
    MissionBonuses::Resolution bonuses=MissionBonuses::resolve(finalBonusChoice,requestMoney&&guildLevel()>=1,.55f+GuildProgression::chanceBonus(localReputation(),escortReputation),EscortConfig::CounterOfferChance,rollFinalBonus);
    extra=bonuses.cats;acceptedPrimeCount=bonuses.count;
    repGain-=acceptedPrimeCount;
    int tip=finalClientTip;
    bool itemReward=(currentContract.wealth>=ECW_MERCHANT&&UtilityT::random(0.0f,1.0f)<0.08f)&&grantRareItemReward();
    reward+=extra+tip;
    const int grossWithAdvance=reward+advancePaid;
    const int pacePenalty=EscortMissionRules::forcedPacePenalty(grossWithAdvance,missionForcedPace);
    reward=std::max(0,grossWithAdvance-pacePenalty-advancePaid);
    payrollContractReward("personal:"+currentMissionFiscalId,reward,currentContract.type==MCT_CARAVAN?"finance.contract.caravan":currentContract.type==MCT_SCIENCE?"finance.contract.science":"finance.contract.escort");
    createFiscalEntryForSuccess(reward+advancePaid,extra,tip);
    escortReputation=GuildProgression::clampRep(escortReputation+progressReportGlobal);
    changeLocalReputation(repGain,Loc::text("ui.successful_escort_service"),false,false);
    int oldGuildLevel=guildLevel();int guildPointsEarned=pendingGuildXp-MissionBonuses::xpCost(pendingGuildXp,acceptedPrimeCount);GuildProgression::award(guildPoints,guildPrestige,guildPointsEarned);int newGuildLevel=guildLevel();
    totalAdvances+=advancePaid;totalBonuses+=extra;totalTips+=tip;++successfulContracts;totalContractCats+=reward+advancePaid;archiveEscort(1,extra>0?1:0,extra);saveReputations();
    if(escort)escort->sayALine(itemReward?Loc::text("ui.i_enclose_a_useful_item_with_payment_you_deserved"):tip>0?Loc::text("ui.excellent_work_here_s_a_tip_too"):Loc::text("ui.thanks_you_kept_your_commitment"),true);
    std::ostringstream receipt;receipt<<(Loc::text("ui.mission_complete"))<<"\n"<<(Loc::text("ui.reward_before_forced_pace_penalty"))<<grossWithAdvance<<Loc::text("ui.cats_7132fc0");if(missionForcedPace)receipt<<"\n"<<Loc::text("ui.forced_pace_penalty")<<"-10% (-"<<pacePenalty<<Loc::text("ui.cats_7132fc0")<<")";receipt<<"\n"<<(Loc::text("ui.payment_received"))<<std::showpos<<(guildPayroll.receipt("personal:"+currentMissionFiscalId)?guildPayroll.receipt("personal:"+currentMissionFiscalId)->net:reward)<<std::noshowpos<<Loc::text("ui.cats_7132fc0")<<(Loc::text("ui.guild_xp"))<<std::showpos<<(guildPoints-reportOldXp);
    if(localReputation()!=reportOldLocal)receipt<<"\n"<<(Loc::text("ui.local_reputation_b4a2c1a"))<<(localReputation()-reportOldLocal);
    if(escortReputation!=reportOldGlobal)receipt<<"\n"<<(Loc::text("ui.global_reputation_aaa7942"))<<(escortReputation-reportOldGlobal);
    if(guildPrestige!=reportOldPrestige)receipt<<"\n"<<(Loc::text("ui.prestige"))<<(guildPrestige-reportOldPrestige);
    if(requestMoney)receipt<<"\n"<<(Loc::text("ui.bonuses_granted"))<<std::noshowpos<<acceptedPrimeCount;
    if(missionCompletionNotificationsEnabled())ou->showPlayerAMessage(receipt.str(),true);
    if(newGuildLevel>oldGuildLevel){appliedGuildUnlockLevel=-1;updateGuildResearchAccess();scanGuildFurniture();}
    completedEscort=escort;completedCaravan.clear();for(size_t i=0;i<progressMembers.size();++i){Character* member=progressMembers[i].getCharacter();if(member)completedCaravan.push_back(member);}if(completedCaravan.empty())completedCaravan.push_back(escort);completedCleanupClock=60.0f;queueQuestCleanup();
    if(finalWindow)finalWindow->setVisible(false);if(!negotiationWasPaused)ou->userPause(false);resetFinishedMission();if(newGuildLevel>oldGuildLevel)showGuildLevelUpWindow(oldGuildLevel,newGuildLevel);
}
