#pragma once
#include "CaravanHomeAmbushCombat.h"
bool homeBanditTemplate(GameData* squad){
    return squad&&squad->getListSize("animals")==0&&regionalAmbushComposition(squad,2);
}
bool spawnCaravanHomeBandits(Character* guard,Building* home){
    if(!guard||!home||home->isDestroyed()||!guard->getMovement()->isIndoors()||guard->getMovement()->building.toString()!=home->getHandle().toString()||!ou||!ou->theFactory||!ou->factionMgr||!escort)return false;
    std::vector<RegionalAmbushOption> regional,options;
    regionalAmbushOptions(NativeRegionLookup::at(guard->getPosition()),contractOriginFaction,2,regional);
    for(size_t i=0;i<regional.size();++i)if(homeBanditTemplate(regional[i].squad))options.push_back(regional[i]);
    // Villages without a regional human enemy can still host ordinary bandits.
    // Use real, non-unique loaded templates, never guessed record identifiers.
    if(options.empty()){
        lektor<GameData*> squads;ou->gamedata.getDataOfType(squads,SQUAD_TEMPLATE);
        for(unsigned int i=0;i<squads.size();++i){GameData* squad=squads[i];if(!squad)continue;
            std::string name=squad->name;for(size_t j=0;j<name.size();++j)name[j]=(char)tolower((unsigned char)name[j]);
            if(name.find("bandit")==std::string::npos||!homeBanditTemplate(squad))continue;
            Faction* faction=ou->factionMgr->getFactionByStringID(squad->getFromList("faction",0));
            if(!faction||faction==escort->getFaction()||faction==contractOriginFaction||(ou->player&&faction==ou->player->participant))continue;
            options.push_back(RegionalAmbushOption(squad,faction,1));
        }
    }
    if(options.empty())return false;
    int weight=0;for(size_t i=0;i<options.size();++i)weight+=options[i].weight;
    size_t pick=regionalAmbushPick(options,UtilityT::randomInt(0,weight-1));if(pick>=options.size())return false;
    AmbushSpawnPlan::Plan plan;std::vector<ContractGroupPlan::Binding> bindings;std::string reason;
    if(!prepareAmbushPlan(options[pick].squad,2,plan,bindings,reason,10))return false;
    Platoon* attackers=0;
    try{
        ContractGroupPlan::ScopedCounts capped(bindings);
        // Spawn only at the reveal, using the confirmed interior floor and home.
        // No hostile actor exists during the preceding normal-looking delivery.
        attackers=ou->theFactory->createRandomSquad(options[pick].faction,guard->getPosition(),tradeTown(),1,home,options[pick].squad,0,0,0,false,hand(escort),tradeTown(),1.0f,SQ_ROAMING,false);
    }catch(...){ErrorLog("CARAVAN HOME AMBUSH factory failed");return false;}
    if(!attackers||!attackers->activePlatoon)return false;
    unsigned int count=attackers->activePlatoon->things.size();
    bool valid=count==10;
    for(unsigned int i=0;i<count;++i){Character* c=static_cast<Character*>(attackers->activePlatoon->things[i]);if(!c||c->isAnimal()||c->isPlayerCharacter())valid=false;}
    if(!valid){rejectOversizedAmbush(attackers,"house ambush requires ten human bandits");return false;}
    const AmbushBalance::Profile profile=AmbushBalance::profileForDifficulty(currentContract.dangerLevel);
    float playerDistance=0;Character* player=nearestPlayer(escort->getPosition(),playerDistance);
    for(unsigned int i=0;i<count;++i){Character* enemy=static_cast<Character*>(attackers->activePlatoon->things[i]);CharStats* stats=enemy->getStats();
        if(stats){float value=UtilityT::random((float)profile.minimum,(float)profile.maximum);
            stats->_strength=value;stats->_dexterity=value;stats->_toughness=value;stats->__meleeAttack=value;stats->_meleeDefence=value;stats->dodging=value;
            stats->katanas=value;stats->sabres=value;stats->hackers=value;stats->blunt=value;stats->heavyWeapons=value;stats->polearms=value;stats->unarmed=value;stats->perception=value;stats->bows=value;stats->turrets=value;}
        // Individual aggression leaves the world's faction relations intact.
        Character* target=(i%2&&player&&!player->isAnimal()&&playerDistance<2250000)?player:escort;
        enemy->attackTarget(target);
    }
    std::vector<Character*> enemies;
    for(unsigned int i=0;i<count;++i)enemies.push_back(static_cast<Character*>(attackers->activePlatoon->things[i]));
    rememberCaravanAmbushEnemies(enemies);
    ++journeyData.ambushes;
    DebugLog(std::string("CARAVAN HOME AMBUSH spawned count=10 home=")+home->getHandle().toString()+" squad="+options[pick].squad->stringID);
    return true;
}
