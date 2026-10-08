#pragma once
bool canRecoverMission(Character* speaker){
    if(!speaker||speaker->isPlayerCharacter()||speaker->isDead()||!escort
        ||!missionActive||missionPending||contractLifecycle!=CONTRACT_ACTIVE
        ||currentContract.settlementPaid||(finalWindow&&finalWindow->getVisible()))return false;
    if(speaker==escort)return true;
    for(size_t i=0;i<progressMembers.size();++i)if(progressMembers[i].getCharacter()==speaker)return true;
    return false;
}
bool recoverMissionOrders(Character* speaker){
    if(!canRecoverMission(speaker))return false;
    missionFollowing=false;missionFollowTarget.setNull();missionFollowers.clear();
    missionPaused=false;waitingForPlayer=false;waitingHere.clear();
    missionGroup.reset();missionTemporaryLeader=0;
    missionRescue.legacy=MissionLegacyNavigation::State<Ogre::Vector3>();
    missionRescue.passage.clear();missionRescue.leaderWatch=MissionMotionPolicy::Watch();missionRescue.routeRetry=MissionMotionPolicy::RouteRetry();
    missionRescue.motionSuspended=false;missionRescue.localRecovery=false;missionRescue.regroupSeconds=0;missionRescue.failedExitDoors.clear();
    missionRescue.holds.clear();missionRescue.roadPoints.clear();missionRescue.roadNext=0;
    missionRescue.roadPlanned=false;missionRescue.roadFailed=false;missionRescue.regrouping=false;
    missionRescue.safeSeconds=0;missionRescue.lastSpeed=-1;
    missionRescue.recoveryPending=true;missionCasualtyWaiting=true;
    stationaryClock=0;incidentClock=-1;
    for(size_t i=0;i<progressMembers.size();++i){
        Character* c=progressMembers[i].getCharacter();if(!c||c->isPlayerCharacter())continue;
        // Preserve a live medical assignment. Its existing scheduler validates
        // the patient, carrier and kit before retrying; never reset its owner.
        bool treating=false;
        for(size_t t=0;t<missionRescue.tasks.size();++t)
            if(missionRescue.tasks[t].helper.getCharacter()==c)treating=true;
        if(!treating)missionClearTravel(c,"manual mission recovery");
    }
    // Medical reservations and retired leaders intentionally survive recovery.
    // The normal rescue tick waits for safety, validates them and regroups.
    DebugLog(std::string("MISSION RECOVERY requested id=")+currentMissionFiscalId);
    return true;
}
void resumeRecoveredMissionPhase(){
#ifdef MERCENARIE_CARAVAN_TRADE
    if(caravanTradeActive())return;
#endif
    if(!missionActive||missionPending||!escort||missionPaused||waitingForPlayer||missionCasualtyWaiting||missionRescue.motionSuspended)return;
    if(missionRescue.passage.active&&missionRescue.passage.businessGoal.squaredDistance(destination)>1){missionRescue.passage.clear();missionRescue.localRecovery=false;missionRescue.leaderWatch=MissionMotionPolicy::Watch();}
    missionRescue.leaderWatch.fresh();
    if(missionRescue.passage.active&&missionRescue.localRecovery&&!leavingBuilding){
        missionClearTravel(escort,"resume passage alternative");missionIssueOrder(escort,MOVE_CUS_ORDERED,0,missionRescue.localRecoveryTarget);applyMissionPace();return;
    }
    missionRescue.localRecovery=false;
    if(leavingBuilding){
        // Exit takes priority over return/FOLLOW and survives combat and rescue.
        if(!findExteriorWaypoint(escort,exitWaypoint)){missionSuspendMotion();return;}
        bool follow=missionFollowing;missionFollowing=false;issueTravelOrder(exitWaypoint,"resume exit");missionFollowing=follow;
    }else if(scientificResearching){
        if(scientificWillEnter&&!scientificEntryResolved&&missionScienceSelectBuilding())
            missionIssueOrder(escort,UNLOCK_DOOR_HERE,0,missionScienceEntryPoint());
        else scientificMoveClock=0;
        updateMissionFormation(0);
    }else if(missionFollowing){
        missionFollowing=true;
        Character* target=missionFollowTarget.getCharacter();
        if(target){missionClearTravel(escort,"resume player follow");missionIssueOrder(escort,FOLLOW_PLAYER_ORDER,target,target->getPosition());}
        else missionWaitReason(MissionMotionPolicy::Unavailable);
    }else issueTravelOrder(destination,"resume business phase");
    applyMissionPace();
    DebugLog(std::string("MISSION RECOVERY resumed id=")+currentMissionFiscalId);
}
