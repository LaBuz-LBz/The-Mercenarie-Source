#pragma once
#include "../EscortEconomy.h"
#include "../../ContractGroupPlan.h"
#include "../../FrozenPriceTerms.h"
#include "../../ContractReroll.h"
#include "../../ContractFactors.h"
#include <sstream>
#include <locale>

// The one V6 estimate implementation for board offers and accepted contracts.
// Keep frozen basePay/PRICE metadata authoritative after negotiation and reload.
// No UI, engine calls or live globals; mutates only the supplied contract.
namespace ContractPricing {
    inline int rewardDangerLevel(int fallback,const std::string& metadata){
        size_t risk=metadata.find(";PAYRISK=");
        if(risk!=std::string::npos){int n=0;char end=0;std::istringstream in(metadata.substr(risk+9));if((in>>n>>end)&&end==';'&&n>=1&&n<=5)return n;}
        return fallback;
    }
    inline void calculateEstimatedContract(EscortContractData& c,int pct)
    {
        if(c.routeRegions.find(";PRICE85;")!=std::string::npos && (c.type==MCT_MAIL||c.type==MCT_ESCORT||c.type==MCT_CARAVAN)){
            const int rates[]={100,100,115,135,160,200};
            int risk=std::max(1,std::min(5,c.dangerLevel));
            if(c.basePay<=0){
                c.reward=MercRewardBreakdown();
                c.reward.base=c.type==MCT_MAIL?1500:c.type==MCT_ESCORT?2500:3000;
                int perKm=c.type==MCT_MAIL?75:c.type==MCT_ESCORT?130:90;
                double km=_finite(c.distanceKm)?std::max(0.0f,c.distanceKm):0;
                c.reward.distance=(int)std::min(100000.0,km*perKm+.5);
                int basis=c.reward.base+c.reward.distance;
                ContractFactors::Quote factors;
                if(c.type!=MCT_MAIL&&ContractFactors::decode(c.routeRegions,factors))c.reward.mission+=ContractFactors::supplement(basis,factors.fragility+factors.cargoPercent);
                ContractGroupPlan::Plan group;
                if(c.type!=MCT_MAIL&&ContractGroupPlan::decode(c.routeRegions,group))c.reward.mission+=basis*ContractGroupPlan::extraClientPercent(group)/100;
                if(c.type==MCT_MAIL&&c.urgent)c.reward.mission+=basis/3;
                c.reward.beforeDanger=basis+c.reward.mission;
                c.reward.afterDanger=(int)(((long long)c.reward.beforeDanger*rates[risk]+50)/100);
                c.reward.afterRarity=c.reward.afterDanger;
                c.reward.guildHouseBonus=c.source==MCS_GUILD_HOUSE?c.reward.afterRarity*EscortConfig::GuildHouseBonusPercent/100:0;
                c.basePay=EscortEconomy::cap(c.reward.afterRarity+c.reward.guildHouseBonus);
                if(ContractReroll::marked(c.routeRegions))c.basePay=ContractReroll::reward(c.basePay,true);
            }
            int quote[8];int breakdownBase=c.basePay;
            if(ContractReroll::marked(c.routeRegions)&&FrozenPriceTerms::read(c.routeRegions,quote))breakdownBase=quote[7];
            FrozenPriceTerms::restore(c.routeRegions,breakdownBase,c.reward);
            c.danger=rates[risk]/100.0f;
            c.negotiationPercent=std::max(EscortConfig::NegotiationMinPercent,std::min(EscortConfig::NegotiationMaxPercent,pct));
            c.reward.beforeNegotiation=c.basePay;c.reward.negotiation=c.basePay*c.negotiationPercent/100;
            c.totalPay=EscortEconomy::cap(c.basePay+c.reward.negotiation);c.reward.finalBeforeMissionBonuses=c.totalPay;c.finalPay=c.totalPay-c.advance;
            c.initialRate=c.distanceKm>0?c.basePay/c.distanceKm:0;c.negotiatedRate=c.distanceKm>0?c.totalPay/c.distanceKm:0;
            return;
        }
        if(c.routeRegions.find("V6EST:")!=0){EscortEconomy::calculate(c,pct);return;}
        // Region-max difficulty changes combat, not this release's reward scale.
        // Also retain the original financial level if cargo forces a re-quote.
        int financialLevel=rewardDangerLevel(c.dangerLevel,c.routeRegions);
        const float danger=EscortEconomy::dangerMultiplier(financialLevel);
        // A nonzero basePay is the frozen offer, including after save/load.
        if(c.basePay<=0){
            EscortEconomy::calculate(c,0);
            c.reward.base=c.type==MCT_ESCORT?EscortConfig::EscortBase:c.type==MCT_CARAVAN?EscortConfig::CaravanBase:EscortConfig::ScienceBase;
            c.reward.distance=(int)(c.distanceKm*(c.type==MCT_ESCORT?EscortConfig::EscortPerKm:c.type==MCT_CARAVAN?EscortConfig::CaravanPerKm:EscortConfig::SciencePerKm));
            ContractFactors::Quote factors;bool newFactors=ContractFactors::decode(c.routeRegions,factors);
            if(newFactors){
                if(c.type==MCT_ESCORT)c.reward.mission=0; // Wealth is not fragility.
                if(c.type==MCT_CARAVAN)c.reward.mission-=EscortEconomy::cargoBonus(c.cargoClass);
                c.reward.mission+=ContractFactors::supplement(c.reward.base+c.reward.distance,factors.fragility+factors.cargoPercent);
            }
            ContractGroupPlan::Plan group;if(ContractGroupPlan::decode(c.routeRegions,group)){
                // Replace the old caravan headcount lump sum, never charge both.
                // Animals are not additional clients. These templates contain
                // contracted members, not optional hired guards.
                if(c.type==MCT_CARAVAN)c.reward.mission-=EscortEconomy::caravanSizeBonus(c.caravanSize);
                c.reward.mission+=(c.reward.base+c.reward.distance)*ContractGroupPlan::extraClientPercent(group)/100;
            }
            c.reward.beforeDanger=c.reward.base+c.reward.distance+c.reward.mission+c.reward.environment;
            c.reward.afterDanger=(int)(c.reward.beforeDanger*danger+.5f);
            c.reward.afterRarity=(int)(c.reward.afterDanger*EscortEconomy::rarityMultiplier(c.rarity)+.5f);
            c.reward.guildHouseBonus=c.source==MCS_GUILD_HOUSE?c.reward.afterRarity*EscortConfig::GuildHouseBonusPercent/100:0;
            c.basePay=EscortEconomy::cap(c.reward.afterRarity+c.reward.guildHouseBonus);
            if(ContractReroll::marked(c.routeRegions))c.basePay=ContractReroll::reward(std::max(EscortEconomy::minimumFor(c.type),c.basePay),true);
        }
        // Keep the original line-item breakdown; the reroll reduction is applied
        // to the frozen financial base before negotiation, not to mission stats.
        int quote[8];int breakdownBase=c.basePay;
        if(ContractReroll::marked(c.routeRegions)&&FrozenPriceTerms::read(c.routeRegions,quote))breakdownBase=quote[7];
        FrozenPriceTerms::restore(c.routeRegions,breakdownBase,c.reward);
        c.danger=danger;c.negotiationPercent=std::max(EscortConfig::NegotiationMinPercent,std::min(EscortConfig::NegotiationMaxPercent,pct));
        c.reward.beforeNegotiation=c.basePay;c.reward.negotiation=c.basePay*c.negotiationPercent/100;
        c.totalPay=std::max(ContractReroll::marked(c.routeRegions)?0:EscortEconomy::minimumFor(c.type),EscortEconomy::cap(c.basePay+c.reward.negotiation));c.reward.finalBeforeMissionBonuses=c.totalPay;c.finalPay=c.totalPay-c.advance;
        c.initialRate=c.distanceKm>0?c.basePay/c.distanceKm:0;c.negotiatedRate=c.distanceKm>0?c.totalPay/c.distanceKm:0;
    }

}
