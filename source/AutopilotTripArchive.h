#pragma once
#include <map>
#include <string>
#include <stdexcept>
// Append-only save extension; no world access or native movement commands.
template<class Archive,class Vector> void archiveAutopilotTrips(Archive& a,std::map<std::string,Vector>& trips){
    if(a.reading&&a.cursor==a.bytes.size()){trips.clear();return;}
    unsigned int count=(unsigned int)trips.size();a.field(count);
    if(count>4096)throw std::runtime_error("oversized autopilot trips");
    std::map<std::string,Vector> restored;
    if(a.reading){
        for(unsigned int i=0;i<count;++i){std::string id;Vector target;a.field(id);a.field(target.x);a.field(target.y);a.field(target.z);
            if(id.empty()||id.size()>256||restored.count(id)||!(target.x>=-10000000&&target.x<=10000000&&target.y>=-10000000&&target.y<=10000000&&target.z>=-10000000&&target.z<=10000000))throw std::runtime_error("invalid autopilot trip");
            restored[id]=target;
        }
        trips.swap(restored);
    }else for(typename std::map<std::string,Vector>::iterator i=trips.begin();i!=trips.end();++i){std::string id=i->first;const Vector& target=i->second;if(id.empty()||id.size()>256||!(target.x>=-10000000&&target.x<=10000000&&target.y>=-10000000&&target.y<=10000000&&target.z>=-10000000&&target.z<=10000000))throw std::runtime_error("invalid autopilot trip");a.field(id);a.field(i->second.x);a.field(i->second.y);a.field(i->second.z);}
}
