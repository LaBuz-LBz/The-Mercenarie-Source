#include "DelegatedMissionTiming.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

struct Archive {
    bool reading;std::stringstream data;
    Archive(bool r=false):reading(r){}
    template<class T>void field(T& value){if(reading)data.read(reinterpret_cast<char*>(&value),sizeof(value));else data.write(reinterpret_cast<const char*>(&value),sizeof(value));}
};

struct SimulatedMission { DelegatedMissionTiming::State timing;std::string group;bool success;SimulatedMission(const char* id,double remaining,double now,bool result):timing(DelegatedMissionTiming::start(DelegatedMissionTiming::ActivityMailDelivery,remaining*3.0,now)),group(id),success(result){} };
static int processDue(std::vector<SimulatedMission>& missions,double now,std::vector<std::string>& returned){int processed=0;for(size_t i=0;i<missions.size();){if(!DelegatedMissionTiming::completed(missions[i].timing,now)){++i;continue;}returned.push_back(missions[i].group);missions.erase(missions.begin()+i);++processed;}return processed;}

int main(){
    using namespace DelegatedMissionTiming;
    assert(std::fabs(travelHours(60.0)-20.0)<0.0001);
    State state=start(ActivityBountyHunt,60.0,1000.0);
    assert(std::fabs(state.travelHours-20.0)<0.0001);
    assert(std::fabs(state.activityHours-48.0)<0.0001);
    State mail=start(ActivityMailDelivery,60.0,500.0);
    assert(std::fabs(mail.travelHours-20.0)<0.0001);
    assert(mail.activityHours==0.0);
    assert(std::fabs(state.exactReturnWorldHour-1068.0)<0.0001);
    assert(start(ActivityEscort,60.0,1000.0).exactReturnWorldHour==1020.0);
    assert(start(ActivityCaravan,60.0,1000.0).exactReturnWorldHour==1032.0);
    assert(start(ActivityScientificExpedition,60.0,1000.0).exactReturnWorldHour==1068.0);
    PublicEstimate initial=initialEstimate(state);
    assert(initial.minimumHours==54.0&&initial.maximumHours==85.0);
    assert(initial.minimumHours<=68.0&&initial.maximumHours>=68.0);
    PublicEstimate remaining=remainingEstimate(state,1020.0);
    assert(remaining.minimumHours==34.0&&remaining.maximumHours==65.0);
    assert(!completed(state,1067.999)&&completed(state,1068.0));
    bool rejected=false;try{start(ActivityUndefined,60.0,1000.0);}catch(const std::invalid_argument&){rejected=true;}assert(rejected);
    Archive saved;state.archive(saved);Archive loaded(true);loaded.data.str(saved.data.str());State restored;restored.archive(loaded);
    assert(restored.exactReturnWorldHour==state.exactReturnWorldHour);
    assert(restored.estimateEarliestReturnWorldHour==state.estimateEarliestReturnWorldHour);
    assert(restored.estimateLatestReturnWorldHour==state.estimateLatestReturnWorldHour);
    State forty=start(ActivityMailDelivery,120.0,100.0);assert(completed(forty,148.0)&&!completed(forty,120.0));
    std::vector<SimulatedMission> many;many.push_back(SimulatedMission("A",10,100,true));many.push_back(SimulatedMission("B",35,100,false));many.push_back(SimulatedMission("C",70,100,true));std::vector<std::string> returned;assert(processDue(many,148,returned)==2&&many.size()==1&&many[0].group=="C"&&many[0].success);assert(returned.size()==2&&returned[0]=="A"&&returned[1]=="B");
    std::vector<SimulatedMission> all;all.push_back(SimulatedMission("A",5,100,true));all.push_back(SimulatedMission("B",20,100,false));all.push_back(SimulatedMission("C",40,100,true));returned.clear();assert(processDue(all,148,returned)==3&&all.empty()&&returned.size()==3);
    std::vector<SimulatedMission> persisted;persisted.push_back(SimulatedMission("SAVE",40,100,true));Archive timingSaved;persisted[0].timing.archive(timingSaved);Archive timingLoaded(true);timingLoaded.data.str(timingSaved.data.str());State afterReload;afterReload.archive(timingLoaded);assert(completed(afterReload,148));returned.clear();assert(processDue(persisted,148,returned)==1&&persisted.empty());assert(processDue(persisted,148,returned)==0);
    std::cout<<"PASS: 20 min/km, bounty +48 h, world-hour completion, hidden stable range, save/load, undefined types rejected.\n";
}
