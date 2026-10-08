#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
// Stored as an optional contract metadata tag: existing save wire layouts stay compatible.
namespace CaravanTrade {
struct State {
    int phase,step,lastPhrase;
    double endHour,sayClock,moveClock,dwell,attempt,x,y,z;
    std::string shop,previous,seller,visited;
    int shopLine;
    State():phase(0),step(0),lastPhrase(-1),endHour(0),sayClock(30),moveClock(0),dwell(0),attempt(0),x(0),y(0),z(0),shop("-"),previous("-"),seller("-"),visited("-"),shopLine(0){}
};
inline State read(const std::string& metadata){
    State s;size_t p=metadata.find(";TRADE1=");if(p==std::string::npos)return s;
    std::istringstream in(metadata.substr(p+8,metadata.find(';',p+8)-(p+8)));
    if(!(in>>s.phase>>s.step>>s.lastPhrase>>s.endHour>>s.sayClock>>s.moveClock>>s.dwell>>s.attempt>>s.x>>s.y>>s.z>>s.shop>>s.previous))return State();
    if(s.phase<1||s.phase>3||s.step<0||s.step>7||s.lastPhrase< -1||s.lastPhrase>9||!(s.endHour>=0&&s.endHour<1e12))return State();
    size_t q=metadata.find(";SHOPSALE1=");
    if(q!=std::string::npos){std::istringstream sale(metadata.substr(q+11,metadata.find(';',q+11)-(q+11)));
        if(!(sale>>s.shopLine>>s.seller>>s.visited)||s.shopLine<0||s.shopLine>10){s.shopLine=0;s.seller="-";s.visited="-";}
    }else {s.sayClock=0;s.lastPhrase=0;} // Old timed city visits are not purchases.
    return s;
}
inline void write(std::string& metadata,const State& s){
    size_t p=metadata.find(";TRADE1=");if(p!=std::string::npos){size_t e=metadata.find(';',p+8);metadata.erase(p,e==std::string::npos?std::string::npos:e-p+(e+1==metadata.size()||metadata[e+1]==';'?1:0));}
    std::ostringstream out;out<<std::setprecision(15)<<";TRADE1="<<s.phase<<' '<<s.step<<' '<<s.lastPhrase<<' '<<s.endHour<<' '<<s.sayClock<<' '<<s.moveClock<<' '<<s.dwell<<' '<<s.attempt<<' '<<s.x<<' '<<s.y<<' '<<s.z<<' '<<s.shop<<' '<<s.previous<<';';metadata+=out.str();
    p=metadata.find(";SHOPSALE1=");if(p!=std::string::npos){size_t e=metadata.find(';',p+11);metadata.erase(p,e==std::string::npos?std::string::npos:e-p+(e+1==metadata.size()||metadata[e+1]==';'?1:0));}
    std::ostringstream sale;sale<<";SHOPSALE1="<<s.shopLine<<' '<<s.seller<<' '<<s.visited<<';';metadata+=sale.str();
}
inline void begin(State& s,double now){s=State();s.phase=1;s.endHour=0; /* Legacy save field, no duration limit. */}
inline bool active(const State& s){return s.phase==1||s.phase==2;}
inline void expire(State& s,double now){if(s.phase==1&&now>=s.endHour){s.phase=2;s.step=0;s.moveClock=0;}}
inline int phrase(int randomIndex,int previous){int n=std::max(0,std::min(8,randomIndex));return previous<0?n:(n>=previous?n+1:n);}
}
