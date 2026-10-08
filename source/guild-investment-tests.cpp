#include "tests/LocalizedCaptionStub.h"
#include <cassert>
#include <iostream>
#include <map>
#include "GuildProgression.h"
#include "GuildLevelUnlocks.h"
#include "src/Save/FiscalLedgerFormat.h"
// Exercise the actual runtime callback with a fake wallet, UI and storage.
namespace MyGUI {struct Widget {bool visible,enabled;std::string caption;Widget():visible(true),enabled(true){}bool getVisible(){return visible;}void setVisible(bool v){visible=v;}void setEnabled(bool v){enabled=v;}void setCaption(const std::string& s){caption=s;}};}
MyGUI::Widget widgets[12];
MyGUI::Widget *guildInvestmentOverlay=&widgets[0],*guildWindow=&widgets[1],*investmentAmount=&widgets[2],*investmentReward=&widgets[3],*investmentBalance=&widgets[4],*investmentConfirm=&widgets[5],*investmentMinus=&widgets[6],*investmentPlus=&widgets[7],*investmentQuick[]={&widgets[8],&widgets[9],&widgets[10],&widgets[11]};
bool investmentArmed=true,investmentBusy=false,missionWorldChanging=false,missionRestorePending=false,progressWriteBlocked=false,progressLoadFault=false;
int investmentCats=10000,guildPoints=0,guildPrestige=0,appliedGuildUnlockLevel=0;
double currentGameHours=12;
std::string investmentSlot="save",activeMercenarieSaveSlot="save",reputationFile="progress",fiscalFile="fiscal";
FiscalLedger fiscalLedger;std::vector<std::string> contractHistory;
struct Wallet {int money,debits;bool fail;Wallet():money(100000),debits(0),fail(false){}int getMoney(){return money;}void takeMoney(int n){++debits;if(!fail)money-=n;}void addMoney(int n){money+=n;}} wallet;
struct Participant {Wallet* factionOwnerships;Participant():factionOwnerships(&wallet){}} participant;
struct Player {Participant* participant;Player():participant(&::participant){}} player;
struct World {Player* player;World():player(&::player){}void showPlayerAMessage(const char*,bool){}} world;
World* ou=&world;
std::map<std::string,std::string> files;int failWrite=0,writeCount=0,notifiedLevel=0,throwWrite=0;bool throwSerialize=false;
bool atomicMissionFile(const std::string& path,const std::string& bytes){++writeCount;if(writeCount==throwWrite)throw std::runtime_error("injected write exception");if(writeCount==failWrite)return false;files[path]=bytes;return true;}
std::string serializeReputations(){if(throwSerialize)throw std::runtime_error("injected serialization exception");std::ostringstream s;s<<guildPoints<<' '<<guildPrestige;return s.str();}
std::string registerNumber(long long n){std::ostringstream s;s<<n;return s.str();}
int guildLevel(){return GuildProgression::level(guildPoints);}
void closeGuildInvestment(MyGUI::Widget*){investmentArmed=false;guildInvestmentOverlay->setVisible(false);}
void updateGuildResearchAccess(){}void updateGuildMenu(){}void updateFinancesPage(){}
void refreshGuildInvestmentPreview(int,int,int){}
void showGuildLevelUpWindow(int,int n){notifiedLevel=n;}
void financeObserve(){fiscalLedger.cash.observe(wallet.money,currentGameHours);}
std::string readMissionFile(const std::string& p){return files[p];}
#include "GuildInvestmentPersistence.h"
#include "GuildInvestmentRuntime.h"
void reset(){wallet=Wallet();guildPoints=guildPrestige=0;investmentCats=10000;investmentArmed=true;investmentBusy=false;guildInvestmentOverlay->setVisible(true);guildWindow->setVisible(true);fiscalLedger=FiscalLedger();contractHistory.clear();files.clear();writeCount=failWrite=notifiedLevel=0;progressWriteBlocked=false;missionWorldChanging=missionRestorePending=false;investmentSlot=activeMercenarieSaveSlot="save";}
int main(){
    const int amounts[]={10000,20000,30000,50000,100000};const int xp[]={120,240,360,600,1200};
    for(int i=0;i<5;++i)assert(GuildProgression::investmentXp(amounts[i])==xp[i]);
    assert(!GuildProgression::canInvest(10000,9999));assert(!GuildProgression::canInvest(-10000,100000));assert(!GuildProgression::canInvest(15000,100000));assert(!GuildProgression::canInvest(0,100000));
    assert(!GuildProgression::bountyUnlocked(0));assert(GuildProgression::bountyUnlocked(1));assert(GuildProgression::bountyUnlocked(10));
    bool bounty=false;std::vector<GuildLevelUI::Unlock> unlocked=GuildLevelUI::unlocks(1);for(size_t i=0;i<unlocked.size();++i)if(std::string(unlocked[i].en)==Loc::text("ui.bounty_hunt"))bounty=true;assert(bounty);
    reset();refreshGuildInvestment();assert(investmentConfirm->enabled);confirmGuildInvestment(0);confirmGuildInvestment(0);
    assert(wallet.money==90000&&wallet.debits==1&&guildPoints==120&&fiscalLedger.entries.size()==1&&contractHistory.size()==1);
    assert(fiscalLedger.entries[0].grossIncome==-10000&&fiscalLedger.debt(FISCAL_UC)==0&&fiscalLedger.debt(FISCAL_MERCENARY_GUILD)==0);
    assert(fiscalLedger.cash.entries.size()==1&&fiscalLedger.cash.entries[0].amount==-10000&&fiscalLedger.cash.entries[0].category==Finance::Investment);
    std::istringstream saved(files["fiscal"]);FiscalLedger loaded;FiscalLedgerFormat::read(saved,loaded);assert(loaded.entries.size()==1&&loaded.entries[0].grossIncome==-10000&&loaded.nextId==fiscalLedger.nextId);
    guildPoints=0;std::istringstream progress(files["progress"]);progress>>guildPoints>>guildPrestige;confirmGuildInvestment(0);assert(guildPoints==120&&wallet.debits==1);
    reset();closeGuildInvestment(0);confirmGuildInvestment(0);assert(wallet.debits==0&&guildPoints==0);
    reset();wallet.money=9999;refreshGuildInvestment();assert(!investmentConfirm->enabled&&!investmentPlus->enabled);confirmGuildInvestment(0);assert(wallet.debits==0);
    reset();wallet.money=0;confirmGuildInvestment(0);assert(wallet.money==0&&guildPoints==0);
    reset();investmentCats=20000;wallet.money=10000;confirmGuildInvestment(0);assert(wallet.debits==0);
    reset();investmentCats=200000;wallet.money=200000;guildPoints=290;confirmGuildInvestment(0);assert(guildPoints==2690&&guildLevel()==4&&notifiedLevel==4&&wallet.money==0);
    reset();guildPoints=16090;confirmGuildInvestment(0);assert(guildPoints==16100&&guildPrestige==110);
    reset();wallet.fail=true;confirmGuildInvestment(0);assert(guildPoints==0&&wallet.money==100000&&fiscalLedger.entries.empty());
    for(int i=1;i<=2;++i){reset();failWrite=i;confirmGuildInvestment(0);assert(wallet.money==100000&&guildPoints==0&&fiscalLedger.entries.empty()&&contractHistory.empty()&&fiscalLedger.cash.entries.empty());}
    reset();investmentSlot="previous";confirmGuildInvestment(0);assert(wallet.debits==0);
    reset();missionRestorePending=true;confirmGuildInvestment(0);assert(wallet.debits==0);
    reset();progressWriteBlocked=true;confirmGuildInvestment(0);assert(wallet.debits==0);
    // All failures preserve exact existing file bytes as well as business state.
    for(int fault=0;fault<5;++fault){
        reset();files["fiscal"]="@version|1|1\n";files["progress"]="0 0";const std::map<std::string,std::string> before=files;
        if(fault<2)throwWrite=fault+1;else if(fault<4)failWrite=fault-1;else throwSerialize=true;
        confirmGuildInvestment(0);assert(wallet.money==100000&&guildPoints==0&&guildPrestige==0&&fiscalLedger.entries.empty()&&contractHistory.empty()&&!investmentBusy&&files==before);
        confirmGuildInvestment(0);assert(wallet.money==100000);throwWrite=0;throwSerialize=false;
    }
    reset();currentGameHours=-1;confirmGuildInvestment(0);assert(wallet.debits==0&&guildPoints==0&&files.empty()&&!investmentBusy);currentGameHours=12;
    reset();for(int i=0;i<3;++i){investmentArmed=true;guildInvestmentOverlay->setVisible(true);confirmGuildInvestment(0);}
    assert(wallet.money==70000&&guildPoints==360&&fiscalLedger.entries.size()==3&&!investmentBusy);
    fiscalLedger.create("reward","escort","common","mission","A","B",13,1000,0,0,1000,true,FREL_NORMAL,FREL_NORMAL);
    // Historical writer supplies the exact old wire shape; no source is changed.
    std::ostringstream historical;FiscalLedgerFormat::writeUnchecked(historical,fiscalLedger);const std::string original=historical.str();
    FiscalLedger legacy;std::istringstream legacyIn(original);FiscalLedgerFormat::read(legacyIn,legacy);assert(legacy.entries.size()==4&&legacy.entries[0].grossIncome==-10000&&historical.str()==original);
    std::ostringstream modern;FiscalLedgerFormat::write(modern,legacy);FiscalLedger again;std::istringstream modernIn(modern.str());FiscalLedgerFormat::read(modernIn,again);assert(again.entries.size()==4&&again.debt(FISCAL_UC)==legacy.debt(FISCAL_UC));
    legacy.cash=Finance::Journal();legacy.cash.baseline(70000,24);assert(legacy.importLegacyInvestments());assert(legacy.cash.entries.size()==3);assert(!legacy.importLegacyInvestments()&&legacy.cash.entries.size()==3);
    for(int fault=0;fault<4;++fault){FiscalLedger bad=again;if(fault==0)bad.entries.back().grossIncome=-1;if(fault==1)bad.entries[0].bonusIncome=-1;if(fault==2)bad.entries[0].ucTax=1;if(fault==3)bad.entries[0].source="reward";bool rejected=false;try{std::ostringstream out;FiscalLedgerFormat::write(out,bad);}catch(const std::exception&){rejected=true;}assert(rejected);}
    Loc::configure("Localization","fr");assert(std::string(Loc::text("guild.investment"))=="Investissement dans la Guilde");
    Loc::select("en");assert(std::string(Loc::text("guild.investment"))=="Guild investment");Loc::select("zz");assert(std::string(Loc::text("guild.investment"))=="Guild investment");
    std::cout<<"Guild investment runtime, rollback, persistence, unlock and FR/EN/fallback: PASS\n";
}
