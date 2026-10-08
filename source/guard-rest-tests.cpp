#include <vector>
#include <string>
#include <cassert>
#include <iostream>
namespace Ogre {struct Vector3 {float x,y,z;Vector3():x(0),y(0),z(0){}Vector3 operator-(const Vector3& b)const{Vector3 v;v.x=x-b.x;v.y=y-b.y;v.z=z-b.z;return v;}float squaredLength()const{return x*x+y*y+z*z;}};}
struct Character;
struct hand {Character* p;hand():p(0){}Character* getCharacter()const{return p;}bool operator==(const hand& h)const{return p==h.p;}};
struct AnimationData {int layername;std::string animName;bool restrictsMovementOrders,relocates;AnimationData():layername(0),animName("Mercenarie_GuardRest_v1"),restrictsMovementOrders(false),relocates(false){}};
struct AnimationClass {struct Requirements {AnimationData* _currentAction;Requirements():_currentAction(0){}} animationRequirements;int refreshes;bool active,available,otherAction,missingData;int starts,stops;std::string current;AnimationData pose;AnimationClass():refreshes(0),active(true),available(true),otherAction(false),missingData(false),starts(0),stops(0){}bool getIsActivated(){return active;}void* getAnimationDatasList(){return this;}bool hasAnimation(const std::string& name){return available&&name=="Mercenarie Guard Rest";}void* getAnimationPlaying_datName(const char* n){return current==n?this:0;}bool hasAction(){return otherAction||!current.empty();}AnimationData* getAnimationData(std::string){return missingData?0:&pose;}void playAction(AnimationData*,float,float,bool){++starts;current="Mercenarie Guard Rest";animationRequirements._currentAction=&pose;}void runAnimation(AnimationData*,float,int,float){++refreshes;}void stopAnimation(AnimationData*){}void stopAction(std::string n){if(current==n){++stops;current.clear();}}};
struct CharMovement {bool idle;bool moving;float speed;CharMovement():idle(true),moving(false),speed(0){}bool isIdle(){return idle;}bool isCurrentlyMoving(){return moving;}float getCurrentSpeed(){return speed;}};
struct Character {bool player,animal,dead,disabled,ragdoll,carried,combat,isCarryingSomething;CharMovement movement;AnimationClass animation;Ogre::Vector3 position;Character():player(true),animal(false),dead(false),disabled(false),ragdoll(false),carried(false),combat(false),isCarryingSomething(false){}bool isPlayerCharacter(){return player;}bool isAnimal(){return animal;}bool isDead(){return dead;}bool isDisabled(){return disabled;}bool isRagdoll(){return ragdoll;}bool isBeingCarried(){return carried;}bool isInCombatMode(bool,bool){return combat;}CharMovement* getMovement(){return &movement;}AnimationClass* getAnimationClass(){return &animation;}hand getHandle(){hand h;h.p=this;return h;}Ogre::Vector3 getPosition(){return position;}};
namespace Loc {const char* text(const char* s){return s;}}
struct Player {hand selectedCharacter;};struct World {Player* player;std::string message;void showPlayerAMessage(const char* s,bool){message=s;}};World world;World* ou=&world;
void DebugLog(const std::string&){}
#include "GuardRestRuntime.h"
void select(Character& c){GuardRest::discardWorld();world.player->selectedCharacter=c.getHandle();}
int main(){Player player;world.player=&player;Character c;c.movement.idle=false;c.movement.speed=5;select(c);GuardRest::apply();assert(c.animation.starts==1);for(int i=0;i<1000;i++)GuardRest::update();assert(c.animation.starts==1&&c.animation.stops==0&&c.animation.refreshes==1000);GuardRest::apply();assert(c.animation.starts==1);
 c.movement.speed=1;c.movement.moving=true;GuardRest::update();assert(c.animation.stops==1&&GuardRest::members.empty());c.movement.speed=0;c.movement.moving=false;GuardRest::update();assert(c.animation.starts==1);
 for(int mode=0;mode<8;++mode){Character x;select(x);GuardRest::apply();if(mode==0)x.combat=true;if(mode==1)x.ragdoll=true;if(mode==2)x.dead=true;if(mode==3)x.disabled=true;if(mode==4)x.carried=true;if(mode==5)x.isCarryingSomething=true;if(mode==6)x.position.x=.1f;if(mode==7)x.movement.moving=true;GuardRest::update();assert(x.animation.stops==1&&GuardRest::members.empty());}
 for(int mode=0;mode<8;++mode){Character x;select(x);if(mode==0)x.player=false;if(mode==1)x.animal=true;if(mode==2)x.combat=true;if(mode==3)x.movement.moving=true;if(mode==4)x.animation.available=false;if(mode==5)x.animation.missingData=true;if(mode==6)x.animation.pose.restrictsMovementOrders=true;if(mode==7)x.animation.otherAction=true;GuardRest::apply();assert(x.animation.starts==0&&GuardRest::members.empty());if(mode==3)assert(world.message=="developer.guard_rest.moving");if(mode==7)assert(world.message=="developer.guard_rest.busy");}
 Character x;select(x);GuardRest::apply();x.animation.current="native work";x.movement.moving=true;GuardRest::update();assert(x.animation.current=="native work"&&x.animation.stops==0);
 Character a,b;select(a);GuardRest::apply();world.player->selectedCharacter=b.getHandle();GuardRest::apply();a.movement.moving=true;GuardRest::update();assert(a.animation.stops==1&&b.animation.stops==0&&GuardRest::members.size()==1);
 GuardRest::discardWorld();GuardRest::update();assert(GuardRest::members.empty()&&b.animation.stops==0);
 Character job;select(job);GuardRest::apply();job.animation.otherAction=true;job.animation.current="native work";job.animation.animationRequirements._currentAction=0;GuardRest::update();assert(GuardRest::members.empty()&&job.animation.refreshes==0&&job.animation.current=="native work");Character change;select(change);GuardRest::apply();GuardRest::releaseSelected();assert(GuardRest::members.empty()&&change.animation.stops==1);world.player->selectedCharacter=hand();GuardRest::apply();assert(GuardRest::members.empty());
 std::cout<<"PASS: temporary pose, stale path state accepted; physical movement/combat/incapacity cancellation, no replay, invalid selection/resources, other actions preserved, multiple guards and world reset\n";
}







