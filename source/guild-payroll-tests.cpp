#include <cassert>
#include <iostream>
#include "src/GuildPayroll.h"
#include "src/GuildPayrollPersistence.h"
#include "src/PayrollPresentation.h"
#include "MissionArchive.h"
using namespace GuildPayroll;
void add(State& s,const char* id,int r=0,double h=0,bool robot=false){s.observe(id,id,r,robot,false,0,h);}
State roundTrip(State s){MissionArchive out;s.archive(out);MissionArchive in(out.bytes);State result;result.archive(in);in.finish();return result;}
int main(){
 assert(rank(24,24,24,24,0,24,0)==0);assert(rank(25,25,25,0,25,0,25)==1);
 assert(rank(40,40,40,40,0,40,0)==2);assert(rank(60,60,60,0,60,0,60)==3);assert(rank(80,80,80,80,0,80,0)==4);
 assert(rank(150,150,1,150,0,150,0)==2);assert(rank(100,100,40,100,0,100,0)==3);
 assert(rank(100,100,60,100,0,100,0)==4);
 State s;assert(s.enabled);for(int i=0;i<5;++i){assert(s.rates[i]==defaultRate(i));s.rate(i,0,0);assert(s.rates[i]==10);for(int v=15;v<=200;v+=5){s.rate(i,v,0);assert(s.rates[i]==v);}s.rate(i,300,0);assert(s.rates[i]==200);}s=State();
 State low;for(int i=0;i<5;++i)low.rate(i,10+i*5,0);State restored=roundTrip(low);for(int i=0;i<5;++i)assert(restored.rates[i]==10+i*5);State previous=roundTrip(State());for(int i=0;i<5;++i)assert(previous.rates[i]==defaultRate(i));
 add(s,"Kang",2);add(s,"Burn",4,0,true);s.observe("animal","animal",0,false,true,0,0);assert(!s.members.count("animal"));
 s.observe("Kang","Kang",3,false,false,0,96);add(s,"Beep",1,144);s.advance(168);
 assert(s.members["Kang"].rank==3&&s.bills.size()==1&&s.bills[0].total==650+420);assert(s.debt()==1070);assert(s.members["Burn"].rate==0);
 assert(s.bills[0].lines.size()==3);Money bill=s.pending()->id;assert(s.answer(bill)&&!s.answer(bill));assert(s.debt()==1070);
 assert(s.repay(200,168,"partial")==200&&s.debt()==870);assert(s.repay(10000,168,"complete")==870&&s.debt()==0);
 State rates;add(rates,"a",2);add(rates,"b",2);rates.rate(2,110,96);rates.advance(168);assert(rates.debt()==2*650);
 PayrollPresentation::Summary summary=PayrollPresentation::summarize(rates.bills[0]);assert(summary.total==1300&&summary.ranks[2].total==1300&&summary.ranks[2].count==2&&summary.ranks[2].rates.size()==2);
 summary=PayrollPresentation::summarize(s.bills[0]);assert(summary.total==1070&&summary.ranks[3].total==650&&summary.ranks[3].changed&&summary.count==3);Money rankTotal=0;for(int r=0;r<5;++r)rankTotal+=summary.ranks[r].total;assert(rankTotal==summary.total);
 State lowCycle;add(lowCycle,"a",4);lowCycle.rate(4,10,24);lowCycle.advance(168);assert(lowCycle.debt()==140+60);assert(PayrollPresentation::summarize(lowCycle.bills[0]).total==200);
 State recruit;recruit.init(0);add(recruit,"new",0,167);recruit.advance(168);assert(recruit.debt()==350);recruit.advance(504);assert(recruit.bills.size()==3&&recruit.debt()==1050);recruit.advance(504);assert(recruit.debt()==1050);
 State limbs;add(limbs,"a");for(int i=0;i<4;++i){std::vector<Money> claims=limbs.observe("a","a",0,false,false,(1<<(i+1))-1,i+1);assert(claims.size()==1);assert(limbs.debt(Limb)==1000*(i+1));}
 limbs=roundTrip(limbs);assert(limbs.observe("a","a",0,false,false,15,5).empty());assert(limbs.observe("a","a",0,false,false,0,6).empty());assert(limbs.observe("a","a",0,false,false,15,7).empty());assert(limbs.debt(Limb)==4000);
 State baseline;baseline.observe("old","old",2,false,false,15,0);assert(baseline.debt()==0);
 State order;add(order,"a");order.addDebt("a",Limb,3000,20,"limb");order.addDebt("a",Salary,2000,10,"salary");
 assert(order.withholding(10000)==5000);order.recordReward("personal",6000,3000,30,std::vector<std::string>());assert(order.debts[1].remaining==0&&order.debts[0].remaining==2000);assert(order.receipt("personal")->net==3000);
 order.recordReward("personal",6000,3000,30,std::vector<std::string>());assert(order.debt()==2000&&order.receipts.size()==1);assert(order.withholding(10000)==2000);assert(order.withholding(101)==50);
 order=roundTrip(order);assert(order.receipt("personal")&&order.members["a"].wages==2000&&order.members["a"].compensation==1000&&order.debts[0].repayments.size()==1);
 State off;add(off,"a");off.advance(24);off.enable(false,24);Money old=off.debt();assert(old==50);off.observe("a","a",4,false,false,1,800);assert(off.debt()==old&&off.bills.empty()&&off.withholding(10000)==0&&!off.alert(0));
 off.rate(4,170,800);off.enable(true,900);assert(off.next==1068&&off.debt()==old&&off.estimate()==1190);off.observe("a","a",4,false,false,1,901);assert(off.debt()==old);off.advance(1068);assert(off.debt()==old+1190);
 State warning;add(warning,"a");warning.advance(143);assert(!warning.alert(0));warning.advance(144);assert(warning.alert(0));assert(!warning.alert(0));warning=roundTrip(warning);assert(!warning.alert(0));
 State death;add(death,"a");death.addDebt("a",Limb,1000,10,"limb");death.observe("a","a",1,false,true,0,48);assert(death.members["a"].former&&death.debt(Salary)==100);death.advance(1000);assert(death.estimate()==0&&death.bills.empty()&&death.debt()==1100);
 State revenue;add(revenue,"a");add(revenue,"b");add(revenue,"robot",4,0,true);std::vector<std::string> p;p.push_back("b");p.push_back("a");p.push_back("a");p.push_back("robot");revenue.participants["group"]=p;revenue.complete("group");revenue.complete("group");
 revenue.recordReward("delegated",10001,0,5,p);assert(revenue.members["a"].income==3334&&revenue.members["b"].income==3334&&revenue.members["robot"].income==3333);assert(revenue.members["a"].contracts==1&&revenue.members["robot"].wages==0);
 revenue=roundTrip(revenue);revenue.complete("group");assert(revenue.members["a"].contracts==1);revenue.recordReward("no-identity",1000,0,7,std::vector<std::string>());assert(revenue.members["a"].income==3334);
 State a=roundTrip(limbs),b=roundTrip(off);State world=a;world=State();assert(world.debt()==0&&world.members.empty()&&world.enabled);world=b;assert(world.debt()==off.debt());world=a;assert(world.debt()==limbs.debt()&&world.members.count("a"));
 // Execute the same optional extension used by Mission.v4. Validation cannot
 // mutate a live world; a V8/V9 archive with no extension starts from defaults.
 MissionArchive oldSave(std::string(""));archiveOptional(oldSave,world,true);oldSave.finish();assert(world.members.empty()&&world.enabled);
 MissionArchive saveA;archiveOptional(saveA,a,true);MissionArchive validate(saveA.bytes);world=b;archiveOptional(validate,world,false);assert(world.debt()==b.debt());
 MissionArchive loadA(saveA.bytes);archiveOptional(loadA,world,true);loadA.finish();assert(world.debt()==a.debt());
 State large;add(large,"a");for(int i=0;i<18000;++i)large.addDebt("a",Salary,100,24*i,"payroll.salary");MissionArchive big;large.archive(big);assert(big.bytes.size()>1048576);std::string sealed=sealMissionArchive(big.bytes);assert(openMissionArchive(sealed)==big.bytes);
 std::cout<<"PASS ranks/base component rules; common 7-day payroll, late recruit, rank/rate segments, skeleton zero; full/partial/refusal/idempotency; 4 limbs/prostheses/save-load; FIFO and 50% cap; option/warning/death/career; attribution; A/B/A archive round-trip\n";
}
