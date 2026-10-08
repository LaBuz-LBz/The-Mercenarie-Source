#include "GammaFixRules.h"
#include "src/FiscalSystem.h"
#include <cassert>
#include <iostream>
int main(){using namespace GammaFixRules;assert(!businessFinished(VisitWaiting));assert(businessFinished(VisitAccepted)&&businessFinished(VisitRefused)&&businessFinished(VisitPaid)&&businessFinished(VisitExtended)&&businessFinished(VisitEscalated));assert(!mayReseat(false,true)&&!mayReseat(true,false)&&mayReseat(true,true));FiscalLedger ledger;ledger.create("one","escort","common","tavern","Hub","Stack",1,10000,0,0,10000,true,FREL_NORMAL,FREL_NORMAL);assert(ledger.debt(FISCAL_UC)==2000&&ledger.debt(FISCAL_MERCENARY_GUILD)==1000);assert(!ledger.payAll(FISCAL_UC,1999));assert(ledger.payAll(FISCAL_UC,2000));assert(ledger.debt(FISCAL_UC)==0&&!ledger.payAll(FISCAL_UC,2000));assert(ledger.debt(FISCAL_MERCENARY_GUILD)==1000);std::cout<<"fixe gamma tests: OK\n";}
