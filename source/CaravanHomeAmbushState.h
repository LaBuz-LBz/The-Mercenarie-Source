#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "CaravanBattleResolution.h"
namespace CaravanHomeAmbush {
struct State {
    bool attempted,triggered;std::string runner;double remaining,retry;
    State():attempted(false),triggered(false),runner("-"),remaining(0),retry(0){}
};
inline State read(const std::string& metadata){
    State s;size_t p=metadata.find(";HOMEAMBUSH1=");if(p==std::string::npos)return s;p+=13;
    std::istringstream in(metadata.substr(p,metadata.find(';',p)-p));
    if(!(in>>s.attempted>>s.triggered>>s.runner>>s.remaining>>s.retry)||s.runner.size()>100||!(s.remaining>=0&&s.remaining<=20)||!(s.retry>=0&&s.retry<=5)||(!s.attempted&&s.triggered))throw std::runtime_error("invalid caravan house ambush");
    return s;
}
inline void write(std::string& metadata,const State& s){
    size_t p=metadata.find(";HOMEAMBUSH1=");if(p!=std::string::npos){size_t e=metadata.find(';',p+1);metadata.erase(p,e==std::string::npos?std::string::npos:e-p+1);}
    std::ostringstream out;out<<std::setprecision(15)<<";HOMEAMBUSH1="<<s.attempted<<' '<<s.triggered<<' '<<s.runner<<' '<<s.remaining<<' '<<s.retry<<';';metadata+=out.str();
}
inline bool fleeing(const std::string& metadata,const std::string& actor){State s=read(metadata);return s.triggered&&s.remaining>0&&s.runner==actor;}
}
