// Included after saveFiscalLedger; all mutations remain on the game thread.
bool financeReady();
int financeBalance(){return ou&&ou->player&&ou->player->participant&&ou->player->participant->factionOwnerships?ou->player->participant->factionOwnerships->getMoney():-1;}
void financeObserve(){if(!financeReady())return;int balance=financeBalance();if(balance>=0){bool changed=fiscalLedger.cash.observe(balance,currentGameHours);if(fiscalLedger.importLegacyInvestments())changed=true;if(changed)saveFiscalLedger();}}
// Explicit mod-owned payment boundaries, not speculative merchant hooks.
void financeChange(int amount,int category,const char* description){
 int before=financeBalance();if(before<0)return;
 bool track=financeReady();if(track)fiscalLedger.cash.observe(before,currentGameHours);
 if(amount>=0)ou->player->participant->factionOwnerships->addMoney(amount);
 else ou->player->participant->factionOwnerships->takeMoney(-amount);
 int after=financeBalance();
 if(track&&after>=0){fiscalLedger.cash.record((long long)after-before,after,currentGameHours,category,description);saveFiscalLedger();}
}

// Retire debt only after the native wallet confirms the exact debit.
bool financePayDebt(FiscalOrganisation organisation){
 if(!financeReady())return false;long long debt=fiscalLedger.debt(organisation);int before=financeBalance();
 if(debt<=0||debt>2147483647LL||before<debt)return false;
 fiscalLedger.cash.observe(before,currentGameHours);
 ou->player->participant->factionOwnerships->takeMoney((int)debt);int after=financeBalance();
 if(after!=before-debt){if(after>=0&&after<before)ou->player->participant->factionOwnerships->addMoney(before-after);return false;}
 if(!fiscalLedger.payAll(organisation,before))return false;
 fiscalLedger.cash.record(-debt,after,currentGameHours,Finance::Tax,"finance.tax_payment");saveFiscalLedger();return true;
}
