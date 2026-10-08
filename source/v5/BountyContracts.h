#include "../Localization.h"
#include "../InternalConfig.h"
#pragma once
#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include <sstream>

// Engine-independent V5 rules. No global escort state and no runtime pointer IDs.
namespace MercenarieV5 {
enum BountyState { BountyNone, BountySpawning, BountyActive, BountyHandingOver,
    BountyCompleted, BountyTargetDead, BountyAbandoned, BountyPlayerCage, BountyLost };
enum IssuerFaction { UnitedCities, HolyNation, ShekKingdom };
inline int bountyDifficulty(int amount){return amount<21000?0:amount<36000?1:2;}
inline double bountyRaidDelay(int difficulty){return MercenarieConfig::Bounties::BaseRaidDelaySeconds+MercenarieConfig::Bounties::DifficultyRaidDelaySeconds*difficulty;}
// Sticky raid decision: leaving the area pauses time, never restores the camp.
inline int advanceBountyEncounter(double& seconds,int encounter,bool inside,bool nearPoint,double elapsed,int difficulty){
    if(!inside||elapsed<=0)return encounter;
    if(encounter==2){seconds+=elapsed;return 2;}
    if(seconds>=bountyRaidDelay(difficulty))return 2;
    seconds+=elapsed;
    if(nearPoint)return 1;
    return seconds>=bountyRaidDelay(difficulty)?2:0;
}
struct ActorIdentity {
    unsigned int type,container,containerSerial,index,serial;
    ActorIdentity():type(0),container(0),containerSerial(0),index(0),serial(0){}
    bool valid()const{return container||containerSerial||index||serial;}
    bool operator==(const ActorIdentity& b)const{return type==b.type&&container==b.container&&containerSerial==b.containerSerial&&index==b.index&&serial==b.serial;}
    template<class Archive> void archive(Archive& a){a.field(type);a.field(container);a.field(containerSerial);a.field(index);a.field(serial);}
};
struct BountyOffer {
    std::string id,targetName,areaId;
    IssuerFaction faction;
    ActorIdentity issuer;
    int amount,guards,combatMin,combatMax;
    float x,y,z;
    BountyOffer():faction(UnitedCities),amount(7000),guards(2),combatMin(15),combatMax(25),x(0),y(0),z(0){}
    template<class Archive> void archive(Archive& a){a.field(id);a.field(targetName);a.field(areaId);a.field(faction);issuer.archive(a);a.field(amount);a.field(guards);a.field(combatMin);a.field(combatMax);a.field(x);a.field(y);a.field(z);}
};
inline bool revisedBounty(const BountyOffer& o){return o.id.find(":economy2")!=std::string::npos;}
inline int bountyDifficulty(const BountyOffer& o){return revisedBounty(o)?(o.amount<=10000?0:o.amount<=15000?1:2):bountyDifficulty(o.amount);}
inline int rollBountyDifficulty(unsigned int roll){roll%=100;return roll<55?0:roll<85?1:2;}
inline void scaleBountyDifficulty(BountyOffer& o){
    if(o.amount<7000||o.amount>50000)throw std::runtime_error("invalid bounty value");
    const int strength=o.amount-7000;
    const int range=revisedBounty(o)?18000:43000;
    o.guards=2+strength*8/range;
    o.combatMin=15+strength*45/range;
    o.combatMax=o.combatMin+10;
}
// Caller supplies a save-persisted issuer ID and RNG. No wall-clock randomness.
template<class Random> std::vector<BountyOffer> makeBountyOffers(const std::string& issuerId,
    const ActorIdentity& issuer,IssuerFaction faction,unsigned int rotation,Random& random){
    std::vector<BountyOffer> offers;std::set<int> amounts;
    for(unsigned int i=0;i<3;++i){
        BountyOffer o;o.issuer=issuer;o.faction=faction;
        std::ostringstream id;id<<issuerId<<":"<<rotation<<":"<<i<<":economy2";o.id=id.str();
        const int difficulty=rollBountyDifficulty(static_cast<unsigned int>(random()));
        const int minimum=difficulty==0?7000:difficulty==1?10001:15001;
        const int maximum=difficulty==0?10000:difficulty==1?15000:25000;
        o.amount=minimum+static_cast<unsigned int>(random())%(maximum-minimum+1);
        // Bounded even with a constant RNG; offers from this issuer cannot duplicate values.
        while(amounts.count(o.amount))o.amount=o.amount==maximum?minimum:o.amount+1;
        amounts.insert(o.amount);scaleBountyDifficulty(o);
        offers.push_back(o);
    }return offers;
}
struct BountyBoard {
    double nextRefreshHour;
    unsigned int rotation;
    std::vector<BountyOffer> offers;
    BountyBoard():nextRefreshHour(0),rotation(0){}
    bool refreshDue(double gameHours)const{return offers.empty()||gameHours>=nextRefreshHour;}
    bool consume(const std::string& id,double gameHours){
        if(refreshDue(gameHours))return false;
        for(std::vector<BountyOffer>::iterator i=offers.begin();i!=offers.end();++i)if(i->id==id){offers.erase(i);return true;}
        return false;
    }
    template<class Random> void refresh(double gameHours,const std::string& issuerId,const ActorIdentity& issuer,IssuerFaction faction,Random& random){
        if(!refreshDue(gameHours))return;
        offers=makeBountyOffers(issuerId,issuer,faction,++rotation,random);nextRefreshHour=gameHours+MercenarieConfig::Bounties::BoardRefreshHours;
    }
    template<class Archive> void archive(Archive& a){
        a.field(nextRefreshHour);a.field(rotation);unsigned int n=(unsigned int)offers.size();a.field(n);
        if(n>3)throw std::runtime_error("invalid bounty board");if(a.reading)offers.resize(n);
        for(unsigned int i=0;i<n;++i)offers[i].archive(a);
    }
};
struct BountyContract {
    BountyState state;
    BountyOffer offer;
    ActorIdentity target;
    std::vector<ActorIdentity> group;
    bool paymentRecorded;
    BountyContract():state(BountyNone),paymentRecorded(false){}
    bool occupied()const{return state==BountySpawning||state==BountyActive||state==BountyHandingOver;}
    bool accept(const BountyOffer& chosen){
        if(occupied()||chosen.id.empty()||!chosen.issuer.valid()||chosen.areaId.empty()||chosen.targetName.empty())return false;
        if(chosen.amount<7000||chosen.amount>50000)return false;
        offer=chosen;target=ActorIdentity();group.clear();paymentRecorded=false;state=BountySpawning;return true;
    }
    bool bindSpawn(const ActorIdentity& leader,const std::vector<ActorIdentity>& members){
        if(state!=BountySpawning||target.valid()||!leader.valid()||members.empty())return false;
        bool found=false;for(size_t i=0;i<members.size();++i)if(members[i]==leader)found=true;
        if(!found)return false;target=leader;group=members;state=BountyActive;return true;
    }
    // A missing streamed-out handle is NOT proof of death or disappearance.
    void observe(bool loaded,bool dead,bool inPlayerCage){
        if(state!=BountyActive||!loaded)return;
        if(dead)state=BountyTargetDead;else if(inPlayerCage)state=BountyPlayerCage;
    }
    bool abandon(){if(state!=BountyActive)return false;state=BountyAbandoned;return true;}
    bool beginHandover(const std::string& contractId,const ActorIdentity& giver,const ActorIdentity& carried,bool alive,bool playerCarrier){
        if(state!=BountyActive||contractId!=offer.id||!(giver==offer.issuer)||!(carried==target)||!alive||!playerCarrier||paymentRecorded)return false;
        state=BountyHandingOver;return true;
    }
    // This acknowledges the engine transfer/payment; it never pays money itself.
    // Runtime adapter must use one payment authority, including vanilla cage rewards.
    bool acknowledgeHandover(bool transferred,bool singlePaymentConfirmed){
        if(state!=BountyHandingOver||!transferred||!singlePaymentConfirmed||paymentRecorded)return false;
        paymentRecorded=true;state=BountyCompleted;return true;
    }
    bool reputationPenalty()const{return state==BountyTargetDead;}
    template<class Archive> void archive(Archive& a){
        a.field(state);offer.archive(a);target.archive(a);a.field(paymentRecorded);
        unsigned int n=(unsigned int)group.size();a.field(n);if(n>64)throw std::runtime_error("invalid bounty group");
        if(a.reading)group.resize(n);for(unsigned int i=0;i<n;++i)group[i].archive(a);
        if((state<BountyNone||state>BountyLost||offer.amount<7000||offer.amount>50000||offer.faction<UnitedCities||offer.faction>ShekKingdom))throw std::runtime_error("invalid bounty contract");
        if((state==BountyActive||state==BountyHandingOver||state==BountyCompleted)&&!target.valid())throw std::runtime_error("missing bounty identity");
    }
};
enum BountyText { AskContracts, AlreadyHunting, HavePrisoner, AliveRequired, CageCancelled, TargetDied, SearchArea, BountyTitle };
inline const char* bountyText(BountyText key,bool french){
    if(key<AskContracts||key>BountyTitle)return "";
    Loc::select(french?"fr":"en");
    const char* keys[]={"ui.do_you_have_any_contracts_for_my_guild","ui.one_contract_at_a_time_leave_some_work_for","ui.i_ve_got_your_target","ui.bring_this_target_back_alive","ui.bounty_contract_cancelled_the_target_was_put_in_your","ui.bounty_hunt_failed_the_target_has_died","ui.search_area","ui.bounty_hunt_414870e"};
    return Loc::text(keys[key]);
}
}
