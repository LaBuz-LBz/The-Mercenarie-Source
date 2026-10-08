#pragma once
#include <algorithm>
#include <limits>
namespace MissionDebugRules {
inline bool humanoidCandidate(bool animal,bool player){return !animal&&!player;}
inline bool finite(float v){return v==v&&v<=std::numeric_limits<float>::max()&&v>=-std::numeric_limits<float>::max();}
inline float bluntDamage(float maximum,float flesh,float stun){
    if(!finite(maximum)||!finite(flesh)||!finite(stun)||maximum<=0||stun<0||flesh-stun<maximum*0.5f)return 0;
    return std::min(maximum*0.15f,(flesh-stun)*0.2f);
}
// Small untreated cuts on two organic parts make a medkit useful. Keep the
// combined cut + blunt injury well above zero, even at the preflight boundary.
inline float treatableCut(float maximum,float flesh,float stun){
    if(bluntDamage(maximum,flesh,stun)<=0)return 0;
    return std::min(maximum*0.10f,(flesh-stun)*0.10f);
}
inline bool active(bool running,bool pending,bool paid,bool cleanup,bool leaderAlive){return running&&!pending&&!paid&&!cleanup&&leaderAlive;}
// Unsigned subtraction followed by signed comparison tolerates GetTickCount wrap.
inline bool ready(bool busy,unsigned long now,unsigned long deadline){return !busy&&(deadline==0||(long)(now-deadline)>=0);}
}
