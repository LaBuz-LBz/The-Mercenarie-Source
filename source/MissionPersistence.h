#pragma once
#include "MissionArchive.h"
#include "src/Save/Sections.h"
#include <boost/shared_ptr.hpp>
#include "MissionReportState.h"
#include "AutopilotTripArchive.h"

#include "MissionRescuePersistence.h"
#include "src/Save/Transaction.h"
#include <kenshi/SaveFileSystem.h>
#include <kenshi/GameDataManager.h>

// Included after the mission implementation so state types are complete.
namespace {
std::string missionRestoreBytes;
bool missionRestorePending=false,missionWorldChanging=false,missionRestoreNotice=false;
float missionRestoreClock=0;
bool missionRestoreWaitingForGroup=false;
bool missionRestoreWaitLogged=false;
std::string restoredFinalCaption;
bool restoredFinalVisible=false;
bool restoredProgressV2=false;
bool progressLoadFault=false;
std::string snapshotFiscal,snapshotReputation,snapshotBoards;
struct PreparedMissionSave {
    const std::string incident,path,mission,reputation,fiscal,boards;
    mutable SaveTransaction::Record transaction;
    PreparedMissionSave(const std::string& i,const std::string& p,const std::string& m,const std::string& r,const std::string& f,const std::string& b):incident(i),path(p),mission(m),reputation(r),fiscal(f),boards(b){}
};
boost::shared_ptr<const PreparedMissionSave> pendingMissionSave;
bool pendingMissionSaveErrorReported=false;
SaveDiagnostics::Failure pendingMissionSaveRoot;
DWORD pendingMissionRetryAt=0;
unsigned int pendingMissionRetryCount=0;
bool pendingNativeComplete=false;
std::string pendingNativeError;
bool nativeSaveCompletionHookReady=false;
bool nativeSerializationHookReady=false;
bool atomicMissionFile(const std::string& path,const std::string& bytes);
bool publishMissionSave(const PreparedMissionSave& prepared,SaveDiagnostics::Operation& op){
    op.component="transaction";op.path=prepared.transaction.slot;
    try{SaveTransaction::commit(prepared.transaction);saveLog(op,"COMMITTED");if(SaveTransaction::disabledRecord(prepared.transaction))DebugLog("[CLEANUP] disable marker committed; complete (native save verified)");return true;}
    catch(const std::exception& e){if(SaveTransaction::disabledRecord(prepared.transaction))ErrorLog(std::string("[CLEANUP] FAILED phase=publication ")+e.what());saveRemember(SaveDiagnostics::exceptionFailure(e));return false;}
}
void flushPendingMissionSave(){
    if(!pendingMissionSave||savePreparing||!pendingNativeComplete||pendingMissionRetryCount>=5)return;
    const DWORD now=GetTickCount();if(pendingMissionRetryAt&&(LONG)(now-pendingMissionRetryAt)<0)return;
    pendingMissionRetryAt=now+(2000u<<pendingMissionRetryCount);++pendingMissionRetryCount;
    MercenariePerf::Event perf("SAVE","async-publication");perf.id(pendingMissionSave->incident);
    SaveDiagnostics::Operation op;op.id=pendingMissionSave->incident;op.phase="publication-retry";op.nativeCalled=op.nativeSucceeded=true;
    if(pendingMissionSaveErrorReported)op.remember(pendingMissionSaveRoot);
    try{SaveOperationGuard guard(op);savePreparing=false;
        if(!pendingNativeError.empty()){
            op.nativeSucceeded=false;
            if(MercenarieCleanup::disabled)ErrorLog("[CLEANUP] FAILED phase=native-save");
            SaveTransaction::failNative(pendingMissionSave->transaction);
            SaveDiagnostics::Failure f;f.reason=SaveDiagnostics::NativeFailed;f.detail=pendingNativeError;
            op.remember(f);saveLog(op,"NATIVE_SAVE_FAILED",&f);saveNotify(op,"persistence.lot1.native_failed");
            pendingMissionSave.reset();return;
        }
        if(pendingMissionSave->transaction.phase==SaveTransaction::NativeSaving)SaveTransaction::confirmNative(pendingMissionSave->transaction);
        if(publishMissionSave(*pendingMissionSave,op)){saveLog(op,"COMPLETE");pendingMissionSave.reset();pendingMissionSaveErrorReported=false;}
        else if(!pendingMissionSaveErrorReported){pendingMissionSaveRoot=op.first;pendingMissionSaveErrorReported=true;saveNotify(op,"persistence.lot1.publish_failed");}
    }catch(const std::exception& e){op.remember(SaveDiagnostics::exceptionFailure(e));try{saveLog(op,"RETRY_FAILED",&op.first);if(!pendingMissionSaveErrorReported){pendingMissionSaveRoot=op.first;pendingMissionSaveErrorReported=true;saveNotify(op,"persistence.lot1.publish_failed");}}catch(...){} }
    catch(...){try{ErrorLog("MercenarieSave: publication-retry unknown C++ exception");}catch(...){} }
}

void (*missionSaveSyncOriginal)(SaveFileSystem*);
bool (*missionNativeSerializeOriginal)(GameDataContainer*,const std::string&,Serialisable*);
bool missionNativeSerializeHook(GameDataContainer* data,const std::string& path,Serialisable* extra){
    const bool result=missionNativeSerializeOriginal&&missionNativeSerializeOriginal(data,path,extra);
    if(!result&&pendingMissionSave&&activeSaveOperation&&activeSaveOperation->nativeCalled)
        pendingNativeError="native data serialization failed: "+path;
    return result;
}
void missionSaveSyncHook(SaveFileSystem* fs){
    // sync clears failedToCopyError and changes COMPLETE to NORMAL. Observe
    // both BEFORE delegating; never infer success from saveGame's return code.
    if(fs&&pendingMissionSave&&fs->state==SaveFileSystem::COMPLETE){
        try{if(SaveTransaction::canonical(fs->getActiveSave())==pendingMissionSave->transaction.slot){
            if(pendingNativeError.empty())pendingNativeError=fs->failedToCopyError;pendingNativeComplete=true;
            DebugLog(std::string("MercenarieSave: native completion slot=")+pendingMissionSave->transaction.slot+" result="+(pendingNativeError.empty()?"OK":"FAILED"));
        }}catch(const std::exception& e){pendingNativeError=e.what();pendingNativeComplete=true;}
    }
    if(missionSaveSyncOriginal)missionSaveSyncOriginal(fs);
}

struct EngineMissionArchive:MissionArchive {
    unsigned int missing;
    bool resolve;
    bool requireActors;
    bool validationOnly;
    EngineMissionArchive():missing(0),resolve(true),requireActors(true),validationOnly(false){}
    EngineMissionArchive(const std::string& b,bool r):MissionArchive(b),missing(0),resolve(r),requireActors(true),validationOnly(false){}
    template<class T> void field(T& v){try{MissionArchive::field(v);}catch(const std::exception& e){SaveDiagnostics::Failure f=SaveDiagnostics::exceptionFailure(e);f.size=bytes.size();throw SaveDiagnostics::Error(f);}}
    template<class T> void field(std::vector<T>& v){unsigned int n=(unsigned int)v.size();field(n);if(n>4096){SaveDiagnostics::Failure f;f.reason=SaveDiagnostics::SizeLimit;f.detail="oversized actors/list";f.count=n;f.size=bytes.size();throw SaveDiagnostics::Error(f);}requireElements(n,1);if(reading)v.resize(n);for(unsigned int i=0;i<n;++i)field(v[i]);}
    void field(Ogre::Vector3& v){field(v.x);field(v.y);field(v.z);}
    void field(hand& h){
        MercenariePerf::Phase perf("references");
        // Persist numeric handle identity, never its vtable or an address.
        field(h.type);field(h.container);field(h.containerSerial);field(h.index);field(h.serial);
    }
    void field(Character*& c){hand h;if(!reading&&c)h=c->getHandle();else h.setNull();field(h);if(reading){c=resolve&&!h.isNull()?h.getCharacter():0;if(resolve&&requireActors&&!h.isNull()&&!c)++missing;}}
    void field(TownBase*& t){hand h;if(!reading&&t)h=hand(t);else h.setNull();field(h);if(reading)t=resolve&&!h.isNull()?h.getTown():0;}
    void field(Faction*& f){std::string id;if(!reading&&f&&f->getData())id=f->getData()->stringID;field(id);if(reading)f=resolve&&ou&&ou->factionMgr&&!id.empty()?ou->factionMgr->getFactionByStringID(id):0;}
    void field(MercRewardBreakdown& v){field(v.base);field(v.distance);field(v.mission);field(v.environment);field(v.beforeDanger);field(v.afterDanger);field(v.afterRarity);field(v.guildHouseBonus);field(v.beforeNegotiation);field(v.negotiation);field(v.finalBeforeMissionBonuses);}
    void field(EscortContractData& v){field(v.schemaVersion);field(v.origin);field(v.destination);field(v.originId);field(v.destinationId);field(v.routeRegions);
#ifdef MERCENARIE_CARAVAN_DELIVERY
        if(reading)CaravanDelivery::read(v.routeRegions);
#ifdef MERCENARIE_CARAVAN_VISIT
        if(reading)CaravanVisit::read(v.routeRegions);
#endif // Validate before committing any restored quest.
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
        if(reading)CaravanHomeAmbush::read(v.routeRegions),CaravanBattleResolution::read(v.routeRegions);
#ifdef MERCENARIE_CARAVAN_SPEECH
        if(reading)CaravanNegotiation::read(v.routeRegions,0);
#endif
#endif
#endif
        field(v.type);field(v.source);field(v.rarity);field(v.caravanSize);field(v.cargoClass);field(v.studyClass);field(v.studyDuration);field(v.environmentTags);field(v.dangerLevel);field(v.groupSize);field(v.cargoValue);field(v.selectedBonuses);field(v.bonusPay);field(v.advance);field(v.finalPay);field(v.totalPay);field(v.counterOffers);field(v.negotiationPercent);field(v.basePay);field(v.distanceKm);field(v.danger);field(v.initialRate);field(v.negotiatedRate);field(v.wealth);field(v.personality);field(v.prestigious);field(v.urgent);field(v.advancePaidOnce);field(v.settlementPaid);field(v.reward);}
    void field(EscortJourneyData& v){field(v.seconds);field(v.distanceTravelled);field(v.combats);field(v.ambushes);field(v.dangerousRegions);field(v.detours);field(v.knockedOut);field(v.gravelyInjured);field(v.mutilated);field(v.arrivedOnTime);}
    void field(MailContracts::Step& v){field(v.stepId);field(v.letterHandle);field(v.recipientId);field(v.recipientName);field(v.townId);field(v.townName);field(v.carrierId);field(v.carrierName);field(v.recipientRole);field(v.delivered);field(v.distanceKm);}
    void field(MailContracts::Contract& v){v.archive(*this);}
    void field(DelegatedMissionState& v){v.archive(*this);}
    void field(WaitingHereState& v){field(v.actor);field(v.position);field(v.movement);field(v.center);field(v.centerTarget);field(v.returning);}
    void field(GuildVisitor& v){
        SavedActorResolution::archive(*this,v.leader,v.restoredLeader);
        SavedActorResolution::archive(*this,v.members,v.restoredMembers);
        field(v.patience);field(v.remarkClock);field(v.orderRefresh);field(v.type);field(v.houseKey);field(v.offerKey);field(v.targetHouse);field(v.assignedSeats);field(v.urgent);field(v.vip);field(v.exceptional);field(v.arrivalNotified);
        if(reading&&v.restoredMembers.pending){v.leader=0;v.restoredLeader.pending=!v.restoredLeader.identity.isNull();v.restoredLeader.retry=0;}
    }
    void field(DepartingVisitors& v){SavedActorResolution::archive(*this,v.members,v.restoredMembers);field(v.cleanup);}
    void field(RefusedDeparture& v){field(v.actor);field(v.rendezvous);field(v.target);field(v.retry);}
    void field(FiscalParty& v){field(v.organisation);field(v.raid);field(v.leader);field(v.members);field(v.stuckClock);field(v.seatRefresh);field(v.waitingForConversation);field(v.lastPosition);}
    void field(GuildSeatReservation& v){field(v.actor);field(v.seat);field(v.kind);field(v.queueKey);}
    void field(GuildSeatTicketState& v){field(v.key);field(v.ticket);}
    void field(GuildTravelRecord& v){field(v.actor);field(v.office);v.state.archive(*this);if(reading){v.commandedSeat.setNull();v.rejectedSeats.clear();v.rejectCooldown=0;v.seatRetry=v.resolveRetry=0;v.destinationValid=v.missingLogged=false;v.pendingSeatRetry=GuildVisitorTravel::None;v.lastPosition=Ogre::Vector3::ZERO;}}
    void field(MissionPlatoonPrototype::Member& v){field(v.character);field(v.originalPlatoon);field(v.temporaryPlatoon);field(v.originalIndex);field(v.originalLeader);field(v.returnPosition);}
    void field(MissionPlatoonPrototype::TestMission& v){field(v.id);field(v.members);}
};
template<class A> void archiveMissionPlatoons(A& a){MissionPlatoonPrototype::archive(a);}
void clearMissionPlatoons(){MissionPlatoonPrototype::clearForImport();}
template<class A> void archiveMissionAbsence(A& a){MissionAbsencePrototype::archive(a);}
void clearMissionAbsence(){MissionAbsencePrototype::clearPersistence();}
void archiveMissionState(EngineMissionArchive& a){
    MercenariePerf::Phase perf(a.reading?"reconstruction":"serialization");
#define MISSION_TOWN_STORAGE static std::string townStorage;
#include "MissionStateSchema.h"
#undef MISSION_TOWN_STORAGE
}
#include "MissionValidation.h"

std::string readMissionFile(const std::string& path){
    std::string bytes;UnicodeFileSystem::Result result=UnicodeFileSystem::readDetailed(path,bytes,MissionArchive::MaximumBytes+1024);
    if(result.status==UnicodeFileSystem::ReadMissing)return "";
    if(!result.ok)throw SaveDiagnostics::Error(saveIOFailure(result));
    return bytes;
}
bool atomicMissionFile(const std::string& path,const std::string& bytes){
    if(savePreparing){SaveDiagnostics::Failure f;f.reason=SaveDiagnostics::PrepareWrite;f.detail="write forbidden during prepare";f.path=path;saveRemember(f);throw SaveDiagnostics::Error(f);}
    if(path.find("GuildEscortReputation.dat")!=std::string::npos&&!GuildProgression::validProgress(bytes)){SaveDiagnostics::Failure f;f.reason=SaveDiagnostics::SizeLimit;f.path=path;f.detail="invalid or oversized progression; previous file preserved";lastPersistenceIO=f;saveRemember(f);return false;}
    UnicodeFileSystem::Result result;
    // Preserve the existing progression recovery file. This is a runtime data
    // safeguard, not an installation backup; never run it during preparation.
    if(path.find("GuildEscortReputation.dat")!=std::string::npos){
        std::string previous;result=UnicodeFileSystem::readDetailed(path,previous,GuildProgression::MaximumProgressBytes);
        if(result.status==UnicodeFileSystem::ReadMissing)result=UnicodeFileSystem::Result();
        else if(result.ok&&previous!=bytes&&GuildProgression::validProgress(previous))
            result=UnicodeFileSystem::atomicWrite(path+".bak",previous);
    }
    if(result.ok)result=UnicodeFileSystem::atomicWrite(path,bytes);
    if(!result.ok){lastPersistenceIO=saveIOFailure(result);saveRemember(lastPersistenceIO);if(!activeSaveOperation)ErrorLog(SaveDiagnostics::record(SaveDiagnostics::code("IO",lastPersistenceIO.reason),"runtime",result.detail+" stage="+result.stage+" win32="+FiscalLedger::toString(result.error),path));}
    return result.ok;
}
std::string captureMissionState(){
    MercenariePerf::Phase perf("capture-serialization-validation");
    saveStage("estate","available");
    if(estateBusy||!estateState.available)throw std::runtime_error(estateRootError.empty()?"estate state unavailable during save":estateRootError);
    // Capture must not advance leases, debit wallets or persist working files.
    saveStage("reputation","progressWriteBlocked");
    if(progressWriteBlocked){if(hasWorldPersistenceRoot)throw SaveDiagnostics::Error(worldPersistenceRoot);throw std::runtime_error("guild progress unavailable: refusing to save zero over original");}
    if(rewardedContractIds.size()>GuildProgression::MaximumPaidIds){SaveDiagnostics::Failure f;f.reason=SaveDiagnostics::ProgressLimit;f.data="rewardedContractIds";f.count=rewardedContractIds.size();f.detail="rewardedContractIds limit: count exceeds 250000";throw SaveDiagnostics::Error(f);}
    snapshotReputation=serializeReputations();
    if(snapshotReputation.size()>GuildProgression::MaximumProgressBytes)throw std::runtime_error("progression byte limit exceeds 16777216");
    {MercenariePerf::Phase perf("validation");if(!GuildProgression::validProgress(snapshotReputation))throw std::runtime_error("guild progression invalid before save");}
    saveStage("fiscal","snapshot");
    std::ostringstream financeSnapshot;FiscalLedgerFormat::write(financeSnapshot,fiscalLedger);
    if(!financeSnapshot.good())throw std::runtime_error("fiscal serialization failed");snapshotFiscal=financeSnapshot.str();
    saveStage("boards","snapshot");snapshotBoards=serializeContractBoards();
    restoredFinalVisible=finalWindow&&finalWindow->getVisible()&&!reportReadOnly;
    restoredFinalCaption=restoredFinalVisible&&finalSummaryText?std::string(finalSummaryText->getCaption()):"";
    saveStage("missions","archive");if(activeSaveOperation)activeSaveOperation->phase="serialization";EngineMissionArchive a;archiveMissionState(a);return a.bytes;
}
void discardMissionWorldReferences(){
    completedEscortRestore=SavedActorResolution::Actor();completedCaravanRestore=SavedActorResolution::Group();caravanRestore=scientificRestore=SavedActorResolution::Group();
    importedDomainsSuspended=false;importedArtisans.clear();importedArtisanHour=-1;payrollRecoveryConfirm.clear();
    GuardRest::discardWorld();GuardSalute::discardWorld();
    estateResetWorld();
    closePayrollWindowsForWorld();guildPayroll=GuildPayroll::State();payrollSyncing=false;
    resetContractBoardWorld();
    resetMissionBookNativeUse();
    MissionAbsencePrototype::discardRuntimeReferences();
    resetQuestContexts();
    missionGroup.reset();
    finalBonusChoice.reset();
    progressMembers.clear();progressDeadMembers.clear();progressRosterKnown=false;progressAnyClientKo=false;progressReportLocal=progressReportGlobal=0;
    testRoute.cancel();
    // The old world is being unloaded: never destroy or issue jobs through its pointers.
    missionRescue=MissionRescueState();missionTemporaryLeader=0;
    escort=0;escortHandle.setNull();contractBarman=0;contractBarmanHandle.setNull();
    contractOriginTown=0;contractOriginFaction=0;
    missionActive=missionPending=missionPaused=missionFollowing=false;missionPace=EscortPace::Normal;missionForcedPace=false;missionCasualtyWaiting=false;missionSeparation=EscortMissionRules::Separation();carriedDestinationDistance=-1;contractLifecycle=CONTRACT_NONE;
    waitingForPlayer=leavingBuilding=caravanMission=caravanReturning=scientificMission=scientificResearching=scientificReturning=false;
    missionFollowTarget.setNull();missionFollowers.clear();waitingHere.clear();
    caravanCustomerState.clear();
    caravanMembers.clear();scientificMembers.clear();guildVisitors.clear();departingVisitors.clear();guildSeatReservations.clear();guildSeatTickets.clear();guildTravelRecords.clear();fiscalTargetOffice.setNull();guildSeatNextTicket=1;
    refusedDepartures.clear();completedCaravan.clear();completedEscort=0;fiscalParty=FiscalParty();
    guildBuilding=guildClientChair=guildFiscalChair=0;guildClientChairs.clear();guildWaitingChairs.clear();
    guildClientSystemActive=false;guildHouseOperational=false;
    designatedGuildHouseHandle.setNull();pendingGuildHouseHandle.setNull();guildFacilityScanClock=0;
    guildFurnitureRecovery=GuildFurnitureRecovery::State();
    negotiationOpen=false;negotiationSuspended=false;currentContract=EscortContractData();journeyData=EscortJourneyData();
    fcsLanguageApplied=false;appliedGuildUnlockLevel=-1;
}
void releaseMissionUIBeforeLoad(){
    ++mercenarieUISession;DebugLog("MercenarieUI lifecycle: event=release_before_load_or_shutdown");
    if(MyGUI::Gui::getInstancePtr()&&mercenarieFindLiveWidget(MyGUI::Gui::getInstance().getEnumerator(),guildWindow))closeGuildInvestment(0);
    else{guildInvestmentOverlay=0;investmentPreviousPages.clear();investmentPageActive=false;}
    jWasDown=pWasDown=autopilotWasDown=false;
    resetFinanceView();resetGuildPages();resetGuildMenuViewReferences();resetMailMapSelection();
    clearRouteTestUI();
    rewardPopup87=0;
    resetBoard77();
    resetMissionBookPresentation();
    // Child of trackerWindow: destroyed with its parent, never reuse after load.
    bountyTrackerEntry=0;
    guildKeyCapture=false;guildKeyLabel=0;guildKeyTip=0;
    mercenarieDestroyLiveWidget(mercenarieAutopilotWindow);
    mercenarieAutopilotWindow=0;autopilotActor.setNull();autopilotTrips.clear();autopilotTowns.clear();resetAutopilotView();
    MyGUI::Gui* g=MyGUI::Gui::getInstancePtr();
    if(g){
        mercenarieDestroyLiveWidget(g->findWidget<MyGUI::Widget>("MercenarieCaravanTradeTimer",false));
        mercenarieDestroyLiveWidget(g->findWidget<MyGUI::Widget>("MercenarieCaravanScenarioPicker",false));
        mercenarieDestroyLiveWidget(g->findWidget<MyGUI::Widget>("MercenarieUnavailable",false));
        mercenarieDestroyLiveWidget(mercenarieLauncherMenu);
        mercenarieDestroyLiveWidget(trackerIcon);
        mercenarieDestroyLiveWidget(trackerWindow);
        mercenarieDestroyLiveWidget(negotiationWindow);
        mercenarieDestroyLiveWidget(contractDecisionWindow);
        mercenarieDestroyLiveWidget(guildWindow);
        guildInvestmentOverlay=0;investmentAmount=investmentReward=investmentBalance=0;
        investmentConfirm=investmentMinus=investmentPlus=0;for(int i=0;i<4;++i)investmentQuick[i]=0;
        mercenarieDestroyLiveWidget(fiscalCollectorWindow);
        mercenarieDestroyLiveWidget(defineGuildHouseButton);
        mercenarieDestroyLiveWidget(guildLevelUpWindow);
        mercenarieDestroyLiveWidget(developerWindow);
        mercenarieDestroyLiveWidget(developerFinishPicker);
        mercenarieDestroyLiveWidget(missionDebugPicker);
        mercenarieDestroyLiveWidget(missionDebugTip);
        mercenarieDestroyLiveWidget(finalWindow);
        mercenarieDestroyLiveWidget(mailDeliveryWindow89);
        mercenarieDestroyLiveWidget(guildNameWindow);
        mercenarieDestroyLiveWidget(contractsWindow);
        mercenarieDestroyLiveWidget(resetMercenarieImportCheck);
        mercenarieDestroyLiveWidget(resetMercenarieImportLabel);
        mercenarieDestroyLiveWidget(cleanupMercenarieImportCheck);mercenarieDestroyLiveWidget(cleanupMercenarieImportLabel);
    }
    mercenarieLauncherMenu=0;
    trackerIcon=0;
    trackerWindow=0;
    trackerEntry=0;resetQuestTrackerView();
    negotiationWindow=0;negotiationContent=0;negotiationConditions=0;negotiationRatePanel=0;negotiationFill=0;negotiationFrame=0;
    offerText=0;
    proposalText=0;
    sliderPercentText=0;
    reactionText=0;
    priceSlider=0;
    negotiationSliderPanel=0;
    negotiationSliderTrack=0;
    negotiationSliderTitle=0;
    sliderMinusButton=0;
    sliderPlusButton=0;
    proposeButton=0;
    counterButton=0;
    refuseButton=0;
    returnButton=0;
    contractDecisionWindow=0;basicContractTick=0;
    contractDecisionText=0;
    contractDecisionAccept=0;
    contractDecisionRefuse=0;
    guildWindow=0;
    guildZonesText=0;
    guildDetailsText=0;
    guildStatsText=0;
    guildHeroTitleText=0;
    guildHeroLevelText=0;
    guildHeroXpText=0;
    guildHeroUnlockText=0;
    guildHeroDescriptionText=0;
    guildHistoryText=0;
    guildSuccessText=0;
    guildCatsText=0;
    guildBenefitsText=0;
    guildContractText=0;
    guildHouseText=0;
    guildActivityText=0;
    guildLevelProgress=0;
    guildLevelNumberText=0;
    for(int i=0;i<8;++i)guildOverviewPanels[i]=0;
    guildReputationPanel=0;
    for(int i=0;i<6;++i)guildTabButtons[i]=0;
    guildRelationsPanel=0;




    guildOfficesPanel=0;
    guildOfficesText=0;
    guildOfficeCountText=0;
    guildOfficeDetailText=0;
    guildOfficeCreateText=0;
    guildOfficeDefineButton=0;
    guildFinancesPanel=0;
    guildFinancesText=0;
    guildFinanceGrossText=0;
    guildFinancePaidText=0;
    guildFinanceDebtText=0;
    guildFinanceUcText=0;
    guildFinanceGuildText=0;
    guildFinanceHistoryText=0;
    for(int i=0;i<2;++i)fiscalPayButtons[i]=0;
    fiscalRulesButton=0;
    fiscalCollectorWindow=0;
    fiscalCollectorText=0;
    fiscalCollectorPay=0;
    fiscalCollectorDetail=0;
    fiscalCollectorExtension=0;
    fiscalCollectorRefuse=0;
    for(int i=0;i<4;++i)reputationFilterButtons[i]=0;
    defineGuildHouseButton=0;
    resetMercenarieImportCheck=0;
    resetMercenarieImportLabel=0;cleanupMercenarieImportCheck=0;cleanupMercenarieImportLabel=0;
    guildLevelUpWindow=0;
    guildLevelUpText=0;
    guildLevelUpClose=0;
    guildLevelCardsCanvas=0;
    guildLevelQueue.clear();
    guildLevelUpWasPaused=false;
    for(int i=0;i<16;++i)reputationRows[i]=0;
    for(int i=0;i<16;++i)reputationIcons[i]=0;
    reputationPageDetails=0;
    reputationPageSummary=0;
    reputationPageEmpty=0;
    developerWindow=0;
    developerRaidButton=developerKoButton=0;missionDebugPicker=0;missionDebugTip=0;missionDebugBusy=false;missionDebugReadyAt=0;
    developerFinishPicker=0;developerFinishCount=0;
    developerStatus=0;
    for(int i=0;i<6;++i)bonusButtons[i]=0;
    paymentBreakdownText=0;
    mailDeliveryWindow89=0;
    finalWindow=0;reportReadOnly=false;
    finalSummaryText=0;
    finalBonusPreview=0;for(int i=0;i<MissionBonuses::Count;++i)finalBonusButtons[i]=0;
    finalCashButton=0;
    finalHalfCashButton=0;
    finalReputationButton=0;
    guildNameWindow=0;
    guildNameEdit=0;
    guildNameConfirm=0;
    contractsWindow=0;
    for(int i=0;i<6;++i)contractButtons[i]=0;
    for(int i=0;i<6;++i){offerNameV6[i]=offerTypeV6[i]=offerRarityV6[i]=offerPriceV6[i]=0;offerIconV6[i]=0;offerRerollV6[i]=0;for(int j=0;j<4;++j)offerEdgesV6[i][j]=0;}
    boardXpV6=0;boardMissionIconV6=0;for(int i=0;i<5;++i)boardDangerV6[i]=0;
    contractsDetails=0;
    for(int i=0;i<4;++i)contractsInfo[i]=0;
    contractsRewardAmount=0;
    contractsRewardReputation=0;
    contractsRewardBonus=0;
    contractsMap=0;
    contractsMapImage=0;
    contractRouteDots.clear(); // Children destroyed with the map, never reuse.
    contractsMapPanel=0;
    contractsLegend=0;
    contractsAccept=0;
    contractsRefreshText=0;
    for(int i=0;i<14;++i)cityMarkers[i]=0;
    for(int i=0;i<14;++i)routeCityIcons[i]=0;
    contractDialogueGiver.setNull();contractBoardOpenedFromDialogue=false;
    for(int i=0;i<4;++i)registerValues[i]=0;
    for(int i=0;i<16;++i)registerXpFill[i]=0;
    for(int i=0;i<6;++i)registerTabMarks[i]=0;
    registerXpDetail=0;configuredMercenarieImportMenu=0;activeFiscalOrganisation=-1;
    guildOptionsPanel=0;clientTrackerCheck=0;clientOptionTip=0;contractRewardSlider=0;contractRewardValue=0;contractRewardTip=0;
    optionsScroll=0;optionsSearch=0;optionsSearchHint=0;optionsCategoryFilter=0;optionsResetAllButton=0;optionsRows.clear();guildKeyLabel=0;guildKeyTip=0;for(int i=0;i<7;++i){optionsCategoryHeaders[i]=0;optionsCategoryBodies[i]=0;optionsCategoryChevrons[i]=0;optionsCategoryTitles[i]=0;optionsCategoryDescriptions[i]=0;}for(int i=0;i<ClientOptions::ActionCount;++i)optionKeyLabels[i]=0;guildKeyCapture=false;
}
bool restoreMissionState(){
    if(!missionRestorePending)return true;
    MercenariePerf::Event perf("LOAD","restore-attempt");
    try{
        missionRestoreWaitingForGroup=false;
        EngineMissionArchive a(missionRestoreBytes,true);archiveMissionState(a);
        // Inventory streaming is independent of guild visitors and other actors.
        // Run even when the world-state gate below must still wait.
        restoreMailRouteSnapshots();
        restoreMailLetterIdentities();
        // Progress belongs to the save, not to actor streaming or disk writability.
        if(!snapshotReputation.empty()){std::istringstream progress(snapshotReputation);readReputations(progress);}
        if(progressWriteBlocked)throw std::runtime_error("guild progression unreadable");
        // Pointer-only world groups retain their own identities and retry locally.
        // The archive and progression are decoded/applied once, never per frame.
        escort=escortHandle.isNull()?0:escortHandle.getCharacter();
        refreshQuestActors();
        // Missing world actors still wait without spawning replacements. The
        // group notice is reserved for actual EN_MISSION party restoration.
        if(missionActive||missionPending){for(size_t i=0;i<progressMembers.size();++i){Character* member=progressMembers[i].getCharacter();ActivePlatoon* active=member?member->getPlatoon():0;if(active&&active->me)active->me->setPersistentSquad(true);}}
        if(!progressRosterKnown&&progressMembers.empty()&&escort){const std::vector<Character*>& group=caravanMission?caravanMembers:scientificMembers;if(group.empty())progressMembers.push_back(escort->getHandle());else for(size_t i=0;i<group.size();++i)if(group[i])progressMembers.push_back(group[i]->getHandle());progressRosterKnown=progressMembers.size()>=(size_t)std::max(1,currentContract.groupSize);}
        // Unloaded mission parties resume independently when their handles resolve.
        if(!atomicMissionFile(fiscalFile,snapshotFiscal)||!atomicMissionFile(contractBoardsFile,snapshotBoards))
            throw std::runtime_error("cannot restore financial snapshot");
        fiscalLedger=FiscalLedger();savedContractBoards.clear();contractBoardsLoaded=false;guildRerolls=ContractReroll::Charges();resetV9PendingActions();
        loadFiscalLedger();loadContractBoards();
        saveReputations();
        std::map<std::string,CityContractBoard>::iterator b=savedContractBoards.find(currentBoardKey);
        if(b!=savedContractBoards.end())for(int i=0;i<6;++i)boardOffers[i]=b->second.offers[i];
        contractBarman=resolveContractBarman();negotiationOpen=false;
        if(missionPending){negotiationSuspended=true;contractLifecycle=CONTRACT_CLIENT_MEETING;}
        // Per-quest order restoration runs when that quest's own roster is ready.
        // Do not clear saved followers or issue duplicate jobs here.
        for(size_t i=0;i<waitingHere.size();++i)waitingHere[i].returning=false;
        if(restoredFinalVisible&&!currentContract.settlementPaid){
            if(!restoredProgressV2){pendingGuildXp=calculateGuildXpReward();Loc::Catalogue args;std::ostringstream xp,local,global,payment,bonuses;xp<<pendingGuildXp;local<<std::showpos<<progressReportLocal;global<<std::showpos<<progressReportGlobal;payment<<missionReward;bonuses<<earnedFinalBonus;args["xp"]=xp.str();args["local"]=local.str();args["global"]=global.str();args["payment"]=payment.str();args["bonuses"]=bonuses.str();restoredFinalCaption=Loc::format("mission.report.restored",args);}
            createFinalWindow();
            if(!finalBonusChoice.prepared)prepareFinalBonuses();
            if(finalWindow){refreshFinalBonuses();finalWindow->setVisible(true);if(ou)ou->userPause(true);}
        }
        if(!caravanRestore.pending&&!scientificRestore.pending)updateMissionFormation(0);
        unsigned int waitingVisitors=0,waitingDepartures=0;
        for(size_t i=0;i<guildVisitors.size();++i)if(guildVisitors[i].restoredLeader.pending||guildVisitors[i].restoredMembers.pending)++waitingVisitors;
        for(size_t i=0;i<departingVisitors.size();++i)if(departingVisitors[i].restoredMembers.pending)++waitingDepartures;
        std::ostringstream domains;domains<<"MercenarieSave: phase=restore result=DECODED_ONCE visitors_waiting="<<waitingVisitors<<" departures_waiting="<<waitingDepartures<<" delegation_waiting="<<MissionAbsencePrototype::persistent.active;DebugLog(domains.str());
        missionRestorePending=false;missionRestoreBytes.clear();
        diagnosticTrace("MISSION restored persistent party");return true;
    }catch(const std::exception& e){
        progressLoadFault=true;progressWriteBlocked=true;
        ErrorLog(std::string("Guild Escort: mission restore failed: ")+e.what());
        discardMissionWorldReferences();missionRestorePending=false;missionRestoreBytes.clear();
        reportPersistenceException("RESTORE",e,reputationFile,"v8.literal.118");
        return false;
    }
}
void updateSavedAbsenceRestore(float elapsed){
    if(!MissionAbsencePrototype::persistent.active)return;
    MissionAbsencePrototype::restoreElapsed+=elapsed;
    if(MissionAbsencePrototype::restoreElapsed>15&&!missionRestoreNotice){missionRestoreNotice=true;if(ou)ou->showPlayerAMessage(Loc::text("ui.guild_mission_waiting_for_the_saved_group_to_load"),true);}
    MissionAbsencePrototype::restoreRetry-=elapsed;
    if(MissionAbsencePrototype::restoreRetry>0||MissionAbsencePrototype::restoreFault)return;
    MissionAbsencePrototype::restoreRetry=1;
    try{MissionAbsencePrototype::restorePersistentState(MissionAbsencePrototype::restoreElapsed);}
    catch(const std::exception& e){
        MissionAbsencePrototype::restoreFault=true;
        ErrorLog(std::string("MercenarieSave: phase=restore component=delegation result=DOMAIN_SUSPENDED reason=")+e.what());
        reportPersistenceError("RESTORE",SaveDiagnostics::NativeFailed,e.what(),reputationFile,"v8.literal.118");
    }
}
std::string missionSidecarPath(const std::string& location,const std::string& name){
    // The native saveGame/loadGame API supplies the save root and the actual slot,
    // including autosave names. Do not use getCurrentGame (which can lag Save As).
    return GuildSavePaths::directory(location,name)+"/Mission.v4";
}
}
int (*missionSaveOriginal)(SaveManager*,const std::string&,const std::string&);
int missionSaveHook(SaveManager* m,const std::string& location,const std::string& name){
    MercenariePerf::Event perf("SAVE","prepare-and-native-call");
    SaveDiagnostics::Operation op;
    try{
        static unsigned long sequence=0;std::ostringstream id;id<<"SAVE-"<<std::setfill('0')<<std::setw(6)<<++sequence;op.id=id.str();perf.id(op.id);op.path=location+"/"+name;
        if(activeSaveOperation){SaveDiagnostics::Failure f;f.detail="reentrant native save refused";f.reason=SaveDiagnostics::PendingPublication;saveRemember(f);return -1;}
        // An explicit subsequent save request can retry the SAME retained
        // generation after a transient fault; it never replaces its bytes.
        if(pendingMissionSave&&pendingNativeComplete&&pendingMissionRetryCount>=5){pendingMissionRetryCount=0;pendingMissionRetryAt=0;}
        flushPendingMissionSave();
        SaveOperationGuard guard(op);saveLog(op,"BEGIN");
        if(pendingMissionSave)throw std::runtime_error("previous sidecar publication still pending");
        if(!missionSaveOriginal||!nativeSaveCompletionHookReady||!nativeSerializationHookReady){SaveDiagnostics::Failure f;f.reason=SaveDiagnostics::HookUnavailable;f.detail="native save hook unavailable";throw SaveDiagnostics::Error(f);}
        if(artisanBusy){SaveDiagnostics::Failure f;f.reason=SaveDiagnostics::ArtisanBusy;f.component="artisan";f.detail="artisan operation in progress";throw SaveDiagnostics::Error(f);}
        if(missionWorldChanging||missionRestorePending)throw std::runtime_error("save deferred: world restoration not complete");
        if(!MercenarieCleanup::protectedSource.empty()&&(MercenarieCleanup::rotatingSlot(name)||SaveTransaction::canonical(SaveTransaction::parent(SaveTransaction::parent(missionSidecarPath(location,name))))==MercenarieCleanup::protectedSource))throw std::runtime_error("cleanup: save under a NEW manual slot; import source and rotating slots are protected until reload");
        const bool disabled=MercenarieCleanup::disabled;if(disabled){MercenarieCleanup::researchCleaned=false;cleanupNativeOwnedResearch();}
        if(disabled&&!MercenarieCleanup::researchCleaned)throw std::runtime_error("cleanup: native research state not ready; save deferred");
        if(disabled&&progressWriteBlocked)throw std::runtime_error("cleanup: incomplete world operation; save refused");
        const std::string payload=disabled?std::string(MercenarieCleanup::marker()):captureMissionState();
        op.component="missions";op.phase="validation";op.data="complete archive";
        if(payload.empty()||payload.size()>MissionArchive::MaximumBytes)throw std::runtime_error("oversized or empty total archive");
        saveLog(op,"VALIDATED");op.phase="seal";
        const std::string sealed=disabled?payload:sealMissionArchive(payload);
        if(!disabled&&openMissionArchive(sealed)!=payload)throw std::runtime_error("mission checksum round-trip failed");
        const boost::shared_ptr<const PreparedMissionSave> prepared(new PreparedMissionSave(op.id,missionSidecarPath(location,name),sealed,disabled?std::string():snapshotReputation,disabled?std::string():snapshotFiscal,disabled?std::string():snapshotBoards));
        op.phase="transaction-prepare";
        const std::string values[]={prepared->mission,prepared->reputation,prepared->fiscal,prepared->boards};
        prepared->transaction=SaveTransaction::begin(SaveTransaction::parent(SaveTransaction::parent(prepared->path)),values);
        SaveTransaction::markNative(prepared->transaction);
        SaveFileSystem* nativeFiles=SaveFileSystem::getSingleton();
        if(!nativeFiles)throw std::runtime_error("native save filesystem unavailable");
        const std::string nativeMarker=nativeFiles->writeFile("TheMercenarie/NativeGeneration.v1");
        if(nativeMarker.empty())throw std::runtime_error("native generation staging unavailable");
        SaveTransaction::write(nativeMarker,prepared->transaction.encode());
        pendingMissionSave=prepared;pendingNativeComplete=false;pendingNativeError.clear();
        pendingMissionSaveErrorReported=false;pendingMissionRetryAt=0;pendingMissionRetryCount=0;
        saveLog(op,"PREPARED");
        op.phase="native";op.component="kenshi";savePreparing=false;op.nativeCalled=true;
        int result;{MercenariePerf::Mode native(true);result=missionSaveOriginal(m,location,name);}
        if(result!=0&&result!=2){
            std::ostringstream detail;detail<<"native return="<<result;pendingNativeError=detail.str();
            SaveFileSystem* fs=SaveFileSystem::getSingleton();
            if(!fs||fs->state==SaveFileSystem::NORMAL)pendingNativeComplete=true;
            SaveDiagnostics::Failure f;f.reason=SaveDiagnostics::NativeFailed;f.detail=pendingNativeError;
            saveRemember(f);saveLog(op,"NATIVE_SAVE_FAILED");saveNotify(op,"persistence.lot1.native_failed");return result;
        }
        // Native copying is asynchronous. The sync hook confirms the matching
        // slot and retains errors before the engine clears them.
        saveLog(op,"NATIVE_SAVING");
        requestMercenarieDataSlot(location,name);progressVirginWorld=false;
        flushPendingMissionSave();return result;
    }catch(const std::exception& e){
        if(MercenarieCleanup::disabled)ErrorLog(std::string("[CLEANUP] FAILED phase=save ")+e.what());
        if(op.nativeCalled&&pendingMissionSave){pendingNativeError=e.what();SaveFileSystem* fs=SaveFileSystem::getSingleton();if(!fs||fs->state==SaveFileSystem::NORMAL)pendingNativeComplete=true;}
        try{op.remember(SaveDiagnostics::exceptionFailure(e));saveLog(op,"ROOT_ERROR",&op.first);saveLog(op,op.nativeSucceeded?"PUBLICATION_FAILED":op.nativeCalled?"NATIVE_SAVE_FAILED":"PREPARE_FAILED");saveNotify(op,op.nativeSucceeded?"persistence.lot1.publish_failed":op.nativeCalled?"persistence.lot1.native_failed":"persistence.lot1.prepare_failed");}catch(...){try{ErrorLog("MercenarieSave: failure while reporting C++ save exception");}catch(...){} }
        return -1;
    }catch(...){
        if(op.nativeCalled&&pendingMissionSave){pendingNativeError="unknown native C++ save exception";SaveFileSystem* fs=SaveFileSystem::getSingleton();if(!fs||fs->state==SaveFileSystem::NORMAL)pendingNativeComplete=true;}
        try{SaveDiagnostics::Failure f;f.detail="unknown C++ exception";op.remember(f);saveLog(op,"ROOT_ERROR",&op.first);saveLog(op,op.nativeSucceeded?"PUBLICATION_FAILED":op.nativeCalled?"NATIVE_SAVE_FAILED":"PREPARE_FAILED");saveNotify(op,op.nativeSucceeded?"persistence.lot1.publish_failed":op.nativeCalled?"persistence.lot1.native_failed":"persistence.lot1.prepare_failed");}catch(...){try{ErrorLog("MercenarieSave: unreportable C++ save exception");}catch(...){} }
        return -1;
    }
}
int (*missionLoadOriginal)(SaveManager*,const std::string&,const std::string&);
int missionLoadHook(SaveManager* m,const std::string& location,const std::string& name){
    MercenariePerf::Event perf("LOAD","preflight-and-native-call");
    // All disk checks precede reference/UI teardown. An interrupted generation
    // or damaged envelope must not replace the currently running world.
    std::string preparedPayload;bool disabled=false;
    try{
        flushPendingMissionSave();
        if(pendingMissionSave)throw std::runtime_error("save completion/publication still pending; load deferred");
        SaveTransaction::preflight(SaveTransaction::parent(SaveTransaction::parent(missionSidecarPath(location,name))));
        disabled=SaveTransaction::disabledSlot(SaveTransaction::parent(SaveTransaction::parent(missionSidecarPath(location,name))));
        const std::string bytes=disabled?std::string():readMissionFile(missionSidecarPath(location,name));
        if(!bytes.empty()){preparedPayload=openMissionArchive(bytes);validateMissionPayload(preparedPayload);}
    }catch(const std::exception& e){const SaveDiagnostics::Failure f=SaveDiagnostics::exceptionFailure(e);reportPersistenceError("LOAD",f.reason,SaveDiagnostics::describe(f),missionSidecarPath(location,name),"v8.literal.121");return -1;}
    hasWorldPersistenceRoot=false;worldPersistenceRoot=SaveDiagnostics::Failure();
    progressLoadFault=false;
    mercenarieFreshWorld=false;
    const std::string progressDir=missionSidecarPath(location,name);
    const std::string folder=progressDir.substr(0,progressDir.find_last_of("/\\"));
    if(!missionLoadOriginal){reportPersistenceError("LOAD",SaveDiagnostics::HookUnavailable,"native load hook unavailable",progressDir,"v8.literal.121");return -1;}
    releaseBountyWindow();bountyWorld=MercenarieV5::BountyWorldState();
    flushPendingMissionSave();
    progressVirginWorld=GuildSavePaths::uninitialized(folder);
    missionWorldChanging=true;missionRestorePending=false;missionRestoreBytes.clear();missionRestoreNotice=false;missionRestoreWaitLogged=false;missionRestoreClock=0;
    MissionAbsencePrototype::clearPersistence();
    artisanReset();releaseMissionUIBeforeLoad();discardMissionWorldReferences();
    resetMailItemIdentities();
    MercenarieCleanup::disabled=disabled;MercenarieCleanup::protectedSource.clear();
    if(disabled)clearMercenarieForCleanup();
    int result=-1;
    try{MercenariePerf::Mode native(true);result=missionLoadOriginal(m,location,name);}
    catch(const std::exception& e){missionWorldChanging=false;progressLoadFault=progressWriteBlocked=true;reportPersistenceException("LOAD",e,progressDir,"v8.literal.121");return -1;}
    catch(...){missionWorldChanging=false;progressLoadFault=progressWriteBlocked=true;reportPersistenceError("LOAD",SaveDiagnostics::NativeFailed,"unknown native C++ load exception",progressDir,"persistence.native_load_failed");return -1;}
    if(result==0||result==2){requestMercenarieDataSlot(location,name);if(disabled)progressWriteBlocked=false;}
    activeMercenarieSaveSlot.clear();
    try{
        if((result==0||result==2)&&!preparedPayload.empty()){
            missionRestoreBytes=preparedPayload;
            // The full payload was checked in isolated storage before native load.
            discardMissionWorldReferences();missionRestorePending=true;
        }
    }catch(const std::exception& e){progressLoadFault=true;progressWriteBlocked=true;ErrorLog(std::string("Guild Escort: invalid mission save: ")+e.what());discardMissionWorldReferences();reportPersistenceException("LOAD",e,progressDir,"v8.literal.121");}
    if(result!=0&&result!=2){progressLoadFault=progressWriteBlocked=true;std::ostringstream detail;detail<<"native load returned "<<result;reportPersistenceError("LOAD",SaveDiagnostics::NativeFailed,detail.str(),progressDir,"persistence.native_load_failed");}
    if(!MercenarieCleanup::disabled)scheduleGuildFurnitureRecovery();missionWorldChanging=false;std::stringstream loadResult;loadResult<<location<<"/"<<name<<" result="<<result;diagnosticTrace("MISSION native load returned",loadResult.str().c_str());
    return result;
}
void (*mercenarieNewGameOriginal)(SaveManager*,const std::string&);
void mercenarieNewGameHook(SaveManager* manager,const std::string& startId){
    flushPendingMissionSave();
    if(pendingMissionSave){reportPersistenceError("SAVE",SaveDiagnostics::PendingPublication,"save completion/publication pending; new game deferred",pendingMissionSave->path,"persistence.lot1.publish_failed");return;}
    MercenarieCleanup::disabled=false;MercenarieCleanup::protectedSource.clear();
    hasWorldPersistenceRoot=false;worldPersistenceRoot=SaveDiagnostics::Failure();
    progressLoadFault=false;
    progressWriteBlocked=false;rewardedContractIds.clear();
    flushPendingMissionSave();
    missionWorldChanging=true;missionRestorePending=false;missionRestoreBytes.clear();MissionAbsencePrototype::clearPersistence();
    releaseBountyWindow();bountyWorld=MercenarieV5::BountyWorldState();
    artisanReset();releaseMissionUIBeforeLoad();discardMissionWorldReferences();
    mercenarieExplicitSlot=false;mercenarieFreshWorld=true;progressVirginWorld=true;activeMercenarieSaveSlot.clear();
    fiscalLedger=FiscalLedger();delegatedMission.clear();delegatedMissions.clear();mailContracts.clear();successfulContracts=failedContracts=0;
    totalContractCats=totalAdvances=totalBonuses=totalTips=0;escortReputation=0;guildPoints=0;guildPrestige=0;guildInvestmentNextHour=0;
    contractHistory.clear();contractSeeds.clear();localReputations.clear();cityMemories.clear();archivedReputationAliases.clear();guildHouseNames.clear();guildHouseCities.clear();
    savedContractBoards.clear();contractBoardsLoaded=false;guildRerolls=ContractReroll::Charges();resetV9PendingActions();appliedGuildUnlockLevel=-1;
    configureMercenarieDataSlot();
    resetMailItemIdentities();mercenarieNewGameOriginal(manager,startId);
    missionWorldChanging=false;
}

