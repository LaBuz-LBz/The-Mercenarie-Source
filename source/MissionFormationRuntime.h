#pragma once
#include "MissionMotionPolicy.h"
#ifdef MERCENARIE_CARAVAN_FORMATION
#include "CaravanFormationRuntime.h"
#endif
// All followers target the current logical leader explicitly, including after succession.
struct MissionGroupController {
#ifdef MERCENARIE_CARAVAN_FORMATION
    CaravanFormationController caravan;
#endif
    std::vector<MissionMotionPolicy::Watch> watches;
    std::vector<Character*> assigned;
    std::vector<float> refresh;
    std::vector<bool> approaching;
    std::vector<Ogre::Vector3> approachTargets;
    Character* assignedLeader;
    MissionGroupController():assignedLeader(0){}
    void reset(){
#ifdef MERCENARIE_CARAVAN_FORMATION
caravan.reset();
#endif
watches.clear();assigned.clear();refresh.clear();approaching.clear();approachTargets.clear();assignedLeader=0;}
    void invalidate(Character* actor){for(size_t i=0;i<assigned.size();++i)if(assigned[i]==actor){assigned[i]=0;refresh[i]=0;}}
    void tick(Character* leader,const std::vector<hand>& roster,bool active,float dt,bool targetedLeader=false,bool regroup=false){
        (void)targetedLeader;
        if(!leader){reset();return;}
        if(assignedLeader!=leader){reset();assignedLeader=leader;}
#ifdef MERCENARIE_CARAVAN_FORMATION
        const bool spreadBefore=caravan.engaged;
        if(caravan.tick(leader,roster,active,dt,regroup))return;
        if(spreadBefore){
            for(size_t i=0;i<roster.size();++i){Character* c=roster[i].getCharacter();if(c&&c!=leader&&!c->isPlayerCharacter())missionClearTravel(c,"caravan formation falls back to follow");}
            assigned.clear();refresh.clear();watches.clear();approaching.clear();approachTargets.clear();
        }
#endif
        if(assigned.size()<roster.size())assigned.resize(roster.size(),0);
        if(refresh.size()<roster.size())refresh.resize(roster.size(),0);
        if(approaching.size()<roster.size())approaching.resize(roster.size(),false);
        if(approachTargets.size()<roster.size())approachTargets.resize(roster.size());
        if(watches.size()<roster.size())watches.resize(roster.size());
        float closeSquared=1600.0f;
#ifdef MERCENARIE_CARAVAN_TRADE
        extern bool caravanMission;
        if(caravanMission)closeSquared=6400.0f;
#endif
        for(size_t i=0;i<roster.size();++i){
            Character* member=roster[i].getCharacter();
            if(!member||member==leader){assigned[i]=0;continue;}
            if(!active||leader->isDead()||member->isDead()){
                if(assigned[i]||missionFollowOrderPresent(member)){missionClearTravel(member,"formation suspended");}
                assigned[i]=0;refresh[i]=0;approaching[i]=false;continue;
            }
            if(member->isBeingCarried()||!member->getMedical()||member->getMedical()->isUnconcious()){assigned[i]=0;refresh[i]=0;continue;}
            if(missionCombatThreat(member)){missionYieldTravelToCombat(member);assigned[i]=0;refresh[i]=0;approaching[i]=false;watches[i].fresh();continue;}
            if(assigned[i]!=member)watches[i]=MissionMotionPolicy::Watch();
            const bool distant=member->getPosition().squaredDistance(leader->getPosition())>closeSquared;
            if(distant&&assigned[i]==member&&regroup==approaching[i]&&dt>0){
                const bool wrong=!missionFollowerTargetValid(member,leader,regroup);
                MissionMotionPolicy::Action action=watches[i].tick(member->getPosition(),leader->getPosition(),member->getMovement()->isCurrentlyMoving(),member->getMovement()->pathFailed()||wrong,dt,true);
                if(action!=MissionMotionPolicy::None){missionRecoverFollower(member,leader,action,regroup);refresh[i]=5;continue;}
                if(watches[i].step>0)continue; // bounded scheduler owns retries now
            }else if(!distant)watches[i]=MissionMotionPolicy::Watch();
            refresh[i]=std::max(0.0f,refresh[i]-dt);
            // Native FOLLOW can settle outside the regroup release radius.
            // During regroup only, give distant members a real approach order.
            // Keep it intact until arrival; never stack FOLLOW and MOVE.
            if(regroup&&member->getPosition().squaredDistance(leader->getPosition())>closeSquared){
                if(!approaching[i]||(refresh[i]<=0&&(!missionNativeTaskPresent(member,MOVE_CUS_ORDERED)||approachTargets[i].squaredDistance(leader->getPosition())>100.0f))){
                    missionClearTravel(member,"regroup approach current leader");
                    missionIssueOrder(member,MOVE_CUS_ORDERED,0,leader->getPosition());
                    missionOrderTrace(member,"regroup approach issued",leader);
                    approaching[i]=true;approachTargets[i]=leader->getPosition();refresh[i]=5.0f;
                }
                assigned[i]=member;continue;
            }
            if(regroup){
                // FOLLOW may choose a formation offset outside the release
                // radius again. Hold arrived members until the whole group is
                // ready instead of alternating MOVE/FOLLOW indefinitely.
                missionHoldTravel(member,"regroup member arrived");
                approaching[i]=true;assigned[i]=member;refresh[i]=0;continue;
            }
            if(approaching[i]){missionClearTravel(member,"regroup approach finished");approaching[i]=false;assigned[i]=0;refresh[i]=0;}
            if(assigned[i]==member){
                if(refresh[i]>0)continue;
                refresh[i]=5.0f;
                // Inspect instead of destroying/recreating a functioning native order.
                if(missionFollowOrderPresent(member))continue;
            }
            missionClearTravel(member,assigned[i]?"follow missing, repair":"join current leader");
            missionIssueOrder(member,FOLLOW_PLAYER_ORDER,leader,leader->getPosition());
            missionOrderTrace(member,"follow issued",leader);
            assigned[i]=member;refresh[i]=5.0f;
        }
    }
};
MissionGroupController missionGroup;
void updateMissionFormation(float dt){
#ifdef MERCENARIE_CARAVAN_TRADE
    if(caravanTradeActive())return;
#endif
    bool active=missionActive&&!missionPending&&!missionPaused&&!missionCasualtyWaiting&&!(finalWindow&&finalWindow->getVisible());
    missionGroup.tick(escort,progressMembers,active,dt,true);
}
