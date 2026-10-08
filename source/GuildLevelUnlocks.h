#include "Localization.h"
#pragma once
#include "InternalConfig.h"
#include <vector>
#include <algorithm>
namespace GuildLevelUI {
struct Unlock {int level;const char* icon;const char* fr;const char* en;const char* detailFr;const char* detailEn;const char* noteFr;const char* noteEn;};
inline const std::vector<Unlock>& catalog(){
    static std::vector<Unlock> rows;
    static std::string locale;
    if(rows.empty()||locale!=Loc::engine().language){locale=Loc::engine().language;
        const Unlock values[]={
            {1,"LevelFeatureBonuses.png",Loc::text("ui.mission_bonuses"),Loc::text("ui.mission_bonuses"),Loc::text("ui.you_can_now_request_bonuses_at_the_end_of"),Loc::text("ui.you_can_now_request_bonuses_at_the_end_of"),Loc::text("ui.select_earned_bonuses_in_the_contract_report"),Loc::text("ui.select_earned_bonuses_in_the_contract_report")},
            {MercenarieConfig::Bounties::UnlockLevel,"LevelFeatureBonuses.png",Loc::text("ui.bounty_hunt"),Loc::text("ui.bounty_hunt"),Loc::text("guild.bounty_unlocked"),Loc::text("guild.bounty_unlocked"),Loc::text("guild.bounty_unlocked"),Loc::text("guild.bounty_unlocked")},
            {1,"LevelFeatureBonuses.png",Loc::text("artisan.unlock.title"),Loc::text("artisan.unlock.title"),Loc::text("artisan.unlock.body"),Loc::text("artisan.unlock.body"),"",""},
            {2,"LevelFeatureNegotiation.png",Loc::text("ui.contract_negotiation"),Loc::text("ui.contract_negotiation"),Loc::text("ui.negotiate_the_price_and_terms_of_your_contracts"),Loc::text("ui.negotiate_the_price_and_terms_of_your_contracts"),Loc::text("ui.discuss_your_payment_with_the_client"),Loc::text("ui.discuss_your_payment_with_the_client")},
            {3,"LevelFeatureHouse.png",Loc::text("guild.level3.house.title"),Loc::text("guild.level3.house.title"),Loc::text("guild.level3.house.body"),Loc::text("guild.level3.house.body"),"",""},
            {3,"LevelFeatureOffices.png",Loc::text("guild.level3.delegation.title"),Loc::text("guild.level3.delegation.title"),Loc::text("guild.level3.delegation.body"),Loc::text("guild.level3.delegation.body"),"",""},
            {4,"LevelFeatureOffices.png",Loc::text("ui.guild_house_network"),Loc::text("ui.guild_house_network"),Loc::text("ui.you_can_establish_two_additional_guild_houses"),Loc::text("ui.you_can_establish_two_additional_guild_houses"),Loc::text("ui.your_limit_increases_to_three_guild_houses_in_total"),Loc::text("ui.your_limit_increases_to_three_guild_houses_in_total")},
            {10,"LevelFeaturePrestige.png",Loc::text("ui.guild_prestige"),Loc::text("ui.guild_prestige"),Loc::text("ui.you_have_reached_the_maximum_guild_level"),Loc::text("ui.you_have_reached_the_maximum_guild_level"),Loc::text("ui.additional_xp_now_contributes_to_your_prestige"),Loc::text("ui.additional_xp_now_contributes_to_your_prestige")}
        };
        rows.assign(values,values+sizeof(values)/sizeof(values[0]));
    }
    return rows;
}
inline std::vector<Unlock> unlocks(int level){std::vector<Unlock> out;const std::vector<Unlock>& all=catalog();for(size_t i=0;i<all.size();++i)if(all[i].level==level)out.push_back(all[i]);return out;}
inline std::string level3Tooltip(){return std::string(Loc::text("guild.level3.title"))+"\n\n"+Loc::text("guild.level3.house.title")+"\n"+Loc::text("guild.level3.house.body")+"\n\n"+Loc::text("guild.level3.delegation.title")+"\n"+Loc::text("guild.level3.delegation.body");}
struct Transition{int oldLevel,newLevel;Transition(int a,int b):oldLevel(a),newLevel(b){}};
inline void enqueue(std::vector<Transition>& queue,int oldLevel,int newLevel){
    oldLevel=std::max(0,std::min(10,oldLevel));newLevel=std::max(0,std::min(10,newLevel));
    for(int level=oldLevel+1;level<=newLevel;++level){bool present=false;for(size_t i=0;i<queue.size();++i)if(queue[i].newLevel==level)present=true;if(!present)queue.push_back(Transition(level-1,level));}
}
struct Layout {
    int width,height,cardHeight,areaY,areaHeight,quoteY,buttonY;float scale;
    Layout(int screenW,int screenH,int count){
        scale=std::min(1.0f,std::min((screenW-28)/1380.0f,(screenH-28)/834.0f));
        width=(int)(1380*scale);cardHeight=(int)(144*scale);areaY=(int)(486*scale);
        int fixed=(int)(690*scale),cap=screenH-28;
        areaHeight=std::min(std::max(1,count)*cardHeight,std::max(cardHeight,cap-fixed));
        height=fixed+areaHeight;quoteY=areaY+areaHeight+(int)(14*scale);buttonY=height-(int)(94*scale);
    }
};
}
