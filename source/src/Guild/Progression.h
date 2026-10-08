#pragma once
#include <algorithm>
#include <cmath>
#include <climits>
#include "../../InternalConfig.h"
// Pure progression/reward rules. No runtime, UI or persistence dependency.
namespace GuildProgression {
inline int officeLimit(int level){return level>=4?3:level>=3?1:0;}

static const int Version=MercenarieConfig::Progression::Version,MaxXp=MercenarieConfig::Progression::MaxXp;
inline int threshold(int n){return MercenarieConfig::Progression::LevelThresholds[std::max(0,std::min(10,n))];}
inline int level(int xp){int n=0;while(n<10&&xp>=threshold(n+1))++n;return n;}
inline bool bountyUnlocked(int guildLevel){return guildLevel>=MercenarieConfig::Bounties::UnlockLevel;}
inline int investmentXp(int cats){
    const int cost=MercenarieConfig::Progression::CatsToXpCost;
    if(cost<=0||cats<cost||cats%cost)return 0;
    long long xp=(long long)(cats/cost)*MercenarieConfig::Progression::CatsToXpReward;
    return xp>0&&xp<=INT_MAX?(int)xp:0;
}
inline bool canInvest(int cats,int available){return cats==MercenarieConfig::Progression::CatsToXpCost&&cats<=available&&investmentXp(cats)>0;}
inline bool investmentReady(double now,double next){return now==now&&next==next&&now>=0&&now>=next;}
inline int delegatedXp(int normal){return (std::max(0,normal)*60+50)/100;}
inline int migrate(int xp,int version=1){static const int v1[]={0,100,300,750,1350,2150,3150,4400,5900,7700,10000};static const int v2[]={0,100,300,650,1200,2000,3100,4600,6600,9200,12500};static const int v3[]={0,600,1600,3200,5500,8500,12500,17500,24000,32000,42000};const int* old=version==3?v3:version==2?v2:v1;xp=std::max(0,xp);if(xp>=old[10])return MaxXp;int n=0;while(n<9&&xp>=old[n+1])++n;return threshold(n)+(int)((long long)(xp-old[n])*(threshold(n+1)-threshold(n))/(old[n+1]-old[n]));}
inline void award(int& xp,int& prestige,int amount){long long total=(long long)std::max(0,xp)+std::max(0,amount);long long extra=std::max(0LL,total-MaxXp);xp=(int)std::min((long long)MaxXp,total);prestige=(int)std::min((long long)INT_MAX,(long long)std::max(0,prestige)+extra);}
inline float clampRep(float r){return r!=r?0:std::max(-100.0f,std::min(100.0f,r));}
inline float trust(float local,float global){return clampRep(.7f*clampRep(local)+.3f*clampRep(global));}
inline float chanceBonus(float local,float global){return trust(local,global)/1000.0f;}
inline float visitorRate(float local,float global){return 1+trust(local,global)/400.0f;}
struct Result {int base,distance,quality,xp,local,global;Result():base(0),distance(0),quality(0),xp(0),local(0),global(0){}};
inline Result success(int type,float km,int danger,int losses,bool noKo,bool cargo){
 Result r;static const double mult[]={1,1.10,1.25,1.50,1.75};r.base=type==1?45:type==2?60:type==4?25:35;
 r.distance=km!=km||km<0?0:km>=200?40:(int)(km/5);losses=std::max(0,losses);
 r.quality=(losses==0?10:0)+(noKo?5:0)+(type==1&&cargo?10:0);
 r.xp=(int)floor((r.base+r.distance)*mult[std::max(1,std::min(5,danger))-1]+r.quality+.5);
 r.local=3+(losses==0?1:0)+(noKo?1:0)+(type==1&&cargo?1:0)-std::min(6,losses*2);r.global=1-(losses>0?1:0);return r;
}
inline Result failure(bool abandonment){Result r;r.local=abandonment?-4:-8;r.global=abandonment?-1:-2;return r;}
}
