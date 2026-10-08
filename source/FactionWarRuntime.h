// Native diplomacy only. No parallel hostility state and no direct memory writes.
bool factionAtWar(Faction* faction){
    Faction* player=ou&&ou->player?ou->player->participant:0;
    return faction&&player&&faction!=player&&
        ((faction->relations&&faction->relations->isEnemy(player))||
         (player->relations&&player->relations->isEnemy(faction)));
}
bool destinationAtWar(TownBase* town){return town&&factionAtWar(town->getFaction());}
// Verified against the three vanilla data files, without loading a user mod.
const char* diplomacyFactionIds[]={"1083-gamedata.base","defaultEmpireFactionSID","11624-Dialogue (10).mod","1088-gamedata.base","1233-gamedata.base","18000-gamedata.base","56777-Dialogue.mod","58229-Dialogue.mod","49377-rebirth.mod","18891-rebirth.mod"};
Faction* diplomacyFaction(int index){return index>=0&&index<10&&ou&&ou->factionMgr?ou->factionMgr->getFactionByStringID(diplomacyFactionIds[index]):0;}
