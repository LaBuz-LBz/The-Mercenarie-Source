#pragma once
// FOCUSED orders recorded here belong only to collective mission defence.
bool missionDefending(Character* actor){
    if(!actor)return false;
    for(size_t i=0;i<missionRescue.ownedOrders.size();++i){const MissionRescueState::OwnedOrder& o=missionRescue.ownedOrders[i];
        if(o.type!=FOCUSED_MELEE_ATTACK||o.actor.toString()!=actor->getHandle().toString())continue;
        Character* enemy=o.subject.getCharacter();
        if(enemy&&!enemy->isDead()&&!enemy->isDown())return true;
    }return false;
}
void clearMissionDefence(){
    std::vector<hand> actors;
    for(size_t i=0;i<missionRescue.ownedOrders.size();++i)if(missionRescue.ownedOrders[i].type==FOCUSED_MELEE_ATTACK)actors.push_back(missionRescue.ownedOrders[i].actor);
    for(size_t i=0;i<actors.size();++i)missionClearTravel(actors[i].getCharacter(),"mission defence ends",FOCUSED_MELEE_ATTACK);
}
// Native incoming melee threats exist before the victim switches combat mode.
// Leave enemy selection, blocking and attacks to Kenshi; release only mission travel.
bool missionCombatThreat(Character* actor){
    if(!actor||actor->isDead()||actor->isBeingCarried()||!actor->getMedical()||actor->getMedical()->isUnconcious())return false;
    if(actor->isInCombatMode(true,true)||missionDefending(actor))return true;
    AI* ai=actor->getAI();return ai&&actor->getBody()&&ai->iAmBeingMeleeAttacked_general(actor->getHandle(),actor->getPosition());
}
void missionYieldTravelToCombat(Character* actor){
    if(!missionCombatThreat(actor))return;
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
    if(caravanHomeAmbushFleeing(actor))return;
#endif
    for(size_t i=0;i<missionRescue.ownedOrders.size();++i){
        if(missionLocomotionType(missionRescue.ownedOrders[i].type)&&missionRescue.ownedOrders[i].actor.toString()==actor->getHandle().toString()){
            missionClearTravel(actor,"incoming attack interrupts mission travel");return;
        }
    }
}
