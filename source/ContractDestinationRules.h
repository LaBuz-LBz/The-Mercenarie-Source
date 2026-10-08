#pragma once
#include <string>
namespace ContractDestinationRules {
inline bool scientific(const std::string& id){
    static const char* ids[]={"51398-rebirth.mod","1075-gamedata.base","49765-rebirth.mod","58703-rebirth.mod","62796-rebirth.mod","55644-rebirth.mod","50993-rebirth.mod","51423-rebirth.mod","48796-rebirth.mod","48446-rebirth.mod","51383-rebirth.mod"};
    for(size_t i=0;i<sizeof(ids)/sizeof(ids[0]);++i)if(id==ids[i])return true;
    return false;
}
inline bool blocked(const std::string& id){
    static const char* ids[]={"49367-rebirth.mod", // Sunken Ruins (existing exclusion)
        "1532824-rebirth.mod", // Cat-Lon's Exile
        "50522-rebirth.mod", // Cannibal Capital
        "52210-Newwworld.mod", // Black Desert City
        "96389-Dialogue.mod", // Fish Isle
        "1032-gamedata.base" // Southern Hive capital
    };
    for(size_t i=0;i<sizeof(ids)/sizeof(ids[0]);++i)if(id==ids[i])return true;
    return false;
}
}
