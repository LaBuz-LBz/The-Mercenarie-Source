#pragma once
struct MissionAttackPair {Character* victim;Character* enemy;};
bool missionDefenceMember(Character* c){
    if(c==escort)return true;
    for(size_t i=0;i<progressMembers.size();++i)if(progressMembers[i].getCharacter()==c)return true;
    return false;
}
bool missionDefenceAvailable(Character* c){
    if(!c||c->isDead()||c->isBeingCarried()||c->isCarryingSomething||!rescueCanWalk(c)||!c->getAI()||!c->getAI()->getTaskSystem())return false;
    if(missionNativeTaskPresent(c,FIRST_AID_ORDER)||missionNativeTaskPresent(c,LIFT_PERSON_PLAYER_ORDER)||missionNativeTaskPresent(c,PICKUP))return false;
    for(size_t i=0;i<missionRescue.tasks.size();++i)if(missionRescue.tasks[i].helper.getCharacter()==c)return false;
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
    if(caravanHomeAmbushFleeing(c))return false;
#endif
    return true;
}
// Persist exact ownership in contract metadata; player orders can survive a
// native save/load, so their later radius/death cleanup must survive it too.
void missionRestoreDefenceOwnership(){
    size_t p=currentContract.routeRegions.find(";DEFENCE1=");if(p==std::string::npos)return;
    p+=10;std::istringstream in(currentContract.routeRegions.substr(p,currentContract.routeRegions.find(';',p)-p));
    std::string actor,target;float x,y,z;unsigned count=0;
    while(in>>actor>>target>>x>>y>>z){
        if(++count>128)break;
        MissionRescueState::OwnedOrder o;o.actor.fromString(actor);o.subject.fromString(target);o.type=FOCUSED_MELEE_ATTACK;o.position=Ogre::Vector3(x,y,z);
        bool exists=false;
        for(size_t i=0;i<missionRescue.ownedOrders.size();++i)if(missionRescue.ownedOrders[i].type==o.type&&missionRescue.ownedOrders[i].actor.toString()==actor)exists=true;
        if(!exists)missionRescue.ownedOrders.push_back(o);
    }
}
void missionSaveDefenceOwnership(){
    size_t p=currentContract.routeRegions.find(";DEFENCE1=");
    if(p!=std::string::npos){size_t end=currentContract.routeRegions.find(';',p+10);currentContract.routeRegions.erase(p,end==std::string::npos?std::string::npos:end-p+(end+1==currentContract.routeRegions.size()||currentContract.routeRegions[end+1]==';'?1:0));}
    std::ostringstream out;out<<std::setprecision(9)<<";DEFENCE1=";unsigned count=0;
    for(size_t i=0;i<missionRescue.ownedOrders.size();++i){const MissionRescueState::OwnedOrder& o=missionRescue.ownedOrders[i];if(o.type!=FOCUSED_MELEE_ATTACK)continue;
        out<<o.actor.toString()<<' '<<o.subject.toString()<<' '<<o.position.x<<' '<<o.position.y<<' '<<o.position.z<<' ';++count;
    }
    if(count)currentContract.routeRegions+=out.str()+";";
}
bool tickMissionGroupDefence(){
    missionRestoreDefenceOwnership();
    if(!missionActive||missionPending||missionPaused){clearMissionDefence();missionSaveDefenceOwnership();return false;}
    std::vector<MissionAttackPair> threats;std::vector<Character*> members;
    if(escort)members.push_back(escort);
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(c&&std::find(members.begin(),members.end(),c)==members.end())members.push_back(c);}
    for(size_t i=0;i<members.size();++i){Character* victim=members[i];
        if(victim->isDead()||victim->isBeingCarried())continue;
        lektor<hand> attackers;victim->getAllAttackers(attackers);
        for(unsigned int j=0;j<attackers.size();++j){Character* enemy=attackers[j].getCharacter();
            // Real incoming attacks only: no faction-wide hostility or bystanders.
            if(!enemy||enemy==victim||enemy->isPlayerCharacter()||missionDefenceMember(enemy)||enemy->isDead()||enemy->isDown()||enemy->isBeingCarried())continue;
            if(enemy->getPosition().squaredDistance(victim->getPosition())>62500.0f)continue;
            MissionAttackPair pair;pair.victim=victim;pair.enemy=enemy;threats.push_back(pair);
        }
    }
    std::vector<Character*> helpers=members;
    if(ou&&ou->player)for(unsigned int i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];
        if(c&&std::find(helpers.begin(),helpers.end(),c)==helpers.end())helpers.push_back(c);
    }
    // Include previous helpers so distant players have our order released promptly.
    for(size_t i=0;i<missionRescue.ownedOrders.size();++i)if(missionRescue.ownedOrders[i].type==FOCUSED_MELEE_ATTACK){Character* c=missionRescue.ownedOrders[i].actor.getCharacter();
        if(c&&std::find(helpers.begin(),helpers.end(),c)==helpers.end())helpers.push_back(c);
    }
    for(size_t i=0;i<helpers.size();++i){Character* c=helpers[i];Character* target=0;float best=3.4e38f;
        if(missionDefenceAvailable(c))for(size_t j=0;j<threats.size();++j){
            if(c->isPlayerCharacter()&&c->getPosition().squaredDistance(threats[j].victim->getPosition())>2500.0f)continue;
            float d=c->getPosition().squaredDistance(threats[j].enemy->getPosition());
            if(d<best){best=d;target=threats[j].enemy;}
        }
        bool owned=false;
        for(size_t j=0;j<missionRescue.ownedOrders.size();++j)if(missionRescue.ownedOrders[j].type==FOCUSED_MELEE_ATTACK&&missionRescue.ownedOrders[j].actor.getCharacter()==c)owned=true;
        if(!target){if(owned)missionClearTravel(c,"local defence released",FOCUSED_MELEE_ATTACK);continue;}
        bool same=false;
        AITaskSytem* tasks=c->getAI()->getTaskSystem();
        for(size_t j=0;j<tasks->orders.list.size();++j){Tasker* task=tasks->orders.list[j];if(task&&task->key()==FOCUSED_MELEE_ATTACK&&task->subject.getCharacter()==target){same=true;break;}}
        if(same)continue;
        // Do not redirect somebody already fighting under native/player control.
        if(c->isInCombatMode(true,true)&&!missionDefending(c))continue;
        missionClearTravel(c,"help attacked mission member");
        missionIssueOrder(c,FOCUSED_MELEE_ATTACK,target,target->getPosition());
        // Put assistance ahead of existing player movement, preserving that
        // queue for later rather than destroying their orders or permanent jobs.
        for(size_t j=0;j<tasks->orders.list.size();++j){Tasker* task=tasks->orders.list[j];
            if(task&&task->key()==FOCUSED_MELEE_ATTACK&&missionOwnedTask(c,task)){
                tasks->orders.list.erase(tasks->orders.list.begin()+j);tasks->orders.list.insert(tasks->orders.list.begin(),task);break;
            }
        }
        if(missionLocomotionType(tasks->getCurrentGoal().key())||tasks->getCurrentGoal().key()==FOLLOW_SQUADLEADER||tasks->getCurrentGoal().key()==BODYGUARD)tasks->clearCurrentGoal(true);
        DebugLog(std::string("MISSION GROUP DEFENCE actor=")+c->getHandle().toString()+" target="+target->getHandle().toString()+(c->isPlayerCharacter()?" player_radius=50":" mission_member"));
    }
    missionSaveDefenceOwnership();
    return !threats.empty();
}
