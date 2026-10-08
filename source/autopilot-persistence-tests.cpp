#include "MissionArchive.h"
#include "AutopilotTripArchive.h"
#include <cassert>
#include <limits>
#include <iostream>
struct Vector {float x,y,z;Vector(float a=0,float b=0,float c=0):x(a),y(b),z(c){}};
typedef std::map<std::string,Vector> Trips;
std::string save(Trips trips){MissionArchive a;archiveAutopilotTrips(a,trips);return sealMissionArchive(a.bytes);}
void load(const std::string& bytes,Trips& trips){MissionArchive a(openMissionArchive(bytes));archiveAutopilotTrips(a,trips);a.finish();}
int main(){
 Trips first;first["actor-A"]=Vector(100,2,-300);first["actor-B"]=Vector(20,3,40);
 std::string slotA=save(first);Trips restored;load(slotA,restored);assert(restored.size()==2&&restored["actor-A"].z==-300);
 // Other save, including identical actor ID, must replace the complete map.
 Trips second;second["actor-A"]=Vector(900,0,1);load(save(second),restored);assert(restored.size()==1&&restored["actor-A"].x==900);
 // Legacy archive has no extension; stale trips are cleared.
 MissionArchive legacy("");archiveAutopilotTrips(legacy,restored);assert(restored.empty());
 load(slotA,restored);restored.erase("actor-A");load(save(restored),first);assert(first.size()==1&&!first.count("actor-A"));
 bool rejected=false;std::string raw=openMissionArchive(slotA);raw.resize(raw.size()-1);try{MissionArchive broken(raw);archiveAutopilotTrips(broken,restored);}catch(...){rejected=true;}assert(rejected);
 rejected=false;Trips invalid;invalid["bad"]=Vector(std::numeric_limits<float>::quiet_NaN(),0,0);try{load(save(invalid),restored);}catch(...){rejected=true;}assert(rejected);
 rejected=false;MissionArchive tooMany;unsigned int count=4097;tooMany.field(count);try{MissionArchive bad(tooMany.bytes);archiveAutopilotTrips(bad,restored);}catch(...){rejected=true;}assert(rejected);
 std::cout<<"PASS: production Auto-Pilot codec, exact save isolation, old saves, cancellation, truncation, NaN and count limits.\n";
}
