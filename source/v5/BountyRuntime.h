#include "../Localization.h"
#pragma once
#include "BountyNative.h"
#include "BountySave.h"
#include <kenshi/Building/UseableStuff.h>
#include <kenshi/SensoryData.h>
#include "NavMeshCompat.h"
#include <kenshi/gui/MapScreen.h>
namespace {
GuildHistory::Snapshot bountySnapshot(const MercenarieV5::BountyContract& c);
void archiveBounty(const MercenarieV5::BountyContract& c,int status);

MercenarieV5::BountyWorldState bountyWorld;
MyGUI::Window* bountyWindow=0;
MyGUI::Window* bountyReportWindow=0;
std::string bountyDifficultyName(const MercenarieV5::BountyOffer& offer){
    int d=MercenarieV5::bountyDifficulty(offer);
    return d==0?registerLanguage(Loc::text("ui.easy"),Loc::text("ui.easy")):d==1?registerLanguage(Loc::text("ui.normal"),Loc::text("ui.normal")):registerLanguage(Loc::text("ui.hard"),Loc::text("ui.hard"));
}
MyGUI::TextBox* bountyOfferText=0;
hand bountyViewingIssuer;
int bountySelection=0;
MyGUI::Button* bountyAccept=0;
MyGUI::ImageBox* bountyMap=0;
MyGUI::TextBox* bountyZone=0;
MyGUI::ImageBox* bountyPlaceIcon=0;
MyGUI::TextBox* bountyName=0;
MyGUI::TextBox* bountyFacts=0;
MyGUI::TextBox* bountyRowNames[3]={0,0,0};
MyGUI::TextBox* bountyRowAmounts[3]={0,0,0};
MyGUI::Widget* bountyRowMarks[3]={0,0,0};
std::vector<MyGUI::Widget*> bountyRing;
void updateBountyDossier();
void selectBountyDossier(MyGUI::Widget* button);
MyGUI::Button* bountyAcceptButtons[3]={0,0,0};
std::vector<MercenarieV5::BountyOffer> bountyDisplayedOffers;
MyGUI::Button* bountyTrackerEntry=0;
const char* bountyTargetFactionId="200-gamedata.base";
std::string bountyTargetFactionName(const MercenarieV5::BountyOffer& offer){
    Faction* faction=0;
    if(bountyWorld.contract.offer.id==offer.id&&bountyWorld.contract.target.valid()){
        Character* target=MercenarieV5::bountyHandle(bountyWorld.contract.target).getCharacter();
        if(target)faction=target->getFaction();
    }
    // Before spawning or during streaming, use the very same faction as the factory.
    if(!faction&&ou&&ou->factionMgr)faction=ou->factionMgr->getFactionByStringID(bountyTargetFactionId);
    return faction?faction->name:registerLanguage(Loc::text("ui.unavailable_c049803"),Loc::text("ui.unavailable_c049803"));
}
void retireBountyCamp(){
    for(size_t i=0;i<bountyWorld.campObjects.size();++i)bountyWorld.campCleanup.push_back(bountyWorld.campObjects[i]);
    bountyWorld.campObjects.clear();
}
bool isContractBountyPlatoon(Platoon* platoon){
    return platoon&&!bountyWorld.suspended&&bountyWorld.contract.occupied()&&bountyWorld.contract.target.valid()&&
        MercenarieV5::bountyIdentity(platoon->getSquadLeader_theRealOne())==bountyWorld.contract.target;
}
void recoverExistingBountyPlatoon(){
    if(!ou||!ou->factionMgr||!ou->player||!ou->navmesh||!ou->zoneMgr||!bountyWorld.contract.occupied()||bountyWorld.suspended)return;
    Faction* faction=ou->factionMgr->getFactionByStringID(bountyTargetFactionId);if(!faction)return;
    const lektor<Platoon*>* lists[]={faction->getActivePlatoons(),faction->getUnloadedPlatoons()};
    Platoon* found=0;
    for(int list=0;list<2&&!found;++list)if(lists[list])for(unsigned int i=0;i<lists[list]->size();++i)
        if(isContractBountyPlatoon((*lists[list])[i])){found=(*lists[list])[i];break;}
    std::ofstream diagnostic("mods/Guild Escort Contracts/bounty-streaming.log",std::ios::trunc);
    diagnostic<<"contract="<<bountyWorld.contract.offer.id<<"\nexactPlatoonFound="<<(found?1:0)<<"\n";
    if(!found)return; // Missing does not authorize creation of a replacement target.
    diagnostic<<"persistent="<<found->isPersistentSquad()<<"\nactive="<<(found->activePlatoon?1:0)<<"\nunloaded="<<(found->unloadedPlatoon?1:0)<<"\ndead="<<found->isDead<<"\n";
    found->setPersistentSquad(true);
    if(found->isDead||found->activePlatoon||!found->unloadedPlatoon)return;
    const Ogre::Vector3 position=found->getPosition();
    diagnostic<<"position="<<position.x<<","<<position.y<<","<<position.z<<"\n";
    if(!ou->navmesh->isLoaded(ou->zoneMgr->getMapSector(position)))return;
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){
        Character* player=ou->player->playerCharacters[i];if(!player||player->isDead())continue;
        Ogre::Vector3 delta=player->getPosition()-position;delta.y=0;
        if(delta.squaredLength()<1000000.0f){found->activate();return;}
    }
}
bool bountyOutsideSettlements(const Ogre::Vector3& point,float margin);
std::set<std::string> bountyCampVisualAttempts;
void updateBountyCamp(){
    if(!ou||!ou->theFactory||!shou||!shou->townList)return;
    if(!bountyWorld.contract.occupied())retireBountyCamp();
    for(size_t i=0;i<bountyWorld.campCleanup.size();){
        Building* object=MercenarieV5::bountyHandle(bountyWorld.campCleanup[i]).getBuilding();
        if(!object||!object->getFaction()){++i;continue;}
        object->getFaction()->destroyObject(object);
        bountyWorld.campCleanup.erase(bountyWorld.campCleanup.begin()+i);
    }
    // Repair the existing objects, not new copies, when their zone is loaded.
    if(bountyWorld.contract.state==MercenarieV5::BountyActive&&bountyWorld.encounter!=2){
        for(size_t i=0;i<bountyWorld.campObjects.size();++i){
            hand id=MercenarieV5::bountyHandle(bountyWorld.campObjects[i]);
            Building* object=id.getBuilding();
            if(!object||!ou->zoneMgr||!ou->navmesh)continue;
            if(!ou->navmesh->isLoaded(ou->zoneMgr->getMapSector(object->getPosition())))continue;
            if(!bountyCampVisualAttempts.insert(id.toString()).second)continue;
            if(!object->isPhysical())object->createPhysical();
            object->setVisible(true);
        }
    }
    if(bountyWorld.encounter==2||bountyWorld.campStarted||bountyWorld.contract.state!=MercenarieV5::BountyActive||!ou->navmesh)return;
    const MercenarieV5::BountyContract& c=bountyWorld.contract;
    Character* leader=MercenarieV5::bountyHandle(c.target).getCharacter();
    if(!leader||!leader->getFaction())return;
    Ogre::Vector3 origin(c.offer.x,c.offer.y,c.offer.z),ground=origin;
    ou->navmesh->getClosestPoint(origin,300.0f,1.0f,false,ground);
    if(!ou->navmesh->getPositionValid(ground)||!bountyOutsideSettlements(ground,600))return;
    origin=ground;
    const Ogre::Vector3 offsets[]={Ogre::Vector3(0,0,0),Ogre::Vector3(5,0,3),Ogre::Vector3(-5,0,3),Ogre::Vector3(0,0,10)};
    Ogre::Vector3 positions[4];
    for(int i=0;i<4;++i){
        positions[i]=origin+offsets[i];ou->navmesh->getClosestPoint(origin+offsets[i],8.0f,1.0f,false,positions[i]);
        if(!ou->navmesh->getPositionValid(positions[i]))return;
        positions[i].y=UtilityT::getTerrainHeight(positions[i].x,positions[i].z);
    }
    GameData* fire=ou->gamedata.getData("16848-nodes_otto1.mod",BUILDING);
    GameData* bed=ou->gamedata.getData("16845-nodes_otto1.mod",BUILDING);
    GameData* tent=ou->gamedata.getData("2614-Cannibals.mod",BUILDING);
    if(!fire||!bed||!tent)return;
    // One creation attempt, including partial creation; never duplicate on streaming.
    bountyWorld.campStarted=true;
    bountyWorld.contract.offer.x=origin.x;bountyWorld.contract.offer.y=origin.y;bountyWorld.contract.offer.z=origin.z;
    for(int i=0;i<4;++i){
        Building* object=ou->theFactory->createBuilding(i==0?fire:i==3?tent:bed,positions[i],shou->townList->getNullTown(),leader->getFaction(),Ogre::Quaternion::IDENTITY,0,0,0,0,0,false,true,false,0,true);
        if(object){object->setVisible(true);bountyWorld.campObjects.push_back(MercenarieV5::bountyIdentity(object->getHandle()));}
    }
}
void bountyCampMarkerClicked(MyGUI::Widget* sender){
    if(questPanelLocked())return;
    if(sender){std::string name=sender->getName();if(!name.empty()&&name[name.size()-1]>='0'&&name[name.size()-1]<='4')selectTrackerQuest(name[name.size()-1]-'0',true);}
    if(!bountyWorld.contract.occupied()||!bountyWorld.contract.target.valid()||bountyWorld.suspended||!ou||!ou->player)return;
    const MercenarieV5::BountyOffer& o=bountyWorld.contract.offer;
    ou->player->getCamera()->teleport(Ogre::Vector3(o.x,o.y,o.z));
}
float bountySearchRadius(const MercenarieV5::BountyOffer& offer,Town* area){
    Ogre::Vector3 delta=Ogre::Vector3(offer.x,offer.y,offer.z)-area->getPosition();delta.y=0;
    return (1+MercenarieV5::bountyDifficulty(offer))*2.5f*std::max(5000.0f,(float)ceil((std::max(100.0f,area->getRadius())+3500.0f)/500.0f)*500.0f);
}
void updateBountyCampMap(MapScreen* map){
    if(!map||!map->mapImage)return;
    // Search area is public from acceptance; its centre is the named settlement,
    // never the secret camp. Round the radius up so the camp stays inside it.
    MyGUI::Widget* search=map->mapImage->findWidget((std::string("MercenarieBountySearchArea")+registerNumber(bountyQuestSlot())));
    const bool searching=bountyWorld.contract.occupied()&&!bountyWorld.suspended;
    if(!search&&searching){
        search=map->mapImage->createWidget<MyGUI::Widget>("",0,0,map->mapImage->getWidth(),map->mapImage->getHeight(),MyGUI::Align::Stretch,(std::string("MercenarieBountySearchArea")+registerNumber(bountyQuestSlot())));
        search->setNeedMouseFocus(false);
        for(int i=0;i<96;++i){
            std::stringstream name;name<<"BountySearchDot"<<i;
            MyGUI::Widget* dot=search->createWidget<MyGUI::Widget>("WhiteSkin",0,0,3,3,MyGUI::Align::Default,name.str());
            dot->setColour(registerAmber);dot->setNeedMouseFocus(false);
        }
        MyGUI::TextBox* label=search->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",0,0,300,28,MyGUI::Align::Default,"BountySearchLabel");
        label->setFontHeight(16);label->setTextColour(registerAmber);label->setNeedMouseFocus(false);
    }
    if(search){
        Town* area=shou&&shou->townList?shou->townList->getTownBySID(bountyWorld.contract.offer.areaId):0;
        search->setVisible(searching&&area);
        if(searching&&area){
            const MercenarieV5::BountyOffer& offer=bountyWorld.contract.offer;
            Ogre::Vector3 centre=area->getPosition(),delta=Ogre::Vector3(offer.x,offer.y,offer.z)-centre;delta.y=0;
            float radius=bountySearchRadius(offer,area);
            for(int i=0;i<96;++i){
                double angle=i*6.28318530718/96;
                MyGUI::IntPoint p=map->worldToMapCoords(centre+Ogre::Vector3((float)cos(angle)*radius,0,(float)sin(angle)*radius));
                std::stringstream name;name<<"BountySearchDot"<<i;
                if(MyGUI::Widget* dot=search->findWidget(name.str()))dot->setPosition(p.left-1,p.top-1);
            }
            MyGUI::IntPoint top=map->worldToMapCoords(centre+Ogre::Vector3(0,0,radius));
            MyGUI::TextBox* label=static_cast<MyGUI::TextBox*>(search->findWidget("BountySearchLabel"));
            label->setPosition(top.left-150,top.top-30);
            MercenarieFonts::caption(label,registerLanguage(Loc::text("ui.search_area_d7e8c51"),Loc::text("ui.search_area_d7e8c51"))+area->getName());
        }
    }
    MyGUI::Widget* marker=map->mapImage->findWidget((std::string("MercenarieBountyCampMarker")+registerNumber(bountyQuestSlot())));
    const bool visible=bountyWorld.contract.occupied()&&bountyWorld.contract.target.valid()&&!bountyWorld.suspended;
    if(!visible){if(marker)marker->setVisible(false);return;}
    if(!marker){
        MyGUI::Button* button=map->mapImage->createWidget<MyGUI::Button>("Kenshi_Button1",0,0,190,32,MyGUI::Align::Default,(std::string("MercenarieBountyCampMarker")+registerNumber(bountyQuestSlot())));
        button->eventMouseButtonClick+=MyGUI::newDelegate(bountyCampMarkerClicked);
        button->setTextColour(registerAmber);button->setFontHeight(14);marker=button;
    }
    const MercenarieV5::BountyOffer& o=bountyWorld.contract.offer;
    MyGUI::IntPoint position=map->worldToMapCoords(Ogre::Vector3(o.x,o.y,o.z));
    marker->setPosition(position.left-95,position.top-16);marker->setVisible(true);
    static_cast<MyGUI::Button*>(marker)->setCaption((bountyWorld.encounter==2?registerLanguage(Loc::text("ui.last_known"),Loc::text("ui.last_known")):registerLanguage(Loc::text("ui.camp"),Loc::text("ui.camp")))+o.targetName);
}
void bountyTrackerClicked(MyGUI::Widget*){
    if(!bountyWorld.contract.target.valid()||!bountyWorld.contract.occupied()||bountyWorld.suspended||!ou||!ou->player)return;
    hand id=MercenarieV5::bountyHandle(bountyWorld.contract.target);
    Character* target=id.getCharacter();if(!target||target->isDead()){bountyCampMarkerClicked(0);return;}
    ou->player->getCamera()->teleport(target->getPosition());
    ou->player->getCamera()->followObject(id);
}
void appendBountyTrackerItem(){
    if(!bountyWorld.contract.occupied())return;
    const MercenarieV5::BountyContract& c=bountyWorld.contract;
    QuestTrackerItem item;item.type=3;item.objective=registerLanguage(Loc::text("ui.target"),Loc::text("ui.target"))+c.offer.targetName;
    Town* trackerArea=shou&&shou->townList?shou->townList->getTownBySID(c.offer.areaId):0;
    if(trackerArea)item.location=mercenarieLocalize(frenchPlaceName(trackerArea->getName()));
    item.state=bountyWorld.suspended?registerLanguage(Loc::text("ui.waiting"),Loc::text("ui.waiting")):c.state==MercenarieV5::BountyHandingOver?registerLanguage(Loc::text("ui.handing_over_target"),Loc::text("ui.handing_over_target")):bountyWorld.targetSpotted?registerLanguage(Loc::text("ui.capture_target_return_alive"),Loc::text("ui.capture_target_return_alive")):registerLanguage(Loc::text("ui.search_for_target"),Loc::text("ui.search_for_target"));
    if(bountyWorld.targetSpotted){Character* target=MercenarieV5::bountyHandle(c.target).getCharacter();if(target){float squared=0;if(nearestPlayer(target->getPosition(),squared))item.distance=QuestTrackerText::distance(Ogre::Math::Sqrt(squared),gMercenarieEnglish);}}
    item.click=bountyTrackerClicked;item.clickable=c.target.valid()&&!bountyWorld.suspended;questTrackerItems.push_back(item);
}
void updateBountyTrackerUI(){}
// Conservative exclusion disk includes the settlement radius and a safety margin.
bool bountyOutsideSettlements(const Ogre::Vector3& point,float margin){
    if(!shou||!shou->townList)return false;
    lektor<RootObject*>& towns=shou->townList->getAllTowns();
    for(unsigned int i=0;i<towns.size();++i){
        Town* town=dynamic_cast<Town*>(towns[i]);if(!town||!town->getFaction())continue;
        Ogre::Vector3 delta=point-town->getPosition();delta.y=0;
        float radius=std::max(100.0f,town->getRadius())+margin;
        if(delta.squaredLength()<radius*radius)return false;
    }
    return true;
}
bool bountySafeAnchor(Town* town,Ogre::Vector3& point){
    if(!town)return false;
    const Ogre::Vector3 origin=town->getPosition();
    for(int ring=0;ring<6;++ring)for(int direction=0;direction<16;++direction){
        double angle=direction*6.28318530718/16;
        float radius=std::max(100.0f,town->getRadius())+1200+ring*300;
        Ogre::Vector3 candidate=origin+Ogre::Vector3((float)cos(angle)*radius,0,(float)sin(angle)*radius);
        if(bountyOutsideSettlements(candidate,800)){point=candidate;return true;}
    }
    return false;
}
bool bountySafeSegment(const Ogre::Vector3& from,const Ogre::Vector3& to){
    for(int sample=0;sample<=12;++sample)
        if(!bountyOutsideSettlements(from+(to-from)*(sample/12.0f),350))return false;
    return true;
}
void keepBountyGroupOutside(){
    MercenarieV5::BountyContract& c=bountyWorld.contract;
    if(c.state!=MercenarieV5::BountyActive||bountyWorld.encounter==2)return;
    // Do not send the supporting guards back to camp during a group fight.
    for(size_t i=0;i<c.group.size();++i){
        Character* member=MercenarieV5::bountyHandle(c.group[i]).getCharacter();
        if(member&&!member->isDead()&&!member->isBeingCarried()&&member->isInCombatMode(true,true))return;
    }
    Ogre::Vector3 anchor(c.offer.x,c.offer.y,c.offer.z);
    if(!bountyWorld.campStarted&&!bountyOutsideSettlements(anchor,600)){
        Town* town=shou&&shou->townList?shou->townList->getTownBySID(c.offer.areaId):0;
        if(!bountySafeAnchor(town,anchor))return;
        c.offer.x=anchor.x;c.offer.y=anchor.y;c.offer.z=anchor.z;
    }
    static unsigned int patrolStep=0;++patrolStep;
    for(size_t i=0;i<c.group.size();++i){
        Character* actor=MercenarieV5::bountyHandle(c.group[i]).getCharacter();
        if(!actor||actor->isDead()||actor->isBeingCarried())continue;
        AI* brain=actor->getAI();
        if(!brain||brain->iAmImprisoned(actor->getHandle(),actor->getPosition())||!brain->targetIsConscious(actor->getHandle(),actor->getPosition()))continue;
        // Never move a captive being carried by the player or interfere with custody.
        bool unsafe=!bountyOutsideSettlements(actor->getPosition(),450);
        if(actor->isInCombatMode(true,true))continue;
        const bool nearby=actor->getPosition().squaredDistance(anchor)<6400.0f;
        if(!unsafe&&nearby&&(patrolStep%6!=0||actor->isInCombatMode(true,true)))continue;
        Ogre::Vector3 destination=anchor;
        if(nearby&&!unsafe){
            double angle=(patrolStep/6+i)*2.399963;
            destination+=Ogre::Vector3((float)cos(angle)*25,0,(float)sin(angle)*25);
        }
        // Use short exterior steps; never issue a cross-town destination.
        Ogre::Vector3 from=actor->getPosition();
        if(!bountySafeSegment(from,destination)){
            bool found=false;float best=1e30f;
            for(int direction=0;direction<16;++direction){
                double angle=direction*6.28318530718/16;
                Ogre::Vector3 candidate=from+Ogre::Vector3((float)cos(angle)*600,0,(float)sin(angle)*600);
                if(!bountyOutsideSettlements(candidate,500))continue;
                if(!unsafe&&!bountySafeSegment(from,candidate))continue;
                float score=candidate.squaredDistance(anchor);
                if(score<best){best=score;destination=candidate;found=true;}
            }
            if(!found)continue;
        }
        actor->removeJob(WANDER_TOWN);actor->removeJob(WANDERER);actor->removeJob(PATROL);
        actor->removeJob(FOLLOW_SQUADLEADER);actor->removeJob(HOLD_POSITION);
        actor->removeJob(MOVE_CUS_ORDERED);
        if(AI* ai=actor->getAI()){hand none;ai->setCenterOfMovementTarget(none);ai->setCenterOfMovement(anchor);ai->setManuveringFreedomLevel(AI::HOLD_GROUND);}
        actor->addJob(MOVE_CUS_ORDERED,0,false,false,destination);
        actor->getMovement()->setRoadPreference(0.0f);
        actor->getMovement()->setRoadDestination(destination);
    }
}
bool chooseBountyArea(MercenarieV5::BountyOffer& offer,Character* giver){
    if(!giver||!shou||!shou->townList)return false;
    std::vector<Town*> nearby;
    lektor<RootObject*>& towns=shou->townList->getAllTowns();
    for(unsigned int i=0;i<towns.size();++i){
        Town* town=dynamic_cast<Town*>(towns[i]);if(!town||!town->getGameData()||town->getName().empty()||invalidContractDestination(town->getGameData()->stringID))continue;
        if(ou&&ou->player&&town->getFaction()==ou->player->getFaction())continue;
        float d=town->getPosition().squaredDistance(giver->getPosition());
        if(d>=9000000.0f&&d<=900000000.0f)nearby.push_back(town);
    }
    if(nearby.empty())return false;
    Town* town=nearby[UtilityT::randomInt(0,(int)nearby.size()-1)];
    Ogre::Vector3 point;if(!bountySafeAnchor(town,point))return false;
    offer.areaId=town->getGameData()->stringID;offer.x=point.x;offer.y=point.y;offer.z=point.z;
    const char* first[]={"Rovan","Karg","Sera","Dren","Vesk","Tora","Harn","Zek"};
    const char* last[]={"Tal","Vorn","Kesh","Ren","Soth","Var","Nesh","Dar"};
    offer.targetName=std::string(first[UtilityT::randomInt(0,7)])+" "+last[UtilityT::randomInt(0,7)];
    return true;
}
void acceptBountyOffer(MyGUI::Widget* button){
    if(!GuildProgression::bountyUnlocked(guildLevel()))return;
    if(!questCapacityAvailable())return;
    if(bountyWorld.suspended||!ou||!ou->theFactory||!ou->factionMgr)return;
    if(bountyWorld.campCleanup.size()>250){ou->showPlayerAMessage(registerLanguage(Loc::text("ui.previous_camp_cleanup_is_pending"),Loc::text("ui.previous_camp_cleanup_is_pending")),true);return;}
    Character* giver=bountyViewingIssuer.isNull()?0:bountyViewingIssuer.getCharacter();
    MercenarieV5::IssuerFaction faction;if(!MercenarieV5::bountyIssuer(giver,faction))return;
    if(bountyWorld.contract.occupied()){giver->sayALine(MercenarieV5::bountyText(MercenarieV5::AlreadyHunting,!gMercenarieEnglish),true);return;}
    int selected=button==bountyAccept?bountySelection:-1;
    if(selected<0||selected>=(int)bountyDisplayedOffers.size())return;
    MercenarieV5::BountyOffer chosen=bountyDisplayedOffers[selected];
    if(missionBookDelegationContext){delegationOfferIdentity=chosen.id;delegationIssuerIdentity=bountyViewingIssuer.toString();beginDelegationSelection(2,selected);if(bountyWindow)bountyWindow->setVisible(false);return;}
    Town* searchArea=shou&&shou->townList?shou->townList->getTownBySID(chosen.areaId):0;
    if(!searchArea)return;
    // Global settlement data is available at acceptance; terrain is checked locally
    // on approach. This secret point is fixed even when its terrain is unloaded.
    bool siteFound=false;
    for(int trial=0;trial<96&&!siteFound;++trial){
        double angle=UtilityT::randomInt(0,3599)*6.28318530718/3600;
        float r=bountySearchRadius(chosen,searchArea)*(0.2f+UtilityT::randomInt(0,700)/1000.0f);
        Ogre::Vector3 candidate=searchArea->getPosition()+Ogre::Vector3((float)cos(angle)*r,0,(float)sin(angle)*r);
        if(!bountyOutsideSettlements(candidate,800))continue;
        chosen.x=candidate.x;chosen.y=candidate.y;chosen.z=candidate.z;siteFound=true;
    }
    if(!siteFound)return;
    Ogre::Vector3 spawnPoint(chosen.x,chosen.y,chosen.z);
    if(!bountyOutsideSettlements(spawnPoint,800)){
        Town* area=shou&&shou->townList?shou->townList->getTownBySID(chosen.areaId):0;
        if(!bountySafeAnchor(area,spawnPoint)){ou->showPlayerAMessage(registerLanguage(Loc::text("ui.no_safe_exterior_area_for_this_contract"),Loc::text("ui.no_safe_exterior_area_for_this_contract")),true);return;}
        chosen.x=spawnPoint.x;chosen.y=spawnPoint.y;chosen.z=spawnPoint.z;
    }
    std::map<std::string,MercenarieV5::BountyBoard>::iterator entry=bountyWorld.boards.find(bountyViewingIssuer.toString());
    if(entry==bountyWorld.boards.end()||entry->second.refreshDue(currentGameHours))return;
    bool found=false;for(size_t i=0;i<entry->second.offers.size();++i)if(entry->second.offers[i].id==chosen.id)found=true;
    if(!found)return;
    ContractRewards::freeze(chosen.id,contractRewardPercent);
    std::stringstream sid;sid<<(881600+chosen.guards)<<"-Guild Escort Contracts.mod";
    GameData* squad=ou->gamedata.getData(sid.str(),SQUAD_TEMPLATE);
    Faction* bandits=ou->factionMgr->getFactionByStringID(bountyTargetFactionId);
    if(!squad||!bandits||!bountyWorld.contract.accept(chosen))return;
    {GuildHistory::Snapshot seed=bountySnapshot(bountyWorld.contract);seed.status=0;seed.accepted=currentGameHours;Character* giver=bountyViewingIssuer.getCharacter();if(giver)seed.giverName=giver->getName();contractSeeds[seed.id]=seed;saveReputations();}
    entry->second.consume(chosen.id,currentGameHours);
    bountyWorld.targetSpotted=false;
    retireBountyCamp();bountyWorld.campStarted=false;
    bountyWorld.searchSeconds=0;bountyWorld.searchAttempts=0;
    bountyWorld.encounter=0;bountyWorld.ambushAnnounced=false;
    bountyWorld.missionSeconds=0;bountyWorld.secretChosen=true;
    bountyWorld.secretX=chosen.x;bountyWorld.secretY=chosen.y;bountyWorld.secretZ=chosen.z;
    updateTrackerUI();
    bountyWindow->setVisible(false);
    ou->showPlayerAMessage(Loc::text("ui.bounty_contract_accepted_bring_the_target_back_alive_to"),true);
}
struct BountyRandom {unsigned int operator()(){return (unsigned int)UtilityT::randomInt(0,2147483646);}};
void closeBountyWindow(MyGUI::Window* window,const std::string&){missionBookDelegationContext=false;if(window)window->setVisible(false);}
void closeBountyReport(MyGUI::Widget*){if(bountyReportWindow)bountyReportWindow->setVisible(false);}
void showBountyReport(const std::string& text){
    if(!MyGUI::Gui::getInstancePtr())return;
    if(bountyReportWindow)MyGUI::Gui::getInstance().destroyWidget(bountyReportWindow);
    const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
    int w=std::min(850,view.width-30),h=std::min(600,view.height-30);
    bountyReportWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","MercenarieBountyReport");
    MercenarieFonts::caption(bountyReportWindow,registerLanguage(Loc::text("ui.the_mercenarie_mission_report"),Loc::text("ui.the_mercenarie_mission_report")));
    bountyReportWindow->eventWindowButtonPressed+=MyGUI::newDelegate(closeBountyWindow);
    MyGUI::Widget* client=bountyReportWindow->getClientWidget();applyMercenarieFrame(client,true);
    MyGUI::TextBox* body=registerText(client,24,20,client->getWidth()-48,client->getHeight()-96,18,text,registerIvory);
    body->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    MyGUI::Button* close=client->createWidget<MyGUI::Button>("Kenshi_Button1",20,client->getHeight()-60,client->getWidth()-40,40,MyGUI::Align::Default);
    MercenarieFonts::caption(close,registerLanguage(Loc::text("ui.close_report"),Loc::text("ui.close_report")));close->setTextColour(registerAmber);
    close->eventMouseButtonClick+=MyGUI::newDelegate(closeBountyReport);
}
void releaseBountyWindow(){
    mercenarieDestroyLiveWidget(bountyReportWindow);
    bountyReportWindow=0;
    bountyCampVisualAttempts.clear();
    mercenarieDestroyLiveWidget(bountyWindow);
    bountyRing.clear();bountyName=0;bountyFacts=0;
    bountyWindow=0;bountyOfferText=0;bountyMap=0;bountyZone=0;bountyAccept=0;bountyViewingIssuer.setNull();bountyDisplayedOffers.clear();
    for(int i=0;i<3;++i)bountyAcceptButtons[i]=0;
}
MercenarieV5::BountyBoard& synchronizeBountySource(Character* giver,MercenarieV5::IssuerFaction faction){
    const hand issuer=giver->getHandle();const std::string key=issuer.toString();
    MercenarieV5::BountyBoard& board=bountyWorld.boards[key];
    bool oldPrices=false;for(size_t i=0;i<board.offers.size();++i)if(!MercenarieV5::revisedBounty(board.offers[i]))oldPrices=true;
    if(oldPrices)board.nextRefreshHour=0;
    BountyRandom random;board.refresh(currentGameHours,key,MercenarieV5::bountyIdentity(issuer),faction,random);
    for(size_t i=0;i<board.offers.size();++i)if(board.offers[i].areaId.empty())chooseBountyArea(board.offers[i],giver);
    return board;
}
void openBountyOffers(Character* giver){
    if(!GuildProgression::bountyUnlocked(guildLevel())){if(ou)ou->showPlayerAMessage(Loc::text("guild.bounty_locked"),true);return;}
    // Mission Management only reads/delegates the issuer's existing board.
    // Its visible contracts window must not block the shared SECURITY pool.
    if(MissionOfferViewRules::needsQuestPreparation(missionBookDelegationContext)&&!prepareBountyQuest())return;
    MercenarieV5::IssuerFaction faction;
    if(!MercenarieV5::bountyIssuer(giver,faction)||bountyWorld.suspended)return;
    if(bountyWorld.contract.occupied()){
        giver->sayALine(MercenarieV5::bountyText(MercenarieV5::AlreadyHunting,!gMercenarieEnglish),true);return;
    }
    bountyViewingIssuer=giver->getHandle();
    const std::string key=bountyViewingIssuer.toString();
    MercenarieV5::BountyBoard& board=synchronizeBountySource(giver,faction);
    bountyDisplayedOffers=board.offers;
    if(!MyGUI::Gui::getInstancePtr())return;
    if(!bountyWindow){
        const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();
        int w=std::min(1400,view.width-30),h=std::min(920,view.height-30);
        bountyWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","MercenarieBountyBoard");
        bountyWindow->eventWindowButtonPressed+=MyGUI::newDelegate(closeBountyWindow);
        MyGUI::Widget* client=bountyWindow->getClientWidget();applyMercenarieFrame(client,true);
        const int cw=client->getWidth(),ch=client->getHeight(),pad=16,gap=12;
        const int left=(cw-pad*2-gap)/2,right=pad+left+gap,bodyH=ch-94;
        const int listH=bodyH*30/100,rowH=(listH-32)/3;
        MyGUI::Widget* list=registerPanel(client,pad,pad,left,listH);
        for(int i=0;i<3;++i){
            MyGUI::Button* row=list->createWidget<MyGUI::Button>("Kenshi_Button1",12,12+i*(rowH+4),left-24,rowH,MyGUI::Align::Default);
            bountyAcceptButtons[i]=row;row->eventMouseButtonClick+=MyGUI::newDelegate(selectBountyDossier);
            bountyRowMarks[i]=registerSolid(row,2,2,4,rowH-4,registerAmber);
            bountyRowNames[i]=registerText(row,16,7,(left-56)*58/100,rowH-12,22,"",registerIvory);
            bountyRowAmounts[i]=registerText(row,(left-56)*58/100+16,7,(left-56)*42/100,rowH-12,22,"",registerIvory);
            bountyRowAmounts[i]->setTextAlign(MyGUI::Align::Right|MyGUI::Align::VCenter);
        }
        int dossierH=bodyH-listH-gap;
        MyGUI::Widget* dossier=registerPanel(client,pad,pad+listH+gap,left,dossierH);
        int fs=dossierH<420?16:20;
        registerText(dossier,20,12,left-40,26,fs,registerLanguage(Loc::text("ui.dossier"),Loc::text("ui.dossier")),registerIvory);
        registerLine(dossier,20,42,left-40);
        bountyName=registerText(dossier,20,50,left-40,40,fs+10,"",registerIvory);
        bountyFacts=registerText(dossier,20,98,left-40,dossierH*40/100,fs,"",registerIvory);
        int crimeY=98+dossierH*40/100;
        registerLine(dossier,20,crimeY,left-40);
        registerText(dossier,20,crimeY+10,left-40,26,fs,registerLanguage(Loc::text("ui.wanted_for"),Loc::text("ui.wanted_for")),registerIvory);
        bountyOfferText=registerText(dossier,20,crimeY+42,left-40,dossierH-crimeY-105,fs,"",registerIvory);
        registerLine(dossier,20,dossierH-57,left-40);
        registerText(dossier,20,dossierH-45,left-40,34,fs+3,registerLanguage(Loc::text("ui.capture_alive"),Loc::text("ui.capture_alive")),registerAmber);
        MyGUI::Widget* mapPanel=registerPanel(client,right,pad,cw-right-pad,bodyH);
        // Match the crop aspect ratio to the widget: never stretch the world map.
        bountyMap=mapPanel->createWidget<MyGUI::ImageBox>("ImageBox",12,12,mapPanel->getWidth()-24,bodyH-68,MyGUI::Align::Default);
        bountyMap->setImageInfo("GuildEscortMap.png",MyGUI::IntCoord(0,0,2048,2048),MyGUI::IntSize(2048,2048));bountyMap->setImageIndex(0);
        bountyMap->setNeedMouseFocus(false);
        // Dashed circle made of inexpensive native UI primitives, updated only on selection.
        for(int i=0;i<180;++i)if(i%9<6)bountyRing.push_back(registerSolid(bountyMap,0,0,3,3,registerAmber));
        bountyPlaceIcon=bountyMap->createWidget<MyGUI::ImageBox>("ImageBox",0,0,32,32,MyGUI::Align::Default);bountyPlaceIcon->setNeedMouseFocus(false);
        bountyZone=registerText(bountyMap,0,0,240,48,18,"",MyGUI::Colour(1,1,1));
        bountyZone->setTextAlign(MyGUI::Align::Center);
        registerText(mapPanel,20,bodyH-43,mapPanel->getWidth()-40,30,16,registerLanguage(Loc::text("ui.approximate_search_area"),Loc::text("ui.approximate_search_area")),registerIvory);
        bountyAccept=client->createWidget<MyGUI::Button>("Kenshi_Button1",pad,ch-65,cw-pad*2,49,MyGUI::Align::Default);
        bountyAccept->setFontHeight(24);bountyAccept->setTextColour(registerAmber);
        bountyAccept->eventMouseButtonClick+=MyGUI::newDelegate(acceptBountyOffer);
    }
    bountySelection=0;
    MercenarieFonts::caption(bountyWindow,Loc::text("ui.the_mercenarie_wanted_notices"));
    updateBountyDossier();bountyWindow->setVisible(true);
}
void selectBountyDossier(MyGUI::Widget* button){
    for(int i=0;i<3;++i)if(button==bountyAcceptButtons[i])bountySelection=i;
    updateBountyDossier();
}
void updateBountyDossier(){
    if(!bountyOfferText||!bountyAccept)return;
    MercenarieFonts::caption(bountyAccept,Loc::text("contract.accept"));
    for(int i=0;i<3;++i){
        const bool exists=i<(int)bountyDisplayedOffers.size();bountyAcceptButtons[i]->setVisible(exists);if(!exists)continue;
        const MercenarieV5::BountyOffer& o=bountyDisplayedOffers[i];std::stringstream title;
        title<<(o.targetName.empty()?(Loc::text("ui.unavailable_c049803")):o.targetName)<<" | "<<displayedContractCats(ContractReroll::bountyReward(o.amount,o.id))<<Loc::text("ui.cats");
        bountyAcceptButtons[i]->setStateSelected(i==bountySelection);
        MercenarieFonts::caption(bountyRowNames[i],o.targetName.empty()?registerLanguage(Loc::text("ui.unavailable_c049803"),Loc::text("ui.unavailable_c049803")):o.targetName);
        MercenarieFonts::caption(bountyRowAmounts[i],registerNumber(displayedContractCats(ContractReroll::bountyReward(o.amount,o.id)))+Loc::text("ui.cats"));
        bountyRowMarks[i]->setVisible(i==bountySelection);
        bountyRowNames[i]->setTextColour(i==bountySelection?registerAmber:registerIvory);
        bountyRowAmounts[i]->setTextColour(i==bountySelection?registerAmber:registerIvory);
    }
    MercenarieFonts::caption(bountyName,"");MercenarieFonts::caption(bountyFacts,"");MercenarieFonts::caption(bountyOfferText,"");
    bountyZone->setVisible(false);bountyMap->setVisible(false);
    if(bountySelection<0||bountySelection>=(int)bountyDisplayedOffers.size()){bountyAccept->setEnabled(false);bountyZone->setVisible(false);return;}
    const MercenarieV5::BountyOffer& o=bountyDisplayedOffers[bountySelection];
    Town* area=shou&&shou->townList?shou->townList->getTownBySID(o.areaId):0;
    const bool valid=area&&!area->getName().empty()&&!o.targetName.empty();
    bountyAccept->setEnabled(valid);bountyZone->setVisible(valid&&ou&&ou->zoneMgr);
    if(!valid){MercenarieFonts::caption(bountyOfferText,Loc::text("ui.no_valid_location_is_available_for_this_offer_this"));return;}
    const std::string place=area->getName();
    MercenarieFonts::caption(bountyName,o.targetName);
    std::stringstream facts;
    facts<<registerLanguage(Loc::text("ui.bounty"),Loc::text("ui.bounty"))<<registerNumber(displayedContractCats(ContractReroll::bountyReward(o.amount,o.id)))<<Loc::text("ui.cats_7132fc0")
         <<Loc::text("ui.faction")<<bountyTargetFactionName(o)<<"\n"
         <<registerLanguage(Loc::text("ui.difficulty"),Loc::text("ui.difficulty"))<<bountyDifficultyName(o)<<"\n"
         <<registerLanguage(Loc::text("ui.guards"),Loc::text("ui.guards"))<<o.guards<<"\n"
         <<registerLanguage(Loc::text("ui.estimated_combat"),Loc::text("ui.estimated_combat"))<<o.combatMin<<" - "<<o.combatMax<<"\n"
         <<registerLanguage(Loc::text("ui.near"),Loc::text("ui.near"))<<place;
    MercenarieFonts::caption(bountyFacts,facts.str());bountyFacts->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    std::stringstream text;
    const char* fr[]={Loc::text("ui.caravan_raids_and_murders_of_travellers_near"),Loc::text("ui.armed_robberies_and_kidnappings_around"),Loc::text("ui.murders_and_theft_of_trade_goods_near")};
    const char* en[]={Loc::text("ui.caravan_raids_and_murders_of_travellers_near"),Loc::text("ui.armed_robberies_and_kidnappings_around"),Loc::text("ui.murders_and_theft_of_trade_goods_near")};
    unsigned int variant=0;for(size_t i=0;i<o.id.size();++i)variant=variant*33+(unsigned char)o.id[i];
    text<<(gMercenarieEnglish?en[variant%3]:fr[variant%3])<<place<<".";
    // Explicit line wrapping for Kenshi TextBox skins which do not wrap automatically.
    std::istringstream words(text.str());std::string word,line,wrapped;
    while(words>>word){
        std::string candidate=line.empty()?word:line+" "+word;
        MercenarieFonts::caption(bountyOfferText,candidate);
        if(!line.empty()&&bountyOfferText->getTextSize().width>bountyOfferText->getWidth()){
            if(!wrapped.empty())wrapped+="\n";wrapped+=line;line=word;
        }else line=candidate;
    }
    if(!line.empty()){if(!wrapped.empty())wrapped+="\n";wrapped+=line;}
    MercenarieFonts::caption(bountyOfferText,wrapped);bountyOfferText->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    if(ou&&ou->zoneMgr){
        iVector2 sector=ou->zoneMgr->getMapSector(area->getPosition());
        int tx=std::max(0,std::min(2047,(int)((sector.x+0.5f)*32))),ty=std::max(0,std::min(2047,(int)((sector.y+0.5f)*32)));
        iVector2 scaleWest=ou->zoneMgr->getMapSector(area->getPosition()+Ogre::Vector3(-32000,0,0));
        iVector2 scaleEast=ou->zoneMgr->getMapSector(area->getPosition()+Ogre::Vector3(32000,0,0));
        float mapScale=abs(scaleEast.x-scaleWest.x)*32.0f/64000.0f;
        int desired=(int)ceil(bountySearchRadius(o,area)*mapScale*2.5f);
        int cropW=std::min(2048,std::max(512,desired)),cropH=std::min(2048,cropW*bountyMap->getHeight()/bountyMap->getWidth());
        int cx=std::max(0,std::min(2048-cropW,tx-cropW/2)),cy=std::max(0,std::min(2048-cropH,ty-cropH/2));
        bountyMap->setImageInfo("GuildEscortMap.png",MyGUI::IntCoord(cx,cy,cropW,cropH),MyGUI::IntSize(cropW,cropH));bountyMap->setImageIndex(0);
        bountyMap->setVisible(true);
        int px=(tx-cx)*bountyMap->getWidth()/cropW,py=(ty-cy)*bountyMap->getHeight()/cropH;
        mercenariePlaceImage(bountyPlaceIcon,area);bountyPlaceIcon->setPosition(px-16,py-16);
        MercenarieFonts::caption(bountyZone,place);
        bountyZone->setPosition(std::max(0,std::min(bountyMap->getWidth()-240,px-120)),std::max(0,std::min(bountyMap->getHeight()-48,py+20)));
        // Derive world-to-texture scale from the engine's sector mapping.
        // Both maps now use the same world-space search radius.
        iVector2 west=ou->zoneMgr->getMapSector(area->getPosition()+Ogre::Vector3(-32000,0,0));
        iVector2 east=ou->zoneMgr->getMapSector(area->getPosition()+Ogre::Vector3(32000,0,0));
        float texelsPerUnit=abs(east.x-west.x)*32.0f/64000.0f;
        const int radius=std::max(2,(int)(bountySearchRadius(o,area)*texelsPerUnit*bountyMap->getWidth()/cropW));
        size_t dash=0;
        for(int i=0;i<180;++i)if(i%9<6){
            double angle=i*6.283185307179586/180;
            bountyRing[dash++]->setPosition(px+(int)(cos(angle)*radius)-1,py+(int)(sin(angle)*radius)-1);
        }
    }
}

GuildHistory::Snapshot bountySnapshot(const MercenarieV5::BountyContract& c){GuildHistory::Snapshot s;s.id="bounty:"+c.offer.id;s.type="bounty";s.giver=2;s.giverId=MercenarieV5::bountyHandle(c.offer.issuer).toString();s.target=c.offer.targetName;s.destinationId=c.offer.areaId;s.reward=ContractRewards::apply(ContractReroll::bountyReward(c.offer.amount,c.offer.id),ContractRewards::snapshot(c.offer.id));s.difficulty=1+2*MercenarieV5::bountyDifficulty(c.offer);return s;}
void archiveBounty(const MercenarieV5::BountyContract& c,int status){GuildHistory::Snapshot s=bountySnapshot(c);std::map<std::string,GuildHistory::Snapshot>::iterator seed=contractSeeds.find(s.id);if(seed!=contractSeeds.end())s=seed->second;s.status=status;s.completed=currentGameHours;if(s.accepted>=0&&currentGameHours>=s.accepted)s.duration=currentGameHours-s.accepted;GuildHistory::append(contractHistory,s);contractSeeds.erase(s.id);}
void finishBountyContract(Character* giver,bool vanillaPaid){
    MercenarieV5::BountyContract& c=bountyWorld.contract;
    if(c.paymentRecorded||!giver||!ou||!ou->player||!ou->player->participant||!ou->player->participant->factionOwnerships)return;
    if(c.state!=MercenarieV5::BountyHandingOver)return;
    const int oldReportXp=guildPoints,oldReportPrestige=guildPrestige;
    const float oldReportGlobal=escortReputation;
    const int baseRewardCats=c.offer.amount;
    const int reward=ContractRewards::apply(ContractReroll::bountyReward(baseRewardCats,c.offer.id),ContractRewards::snapshot(c.offer.id));
    if(!vanillaPaid)financeChange(reward,Finance::Contract,"finance.bounty");
    else if(reward>baseRewardCats)financeChange(reward-baseRewardCats,Finance::Contract,"finance.bounty_adjustment");
    else if(reward<baseRewardCats)financeChange(reward-baseRewardCats,Finance::Contract,"finance.bounty_adjustment");
    payrollContractReward("bounty:"+c.offer.id,reward,"finance.bounty","",true);
    c.acknowledgeHandover(true,true);archiveBounty(c,1);
    FiscalRelation uc=actualFiscalRelation(FISCAL_UC),mg=actualFiscalRelation(FISCAL_MERCENARY_GUILD);
    fiscalLedger.create("bounty:"+c.offer.id,Loc::text("ui.bounty_hunt_414870e"),Loc::text("common.commun"),Loc::text("ui.police_station"),giver->getName(),c.offer.targetName,currentGameHours,reward,0,0,reward,c.offer.faction==MercenarieV5::UnitedCities,uc,mg);
    saveFiscalLedger();escortReputation=EscortReputation::clamp(escortReputation+2.0f);
    // Preserve the separate bounty reward rules, but use the shared level cap
    // and prestige accounting so a bounty can never lower level-10 XP.
    GuildProgression::award(guildPoints,guildPrestige,30+baseRewardCats/500);++successfulContracts;totalContractCats+=reward;
    appliedGuildUnlockLevel=-1;saveReputations();
    retireBountyCamp();updateTrackerUI();
    std::ostringstream receipt;receipt<<registerLanguage(Loc::text("ui.mission_complete"),Loc::text("ui.mission_complete"))<<"\n"<<(vanillaPaid?registerLanguage(Loc::text("ui.already_paid_by_kenshi"),Loc::text("ui.already_paid_by_kenshi")):registerLanguage(Loc::text("ui.payment_received"),Loc::text("ui.payment_received")))<<reward<<Loc::text("ui.cats_7132fc0")<<registerLanguage(Loc::text("ui.guild_xp"),Loc::text("ui.guild_xp"))<<reportSigned(guildPoints-oldReportXp)<<"\n"<<registerLanguage(Loc::text("ui.global_reputation_aaa7942"),Loc::text("ui.global_reputation_aaa7942"))<<reportSigned(escortReputation-oldReportGlobal)<<"\n"<<registerLanguage(Loc::text("ui.prestige"),Loc::text("ui.prestige"))<<reportSigned(guildPrestige-oldReportPrestige);
    ou->showPlayerAMessage(receipt.str(),true);
    // A bounty is already paid at handover. Reuse the report surface as a receipt;
    // closing it must never enter the escort settlement path.
    if(finalWindow&&finalWindow->getVisible())return;
    buildFinalReport();if(!finalWindow)return;reportReadOnly=true;negotiationWasPaused=ou->isPaused();ou->userPause(true);
    setMissionIconV6(reportMissionIcon,3);MercenarieFonts::caption(reportType,missionLabelV6(3));reportType->setTextColour(missionColourV6(3));
    MercenarieFonts::caption(reportFields[0],giver->getName()+" > "+c.offer.targetName);MercenarieFonts::caption(reportFields[1],registerNumber(reward)+Loc::text("ui.cats"));MercenarieFonts::caption(reportFields[2],Loc::text("ui.0_cats"));
    MercenarieFonts::caption(reportFields[3],registerLanguage(Loc::text("ui.not_tracked"),Loc::text("ui.not_tracked")));MercenarieFonts::caption(reportFields[4],registerNumber((int)(bountyWorld.missionSeconds/60))+Loc::text("ui.min_f4e8a8b"));MercenarieFonts::caption(reportFields[5],bountyDifficultyName(c.offer));MercenarieFonts::caption(reportFields[6],registerLanguage(Loc::text("ui.1_target_delivered_alive"),Loc::text("ui.1_target_delivered_alive")));
    for(int i=0;i<5;++i)reportSkulls[i]->setVisible(false);
    MercenarieFonts::caption(reportGains[0],registerNumber(reward)+Loc::text("ui.cats"));MercenarieFonts::caption(reportGains[1],Loc::text("ui.0_cats"));MercenarieFonts::caption(reportTotalLabel,registerLanguage(Loc::text("ui.total_received"),Loc::text("ui.total_received")));MercenarieFonts::caption(reportGains[2],registerNumber(reward)+Loc::text("ui.cats_3f70b99"));
    MercenarieFonts::caption(reportGains[3],reportSigned(guildPoints-oldReportXp));MercenarieFonts::caption(reportGains[4],"+0");MercenarieFonts::caption(reportGains[5],reportSigned(escortReputation-oldReportGlobal));MercenarieFonts::caption(reportGains[6],reportSigned(guildPrestige-oldReportPrestige));
    MercenarieFonts::caption(reportHint,registerLanguage(Loc::text("ui.contract_already_settled_at_handover_no_additional_bonuses"),Loc::text("ui.contract_already_settled_at_handover_no_additional_bonuses")));MercenarieFonts::caption(reportCount,registerLanguage(Loc::text("ui.bonuses_selected_0_0"),Loc::text("ui.bonuses_selected_0_0")));
    for(int i=0;i<MissionBonuses::Count;++i){MercenarieFonts::caption(finalBonusButtons[i],"");finalBonusButtons[i]->setEnabled(false);finalBonusButtons[i]->setStateSelected(false);reportBonusSelection92(i,false);MercenarieFonts::caption(reportAmounts[i],registerLanguage(Loc::text("ui.not_applicable"),Loc::text("ui.not_applicable")));reportAmounts[i]->setTextColour(MyGUI::Colour(.58f,.61f,.64f));reportIcons[i]->setColour(MyGUI::Colour(.44f,.47f,.49f));reportNames[i]->setTextColour(MyGUI::Colour(.58f,.61f,.64f));reportDescriptions[i]->setTextColour(MyGUI::Colour(.5f,.53f,.56f));for(int e=0;e<4;++e)reportEdges[i][e]->setColour(MyGUI::Colour(.38f,.41f,.43f));}
    MercenarieFonts::caption(reportBefore,registerLanguage(Loc::text("ui.handover_completed_rewards_applied_once"),Loc::text("ui.handover_completed_rewards_applied_once")));
    MercenarieFonts::caption(finalBonusPreview,vanillaPaid?registerLanguage(Loc::text("ui.paid_by_kenshi_no_additional_payment"),Loc::text("ui.paid_by_kenshi_no_additional_payment")):registerLanguage(Loc::text("ui.single_contract_payment_recorded"),Loc::text("ui.single_contract_payment_recorded")));MercenarieFonts::caption(reportAfter,receipt.str());
    finalReputationButton->setEnabled(false);finalHalfCashButton->setEnabled(false);finalCashButton->setEnabled(true);MercenarieFonts::caption(finalCashButton,registerLanguage(Loc::text("ui.close_report"),Loc::text("ui.close_report")));
    MercenarieFonts::caption(finalReputationButton,registerLanguage(Loc::text("ui.payment_completed"),Loc::text("ui.payment_completed")));MercenarieFonts::caption(finalHalfCashButton,registerLanguage(Loc::text("ui.no_additional_bonuses"),Loc::text("ui.no_additional_bonuses")));
    for(int i=0;i<7;++i){fitRegisterText(reportFields[i],rf(17));fitRegisterText(reportGains[i],rf(i==2?26:18));}
    refreshReport88(reward,0,0);reportTip88->setCaption("0 Cats");
    MercenarieFonts::caption(reportAfter,registerLanguage(Loc::text("ui.payment_8693bf5"),Loc::text("ui.payment"))+registerNumber(reward)+Loc::text("ui.cats_xp")+reportSigned(guildPoints-oldReportXp)+Loc::text("ui.prestige_e9bab8f")+reportSigned(guildPrestige-oldReportPrestige)+registerLanguage(Loc::text("ui.global_reputation_1bd3f63"),Loc::text("ui.global_reputation_1bd3f63"))+reportSigned(escortReputation-oldReportGlobal));
    finalWindow->setVisible(true);
}
void settleBountyHandover(){
    MercenarieV5::BountyContract& c=bountyWorld.contract;
    Character* giver=MercenarieV5::bountyHandle(c.offer.issuer).getCharacter();
    if(c.state==MercenarieV5::BountyHandingOver&&MercenarieV5::observeBountyTransfer(c,giver)==MercenarieV5::TransferComplete)finishBountyContract(giver,false);
}
// Observe native handover without intercepting or paying the vanilla reward.
bool observeVanillaBountyDelivery(Character* giver){
    MercenarieV5::BountyContract& c=bountyWorld.contract;
    if(bountyWorld.suspended||c.state!=MercenarieV5::BountyActive||c.paymentRecorded||!giver||!giver->getFaction())return false;
    if(!(MercenarieV5::bountyIdentity(giver->getHandle())==c.offer.issuer))return false;
    Character* target=MercenarieV5::bountyHandle(c.target).getCharacter();
    if(!target||target->isDead()||giver->carryingObject.isNull()||giver->carryingObject.getCharacter()!=target)return false;
    if(!target->crimes.bountyAlreadyBeenClaimedByPlayer(giver->getFaction()))return false;
    c.state=MercenarieV5::BountyHandingOver;
    finishBountyContract(giver,true);return true;
}
void handOverBounty(Character* giver){
    selectQuestActor(giver);
    if(bountyWorld.suspended||!ou||!ou->player)return;
    MercenarieV5::BountyContract& c=bountyWorld.contract;
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* carrier=ou->player->playerCharacters[i];
        if(!MercenarieV5::playerCarriesBounty(c,carrier,ou->player->getFaction()))continue;
        MercenarieV5::BountyTransferResult result=MercenarieV5::transferBountyPrisoner(c,giver,carrier,ou->player->getFaction());
        if(result==MercenarieV5::TransferComplete)settleBountyHandover();
        else ou->showPlayerAMessage(Loc::text("ui.prisoner_transfer_not_confirmed_no_contract_payment_issued"),true);
        return;
    }
}
void updateBountyAmbush(){
    MercenarieV5::BountyContract& c=bountyWorld.contract;
    if(c.state!=MercenarieV5::BountyActive||!ou||!ou->player||!shou||!shou->townList)return;
    const bool raid=bountyWorld.encounter==2;
    Character* leader=MercenarieV5::bountyHandle(c.target).getCharacter();
    Town* area=shou->townList->getTownBySID(c.offer.areaId);
    if(!area)return;
    Character* attacker=0;
    for(size_t i=0;i<c.group.size();++i){
        Character* member=MercenarieV5::bountyHandle(c.group[i]).getCharacter();
        if(member&&!member->isDead()&&!member->isBeingCarried()&&member->getAI()&&
           member->getAI()->targetIsConscious(member->getHandle(),member->getPosition())&&
           !member->getAI()->iAmImprisoned(member->getHandle(),member->getPosition())){attacker=member;break;}
    }
    if(!attacker)return;
    Character* victim=0;float distance=1e30f;float radius=bountySearchRadius(c.offer,area);
    bool groupFighting=false,playerSeen=false;
    for(size_t i=0;i<c.group.size();++i){
        Character* member=MercenarieV5::bountyHandle(c.group[i]).getCharacter();
        if(member&&!member->isDead()&&!member->isBeingCarried()&&member->isInCombatMode(true,true))groupFighting=true;
    }
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){
        Character* player=ou->player->playerCharacters[i];if(!player||player->isDead()||player->isBeingCarried())continue;
        Ogre::Vector3 delta=player->getPosition()-area->getPosition();delta.y=0;
        if(raid&&!groupFighting&&delta.squaredLength()>radius*radius)continue;
        float d=1e30f;
        for(size_t m=0;m<c.group.size();++m){
            Character* member=MercenarieV5::bountyHandle(c.group[m]).getCharacter();
            if(!member||member->isDead()||member->isBeingCarried()||!member->getAI()||
               !member->getAI()->targetIsConscious(member->getHandle(),member->getPosition())||
               member->getAI()->iAmImprisoned(member->getHandle(),member->getPosition()))continue;
            float separation=player->getPosition().squaredDistance(member->getPosition());d=std::min(d,separation);
            if(separation<=14400.0f&&member->getSensoryData()&&member->getSensoryData()->canISeeThisGuy(player))playerSeen=true;
        }
        if(!raid&&d>90000.0f)continue;
        if(d<distance){distance=d;victim=player;}
    }
    if(!victim)return;
    bool attack=groupFighting||playerSeen||(raid&&(bountyWorld.ambushAnnounced||distance<=6400.0f));
    if(!raid&&!attack)return;
    if(raid&&attack&&!bountyWorld.ambushAnnounced){
        const char* fr[]={Loc::text("ui.looking_for_us_look_no_further_death_has_arrived"),Loc::text("ui.your_guild_put_a_price_on_my_head_come"),Loc::text("ui.the_hunters_have_become_the_prey")};
        const char* en[]={Loc::text("ui.looking_for_us_look_no_further_death_has_arrived"),Loc::text("ui.your_guild_put_a_price_on_my_head_come"),Loc::text("ui.the_hunters_have_become_the_prey")};
        unsigned int variant=0;for(size_t i=0;i<c.offer.id.size();++i)variant=variant*33+(unsigned char)c.offer.id[i];
        Character* speaker=leader&&!leader->isDead()&&!leader->isBeingCarried()?leader:attacker;
        speaker->sayALine(gMercenarieEnglish?en[variant%3]:fr[variant%3],true);
        bountyWorld.ambushAnnounced=true;bountyWorld.targetSpotted=true;
    }
    for(size_t i=0;i<c.group.size();++i){
        Character* member=MercenarieV5::bountyHandle(c.group[i]).getCharacter();
        if(!member||member->isDead()||member->isBeingCarried()||!member->getAI())continue;
        if(member->getAI()->iAmImprisoned(member->getHandle(),member->getPosition()))continue;
        if(!member->getAI()->targetIsConscious(member->getHandle(),member->getPosition()))continue;
        hand none;member->getAI()->setCenterOfMovementTarget(none);
        member->getAI()->setCenterOfMovement(victim->getPosition());
        member->getAI()->setManuveringFreedomLevel(AI::ROAM_FAR);
        member->removeJob(FOLLOW_SQUADLEADER);member->removeJob(WANDERER);
        member->removeJob(WANDER_TOWN);member->removeJob(PATROL);
        member->removeJob(HOLD_POSITION);member->removeJob(MOVE_CUS_ORDERED);
        if(attack){
            // Attack orders alone do not establish reciprocal hostility. Use the
            // native temporary-enemy memory, scoped to these actors, not factions.
            for(size_t p=0;p<ou->player->playerCharacters.size();++p){
                Character* player=ou->player->playerCharacters[p];
                if(!player||player->isDead()||player->isBeingCarried())continue;
                if(player!=victim&&player->getPosition().squaredDistance(member->getPosition())>250000.0f)continue;
                if(!member->getCharacterMemoryTag(player,ST_TEMPORARY_ENEMY))member->rememberCharacter(player,ST_TEMPORARY_ENEMY);
                if(!player->getCharacterMemoryTag(member,ST_TEMPORARY_ENEMY))player->rememberCharacter(member,ST_TEMPORARY_ENEMY);
            }
            if(member->getAttackTarget()!=victim->getHandle()||!member->isInCombatMode(true,true)){
                member->attackTarget(victim);member->reThinkCurrentAIAction();
            }
        }
        else{
            member->addJob(MOVE_CUS_ORDERED,0,false,false,victim->getPosition());
            member->getMovement()->setRoadPreference(0.0f);
            member->getMovement()->setRoadDestination(victim->getPosition());
        }
    }
}
void searchForBountyCamp(float elapsed){
    MercenarieV5::BountyContract& c=bountyWorld.contract;
    if(ou&&ou->isPaused())return;
    if(bountyWorld.suspended||c.state!=MercenarieV5::BountySpawning||c.target.valid()||!ou||!ou->player||!ou->navmesh||!ou->theFactory||!ou->factionMgr||!shou||!shou->townList)return;
    Town* area=shou->townList->getTownBySID(c.offer.areaId);if(!area)return;
    float radius=bountySearchRadius(c.offer,area);
    Character* searcher=0;bool nearPoint=false;
    const Ogre::Vector3 secret(bountyWorld.secretX,bountyWorld.secretY,bountyWorld.secretZ);
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){
        Character* actor=ou->player->playerCharacters[i];if(!actor||actor->isDead()||actor->isBeingCarried())continue;
        Ogre::Vector3 delta=actor->getPosition()-area->getPosition();delta.y=0;
        if(delta.squaredLength()<=radius*radius){
            if(!searcher)searcher=actor;
            Ogre::Vector3 separation=actor->getPosition()-secret;separation.y=0;
            if(separation.squaredLength()<=1000000.0f){nearPoint=true;searcher=actor;}
        }
    }
    // No RNG roll and no accumulated time when every party member is outside.
    if(!searcher)return;
    if(!bountyWorld.secretChosen)return;
    bountyWorld.encounter=MercenarieV5::advanceBountyEncounter(bountyWorld.searchSeconds,bountyWorld.encounter,true,nearPoint,elapsed,MercenarieV5::bountyDifficulty(c.offer));
    if(bountyWorld.encounter==0)return;
    // Limit terrain/factory retries to once per five seconds, not every frame.
    unsigned int due=1+(unsigned int)(bountyWorld.searchSeconds/5.0);
    if(due==bountyWorld.searchAttempts)return;
    bountyWorld.searchAttempts=due;
    {
        std::ofstream diagnostic("mods/Guild Escort Contracts/bounty-status.log",std::ios::trunc);
        diagnostic<<"contract="<<c.offer.id<<"\nstate=searching\nsecondsInArea="<<bountyWorld.searchSeconds<<"\nattempts="<<bountyWorld.searchAttempts<<"\n";
    }
    Ogre::Vector3 destination;bool valid=false;
    if(bountyWorld.encounter==1){
        if(!nearPoint)return;
        if(!ou->zoneMgr||!ou->navmesh->isLoaded(ou->zoneMgr->getMapSector(secret)))return;
        Ogre::Vector3 terrain=secret;terrain.y=UtilityT::getTerrainHeight(terrain.x,terrain.z);
        destination=terrain;ou->navmesh->getClosestPoint(terrain,80.0f,1.0f,false,destination);
        Ogre::Vector3 offset=destination-secret;offset.y=0;
        valid=offset.squaredLength()<=6400.0f&&ou->navmesh->getPositionValid(destination)&&bountyOutsideSettlements(destination,600);
    }
    int offset=UtilityT::randomInt(0,15);
    for(int ring=0;bountyWorld.encounter==2&&ring<3&&!valid;++ring)for(int i=0;i<16&&!valid;++i){
        double angle=(i+offset)*6.28318530718/16;
        int distance=bountyWorld.encounter==2?600+ring*150:1000+ring*200;
        Ogre::Vector3 point=searcher->getPosition()+Ogre::Vector3((float)cos(angle)*distance,0,(float)sin(angle)*distance);
        Ogre::Vector3 ground=point;ou->navmesh->getClosestPoint(point,80.0f,1.0f,false,ground);
        Ogre::Vector3 delta=ground-area->getPosition();delta.y=0;
        if(delta.squaredLength()>radius*radius||!ou->navmesh->getPositionValid(ground)||!bountyOutsideSettlements(ground,600))continue;
        bool tooClose=false;
        for(size_t p=0;p<ou->player->playerCharacters.size();++p){
            Character* observer=ou->player->playerCharacters[p];
            if(observer){
                Ogre::Vector3 separation=observer->getPosition()-ground;separation.y=0;
                float minimum=bountyWorld.encounter==2?450.0f:800.0f;
                if(separation.squaredLength()<minimum*minimum){tooClose=true;break;}
            }
        }
        if(tooClose)continue;
        destination=ground;valid=true;
    }
    if(!valid){
        return;
    }
    std::stringstream sid;sid<<(881600+c.offer.guards)<<"-Guild Escort Contracts.mod";
    GameData* squad=ou->gamedata.getData(sid.str(),SQUAD_TEMPLATE);
    Faction* bandits=ou->factionMgr->getFactionByStringID(bountyTargetFactionId);
    const char* factionIds[]={"defaultEmpireFactionSID","1083-gamedata.base","11624-Dialogue (10).mod"};
    Faction* law=ou->factionMgr->getFactionByStringID(factionIds[(int)c.offer.faction]);
    if(!squad||!bandits||!law)return;
    c.offer.x=destination.x;c.offer.y=destination.y;c.offer.z=destination.z;
    MercenarieV5::BountySpawnResult spawn=MercenarieV5::spawnBountyGroup(c,ou->theFactory,squad,bandits,law,0);
    if(spawn.complete){
        updateBountyCamp();
        updateBountyAmbush();updateTrackerUI();
        ou->showPlayerAMessage(bountyWorld.encounter==1?registerLanguage(Loc::text("ui.you_are_near_the_last_known_position_search_the"),Loc::text("ui.you_are_near_the_last_known_position_search_the")):registerLanguage(Loc::text("ui.armed_men_are_approaching_prepare_for_an_ambush"),Loc::text("ui.armed_men_are_approaching_prepare_for_an_ambush")),true);
        return;
    }
    // Retain an uncertain partial squad; never roll again and duplicate its leader.
    if(spawn.platoon){
        bountyWorld.suspended=true;
        ou->showPlayerAMessage(registerLanguage(Loc::text("ui.camp_creation_incomplete_search_suspended_to_prevent_duplicate_ta"),Loc::text("ui.camp_creation_incomplete_search_suspended_to_prevent_duplicate_ta")),true);
    }
}
void updateBountyContract(float elapsed){
    if(bountyWorld.contract.occupied()&&!bountyWorld.suspended&&ou&&!ou->isPaused()&&elapsed>0)bountyWorld.missionSeconds+=elapsed;
    searchForBountyCamp(elapsed);
    static float clocks[5]={0};float& clock=clocks[bountyQuestSlot()];clock-=elapsed;if(clock>0)return;clock=1.0f;
    if(bountyWorld.suspended)return;
    static int recoveryTicks[5]={0};int& recoveryTick=recoveryTicks[bountyQuestSlot()];if(++recoveryTick>=5){recoveryTick=0;recoverExistingBountyPlatoon();}
    updateBountyCamp();
    MercenarieV5::BountyContract& c=bountyWorld.contract;
    if(observeVanillaBountyDelivery(MercenarieV5::bountyHandle(c.offer.issuer).getCharacter()))return;
    if(c.state==MercenarieV5::BountyHandingOver){settleBountyHandover();return;}
    if(c.state!=MercenarieV5::BountyActive)return;
    Character* target=MercenarieV5::bountyHandle(c.target).getCharacter();
    // Small diagnostic snapshot, no respawn when a saved handle is unresolved.
    static int diagnosticTick=0;
    if(diagnosticTick++%15==0){
        std::ofstream diagnostic("mods/Guild Escort Contracts/bounty-status.log",std::ios::trunc);
        diagnostic<<"contract="<<c.offer.id<<"\ntarget="<<c.offer.targetName
                  <<"\nhandle="<<MercenarieV5::bountyHandle(c.target).toString()
                  <<"\nloaded="<<(target?1:0)<<"\nspotted="<<bountyWorld.targetSpotted
                  <<"\nanchor="<<c.offer.x<<","<<c.offer.y<<","<<c.offer.z
                  <<"\ncampStarted="<<bountyWorld.campStarted<<"\ncampObjects="<<bountyWorld.campObjects.size();
        diagnostic<<"\nencounter="<<bountyWorld.encounter<<"\nambushAnnounced="<<bountyWorld.ambushAnnounced;
        for(size_t i=0;i<bountyWorld.campObjects.size();++i){
            Building* object=MercenarieV5::bountyHandle(bountyWorld.campObjects[i]).getBuilding();
            diagnostic<<"\ncampObject"<<i<<"Loaded="<<(object?1:0);
            if(object)diagnostic<<" physical="<<object->isPhysical()<<" created="<<object->isCreated()<<" visible="<<object->getVisible();
        }
        diagnostic<<"\nhandleDiagnostic="<<MercenarieV5::bountyHandle(c.target).debugWhatHappenedToMe();
        unsigned int membersLoaded=0;for(size_t i=0;i<c.group.size();++i)if(MercenarieV5::bountyHandle(c.group[i]).getCharacter())++membersLoaded;
        diagnostic<<"\ngroupLoaded="<<membersLoaded<<"/"<<c.group.size();
        float nearest=1e30f;
        if(ou&&ou->player)for(size_t i=0;i<ou->player->playerCharacters.size();++i){
            Character* player=ou->player->playerCharacters[i];if(!player)continue;
            Ogre::Vector3 offset=player->getPosition()-Ogre::Vector3(c.offer.x,c.offer.y,c.offer.z);offset.y=0;
            nearest=std::min(nearest,offset.length());
        }
        diagnostic<<"\nnearestPlayerToCamp="<<nearest;
        if(target){Ogre::Vector3 position=target->getPosition();diagnostic<<"\nposition="<<position.x<<","<<position.y<<","<<position.z<<"\ndead="<<target->isDead();}
        diagnostic<<"\n";
    }
    updateBountyAmbush();
    if(!target)return;
    // For ambushes only, offer coordinates are the persisted last loaded position.
    // Camp coordinates stay fixed even when the leader is carried elsewhere.
    if(bountyWorld.encounter==2){
        Ogre::Vector3 last=target->getPosition();c.offer.x=last.x;c.offer.y=last.y;c.offer.z=last.z;
    }
    if(!target->isDead()&&!bountyWorld.targetSpotted&&ou&&ou->player){
        for(size_t i=0;i<ou->player->playerCharacters.size();++i){
            Character* scout=ou->player->playerCharacters[i];
            if(!scout||scout->isDead()||scout->isBeingCarried()||scout->getPosition().squaredDistance(target->getPosition())>14400.0f)continue;
            if(!scout->getAI()||!scout->getAI()->targetIsConscious(scout->getHandle(),scout->getPosition()))continue;
            if(!scout->getSensoryData()||!scout->getSensoryData()->canISeeThisGuy(target))continue;
            bountyWorld.targetSpotted=true;
            scout->sayALine(registerLanguage(Loc::text("ui.target_spotted"),Loc::text("ui.target_spotted")),true);
            break;
        }
    }
    static int patrolTicks[5]={0};int& patrolTick=patrolTicks[bountyQuestSlot()];if(++patrolTick>=5){patrolTick=0;keepBountyGroupOutside();}
    if(target->isDead()){c.observe(true,true,false);archiveBounty(c,2);escortReputation=EscortReputation::clamp(escortReputation-3.0f);++failedContracts;saveReputations();if(ou)ou->showPlayerAMessage(MercenarieV5::bountyText(MercenarieV5::TargetDied,!gMercenarieEnglish),true);}
}
}
void (*bountyPrisonOriginal)(Character*,bool,UseableStuff*);
void (*bountyPersistenceOriginal)(Platoon*);
void bountyPersistenceHook(Platoon* platoon){
    if(MercenarieCleanup::disabled){bountyPersistenceOriginal(platoon);return;}
    if(isAnyQuestBountyPlatoon(platoon,false)){platoon->setPersistentSquad(true);return;}
    bountyPersistenceOriginal(platoon);
}
void (*bountyUnloadedOriginal)(Platoon*);
void bountyUnloadedHook(Platoon* platoon){
    if(MercenarieCleanup::disabled){bountyUnloadedOriginal(platoon);return;}
    // A mission camp is stationary. Do not apply roaming off-screen simulation.
    if(isAnyQuestBountyPlatoon(platoon,true)){platoon->setPersistentSquad(true);return;}
    bountyUnloadedOriginal(platoon);
}
void (*bountyMapUpdateOriginal)(MapScreen*);
void bountyMapUpdateHook(MapScreen* map){
    if(MercenarieCleanup::disabled){bountyMapUpdateOriginal(map);return;}
    bountyMapUpdateOriginal(map);updateAllQuestBountyMaps(map);updateAllQuestMissionMaps(map);
}
void bountyPrisonHook(Character* person,bool on,UseableStuff* cage){
    if(MercenarieCleanup::disabled){if(bountyPrisonOriginal)bountyPrisonOriginal(person,on,cage);return;}
    const bool cancel=person&&on&&cage&&ou&&ou->player&&cage->getFaction()==ou->player->getFaction();
    if(bountyPrisonOriginal)bountyPrisonOriginal(person,on,cage);
    if(cancel)observeQuestPrison(person);
}
