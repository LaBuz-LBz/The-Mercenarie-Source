#pragma once
#include <algorithm>

namespace EscortMissionRules {
inline int forcedPacePenalty(int cappedReward,bool forced){return forced?std::max(0,cappedReward)/10:0;}
inline int afterForcedPacePenalty(int cappedReward,bool forced){return std::max(0,cappedReward-forcedPacePenalty(cappedReward,forced));}

struct Separation {
    float suspiciousSeconds;
    bool warned;
    Separation():suspiciousSeconds(0),warned(false){}
};
enum SeparationResult { SeparationSafe, SeparationWarn, SeparationFail };
inline SeparationResult updateSeparation(Separation& s,bool abnormal,float elapsed){
    if(!abnormal){s.suspiciousSeconds=0;s.warned=false;return SeparationSafe;}
    s.suspiciousSeconds+=std::max(0.0f,elapsed);
    if(s.suspiciousSeconds>=90.0f)return SeparationFail;
    if(s.suspiciousSeconds>=30.0f&&!s.warned){s.warned=true;return SeparationWarn;}
    return SeparationSafe;
}
}
