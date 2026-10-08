#pragma once
namespace MissionGateGeometry {
inline float side(const Ogre::Vector3& p,const Ogre::Vector3& center,const Ogre::Vector3& axis){return (p.x-center.x)*axis.x+(p.z-center.z)*axis.z;}
inline bool frame(const Ogre::Vector3& inside,const Ogre::Vector3& outside,Ogre::Vector3& center,Ogre::Vector3& axis){
    float x=outside.x-inside.x,z=outside.z-inside.z,n=sqrt(x*x+z*z);
    if(!(n>1&&n<300))return false;
    center=Ogre::Vector3((inside.x+outside.x)*.5f,(inside.y+outside.y)*.5f,(inside.z+outside.z)*.5f);axis=Ogre::Vector3(x/n,0,z/n);return true;
}
inline bool choose(const Ogre::Vector3& actor,const Ogre::Vector3& center,const Ogre::Vector3& axis,const std::vector<Ogre::Vector3>& points,size_t first,size_t& result,float approachLimit=180.0f){
    const float start=side(actor,center,axis);if(fabs(start)<2||fabs(start)>approachLimit)return false;
    const float sign=start<0?1.0f:-1.0f;
    Ogre::Vector3 previous=actor;float distance=0;bool crossed=false;
    for(size_t i=first;i<points.size()&&i<first+8;++i){
        const Ogre::Vector3 p=points[i];float dx=p.x-previous.x,dz=p.z-previous.z;distance+=sqrt(dx*dx+dz*dz);if(distance>approachLimit+220)return false;
        float a=side(previous,center,axis)*sign,b=side(p,center,axis)*sign;
        if(!crossed&&a<=0&&b>0){
            float t=-a/(b-a);float x=previous.x+t*dx-center.x,z=previous.z+t*dz-center.z;
            if(x*x+z*z>625)return false; // itinerary must cross the opening, not a nearby wall
            crossed=true;
        }
        if(crossed&&b>=45){if(b>180)return false;result=i;return true;}
        previous=p;
    }
    return false;
}
}
