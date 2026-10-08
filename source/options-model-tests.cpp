#include "src/UI/ClientOptions.h"
#include <cassert>
#include <iostream>
#include <sstream>
struct Valid { bool operator()(int value)const{return value>=1&&value<=100;} };
int main(){
    using namespace ClientOptions;
    Settings defaults(true,36,25,0);
    assert(defaults.showClientTracker&&defaults.showRoutes&&defaults.detailedOffers);
    assert(defaults.bindings[OpenGuildManagement]==36&&defaults.bindings[OpenCheatMenu]==25&&defaults.bindings[OpenAutopilot]==0);
    assert(definitions().size()==21);assert(defaults.realEstateEnabled);
    std::istringstream legacy("showClientTracker=0\nguildManagementHotkey=24\n");read(legacy,defaults,Valid());
    assert(!defaults.showClientTracker&&defaults.bindings[OpenGuildManagement]==24&&defaults.bindings[OpenCheatMenu]==25);
    assert(defaults.realEstateEnabled);
    std::istringstream off("realEstateEnabled=0\n");read(off,defaults,Valid());assert(!defaults.realEstateEnabled);
    std::istringstream modern("language=fr\nshowRoutes=0\nconfirmAccept=0\nnotificationDuration=99\ncheatMenuHotkey=42\nautopilotHotkey=42\n");read(modern,defaults,Valid());
    assert(defaults.language=="fr"&&!defaults.showRoutes&&!defaults.confirmAccept&&defaults.notificationDuration==15);
    assert(conflict(defaults,OpenAutopilot,42)==OpenCheatMenu);
    defaults.bindings[OpenAutopilot]=0;std::ostringstream saved;write(saved,defaults);Settings loaded(true,36,25,0);std::istringstream input(saved.str());read(input,loaded,Valid());
    assert(!loaded.realEstateEnabled);
    assert(loaded.language=="fr"&&!loaded.showRoutes&&!loaded.confirmAccept&&loaded.bindings[OpenCheatMenu]==42);
    assert(validLanguage("auto")&&validLanguage("fr")&&validLanguage("en")&&validLanguage("ru")&&validLanguage("pl")&&validLanguage("de")&&validLanguage("zh-TW")&&!validLanguage("../de"));
    const char* langs[]={"auto","fr","en","pl","ru"};for(int i=0;i<5;++i){defaults.language=langs[i];std::ostringstream disk;write(disk,defaults);Settings fresh;std::istringstream bytes(disk.str());read(bytes,fresh,Valid());assert(fresh.language==langs[i]&&!fresh.developerMode);}
    defaults.developerMode=true;std::ostringstream session;write(session,defaults);assert(session.str().find("developerMode=0")!=std::string::npos);assert(defaults.developerMode);
    Settings restarted;std::istringstream restart(session.str());read(restart,restarted,Valid());assert(!restarted.developerMode);
    std::istringstream oldCheats("developerMode=1\n");read(oldCheats,restarted,Valid());assert(!restarted.developerMode);
    std::cout<<"PASS: legacy migration, centralized definitions, persistence, limits and key conflicts\n";
}
