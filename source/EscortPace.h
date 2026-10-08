#pragma once

namespace EscortPace {
enum State { Normal=0, Accelerated=1 };

inline bool valid(int state){return state==Normal||state==Accelerated;}
inline bool accelerate(State& state){if(state!=Normal)return false;state=Accelerated;return true;}
inline bool slowDown(State& state){if(state!=Accelerated)return false;state=Normal;return true;}

// Kenshi native MoveSpeed values: WALK=0, RUN=2.
inline int nativeSpeed(State state){return state==Accelerated?2:0;}
}
