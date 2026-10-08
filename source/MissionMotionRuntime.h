#pragma once
void missionWaitReason(MissionMotionPolicy::Reason reason){
    if(missionRescue.waitReason==reason)return;
    missionRescue.waitReason=reason;
    const char* keys[]={"motion.ready","motion.regroup","motion.care","motion.player","motion.unavailable","motion.exit","motion.recalculating","motion.inaccessible","motion.suspended"};
    DebugLog(std::string("MISSION WAIT id=")+currentMissionFiscalId+" reason="+keys[reason]);
    if(!missionNavigationNotice(reason)&&ou&&(reason==MissionMotionPolicy::Suspended||reason==MissionMotionPolicy::Inaccessible||missionRescue.waitNoticeClock<=0)){
        ou->showPlayerAMessage(Loc::text(keys[reason]),true);missionRescue.waitNoticeClock=15;
    }
}
void missionSuspendMotion(){
    if(missionRequestPlayerGuide())return;
    missionRescue.motionSuspended=true;missionPaused=true;
    missionWaitReason(MissionMotionPolicy::Suspended);
    if(escort)missionHoldTravel(escort,"bounded recovery exhausted");
}
bool missionFollowerTargetValid(Character* actor,Character* leader,bool approach){
    if(!actor||!actor->getAI()||!actor->getAI()->getTaskSystem())return false;
    AITaskSytem* tasks=actor->getAI()->getTaskSystem();
    for(size_t i=0;i<tasks->orders.list.size();++i){Tasker* t=tasks->orders.list[i];if(!t)continue;
        if(approach&&t->key()==MOVE_CUS_ORDERED&&t->location.squaredDistance(leader->getPosition())<100)return true;
        if(!approach&&t->key()==FOLLOW_PLAYER_ORDER&&t->subject.toString()==leader->getHandle().toString())return true;
    }return false;
}
void missionRecoverFollower(Character* member,Character* leader,MissionMotionPolicy::Action action,bool approach){
    using namespace MissionMotionPolicy;
    if(action==None||action==Wait)return;
    if(action==Suspend){missionSuspendMotion();return;}
    if(action==Repath||action==Local||action==Global)member->getMovement()->invalidatePath();
    Ogre::Vector3 target=leader->getPosition();
    if(action==Local&&ou&&ou->navmesh){Ogre::Vector3 point;const Ogre::Vector3 pos=member->getPosition();
        Ogre::Vector3 wanted=pos+(target-pos).normalisedCopy()*8.0f;
        if(ou->navmesh->getClosestPoint(wanted,8.0f,1.0f,false,point)&&member->getMovement()->havokCharacter&&ou->navmesh->pathExists(member->getMovement()->havokCharacter,point)==1){missionClearTravel(member,"follower local recovery");missionIssueOrder(member,MOVE_CUS_ORDERED,0,point);return;}}
    missionClearTravel(member,"follower bounded recovery");
    missionIssueOrder(member,approach?MOVE_CUS_ORDERED:FOLLOW_PLAYER_ORDER,approach?0:leader,target);
}
