#pragma once
#include <algorithm>
namespace MissionLegacyNavigation {
template<class V> struct State {
    bool active,attempted;int retries;float best,stalled,cooldown;V goal;
    State():active(false),attempted(false),retries(0),best(-1),stalled(0),cooldown(0){}
    void select(const V& destination){if(goal.squaredDistance(destination)>1){*this=State();goal=destination;}}
    bool begin(const V& destination){select(destination);if(attempted)return false;active=attempted=true;return true;}
    // Original V8 progress thresholds, with a bounded last-resort retry budget.
    int tick(float distance,bool moving,bool failed,float elapsed){
        cooldown=std::max(0.0f,cooldown-elapsed);
        // V8-STABLE/MissionRouteRecovery.h, kept separate from the modern
        // compatibility entry point whose progress threshold has changed.
        bool due=false;
        if(best<=0||distance+2500.0f<best){best=distance;stalled=0;}
        else {stalled+=elapsed;due=stalled>=(moving?45.0f:8.0f);}
        if(cooldown>0||(!failed&&!due))return 0;
        if(retries>=2)return -1;
        ++retries;cooldown=8;stalled=0;best=distance;return 1;
    }
};
}
