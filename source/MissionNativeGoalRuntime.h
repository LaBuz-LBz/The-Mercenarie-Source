#include "CleanupState.h"
#pragma once
// Included after QuestContexts and MissionPersistence. Never switch contexts
// inside the native AI callback: other actors can be evaluated between our ticks.
namespace {
int missionNativeGoalOwner(Character* actor){
    if(missionWorldChanging||missionRestorePending||!actor||actor->isPlayerCharacter())return -1;
    const std::string identity=actor->getHandle().toString();
    for(int slot=0;slot<maximumActiveQuests;++slot){
        const bool selected=slot==selectedEscortQuest;
        if(!(selected?missionActive:escortQuests[slot].v_missionActive))continue;
        const hand& leader=selected?escortHandle:escortQuests[slot].v_escortHandle;
        if(!leader.isNull()&&leader.toString()==identity)return slot;
        const std::vector<hand>& members=selected?progressMembers:escortQuests[slot].v_progressMembers;
        for(size_t i=0;i<members.size();++i)if(members[i].getCharacter()==actor)return slot;
    }
    return -1;
}
bool (*missionRunGoalsOriginal)(AITaskSytem*,MissionNativeGoalScores&,bool);
void missionBlockedGoalTrace(Character* actor,TaskType type,int slot){
    const std::string& id=slot==selectedEscortQuest?currentMissionFiscalId:escortQuests[slot].v_currentMissionFiscalId;
    std::ostringstream key;key<<id<<':'<<actor->getHandle().toString()<<':'<<(int)type;
    static std::map<std::string,unsigned long> last;
    const unsigned long now=GetTickCount();
    std::map<std::string,unsigned long>::iterator previous=last.find(key.str());
    if(previous!=last.end()&&now-previous->second<5000)return;
    if(last.size()>512)last.clear();last[key.str()]=now;
    std::ostringstream line;line<<"MISSION NATIVE GOAL BLOCKED id="<<id<<" actor="<<actor->getHandle().toString()<<" goal="<<(int)type;
    DebugLog(line.str());
}
bool missionRunGoalsHook(AITaskSytem* tasks,MissionNativeGoalScores& goals,bool playerOrder){
    if(MercenarieCleanup::disabled)return missionRunGoalsOriginal(tasks,goals,playerOrder);
    const int slot=playerOrder?-1:missionNativeGoalOwner(tasks?tasks->character:0);
    bool customer=false;
#ifdef MERCENARIE_CARAVAN_CUSTOMERS
    customer=!playerOrder&&caravanCustomerOwned(tasks?tasks->character:0);
#endif
    bool fleeing=false,bandit=false;
#ifdef MERCENARIE_CARAVAN_BATTLE_CONTROL
    if(!playerOrder&&tasks&&tasks->character){
        for(int i=0;i<maximumActiveQuests;++i){const bool selected=i==selectedEscortQuest;
            if(!(selected?missionActive:escortQuests[i].v_missionActive))continue;
            const std::string& metadata=selected?currentContract.routeRegions:escortQuests[i].v_currentContract.routeRegions;
            if(CaravanBattleResolution::read(metadata).ended)continue;
            if(CaravanHomeAmbush::fleeing(metadata,tasks->character->getHandle().toString()))fleeing=true;
            std::vector<hand> enemies=caravanAmbushEnemies(metadata);
            for(size_t j=0;j<enemies.size();++j)if(enemies[j].getCharacter()==tasks->character)bandit=true;
        }
    }
#endif
    if(slot>=0||customer||fleeing||bandit){
        for(MissionNativeGoalScores::iterator i=goals.begin();i!=goals.end();){
            const TaskType type=i->second?TaskMatch(i->second).key():NULL_TASK;
            if(missionConflictingNativeGoal(type)||((fleeing||bandit)&&(type==MELEE_ATTACK||type==FOCUSED_MELEE_ATTACK||type==UNPROVOKED_FOCUSED_MELEE_ATTACK||type==CHOOSE_ENEMY_AND_ATTACK||type==CHOOSE_ATTACKER_OF_ALLY||type==ATTACK_CHARACTERS_ATTACKER||type==ATTACK_ATTACKERS_OF||type==ATTACK_ENEMIES||type==ATTACK_ENEMIES_AND_NEUTRALS||type==RANGED_ATTACK||type==RANGED_ATTACK_FOCUSED))){
                if(slot>=0)missionBlockedGoalTrace(tasks->character,type,slot);
                MissionNativeGoalScores::iterator removed=i++;goals.erase(removed);
            }else ++i;
        }
    }
    return missionRunGoalsOriginal(tasks,goals,playerOrder);
}
}
