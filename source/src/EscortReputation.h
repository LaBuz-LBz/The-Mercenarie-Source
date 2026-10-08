#pragma once
#include "EscortConfig.h"

namespace EscortReputation
{
    inline float clamp(float value){return value>100?100:(value<-100?-100:value);}
    inline float globalFromQuality(bool success,bool healthy,bool noKo,bool fast,bool prestigious)
    {
        if(!success)return EscortConfig::ReputationFailure*EscortConfig::GlobalFailureMultiplier;
        return EscortConfig::ReputationGlobalSuccess+(healthy?0.15f:0)+(noKo?0.10f:0)+(fast?0.10f:0)+(prestigious?0.20f:0);
    }
}
