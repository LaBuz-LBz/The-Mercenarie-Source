#include "MailContracts.h"
#include "DelegatedMissionTiming.h"
#include "MailOfferPlan.h"
#include "MailPresentation.h"
#include "MailRoutePlanner.h"
#include "ContractDestinationRules.h"
#include "src/EscortEconomy.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <cstring>

using namespace MailContracts;

struct Archive {
    bool reading;size_t cursor;std::vector<char> bytes;
    Archive(bool load=false):reading(load),cursor(0){}
    template<class T> void scalar(T& value){if(reading){assert(cursor+sizeof(T)<=bytes.size());std::memcpy(&value,&bytes[cursor],sizeof(T));cursor+=sizeof(T);}else{const char* p=reinterpret_cast<const char*>(&value);bytes.insert(bytes.end(),p,p+sizeof(T));}}
    void field(std::string& value){size_t n=value.size();scalar(n);if(reading){assert(cursor+n<=bytes.size());value.assign(&bytes[cursor],n);cursor+=n;}else bytes.insert(bytes.end(),value.begin(),value.end());}
    void field(bool& value){unsigned char v=value?1:0;scalar(v);if(reading)value=v!=0;}
    template<class T> void field(T& value){scalar(value);}
    template<class T> void field(std::vector<T>& values){size_t n=values.size();scalar(n);if(reading)values.resize(n);for(size_t i=0;i<n;++i)field(values[i]);}
    void field(Step& value){value.archive(*this);}
    void field(Contract& value){value.archive(*this);}
};

static Step step(const char* id,const char* letter,const char* npc,const char* town,SenderRole role,double km){Step s;s.stepId=id;s.letterHandle=letter;s.recipientId=npc;s.townId=town;s.recipientRole=role;s.distanceKm=km;return s;}
static Contract contract(const char* id){Contract c;c.contractId=id;c.offerId=std::string("offer-")+id;c.senderId="hub-barman";c.originTownId="hub";c.senderRole=RoleBarman;c.status=MailActive;c.steps.push_back(step("s1","letter-a","squin-police","squin",RolePolice,120));c.steps.push_back(step("s2","letter-b","stack-barman","stack",RoleBarman,190));return c;}

int main(){
    assert(std::string(MailPresentation::typeKey(1))=="ui.message_delivery");
    assert(std::string(MailPresentation::typeKey(2))=="ui.multiple_message_delivery");
    assert(ContractDestinationRules::blocked("1532824-rebirth.mod"));
    assert(ContractDestinationRules::blocked("49367-rebirth.mod"));
    assert(!ContractDestinationRules::blocked("49386-rebirth.mod"));
    assert(networkAllows(RoleBarman,RoleMerchant)&&networkAllows(RolePolice,RolePolice));
    assert(!networkAllows(RolePolice,RoleBarman)&&networkAllows(RoleShinobi,RoleShinobi));
    Contract a=contract("A"),b=contract("B");b.steps[0].letterHandle="letter-c";b.steps[1].letterHandle="letter-d";
    assert(validDefinition(a)&&validDefinition(b));
    assert(deliver(a,"B","letter-a","squin-police",1)==DeliveryWrongContract);
    assert(deliver(a,"A","letter-c","squin-police",1)==DeliveryWrongLetter);
    assert(deliver(a,"A","letter-a","wrong-npc",1)==DeliveryWrongRecipient);
    assert(deliver(a,"A","letter-a","squin-police",1)==DeliveryAccepted);
    assert(deliver(a,"A","letter-a","squin-police",1)==DeliveryAlreadyDone);
    assert(deliveredCount(a)==1&&deliveredCount(b)==0&&a.status==MailActive);
    assert(deliver(a,"A","letter-b","stack-barman",1)==DeliveryAccepted&&a.status==MailCompleted);
    assert(settleReward(a)&&!settleReward(a));
    Contract duplicateTown=contract("D");duplicateTown.steps[1].townId="squin";assert(!validDefinition(duplicateTown));
    Contract origin=contract("O");origin.steps[0].townId="hub";assert(!validDefinition(origin));
    Contract wrongNetwork=contract("N");wrongNetwork.senderRole=RolePolice;wrongNetwork.steps[1].recipientRole=RoleBarman;assert(!validDefinition(wrongNetwork));
    Contract urgent=contract("U");urgent.urgent=true;urgent.acceptedAtWorldHour=100;urgent.deadlineWorldHour=110;assert(validDefinition(urgent));
    assert(!hasDeadline(urgent)&&!expired(urgent,100000));expire(urgent,100000);assert(urgent.status==MailActive&&!urgent.steps[0].letterHandle.empty());assert(deliver(urgent,"U","letter-a","squin-police",100000)==DeliveryAccepted&&urgent.status==MailActive);
    Contract cancelled=contract("C");cancel(cancelled);assert(cancelled.status==MailCancelled&&cancelled.steps[0].letterHandle.empty());assert(b.steps[0].letterHandle=="letter-c");
    assert(std::fabs(urgentDeadlineHours(10)-17.5)<.0001&&urgentDeadlineHours(1)==6.0);
    DelegatedMissionTiming::State timing=DelegatedMissionTiming::start(DelegatedMissionTiming::ActivityMailDelivery,90,1000);
    assert(std::fabs(timing.travelHours-30.0)<.0001&&timing.activityHours==0.0&&timing.exactReturnWorldHour==1030.0);
    MailOfferPlan::Plan plan;plan.urgent=true;MailOfferPlan::Destination d1;d1.townId="squin";d1.townName="Squin";d1.distanceKm=120;d1.role=RolePolice;plan.destinations.push_back(d1);MailOfferPlan::Destination d2;d2.townId="stack";d2.townName="Stack";d2.distanceKm=80;d2.role=RoleBarman;plan.destinations.push_back(d2);MailOfferPlan::Plan decoded;assert(MailOfferPlan::decode(MailOfferPlan::encode(plan),decoded)&&decoded.urgent&&decoded.destinations.size()==2&&decoded.destinations[1].townId=="stack");
    typedef MailRoutePlanner::Point P;typedef MailRoutePlanner::Candidate C;P linearOrigin(5,0),linearFirst(6,0);std::vector<C> linear;linear.push_back(C(4,P(4,0),2));linear.push_back(C(7,P(7,0),1));linear.push_back(C(8,P(8,0),2));std::vector<C> forward=MailRoutePlanner::coherentCandidates(linearOrigin,linearOrigin,linearFirst,linearFirst,linear);assert(forward.size()==2&&forward[0].index!=4&&forward[1].index!=4);
    assert(!MailRoutePlanner::coherent(linearOrigin,linearOrigin,linearFirst,linearFirst,P(4,0)));
    assert(MailRoutePlanner::coherent(P(0,0),P(0,0),P(10,0),P(10,0),P(18,5)));
    std::vector<C> noThird;noThird.push_back(C(1,P(4,0),1));assert(MailRoutePlanner::coherentCandidates(linearOrigin,linearOrigin,linearFirst,linearFirst,noThird).empty());
    std::vector<C> one;one.push_back(C(7,P(7,1),2));assert(MailRoutePlanner::coherentCandidates(linearOrigin,linearOrigin,linearFirst,linearFirst,one).size()==1);std::vector<C> none;assert(MailRoutePlanner::coherentCandidates(linearOrigin,linearOrigin,linearFirst,linearFirst,none).empty());
    std::vector<double> routeLegs;routeLegs.push_back(30);routeLegs.push_back(20);routeLegs.push_back(25);double routeKm=MailRoutePlanner::totalDistanceKm(routeLegs);assert(routeKm==75);assert(std::fabs(urgentDeadlineHours(DelegatedMissionTiming::travelHours(routeKm))-43.75)<.0001);DelegatedMissionTiming::State routeTiming=DelegatedMissionTiming::start(DelegatedMissionTiming::ActivityMailDelivery,routeKm,10);assert(routeTiming.totalDistanceKm==75&&routeTiming.travelHours==25&&routeTiming.activityHours==0);
    MailOfferPlan::Plan ordered;ordered.urgent=true;for(int i=0;i<3;++i){MailOfferPlan::Destination d;d.townId=std::string("town")+char('A'+i);d.townName=d.townId;d.distanceKm=routeLegs[i];d.role=RoleBarman;ordered.destinations.push_back(d);}MailOfferPlan::Plan orderedReload;assert(MailOfferPlan::decode(MailOfferPlan::encode(ordered),orderedReload)&&orderedReload.destinations[0].townId=="townA"&&orderedReload.destinations[2].townId=="townC"&&MailRoutePlanner::totalDistanceKm(routeLegs)==75);
    Contract freeOrder=contract("FREE");assert(deliver(freeOrder,"FREE","letter-b","stack-barman",1)==DeliveryAccepted&&deliver(freeOrder,"FREE","letter-a","squin-police",1)==DeliveryAccepted&&freeOrder.status==MailCompleted);
    std::vector<C> variety;variety.push_back(C(7,P(7,0),1));variety.push_back(C(8,P(8,2),2));variety.push_back(C(9,P(9,-2),3));assert(MailRoutePlanner::coherentCandidates(linearOrigin,linearOrigin,linearFirst,linearFirst,variety).size()>=2);
    EscortContractData price;price.type=MCT_MAIL;price.distanceKm=100;price.dangerLevel=1;EscortEconomy::calculate(price,0);assert(price.totalPay==3000);price.urgent=true;EscortEconomy::calculate(price,0);assert(price.totalPay==4000);
    Contract saved=contract("SAVE");saved.originTownName="The Hub";saved.steps[0].townName="Squin";saved.steps[0].carrierName="Kang";saved.steps[0].delivered=true;saved.urgent=true;saved.acceptedAtWorldHour=100;saved.deadlineWorldHour=140;saved.suggestedRoute.push_back(1);saved.suggestedRoute.push_back(0);Archive writer;saved.archive(writer);Archive reader(true);reader.bytes=writer.bytes;Contract restored;restored.archive(reader);assert(reader.cursor==reader.bytes.size());assert(restored.contractId=="SAVE"&&restored.originTownName=="The Hub"&&restored.steps.size()==2&&restored.steps[0].delivered&&restored.steps[0].carrierName=="Kang"&&restored.deadlineWorldHour==140&&restored.suggestedRoute[0]==1);
    std::cout<<"mail contract tests: OK\n";
}
