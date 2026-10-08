#pragma once
#include <vector>

namespace MissionOfferViewRules {
inline bool needsQuestPreparation(bool missionManagementContext){return !missionManagementContext;}

template<class Offer>
inline std::vector<Offer> snapshot(const std::vector<Offer>& sharedPool){return sharedPool;}

template<class Offer,size_t N>
inline std::vector<Offer> snapshot(const Offer (&sharedPool)[N]){return std::vector<Offer>(sharedPool,sharedPool+N);}
}
