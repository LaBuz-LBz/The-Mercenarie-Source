#pragma once
#include <string>
namespace MercenarieCleanup {
// World-scoped, cleared explicitly on every load/new game/import transition.
static bool disabled=false;
static bool researchCleaned=false;
static std::string protectedSource;
inline const char* marker(){return "THE-MERCENARIE-DISABLED-1\n";}
inline const char* markerName(){return "Disabled.v1";}
inline bool validMarker(const std::string& value){return value==marker();}
inline bool rotatingSlot(std::string name){
    for(size_t i=0;i<name.size();++i)if(name[i]>='A'&&name[i]<='Z')name[i]=(char)(name[i]-'A'+'a');
    return name.find("autosave")==0||name.find("quicksave")==0||name.find("quick save")==0;
}
inline bool ownedItemField(const std::string& key){return key=="TheMercenarie.MailStep.V1"||key=="TheMercenarie.MailCaption.V1";}
inline bool ownedDefinition(const std::string& id){
    static const char* ids[]={
#include "CleanupOwnedDefinitions.generated.h"
    };
    for(size_t i=0;i<sizeof(ids)/sizeof(ids[0]);++i)if(id==ids[i])return true;
    return false;
}
}
