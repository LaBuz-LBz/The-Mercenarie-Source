#pragma once
#include <vector>
#include <cmath>
#include <cstddef>
// Runtime-only leader episode. The follower policy is deliberately unchanged.
namespace MissionPassageRecovery {
template<class V> struct Episode {
    bool active;float seconds;V origin,failedGoal,businessGoal;
    size_t joinIndex;std::vector<V> rejected;
    Episode():active(false),seconds(0),joinIndex((size_t)-1){}
    void clear(){active=false;seconds=0;joinIndex=(size_t)-1;rejected.clear();}
    void begin(const V& p,const V& target,const V& business){clear();active=true;origin=p;failedGoal=target;businessGoal=business;rejected.push_back(target);}
    bool different(const V& p)const {for(size_t i=0;i<rejected.size();++i)if(p.squaredDistance(rejected[i])<144.0f)return false;return true;}
    void reject(const V& p){if(rejected.size()<8)rejected.push_back(p);}
    bool useful(const V& actor,const V& wanted,const V& projected)const{
        return projected.x==projected.x&&projected.y==projected.y&&projected.z==projected.z
            &&projected.squaredDistance(wanted)<=900&&projected.squaredDistance(actor)>=36
            &&projected.squaredDistance(actor)<=57600&&different(projected);
    }
    bool escaped(const V& p,const V& target,bool detour)const{
        if(p.squaredDistance(origin)<400)return false;
        // Match the road arrival tolerance; native walking need not land exactly on a point.
        if(detour)return p.squaredDistance(target)<=256&&(joinIndex!=(size_t)-1||p.squaredDistance(origin)>=1600);
        return std::sqrt(origin.squaredDistance(failedGoal))-std::sqrt(p.squaredDistance(failedGoal))>=8;
    }
};
}
