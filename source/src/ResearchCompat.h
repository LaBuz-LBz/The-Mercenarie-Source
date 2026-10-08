#pragma once
#include <cstddef>

#include <kenshi/Enums.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/lektor.h>

class GameData;

class ResearchItem
{
public:
    GameData* gamedata;
    float unknown08;
    int unknown0C;
};

// Runtime layout used by Kenshi 1.0.65. The artisan adapter only reads it;
// Kenshi remains responsible for constructing and serialising research state.
class Research
{
public:
    class BuildingUpgrades
    {
    public:
        float productionMult;
        float powerOutput;
        float powerCapacity;
        BuildingUpgrades():productionMult(1.0f),powerOutput(0),powerCapacity(0){}
    };

    size_t unknown00;
    int techLevel;
    bool changedSoUpdateGUI;
    // Native known-object query (Steam 1.0.65 RVA 0x82e430) uses this map,
    // NOT enabledObjects. Rebuilt from completed research by 0x8324e0.
    Ogre::map<itemType, lektor<GameData*> >::type knownObjectsByType;
    Ogre::deque<ResearchItem>::type researchQueue;
    Ogre::map<GameData*, BuildingUpgrades>::type buildingUpgradeResearchs;
    Ogre::map<std::string, Ogre::vector<ResearchItem>::type>::type unknown98;
    lektor<std::string> unknownC0;
    lektor<std::string> paid;
    ogre_unordered_set<GameData*>::type finished;
    ogre_unordered_set<GameData*>::type enabledObjects;
    int maxTechLevel;
    int unknown174;
    float unknown178;
};

// Match the audited native member access offsets without changing the ABI.
typedef char ResearchKnownObjectsOffset[(offsetof(Research,knownObjectsByType)==0x10)?1:-1];
typedef char ResearchFinishedOffset[(offsetof(Research,finished)==0xf0)?1:-1];
typedef char ResearchEnabledObjectsOffset[(offsetof(Research,enabledObjects)==0x130)?1:-1];
