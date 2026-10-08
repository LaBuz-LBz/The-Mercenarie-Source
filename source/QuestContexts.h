#pragma once
// The legacy mission routines run against one selected context. Every switch
// saves that context first; finances and guild/world data are never swapped.
namespace {
struct EscortQuestContext {
    decltype(escort) v_escort;
    decltype(escortHandle) v_escortHandle;
    decltype(destination) v_destination;
    decltype(missionReward) v_missionReward;
    decltype(healthBonus) v_healthBonus;
    decltype(missionActive) v_missionActive;
    decltype(missionPending) v_missionPending;
    decltype(contractLifecycle) v_contractLifecycle;
    decltype(waitingForPlayer) v_waitingForPlayer;
    decltype(leavingBuilding) v_leavingBuilding;
    decltype(exitWaypoint) v_exitWaypoint;
    decltype(updateClock) v_updateClock;
    decltype(stationaryClock) v_stationaryClock;
    decltype(journeyStart) v_journeyStart;
    decltype(journeyDistanceSquared) v_journeyDistanceSquared;
    decltype(missionPaused) v_missionPaused;
    decltype(missionFollowing) v_missionFollowing;
    decltype(missionFollowTarget) v_missionFollowTarget;
    decltype(missionFollowers) v_missionFollowers;
    decltype(waitingHere) v_waitingHere;
    decltype(midpointChecked) v_midpointChecked;
    decltype(quarterSpeech) v_quarterSpeech;
    decltype(finalSpeech) v_finalSpeech;
    decltype(rareContract) v_rareContract;
    decltype(contractOriginFaction) v_contractOriginFaction;
    decltype(selectedDistance) v_selectedDistance;
    decltype(wasInCombat) v_wasInCombat;
    decltype(unconsciousSeconds) v_unconsciousSeconds;
    decltype(carriedByPlayerSeconds) v_carriedByPlayerSeconds;
    decltype(baseReward) v_baseReward;
    decltype(proposedReward) v_proposedReward;
    decltype(counterOffer) v_counterOffer;
    decltype(advancePaid) v_advancePaid;
    decltype(negotiatedAdvancePercent) v_negotiatedAdvancePercent;
    decltype(negotiationInsistence) v_negotiationInsistence;
    decltype(missionTierIndex) v_missionTierIndex;
    decltype(pendingGuildXp) v_pendingGuildXp;
    decltype(clientBudgetMultiplier) v_clientBudgetMultiplier;
    decltype(personalityAcceptance) v_personalityAcceptance;
    decltype(counterOfferActive) v_counterOfferActive;
    decltype(negotiationWasPaused) v_negotiationWasPaused;
    decltype(originCity) v_originCity;
    decltype(missionElapsed) v_missionElapsed;
    decltype(expectedTravelTime) v_expectedTravelTime;
    decltype(escortWasKnockedOut) v_escortWasKnockedOut;
    decltype(journeyCombatCount) v_journeyCombatCount;
    decltype(proximitySeconds) v_proximitySeconds;
    decltype(journeySeconds) v_journeySeconds;
    decltype(incidentClock) v_incidentClock;
    decltype(importantAlertClock) v_importantAlertClock;
    decltype(lastImportantAlert) v_lastImportantAlert;
    decltype(lastJourneyPosition) v_lastJourneyPosition;
    decltype(currentContract) v_currentContract;
    decltype(journeyData) v_journeyData;
    decltype(earnedFinalBonus) v_earnedFinalBonus;
    decltype(earnedFinalBonusCount) v_earnedFinalBonusCount;
    decltype(finalClientTip) v_finalClientTip;
    decltype(requestedFinalBonusPercent) v_requestedFinalBonusPercent;
    decltype(finalHealthy) v_finalHealthy;
    decltype(finalFast) v_finalFast;
    decltype(boardCaravan) v_boardCaravan;
    decltype(boardScientific) v_boardScientific;
    decltype(caravanMission) v_caravanMission;
    decltype(caravanReturning) v_caravanReturning;
    decltype(caravanOrigin) v_caravanOrigin;
    decltype(caravanMembers) v_caravanMembers;
    decltype(caravanCargoType) v_caravanCargoType;
    decltype(caravanCargoValue) v_caravanCargoValue;
    decltype(caravanInitialMembers) v_caravanInitialMembers;
    decltype(travelIncidentTriggered) v_travelIncidentTriggered;
    decltype(scientificMission) v_scientificMission;
    decltype(scientificResearching) v_scientificResearching;
    decltype(scientificReturning) v_scientificReturning;
    decltype(scientificEntryAttempted) v_scientificEntryAttempted;
    decltype(scientificInsideDiscovery) v_scientificInsideDiscovery;
    decltype(scientificWillEnter) v_scientificWillEnter;
    decltype(scientificEntryResolved) v_scientificEntryResolved;
    decltype(scientificResearchSeconds) v_scientificResearchSeconds;
    decltype(scientificMoveClock) v_scientificMoveClock;
    decltype(scientificCommentClock) v_scientificCommentClock;
    decltype(scientificEntryClock) v_scientificEntryClock;
    decltype(scientificOrigin) v_scientificOrigin;
    decltype(scientificRuinCenter) v_scientificRuinCenter;
    decltype(scientificMembers) v_scientificMembers;
    decltype(currentMissionFiscalId) v_currentMissionFiscalId;
    decltype(visitorOfferUrgent) v_visitorOfferUrgent;
    decltype(visitorOfferVip) v_visitorOfferVip;
    decltype(visitorOfferExceptional) v_visitorOfferExceptional;
    decltype(negotiationSuspended) v_negotiationSuspended;
    decltype(contractBarmanHandle) v_contractBarmanHandle;
    decltype(contractOriginTown) v_contractOriginTown;
    decltype(completedCleanupClock) v_completedCleanupClock;
    decltype(completedEscort) v_completedEscort;
    decltype(completedCaravan) v_completedCaravan;
    decltype(selectedProfileSquad) v_selectedProfileSquad;
    decltype(selectedProfileName) v_selectedProfileName;
    decltype(selectedProfileGroupSize) v_selectedProfileGroupSize;
    decltype(selectedProfileRarity) v_selectedProfileRarity;
    decltype(destinationNameStorage) v_destinationNameStorage;
    decltype(reportStartHour) v_reportStartHour;
    decltype(reportEndHour) v_reportEndHour;
    decltype(progressMembers) v_progressMembers;
    decltype(progressRosterKnown) v_progressRosterKnown;
    decltype(progressAnyClientKo) v_progressAnyClientKo;
    decltype(progressReportLocal) v_progressReportLocal;
    decltype(progressReportGlobal) v_progressReportGlobal;
    decltype(negotiationOpen) v_negotiationOpen;
    decltype(progressDeadMembers) v_progressDeadMembers;
    decltype(finalBonusChoice) v_finalBonusChoice;
    decltype(testRoute) v_testRoute;
    decltype(missionGroup) v_missionGroup;
    decltype(missionRescue) v_missionRescue;
    decltype(missionPace) v_missionPace;
    decltype(missionForcedPace) v_missionForcedPace;
    decltype(missionCasualtyWaiting) v_missionCasualtyWaiting;
    decltype(missionTemporaryLeader) v_missionTemporaryLeader;
    decltype(missionSeparation) v_missionSeparation;
    decltype(carriedDestinationDistance) v_carriedDestinationDistance;

    decltype(contractBarman) v_contractBarman;
    SavedActorResolution::Actor v_completedEscortRestore;
    SavedActorResolution::Group v_completedCaravanRestore,v_caravanRestore,v_scientificRestore;
    std::string town;
    explicit EscortQuestContext(bool captureLive=true):v_escort(),v_escortHandle(),v_destination(),v_missionReward(),v_healthBonus(),v_missionActive(),v_missionPending(),v_contractLifecycle(),v_waitingForPlayer(),v_leavingBuilding(),v_exitWaypoint(),v_updateClock(),v_stationaryClock(),v_journeyStart(),v_journeyDistanceSquared(),v_missionPaused(),v_missionFollowing(),v_missionFollowTarget(),v_missionFollowers(),v_waitingHere(),v_midpointChecked(),v_quarterSpeech(),v_finalSpeech(),v_rareContract(),v_contractOriginFaction(),v_selectedDistance(),v_wasInCombat(),v_unconsciousSeconds(),v_carriedByPlayerSeconds(),v_baseReward(),v_proposedReward(),v_counterOffer(),v_advancePaid(),v_negotiatedAdvancePercent(),v_negotiationInsistence(),v_missionTierIndex(),v_pendingGuildXp(),v_clientBudgetMultiplier(),v_personalityAcceptance(),v_counterOfferActive(),v_negotiationWasPaused(),v_originCity(),v_missionElapsed(),v_expectedTravelTime(),v_escortWasKnockedOut(),v_journeyCombatCount(),v_proximitySeconds(),v_journeySeconds(),v_incidentClock(),v_importantAlertClock(),v_lastImportantAlert(),v_lastJourneyPosition(),v_currentContract(),v_journeyData(),v_earnedFinalBonus(),v_earnedFinalBonusCount(),v_finalClientTip(),v_requestedFinalBonusPercent(),v_finalHealthy(),v_finalFast(),v_boardCaravan(),v_boardScientific(),v_caravanMission(),v_caravanReturning(),v_caravanOrigin(),v_caravanMembers(),v_caravanCargoType(),v_caravanCargoValue(),v_caravanInitialMembers(),v_travelIncidentTriggered(),v_scientificMission(),v_scientificResearching(),v_scientificReturning(),v_scientificEntryAttempted(),v_scientificInsideDiscovery(),v_scientificWillEnter(),v_scientificEntryResolved(),v_scientificResearchSeconds(),v_scientificMoveClock(),v_scientificCommentClock(),v_scientificEntryClock(),v_scientificOrigin(),v_scientificRuinCenter(),v_scientificMembers(),v_currentMissionFiscalId(),v_visitorOfferUrgent(),v_visitorOfferVip(),v_visitorOfferExceptional(),v_negotiationSuspended(),v_contractBarmanHandle(),v_contractOriginTown(),v_completedCleanupClock(),v_completedEscort(),v_completedCaravan(),v_selectedProfileSquad(),v_selectedProfileName(),v_selectedProfileGroupSize(),v_selectedProfileRarity(),v_destinationNameStorage(),v_reportStartHour(),v_reportEndHour(),v_progressMembers(),v_progressRosterKnown(),v_progressAnyClientKo(),v_progressReportLocal(),v_progressReportGlobal(),v_negotiationOpen(),v_progressDeadMembers(),v_finalBonusChoice(),v_testRoute(),v_missionGroup(),v_missionRescue(),v_missionPace(),v_missionForcedPace(),v_missionCasualtyWaiting(),v_missionTemporaryLeader(),v_missionSeparation(),v_carriedDestinationDistance(),v_contractBarman(){if(captureLive)capture();}
    void capture(){
        v_escort=escort;
        v_escortHandle=escortHandle;
        v_destination=destination;
        v_missionReward=missionReward;
        v_healthBonus=healthBonus;
        v_missionActive=missionActive;
        v_missionPending=missionPending;
        v_contractLifecycle=contractLifecycle;
        v_waitingForPlayer=waitingForPlayer;
        v_leavingBuilding=leavingBuilding;
        v_exitWaypoint=exitWaypoint;
        v_updateClock=updateClock;
        v_stationaryClock=stationaryClock;
        v_journeyStart=journeyStart;
        v_journeyDistanceSquared=journeyDistanceSquared;
        v_missionPaused=missionPaused;
        v_missionFollowing=missionFollowing;
        v_missionFollowTarget=missionFollowTarget;
        v_missionFollowers=missionFollowers;
        v_waitingHere=waitingHere;
        v_midpointChecked=midpointChecked;
        v_quarterSpeech=quarterSpeech;
        v_finalSpeech=finalSpeech;
        v_rareContract=rareContract;
        v_contractOriginFaction=contractOriginFaction;
        v_selectedDistance=selectedDistance;
        v_wasInCombat=wasInCombat;
        v_unconsciousSeconds=unconsciousSeconds;
        v_carriedByPlayerSeconds=carriedByPlayerSeconds;
        v_baseReward=baseReward;
        v_proposedReward=proposedReward;
        v_counterOffer=counterOffer;
        v_advancePaid=advancePaid;
        v_negotiatedAdvancePercent=negotiatedAdvancePercent;
        v_negotiationInsistence=negotiationInsistence;
        v_missionTierIndex=missionTierIndex;
        v_pendingGuildXp=pendingGuildXp;
        v_clientBudgetMultiplier=clientBudgetMultiplier;
        v_personalityAcceptance=personalityAcceptance;
        v_counterOfferActive=counterOfferActive;
        v_negotiationWasPaused=negotiationWasPaused;
        v_originCity=originCity;
        v_missionElapsed=missionElapsed;
        v_expectedTravelTime=expectedTravelTime;
        v_escortWasKnockedOut=escortWasKnockedOut;
        v_journeyCombatCount=journeyCombatCount;
        v_proximitySeconds=proximitySeconds;
        v_journeySeconds=journeySeconds;
        v_incidentClock=incidentClock;
        v_importantAlertClock=importantAlertClock;
        v_lastImportantAlert=lastImportantAlert;
        v_lastJourneyPosition=lastJourneyPosition;
        v_currentContract=currentContract;
        v_journeyData=journeyData;
        v_earnedFinalBonus=earnedFinalBonus;
        v_earnedFinalBonusCount=earnedFinalBonusCount;
        v_finalClientTip=finalClientTip;
        v_requestedFinalBonusPercent=requestedFinalBonusPercent;
        v_finalHealthy=finalHealthy;
        v_finalFast=finalFast;
        v_boardCaravan=boardCaravan;
        v_boardScientific=boardScientific;
        v_caravanMission=caravanMission;
        v_caravanReturning=caravanReturning;
        v_caravanOrigin=caravanOrigin;
        v_caravanMembers=caravanMembers;
        v_caravanCargoType=caravanCargoType;
        v_caravanCargoValue=caravanCargoValue;
        v_caravanInitialMembers=caravanInitialMembers;
        v_travelIncidentTriggered=travelIncidentTriggered;
        v_scientificMission=scientificMission;
        v_scientificResearching=scientificResearching;
        v_scientificReturning=scientificReturning;
        v_scientificEntryAttempted=scientificEntryAttempted;
        v_scientificInsideDiscovery=scientificInsideDiscovery;
        v_scientificWillEnter=scientificWillEnter;
        v_scientificEntryResolved=scientificEntryResolved;
        v_scientificResearchSeconds=scientificResearchSeconds;
        v_scientificMoveClock=scientificMoveClock;
        v_scientificCommentClock=scientificCommentClock;
        v_scientificEntryClock=scientificEntryClock;
        v_scientificOrigin=scientificOrigin;
        v_scientificRuinCenter=scientificRuinCenter;
        v_scientificMembers=scientificMembers;
        v_currentMissionFiscalId=currentMissionFiscalId;
        v_visitorOfferUrgent=visitorOfferUrgent;
        v_visitorOfferVip=visitorOfferVip;
        v_visitorOfferExceptional=visitorOfferExceptional;
        v_negotiationSuspended=negotiationSuspended;
        v_contractBarmanHandle=contractBarmanHandle;
        v_contractOriginTown=contractOriginTown;
        v_completedCleanupClock=completedCleanupClock;
        v_completedEscort=completedEscort;v_completedEscortRestore=completedEscortRestore;v_completedCaravanRestore=completedCaravanRestore;v_caravanRestore=caravanRestore;v_scientificRestore=scientificRestore;
        v_completedCaravan=completedCaravan;
        v_selectedProfileSquad=selectedProfileSquad;
        v_selectedProfileName=selectedProfileName;
        v_selectedProfileGroupSize=selectedProfileGroupSize;
        v_selectedProfileRarity=selectedProfileRarity;
        v_destinationNameStorage=destinationNameStorage;
        v_reportStartHour=reportStartHour;
        v_reportEndHour=reportEndHour;
        v_progressMembers=progressMembers;
        v_progressRosterKnown=progressRosterKnown;
        v_progressAnyClientKo=progressAnyClientKo;
        v_progressReportLocal=progressReportLocal;
        v_progressReportGlobal=progressReportGlobal;
        v_negotiationOpen=negotiationOpen;
        v_progressDeadMembers=progressDeadMembers;
        v_finalBonusChoice=finalBonusChoice;
        v_testRoute=testRoute;
        v_missionGroup=missionGroup;
        v_missionRescue=missionRescue;
        v_missionPace=missionPace;
        v_missionForcedPace=missionForcedPace;
        v_missionCasualtyWaiting=missionCasualtyWaiting;
        v_missionTemporaryLeader=missionTemporaryLeader;
        v_missionSeparation=missionSeparation;
        v_carriedDestinationDistance=carriedDestinationDistance;

        v_contractBarman=contractBarman;
        town=destinationTown?destinationTown:"";
    }
    void restore() const;
    bool occupied() const {return v_missionActive||v_missionPending;}
    bool reusable() const {return !occupied()&&!v_completedEscortRestore.pending&&!v_completedCaravanRestore.pending;}
    template<class A> void archive(A& a){
        const bool required=a.requireActors;a.requireActors=false;
        a.field(v_escort);
        a.field(v_escortHandle);
        a.field(v_destination);
        a.field(v_missionReward);
        a.field(v_healthBonus);
        a.field(v_missionActive);
        a.field(v_missionPending);
        a.field(v_contractLifecycle);
        a.field(v_waitingForPlayer);
        a.field(v_leavingBuilding);
        a.field(v_exitWaypoint);
        a.field(v_updateClock);
        a.field(v_stationaryClock);
        a.field(v_journeyStart);
        a.field(v_journeyDistanceSquared);
        a.field(v_missionPaused);
        a.field(v_missionFollowing);
        a.field(v_missionFollowTarget);
        a.field(v_missionFollowers);
        a.field(v_waitingHere);
        a.field(v_midpointChecked);
        a.field(v_quarterSpeech);
        a.field(v_finalSpeech);
        a.field(v_rareContract);
        a.field(v_contractOriginFaction);
        a.field(v_selectedDistance);
        a.field(v_wasInCombat);
        a.field(v_unconsciousSeconds);
        a.field(v_carriedByPlayerSeconds);
        a.field(v_baseReward);
        a.field(v_proposedReward);
        a.field(v_counterOffer);
        a.field(v_advancePaid);
        a.field(v_negotiatedAdvancePercent);
        a.field(v_negotiationInsistence);
        a.field(v_missionTierIndex);
        a.field(v_pendingGuildXp);
        a.field(v_clientBudgetMultiplier);
        a.field(v_personalityAcceptance);
        a.field(v_counterOfferActive);
        a.field(v_negotiationWasPaused);
        a.field(v_originCity);
        a.field(v_missionElapsed);
        a.field(v_expectedTravelTime);
        a.field(v_escortWasKnockedOut);
        a.field(v_journeyCombatCount);
        a.field(v_proximitySeconds);
        a.field(v_journeySeconds);
        a.field(v_incidentClock);
        a.field(v_importantAlertClock);
        a.field(v_lastImportantAlert);
        a.field(v_lastJourneyPosition);
        a.field(v_currentContract);
        a.field(v_journeyData);
        a.field(v_earnedFinalBonus);
        a.field(v_earnedFinalBonusCount);
        a.field(v_finalClientTip);
        a.field(v_requestedFinalBonusPercent);
        a.field(v_finalHealthy);
        a.field(v_finalFast);
        a.field(v_boardCaravan);
        a.field(v_boardScientific);
        a.field(v_caravanMission);
        a.field(v_caravanReturning);
        a.field(v_caravanOrigin);
        SavedActorResolution::archive(a,v_caravanMembers,v_caravanRestore);
        a.field(v_caravanCargoType);
        a.field(v_caravanCargoValue);
        a.field(v_caravanInitialMembers);
        a.field(v_travelIncidentTriggered);
        a.field(v_scientificMission);
        a.field(v_scientificResearching);
        a.field(v_scientificReturning);
        a.field(v_scientificEntryAttempted);
        a.field(v_scientificInsideDiscovery);
        a.field(v_scientificWillEnter);
        a.field(v_scientificEntryResolved);
        a.field(v_scientificResearchSeconds);
        a.field(v_scientificMoveClock);
        a.field(v_scientificCommentClock);
        a.field(v_scientificEntryClock);
        a.field(v_scientificOrigin);
        a.field(v_scientificRuinCenter);
        SavedActorResolution::archive(a,v_scientificMembers,v_scientificRestore);
        a.field(v_currentMissionFiscalId);
        a.field(v_visitorOfferUrgent);
        a.field(v_visitorOfferVip);
        a.field(v_visitorOfferExceptional);
        a.field(v_negotiationSuspended);
        a.field(v_contractBarmanHandle);
        a.field(v_contractOriginTown);
        a.field(v_completedCleanupClock);
        SavedActorResolution::archive(a,v_completedEscort,v_completedEscortRestore);
        SavedActorResolution::archive(a,v_completedCaravan,v_completedCaravanRestore);
        a.field(v_selectedProfileSquad);
        a.field(v_selectedProfileName);
        a.field(v_selectedProfileGroupSize);
        a.field(v_selectedProfileRarity);
        a.field(v_destinationNameStorage);
        a.field(v_reportStartHour);
        a.field(v_reportEndHour);
        a.field(v_progressMembers);
        a.field(v_progressRosterKnown);
        a.field(v_progressAnyClientKo);
        a.field(v_progressReportLocal);
        a.field(v_progressReportGlobal);
        a.field(v_negotiationOpen);
        a.field(town);
        a.requireActors=required;
        unsigned int n=(unsigned int)v_progressDeadMembers.size();a.field(n);
        if(n>4096)throw std::runtime_error("invalid dead roster");
        if(a.reading){v_progressDeadMembers.clear();for(unsigned int i=0;i<n;++i){unsigned int id=0;a.field(id);v_progressDeadMembers.insert(id);}}
        else for(std::set<unsigned int>::const_iterator i=v_progressDeadMembers.begin();i!=v_progressDeadMembers.end();++i){unsigned int id=*i;a.field(id);}
        MissionBonuses::archive(a,v_finalBonusChoice);
        std::string route;if(!a.reading&&v_testRoute.status!=RoutePrototype::Idle){route=v_testRoute.save();RoutePrototype::Route::load(route,v_currentMissionFiscalId);}a.field(route);
        if(a.reading){v_testRoute.cancel();if(!route.empty())v_testRoute=RoutePrototype::Route::load(route,v_currentMissionFiscalId);v_missionGroup.reset();v_contractBarman=0;v_negotiationOpen=false;if(v_missionPending){v_negotiationSuspended=true;v_contractLifecycle=CONTRACT_CLIENT_MEETING;}}
        if(v_missionReward<0||v_advancePaid<0||v_contractLifecycle<CONTRACT_NONE||v_contractLifecycle>CONTRACT_FAILED||v_currentContract.type<MCT_ESCORT||v_currentContract.type>MCT_SCIENCE||v_currentContract.rarity<MCR_COMMON||v_currentContract.rarity>MCR_LEGENDARY||v_reportStartHour!=v_reportStartHour||v_reportEndHour!=v_reportEndHour||v_reportStartHour< -1||v_reportEndHour< -1)throw std::runtime_error("invalid quest context");
    }
};
std::string selectedQuestTown;
void EscortQuestContext::restore() const {
    escort=v_escort;
    escortHandle=v_escortHandle;
    destination=v_destination;
    missionReward=v_missionReward;
    healthBonus=v_healthBonus;
    missionActive=v_missionActive;
    missionPending=v_missionPending;
    contractLifecycle=v_contractLifecycle;
    waitingForPlayer=v_waitingForPlayer;
    leavingBuilding=v_leavingBuilding;
    exitWaypoint=v_exitWaypoint;
    updateClock=v_updateClock;
    stationaryClock=v_stationaryClock;
    journeyStart=v_journeyStart;
    journeyDistanceSquared=v_journeyDistanceSquared;
    missionPaused=v_missionPaused;
    missionFollowing=v_missionFollowing;
    missionFollowTarget=v_missionFollowTarget;
    missionFollowers=v_missionFollowers;
    waitingHere=v_waitingHere;
    midpointChecked=v_midpointChecked;
    quarterSpeech=v_quarterSpeech;
    finalSpeech=v_finalSpeech;
    rareContract=v_rareContract;
    contractOriginFaction=v_contractOriginFaction;
    selectedDistance=v_selectedDistance;
    wasInCombat=v_wasInCombat;
    unconsciousSeconds=v_unconsciousSeconds;
    carriedByPlayerSeconds=v_carriedByPlayerSeconds;
    baseReward=v_baseReward;
    proposedReward=v_proposedReward;
    counterOffer=v_counterOffer;
    advancePaid=v_advancePaid;
    negotiatedAdvancePercent=v_negotiatedAdvancePercent;
    negotiationInsistence=v_negotiationInsistence;
    missionTierIndex=v_missionTierIndex;
    pendingGuildXp=v_pendingGuildXp;
    clientBudgetMultiplier=v_clientBudgetMultiplier;
    personalityAcceptance=v_personalityAcceptance;
    counterOfferActive=v_counterOfferActive;
    negotiationWasPaused=v_negotiationWasPaused;
    originCity=v_originCity;
    missionElapsed=v_missionElapsed;
    expectedTravelTime=v_expectedTravelTime;
    escortWasKnockedOut=v_escortWasKnockedOut;
    journeyCombatCount=v_journeyCombatCount;
    proximitySeconds=v_proximitySeconds;
    journeySeconds=v_journeySeconds;
    incidentClock=v_incidentClock;
    importantAlertClock=v_importantAlertClock;
    lastImportantAlert=v_lastImportantAlert;
    lastJourneyPosition=v_lastJourneyPosition;
    currentContract=v_currentContract;
    journeyData=v_journeyData;
    earnedFinalBonus=v_earnedFinalBonus;
    earnedFinalBonusCount=v_earnedFinalBonusCount;
    finalClientTip=v_finalClientTip;
    requestedFinalBonusPercent=v_requestedFinalBonusPercent;
    finalHealthy=v_finalHealthy;
    finalFast=v_finalFast;
    boardCaravan=v_boardCaravan;
    boardScientific=v_boardScientific;
    caravanMission=v_caravanMission;
    caravanReturning=v_caravanReturning;
    caravanOrigin=v_caravanOrigin;
    caravanMembers=v_caravanMembers;
    caravanCargoType=v_caravanCargoType;
    caravanCargoValue=v_caravanCargoValue;
    caravanInitialMembers=v_caravanInitialMembers;
    travelIncidentTriggered=v_travelIncidentTriggered;
    scientificMission=v_scientificMission;
    scientificResearching=v_scientificResearching;
    scientificReturning=v_scientificReturning;
    scientificEntryAttempted=v_scientificEntryAttempted;
    scientificInsideDiscovery=v_scientificInsideDiscovery;
    scientificWillEnter=v_scientificWillEnter;
    scientificEntryResolved=v_scientificEntryResolved;
    scientificResearchSeconds=v_scientificResearchSeconds;
    scientificMoveClock=v_scientificMoveClock;
    scientificCommentClock=v_scientificCommentClock;
    scientificEntryClock=v_scientificEntryClock;
    scientificOrigin=v_scientificOrigin;
    scientificRuinCenter=v_scientificRuinCenter;
    scientificMembers=v_scientificMembers;
    currentMissionFiscalId=v_currentMissionFiscalId;
    visitorOfferUrgent=v_visitorOfferUrgent;
    visitorOfferVip=v_visitorOfferVip;
    visitorOfferExceptional=v_visitorOfferExceptional;
    negotiationSuspended=v_negotiationSuspended;
    contractBarmanHandle=v_contractBarmanHandle;
    contractOriginTown=v_contractOriginTown;
    completedCleanupClock=v_completedCleanupClock;
    completedEscort=v_completedEscort;completedEscortRestore=v_completedEscortRestore;completedCaravanRestore=v_completedCaravanRestore;caravanRestore=v_caravanRestore;scientificRestore=v_scientificRestore;
    completedCaravan=v_completedCaravan;
    selectedProfileSquad=v_selectedProfileSquad;
    selectedProfileName=v_selectedProfileName;
    selectedProfileGroupSize=v_selectedProfileGroupSize;
    selectedProfileRarity=v_selectedProfileRarity;
    destinationNameStorage=v_destinationNameStorage;
    reportStartHour=v_reportStartHour;
    reportEndHour=v_reportEndHour;
    progressMembers=v_progressMembers;
    progressRosterKnown=v_progressRosterKnown;
    progressAnyClientKo=v_progressAnyClientKo;
    progressReportLocal=v_progressReportLocal;
    progressReportGlobal=v_progressReportGlobal;
    negotiationOpen=v_negotiationOpen;
    progressDeadMembers=v_progressDeadMembers;
    finalBonusChoice=v_finalBonusChoice;
    testRoute=v_testRoute;
    missionGroup=v_missionGroup;
    missionRescue=v_missionRescue;
    missionPace=v_missionPace;
    missionForcedPace=v_missionForcedPace;
    missionCasualtyWaiting=v_missionCasualtyWaiting;
    missionTemporaryLeader=v_missionTemporaryLeader;
    missionSeparation=v_missionSeparation;
    carriedDestinationDistance=v_carriedDestinationDistance;

    contractBarman=v_contractBarman;
    selectedQuestTown=town;destinationTown=selectedQuestTown.c_str();destinationName=destinationNameStorage.c_str();
    refreshQuestActors();
}
const int maximumActiveQuests=5;
const EscortQuestContext emptyEscortQuest;
EscortQuestContext escortQuests[maximumActiveQuests];
MercenarieV5::BountyWorldState bountyQuests[maximumActiveQuests];
int selectedEscortQuest=0,selectedBountyQuest=0;
bool questContextIteration=false;
unsigned long questFiscalSequence=0;
struct RetiredQuestGroup {std::vector<hand> members;float seconds;std::set<std::string> aiReleased;std::map<std::string,int> observedGoals;};
std::vector<RetiredQuestGroup> retiredQuestGroups;
bool questOrdersNeedRestore[5]={false};
float questActorRetry[5]={0};
void refreshQuestActors(){
    escort=escortHandle.isNull()?0:escortHandle.getCharacter();contractBarman=contractBarmanHandle.isNull()?0:contractBarmanHandle.getCharacter();
    if(caravanRestore.pending||scientificRestore.pending)return;
    if((missionActive||missionPending)&&!progressRosterKnown&&progressMembers.empty()&&escort){const std::vector<Character*>& group=caravanMission?caravanMembers:scientificMembers;if(group.empty())progressMembers.push_back(escort->getHandle());else for(size_t i=0;i<group.size();++i)if(group[i])progressMembers.push_back(group[i]->getHandle());progressRosterKnown=progressMembers.size()>=(size_t)std::max(1,currentContract.groupSize);}
    if(missionActive||missionPending){caravanMembers.clear();scientificMembers.clear();for(size_t i=0;i<progressMembers.size();++i){Character* c=progressMembers[i].getCharacter();if(!c||(c!=escort&&c->isDead()))continue;if(caravanMission)caravanMembers.push_back(c);if(scientificMission)scientificMembers.push_back(c);}}
}
void queueQuestCleanup(){RetiredQuestGroup r;r.seconds=completedCleanupClock;r.members=progressMembers;if(r.members.empty())for(size_t i=0;i<completedCaravan.size();++i)if(completedCaravan[i])r.members.push_back(completedCaravan[i]->getHandle());if(!r.members.empty())retiredQuestGroups.push_back(r);completedEscort=0;completedCaravan.clear();}
std::string allocateQuestFiscalId(){
    for(;;){std::ostringstream s;s<<"QUEST-"<<std::fixed<<currentGameHours<<"-"<<fiscalLedger.nextId<<"-"<<++questFiscalSequence;std::string id=s.str();bool used=rewardedContractIds.count(id)||fiscalLedger.contains(id)||currentMissionFiscalId==id;
        for(int i=0;i<maximumActiveQuests;++i)if(escortQuests[i].v_currentMissionFiscalId==id)used=true;if(!used)return id;}
}
const char* questEscortFactionId(){static const char* ids[]={"880050-Guild Escort Contracts.mod","882051-Guild Escort Contracts.mod","882052-Guild Escort Contracts.mod","882053-Guild Escort Contracts.mod","882054-Guild Escort Contracts.mod"};return ids[selectedEscortQuest];}
int bountyQuestSlot(){return selectedBountyQuest;}
void selectEscortQuest(int slot){if(slot<0||slot>=maximumActiveQuests||slot==selectedEscortQuest)return;escortQuests[selectedEscortQuest].capture();selectedEscortQuest=slot;escortQuests[slot].restore();}
void captureBountyQuest(){std::map<std::string,MercenarieV5::BountyBoard> boards;boards.swap(bountyWorld.boards);bountyQuests[selectedBountyQuest]=bountyWorld;boards.swap(bountyWorld.boards);}
void selectBountyQuest(int slot){
    if(slot<0||slot>=maximumActiveQuests||slot==selectedBountyQuest)return;
    // Boards are world data: preserve their nodes without copying the entire
    // board catalogue at every mission tick or tracker refresh.
    std::map<std::string,MercenarieV5::BountyBoard> boards;boards.swap(bountyWorld.boards);
    bountyQuests[selectedBountyQuest]=bountyWorld;selectedBountyQuest=slot;bountyWorld=bountyQuests[slot];boards.swap(bountyWorld.boards);
}
int activeQuestCount(){int n=0;for(int i=0;i<maximumActiveQuests;++i){if(i==selectedEscortQuest?(missionActive||missionPending):escortQuests[i].occupied())++n;if(i==selectedBountyQuest?bountyWorld.contract.occupied():bountyQuests[i].contract.occupied())++n;}return n;}
bool questCapacityAvailable(){if(activeQuestCount()<maximumActiveQuests)return true;if(ou)ou->showPlayerAMessage(Loc::text("quest.capacity.full"),true);return false;}
bool questPanelLocked(){return (finalWindow&&finalWindow->getVisible())||(negotiationWindow&&negotiationWindow->getVisible())||(contractDecisionWindow&&contractDecisionWindow->getVisible())||(contractsWindow&&contractsWindow->getVisible())||(bountyWindow&&bountyWindow->getVisible());}
bool prepareEscortQuest(){
    if(questPanelLocked()||!questCapacityAvailable())return false;
    escortQuests[selectedEscortQuest].capture();
    bool urgent=visitorOfferUrgent,vip=visitorOfferVip,exceptional=visitorOfferExceptional;
    for(int i=0;i<maximumActiveQuests;++i)if(escortQuests[i].reusable()){
        selectEscortQuest(i);
        if(!completedCaravan.empty()){RetiredQuestGroup retired;retired.seconds=completedCleanupClock;for(size_t j=0;j<completedCaravan.size();++j)if(completedCaravan[j])retired.members.push_back(completedCaravan[j]->getHandle());retiredQuestGroups.push_back(retired);}
        emptyEscortQuest.restore();visitorOfferUrgent=urgent;visitorOfferVip=vip;visitorOfferExceptional=exceptional;return true;}
    return false;
}
bool prepareBountyQuest(){
    if(questPanelLocked()||!questCapacityAvailable())return false;
    captureBountyQuest();
    for(int i=0;i<maximumActiveQuests;++i)if(!bountyQuests[i].contract.occupied()&&!bountyQuests[i].suspended){selectBountyQuest(i);return true;}return false;
}
void selectQuestActor(Character* actor){
    MercenariePerf::Phase perf("eligibility-quest-actor");
    if(!actor||questContextIteration||questPanelLocked())return;
    for(int i=0;i<maximumActiveQuests;++i){
        const hand& h=i==selectedEscortQuest?escortHandle:escortQuests[i].v_escortHandle;if(!h.isNull()&&h==actor->getHandle()){selectEscortQuest(i);return;}
        const std::vector<hand>& members=i==selectedEscortQuest?progressMembers:escortQuests[i].v_progressMembers;
        for(size_t member=0;member<members.size();++member)if(!members[member].isNull()&&members[member]==actor->getHandle()){selectEscortQuest(i);return;}
    }
    if(!ou||!ou->player)return;
    for(int i=0;i<maximumActiveQuests;++i){const MercenarieV5::BountyContract& c=i==selectedBountyQuest?bountyWorld.contract:bountyQuests[i].contract;
        if(!c.occupied()||!(c.offer.issuer==MercenarieV5::bountyIdentity(actor->getHandle())))continue;
        for(size_t j=0;j<ou->player->playerCharacters.size();++j)if(MercenarieV5::playerCarriesBounty(c,ou->player->playerCharacters[j],ou->player->getFaction())){selectBountyQuest(i);return;}
    }
}
void resetQuestContexts(){selectedEscortQuest=selectedBountyQuest=0;questFiscalSequence=0;retiredQuestGroups.clear();for(int i=0;i<maximumActiveQuests;++i){escortQuests[i]=emptyEscortQuest;bountyQuests[i]=MercenarieV5::BountyWorldState();questOrdersNeedRestore[i]=false;questActorRetry[i]=0;}questContextIteration=false;}
void appendAllQuestItems(){
    int e=selectedEscortQuest,b=selectedBountyQuest;
    for(int i=0;i<maximumActiveQuests;++i){selectEscortQuest(i);size_t first=questTrackerItems.size();appendEscortTrackerItem();for(size_t j=first;j<questTrackerItems.size();++j)questTrackerItems[j].slot=i;}
    selectEscortQuest(e);
    for(int i=0;i<maximumActiveQuests;++i){selectBountyQuest(i);size_t first=questTrackerItems.size();appendBountyTrackerItem();for(size_t j=first;j<questTrackerItems.size();++j){questTrackerItems[j].slot=i;questTrackerItems[j].bounty=true;}}
    selectBountyQuest(b);
}
void selectTrackerQuest(int slot,bool bounty){if(bounty)selectBountyQuest(slot);else selectEscortQuest(slot);}
bool isAnyQuestBountyPlatoon(Platoon* platoon,bool campOnly){
    if(!platoon)return false;MercenarieV5::ActorIdentity leader=MercenarieV5::bountyIdentity(platoon->getSquadLeader_theRealOne());
    for(int i=0;i<maximumActiveQuests;++i){const MercenarieV5::BountyWorldState& w=i==selectedBountyQuest?bountyWorld:bountyQuests[i];if(!w.suspended&&w.contract.occupied()&&w.contract.target.valid()&&w.contract.target==leader&&(!campOnly||w.encounter==1))return true;}return false;
}
void observeQuestPrison(Character* person){
    if(!person)return;MercenarieV5::ActorIdentity id=MercenarieV5::bountyIdentity(person->getHandle());
    for(int i=0;i<maximumActiveQuests;++i){MercenarieV5::BountyWorldState& w=i==selectedBountyQuest?bountyWorld:bountyQuests[i];if(w.contract.state==MercenarieV5::BountyActive&&w.contract.target==id){w.contract.observe(true,false,true);ou->showPlayerAMessage(MercenarieV5::bountyText(MercenarieV5::CageCancelled,!gMercenarieEnglish),true);return;}}
}
void updateAllQuestBountyMaps(MapScreen* map){int b=selectedBountyQuest;for(int i=0;i<maximumActiveQuests;++i){selectBountyQuest(i);updateBountyCampMap(map);}selectBountyQuest(b);}
void tickEscortMission(float dt);
void tickAllQuests(float dt){
    if(developerFinishPicker&&developerFinishPicker->getVisible())return;
    if(questPanelLocked())return;
    for(size_t i=0;i<retiredQuestGroups.size();){RetiredQuestGroup& r=retiredQuestGroups[i];r.seconds-=dt;for(size_t j=0;j<r.members.size();++j)observeRetiredMissionActor(r.members[j].getCharacter(),r.aiReleased,r.observedGoals);if(r.seconds>0){++i;continue;}bool unloaded=false;for(size_t j=0;j<r.members.size();++j){if(r.members[j].isNull())continue;Character* c=r.members[j].getCharacter();if(!c){unloaded=true;continue;}Faction* f=c->getFaction();if(f){f->destroyObject(c);r.members[j].setNull();}}if(unloaded)++i;else retiredQuestGroups.erase(retiredQuestGroups.begin()+i);}
    int e=selectedEscortQuest,b=selectedBountyQuest;questContextIteration=true;
    for(int i=0;i<maximumActiveQuests;++i){selectEscortQuest(i);questActorRetry[i]-=dt;if(questActorRetry[i]>0)continue;caravanRestore.resolve(caravanMembers,dt);scientificRestore.resolve(scientificMembers,dt);if(caravanRestore.pending||scientificRestore.pending)continue;refreshQuestActors();if((missionActive||missionPending)&&!escort&&!escortHandle.isNull()){questActorRetry[i]=1;continue;}
        if(questOrdersNeedRestore[i]&&escort){
#ifdef MERCENARIE_ESCORT_PERSONNEL
            restoreCurrentPersonnel();
#endif
            if(missionPending){negotiationSuspended=true;contractLifecycle=CONTRACT_CLIENT_MEETING;}
            for(size_t member=0;member<progressMembers.size();++member)missionAdoptRestoredTravel(progressMembers[member].getCharacter());
            missionRescue.leaderWatch.fresh();missionRescue.roadPlanned=false;
            if(!escort->isInCombatMode(true,true))resumeRecoveredMissionPhase();
            enforceWaitingHere();questOrdersNeedRestore[i]=missionFollowing&&!missionFollowTarget.getCharacter();
        }
        tickEscortMission(dt);if(finalWindow&&finalWindow->getVisible()){questContextIteration=false;return;}}
    selectEscortQuest(e);
    for(int i=0;i<maximumActiveQuests;++i){selectBountyQuest(i);updateBountyContract(dt);if(finalWindow&&finalWindow->getVisible()){questContextIteration=false;return;}}
    selectBountyQuest(b);questContextIteration=false;
}
template<class A> void archiveQuestContexts(A& a){
const bool isolated=false;
#include "QuestContextsArchive.h"
}

}
