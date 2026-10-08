#include "MissionOfferViewRules.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct Offer { std::string id,target,zone,giver;int cats,difficulty;Offer(const std::string& i="",const std::string& t="",const std::string& z="",const std::string& g="",int c=0,int d=0):id(i),target(t),zone(z),giver(g),cats(c),difficulty(d){} };
bool same(const Offer& a,const Offer& b){return a.id==b.id&&a.target==b.target&&a.zone==b.zone&&a.giver==b.giver&&a.cats==b.cats&&a.difficulty==b.difficulty;}

int main()
{
    Offer legal[3]={Offer("L1","Broken house","Broken house","Barman",15305,2),Offer("L2","Hybride","Hybride","Barman",15576,1),Offer("L3","Laboratoire","Swamp","Barman",34189,2)};
    const std::vector<Offer> original=MissionOfferViewRules::snapshot(legal);
    const int sequence[]={0,1,2,0,1,0};
    for(size_t step=0;step<sizeof(sequence)/sizeof(sequence[0]);++step){std::vector<Offer> view;if(sequence[step]==0)view=MissionOfferViewRules::snapshot(legal);if(sequence[step]==0){assert(view.size()==original.size());for(size_t i=0;i<view.size();++i)assert(same(view[i],original[i]));}}
    std::vector<Offer> police;police.push_back(Offer("P1","Harn Ren","Lost Library","Police Chief",9096,0));police.push_back(Offer("P2","Sera Tal","Lost Library","Police Chief",7063,0));police.push_back(Offer("P3","Karg Dar","Lost Library","Police Chief",7748,0));
    std::vector<Offer> security=MissionOfferViewRules::snapshot(police);assert(security.size()==police.size());for(size_t i=0;i<police.size();++i)assert(same(security[i],police[i]));
    assert(!MissionOfferViewRules::needsQuestPreparation(true));assert(MissionOfferViewRules::needsQuestPreparation(false));
    std::cout<<"mission offer shared-view lifecycle: OK\n";
}
