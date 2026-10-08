#pragma once
#include "CaravanCustomerSpawn.h"
hand customerHandle(const std::string& id){hand h;h.setNull();if(!id.empty()){bool valid=true;int separators=0;for(size_t i=0;i<id.size();++i)if(id[i]=='-')++separators;else if(id[i]<'0'||id[i]>'9')valid=false;if(valid&&separators==4)h.fromString(id);}return h;}
bool customerProtected(Character* c){return c&&(c->isPlayerCharacter()||c->isBeingCarried()||c->isCarryingSomething||missionNativeSurvival(c)||missionCombatThreat(c)||missionNativeTaskPresent(c,FIRST_AID_ORDER)||missionNativeTaskPresent(c,LIFT_PERSON_PLAYER_ORDER)||missionNativeTaskPresent(c,PICKUP));}
bool customerSafe(Character* c){return c&&!c->isDead()&&!c->isBeingCarried()&&!c->isCarryingSomething&&rescueCanWalk(c)&&!missionNativeSurvival(c)&&!missionCombatThreat(c)&&!missionNativeTaskPresent(c,FIRST_AID_ORDER)&&!missionNativeTaskPresent(c,LIFT_PERSON_PLAYER_ORDER)&&!missionNativeTaskPresent(c,PICKUP);}
void customerClear(CaravanCustomers::Client& record,Character* c){
    if(!c||(record.order!=MOVE_CUS_ORDERED&&record.order!=HOLD_POSITION)||!c->getAI())return;AITaskSytem* tasks=c->getAI()->getTaskSystem();if(!tasks)return;
    Ogre::Vector3 point(record.x,record.y,record.z);
    for(size_t i=0;i<tasks->orders.list.size();){Tasker* task=tasks->orders.list[i];if(task&&task->key()==record.order&&task->subject.isNull()&&task->location.squaredDistance(point)<.01f){
        if(tasks->getCurrentGoal().key()==record.order&&tasks->getCurrentGoal().subject.isNull())tasks->clearCurrentGoal(true);
        tasks->orders.list.erase(tasks->orders.list.begin()+i);delete task;
    }else ++i;}record.order=-1;
}
void customerOrder(CaravanCustomers::Client& r,Character* c,TaskType type,const Ogre::Vector3& point){
    if(!customerSafe(c)||!c->getAI()||!c->getAI()->getTaskSystem())return;customerClear(r,c);hand none;none.setNull();
    c->getAI()->getTaskSystem()->addOrder(type,none,point,false,false);r.order=(int)type;r.x=point.x;r.y=point.y;r.z=point.z;
    c->getMovement()->setDesiredSpeedOrders(WALK);c->getMovement()->setDesiredSpeed(std::min(c->getStats()->getMaxRunSpeed(),c->getMovement()->getStandardWalkSpeed()));
}
const std::vector<CaravanCustomers::Group>& caravanCustomerSnapshot(){
    static std::string previous;static std::vector<CaravanCustomers::Group> cached;
    if(previous!=caravanCustomerState){cached=CaravanCustomers::load(caravanCustomerState);previous=caravanCustomerState;}return cached;
}
bool caravanCustomerOwned(Character* actor){
    if(!actor||actor->isPlayerCharacter())return false;std::string id=actor->getHandle().toString();const std::vector<CaravanCustomers::Group>& groups=caravanCustomerSnapshot();
    for(size_t i=0;i<groups.size();++i)for(size_t j=0;j<groups[i].clients.size();++j)if(groups[i].clients[j].actor==id)return true;return false;
}
bool caravanCustomerSpeed(AbstractMovementBase* movement,MoveSpeed& mode,float& limit){
    const std::vector<CaravanCustomers::Group>& groups=caravanCustomerSnapshot();
    for(size_t i=0;i<groups.size();++i)for(size_t j=0;j<groups[i].clients.size();++j){Character* c=customerHandle(groups[i].clients[j].actor).getCharacter();if(c&&c->getMovement()==movement&&customerSafe(c)){mode=groups[i].clients[j].stage==4?RUN:WALK;limit=mode==RUN?c->getStats()->getMaxRunSpeed():std::min(c->getStats()->getMaxRunSpeed(),c->getMovement()->getStandardWalkSpeed());return true;}}return false;
}
#include "CaravanAccompliceRuntime.h"
bool caravanConversationBusy(){
    if(caravanSpeaking(escort))return true;
    for(size_t i=0;i<progressMembers.size();++i)if(caravanSpeaking(progressMembers[i].getCharacter()))return true;
    const std::vector<CaravanCustomers::Group>& groups=caravanCustomerSnapshot();
    for(size_t i=0;i<groups.size();++i)if(groups[i].id==currentMissionFiscalId)
        for(size_t j=0;j<groups[i].clients.size();++j)if(caravanSpeaking(customerHandle(groups[i].clients[j].actor).getCharacter()))return true;
    return false;
}
bool customerInteriorPoint(Building* home,DoorStuff* door,Ogre::Vector3& point){
    // Spawn belongs to the resident's home, not the merchant's current path.
    // Native door anchors are available even when the interior navigation is not
    // in the global nearest-point query yet. Load physics without revealing graphics.
    if(!home||!door)return false;
    Ogre::Vector3 traceInside=door->getDoorPosInside(),traceOutside=door->getDoorPosOutside(),traceHome=home->getPosition();
    char anchorTrace[640];sprintf_s(anchorTrace,"CARAVAN CUSTOMER ANCHOR setup=%d parentMatches=%d inside=(%.3f,%.3f,%.3f) outside=(%.3f,%.3f,%.3f) homePos=(%.3f,%.3f,%.3f) separation2=%.3f",(int)door->isSetupComplete,(int)(door->parent==home),traceInside.x,traceInside.y,traceInside.z,traceOutside.x,traceOutside.y,traceOutside.z,traceHome.x,traceHome.y,traceHome.z,traceInside.squaredDistance(traceOutside));
    DebugLog(std::string(anchorTrace)+" home="+home->getHandle().toString());
    if(!door->isSetupComplete||door->parent!=home){DebugLog("CARAVAN CUSTOMER REJECT setup-or-parent");return false;}
    Ogre::Vector3 inside=door->getDoorPosInside(),outside=door->getDoorPosOutside();
    float separation=inside.squaredDistance(outside);
    // Door markers include approaches/stairs, not just the width of the opening.
    // Recorded native homes have squared separation 968..1156; a 400 cap rejects all.
    if(!(separation>.01f)){DebugLog("CARAVAN CUSTOMER REJECT coincident-door-anchors");return false;}
    if(!(inside.squaredDistance(home->getPosition())<10000.0f&&outside.squaredDistance(home->getPosition())<10000.0f)){DebugLog("CARAVAN CUSTOMER REJECT anchors-outside-home-range");return false;}
    // Preserve native floor height. Extrapolating the inside/outside vector also
    // extrapolates stair elevation and can place the resident above the floor.
    Ogre::Vector3 wanted=inside;
    if(!(wanted.squaredDistance(home->getPosition())<10000.0f)){DebugLog("CARAVAN CUSTOMER REJECT anchor-distance");return false;}
    home->loadInteriorPhysical(true);
    Ogre::Vector3 valid;unsigned int key=0;
    int projected=ou->navmesh->getClosestPoint(wanted,3.0f,.1f,false,valid,key);
    if(projected==1&&valid.squaredDistance(wanted)<=9&&ou->navmesh->isInterior(key)&&ou->navmesh->getHandle(key)==home->getHandle()){
        point=valid;return true;
    }
    // Do not replace an interior anchor with the exterior floor selected by a
    // global nearest-point query. The native factory also receives the home.
    point=wanted;
    char detail[256];sprintf_s(detail,"CARAVAN CUSTOMERS native home anchor projection=%d key=%u pos=(%.2f,%.2f,%.2f)",projected,key,point.x,point.y,point.z);
    DebugLog(std::string(detail)+" home="+home->getHandle().toString());return true;
}
void createCaravanCustomers(){
    Town* town=tradeTown();if(!town||town->townType!=TOWN_VILLAGE||!escort||!ou||!ou->zoneMgr||!ou->theFactory||!ou->navmesh)return;
    
    std::vector<CaravanCustomers::Group> groups=CaravanCustomers::load(caravanCustomerState);if(groups.size()>=256)return;
    CaravanCustomers::Group group;size_t groupIndex=groups.size();
    for(size_t i=0;i<groups.size();++i)if(groups[i].id==currentMissionFiscalId){group=groups[i];groupIndex=i;break;}
    const int wanted=CaravanVisit::read(currentContract.routeRegions).count;
    if(group.clients.size()>=(size_t)wanted){CaravanCustomerSpawn::complete(currentContract.routeRegions);return;}if(group.retiring)return;
    if(groupIndex==groups.size()&&CaravanCustomerSpawn::completed(currentContract.routeRegions))return;
    CaravanCustomerSpawn::attempt(currentContract.routeRegions);
    group.id=currentMissionFiscalId;group.town=currentContract.destinationId;group.merchant=escort->getHandle().toString();Ogre::Vector3 centre=town->getPosition();group.x=centre.x;group.y=centre.y;group.z=centre.z;
    lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,town,0,false,0,0);
    std::vector<Building*> homes;std::vector<Ogre::Vector3> points;
    int noInterior=0,visible=0,ownership=0,noDoor=0,noPoint=0;
    for(unsigned int i=0;i<buildings.size();++i){Building* b=buildings[i];
        bool occupied=false;if(b)for(size_t k=0;k<group.clients.size();++k)if(group.clients[k].home==b->getHandle().toString())occupied=true;if(occupied)continue;
        if(!b||b->isDestroyed()||!b->hasInterior()){++noInterior;continue;}
        if(b->interiorVisibility){++visible;continue;}
        if(!b->isPublic()&&b->getFaction()!=town->getFaction()){++ownership;continue;}
        bool usableDoor=false,found=false;
        for(unsigned int d=0;d<b->doors.size();++d){DoorStuff* door=b->doors[d]?b->doors[d]->getDoor():0;if(!door||door->isLocked())continue;
            usableDoor=true;Ogre::Vector3 valid;if(customerInteriorPoint(b,door,valid)){homes.push_back(b);points.push_back(valid);found=true;break;}
        }
        if(!usableDoor)++noDoor;else if(!found)++noPoint;
    }
    char diagnostic[512];sprintf_s(diagnostic,"CARAVAN CUSTOMERS scan attempt=%d buildings=%u eligible=%u noInterior=%d visible=%d ownership=%d noUnlockedDoor=%d invalidHomeAnchor=%d",CaravanCustomerSpawn::attempts(currentContract.routeRegions),(unsigned int)buildings.size(),(unsigned int)homes.size(),noInterior,visible,ownership,noDoor,noPoint);DebugLog(std::string(diagnostic));
    // A transient navigation/streaming failure remains pending for the next call.
    if(homes.empty())return;
    GameData* squad=ou->gamedata.getData("880101-Guild Escort Contracts.mod",SQUAD_TEMPLATE);Faction* faction=town->getFaction();if(!squad||!faction){DebugLog("CARAVAN CUSTOMERS missing template or faction");return;}
    int count=wanted-(int)group.clients.size();
    for(int i=0;i<count;++i){int pick=UtilityT::randomInt(0,(int)homes.size()-1);Building* home=homes[pick];Ogre::Vector3 point=points[pick];if(homes.size()>1){homes.erase(homes.begin()+pick);points.erase(points.begin()+pick);}
        Platoon* platoon=ou->theFactory->createRandomSquad(faction,point,town,1,home,squad,0,0,0,true,hand(),town,1.0f,SQ_ROAMING,false);
        if(!platoon||!platoon->activePlatoon)continue;platoon->setPersistentSquad(true);
        for(unsigned int j=0;j<platoon->activePlatoon->things.size();++j){Character* c=static_cast<Character*>(platoon->activePlatoon->things[j]);if(!c)continue;
            if(c->dialogue)c->dialogue->clearConversationList(EV_PLAYER_TALK_TO_ME);
            CaravanCustomers::Client client;client.actor=c->getHandle().toString();client.home=home->getHandle().toString();
            customerOrder(client,c,HOLD_POSITION,c->getPosition());group.clients.push_back(client);
        }
    }
    if(!group.clients.empty()){if(groupIndex<groups.size())groups[groupIndex]=group;else groups.push_back(group);caravanCustomerState=CaravanCustomers::save(groups);if(group.clients.size()==(size_t)wanted)CaravanCustomerSpawn::complete(currentContract.routeRegions);}
    sprintf_s(diagnostic,"CARAVAN CUSTOMERS spawned count=%u",(unsigned int)group.clients.size());DebugLog(std::string(diagnostic)+" id="+group.id);
}
void callCaravanCustomers(){
    createCaravanCustomers();
    std::vector<CaravanCustomers::Group> groups=CaravanCustomers::load(caravanCustomerState);
    for(size_t i=0;i<groups.size();++i)if(groups[i].id==currentMissionFiscalId){
        CaravanCustomers::Group& g=groups[i];
        if(currentContract.routeRegions.find(";RPSALE1;")==std::string::npos){
            // Migrate incomplete old timed interactions, retaining completed new purchases.
            for(size_t j=0;j<g.clients.size();++j)if(!CaravanCustomers::purchased(g.clients[j])){g.clients[j].stage=0;g.clients[j].line=0;g.clients[j].clock=0;g.clients[j].retry=0;}
            g.called=false;currentContract.routeRegions+=";RPSALE1;";
        }
        if(!g.called){g.called=true;g.busy=3600;g.merchant=escort->getHandle().toString();escort->sayALine(Loc::text("caravan.trade.cry.0"),true);DebugLog(std::string("CARAVAN SALES single call id=")+g.id);}
    }
    caravanCustomerState=CaravanCustomers::save(groups);
}
bool caravanCustomersComplete(){
    const std::vector<CaravanCustomers::Group>& groups=caravanCustomerSnapshot();for(size_t i=0;i<groups.size();++i)if(groups[i].id==currentMissionFiscalId)return groups[i].clients.size()==(size_t)CaravanVisit::read(currentContract.routeRegions).count&&CaravanCustomers::complete(groups[i]);return false;
}
bool caravanCustomersBusy(){return !caravanCustomersComplete();}
void endCaravanCustomers(){std::vector<CaravanCustomers::Group> groups=CaravanCustomers::load(caravanCustomerState);for(size_t i=0;i<groups.size();++i)if(groups[i].id==currentMissionFiscalId){groups[i].called=true;groups[i].busy=0;}caravanCustomerState=CaravanCustomers::save(groups);}
void tickCaravanCustomers(float dt){
    if(!ou||!ou->player||!ou->navmesh)return;std::vector<CaravanCustomers::Group> groups=CaravanCustomers::load(caravanCustomerState);if(groups.empty())return;
    for(size_t i=0;i<groups.size();++i){CaravanCustomers::Group& g=groups[i];Ogre::Vector3 centre(g.x,g.y,g.z);Character* merchant=customerHandle(g.merchant).getCharacter();bool playerNear=false,cameraNear=true,onScreen=false,protectedActor=false;
        for(unsigned int p=0;p<ou->player->playerCharacters.size();++p){Character* player=ou->player->playerCharacters[p];if(player&&!player->isDead()&&tradeDistance(player->getPosition(),centre)<2250000)playerNear=true;}
        if(ou->player->getCamera())cameraNear=tradeDistance(ou->player->getCamera()->getCameraPos(),centre)<2250000;
        for(size_t j=0;j<g.clients.size();++j){Character* c=customerHandle(g.clients[j].actor).getCharacter();if(c){onScreen=onScreen||c->isOnScreen;protectedActor=protectedActor||customerProtected(c);}}
        if(CaravanCustomers::unseen(playerNear,cameraNear,onScreen,protectedActor))g.retiring=true;
        // Escape is independent of merchant proximity and group retirement.
        for(size_t j=0;j<g.clients.size();){CaravanCustomers::Client& r=g.clients[j];
            if(r.stage!=4){++j;continue;}Character* c=customerHandle(r.actor).getCharacter();
            customerEscape(g,r,c,dt);
            if(customerEscapeUnseen(g,r,c)&&c->getFaction()){
                customerClear(r,c);DebugLog(std::string("CARAVAN ACCOMPLICE retired actor=")+r.actor);
                c->getFaction()->destroyObject(c);g.clients.erase(g.clients.begin()+j);
            }else ++j;
        }
        if(g.retiring){
            // Keep unloaded identities until they resolve. Delete before visibility,
            // never confuse an unloaded handle with a destroyed character.
            for(size_t j=0;j<g.clients.size();){Character* c=customerHandle(g.clients[j].actor).getCharacter();
                if(g.clients[j].stage!=4&&c&&!c->isOnScreen&&!customerProtected(c)&&c->getFaction()){
                    customerClear(g.clients[j],c);c->getFaction()->destroyObject(c);g.clients.erase(g.clients.begin()+j);
                }else ++j;
            }continue;
        }
        if(!playerNear)continue;
        if(!merchant||merchant->isDead()){g.called=true;g.busy=0;}
        int activeSpeaker=CaravanCustomers::speaker(g);
        for(size_t j=0;j<g.clients.size();++j){CaravanCustomers::Client& r=g.clients[j];Character* c=customerHandle(r.actor).getCharacter();if(!c||c->isPlayerCharacter()||r.stage==4)continue;
            if(!customerSafe(c)){customerClear(r,c);continue;}
            c->getMovement()->setDesiredSpeedOrders(WALK);r.retry=std::max(0.0f,r.retry-dt);
            if(r.stage==0){if(!g.called)continue;r.stage=1;r.retry=0;}
            if(r.stage<3&&(!merchant||merchant->isDead()||g.busy<=0)){r.stage=3;r.retry=0;}
            if(r.stage<3&&!customerSafe(merchant)){customerClear(r,c);continue;}
            if(r.stage==1){float angle=6.28318f*(float)j/(float)g.clients.size();Ogre::Vector3 wanted=merchant->getPosition()+Ogre::Vector3(Ogre::Math::Cos(angle)*3,0,Ogre::Math::Sin(angle)*3),point;
                if(!c->getMovement()->isIndoors()&&tradeDistance(c->getPosition(),merchant->getPosition())<=900){r.stage=2;r.clock=0;DebugLog(std::string("CARAVAN SALES client arrived actor=")+r.actor);customerOrder(r,c,HOLD_POSITION,c->getPosition());}
                else if(r.retry<=0){r.retry=5;if(tradeExterior(c,wanted,point))customerOrder(r,c,MOVE_CUS_ORDERED,point);}
            }
            if(r.stage==2){
                if(tradeDistance(c->getPosition(),merchant->getPosition())>1600||c->getMovement()->isIndoors()){r.stage=1;r.retry=0;continue;}
                if((int)j!=activeSpeaker)continue;
                if(r.line==0&&caravanDeliveryInstructionPending(g.id))continue;
                bool speaking=caravanCrewSpeaking(g.id)||caravanSpeaking(merchant);
                for(size_t k=0;k<g.clients.size();++k)if(caravanSpeaking(customerHandle(g.clients[k].actor).getCharacter()))speaking=true;
                if(!caravanSpeechReady(r.clock,speaking,dt))continue;
                if(r.clock<=0){
                    if(r.line>=9){r.stage=3;r.retry=0;DebugLog(std::string("CARAVAN SALES purchase complete actor=")+r.actor);}
                    else {char key[80];int variant=caravanNegotiationVariant(g.id,(int)j);sprintf_s(key,"caravan.negotiation.%d.%d",variant,r.line);
#ifdef MERCENARIE_CARAVAN_DELIVERY
                        if(caravanDeliveryFor(g.id,(int)j)&&r.line>=6)
                            sprintf_s(key,"caravan.negotiation.%d.delivery.%d",variant,r.line);
#endif
                        if(r.line%2)merchant->sayALine(Loc::text(key),true);else c->sayALine(Loc::text(key),true);
                        DebugLog(std::string("CARAVAN SALES line ")+key+" actor="+r.actor);++r.line;r.clock=.3f;
                    }
                }
            }
            if(r.stage==3&&r.retry<=0){r.retry=20;Ogre::Vector3 point;for(int attempt=0;attempt<8;++attempt){float a=UtilityT::random(0.0f,6.28318f),rad=UtilityT::random(15.0f,65.0f);if(tradeExterior(c,centre+Ogre::Vector3(Ogre::Math::Cos(a)*rad,0,Ogre::Math::Sin(a)*rad),point)){customerOrder(r,c,MOVE_CUS_ORDERED,point);break;}}}
        }
    }
    for(size_t i=0;i<groups.size();)if(groups[i].clients.empty())groups.erase(groups.begin()+i);else ++i;
    caravanCustomerState=CaravanCustomers::save(groups);
}
