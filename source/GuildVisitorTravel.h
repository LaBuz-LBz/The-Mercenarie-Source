#pragma once
#include <stdexcept>
namespace GuildVisitorTravel
{
    enum Phase { Spawned, TravellingToOffice, ArrivedAtOffice, WaitingForSeat, TravellingToSeat, Seated };
    enum Reason { None, InitialOrder, RestoredOrder, NativePathFailure, Stagnation };
    struct State
    {
        Phase phase;float retry,lastDistance,orderAge,failureAge;bool restoreOrder;
        State():phase(Spawned),retry(0),lastDistance(-1),orderAge(0),failureAge(0),restoreOrder(false){}
        template<class Archive> void archive(Archive& a)
        {
            a.field(phase);
            if(phase<Spawned||phase>Seated)throw std::runtime_error("invalid visitor phase");
            if(a.reading){retry=orderAge=failureAge=0;lastDistance=-1;restoreOrder=true;}
        }
    };
    inline bool eligible(const State& s){return s.phase>=ArrivedAtOffice;}
    inline void issued(State& s){s.retry=s.orderAge=s.failureAge=0;s.restoreOrder=false;}
    inline Reason retryReason(State& s,bool progressed,bool failed,float elapsed)
    {
        s.orderAge+=elapsed;
        s.retry=progressed?0:s.retry+elapsed;
        s.failureAge=failed&&!progressed?s.failureAge+elapsed:0;
        if(s.orderAge>=3.0f&&s.failureAge>=3.0f)return NativePathFailure;
        if(s.orderAge>=12.0f&&s.retry>=12.0f)return Stagnation;
        return None;
    }
    // Arrival requires native membership in the target interior, never radius alone.
    inline Reason update(State& s,bool validOffice,bool insideTarget,bool progressed,bool failed,float elapsed)
    {
        if(!validOffice)return None;
        if(s.restoreOrder&&eligible(s)&&!insideTarget)s.phase=TravellingToOffice;
        if(eligible(s))return None;
        if(insideTarget){s.phase=ArrivedAtOffice;issued(s);return None;}
        Reason why=s.restoreOrder?RestoredOrder:s.phase==Spawned?InitialOrder:retryReason(s,progressed,failed,elapsed);
        s.phase=TravellingToOffice;
        return why;
    }
    inline void assigned(State& s,bool hasSeat,bool occupied)
    {if(eligible(s))s.phase=occupied?Seated:hasSeat?TravellingToSeat:WaitingForSeat;}
}
