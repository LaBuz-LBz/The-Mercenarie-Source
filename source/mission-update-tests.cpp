#include "GuildProgression.h"
#include "MissionBonuses.h"
#include "MissionFormation.h"
#include "MissionArchive.h"
#include "EscortPace.h"
#include "EscortMissionRules.h"
#include <cassert>
#include <iostream>
struct Rolls {
    const std::vector<float>* values;size_t index;
    Rolls(const std::vector<float>& v):values(&v),index(0){}
    float operator()(){assert(index<values->size());return (*values)[index++];}
};
// Engine adapter doubles: the production controller below is compiled unchanged.
namespace Ogre {struct Vector3 {float x,y,z;Vector3(float a=0,float b=0,float c=0):x(a),y(b),z(c){}float squaredDistance(const Vector3& p)const{return (x-p.x)*(x-p.x)+(y-p.y)*(y-p.y)+(z-p.z)*(z-p.z);}};}
enum {MOVE_CUS_ORDERED,FOLLOW_PLAYER_ORDER,FOLLOW_SQUADLEADER,HOLD_POSITION,BODYGUARD,RUN};
struct Medical {bool ko;Medical():ko(false){}bool isUnconcious(){return ko;}};
struct Movement {bool failed,indoors,moving;int desired;Movement():failed(false),indoors(false),moving(true),desired(-1){}
    bool pathFailed(){return failed;}bool isIndoors(){return indoors;}bool isCurrentlyMoving(){return moving;}
    void setRoadPreference(float){}void setDesiredSpeedOrders(int speed){desired=speed;}
};
struct Character {
    bool dead,combat;Medical medical;Movement movement;Ogre::Vector3 p,target;Character* subject;int orders,removed,last;
    Character():dead(false),combat(false),subject(0),orders(0),removed(0),last(-1){}
    bool isBeingCarried(){return false;}bool isDead(){return dead;}bool isInCombatMode(bool,bool){return combat;}
    Medical* getMedical(){return &medical;}Movement* getMovement(){return &movement;}Ogre::Vector3 getPosition(){return p;}
    void removeJob(int){++removed;}void addJob(int type,Character* who,bool,bool,Ogre::Vector3 destination){last=type;subject=who;target=destination;++orders;}
};
void missionOrderTrace(Character*,const char*,Character* =0){}
void missionClearTravel(Character* c,const char*){c->removeJob(MOVE_CUS_ORDERED);c->removeJob(FOLLOW_PLAYER_ORDER);c->removeJob(FOLLOW_SQUADLEADER);c->removeJob(HOLD_POSITION);c->removeJob(BODYGUARD);c->last=-1;}
bool missionNativeTaskPresent(Character* c,int type){return c->last==type;}
bool missionFollowOrderPresent(Character* c){return c->last==FOLLOW_PLAYER_ORDER;}
void missionIssueOrder(Character* c,int type,Character* target,const Ogre::Vector3& pos){c->addJob(type,target,false,false,pos);}
void missionHoldTravel(Character* c,const char*){if(c->last!=HOLD_POSITION){missionClearTravel(c,"hold");missionIssueOrder(c,HOLD_POSITION,0,c->getPosition());}}
struct hand {Character** ref;hand(Character** p=0):ref(p){}Character* getCharacter()const{return ref?*ref:0;}};
struct Window {bool visible;Window():visible(false){}bool getVisible(){return visible;}};
bool missionActive=true,missionPending=false,missionPaused=false,missionCasualtyWaiting=false;Window report;Window* finalWindow=&report;
Character* escort=0;std::vector<hand> progressMembers;
Character* missionTemporaryLeader=0;
#include "MissionMotionPolicy.h"
bool missionFollowerTargetValid(Character* c,Character* leader,bool approach){return approach?c->last==MOVE_CUS_ORDERED&&c->target.squaredDistance(leader->getPosition())<100:c->last==FOLLOW_PLAYER_ORDER&&c->subject==leader;}
int followerRecoveries=0,followerSuspensions=0;
void missionRecoverFollower(Character* c,Character* leader,MissionMotionPolicy::Action a,bool approach){if(a==MissionMotionPolicy::Wait)return;if(a==MissionMotionPolicy::Suspend){++followerSuspensions;return;}++followerRecoveries;missionClearTravel(c,"recovery");missionIssueOrder(c,approach?MOVE_CUS_ORDERED:FOLLOW_PLAYER_ORDER,approach?0:leader,leader->getPosition());}
bool missionCombatThreat(Character* c){return c&&c->isInCombatMode(true,true);}
void missionYieldTravelToCombat(Character* c){if(missionFollowOrderPresent(c))missionClearTravel(c,"combat");}
#include "MissionFormationRuntime.h"
int main(){
    EscortPace::State pace=EscortPace::Normal;assert(EscortPace::nativeSpeed(pace)==0);assert(EscortPace::accelerate(pace));assert(pace==EscortPace::Accelerated&&EscortPace::nativeSpeed(pace)==2);assert(!EscortPace::accelerate(pace));assert(EscortPace::slowDown(pace));assert(!EscortPace::slowDown(pace));assert(EscortPace::accelerate(pace));
    MissionArchive paceOut;paceOut.field(pace);MissionArchive paceIn(paceOut.bytes);EscortPace::State restored=EscortPace::Normal;paceIn.field(restored);paceIn.finish();assert(restored==EscortPace::Accelerated);
    EscortPace::State missionOne=EscortPace::Accelerated,missionTwo=EscortPace::Normal,missionThree=EscortPace::Accelerated;assert(missionOne!=missionTwo&&missionTwo!=missionThree);
    assert(EscortMissionRules::afterForcedPacePenalty(20000,false)==20000);assert(EscortMissionRules::afterForcedPacePenalty(20000,true)==18000);assert(EscortMissionRules::afterForcedPacePenalty(100000,true)==90000);
    bool forced=false;forced=true;assert(EscortMissionRules::forcedPacePenalty(20000,forced)==2000);forced=forced||true;assert(EscortMissionRules::forcedPacePenalty(20000,forced)==2000);
    MissionArchive taxOut;taxOut.field(forced);MissionArchive taxIn(taxOut.bytes);bool restoredForced=false;taxIn.field(restoredForced);taxIn.finish();assert(restoredForced);
    EscortMissionRules::Separation separation;assert(EscortMissionRules::updateSeparation(separation,true,29)==EscortMissionRules::SeparationSafe);assert(EscortMissionRules::updateSeparation(separation,true,1)==EscortMissionRules::SeparationWarn);assert(EscortMissionRules::updateSeparation(separation,true,30)==EscortMissionRules::SeparationSafe);assert(EscortMissionRules::updateSeparation(separation,false,1)==EscortMissionRules::SeparationSafe&&!separation.warned);assert(EscortMissionRules::updateSeparation(separation,true,30)==EscortMissionRules::SeparationWarn);assert(EscortMissionRules::updateSeparation(separation,true,60)==EscortMissionRules::SeparationFail);
    MissionBonuses::Choice c;c.amounts[0]=100;c.amounts[1]=200;c.amounts[2]=300;c.selected=7;c.cap(600);
    assert(c.count(c.selected)==3&&c.total(c.selected)==600);
    std::vector<float> yes(3,0),no(6,1),partial;partial.push_back(1);partial.push_back(0);partial.push_back(1);partial.push_back(1);partial.push_back(0);
    MissionBonuses::Resolution r=MissionBonuses::resolve(c,true,.55f,.25f,Rolls(yes));assert(r.count==3&&r.cats==600);
    r=MissionBonuses::resolve(c,true,.55f,.25f,Rolls(no));assert(r.count==0&&r.cats==0&&MissionBonuses::xpCost(106,r.count)==0);
    r=MissionBonuses::resolve(c,true,.55f,.25f,Rolls(partial));assert(r.count==2&&r.cats==350&&MissionBonuses::xpCost(106,r.count)==21);
    r=MissionBonuses::resolve(c,false,.55f,.25f,Rolls(yes));assert(r.count==0&&r.cats==0);
    c.selected=2;r=MissionBonuses::resolve(c,true,.55f,.25f,Rolls(yes));assert(r.count==1&&r.cats==200);
    c.selected=128;r=MissionBonuses::resolve(c,true,.55f,.25f,Rolls(yes));assert(!r.count&&!r.cats);
    for(int xp=0;xp<201;++xp)for(int n=0;n<10;++n){int cost=MissionBonuses::xpCost(xp,n);assert(cost<=xp/2&&cost>=0);if(!n)assert(!cost);}
    c.selected=5;MissionArchive out;MissionBonuses::archive(out,c);
    std::string sealed=sealMissionArchive(out.bytes);
    for(int repeat=0;repeat<4;++repeat){MissionBonuses::Choice restored;MissionArchive in(openMissionArchive(sealed));MissionBonuses::archive(in,restored);in.finish();assert(restored.prepared&&restored.selected==5&&restored.amounts==c.amounts);}
    for(size_t end=0;end<sealed.size();++end){bool rejected=false;try{openMissionArchive(sealed.substr(0,end));}catch(...){rejected=true;}assert(rejected);}
    c.cap(200);assert(c.total(255)<=200);c.reset();assert(!c.prepared&&!c.selected&&!c.count(255));
    Character leader,one,two;Character* lead=&leader;Character* a=&one;Character* b=&two;escort=lead;
    progressMembers.push_back(hand(&lead));progressMembers.push_back(hand(&a));progressMembers.push_back(hand(&b));
    updateMissionFormation(1);
    assert(leader.orders==0&&one.orders==1&&two.orders==1);
    assert(one.subject==lead&&two.subject==lead&&one.last==FOLLOW_PLAYER_ORDER&&two.last==FOLLOW_PLAYER_ORDER);
    for(int tick=0;tick<10;++tick)updateMissionFormation(1);
    assert(one.orders==1&&two.orders==1);
    one.combat=true;leader.combat=true;one.medical.ko=true;
    int removed=one.removed;updateMissionFormation(1);
    assert(one.orders==1&&one.removed==removed);
    one.combat=false;leader.combat=false;one.medical.ko=false;
    missionPaused=true;updateMissionFormation(1);assert(one.removed>removed);
    missionPaused=false;updateMissionFormation(1);assert(one.orders==2&&one.last==FOLLOW_PLAYER_ORDER);
    a=0;updateMissionFormation(1);a=&one;updateMissionFormation(1);assert(one.orders==3);
    missionGroup.reset();updateMissionFormation(1);assert(one.orders==4);
    report.visible=true;updateMissionFormation(1);int previous=one.orders;
    updateMissionFormation(1);assert(one.orders==previous);
    report.visible=false;updateMissionFormation(1);assert(one.orders==previous+1);
    one.dead=true;previous=one.orders;updateMissionFormation(1);assert(one.orders==previous);
    one.dead=false;one.p=Ogre::Vector3(35);two.p=Ogre::Vector3(58);
    missionGroup.tick(&leader,progressMembers,true,1,true,true);
    assert(one.last==HOLD_POSITION&&two.last==MOVE_CUS_ORDERED);
    const int arrivedOrders=one.orders,approachOrders=two.orders;
    for(int tick=0;tick<60;++tick)missionGroup.tick(&leader,progressMembers,true,1,true,true);
    assert(one.last==HOLD_POSITION&&one.orders==arrivedOrders&&two.orders>approachOrders);
    two.p=Ogre::Vector3(35);missionGroup.tick(&leader,progressMembers,true,1,true,true);
    assert(two.last==HOLD_POSITION);
    missionGroup.tick(&leader,progressMembers,true,1,true,false);
    assert(one.last==FOLLOW_PLAYER_ORDER&&two.last==FOLLOW_PLAYER_ORDER);
    std::cout<<"PASS: bonuses/archive; native squad-leader target, five-second observation without restarting live follow, combat/KO preservation, pause/resume, streaming, reload, report and death\n";
}
