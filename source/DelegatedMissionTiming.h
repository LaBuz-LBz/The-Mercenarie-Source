#pragma once
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace DelegatedMissionTiming {

static const double MinutesPerKilometre = 20.0;
static const double BountyActivityHours = 48.0;

enum ActivityType { ActivityUndefined=0, ActivityBountyHunt=1, ActivityEscort=2, ActivityCaravan=3, ActivityScientificExpedition=4, ActivityMailDelivery=5 };

struct State {
    int schemaVersion;
    ActivityType activityType;
    double totalDistanceKm;
    double travelHours;
    double activityHours;
    double startedAtWorldHour;
    double exactReturnWorldHour;
    double estimateEarliestReturnWorldHour;
    double estimateLatestReturnWorldHour;
    bool active;

    State():schemaVersion(1),activityType(ActivityUndefined),totalDistanceKm(0),travelHours(0),activityHours(0),startedAtWorldHour(0),exactReturnWorldHour(0),estimateEarliestReturnWorldHour(0),estimateLatestReturnWorldHour(0),active(false){}

    template<class Archive> void archive(Archive& a){
        a.field(schemaVersion);a.field(activityType);a.field(totalDistanceKm);
        a.field(travelHours);a.field(activityHours);a.field(startedAtWorldHour);
        a.field(exactReturnWorldHour);a.field(estimateEarliestReturnWorldHour);
        a.field(estimateLatestReturnWorldHour);a.field(active);
        if(a.reading&&(schemaVersion!=1||totalDistanceKm<0||travelHours<0||activityHours<0||
            exactReturnWorldHour<startedAtWorldHour||estimateEarliestReturnWorldHour>exactReturnWorldHour||
            estimateLatestReturnWorldHour<exactReturnWorldHour))throw std::runtime_error("invalid delegated mission timing");
    }
};

inline double travelHours(double totalDistanceKm){
    return std::max(0.0,totalDistanceKm)*MinutesPerKilometre/60.0;
}

inline bool activityDuration(ActivityType type,double& hours){
    if(type==ActivityBountyHunt){hours=BountyActivityHours;return true;}
    if(type==ActivityEscort){hours=0.0;return true;}
    if(type==ActivityCaravan){hours=12.0;return true;}
    if(type==ActivityScientificExpedition){hours=48.0;return true;}
    if(type==ActivityMailDelivery){hours=0.0;return true;}
    hours=0;return false;
}

inline State start(ActivityType type,double totalDistanceKm,double currentWorldHour){
    double activity=0;
    if(totalDistanceKm<0||currentWorldHour<0||!activityDuration(type,activity))
        throw std::invalid_argument("delegated mission duration is undefined");
    State state;state.activityType=type;state.totalDistanceKm=totalDistanceKm;
    state.travelHours=travelHours(totalDistanceKm);state.activityHours=activity;
    state.startedAtWorldHour=currentWorldHour;
    const double exactDuration=state.travelHours+state.activityHours;
    state.exactReturnWorldHour=currentWorldHour+exactDuration;
    // Stable, deliberately imprecise public estimate. Store the absolute bounds
    // so opening the UI or loading a save can never reroll them.
    const double minimumDuration=std::floor(exactDuration*0.80);
    const double maximumDuration=std::ceil(exactDuration*1.25);
    state.estimateEarliestReturnWorldHour=currentWorldHour+std::min(minimumDuration,exactDuration);
    state.estimateLatestReturnWorldHour=currentWorldHour+std::max(maximumDuration,exactDuration);
    state.active=true;return state;
}

struct PublicEstimate { double minimumHours,maximumHours; PublicEstimate(double a=0,double b=0):minimumHours(a),maximumHours(b){} };

inline PublicEstimate initialEstimate(const State& state){
    if(!state.active)return PublicEstimate();
    return PublicEstimate(state.estimateEarliestReturnWorldHour-state.startedAtWorldHour,
        state.estimateLatestReturnWorldHour-state.startedAtWorldHour);
}

inline PublicEstimate remainingEstimate(const State& state,double currentWorldHour){
    if(!state.active)return PublicEstimate();
    return PublicEstimate(std::max(0.0,state.estimateEarliestReturnWorldHour-currentWorldHour),
        std::max(0.0,state.estimateLatestReturnWorldHour-currentWorldHour));
}

inline bool completed(const State& state,double currentWorldHour){
    return state.active&&currentWorldHour>=state.exactReturnWorldHour;
}

}
