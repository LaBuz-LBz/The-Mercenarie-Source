#pragma once
// Included after the commerce and customer runtimes. State lives in contract
// metadata, so save/load and concurrent quest slots retain their own deliveries.
bool caravanDeliveryOwns(Character* c){
    return c&&CaravanDelivery::owns(CaravanDelivery::read(currentContract.routeRegions),c->getHandle().toString());
}
bool caravanHasDeliveryGuard(){
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();
        if(c&&c!=escort&&!c->isDead()&&!c->isAnimal()&&!c->isPlayerCharacter())return true;
    }return false;
}
Character* deliveryGuard(const std::string& id){
    // Use the mission's already resolved native handles, including non-leader
    // squad members. Never reinterpret a string as a new native actor handle.
    for(size_t i=0;i<progressMembers.size();++i){
        Character* c=progressMembers[i].getCharacter();
        if(c&&(c->getHandle().toString()==id||progressMembers[i].toString()==id))return c;
    }
    return 0;
}
Building* deliveryHome(const std::string& id){
    Town* town=tradeTown();if(!town||!ou||!ou->zoneMgr)return 0;
    lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,town,0,false,0,0);
    for(unsigned int i=0;i<buildings.size();++i)if(buildings[i]&&buildings[i]->getHandle().toString()==id)return buildings[i];
    return 0;
}
DoorStuff* deliveryDoor(Building* home){
    if(!home||home->isDestroyed()||!home->hasInterior())return 0;
    for(unsigned int i=0;i<home->doors.size();++i){DoorStuff* door=home->doors[i]?home->doors[i]->getDoor():0;
        if(door&&door->isSetupComplete&&door->parent==home&&!door->isLocked())return door;
    }return 0;
}
void deliveryMove(Character* guard,const Ogre::Vector3& point,const char* stage){
    // Keep a live path instead of restarting a long walk every five seconds.
    if(missionNativeTaskPresent(guard,MOVE_CUS_ORDERED)&&!guard->getMovement()->pathFailed()&&guard->getMovement()->getDestination().squaredDistance(point)<4)return;
    std::ostringstream log;log<<"CARAVAN DELIVERY move stage="<<stage<<" actor="<<guard->getHandle().toString()<<" from="<<guard->getPosition()<<" target="<<point;DebugLog(log.str());
    tradeMove(guard,point);
}
bool deliveryEntry(Character* guard,Building* home,Ogre::Vector3& point){
    if(!home||home->isDestroyed()||!home->hasInterior()||!guard||!guard->getMovement()||!guard->getMovement()->havokCharacter||!ou||!ou->navmesh)return false;
    home->loadInteriorPhysical(true);
    for(unsigned int i=0;i<home->doors.size();++i){DoorStuff* door=home->doors[i]?home->doors[i]->getDoor():0;
        if(!door||!door->isSetupComplete||door->parent!=home||door->isLocked())continue;
        point=door->getDoorPosInside();
        if(ou->navmesh->pathExists(guard->getMovement()->havokCharacter,point)==1)return true;
    }return false;
}
void deliveryReturn(CaravanDelivery::Parcel& p,Character* guard,bool failed){
    if(failed&&escort
#ifdef MERCENARIE_CARAVAN_SPEECH
        &&!caravanConversationBusy()
#endif
    )escort->sayALine(Loc::text("caravan.delivery.fallback"),true);
    if(guard)missionClearTravel(guard,"delivery return");
    CaravanDelivery::advance(p,CaravanDelivery::Returning);
}
bool deliveryExit(Character* guard,Ogre::Vector3& point){
    if(!guard||!guard->getMovement()||!guard->getMovement()->isIndoors())return false;
    Building* building=deliveryHome(guard->getMovement()->building.toString());
    if(!building||building->isDestroyed())return false;
    bool found=false;float best=3.4e38f;
    for(unsigned int i=0;i<building->doors.size();++i){DoorStuff* door=building->doors[i]?building->doors[i]->getDoor():0;
        if(!door||!door->isSetupComplete||door->parent!=building||door->isLocked())continue;
        // Preserve the door's native elevation and stairs. A projection onto
        // the exterior road can reject a perfectly usable house exit.
        Ogre::Vector3 outside=door->getDoorPosOutside();
        if(outside.squaredDistance(building->getPosition())>=10000.0f)continue;
        float score=guard->getPosition().squaredDistance(outside);
        if(ou&&ou->navmesh&&guard->getMovement()->havokCharacter&&ou->navmesh->pathExists(guard->getMovement()->havokCharacter,outside)==1)score-=1000000.0f;
        if(score<best){best=score;point=outside;found=true;}
    }
    // An advisory path query may miss an indoor/outdoor transition. The native
    // movement order can open an unlocked door and resolve the actual path.
    return found;
}
bool tickCaravanDelivery(float elapsed){
    CaravanDelivery::State s=CaravanDelivery::read(currentContract.routeRegions);
    if(s.scenario!=2&&s.scenario!=3)return true;
    if(!s.started&&!caravanHasDeliveryGuard()){s.scenario=1;CaravanDelivery::write(currentContract.routeRegions,s);return true;}
    bool purchased[4]={false,false,false,false};
    int clients=3;
#ifdef MERCENARIE_CARAVAN_VISIT
    CaravanVisit::Plan visit=CaravanVisit::read(currentContract.routeRegions);clients=visit.count;
#endif
    const std::vector<CaravanCustomers::Group>& groups=caravanCustomerSnapshot();
    for(size_t i=0;i<groups.size();++i)if(groups[i].id==currentMissionFiscalId&&groups[i].clients.size()==(size_t)clients){
        for(int j=0;j<clients;++j){s.parcels[j].home=groups[i].clients[j].home;purchased[j]=CaravanCustomers::purchased(groups[i].clients[j]);}
        break;
    }
#ifdef MERCENARIE_CARAVAN_VISIT
    if(visit.exists)for(int i=0;i<4;++i)if(!CaravanVisit::delivery(visit,i)&&s.parcels[i].stage==CaravanDelivery::Waiting)s.parcels[i].stage=CaravanDelivery::Done;
#endif
    if(!s.started){
        bool ready=false;for(int i=0;i<clients;++i)if(purchased[i]&&s.parcels[i].stage==CaravanDelivery::Waiting)ready=true;
        if(!ready)return false;
#ifdef MERCENARIE_CARAVAN_SPEECH
        if(caravanConversationBusy())return false;
#endif
        s.started=true;
#ifndef MERCENARIE_CARAVAN_SPEECH
        escort->sayALine(Loc::text("caravan.delivery.order"),true);
#endif
    }
    // Caller already gives combat, first aid and rescue priority over commerce.
    int travelling=0;for(int i=0;i<4;++i)if(CaravanDelivery::active(s.parcels[i]))++travelling;
    for(int i=0;i<4;++i){CaravanDelivery::Parcel& p=s.parcels[i];
        if(p.stage!=CaravanDelivery::Waiting||!purchased[i]||travelling>=2)continue;
#ifdef MERCENARIE_CARAVAN_SPEECH
        if(caravanConversationBusy())break;
#endif
        Character* guard=0;
        for(size_t j=0;j<progressMembers.size();++j){Character* candidate=progressMembers[j].getCharacter();
            if(candidate&&candidate!=escort&&!candidate->isAnimal()&&!candidate->isPlayerCharacter()&&!candidate->isCarryingSomething&&tradeActorAvailable(candidate)&&!CaravanDelivery::owns(s,candidate->getHandle().toString())){guard=candidate;break;}}
        if(!guard){if(travelling==0){p.stage=CaravanDelivery::Done;escort->sayALine(Loc::text("caravan.delivery.no_guard"),true);}continue;}
#ifdef MERCENARIE_CARAVAN_SPEECH
        if(p.retry==0){escort->sayALine(Loc::text("caravan.delivery.order"),true);p.retry=1;break;}
#endif
        p.actor=guard->getHandle().toString();CaravanDelivery::advance(p,CaravanDelivery::Approaching);++travelling;
        missionClearTravel(guard,"delivery assigned");guard->sayALine(Loc::text("caravan.delivery.accept"),true);
        DebugLog(std::string("CARAVAN DELIVERY assigned actor=")+p.actor+" home="+p.home+" id="+currentMissionFiscalId);
    }
    for(int i=0;i<4;++i){CaravanDelivery::Parcel& p=s.parcels[i];if(!CaravanDelivery::active(p))continue;
        Character* guard=deliveryGuard(p.actor);
        if(!guard){ // Streaming is not death; retain identity and wait.
            p.retry=std::max(0.0,p.retry-elapsed);
            if(p.retry<=0){p.retry=10;DebugLog(std::string("CARAVAN DELIVERY unresolved actor=")+p.actor+" id="+currentMissionFiscalId);}
            continue;
        }
        if(guard->isDead()||guard->isBeingCarried()){p.stage=CaravanDelivery::Done;continue;}
        if(!tradeActorAvailable(guard))continue;
        p.elapsed=std::min(86400.0,p.elapsed+elapsed);p.retry=std::max(0.0,p.retry-elapsed);
        guard->getMovement()->setDesiredSpeedOrders(WALK);
        Building* home=deliveryHome(p.home);
        bool inside=guard->getMovement()->isIndoors()&&guard->getMovement()->building.toString()==p.home;
        // Resume old in-flight deliveries through the exterior approach as well.
        if(p.stage==CaravanDelivery::Entering&&!inside&&home&&tradeDistance(guard->getPosition(),home->getPosition())>22500)
            CaravanDelivery::advance(p,CaravanDelivery::Approaching);
        if(p.stage==CaravanDelivery::Approaching||p.stage==CaravanDelivery::Entering){
            if(inside){missionClearTravel(guard,"delivery inside home");missionIssueOrder(guard,HOLD_POSITION,0,guard->getPosition());CaravanDelivery::advance(p,CaravanDelivery::Inside);DebugLog(std::string("CARAVAN DELIVERY entered actor=")+p.actor+" home="+p.home);}
            else if(p.stage==CaravanDelivery::Approaching){
                if(p.retry<=0){p.retry=5;DoorStuff* door=deliveryDoor(home);
                    if(!door){if(p.elapsed>=30)deliveryReturn(p,guard,true);}
                    else {Ogre::Vector3 outside=door->getDoorPosOutside();
                        double speed=std::max(.5f,guard->getMovement()->getStandardWalkSpeed());
                        double budget=std::max(180.0,std::min(1800.0,180.0+2*sqrt(tradeDistance(escort->getPosition(),outside))/speed));
                        if(tradeDistance(guard->getPosition(),outside)<=400){CaravanDelivery::advance(p,CaravanDelivery::Entering);}
                        else if(p.elapsed>budget)deliveryReturn(p,guard,true);
                        else {Ogre::Vector3 point;if(tradeExterior(guard,outside,point))deliveryMove(guard,point,"approach");
                            else DebugLog(std::string("CARAVAN DELIVERY approach path pending actor=")+p.actor+" home="+p.home);}
                    }
                }
            }
            else if(p.elapsed>=90){deliveryReturn(p,guard,true);}
            else if(p.retry<=0){Ogre::Vector3 point;p.retry=5;
                if(deliveryEntry(guard,home,point))deliveryMove(guard,point,"enter");
                else if(!home||home->isDestroyed()||p.elapsed>=20)deliveryReturn(p,guard,true);
            }
        }else if(p.stage==CaravanDelivery::Inside){
            if(!inside){CaravanDelivery::advance(p,CaravanDelivery::Entering);}
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
            else if(s.scenario==3
#ifdef MERCENARIE_CARAVAN_VISIT
                &&(!visit.exists||visit.trap==i)
#endif
                &&p.elapsed>=2&&!CaravanHomeAmbush::read(currentContract.routeRegions).attempted&&triggerCaravanHomeAmbush(guard,home)){
                // Stop all outstanding deliveries. Commercial regroup resumes
                // after combat and rescue; no missing client can block departure.
                for(int k=0;k<4;++k)if(s.parcels[k].stage==CaravanDelivery::Waiting)s.parcels[k].stage=CaravanDelivery::Done;
                CaravanDelivery::write(currentContract.routeRegions,s);return true;
            }
#endif
            else if(p.elapsed>=5){p.delivered=true;guard->sayALine(Loc::text("caravan.delivery.handed"),true);deliveryReturn(p,guard,false);}
            else if(!missionNativeTaskPresent(guard,HOLD_POSITION)){missionClearTravel(guard,"delivery hold");missionIssueOrder(guard,HOLD_POSITION,0,guard->getPosition());}
        }else if(p.stage==CaravanDelivery::Returning){
            if(!guard->getMovement()->isIndoors()&&tradeDistance(guard->getPosition(),escort->getPosition())<=400){
#ifdef MERCENARIE_CARAVAN_SPEECH
                if(caravanConversationBusy())continue;
#endif
                missionClearTravel(guard,"delivery rejoined");missionIssueOrder(guard,FOLLOW_PLAYER_ORDER,escort,escort->getPosition());p.stage=CaravanDelivery::Done;
                guard->sayALine(Loc::text(p.delivered?"caravan.delivery.returned":"caravan.delivery.failed_return"),true);
                DebugLog(std::string("CARAVAN DELIVERY rejoined actor=")+p.actor+" id="+currentMissionFiscalId);
            }else if(p.retry<=0){p.retry=5;Ogre::Vector3 point;
                // Exit first; project only exterior rendezvous points. Never teleport.
                if(guard->getMovement()->isIndoors()){
                    if(deliveryExit(guard,point)||findExteriorWaypoint(guard,point))deliveryMove(guard,point,"exit");
                    else DebugLog(std::string("CARAVAN DELIVERY exit unavailable actor=")+p.actor+" building="+guard->getMovement()->building.toString());
                }
                else if(tradeExterior(guard,escort->getPosition(),point))deliveryMove(guard,point,"return");
            }
        }
    }
    CaravanDelivery::write(currentContract.routeRegions,s);return CaravanDelivery::complete(s);
}
