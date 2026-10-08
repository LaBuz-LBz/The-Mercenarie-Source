#pragma once
bool missionScienceSelectBuilding(){
    if(missionRescue.scienceBuildingScanned)return !missionRescue.scienceBuilding.isNull()&&missionRescue.scienceBuilding.getBuilding();
    missionRescue.scienceBuildingScanned=true;
    if(!shou||!shou->townList||!ou||!ou->zoneMgr)return false;
    Town* town=shou->townList->getTownBySID(currentContract.destinationId);if(!town)return false;
    lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,town,0,false,0,0);
    float best=14400.0f;
    for(unsigned int i=0;i<buildings.size();++i){Building* b=buildings[i];if(!b||b->isDestroyed()||b->doors.size()==0)continue;
        float d=b->getPosition().squaredDistance(scientificRuinCenter);if(d<best){best=d;missionRescue.scienceBuilding=b->getHandle();}}
    return !missionRescue.scienceBuilding.isNull();
}
Ogre::Vector3 missionScienceEntryPoint(){
    Building* b=missionRescue.scienceBuilding.getBuilding();if(!b)return scientificRuinCenter;
    for(unsigned int i=0;i<b->doors.size()&&i<16;++i)if(b->doors[i]&&b->doors[i]->getDoor())return b->doors[i]->getDoor()->getDoorPosInside();
    return b->getPosition();
}
bool missionScienceInsideExpected(){
    return missionScienceSelectBuilding()&&escort&&escort->getMovement()->isIndoors()
        &&escort->getMovement()->building.toString()==missionRescue.scienceBuilding.toString();
}
bool missionSciencePoint(const Ogre::Vector3& wanted,Ogre::Vector3& valid){
    if(!ou||!ou->navmesh)return false;
    // Exterior sampling remains the activity fallback. Inside, observe from the
    // actor's already occupied native surface rather than guessing another floor.
    if(scientificInsideDiscovery&&missionScienceInsideExpected()){valid=escort->getPosition();return true;}
    return missionProjectExteriorRoadPoint(wanted,valid)&&valid.squaredDistance(wanted)<=900&&valid.squaredDistance(scientificRuinCenter)<=10000&&escort->getMovement()->havokCharacter&&ou->navmesh->pathExists(escort->getMovement()->havokCharacter,valid)==1;
}
void beginScientificActivity(){
    missionClearTravel(escort,"science arrival");
    scientificResearching=true;scientificResearchSeconds=UtilityT::random(210.0f,300.0f);scientificMoveClock=0;scientificCommentClock=5;scientificRuinCenter=destination;
    missionRescue.scienceBuildingScanned=false;missionRescue.scienceBuilding.setNull();missionScienceSelectBuilding();
    missionRescue.passage.clear();missionRescue.localRecovery=false;missionRescue.leaderWatch=MissionMotionPolicy::Watch();
    escort->sayALine(std::string(Loc::text("ui.we_have_arrived_at"))+destinationName+Loc::text("ui.protect_the_site_while_we_examine_its_remains"),true);
    ou->showPlayerAMessage(std::string(Loc::text("ui.research_phase_a"))+destinationName+Loc::text("ui.external_observation_before_a_possible_entry_attempt"),true);
}
