#include "Localization.h"
#pragma once
float rollFinalBonus(){return UtilityT::random(0.0f,1.0f);}
void refreshFinalBonuses(){
    if(!finalWindow||!reportBefore)return;
    reportReadOnly=false;finalReputationButton->setEnabled(true);MercenarieFonts::caption(reportTotalLabel,registerLanguage(Loc::text("ui.projected_total"),Loc::text("ui.projected_total")));
    const bool unlocked=guildLevel()>=1;
    const unsigned int mask=unlocked?finalBonusChoice.selected:0;
    const int count=finalBonusChoice.count(mask),cost=MissionBonuses::xpCost(pendingGuildXp,count),extra=finalBonusChoice.total(mask);
    int previewXp=guildPoints,previewPrestige=guildPrestige;GuildProgression::award(previewXp,previewPrestige,pendingGuildXp-cost);
    int baseXp=guildPoints,basePrestige=guildPrestige;GuildProgression::award(baseXp,basePrestige,pendingGuildXp);
    double local=GuildProgression::clampRep(localReputation()+progressReportLocal-count)-localReputation();
    double global=GuildProgression::clampRep(escortReputation+progressReportGlobal)-escortReputation;
    int gross=missionReward+finalClientTip+extra;int pacePenalty=EscortMissionRules::forcedPacePenalty(gross,missionForcedPace);int payable=std::max(0,gross-pacePenalty-advancePaid);
    setMissionIconV6(reportMissionIcon,(int)currentContract.type);MercenarieFonts::caption(reportType,missionLabelV6((int)currentContract.type));reportType->setTextColour(missionColourV6((int)currentContract.type));
    MercenarieFonts::caption(reportFields[0],mercenarieLocalize(originCity)+" > "+mercenarieLocalize(destinationName)+(caravanMission||scientificMission?" > "+mercenarieLocalize(originCity):""));
    MercenarieFonts::caption(reportFields[1],registerNumber(missionReward)+Loc::text("ui.cats"));MercenarieFonts::caption(reportFields[2],registerNumber(advancePaid)+Loc::text("ui.cats"));
    std::ostringstream distance;distance.precision(3);distance<<std::fixed<<journeyData.distanceTravelled/1000.0f<<Loc::text("ui.km");std::string km=distance.str();if(!gMercenarieEnglish)std::replace(km.begin(),km.end(),'.',',');MercenarieFonts::caption(reportFields[3],km);
    int minutes=std::max(0,(int)(reportStartHour>=0&&reportEndHour>=reportStartHour?(reportEndHour-reportStartHour)*60:missionElapsed/60));std::ostringstream duration;duration<<minutes/1440<<registerLanguage(Loc::text("ui.d_f380c84"),Loc::text("ui.d_f380c84"))<<(minutes/60)%24<<Loc::text("ui.h_db20fc1")<<minutes%60<<Loc::text("ui.min_f4e8a8b");if(reportStartHour<0)duration<<registerLanguage(Loc::text("ui.tracked"),Loc::text("ui.tracked"));MercenarieFonts::caption(reportFields[4],duration.str());
    MercenarieFonts::caption(reportFields[5],registerNumber(currentContract.dangerLevel)+"/5");
    int members=progressRosterKnown?(int)progressMembers.size():currentContract.groupSize;MercenarieFonts::caption(reportFields[6],registerNumber(members)+registerLanguage(Loc::text("ui.including_1_leader"),Loc::text("ui.including_1_leader")));
    for(int i=0;i<5;++i)reportSkulls[i]->setVisible(true);
    for(int i=0;i<5;++i)reportSkulls[i]->setColour(i<currentContract.dangerLevel?MyGUI::Colour(.8f,.08f,.17f):MyGUI::Colour(.28f,.32f,.36f));
    MercenarieFonts::caption(reportGains[0],registerNumber(missionReward)+Loc::text("ui.cats"));MercenarieFonts::caption(reportGains[1],registerNumber(extra)+Loc::text("ui.cats"));MercenarieFonts::caption(reportGains[2],registerNumber(payable)+Loc::text("ui.cats_3f70b99"));
    MercenarieFonts::caption(reportGains[3],reportSigned(previewXp-guildPoints));MercenarieFonts::caption(reportGains[4],reportSigned(local));MercenarieFonts::caption(reportGains[5],reportSigned(global));MercenarieFonts::caption(reportGains[6],reportSigned(previewPrestige-guildPrestige));
    MercenarieFonts::caption(reportCount,registerLanguage(Loc::text("ui.bonuses_selected"),Loc::text("ui.bonuses_selected"))+registerNumber(count)+"/"+registerNumber(finalBonusChoice.count(255)));
    MercenarieFonts::caption(reportHint,unlocked?registerLanguage(Loc::text("ui.select_the_bonuses_you_wish_to_request"),Loc::text("ui.select_the_bonuses_you_wish_to_request")):registerLanguage(Loc::text("ui.bonus_requests_unlock_at_guild_level_1"),Loc::text("ui.bonus_requests_unlock_at_guild_level_1")));
    for(int i=0;i<MissionBonuses::Count;++i){
        bool earned=finalBonusChoice.amounts[i]>0,available=earned&&unlocked,selected=(mask&(1u<<i))!=0;
        MercenarieFonts::caption(finalBonusButtons[i],"");finalBonusButtons[i]->setEnabled(available);finalBonusButtons[i]->setStateSelected(selected);
        reportBonusSelection92(i,selected);
        const MyGUI::Colour color=available?MyGUI::Colour(.88f,.68f,.39f):MyGUI::Colour(.44f,.47f,.49f);
        reportIcons[i]->setColour(color);reportNames[i]->setTextColour(available?registerIvory:MyGUI::Colour(.58f,.61f,.64f));reportDescriptions[i]->setTextColour(available?MyGUI::Colour(.73f,.76f,.78f):MyGUI::Colour(.5f,.53f,.56f));
        MercenarieFonts::caption(reportAmounts[i],earned?(unlocked?"+ "+registerNumber(finalBonusChoice.amounts[i])+Loc::text("ui.cats_3f70b99"):registerLanguage(Loc::text("ui.level_1_required"),Loc::text("ui.level_1_required"))):registerLanguage(Loc::text("ui.not_earned"),Loc::text("ui.not_earned")));reportAmounts[i]->setTextColour(available?registerAmber:MyGUI::Colour(.58f,.61f,.64f));
        for(int e=0;e<4;++e)reportEdges[i][e]->setColour(available?registerAmber:MyGUI::Colour(.38f,.41f,.43f));
        fitRegisterText(reportNames[i],rf(18));fitRegisterText(reportDescriptions[i],rf(14));fitRegisterText(reportAmounts[i],rf(available?19:16));
    }
    std::ostringstream before;before<<registerLanguage(Loc::text("ui.reward_before_forced_pace_penalty"),Loc::text("ui.reward_before_forced_pace_penalty"))<<registerNumber(gross)<<Loc::text("ui.cats_7132fc0");if(missionForcedPace)before<<"\n"<<registerLanguage(Loc::text("ui.forced_pace_penalty"),Loc::text("ui.forced_pace_penalty"))<<"-10% (-"<<registerNumber(pacePenalty)<<Loc::text("ui.cats_7132fc0")<<")";before<<"\n"<<registerLanguage(Loc::text("ui.guild_xp"),Loc::text("ui.guild_xp"))<<reportSigned(baseXp-guildPoints)<<"\n"<<registerLanguage(Loc::text("ui.local_reputation_b4a2c1a"),Loc::text("ui.local_reputation_b4a2c1a"))<<reportSigned(GuildProgression::clampRep(localReputation()+progressReportLocal)-localReputation());MercenarieFonts::caption(reportBefore,before.str());
    std::ostringstream preview;preview<<registerLanguage(Loc::text("ui.if_all_bonuses_are_granted"),Loc::text("ui.if_all_bonuses_are_granted"))<<"+"<<registerNumber(extra)<<Loc::text("ui.cats_7132fc0")<<registerLanguage(Loc::text("ui.xp_after_bonuses"),Loc::text("ui.xp_after_bonuses"))<<reportSigned(previewXp-guildPoints)<<registerLanguage(Loc::text("ui.local_reputation_87d5d01"),Loc::text("ui.local_reputation_87d5d01"))<<reportSigned(local)<<"\n"<<registerLanguage(Loc::text("ui.global_reputation_aaa7942"),Loc::text("ui.global_reputation_aaa7942"))<<reportSigned(global)<<"\n"<<registerLanguage(Loc::text("ui.advance_deducted_included_tip"),Loc::text("ui.advance_deducted_included_tip"))<<registerNumber(finalClientTip)<<Loc::text("ui.cats");MercenarieFonts::caption(finalBonusPreview,preview.str());
    std::ostringstream after;after<<registerLanguage(Loc::text("ui.payout"),Loc::text("ui.payout"))<<registerNumber(payable)<<Loc::text("ui.cats_7132fc0")<<registerLanguage(Loc::text("ui.xp"),Loc::text("ui.xp"))<<reportSigned(previewXp-guildPoints)<<registerLanguage(Loc::text("ui.prestige_e9bab8f"),Loc::text("ui.prestige_e9bab8f"))<<reportSigned(previewPrestige-guildPrestige)<<"\n"<<registerLanguage(Loc::text("ui.local_rep"),Loc::text("ui.local_rep"))<<reportSigned(local)<<registerLanguage(Loc::text("ui.global"),Loc::text("ui.global"))<<reportSigned(global);MercenarieFonts::caption(reportAfter,after.str());
    for(int i=0;i<7;++i){fitRegisterText(reportFields[i],rf(17));fitRegisterText(reportGains[i],rf(i==2?26:18));}
    finalCashButton->setEnabled(unlocked&&count>0);MercenarieFonts::caption(finalCashButton,registerLanguage(Loc::text("ui.request_selected_bonuses"),Loc::text("ui.request_selected_bonuses")));
    finalHalfCashButton->setEnabled(unlocked&&finalBonusChoice.count(255)>0);MercenarieFonts::caption(finalHalfCashButton,count>0&&count==finalBonusChoice.count(255)?r88("Tout d\303\251cocher","Clear all"):r88("Tout cocher","Select all"));
    MercenarieFonts::caption(finalReputationButton,registerLanguage(Loc::text("ui.validate_without_bonuses"),Loc::text("ui.validate_without_bonuses")));
    refreshReport88(payable,extra,pacePenalty);
}
void toggleFinalBonus(MyGUI::WidgetPtr sender){
    if(guildLevel()<1)return;
    for(int i=0;i<MissionBonuses::Count;++i)if(sender==finalBonusButtons[i]&&finalBonusChoice.amounts[i]>0)finalBonusChoice.selected^=1u<<i;
    refreshFinalBonuses();
}
void prepareFinalBonuses(){
    finalBonusChoice.reset();
    const int rewardBasis=currentContract.reward.finalBeforeMissionBonuses>0?currentContract.reward.finalBeforeMissionBonuses:missionReward;
    const int rewardPercent=ContractRewards::snapshot(currentContract.routeRegions);
    if(finalFast)finalBonusChoice.amounts[MissionBonuses::Fast]=rewardBasis/8;
    if(!progressAnyClientKo&&!escortWasKnockedOut&&progressRosterKnown)finalBonusChoice.amounts[MissionBonuses::NoKO]=rewardBasis/10;
    if(finalHealthy)finalBonusChoice.amounts[MissionBonuses::Healthy]=rewardBasis/7;
    if(journeyData.ambushes>0)finalBonusChoice.amounts[MissionBonuses::Ambush]=rewardBasis/8;
    if(journeyData.detours>0)finalBonusChoice.amounts[MissionBonuses::Detour]=rewardBasis/10;
    if(journeySeconds>0&&proximitySeconds/journeySeconds>=.70f)finalBonusChoice.amounts[MissionBonuses::Proximity]=std::max(200,rewardBasis/12);
    ContractFactors::Quote cargo;
    if(caravanMission&&progressDeadMembers.empty()&&ContractFactors::decode(currentContract.routeRegions,cargo)&&cargo.count>0&&countContractCargo(caravanMembers,cargo.item)>=cargo.count)
        finalBonusChoice.amounts[MissionBonuses::Cargo]=std::max(300,caravanCargoValue/12);
    if(scientificInsideDiscovery)finalBonusChoice.amounts[MissionBonuses::Discovery]=std::max(500,rewardBasis/5);
    for(int i=0;i<MissionBonuses::Count;++i)finalBonusChoice.amounts[i]=ContractRewards::apply(finalBonusChoice.amounts[i],rewardPercent);
    finalBonusChoice.cap(ContractRewards::apply(rewardBasis/2,rewardPercent));earnedFinalBonus=finalBonusChoice.total(255);earnedFinalBonusCount=finalBonusChoice.count(255);
}

