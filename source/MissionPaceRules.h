#pragma once
#include <cmath>
namespace MissionPaceRules {
// Running uses the actual capacity of the group, not the native jog ceiling.
inline float gaitLimit(float maximum,float walk,bool running){return running?maximum:std::min(maximum,walk);}

inline float leaderLimit(float groupLimit,float farthestSquared,bool accelerated){
    if(!accelerated)return groupLimit;
    // Keep catch-up headroom even before the hard regroup threshold (100).
    // Slow progressively from 40 to 90, without replacing the route with HOLD.
    float gap=std::sqrt(farthestSquared);
    float t=gap<=40?0:gap>=90?1:(gap-40)/50;
    return groupLimit*(0.85f-0.40f*t);
}
}
