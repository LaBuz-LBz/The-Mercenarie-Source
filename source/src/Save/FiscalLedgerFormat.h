#pragma once
#include "../FiscalSystem.h"
#include <istream>
#include <ostream>
#include <sstream>
#include <cstdlib>
#include "TextValidation.h"

// The historical fiscal text protocol. No filesystem, engine, UI or live globals.
// Keep field order, default stream locale/precision and permissive legacy parsing.
namespace FiscalLedgerFormat {
inline std::string cleanField(std::string value)
{
    std::replace(value.begin(), value.end(), '|', '/');
    std::replace(value.begin(), value.end(), '\n', ' ');
    return value;
}

inline void writeUnchecked(std::ostream& out, const FiscalLedger& ledger)
{
    if(ledger.cash.started||!ledger.cash.available)Finance::write(out,ledger.cash);
    out<<"@version|"<<ledger.schemaVersion<<'|'<<ledger.nextId<<'\n';
    for(int i=0;i<2;++i) {
        const FiscalOrganisationState& s=ledger.organisations[i];
        out<<"@org|"<<i<<'|'<<s.state<<'|'<<s.debt<<'|'<<s.paid<<'|'<<s.uncollectible<<'|'<<s.newTaxesAtLastVisit<<'|'<<s.nextCollectionHour<<'|'<<s.deadlineHour<<'|'<<s.firstIntroductionDone<<'|'<<s.extensionUsed<<'|'<<s.preArrivalNotified<<'|'<<s.raidsWon<<'|'<<s.activeRaid<<'|'<<s.rebel<<'|'<<cleanField(s.targetHouseId)<<'\n';
    }
    for(size_t i=0;i<ledger.entries.size();++i) {
        const FiscalEntry& e=ledger.entries[i];
        out<<"@entry|"<<cleanField(e.fiscalId)<<'|'<<cleanField(e.contractId)<<'|'<<cleanField(e.type)<<'|'<<cleanField(e.rarity)<<'|'<<cleanField(e.source)<<'|'<<cleanField(e.origin)<<'|'<<cleanField(e.destination)<<'|'<<e.createdHour<<'|'<<e.grossIncome<<'|'<<e.bonusIncome<<'|'<<e.tips<<'|'<<e.taxableIncome<<'|'<<e.ucTaxable<<'|'<<e.ucAllied<<'|'<<e.guildAllied<<'|'<<e.ucRate<<'|'<<e.guildRate<<'|'<<e.ucTax<<'|'<<e.guildTax<<'|'<<e.ucPaid<<'|'<<e.guildPaid<<'|'<<e.ucStatus<<'|'<<e.guildStatus<<'\n';
    }
}

inline void read(std::istream& in, FiscalLedger& ledger)
{
    std::string bytes((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());SaveText::fiscal(bytes);std::istringstream checked(bytes);
    FiscalLedger loaded;
    std::string line;
    while(std::getline(checked,line)) {
        Finance::readLine(line,loaded.cash);
        if(!loaded.cash.available)throw std::runtime_error("invalid finance journal");
        std::stringstream ss(line);
        std::string f[25];
        for(int i=0;i<25;++i)std::getline(ss,f[i],'|');
        if(f[0]=="@version") {
            loaded.schemaVersion=atoi(f[1].c_str());
            loaded.nextId=std::max<unsigned long>(1,strtoul(f[2].c_str(),0,10));
        }
        else if(f[0]=="@org") {
            int n=atoi(f[1].c_str());
            if(n<0||n>1)continue;
            FiscalOrganisationState& s=loaded.organisations[n];
            s.state=(FiscalState)atoi(f[2].c_str());
            s.debt=_atoi64(f[3].c_str());
            s.paid=_atoi64(f[4].c_str());
            s.uncollectible=_atoi64(f[5].c_str());
            s.newTaxesAtLastVisit=_atoi64(f[6].c_str());
            s.nextCollectionHour=atof(f[7].c_str());
            s.deadlineHour=atof(f[8].c_str());
            s.firstIntroductionDone=atoi(f[9].c_str())!=0;
            s.extensionUsed=atoi(f[10].c_str())!=0;
            s.preArrivalNotified=atoi(f[11].c_str())!=0;
            s.raidsWon=atoi(f[12].c_str());
            if(SaveText::fields(line).size()==16) {
                s.activeRaid=atoi(f[13].c_str());
                s.rebel=atoi(f[14].c_str())!=0;
                s.targetHouseId=f[15];
            }
            else {
                s.activeRaid=0;
                s.rebel=atoi(f[13].c_str())!=0;
                s.targetHouseId=f[14];
            }
        }
        else if(f[0]=="@entry") {
            FiscalEntry e;
            e.fiscalId=f[1];
            e.contractId=f[2];
            e.type=f[3];
            e.rarity=f[4];
            e.source=f[5];
            e.origin=f[6];
            e.destination=f[7];
            e.createdHour=atof(f[8].c_str());
            e.grossIncome=atoi(f[9].c_str());
            e.bonusIncome=atoi(f[10].c_str());
            e.tips=atoi(f[11].c_str());
            e.taxableIncome=atoi(f[12].c_str());
            e.ucTaxable=atoi(f[13].c_str())!=0;
            e.ucAllied=atoi(f[14].c_str())!=0;
            e.guildAllied=atoi(f[15].c_str())!=0;
            e.ucRate=atoi(f[16].c_str());
            e.guildRate=atoi(f[17].c_str());
            e.ucTax=atoi(f[18].c_str());
            e.guildTax=atoi(f[19].c_str());
            e.ucPaid=atoi(f[20].c_str());
            e.guildPaid=atoi(f[21].c_str());
            e.ucStatus=(FiscalEntryStatus)atoi(f[22].c_str());
            e.guildStatus=(FiscalEntryStatus)atoi(f[23].c_str());
            loaded.entries.push_back(e);
        }
    }
    loaded.importLegacyInvestments();loaded.cash.observed=false;
    ledger=loaded;
}
inline void write(std::ostream& out,const FiscalLedger& ledger){std::ostringstream checked;checked.imbue(std::locale::classic());checked.precision(17);writeUnchecked(checked,ledger);SaveText::fiscal(checked.str());out<<checked.str();}
}
