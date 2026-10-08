#pragma once
#include "../InternalConfig.h"
namespace EscortConfig {
static const int EscortBase=MercenarieConfig::Contracts::EscortBaseCats,EscortPerKm=MercenarieConfig::Contracts::EscortCatsPerKm,EscortMinimum=MercenarieConfig::Contracts::EscortMinimumCats;
static const int CaravanBase=MercenarieConfig::Contracts::CaravanBaseCats,CaravanPerKm=MercenarieConfig::Contracts::CaravanCatsPerKm,CaravanMinimum=MercenarieConfig::Contracts::CaravanMinimumCats;
static const int ScienceBase=MercenarieConfig::Contracts::ScienceBaseCats,SciencePerKm=MercenarieConfig::Contracts::ScienceCatsPerKm,ScienceMinimum=MercenarieConfig::Contracts::ScienceMinimumCats;
static const int MailBase=MercenarieConfig::Contracts::MailBaseCats,MailPerKm=MercenarieConfig::Contracts::MailCatsPerKm,MailMinimum=MercenarieConfig::Contracts::MailMinimumCats;
static const int NegotiationMinPercent=MercenarieConfig::Contracts::NegotiationMinPercent,NegotiationMaxPercent=MercenarieConfig::Contracts::NegotiationMaxPercent,AbsoluteContractCap=MercenarieConfig::Contracts::AbsoluteCatsCap,GuildHouseBonusPercent=MercenarieConfig::Contracts::GuildHouseBonusPercent,MaxCaravanMembers=MercenarieConfig::Contracts::MaxCaravanMembers,MaxEnvironmentalBonus=MercenarieConfig::Contracts::MaxEnvironmentalBonusCats;
static const bool LegendaryEnabled=false;static const float HalfAdvance=MercenarieConfig::Contracts::HalfAdvance,GlobalReputationWeight=MercenarieConfig::Contracts::GlobalReputationWeight,LocalReputationWeight=MercenarieConfig::Contracts::LocalReputationWeight,CounterOfferChance=MercenarieConfig::Contracts::CounterOfferChance;
static const int MaxCounterOffers=MercenarieConfig::Contracts::MaxCounterOffers,UnconsciousCancelSeconds=MercenarieConfig::Contracts::UnconsciousCancelSeconds,CarriedCancelSeconds=MercenarieConfig::Contracts::CarriedCancelSeconds;
static const float ReputationSuccessBase=MercenarieConfig::Reputation::SuccessBase,ReputationGlobalSuccess=MercenarieConfig::Reputation::GlobalSuccess,ReputationFailure=MercenarieConfig::Reputation::Failure,ReputationDeath=MercenarieConfig::Reputation::Death,ReputationCage=MercenarieConfig::Reputation::Cage,ReputationUnconscious=MercenarieConfig::Reputation::Unconscious,GlobalFailureMultiplier=MercenarieConfig::Reputation::GlobalFailureMultiplier;
}
