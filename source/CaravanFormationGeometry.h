#pragma once
#include <cmath>
#include <algorithm>
namespace CaravanFormation {
struct Offset {float side,forward;Offset(float s,float f):side(s),forward(f){}};
inline Offset slot(bool animal,unsigned index,unsigned guardCount=0){
    if(animal)return Offset(0,(index%2?1.0f:-1.0f)*std::min(60.0f,40.0f+12.0f*(index/2)));
    if(guardCount==1)return Offset(0,18);
    return Offset(index%2?18.0f:-18.0f,((index/2)%2?-1.0f:1.0f)*std::min(48.0f,18.0f+12.0f*(index/4)));
}
template<class V> V target(const V& centre,float dx,float dz,const Offset& slot){
    float length=std::sqrt(dx*dx+dz*dz);if(length<.01f){dx=0;dz=1;length=1;}
    dx/=length;dz/=length;
    return V(centre.x+dx*slot.forward+dz*slot.side,centre.y,centre.z+dz*slot.forward-dx*slot.side);
}
}
