#pragma once
bool caravanHomeAmbushFleeing(Character* c){return c&&CaravanHomeAmbush::fleeing(currentContract.routeRegions,c->getHandle().toString());}
void tickCaravanHomeAmbushFlight(float elapsed){
    if(!missionActive||missionPaused)return;
    CaravanHomeAmbush::State s=CaravanHomeAmbush::read(currentContract.routeRegions);if(!s.triggered||s.remaining<=0)return;
    Character* guard=deliveryGuard(s.runner);if(!guard)return;
    if(guard->isDead()||guard->isBeingCarried()||!rescueCanWalk(guard)){s.remaining=0;CaravanHomeAmbush::write(currentContract.routeRegions,s);return;}
    // Never replace medical assistance with the cinematic escape.
    if(guard->isCarryingSomething||missionNativeTaskPresent(guard,FIRST_AID_ORDER)||missionNativeTaskPresent(guard,LIFT_PERSON_PLAYER_ORDER))return;
#ifdef MERCENARIE_CARAVAN_BATTLE_CONTROL
    CaravanBattleResolution::State resolution=CaravanBattleResolution::read(currentContract.routeRegions);
    Ogre::Vector3 refuge=resolution.anchored?Ogre::Vector3((float)resolution.x,(float)resolution.y,(float)resolution.z):escort->getPosition();
#else
    Ogre::Vector3 refuge=escort->getPosition();
#endif
    /* Escape ends at the caravan, never at a fixed timeout. */s.retry=std::max(0.0,s.retry-elapsed);
    if(!guard->getMovement()->isIndoors()&&escort&&tradeDistance(guard->getPosition(),refuge)<=400)s.remaining=0;
    else if(s.retry<=0){s.retry=2;Ogre::Vector3 outside;
        if((guard->getMovement()->isIndoors()&&deliveryExit(guard,outside))||(!guard->getMovement()->isIndoors()&&escort&&tradeExterior(guard,refuge,outside))){
            missionClearTravel(guard,"ambush escape through home door");missionIssueOrder(guard,MOVE_CUS_ORDERED,0,outside);
#ifdef MERCENARIE_CARAVAN_BATTLE_CONTROL
            hand none;none.setNull();guard->getAI()->getTaskSystem()->addOrder(MOVE_CUS_ORDERED,none,outside,true,false);
#endif
            guard->getMovement()->setDesiredSpeedOrders(RUN);guard->getMovement()->setDesiredSpeed(guard->getStats()->getMaxRunSpeed());
        }
    }
    CaravanHomeAmbush::write(currentContract.routeRegions,s);
}
bool triggerCaravanHomeAmbush(Character* guard,Building* home){
    CaravanHomeAmbush::State s=CaravanHomeAmbush::read(currentContract.routeRegions);if(s.attempted)return s.triggered;
    // Persist the one-shot barrier before the native factory is called.
    s.attempted=true;CaravanHomeAmbush::write(currentContract.routeRegions,s);
    if(!spawnCaravanHomeBandits(guard,home)){ErrorLog("CARAVAN HOME AMBUSH unavailable; normal delivery retained");return false;}
#ifdef MERCENARIE_CARAVAN_BATTLE_CONTROL
    CaravanBattleResolution::State resolution;resolution.anchored=true;Ogre::Vector3 anchor=escort->getPosition();resolution.x=anchor.x;resolution.y=anchor.y;resolution.z=anchor.z;CaravanBattleResolution::write(currentContract.routeRegions,resolution);
#endif
    s.triggered=true;s.runner=guard->getHandle().toString();s.remaining=15;s.retry=0;CaravanHomeAmbush::write(currentContract.routeRegions,s);
    guard->sayALine(Loc::text("caravan.ambush.help"),true);
    fleeCaravanAccomplice(home);
    endCaravanCustomers();
    tickCaravanHomeAmbushFlight(0);
    DebugLog(std::string("CARAVAN HOME AMBUSH triggered guard=")+s.runner+" id="+currentMissionFiscalId);
    return true;
}
