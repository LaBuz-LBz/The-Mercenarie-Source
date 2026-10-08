#pragma once
bool missionGuidancePending(){return caravanMission&&currentContract.routeRegions.find(";NAVGUIDE=P;")!=std::string::npos;}
bool missionPlayerGuided(){return caravanMission&&currentContract.routeRegions.find(";NAVGUIDE=1;")!=std::string::npos;}
void missionClearGuidanceTag(){
    size_t p=currentContract.routeRegions.find(";NAVGUIDE=");if(p!=std::string::npos){size_t e=currentContract.routeRegions.find(';',p+10);currentContract.routeRegions.erase(p,e==std::string::npos?std::string::npos:e-p+1);}
}
bool missionNavigationNotice(MissionMotionPolicy::Reason reason){
    if(!caravanMission||(reason!=MissionMotionPolicy::Recalculating&&reason!=MissionMotionPolicy::Inaccessible&&reason!=MissionMotionPolicy::Suspended))return false;
    if(!missionPlayerGuided()&&!missionGuidancePending()&&!missionRescue.navigationRP.thinking){
        missionRescue.navigationRP.thinking=true;missionRescue.navigationRP.spoken=false;missionRescue.navigationRP.moving=0;
        if(escort)missionRescue.navigationRP.origin=escort->getPosition();
    }
    return true; // Navigation problems are spoken by the chief, never a popup.
}
bool missionRequestPlayerGuide(){
    if(!caravanMission||!missionActive||missionPending||caravanTradeActive())return false;
    if(!missionGuidancePending()&&!missionPlayerGuided())currentContract.routeRegions+=";NAVGUIDE=P;";
    missionRescue.motionSuspended=false;missionPaused=false;
    missionRescue.navigationRP.awaitingGuide=missionGuidancePending();missionRescue.regrouping=false;
    missionRescue.passage.clear();missionRescue.localRecovery=false;missionRescue.gateClearing=false;missionRescue.legacy.active=false;
    missionRescue.leaderWatch=MissionMotionPolicy::Watch();missionGroup.reset();
    if(missionGuidancePending()&&tradeActorAvailable(escort))missionHoldTravel(escort,"await player guidance");
    return true;
}
void missionNavigationRPTick(float elapsed){
    if(!caravanMission||!missionActive||missionPending||!escort)return;
    if(missionPlayerGuided()&&missionFollowing){Character* target=missionFollowTarget.getCharacter();
        if(!target||target->isDead()||target->isBeingCarried()||!rescueCanWalk(target)){clearMissionFollow();missionRequestPlayerGuide();}
    }
    if(missionGuidancePending()){
        if(missionPaused||!tradePartyAvailable()||!ou||!ou->player)return;
        Character* target=0;float best=9000000;
        for(unsigned int i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];
            if(!c||c->isDead()||c->isBeingCarried()||!rescueCanWalk(c))continue;
            float d=c->getPosition().squaredDistance(escort->getPosition());if(d<best){target=c;best=d;}}
        if(!target)return; // Wait for a nearby available player; retain the request.
        clearMissionFollow();releaseWaitingHere();missionClearGuidanceTag();currentContract.routeRegions+=";NAVGUIDE=1;";
        missionFollowTarget=target->getHandle();missionFollowing=true;waitingForPlayer=false;
        missionRescue.navigationRP=MissionNavigationRP::State<Ogre::Vector3>();
        missionClearTravel(escort,"player guides caravan");missionIssueOrder(escort,FOLLOW_PLAYER_ORDER,target,target->getPosition());
        missionFollowers.push_back(escort->getHandle());updateMissionFormation(0);
        escort->sayALine(Loc::text("caravan.navigation.guide"),true);
        DebugLog("MISSION PLAYER GUIDE begin id="+currentMissionFiscalId+" player="+target->getHandle().toString());return;
    }
    if(missionPlayerGuided()||missionPaused||missionFollowing||caravanTradeActive()||!tradePartyAvailable())return;
    MissionNavigationRP::State<Ogre::Vector3>& rp=missionRescue.navigationRP;
    if(!rp.thinking)return;
    if(!rp.spoken){escort->sayALine(Loc::text("caravan.navigation.thinking"),true);rp.spoken=true;return;}
    // A successful request alone is not recovery. Wait for real movement.
    if(escort->getMovement()->isCurrentlyMoving()&&!escort->getMovement()->pathFailed()&&escort->getPosition().squaredDistance(rp.origin)>=400)rp.moving+=elapsed;
    else rp.moving=0;
    if(rp.moving>=3){escort->sayALine(Loc::text("caravan.navigation.recovered"),true);rp=MissionNavigationRP::State<Ogre::Vector3>();}
}
bool missionFollowingAtDestination(){
    return escort&&!missionCombatThreat(escort)&&escort->getMedical()&&!escort->getMedical()->isUnconcious()
        &&escort->getPosition().squaredDistance(destination)<=(scientificMission&&!scientificReturning?6400.0f:250000.0f);
}
