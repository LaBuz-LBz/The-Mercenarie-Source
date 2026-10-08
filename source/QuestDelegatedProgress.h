#pragma once
#include "DelegatedMissionTiming.h"
// Read-only projection of the existing persisted mission clock.
namespace QuestDelegatedProgress {
struct Value {bool valid;double remaining,ratio;int percent;Value():valid(false),remaining(0),ratio(0),percent(0){}};
inline bool finite(double x){return x==x&&x>=0&&x<1e9;}
inline Value read(const DelegatedMissionTiming::State& s,double now){
 Value v;if(!finite(now)||!finite(s.startedAtWorldHour)||!finite(s.exactReturnWorldHour)||s.exactReturnWorldHour<s.startedAtWorldHour)return v;
 const double total=s.exactReturnWorldHour-s.startedAtWorldHour;
 v.valid=true;v.remaining=std::max(0.0,s.exactReturnWorldHour-std::max(now,s.startedAtWorldHour));
 v.ratio=total>0?std::max(0.0,std::min(1.0,(now-s.startedAtWorldHour)/total)):(now>=s.exactReturnWorldHour?1.0:0.0);
 v.percent=v.ratio>=1?100:std::min(99,(int)std::floor(v.ratio*100.0+1e-8));return v;
}
inline int missionType(DelegatedMissionTiming::ActivityType a){
 using namespace DelegatedMissionTiming;
 return a==ActivityEscort?0:a==ActivityCaravan?1:a==ActivityScientificExpedition?2:a==ActivityBountyHunt?3:a==ActivityMailDelivery?4:-1;
}
}
