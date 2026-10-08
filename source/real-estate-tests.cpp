#include "src/RealEstate.h"
#include <cassert>
#include <iostream>
using namespace RealEstate;
Identity house(const char* uid){Identity b;b.uid=uid;b.layout="layout";b.building="building-fcs";b.town="town-fcs";b.owner="owner-fcs";b.name="House";b.city="Town";return b;}
int main(){
    // Preference OFF is safe only without obligations. Never mutate on inspection.
    State optionState;assert(!requiresSystem(optionState));
    for(int n=0;n<100;++n){optionState=load(save(optionState));assert(!requiresSystem(optionState)&&optionState.leases.empty());}
    Money optionLease=optionState.sign(house("option"),1000,0,100);
    std::string original=save(optionState);assert(requiresSystem(optionState)&&save(optionState)==original);
    optionState.advance(168,0);assert(requiresSystem(optionState));
    assert(optionState.close(optionLease,Terminated));assert(requiresSystem(optionState));
    assert(optionState.pay(optionLease,1,100));assert(!requiresSystem(optionState));
    State bought;Money boughtId=bought.sign(house("bought"),1000,0,100);
    assert(bought.close(boughtId,Purchased));assert(!requiresSystem(bought));
    assert(load(save(bought)).leases[boughtId].status==Purchased);
    State saveA;saveA.sign(house("save-a"),1000,0,100);std::string bytesA=save(saveA);
    State switched=load(bytesA);assert(requiresSystem(switched));switched=load("");assert(!requiresSystem(switched));
    switched=load(bytesA);assert(requiresSystem(switched)&&save(switched)==bytesA);
    switched.suspendAll(0);assert(requiresSystem(switched));
    State unavailable;unavailable.available=false;assert(requiresSystem(unavailable));
    // Native saves omit seconds; sidecars retain them. Reproduce qqq exactly.
    State rounded;rounded.advance(34.149264335632324,500000);
    rounded=load(save(rounded));rounded.advance(34.0+8.0/60.0,500000);
    assert(rounded.available&&rounded.observedHour==34.149264335632324);
    State clockLease;Money clockId=clockLease.sign(house("clock"),1000,0,100);
    std::vector<Payment> clockPaid=clockLease.advance(168.005,1000);
    assert(clockPaid.size()==1&&clockLease.leases[clockId].sequence==2);
    clockLease=load(save(clockLease));assert(clockLease.advance(168.0,1000).empty());
    assert(clockLease.leases[clockId].next==336&&clockLease.leases[clockId].sequence==2);
    assert(clockLease.advance(168.006,1000).empty());
    bool backwards=false;try{clockLease.advance(167.9,1000);}catch(const std::exception&){backwards=true;}assert(backwards);
    for(int tick=1;tick<=3600;++tick){double now=34.149264335632324+tick/3600.0;rounded.advance(now,500000);if(tick%30==0){rounded=load(save(rounded));rounded.advance(std::floor(now*60)/60,500000);assert(rounded.available);}}
    Money r=0,b=0;assert(prices(30000,r,b)&&r==3000&&b==300000);assert(prices(12800,r,b)&&r==1280&&b==128000);assert(prices(1,r,b)&&r==1&&b==10);assert(prices(19,r,b)&&r==2);assert(!prices(0,r,b)&&!prices(-1,r,b)&&!prices(214748365,r,b));
    State s;assert(!s.sign(house("a"),12800,0,1279));Money a=s.sign(house("a"),12800,0,1280);assert(a&&!s.sign(house("a"),12800,0,99999));assert(s.debt()==0&&s.leases[a].next==168);
    assert(s.advance(168,1279).empty());assert(s.debt()==1280&&s.leases[a].status==Active);s.advance(336,0);s.advance(504,0);assert(s.debt()==3840);s.advance(504,0);assert(s.debt()==3840);s.advance(528,0);assert(s.leases[a].status==Active&&!s.canBuy());
    assert(!s.pay(a,1,1279));assert(s.pay(a,1,1280));assert(!s.pay(a,1,1280));assert(s.debt()==2560);assert(s.leases[a].next==672);
    std::vector<Payment> p=s.advance(672,1280);assert(p.size()==1&&p[0].amount==1280&&s.debt()==2560);
    State reload=load(save(s));assert(reload.debt()==2560&&!reload.canBuy());assert(reload.pay(a,2,1280));assert(reload.pay(a,3,1280));assert(reload.canBuy());assert(reload.close(a,Purchased));assert(reload.advance(10000,999999).empty());
    State multi;Money x=multi.sign(house("x"),1000,0,100),y=multi.sign(house("y"),2000,0,200);assert(multi.advance(168,200).empty());assert(multi.debt()==300&&multi.leases[x].debt()==100&&multi.leases[y].debt()==200);assert(multi.pay(y,1,200));assert(!multi.canBuy());assert(!multi.close(y,Purchased));assert(multi.close(x,Terminated));assert(multi.debt()==100);assert(multi.pay(x,1,100)&&multi.canBuy());
    State enough;enough.sign(house("z"),2000,0,200);enough.sign(house("b"),1000,0,100);p=enough.advance(168,300);assert(p.size()==2&&enough.debt()==0);assert(enough.advance(168,300).empty());
    State gap;Money g=gap.sign(house("g"),1000,0,100);gap.advance(3*168,0);assert(gap.leases[g].due.size()==1&&gap.leases[g].due[0].count==3&&gap.debt()==300);gap.pay(g,1,100);gap.advance(4*168,100);gap.advance(5*168,0);assert(gap.leases[g].due.size()==2&&gap.debt()==300);
    State imp;Money i=imp.sign(house("i"),1000,0,100);imp.advance(200,0);imp.suspendAll(10);assert(imp.debt()==100&&imp.leases[i].remaining==136);assert(!imp.pay(i,1,100));imp.advance(1000,99999);assert(imp.debt()==100&&imp.forecast(1000)==0);imp=load(save(imp));assert(imp.leases[i].suspended);assert(imp.resume(i,1000));assert(imp.leases[i].next==1136);assert(imp.advance(1135,0).empty());imp.advance(1136,0);assert(imp.debt()==200);
    State distinct;Identity one=house("same"),two=one;two.x=20;assert(distinct.sign(one,1000,0,100));assert(distinct.sign(two,1000,0,100));
    State large;Money k=large.sign(house("large"),214748364,0,21474837);large.advance(168*101,0);assert(large.debt()>2147483647LL&&!large.canBuy());assert(load(save(large)).debt()==large.debt());assert(large.pay(k,1,21474837));assert(!large.canBuy());
    bool corrupt=false;std::string damaged=save(large);damaged[damaged.size()-1]^=1;try{load(damaged);}catch(const std::exception&){corrupt=true;}assert(corrupt);
    bool duplicate=false;State invalid=large;Lease copy=invalid.leases[k];copy.id=invalid.nextId++;invalid.leases[copy.id]=copy;try{load(save(invalid));}catch(const std::exception&){duplicate=true;}assert(duplicate);
    State legacy=load("");assert(legacy.canBuy()&&legacy.leases.empty());
    State destroyed;Money d=destroyed.sign(house("destroyed"),1000,0,100);destroyed.advance(168,0);assert(destroyed.close(d,Destroyed));destroyed.advance(1000,9999);assert(destroyed.debt()==100);
    std::cout<<"Real estate: prices, ledger, multi-lease choice, debt gate, persistence, import suspension, corruption and residual debt PASS\n";
}
