#pragma once
bool missionLegacyEligible(){
    return missionActive&&!missionPending&&!missionPaused&&!missionFollowing&&!waitingForPlayer&&!missionCasualtyWaiting&&!scientificResearching&&!leavingBuilding&&!caravanTradeActive()
        &&escort&&escort->getMovement()&&!escort->getMovement()->isIndoors()&&!escort->isDead()&&!escort->isBeingCarried()
        &&rescueCanWalk(escort)&&!missionCombatThreat(escort)&&!missionNativeSurvival(escort)
        &&!missionNativeTaskPresent(escort,FIRST_AID_ORDER)&&!missionNativeTaskPresent(escort,LIFT_PERSON_PLAYER_ORDER)&&!missionNativeTaskPresent(escort,PICKUP)
        &&missionRescue.tasks.empty()&&!missionRescue.regrouping;
}
bool missionTryLegacyFallback(){
    if(!missionLegacyEligible()||!missionRescue.legacy.begin(destination))return false;
    missionRescue.legacy.cooldown=8;
    missionRescue.passage.clear();missionRescue.localRecovery=false;missionRescue.gateClearing=false;
    missionRescue.roadPoints.clear();missionRescue.roadNext=0;missionRescue.roadPlanned=false;missionRescue.roadFailed=false;
    missionRescue.leaderWatch=MissionMotionPolicy::Watch();
    DebugLog("MISSION V8 FALLBACK begin id="+currentMissionFiscalId);
    issueTravelOrder(destination,"V8 last resort native navigation");return true;
}
void missionSuspendTravelOrLegacy(){if(!missionTryLegacyFallback())missionSuspendMotion();}
void missionLegacyTick(float elapsed){
    if(!missionRescue.legacy.active||!missionLegacyEligible())return;
    int action=missionRescue.legacy.tick(escort->getPosition().squaredDistance(destination),escort->getMovement()->isCurrentlyMoving(),escort->getMovement()->pathFailed(),elapsed);
    if(action<0){DebugLog("MISSION V8 FALLBACK exhausted id="+currentMissionFiscalId);missionSuspendMotion();return;}
    if(action>0){escort->getMovement()->invalidatePath();issueTravelOrder(destination,"V8 bounded native retry");}
}
