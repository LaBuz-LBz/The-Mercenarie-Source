#pragma once
#include <vector>
#include <cmath>
#include <cstddef>
namespace MissionRoadProgress {
// Native walking can pass an approximate road centre without entering its
// 16m arrival circle. Recognise a passed cross-section only on a straight
// local corridor. This consumes an itinerary sample, never a physical obstacle.
template<class V> bool passedSample(const std::vector<V>& points,size_t i,const V& p){
    if(i>=points.size()||points.size()-i<2)return false; // final objective
    const V& q=points[i];const V& next=points[i+1];
    const double vx=next.x-q.x,vz=next.z-q.z,n=vx*vx+vz*vz;
    if(!(n>=16&&n<=90000))return false;
    double ax,az;
    if(i){ax=q.x-points[i-1].x;az=q.z-points[i-1].z;}
    else {
        // A freshly planned route starts at the road attachment point. Require
        // two forward samples to establish its direction, not an invented origin.
        if(points.size()<3)return false;
        ax=points[2].x-next.x;az=points[2].z-next.z;
    }
    const double a=ax*ax+az*az,dot=ax*vx+az*vz;
    if(!(a>=16&&a<=90000&&dot>0&&dot*dot>=0.95*0.95*a*n))return false; // retain bends/U-turns
    const double dx=p.x-q.x,dz=p.z-q.z,length=std::sqrt(n);
    const double along=(dx*vx+dz*vz)/length,cross=(dx*vz-dz*vx)/length;
    return along>=(i?0.5:-1.0)&&along<=60&&cross*cross<=1024&&dx*dx+dz*dz<=4096;
}
// Hand off a straight intermediate leg before native arrival can end MOVE.
// Reuse the same straight-corridor checks as passedSample; corners stay exact.
template<class V> bool handoffSample(const std::vector<V>& points,size_t i,const V& p){
    if(i>=points.size()||points.size()-i<2)return false;
    const V& q=points[i];const V& next=points[i+1];
    const double vx=next.x-q.x,vz=next.z-q.z,n=vx*vx+vz*vz;
    if(!(n>=16&&n<=90000))return false;
    const double length=std::sqrt(n);
    const V probe((float)(q.x+vx/length),q.y,(float)(q.z+vz/length));
    if(!passedSample(points,i,probe))return false;
    const double dx=p.x-q.x,dz=p.z-q.z;
    const double along=(dx*vx+dz*vz)/length,cross=(dx*vz-dz*vx)/length;
    // Match the passed-sample lateral corridor in both travel directions.
    // Bound longitudinal approach separately: lateral offset must not delay
    // handoff until native MOVE has already ended at an approximate road sample.
    return along>=-32&&along<0&&cross*cross<=1024;
}

}
