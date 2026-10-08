#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <iostream>
namespace Ogre{struct Vector3{float x,y,z;Vector3(float a=0,float b=0,float c=0):x(a),y(b),z(c){}float squaredDistance(const Vector3& p)const{return (x-p.x)*(x-p.x)+(y-p.y)*(y-p.y)+(z-p.z)*(z-p.z);}};}
std::ostream& operator<<(std::ostream& o,const Ogre::Vector3& p){return o<<p.x;}
struct Building;struct Character;
struct hand{Building* b;Character* c;hand():b(0),c(0){}Building* getBuilding(){return b;}std::string toString()const{std::ostringstream s;s<<b<<c;return s.str();}};
template<class T>struct lektor:std::vector<T>{};
struct RootObject{virtual ~RootObject(){}};struct TownBase:RootObject{};struct Town:TownBase{Ogre::Vector3 getPosition(){return Ogre::Vector3();}};
struct DoorStuff{bool open,locked;DoorStuff():open(true),locked(false){}bool isOpen(){return open;}bool isLocked(){return locked;}Ogre::Vector3 getDoorPosInside(){return Ogre::Vector3(-10);}Ogre::Vector3 getDoorPosOutside(){return Ogre::Vector3(10);}};
struct Building{DoorStuff door;bool gate;Building():gate(true){}bool isGate(){return gate;}bool isDestroyed(){return false;}Ogre::Vector3 getPosition(){return Ogre::Vector3();}DoorStuff* getDoor(){return &door;}hand getHandle(){hand h;h.b=this;return h;}};
struct Movement{void* havokCharacter;Movement():havokCharacter(this){}};
Town localTown;
struct Character{Movement move;Ogre::Vector3 pos;bool combat;Character():pos(-90),combat(false){}hand getHandle(){hand h;h.c=this;return h;}Ogre::Vector3 getPosition(){return pos;}bool isDead(){return false;}bool isBeingCarried(){return false;}bool isInCombatMode(bool,bool){return combat;}Movement* getMovement(){return &move;}TownBase* getCurrentTownLocation(){return &localTown;}};
struct Zone{lektor<Building*> buildings;void findAllBuildings(lektor<Building*>& out,TownBase*,int,bool,int,int){out=buildings;}};
struct Nav{int result,calls;Nav():result(1),calls(0){}int pathExists(void*,const Ogre::Vector3&){++calls;return result;}};
struct World{Zone* zoneMgr;Nav* navmesh;};World* ou;
struct TownList{lektor<RootObject*> towns;lektor<RootObject*>& getAllTowns(){return towns;}};
struct Shared{TownList* townList;};Shared* shou=0;
bool missionActive=true,missionPending=false,missionPaused=false,missionFollowing=false,waitingForPlayer=false,scientificResearching=false,leavingBuilding=false,missionCasualtyWaiting=false;
Character* escort;std::string currentMissionFiscalId="gate-test";
void DebugLog(const std::string&){}
bool project=true;bool missionProjectExteriorRoadPoint(const Ogre::Vector3& p,Ogre::Vector3& out){out=p;return project;}
#include "MissionRescueState.h"
#include "MissionGateRuntime.h"
int main(){
    Character actor,other;escort=&actor;Building gate;Zone zone;zone.buildings.push_back(&gate);Nav nav;World world;world.zoneMgr=&zone;world.navmesh=&nav;ou=&world;
    MissionRescueState initial;initial.roadPlanned=true;initial.roadPoints.push_back(Ogre::Vector3(-5));initial.roadPoints.push_back(Ogre::Vector3(10));initial.roadPoints.push_back(Ogre::Vector3(70));initial.roadPoints.push_back(Ogre::Vector3(500));
    missionRescue=initial;gate.door.open=false;assert(!missionGateTick(1)&&missionRescue.roadNext==0);gate.door.open=true;
    missionRescue=initial;gate.door.locked=true;assert(!missionGateTick(1));gate.door.locked=false;
    missionRescue=initial;nav.result=-1;assert(!missionGateTick(1)&&!missionRescue.gateClearing);nav.result=0;missionRescue=initial;assert(!missionGateTick(1));nav.result=1;
    missionRescue=initial;project=false;assert(!missionGateTick(1));project=true;
    missionRescue=initial;missionCasualtyWaiting=true;assert(!missionGateTick(1));missionCasualtyWaiting=false;
    missionRescue=initial;assert(missionGateTick(1)&&missionRescue.roadNext==2&&missionRescue.gateClearing&&missionRescue.roadPoints.back().x==500);
    int calls=nav.calls;assert(!missionGateTick(1)&&nav.calls==calls); // pinned exit, no repeated path query
    MissionRescueState crossing=missionRescue;missionRescue=initial;assert(!missionRescue.gateClearing);missionRescue=crossing;
    actor.pos=Ogre::Vector3(60);assert(!missionGateTick(1)&&!missionRescue.gateClearing);actor.pos=Ogre::Vector3(-90);
    missionRescue=crossing;assert(!missionGateTick(30)&&!missionRescue.gateClearing&&missionRescue.gateScanClock==30);
    missionRescue=crossing;gate.door.open=false;missionGateTick(1);assert(!missionRescue.gateClearing);gate.door.open=true;
    missionRescue=crossing;escort=&other;missionGateTick(1);assert(!missionRescue.gateClearing);escort=&actor;
    missionRescue=initial;missionRescue.roadPoints.pop_back();assert(!missionGateTick(1)); // final objective cannot be skipped
    std::cout<<"PASS production gate runtime: door state, path denied/pending, projection, rescue priority, pinned exit, final objective, timeout, leader change, per-quest state\n";
}
