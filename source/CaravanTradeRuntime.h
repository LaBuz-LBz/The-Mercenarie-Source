#pragma once
bool caravanTradeActive(){return caravanMission&&!caravanReturning&&CaravanTrade::active(CaravanTrade::read(currentContract.routeRegions));}
bool tradeActorAvailable(Character* c){
    if(!c||c->isDead()||c->isBeingCarried()||!rescueCanWalk(c)||missionCombatThreat(c)||missionNativeSurvival(c))return false;
    if(missionNativeTaskPresent(c,FIRST_AID_ORDER)||missionNativeTaskPresent(c,LIFT_PERSON_PLAYER_ORDER)||missionNativeTaskPresent(c,PICKUP))return false;
    for(size_t i=0;i<missionRescue.tasks.size();++i)if(missionRescue.tasks[i].helper.getCharacter()==c)return false;
    return true;
}
bool tradePartyAvailable(){
    if(missionCasualtyWaiting||!missionRescue.tasks.empty()||missionRescue.safeSeconds<2||!tradeActorAvailable(escort))return false;
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();
        if(!c){if(!progressDeadMembers.count((unsigned int)i))return false;continue;}
        if(c->isDead())continue;
        if(c->isBeingCarried()){if(!missionGroupCarrierOf(c))return false;continue;}
        if(!tradeActorAvailable(c))return false;
    }return true;
}
void suspendCaravanTrade(){
    if(!caravanTradeActive()||tradePartyAvailable())return;
    // Only retire our locomotion; treatment, carrying and native combat remain untouched.
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
        if(caravanHomeAmbushFleeing(c))continue;
#endif
        if(c&&tradeActorAvailable(c)&&missionNativeTaskPresent(c,MOVE_CUS_ORDERED))missionClearTravel(c,"commerce interrupted");}
}
float tradeDistance(const Ogre::Vector3& a,const Ogre::Vector3& b){float x=a.x-b.x,z=a.z-b.z;return x*x+z*z;}
bool tradeMove(Character* c,const Ogre::Vector3& point){
    if(!tradeActorAvailable(c))return false;
    missionClearTravel(c,"caravan commerce move");missionIssueOrder(c,MOVE_CUS_ORDERED,0,point);return true;
}
bool tradeExterior(Character* c,const Ogre::Vector3& wanted,Ogre::Vector3& point){
    return c&&c->getMovement()&&c->getMovement()->havokCharacter&&missionProjectExteriorRoadPoint(wanted,point)
        &&tradeDistance(wanted,point)<900&&ou&&ou->navmesh&&ou->navmesh->pathExists(c->getMovement()->havokCharacter,point)==1;
}
Town* tradeTown(){return shou&&shou->townList?shou->townList->getTownBySID(currentContract.destinationId):0;}
bool tradeShopPoint(Building* b,Ogre::Vector3& point){
    if(!b||b->isDestroyed()||!b->isAShop()||!b->isPublic())return false;
    for(unsigned int i=0;i<b->doors.size();++i){DoorStuff* door=b->doors[i]?b->doors[i]->getDoor():0;
        if(!door||door->isLocked()||!door->isOpen())continue;
        point=door->getDoorPosInside();
        if(escort&&escort->getMovement()->havokCharacter&&ou->navmesh->pathExists(escort->getMovement()->havokCharacter,point)==1)return true;
    }return false;
}
Building* tradeFindShop(const std::string& id){
    Town* t=tradeTown();if(!t||!ou||!ou->zoneMgr||id=="-")return 0;
    lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,t,0,false,0,0);
    for(unsigned int i=0;i<buildings.size();++i)if(buildings[i]&&buildings[i]->getHandle().toString()==id)return buildings[i];return 0;
}
Character* tradeShopSeller(Building* shop){
    // Resolve the actual resident trader each tick; never retain a native pointer.
    Character* seller=shop?shop->getResidentSquadLeader():0;
    if(!seller||seller==escort||seller->isPlayerCharacter()||!seller->isATrader()||seller->isDead()||!seller->getMovement())return 0;
    if(!seller->getMovement()->isIndoors()||seller->getMovement()->building.toString()!=shop->getHandle().toString())return 0;
    return seller;
}
bool tradeShopVisited(const CaravanTrade::State& s,const std::string& id){return s.visited.find(","+id+",")!=std::string::npos;}
void tradeShopAbandon(CaravanTrade::State& s){
    DebugLog(std::string("CARAVAN SHOP unavailable, not counted shop=")+s.shop);
    s.previous=s.shop;s.shop="-";s.seller="-";s.shopLine=0;s.sayClock=0;s.step=3;s.moveClock=0;
    missionClearTravel(escort,"commerce seller unavailable");
}
bool tradeShopConversation(CaravanTrade::State& s,float elapsed){
    Building* shop=tradeFindShop(s.shop);Character* seller=tradeShopSeller(shop);
    if(!seller){if(s.attempt>=45)tradeShopAbandon(s);return false;}
    if(!tradeActorAvailable(seller)||seller->isCarryingSomething){s.attempt=0;return false;}
    std::string id=seller->getHandle().toString();
    if(s.seller!=id){s.seller=id;s.shopLine=0;s.sayClock=0;}
    float height=escort->getPosition().y-seller->getPosition().y;
    if(tradeDistance(escort->getPosition(),seller->getPosition())>400||height>10||height< -10){
        if(s.attempt>=45){tradeShopAbandon(s);return false;}
        if(s.moveClock<=0){
            if(escort->getMovement()->havokCharacter&&ou&&ou->navmesh&&ou->navmesh->pathExists(escort->getMovement()->havokCharacter,seller->getPosition())==1)tradeMove(escort,seller->getPosition());
            s.moveClock=5;
        }return false;
    }
    s.attempt=0;
    if(!missionNativeTaskPresent(escort,HOLD_POSITION)){missionClearTravel(escort,"commerce negotiating");missionIssueOrder(escort,HOLD_POSITION,0,escort->getPosition());}
    // The shopkeeper keeps all native orders, including medicine and combat.
#ifdef MERCENARIE_CARAVAN_SPEECH
    if(!caravanSpeechReady(s.sayClock,caravanSpeaking(escort)||caravanSpeaking(seller),elapsed))return false;
#else
    s.sayClock-=elapsed;if(s.sayClock>0)return false;
#endif
    if(s.shopLine>=10)return true;
    char key[80];sprintf_s(key,"caravan.shop.sale.%d.%d",std::min(2,std::max(0,s.lastPhrase)),s.shopLine);
    Character* speaker=(s.shopLine%2)?seller:escort;speaker->sayALine(Loc::text(key),true);
    DebugLog(std::string("CARAVAN SHOP dialogue ")+key+" seller="+id+" shop="+s.shop);
    ++s.shopLine;s.sayClock=.3f;return false;
}
bool tradeChooseShop(CaravanTrade::State& s){
    Town* t=tradeTown();if(!t||t->townType!=TOWN_TOWN||!ou||!ou->zoneMgr||!ou->navmesh)return false;
    lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,t,0,false,0,0);
    std::vector<Building*> choices;std::vector<Ogre::Vector3> points;
    for(unsigned int i=0;i<buildings.size();++i){Ogre::Vector3 p;Building* b=buildings[i];
        if(b&&!tradeShopVisited(s,b->getHandle().toString())&&b->getHandle().toString()!=s.previous&&tradeShopPoint(b,p)&&tradeShopSeller(b)){choices.push_back(b);points.push_back(p);}}
    if(choices.empty()){s.previous="-";return false;}int n=UtilityT::randomInt(0,(int)choices.size()-1);
    s.shop=choices[n]->getHandle().toString();s.seller="-";s.shopLine=0;s.sayClock=0;s.step=1;s.attempt=0;s.moveClock=0;s.x=points[n].x;s.y=points[n].y;s.z=points[n].z;
    return tradeMove(escort,points[n]);
}
bool tradeWander(CaravanTrade::State& s){
    Town* t=tradeTown();Ogre::Vector3 centre=t?t->getPosition():destination,point;
    for(int i=0;i<8;++i){float a=UtilityT::random(0.0f,6.28318f),r=UtilityT::random(10.0f,65.0f);
        Ogre::Vector3 wanted=centre+Ogre::Vector3(Ogre::Math::Cos(a)*r,0,Ogre::Math::Sin(a)*r);
        if(tradeExterior(escort,wanted,point)){s.step=4;s.attempt=0;s.moveClock=20;s.x=point.x;s.y=point.y;s.z=point.z;return tradeMove(escort,point);}}
    s.step=0;s.moveClock=5;return false;
}
void tradeFollowers(float elapsed,bool regroup){
    // A local shop visit must not consume the road retry budget or suspend the
    // contract when a pack animal cannot follow through a shop doorway.
    if(!tradePartyAvailable())return;
#ifdef MERCENARIE_CARAVAN_FORMATION
    // Walking to the exterior rendezvous is already caravan travel. Use the
    // same native slots, but never take ownership from a home delivery.
    bool deliveryBusy=false;
#ifdef MERCENARIE_CARAVAN_DELIVERY
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(c&&caravanDeliveryOwns(c)){deliveryBusy=true;break;}}
#endif
    if(regroup&&!deliveryBusy&&missionGroup.caravan.tick(escort,progressMembers,true,elapsed,false))return;
    missionGroup.caravan.engaged=false;
#endif
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();
        if(c==escort||!tradeActorAvailable(c))continue;
#ifdef MERCENARIE_CARAVAN_DELIVERY
        if(caravanDeliveryOwns(c))continue;
#endif
        if(!missionFollowOrderPresent(c)){
            missionClearTravel(c,"commerce accompany merchant");
            missionIssueOrder(c,FOLLOW_PLAYER_ORDER,escort,escort->getPosition());
        }
    }
}
bool tradeRegroup(CaravanTrade::State& s){
    // FOLLOW may be satisfied farther away than our departure radius. Give
    // stragglers a reachable rendezvous instead of waiting on that same order.
    bool ready=true,refresh=s.moveClock<=0;if(refresh)s.moveClock=10;
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();
        if(!c){if(!progressDeadMembers.count((unsigned int)i))ready=false;continue;}
        if(c==escort||c->isDead()||(c->isBeingCarried()&&missionGroupCarrierOf(c)))continue;
        if(!tradeActorAvailable(c)){ready=false;continue;}
        float distance=tradeDistance(c->getPosition(),escort->getPosition());
        if(c->getMovement()->isIndoors()||distance>6400){
            ready=false;if(!refresh)continue;
            float angle=6.28318f*(float)i/(float)progressMembers.size();
            Ogre::Vector3 wanted=escort->getPosition()+Ogre::Vector3(Ogre::Math::Cos(angle)*8,0,Ogre::Math::Sin(angle)*8),point;
            bool reachable=false;
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
            if(c->getMovement()->isIndoors()&&caravanDeliveryOwns(c))reachable=deliveryExit(c,point);
#endif
            if(reachable||tradeExterior(c,wanted,point)){
                missionClearTravel(c,"commerce regroup approach");missionIssueOrder(c,MOVE_CUS_ORDERED,0,point);
            }else DebugLog(std::string("CARAVAN TRADE regroup path unavailable actor=")+c->getHandle().toString());
            char detail[128];sprintf_s(detail,"CARAVAN TRADE regroup waiting distance2=%.2f indoors=%d",distance,(int)c->getMovement()->isIndoors());DebugLog(std::string(detail)+" actor="+c->getHandle().toString());
        }else if(!missionFollowOrderPresent(c)){
            missionClearTravel(c,"commerce regroup reached");missionIssueOrder(c,FOLLOW_PLAYER_ORDER,escort,escort->getPosition());
        }
    }return ready;
}
bool tradeExit(CaravanTrade::State& s){
    Ogre::Vector3 point;
    if(!findExteriorWaypoint(escort,point))return false;
    s.step=3;s.attempt=0;s.moveClock=10;s.x=point.x;s.y=point.y;s.z=point.z;return tradeMove(escort,point);
}
// Phase 2 steps 5/6 persist the exterior rendezvous in the existing x/y/z fields.
bool tradeChooseRendezvous(CaravanTrade::State& s){
    Town* town=tradeTown();if(!town){s.step=6;s.moveClock=0;DebugLog("CARAVAN TRADE town unavailable: use return road id="+currentMissionFiscalId);return true;}
    Ogre::Vector3 centre=town->getPosition(),point;
    float radius=std::max(40.0f,town->getRadius());
    Ogre::Vector3 preferred=town->getPositionOutsideTownGates(100.0f);
    // The native gate exit can be outside the walls while still inside the
    // town's administrative radius. Do not reject it on that radius alone.
    bool found=tradeDistance(preferred,centre)>1600&&tradeExterior(escort,preferred,point);
    // Ungated villages and invalid gate hints: check reachable points around the
    // town boundary, starting towards home. Never use an unverified coordinate.
    float angle=atan2(caravanOrigin.z-centre.z,caravanOrigin.x-centre.x);
    for(int i=0;!found&&i<16;++i){
        float a=angle+(i%2?1.0f:-1.0f)*(float)((i+1)/2)*.392699f;
        Ogre::Vector3 wanted=centre+Ogre::Vector3(Ogre::Math::Cos(a)*(radius+100),0,Ogre::Math::Sin(a)*(radius+100));
        found=tradeExterior(escort,wanted,point)&&tradeDistance(point,centre)>(radius+40)*(radius+40);
    }
    if(!found){
        s.dwell+=1;
        if(s.dwell>=3){
            // Let the established road navigator handle the departure instead
            // of repeating the same failed local probes forever.
            s.step=6;s.moveClock=0;s.x=escort->getPosition().x;s.y=escort->getPosition().y;s.z=escort->getPosition().z;
            DebugLog("CARAVAN TRADE exterior search exhausted: use return road id="+currentMissionFiscalId);return true;
        }
        Ogre::Vector3 gate=town->getPositionOutsideTownGates(18.0f);
        if(tradeDistance(gate,escort->getPosition())>400&&tradeExterior(escort,gate,point)){
            s.x=point.x;s.y=point.y;s.z=point.z;s.step=7;s.moveClock=0;s.attempt=0;
            DebugLog("CARAVAN TRADE staged gate exit id="+currentMissionFiscalId);return true;
        }
        s.moveClock=10;DebugLog("CARAVAN TRADE exterior rendezvous unavailable id="+currentMissionFiscalId);return false;
    }
    s.x=point.x;s.y=point.y;s.z=point.z;s.step=5;s.moveClock=0;s.attempt=0;
    missionGroup.reset();DebugLog("CARAVAN TRADE exterior rendezvous selected id="+currentMissionFiscalId);return true;
}
bool tradeReachRendezvous(CaravanTrade::State& s,float elapsed){
    if(s.step!=5&&s.step!=6&&s.step!=7){
        if(s.moveClock>0||!tradeChooseRendezvous(s)){tradeFollowers(elapsed,true);return false;}
    }
    Ogre::Vector3 point((float)s.x,(float)s.y,(float)s.z);
    if(s.step==6)return true;
    // Preserve medical/combat priority (tradePartyAvailable), and prevent the
    // merchant from abandoning a genuinely distant companion while exiting.
    bool distant=false;
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();
        if(c&&c!=escort&&!c->isDead()&&!c->isBeingCarried()&&tradeDistance(c->getPosition(),escort->getPosition())>160000)distant=true;
    }
    if(distant){missionHoldTravel(escort,"town exit wait for companion");tradeRegroup(s);return false;}
    if(tradeDistance(escort->getPosition(),point)<=400){
        if(s.step==7){s.step=0;s.moveClock=0;missionClearTravel(escort,"gate reached: select exterior regroup");tradeFollowers(elapsed,true);return false;}
        s.step=6;s.moveClock=0;missionClearTravel(escort,"exterior rendezvous reached");return true;
    }
    if(s.moveClock<=0){
        if(escort->getMovement()->pathFailed()||s.attempt>90){
            s.step=0;s.moveClock=0;missionClearTravel(escort,"reselect exterior rendezvous");
            if(!tradeChooseRendezvous(s))return false;
            if(s.step==6)return true;
            point=Ogre::Vector3((float)s.x,(float)s.y,(float)s.z);
        }
        tradeMove(escort,point);s.moveClock=10;
    }
    tradeFollowers(elapsed,true);return false;
}
void beginCaravanTrade(){
    CaravanTrade::State s;CaravanTrade::begin(s,currentGameHours);s.lastPhrase=0;CaravanTrade::write(currentContract.routeRegions,s);
    currentContract.routeRegions+=";SALEFLOW=1;";
    clearMissionFollow();missionGroup.reset();
    missionRescue.passage.clear();missionRescue.localRecovery=false;missionRescue.leaderWatch=MissionMotionPolicy::Watch();
    for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(tradeActorAvailable(c))missionClearTravel(c,"caravan delivery commerce");}
#ifdef MERCENARIE_CARAVAN_DELIVERY
    Town* arrivalTown=tradeTown();
    if(arrivalTown&&arrivalTown->townType==TOWN_VILLAGE){
#ifdef MERCENARIE_CARAVAN_VISIT
        initializeCaravanVisit();
#else
        CaravanDelivery::State choice;choice.scenario=0;CaravanDelivery::write(currentContract.routeRegions,choice);
#endif
}
#endif
#ifdef MERCENARIE_CARAVAN_CUSTOMERS
#ifdef MERCENARIE_CARAVAN_DELIVERY
    if(CaravanDelivery::read(currentContract.routeRegions).scenario!=0)
#endif
    createCaravanCustomers();
#endif
    DebugLog(std::string("CARAVAN TRADE begin id=")+currentMissionFiscalId);
}
bool tickCaravanTrade(float elapsed){
    if(!caravanTradeActive())return false;
    CaravanTrade::State s=CaravanTrade::read(currentContract.routeRegions);
    if(currentContract.routeRegions.find(";SALEFLOW=1;")==std::string::npos){
        currentContract.routeRegions+=";SALEFLOW=1;";s.phase=1;s.step=0;s.lastPhrase=0;s.moveClock=0;s.attempt=0;
    }
#ifdef MERCENARIE_CARAVAN_CUSTOMERS
    if(s.phase==2)endCaravanCustomers();
#endif
    if(missionPaused||!tradePartyAvailable()){CaravanTrade::write(currentContract.routeRegions,s);return true;}
#ifdef MERCENARIE_CARAVAN_DELIVERY
    if(CaravanDelivery::read(currentContract.routeRegions).scenario==0){
        if(!missionNativeTaskPresent(escort,HOLD_POSITION)){missionClearTravel(escort,"scenario choice pending");missionIssueOrder(escort,HOLD_POSITION,0,escort->getPosition());}
#ifdef MERCENARIE_CARAVAN_VISIT
        initializeCaravanVisit();
#else
        tradeFollowers(elapsed,false);showCaravanScenarioPicker();return true;
#endif
    }
#endif
    applyMissionPace();s.moveClock-=elapsed;s.attempt+=elapsed;
    Town* town=tradeTown();bool village=town&&town->townType==TOWN_VILLAGE;
    if(s.phase==1&&village){
        // Reach the village selling point before the one and only call.
        if(s.step!=1&&s.step!=2){Ogre::Vector3 point;
            if(tradeExterior(escort,town->getPosition(),point)){s.x=point.x;s.y=point.y;s.z=point.z;s.step=1;s.moveClock=0;}}
        if(s.step==1){Ogre::Vector3 point((float)s.x,(float)s.y,(float)s.z);
            if(!escort->getMovement()->isIndoors()&&tradeDistance(escort->getPosition(),point)<=400){s.step=2;s.attempt=10;missionClearTravel(escort,"commerce selling point reached");}
            else if(s.moveClock<=0){tradeMove(escort,point);s.moveClock=10;}}
        if(s.step==2){
#ifdef MERCENARIE_CARAVAN_CUSTOMERS
            if(s.attempt>=10){callCaravanCustomers();s.attempt=0;}
            bool deliveriesComplete=true;
#ifdef MERCENARIE_CARAVAN_DELIVERY
            deliveriesComplete=tickCaravanDelivery(elapsed);
#endif
            bool ambushed=false;
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
            ambushed=CaravanHomeAmbush::read(currentContract.routeRegions).triggered;
#endif
            if(ambushed||(caravanCustomersComplete()&&deliveriesComplete)){
                s.phase=2;s.step=0;s.moveClock=0;endCaravanCustomers();
            }
#endif
        }
        if(s.phase==1){
            if(s.step==2&&!missionNativeTaskPresent(escort,HOLD_POSITION)){missionClearTravel(escort,"commerce serving clients");missionIssueOrder(escort,HOLD_POSITION,0,escort->getPosition());}
            tradeFollowers(elapsed,false);CaravanTrade::write(currentContract.routeRegions,s);return true;
        }
    }
    if(s.phase==2){
#ifdef MERCENARIE_CARAVAN_SPEECH
        if(caravanSpeaking(escort)){CaravanTrade::write(currentContract.routeRegions,s);return true;}
#endif
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
        if(CaravanHomeAmbush::read(currentContract.routeRegions).remaining>0||!tradePartyAvailable()){CaravanTrade::write(currentContract.routeRegions,s);return true;}
#endif
        if(escort->getMovement()->isIndoors()){
            if(s.step!=3||s.moveClock<=0)tradeExit(s);
            tradeFollowers(elapsed,true);
        }else{
            if(!tradeReachRendezvous(s,elapsed)){CaravanTrade::write(currentContract.routeRegions,s);return true;}
            if(!missionNativeTaskPresent(escort,HOLD_POSITION)){missionClearTravel(escort,"commerce regroup hold");missionIssueOrder(escort,HOLD_POSITION,0,escort->getPosition());}
            bool ready=tradeRegroup(s);
            if(ready){s.phase=3;CaravanTrade::write(currentContract.routeRegions,s);caravanReturning=true;destination=caravanOrigin;
                journeyStart=escort->getPosition();journeyDistanceSquared=journeyStart.squaredDistance(destination);quarterSpeech=midpointChecked=finalSpeech=false;
                missionRescue.regrouping=false;missionRescue.regroupSeconds=0;missionRescue.leaderWatch=MissionMotionPolicy::Watch();
                missionRescue.roadPoints.clear();missionRescue.roadNext=0;missionRescue.roadPlanned=false;missionGroup.reset();
                applyMissionPace();issueTravelOrder(destination,"caravan commerce complete");escort->sayALine(Loc::text("ui.the_goods_are_delivered_we_are_now_heading_back"),true);
                DebugLog(std::string("CARAVAN TRADE return id=")+currentMissionFiscalId);return true;}
        }
    }else if(s.step==1){
        Building* shop=tradeFindShop(s.shop);Ogre::Vector3 p;
        if(escort->getMovement()->isIndoors()&&escort->getMovement()->building.toString()==s.shop){
            s.step=2;s.attempt=0;s.moveClock=0;missionClearTravel(escort,"commerce shop reached");
        }else if(!tradeShopPoint(shop,p)||s.attempt>45||escort->getMovement()->pathFailed()){
            s.previous=s.shop;s.shop="-";s.step=0;s.moveClock=0;missionClearTravel(escort,"commerce shop inaccessible");
        }else if(s.moveClock<=0){tradeMove(escort,Ogre::Vector3((float)s.x,(float)s.y,(float)s.z));s.moveClock=10;}
    }else if(s.step==2){
        if(!escort->getMovement()->isIndoors()||escort->getMovement()->building.toString()!=s.shop){s.step=1;s.attempt=0;s.moveClock=0;}
        else {if(tradeShopConversation(s,elapsed)){s.lastPhrase=std::max(0,s.lastPhrase)+1;s.visited+=","+s.shop+",";s.previous=s.shop;s.shop="-";s.seller="-";s.shopLine=0;
            if(s.lastPhrase>=3){s.phase=2;s.step=0;s.moveClock=0;}else {s.step=3;s.moveClock=0;tradeExit(s);}
            DebugLog(std::string("CARAVAN TRADE shop agreement completed id=")+currentMissionFiscalId);}}
    }else if(s.step==3){
        if(!escort->getMovement()->isIndoors()){s.step=0;s.moveClock=0;}
        else if(s.moveClock<=0)tradeExit(s);
    }else if(s.moveClock<=0){
        if(escort->getMovement()->isIndoors())tradeExit(s);
        else if(!tradeChooseShop(s))tradeWander(s);
    }
    if(s.phase==1)tradeFollowers(elapsed,false);
    CaravanTrade::write(currentContract.routeRegions,s);return true;
}
#ifndef CARAVAN_TRADE_TEST
void updateCaravanTradeHUD(){
#ifdef MERCENARIE_CARAVAN_DELIVERY
    maintainCaravanScenarioPicker();
#endif
    MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();if(!gui)return;
    MyGUI::TextBox* label=gui->findWidget<MyGUI::TextBox>("MercenarieCaravanTradeTimer",false);
    if(label)label->setVisible(false); // Remove the legacy countdown after loading old saves.

}
#endif
