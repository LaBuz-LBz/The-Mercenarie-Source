#pragma once

// A temporary native action. No AI orders, faction changes or save data.
namespace GuardRest {
static const char* const dataName="Mercenarie Guard Rest";
static const char* const animationName="Mercenarie_GuardRest_v1";
struct Member { hand actor; Ogre::Vector3 origin; unsigned ticks; Member():ticks(0){} };
static std::vector<Member> members;

inline bool eligible(Character* c) {
    return c&&c->isPlayerCharacter()&&!c->isAnimal()&&!c->isDead()&&
        !c->isDisabled()&&!c->isRagdoll()&&!c->isBeingCarried()&&
        !c->isCarryingSomething&&!c->isInCombatMode(true,true)&&c->getMovement();
}
inline bool stationary(Character* c) {
    // Path-following idle and cached speed can remain stale after arrival.
    return !c->getMovement()->isCurrentlyMoving();
}
inline void message(const char* key) {if(ou)ou->showPlayerAMessage(Loc::text(key),true);}
inline void unavailable(const char* reason) {DebugLog(std::string("Guard rest unavailable: ")+reason);message("developer.guard_rest.missing");}
inline void discardWorld() {members.clear();}
inline void stop(Character* c) {
    AnimationClass* a=c?c->getAnimationClass():0;
    if(a&&a->getIsActivated()){
        a->stopAction(std::string(dataName));
        AnimationData* pose=a->getAnimationData(std::string(dataName));
        if(pose)a->stopAnimation(pose);
    }
}
inline void releaseSelected() {
    Character* c=ou&&ou->player?ou->player->selectedCharacter.getCharacter():0;
    for(size_t i=0;c&&i<members.size();++i)if(members[i].actor==c->getHandle()){
        stop(c);members.erase(members.begin()+i);return;
    }
}inline void apply() {
    DebugLog("Guard rest: button clicked");
    Character* c=ou&&ou->player?ou->player->selectedCharacter.getCharacter():0;
    if(!eligible(c)){DebugLog("Guard rest rejected: character state or ownership");message("developer.guard_rest.invalid");return;}
    if(!stationary(c)){DebugLog("Guard rest rejected: physical movement is active");message("developer.guard_rest.moving");return;}
    AnimationClass* a=c->getAnimationClass();
    if(!a||!a->getIsActivated()||!a->getAnimationDatasList()){
        unavailable("actor animation system not ready");return;
    }
    for(size_t i=0;i<members.size();++i)if(members[i].actor==c->getHandle()){message("developer.guard_rest.requested");DebugLog("Guard rest: already tracked");return;}
    if(a->hasAction()){DebugLog("Guard rest rejected: another native action is active");message("developer.guard_rest.busy");return;}
    AnimationData* pose=a->getAnimationData(std::string(dataName));
    if(!pose||pose->animName!=animationName||pose->restrictsMovementOrders||pose->relocates){
        unavailable("missing or unsafe animation record");return;
    }
    // hasAnimation resolves an FCS data name; during animation work, a raw Ogre name returns false.
    if(!a->hasAnimation(std::string(dataName))){unavailable("FCS pose found but actor skeleton lacks it");return;}
    a->playAction(pose,1.0f,1.0f,false);
    DebugLog(std::string("Guard rest: action requested; native hasAction=")+(a->hasAction()?"yes":"no"));
    message("developer.guard_rest.requested");
    Member member;member.actor=c->getHandle();member.origin=c->getPosition();members.push_back(member);
}
inline void update() {
    for(size_t i=0;i<members.size();) {
        Character* c=members[i].actor.getCharacter();
        ++members[i].ticks;
        if(c&&(members[i].ticks==1||members[i].ticks==30)){AnimationClass* a=c->getAnimationClass();DebugLog(std::string("Guard rest: follow-up playing=")+(a&&a->getAnimationPlaying_datName(dataName)?"yes":"no"));}
        if(!eligible(c)||!stationary(c)||(c->getPosition()-members[i].origin).squaredLength()>0.0004f) {
            DebugLog(std::string("Guard rest stopped: ")+(!eligible(c)?"character state":!stationary(c)?"physical movement":"position changed"));
            stop(c);members.erase(members.begin()+i);
        }else {
            AnimationClass* a=c->getAnimationClass();
            AnimationData* pose=a&&a->getAnimationDatasList()?a->getAnimationData(std::string(dataName)):0;
            // Native loop animations must be submitted each update. runAnimation
            // keeps the existing track alive; it does not restart playAction.
            if(!a||!a->getIsActivated()||!pose||
               (a->hasAction()&&a->animationRequirements._currentAction!=pose)){
                DebugLog("Guard rest stopped: animation unavailable or another action took priority");
                stop(c);members.erase(members.begin()+i);continue;
            }
            a->runAnimation(pose,1.0f,pose->layername,1.0f);
            ++i;
        }
    }
}
}



