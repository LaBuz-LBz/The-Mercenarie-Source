#pragma once
// Mission-created parties only. Never use this on player characters or visitors.
bool missionTravelTask(TaskType type){
    return type==MOVE_CUS_ORDERED||type==FOLLOW_PLAYER_ORDER||type==FOLLOW_SQUADLEADER
        ||type==BODYGUARD||type==HOLD_POSITION||type==UNLOCK_DOOR_HERE;
}
bool missionNativeSurvival(Character* actor){
    if(!actor)return false;
    if(actor->isInCombatMode(true,true))return true;
    if(!actor->getAI()||!actor->getAI()->getTaskSystem())return false;
    const TaskType type=actor->getAI()->getTaskSystem()->getCurrentGoal().key();
    return type==RUN_AWAY||type==RUN_AWAY_HOMETOWN||type==RUN_AWAY_FORCED
        ||type==TOTAL_ESCAPE||type==ESCAPE_KIDNAP||type==ESCAPE_KIDNAP_STR
        ||type==FOLLOW_URGENT_ESCAPE||type==SELF_PRESERVATION;
}
void cleanupMissionActorAI(Character* actor,const char* reason){
    if(!actor||actor->isPlayerCharacter())return;
    AITaskSytem* tasks=actor->getAI()?actor->getAI()->getTaskSystem():0;
    const TaskType oldGoal=tasks?tasks->getCurrentGoal().key():NULL_TASK;
    // These spawned NPCs receive their explicit queue from the mission DLL.
    // Native autonomous goals and squad packages are separate from this queue.
    if(tasks)tasks->clearOrders();
    const TaskType owned[]={MOVE_CUS_ORDERED,FOLLOW_PLAYER_ORDER,FOLLOW_SQUADLEADER,BODYGUARD,HOLD_POSITION,UNLOCK_DOOR_HERE};
    for(size_t i=0;i<sizeof(owned)/sizeof(owned[0]);++i){actor->removeJob(owned[i]);if(tasks)tasks->removeGoal(owned[i]);}
    // Removing a job does not necessarily terminate its executing goal.
    if(tasks&&missionTravelTask(tasks->getCurrentGoal().key()))tasks->clearCurrentGoal(true);
    if(actor->getAI()){
        hand none;none.setNull();actor->getAI()->setCenterOfMovementTarget(none);
        actor->getAI()->setCenterOfMovement(actor->getPosition());
        actor->getAI()->setManuveringFreedomLevel(AI::ROAM_FAR);
    }
    if(actor->getMovement()){
        actor->getMovement()->leaveSpeedGroup();
        actor->getMovement()->setDesiredSpeedOrders(RUN);
        actor->getMovement()->restoreDesiredSpeed();
        actor->getMovement()->setRoadPreference(0.0f);
        // Never halt a surviving combat/escape/medical goal or drop a passenger.
        if(missionTravelTask(oldGoal)){actor->getMovement()->halt();actor->getMovement()->invalidatePath();}
    }
    std::ostringstream out;out<<"MISSION AI ORDER id="<<currentMissionFiscalId<<" npc="<<actor->getHandle().toString()
        <<" state=RELEASED oldGoal="<<(int)oldGoal<<" newGoal="<<(tasks?(int)tasks->getCurrentGoal().key():-1)
        <<" reason="<<reason;DebugLog(out.str());
}
void finalizeMissionAI(){
    clearMissionDefence();
    // Disable every producer before releasing actors, including speed hooks.
    missionActive=false;missionPending=false;missionFollowing=false;missionPaused=false;
    missionFollowTarget.setNull();missionFollowers.clear();waitingHere.clear();
    missionGroup.reset();missionTemporaryLeader=0;missionCasualtyWaiting=false;
    // Keep native treatment and physical carrying; only retire our scheduler.
    for(size_t i=0;i<missionRescue.tasks.size();++i){
        Character* helper=missionRescue.tasks[i].helper.getCharacter();
        if(!helper||helper->isPlayerCharacter())continue;
        const TaskType medical[]={FIRST_AID_ORDER,LIFT_PERSON_PLAYER_ORDER,PICKUP};
        for(size_t j=0;j<3;++j){helper->removeJob(medical[j]);if(helper->getAI()&&helper->getAI()->getTaskSystem())helper->getAI()->getTaskSystem()->removeGoal(medical[j]);}
    }
    missionRescue=MissionRescueState();
    for(size_t i=0;i<progressMembers.size();++i)cleanupMissionActorAI(progressMembers[i].getCharacter(),"mission finalized");
    if(escort)cleanupMissionActorAI(escort,"mission leader finalized");
    destination=Ogre::Vector3::ZERO;exitWaypoint=Ogre::Vector3::ZERO;
}

void observeRetiredMissionActor(Character* c,std::set<std::string>& released,std::map<std::string,int>& observed){
    if(!c)return;
    const std::string id=c->getHandle().toString();
    if(released.insert(id).second)cleanupMissionActorAI(c,"retired party / load repair");
    if(!c->getAI()||!c->getAI()->getTaskSystem())return;
    AITaskSytem* tasks=c->getAI()->getTaskSystem();const int goal=(int)tasks->getCurrentGoal().key();
    if(!observed.count(id)||observed[id]!=goal){
        std::ostringstream out;out<<"MISSION AI ORDER state=RETIRED npc="<<id
            <<" oldGoal="<<(observed.count(id)?observed[id]:-1)<<" newGoal="<<goal
            <<" target="<<tasks->getCurrentGoal().subject.toString()<<" reason=post_completion_observation";
        DebugLog(out.str());observed[id]=goal;
    }
}
