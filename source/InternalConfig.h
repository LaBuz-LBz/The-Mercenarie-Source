#pragma once

// Internal gameplay configuration. These values preserve the existing balance;
// Options.ini remains reserved for player-facing preferences.
namespace MercenarieConfig {
namespace Contracts {
static const int EscortBaseCats=2000,EscortCatsPerKm=100,EscortMinimumCats=1000;
// Caravan and science store the travelled round trip (2x displayed distance),
// therefore 50 technical Cats/km is economically 100 Cats/displayed km.
static const int CaravanBaseCats=4000,CaravanCatsPerKm=50,CaravanMinimumCats=2000;
static const int ScienceBaseCats=3750,ScienceCatsPerKm=50,ScienceMinimumCats=3000;
static const int MailBaseCats=500,MailCatsPerKm=25,MailMinimumCats=750;
static const int NegotiationMinPercent=-20,NegotiationMaxPercent=25;
static const int AbsoluteCatsCap=100000,GuildHouseBonusPercent=20,MaxCaravanMembers=8,MaxEnvironmentalBonusCats=2000;
static const float HalfAdvance=.50f,GlobalReputationWeight=.003f,LocalReputationWeight=.005f,CounterOfferChance=.72f;
static const int MaxCounterOffers=1,UnconsciousCancelSeconds=1000,CarriedCancelSeconds=20;
}
namespace Reputation {
static const float SuccessBase=2.0f,GlobalSuccess=.35f,Failure=-8.0f,Death=-15.0f,Cage=-12.0f,Unconscious=-10.0f,GlobalFailureMultiplier=.20f;
static const float CancellationGlobal=-1.0f,CancellationLocal=-4.0f;
}
namespace Fiscal {
static const int UnitedCitiesRatePercent=20,MercenaryGuildRatePercent=10,AlliedRatePercent=5,RebelRatePercent=0;
}
namespace Progression {
static const int Version=4,MaxXp=16100;
// Cumulative XP thresholds. The save migration preserves the current level
// and the proportional progress within that level.
static const int LevelThresholds[]={0,300,800,1550,2550,3850,5500,7500,9900,12750,16100};
// Guild investment: indivisible Cats batches and XP per batch.
static const int CatsToXpCost=10000,CatsToXpReward=120;
}
namespace Bounties {
static const int UnlockLevel=1;
static const double BaseRaidDelaySeconds=180.0,DifficultyRaidDelaySeconds=120.0;
static const double BoardRefreshHours=72.0;
static const int MinimumCats=7000,MaximumCats=50000;
}
}
