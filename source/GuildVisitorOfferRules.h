#pragma once
#include <vector>

namespace GuildVisitorOfferRules {
enum Profile { Civilian=0, Merchant=1, Soldier=2, Family=3, Scientist=4, Mercenary=5, Noble=6 };
enum Type { Escort=0, Caravan=1, Science=2, Mail=4 };

inline int weight(int profile,int type)
{
    static const int weights[7][4]={
        {50,10,10,30}, // civilian
        {20,40, 5,35}, // merchant
        {60,25, 5,10}, // soldier
        {60, 5, 5,30}, // family
        {20, 5,65,10}, // scientist
        {65,20, 5,10}, // mercenary
        {50,20,10,20}  // noble
    };
    if(profile<0)profile=0;else if(profile>6)profile=6;
    const int column=type==Escort?0:type==Caravan?1:type==Science?2:3;
    return weights[profile][column];
}

inline int choose(int profile,const std::vector<int>& recent,int roll)
{
    const int types[4]={Escort,Caravan,Science,Mail};int adjusted[4],total=0;
    for(int i=0;i<4;++i){adjusted[i]=weight(profile,types[i]);if(!recent.empty()&&recent.back()==types[i]){adjusted[i]/=5;if(adjusted[i]<1)adjusted[i]=1;}else if(recent.size()>1&&recent[recent.size()-2]==types[i]){adjusted[i]/=2;if(adjusted[i]<1)adjusted[i]=1;}total+=adjusted[i];}
    int cursor=total?roll%total:0;for(int i=0;i<4;++i){if(cursor<adjusted[i])return types[i];cursor-=adjusted[i];}return Escort;
}

inline void remember(std::vector<int>& recent,int type){recent.push_back(type);if(recent.size()>3)recent.erase(recent.begin());}
inline bool giverMayDepart(int type){return type==Escort||type==Caravan||type==Science||type==Mail;}
}
