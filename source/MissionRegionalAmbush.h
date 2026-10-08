#pragma once
// Included in the mission runtime after the native composition validators.
struct RegionalAmbushOption {
    GameData* squad;Faction* faction;int weight;
    RegionalAmbushOption(GameData* s,Faction* f,int w):squad(s),faction(f),weight(w){}
};
bool regionalAmbushComposition(GameData* squad,int difficulty){
    if(!squad||squad->type!=SQUAD_TEMPLATE)return false;
    // These secondary pools are not counted by the existing bounded spawner.
    const char* extra[]={"squad2","animals2","slaves"};
    for(int i=0;i<3;++i)if(squad->getListSize(extra[i])>0)return false;
    const char* randomCounts[]={"num random chars","num random chars max"};
    for(int i=0;i<2;++i){auto n=squad->idata.find(randomCounts[i]);if(n!=squad->idata.end()&&n->second!=0)return false;}
    auto trader=squad->bdata.find("is trader");if(trader!=squad->bdata.end()&&trader->second)return false;
    const char* lists[]={"leader","squad","animals"};
    for(int l=0;l<3;++l){
        const Ogre::vector<GameDataReference>::type* refs=squad->getReferenceListIfExists(lists[l]);
        if(!refs)continue;if(refs->size()>64)return false;
        for(size_t i=0;i<refs->size();++i){
            GameData* actor=(*refs)[i].sid.empty()?0:(*refs)[i].getPtr(squad->getSourceContainer());
            if(!actor||actor->type!=CHARACTER)return false;
            auto unique=actor->bdata.find("unique");if(unique!=actor->bdata.end()&&unique->second)return false;
        }
    }
    const Ogre::vector<GameDataReference>::type* states=squad->getReferenceListIfExists("world state");
    if(states&&!states->empty()){
        if(states->size()>64)return false;
        for(size_t i=0;i<states->size();++i){
            GameData* state=(*states)[i].sid.empty()?0:(*states)[i].getPtr(squad->getSourceContainer());
            if(!state||state->type!=WORLD_EVENT_STATE)return false;
        }
        if(!WorldEventStateQuery::checkAllStatesInObject(squad,"world state"))return false;
    }
    std::string reason;AmbushSpawnPlan::Plan plan;std::vector<ContractGroupPlan::Binding> bindings;
    return usableAmbushTemplate(squad,reason)&&prepareAmbushPlan(squad,difficulty,plan,bindings,reason);
}
void regionalAmbushOptions(GameData* biome,Faction* origin,int difficulty,std::vector<RegionalAmbushOption>& out){
    out.clear();
    if(!biome||biome->type!=BIOME_GROUP||!origin||!origin->relations||!ou||!ou->factionMgr)return;
    const Ogre::vector<GameDataReference>::type* refs=biome->getReferenceListIfExists("homeless spawns");
    if(!refs||refs->size()>1024)return;
    for(size_t i=0;i<refs->size();++i){
        const GameDataReference& ref=(*refs)[i];
        if(ref.sid.empty()||ref.values.value[0]<=0)continue;
        GameData* squad=ref.getPtr(biome->getSourceContainer());
        if(!squad||squad->type!=SQUAD_TEMPLATE)continue;
        const Ogre::vector<GameDataReference>::type* factions=squad->getReferenceListIfExists("faction");
        if(!factions||factions->size()!=1||(*factions)[0].sid.empty())continue;
        Faction* faction=ou->factionMgr->getFactionByStringID((*factions)[0].sid);
        if(!faction||!origin->relations->isEnemy(faction)||!regionalAmbushComposition(squad,difficulty))continue;
        out.push_back(RegionalAmbushOption(squad,faction,std::min(10000,ref.values.value[0])));
    }
}
// One bounded weighted draw, using the region's relative spawn frequencies.
size_t regionalAmbushPick(const std::vector<RegionalAmbushOption>& options,int ticket){
    for(size_t i=0;i<options.size();++i){if(ticket<options[i].weight)return i;ticket-=options[i].weight;}
    return options.size();
}
