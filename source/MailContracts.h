#pragma once

#include <algorithm>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace MailContracts {

enum SenderRole { RoleUndefined=0, RoleBarman=1, RolePolice=2, RoleMerchant=3, RoleShinobi=4 };
enum MissionStatus { MailOffered=0, MailActive=1, MailCompleted=2, MailFailed=3, MailCancelled=4 };

inline bool networkAllows(SenderRole sender,SenderRole recipient){
    if(sender==RoleBarman)return recipient==RoleBarman||recipient==RolePolice||recipient==RoleMerchant;
    if(sender==RolePolice)return recipient==RolePolice;
    if(sender==RoleMerchant)return recipient==RoleMerchant||recipient==RolePolice||recipient==RoleBarman;
    if(sender==RoleShinobi)return recipient==RoleShinobi;
    return false;
}

struct Step {
    std::string stepId,letterHandle,recipientId,recipientName,townId,townName,carrierId,carrierName;
    SenderRole recipientRole;
    bool delivered;
    double distanceKm;
    Step():recipientRole(RoleUndefined),delivered(false),distanceKm(0){}
    template<class Archive> void archive(Archive& a){
        a.field(stepId);a.field(letterHandle);a.field(recipientId);a.field(recipientName);
        a.field(townId);a.field(townName);a.field(carrierId);a.field(carrierName);
        a.field(recipientRole);a.field(delivered);a.field(distanceKm);
    }
};

struct Contract {
    int schemaVersion;
    std::string contractId,offerId,senderId,senderName,originTownId,originTownName;
    SenderRole senderRole;
    MissionStatus status;
    std::vector<Step> steps;
    std::vector<int> suggestedRoute;
    bool urgent,delegated,rewardPaid,resultRolled,delegatedSuccess;
    double acceptedAtWorldHour,deadlineWorldHour;
    int reward,guildXp,reputation;
    // Frozen world coordinates: origin, then one destination per step (x/y/z).
    // A town data SID identifies a template, not necessarily the physical town.
    std::vector<double> routePoints;
    std::string originInstanceId;
    Contract():schemaVersion(2),senderRole(RoleUndefined),status(MailOffered),urgent(false),delegated(false),rewardPaid(false),resultRolled(false),delegatedSuccess(false),acceptedAtWorldHour(0),deadlineWorldHour(0),reward(0),guildXp(0),reputation(0){}
    template<class Archive> void archive(Archive& a){
        a.field(schemaVersion);a.field(contractId);a.field(offerId);a.field(senderId);a.field(senderName);
        a.field(originTownId);a.field(originTownName);a.field(senderRole);a.field(status);a.field(steps);
        a.field(suggestedRoute);a.field(urgent);a.field(delegated);a.field(rewardPaid);a.field(resultRolled);
        a.field(delegatedSuccess);a.field(acceptedAtWorldHour);a.field(deadlineWorldHour);
        a.field(reward);a.field(guildXp);a.field(reputation);
        if(schemaVersion<1||schemaVersion>2)throw std::runtime_error("unsupported mail contract schema");
        if(schemaVersion>=2){a.field(routePoints);a.field(originInstanceId);}
        else if(a.reading){routePoints.clear();originInstanceId.clear();}
        if(!routePoints.empty()&&routePoints.size()!=(steps.size()+1)*3)throw std::runtime_error("invalid mail route snapshot");
        for(size_t i=0;i<routePoints.size();++i)if(!(routePoints[i]>=-10000000&&routePoints[i]<=10000000))throw std::runtime_error("invalid mail route coordinate");
    }
};

inline int deliveredCount(const Contract& c){int n=0;for(size_t i=0;i<c.steps.size();++i)if(c.steps[i].delivered)++n;return n;}
inline bool allDelivered(const Contract& c){return !c.steps.empty()&&deliveredCount(c)==(int)c.steps.size();}
// Keep legacy archive fields, but message deliveries no longer have a time limit.
inline bool hasDeadline(const Contract&){return false;}
inline bool expired(const Contract&,double){return false;}

inline bool validDefinition(const Contract& c){
    if(c.contractId.empty()||c.offerId.empty()||c.senderId.empty()||c.originTownId.empty()||c.senderRole==RoleUndefined)return false;
    if(c.steps.empty()||c.steps.size()>3)return false;
    std::set<std::string> stepIds,towns,recipients,letters;
    for(size_t i=0;i<c.steps.size();++i){const Step& s=c.steps[i];
        if(s.stepId.empty()||s.recipientId.empty()||s.townId.empty()||s.townId==c.originTownId||s.recipientRole==RoleUndefined||s.distanceKm<=0)return false;
        if(!networkAllows(c.senderRole,s.recipientRole))return false;
        if(!stepIds.insert(s.stepId).second||!towns.insert(s.townId).second||!recipients.insert(s.recipientId).second)return false;
        if(!c.delegated&&(s.letterHandle.empty()||!letters.insert(s.letterHandle).second))return false;
    }
    if(c.urgent&&(c.deadlineWorldHour<=c.acceptedAtWorldHour))return false;
    if(!c.suggestedRoute.empty()){
        if(c.suggestedRoute.size()!=c.steps.size())return false;std::set<int> indices;
        for(size_t i=0;i<c.suggestedRoute.size();++i)if(c.suggestedRoute[i]<0||c.suggestedRoute[i]>=(int)c.steps.size()||!indices.insert(c.suggestedRoute[i]).second)return false;
    }
    return true;
}

enum DeliveryResult { DeliveryAccepted,DeliveryWrongContract,DeliveryWrongLetter,DeliveryWrongRecipient,DeliveryAlreadyDone,DeliveryInactive,DeliveryExpired };

inline DeliveryResult deliver(Contract& c,const std::string& contractId,const std::string& letterHandle,const std::string& recipientId,double worldHour){
    if(c.contractId!=contractId)return DeliveryWrongContract;
    if(c.status!=MailActive)return DeliveryInactive;
    if(expired(c,worldHour)){c.status=MailFailed;return DeliveryExpired;}
    Step* letter=0;for(size_t i=0;i<c.steps.size();++i)if(c.steps[i].letterHandle==letterHandle){letter=&c.steps[i];break;}
    if(!letter)return DeliveryWrongLetter;
    if(letter->recipientId!=recipientId)return DeliveryWrongRecipient;
    if(letter->delivered)return DeliveryAlreadyDone;
    letter->delivered=true;letter->carrierId.clear();letter->carrierName.clear();
    if(allDelivered(c))c.status=MailCompleted;
    return DeliveryAccepted;
}

inline void expire(Contract& c,double worldHour){if(expired(c,worldHour)){c.status=MailFailed;for(size_t i=0;i<c.steps.size();++i)if(!c.steps[i].delivered)c.steps[i].letterHandle.clear();}}
inline void cancel(Contract& c){if(c.status==MailActive||c.status==MailOffered){c.status=MailCancelled;for(size_t i=0;i<c.steps.size();++i)c.steps[i].letterHandle.clear();}}
inline bool settleReward(Contract& c){if(c.status!=MailCompleted||c.rewardPaid)return false;c.rewardPaid=true;return true;}

inline double urgentDeadlineHours(double estimatedTravelHours){return std::max(6.0,estimatedTravelHours*1.75);}

}
