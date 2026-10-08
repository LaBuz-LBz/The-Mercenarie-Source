// Shared wire schema: live restoration and isolated validation use identical fields.

#define M(x) {saveStage("missions",#x);a.field(x);}
    a.requireActors=false; // Mission actors have durable identities in the roster.
    M(escort);
    M(escortHandle);
    M(destination);
    M(missionReward);
    M(healthBonus);
    M(missionActive);
    M(missionPending);
    M(contractLifecycle);
    M(waitingForPlayer);
    M(leavingBuilding);
    M(exitWaypoint);
    M(updateClock);
    M(stationaryClock);
    M(journeyStart);
    M(journeyDistanceSquared);
    M(missionPaused);
    M(missionFollowing);
    M(missionFollowTarget);
    M(missionFollowers);
    M(waitingHere);
    M(midpointChecked);
    M(quarterSpeech);
    M(finalSpeech);
    M(rareContract);
    M(contractOriginFaction);
    M(selectedDistance);
    M(wasInCombat);
    M(unconsciousSeconds);
    M(carriedByPlayerSeconds);
    M(baseReward);
    M(proposedReward);
    M(counterOffer);
    M(advancePaid);
    M(negotiatedAdvancePercent);
    M(negotiationInsistence);
    M(missionTierIndex);
    M(pendingGuildXp);
    M(clientBudgetMultiplier);
    M(personalityAcceptance);
    M(counterOfferActive);
    M(negotiationWasPaused);
    M(originCity);
    M(missionElapsed);
    M(expectedTravelTime);
    M(escortWasKnockedOut);
    M(journeyCombatCount);
    M(proximitySeconds);
    M(journeySeconds);
    M(incidentClock);
    M(importantAlertClock);
    M(lastImportantAlert);
    M(lastJourneyPosition);
    M(currentContract);
    M(journeyData);
    M(earnedFinalBonus);
    M(earnedFinalBonusCount);
    M(finalClientTip);
    M(requestedFinalBonusPercent);
    M(finalHealthy);
    M(finalFast);
    M(boardCaravan);
    M(boardScientific);
    M(caravanMission);
    M(caravanReturning);
    M(caravanOrigin);
    SavedActorResolution::archive(a,caravanMembers,caravanRestore);
    M(caravanCargoType);
    M(caravanCargoValue);
    M(caravanInitialMembers);
    M(travelIncidentTriggered);
    M(scientificMission);
    M(scientificResearching);
    M(scientificReturning);
    M(scientificEntryAttempted);
    M(scientificInsideDiscovery);
    M(scientificWillEnter);
    M(scientificEntryResolved);
    M(scientificResearchSeconds);
    M(scientificMoveClock);
    M(scientificCommentClock);
    M(scientificEntryClock);
    M(scientificOrigin);
    M(scientificRuinCenter);
    SavedActorResolution::archive(a,scientificMembers,scientificRestore);
    M(currentMissionFiscalId);
    a.requireActors=true; // Keep the existing world-data restoration checks.
    M(fiscalParty);
    M(guildVisitors);
    M(guildVisitorSequence);
    M(departingVisitors);
    M(guildVisitorSpawnClock);
    M(visitorOfferUrgent);
    M(visitorOfferVip);
    M(visitorOfferExceptional);
    M(negotiationSuspended);
    M(contractBarmanHandle);
    M(contractOriginTown);
    M(completedCleanupClock);
    SavedActorResolution::archive(a,completedEscort,completedEscortRestore);
    M(refusedDepartures);
    SavedActorResolution::archive(a,completedCaravan,completedCaravanRestore);
    M(selectedProfileSquad);
    M(selectedProfileName);
    M(selectedProfileGroupSize);
    M(selectedProfileRarity);
    M(currentBoardKey);
    M(selectedOffer);
    M(developerTimeOffsetHours);
    M(designatedGuildHouseHandle);
    M(snapshotFiscal);
    M(snapshotReputation);
    M(snapshotBoards);
    M(restoredFinalVisible);
    M(restoredFinalCaption);
    MISSION_TOWN_STORAGE
    if(!a.reading)townStorage=destinationTown?destinationTown:"";
    M(townStorage);M(destinationNameStorage);
    if(a.reading){destinationTown=townStorage.c_str();destinationName=destinationNameStorage.c_str();}
#undef M
    // Optional trailing extension: existing V4 save payloads remain readable.
    if(!a.reading||a.cursor<a.bytes.size()){
        saveStage("bounty","contracts");std::string bountyBytes;if(!a.reading)bountyBytes=bountyWorld.save();
        a.field(bountyBytes);if(a.reading)bountyWorld.load(bountyBytes);
    }else bountyWorld=MercenarieV5::BountyWorldState();
    if(!a.reading||(a.reading&&a.cursor<a.bytes.size())){
        std::string routeBytes;if(!a.reading&&testRoute.missionId==currentMissionFiscalId&&testRoute.status!=RoutePrototype::Idle){routeBytes=testRoute.save();RoutePrototype::Route::load(routeBytes,currentMissionFiscalId);}
        a.field(routeBytes);
        if(a.reading){testRoute.cancel();if(!routeBytes.empty()){
            RoutePrototype::Route savedRoute=RoutePrototype::Route::load(routeBytes,currentMissionFiscalId);
            if(!a.validationOnly&&routeTestEnabled())testRoute=savedRoute;
        }}
    }else if(a.reading)testRoute.cancel();
    if(!a.reading||a.cursor<a.bytes.size()){
        if(a.reading)restoredProgressV2=true;
        GuildProgression::archiveOutcome(a,progressMembers,progressRosterKnown,progressAnyClientKo,progressReportLocal,progressReportGlobal,progressDeadMembers);
    }else if(a.reading){restoredProgressV2=false;progressMembers.clear();progressDeadMembers.clear();progressRosterKnown=false;progressAnyClientKo=escortWasKnockedOut;progressReportLocal=progressReportGlobal=0;}
    if(!a.reading||a.cursor<a.bytes.size())MissionBonuses::archive(a,finalBonusChoice);
    else if(a.reading)finalBonusChoice.reset();
    archiveReportTime(a,reportStartHour,reportEndHour);
    saveStage("quests","contexts");archiveQuestContexts(a);
    if(!a.reading||a.cursor<a.bytes.size())archiveMissionPlatoons(a);
    else if(a.reading)clearMissionPlatoons();
    if(!a.reading||a.cursor<a.bytes.size())a.field(missionPace);
    else if(a.reading)missionPace=EscortPace::Normal;
    if(!a.reading||a.cursor<a.bytes.size()){a.field(missionForcedPace);a.field(missionCasualtyWaiting);a.field(missionSeparation.suspiciousSeconds);a.field(missionSeparation.warned);a.field(carriedDestinationDistance);}
    else if(a.reading){missionForcedPace=false;missionCasualtyWaiting=false;missionSeparation=EscortMissionRules::Separation();carriedDestinationDistance=-1;}
    // Optional V1 EN_MISSION extension. Older sidecars end before this block.
    if(!a.reading||a.cursor<a.bytes.size())archiveMissionAbsence(a);
    else if(a.reading)clearMissionAbsence();
    // Optional delegated-mission extension. Older saves end before this block.
    saveStage("delegation","legacy");if(!a.reading||a.cursor<a.bytes.size())delegatedMission.archive(a);
    else if(a.reading)delegatedMission.clear();
    // Optional mail-contract extension. Old saves end before this field.
    saveStage("mail","contracts");if(!a.reading||a.cursor<a.bytes.size())a.field(mailContracts);
    else if(a.reading)mailContracts.clear();
    // V2 multi-group delegation extension. The preceding singleton remains as
    // a compatibility bridge for old sidecars and is migrated after reading.
    saveStage("delegation","groups");if(!a.reading||a.cursor<a.bytes.size())a.field(delegatedMissions);
    else if(a.reading)delegatedMissions.clear();
    if(a.reading&&delegatedMissions.empty()&&delegatedMission.timing.active){if(delegatedMission.groupId.empty())delegatedMission.groupId="legacy";delegatedMissions.push_back(delegatedMission);delegatedMission.clear();}
    // Optional shared-seat queue extension. Reservations cover the interval while
    // an actor is still walking to a chair; tickets preserve FIFO across save/load.
    if(!a.reading||a.cursor<a.bytes.size()){
        std::vector<GuildSeatTicketState> ticketState;
        if(!a.reading)for(std::map<std::string,unsigned long>::const_iterator it=guildSeatTickets.begin();it!=guildSeatTickets.end();++it){GuildSeatTicketState state;state.key=it->first;state.ticket=it->second;ticketState.push_back(state);}
        a.field(ticketState);a.field(guildSeatNextTicket);a.field(guildSeatReservations);
        if(a.reading){guildSeatTickets.clear();for(size_t i=0;i<ticketState.size();++i)if(!ticketState[i].key.empty()&&ticketState[i].ticket)guildSeatTickets[ticketState[i].key]=ticketState[i].ticket;if(guildSeatNextTicket==0)guildSeatNextTicket=1;}
    }else if(a.reading){guildSeatTickets.clear();guildSeatReservations.clear();guildSeatNextTicket=1;if(!a.validationOnly)for(size_t i=0;i<guildVisitors.size();++i)guildSeatTicket("VISITOR:"+guildVisitors[i].offerKey);if(!a.validationOnly&&fiscalParty.waitingForConversation&&!fiscalParty.leader.isNull())guildSeatTicket(guildFiscalQueueKey());}
    // Optional travel extension, appended after the previous seat queue format.
    // Older saves reconstruct phases from physical arrival; tickets remain intact.
    if(!a.reading||a.cursor<a.bytes.size()){a.field(guildTravelRecords);a.field(fiscalTargetOffice);}
    else if(a.reading){guildTravelRecords.clear();fiscalTargetOffice.setNull();}
    // Optional V8 extension: restore tracking only, never reissue movement.
    archiveAutopilotTrips(a,autopilotTrips);
    // Optional V9 payroll extension: old archives initialize an empty ledger.
    saveStage("payroll","ledger");GuildPayroll::archiveOptional(a,guildPayroll,!a.reading||a.resolve||a.validationOnly);
    // Optional map selection, per save. Never restore an index into another
    // world's list; the overlay validates this contract identity before use.
    if(!a.reading||a.cursor<a.bytes.size())a.field(selectedMailMapId);
    else selectedMailMapId.clear();
    if(!a.reading||a.cursor<a.bytes.size()){
        saveStage("artisan","orders/clock");std::string bytes;if(!a.reading){bytes=ArtisanClock::save(artisanLedger,developerTimeOffsetHours);double checkedClock=0;ArtisanClock::load(bytes,checkedClock);}a.field(bytes);
        if(a.reading){double clock=0;ArtisanOrders::Ledger checked=ArtisanClock::load(bytes,clock);if(a.resolve||a.validationOnly){ArtisanClock::alignToShared(checked,clock,developerTimeOffsetHours);artisanLedger.swap(checked);}}
    }else if(a.reading&&a.resolve){artisanLedger=ArtisanOrders::Ledger();}
    archiveMissionRescue(a);
    // Optional V9 estate snapshot shares the native save boundary and wallet.
    if(!a.reading||a.cursor<a.bytes.size()){
        saveStage("estate","leases/clock");std::string bytes;if(!a.reading)bytes=RealEstate::save(estateState);a.field(bytes);
        RealEstate::State checked=RealEstate::load(bytes);snapshotEstate=bytes;
        if(a.reading&&a.resolve)estateState.swap(checked);
    }else if(a.reading){snapshotEstate.clear();if(a.resolve)estateState=RealEstate::State();}
    #include "MissionSectionsSchema.h"
    if(a.reading){
        for(int i=(int)guildVisitors.size()-1;i>=0;--i)if(guildVisitors[i].offerKey.find("PAY:")==0&&!guildVisitors[i].leader&&!guildVisitors[i].restoredLeader.pending){
            const std::string group=guildVisitors[i].offerKey.substr(4);guildVisitors.erase(guildVisitors.begin()+i);
            for(size_t m=0;m<delegatedMissions.size();++m)if(delegatedMissions[m].groupId==group&&delegatedMissions[m].completed&&delegatedMissions[m].success&&delegatedMissions[m].paymentState==2)delegatedMissions[m].paymentState=1;
        }
    }

    saveStage("missions","invariants");a.finish();
    if(contractLifecycle<CONTRACT_NONE||contractLifecycle>CONTRACT_FAILED)throw std::runtime_error("invalid mission state: contractLifecycle");
    if(selectedOffer<0||selectedOffer>=6)throw std::runtime_error("invalid mission state: selectedOffer");
    if(currentContract.type<MCT_ESCORT||currentContract.type>MCT_SCIENCE)throw std::runtime_error("invalid mission state: currentContract.type");
    if(currentContract.rarity<MCR_COMMON||currentContract.rarity>MCR_LEGENDARY)throw std::runtime_error("invalid mission state: currentContract.rarity");
    if(missionReward<0||advancePaid<0)throw std::runtime_error("invalid mission state: missionReward/advancePaid");
    if(!EscortPace::valid((int)missionPace))throw std::runtime_error("invalid mission state: missionPace");
    if(fiscalParty.organisation < -1||fiscalParty.organisation>1)throw std::runtime_error("invalid mission state: fiscalParty.organisation");
