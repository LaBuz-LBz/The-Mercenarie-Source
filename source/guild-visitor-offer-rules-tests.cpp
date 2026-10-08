#include "GuildVisitorOfferRules.h"
#include <cassert>
#include <set>
#include <iostream>
int main(){
    using namespace GuildVisitorOfferRules;
    assert(weight(Scientist,Science)>weight(Scientist,Mail));
    assert(weight(Merchant,Caravan)>weight(Merchant,Science));
    assert(weight(Soldier,Escort)>weight(Soldier,Mail));
    std::vector<int> recent;std::set<int> seen;
    for(int i=0;i<400;++i){int type=choose(i%7,recent,i*37+11);seen.insert(type);remember(recent,type);assert(recent.size()<=3);}
    assert(seen.size()==4);
    std::vector<int> repeated(1,Mail);int mail=0,other=0;for(int i=0;i<100;++i)(choose(Civilian,repeated,i)==Mail?mail:other)++;
    assert(other>mail);
    assert(giverMayDepart(Escort)&&giverMayDepart(Caravan)&&giverMayDepart(Science)&&giverMayDepart(Mail));
    std::cout<<"guild visitor offer rules tests: OK\n";
}
