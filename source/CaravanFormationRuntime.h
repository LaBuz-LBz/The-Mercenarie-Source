#pragma once
#include "CaravanFormationGeometry.h"
bool caravanNativeFormationReady=false;
struct CaravanFormationController {
    struct Slot {hand actor;Ogre::Vector3 offset;bool enabled;Slot():offset(0,0,0),enabled(false){}};
    std::vector<Slot> slots;bool engaged;hand leaderHandle;
    CaravanFormationController():engaged(false){}
    void reset(){slots.clear();engaged=false;leaderHandle.setNull();}
    bool available(Character* c){
        if(!c||c->isPlayerCharacter()||c->isDead()||c->isBeingCarried()||c->isCarryingSomething||!c->getMedical()||c->getMedical()->isUnconcious()||c->getMedical()->isCrippled()||!c->getMovement()||!c->getAI())return false;
        if(missionCombatThreat(c)||missionNativeTaskPresent(c,FIRST_AID_ORDER)||missionNativeTaskPresent(c,LIFT_PERSON_PLAYER_ORDER)||missionNativeTaskPresent(c,PICKUP))return false;
        for(size_t i=0;i<missionRescue.tasks.size();++i)if(missionRescue.tasks[i].helper.getCharacter()==c)return false;
        return true;
    }
    bool tick(Character* leader,const std::vector<hand>& roster,bool active,float dt,bool regroup){
        extern bool caravanMission;(void)dt;
        bool use=caravanNativeFormationReady&&caravanMission&&active&&!regroup&&available(leader)&&!leader->getMovement()->isIndoors()
            &&!missionRescue.gateClearing&&!missionRescue.passage.active&&!missionRescue.localRecovery;
        missionRescue.formationSpeedScale=1;
        if(!use){engaged=false;return false;}
        if(!engaged)DebugLog("CARAVAN NATIVE FOLLOW active: persistent follow with native offsets");
        engaged=true;leaderHandle=leader->getHandle();slots.resize(roster.size());unsigned guards=0,animals=0,count=0;
        for(size_t i=0;i<roster.size();++i){Character* c=roster[i].getCharacter();if(c&&c!=leader&&!c->isDead()&&!c->isPlayerCharacter()&&!c->isAnimal())++count;}
        for(size_t i=0;i<roster.size();++i){Slot& state=slots[i];state.enabled=false;Character* c=roster[i].getCharacter();
            if(!c||c==leader||c->isDead()||c->isPlayerCharacter())continue;
            CaravanFormation::Offset position=CaravanFormation::slot(c->isAnimal(),c->isAnimal()?animals++:guards++,count);
            state.actor=c->getHandle();state.offset=Ogre::Vector3(position.forward,0,-position.side);
            if(!available(c)){missionClearTravel(c,"native formation yields to care or combat");continue;}
            state.enabled=!c->getMovement()->isIndoors()&&c->getPosition().squaredDistance(leader->getPosition())<=6400;
            if(!missionFollowerTargetValid(c,leader,false)){
                missionClearTravel(c,"native caravan follow");missionIssueOrder(c,FOLLOW_PLAYER_ORDER,leader,leader->getPosition());
            }
        }
        return true;
    }
    const Ogre::Vector3* offsetFor(Character* c){
        if(!engaged||!c)return 0;
        Character* leader=leaderHandle.getCharacter();if(!leader||leader->isDead()||c->isDead()||c->isBeingCarried()||c->isPlayerCharacter()||!c->getMovement()||c->getMovement()->isIndoors()||c->isInCombatMode(true,true)||!missionFollowerTargetValid(c,leader,false))return 0;
        for(size_t i=0;i<slots.size();++i)if(slots[i].enabled&&slots[i].actor.getCharacter()==c)return &slots[i].offset;
        return 0;
    }
};
