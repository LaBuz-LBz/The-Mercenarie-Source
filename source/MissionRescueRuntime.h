#pragma once
bool missionNativeSurvival(Character* actor);
void resumeRecoveredMissionPhase();
// Included in the mission namespace. One state is captured per quest context.
bool rescueCanWalk(Character* c){
    return c&&!c->isDead()&&c->getMedical()&&!c->getMedical()->isUnconcious()
        &&c->getMedical()->canGetUpWakeUp()&&!c->getMedical()->isCrippled()
        &&c->getStats()&&c->getStats()->getMaxRunSpeed()>0;
}
bool rescueHelperValid(Character* c){
    return rescueCanWalk(c)&&!c->isAnimal()&&!c->isBeingCarried()
        &&!c->isDown()&&!c->getMedical()->isProbablyDying();
}
bool rescueHasKit(Character* c){
    if(!c||!c->getInventory())return false;
    lektor<Item*> kits;c->getInventory()->getAllItemsWithFunction(kits,ITEM_FIRSTAID);
    for(unsigned int i=0;i<kits.size();++i)if(kits[i]&&kits[i]->chargesLeft>0)return true;
    return false;
}
void rescueLog(const char* event,Character* patient=0,Character* helper=0){
    std::ostringstream s;s<<"MISSION "<<event<<" id="<<currentMissionFiscalId;
    if(patient)s<<" patient="<<patient->getHandle().toString()<<" leader="<<(patient==escort);
    if(helper)s<<" helper="<<helper->getHandle().toString();DebugLog(s.str());
}
// removeJob alone does not end the task already running inside the native AI.
// Only end medical/carry goals owned by this rescue assignment, never combat.
void rescueStopAction(Character* helper,const char* reason){
    if(!helper)return;
    missionClearTravel(helper,reason);
    missionClearTravel(helper,reason,FIRST_AID_ORDER);
    missionClearTravel(helper,reason,LIFT_PERSON_PLAYER_ORDER);
    // Only the obsolete pickup action from the previous rescue implementation
    // is removed by type. Live medical queues remain owned by their assignment.
    helper->removeJob(PICKUP);
}
void rescueRelease(MissionRescueTask& task){
    Character* helper=task.helper.isNull()?0:task.helper.getCharacter();
    if(helper)rescueStopAction(helper,"rescue assignment released");
    task.helper.setNull();task.retry=0;
}
void clearMissionRescue(){
    clearMissionDefence();
    for(size_t i=0;i<missionRescue.holds.size();++i){Character* c=missionRescue.holds[i].actor.getCharacter();if(c&&c->getAI())c->getAI()->setManuveringFreedomLevel(AI::ROAM_FAR);}

    for(size_t i=0;i<missionRescue.tasks.size();++i)rescueRelease(missionRescue.tasks[i]);
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(c&&c->getMovement()){c->getMovement()->leaveSpeedGroup();c->getMovement()->setDesiredSpeedOrders(missionActive?(missionPace==EscortPace::Accelerated?RUN:WALK):RUN);}}
    missionRescue=MissionRescueState();
}
void giveMissionSpawnKits(Character* c){
    if(!c||!ou||!ou->theFactory)return;
    GameData* kit=ou->gamedata.getData("209-gamedata.base",ITEM);
    if(!kit){ErrorLog("MISSION SPAWN KITS: vanilla kit missing");return;}
    int inserted=0;
    for(int i=0;i<4;++i){Item* item=ou->theFactory->createItem(kit,hand(),0,0,0,0);if(item&&c->giveItem(item,false,true))++inserted;}
    std::ostringstream s;s<<"MISSION SPAWN KITS actor="<<c->getHandle().toString()<<" added="<<inserted;DebugLog(s.str());
    if(inserted!=4)ErrorLog("MISSION SPAWN KITS: inventory full, no retry/no ground drop");
}
bool rescueRetired(Character* c){for(size_t i=0;i<missionRescue.retiredLeaders.size();++i)if(c&&missionRescue.retiredLeaders[i].toString()==c->getHandle().toString())return true;return false;}
void rescueRetireDialogue(Character* c){
    if(!c||c==escort||!c->dialogue)return;
    if(!c->dialogue->_hasEnded)c->dialogue->endDialogue(true);
    c->dialogue->clearConversationList(EV_PLAYER_TALK_TO_ME);
}
bool rescueSucceedLeader(){
    if(rescueHelperValid(escort))return true;
    // Heal first when the living leader is still on the ground.
    if(escort&&!escort->isDead()&&!escort->isBeingCarried())return false;
    Character* next=missionGroupCarrierOf(escort);
    if(!rescueHelperValid(next)||rescueRetired(next))next=0;
    for(size_t i=0;!next&&i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(c!=escort&&rescueHelperValid(c)&&!rescueRetired(c))next=c;}
    if(!next)return false;
    Character* old=escort;if(old)missionRescue.retiredLeaders.push_back(old->getHandle());
    rescueLog(old&&old->isDead()?"LEADER SUCCESSION dead":"LEADER SUCCESSION KO",old,next);
    missionClearTravel(old,"old leader retired");missionClearTravel(next,"carrier promoted");
    escort=next;escortHandle=next->getHandle();missionTemporaryLeader=0;missionGroup.reset();
    rescueRetireDialogue(old);
    // Force exactly one route/follow restoration, even for sudden death without prior hold.
    missionCasualtyWaiting=true;missionOrderTrace(next,"leader committed",old);
    if(next->getPlatoon())next->getPlatoon()->setSquadLeader(next);
    if(next->dialogue){GameData* conversation=ou->gamedata.getData("880024-Guild Escort Contracts.mod",DIALOGUE);if(conversation){next->dialogue->clearConversationList(EV_PLAYER_TALK_TO_ME);next->dialogue->addConversation(conversation,EV_PLAYER_TALK_TO_ME);}}
    return true;
}
// Returns true while normal travel must remain suspended. Called once per second.
bool tickMissionRescue(float elapsed){
    // Also repair native dialogue restored from saves made before this fix.
    for(size_t i=0;i<missionRescue.retiredLeaders.size();++i)rescueRetireDialogue(missionRescue.retiredLeaders[i].getCharacter());
    bool threat=false,missing=false;
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(!c){if(!progressDeadMembers.count((unsigned int)i))missing=true;continue;}if(!c->isDead()&&missionNativeSurvival(c))threat=true;
        lektor<hand> attackers;c->getAllAttackers(attackers);for(unsigned int a=0;a<attackers.size();++a){Character* enemy=attackers[a].getCharacter();if(enemy&&!enemy->isDead()&&!enemy->isDown()&&enemy->getPosition().squaredDistance(c->getPosition())<3600.0f)threat=true;}}
    missionRescue.safeSeconds=threat?0:missionRescue.safeSeconds+elapsed;
    const bool safe=missionRescue.safeSeconds>=2.0f;
    const bool recovering=missionRescue.recoveryPending;
    if(recovering&&!safe)return true;
    // Observe native carrying first, including a player carrier. Never duplicate it.
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(!c||c->isDead())continue;
        // scoreFirstAidNeed reads a periodically cached value. Refresh from actual
        // flesh + bandaging before choosing aid versus carry (KO is not a wound).
        if(c->getMedical())c->getMedical()->precalculateFirstAidNeedScore();
        Character* carrier=missionGroupCarrierOf(c);
        if(carrier&&safe&&rescueCanWalk(c)){carrier->dropCarriedObject(false,false);missionClearTravel(c,"recovered passenger, erase old route");missionGroup.invalidate(c);rescueLog("CARRY RELEASE recovered",c,carrier);}
        if(c->isBeingCarried()||(!c->isDown()&&rescueCanWalk(c)&&c->getMedical()->scoreFirstAidNeed(false)<=0))continue;
        bool exists=false;for(size_t t=0;t<missionRescue.tasks.size();++t)if(missionRescue.tasks[t].patient.toString()==c->getHandle().toString())exists=true;
        if(!exists){MissionRescueTask task;task.patient=c->getHandle();missionRescue.tasks.push_back(task);rescueLog("CASUALTY",c);}
    }
    bool hold=missing;
    std::set<std::string> reserved;
    for(size_t t=0;t<missionRescue.tasks.size();++t){MissionRescueTask& task=missionRescue.tasks[t];Character* c=task.patient.getCharacter();Character* h=task.helper.isNull()?0:task.helper.getCharacter();
        const bool needsAid=c&&!c->isDead()&&c->getMedical()->scoreFirstAidNeed(false)>0;
        if(!c||c->isDead()||c->isBeingCarried()||(!needsAid&&rescueCanWalk(c))){if(c&&c->isBeingCarried()&&h)rescueLog("CARRY CONFIRMED",c,h);rescueRelease(task);continue;}
        if(!rescueHelperValid(h)||h->isCarryingSomething||(needsAid&&!rescueHasKit(h))||reserved.count(h->getHandle().toString()))rescueRelease(task);
        else reserved.insert(h->getHandle().toString());
    }
    // Stop route orders before issuing medical jobs; never halt an assigned medic.
    for(size_t t=0;t<missionRescue.tasks.size();++t){Character* c=missionRescue.tasks[t].patient.getCharacter();if(c&&!c->isDead()&&!c->isBeingCarried()&&(c->getMedical()->scoreFirstAidNeed(false)>0||!rescueCanWalk(c)))hold=true;}
    if(hold){
        missionWaitReason(missing?MissionMotionPolicy::Unavailable:MissionMotionPolicy::Care);
        missionGroup.reset();
        for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(!c||c->isDead()||c->isBeingCarried()||c->isInCombatMode(true,true))continue;c->removeJob(FOLLOW_PLAYER_ORDER);c->removeJob(FOLLOW_SQUADLEADER);c->removeJob(BODYGUARD);if(!reserved.count(c->getHandle().toString())){missionHoldTravel(c,"casualty hold");}}
    }
    for(size_t t=0;t<missionRescue.tasks.size();){MissionRescueTask& task=missionRescue.tasks[t];Character* c=task.patient.getCharacter();
        if(!c){++t;continue;} // Streaming is not death; retain the handle.
        const bool aid=!c->isDead()&&c->getMedical()->scoreFirstAidNeed(false)>0;
        if(c->isDead()||c->isBeingCarried()||(!aid&&rescueCanWalk(c))){rescueRelease(task);missionRescue.tasks.erase(missionRescue.tasks.begin()+t);continue;}
        if(!safe){if(task.phase!=MissionRescueTask::WaitingForSafe){rescueRelease(task);task.phase=MissionRescueTask::WaitingForSafe;rescueLog("WAITING FOR SAFE",c);}++t;continue;}
        Character* helper=task.helper.isNull()?0:task.helper.getCharacter();
        if(!helper){float best=1e30f;for(size_t i=0;i<progressMembers.size();++i){Character* h=progressMembers[i].getCharacter();if(h==c||!rescueHelperValid(h)||h->isCarryingSomething||h->isInCombatMode(true,true)||reserved.count(h->getHandle().toString())||(aid&&!rescueHasKit(h)))continue;float d=h->getPosition().squaredDistance(c->getPosition());if(d<best){best=d;helper=h;}}
            // A conscious walking patient can use their own kit when no other
            // mission medic is available. Never assign self-carry or steal a medic.
            if(!helper&&aid&&rescueHelperValid(c)&&!c->isCarryingSomething&&!c->isInCombatMode(true,true)&&!reserved.count(c->getHandle().toString())&&rescueHasKit(c))helper=c;
            if(helper){task.helper=helper->getHandle();reserved.insert(helper->getHandle().toString());task.phase=MissionRescueTask::Detected;task.retry=0;}}
        if(!helper){++t;continue;}
        task.retry-=elapsed;
        task.diagnosticClock-=elapsed;
        if(task.diagnosticClock<=0){
            std::ostringstream out;out<<"MISSION RESCUE STATE id="<<currentMissionFiscalId<<" patient="<<c->getHandle().toString()<<" helper="<<helper->getHandle().toString()<<" aid_score="<<c->getMedical()->scoreFirstAidNeed(false)<<" ko="<<c->getMedical()->isUnconcious()<<" phase="<<(int)task.phase<<" distance_squared="<<helper->getPosition().squaredDistance(c->getPosition());DebugLog(out.str());
            missionOrderTrace(helper,"rescue action sample",c);task.diagnosticClock=10;
        }
        if(aid){if(task.phase!=MissionRescueTask::FirstAid||(task.retry<=0&&!missionNativeTaskPresent(helper,FIRST_AID_ORDER))){rescueStopAction(helper,"first aid issued");missionIssueOrder(helper,FIRST_AID_ORDER,c,c->getPosition());if(task.phase!=MissionRescueTask::FirstAid)rescueLog("MEDIC ASSIGNED",c,helper);task.phase=MissionRescueTask::FirstAid;task.retry=10;}}
        else {if(task.phase!=MissionRescueTask::CarrierAssigned){rescueStopAction(helper,"treatment finished: switch to carry");rescueLog("MEDIC COMPLETE",c,helper);task.phase=MissionRescueTask::CarrierAssigned;task.retry=0;rescueLog("CARRY ASSIGNED",c,helper);}
            if(helper->getPosition().squaredDistance(c->getPosition())<=16.0f&&!missionNativeTaskPresent(helper,LIFT_PERSON_PLAYER_ORDER)){missionClearTravel(helper,"native pickup");helper->pickupObject(c);}
            else if(task.retry<=0&&!missionNativeTaskPresent(helper,LIFT_PERSON_PLAYER_ORDER)){
                // PICKUP is the low-level action, not the character carry goal.
                // Use the same goal offered by Kenshi's native lift-person menu;
                // it plans the approach and then executes the pickup action.
                missionClearTravel(helper,"native carry approach");
                missionIssueOrder(helper,LIFT_PERSON_PLAYER_ORDER,c,c->getPosition());
                rescueLog("CARRY LIFT PERSON ORDER",c,helper);task.retry=5;
            }}
        ++t;
    }
    const bool leaderReady=rescueSucceedLeader();
    // A formation straggler also holds travel; resume with hysteresis.
    bool commerceOwnsRegroup=false;
#ifdef MERCENARIE_CARAVAN_DELIVERY
    commerceOwnsRegroup=caravanTradeActive();
    // Couriers deliberately leave formation. Commerce owns their return barrier;
    // road regrouping must not recall them or suspend the mission after 120s.
    if(commerceOwnsRegroup){missionRescue.regrouping=false;missionRescue.regroupSeconds=0;}
#endif
    float farthest=0;Character* straggler=0;
    if(escort)for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(c&&!c->isDead()&&!c->isBeingCarried()&&rescueCanWalk(c)){float distance=c->getPosition().squaredDistance(escort->getPosition());if(distance>farthest){farthest=distance;straggler=c;}}}
    if(!hold&&leaderReady&&!missionPaused&&!missionFollowing&&!missionRescue.navigationRP.awaitingGuide&&!commerceOwnsRegroup){
        float startSquared=10000.0f,releaseSquared=2500.0f;
#ifdef MERCENARIE_CARAVAN_TRADE
        if(caravanMission){startSquared=22500.0f;releaseSquared=6400.0f;}
#endif
        if(farthest>startSquared)missionRescue.regrouping=true;
        else if(farthest<=releaseSquared)missionRescue.regrouping=false;
        const bool clearGateFirst=missionRescue.gateClearing&&missionRescue.gateClearSeconds>0&&farthest<40000.0f;
        if(missionRescue.regrouping&&!clearGateFirst){
            missionWaitReason(MissionMotionPolicy::Regroup);
            missionRescue.regroupSeconds+=elapsed;
            if(missionRescue.regroupSeconds>=120){missionSuspendMotion();return true;}
            missionRescue.regroupLogClock-=elapsed;
            if(missionRescue.regroupLogClock<=0){
                std::ostringstream s;s<<"MISSION REGROUP WAIT id="<<currentMissionFiscalId<<" leader="<<escort->getHandle().toString()<<" member="<<(straggler?straggler->getHandle().toString():"none")<<" distance_squared="<<farthest<<" release_squared="<<releaseSquared<<" destination="<<destination;DebugLog(s.str());
                if(straggler)missionOrderTrace(straggler,"regroup limiting member",escort);
                missionRescue.regroupLogClock=10;
            }
            if(!escort->isInCombatMode(true,true)){missionHoldTravel(escort,"regroup hold");}
            missionGroup.tick(escort,progressMembers,true,elapsed,true,true);hold=true;
        }else {missionRescue.regroupLogClock=0;missionRescue.regroupSeconds=0;}
    }
    hold=hold||!leaderReady;
    if(hold!=missionCasualtyWaiting){rescueLog(hold?"GROUP HOLD":"GROUP RESUME",escort);missionCasualtyWaiting=hold;if(!hold&&!recovering)resumeRecoveredMissionPhase();}
    if(recovering&&!hold){missionRescue.recoveryPending=false;resumeRecoveredMissionPhase();}
    return hold;
}
