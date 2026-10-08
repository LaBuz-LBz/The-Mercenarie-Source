#include "Localization.h"
#pragma once
// Included after MissionPersistence: native imports regenerate actors. Only
// durable guild data is restored; mission actor handles are not reused.
namespace {
struct GuildImportData {
    std::string missionPayload,reputation,fiscal,boards,estate;
    GuildPayroll::State payroll;ArtisanOrders::Ledger artisan;double clock;
    bool virgin,recoveryPending;
    GuildImportData():clock(0),virgin(false),recoveryPending(false){}
};
GuildImportData readGuildImportData(const std::string& location,const std::string& name,bool reset=false){
    GuildImportData data;
    const std::string directory=GuildSavePaths::directory(location,name);
    data.virgin=GuildSavePaths::uninitialized(directory);
    // Reset preserves the source on disk but does not depend on its payload.
    if(reset)return data;
    const std::string mission=readMissionFile(directory+"/Mission.v4");
    if(!mission.empty()){
        data.missionPayload=openMissionArchive(mission);
        boost::shared_ptr<MissionValidationState> checked=validateMissionPayload(data.missionPayload);
        data.reputation=checked->snapshotReputation;data.fiscal=checked->snapshotFiscal;
        data.boards=checked->snapshotBoards;data.estate=checked->snapshotEstate;
        data.payroll=checked->guildPayroll;data.artisan=checked->artisanLedger;data.clock=checked->developerTimeOffsetHours;data.recoveryPending=!checked->importedArtisans.empty();
        if(!GuildProgression::validProgress(data.reputation))throw std::runtime_error("invalid imported progression snapshot");
        return data;
    }
    data.reputation=readMissionFile(directory+"/GuildEscortReputation.dat");
    if(data.missionPayload.empty()&&!GuildProgression::validProgress(data.reputation)){
        const std::string backup=readMissionFile(directory+"/GuildEscortReputation.dat.bak");
        if(GuildProgression::validProgress(backup))data.reputation=backup;
        else if(!data.virgin)throw std::runtime_error("missing or invalid imported guild progression");
    }
    data.fiscal=readMissionFile(directory+"/GuildEscortFiscal.dat");
    data.boards=readMissionFile(directory+"/GuildEscortBoards.dat");
    SaveText::boards(data.boards);FiscalLedger checkedFiscal;std::istringstream fiscalIn(data.fiscal);FiscalLedgerFormat::read(fiscalIn,checkedFiscal);
    return data;
}
void unpackGuildImportSnapshot(GuildImportData& data,bool reset=false){
    if(reset||data.missionPayload.empty())return;
    // Already decoded in isolated storage before the native import. Never apply
    // source actors/quests to the newly imported world merely to extract finances.
    if(!reset&&!GuildProgression::validProgress(data.reputation))throw std::runtime_error("invalid imported progression snapshot");
}
void clearGuildProgressForImport(){
    // Imported games regenerate actor identities. Never reuse EN_MISSION handles
    // from the source save; imported Characters therefore cannot remain locked.
    MissionAbsencePrototype::clearPersistence();
    MissionPlatoonPrototype::clearForImport();
    contractRewardPercent=100;
    rewardedContractIds.clear();fiscalLedger=FiscalLedger();successfulContracts=failedContracts=0;
    totalContractCats=totalAdvances=totalBonuses=totalTips=0;escortReputation=0;guildPoints=0;guildPrestige=0;
    contractHistory.clear();localReputations.clear();cityMemories.clear();archivedReputationAliases.clear();
    guildHouseNames.clear();guildHouseCities.clear();designatedGuildHouseKey.clear();operationalAnnouncementKey.clear();
    designatedGuildHouseHandle.setNull();savedContractBoards.clear();contractBoardsLoaded=false;appliedGuildUnlockLevel=-1;
}
}
