#include "Localization.h"
#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include <stdexcept>
namespace MissionBonuses {
enum Kind { Fast, NoKO, Healthy, Ambush, Detour, Proximity, Cargo, Discovery, Count };
struct Choice {
    std::vector<int> amounts;
    unsigned int selected;
    bool prepared;
    Choice():amounts(Count,0),selected(0),prepared(false){}
    void reset(){amounts.assign(Count,0);selected=0;prepared=false;}
    int count(unsigned int mask)const{int n=0;for(int i=0;i<Count;++i)if(amounts[i]>0&&(mask&(1u<<i)))++n;return n;}
    int total(unsigned int mask)const{int n=0;for(int i=0;i<Count;++i)if(mask&(1u<<i))n+=amounts[i];return n;}
    void cap(int maximum){
        int sum=total(255);if(sum>maximum&&sum>0)for(int i=0;i<Count;++i)amounts[i]=(int)((long long)amounts[i]*maximum/sum);
        prepared=true;
    }
};
inline int xpCost(int,int){return 0;}
struct Resolution {int cats,count;Resolution():cats(0),count(0){}};
template<class Roll> Resolution resolve(const Choice& choice,bool unlocked,float acceptance,float counter,Roll roll){
    Resolution result;if(!unlocked||!choice.prepared)return result;
    acceptance=std::max(.03f,std::min(.97f,acceptance));counter=std::max(0.0f,std::min(1.0f,counter));
    for(int i=0;i<Count;++i)if((choice.selected&(1u<<i))&&choice.amounts[i]>0){
        if(roll()<=acceptance){result.cats+=choice.amounts[i];++result.count;}
        else if(choice.amounts[i]>=2&&roll()<counter){result.cats+=choice.amounts[i]/2;++result.count;}
    }
    return result;
}
inline const char* name(int kind,bool english){
    const char* fr[]={Loc::text("mission.bonus.name.0"),Loc::text("mission.bonus.name.1"),Loc::text("mission.bonus.name.2"),Loc::text("mission.bonus.name.3"),Loc::text("mission.bonus.name.4"),Loc::text("mission.bonus.name.5"),Loc::text("mission.bonus.name.6"),Loc::text("mission.bonus.name.7")};
    const char* en[]={Loc::text("mission.bonus.name.0"),Loc::text("mission.bonus.name.1"),Loc::text("mission.bonus.name.2"),Loc::text("mission.bonus.name.3"),Loc::text("mission.bonus.name.4"),Loc::text("mission.bonus.name.5"),Loc::text("mission.bonus.name.6"),Loc::text("mission.bonus.name.7")};
    return (english?en:fr)[std::max(0,std::min(Count-1,kind))];
}
template<class Archive> void archive(Archive& a,Choice& choice){
    std::string marker="MISSION-BONUSES-1";a.field(marker);if(marker!="MISSION-BONUSES-1")throw std::runtime_error("invalid bonus version");
    a.field(choice.amounts);a.field(choice.selected);a.field(choice.prepared);
    if(choice.amounts.size()!=Count||choice.selected>255)throw std::runtime_error("invalid bonus selection");
    for(int i=0;i<Count;++i)if(choice.amounts[i]<0||choice.amounts[i]>10000000)throw std::runtime_error("invalid bonus amount");
}
}
