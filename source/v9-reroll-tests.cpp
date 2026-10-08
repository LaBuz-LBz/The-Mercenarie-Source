#include <cassert>
#include <iostream>
#include "ContractReroll.h"
#include "src/Contracts/EstimatedPricing.h"
#include "v5/BountySave.h"
#include "GuildVisitorOfferRules.h"
#include "src/Guild/Progression.h"
int main(){
    using namespace ContractReroll;
    Charges c;assert(c.remaining==2&&c.started==-1&&c.ends==-1);
    assert(c.consume(10)&&c.remaining==1&&c.started==10&&c.ends==34);
    const std::string one=c.encode();assert(c.consume(15)&&c.remaining==0&&c.ends==34);
    assert(!c.consume(20)&&c.ends==34);c.refresh(33.999);assert(c.remaining==0);
    c.refresh(34);assert(c.remaining==2&&c.ends==-1&&c.sequence==2);
    assert(c.decode(one));c.refresh(10);assert(c.remaining==1&&c.hoursLeft(10)==24);
    // Paused world time cannot advance cooldown; fractional hours survive saves.
    for(int i=0;i<500;++i)c.refresh(10);assert(c.remaining==1&&c.ends==34);
    c.refresh(34);assert(c.remaining==2);assert(c.consume(45.123456789));
    Charges loaded;assert(loaded.decode(c.encode()));assert(loaded.started==c.started&&loaded.ends==c.ends);
    assert(!loaded.decode("3|-1|-1|1"));assert(!loaded.decode("1|10|50|1"));assert(!loaded.decode("0|nan|nan|1"));
    Charges another;assert(another.remaining==2);assert(loaded.decode(one));assert(loaded.remaining==1&&another.remaining==2);
    // Simulated offer rotation has no ownership of Charges.
    std::vector<std::string> rotation(6);const std::string before=loaded.encode();rotation.clear();assert(loaded.encode()==before);
    std::string metadata="V6EST:1:ROAD";assert(!marked(metadata));assert(reward(10000,false)==10000);
    mark(metadata,10000,1);assert(marked(metadata));const std::string once=metadata;mark(metadata,7500,2);assert(metadata==once);
    assert(reward(10000,marked(metadata))==7500);assert(reward(10001,true)==7500);
    for(int type=0;type<3;++type){
        EscortContractData natural;natural.type=(MercContractType)type;natural.routeRegions="V6EST:1:ROAD";natural.distanceKm=25;natural.dangerLevel=2;
        ContractPricing::calculateEstimatedContract(natural,0);const int normal=natural.totalPay;
        EscortContractData rerolled=natural;mark(rerolled.routeRegions,normal,1);rerolled.basePay=reward(normal,true);
        ContractPricing::calculateEstimatedContract(rerolled,0);assert(rerolled.totalPay==normal*3/4);
        const int reduced=rerolled.basePay;ContractPricing::calculateEstimatedContract(rerolled,20);assert(rerolled.totalPay==reduced+reduced*20/100);
        ContractPricing::calculateEstimatedContract(rerolled,0);assert(rerolled.totalPay==reduced&&marked(rerolled.routeRegions));
        // Recalculate after partial caravan cargo provisioning: one discount only.
        rerolled.basePay=0;ContractPricing::calculateEstimatedContract(rerolled,0);assert(rerolled.totalPay==reduced);
        std::string next="V6EST:1:ROAD";mark(next,normal,2);assert(reward(normal,marked(next))==reduced);
        const GuildProgression::Result xp=GuildProgression::success(natural.type,natural.distanceKm,natural.dangerLevel,0,true,false);
        const GuildProgression::Result after=GuildProgression::success(rerolled.type,rerolled.distanceKm,rerolled.dangerLevel,0,true,false);
        assert(xp.xp==after.xp&&xp.local==after.local&&xp.global==after.global);
    }
    EscortContractData quoted;quoted.routeRegions="V6EST:1:ROAD;PRICE=1000,3000,4000,2000,0,0,0,10000;";mark(quoted.routeRegions,10000,1);quoted.basePay=7500;quoted.distanceKm=20;
    ContractPricing::calculateEstimatedContract(quoted,0);assert(quoted.reward.base==1000&&quoted.reward.beforeNegotiation==7500&&quoted.totalPay==7500);
    MercenarieV5::BountyOffer bounty;bounty.id="officer:1:0:economy2:reroll:1";bounty.amount=7000;bounty.issuer.container=1;bounty.areaId="enemy-town";bounty.targetName="Target";
    MercenarieV5::scaleBountyDifficulty(bounty);int difficulty=MercenarieV5::bountyDifficulty(bounty),xp=30+bounty.amount/500;
    assert(bountyReward(bounty.amount,bounty.id)==5250);assert(bounty.amount==7000&&MercenarieV5::bountyDifficulty(bounty)==difficulty&&30+bounty.amount/500==xp);
    MercenarieV5::BountyWorldState world;world.boards["officer"].offers.push_back(bounty);assert(world.contract.accept(bounty));
    MercenarieV5::BountyWorldState restored;restored.load(world.save());assert(restored.contract.offer.id==bounty.id);assert(bountyReward(restored.contract.offer.amount,restored.contract.offer.id)==5250);
    for(int profile=0;profile<7;++profile){std::vector<int> recent;std::set<int> types;for(int roll=0;roll<1000;++roll){int type=GuildVisitorOfferRules::choose(profile,recent,roll);assert(type==0||type==1||type==2||type==4);types.insert(type);}assert(types.size()==4);}
    std::cout<<"V9 charges, 24h, save/load, old state, pricing, negotiation, bounty archive, giver capabilities: PASS\n";
}
