#pragma once
#include <string>
#include <algorithm>

namespace FemaleBodyCompatibility {
inline std::string normalized(std::string path){
    for(size_t i=0;i<path.size();++i){if(path[i]=='\\')path[i]='/';if(path[i]>='A'&&path[i]<='Z')path[i]+='a'-'A';}
    while(path.compare(0,2,"./")==0)path.erase(0,2);
    return path;
}
inline bool supportedBody(const std::string& mesh){
    const std::string p=normalized(mesh);
    return p=="mods/nude mod hd female only/character/meshes/human/human_female.mesh" ||
           p=="mods/nude mod hd female only/character/meshes/bone/bone_female.mesh";
}
struct Variant {const char* item;const char* original;const char* fitted;};
inline const Variant* variants(){
    static const Variant v[]={
        {"910004-Holy Nation Mercenary Plastron.mod","./mods/Guild Escort Contracts/meshes/guild_pants_female.mesh","./mods/Guild Escort Contracts/meshes/guild_pants_female_hd.mesh"},
        {"980003-Holy Nation Mercenary Plastron.mod","./data/items/armour/meshes/cargopants_f.mesh","./mods/Guild Escort Contracts/meshes/guild_service_pants_female_hd.mesh"}
    };return v;
}
inline bool mayReplace(const std::string& current,const Variant& v){return normalized(current)==normalized(v.original);}
}
