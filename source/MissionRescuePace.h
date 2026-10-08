#pragma once
#include "MissionPaceRules.h"
bool rescueCanWalk(Character* c);
bool missionNativeSurvival(Character* c);
    void applyMissionPace()
    {
        if(!missionActive)return;
        const MoveSpeed speed=
#ifdef MERCENARIE_CARAVAN_TRADE
            caravanTradeActive()?WALK:
#endif
            missionPace==EscortPace::Accelerated?RUN:WALK;
        float limit=1e30f;Character* slowest=0;
        for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(!c||c->isDead()||c->isBeingCarried()||!c->getStats())continue;float available=MissionPaceRules::gaitLimit(c->getStats()->getMaxRunSpeed(),c->getMovement()->getStandardWalkSpeed(),speed==RUN);if(available<limit){limit=available;slowest=c;}}
        if(!slowest)return;
        float gapSquared=0;
        if(escort)for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(c&&c!=escort&&!c->isDead()&&!c->isBeingCarried()&&rescueCanWalk(c))gapSquared=std::max(gapSquared,c->getPosition().squaredDistance(escort->getPosition()));}
        float leaderLimit=MissionPaceRules::leaderLimit(limit,gapSquared,speed==RUN);
#ifdef MERCENARIE_CARAVAN_FORMATION
        if(caravanMission&&!caravanTradeActive())leaderLimit*=missionRescue.formationSpeedScale;
#endif
        const float previousLimit=missionRescue.lastSpeed;missionRescue.lastSpeed=limit; // Publish the new limit before native speed hooks run.
        for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
            if(caravanHomeAmbushFleeing(c))continue;
#endif
            if(c&&!c->isDead()&&!c->isBeingCarried()&&!missionNativeSurvival(c)&&c->getMovement()){c->getMovement()->leaveSpeedGroup();c->getMovement()->setDesiredSpeedOrders(speed);c->getMovement()->setDesiredSpeed(speed);c->getMovement()->setDesiredSpeed(c==escort?leaderLimit:limit);}}
        if(fabs(previousLimit-limit)>0.25f){std::ostringstream s;s<<"MISSION SPEED LIMIT id="<<currentMissionFiscalId<<" member="<<slowest->getHandle().toString()<<" limit="<<limit;DebugLog(s.str());missionRescue.lastSpeed=limit;}

    }

