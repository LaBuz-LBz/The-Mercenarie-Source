// Exercises the production context switcher/serializer with engine calls mocked.
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include "EscortContract.h"
#include "MissionBonuses.h"
#include "EscortPace.h"
#include "EscortMissionRules.h"
#include "RoutePrototype.h"
#include "v5/BountySave.h"
namespace Ogre {struct Vector3{float x,y,z;Vector3(float a=0,float b=0,float c=0):x(a),y(b),z(c){}};}
namespace AI {enum AIManuverabilityOrders {Free};}
enum ContractLifecycleState {CONTRACT_NONE,CONTRACT_CLIENT_MEETING,CONTRACT_NEGOTIATING,CONTRACT_ACTIVE,CONTRACT_COMPLETED,CONTRACT_FAILED};
struct Character;struct TownBase;
std::map<unsigned int,Character*> actors;
struct hand {unsigned int type,container,containerSerial,index,serial;hand(unsigned int n=0):type(0),container(0),containerSerial(0),index(0),serial(n){}bool isNull()const{return !serial;}void setNull(){serial=0;}bool operator==(const hand& h)const{return serial==h.serial;}Character* getCharacter()const{return actors.count(serial)?actors[serial]:0;}};
#define MISSION_RESCUE_TYPES_ONLY
#include "MissionRescueState.h"
#undef MISSION_RESCUE_TYPES_ONLY
struct Faction {void destroyObject(Character*);};
struct TownBase {};
enum {FOLLOW_PLAYER_ORDER};
struct Character {hand id;bool dead;int orders;Faction faction;Character(unsigned int n=0):id(n),dead(false),orders(0){}hand getHandle(){return id;}bool isDead(){return dead;}Faction* getFaction(){return &faction;}Ogre::Vector3 getPosition(){return Ogre::Vector3();}bool isInCombatMode(bool,bool){return false;}void addJob(int,Character*,bool,bool,Ogre::Vector3){++orders;}};
#include "SavedActorResolution.h"
SavedActorResolution::Actor completedEscortRestore;SavedActorResolution::Group completedCaravanRestore,caravanRestore,scientificRestore;
void Faction::destroyObject(Character* c){actors.erase(c->id.serial);}
struct Platoon {hand leader;hand getSquadLeader_theRealOne(){return leader;}};
struct WaitingHereState {hand actor;Ogre::Vector3 position;AI::AIManuverabilityOrders movement;Ogre::Vector3 center;hand centerTarget;bool returning;};
struct MissionGroupController {void reset(){}};
struct Window{bool visible;Window():visible(false){}bool getVisible(){return visible;}};
Window* finalWindow=0;Window* negotiationWindow=0;Window* contractDecisionWindow=0;Window* contractsWindow=0;Window* bountyWindow=0;Window* developerFinishPicker=0;
struct Player {std::vector<Character*> playerCharacters;Faction faction;Faction* getFaction(){return &faction;}};
struct World{Player* player;std::string notice;void showPlayerAMessage(const std::string& s,bool){notice=s;}};
World world;World* ou=&world;bool gMercenarieEnglish=false;
struct Ledger {unsigned long nextId;std::set<std::string> ids;Ledger():nextId(1){}bool contains(const std::string& id){return ids.count(id)!=0;}} fiscalLedger;
double currentGameHours=24;
std::set<std::string> rewardedContractIds;
namespace MercenarieV5 {ActorIdentity bountyIdentity(hand h){ActorIdentity i;i.serial=h.serial;return i;}bool playerCarriesBounty(const BountyContract& c,Character* p,Faction*){return p&&p->id.serial==c.target.serial+100;}}
namespace {
#include "quest-test-globals.generated.h"
const char* destinationTown="town";const char* destinationName="Town";
MercenarieV5::BountyWorldState bountyWorld;
struct QuestTrackerItem {int slot;bool bounty;QuestTrackerItem():slot(0),bounty(false){}};
std::vector<QuestTrackerItem> questTrackerItems;
void appendEscortTrackerItem(){if(missionActive||missionPending)questTrackerItems.push_back(QuestTrackerItem());}
void appendBountyTrackerItem(){if(bountyWorld.contract.occupied())questTrackerItems.push_back(QuestTrackerItem());}
void refreshQuestActors();
void issueTravelOrder(Ogre::Vector3){if(escort)++escort->orders;}
void missionIssueOrder(Character* c,int type,Character* target,const Ogre::Vector3& pos){c->addJob(type,target,false,false,pos);}
void missionAdoptRestoredTravel(Character*){}
void resumeRecoveredMissionPhase(){if(!missionActive||missionPending||!escort||missionPaused||waitingForPlayer||scientificResearching)return;if(missionFollowing){Character* target=missionFollowTarget.getCharacter();if(target)missionIssueOrder(escort,FOLLOW_PLAYER_ORDER,target,target->getPosition());}else issueTravelOrder(leavingBuilding?exitWaypoint:destination);}
void enforceWaitingHere(){}
int escortTicks[5]={0},bountyTicks[5]={0};int bountyQuestSlot();
void updateBountyContract(float){++bountyTicks[bountyQuestSlot()];}
struct MapScreen{};void updateBountyCampMap(MapScreen*){}
}
void observeRetiredMissionActor(Character*,std::set<std::string>&,std::map<std::string,int>&){}
#include "QuestContexts.h"
namespace {void tickEscortMission(float dt){if(escort&&(missionActive||missionPending)){++escortTicks[selectedEscortQuest];missionElapsed+=dt;}}}
#include "MissionRescuePersistence.h"
struct TestArchive:MissionArchive {
    bool requireActors,resolve;TestArchive():requireActors(true),resolve(true){}TestArchive(const std::string& b):MissionArchive(b),requireActors(true),resolve(true){}
    template<class T>void field(T& x){MissionArchive::field(x);}
    template<class T>void field(std::vector<T>& v){unsigned int n=(unsigned int)v.size();field(n);if(n>4096)throw std::runtime_error("oversize");if(reading)v.resize(n);for(unsigned int i=0;i<n;++i)field(v[i]);}
    void field(hand& h){field(h.type);field(h.container);field(h.containerSerial);field(h.index);field(h.serial);}
    void field(Character*& c){hand h;if(!reading&&c)h=c->getHandle();field(h);if(reading)c=h.getCharacter();}
    void field(Faction*& f){unsigned int n=0;field(n);if(reading)f=0;}
    void field(TownBase*& t){unsigned int n=0;field(n);if(reading)t=0;}
    void field(Ogre::Vector3& v){field(v.x);field(v.y);field(v.z);}
    void field(WaitingHereState& v){field(v.actor);field(v.position);field(v.movement);field(v.center);field(v.centerTarget);field(v.returning);}
#include "quest-test-contract-archive.generated.h"
};
std::string savePool(){TestArchive a;EscortQuestContext selected;selected.archive(a);std::string b=bountyWorld.save();a.field(b);archiveQuestContexts(a);archiveMissionRescue(a);return a.bytes;}
void loadPool(const std::string& bytes){resetQuestContexts();emptyEscortQuest.restore();bountyWorld=MercenarieV5::BountyWorldState();TestArchive a(bytes);EscortQuestContext selected;selected.archive(a);selected.restore();std::string b;a.field(b);bountyWorld.load(b);archiveQuestContexts(a);archiveMissionRescue(a);a.finish();}
void clearPool(){resetQuestContexts();emptyEscortQuest.restore();bountyWorld=MercenarieV5::BountyWorldState();}
MercenarieV5::BountyOffer bountyOffer(int n){MercenarieV5::BountyOffer o;std::ostringstream id;id<<"bounty-"<<n;o.id=id.str();o.areaId="area";o.targetName="target";o.issuer.serial=90;o.amount=7000+n;return o;}
int main(){
    Player player;world.player=&player;Character people[5];for(int i=0;i<5;++i){people[i].id=hand(i+1);actors[i+1]=&people[i];}
    clearPool();std::set<std::string> ids;
    for(int i=0;i<5;++i){assert(prepareEscortQuest());missionActive=true;contractLifecycle=CONTRACT_ACTIVE;escortHandle=people[i].id;escort=&people[i];progressMembers.assign(1,escortHandle);missionReward=1000+i*200;advancePaid=100+i;currentMissionFiscalId=allocateQuestFiscalId();assert(ids.insert(currentMissionFiscalId).second);destinationNameStorage=std::string("Town ")+char('A'+i);destinationName=destinationNameStorage.c_str();currentContract.destination=destinationName;finalBonusChoice.prepared=true;finalBonusChoice.amounts[0]=i*25;finalBonusChoice.selected=1;assert(activeQuestCount()==i+1);}
    const int selected=selectedEscortQuest;const std::string id=currentMissionFiscalId;
    assert(!prepareEscortQuest()&&!prepareBountyQuest());assert(selectedEscortQuest==selected&&currentMissionFiscalId==id);assert(world.notice.find("5")!=std::string::npos);
    gMercenarieEnglish=true;assert(!questCapacityAvailable());assert(world.notice.find("Maximum 5 active quests")!=std::string::npos);
    for(int i=0;i<5;++i){selectEscortQuest(i);assert(missionReward==1000+i*200&&advancePaid==100+i);assert(escortHandle==people[i].id);assert(currentContract.destination==std::string("Town ")+char('A'+i));assert(finalBonusChoice.amounts[0]==i*25);}
    selectEscortQuest(3);tickAllQuests(1);assert(selectedEscortQuest==3);for(int i=0;i<5;++i)assert(escortTicks[i]==1);
    questTrackerItems.clear();appendAllQuestItems();assert(questTrackerItems.size()==5&&selectedEscortQuest==3);
    Window panel;panel.visible=true;finalWindow=&panel;tickAllQuests(1);assert(escortTicks[0]==1&&!prepareEscortQuest());finalWindow=0;
    developerFinishPicker=&panel;tickAllQuests(1);assert(escortTicks[0]==1&&selectedEscortQuest==3);developerFinishPicker=0;
    // Distinct per-mission rescue/allure state survives context switches and save/load.
    selectEscortQuest(0);missionPace=EscortPace::Accelerated;missionForcedPace=true;missionRescue.retiredLeaders.push_back(people[4].id);missionRescue.retiredLeaders.push_back(people[3].id);
    selectEscortQuest(1);assert(missionRescue.retiredLeaders.empty()&&missionPace==EscortPace::Normal);
    selectEscortQuest(0);assert(missionRescue.retiredLeaders.size()==2&&missionPace==EscortPace::Accelerated);
    selectEscortQuest(3);
    const std::string bytes=savePool();actors.erase(2);loadPool(bytes);assert(activeQuestCount()==5&&selectedEscortQuest==3);selectEscortQuest(1);assert(!escort&&escortHandle.serial==2&&missionReward==1200);tickAllQuests(1);assert(escortTicks[0]==2&&escortTicks[1]==1&&escortTicks[2]==2);actors[2]=&people[1];tickAllQuests(1);assert(escortTicks[1]==2&&people[1].orders>0);
    selectEscortQuest(0);assert(missionPace==EscortPace::Accelerated&&missionForcedPace&&missionRescue.retiredLeaders.size()==2);assert(escortHandle==people[0].id);assert(missionRescue.tasks.empty());
    selectEscortQuest(1);assert(missionRescue.retiredLeaders.empty());
    // Save while an actor is unloaded: its durable identity must survive again.
    actors.erase(2);selectEscortQuest(1);refreshQuestActors();std::string unloaded=savePool();loadPool(unloaded);actors[2]=&people[1];selectEscortQuest(1);refreshQuestActors();assert(escort==&people[1]);
    // Completion, failure and cancellation all release slots immediately.
    for(int i=4;i>=0;--i){selectEscortQuest(i);missionActive=missionPending=false;currentContract.settlementPaid=true;assert(activeQuestCount()==i);questTrackerItems.clear();appendAllQuestItems();assert(questTrackerItems.size()==(size_t)i);}
    assert(prepareEscortQuest());missionPending=true;assert(activeQuestCount()==1);
    clearPool();for(int i=0;i<3;++i){assert(prepareEscortQuest());missionPending=true;escortHandle=people[i].id;escort=&people[i];progressMembers.assign(1,escortHandle);currentMissionFiscalId=allocateQuestFiscalId();}
    for(int i=0;i<2;++i){assert(prepareBountyQuest());assert(bountyWorld.contract.accept(bountyOffer(i)));MercenarieV5::ActorIdentity target;target.serial=20+i;std::vector<MercenarieV5::ActorIdentity> group(1,target);assert(bountyWorld.contract.bindSpawn(target,group));}
    assert(activeQuestCount()==5&&!prepareEscortQuest()&&!prepareBountyQuest());bountyWorld.boards["shared"].rotation=7;selectBountyQuest(0);assert(bountyWorld.boards["shared"].rotation==7);bountyWorld.boards["shared"].rotation=8;selectBountyQuest(1);assert(bountyWorld.boards["shared"].rotation==8);
    std::string mixed=savePool();loadPool(mixed);assert(activeQuestCount()==5);selectBountyQuest(0);MercenarieV5::BountyContract& c=bountyWorld.contract;assert(c.beginHandover(c.offer.id,c.offer.issuer,c.target,true,true));assert(c.acknowledgeHandover(true,true));assert(!c.acknowledgeHandover(true,true));assert(activeQuestCount()==4);selectBountyQuest(1);assert(!bountyWorld.contract.paymentRecorded&&bountyWorld.contract.state==MercenarieV5::BountyActive);assert(prepareEscortQuest());missionPending=true;assert(activeQuestCount()==5);
    bool rejected=false;try{loadPool(mixed.substr(0,mixed.size()-1));}catch(...){rejected=true;}assert(rejected);
    clearPool();assert(prepareEscortQuest());missionActive=true;escort=&people[0];escortHandle=people[0].id;caravanMission=true;
    progressMembers.assign(1,hand(999));caravanRestore.identities=progressMembers;caravanRestore.pending=true;
    const int waitingSlot=selectedEscortQuest;
    assert(prepareEscortQuest());missionActive=true;escort=&people[1];escortHandle=people[1].id;progressMembers.assign(1,escortHandle);
    const int readySlot=selectedEscortQuest,waitingTicks=escortTicks[waitingSlot],readyTicks=escortTicks[readySlot];
    tickAllQuests(0.1f);assert(escortTicks[waitingSlot]==waitingTicks&&escortTicks[readySlot]==readyTicks+1);
    std::string pendingSave=savePool();loadPool(pendingSave);selectEscortQuest(waitingSlot);assert(caravanRestore.pending&&caravanRestore.identities[0].serial==999);
    Character late(999);actors[999]=&late;tickAllQuests(1.0f);assert(!caravanRestore.pending||selectedEscortQuest!=waitingSlot);
    selectEscortQuest(waitingSlot);assert(!caravanRestore.pending&&caravanMembers.size()==1&&caravanMembers[0]==&late);

    // Real archive round trips in every business phase, including dirty runtime state.
    for(int phase=0;phase<8;++phase){
        clearPool();assert(prepareEscortQuest());missionActive=true;escort=&people[0];escortHandle=people[0].id;progressMembers.assign(1,escortHandle);
        currentMissionFiscalId="motion-save";leavingBuilding=phase==0;scientificMission=phase==3||phase==4;
        scientificResearching=phase==3;scientificReturning=phase==4;caravanReturning=phase==5;
        wasInCombat=phase==2;missionPaused=phase==6;missionFollowing=phase==7;missionFollowTarget=people[1].id;
        exitWaypoint.x=55;destination.x=900;scientificResearchSeconds=127;
        missionRescue.leaderWatch.step=4;missionRescue.leaderWatch.initialized=true;missionRescue.routeRetry.attempts=2;
        missionRescue.regrouping=phase==1;missionRescue.motionSuspended=phase==6;missionRescue.roadPlanned=true;
        const std::string snapshot=savePool();loadPool(snapshot);
        assert(leavingBuilding==(phase==0)&&scientificResearching==(phase==3)&&scientificReturning==(phase==4)&&caravanReturning==(phase==5));
        assert(wasInCombat==(phase==2)&&missionPaused==(phase==6)&&missionFollowing==(phase==7));
        assert(destination.x==900&&exitWaypoint.x==55&&scientificResearchSeconds==127);
        assert(!missionRescue.leaderWatch.initialized&&missionRescue.leaderWatch.step==0&&missionRescue.routeRetry.attempts==0&&!missionRescue.roadPlanned);
        assert(missionRescue.ownedOrders.empty()&&missionRescue.tasks.empty()&&!missionRescue.regrouping);
    }
    std::cout<<"PASS movement save/load: exit, outbound, regroup, combat, science, science return, caravan return, suspended recovery and FOLLOW; fresh native measurements.\n";
    std::cout<<"PASS domain isolation: ready quest ticks while another roster waits; unresolved roster survives save/load and resumes with the same identity.\n";
    std::cout<<"PASS: production quest pool: 5 escorts, mixed 3+2, sixth rejected FR/EN, isolated data/bonus selections, unique IDs, per-slot ticks, shared boards, report lock, save/load, unloaded actors, slot release, bounty single settlement, truncated save rejection\n";
}
