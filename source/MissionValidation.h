#pragma once
// This owns every destination touched by MissionStateSchema, quest contexts and
// rescue decoding. No live snapshot/rollback: rejected data never reaches globals.
#define MV(x) typedef decltype(x) MissionValidationType_##x;
#include "MissionValidationFields.h"
#undef MV
struct MissionValidationState {
    struct QuestStorage {
        std::vector<EscortQuestContext> values;
        QuestStorage():values(maximumActiveQuests,EscortQuestContext(false)){}
        EscortQuestContext& operator[](int i){return values[i];}
        const EscortQuestContext& operator[](int i)const{return values[i];}
    } escortQuests;
#define MV(x) MissionValidationType_##x x;
#include "MissionValidationFields.h"
#undef MV
    unsigned int platoonNextId;
    std::vector<MissionPlatoonPrototype::TestMission> platoons;
    MissionAbsencePrototype::PersistentState absence;
    std::string townStorage;
    void clearMissionPlatoons(){platoons.clear();platoonNextId=1;}
    template<class A> void archiveMissionPlatoons(A& a){
        MissionPlatoonPrototype::archiveState(a,platoonNextId,platoons);
    }
    void clearMissionAbsence(){absence=MissionAbsencePrototype::PersistentState();}
    template<class A> void archiveMissionAbsence(A& a){MissionAbsencePrototype::archive(a,&absence);}
    void resetQuestContexts(){selectedEscortQuest=selectedBountyQuest=0;questFiscalSequence=0;retiredQuestGroups.clear();for(int i=0;i<maximumActiveQuests;++i){escortQuests[i]=EscortQuestContext(false);bountyQuests[i]=MercenarieV5::BountyWorldState();questOrdersNeedRestore[i]=false;}}
    int activeQuestCount() const {int n=0;for(int i=0;i<maximumActiveQuests;++i){if(i==selectedEscortQuest?(missionActive||missionPending):escortQuests[i].occupied())++n;if(i==selectedBountyQuest?bountyWorld.contract.occupied():bountyQuests[i].contract.occupied())++n;}return n;}
    template<class A> void archiveQuestContexts(A& a){
const bool isolated=true;
#include "QuestContextsArchive.h"
    }
    template<class A> void archiveMissionRescue(A& a){
#include "MissionRescueArchive.h"
    }
    void decode(EngineMissionArchive& a){
#define MISSION_TOWN_STORAGE
#include "MissionStateSchema.h"
#undef MISSION_TOWN_STORAGE
    }
};
boost::shared_ptr<MissionValidationState> validateMissionPayload(const std::string& bytes){
    MercenariePerf::Phase perf("validation");
    boost::shared_ptr<MissionValidationState> state(new MissionValidationState());
    EngineMissionArchive archive(bytes,false);archive.validationOnly=true;state->decode(archive);
    if(!state->snapshotReputation.empty()&&!GuildProgression::validProgress(state->snapshotReputation))throw std::runtime_error("invalid progression snapshot");
    SaveText::boards(state->snapshotBoards);
    FiscalLedger fiscal;std::istringstream fiscalBytes(state->snapshotFiscal);FiscalLedgerFormat::read(fiscalBytes,fiscal);
    return state;
}
