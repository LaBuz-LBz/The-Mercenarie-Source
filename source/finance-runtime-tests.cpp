#include <cassert>
#include <iostream>
#include "src/Save/FiscalLedgerFormat.h"
struct Wallet {int money;bool refuse;Wallet():money(1000),refuse(false){}int getMoney(){return money;}void addMoney(int n){if(!refuse)money+=n;}void takeMoney(int n){if(!refuse)money-=n;}} wallet;
struct Participant{Wallet* factionOwnerships;Participant():factionOwnerships(&wallet){}} participant;
struct Player{Participant* participant;Player():participant(&::participant){}} player;
struct World{Player* player;World():player(&::player){}} world;
World* ou=&world;bool ready=true;double currentGameHours=240;FiscalLedger fiscalLedger;std::string saved;
void saveFiscalLedger(){std::ostringstream s;FiscalLedgerFormat::write(s,fiscalLedger);saved=s.str();}
#include "FinanceRuntime.h"
bool financeReady(){return ready;}
int main(){
 fiscalLedger.create("debt-test","contract","","","","",240,1000,0,0,1000,false,FREL_NORMAL,FREL_NORMAL);long long due=fiscalLedger.debt(FISCAL_MERCENARY_GUILD);assert(due>0);financeObserve();wallet.refuse=true;assert(!financePayDebt(FISCAL_MERCENARY_GUILD));assert(fiscalLedger.debt(FISCAL_MERCENARY_GUILD)==due&&wallet.money==1000&&fiscalLedger.cash.entries.empty());wallet.refuse=false;assert(financePayDebt(FISCAL_MERCENARY_GUILD));assert(wallet.money==1000-due&&fiscalLedger.debt(FISCAL_MERCENARY_GUILD)==0&&fiscalLedger.cash.entries.size()==1&&fiscalLedger.cash.entries[0].amount==-due);assert(!financePayDebt(FISCAL_MERCENARY_GUILD));{FiscalLedger loaded;std::istringstream input(saved);FiscalLedgerFormat::read(input,loaded);fiscalLedger=loaded;financeObserve();assert(!financePayDebt(FISCAL_MERCENARY_GUILD)&&fiscalLedger.cash.entries.size()==1);}
 fiscalLedger=FiscalLedger();wallet.money=1000;

 financeObserve();assert(fiscalLedger.cash.entries.empty());
 wallet.money+=250;financeChange(500,Finance::Contract,"finance.contract");assert(fiscalLedger.cash.entries.size()==2);assert(fiscalLedger.cash.entries[0].category==Finance::Other&&fiscalLedger.cash.entries[0].amount==250);assert(fiscalLedger.cash.entries[1].category==Finance::Contract&&fiscalLedger.cash.entries[1].amount==500);
 financeObserve();assert(fiscalLedger.cash.entries.size()==2);financeChange(-200,Finance::Tax,"finance.tax_payment");assert(fiscalLedger.cash.entries.back().amount==-200&&fiscalLedger.cash.entries.back().balance==1550);
 wallet.refuse=true;financeChange(-100,Finance::Tax,"finance.tax_payment");assert(fiscalLedger.cash.entries.size()==3);wallet.refuse=false;
 // True snapshot reload, then a different native wallet: establish a fresh
 // observation baseline rather than counting the load as a debit.
 FiscalLedger restored;std::istringstream in(saved);FiscalLedgerFormat::read(in,restored);fiscalLedger=restored;wallet.money=50;financeObserve();assert(fiscalLedger.cash.entries.size()==3);financeChange(100,Finance::Contract,"finance.contract");assert(fiscalLedger.cash.entries.back().amount==100);
 ready=false;wallet.money=999;financeObserve();assert(fiscalLedger.cash.entries.size()==4);ou=0;financeChange(100,Finance::Contract,"finance.contract");assert(fiscalLedger.cash.entries.size()==4);
 std::cout<<"PASS production finance runtime: reconciliation, exact native deltas, refused payments, snapshots, load baseline and guards\n";
}
