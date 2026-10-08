#pragma once
void engageCaravanHomeAmbush(const std::vector<Character*>& enemies){
    if(enemies.empty())return;
    // The trade HOLD/FOLLOW orders otherwise leave unthreatened guards watching
    // the one actor selected by a bandit. Alert the whole NPC roster at reveal.
    std::vector<Character*> defenders; if(escort)defenders.push_back(escort);
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();
        if(c&&std::find(defenders.begin(),defenders.end(),c)==defenders.end())defenders.push_back(c);
    }
    size_t engaged=0;
    for(size_t i=0;i<defenders.size();++i){Character* c=defenders[i];
        if(!c||c->isPlayerCharacter()||c->isDead()||c->isBeingCarried()||c->isCarryingSomething||!rescueCanWalk(c)||!c->getAI())continue;
        if(missionNativeTaskPresent(c,FIRST_AID_ORDER)||missionNativeTaskPresent(c,LIFT_PERSON_PLAYER_ORDER)||missionNativeTaskPresent(c,PICKUP))continue;
        missionClearTravel(c,"house ambush collective defence");
        c->getAI()->setManuveringFreedomLevel(AI::ROAM_FAR);
        c->attackTarget(enemies[engaged%enemies.size()]);++engaged;
    }
    std::ostringstream log;log<<"CARAVAN HOME AMBUSH defenders engaged="<<engaged;DebugLog(log.str());
}
