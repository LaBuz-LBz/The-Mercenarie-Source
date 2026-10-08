#include "QuestDelegatedProgress.h"
#include "WorldServiceClock.h"
#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <limits>
struct DelegatedMissionState;
void ensureDelegatedPaymentClient(DelegatedMissionState&);void archiveDelegated(DelegatedMissionState&,int);
namespace MissionAbsencePrototype{bool available=true;int returns=0;bool returnGroup(const std::string&){if(!available)return false;++returns;return true;}}
struct Payroll{int n;Payroll():n(0){}void complete(const std::string&){++n;}}guildPayroll;
namespace GuildProgression{void award(int& p,int&,int xp){p+=xp;}float clampRep(float p){return p;}}
int successfulContracts=0,failedContracts=0,guildPoints=0,guildPrestige=0;float escortReputation=0;
double currentGameHours=100;std::set<std::string> rewardedContractIds;std::vector<std::string> contractHistory;
struct Options{bool showNotifications,notifyDelegatedComplete;Options():showNotifications(true),notifyDelegatedComplete(true){}}clientOptions;
int reports=0,saves=0;void saveReputations(){++saves;}void showDelegatedReport(int){++reports;}void DebugLog(const std::string&){}
#include "quest-progress-runtime.generated.h"
void ensureDelegatedPaymentClient(DelegatedMissionState&){}void archiveDelegated(DelegatedMissionState&,int){}
struct Archive{
 bool reading;std::stringstream data;Archive(bool r=false):reading(r){}
 template<class T>void field(T& v){if(reading)data.read(reinterpret_cast<char*>(&v),sizeof(v));else data.write(reinterpret_cast<const char*>(&v),sizeof(v));}
 void field(std::string& s){size_t n=s.size();field(n);if(reading){s.resize(n);if(n)data.read(&s[0],n);}else if(n)data.write(s.data(),n);}
};
int main(){
 using namespace DelegatedMissionTiming;using QuestDelegatedProgress::read;
 State s=start(ActivityScientificExpedition,0,100);assert(s.exactReturnWorldHour==148);
 for(int step=0;step<=4;++step){QuestDelegatedProgress::Value p=read(s,100+12*step);assert(p.valid&&p.remaining==48-12*step&&p.percent==25*step);assert(std::fabs(p.ratio-step*.25)<1e-9);}
 assert(read(s,99).percent==0&&read(s,99).remaining==48);assert(read(s,10000).percent==100&&read(s,10000).remaining==0);assert(read(s,147.9999).percent==99);
 assert(read(s,WorldServiceClock::now(100,24)).percent==50);assert(read(s,WorldServiceClock::now(100,48)).percent==100);
 DelegatedMissionState m;m.groupId="save-test";m.offerIdentity="offer";m.timing=s;m.resultRolled=true;m.success=true;m.sent=4;m.guildXp=35;m.reputationDelta=1;
 Archive save;m.archive(save);float offset=24;save.field(offset);Archive load(true);load.data.str(save.data.str());DelegatedMissionState restored;restored.archive(load);float savedOffset=0;load.field(savedOffset);
 assert(restored.groupId==m.groupId&&restored.timing.active);assert(read(restored.timing,WorldServiceClock::now(100,savedOffset)).percent==50);
 delegatedMissions.push_back(restored);currentGameHours=147;updateDelegatedMission();assert(!delegatedMissions[0].completed&&MissionAbsencePrototype::returns==0);
 currentGameHours=148;MissionAbsencePrototype::available=false;updateDelegatedMission();assert(!delegatedMissions[0].completed&&delegatedMissions[0].timing.active&&read(delegatedMissions[0].timing,currentGameHours).percent==100);
 MissionAbsencePrototype::available=true;updateDelegatedMission();assert(delegatedMissions[0].completed&&!delegatedMissions[0].timing.active);assert(delegatedMissions[0].returned==4&&delegatedMissions[0].paymentState==1&&MissionAbsencePrototype::returns==1&&reports==1&&saves==1&&guildPayroll.n==1);
 updateDelegatedMission();assert(MissionAbsencePrototype::returns==1&&reports==1&&saves==1);
 m.groupId="failure";m.success=false;delegatedMissions.push_back(m);updateDelegatedMission();assert(delegatedMissions.back().completed&&delegatedMissions.back().paymentState==0&&failedContracts==1);
 State zero=start(ActivityEscort,0,100);assert(read(zero,100).percent==100);State invalid=s;invalid.exactReturnWorldHour=99;assert(!read(invalid,100).valid);invalid=s;invalid.exactReturnWorldHour=std::numeric_limits<double>::infinity();assert(!read(invalid,100).valid);assert(!read(s,std::numeric_limits<double>::quiet_NaN()).valid);
 assert(QuestDelegatedProgress::missionType(ActivityBountyHunt)==3&&QuestDelegatedProgress::missionType(ActivityUndefined)==-1);
 std::cout<<"PASS: exact 48h timeline 0/25/50/75/100; +24/+48; production mission archive and debug-offset reload; actual completion routine returns once, waits for real return, records success/failure and removes active timing; invalid time guards.\n";
}
