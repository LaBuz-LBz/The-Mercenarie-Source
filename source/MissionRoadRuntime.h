#pragma once
#include "MissionRoadProgress.h"
#include "MissionRoadProjection.h"
// Resolve only local intermediate samples. The graph is an itinerary, not a
// navmesh: Kenshi must choose the actual walkable surface and route to it.
void missionRoadPrepareLocalTarget(){
    if(missionRescue.gateClearing)return; // keep the verified gate exit stable until clearance ends
    if(!escort||missionRescue.roadNext>=missionRescue.roadPoints.size())return;
    const size_t original=missionRescue.roadNext;
    if(original+1>=missionRescue.roadPoints.size())return; // exact final objective
    const Ogre::Vector3 p=escort->getPosition(),q=missionRescue.roadPoints[original];
    const float dx=p.x-q.x,dz=p.z-q.z;
    if(dx*dx+dz*dz>90000.0f)return; // do not query unloaded/distant terrain
    const bool elevated=dx*dx+dz*dz<=8100.0f&&fabs(p.y-q.y)>20.0f;
    if(missionRescue.roadProjectedIndex==original&&(!elevated||missionRescue.roadProjectionElevated))return;
    size_t candidate=original;
    // A local height mismatch usually means a stair/gate sample is underneath
    // the actor. Give native navigation room to leave that passage. Keep bends:
    // stop looking ahead if the road deviates from the original forward segment.
    if(elevated){
        const Ogre::Vector3 next=missionRescue.roadPoints[original+1];
        const float vx=next.x-q.x,vz=next.z-q.z,n=vx*vx+vz*vz;
        if(n>1.0f){
            float travelled=0,previousAlong=0;
            for(size_t i=original+1;i+1<missionRescue.roadPoints.size()&&i<=original+4;++i){
                const Ogre::Vector3 a=missionRescue.roadPoints[i-1],b=missionRescue.roadPoints[i];
                const float sx=b.x-a.x,sz=b.z-a.z;travelled+=sqrt(sx*sx+sz*sz);
                const float bx=b.x-q.x,bz=b.z-q.z,along=(bx*vx+bz*vz)/sqrt(n),cross=bx*vz-bz*vx;
                if(travelled>180.0f||along<previousAlong||cross*cross>144.0f*n)break;
                candidate=i;previousAlong=along;if(travelled>=120.0f)break;
            }
        }
    }
    Ogre::Vector3 projected;
    Ogre::Vector3 wanted=missionRescue.roadPoints[candidate];
    missionRescue.roadProjectedIndex=original;missionRescue.roadProjectionElevated=elevated;
    if(!missionProjectLocalRoadPoint(wanted,p,projected)){
        // The lookahead can be on a different level in a mountain switchback.
        // Try the current sample before leaving an unprojected elevated target.
        if(candidate==original)return;
        candidate=original;wanted=missionRescue.roadPoints[original];
        if(!missionProjectLocalRoadPoint(wanted,p,projected))return;
    }
    // Reject a projection back onto the actor: that cannot clear the passage.
    if(candidate!=original&&projected.squaredDistance(p)<256.0f)return;
    missionRescue.roadPoints[candidate]=projected;missionRescue.roadNext=candidate;missionRescue.roadProjectedIndex=candidate;
    if(candidate!=original||projected.squaredDistance(wanted)>1.0f){std::ostringstream log;log<<"MISSION ROAD LOCAL NAV id="<<currentMissionFiscalId<<" from_index="<<original<<" to_index="<<candidate<<" actor="<<p<<" raw="<<wanted<<" projected="<<projected;DebugLog(log.str());}
}
bool missionRoadTarget(const Ogre::Vector3& requested,Ogre::Vector3& target){
    target=requested;
    if(missionRescue.passage.active&&missionRescue.passage.businessGoal.squaredDistance(destination)>1){missionRescue.passage.clear();missionRescue.localRecovery=false;missionRescue.leaderWatch=MissionMotionPolicy::Watch();}
    if(missionRescue.passage.active&&missionRescue.localRecovery){target=missionRescue.localRecoveryTarget;return true;}
    if(leavingBuilding||scientificResearching||requested.squaredDistance(destination)>1)return true;
    if(missionRescue.roadGoal.squaredDistance(destination)>1)missionRescue.routeRetry=MissionMotionPolicy::RouteRetry();
    if(!missionRescue.roadPlanned||missionRescue.roadGoal.squaredDistance(destination)>1){
        static PricingRoads::Graph graph;static bool loaded=false;
        if(!loaded){try{std::ifstream file("mods/Guild Escort Contracts/pricing-roads.dat");graph.read(file);loaded=!graph.points.empty();}catch(const std::exception& e){ErrorLog(std::string("MISSION ROAD DATA: ")+e.what());}}
        Ogre::Vector3 p=escort->getPosition();std::vector<RoutePrototype::Point> path;
        bool ok=loaded&&MissionRoadPath::plan(graph,RoutePrototype::Point(p.x,p.y,p.z),RoutePrototype::Point(destination.x,destination.y,destination.z),path);
        missionRescue.roadPoints.clear();missionRescue.roadNext=0;missionRescue.roadProjectedIndex=(size_t)-1;missionRescue.roadGoal=destination;missionRescue.roadPlanned=true;missionRescue.roadFailed=!ok;
        if(!ok)missionRescue.routeRetry.failed();
        else missionRescue.routeRetry=MissionMotionPolicy::RouteRetry();
        if(ok)for(size_t i=0;i<path.size();++i)missionRescue.roadPoints.push_back(Ogre::Vector3((float)path[i].x,(float)path[i].y,(float)path[i].z));
        std::ostringstream log;log<<"MISSION ROAD PLAN id="<<currentMissionFiscalId<<" success="<<ok<<" points="<<path.size()<<" from="<<p<<" destination="<<destination;DebugLog(log.str());
    }
    if(missionRescue.roadFailed)return false;
    missionRoadPrepareLocalTarget();
    if(missionRescue.roadNext<missionRescue.roadPoints.size())target=missionRescue.roadPoints[missionRescue.roadNext];
    return true;
}
bool missionRoadAdvance(){
    if(missionRescue.passage.active&&missionRescue.localRecovery)return false;
    if(!missionActive||missionPending||missionPaused||missionFollowing||missionCasualtyWaiting||waitingForPlayer||scientificResearching||leavingBuilding||!escort||escort->isDead()||escort->isBeingCarried()||escort->isInCombatMode(true,true))return false;
    if(!missionRescue.roadPlanned||missionRescue.roadFailed||missionRescue.roadNext>=missionRescue.roadPoints.size())return false;
    const size_t oldIndex=missionRescue.roadNext;const Ogre::Vector3 oldTarget=missionRescue.roadPoints[oldIndex];
    // Outdoor itinerary samples are reached in XZ. Height belongs to native
    // walking, not to intermediate road arrival. Keep doors and final goals 3D.
    const Ogre::Vector3 current=escort->getPosition();
    const float nearX=current.x-oldTarget.x,nearZ=current.z-oldTarget.z;
    const bool passed=MissionRoadProgress::passedSample(missionRescue.roadPoints,oldIndex,current);
    const bool handoff=!missionRescue.motionSuspended&&!missionRescue.regrouping&&missionRoadCanHandoff()&&MissionRoadProgress::handoffSample(missionRescue.roadPoints,oldIndex,current);
    if(!missionRescue.gateClearing&&oldIndex+1<missionRescue.roadPoints.size()&&
       (nearX*nearX+nearZ*nearZ<=256.0f||passed||handoff)&&missionRoadUsesHorizontalArrival()){
        if(handoff&&nearX*nearX+nearZ*nearZ>256.0f){
            std::ostringstream log;log<<"MISSION ROAD EARLY HANDOFF id="<<currentMissionFiscalId<<" index="<<oldIndex<<" actor="<<current<<" sample="<<oldTarget;DebugLog(log.str());
        }
        if(passed&&nearX*nearX+nearZ*nearZ>256.0f){
            std::ostringstream log;log<<"MISSION ROAD PASSED SAMPLE id="<<currentMissionFiscalId<<" index="<<oldIndex<<" actor="<<current<<" sample="<<oldTarget;DebugLog(log.str());
        }
        if(fabs(current.y-oldTarget.y)>40.0f){
            std::ostringstream log;log<<"MISSION ROAD XZ ARRIVAL id="<<currentMissionFiscalId<<" index="<<oldIndex<<" actor="<<current<<" sample="<<oldTarget;DebugLog(log.str());
        }
        // A consumed road sample plus net displacement proves genuine escape,
        // even when the old failed sample is now far behind or above the actor.
        if(missionRescue.passage.active&&!missionRescue.localRecovery){
            const Ogre::Vector3 origin=missionRescue.passage.origin;
            const float ex=current.x-origin.x,ez=current.z-origin.z;
            if(ex*ex+ez*ez>=400.0f){
                DebugLog("MISSION PASSAGE RECOVERED id="+currentMissionFiscalId+" reason=forward road arrival");
                missionRescue.passage.clear();missionRescue.leaderWatch=MissionMotionPolicy::Watch();
            }
        }
        const float rx=current.x-missionRescue.roadContinuityOrigin.x,rz=current.z-missionRescue.roadContinuityOrigin.z;
        if(missionRescue.roadContinuityRetries&&rx*rx+rz*rz>=1600.0f){missionRescue.roadContinuityRetries=0;missionRescue.roadContinuityCooldown=0;}
        ++missionRescue.roadNext;return true;
    }
    missionRoadPrepareLocalTarget();
    if(oldIndex!=missionRescue.roadNext||oldTarget.squaredDistance(missionRescue.roadPoints[missionRescue.roadNext])>1.0f)return true;
    const Ogre::Vector3 p=escort->getPosition();const Ogre::Vector3 q=missionRescue.roadPoints[missionRescue.roadNext];
    float dx=p.x-q.x,dz=p.z-q.z;
    if(dx*dx+dz*dz>256.0f||fabs(p.y-q.y)>40)return false;
    ++missionRescue.roadNext;return true;
}
// Road samples are interpolated world-data positions, not reachable navmesh
// anchors. At a gate/stair a native move can finish nearby on another height.
// After the existing no-progress timeout, bypass only one nearby intermediate
// sample, retaining the road itinerary and the exact final destination.
bool missionRoadRecoverStalledPoint(bool timedOut,bool moving){
    if(!timedOut||moving||!missionActive||missionPending||missionPaused||missionFollowing||missionCasualtyWaiting||waitingForPlayer||scientificResearching||leavingBuilding||!escort||escort->isDead()||escort->isBeingCarried()||escort->isInCombatMode(true,true))return false;
    if(!missionRescue.roadPlanned||missionRescue.roadFailed||missionRescue.roadNext>=missionRescue.roadPoints.size()||missionRescue.roadPoints.size()-missionRescue.roadNext<=1)return false;
    const Ogre::Vector3 p=escort->getPosition(),q=missionRescue.roadPoints[missionRescue.roadNext];
    const float dx=p.x-q.x,dz=p.z-q.z;
    if(dx*dx+dz*dz>3600.0f)return false; // one sample spacing; never skip a distant obstruction
    std::ostringstream log;log<<"MISSION ROAD RECOVERY id="<<currentMissionFiscalId<<" reason=stalled intermediate sample index="<<missionRescue.roadNext<<" position="<<p<<" rejected="<<q<<" next="<<missionRescue.roadPoints[missionRescue.roadNext+1];DebugLog(log.str());
    ++missionRescue.roadNext;return true;
}
