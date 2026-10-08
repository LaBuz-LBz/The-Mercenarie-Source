#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include "../InternalConfig.h"
#include "FinanceJournal.h"

enum FiscalOrganisation { FISCAL_UC=0, FISCAL_MERCENARY_GUILD=1 };
enum FiscalRelation { FREL_NORMAL=0, FREL_ALLIED=1, FREL_REBEL=2 };
enum FiscalEntryStatus { FENTRY_DUE=0, FENTRY_PAID=1, FENTRY_NONTAXABLE=2, FENTRY_FAILED=3, FENTRY_CANCELLED=4, FENTRY_UNCOLLECTIBLE=5 };
enum FiscalState {
    FSTATE_NORMAL=0,FSTATE_COLLECTION_DUE,FSTATE_EXTENSION,FSTATE_WARNING,
    FSTATE_WAITING_FOR_PLAYER,FSTATE_COLLECTOR_TRAVELLING,FSTATE_DIALOGUE,
    FSTATE_RAID_1_PENDING,FSTATE_RAID_1_COMBAT,FSTATE_RAID_1_DEFEATED,
    FSTATE_RAID_2_PENDING,FSTATE_RAID_2_COMBAT,FSTATE_RAID_2_DEFEATED,
    FSTATE_RAID_3_PENDING,FSTATE_RAID_3_COMBAT,FSTATE_PLAYER_DEFEATED,FSTATE_REBEL
};

struct FiscalEntry {
    std::string fiscalId,contractId,type,rarity,source,origin,destination;
    double createdHour;int grossIncome,bonusIncome,tips,taxableIncome;
    bool ucTaxable,ucAllied,guildAllied;int ucRate,guildRate,ucTax,guildTax,ucPaid,guildPaid;
    FiscalEntryStatus ucStatus,guildStatus;
    FiscalEntry():createdHour(0),grossIncome(0),bonusIncome(0),tips(0),taxableIncome(0),ucTaxable(false),ucAllied(false),guildAllied(false),ucRate(0),guildRate(0),ucTax(0),guildTax(0),ucPaid(0),guildPaid(0),ucStatus(FENTRY_NONTAXABLE),guildStatus(FENTRY_NONTAXABLE){}
};

struct FiscalOrganisationState {
    FiscalOrganisation organisation;FiscalState state;long long debt,paid,uncollectible,newTaxesAtLastVisit;
    double nextCollectionHour,deadlineHour;bool firstIntroductionDone,extensionUsed,preArrivalNotified;int raidsWon,activeRaid;
    std::string targetHouseId;bool rebel;
    FiscalOrganisationState(FiscalOrganisation o=FISCAL_UC):organisation(o),state(FSTATE_NORMAL),debt(0),paid(0),uncollectible(0),newTaxesAtLastVisit(0),nextCollectionHour(0),deadlineHour(0),firstIntroductionDone(false),extensionUsed(false),preArrivalNotified(false),raidsWon(0),activeRaid(0),rebel(false){}
};

struct FiscalLedger {
    Finance::Journal cash;
    int schemaVersion;unsigned long nextId;std::vector<FiscalEntry> entries;FiscalOrganisationState organisations[2];
    FiscalLedger():schemaVersion(1),nextId(1){organisations[0]=FiscalOrganisationState(FISCAL_UC);organisations[1]=FiscalOrganisationState(FISCAL_MERCENARY_GUILD);}
    void swap(FiscalLedger& other){
        cash.swap(other.cash);entries.swap(other.entries);std::swap(schemaVersion,other.schemaVersion);std::swap(nextId,other.nextId);
        for(int i=0;i<2;++i){
            FiscalOrganisationState& a=organisations[i];FiscalOrganisationState& b=other.organisations[i];
            a.targetHouseId.swap(b.targetHouseId);
            std::swap(a.organisation,b.organisation);std::swap(a.state,b.state);std::swap(a.debt,b.debt);std::swap(a.paid,b.paid);std::swap(a.uncollectible,b.uncollectible);std::swap(a.newTaxesAtLastVisit,b.newTaxesAtLastVisit);
            std::swap(a.nextCollectionHour,b.nextCollectionHour);std::swap(a.deadlineHour,b.deadlineHour);std::swap(a.firstIntroductionDone,b.firstIntroductionDone);std::swap(a.extensionUsed,b.extensionUsed);std::swap(a.preArrivalNotified,b.preArrivalNotified);std::swap(a.raidsWon,b.raidsWon);std::swap(a.activeRaid,b.activeRaid);std::swap(a.rebel,b.rebel);
        }
    }
    static int rate(FiscalOrganisation o,FiscalRelation r){if(r==FREL_REBEL)return MercenarieConfig::Fiscal::RebelRatePercent;if(r==FREL_ALLIED)return MercenarieConfig::Fiscal::AlliedRatePercent;return o==FISCAL_UC?MercenarieConfig::Fiscal::UnitedCitiesRatePercent:MercenarieConfig::Fiscal::MercenaryGuildRatePercent;}
    bool contains(const std::string& id)const{for(size_t i=0;i<entries.size();++i)if(entries[i].fiscalId==id||(!entries[i].contractId.empty()&&entries[i].contractId==id))return true;return false;}
    static int taxAmount(int taxable,int pct){if(taxable<=0||pct<=0)return 0;long long n=(long long)taxable*pct;return (int)((n+50)/100);}
    FiscalEntry create(const std::string& contractId,const std::string& type,const std::string& rarity,const std::string& source,const std::string& origin,const std::string& destination,double hour,int gross,int bonuses,int tips,int taxable,bool ucTaxable,FiscalRelation ucRelation,FiscalRelation guildRelation){
        FiscalEntry e;e.contractId=contractId;e.fiscalId="TAX-"+toString(nextId++);e.type=type;e.rarity=rarity;e.source=source;e.origin=origin;e.destination=destination;e.createdHour=hour;e.grossIncome=std::max(0,gross);e.bonusIncome=std::max(0,bonuses);e.tips=std::max(0,tips);e.taxableIncome=std::max(0,taxable);e.ucTaxable=ucTaxable;e.ucAllied=ucRelation==FREL_ALLIED;e.guildAllied=guildRelation==FREL_ALLIED;e.ucRate=ucTaxable?rate(FISCAL_UC,ucRelation):0;e.guildRate=rate(FISCAL_MERCENARY_GUILD,guildRelation);e.ucTax=taxAmount(e.taxableIncome,e.ucRate);e.guildTax=taxAmount(e.taxableIncome,e.guildRate);e.ucStatus=e.ucTax?FENTRY_DUE:FENTRY_NONTAXABLE;e.guildStatus=e.guildTax?FENTRY_DUE:(guildRelation==FREL_REBEL?FENTRY_UNCOLLECTIBLE:FENTRY_NONTAXABLE);entries.push_back(e);organisations[0].debt+=e.ucTax;organisations[1].debt+=e.guildTax;return e;
    }
    void recordNoTax(const std::string& contractId,const std::string& type,const std::string& origin,const std::string& destination,double hour,FiscalEntryStatus status){if(contains(contractId))return;FiscalEntry e;e.contractId=contractId;e.fiscalId="TAX-"+toString(nextId++);e.type=type;e.origin=origin;e.destination=destination;e.createdHour=hour;e.ucStatus=e.guildStatus=status;entries.push_back(e);}
    bool importLegacyInvestments(){
    // Only certified investment debits predating cash coverage can be imported.
    // Fiscal rewards and unpaid taxes are NOT cash movement evidence.
    if(!cash.started||!cash.available||cash.legacyInvestmentsImported)return false;
    {
        bool observed=cash.observed;long long balance=cash.lastBalance;
        for(size_t i=0;i<entries.size();++i){const FiscalEntry& e=entries[i];
            if(e.type=="guild.investment"&&e.source=="guild.investment"&&e.grossIncome<0&&e.createdHour>=0&&e.createdHour<cash.startedHour-std::max(0.000001,0.51*std::pow(10.0,std::floor(std::log10(std::max(1.0,cash.startedHour)))-5)))
                cash.record(e.grossIncome,-1,e.createdHour,Finance::Investment,"finance.investment","legacy:"+e.fiscalId);
        }
        cash.legacyInvestmentsImported=true;cash.observed=observed;cash.lastBalance=balance;
    }
return true;
    }
    long long debt(FiscalOrganisation o)const{return organisations[(int)o].debt;}
    void recordInvestment(int cats,double hour){
        if(cats<=0||!(hour>=0&&hour<=1e9))throw std::runtime_error("invalid investment amount or hour");
        FiscalEntry e;e.fiscalId="INV-"+toString(nextId++);e.contractId=e.fiscalId;
        e.type="guild.investment";e.source="guild.investment";e.createdHour=hour;
        e.grossIncome=-cats;entries.push_back(e);
    }
    bool payAll(FiscalOrganisation o,long long available){FiscalOrganisationState& s=organisations[(int)o];if(s.debt<=0||available<s.debt)return false;long long remaining=s.debt;for(size_t i=0;i<entries.size()&&remaining>0;++i){int* tax=o==FISCAL_UC?&entries[i].ucTax:&entries[i].guildTax;int* paidPart=o==FISCAL_UC?&entries[i].ucPaid:&entries[i].guildPaid;FiscalEntryStatus* status=o==FISCAL_UC?&entries[i].ucStatus:&entries[i].guildStatus;int due=std::max(0,*tax-*paidPart);int applied=(int)std::min<long long>(due,remaining);*paidPart+=applied;remaining-=applied;if(*paidPart>=*tax&&*tax>0)*status=FENTRY_PAID;}s.paid+=s.debt;s.debt=0;s.state=FSTATE_NORMAL;s.extensionUsed=false;s.raidsWon=0;s.activeRaid=0;s.deadlineHour=0;s.nextCollectionHour=0;s.preArrivalNotified=false;return true;}
    void becomeRebel(FiscalOrganisation o){FiscalOrganisationState& s=organisations[(int)o];s.uncollectible+=s.debt;s.debt=0;s.rebel=true;s.state=FSTATE_REBEL;s.activeRaid=0;s.deadlineHour=0;s.nextCollectionHour=0;for(size_t i=0;i<entries.size();++i){FiscalEntryStatus& st=o==FISCAL_UC?entries[i].ucStatus:entries[i].guildStatus;if(st==FENTRY_DUE)st=FENTRY_UNCOLLECTIBLE;}}
    static std::string toString(unsigned long n){char b[32];sprintf_s(b,"%lu",n);return b;}
};
