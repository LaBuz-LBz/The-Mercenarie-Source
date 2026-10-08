#include "ImportRecovery.h"
// Native adapter. Included after financeChange, before views/settlements.
GuildPayroll::State guildPayroll;
bool payrollSyncing=false;
void payrollAutoResume();
void closePayrollWindowsForWorld();
void updatePayrollWindows();
void openPayrollFinances(MyGUI::Widget*);
std::string payrollNumber(GuildPayroll::Money n){std::ostringstream s;s<<n;return s.str();}
const char* payrollRankKey(int r){const char* keys[]={"payroll.rank.recruit","payroll.rank.mercenary","payroll.rank.veteran","payroll.rank.elite","payroll.rank.master"};return keys[std::max(0,std::min(4,r))];}
int payrollRank(Character* c){if(!c||!c->getStats())return 0;CharStats* s=c->getStats();return GuildPayroll::rank(s->getStat(STAT_STRENGTH,true),s->getStat(STAT_TOUGHNESS,true),s->getStat(STAT_DEXTERITY,true),s->getStat(STAT_MELEE_ATTACK,true),s->getStat(STAT_MARTIALARTS,true),s->getStat(STAT_MELEE_DEFENCE,true),s->getStat(STAT_DODGE,true));}
const char* payrollRankLabel(Character* c){return c&&c->isAnimal()?Loc::text("payroll.animal"):!c||!c->getStats()?Loc::text("common.inconnu"):Loc::text(payrollRankKey(payrollRank(c)));}
std::string payrollIdentity(Character* c){return c?c->getHandle().toString():"";}
Character* payrollCharacter(const std::string& id){hand h;h.fromString(id);return h.isNull()?0:h.getCharacter();}
GuildPayroll::Money payrollDebit(GuildPayroll::Money amount,const char* key){if(importedDomainsSuspended)return 0;int before=financeBalance();if(before<=0||amount<=0)return 0;int take=(int)std::min(amount,(GuildPayroll::Money)before);financeChange(-take,Finance::Wages,key);int after=financeBalance();return after>=0?std::max(0,before-after):0;}
void payrollObserveCharacter(Character* c){
    if(!c||c->isAnimal())return;
    if(!c->getRace()||!c->getStats()){
        std::map<std::string,GuildPayroll::Member>::iterator known=guildPayroll.members.find(payrollIdentity(c));
        if(c->isDead()&&known!=guildPayroll.members.end())guildPayroll.observe(known->first,c->getName(),known->second.rank,known->second.robot,true,known->second.observedLoss,currentGameHours);
        return;
    }
    unsigned int mask=0;for(int limb=0;limb<4;++limb){LimbState state=c->medical.getLimbState((RobotLimbs::Limb)limb);if(state==LIMB_STUMP||state==LIMB_REPLACED)mask|=1<<limb;}
    const bool departed=c->isDead()||(ou&&ou->player&&c->getFaction()!=ou->player->getFaction());
    std::vector<GuildPayroll::Money> claims=guildPayroll.observe(payrollIdentity(c),c->getName(),payrollRank(c),c->getRace()->robot,departed,mask,currentGameHours);
    for(size_t i=0;i<claims.size();++i){GuildPayroll::Money paid=payrollDebit(1000,"payroll.compensation_payment");guildPayroll.repay(paid,currentGameHours,"payroll.compensation_payment",claims[i]);}
}
void payrollSync(){
    if(payrollSyncing||!financeReady()||!ou||!ou->player||progressWriteBlocked)return;
    // Recover only after the world/finance guards are ready. This pass only
    // relinks the ledger; ordinary observation and debits start on a later tick.
    if(importedDomainsSuspended){payrollAutoResume();return;}payrollSyncing=true;
    // Scan only the actual player roster, not arbitrary faction NPCs. Delegated
    // characters remain in this native collection (MissionAbsencePrototype).
    guildPayroll.init(currentGameHours);
    std::set<std::string> observed;
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(!c||c->isAnimal())continue;observed.insert(payrollIdentity(c));payrollObserveCharacter(c);}
    for(std::map<std::string,GuildPayroll::Member>::iterator i=guildPayroll.members.begin();i!=guildPayroll.members.end();++i)if(!i->second.former&&!observed.count(i->first)){Character* c=payrollCharacter(i->first);if(c)payrollObserveCharacter(c);}
    guildPayroll.advance(currentGameHours);payrollSyncing=false;
    int cash=financeBalance();if(cash>=0&&guildPayroll.alert(cash)){Loc::Catalogue a;a["amount"]=payrollNumber(guildPayroll.estimate()+guildPayroll.debt()-cash);ou->showPlayerAMessage(Loc::format("payroll.warning",a),true);}
}
void payrollSetEnabled(bool enabled){if(importedDomainsSuspended)return;payrollSync();guildPayroll.enable(enabled,currentGameHours);}
void payrollRememberParticipants(const std::string& group,const std::vector<Character*>& people){if(importedDomainsSuspended)return;payrollSync();std::vector<std::string>& ids=guildPayroll.participants[group];ids.clear();for(size_t i=0;i<people.size();++i)if(people[i]&&!people[i]->isAnimal())ids.push_back(payrollIdentity(people[i]));}
std::string payrollReceiptText(const GuildPayroll::Receipt& r){Loc::Catalogue a;a["gross"]=payrollNumber(r.gross);a["paid"]=payrollNumber(r.repaid);a["net"]=payrollNumber(r.net);a["debt"]=payrollNumber(r.remaining);return Loc::format("payroll.receipt",a);}
void payrollContractReward(const std::string& id,int gross,const char* description,const std::string& group="",bool alreadyPaid=false){
    if(guildPayroll.receipt(id))return;
    payrollSync();int before=financeBalance();if(before<0)return;
    if(!alreadyPaid){financeChange(gross,Finance::Contract,description);if((long long)financeBalance()-before!=gross)return;}
    if(importedDomainsSuspended){std::vector<std::string> none;guildPayroll.recordReward(id,gross,0,currentGameHours,none);return;}
    GuildPayroll::Money withheld=payrollDebit(guildPayroll.withholding(gross),"payroll.contract_repayment");
    std::vector<std::string> people;std::map<std::string,std::vector<std::string> >::iterator p=guildPayroll.participants.find(group);if(!group.empty()&&p!=guildPayroll.participants.end())people=p->second;
    guildPayroll.recordReward(id,gross,withheld,currentGameHours,people);
    const GuildPayroll::Receipt* receipt=guildPayroll.receipt(id);if(receipt&&receipt->repaid>0&&ou)ou->showPlayerAMessage(payrollReceiptText(*receipt),true);
}

std::vector<ImportRecovery::Person> payrollRecoveryPeople(){std::vector<ImportRecovery::Person> people;if(!ou||!ou->player)return people;for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(!c||c->isAnimal()||c->isDead()||!c->getRace())continue;unsigned int loss=0;for(int l=0;l<4;++l){LimbState state=c->medical.getLimbState((RobotLimbs::Limb)l);if(state==LIMB_STUMP||state==LIMB_REPLACED)loss|=1<<l;}people.push_back(ImportRecovery::Person(payrollIdentity(c),c->getName(),c->getRace()->robot,loss));}return people;}
std::string payrollRecoverySignature(){std::map<std::string,std::string> links;if(!ImportRecovery::payrollPlan(guildPayroll,payrollRecoveryPeople(),links))return "";std::ostringstream s;s<<"ready\n";for(std::map<std::string,std::string>::const_iterator i=links.begin();i!=links.end();++i)s<<i->first.size()<<":"<<i->first<<i->second.size()<<":"<<i->second;return s.str();}
std::string payrollRecoveryConfirm;
void payrollAutoResume(){
    // Do not decide uniqueness while native roster entries are incomplete.
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(!c||(!c->isAnimal()&&!c->isDead()&&(!c->getRace()||!c->getStats())))return;}
    if(ImportRecovery::autoResumePayroll(guildPayroll,importedDomainsSuspended,payrollRecoveryPeople(),currentGameHours))payrollRecoveryConfirm.clear();
}
