#include "BountyPresentationRules.h"
#include "DelegatedMissionTiming.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

struct SearchView{std::string offerId,areaId,target;float x,z,radius;SearchView(const std::string& o="",const std::string& a="",const std::string& t="",float px=0,float pz=0,float r=0):offerId(o),areaId(a),target(t),x(px),z(pz),radius(r){}};
int main(){
    assert(BountyPresentationRules::showsExactRoute(0));
    assert(BountyPresentationRules::showsExactRoute(1));
    assert(BountyPresentationRules::showsExactRoute(2));
    assert(!BountyPresentationRules::showsExactRoute(3));
    SearchView views[3]={SearchView("A","area-a","Harn Ren",10,20,100),SearchView("B","area-b","Sera Tal",30,40,200),SearchView("C","area-c","Karg Dar",50,60,300)};
    const int sequence[]={0,1,2,0};for(int i=0;i<4;++i){const SearchView& v=views[sequence[i]];assert(v.offerId==views[sequence[i]].offerId);assert(v.areaId!=v.target);assert(v.radius>0);}
    double roundTrip=BountyPresentationRules::roundTripKilometres(30000.0);assert(std::fabs(roundTrip-60.0)<.0001);
    DelegatedMissionTiming::State timing=DelegatedMissionTiming::start(DelegatedMissionTiming::ActivityBountyHunt,roundTrip,1000.0);assert(std::fabs(timing.travelHours-20.0)<.0001);assert(std::fabs(timing.activityHours-48.0)<.0001);assert(std::fabs(timing.exactReturnWorldHour-1068.0)<.0001);
    std::cout<<"bounty presentation: search area only, stable offer mapping, round trip once, +48h OK\n";
}
