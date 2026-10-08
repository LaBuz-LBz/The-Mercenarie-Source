#include "MissionDebugRules.h"
#include "AmbushSpawnPlan.h"
#include "AmbushBalance.h"
#include <cassert>
#include <iostream>
int main(){
assert(MissionDebugRules::humanoidCandidate(false,false));assert(!MissionDebugRules::humanoidCandidate(true,false));assert(!MissionDebugRules::humanoidCandidate(false,true));assert(!MissionDebugRules::humanoidCandidate(true,true));
using namespace MissionDebugRules;
assert(!active(false,false,false,false,true));assert(!active(true,true,false,false,true));assert(!active(true,false,true,false,true));assert(!active(true,false,false,true,true));assert(!active(true,false,false,false,false));assert(active(true,false,false,false,true));
assert(!ready(true,1000,0));assert(!ready(false,1000,1750));assert(ready(false,1750,1750));assert(ready(false,2000,1750));assert(!ready(false,0xfffffff0UL,100));assert(ready(false,101,100));
assert(bluntDamage(100,40,0)==0);assert(bluntDamage(100,100,60)==0);assert(bluntDamage(0,100,0)==0);assert(bluntDamage(100,std::numeric_limits<float>::quiet_NaN(),0)==0);
for(int max=25;max<=300;max+=25)for(int percent=50;percent<=100;++percent){float health=max*percent/100.0f;float d=bluntDamage((float)max,health,0);assert(d>0);assert(health-d>=max*0.39f);assert(d<=max*0.151f);}
assert(treatableCut(100,40,0)==0);assert(treatableCut(100,100,60)==0);assert(treatableCut(0,100,0)==0);
for(int maximum=25;maximum<=300;maximum+=25)for(int percent=50;percent<=100;++percent){float health=maximum*percent/100.0f;float cut=treatableCut((float)maximum,health,0),blunt=bluntDamage((float)maximum,health,0);assert(cut>0&&cut<=maximum*.101f);assert(health-cut-blunt>=maximum*.349f);}
assert(AmbushSpawnPlan::validLeaderTuple(1,0));assert(AmbushSpawnPlan::validLeaderTuple(1,100));assert(!AmbushSpawnPlan::validLeaderTuple(0,0));assert(!AmbushSpawnPlan::validLeaderTuple(1,-1));assert(!AmbushSpawnPlan::validLeaderTuple(1,101));
for(int difficulty=1;difficulty<=5;++difficulty){std::vector<AmbushSpawnPlan::Entry> entries;entries.push_back(AmbushSpawnPlan::Entry(AmbushSpawnPlan::Leader,1,1));entries.push_back(AmbushSpawnPlan::Entry(AmbushSpawnPlan::Member,25,40));AmbushSpawnPlan::Plan plan;assert(AmbushSpawnPlan::build(entries,difficulty,plan));assert(plan.cap==AmbushSpawnPlan::capForDifficulty(difficulty));assert(plan.maximumActors<=20);assert(plan.maximumActors<=plan.cap);assert(AmbushBalance::valid(AmbushBalance::profileForDifficulty(difficulty)));}
assert(!AmbushBalance::allowed(1,AmbushBalance::SandJounin));assert(!AmbushBalance::allowed(2,AmbushBalance::Cannibal));assert(AmbushBalance::allowed(3,AmbushBalance::Cannibal));assert(AmbushBalance::allowed(5,AmbushBalance::SandJounin));
std::cout<<"PASS: mission gates, click lock/wrap, 612 damage margins, 5 production raid difficulties and faction gates\n";
}
