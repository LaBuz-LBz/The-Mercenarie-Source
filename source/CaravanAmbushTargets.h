#pragma once
std::vector<hand> caravanAmbushEnemies(const std::string& metadata){
    std::vector<hand> out;size_t p=metadata.find(";HOUSEENEMIES1=");if(p==std::string::npos)return out;
    p+=15;std::istringstream in(metadata.substr(p,metadata.find(';',p)-p));std::string id;
    while(in>>id){if(out.size()>=10||!CaravanCustomers::identity(id))throw std::runtime_error("invalid house attackers");hand h;h.fromString(id);out.push_back(h);}return out;
}
void rememberCaravanAmbushEnemies(const std::vector<Character*>& enemies){
    std::ostringstream out;out<<";HOUSEENEMIES1=";for(size_t i=0;i<enemies.size();++i)out<<enemies[i]->getHandle().toString()<<' ';currentContract.routeRegions+=out.str()+";";
}
bool houseFighter(Character* c){return c&&!c->isDead()&&!c->isBeingCarried()&&!c->isCarryingSomething&&rescueCanWalk(c)&&c->getAI()&&!missionNativeTaskPresent(c,FIRST_AID_ORDER)&&!missionNativeTaskPresent(c,LIFT_PERSON_PLAYER_ORDER);}
void houseAttack(Character* c,Character* target){
    if(!houseFighter(c)||!target||c->isPlayerCharacter())return;
    AITaskSytem* tasks=c->getAI()->getTaskSystem();if(!tasks)return;
    if(tasks->getCurrentGoal().key()==UNPROVOKED_FOCUSED_MELEE_ATTACK&&tasks->getCurrentGoal().subject.getCharacter()==target)return;
    missionClearTravel(c,"house battle focus");
    c->getAI()->setManuveringFreedomLevel(AI::ROAM_FAR);
    c->getAI()->setCenterOfMovementTarget(target->getHandle());
    // Explicit target order takes precedence over autonomous enemy selection.
    tasks->addOrder(UNPROVOKED_FOCUSED_MELEE_ATTACK,target->getHandle(),target->getPosition(),true,false);
}

void houseReleaseAttack(Character* c){
    if(!c||c->isPlayerCharacter()||!c->getAI())return;AITaskSytem* tasks=c->getAI()->getTaskSystem();if(!tasks)return;
    for(size_t i=0;i<tasks->orders.list.size();){Tasker* t=tasks->orders.list[i];
        if(t&&t->key()==UNPROVOKED_FOCUSED_MELEE_ATTACK){if(tasks->getCurrentGoal().key()==t->key())tasks->clearCurrentGoal(true);tasks->orders.list.erase(tasks->orders.list.begin()+i);delete t;}else ++i;
    }
}
bool tickCaravanAmbushBattle(float elapsed=1){
    if(!missionActive||missionPaused)return false;
    CaravanBattleResolution::State resolution=CaravanBattleResolution::read(currentContract.routeRegions);if(resolution.ended)return false;
    std::vector<hand> saved=caravanAmbushEnemies(currentContract.routeRegions);if(saved.empty())return false;
    if(!resolution.anchored&&escort){resolution.anchored=true;Ogre::Vector3 p=escort->getPosition();resolution.x=p.x;resolution.y=p.y;resolution.z=p.z;}
    Ogre::Vector3 refuge((float)resolution.x,(float)resolution.y,(float)resolution.z);
    std::vector<Character*> enemies,allies,targets;bool allDead=true;
    for(size_t i=0;i<saved.size();++i){Character* c=saved[i].getCharacter();if(!c||c->isPlayerCharacter()){allDead=false;continue;}
        // Only this event's spawned enemies: never a fall animation or a bystander.
        if(!c->isDead()&&c->getMedical()&&c->getMedical()->isUnconcious()){c->declareDead();DebugLog(std::string("CARAVAN BANDIT KO death actor=")+c->getHandle().toString());}
        if(!c->isDead())allDead=false;
        if(houseFighter(c)&&tradeDistance(c->getPosition(),refuge)<2250000)enemies.push_back(c);
    }
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(houseFighter(c)&&!c->isAnimal()){allies.push_back(c);if(!caravanHomeAmbushFleeing(c))targets.push_back(c);}}
    for(unsigned int i=0;ou&&ou->player&&i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(houseFighter(c)&&!c->isAnimal()&&tradeDistance(c->getPosition(),refuge)<2250000)targets.push_back(c);}
    resolution.quiet=enemies.empty()?std::min(30.0,resolution.quiet+elapsed):0;
    if(resolution.quiet>=8){
        if(allDead&&houseFighter(escort)){
#ifdef MERCENARIE_CARAVAN_SPEECH
            if(caravanSpeaking(escort))return true;
#endif
            escort->sayALine(Loc::text("caravan.ambush.victory"),true);
        }
        resolution.ended=true;
        for(size_t i=0;i<saved.size();++i)houseReleaseAttack(saved[i].getCharacter());
        for(size_t i=0;i<progressMembers.size();++i)houseReleaseAttack(progressMembers[i].getCharacter());
        CaravanBattleResolution::write(currentContract.routeRegions,resolution);DebugLog("CARAVAN HOME AMBUSH resolved permanently");return false;
    }
    CaravanBattleResolution::write(currentContract.routeRegions,resolution);
    for(size_t i=0;i<enemies.size()&&!targets.empty();++i)houseAttack(enemies[i],targets[i%targets.size()]);
    CaravanHomeAmbush::State escape=CaravanHomeAmbush::read(currentContract.routeRegions);Character* runner=deliveryGuard(escape.runner);
    for(size_t i=0;i<allies.size();++i){Character* c=allies[i];
        if(caravanHomeAmbushFleeing(c))continue;
        if(missionDefending(c))continue;
        if(c==escort){
            if(caravanHomeAmbushFleeing(c))continue;
            houseReleaseAttack(c);
            if(tradeDistance(c->getPosition(),refuge)>400){Ogre::Vector3 point;if(tradeExterior(c,refuge,point))missionIssueOrder(c,MOVE_CUS_ORDERED,0,point);}
            else if(!missionNativeTaskPresent(c,HOLD_POSITION))missionIssueOrder(c,HOLD_POSITION,0,c->getPosition());
            continue;
        }
        Character* target=0;float best=3.4e38f;Ogre::Vector3 protect=runner?runner->getPosition():refuge;
        for(size_t j=0;j<enemies.size();++j){float d=tradeDistance(enemies[j]->getPosition(),protect);if(d<best){best=d;target=enemies[j];}}
        if(target)houseAttack(c,target);
    }
    return true;
}
