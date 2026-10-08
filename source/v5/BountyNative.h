#pragma once
#include "BountyContracts.h"
#include <kenshi/Character.h>
#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/CharStats.h>
#define CharacterMessage AICharacterMessage
#include <kenshi/AI/AI.h>
#undef CharacterMessage

namespace MercenarieV5 {
inline ActorIdentity bountyIdentity(const hand& h){
    ActorIdentity id;if(h.isNull())return id;
    id.type=(unsigned int)h.type;id.container=h.container;id.containerSerial=h.containerSerial;id.index=h.index;id.serial=h.serial;return id;
}
inline hand bountyHandle(const ActorIdentity& id){
    if(!id.valid())return hand();
    return hand(id.index,id.serial,(itemType)id.type,id.container,id.containerSerial);
}
// Deliberately restricted to verified station templates, their real leader and faction.
// A matching name, roaming inquisitor or ordinary Hundred Guardian is insufficient.
inline bool bountyIssuer(Character* c,IssuerFaction& faction){
    if(!c||c->isDead()||!c->getGameData()||!c->getFaction()||!c->getFaction()->data)return false;
    // The dedicated UC Police Chief record already identifies the officer.
    // His loaded platoon can differ from the original station template.
    if(c->getGameData()->stringID=="1833-gamedata.base"&&c->getFaction()->data->stringID=="defaultEmpireFactionSID"){faction=UnitedCities;return true;}
    if(!c->platoon||!c->platoon->me)return false;
    Platoon* p=c->platoon->me;
    if(!p->squadTemplate||c->platoon->getSquadLeader_theRealOne()!=c)return false;
    const std::string actor=c->getGameData()->stringID,squad=p->squadTemplate->stringID,fac=c->getFaction()->data->stringID;
    if(actor=="1833-gamedata.base"&&fac=="defaultEmpireFactionSID"&&(squad=="1120-gamedata.base"||squad=="1533541-__world reactions Slavers.mod")){faction=UnitedCities;return true;}
    if(actor=="18882-rebirth.mod"&&fac=="1083-gamedata.base"&&squad=="18886-rebirth.mod"){faction=HolyNation;return true;}
    if(actor=="11623-Dialogue (10).mod"&&fac=="11624-Dialogue (10).mod"&&squad=="59292-rebirth.mod"){faction=ShekKingdom;return true;}
    return false;
}
// Call only once, on the newly spawned mission leader. Never on restoration.
inline bool assignInitialNativeBounty(BountyContract& contract,Character* leader,Faction* issuerFaction){
    if(contract.state!=BountySpawning||contract.target.valid()||!leader||!issuerFaction||leader->isDead())return false;
    if(leader->crimes.getTotalBounty()!=0)return false;
    leader->crimes.unfairAddToBounty(issuerFaction,contract.offer.amount);
    return leader->crimes.getActualBounty(issuerFaction)==contract.offer.amount;
}
inline bool playerCarriesBounty(const BountyContract& contract,Character* carrier,Faction* playerFaction){
    if(!carrier||!playerFaction||carrier->getFaction()!=playerFaction||!contract.target.valid())return false;
    Character* prisoner=carrier->carryingObject.isNull()?0:carrier->carryingObject.getCharacter();
    return prisoner&&!prisoner->isDead()&&bountyIdentity(prisoner->getHandle())==contract.target;
}
inline void prepareBountyFighter(Character* c,int strength,const Ogre::Vector3& anchor){
    if(!c)return;
    if(CharStats* s=c->getStats()){
        s->_strength=(float)strength;s->_dexterity=(float)strength;s->_toughness=(float)strength;
        s->__meleeAttack=(float)strength;s->_meleeDefence=(float)strength;
        s->katanas=s->sabres=s->hackers=s->blunt=s->heavyWeapons=s->polearms=(float)strength;
    }
    if(AI* ai=c->getAI()){
        hand none;ai->setCenterOfMovementTarget(none);ai->setCenterOfMovement(anchor);
        ai->setManuveringFreedomLevel(AI::HOLD_GROUND);
    }
    c->addJob(HOLD_POSITION,0,false,false,anchor);
}
struct BountySpawnResult {
    Platoon* platoon;
    bool complete;
    BountySpawnResult():platoon(0),complete(false){}
};
// Returns partial squads to the caller for explicit cleanup on failure; never hides them.
// Template must contain exactly one leader plus the chosen guard count.
inline BountySpawnResult spawnBountyGroup(BountyContract& contract,RootObjectFactory* factory,
    GameData* squadTemplate,Faction* bandits,Faction* issuerFaction,TownBase* nearbyTown){
    BountySpawnResult result;
    if(contract.state!=BountySpawning||contract.target.valid()||!factory||!squadTemplate||!bandits||!issuerFaction)return result;
    Ogre::Vector3 anchor(contract.offer.x,contract.offer.y,contract.offer.z);
    result.platoon=factory->createRandomSquad(bandits,anchor,0,1,0,squadTemplate,0,0,0,true,hand(),0,1.0f,SQ_ROAMING,false);
    if(!result.platoon)return result;
    result.platoon->setPersistentSquad(true);
    ActivePlatoon* active=result.platoon->activePlatoon;
    if(!active||active->things.size()!=(unsigned int)(contract.offer.guards+1))return result;
    Character* leader=active->getSquadLeader_theRealOne();if(!leader)return result;
    std::vector<ActorIdentity> members;
    for(unsigned int i=0;i<active->things.size();++i){
        Character* c=static_cast<Character*>(active->things[i]);if(!c)return result;
        members.push_back(bountyIdentity(c->getHandle()));
        prepareBountyFighter(c,c==leader?contract.offer.combatMax:contract.offer.combatMin,anchor);
    }
    leader->setName(contract.offer.targetName);
    if(!assignInitialNativeBounty(contract,leader,issuerFaction))return result;
    result.complete=contract.bindSpawn(bountyIdentity(leader->getHandle()),members);
    return result;
}
enum BountyTransferResult { TransferRejected, TransferPending, TransferComplete, TransferRecoveryRequired };
// No reward is issued here. Remove the native bounty before transferring custody,
// so subsequent automatic police imprisonment cannot also award it.
// The caller must persist/settle the one contract payment only after TransferComplete.
inline BountyTransferResult transferBountyPrisoner(BountyContract& contract,Character* giver,
    Character* carrier,Faction* playerFaction){
    if(!giver||!carrier||giver->isDead()||giver->isCarryingSomething||!playerCarriesBounty(contract,carrier,playerFaction))return TransferRejected;
    IssuerFaction kind;if(!bountyIssuer(giver,kind)||kind!=contract.offer.faction)return TransferRejected;
    if(carrier->getPosition().squaredDistance(giver->getPosition())>100.0f)return TransferRejected;
    Character* target=carrier->carryingObject.getCharacter();Faction* law=giver->getFaction();
    if(!target||target->crimes.getActualBounty(law)!=contract.offer.amount||target->crimes.bountyAlreadyBeenClaimedByPlayer(law))return TransferRejected;
    if(!contract.beginHandover(contract.offer.id,bountyIdentity(giver->getHandle()),bountyIdentity(target->getHandle()),!target->isDead(),true))return TransferRejected;
    target->crimes.clearBounty(law);
    if(target->crimes.getActualBounty(law)!=0){contract.state=BountyActive;return TransferRejected;}
    carrier->dropCarriedObject(false,false);
    giver->pickupObject(target);
    if(!giver->carryingObject.isNull()&&giver->carryingObject.getCharacter()==target)return TransferComplete;
    // Native carry operations can complete asynchronously. Never pay or retry blindly.
    // Keep HandingOver until the runtime observes custody or rolls back explicitly.
    return TransferPending;
}
inline BountyTransferResult observeBountyTransfer(const BountyContract& contract,Character* giver){
    if(contract.state!=BountyHandingOver||!giver||!(bountyIdentity(giver->getHandle())==contract.offer.issuer))return TransferRejected;
    Character* target=bountyHandle(contract.target).getCharacter();
    if(!target)return TransferPending;
    if(target->isDead())return TransferRecoveryRequired;
    if(!giver->carryingObject.isNull()&&giver->carryingObject.getCharacter()==target)return TransferComplete;
    return TransferPending;
}
}
