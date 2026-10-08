#include <cassert>
#include <iostream>
#include "src/Save/FiscalLedgerFormat.h"
#include "src/FinancePresentation.h"
int main(){
 {
 FiscalLedger f;Finance::Journal& j=f.cash;
 for(int i=0;i<13;++i)j.record(-67,10000-i*67,240+i*.01,Finance::Other,"finance.unidentified");
 std::ostringstream before;FiscalLedgerFormat::write(before,f);
 std::vector<Finance::DisplayRow> r=Finance::recent(j,241,7);assert(r.size()==1&&r[0].movements==13&&r[0].entry.amount==-871);
 j.record(20,9200,240.2,Finance::Other,"finance.unidentified");assert(Finance::recent(j,241,7).size()==2);
 j.record(-20,9180,240.3,Finance::Tax,"finance.tax_payment");j.record(-67,9113,240.4,Finance::Other,"finance.unidentified");assert(Finance::recent(j,241,7).size()==4);
 j.record(-67,9046,242,Finance::Other,"finance.unidentified");assert(Finance::recent(j,242,7).size()==5);
 std::ostringstream snapshot;FiscalLedgerFormat::write(snapshot,f);Finance::recent(j,242,7);Finance::displayEntries(j,242,7);std::ostringstream after;FiscalLedgerFormat::write(after,f);assert(snapshot.str()==after.str());
 assert(Finance::recent(j,24*40,7).empty());
 for(int n=0;n<5000;++n)j.record(n%2?70:-60,20000,24*39+n*.00001,Finance::Other,"finance.unidentified");
 for(int period=7;period<=30;period+=23){std::vector<Finance::Entry> rows=Finance::displayEntries(j,24*40,period);Finance::Totals t=j.totals(24*40,period);long long in=0,out=0,catIn[Finance::Count]={0},catOut[Finance::Count]={0};bool archive=false;
 for(size_t n=0;n<rows.size();++n){const Finance::Entry& e=rows[n];if(e.amount>0){in+=e.amount;catIn[e.category]+=e.amount;}else{out-=e.amount;catOut[e.category]-=e.amount;}if(e.description=="finance.archived"){archive=true;assert(e.balance==-1);}}
 assert(archive&&in==t.income&&out==t.expense);for(int c=0;c<Finance::Count;++c)assert(catIn[c]==t.in[c]&&catOut[c]==t.out[c]);}
 assert(Finance::inPeriod(24*34,24*40,7)&&!Finance::inPeriod(24*33,24*40,7)&&!Finance::inPeriod(24*41,24*40,7));
 FiscalLedger debt;debt.create("tax", "contract","","","","",240,1000,0,0,1000,true,FREL_NORMAL,FREL_NORMAL);long long due=debt.debt(FISCAL_UC);assert(due>0&&debt.cash.totals(240,7).expense==0);assert(debt.payAll(FISCAL_UC,10000));debt.cash.record(-due,10000-due,241,Finance::Tax,"finance.tax_payment");assert(debt.debt(FISCAL_UC)==0&&debt.cash.totals(241,7).expense==due);
 std::cout<<"PASS presentation: grouping boundaries/signs/known events, immutable persistence, 7/30 days, archive reconciliation by category, unpaid vs paid debt\n";
 }
 FiscalLedger ledger;Finance::Journal& j=ledger.cash;
 assert(!j.observe(10000,240)&&j.entries.empty());assert(j.observe(9000,241));assert(j.entries.back().category==Finance::Other);
 assert(!j.observe(9000,241));assert(j.record(3000,12000,242,Finance::Contract,"finance.contract","C1"));assert(!j.record(3000,12000,242,Finance::Contract,"finance.contract","C1"));
 Finance::Totals totals=j.totals(242,30);assert(totals.income==3000&&totals.expense==1000&&totals.out[Finance::Other]==1000);
 std::ostringstream bytes;FiscalLedgerFormat::write(bytes,ledger);FiscalLedger restored;std::istringstream input(bytes.str());FiscalLedgerFormat::read(input,restored);assert(restored.cash.available&&restored.cash.entries.size()==2&&!restored.cash.observed);assert(restored.cash.totals(242,30).income==3000);
 // Native load may restore a different balance: never generate a reload delta.
 restored.cash.observe(500,242);assert(restored.cash.entries.size()==2);assert(!restored.cash.record(3000,3500,242,Finance::Contract,"finance.contract","C1"));
 std::istringstream old("@version|1|1\n");FiscalLedgerFormat::read(old,restored);assert(restored.cash.entries.empty()&&restored.cash.available&&!restored.cash.started);
 for(int i=0;i<10000;++i)j.record(i%2?-1:2,10000,243,Finance::Contract,"finance.contract");assert(j.entries.size()==4096&&j.truncated&&j.days.size()==1);assert(j.totals(243,30).income==13000&&j.totals(243,30).expense==6000);
 for(int i=11;i<500;++i)j.record(1,10000,i*24,Finance::Sale,"finance.sales");assert(j.days.size()==366);assert(j.totals(499*24,7).income==7);assert(j.totals(499*24,30).income==30);
 std::ostringstream huge;FiscalLedgerFormat::write(huge,ledger);std::istringstream hugeIn(huge.str());FiscalLedgerFormat::read(hugeIn,restored);assert(restored.cash.available&&restored.cash.entries.size()==4096&&restored.cash.days.size()==366);assert(restored.cash.totals(499*24,90).income==90);
 // Malformed/unknown journal data is never treated as trustworthy zero history.
 std::ostringstream beforeBad;FiscalLedgerFormat::write(beforeBad,restored);
 for(int mode=0;mode<2;++mode){std::istringstream bad(mode?"@version|1|1\n@finance|99|1|0|0|0|1\n":"@finance|99|1|0|0|0|1\n");bool rejected=false;try{FiscalLedgerFormat::read(bad,restored);}catch(const std::exception&){rejected=true;}assert(rejected);std::ostringstream afterBad;FiscalLedgerFormat::write(afterBad,restored);assert(afterBad.str()==beforeBad.str());}
 Finance::Journal inOnly,outOnly;inOnly.record(30,30,0,Finance::Sale,"finance.sales");outOnly.record(-20,0,0,Finance::Tax,"finance.tax_payment");assert(inOnly.totals(0,7).expense==0&&outOnly.totals(0,7).income==0);assert(Finance::matches(inOnly.entries[0],1)&&!Finance::matches(inOnly.entries[0],2));
 assert(Finance::Forecast().wages.available==false);
 std::cout<<"PASS finance: persistence, legacy saves, reload baseline, deduplication, 10000 entries, retention, calendar windows, categories, unknown format and future providers\n";
}
