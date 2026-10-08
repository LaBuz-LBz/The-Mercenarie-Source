#pragma once
#include <string>
namespace GuildFurnitureRecovery {
struct State {
    bool active;
    int attempts;
    float clock;
    State():active(false),attempts(0),clock(0.0f){}
};
inline void begin(State& state){state.active=true;state.attempts=0;state.clock=1.0f;}
inline bool tick(State& state,float elapsed){if(!state.active)return false;state.clock-=elapsed;if(state.clock>0.0f)return false;++state.attempts;return true;}
inline void completeAttempt(State& state,bool ready){if(ready||state.attempts>=15){state.active=false;state.clock=0.0f;}else state.clock=2.0f;}
inline bool sameHouse(const std::string& expected,const std::string& candidate){return !expected.empty()&&expected==candidate;}
}
