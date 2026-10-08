#pragma once
#include "EscortPersonnelPlan.h"
bool personnelInternalOrder=false;
struct PersonnelOrderScope{bool old;PersonnelOrderScope():old(personnelInternalOrder){personnelInternalOrder=true;}~PersonnelOrderScope(){personnelInternalOrder=old;}};
Character* personnelPlayer(const std::string& id){if(!ou||!ou->player)return 0;for(unsigned int i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(c&&c->getHandle().toString()==id)return c;}return 0;}
std::string& personnelMetadata(int slot){return slot==selectedEscortQuest?currentContract.routeRegions:escortQuests[slot].v_currentContract.routeRegions;}
bool personnelActive(int slot){return slot==selectedEscortQuest?missionActive:escortQuests[slot].v_missionActive;}
Character* personnelLeader(int slot){return (slot==selectedEscortQuest?escortHandle:escortQuests[slot].v_escortHandle).getCharacter();}
int personnelAssignment(Character* c){if(!c)return -1;std::string id=c->getHandle().toString();for(int slot=0;slot<maximumActiveQuests;++slot)if(personnelActive(slot)){
 std::vector<EscortPersonnel::Member> list=EscortPersonnel::read(personnelMetadata(slot));for(size_t i=0;i<list.size();++i)if(list[i].id==id)return slot;}return -1;}
bool personnelMatches(Tasker* task,const std::string& target){return task&&!target.empty()&&task->key()==FOLLOW_PLAYER_ORDER&&task->subject.toString()==target;}
void personnelRemoveFollow(Character* c,EscortPersonnel::Member& member){
 if(!c||!c->getAI()||!c->getAI()->getTaskSystem())return;AITaskSytem* tasks=c->getAI()->getTaskSystem();bool removed=false;
 for(size_t i=0;i<tasks->orders.list.size();){Tasker* t=tasks->orders.list[i];if(!personnelMatches(t,member.target)){++i;continue;}
 if(tasks->getCurrentGoal().key()==FOLLOW_PLAYER_ORDER&&tasks->getCurrentGoal().subject.toString()==member.target)tasks->clearCurrentGoal(true);
 tasks->orders.list.erase(tasks->orders.list.begin()+i);delete t;removed=true;}
 if(removed&&!tasks->hasPlayerOrders()&&!c->isInCombatMode(true,true)&&c->getMovement())c->getMovement()->halt();member.target.clear();
}
bool personnelEligible(Character* c){return c&&c->isPlayerCharacter()&&!c->isAnimal()&&!c->isDead();}
bool personnelSameTown(Character* c){if(!personnelEligible(c)||!contractOriginTown)return false;TownBase* town=c->getCurrentTownLocation();if(town!=contractOriginTown)return false;
 Ogre::Vector3 delta=c->getPosition()-contractOriginTown->getPosition();float radius=std::max(40.0f,contractOriginTown->getRadius());return delta.x*delta.x+delta.z*delta.z<=radius*radius;}
void clearCurrentPersonnel(){std::vector<EscortPersonnel::Member> list=EscortPersonnel::read(currentContract.routeRegions);for(size_t i=0;i<list.size();++i)personnelRemoveFollow(personnelPlayer(list[i].id),list[i]);EscortPersonnel::write(currentContract.routeRegions,std::vector<EscortPersonnel::Member>());}
void tickEscortPersonnel(){
 if(missionWorldChanging||missionRestorePending||MercenarieCleanup::disabled)return;
 for(int slot=0;slot<maximumActiveQuests;++slot){std::string& metadata=personnelMetadata(slot);std::vector<EscortPersonnel::Member> list=EscortPersonnel::read(metadata);if(list.empty())continue;
 bool active=personnelActive(slot);Character* leader=personnelLeader(slot);
 for(size_t i=0;i<list.size();++i){EscortPersonnel::Member& m=list[i];Character* c=personnelPlayer(m.id);if(!c)continue;
 if(!active||!personnelEligible(c)||!m.enabled){personnelRemoveFollow(c,m);continue;}
 if(!leader||leader->isDead()||!c->getAI()||!c->getMovement()||c->isBeingCarried()||c->isCarryingSomething||!c->getMedical()||c->getMedical()->isUnconcious()||c->getMedical()->isCrippled()||missionCombatThreat(c))continue;
 AITaskSytem* tasks=c->getAI()->getTaskSystem();if(!tasks)continue;
 // Only input hooks disable Guard. Native combat/care may replace orders.
 bool other=false,ours=false;for(size_t j=0;j<tasks->orders.list.size();++j){if(personnelMatches(tasks->orders.list[j],m.target))ours=true;else other=true;}
 if(other)continue; // allow automatic combat/care orders to finish
 const std::string target=leader->getHandle().toString();if(ours&&m.target==target)continue;
 if(ours)personnelRemoveFollow(c,m);
 if(!ours)m.target.clear(); // automatically restore a follow lost during combat
 {PersonnelOrderScope own;tasks->addOrder(FOLLOW_PLAYER_ORDER,leader->getHandle(),leader->getPosition(),false,false);}m.target=target;
 }
 if(!active)list.clear();EscortPersonnel::write(metadata,list);
 }
}
const Ogre::Vector3* personnelNativeOffset(Character* c){
 if(!personnelEligible(c))return 0;int slot=personnelAssignment(c);if(slot<0)return 0;Character* leader=personnelLeader(slot);if(!leader||!c->getAI()||c->isBeingCarried()||c->isInCombatMode(true,true))return 0;
 std::vector<EscortPersonnel::Member> list=EscortPersonnel::read(personnelMetadata(slot));bool animals=false;
 const std::vector<hand>& roster=slot==selectedEscortQuest?progressMembers:escortQuests[slot].v_progressMembers;
 for(size_t i=0;i<roster.size();++i){Character* a=roster[i].getCharacter();if(a&&!a->isDead()&&a->isAnimal())animals=true;}
 for(size_t i=0;i<list.size();++i)if(list[i].id==c->getHandle().toString()&&list[i].enabled&&list[i].target==leader->getHandle().toString()){
 AITaskSytem* tasks=c->getAI()->getTaskSystem();if(!tasks||tasks->orders.list.empty()||!personnelMatches(tasks->orders.list.front(),list[i].target))return 0;
 EscortPersonnel::Position p=EscortPersonnel::formationPosition(EscortPersonnel::formation(personnelMetadata(slot)),i,list.size(),animals);static Ogre::Vector3 offset;offset=Ogre::Vector3(p.forward,0,-p.side);return &offset;}
 return 0;
}
void personnelSetGuard(Character* c,bool on){int slot=personnelAssignment(c);if(slot<0)return;std::string& metadata=personnelMetadata(slot);std::vector<EscortPersonnel::Member> list=EscortPersonnel::read(metadata);
 for(size_t i=0;i<list.size();++i)if(list[i].id==c->getHandle().toString()){personnelRemoveFollow(c,list[i]);list[i].enabled=on;
 if(on&&c->getAI()&&personnelLeader(slot)){Character* leader=personnelLeader(slot);{PersonnelOrderScope own;c->getAI()->getTaskSystem()->addOrder(FOLLOW_PLAYER_ORDER,leader->getHandle(),leader->getPosition(),true,false);}list[i].target=leader->getHandle().toString();}}
 EscortPersonnel::write(metadata,list);
}

void restoreCurrentPersonnel(){std::vector<EscortPersonnel::Member> list=EscortPersonnel::read(currentContract.routeRegions);for(size_t i=0;i<list.size();++i){Character* c=personnelPlayer(list[i].id);if(!c||!list[i].enabled||!c->getAI())continue;AITaskSytem* tasks=c->getAI()->getTaskSystem();if(tasks&&tasks->orders.list.empty())list[i].target.clear();}EscortPersonnel::write(currentContract.routeRegions,list);}

void releaseAllPersonnel(){for(int slot=0;slot<maximumActiveQuests;++slot){std::string& metadata=personnelMetadata(slot);std::vector<EscortPersonnel::Member> list=EscortPersonnel::read(metadata);for(size_t i=0;i<list.size();++i)personnelRemoveFollow(personnelPlayer(list[i].id),list[i]);EscortPersonnel::write(metadata,std::vector<EscortPersonnel::Member>());}}

// Guard is persistent intent. Missing native orders never mean user cancellation.
void personnelSyncGuard(Character*){}
void personnelManualOrder(Character* c){
 if(personnelInternalOrder||missionWorldChanging||missionRestorePending||MercenarieCleanup::disabled||!personnelEligible(c))return;
 PersonnelOrderScope own;personnelSetGuard(c,false);
}
