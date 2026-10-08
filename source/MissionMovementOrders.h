#pragma once
// Clear only explicit mission locomotion orders; native combat goals remain owned by Kenshi.
void missionOrderTrace(Character* actor,const char* reason,Character* target=0){
    if(!actor)return;
    std::ostringstream s;s<<"MISSION ORDER id="<<currentMissionFiscalId<<" reason="<<reason
        <<" actor="<<actor->getHandle().toString()<<" leader="<<(escort?escort->getHandle().toString():"none")
        <<" target="<<(target?target->getHandle().toString():"none")<<" pos="<<actor->getPosition()<<" destination="<<destination;
    if(actor->getAI()&&actor->getAI()->getTaskSystem()){AITaskSytem* tasks=actor->getAI()->getTaskSystem();s<<" native_goal="<<(int)tasks->getCurrentGoal().key()<<" native_target="<<tasks->getCurrentGoal().subject.toString()<<" move="<<tasks->hasPlayerOrder(MOVE_CUS_ORDERED)<<" follow="<<tasks->hasPlayerOrder(FOLLOW_PLAYER_ORDER)<<" squad_follow="<<tasks->hasPlayerOrder(FOLLOW_SQUADLEADER)<<" hold="<<tasks->hasPlayerOrder(HOLD_POSITION)<<" aid="<<tasks->hasPlayerOrder(FIRST_AID_ORDER)<<" lift="<<tasks->hasPlayerOrder(LIFT_PERSON_PLAYER_ORDER)<<" obsolete_pickup="<<tasks->hasPlayerOrder(PICKUP);}
    if(actor->getMovement()){s<<" movement_destination="<<actor->getMovement()->getDestination()<<" path_destination="<<actor->getMovement()->pathDestination<<" road_weight="<<actor->getMovement()->roadWeight<<" road_follower="<<(actor->getMovement()->roadFollower!=0)<<" moving="<<actor->getMovement()->isCurrentlyMoving()<<" path_failed="<<actor->getMovement()->pathFailed();}
    DebugLog(s.str());
}
bool missionLocomotionType(int type){return type==MOVE_CUS_ORDERED||type==FOLLOW_PLAYER_ORDER||type==HOLD_POSITION||type==UNLOCK_DOOR_HERE;}
bool missionOwnedTask(Character* actor,Tasker* task){
    if(!actor||!task)return false;
    for(size_t i=0;i<missionRescue.ownedOrders.size();++i){const MissionRescueState::OwnedOrder& o=missionRescue.ownedOrders[i];
        if(o.actor.toString()==actor->getHandle().toString()&&o.type==task->key()&&o.subject.toString()==task->subject.toString()&&o.position.squaredDistance(task->location)<1.0f)return true;
    }return false;
}
void missionClearTravel(Character* actor,const char* reason,int ownedType=-1){
    if(!actor||!actor->getAI()||!actor->getAI()->getTaskSystem())return;
    missionOrderTrace(actor,reason);AITaskSytem* tasks=actor->getAI()->getTaskSystem();bool removed=false;
    for(size_t i=0;i<tasks->orders.list.size();){Tasker* task=tasks->orders.list[i];
        if(!task){++i;continue;}
        if((ownedType<0?!missionLocomotionType(task->key()):task->key()!=ownedType)||!missionOwnedTask(actor,task)){++i;continue;}
        // Clear only the executing owned goal; leave medical/interaction goals intact.
        if(tasks->getCurrentGoal().key()==task->key()&&tasks->getCurrentGoal().subject.toString()==task->subject.toString())tasks->clearCurrentGoal(true);
        tasks->orders.list.erase(tasks->orders.list.begin()+i);delete task;removed=true;
    }
    for(size_t i=0;i<missionRescue.ownedOrders.size();){const MissionRescueState::OwnedOrder& o=missionRescue.ownedOrders[i];
        if(o.actor.toString()==actor->getHandle().toString()&&(ownedType<0?missionLocomotionType(o.type):o.type==ownedType))missionRescue.ownedOrders.erase(missionRescue.ownedOrders.begin()+i);else ++i;
    }
    if(removed&&actor->getMovement()&&!actor->isInCombatMode(true,true)&&!tasks->hasPlayerOrders()){
        actor->getMovement()->halt();actor->getMovement()->setRoadPreference(0.0f);actor->getMovement()->invalidatePath();
    }
}
// addJob installs native jobs/goals, not necessarily player orders. The log
// showed goal 44 (FOLLOW_PLAYER_ORDER) with hasPlayerOrder == false.
bool missionNativeTaskPresent(Character* actor,TaskType type){
    if(!actor||!actor->getAI()||!actor->getAI()->getTaskSystem())return false;
    AITaskSytem* tasks=actor->getAI()->getTaskSystem();
    return tasks->hasPlayerOrder(type)||tasks->hasGoal(type)||tasks->hasPermajob(type)||tasks->getCurrentGoal().key()==type;
}
bool missionFollowOrderPresent(Character* actor){return missionNativeTaskPresent(actor,FOLLOW_PLAYER_ORDER);}
// Explicit orders take precedence over the roaming squad's shopping/town goals.
// Character::addJob did not populate this queue in the captured native session.
void missionIssueOrder(Character* actor,TaskType type,Character* target,const Ogre::Vector3& position){
    if(!missionActive||!actor||!actor->getAI()||!actor->getAI()->getTaskSystem())return;
    if(type!=HOLD_POSITION){
        for(size_t i=0;i<missionRescue.holds.size();++i)if(missionRescue.holds[i].actor.toString()==actor->getHandle().toString()){missionRescue.holds.erase(missionRescue.holds.begin()+i);break;}
        actor->getAI()->setManuveringFreedomLevel(AI::ROAM_FAR);
        hand center;if(target)center=target->getHandle();else center.setNull();
        actor->getAI()->setCenterOfMovementTarget(center);
        actor->getAI()->setCenterOfMovement(actor->getPosition());
    }
    hand subject;if(target)subject=target->getHandle();else subject.setNull();
    // Replace only the task recorded for this actor/type; never clear the whole queue.
    for(size_t i=0;i<missionRescue.ownedOrders.size();++i){MissionRescueState::OwnedOrder& old=missionRescue.ownedOrders[i];
        if(old.actor.toString()==actor->getHandle().toString()&&old.type==type){
            AITaskSytem* tasks=actor->getAI()->getTaskSystem();
            for(size_t j=0;j<tasks->orders.list.size();){Tasker* task=tasks->orders.list[j];if(task&&task->key()==type&&missionOwnedTask(actor,task)){if(tasks->getCurrentGoal().key()==type&&tasks->getCurrentGoal().subject.toString()==task->subject.toString())tasks->clearCurrentGoal(true);tasks->orders.list.erase(tasks->orders.list.begin()+j);delete task;}else ++j;}
            missionRescue.ownedOrders.erase(missionRescue.ownedOrders.begin()+i);break;
        }
    }
    actor->getAI()->getTaskSystem()->addOrder(type,subject,position,false,false);
    MissionRescueState::OwnedOrder owned;owned.actor=actor->getHandle();owned.type=type;owned.subject=subject;owned.position=position;missionRescue.ownedOrders.push_back(owned);
    missionOrderTrace(actor,"explicit order installed",target);
}
// A normal road handoff updates the existing task without replacing its goal.
// Keep the native movement destination in
// sync immediately, rather than waiting for a later AI goal-selection tick.
bool missionContinueTravel(Character* actor,const Ogre::Vector3& target){
    if(!missionActive||!actor||actor->isInCombatMode(true,true)||!actor->getAI()||!actor->getMovement())return false;
    AITaskSytem* tasks=actor->getAI()->getTaskSystem();
    if(!tasks||!tasks->hasPlayerOrder(MOVE_CUS_ORDERED)||tasks->getCurrentGoal().key()!=MOVE_CUS_ORDERED||actor->getMovement()->pathFailed()||!actor->getMovement()->isCurrentlyMoving())return false;
    // addOrder(clearOld=true) destroys the current goal even without halt.
    // Preserve the existing order and all action copies of its old location.
    if(tasks->orders.list.size()!=1)return false;
    Tasker* order=tasks->orders.list.front();
    if(!order||!missionOwnedTask(actor,order)||order->key()!=MOVE_CUS_ORDERED||!order->subject.isNull())return false;
    const Ogre::Vector3 previous=order->location;
    // CharBody owns the executing action separately from the AI queues.
    // Updating only those queues lets runAction restore the old destination
    // and finish each short segment (native bodyTaskComplete -> endAction).
    Tasker* bodyAction=actor->getBody()?actor->getBody()->currentAction:0;
    if(bodyAction&&bodyAction->key()==MOVE_CUS_ORDERED&&bodyAction->subject.isNull()&&bodyAction->location.squaredDistance(previous)<1.0f){
        bodyAction->location=target;
        missionOrderTrace(actor,"executing body move target updated");
    }
    for(size_t i=0;i<tasks->actions.list.size();++i){Tasker* action=tasks->actions.list[i];if(action&&action->subject.isNull()&&action->location.squaredDistance(previous)<1.0f)action->location=target;}
    for(size_t i=0;i<tasks->actionsTryList.list.size();++i){Tasker* action=tasks->actionsTryList.list[i];if(action&&action->subject.isNull()&&action->location.squaredDistance(previous)<1.0f)action->location=target;}
    Tasker* sub=tasks->currentlySubTasking;
    if(sub&&sub->subject.isNull()&&sub->location.squaredDistance(previous)<1.0f)sub->location=target;
    for(size_t i=0;i<missionRescue.ownedOrders.size();++i){MissionRescueState::OwnedOrder& o=missionRescue.ownedOrders[i];if(o.actor.toString()==actor->getHandle().toString()&&o.type==MOVE_CUS_ORDERED&&o.position.squaredDistance(previous)<1)o.position=target;}
    order->location=target;
    actor->getMovement()->setDestination(target,HIGH_PRIORITY,false);
    missionOrderTrace(actor,"road target updated in existing order");
    return true;
}
void missionHoldTravel(Character* actor,const char* reason){
    if(!actor||actor->isInCombatMode(true,true))return;
    if(!actor->getAI()||!actor->getMovement())return;
    size_t index=0;for(;index<missionRescue.holds.size();++index)if(missionRescue.holds[index].actor.toString()==actor->getHandle().toString())break;
    if(index==missionRescue.holds.size()){
        MissionHoldAnchor anchor;anchor.actor=actor->getHandle();anchor.position=actor->getPosition();missionRescue.holds.push_back(anchor);
        missionClearTravel(actor,reason);
        missionIssueOrder(actor,HOLD_POSITION,0,anchor.position);
    }
    // Waiting must not send an actor back to an obsolete position after combat
    // or a native displacement. Re-anchor and hold where the actor is now.
    if(actor->getPosition().squaredDistance(missionRescue.holds[index].position)>4.0f){
        missionRescue.holds[index].position=actor->getPosition();
        missionClearTravel(actor,"hold displaced: stop here, no return route");
        missionIssueOrder(actor,HOLD_POSITION,0,actor->getPosition());
    }
    const Ogre::Vector3 anchor=missionRescue.holds[index].position;
    hand none;none.setNull();actor->getAI()->setCenterOfMovementTarget(none);
    actor->getAI()->setCenterOfMovement(anchor);actor->getAI()->setManuveringFreedomLevel(AI::HOLD_GROUND);
    if(!missionNativeTaskPresent(actor,HOLD_POSITION)){
        missionClearTravel(actor,"maintain hold");
        missionIssueOrder(actor,HOLD_POSITION,0,anchor);
    }
    actor->getMovement()->halt();
}

void missionAdoptRestoredTravel(Character* actor){
    if(!actor||actor->isPlayerCharacter()||!actor->getAI()||!actor->getMovement())return;
    AITaskSytem* tasks=actor->getAI()->getTaskSystem();if(!tasks)return;
    for(size_t i=0;i<tasks->orders.list.size();++i){Tasker* t=tasks->orders.list[i];if(!t||missionOwnedTask(actor,t))continue;
        bool match=t->key()==MOVE_CUS_ORDERED&&t->subject.isNull()&&t->location.squaredDistance(actor->getMovement()->pathDestination)<1;
        if(t->key()==FOLLOW_PLAYER_ORDER)match=t->subject.toString()==escortHandle.toString()||t->subject.toString()==missionFollowTarget.toString();
        if(t->key()==HOLD_POSITION)match=t->location.squaredDistance(actor->getPosition())<4;
        if(!match)continue;
        MissionRescueState::OwnedOrder o;o.actor=actor->getHandle();o.type=t->key();o.subject=t->subject;o.position=t->location;missionRescue.ownedOrders.push_back(o);
    }
}

// Only repair genuinely idle native travel. Never replace a foreign goal or
// queued care, FOLLOW, HOLD, interaction or combat action.
bool missionRoadTravelMissing(Character* actor){
    if(!actor||!actor->getAI()||!actor->getMovement())return false;
    AITaskSytem* tasks=actor->getAI()->getTaskSystem();
    if(!tasks||!tasks->orders.list.empty()||tasks->getCurrentGoal().key()!=NULL_TASK)return false;
    Tasker* body=actor->getBody()?actor->getBody()->currentAction:0;
    if(body&&body->key()!=MOVE_CUS_ORDERED)return false;
    for(size_t i=0;i<tasks->actions.list.size();++i)if(tasks->actions.list[i]&&tasks->actions.list[i]->key()!=MOVE_CUS_ORDERED)return false;
    for(size_t i=0;i<tasks->actionsTryList.list.size();++i)if(tasks->actionsTryList.list[i]&&tasks->actionsTryList.list[i]->key()!=MOVE_CUS_ORDERED)return false;
    if(tasks->currentlySubTasking&&tasks->currentlySubTasking->key()!=MOVE_CUS_ORDERED)return false;
    return true;
}
