#include "MissionRoadPath.h"
#include <cassert>
#include <iostream>
#include <fstream>
namespace Ogre {struct Vector3 {float x,y,z;Vector3(float a=0,float b=0,float c=0):x(a),y(b),z(c){}float squaredDistance(const Vector3& v)const{return (x-v.x)*(x-v.x)+(y-v.y)*(y-v.y)+(z-v.z)*(z-v.z);}};}
std::ostream& operator<<(std::ostream& s,const Ogre::Vector3& p){return s<<p.x<<","<<p.y<<","<<p.z;}
struct hand{};
#include "MissionRescueState.h"
struct Character{Ogre::Vector3 pos;bool combat;Character():combat(false){}Ogre::Vector3 getPosition(){return pos;}bool isDead(){return false;}bool isBeingCarried(){return false;}bool isInCombatMode(bool,bool){return combat;}};
Character* escort=0;
bool missionActive=true,missionPending=false,missionPaused=false,missionFollowing=false,missionCasualtyWaiting=false,waitingForPlayer=false,scientificResearching=false,leavingBuilding=false;
Ogre::Vector3 destination;std::string currentMissionFiscalId="road-test";
void DebugLog(const std::string&){}void ErrorLog(const std::string&){}
bool projectAvailable=false,mountainFixture=false;int projectCalls=0;Ogre::Vector3 projectionOffset;
bool missionProjectExteriorRoadPoint(const Ogre::Vector3& wanted,Ogre::Vector3& projected){++projectCalls;
 if(mountainFixture){if(wanted.y>4400||wanted.z> -81790)return false;projected=Ogre::Vector3(-25506.2f,4366.52f,-81801.1f);return true;}
 projected=Ogre::Vector3(wanted.x+projectionOffset.x,wanted.y+projectionOffset.y,wanted.z+projectionOffset.z);return projectAvailable;}
bool missionRoadUsesHorizontalArrival(){return true;}
bool missionRoadStillMoving(){return false;}
bool missionRoadCanHandoff(){return false;}
#include "MissionRoadRuntime.h"
#include "MissionGateGeometry.h"
void gateGeometryChecks(){
    Ogre::Vector3 center,axis;assert(MissionGateGeometry::frame(Ogre::Vector3(-10,0,0),Ogre::Vector3(10,0,0),center,axis));
    std::vector<Ogre::Vector3> route;route.push_back(Ogre::Vector3(-5,0,0));route.push_back(Ogre::Vector3(10,0,0));route.push_back(Ogre::Vector3(70,0,0));route.push_back(Ogre::Vector3(200,0,0));size_t next=99;
    assert(MissionGateGeometry::choose(Ogre::Vector3(-90,0,0),center,axis,route,0,next)&&next==2);
    std::vector<Ogre::Vector3> reverse;reverse.push_back(Ogre::Vector3(5,0,0));reverse.push_back(Ogre::Vector3(-70,0,0));
    assert(MissionGateGeometry::choose(Ogre::Vector3(90,0,0),center,axis,reverse,0,next)&&next==1);
    assert(!MissionGateGeometry::choose(Ogre::Vector3(300,0,0),center,axis,reverse,0,next));
    std::vector<Ogre::Vector3> beside;beside.push_back(Ogre::Vector3(-70,0,100));
    assert(!MissionGateGeometry::choose(Ogre::Vector3(90,0,100),center,axis,beside,0,next)); // crosses a wall, not the opening
    assert(!MissionGateGeometry::choose(Ogre::Vector3(20,0,0),center,axis,route,1,next)); // already across, no backward target
    assert(!MissionGateGeometry::frame(center,center,center,axis));
    std::cout<<"PASS gate geometry: both directions, opening intersection, nearby bounds, no backward crossing, invalid anchors\n";
}
void localNavigationChecks(){
    Character leader;escort=&leader;leader.pos=Ogre::Vector3(0,75,0);destination=Ogre::Vector3(1000,0,0);
    MissionRescueState initial;initial.roadPlanned=true;initial.roadGoal=destination;
    for(int i=0;i<5;++i)initial.roadPoints.push_back(Ogre::Vector3(20.0f+i*60,0,0));initial.roadPoints.push_back(destination);
    missionRescue=initial;projectAvailable=false;Ogre::Vector3 target;assert(missionRoadTarget(destination,target)&&missionRescue.roadNext==0); // unavailable navmesh: no unverified skip
    missionRescue=initial;projectAvailable=true;projectionOffset=Ogre::Vector3(0,10,0);projectCalls=0;
    assert(missionRoadTarget(destination,target)&&missionRescue.roadNext==2&&target.x==140&&target.y==10);
    assert(projectCalls==1);missionRoadTarget(destination,target);assert(projectCalls==1); // cached, no frame-by-frame queries
    assert(missionRescue.roadPoints.back().x==destination.x);assert(!missionRoadAdvance()); // no teleport or fake arrival
    MissionRescueState other=initial;MissionRescueState first=missionRescue;missionRescue=other;assert(missionRescue.roadNext==0);missionRescue=first;assert(missionRescue.roadNext==2); // per-quest runtime isolation
    missionRescue=initial;projectionOffset=Ogre::Vector3(100,0,0);missionRoadTarget(destination,target);assert(missionRescue.roadNext==0); // unreasonable projection refused
    missionRescue=initial;projectionOffset=Ogre::Vector3();missionRescue.roadPoints[2]=Ogre::Vector3(80,0,60);missionRoadTarget(destination,target);assert(missionRescue.roadNext==1); // right-angle bend retained
    missionRescue=initial;leader.pos=Ogre::Vector3(0,0,0);missionRoadTarget(destination,target);assert(missionRescue.roadNext==0); // ordinary road unchanged
    leader.pos=Ogre::Vector3(0,75,0);assert(missionRoadAdvance()&&missionRescue.roadNext==2); // stair mismatch appearing after original order: proactive, no timeout
    missionRescue=initial;projectAvailable=false;projectCalls=0;assert(!missionRoadAdvance());int failedCalls=projectCalls;assert(!missionRoadAdvance());assert(projectCalls==failedCalls); // failed native lookup is not repeated every frame
    projectAvailable=true;
    missionRescue=initial;missionRescue.roadNext=5;projectCalls=0;missionRoadTarget(destination,target);assert(projectCalls==0&&target.x==1000); // final destination untouched
    projectAvailable=false;missionRescue=MissionRescueState();
    std::cout<<"PASS local native projection, height-mismatch lookahead, cache, unavailable/invalid projection, corners, final target and per-quest isolation\n";
}
void runtimeChecks(){
    Character leader,replacement;escort=&leader;destination=Ogre::Vector3(1000,0,0);
    missionRescue.roadPlanned=true;missionRescue.roadGoal=destination;
    missionRescue.roadPoints.push_back(Ogre::Vector3(60,0,0));missionRescue.roadPoints.push_back(Ogre::Vector3(120,0,0));
    Ogre::Vector3 target;assert(missionRoadTarget(destination,target)&&target.x==60);
    leader.pos=Ogre::Vector3(50,0,0);missionCasualtyWaiting=true;assert(!missionRoadAdvance());missionCasualtyWaiting=false;
    missionPaused=true;assert(!missionRoadAdvance());missionPaused=false;
    leader.combat=true;assert(!missionRoadAdvance());leader.combat=false;
    assert(missionRoadAdvance()&&missionRescue.roadNext==1);assert(missionRoadTarget(destination,target)&&target.x==120);
    leader.pos=Ogre::Vector3(0,0,0);assert(!missionRoadAdvance()&&missionRescue.roadNext==1); // never regress to a consumed point
    escort=&replacement;assert(missionRoadTarget(destination,target)&&target.x==120); // succession preserves path
    leavingBuilding=true;assert(missionRoadTarget(Ogre::Vector3(20,0,0),target)&&target.x==20);leavingBuilding=false;
    missionRescue.roadFailed=true;assert(!missionRoadTarget(destination,target)); // no straight-line fallback
    missionRescue=MissionRescueState();
    escort=&leader;leader.pos=Ogre::Vector3(-37260,737.046f,-42382.5f);
    missionRescue.roadPlanned=true;missionRescue.roadGoal=destination;
    missionRescue.roadPoints.push_back(Ogre::Vector3(-37243.4f,661.352f,-42385));
    missionRescue.roadPoints.push_back(Ogre::Vector3(-37200,660,-42380));
    missionRescue.roadPoints.push_back(destination);
    assert(!missionRoadAdvance()); // actual log: close in XZ, wrong road height
    assert(!missionRoadRecoverStalledPoint(false,false));
    assert(!missionRoadRecoverStalledPoint(true,true));
    missionCasualtyWaiting=true;assert(!missionRoadRecoverStalledPoint(true,false));missionCasualtyWaiting=false;
    missionPaused=true;assert(!missionRoadRecoverStalledPoint(true,false));missionPaused=false;
    waitingForPlayer=true;assert(!missionRoadRecoverStalledPoint(true,false));waitingForPlayer=false;
    leader.combat=true;assert(!missionRoadRecoverStalledPoint(true,false));leader.combat=false;
    assert(missionRoadRecoverStalledPoint(true,false)&&missionRescue.roadNext==1);
    assert(missionRoadTarget(destination,target)&&target.x==-37200); // keep itinerary, no direct final jump
    leader.pos=Ogre::Vector3(-38000,737,-42382);assert(!missionRoadRecoverStalledPoint(true,false));
    missionRescue.roadNext=2;leader.pos=destination;assert(!missionRoadRecoverStalledPoint(true,false)); // never consume final target by timeout
    missionRescue=MissionRescueState();
}
int main(){
    // Recorded mountain stall: graph elevation 4480, actor 4372. A native
    // ground-level projection must not be rejected against the obsolete height.
    {Character leader;escort=&leader;leader.pos=Ogre::Vector3(-25528.5f,4372.16f,-81778);destination=Ogre::Vector3(-24740.5f,5054.3f,-86186.2f);
     missionRescue=MissionRescueState();missionRescue.roadPlanned=true;missionRescue.roadGoal=destination;
     missionRescue.roadPoints.push_back(Ogre::Vector3(-25504.5f,4480.37f,-81802.9f));
     missionRescue.roadPoints.push_back(Ogre::Vector3(-25545.7f,4500.39f,-81768.5f));
     missionRescue.roadPoints.push_back(Ogre::Vector3(-25586.9f,4520.41f,-81734.1f));missionRescue.roadPoints.push_back(destination);
     mountainFixture=true;Ogre::Vector3 target;assert(missionRoadTarget(destination,target));assert(target.y==4366.52f&&missionRescue.roadNext==0);assert(leader.pos.y==4372.16f);assert(missionRescue.roadPoints.back().y==destination.y);
     int calls=projectCalls;missionRoadTarget(destination,target);assert(projectCalls==calls);mountainFixture=false;missionRescue=MissionRescueState();}
    gateGeometryChecks();
    runtimeChecks();
    localNavigationChecks();
    using RoutePrototype::Point;
    PricingRoads::Graph g;unsigned int a=g.add(Point(0,0,0)),b=g.add(Point(1000,0,0)),c=g.add(Point(1000,0,1000));g.link(a,b);g.link(b,c);
    std::vector<Point> path;
    assert(MissionRoadPath::plan(g,Point(400,0,0),Point(800,0,0),path));
    double x=400;for(size_t i=0;i<path.size();++i){assert(path[i].x>=x&&path[i].x<=800&&path[i].z==0);assert(path[i].x-x<=60.01);x=path[i].x;}assert(x==800); // no return to the starting endpoint
    assert(MissionRoadPath::plan(g,Point(400,0,0),Point(1000,0,800),path));
    for(size_t i=0;i<path.size();++i)assert(path[i].z==0||path[i].x==1000); // road corner retained, no diagonal shortcut
    assert(MissionRoadPath::plan(g,Point(800,0,0),Point(400,0,0),path));assert(path.back().x==400);
    PricingRoads::Graph empty;assert(!MissionRoadPath::plan(empty,Point(),Point(100,0,0),path));
    assert(!MissionRoadPath::plan(g,Point(50000,0,0),Point(),path));
    PricingRoads::Graph real;std::ifstream file("pricing-roads.dat");real.read(file);
    Point start(-50412,1513.07,2271.96),goal(-105157,135.024,-28044.8);
    assert(MissionRoadPath::plan(real,start,goal,path));assert(RoutePrototype::distance(path.back(),goal)<0.1);
    Point previous=start;double distance=0;
    for(size_t i=0;i<path.size();++i)assert(RoutePrototype::distance(path[i],Point(-51032.72,1539.615,2782.943))>300); // avoid the observed town-gate spur
    for(size_t i=0;i<path.size();++i){double segment=RoutePrototype::distance(previous,path[i]);assert(segment<=60.01);distance+=segment;previous=path[i];}
    std::cout<<"PASS road projection, forward/reverse endpoints, corners, absent network, real saved mission: "<<path.size()<<" points length="<<distance<<"\n";
    for(size_t i=0;i<5&&i<path.size();++i)std::cout<<path[i].x<<","<<path[i].y<<","<<path[i].z<<"\n";
}
