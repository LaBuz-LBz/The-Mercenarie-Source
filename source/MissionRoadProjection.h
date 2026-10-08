#pragma once
// Road data describes an itinerary, not the current height of the walkable mesh.
// Validate against the projection seed, not the obsolete road elevation.
bool missionProjectLocalRoadPoint(const Ogre::Vector3& wanted,const Ogre::Vector3& actor,Ogre::Vector3& projected){
    if(missionProjectExteriorRoadPoint(wanted,projected)&&projected.x==projected.x&&projected.y==projected.y&&projected.z==projected.z&&projected.squaredDistance(wanted)<=900)return true;
    const float dx=wanted.x-actor.x,dz=wanted.z-actor.z;
    if(dx*dx+dz*dz>90000||fabs(wanted.y-actor.y)<=20)return false;
    Ogre::Vector3 seed=wanted;seed.y=actor.y;
    if(!missionProjectExteriorRoadPoint(seed,projected)||projected.x!=projected.x||projected.y!=projected.y||projected.z!=projected.z||projected.squaredDistance(seed)>900)return false;
    return true;
}
