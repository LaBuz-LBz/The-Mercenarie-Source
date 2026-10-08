#pragma once
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <string>

// Pure contract-Cats policy. Percent values are stored as integers so every
// screen and the actual payment use the exact same half-up rounding rule.
namespace ContractRewards {
inline int clampPercent(int percent){return std::max(50,std::min(150,percent));}
inline bool validPercent(int percent){return percent>=50&&percent<=150&&percent%10==0;}
inline int apply(int cats,int percent){
    if(cats<=0)return 0;
    const long long scaled=(long long)cats*clampPercent(percent);
    return (int)std::min(2147483647LL,(scaled+50)/100);
}
inline std::string token(int percent){std::ostringstream out;out<<";REWARD1="<<clampPercent(percent)<<';';return out.str();}
inline int snapshot(const std::string& data){
    const std::string key=";REWARD1=";size_t at=data.find(key);
    if(at==std::string::npos)return 100; // contracts from older saves
    int value=std::atoi(data.c_str()+at+key.size());return validPercent(value)?value:100;
}
inline void freeze(std::string& data,int percent){
    const std::string key=";REWARD1=";size_t at=data.find(key);
    if(at!=std::string::npos){size_t end=data.find(';',at+key.size());data.erase(at,(end==std::string::npos?data.size():end+1)-at);}
    data+=token(percent);
}
inline const char* label(int percent){
    switch(clampPercent(percent)){case 50:return "-50 %";case 60:return "-40 %";case 70:return "-30 %";case 80:return "-20 %";case 90:return "-10 %";case 110:return "+10 %";case 120:return "+20 %";case 130:return "+30 %";case 140:return "+40 %";case 150:return "+50 %";default:return "STANDARD";}
}
}
