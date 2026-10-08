#pragma once
bool roadAmbushPoint(Ogre::Vector3& point){
    if(!escort||!escort->getMovement()||!escort->getMovement()->havokCharacter||!ou||!ou->navmesh)return false;
    const Ogre::Vector3 origin=escort->getPosition();
    const float start=UtilityT::random(0.0f,6.283185f);
    for(int ring=0;ring<3;++ring)for(int i=0;i<12;++i){
        const float angle=start+i*.523599f,radius=45.0f+ring*25.0f;
        Ogre::Vector3 wanted=origin+Ogre::Vector3(Ogre::Math::Cos(angle)*radius,0,Ogre::Math::Sin(angle)*radius),candidate;
        if(!missionProjectExteriorRoadPoint(wanted,candidate)||!ou->navmesh->getPositionValid(candidate)||ou->navmesh->isInterior(ou->navmesh->getFaceKey(candidate)))continue;
        if(candidate.squaredDistance(wanted)>900||candidate.squaredDistance(origin)<900||candidate.squaredDistance(origin)>22500)continue;
        if(ou->navmesh->pathExists(escort->getMovement()->havokCharacter,candidate)!=1)continue;
        bool occupied=false;
        if(ou->player)for(unsigned int j=0;j<ou->player->playerCharacters.size();++j){Character* player=ou->player->playerCharacters[j];
            if(player&&player->getPosition().squaredDistance(candidate)<100)occupied=true;
        }
        if(!occupied){point=candidate;return true;}
    }
    return false;
}
bool roadAmbushActorReady(Character* enemy,const Ogre::Vector3& point){
    if(!enemy||enemy->isPlayerCharacter()||enemy->isDead()||enemy->isDown()||!enemy->getAI()||!enemy->getAI()->getTaskSystem()||!enemy->getMovement())return false;
    if(enemy->getMovement()->isIndoors()||!enemy->getMovement()->havokCharacter||enemy->getPosition().squaredDistance(point)>10000)return false;
    // The factory may scatter the squad: validate the resulting locations too.
    if(!ou->navmesh->getPositionValid(enemy->getPosition())||ou->navmesh->isInterior(ou->navmesh->getFaceKey(enemy->getPosition())))return false;
    return ou->navmesh->pathExists(enemy->getMovement()->havokCharacter,escort->getPosition())==1;
}
void roadAmbushEngage(Character* enemy){
    enemy->getAI()->setManuveringFreedomLevel(AI::ROAM_FAR);
    enemy->getAI()->setCenterOfMovementTarget(escort->getHandle());
    enemy->attackTarget(escort);
    enemy->getAI()->getTaskSystem()->addOrder(UNPROVOKED_FOCUSED_MELEE_ATTACK,escort->getHandle(),escort->getPosition(),true,false);
}
