#pragma once
#include <cmath>
namespace MissionMotionPolicy {
enum Action { None, Wait, Restore, Repath, Local, Global, Suspend };
enum Reason { Ready, Regroup, Care, Player, Unavailable, Exit, Recalculating, Inaccessible, Suspended };
struct Watch {
    bool initialized, suspended; float x,y,z,tx,ty,tz,best,idle,healthy,cooldown; int step;
    Watch():initialized(false),suspended(false),x(0),y(0),z(0),tx(0),ty(0),tz(0),best(0),idle(0),healthy(0),cooldown(0),step(0){}
    void fresh(){initialized=false;idle=0;healthy=0;} // preserve exhausted budget across interruptions
    template<class V> Action tick(const V& p,const V& goal,bool moving,bool failed,float dt,bool targetMoves=false){
        if(suspended)return None;
        const float distance=std::sqrt(p.squaredDistance(goal));
        if(!initialized||(!targetMoves&&((tx-goal.x)*(tx-goal.x)+(ty-goal.y)*(ty-goal.y)+(tz-goal.z)*(tz-goal.z)>1))){
            initialized=true;x=p.x;y=p.y;z=p.z;tx=goal.x;ty=goal.y;tz=goal.z;best=distance;idle=0;healthy=0;
        }
        const float displacement=(x-p.x)*(x-p.x)+(y-p.y)*(y-p.y)+(z-p.z)*(z-p.z);
        // Metres, never a subtraction of squared distances. Actual displacement
        // allows legitimate native detours away from a local waypoint.
        const bool progress=displacement>=0.25f||distance+0.5f<best;
        if(progress){x=p.x;y=p.y;z=p.z;best=distance;healthy+=idle+dt;idle=0;if(healthy>=20){step=0;cooldown=0;}}
        else {idle+=dt;if(idle>=8)healthy=0;}
        cooldown-=dt;if(cooldown>0)return None;
        if((!failed||progress)&&idle<(moving?45.0f:8.0f))return None;
        if(step==0){++step;cooldown=2;return Wait;}
        if(step==1){++step;cooldown=5;return Restore;}
        if(step==2){++step;cooldown=8;return Repath;}
        if(step==3){++step;cooldown=12;return Local;}
        if(step==4){++step;cooldown=20;return Global;}
        suspended=true;return Suspend;
    }
};
struct RouteRetry {
    unsigned attempts;float cooldown;bool exhausted;
    RouteRetry():attempts(0),cooldown(0),exhausted(false){}
    bool due(float dt){cooldown-=dt;return !exhausted&&cooldown<=0;}
    void failed(){++attempts;cooldown=15;exhausted=attempts>=3;}
};
inline bool scienceArrived(float distanceSquared){return distanceSquared<=6400.0f;}
}
