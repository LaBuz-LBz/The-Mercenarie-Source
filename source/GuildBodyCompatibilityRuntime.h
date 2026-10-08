#include "src/FemaleBodyCompatibility.h"

// RE_Kenshi loads the plugin after the active FCS data. Use the effective race
// mesh, not an installed folder or mods.cfg, so a disabled body mod does nothing.
void applyGuildBodyCompatibility(){
    static bool checked=false;
    if(checked||!ou)return;
    lektor<GameData*> races;ou->gamedata.getDataOfType(races,RACE);
    if(races.size()==0)return;
    checked=true;
    bool hd=false;
    for(unsigned int i=0;i<races.size();++i){
        if(!races[i])continue;
        const auto it=races[i]->filesdata.find("female mesh");
        if(it!=races[i]->filesdata.end()&&FemaleBodyCompatibility::supportedBody(it->second)){hd=true;break;}
    }
    if(!hd){DebugLog("Mercenarie body compatibility: mode=default; female pants unchanged");return;}
    const FemaleBodyCompatibility::Variant* v=FemaleBodyCompatibility::variants();
    GameData* items[2]={0,0};
    // Prepare every string before touching native data; never overwrite a mesh
    // replaced by another equipment mod, or use an absent compatibility asset.
    std::string prepared[2];
    for(int i=0;i<2;++i){
        items[i]=ou->gamedata.getData(v[i].item,ARMOUR);
        if(!items[i]){ErrorLog("Mercenarie body compatibility: missing guild pants record; default preserved");return;}
        const auto it=items[i]->filesdata.find("mesh female");
        if(it==items[i]->filesdata.end()||!FemaleBodyCompatibility::mayReplace(it->second,v[i])){
            ErrorLog("Mercenarie body compatibility: external pants override; both meshes preserved");return;
        }
        if(!std::ifstream(v[i].fitted,std::ios::binary).good()){
            ErrorLog("Mercenarie body compatibility: missing fitted asset; default preserved");return;
        }
        prepared[i]=v[i].fitted;
    }
    for(int i=0;i<2;++i)items[i]->filesdata.find("mesh female")->second.swap(prepared[i]);
    DebugLog("Mercenarie body compatibility: mode=nude-hd-female; guild=1 service=1; male/body/save data unchanged");
}
