#pragma once
// Included after the complete V9 persistence/runtime adapters.
#include "CleanupResearchState.h"
namespace {
void cleanupNativeOwnedResearch(){
    if(!MercenarieCleanup::disabled||MercenarieCleanup::researchCleaned||!ou||!ou->player||!ou->player->technology)return;
    const MercenarieCleanup::ResearchCounts removed=MercenarieCleanup::clearResearch(*ou->player->technology);
    MercenarieCleanup::researchCleaned=true;
    if(removed.total()){std::ostringstream log;log<<"[CLEANUP] native research finished="<<removed.finished<<" enabled="<<removed.enabled<<" known="<<removed.known<<" queued="<<removed.queued<<" paid="<<removed.paid;DebugLog(log.str());}
}
void cleanupDisabledFrame(){
    if(missionWorldChanging)return;
    cleanupNativeOwnedResearch();
    if(!key||!key->keyboard)return;
    const bool guild=key->keyboard->isKeyDown((OIS::KeyCode)clientOptions.bindings[ClientOptions::OpenGuildManagement]);
    if(guild&&!jWasDown&&ou)ou->showPlayerAMessage(Loc::text("cleanup.disabled"),true);jWasDown=guild;
}
void clearMercenarieForCleanup(){
    MercenarieCleanup::researchCleaned=false;
    discardMissionWorldReferences();clearGuildProgressForImport();artisanReset();
    bountyWorld=MercenarieV5::BountyWorldState();
    delegatedMission.clear();delegatedMissions.clear();mailContracts.clear();
    autopilotTrips.clear();contractSeeds.clear();guildHouseOriginalNames.clear();
    currentGuildHouseKey.clear();currentGuildHouseName.clear();
    snapshotFiscal.clear();snapshotReputation.clear();snapshotBoards.clear();snapshotEstate.clear();
    restoredFinalVisible=false;restoredFinalCaption.clear();
    missionRestorePending=false;missionRestoreBytes.clear();
    developerTimeOffsetHours=0;developerFurnitureOverride=-1;
    resetMailItemIdentities();
    reputationFile.clear();fiscalFile.clear();contractBoardsFile.clear();
    activeMercenarieSaveSlot="__disabled__";
}
void logCleanupInventory(const std::string& directory){
    DebugLog("[CLEANUP] begin");
    try{
        const std::string bytes=readMissionFile(directory+"/Mission.v4");
        if(bytes.empty()){DebugLog("[CLEANUP] legacy/empty payload; all owned companions excluded from import");return;}
        boost::shared_ptr<MissionValidationState> s=validateMissionPayload(openMissionArchive(bytes));
        std::ostringstream log;log<<"[CLEANUP] selected contract removed="<<(s->missionActive||s->missionPending?1:0)
            <<" mail records removed="<<s->mailContracts.size()<<" delegated records removed="<<s->delegatedMissions.size()
            <<" artisan orders removed="<<s->artisanLedger.orders.size();DebugLog(log.str());
        unsigned int offices=0;std::istringstream in(s->snapshotReputation);std::string line;
        while(std::getline(in,line))if(line.find("@house|")==0)++offices;
        std::ostringstream office;office<<"[CLEANUP] offices removed="<<offices<<"; all quest contexts, bounty, progression, finance, payroll, estate, recovery and actor references excluded";DebugLog(office.str());
    }catch(const std::exception& e){DebugLog(std::string("[CLEANUP] inventory unavailable (corrupt mod payload ignored): ")+e.what());}
}
struct CleanupImportConfirmation {
    SaveManager* manager;ImportGameMenu* menu;std::string root,name;int flags;unsigned int epoch;bool remove,approved;
    CleanupImportConfirmation():manager(0),menu(0),flags(0),epoch(0),remove(false),approved(false){}
} cleanupConfirmation;
void answerCleanupImport(int answer);
bool confirmCleanupImport(SaveManager* manager,const std::string& root,const std::string& name,int flags,bool remove){
    if(cleanupConfirmation.approved){
        const bool valid=cleanupConfirmation.manager==manager&&cleanupConfirmation.root==root&&cleanupConfirmation.name==name&&cleanupConfirmation.flags==flags&&cleanupConfirmation.remove==remove&&cleanupConfirmation.epoch==v9WorldEpoch;
        cleanupConfirmation.approved=false;return valid;
    }
    cleanupConfirmation=CleanupImportConfirmation();cleanupConfirmation.manager=manager;
    cleanupConfirmation.menu=manager?manager->importMenu:0;cleanupConfirmation.root=root;cleanupConfirmation.name=name;
    cleanupConfirmation.flags=flags;cleanupConfirmation.remove=remove;cleanupConfirmation.epoch=v9WorldEpoch;
    v9Confirm(Loc::text(remove?"cleanup.confirm.title":"cleanup.reset.title"),std::string(Loc::text(remove?"cleanup.confirm.body":"cleanup.reset.body"))+"\n\n"+name,remove?"cleanup.confirm.action":"cleanup.reset.action",answerCleanupImport);
    return false;
}
}
int saveManagerImportGameHook(SaveManager*,const std::string&,const std::string&,int);
namespace {
void answerCleanupImport(int answer){
    CleanupImportConfirmation pending=cleanupConfirmation;cleanupConfirmation.approved=false;
    if(answer!=1||!pending.manager||pending.manager!=SaveManager::getSingleton()||pending.manager->importMenu!=pending.menu||pending.epoch!=v9WorldEpoch)return;
    cleanupConfirmation.approved=true;
    saveManagerImportGameHook(pending.manager,pending.root,pending.name,pending.flags);
}
}
