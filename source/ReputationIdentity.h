#pragma once
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <locale>
namespace ReputationIdentity {
inline std::string key(const std::string& name){
    static const char* aliases[][2]={
        {"Le Hub","The Hub"},{"Bout-du-Monde","World's End"},
        {"Village Flotsam","Flotsam"},{"Flotsam Village","Flotsam"},
        {"Colline aux Cloques","Blister Hill"},{"Mauvaises Dents","Bad Teeth"},
        {"Relais","Waystation"},{"Avant-poste","Waystation"},{"Griffure Noire","Black Scratch"},
        {"Village de p\xC3\xAA" "cheurs","Fishing Village"},{"Village de pecheurs","Fishing Village"},
        {"Village P\xC3\xAA" "cheurs","Fishing Village"},{"Village Pecheurs","Fishing Village"}
    };
    for(size_t i=0;i<sizeof(aliases)/sizeof(aliases[0]);++i)if(name==aliases[i][0])return aliases[i][1];
    // Unknown/modded names and old region records remain byte-for-byte stable.
    return name;
}
template<class Memory> void migrate(std::map<std::string,float>& reps,std::map<std::string,Memory>& memories,std::vector<std::string>& archived){
    const std::map<std::string,float> original=reps;
    const std::map<std::string,Memory> oldMemories=memories;
    std::map<std::string,std::vector<std::string> > groups;
    for(std::map<std::string,float>::const_iterator i=original.begin();i!=original.end();++i)groups[key(i->first)].push_back(i->first);
    for(std::map<std::string,std::vector<std::string> >::const_iterator group=groups.begin();group!=groups.end();++group){
        const std::string& canonical=group->first;const std::vector<std::string>& names=group->second;
        std::string chosen=original.count(canonical)?canonical:names[0];
        // Keep an existing canonical nonzero score. An empty default must not
        // hide an earned legacy score. Never add potentially overlapping scores.
        if(original.find(chosen)->second==0)for(size_t n=0;n<names.size();++n)if(original.find(names[n])->second!=0){chosen=names[n];break;}
        for(size_t n=0;n<names.size();++n){
            const std::string& name=names[n];Memory memory;typename std::map<std::string,Memory>::const_iterator m=oldMemories.find(name);if(m!=oldMemories.end())memory=m->second;
            if(names.size()>1||name!=canonical){std::ostringstream row;row.imbue(std::locale::classic());row<<name<<'|'<<original.find(name)->second<<'|'<<memory.abuses<<'|'<<memory.recovery;archived.push_back(row.str());}
            reps.erase(name);memories.erase(name);
        }
        reps[canonical]=original.find(chosen)->second;
        typename std::map<std::string,Memory>::const_iterator selected=oldMemories.find(chosen);memories[canonical]=selected==oldMemories.end()?Memory():selected->second;
    }
}
}
