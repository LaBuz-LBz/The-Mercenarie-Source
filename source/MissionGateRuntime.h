#pragma once
#include "MissionGateGeometry.h"
// Runtime-only state belongs to each quest's MissionRescueState.
bool missionGateTick(float elapsed){
    if(missionRescue.passage.active)return false; // do not overwrite a bounded leader recovery
    missionRescue.gateScanClock=std::max(0.0f,missionRescue.gateScanClock-elapsed);
    if(missionRescue.gateClearing){
        missionRescue.gateClearSeconds-=elapsed;
        Building* gate=missionRescue.gateHandle.getBuilding();DoorStuff* door=gate?gate->getDoor():0;
        if(!escort||escort->getHandle().toString()!=missionRescue.gateLeader.toString()||!door||!door->isOpen()||missionRescue.gateClearSeconds<=0||escort->getPosition().squaredDistance(missionRescue.gateExit)<400.0f||missionPaused||missionFollowing||waitingForPlayer||scientificResearching||leavingBuilding||escort->isInCombatMode(true,true)){
            missionRescue.gateClearing=false;missionRescue.gateScanClock=30;DebugLog("MISSION GATE clearance finished or interrupted id="+currentMissionFiscalId);
        }
        return false;
    }
    if(missionRescue.gateScanClock>0)return false;
    missionRescue.gateScanClock=2;
    if(!missionActive||missionPending||missionPaused||missionFollowing||waitingForPlayer||scientificResearching||leavingBuilding||!escort||escort->isDead()||escort->isBeingCarried()||escort->isInCombatMode(true,true)||!ou||!ou->zoneMgr||!ou->navmesh||!missionRescue.roadPlanned||missionRescue.roadFailed)return false;
    if(missionCasualtyWaiting&&!missionRescue.regrouping)return false;
    TownBase* town=escort->getCurrentTownLocation();
    // Use loaded nearby towns only; never request loading distant terrain.
    if(!town&&shou&&shou->townList){
        lektor<RootObject*>& towns=shou->townList->getAllTowns();float best=4000000.0f;
        for(unsigned int i=0;i<towns.size();++i){Town* t=dynamic_cast<Town*>(towns[i]);if(t){float d=t->getPosition().squaredDistance(escort->getPosition());if(d<best){town=t;best=d;}}}
    }
    if(!town||!escort->getMovement()||!escort->getMovement()->havokCharacter)return false;
    lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,town,0,false,0,0);
    for(unsigned int i=0;i<buildings.size();++i){
        Building* gate=buildings[i];if(!gate||!gate->isGate()||gate->isDestroyed()||gate->getPosition().squaredDistance(escort->getPosition())>90000.0f)continue;
        DoorStuff* door=gate->getDoor();if(!door||!door->isOpen()||door->isLocked())continue;
        Ogre::Vector3 center,axis;if(!MissionGateGeometry::frame(door->getDoorPosInside(),door->getDoorPosOutside(),center,axis))continue;
        size_t next=0;if(!MissionGateGeometry::choose(escort->getPosition(),center,axis,missionRescue.roadPoints,missionRescue.roadNext,next))continue;
        // Never move or consume the exact final mission objective.
        if(next+1>=missionRescue.roadPoints.size())continue;
        Ogre::Vector3 target;if(!missionProjectExteriorRoadPoint(missionRescue.roadPoints[next],target)||target.squaredDistance(missionRescue.roadPoints[next])>900.0f)continue;
        float from=MissionGateGeometry::side(escort->getPosition(),center,axis),to=MissionGateGeometry::side(target,center,axis);
        if(!(from*to<0&&fabs(to)>=40))continue;
        if(ou->navmesh->pathExists(escort->getMovement()->havokCharacter,target)!=1)continue;
        missionRescue.roadPoints[next]=target;missionRescue.roadNext=next;missionRescue.roadProjectedIndex=next;missionRescue.roadProjectionElevated=true;
        missionRescue.gateHandle=gate->getHandle();missionRescue.gateLeader=escort->getHandle();missionRescue.gateExit=target;missionRescue.gateClearing=true;missionRescue.gateClearSeconds=20;
        std::ostringstream s;s<<"MISSION GATE crossing id="<<currentMissionFiscalId<<" gate="<<gate->getHandle().toString()<<" from="<<escort->getPosition()<<" exit="<<target<<" index="<<next;DebugLog(s.str());return true;
    }
    return false;
}
