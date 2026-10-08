#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <cmath>
struct Character;
struct hand{Character* c;hand(Character* p=0):c(p){} bool isNull()const{return !c;}void setNull(){c=0;}Character* getCharacter()const{return c;}bool operator==(const hand& h)const{return c==h.c;}std::string toString()const{std::ostringstream s;s<<c;return s.str();}};
template<class T> struct lektor:std::vector<T>{};
struct Vec{static const Vec ZERO;float x,y,z;Vec(float a=0):x(a),y(0),z(0){}float squaredDistance(const Vec& b)const{return (x-b.x)*(x-b.x);}};
std::ostream& operator<<(std::ostream& o,const Vec& v){return o<<v.x;}
const Vec Vec::ZERO;
namespace Ogre{typedef Vec Vector3;}
enum {MELEE_ATTACK,FOCUSED_MELEE_ATTACK,UNPROVOKED_FOCUSED_MELEE_ATTACK,CHOOSE_ENEMY_AND_ATTACK,CHOOSE_ATTACKER_OF_ALLY,ATTACK_CHARACTERS_ATTACKER,ATTACK_ATTACKERS_OF,ATTACK_ENEMIES_AND_NEUTRALS,RANGED_ATTACK,RANGED_ATTACK_FOCUSED,EV_PLAYER_TALK_TO_ME,PICKUP,LIFT_PERSON_PLAYER_ORDER,FIRST_AID_ORDER,MOVE_CUS_ORDERED,HOLD_POSITION,FOLLOW_PLAYER_ORDER,FOLLOW_SQUADLEADER,BODYGUARD,ITEM_FIRSTAID,ITEM,DIALOGUE,DIALOGUE_PACKAGE,JOG,WALK,JOB_MEDIC,NULL_TASK,PATROL,WANDERER,PATROL_TOWN,WANDER_TOWN,WANDERING_TRADER,SHOPPING,RELAX_IN_TOWN_PACKAGE,GO_HOMEBUILDING,STAY_IN_HOME,TRAVEL_TO_TARGET_TOWN,ATTACK_ENEMIES,RUN,UNLOCK_DOOR_HERE,RUN_AWAY,RUN_AWAY_HOMETOWN,RUN_AWAY_FORCED,TOTAL_ESCAPE,ESCAPE_KIDNAP,ESCAPE_KIDNAP_STR,FOLLOW_URGENT_ESCAPE,SELF_PRESERVATION};
typedef int MoveSpeed;typedef int TaskType;enum {HIGH_PRIORITY};
#include "MissionNativeGoalRules.h"
namespace EscortPace{enum State{Normal,Accelerated};}
struct GameData{};struct Item{float chargesLeft;Item():chargesLeft(100){}};
struct Inventory{std::vector<Item*> items;void getAllItemsWithFunction(lektor<Item*>& out,int){out.assign(items.begin(),items.end());}};
struct Medical{bool ko,dead,cripple,dying;float refreshedNeed;void precalculateFirstAidNeedScore(){if(refreshedNeed>=0)need=refreshedNeed;}float need;Medical():refreshedNeed(-1),ko(false),dead(false),cripple(false),dying(false),need(0){}bool isUnconcious(){return ko;}bool canGetUpWakeUp(){return !ko;}bool isCrippled(){return cripple;}bool isProbablyDying(){return dying;}float scoreFirstAidNeed(bool){return need;}};
struct Stats{float speed;Stats():speed(8){}float getMaxRunSpeed(){return speed;}};
struct Movement{bool moving;int halts,retargets;void setDestination(const Vec& p,int,bool){pathDestination=p;++retargets;}int orderMode;Vec pathDestination;float roadWeight;void* roadFollower;Vec getDestination(){return Vec();}bool isCurrentlyMoving(){return moving;}bool pathFailed(){return false;}float desired,roadPreference;int invalidations;Movement():moving(false),halts(0),retargets(0),orderMode(-1),roadWeight(0),roadFollower(0),desired(-1),roadPreference(0),invalidations(0){}void setRoadPreference(float v){roadPreference=v;}void invalidatePath(){++invalidations;}void restoreDesiredSpeed(){desired=8;}void leaveSpeedGroup(){}void setDesiredSpeedOrders(int mode){orderMode=mode;}void setDesiredSpeedOrders(float v){desired=v;}void setDesiredSpeed(float v){desired=v;}float getStandardWalkSpeed(){return 3;}void halt(){++halts;moving=false;}};
struct Platoon{Character* leader;void setSquadLeader(Character* c){leader=c;}};
struct Dialogue{bool canTalk,ended,_hasEnded;Character* me;Dialogue():canTalk(true),ended(false),_hasEnded(false),me(0){}void endDialogue(bool){ended=true;_hasEnded=true;}void clearConversationList(int event){assert(event==EV_PLAYER_TALK_TO_ME);canTalk=false;}void addConversation(GameData*,int event){assert(event==EV_PLAYER_TALK_TO_ME);canTalk=true;}void addDialoguePackage(GameData*){}};
struct Tasker{TaskType type;hand subject;Vec location;Tasker(TaskType t):type(t){}TaskType key(){return type;}};
struct CombatClass{bool wantsToBlock;CombatClass():wantsToBlock(false){}};
struct CharBody{CombatClass combat;CombatClass* getCombatClass(){return &combat;}Tasker* currentAction;CharBody():currentAction(0){}};
struct FakeQueue{std::vector<Tasker*> list;};
struct FakeTaskSystem{FakeQueue orders,actions,actionsTryList;Tasker* currentlySubTasking;int nativeGoal;int explicitCount;Character* character;Character* actor;std::set<int>* orderSet;FakeTaskSystem():currentlySubTasking(0),nativeGoal(NULL_TASK),character(0),explicitCount(0),actor(0),orderSet(0){}void addOrder(int,const hand&,const Vec&,bool,bool);bool hasPlayerOrders(){return !orders.list.empty();}bool hasPlayerOrder(int t){for(size_t i=0;i<orders.list.size();++i)if(orders.list[i]->key()==t)return true;return orderSet&&orderSet->count(t)>0;}bool hasGoal(int t){return orderSet&&orderSet->count(t)>0;}bool hasPermajob(int){return false;}void removeGoal(int t){if(orderSet)orderSet->erase(t);}void clearOrders(){if(orderSet)orderSet->clear();}void clearCurrentGoal(bool){nativeGoal=NULL_TASK;}struct Goal{hand subject;int type;int key(){return type;}};Goal getCurrentGoal(){Goal g;g.type=nativeGoal;for(size_t i=0;i<orders.list.size();++i)if(orders.list[i]->key()==nativeGoal){g.subject=orders.list[i]->subject;break;}return g;}};
typedef FakeTaskSystem AITaskSytem;
struct FakeAI{bool incoming;FakeAI():incoming(false){}bool iAmBeingMeleeAttacked_general(const hand&,const Vec&){return incoming;}enum AIManuverabilityOrders{HOLD_GROUND,STAY_CLOSE,ROAM_FAR};int movement;Vec center;void setManuveringFreedomLevel(AIManuverabilityOrders v){movement=v;}void setCenterOfMovement(Vec p){center=p;}void setCenterOfMovementTarget(hand){}FakeTaskSystem tasks;FakeTaskSystem* getTaskSystem(){return &tasks;}};
typedef FakeAI AI;
struct Character{void sayALine(const std::string&,bool){}CharBody body;CharBody* getBody(){return &body;}
    FakeAI ai;FakeAI* getAI(){return &ai;}
    Medical med;Stats stats;Inventory inv;Movement move;Platoon platoon;Dialogue* dialogue;
    bool player;bool animal,carried,combat,isCarryingSomething;Character* cargo;Vec pos;std::set<int> jobs;int aidOrders;Character* aidTarget;Character* orderTarget;
    Character():player(false),dialogue(0),animal(false),carried(false),combat(false),isCarryingSomething(false),cargo(0),aidOrders(0),aidTarget(0),orderTarget(0){ai.tasks.orderSet=&jobs;ai.tasks.actor=this;ai.tasks.character=this;}
    bool isPlayerCharacter()const{return player;}bool isDead(){return med.dead;}bool isAnimal(){return animal;}bool isBeingCarried(){return carried;}bool isDown(){return med.ko||med.cripple;}bool isInCombatMode(bool,bool){return combat;}
    Medical* getMedical(){return &med;}Stats* getStats(){return &stats;}Inventory* getInventory(){return &inv;}Movement* getMovement(){return &move;}Platoon* getPlatoon(){return &platoon;}Vec getPosition(){return pos;}hand getHandle(){return hand(this);}void getAllAttackers(lektor<hand>&){}
    void removeJob(int j){jobs.erase(j);}void addJob(int j,Character* c,bool,bool,Vec){jobs.insert(j);orderTarget=c;if(j==FIRST_AID_ORDER){++aidOrders;aidTarget=c;}}
    void pickupObject(Character* c){assert(!isCarryingSomething&&!c->carried);cargo=c;isCarryingSomething=true;c->carried=true;}
    void dropCarriedObject(bool,bool){assert(cargo);cargo->carried=false;cargo=0;isCarryingSomething=false;}
    bool giveItem(Item* i,bool drop,bool destroy){assert(!drop&&destroy);inv.items.push_back(i);return true;}
};
void FakeTaskSystem::addOrder(int type,const hand& target,const Vec& pos,bool clear,bool shift){assert(!shift);++explicitCount;if(clear)clearOrders();Tasker* t=new Tasker(type);t->subject=target;t->location=pos;orders.list.push_back(t);actor->orderTarget=target.c;if(type==FIRST_AID_ORDER){++actor->aidOrders;actor->aidTarget=target.c;}}
struct Factory{Item* createItem(GameData*,hand,int,int,int,int){return new Item;}};
struct Data{GameData data;GameData* getData(const char*,int){return &data;}};
struct World{Factory factory;Factory* theFactory;Data gamedata;World():theFactory(&factory){}} world;World* ou=&world;
void DebugLog(const std::string&){}void ErrorLog(const std::string&){}
std::string currentMissionFiscalId="test";Character* escort=0;hand escortHandle;Character* missionTemporaryLeader=0;
std::vector<hand> progressMembers;std::set<unsigned int> progressDeadMembers;
bool missionActive=true;bool missionPaused=false,missionFollowing=false,waitingForPlayer=false,scientificResearching=false,missionCasualtyWaiting=false;
hand missionFollowTarget;
EscortPace::State missionPace=EscortPace::Normal;Vec destination;
bool missionPending=false;struct Window{bool visible;Window():visible(false){}bool getVisible(){return visible;}};Window* finalWindow=0;
#include "MissionRescueState.h"
#include "MissionMovementOrders.h"
bool missionFollowerTargetValid(Character* c,Character* leader,bool approach){for(size_t i=0;i<c->ai.tasks.orders.list.size();++i){Tasker* t=c->ai.tasks.orders.list[i];if(approach?t->key()==MOVE_CUS_ORDERED:t->key()==FOLLOW_PLAYER_ORDER&&t->subject.c==leader)return true;}return false;}
void missionSuspendMotion(){missionRescue.motionSuspended=true;missionPaused=true;}
void missionWaitReason(MissionMotionPolicy::Reason reason){missionRescue.waitReason=reason;}
void missionRecoverFollower(Character* c,Character* leader,MissionMotionPolicy::Action a,bool approach){if(a==MissionMotionPolicy::Wait)return;if(a==MissionMotionPolicy::Suspend){missionSuspendMotion();return;}missionClearTravel(c,"test recovery");missionIssueOrder(c,approach?MOVE_CUS_ORDERED:FOLLOW_PLAYER_ORDER,approach?0:leader,leader->getPosition());}
#include "MissionCombatResponse.h"
#include "MissionFormationRuntime.h"
Vec lastTravelTarget;int travelOrders=0;void issueTravelOrder(Vec target,const char* reason="test route"){lastTravelTarget=target;++travelOrders;missionClearTravel(escort,"test route");missionIssueOrder(escort,MOVE_CUS_ORDERED,0,target);updateMissionFormation(0);}
Character* missionGroupCarrierOf(Character* c){for(size_t i=0;i<progressMembers.size();++i)if(progressMembers[i].c->cargo==c)return progressMembers[i].c;return 0;}

#include "MissionRescueRuntime.h"
std::vector<hand> missionFollowers;std::vector<int> waitingHere;Vec exitWaypoint;
#include "MissionAICleanup.h"
#include "MissionRescuePace.h"
enum {CONTRACT_ACTIVE,CONTRACT_NONE};int contractLifecycle=CONTRACT_ACTIVE;
struct RecoveryContract{bool settlementPaid;RecoveryContract():settlementPaid(false){}}currentContract;
float stationaryClock=0,incidentClock=0,scientificMoveClock=0;
bool scientificWillEnter=false,scientificEntryResolved=false,leavingBuilding=false;
Vec scientificRuinCenter;
bool findExteriorWaypoint(Character*,Vec& target){target=exitWaypoint;return true;}
bool missionScienceSelectBuilding(){return true;}Vec missionScienceEntryPoint(){return scientificRuinCenter;}
#include "MissionRecovery.h"
// Exercise the production native-goal hook with scored autonomous candidates,
// including two independently active quest slots and a simulated load boundary.

struct TaskMatch{Tasker* task;TaskMatch(Tasker* t):task(t){}TaskType key(){return task->type;}};
typedef std::map<float,Tasker*> MissionNativeGoalScores;
bool missionWorldChanging=false,missionRestorePending=false;
const int maximumActiveQuests=2;int selectedEscortQuest=0;
struct FakeQuest{bool v_missionPaused;bool v_missionCasualtyWaiting;EscortPace::State v_missionPace;MissionRescueState v_missionRescue;bool v_missionActive;hand v_escortHandle;std::vector<hand> v_progressMembers;std::string v_currentMissionFiscalId;FakeQuest():v_missionPaused(false),v_missionCasualtyWaiting(false),v_missionPace(EscortPace::Normal),v_missionActive(false){}};
FakeQuest escortQuests[maximumActiveQuests];
unsigned long GetTickCount(){static unsigned long now=0;return now+=6000;}
#include "MissionNativeGoalRuntime.h"
#include "MissionOffscreenRuntime.h"
bool nativeOffscreenResult=false;
bool nativeOffscreenDecision(Character*){return nativeOffscreenResult;}
void offscreenRegression(){
    Character leader,member,otherLeader,outsider,player;player.player=true;
    escortHandle=&leader;progressMembers.clear();progressMembers.push_back(&member);progressMembers.push_back(&player);
    missionActive=true;escortQuests[1].v_missionActive=true;escortQuests[1].v_escortHandle=&otherLeader;
    missionOffscreenImmuneOriginal=nativeOffscreenDecision;
    assert(missionOffscreenImmuneHook(&leader)&&missionOffscreenImmuneHook(&member)&&missionOffscreenImmuneHook(&otherLeader));
    assert(!missionOffscreenImmuneHook(&outsider)&&!missionOffscreenImmuneHook(&player));
    member.med.dead=true;assert(!missionOffscreenImmuneHook(&member));member.med.dead=false;
    missionRestorePending=true;assert(!missionOffscreenImmuneHook(&leader));missionRestorePending=false;
    missionWorldChanging=true;assert(!missionOffscreenImmuneHook(&leader));missionWorldChanging=false;
    missionActive=false;assert(!missionOffscreenImmuneHook(&leader)&&missionOffscreenImmuneHook(&otherLeader));
    escortQuests[1].v_missionActive=false;assert(!missionOffscreenImmuneHook(&otherLeader));
    nativeOffscreenResult=true;assert(missionOffscreenImmuneHook(&outsider)&&missionOffscreenImmuneHook(&player));
    nativeOffscreenResult=false;missionActive=true;progressMembers.clear();escortHandle.setNull();
}
TaskType lastNativeChoice=NULL_TASK;
bool simulateNativeGoals(AITaskSytem* tasks,MissionNativeGoalScores& goals,bool playerOrder){
    lastNativeChoice=goals.empty()?NULL_TASK:goals.rbegin()->second->type;
    if(!playerOrder&&missionConflictingNativeGoal(lastNativeChoice)){
        tasks->clearOrders();tasks->nativeGoal=lastNativeChoice;
        tasks->actor->pos=Vec(tasks->actor->pos.x-60);
    }
    return !goals.empty();
}
void nativeGoalRegression(){
    Character leader,member,otherLeader,outsider,player;player.player=true;
    escort=&leader;escortHandle=&leader;progressMembers.clear();progressMembers.push_back(&member);progressMembers.push_back(&player);
    missionActive=true;escortQuests[1].v_missionActive=true;escortQuests[1].v_escortHandle=&otherLeader;
    escortQuests[1].v_currentMissionFiscalId="second";missionRunGoalsOriginal=simulateNativeGoals;
    Tasker patrol(PATROL_TOWN),relax(RELAX_IN_TOWN_PACKAGE),fight(ATTACK_ENEMIES),aid(FIRST_AID_ORDER),move(MOVE_CUS_ORDERED);
    MissionNativeGoalScores goals;goals[100]=&patrol;goals[90]=&relax;goals[80]=&fight;goals[70]=&aid;
    assert(missionRunGoalsHook(&leader.ai.tasks,goals,false));assert(goals.size()==2&&lastNativeChoice==ATTACK_ENEMIES);
    goals.clear();goals[100]=&patrol;assert(!missionRunGoalsHook(&otherLeader.ai.tasks,goals,false)&&goals.empty());
    goals[100]=&patrol;missionRunGoalsHook(&outsider.ai.tasks,goals,false);assert(lastNativeChoice==PATROL_TOWN);
    missionRunGoalsHook(&player.ai.tasks,goals,false);assert(lastNativeChoice==PATROL_TOWN);
    missionRunGoalsHook(&member.ai.tasks,goals,true);assert(lastNativeChoice==PATROL_TOWN); // explicit orders unchanged
    missionRestorePending=true;missionRunGoalsHook(&member.ai.tasks,goals,false);assert(lastNativeChoice==PATROL_TOWN);missionRestorePending=false;
    missionWorldChanging=true;missionRunGoalsHook(&member.ai.tasks,goals,false);assert(lastNativeChoice==PATROL_TOWN);missionWorldChanging=false;
    missionActive=false;missionRunGoalsHook(&member.ai.tasks,goals,false);assert(lastNativeChoice==PATROL_TOWN);missionActive=true;
    member.ai.tasks.nativeGoal=PATROL_TOWN;missionClearTravel(&member,"unowned town goal preserved");assert(member.ai.tasks.nativeGoal==PATROL_TOWN);
    member.ai.tasks.nativeGoal=ATTACK_ENEMIES;missionClearTravel(&member,"combat remains native");assert(member.ai.tasks.nativeGoal==ATTACK_ENEMIES);
    missionRescue=MissionRescueState();missionIssueOrder(&leader,MOVE_CUS_ORDERED,0,Vec(1000));
    const int issued=leader.ai.tasks.explicitCount;
    for(int second=0;second<60;++second){goals.clear();goals[100]=second%2?&patrol:&relax;assert(!missionRunGoalsHook(&leader.ai.tasks,goals,false));leader.pos=Vec(leader.pos.x+5);assert(leader.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED));}
    assert(leader.pos.x==300&&leader.ai.tasks.explicitCount==issued); // no periodic fight over the route
    goals.clear();goals[100]=&move;assert(missionRunGoalsHook(&leader.ai.tasks,goals,true)&&lastNativeChoice==MOVE_CUS_ORDERED);
    missionHoldTravel(&leader,"regroup");leader.pos=Vec(350);missionHoldTravel(&leader,"displaced");assert(!leader.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&leader.ai.tasks.hasPlayerOrder(HOLD_POSITION)&&leader.ai.center.x==350);
    escortQuests[1].v_missionActive=false;
}
void setup(Character& a,Character& b,Character& c,Character& d){missionActive=true;missionPending=false;waitingForPlayer=false;scientificResearching=false;finalWindow=0;missionRescue=MissionRescueState();missionGroup.reset();progressMembers.clear();progressDeadMembers.clear();escort=&a;escortHandle=&a;missionCasualtyWaiting=false;missionPaused=missionFollowing=false;progressMembers.push_back(&a);progressMembers.push_back(&b);progressMembers.push_back(&c);progressMembers.push_back(&d);for(size_t i=0;i<progressMembers.size();++i)giveMissionSpawnKits(progressMembers[i].c);}
struct DialogLineData{std::string getStringID(){return "unrelated-line";}};
bool isContractGreeting(const std::string& id){return id=="880026-Guild Escort Contracts.mod"||id=="880034-Guild Escort Contracts.mod"||id=="880044-Guild Escort Contracts.mod";}
namespace Loc {const char* text(const char* key){return std::string(key)=="mission.native.time_to_go"?"Il est temps de partir.":"On dirait que c'est fini...";}}
#include "MissionSpeechRuntime.h"
std::string spokenText;DialogLineData* spokenData=0;
void captureSpeech(Dialogue*,const std::string& text,DialogLineData* line){spokenText=text;spokenData=line;}

typedef Movement AbstractMovementBase;
#include "MissionPaceGuard.h"
int guardedMode=-1;float guardedSpeed=-1;
void captureSpeedMode(AbstractMovementBase*,MoveSpeed value){guardedMode=value;}
void captureSpeedFloat(AbstractMovementBase*,float value){guardedSpeed=value;}
int main(){
    {Character a,b,c,d;setup(a,b,c,d);progressMembers.resize(1);a.med.need=30;
        assert(tickMissionRescue(1));assert(!a.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));
        assert(tickMissionRescue(1));assert(missionRescue.tasks.size()==1&&missionRescue.tasks[0].helper.c==&a);
        assert(a.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));assert(!a.ai.tasks.hasPlayerOrder(HOLD_POSITION));
        assert(tickMissionRescue(5));assert(a.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));
        int before=travelOrders;a.med.need=0;assert(!tickMissionRescue(1));assert(!missionCasualtyWaiting&&missionRescue.tasks.empty());assert(travelOrders==before+1);
        assert(!tickMissionRescue(1));assert(travelOrders==before+1);
    }
    {Character a,b,c,d;setup(a,b,c,d);progressMembers.resize(1);a.med.ko=true;a.med.need=30;
        assert(tickMissionRescue(3));assert(!a.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));
        a.med.ko=false;assert(tickMissionRescue(2));assert(a.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));
    }
    std::cout<<"PASS solo self-care: safe delay, own kit, HOLD released, treatment retained, resume once, unconscious rejection\n";

    {
        Character actor;missionActive=true;actor.move.moving=true;
        actor.jobs.insert(MOVE_CUS_ORDERED);actor.ai.tasks.nativeGoal=MOVE_CUS_ORDERED;
        Tasker bodyAction(MOVE_CUS_ORDERED);actor.body.currentAction=&bodyAction;
        Tasker order(MOVE_CUS_ORDERED),action(MOVE_CUS_ORDERED),sub(MOVE_CUS_ORDERED),unrelated(MOVE_CUS_ORDERED);unrelated.location=Vec(900);
        actor.ai.tasks.orders.list.push_back(&order);actor.ai.tasks.actions.list.push_back(&action);actor.ai.tasks.actionsTryList.list.push_back(&unrelated);actor.ai.tasks.currentlySubTasking=&sub;
        MissionRescueState::OwnedOrder owned;owned.actor=actor.getHandle();owned.type=MOVE_CUS_ORDERED;owned.position=order.location;missionRescue.ownedOrders.push_back(owned);
        assert(missionContinueTravel(&actor,Vec(120)));
        assert(actor.ai.tasks.explicitCount==0&&actor.ai.tasks.nativeGoal==MOVE_CUS_ORDERED&&order.location.x==120&&action.location.x==120&&sub.location.x==120&&unrelated.location.x==900);
        assert(actor.move.halts==0&&actor.move.invalidations==0&&actor.move.retargets==1&&actor.move.pathDestination.x==120);
        actor.combat=true;assert(!missionContinueTravel(&actor,Vec(180)));actor.combat=false;
        actor.ai.tasks.nativeGoal=FIRST_AID_ORDER;assert(!missionContinueTravel(&actor,Vec(180)));
        actor.ai.tasks.nativeGoal=MOVE_CUS_ORDERED;actor.move.moving=false;assert(!missionContinueTravel(&actor,Vec(180)));
        assert(actor.move.retargets==1);
        actor.move.moving=true;
        for(int i=1;i<=60;++i){assert(missionContinueTravel(&actor,Vec(120+i*60)));assert(order.location.x==120+i*60&&action.location.x==order.location.x&&sub.location.x==order.location.x&&bodyAction.location.x==order.location.x);}
        assert(actor.ai.tasks.explicitCount==0&&actor.ai.tasks.nativeGoal==MOVE_CUS_ORDERED&&actor.move.halts==0&&actor.move.invalidations==0);
        bodyAction.type=FIRST_AID_ORDER;const float medicalTarget=bodyAction.location.x;
        assert(missionContinueTravel(&actor,Vec(5000))&&bodyAction.location.x==medicalTarget); // do not rewrite a medical action
        actor.ai.tasks.orders.list.push_back(&unrelated);assert(!missionContinueTravel(&actor,Vec(9999))); // preserve a queued order
    }
    offscreenRegression();
    nativeGoalRegression();
    {Character a,b,c,d;setup(a,b,c,d);b.med.ko=c.med.ko=true;b.pos=Vec(1);c.pos=Vec(20);d.pos=Vec(28);
        assert(tickMissionRescue(2));assert(a.cargo==&b);assert(d.ai.tasks.hasPlayerOrder(LIFT_PERSON_PLAYER_ORDER)&&d.orderTarget==&c);
        int issued=d.ai.tasks.explicitCount;for(int i=0;i<8;++i){assert(tickMissionRescue(1));assert(d.ai.tasks.hasPlayerOrder(LIFT_PERSON_PLAYER_ORDER));}
        assert(d.ai.tasks.explicitCount==issued&&!c.carried); // native carry goal approach is not restarted
        d.pickupObject(&c);tickMissionRescue(1);assert(!missionCasualtyWaiting&&a.cargo==&b&&d.cargo==&c);
        updateMissionFormation(1);assert(d.orderTarget==&a&&d.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER));
    }
    {Character a,b,c,d;setup(a,b,c,d);destination=Vec(1000);b.pos=Vec(110);updateMissionFormation(1);
        assert(b.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER));assert(tickMissionRescue(2));
        assert(b.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&!b.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER));
        int issued=b.ai.tasks.explicitCount;tickMissionRescue(1);assert(b.ai.tasks.explicitCount==issued); // do not restart approach
        b.pos=Vec(60);assert(tickMissionRescue(1));assert(b.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)); // native FOLLOW can stop here
        int routes=travelOrders;b.pos=Vec(40);assert(!tickMissionRescue(1));updateMissionFormation(1);
        assert(travelOrders==routes+1&&a.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED));
        assert(b.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER)&&!b.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&b.orderTarget==&a);
        for(int i=0;i<20;++i){a.pos=Vec(float(i*5));b.pos=Vec(a.pos.x+40);c.pos=d.pos=a.pos;assert(!tickMissionRescue(1));updateMissionFormation(1);}
        assert(travelOrders==routes+1&&destination.x==1000);
    }
    {Character a,b,c,d;setup(a,b,c,d);assert(a.inv.items.size()==4&&d.inv.items.size()==4);b.med.ko=true;b.med.need=5;a.pos=100;c.pos=1;d.pos=10;assert(tickMissionRescue(2));assert(c.aidTarget==&b&&a.aidOrders==0);assert(b.med.need==5);tickMissionRescue(1);assert(c.aidOrders==1&&c.inv.items.size()==4);b.med.need=0;tickMissionRescue(1);assert(c.cargo==&b);b.med.ko=false;tickMissionRescue(1);assert(!b.carried&&!c.cargo);}
    {Character a,b,c,d;setup(a,b,c,d);a.med.ko=b.med.ko=true;a.med.need=b.med.need=5;tickMissionRescue(2);assert(c.aidTarget&&d.aidTarget&&c.aidTarget!=d.aidTarget);c.combat=true;tickMissionRescue(1);assert(!c.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER)&&!d.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));c.combat=false;tickMissionRescue(1);assert(!c.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));tickMissionRescue(1);assert(c.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER)&&d.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));a.med.need=b.med.need=0;tickMissionRescue(1);assert(a.carried&&b.carried);assert(escort==&c);a.med.ko=false;b.med.ko=false;tickMissionRescue(1);assert(escort==&c);c.med.ko=true;tickMissionRescue(1);assert(escort==&b||escort==&d);assert(escort!=&a);}
    {Character a,b,c,d;setup(a,b,c,d);b.med.ko=true;b.med.need=5;for(size_t i=0;i<progressMembers.size();++i)progressMembers[i].c->inv.items.clear();assert(tickMissionRescue(2));assert(a.aidOrders+c.aidOrders+d.aidOrders==0);b.med.need=0;b.carried=true;tickMissionRescue(1);assert(!a.cargo&&!c.cargo&&!d.cargo);b.carried=false;assert(tickMissionRescue(1));assert(a.cargo==&b);}
    {Character a,b,c,d;setup(a,b,c,d);a.med.dead=true;tickMissionRescue(2);assert(escort==&b);b.med.dead=true;tickMissionRescue(1);assert(escort==&c);assert(a.aidOrders==0);}
    {Character a,b,c,d;setup(a,b,c,d);a.med.ko=b.med.ko=c.med.ko=d.med.ko=true;assert(tickMissionRescue(2));assert(!a.cargo&&!b.cargo);b.med.ko=false;tickMissionRescue(1);assert(escort==&b);assert(missionCasualtyWaiting);}
    {Character a,b,c,d,outsider;setup(a,b,c,d);b.med.ko=true;b.med.need=5;a.pos=100;c.pos=10;d.pos=30;giveMissionSpawnKits(&outsider);tickMissionRescue(2);assert(c.aidTarget==&b&&outsider.aidOrders==0);c.med.ko=true;tickMissionRescue(1);assert(d.aidTarget==&b);clearMissionRescue();assert(missionRescue.tasks.empty()&&!d.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));}
    {Character a,b,c,d;setup(a,b,c,d);missionPace=EscortPace::Accelerated;b.stats.speed=4;applyMissionPace();assert(fabs(a.move.desired-3.4f)<.001f&&d.move.desired==4&&missionPace==EscortPace::Accelerated);b.stats.speed=6;applyMissionPace();assert(fabs(a.move.desired-5.1f)<.001f);b.carried=true;c.stats.speed=5;applyMissionPace();assert(fabs(a.move.desired-4.25f)<.001f);missionPace=EscortPace::Normal;applyMissionPace();assert(a.move.desired==3&&a.move.orderMode==WALK);}
    {Character a,b,c,d;setup(a,b,c,d);b.pos=110;assert(tickMissionRescue(2)&&missionRescue.regrouping);b.pos=60;assert(tickMissionRescue(1));b.pos=40;assert(!tickMissionRescue(1));}
    {Character a,b,c,d;setup(a,b,c,d);b.pos=110;missionRescue.gateClearing=true;missionRescue.gateClearSeconds=20;
        assert(!tickMissionRescue(1)&&missionRescue.regrouping&&!missionCasualtyWaiting); // clear the opening before waiting
        missionRescue.gateClearSeconds=0;assert(tickMissionRescue(1)&&missionCasualtyWaiting); // bounded exemption
        missionRescue.gateClearSeconds=20;b.pos=220;assert(tickMissionRescue(1)); // never leave a distant member behind
    }
    {Character a,b,c,d;setup(a,b,c,d);a.med.ko=true;a.carried=true;tickMissionRescue(2);assert(escort==&b);a.carried=false;a.med.ko=false;tickMissionRescue(1);assert(escort==&b);}
    {Character a,b,c,d;setup(a,b,c,d);destination=Vec(1000);missionIssueOrder(&a,MOVE_CUS_ORDERED,0,destination);a.move.roadPreference=1;
        a.med.ko=true;a.med.need=5;assert(tickMissionRescue(2));assert(b.aidTarget==&a);
        a.med.need=0;tickMissionRescue(1);assert(escort==&b&&b.cargo==&a);assert(!a.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&a.move.roadPreference==0);
        tickMissionRescue(1);updateMissionFormation(1);assert(b.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&c.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER));assert(c.aidTarget==0);
        a.med.ko=false;tickMissionRescue(1);updateMissionFormation(1);assert(escort==&b&&!a.carried&&a.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER)&&!a.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED));
        int before=a.move.invalidations;for(int i=0;i<30;++i){a.pos=b.pos=c.pos=d.pos=Vec(float(i*5));tickMissionRescue(1);updateMissionFormation(1);assert(escort==&b&&a.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER)&&!a.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&destination.x==1000);}
        assert(a.move.invalidations==before&&a.orderTarget==&b&&c.orderTarget==&b);assert(a.ai.tasks.orders.list.size()==1&&b.ai.tasks.orders.list.size()==1);
        a.pos=Vec(0);b.pos=c.pos=d.pos=Vec(200);assert(tickMissionRescue(1));assert(missionRescue.regrouping&&a.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&!a.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER));
        a.pos=Vec(190);assert(!tickMissionRescue(1));updateMissionFormation(1);assert(b.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&a.orderTarget==&b&&destination.x==1000);
        a.jobs.clear();updateMissionFormation(5);assert(a.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER)&&a.orderTarget==&b); // missing follow repaired
        // working follow order is never periodically restarted
    }
    {Character a,b,c,d;setup(a,b,c,d);destination=Vec(1000);b.med.ko=c.med.ko=true;b.med.need=c.med.need=5;
        tickMissionRescue(2);b.med.need=c.med.need=0;tickMissionRescue(1);tickMissionRescue(1);
        assert(a.cargo&&d.cargo);d.pos=Vec(150);assert(tickMissionRescue(1));assert(a.ai.tasks.hasPlayerOrder(HOLD_POSITION)&&!a.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED));
        int before=a.move.invalidations;tickMissionRescue(1);assert(a.move.invalidations==before&&a.ai.tasks.hasPlayerOrder(HOLD_POSITION));
        d.pos=Vec(20);assert(!tickMissionRescue(1));updateMissionFormation(1);assert(a.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED)&&!a.ai.tasks.hasPlayerOrder(HOLD_POSITION));assert(d.orderTarget==&a);
        int resets=d.move.invalidations;for(int i=0;i<20;++i){tickMissionRescue(1);updateMissionFormation(1);}assert(d.move.invalidations==resets);assert(d.ai.tasks.explicitCount>0&&a.ai.tasks.explicitCount>0);
    }
    {Character a,b,c,d;setup(a,b,c,d);missionHoldTravel(&a,"test anchor");a.pos=Vec(20);a.jobs.clear();missionHoldTravel(&a,"town goal replaced hold");assert(a.ai.center.x==20&&a.ai.movement==AI::HOLD_GROUND&&a.ai.tasks.hasPlayerOrder(HOLD_POSITION)&&!a.ai.tasks.hasPlayerOrder(MOVE_CUS_ORDERED));assert(missionRescue.holds[0].position.x==20);missionIssueOrder(&a,MOVE_CUS_ORDERED,0,Vec(1000));assert(missionRescue.holds.empty()&&a.ai.movement==AI::ROAM_FAR);}
    // A cached positive score must not keep a fully bandaged KO in first aid.
    {Character a,b,c,d;setup(a,b,c,d);a.pos=100;c.pos=8;d.pos=30;b.med.ko=true;b.med.need=5;
        tickMissionRescue(2);assert(c.aidTarget==&b);
        c.ai.tasks.nativeGoal=FIRST_AID_ORDER;b.med.refreshedNeed=0;
        tickMissionRescue(1);assert(b.med.need==0&&!c.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER));
        assert(c.ai.tasks.nativeGoal==NULL_TASK&&c.ai.tasks.hasPlayerOrder(LIFT_PERSON_PLAYER_ORDER));
        int issued=c.ai.tasks.explicitCount;tickMissionRescue(1);assert(c.ai.tasks.explicitCount==issued);
        c.pickupObject(&b);tickMissionRescue(1);assert(!missionCasualtyWaiting);
    }
    // Reproduce the captured session: no wounds, still KO, helpers ~21/24
    // units away, and a queued PICKUP action that never became a native goal.
    {Character a,b,c,d;setup(a,b,c,d);a.med.ko=d.med.ko=true;
        b.pos=21;c.pos=24;d.pos=48;b.jobs.insert(PICKUP);c.jobs.insert(PICKUP);
        assert(tickMissionRescue(2));assert(b.ai.tasks.hasPlayerOrder(LIFT_PERSON_PLAYER_ORDER)&&c.ai.tasks.hasPlayerOrder(LIFT_PERSON_PLAYER_ORDER));
        assert(!b.jobs.count(PICKUP)&&!c.jobs.count(PICKUP)&&!b.aidOrders&&!c.aidOrders);
        int issued=b.ai.tasks.explicitCount+c.ai.tasks.explicitCount;
        for(int i=0;i<12;++i)tickMissionRescue(1);
        assert(b.ai.tasks.explicitCount+c.ai.tasks.explicitCount==issued);
        b.pickupObject(&a);c.pickupObject(&d);tickMissionRescue(1);
        assert(!missionCasualtyWaiting&&escort==&b&&b.cargo==&a&&c.cargo==&d);
    }
    // Newly inflicted wounds must preempt an existing pickup despite stale zero.
    {Character a,b,c,d;setup(a,b,c,d);a.pos=100;c.pos=8;d.pos=30;b.med.ko=true;
        tickMissionRescue(2);assert(c.ai.tasks.hasPlayerOrder(LIFT_PERSON_PLAYER_ORDER));c.ai.tasks.nativeGoal=LIFT_PERSON_PLAYER_ORDER;
        b.med.refreshedNeed=5;tickMissionRescue(1);
        assert(c.aidTarget==&b&&c.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER)&&!c.ai.tasks.hasPlayerOrder(LIFT_PERSON_PLAYER_ORDER)&&c.ai.tasks.nativeGoal==NULL_TASK);
    }
    assert(std::string(missionSpeechKey("Time for me to go"))=="mission.native.time_to_go");
    assert(std::string(missionSpeechKey("Looks like it's over..."))=="mission.native.looks_over");
    assert(!missionSpeechKey("An unrelated line"));
    {Character a,b,c,d,outsider;setup(a,b,c,d);missionSayOriginal=&captureSpeech;Dialogue speech;DialogLineData line;
        speech.me=&b;missionSayHook(&speech,"Time for me to go",&line);assert(spokenText=="Il est temps de partir."&&spokenData==&line);
        speech.me=&outsider;missionSayHook(&speech,"Time for me to go",&line);assert(spokenText=="Time for me to go");
        speech.me=&b;missionSayHook(&speech,"Another line",&line);assert(spokenText=="Another line");
        missionWorldChanging=true;missionSayHook(&speech,"Time for me to go",&line);assert(spokenText=="Time for me to go");missionWorldChanging=false;
    }
    {Character a,b,c,d,outsider;setup(a,b,c,d);missionSpeedEnumOriginal=&captureSpeedMode;missionSpeedFloatOriginal=&captureSpeedFloat;
        a.med.ko=true;b.pickupObject(&a);tickMissionRescue(2);assert(escort==&b&&!missionCasualtyWaiting);
        applyMissionPace();missionSpeedEnumHook(&b.move,JOG);assert(guardedMode==WALK&&guardedSpeed==3&&b.move.orderMode==WALK);
        missionSpeedFloatHook(&c.move,100);assert(guardedSpeed==3);
        missionPace=EscortPace::Accelerated;applyMissionPace();missionSpeedEnumHook(&b.move,JOG);assert(guardedMode==RUN&&fabs(guardedSpeed-6.8f)<.001f);
        b.combat=true;missionSpeedFloatHook(&b.move,100);assert(guardedSpeed==100);
        missionSpeedFloatHook(&outsider.move,100);assert(guardedSpeed==100);
        missionCasualtyWaiting=true;missionSpeedFloatHook(&c.move,100);assert(guardedSpeed<=55);
    }
    {Character a,b,c,d;setup(a,b,c,d);missionPace=EscortPace::Accelerated;a.pos=70;c.pos=d.pos=a.pos;
        applyMissionPace();assert(a.move.desired<b.move.desired&&a.move.desired>0);
        missionSpeedEnumHook(&a.move,JOG);assert(fabs(guardedSpeed-a.move.desired)<.001f);
        // A slower follower can close a gap without the 100-unit hard stop.
        for(int i=0;i<300;++i){applyMissionPace();a.pos.x+=a.move.desired*.1f;b.pos.x+=b.move.desired*.1f;c.pos=d.pos=a.pos;if(b.pos.x>a.pos.x)b.pos=a.pos;assert(!tickMissionRescue(.1f));}
        assert(fabs(a.pos.x-b.pos.x)<40);
    }
    {Character a,b,c,d;setup(a,b,c,d);a.stats.speed=b.stats.speed=c.stats.speed=d.stats.speed=100;missionPace=EscortPace::Normal;applyMissionPace();assert(missionRescue.lastSpeed==3);
        missionPace=EscortPace::Accelerated;applyMissionPace();missionSpeedEnumHook(&b.move,WALK);assert(guardedMode==RUN&&guardedSpeed==100&&b.move.orderMode==RUN);
        missionPace=EscortPace::Normal;applyMissionPace();missionSpeedEnumHook(&b.move,RUN);assert(guardedMode==WALK&&guardedSpeed==3);}
    assert(MissionPaceRules::gaitLimit(100,15,true)==100);
    assert(MissionPaceRules::gaitLimit(100,15,false)==15);
    {Character a,b,c,d;Dialogue oldTalk,newTalk;setup(a,b,c,d);a.dialogue=&oldTalk;b.dialogue=&newTalk;
        a.med.ko=true;b.pickupObject(&a);tickMissionRescue(2);
        assert(escort==&b&&!oldTalk.canTalk&&oldTalk.ended&&newTalk.canTalk);
        a.med.ko=false;tickMissionRescue(1);assert(escort==&b&&!oldTalk.canTalk&&newTalk.canTalk);
        oldTalk.canTalk=true;tickMissionRescue(1);assert(!oldTalk.canTalk); // old-save dialogue repaired
    }
    {Character a,b,c,d;setup(a,b,c,d);
        missionFollowing=true;missionFollowTarget=hand(&d);missionFollowers.push_back(hand(&a));waitingHere.push_back(1);
        a.ai.tasks.nativeGoal=FOLLOW_PLAYER_ORDER;b.ai.tasks.nativeGoal=RUN_AWAY;c.ai.tasks.nativeGoal=FIRST_AID_ORDER;
        a.jobs.insert(FOLLOW_PLAYER_ORDER);b.jobs.insert(FOLLOW_PLAYER_ORDER);c.jobs.insert(HOLD_POSITION);
        b.pickupObject(&d);finalizeMissionAI();
        assert(!missionActive&&!missionFollowing&&missionFollowers.empty()&&missionFollowTarget.isNull()&&waitingHere.empty());
        assert(a.ai.tasks.nativeGoal==NULL_TASK&&b.ai.tasks.nativeGoal==RUN_AWAY&&c.ai.tasks.nativeGoal==FIRST_AID_ORDER);
        assert(!a.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER)&&!b.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER)&&!c.ai.tasks.hasPlayerOrder(HOLD_POSITION));
        assert(b.cargo==&d&&d.carried&&missionRescue.tasks.empty()&&missionGroup.assigned.empty());
        int before=a.ai.tasks.explicitCount;missionIssueOrder(&a,FOLLOW_PLAYER_ORDER,&b,b.pos);assert(a.ai.tasks.explicitCount==before);
        missionSpeedFloatHook(&a.move,100);assert(guardedSpeed==100);
        cleanupMissionActorAI(&a,"repeat / load");assert(a.ai.tasks.nativeGoal==NULL_TASK);
        std::set<std::string> released;std::map<std::string,int> observed;
        a.ai.tasks.nativeGoal=FOLLOW_PLAYER_ORDER;a.jobs.insert(FOLLOW_PLAYER_ORDER);
        observeRetiredMissionActor(&a,released,observed);assert(a.ai.tasks.nativeGoal==NULL_TASK&&!a.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER));
        a.ai.tasks.nativeGoal=RUN_AWAY;observeRetiredMissionActor(&a,released,observed);assert(a.ai.tasks.nativeGoal==RUN_AWAY);
        released.clear();observed.clear(); // Runtime-only observations reset at a load boundary.
        a.ai.tasks.nativeGoal=MOVE_CUS_ORDERED;observeRetiredMissionActor(&a,released,observed);assert(a.ai.tasks.nativeGoal==NULL_TASK);
    }
    {Character a,b,c,d;setup(a,b,c,d);a.ai.tasks.nativeGoal=RUN_AWAY;
        missionSpeedFloatHook(&a.move,100);assert(guardedSpeed==100);
        a.player=true;a.jobs.insert(FOLLOW_PLAYER_ORDER);cleanupMissionActorAI(&a,"player exclusion");assert(a.ai.tasks.hasPlayerOrder(FOLLOW_PLAYER_ORDER));
    }
    {Character a,b,c,d,outsider;setup(a,b,c,d);contractLifecycle=CONTRACT_ACTIVE;currentContract.settlementPaid=false;
        missionFollowing=true;missionPaused=true;waitingForPlayer=true;missionFollowTarget=hand(&outsider);
        missionRescue.roadPlanned=true;missionRescue.roadFailed=true;missionRescue.roadPoints.push_back(Vec(50));
        missionIssueOrder(&a,FOLLOW_PLAYER_ORDER,&outsider,outsider.pos);a.ai.tasks.nativeGoal=FOLLOW_PLAYER_ORDER;destination=Vec(900);const int before=travelOrders;
        assert(!recoverMissionOrders(&outsider));assert(recoverMissionOrders(&b));
        assert(missionActive&&!missionFollowing&&!missionPaused&&!waitingForPlayer&&missionCasualtyWaiting);
        assert(missionRescue.recoveryPending&&!missionRescue.roadPlanned&&!missionRescue.roadFailed&&destination.x==900);
        assert(a.ai.tasks.nativeGoal==NULL_TASK&&travelOrders==before);
        a.ai.tasks.nativeGoal=RUN_AWAY;assert(tickMissionRescue(3));assert(travelOrders==before&&a.ai.tasks.nativeGoal==RUN_AWAY);
        a.ai.tasks.nativeGoal=NULL_TASK;assert(tickMissionRescue(1));assert(!tickMissionRescue(1));
        assert(!missionRescue.recoveryPending&&!missionCasualtyWaiting&&travelOrders==before+1);
        assert(!tickMissionRescue(2));assert(travelOrders==before+1); // no retry every tick
        currentContract.settlementPaid=true;assert(!recoverMissionOrders(&a));currentContract.settlementPaid=false;
        Window report;report.visible=true;finalWindow=&report;assert(!recoverMissionOrders(&a));finalWindow=0;
        missionActive=false;assert(!recoverMissionOrders(&a));missionActive=true;
        missionPending=true;assert(!recoverMissionOrders(&a));missionPending=false;
    }
    {Character a,b,c,d;setup(a,b,c,d);destination=Vec(700);leavingBuilding=true;exitWaypoint=Vec(55);
        assert(recoverMissionOrders(&a));assert(!tickMissionRescue(2));assert(destination.x==700);
        assert(a.pos.x==0);leavingBuilding=false; // order issued without teleporting
    }
    {Character a,b,c,d;setup(a,b,c,d);scientificResearching=true;scientificWillEnter=true;scientificEntryResolved=false;
        scientificRuinCenter=Vec(80);const int before=travelOrders;assert(recoverMissionOrders(&a));assert(!tickMissionRescue(2));
        assert(a.ai.tasks.hasPlayerOrder(UNLOCK_DOOR_HERE)&&travelOrders==before);scientificResearching=false;scientificWillEnter=false;
    }
    {Character a,b,c,d;setup(a,b,c,d);a.med.ko=true;b.pickupObject(&a);escort=&b;escortHandle=hand(&b);
        missionRescue.retiredLeaders.push_back(hand(&a));assert(recoverMissionOrders(&b));
        assert(b.cargo==&a&&a.carried&&missionRescue.retiredLeaders.size()==1);
        assert(!tickMissionRescue(2));assert(escort==&b&&b.cargo==&a);
    }
    {Character a,b,c,d;setup(a,b,c,d);b.med.need=50;
        MissionRescueTask task;task.patient=hand(&b);task.helper=hand(&a);task.phase=MissionRescueTask::FirstAid;
        missionRescue.tasks.push_back(task);a.jobs.insert(FIRST_AID_ORDER);a.ai.tasks.nativeGoal=FIRST_AID_ORDER;
        const int before=travelOrders;assert(recoverMissionOrders(&c));
        assert(a.ai.tasks.hasPlayerOrder(FIRST_AID_ORDER)&&a.ai.tasks.nativeGoal==FIRST_AID_ORDER&&missionRescue.tasks.size()==1);
        assert(tickMissionRescue(2));assert(missionRescue.recoveryPending&&travelOrders==before);
        b.med.need=0;assert(!tickMissionRescue(2));assert(!missionRescue.recoveryPending&&travelOrders==before+1);
    }
    {Character a,b,c,d;setup(a,b,c,d);MercenarieCleanup::disabled=true;
        missionSpeedFloatOriginal=&captureSpeedFloat;missionSpeedFloatHook(&a.move,123);assert(guardedSpeed==123);
        missionOffscreenImmuneOriginal=nativeOffscreenDecision;nativeOffscreenResult=false;assert(!missionOffscreenImmuneHook(&a));
        missionRunGoalsOriginal=simulateNativeGoals;Tasker patrol(PATROL_TOWN);MissionNativeGoalScores goals;goals[100]=&patrol;
        missionRunGoalsHook(&a.ai.tasks,goals,false);assert(goals.size()==1&&lastNativeChoice==PATROL_TOWN);
        MercenarieCleanup::disabled=false;
    }
    std::cout<<"PASS disabled runtime native speed/offscreen/goals despite stale mission state.\n";
    std::cout<<"PASS production rescue runtime: kits, nearest, reservation, parallel, threat, native jobs, carry/release, succession, death, all KO, no kits, player carry, isolation, reassignment, cleanup\n";
}



