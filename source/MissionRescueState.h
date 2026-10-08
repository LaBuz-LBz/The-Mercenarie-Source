#pragma once
#include "MissionMotionPolicy.h"
#include "MissionPassageRecovery.h"
#include "MissionLegacyNavigation.h"
#include "MissionNavigationRPState.h"
// Only reservations are mod state. Health, supplies and carrying remain native.
struct MissionRescueTask {
    hand patient, helper;
    enum Phase { Detected, WaitingForSafe, FirstAid, CarrierAssigned };
    Phase phase;
    float retry,diagnosticClock;
    MissionRescueTask():phase(Detected),retry(0),diagnosticClock(0){}
};
struct MissionHoldAnchor {hand actor;Ogre::Vector3 position;};
struct MissionRescueState {
    MissionNavigationRP::State<Ogre::Vector3> navigationRP;
    MissionLegacyNavigation::State<Ogre::Vector3> legacy;
    MissionPassageRecovery::Episode<Ogre::Vector3> passage;
    MissionMotionPolicy::Watch leaderWatch;
    MissionMotionPolicy::RouteRetry routeRetry;
    MissionMotionPolicy::Reason waitReason;
    float waitNoticeClock,regroupSeconds;
    float formationSpeedScale; // Runtime-only; reset on load.
    bool motionSuspended,localRecovery,scienceBuildingScanned;
    Ogre::Vector3 localRecoveryTarget;
    hand scienceBuilding;
    std::vector<hand> failedExitDoors;
    struct OwnedOrder { hand actor,subject; int type; Ogre::Vector3 position; };
    std::vector<OwnedOrder> ownedOrders;
    void resetMotion(){formationSpeedScale=1;navigationRP=MissionNavigationRP::State<Ogre::Vector3>();legacy=MissionLegacyNavigation::State<Ogre::Vector3>();doorApproachClock=0;passage.clear();leaderWatch=MissionMotionPolicy::Watch();routeRetry=MissionMotionPolicy::RouteRetry();waitReason=MissionMotionPolicy::Ready;waitNoticeClock=regroupSeconds=0;motionSuspended=localRecovery=scienceBuildingScanned=false;scienceBuilding=hand();failedExitDoors.clear();ownedOrders.clear();}
    bool gateClearing;
    float gateScanClock,gateClearSeconds;
    float doorApproachClock;
    hand gateHandle,gateLeader;
    Ogre::Vector3 gateExit;
    std::vector<Ogre::Vector3> roadPoints;
    Ogre::Vector3 roadGoal;
    size_t roadNext;
    size_t roadProjectedIndex; // Runtime-only; never added to the save format.
    bool roadProjectionElevated;
    bool roadPlanned,roadFailed;
    unsigned roadContinuityRetries;float roadContinuityCooldown;Ogre::Vector3 roadContinuityOrigin;
    std::vector<MissionHoldAnchor> holds;
    std::vector<MissionRescueTask> tasks;
    std::vector<hand> retiredLeaders;
    float safeSeconds, lastSpeed, regroupLogClock, navigationLogClock;
    bool regrouping;
    bool recoveryPending; // Runtime-only; load already rebuilds rescue and route.
    MissionRescueState():formationSpeedScale(1),waitReason(MissionMotionPolicy::Ready),waitNoticeClock(0),regroupSeconds(0),motionSuspended(false),localRecovery(false),scienceBuildingScanned(false),gateClearing(false),doorApproachClock(0),gateScanClock(0),gateClearSeconds(0),roadNext(0),roadProjectedIndex((size_t)-1),roadProjectionElevated(false),roadPlanned(false),roadFailed(false),roadContinuityRetries(0),roadContinuityCooldown(0),safeSeconds(0),lastSpeed(-1),regroupLogClock(0),navigationLogClock(0),regrouping(false),recoveryPending(false){}
};
#ifndef MISSION_RESCUE_TYPES_ONLY
MissionRescueState missionRescue;
#endif
