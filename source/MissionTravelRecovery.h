#pragma once
#include "MissionDoorTransit.h"
void missionRejectExit(){
    Building* building=escort->getMovement()->building.getBuilding();if(!building)return;
    Building* closest=0;float best=1e30f;
    for(unsigned int i=0;i<building->doors.size()&&i<16;++i){Building* b=building->doors[i];if(!b||!b->getDoor())continue;
        float d=b->getDoor()->getDoorPosOutside_extraFarOut(3).squaredDistance(exitWaypoint);if(d<best){best=d;closest=b;}}
    if(closest&&missionRescue.failedExitDoors.size()<3)missionRescue.failedExitDoors.push_back(closest->getHandle());
}

// A handful of targets, not a second pathfinder. Kenshi projects and tests each
// candidate; an accessible target is not a guarantee that an animation/door can
// be traversed, so actual progress and the episode deadline remain authoritative.
bool missionPassageCandidate(const Ogre::Vector3& wanted,size_t join,Ogre::Vector3& result){
    Ogre::Vector3 projected=wanted;int reachable=-2;
    bool projectedOK=ou&&ou->navmesh&&ou->navmesh->getClosestPoint(wanted,30.0f,1.0f,false,projected)==1;
    bool distinct=projectedOK&&missionRescue.passage.useful(escort->getPosition(),wanted,projected);
    if(distinct&&escort->getMovement()->havokCharacter)reachable=ou->navmesh->pathExists(escort->getMovement()->havokCharacter,projected);
    std::ostringstream out;out<<"MISSION PASSAGE CANDIDATE id="<<currentMissionFiscalId<<" actor="<<escort->getPosition()<<" wanted="<<wanted<<" projected="<<projected<<" projection="<<projectedOK<<" distinct="<<distinct<<" native_access="<<reachable<<" join="<<join;DebugLog(out.str());
    if(!distinct||reachable!=1)return false;
    result=projected;missionRescue.passage.joinIndex=join;return true;
}
bool missionPassageAlternative(Ogre::Vector3& result,bool replanned){
    const Ogre::Vector3 pos=escort->getPosition();
    // First look beyond the rejected road sample: do not end native movement
    // on a raw road point below a stair/door. Raw height and actor height are
    // independent projection seeds, both checked by native pathExists.
    size_t begin=missionRescue.roadNext+(replanned?0:1);
    for(size_t i=begin;i<missionRescue.roadPoints.size()&&i+1<missionRescue.roadPoints.size()&&i<begin+3;++i){
        Ogre::Vector3 wanted=missionRescue.roadPoints[i];
        if(wanted.squaredDistance(pos)>57600)break;
        if(missionPassageCandidate(wanted,i,result))return true;
        wanted.y=pos.y;if(missionPassageCandidate(wanted,i,result))return true;
    }
    Ogre::Vector3 wanted=missionRescue.passage.failedGoal;wanted.y=pos.y;
    if(missionPassageCandidate(wanted,(size_t)-1,result))return true;
    float dx=wanted.x-pos.x,dz=wanted.z-pos.z,n=std::sqrt(dx*dx+dz*dz);
    if(n<1){dx=1;dz=0;n=1;}dx/=n;dz/=n;
    // Alternate approaches on the actor's level, including a short back-off.
    const float ahead[5]={8,8,0,0,-8},side[5]={8,-8,16,-16,0};
    for(int i=0;i<5;++i){wanted=Ogre::Vector3(pos.x+dx*ahead[i]-dz*side[i],pos.y,pos.z+dz*ahead[i]+dx*side[i]);if(missionPassageCandidate(wanted,(size_t)-1,result))return true;}
    return false;
}
void missionPassageIssue(const Ogre::Vector3& point,const char* reason,bool continuous=false){
    const bool continued=continuous&&missionContinueTravel(escort,point);
    if(!continued){missionClearTravel(escort,reason);missionIssueOrder(escort,MOVE_CUS_ORDERED,0,point);}
    missionRescue.localRecoveryTarget=point;missionRescue.localRecovery=true;
    missionRescue.passage.reject(point); // never offer this same endpoint again in the episode
    missionRescue.leaderWatch.fresh();
    std::ostringstream out;out<<"MISSION PASSAGE ALTERNATIVE id="<<currentMissionFiscalId<<" reason="<<reason<<" continued="<<continued<<" from="<<escort->getPosition()<<" rejected="<<missionRescue.passage.failedGoal<<" target="<<point<<" join="<<missionRescue.passage.joinIndex;DebugLog(out.str());
}
// Repair an order lost at a waypoint without waiting for the stalled-path
// recovery. Two attempts per area; only verified forward arrival rearms them.
bool missionRoadContinuityTick(float elapsed){
    if(!missionActive||missionPending||missionPaused||missionFollowing||missionCasualtyWaiting||waitingForPlayer||scientificResearching||leavingBuilding||missionRescue.motionSuspended||missionRescue.regrouping||missionRescue.gateClearing||missionRescue.passage.active||missionRescue.localRecovery||!escort||escort->isDead()||escort->isBeingCarried()||escort->isInCombatMode(true,true)||!escort->getMovement()||escort->getMovement()->isIndoors())return false;
    if(!missionRescue.roadPlanned||missionRescue.roadFailed||missionRescue.roadNext+1>=missionRescue.roadPoints.size())return false;
    missionRescue.roadContinuityCooldown=std::max(0.0f,missionRescue.roadContinuityCooldown-elapsed);
    if(missionRescue.roadContinuityRetries>=2||missionRescue.roadContinuityCooldown>0||!missionRoadTravelMissing(escort))return false;
    if(!missionRescue.roadContinuityRetries)missionRescue.roadContinuityOrigin=escort->getPosition();
    ++missionRescue.roadContinuityRetries;missionRescue.roadContinuityCooldown=2;
    DebugLog("MISSION ROAD CONTINUITY id="+currentMissionFiscalId+" reason=missing native order");
    issueTravelOrder(destination,"restore missing road order");
    missionRescue.leaderWatch.fresh();return true;
}
void missionTickTravelRecovery(float elapsed,bool exiting){
    using namespace MissionMotionPolicy;
    if(!escort||!escort->getMovement()||missionRescue.motionSuspended)return;
    if(missionRescue.legacy.active){missionLegacyTick(elapsed);return;}
    MissionPassageRecovery::Episode<Ogre::Vector3>& episode=missionRescue.passage;
    if(episode.active&&episode.businessGoal.squaredDistance(destination)>1){episode.clear();missionRescue.localRecovery=false;missionRescue.leaderWatch=Watch();}
    if(episode.active){
        episode.seconds+=elapsed;
        if(episode.escaped(escort->getPosition(),missionRescue.localRecoveryTarget,missionRescue.localRecovery)){
            const size_t join=episode.joinIndex;const bool detour=missionRescue.localRecovery;
            std::ostringstream out;out<<"MISSION PASSAGE RECOVERED id="<<currentMissionFiscalId<<" from="<<episode.origin<<" now="<<escort->getPosition()<<" seconds="<<episode.seconds;DebugLog(out.str());
            episode.clear();missionRescue.localRecovery=false;missionRescue.leaderWatch=Watch();
            if(detour){if(join!=(size_t)-1&&join<missionRescue.roadPoints.size()){missionRescue.roadNext=join+1;missionRescue.roadProjectedIndex=(size_t)-1;}issueTravelOrder(exiting?exitWaypoint:destination,"next road point");return;}
        }else if(episode.seconds>=120){DebugLog("MISSION PASSAGE EXHAUSTED id="+currentMissionFiscalId+" reason=no net escape in bounded episode");missionSuspendTravelOrLegacy();return;}
        // Only the leader's active episode inhibits healthy-motion rearming.
        // Jitter/shuttling near the same obstruction must not replenish retries.
        if(episode.active)missionRescue.leaderWatch.healthy=0;
    }
    if(!exiting&&missionRescue.roadFailed){
        if(missionRescue.routeRetry.exhausted){missionSuspendTravelOrLegacy();return;}
        if(missionRescue.routeRetry.due(elapsed)){missionRescue.roadPlanned=false;missionWaitReason(Recalculating);issueTravelOrder(destination,"bounded failed route retry");}
        return;
    }
    if(!exiting&&missionRoadContinuityTick(elapsed))return;
    const Ogre::Vector3 target=missionRescue.localRecoveryTarget;
    const int previousStep=missionRescue.leaderWatch.step;
    const float previousCooldown=missionRescue.leaderWatch.cooldown;
    const Action action=missionRescue.leaderWatch.tick(escort->getPosition(),target,escort->getMovement()->isCurrentlyMoving(),escort->getMovement()->pathFailed(),elapsed);
    if(episode.active&&missionRescue.leaderWatch.step<previousStep){
        missionRescue.leaderWatch.step=previousStep;
        missionRescue.leaderWatch.cooldown=std::max(0.0f,previousCooldown-elapsed);
        missionRescue.leaderWatch.healthy=0;
    }
    if(action==None){if(missionRescue.leaderWatch.step==0)missionWaitReason(exiting?Exit:Ready);return;}
    if(!episode.active)episode.begin(escort->getPosition(),target,destination);
    std::ostringstream state;state<<"MISSION PASSAGE STEP id="<<currentMissionFiscalId<<" stage="<<(int)action<<" seconds="<<episode.seconds<<" pos="<<escort->getPosition()<<" local="<<target<<" goal="<<destination<<" native_destination="<<escort->getMovement()->pathDestination<<" path_failed="<<escort->getMovement()->pathFailed();DebugLog(state.str());
    if(action==Suspend){missionSuspendTravelOrLegacy();return;}
    missionWaitReason(Recalculating);if(action==Wait)return;
    if(action==Repath||action==Local||action==Global)escort->getMovement()->invalidatePath();
    if(action==Local){
        if(exiting){missionRejectExit();if(findExteriorWaypoint(escort,exitWaypoint)){issueTravelOrder(exitWaypoint,"alternative accessible exit");return;}}
        else {Ogre::Vector3 point;if(missionDoorTransitAlternative(point)||missionPassageAlternative(point,false)){missionPassageIssue(point,"local distinct native target");return;}}
        DebugLog("MISSION PASSAGE NO ALTERNATIVE id="+currentMissionFiscalId+" stage=local; no duplicate MOVE");return;
    }
    if(action==Global){
        if(exiting){missionRejectExit();if(!findExteriorWaypoint(escort,exitWaypoint)){missionSuspendTravelOrLegacy();return;}issueTravelOrder(exitWaypoint,"last alternative exit");return;}
        missionRescue.localRecovery=false;missionRescue.roadPlanned=false;missionRescue.roadPoints.clear();missionRescue.roadNext=0;
        Ogre::Vector3 planned;
        if(!missionRoadTarget(destination,planned)){missionWaitReason(Inaccessible);return;}
        std::ostringstream log;log<<"MISSION PASSAGE REPLAN id="<<currentMissionFiscalId<<" proposed="<<planned<<" equivalent_failed_endpoint="<<!episode.different(planned);DebugLog(log.str());
        Ogre::Vector3 point;
        if(missionDoorTransitAlternative(point)||missionPassageAlternative(point,true)){missionPassageIssue(point,"global checked distinct approach");return;}
        DebugLog("MISSION PASSAGE EXHAUSTED id="+currentMissionFiscalId+" reason=replan provides no distinct reachable endpoint");missionSuspendTravelOrLegacy();return;
    }
    // The two early stages deliberately restore/repath the current order. Only
    // Local/Global claim an alternative; they never silently reissue a reject.
    issueTravelOrder(exiting?exitWaypoint:destination,"bounded navigation recovery");
}

// Start the same bounded transit used by Local before the leader stalls.
// Two-second metadata scan; one MOVE, then normal episode supervision.
bool missionDoorApproachTick(float elapsed,bool routeHandoff=false){
    if(!missionActive||missionPending||missionPaused||missionFollowing||missionCasualtyWaiting||waitingForPlayer||scientificResearching||leavingBuilding||missionRescue.motionSuspended||!escort||escort->isDead()||escort->isBeingCarried()||escort->isInCombatMode(true,true)||!escort->getMovement()||!missionRescue.roadPlanned||missionRescue.roadFailed||missionRescue.passage.active)return false;
    missionRescue.doorApproachClock=std::max(0.0f,missionRescue.doorApproachClock-elapsed);
    if(missionRescue.doorApproachClock>0&&!routeHandoff)return false;
    missionRescue.doorApproachClock=2;
    missionRescue.passage.begin(escort->getPosition(),missionRescue.localRecoveryTarget,destination);
    Ogre::Vector3 point;
    if(!missionDoorTransitAlternative(point,true)){missionRescue.passage.clear();return false;}
    // Count this as the Local attempt, not a new unlimited recovery channel.
    missionRescue.leaderWatch.step=4;missionRescue.leaderWatch.cooldown=12;
    missionPassageIssue(point,"open doorway on approach",true);
    return true;
}
