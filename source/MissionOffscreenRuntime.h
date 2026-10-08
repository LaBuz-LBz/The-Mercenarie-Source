#include "CleanupState.h"
#pragma once
// Use the native full-simulation decision, not camera movement, teleports,
// synthetic elapsed travel or manual calls to Character::update.
namespace {
bool (*missionOffscreenImmuneOriginal)(Character*);
bool missionOffscreenImmuneHook(Character* actor){
    if(MercenarieCleanup::disabled)return missionOffscreenImmuneOriginal?missionOffscreenImmuneOriginal(actor):false;
    if(actor&&!actor->isDead()&&missionNativeGoalOwner(actor)>=0)return true;
    return missionOffscreenImmuneOriginal?missionOffscreenImmuneOriginal(actor):false;
}
}
