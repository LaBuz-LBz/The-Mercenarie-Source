#pragma once
#include <cmath>
namespace MissionRouteRecovery {
// Compatibility entry point for existing route tests; both inputs are squared,
// but the threshold is measured in metres.
inline bool shouldRetry(float remainingSquared,bool moving,float elapsed,float& best,float& idle){
    if(best<0||std::sqrt(best)-std::sqrt(remainingSquared)>=0.5f){best=remainingSquared;idle=0;return false;}
    idle+=elapsed;return idle>=(moving?45.0f:8.0f);
}
}
