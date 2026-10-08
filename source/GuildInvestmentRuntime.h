// Runs only in the existing game/UI thread. Closing consumes the confirmation
// before any mutation, so queued clicks cannot apply the same exchange twice.
namespace {
int investmentAvailable(){
    if(missionWorldChanging||missionRestorePending||progressWriteBlocked||progressLoadFault||!ou||!ou->player||!ou->player->participant||!ou->player->participant->factionOwnerships)return -1;
    return ou->player->participant->factionOwnerships->getMoney();
}
void refreshGuildInvestment(){
    if(!guildInvestmentOverlay||!guildInvestmentOverlay->getVisible())return;
    if(!guildWindow->getVisible()||investmentSlot!=activeMercenarieSaveSlot){closeGuildInvestment(0);return;}
    int available=investmentAvailable(),step=MercenarieConfig::Progression::CatsToXpCost;
    const int blocks=step>0?investmentCats/step:0,earned=GuildProgression::investmentXp(investmentCats);
    MercenarieFonts::caption(investmentAmount,registerNumber(blocks));
    if(investmentReward)MercenarieFonts::caption(investmentReward,registerNumber(earned)+Loc::text("ui.xp_67b2c80"));
    MercenarieFonts::caption(investmentBalance,std::string(Loc::text("guild.you_have"))+registerNumber(std::max(0,available-investmentCats))+Loc::text("ui.cats"));
    refreshGuildInvestmentPreview(available,step,earned);
    if(investmentMaxText){int hours=(int)ceil(std::max(0.0,guildInvestmentNextHour-currentGameHours));MercenarieFonts::caption(investmentMaxText,hours?std::string(Loc::text("guild.investment.wait"))+registerNumber(hours)+" h":Loc::text("guild.investment.weekly"));}
    investmentConfirm->setEnabled(investmentArmed&&!investmentBusy&&GuildProgression::investmentReady(currentGameHours,guildInvestmentNextHour)&&GuildProgression::canInvest(investmentCats,available));
    investmentMinus->setEnabled(false);
    investmentPlus->setEnabled(false);
    const int quick[]={1,2,5,10};for(int i=0;i<4;++i)if(investmentQuick[i])investmentQuick[i]->setEnabled(false);
}
void confirmGuildInvestment(MyGUI::Widget*){
    if(!investmentArmed||investmentBusy||!guildInvestmentOverlay||!guildInvestmentOverlay->getVisible()||!guildWindow||!guildWindow->getVisible())return;
    if(investmentSlot!=activeMercenarieSaveSlot||reputationFile.empty()||fiscalFile.empty()){closeGuildInvestment(0);return;}
    const int cats=investmentCats,available=investmentAvailable();
    if(!GuildProgression::investmentReady(currentGameHours,guildInvestmentNextHour)||!GuildProgression::canInvest(cats,available)){refreshGuildInvestment();return;}
    investmentBusy=true;
    struct BusyReset {~BusyReset(){investmentBusy=false;}} busyReset;
    closeGuildInvestment(0);
    const double oldNext=guildInvestmentNextHour;
    const int oldXp=guildPoints,oldPrestige=guildPrestige,oldLevel=guildLevel();
    try {
        // Prepare all potentially throwing allocations and fiscal validation on
        // private values. Even a malformed old ledger cannot debit the wallet.
        FiscalLedger nextLedger=fiscalLedger;
        std::vector<std::string> nextHistory=contractHistory;
        nextLedger.cash.observe(available,currentGameHours);
        nextLedger.recordInvestment(cats,currentGameHours);
        nextLedger.cash.record(-cats,available-cats,currentGameHours,Finance::Investment,"finance.investment");
        nextHistory.push_back("@investment:"+registerNumber(cats)+" Cats -> "+registerNumber(GuildProgression::investmentXp(cats))+" XP");
        std::ostringstream fiscal;FiscalLedgerFormat::write(fiscal,nextLedger);
        std::string progress;
        // The legacy progress writer reads globals. Swap only for this bounded
        // serialization, then restore before touching money or storage.
        contractHistory.swap(nextHistory);
        try {guildInvestmentNextHour=currentGameHours+168;GuildProgression::award(guildPoints,guildPrestige,GuildProgression::investmentXp(cats));progress=serializeReputations();}
        catch(...){guildInvestmentNextHour=oldNext;guildPoints=oldXp;guildPrestige=oldPrestige;contractHistory.swap(nextHistory);throw;}
        const int nextXp=guildPoints,nextPrestige=guildPrestige;
        guildInvestmentNextHour=oldNext;guildPoints=oldXp;guildPrestige=oldPrestige;contractHistory.swap(nextHistory);
        ou->player->participant->factionOwnerships->takeMoney(cats);
        try {
            if(ou->player->participant->factionOwnerships->getMoney()!=available-cats)throw std::runtime_error("investment debit mismatch");
            if(!persistGuildInvestment(fiscal.str(),progress))throw std::runtime_error("investment storage failed");
        }catch(...){
            const int missing=available-ou->player->participant->factionOwnerships->getMoney();
            if(missing>0)ou->player->participant->factionOwnerships->addMoney(missing);
            throw;
        }
        // No allocations after storage succeeds: swap the prepared business state.
        fiscalLedger.swap(nextLedger);contractHistory.swap(nextHistory);
        guildInvestmentNextHour=currentGameHours+168;guildPoints=nextXp;guildPrestige=nextPrestige;
    }catch(...){ou->showPlayerAMessage(Loc::text("guild.exchange_failed"),true);updateGuildMenu();return;}
    investmentBusy=false;appliedGuildUnlockLevel=-1;updateGuildResearchAccess();
    updateGuildMenu();updateFinancesPage();
    if(guildLevel()>oldLevel)showGuildLevelUpWindow(oldLevel,guildLevel());
}
}
