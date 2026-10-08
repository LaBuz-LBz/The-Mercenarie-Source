#pragma once
namespace CommunityTranslations {
inline void discover(GameWorld* world){
    // The active list can be populated in stages. Cache roots in the registry,
    // not a permanent "complete" flag after the first non-empty list.
    // Rechecking paths costs no disk IO once these roots have been scanned.
    if(!world||world->activeMods.size()==0)return;
    Loc::registry().scan(Loc::engine().directory);
    for(size_t i=0;i<world->activeMods.size();++i){
        const ModInfo* mod=world->activeMods[static_cast<unsigned int>(i)];
        if(mod&&!mod->isBaseMod&&!mod->path.empty())
            Loc::registry().scan(mod->path+"/Localization");
    }
}
}