#pragma once
#include "src/Guards/Assignment.h"
#include "src/Guards/Persistence.h"
// Included with the other service adapters, before the persistence schema.
GuildGuards::Model guildGuards;
bool delegatedCharacterAbsent(Character*);
namespace GuardsNative {
GuildGuards::Assignment allocator;
std::map<std::string,hand> beds;
std::map<std::string,double> bedRetry,orderRetry;
struct ManagedOrder {Tasker* task;TaskType type;hand subject;Ogre::Vector3 location;ManagedOrder():task(0),type(NULL_TASK){}};
std::map<std::string,ManagedOrder> ownedOrders;
std::vector<hand> bedCache;double bedCacheAt=0;
std::map<std::string,std::map<GuildGuards::Id,double> > failedPaths;
double clock=0;float elapsed=0;bool internalOrder=false,importPending=false;
bool hooksReady=false;
bool goalsReady=false;
void closeUi();
void open(MyGUI::Widget*);
void updatePreview();
void refreshUi();
GuildGuards::Position position(const Ogre::Vector3& p){return GuildGuards::Position(p.x,p.y,p.z);}
Ogre::Vector3 position(const GuildGuards::Position& p){return Ogre::Vector3((float)p.x,(float)p.y,(float)p.z);}
Character* resolve(const std::string& id){hand h;h.fromString(id);return h.isNull()?0:h.getCharacter();}
struct Issuing {bool before;Issuing():before(internalOrder){internalOrder=true;}~Issuing(){internalOrder=before;}};
void discard(){closeUi();allocator.reset();beds.clear();bedRetry.clear();orderRetry.clear();ownedOrders.clear();bedCache.clear();failedPaths.clear();bedCacheAt=0;guildGuards=GuildGuards::Model();clock=0;elapsed=0;importPending=false;}
void releaseOrder(const std::string& id){
    std::map<std::string,ManagedOrder>::iterator i=ownedOrders.find(id);if(i==ownedOrders.end())return;
    Character* c=resolve(id);OrdersReceiver* orders=c?c->getOrdersReciever():0;
    // clearOrders preserves permanent jobs. Only clear our exact outstanding
    // order, never a newer manual order or a native food/combat task.
    Tasker* first=orders?orders->getFirstOrder():0;
    if(first&&first==i->second.task&&first->key()==i->second.type&&first->subject==i->second.subject&&first->getLocation().squaredDistance(i->second.location)<.0001f){Issuing issuing;orders->clearOrders();if(c->getMovement())c->getMovement()->halt();}
    ownedOrders.erase(i);orderRetry.erase(id);beds.erase(id);
}
void rememberOrder(Character* c,const std::string& id){OrdersReceiver* orders=c->getOrdersReciever();Tasker* task=orders?orders->getFirstOrder():0;if(task){ManagedOrder owned;owned.task=task;owned.type=task->key();owned.subject=task->subject;owned.location=task->getLocation();ownedOrders[id]=owned;}}
double health(Character* c){
    double value=1;bool any=false;
    for(int i=0;i<c->medical.getPartCount();++i){MedicalSystem::HealthPartStatus* p=c->medical.getPart((unsigned __int64)i);if(!p)continue;
        if(p==c->medical.leftArm&&c->medical.getLimbState(RobotLimbs::LEFT_ARM)==LIMB_STUMP)continue;
        if(p==c->medical.rightArm&&c->medical.getLimbState(RobotLimbs::RIGHT_ARM)==LIMB_STUMP)continue;
        if(p==c->medical.leftLeg&&c->medical.getLimbState(RobotLimbs::LEFT_LEG)==LIMB_STUMP)continue;
        if(p==c->medical.rightLeg&&c->medical.getLimbState(RobotLimbs::RIGHT_LEG)==LIMB_STUMP)continue;
        double maximum=p->maxHealth();if(!(maximum>0))continue;any=true;
        // Same numerator and maximum as native updateDerivedHealths. No epsilon
        // or rounded display percentage is used for the full recovery test.
        double fraction=(p->flesh-p->fleshStun+p->juryRigging)/maximum;
        if(!(fraction>=0))fraction=0;value=std::min(value,fraction);
    }return any?value:0;
}
bool usableBed(Character* c,UseableStuff* bed){
    if(!bed||!bed->isThePlayer()||bed->isBroken()||bed->isDisabled())return false;
    BuildingFunction need=c->getRace()&&c->getRace()->robot?BF_SKELETON_BED:BF_BED;
    return bed->getSpecialFunction()==need&&bed->couldIOperate(c->getHandle())&&bed->isFreeSlot(c->getHandle());
}
UseableStuff* findBed(Character* c,const std::string& id){
    std::map<std::string,hand>::iterator found=beds.find(id);
    if(found!=beds.end()){Building* b=found->second.getBuilding();UseableStuff* bed=b?b->getUseableStuff():0;if(usableBed(c,bed))return bed;beds.erase(found);}
    if(clock<bedRetry[id]||!ou||!ou->zoneMgr)return 0;bedRetry[id]=clock+10;
    if(clock>=bedCacheAt){bedCacheAt=clock+10;bedCache.clear();static lektor<Building*> candidates;candidates.count=0;ou->zoneMgr->findAllBuildings(candidates,0,ou->player->getFaction(),false,0,0);std::set<std::string> seen;for(unsigned int i=0;i<candidates.size();++i){Building* b=candidates[i];if(!b)continue;static lektor<Building*> furniture;furniture.count=0;b->findAllFurnitureWithFunction(furniture,BF_BED);b->findAllFurnitureWithFunction(furniture,BF_SKELETON_BED);if(b->getSpecialFunction()==BF_BED||b->getSpecialFunction()==BF_SKELETON_BED){if(seen.insert(b->getHandle().toString()).second)bedCache.push_back(b->getHandle());}for(unsigned int k=0;k<furniture.size();++k)if(furniture[k]&&seen.insert(furniture[k]->getHandle().toString()).second)bedCache.push_back(furniture[k]->getHandle());}}
    UseableStuff* best=0;float distance=FLT_MAX;
    for(size_t i=0;i<bedCache.size();++i){Building* candidate=bedCache[i].getBuilding();UseableStuff* b=candidate?candidate->getUseableStuff():0;if(!usableBed(c,b))continue;
        bool reserved=false;for(std::map<std::string,hand>::const_iterator j=beds.begin();j!=beds.end();++j)if(j->first!=id&&j->second==b->getHandle()){reserved=true;break;}if(reserved)continue;
        float d=c->getPosition().squaredDistance(b->getPosition());if(d<distance&&c->pathExists(b->getPosition())){best=b;distance=d;}
    }if(best)beds[id]=best->getHandle();return best;
}
void pause(Character* c){
    if(internalOrder||!c)return;std::map<std::string,GuildGuards::Guard>::iterator i=guildGuards.guards.find(c->getHandle().toString());
    if(i==guildGuards.guards.end()||i->second.paused)return;
    releaseOrder(i->first);i->second.paused=true;i->second.assigned=0;beds.erase(i->first);orderRetry.erase(i->first);
    Loc::Catalogue args;args["name"]=c->getName();if(ou)ou->showPlayerAMessage(Loc::format("guards.manual_pause",args),true);
}
void (*manualOriginal)(PlayerInterface*,Building*,TaskType,RootObject*,bool,bool,const Ogre::Vector3&)=0;
void manualHook(PlayerInterface* player,Building* dest,TaskType task,RootObject* subject,bool shift,bool add,const Ogre::Vector3& pos){
    if(!internalOrder&&player)for(size_t i=0;i<player->playerCharacters.size();++i){Character* c=player->playerCharacters[i];if(c&&player->isObjectSelected(c))pause(c);}
    manualOriginal(player,dest,task,subject,shift,add,pos);
}
void (*moveOriginal)(Character*,Building*,RootObject*,const Ogre::Vector3&)=0;
void moveHook(Character* c,Building* b,RootObject* subject,const Ogre::Vector3& p){pause(c);moveOriginal(c,b,subject,p);}
void (*standingOriginal)(PlayerInterface*,MessageForB::StandingOrder)=0;
void standingHook(PlayerInterface* player,MessageForB::StandingOrder order){
    if(!internalOrder&&player&&(order==MessageForB::M_SET_ORDER_HOLD||order==MessageForB::M_SET_ORDER_PASSIVE||order==MessageForB::M_SET_ORDER_CHASE))for(size_t i=0;i<player->playerCharacters.size();++i){Character* c=player->playerCharacters[i];if(c&&player->isObjectSelected(c))pause(c);}
    standingOriginal(player,order);
}
void (*combatMoveOriginal)(CharMovement*,const hand&,float,float,float,bool,float)=0;
bool outsidePost(Character* c,Character* target){
    if(!c||!target||guildGuards.paused)return false;
    std::map<std::string,GuildGuards::Guard>::const_iterator g=guildGuards.guards.find(c->getHandle().toString());
    if(g==guildGuards.guards.end()||g->second.paused||!g->second.assigned)return false;
    std::map<GuildGuards::Id,GuildGuards::Post>::const_iterator p=guildGuards.posts.find(g->second.assigned);
    return p!=guildGuards.posts.end()&&(position(target->getPosition()).distance2(p->second.position)>10000||position(c->getPosition()).distance2(p->second.position)>10000);
}
void combatMoveHook(CharMovement* movement,const hand& target,float minimum,float maximum,float circle,bool power,float speed){
    Character* c=movement?movement->getCharacter():0;
    if(outsidePost(c,target.getCharacter())){const GuildGuards::Guard& g=guildGuards.guards.find(c->getHandle().toString())->second;movement->setDestination(position(guildGuards.posts.find(g.assigned)->second.position),HIGH_PRIORITY,false);return;}
    combatMoveOriginal(movement,target,minimum,maximum,circle,power,speed);
}
bool allowGoal(Character* c,Tasker* task){
    if(!c||!task||guildGuards.paused)return true;
    std::map<std::string,GuildGuards::Guard>::const_iterator g=guildGuards.guards.find(c->getHandle().toString());
    if(g==guildGuards.guards.end()||g->second.paused||!hooksReady)return true;
    if(c->medical.isUnconcious()||c->isBeingCarried()||c->inSomething==IN_PRISON)return true;
    TaskType type=task->key();
    if(type==AQUIRE_FOOD_AT_HOMEBASE||type==GRAB_ONE_FOOD||type==EAT_FOOD_ON_GROUND)return true;
    if(type==USE_BED||type==USE_BED_ORDER){std::map<std::string,hand>::const_iterator b=beds.find(g->first);return b!=beds.end()&&task->subject==b->second;}
    const bool combatTask=type==MELEE_ATTACK||type==FOCUSED_MELEE_ATTACK||type==CHOOSE_ENEMY_AND_ATTACK||type==CHOOSE_ATTACKER_OF_ALLY||type==ATTACK_CHARACTERS_ATTACKER||type==ATTACK_ATTACKERS_OF||type==ATTACK_ENEMIES||type==DEFEAT_SQUAD||type==DEFEAT_SQUAD_LIMIT_CHASE_RANGE||type==RANGED_ATTACK||type==RANGED_ATTACK_FOCUSED;
    if(combatTask||(task->getTaskData()&&task->getTaskData()->aggressionLevel>0))return !outsidePost(c,task->subject.getCharacter());
    return type==MOVE_CUS_ORDERED||type==HOLD_POSITION||type==JOB_REPAIR_ROBOT||type==JOB_MEDIC||type==FIRST_AID_ROBOT||type==SELF_PRESERVATION||type==GET_UP_STAND_UP||type==GET_OUT_OF_BED_IF_ITS_EMERGENCY;
}
void install(){
    bool a=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PlayerInterface::addOrderSelectedCharacters),&manualHook,&manualOriginal)==KenshiLib::SUCCESS;
    bool b=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::_NV_playerMoveOrderDefault),&moveHook,&moveOriginal)==KenshiLib::SUCCESS;
    bool combat=KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharMovement::combatMovementOffensive),&combatMoveHook,&combatMoveOriginal)==KenshiLib::SUCCESS;
    bool standing=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PlayerInterface::setOrderSelectedCharacters),&standingHook,&standingOriginal)==KenshiLib::SUCCESS;
    hooksReady=a&&b&&combat&&standing&&goalsReady;if(!hooksReady)ErrorLog("Guards: required native hooks unavailable; service disabled");
}
void recoverImport(){
    if(!importPending||!ou||!ou->player)return;
    if(!ou->player->playerCharacters.size()||clock<3)return;
    std::map<std::string,std::vector<Character*> > roster;
    std::map<std::string,unsigned int> configured;
    for(std::map<std::string,GuildGuards::Guard>::const_iterator i=guildGuards.guards.begin();i!=guildGuards.guards.end();++i)++configured[i->second.name+(i->second.robot?"\nrobot":"\nhuman")];
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(!c||!c->getRace())return;if(!c->isDead()&&!c->isAnimal())roster[c->getName()+(c->getRace()->robot?"\nrobot":"\nhuman")].push_back(c);}
    std::map<std::string,GuildGuards::Guard> recovered;
    for(std::map<std::string,GuildGuards::Guard>::const_iterator i=guildGuards.guards.begin();i!=guildGuards.guards.end();++i){GuildGuards::Guard g=i->second;std::vector<Character*>& matches=roster[g.name+(g.robot?"\nrobot":"\nhuman")];g.assigned=0;
        if(matches.size()==1&&configured[g.name+(g.robot?"\nrobot":"\nhuman")]==1){g.actor=matches[0]->getHandle().toString();if(!recovered.count(g.actor))recovered[g.actor]=g;}
    }guildGuards.guards.swap(recovered);allocator.reset();importPending=false;
}
void tick(float delta){
    if(!hooksReady||!ou||!ou->player||ou->isPaused()||savePreparing)return;
    elapsed+=std::max(0.0f,delta);clock+=std::max(0.0f,delta);if(elapsed<.5f)return;elapsed=0;recoverImport();if(importPending)return;
    std::map<std::string,GuildGuards::Observation> observations;
    for(std::map<std::string,GuildGuards::Guard>::const_iterator i=guildGuards.guards.begin();i!=guildGuards.guards.end();++i){Character* c=resolve(i->first);GuildGuards::Observation& o=observations[i->first];
        if(!c||!c->isPlayerCharacter()||c->isDead()||!c->getMovement()||delegatedCharacterAbsent(c))continue;
        guildGuards.guards[i->first].name=c->getName();guildGuards.guards[i->first].robot=c->getRace()&&c->getRace()->robot;
        std::map<GuildGuards::Id,double>& rejected=failedPaths[i->first];for(std::map<GuildGuards::Id,double>::iterator j=rejected.begin();j!=rejected.end();)if(clock>=j->second||!guildGuards.posts.count(j->first))rejected.erase(j++);else{o.inaccessible.insert(j->first);++j;}
        o.position=position(c->getPosition());o.ko=c->medical.isUnconcious();o.available=!c->isBeingCarried()&&c->inSomething!=IN_PRISON&&!c->isChainedMode();o.health=health(c);o.combat=c->isInCombatMode(true,true);o.inBed=c->inSomething==IN_BED;
        OrdersReceiver* orders=c->getOrdersReciever();TaskType current=orders?orders->getCurrentGoal().key():NULL_TASK;
        o.eating=current==AQUIRE_FOOD_AT_HOMEBASE||current==GRAB_ONE_FOOD||current==EAT_FOOD_ON_GROUND;
        if(o.available&&!o.ko&&!guildGuards.paused&&!i->second.paused&&(o.health<.6||i->second.recoveryRequired||(o.inBed&&o.health<1)||allocator.runtime[i->first].recovering))o.bedAvailable=findBed(c,i->first)!=0;
        if(i->second.assigned&&guildGuards.posts.count(i->second.assigned)){Character* target=c->getAttackTarget().getCharacter();if(target)o.enemyOutside=position(target->getPosition()).distance2(guildGuards.posts[i->second.assigned].position)>10000;}
    }
    allocator.update(guildGuards,observations,clock);
    for(std::map<std::string,GuildGuards::Guard>::const_iterator i=guildGuards.guards.begin();i!=guildGuards.guards.end();++i){Character* c=resolve(i->first);if(!c||!c->getMovement())continue;GuildGuards::State state=allocator.runtime[i->first].state;
        if(state==GuildGuards::Paused||state==GuildGuards::Unavailable||state==GuildGuards::KnockedOut||state==GuildGuards::Waiting){releaseOrder(i->first);continue;}
        if(state==GuildGuards::SeekingBed){UseableStuff* bed=findBed(c,i->first);if(bed&&clock>=orderRetry[i->first]){Issuing issuing;c->addOrder(bed,USE_BED_ORDER,bed,false,true,bed->getPosition());rememberOrder(c,i->first);orderRetry[i->first]=clock+5;}continue;}
        if(state==GuildGuards::Resting||state==GuildGuards::Combat||state==GuildGuards::Eating||state==GuildGuards::KnockedOut||state==GuildGuards::Unavailable||state==GuildGuards::Paused)continue;
        beds.erase(i->first);
        if(!i->second.assigned)continue;const GuildGuards::Post& p=guildGuards.posts.find(i->second.assigned)->second;
        if(state==GuildGuards::OnPost){double radians=p.heading*3.141592653589793/180; c->getMovement()->faceDirection(Ogre::Vector3((float)std::sin(radians),0,(float)std::cos(radians)));}
        else if(clock>=orderRetry[i->first]){if(!c->pathExists(position(p.position))){failedPaths[i->first][p.id]=clock+30;guildGuards.guards[i->first].assigned=0;releaseOrder(i->first);continue;}Issuing issuing;c->addOrder(0,MOVE_CUS_ORDERED,0,false,true,position(p.position));rememberOrder(c,i->first);orderRetry[i->first]=clock+2;}
    }
    refreshUi();
}
}
