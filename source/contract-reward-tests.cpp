#include <cassert>
#include <iostream>
#include "ContractRewards.h"
#include "InternalConfig.h"
#include "src/Guild/Progression.h"

int main(){
    const int percents[]={50,70,90,100,110,130,150};
    const int expected[]={5000,7000,9000,10000,11000,13000,15000};
    for(int i=0;i<7;++i)assert(ContractRewards::apply(10000,percents[i])==expected[i]);
    assert(ContractRewards::apply(4000,150)==6000);
    assert(ContractRewards::apply(4000,50)==2000);
    assert(ContractRewards::apply(1,50)==1); // single shared half-up rule
    std::string data="V6EST:1:ROAD";assert(ContractRewards::snapshot(data)==100);
    ContractRewards::freeze(data,120);assert(ContractRewards::snapshot(data)==120);
    ContractRewards::freeze(data,150);assert(ContractRewards::snapshot(data)==150);
    assert(data.find("REWARD1=120")==std::string::npos); // no double snapshot
    const int thresholds[]={0,300,800,1550,2550,3850,5500,7500,9900,12750,16100};
    for(int level=0;level<=10;++level){assert(GuildProgression::threshold(level)==thresholds[level]);assert(GuildProgression::level(thresholds[level])==level);}
    const int oldV3[]={0,600,1600,3200,5500,8500,12500,17500,24000,32000,42000};
    for(int level=0;level<=10;++level)assert(GuildProgression::level(GuildProgression::migrate(oldV3[level],3))==level);
    std::cout<<"contract reward and progression tests: OK\n";
}
