// Real production persistence and confirmation handlers with native/UI test doubles.
#include <cassert>
#include <iostream>
#include <map>
#include "CleanupState.h"
#include "ContractReroll.h"
#include "ContractDestinationRules.h"
#include "ContractRouteVisual.h"
#include "src/Save/TextValidation.h"
bool savePreparing=false,progressWriteBlocked=false,progressLoadFault=false;
void reportPersistenceException(const char*,const std::exception&,const std::string&){progressLoadFault=true;}
std::string cleanBoardField(std::string);
#include "src/EscortEconomy.h"
#include "GuildVisitorOfferRules.h"
#include "v5/BountySave.h"
struct Faction;
struct Relations {
    float relation;int writes;Relations():relation(0),writes(0){}
    bool isEnemy(Faction*){return relation<=-30;}
    void declareWar(Faction*){relation=std::min(relation,-35.0f);++writes;}
    void setNoLongerEnemies(Faction*){relation=std::max(relation,-25.0f);++writes;}
};
struct Faction {Relations* relations;std::string name;};
struct Player {Faction* participant;};
struct Manager {Faction* faction;Faction* getFactionByStringID(const char*){return faction;}};
struct Engine {Player* player;Manager* factionMgr;} engine,*ou=&engine;
struct TownBase {Faction* faction;Faction* getFaction(){return faction;}};
#include "FactionWarRuntime.h"
TownBase origin;
TownBase* contractOriginTown=&origin;
struct Character {TownBase* getCurrentTownLocation(){return &origin;}} giver;
struct hand {Character* character;hand(Character* c=0):character(c){}bool isNull()const{return !character;}Character* getCharacter()const{return character;}std::string toString()const{return "officer";}};
namespace MercenarieV5 {
bool bountyIssuer(Character*,IssuerFaction& faction){faction=HolyNation;return true;}
ActorIdentity bountyIdentity(const hand&){ActorIdentity a;a.container=1;return a;}
}
#include "v9-board-types.generated.h"
// This persistence fixture has no native region map. Migration is exercised
// against the real resolver and saved offers in region-risk-tests.cpp.
bool refreshOfferRegions(BoardOffer&){return false;}
struct MissionBookPoolEntry {BoardOffer offer;int sourceIndex;bool security;std::string id,boardKey;hand issuer;};
std::vector<MissionBookPoolEntry> missionBookPool;
std::vector<size_t> missionBookPageEntries;
MissionBookPoolEntry rerollPendingEntry;
bool rerollPendingClassic=false,missionBookDelegationContext=false;
double rerollPendingDeadline=0;float developerTimeOffsetHours=0;
int selectedOffer=0,classicRefreshes=0,confirmations=0;
std::string currentBoardKey;
hand contractBarmanHandle(&giver);
namespace MyGUI {struct Widget {std::map<std::string,std::string> data;std::string getUserString(const char* key){return data[key];}};}
void (*v9ConfirmationCallback)(int)=0;
void v9Confirm(const std::string&,const std::string&,const char*,void(*callback)(int)){++confirmations;v9ConfirmationCallback=callback;}
void updateContractsBoard(){++classicRefreshes;}
struct Window {bool visible;bool getVisible(){return visible;}} window,*contractsWindow=&window;
struct Options {bool developerMode;} clientOptions;
ContractReroll::Charges guildRerolls;
double currentGameHours=0;
std::string contractBoardsFile="saveA",message,rerollPendingId,missionBookSelectedId,rerollPreservedDestination,destinationNameStorage;
const char* destinationTown="",*destinationName="";float selectedDistance=0;
bool contractBoardsLoaded=false,missionBookPoolReady=true,contractAcceptArmed=false,delegationFinalArmed=false,delegationConfirming=false;
unsigned int v9WorldEpoch=1,rerollPendingEpoch=1,warPendingEpoch=1;
int warPendingIndex=0,visitorOfferProfile=0;bool warPendingPeace=false,visitorOfferUrgent=false,visitorOfferVip=false,visitorOfferExceptional=false;
std::vector<int> recentGuildVisitorOfferTypes;
MercenarieV5::BountyWorldState bountyWorld;
std::map<std::string,std::string> files;
bool atomicMissionFile(const std::string& path,const std::string& bytes){files[path]=bytes;return true;}
std::string readMissionFile(const std::string& path){return files[path];}
void ErrorLog(const char*){}
void v9Message(const char* key){message=key;}
bool missionBookSameTown(Character*){return true;}
bool missionBookSelectionValid(){return true;}
void selectMissionBookEntry(size_t){}
void refreshMissionBookContractHub(){}
void openFactionDiplomacy(void*){}
bool isGuildVisitor(Character*){return false;}
void selectGuildVisitorOffer(Character*){visitorOfferProfile=2;}
bool canGenerate=true;
TownBase destination;
void generateContractOffers(Character*,CityContractBoard& board,int slot,int count,bool reroll){
    assert(count==1&&reroll);if(!canGenerate||destinationAtWar(&destination))return;
    BoardOffer& offer=board.offers[slot];offer.available=true;offer.estimatedPay=10000;offer.townId="destination";offer.missionType=4;offer.routeRegions="V6EST:1:ROAD";
}
struct BountyRandom {unsigned int operator()(){return 4;}};
bool chooseBountyArea(MercenarieV5::BountyOffer& offer,Character*){offer.areaId="enemy-city";offer.targetName="Target";return true;}
// Forward declaration because the real handler invokes the real loader.
void loadContractBoards();
void resetV9PendingActions(){++v9WorldEpoch;rerollPendingId.clear();rerollPendingClassic=false;v9ConfirmationCallback=0;}
#include "v9-runtime.generated.h"
void arm(){rerollPendingId="offer";rerollPendingEpoch=v9WorldEpoch;message.clear();}
void setup(){
    window.visible=true;clientOptions.developerMode=true;currentGameHours=10;
    guildRerolls=ContractReroll::Charges();contractBoardsLoaded=true;savedContractBoards.clear();
    rerollPendingClassic=false;v9ConfirmationCallback=0;missionBookPoolReady=true;currentBoardKey="town#TAVERN";
    CityContractBoard& board=savedContractBoards["town#TAVERN"];board.expiresAt=58;
    for(int i=0;i<6;++i){board.offers[i].available=true;board.offers[i].townId="old";board.offers[i].estimatedPay=20000;}
    MissionBookPoolEntry e;e.id="offer";e.boardKey="town#TAVERN";e.sourceIndex=3;e.security=false;e.issuer=hand(&giver);e.offer=board.offers[3];missionBookPool.clear();missionBookPool.push_back(e);
}
int main(){
    Relations native,playerNative;Faction faction={&native,"Holy Nation"},playerFaction={&playerNative,"Player"};Player player={&playerFaction};Manager manager={&faction};engine.player=&player;engine.factionMgr=&manager;destination.faction=&faction;
    setup();arm();confirmReroll(0);assert(guildRerolls.remaining==2);
    arm();confirmReroll(1);assert(guildRerolls.remaining==1&&guildRerolls.ends==34);assert(savedContractBoards["town#TAVERN"].offers[3].estimatedPay==7500);
    for(int i=0;i<6;++i)if(i!=3)assert(savedContractBoards["town#TAVERN"].offers[i].estimatedPay==20000);
    currentGameHours=15;arm();confirmReroll(1);assert(guildRerolls.remaining==0&&guildRerolls.ends==34);assert(savedContractBoards["town#TAVERN"].offers[3].estimatedPay==7500);
    arm();confirmReroll(1);assert(guildRerolls.remaining==0&&message=="v9.reroll.empty");
    // Exact production writer/loader, switched between two independent saves.
    saveContractBoards();std::string saveA=files["saveA"];savedContractBoards.clear();guildRerolls=ContractReroll::Charges();contractBoardsLoaded=false;loadContractBoards();assert(guildRerolls.remaining==0&&guildRerolls.ends==34);assert(ContractReroll::marked(savedContractBoards["town#TAVERN"].offers[3].routeRegions));assert(savedContractBoards["town#TAVERN"].offers[3].estimatedPay==7500);
    contractBoardsFile="V8";files["V8"]="@version|2\n@board|old|100\n";savedContractBoards.clear();contractBoardsLoaded=false;loadContractBoards();assert(guildRerolls.remaining==2&&guildRerolls.ends==-1);
    contractBoardsFile="saveA";savedContractBoards.clear();contractBoardsLoaded=false;loadContractBoards();assert(guildRerolls.remaining==0&&guildRerolls.ends==34);
    setup();canGenerate=false;arm();confirmReroll(1);assert(guildRerolls.remaining==2&&savedContractBoards["town#TAVERN"].offers[3].estimatedPay==20000);canGenerate=true;
    arm();++v9WorldEpoch;confirmReroll(1);assert(guildRerolls.remaining==2); // stale confirmation from another world
    // Integrated peace -> war -> reroll / existing offers -> bounty -> peace.
    assert(!destinationAtWar(&destination));native.relation=-10;assert(!destinationAtWar(&destination));
    warPendingEpoch=v9WorldEpoch;warPendingIndex=0;warPendingPeace=false;confirmDiplomacy(0);assert(native.writes==0);
    warPendingIndex=0;clientOptions.developerMode=false;confirmDiplomacy(1);assert(native.writes==0);
    clientOptions.developerMode=true;warPendingIndex=0;confirmDiplomacy(1);assert(destinationAtWar(&destination)&&native.relation==-35&&playerNative.relation==-35);
    arm();confirmReroll(1);assert(guildRerolls.remaining==2&&savedContractBoards["town#TAVERN"].offers[3].townId=="old");
    MissionBookPoolEntry& e=missionBookPool[0];e.security=true;e.sourceIndex=0;
    MercenarieV5::BountyOffer bounty;bounty.id="offer";bountyWorld.boards["officer"].offers.push_back(bounty);bountyWorld.boards["officer"].nextRefreshHour=58;
    arm();confirmReroll(1);assert(guildRerolls.remaining==1&&bountyWorld.boards["officer"].offers[0].areaId=="enemy-city");assert(ContractReroll::bountyMarked(bountyWorld.boards["officer"].offers[0].id));
    warPendingIndex=0;warPendingPeace=true;confirmDiplomacy(1);assert(!destinationAtWar(&destination)&&native.relation==-25&&playerNative.relation==-25);
    e.security=false;e.sourceIndex=3;arm();confirmReroll(1);assert(savedContractBoards["town#TAVERN"].offers[3].townId=="destination"&&guildRerolls.remaining==0);
    // Actual classic-card click and the same production confirmation transaction.
    setup();missionBookPoolReady=false;MyGUI::Widget clicked;clicked.data["offerSlot"]="4";
    std::string before;saveContractBoards();before=files[contractBoardsFile];
    rerollClassicRow(&clicked);assert(rerollPendingClassic&&v9ConfirmationCallback==confirmReroll&&rerollPendingEntry.sourceIndex==4);
    v9ConfirmationCallback=0;confirmReroll(0);saveContractBoards();assert(files[contractBoardsFile]==before);
    rerollClassicRow(&clicked);v9ConfirmationCallback=0;confirmReroll(1);
    assert(classicRefreshes==1&&guildRerolls.remaining==1&&boardOffers[4].estimatedPay==7500);
    for(int i=0;i<6;++i)if(i!=4)assert(savedContractBoards[currentBoardKey].offers[i].estimatedPay==20000);
    rerollClassicRow(&clicked);v9ConfirmationCallback=0;confirmReroll(1);assert(guildRerolls.remaining==0&&boardOffers[4].estimatedPay==7500&&guildRerolls.ends==34);
    int count=confirmations;rerollClassicRow(&clicked);assert(confirmations==count);
    setup();savedContractBoards[currentBoardKey].offers[4].available=false;count=confirmations;rerollClassicRow(&clicked);assert(confirmations==count);
    setup();rerollClassicRow(&clicked);savedContractBoards[currentBoardKey].expiresAt+=1;v9ConfirmationCallback=0;confirmReroll(1);assert(guildRerolls.remaining==2);
    // A -> menu/new B -> A, executing the exact reset and persistence functions.
    setup();contractBoardsFile="isolationA";guildRerolls.consume(10);developerTimeOffsetHours=48;saveContractBoards();
    const std::string originalA=files[contractBoardsFile];
    resetContractBoardWorld();assert(savedContractBoards.empty()&&!contractBoardsLoaded&&currentBoardKey.empty()&&guildRerolls.remaining==2&&developerTimeOffsetHours==0&&currentGameHours==0);
    for(int i=0;i<6;++i)assert(!boardOffers[i].available&&!ContractReroll::marked(boardOffers[i].routeRegions));
    contractBoardsFile="newB";loadContractBoards();assert(savedContractBoards.empty()&&guildRerolls.remaining==2);
    currentGameHours=100;savedContractBoards["B-only"].expiresAt=currentGameHours+48;savedContractBoards["B-only"].offers[0].available=true;saveContractBoards();
    resetContractBoardWorld();contractBoardsFile="isolationA";loadContractBoards();assert(savedContractBoards.size()==1&&savedContractBoards.count("B-only")==0&&savedContractBoards["town#TAVERN"].expiresAt==58&&guildRerolls.remaining==1&&guildRerolls.ends==34);
    assert(files["isolationA"]==originalA);resetContractBoardWorld();contractBoardsFile="newB";loadContractBoards();assert(savedContractBoards.size()==1&&savedContractBoards["B-only"].expiresAt==148&&guildRerolls.remaining==2);

    // Legacy invalid science offers expire cleanly; valid science and road
    // offers retain their frozen price and geometry through real persistence.
    for(int cycle=0;cycle<300;++cycle){
        setup();contractBoardsFile="science-cycles";
        BoardOffer& scientific=savedContractBoards[currentBoardKey].offers[2];
        scientific.missionType=MCT_SCIENCE;scientific.townId="51398-rebirth.mod";
        scientific.routeRegions="V6EST:1.25:ROAD;PATH=0,0,0;0,0,10000;10000,0,10000;10000,0,0;";
        ContractReroll::mark(scientific.routeRegions,20000,cycle+1);scientific.estimatedPay=15000;
        saveContractBoards();contractBoardsLoaded=false;loadContractBoards();
        const BoardOffer& loaded=savedContractBoards[currentBoardKey].offers[2];
        assert(loaded.available&&loaded.estimatedPay==15000&&ContractRouteVisual::decode(loaded.routeRegions).size()==4);
        assert(savedContractBoards[currentBoardKey].expiresAt==58);
    }
    for(int invalid=0;invalid<3;++invalid){
        setup();contractBoardsFile="old-invalid-science";
        BoardOffer& o=savedContractBoards[currentBoardKey].offers[2];o.missionType=MCT_SCIENCE;
        o.townId=invalid==0?"Mastoc":invalid==1?"49367-rebirth.mod":"";
        saveContractBoards();contractBoardsLoaded=false;loadContractBoards();
        assert(!savedContractBoards[currentBoardKey].offers[2].available&&savedContractBoards[currentBoardKey].expiresAt==0);
        assert(!progressLoadFault&&!progressWriteBlocked);
        saveContractBoards();contractBoardsLoaded=false;loadContractBoards();
        assert(!savedContractBoards[currentBoardKey].offers[2].available);
    }
    std::cout<<"PASS 300 scientific board save/load cycles with reroll geometry; invalid city/excluded/empty targets expire without save faults\n";
    std::cout<<"V9 production save/load and handlers: isolated slots/saves, failures, stale confirmation, native adapter guards, war/reroll/bounty/peace scenario PASS (engine doubles)\n";
}
