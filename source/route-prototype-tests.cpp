#include "RoutePrototype.h"
#include "MissionRouteRecovery.h"
#include "AmbushSpawnPlan.h"
#include "AmbushBalance.h"
#include "RoadRoutePreset.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace RoutePrototype;
int main(){
 using namespace AmbushSpawnPlan;
 assert(capForDifficulty(1)==7&&capForDifficulty(2)==12&&capForDifficulty(3)==16&&capForDifficulty(4)==20&&capForDifficulty(5)==20);
 assert(capForDifficulty(-5)==7&&capForDifficulty(99)==20);
 assert(AmbushBalance::profileForDifficulty(1).minimum==5&&AmbushBalance::profileForDifficulty(1).maximum==15);
 assert(AmbushBalance::profileForDifficulty(5).minimum==50&&AmbushBalance::profileForDifficulty(5).maximum==60);
 assert(AmbushBalance::allowed(1,AmbushBalance::StarvingMob));
 assert(!AmbushBalance::allowed(1,AmbushBalance::Cannibal)&&!AmbushBalance::allowed(1,AmbushBalance::SandJounin));
 assert(AmbushBalance::allowed(3,AmbushBalance::Cannibal)&&!AmbushBalance::allowed(3,AmbushBalance::BloodRaider));
 assert(AmbushBalance::allowed(4,AmbushBalance::BloodRaider)&&!AmbushBalance::allowed(4,AmbushBalance::SandJounin));
 assert(AmbushBalance::allowed(5,AmbushBalance::SandJounin));
 std::vector<Entry> ambush;ambush.push_back(Entry(Leader,1,1));ambush.push_back(Entry(Member,10,40));ambush.push_back(Entry(Animal,2,6));
 Plan low;assert(build(ambush,1,low)&&low.overrideCounts&&low.maximumActors==7&&low.maximumActors<=low.cap&&low.fixedCounts[0]==1&&low.fixedCounts[1]>=1&&low.fixedCounts[2]>=1);
 Plan middle;assert(build(ambush,3,middle)&&middle.maximumActors==16&&middle.maximumActors<=16);
 Plan extreme;assert(build(ambush,5,extreme)&&extreme.maximumActors==20&&extreme.maximumActors<=20);
 std::vector<Entry> small;small.push_back(Entry(Leader,1,1));small.push_back(Entry(Member,2,4));Plan unchanged;
 assert(build(small,5,unchanged)&&unchanged.overrideCounts&&unchanged.maximumActors==20);
 std::vector<Entry> invalid;invalid.push_back(Entry(Member,1,5));Plan rejected;assert(!build(invalid,5,rejected));
 invalid.clear();invalid.push_back(Entry(Leader,1,1));for(int i=0;i<7;++i)invalid.push_back(Entry(i==4?Animal:Member,1,2));assert(!build(invalid,1,rejected));
 std::cout<<"PASS: ambush difficulty caps, absolute cap, small templates, invalid templates, leader and composition preservation\n";
 float best=100000.0f,stalled=0.0f;
 assert(!MissionRouteRecovery::shouldRetry(99000.0f,true,20.0f,best,stalled));
 assert(!MissionRouteRecovery::shouldRetry(97000.0f,true,20.0f,best,stalled));
 assert(stalled==0.0f&&best==97000.0f);
 assert(!MissionRouteRecovery::shouldRetry(98000.0f,true,44.0f,best,stalled));
 assert(MissionRouteRecovery::shouldRetry(97500.0f,true,1.0f,best,stalled));
 best=100000.0f;stalled=0.0f;
 assert(!MissionRouteRecovery::shouldRetry(100000.0f,false,7.0f,best,stalled));
 assert(MissionRouteRecovery::shouldRetry(100000.0f,false,1.0f,best,stalled));
    std::vector<Point> samples;
    assert(recordSample(samples,Point())==SampleAdded);
    assert(recordSample(samples,Point(10,0,0))==SampleIgnored);
    assert(recordSample(samples,Point(60,0,0))==SampleAdded);
    assert(recordSample(samples,Point(1000,0,0))==SampleGap&&samples.size()==2);
    assert(recordSample(samples,Point(60,200,0))==SampleGap&&samples.size()==2);
    samples.resize(511);assert(recordSample(samples,Point())==SampleFull);
    std::vector<Point> p;p.push_back(Point(100,0,0));p.push_back(Point(100,0,200));
    Route r;r.start("test-mission",p);assert(r.lengthFrom(Point())==300);
    assert(r.tick(Point(),1,false,false)==MoveToPoint);
    assert(r.tick(Point(),1,false,false)==NoOrder);
    assert(r.tick(Point(),100,true,true)==NoOrder);
    assert(r.tick(Point(),1,false,true)==MoveToPoint);
    assert(r.tick(Point(100,0,0),1,false,false)==MoveToPoint&&r.next==1);
    std::ifstream fixture("hub-blister.route");assert(fixture.good());
    RoadPreset preset=RoadPreset::read(fixture);
    assert(preset.originId=="18919-Newwworld.mod"&&preset.destinationId=="12628-Newwworld.mod");
    assert(preset.points.size()==77&&fabs(preset.roadLength-58799.9063768628)<.01);
    assert(preset.matches(preset.originId,preset.destinationId,preset.origin,preset.destination));
    assert(!preset.matches(preset.destinationId,preset.originId,preset.destination,preset.origin));
    assert(!preset.matches("wrong",preset.destinationId,preset.origin,preset.destination));
    Point shifted=preset.origin;shifted.x+=200;
    assert(!preset.matches(preset.originId,preset.destinationId,shifted,preset.destination));
    std::vector<Point> automatic=preset.points;automatic.push_back(preset.destination);
    Route automaticRoute;automaticRoute.start("automatic-test",automatic);
    while(automaticRoute.status==Travelling){Point p=automaticRoute.points[automaticRoute.next];automaticRoute.tick(p,1,false,false);automaticRoute=Route::load(automaticRoute.save(),"automatic-test");}
    assert(automaticRoute.status==Complete&&automaticRoute.next==78);
    const char* invalidPresets[]={"", "WRONG", "MERCENARIE-ROAD-PRESET-1 a b 0 0 0 100 0 0 100 999999", "MERCENARIE-ROAD-PRESET-1 a b 0 0 0 100 0 0 100 2 0 0 0", "MERCENARIE-ROAD-PRESET-1 a b 0 0 0 100 0 0 999 2 0 0 0 100 0 0", "MERCENARIE-ROAD-PRESET-1 a b 0 0 0 100 0 0 100 2 0 0 0 100 0 0 trailing"};
    for(unsigned int i=0;i<sizeof(invalidPresets)/sizeof(invalidPresets[0]);++i){std::istringstream in(invalidPresets[i]);bool rejected=false;try{RoadPreset::read(in);}catch(...){rejected=true;}assert(rejected);}
    std::cout<<"PASS: extracted road fixture, 77 unique road points, town guards, all 78 steps and reloads, malformed preset rejection\n";
    std::string saved=r.save();Route restored=Route::load(saved,"test-mission");
    assert(restored.next==1&&restored.points.size()==2);
    assert(restored.tick(Point(100,0,0),1,false,false)==MoveToPoint);
    assert(restored.tick(Point(100,0,0),90,false,false)==StopForReview);
    assert(restored.tick(Point(100,0,200),1,false,false)==NoOrder);
    restored.retry();assert(restored.tick(Point(100,0,200),1,false,false)==Arrived);
    assert(Route::load(restored.save(),"test-mission").status==Complete);
    bool bad=false;try{Route::load(saved,"another-mission");}catch(...){bad=true;}assert(bad);
    for(size_t i=0;i<saved.size();++i){bad=false;try{Route::load(saved.substr(0,i),"test-mission");}catch(...){bad=true;}assert(bad);}
    std::string corrupt=saved;corrupt[corrupt.size()-5]^=1;bad=false;try{Route::load(corrupt,"test-mission");}catch(...){bad=true;}assert(bad);
    r.cancel();assert(r.status==Idle&&r.points.empty());
    bad=false;try{r.start("",p);}catch(...){bad=true;}assert(bad&&r.status==Idle);
    r.start("test",p);assert(r.tick(Point(),1,false,false)==MoveToPoint);assert(r.tick(Point(),1,false,true)==StopForReview);
    Route blocked=Route::load(r.save(),"test");assert(blocked.status==Blocked);
    assert(blocked.tick(Point(),1,true,false)==NoOrder&&blocked.status==Blocked);
    r.start("height",p);assert(r.tick(Point(100,100,0),1,false,false)==MoveToPoint&&r.next==0);
    assert(r.tick(Point(100,0,0),1,true,false)==NoOrder&&r.next==0);
    assert(r.tick(Point(100,0,0),1,false,false)==MoveToPoint&&r.next==1);
    std::cout<<"PASS: waypoints, no order spam, suspension, blocked path, timeout, retry, completion, save identity, corruption and cancellation\n";
}
