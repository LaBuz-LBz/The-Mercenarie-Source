#include "CleanupState.h"
#pragma once
// Translate only observed native barks for mission NPCs. Leave the shared
// dialogue data, actions, unrelated speakers and other lines untouched.
const char* missionSpeechKey(const std::string& line){
    if(line=="Time for me to go")return "mission.native.time_to_go";
    if(line=="Looks like it's over...")return "mission.native.looks_over";
    return 0;
}
#ifndef MISSION_SPEECH_RULES_ONLY
namespace {
void (*missionSayOriginal)(Dialogue*,const std::string&,DialogLineData*);
void missionSayHook(Dialogue* dialogue,const std::string& text,DialogLineData* line){
    if(MercenarieCleanup::disabled){missionSayOriginal(dialogue,text,line);return;}
    if(dialogue&&line&&isContractGreeting(line->getStringID())){
        const int owner=missionNativeGoalOwner(dialogue->me);
        if(owner>=0){
            const bool paused=owner==selectedEscortQuest?missionPaused:escortQuests[owner].v_missionPaused;
            missionSayOriginal(dialogue,Loc::text(paused?"ui.i_am_waiting_here_until_you_tell_me_to":"ui.we_are_travelling_under_your_protection"),line);
            return;
        }
    }
    const char* key=missionSpeechKey(text);
    if(key&&dialogue&&missionNativeGoalOwner(dialogue->me)>=0){
        const std::string translated=Loc::text(key);
        DebugLog(std::string("MISSION SPEECH localized key=")+key+" actor="+dialogue->me->getHandle().toString());
        missionSayOriginal(dialogue,translated,line);
    }else missionSayOriginal(dialogue,text,line);
}
}
#endif
