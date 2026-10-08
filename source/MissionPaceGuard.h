#include "CleanupState.h"
#pragma once
#include "MissionPaceRules.h"
// Native carry/follow tasks can request RUN after the once-per-frame mission
// update. Constrain those requests at the speed setter, using the owning slot.
namespace {
bool missionTravelSpeed(AbstractMovementBase* movement,MoveSpeed& mode,float& limit){
    if(missionWorldChanging||missionRestorePending||!movement)return false;
    for(int slot=0;slot<maximumActiveQuests;++slot){
        const bool selected=slot==selectedEscortQuest;
        if(!(selected?missionActive:escortQuests[slot].v_missionActive))continue;
        const std::vector<hand>& members=selected?progressMembers:escortQuests[slot].v_progressMembers;
        for(size_t i=0;i<members.size();++i){
            Character* c=members[i].getCharacter();
            if(!c||c->getMovement()!=movement)continue;

            if(!c->getStats()||c->isPlayerCharacter()||c->isDead()||c->isBeingCarried())return false;
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
            if(CaravanHomeAmbush::fleeing(selected?currentContract.routeRegions:escortQuests[slot].v_currentContract.routeRegions,c->getHandle().toString())){
                mode=RUN;limit=c->getStats()->getMaxRunSpeed();return true;
            }
#endif
            if(missionNativeSurvival(c))return false;
            mode=(selected?missionPace:escortQuests[slot].v_missionPace)==EscortPace::Accelerated?RUN:WALK;
#ifdef MERCENARIE_CARAVAN_TRADE
            const bool trading=selected?caravanTradeActive():(escortQuests[slot].v_caravanMission&&!escortQuests[slot].v_caravanReturning&&CaravanTrade::active(CaravanTrade::read(escortQuests[slot].v_currentContract.routeRegions)));
            if(trading)mode=WALK;
#endif
            limit=MissionPaceRules::gaitLimit(c->getStats()->getMaxRunSpeed(),c->getMovement()->getStandardWalkSpeed(),mode==RUN);
            const MissionRescueState& rescue=selected?missionRescue:escortQuests[slot].v_missionRescue;
            if(rescue.lastSpeed>=0)limit=std::min(limit,rescue.lastSpeed);
            const hand& leader=selected?escortHandle:escortQuests[slot].v_escortHandle;
            if(leader.getCharacter()==c){
                float gapSquared=0;
                for(size_t j=0;j<members.size();++j){Character* member=members[j].getCharacter();if(member&&member!=c&&!member->isDead()&&!member->isBeingCarried()&&rescueCanWalk(member))gapSquared=std::max(gapSquared,member->getPosition().squaredDistance(c->getPosition()));}
                limit=MissionPaceRules::leaderLimit(limit,gapSquared,mode==RUN);
#ifdef MERCENARIE_CARAVAN_FORMATION
                if(!trading&&(selected?caravanMission:escortQuests[slot].v_caravanMission))limit*=rescue.formationSpeedScale;
#endif
            }
            return true;
        }
    }
#ifdef MERCENARIE_CARAVAN_CUSTOMERS
    if(caravanCustomerSpeed(movement,mode,limit))return true;
#endif
    return false;
}
void (*missionSpeedEnumOriginal)(AbstractMovementBase*,MoveSpeed);
void (*missionSpeedFloatOriginal)(AbstractMovementBase*,float);
void missionSpeedEnumHook(AbstractMovementBase* movement,MoveSpeed requested){
    if(MercenarieCleanup::disabled){missionSpeedEnumOriginal(movement,requested);return;}
    MoveSpeed mode;float limit;
    if(missionTravelSpeed(movement,mode,limit)){
        if(requested!=mode){
            static std::map<AbstractMovementBase*,unsigned long> last;
            const unsigned long now=GetTickCount();
            if(!last.count(movement)||now-last[movement]>=5000){
                if(last.size()>256)last.clear();last[movement]=now;
                std::ostringstream out;out<<"MISSION PACE OVERRIDE movement="<<movement<<" requested="<<(int)requested<<" enforced="<<(int)mode<<" limit="<<limit;DebugLog(out.str());
            }
        }
        movement->setDesiredSpeedOrders(mode);
        missionSpeedEnumOriginal(movement,mode);
        missionSpeedFloatOriginal(movement,limit);
    }else missionSpeedEnumOriginal(movement,requested);
}
void missionSpeedFloatHook(AbstractMovementBase* movement,float requested){
    if(MercenarieCleanup::disabled){missionSpeedFloatOriginal(movement,requested);return;}
    MoveSpeed mode;float limit;
    if(missionTravelSpeed(movement,mode,limit))requested=std::min(requested,limit);
    missionSpeedFloatOriginal(movement,requested);
}
}
