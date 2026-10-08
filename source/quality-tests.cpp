#include "ContractQualityRules.h"
#include "FiscalSystem.h"
#include <cassert>
#include <iostream>
int main(){
    assert(contractTaxBase(8000,10500,1500,1000)==8000);
    assert(contractTaxBase(10500,10500,1500,1000)==8000);
    assert(contractTaxBase(8000,8000,0,0)==8000);
    assert(contractTaxBase(0,2500,1500,1000)==0);
    assert(contractTaxBase(8000,500,1000,0)==0);
    FiscalLedger ledger;
    FiscalEntry e=ledger.create("test","escort","common","guild","Heft","Heng",0,10500,1500,1000,contractTaxBase(8000,10500,1500,1000),true,FREL_NORMAL,FREL_NORMAL);
    assert(e.ucTax==1600&&e.guildTax==800);
    assert(e.bonusIncome==1500&&e.tips==1000);
    assert(ledger.contains("test"));
    assert(!ledger.payAll(FISCAL_UC,1599));assert(ledger.debt(FISCAL_UC)==1600);
    assert(ledger.payAll(FISCAL_UC,1600));assert(!ledger.payAll(FISCAL_UC,1600));
    FiscalLedger allied;
    e=allied.create("ally","escort","common","guild","Heft","Heng",0,10500,1500,1000,8000,true,FREL_ALLIED,FREL_ALLIED);
    assert(e.ucTax==400&&e.guildTax==400);
    allied.recordNoTax("failed","escort","Heft","Heng",0,FENTRY_FAILED);
    assert(allied.debt(FISCAL_UC)==400);
    std::cout<<"PASS: tax exclusions, alliance rates, debt, insufficient funds, duplicate payment, failed contract\n";
}
