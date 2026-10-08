#include <cassert>
#include <set>
#include <string>
#include <iostream>
#include "src/Guild/Level3Access.h"
#include "src/Guild/Progression.h"
#include "GuildLevelUnlocks.h"

struct GameData {};
struct Research {
    std::set<GameData*> finished,enabledObjects;
    bool changedSoUpdateGUI;
    Research():changedSoUpdateGUI(false){}
};
enum {BUILDING,RESEARCH};
GameData book,unlock,furniture,personalContract;
struct Data {
    GameData* getData(const char* id,int type){
        if(std::string(id)==GuildLevel3Access::researchId()&&type==RESEARCH)return &unlock;
        if(std::string(id)=="880137-Guild Escort Contracts.mod"&&type==BUILDING)return &book;
        return 0;
    }
};
struct Player {Research* technology;};
struct World {Player* player;Data gamedata;};
World* ou=0;
bool progressLoadFault=false,progressWriteBlocked=false;
int currentLevel=0;
int guildLevel(){return currentLevel;}
const char* kMissionBookBuildingId="880137-Guild Escort Contracts.mod";
#include "level3-runtime.generated.h"

int main(){
    Research technology;Player player;player.technology=&technology;World world;world.player=&player;ou=&world;
    technology.finished.insert(&furniture);technology.enabledObjects.insert(&personalContract);
    for(int level=0;level<=10;++level){
        currentLevel=level;updateMissionBookLevelAccess();
        assert((technology.enabledObjects.count(&book)!=0)==(level>=3));
        assert((technology.finished.count(&unlock)!=0)==(level>=3));
        assert(GuildLevel3Access::available(level)==(level>=3));
        assert(GuildProgression::officeLimit(level)==(level<3?0:level==3?1:3));
        assert(technology.finished.count(&furniture));assert(technology.enabledObjects.count(&personalContract));
        technology.changedSoUpdateGUI=false;updateMissionBookLevelAccess();assert(!technology.changedSoUpdateGUI);
    }
    // Existing saves at level 3/5/10: no level-up event; equal-level save switches.
    const int loads[]={3,3,5,5,10,0,1,2,3};
    for(int i=0;i<9;++i){Research loaded;loaded.enabledObjects.insert(&personalContract);player.technology=&loaded;currentLevel=loads[i];updateMissionBookLevelAccess();assert((loaded.enabledObjects.count(&book)!=0)==(currentLevel>=3));assert(loaded.changedSoUpdateGUI==(currentLevel>=3));assert(loaded.enabledObjects.count(&personalContract));}
    player.technology=&technology;
    // Old low-level save with previously researched furniture/book: revoke only the book.
    for(int i=0;i<3;++i){currentLevel=i;technology.enabledObjects.insert(&book);technology.finished.insert(&unlock);technology.changedSoUpdateGUI=false;updateMissionBookLevelAccess();assert(!technology.enabledObjects.count(&book)&&!technology.finished.count(&unlock)&&technology.changedSoUpdateGUI);assert(technology.finished.count(&furniture));}
    currentLevel=5;updateMissionBookLevelAccess();technology.enabledObjects.erase(&book);updateMissionBookLevelAccess();assert(technology.enabledObjects.count(&book));
    // Corrupt progress must never revoke an existing save's unlocks.
    currentLevel=0;progressLoadFault=true;updateMissionBookLevelAccess();assert(technology.enabledObjects.count(&book));progressLoadFault=false;progressWriteBlocked=true;updateMissionBookLevelAccess();assert(technology.enabledObjects.count(&book));progressWriteBlocked=false;
    ou=0;updateMissionBookLevelAccess();ou=&world;player.technology=0;updateMissionBookLevelAccess();player.technology=&technology;
    const char* languages[]={"fr","en","pl","ru"};
    for(int i=0;i<4;++i){Loc::configure("Localization",languages[i]);std::string text=GuildLevelUI::level3Tooltip();assert(text==std::string(Loc::text("guild.level3.title"))+"\n\n"+Loc::text("guild.level3.house.title")+"\n"+Loc::text("guild.level3.house.body")+"\n\n"+Loc::text("guild.level3.delegation.title")+"\n"+Loc::text("guild.level3.delegation.body"));assert(GuildLevelUI::unlocks(3).size()==2);assert(text.find("guild.level3.")==std::string::npos);}
    std::cout<<"Level 0-10, 2->3, old saves, same-level reload, research rebuild, unrelated unlocks, FR/EN/PL/RU: PASS\n";
}
