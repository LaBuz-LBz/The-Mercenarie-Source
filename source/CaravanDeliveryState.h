#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
namespace CaravanDelivery {
enum Stage { Waiting, Entering, Inside, Returning, Done, Approaching };
struct Parcel {
    std::string actor,home;
    int stage;
    double elapsed,retry;
    bool delivered;
    Parcel():actor("-"),home("-"),stage(Waiting),elapsed(0),retry(0),delivered(false){}
};
struct State {
    int scenario;bool started;Parcel parcels[4];
    // Old saves retain normal sales; only new arrivals explicitly request a choice.
    State():scenario(1),started(false){parcels[3].stage=Done;}
};
inline State read(const std::string& metadata){
    State s;size_t p=metadata.find(";DELIVERY2=");bool modern=p!=std::string::npos;if(!modern)p=metadata.find(";DELIVERY1=");if(p==std::string::npos)return s;
    p+=11;std::istringstream in(metadata.substr(p,metadata.find(';',p)-p));
    if(!(in>>s.scenario>>s.started)||s.scenario<0||s.scenario>3)throw std::runtime_error("invalid caravan scenario");
    for(int i=0;i<(modern?4:3);++i){Parcel& r=s.parcels[i];
        if(!(in>>r.actor>>r.home>>r.stage>>r.elapsed>>r.retry>>r.delivered)||r.stage<0||r.stage>5||!(r.elapsed>=0&&r.elapsed<=86400)||!(r.retry>=0&&r.retry<=60)||r.actor.size()>100||r.home.size()>100)
            throw std::runtime_error("invalid caravan delivery");
    }return s;
}
inline void write(std::string& metadata,const State& s){
    size_t p=metadata.find(";DELIVERY2=");if(p==std::string::npos)p=metadata.find(";DELIVERY1=");if(p!=std::string::npos){size_t e=metadata.find(';',p+1);metadata.erase(p,e==std::string::npos?std::string::npos:e-p+1);}
    std::ostringstream out;out<<std::setprecision(15)<<";DELIVERY2="<<s.scenario<<' '<<s.started;
    for(int i=0;i<4;++i){const Parcel& r=s.parcels[i];out<<' '<<r.actor<<' '<<r.home<<' '<<r.stage<<' '<<r.elapsed<<' '<<r.retry<<' '<<r.delivered;}
    metadata+=out.str()+";";
}
inline bool active(const Parcel& p){return (p.stage>=Entering&&p.stage<=Returning)||p.stage==Approaching;}
inline bool owns(const State& s,const std::string& actor){for(int i=0;i<4;++i)if(active(s.parcels[i])&&s.parcels[i].actor==actor)return true;return false;}
inline bool complete(const State& s){if(!s.started)return false;for(int i=0;i<4;++i)if(s.parcels[i].stage!=Done)return false;return true;}
inline void advance(Parcel& p,int stage){p.stage=stage;p.elapsed=0;p.retry=0;}
}
