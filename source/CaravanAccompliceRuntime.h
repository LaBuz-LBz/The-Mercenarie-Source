#pragma once
// Stage 4 belongs exclusively to the client of the trapped house. The existing
// archived clock/retry/order fields retain the escape across save/load.
void customerEscape(CaravanCustomers::Group& g,CaravanCustomers::Client& r,Character* c,float dt){
    if(!c||!c->getAI()||!c->getAI()->getTaskSystem()||!c->getStats()||c->isDead()||c->isPlayerCharacter()||c->isBeingCarried()||c->isCarryingSomething||!rescueCanWalk(c))return;
    if(missionNativeTaskPresent(c,FIRST_AID_ORDER)||missionNativeTaskPresent(c,LIFT_PERSON_PLAYER_ORDER))return;
    r.clock=std::min(3600.0f,r.clock+dt);r.retry=std::max(0.0f,r.retry-dt);
    c->getMovement()->setDesiredSpeedOrders(RUN);c->getMovement()->setDesiredSpeed(c->getStats()->getMaxRunSpeed());
    if(r.retry>0)return;r.retry=5;
    Ogre::Vector3 origin(g.x,g.y,g.z),away=c->getPosition()-origin;away.y=0;
    if(away.squaredLength()<1)away=Ogre::Vector3(1,0,0);away.normalise();
    Ogre::Vector3 point;bool found=false;
    for(int i=0;i<7&&!found;++i){float a=(i%2?1.0f:-1.0f)*((i+1)/2)*.35f;
        Ogre::Vector3 direction(away.x*Ogre::Math::Cos(a)-away.z*Ogre::Math::Sin(a),0,away.x*Ogre::Math::Sin(a)+away.z*Ogre::Math::Cos(a));
        found=tradeExterior(c,c->getPosition()+direction*400.0f,point)&&tradeDistance(point,origin)>tradeDistance(c->getPosition(),origin)+2500;
    }
    if(!found)return;
    if(missionNativeTaskPresent(c,MOVE_CUS_ORDERED)&&!c->getMovement()->pathFailed()&&tradeDistance(c->getPosition(),Ogre::Vector3(r.x,r.y,r.z))>1600)return;
    customerClear(r,c);hand none;none.setNull();
    c->getAI()->getTaskSystem()->addOrder(MOVE_CUS_ORDERED,none,point,false,false);
    r.order=MOVE_CUS_ORDERED;r.x=point.x;r.y=point.y;r.z=point.z;
}
bool customerEscapeUnseen(const CaravanCustomers::Group& g,const CaravanCustomers::Client& r,Character* c){
    if(!c||r.clock<30||customerProtected(c)||c->isDead()||c->isOnScreen||!ou->player->getCamera())return false;
    if(tradeDistance(c->getPosition(),Ogre::Vector3(g.x,g.y,g.z))<2250000)return false;
    if(tradeDistance(c->getPosition(),ou->player->getCamera()->getCameraPos())<2250000)return false;
    for(unsigned int i=0;i<ou->player->playerCharacters.size();++i){Character* p=ou->player->playerCharacters[i];if(p&&tradeDistance(p->getPosition(),c->getPosition())<2250000)return false;}
    return true;
}
void fleeCaravanAccomplice(Building* home){
    if(!home)return;std::vector<CaravanCustomers::Group> groups=CaravanCustomers::load(caravanCustomerState);
    for(size_t i=0;i<groups.size();++i)if(groups[i].id==currentMissionFiscalId){
        CaravanCustomers::Group& g=groups[i];
        for(size_t j=0;j<g.clients.size();++j){CaravanCustomers::Client& r=g.clients[j];
            if(r.home!=home->getHandle().toString()||r.stage==4)continue;
#ifdef MERCENARIE_CARAVAN_VISIT
            CaravanVisit::Plan visit=CaravanVisit::read(currentContract.routeRegions);if(visit.exists&&visit.trap!=(int)j)continue;
#endif
            Character* actor=customerHandle(r.actor).getCharacter();customerClear(r,actor);
            r.stage=4;r.clock=0;r.retry=0;
            // Publish RUN before the native speed hook consults the snapshot.
            caravanCustomerState=CaravanCustomers::save(groups);
            customerEscape(g,r,actor,0);
            DebugLog(std::string("CARAVAN ACCOMPLICE fleeing actor=")+r.actor+" home="+r.home);
            break;
        }
    }
    caravanCustomerState=CaravanCustomers::save(groups);
}
