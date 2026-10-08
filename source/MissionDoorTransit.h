#pragma once
#include "MissionGateGeometry.h"
// A normal MOVE through an already open doorway. No door action, gate-code
// change, collision override or direct movement is performed here.
bool missionDoorTransitAlternative(Ogre::Vector3& result,bool quiet=false){
    if(!escort||!escort->getMovement()||!escort->getMovement()->havokCharacter||!ou||!ou->navmesh||!ou->zoneMgr||!missionRescue.roadPlanned||missionRescue.roadFailed)return false;
    TownBase* town=escort->getCurrentTownLocation();
    if(!town&&shou&&shou->townList){
        lektor<RootObject*>& towns=shou->townList->getAllTowns();float best=4000000.0f;
        for(unsigned int i=0;i<towns.size();++i){Town* t=dynamic_cast<Town*>(towns[i]);if(t){float d=t->getPosition().squaredDistance(escort->getPosition());if(d<best){town=t;best=d;}}}
    }
    if(!town)return false;
    lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,town,0,false,0,0);
    std::vector<Building*> doors;
    for(unsigned int i=0;i<buildings.size();++i){Building* b=buildings[i];if(!b||b->isDestroyed())continue;
        if(b->getDoor())doors.push_back(b);
        for(unsigned int j=0;j<b->doors.size()&&j<16;++j)if(b->doors[j]&&b->doors[j]->getDoor())doors.push_back(b->doors[j]);
    }
    const Ogre::Vector3 actor=escort->getPosition();
    DoorStuff* selected=0;Building* owner=0;Ogre::Vector3 center,axis,opposite;size_t join=0;float best=quiet?90000.0f:32400.0f,sign=0;
    for(size_t i=0;i<doors.size();++i){Building* b=doors[i];DoorStuff* door=b->getDoor();if(!door||b->isDestroyed())continue;
        Ogre::Vector3 c,a;if(!MissionGateGeometry::frame(door->getDoorPosInside(),door->getDoorPosOutside(),c,a))continue;
        const float distance=actor.squaredDistance(c);if(distance>=best)continue;
        float from=MissionGateGeometry::side(actor,c,a);Ogre::Vector3 approach=actor;
        // At the threshold the plane sign is unstable. Use the onward itinerary
        // to disambiguate, without moving the actor or accepting a parallel wall.
        if(fabs(from)<2){float onward=0;for(size_t n=missionRescue.roadNext;n<missionRescue.roadPoints.size()&&n<missionRescue.roadNext+8;++n){onward=MissionGateGeometry::side(missionRescue.roadPoints[n],c,a);if(fabs(onward)>5)break;}
            if(fabs(onward)<=5)continue;const float behind=onward>0?-4.0f:4.0f;approach.x+=(behind-from)*a.x;approach.z+=(behind-from)*a.z;from=behind;
        }
        size_t next=0;if(!MissionGateGeometry::choose(approach,c,a,missionRescue.roadPoints,missionRescue.roadNext,next,quiet?300.0f:180.0f)||next+1>=missionRescue.roadPoints.size())continue;
        if(!door->isOpen()||door->isLocked()){if(quiet)continue;std::ostringstream log;log<<"MISSION DOOR TRANSIT rejected id="<<currentMissionFiscalId<<" door="<<b->getHandle().toString()<<" reason=not open/unlocked";DebugLog(log.str());continue;}
        selected=door;owner=b;center=c;axis=a;sign=from<0?1.0f:-1.0f;opposite=sign>0?door->getDoorPosOutside():door->getDoorPosInside();join=next;best=distance;
    }
    if(!selected){if(!quiet)DebugLog("MISSION DOOR TRANSIT no open doorway aligned with route id="+currentMissionFiscalId);return false;}
    // Anchors are measured from the doorway, never from the raw road height.
    const float distances[2]={40,65};
    for(int attempt=0;attempt<2;++attempt){
        Ogre::Vector3 wanted(center.x+axis.x*sign*distances[attempt],opposite.y,center.z+axis.z*sign*distances[attempt]),projected=wanted;
        const int projection=ou->navmesh->getClosestPoint(wanted,40.0f,1.0f,false,projected);
        const float along=MissionGateGeometry::side(projected,center,axis)*sign;
        const float dx=projected.x-center.x-axis.x*sign*along,dz=projected.z-center.z-axis.z*sign*along;
        const bool geometry=projection==1&&projected.squaredDistance(wanted)<=1600&&projected.squaredDistance(actor)>=256&&projected.squaredDistance(actor)<=(quiet?122500.0f:57600.0f)&&along>=20&&along<=100&&dx*dx+dz*dz<=400&&missionRescue.passage.different(projected);
        int access=-2;if(geometry)access=ou->navmesh->pathExists(escort->getMovement()->havokCharacter,projected);
        std::ostringstream log;log<<"MISSION DOOR TRANSIT candidate id="<<currentMissionFiscalId<<" door="<<owner->getHandle().toString()<<" actor="<<actor<<" inside="<<selected->getDoorPosInside()<<" outside="<<selected->getDoorPosOutside()<<" wanted="<<wanted<<" projected="<<projected<<" projection="<<projection<<" opposite_side="<<along<<" geometry="<<geometry<<" native_access="<<access;if(!quiet||(geometry&&access>=0))DebugLog(log.str());
        if(!geometry||access<0)continue;
        // The synchronous connectivity precheck is advisory for this bounded
        // open-door trial. A result of zero is NOT reported as reachable.
        // Kenshi's MOVE/path request decides whether it can actually traverse.
        // Local/Global each issue at most one target; the episode never resets.
        result=projected;missionRescue.passage.joinIndex=join;
        missionRescue.roadPoints[join]=projected;
        DebugLog(std::string("MISSION DOOR TRANSIT native MOVE trial id=")+currentMissionFiscalId+(access==1?" precheck=reachable":" precheck=negative execution-unconfirmed"));return true;
    }
    return false;
}
