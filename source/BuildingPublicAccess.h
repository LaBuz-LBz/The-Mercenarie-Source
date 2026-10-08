#pragma once
#include "CleanupState.h"
// Ownership transfers deliberately preserve resident actors. A former owner's
// shop-closing flag must not override the player's native publicDaytime toggle.
// Keep this query repair independent of the estate option: sold buildings remain
// player-owned after disabling it. No resident, lease or save data is rewritten.
namespace {
bool (*buildingPublicOriginal)(Building*)=0;
bool buildingPublicHook(Building* building)
{
    const bool native=buildingPublicOriginal(building);
    if(MercenarieCleanup::disabled)return native;
    if(!building||!building->isThePlayer()||building->isFurnitureOrDoor()||building->isGate())return native;
    Platoon* resident=building->residentSquad.getPlatoon();
    if(!resident||!resident->getFaction()||resident->getFaction()==building->getFaction())return native;
    return building->publicDaytime;
}
void installBuildingPublicAccess()
{
    if(KenshiLib::AddHook(KenshiLib::GetRealAddress(&Building::_NV_isPublic),&buildingPublicHook,&buildingPublicOriginal)!=KenshiLib::SUCCESS)
        ErrorLog("Building public access: native query repair hook failed");
}
}

