#pragma once

// Developer-only experiment. This is deliberately independent from contracts,
// rewards and normal mission state. It is only reachable from the P test menu.
namespace MissionPlatoonPrototype {
struct Member {
    hand character,originalPlatoon,temporaryPlatoon;
    int originalIndex;
    bool originalLeader;
    Ogre::Vector3 returnPosition;
    Member():originalIndex(-1),originalLeader(false),returnPosition(Ogre::Vector3::ZERO){}
};
struct TestMission {
    unsigned int id;
    std::vector<Member> members;
    TestMission():id(0){}
};
static std::vector<TestMission> missions;
static unsigned int nextId=1;

inline void log(const std::string& text){
    std::ofstream out("mods/Guild Escort Contracts/MissionPlatoonPrototype.log",std::ios::app);
    out<<"[MISSION TEST] "<<text<<"\n";DebugLog(std::string("[MISSION TEST] ")+text);
}
inline void notify(const std::string& text){if(!clientOptions.developerMode)return;log(text);if(ou)ou->showPlayerAMessage(std::string("[MISSION TEST] ")+text,true);}
inline int memberIndex(ActivePlatoon* platoon,Character* character){
    if(!platoon||!character)return -1;lektor<RootObject*>* things=platoon->getThings();
    if(!things)return -1;for(unsigned int i=0;i<things->size();++i)if((*things)[i]==character)return (int)i;return -1;
}
inline bool alreadyAssigned(const hand& character){
    for(size_t m=0;m<missions.size();++m)for(size_t i=0;i<missions[m].members.size();++i)if(missions[m].members[i].character==character)return true;return false;
}
inline std::vector<Character*> selectedPlayers(){
    std::vector<Character*> out;if(!ou||!ou->player)return out;
    for(ogre_unordered_set<hand>::type::const_iterator it=ou->player->selectedCharacters.begin();it!=ou->player->selectedCharacters.end();++it){Character* c=it->getCharacter();if(c&&c->getFaction()==ou->player->participant&&!c->isDead())out.push_back(c);}return out;
}
inline bool unloadNative(Platoon* platoon){
    if(!platoon||!ou||!ou->player||!ou->player->participant)return false;
    ActivePlatoon* active=platoon->getActivePlatoon();if(!active)return platoon->getUnloadedPlatoon()!=0;
    active->serialiseEverythingToDisk(false);
    ou->player->participant->_switchToUnloadedPlatoon(active);
    return platoon->getUnloadedPlatoon()!=0&&platoon->getActivePlatoon()==0;
}
inline bool activate(Platoon* platoon);
inline bool moveCharacter(Character* character,ActivePlatoon* from,ActivePlatoon* to,int index){
    if(!character||!from||!to||character->getPlatoon()!=from)return false;
    if(!from->removeObject(character))return false;
    to->addCharacterAt(character,std::max(0,index));
    if(character->getPlatoon()==to)return true;
    // Best-effort repair: never knowingly leave the character unattached.
    from->addCharacterAt(character,std::max(0,index));
    return false;
}
inline bool restoreMember(Member& member){
    Character* character=member.character.getCharacter();
    Platoon* original=member.originalPlatoon.getPlatoon();
    Platoon* temporary=member.temporaryPlatoon.getPlatoon();
    if(!character||!original||!temporary||!activate(original)||!activate(temporary))return false;
    ActivePlatoon* from=temporary->getActivePlatoon();ActivePlatoon* to=original->getActivePlatoon();
    if(character->getPlatoon()==to)return true;
    if(!moveCharacter(character,from,to,member.originalIndex))return false;
    character->getMovement()->_setPositionAndTeleport(member.returnPosition,0);
    if(member.originalLeader)to->setSquadLeader(character);
    return character->getPlatoon()==to;
}
// Kept only as an isolated record of the failed experiment. Calling
// Faction::_switchToUnloadedPlatoon on a player platoon leaves PlayerInterface
// and PortraitManager holding active Character pointers. Kenshi 1.0.65 then
// crashes in the portrait refresh path (RVA 0x41202C). Do not call this.
inline void startUnsafeInvestigationOnly(){if(!clientOptions.developerMode)return;
    if(!ou||!ou->player||!ou->player->participant){notify("ABORT: player faction unavailable");return;}
    std::vector<Character*> selected=selectedPlayers();if(selected.empty()){notify("ABORT: select at least one player character");return;}
    int normallyPlayable=0;for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(c&&!c->isDead()&&!alreadyAssigned(c->getHandle()))++normallyPlayable;}
    if((int)selected.size()>=normallyPlayable){notify("ABORT: at least one playable character must remain");return;}
    for(size_t i=0;i<selected.size();++i)if(alreadyAssigned(selected[i]->getHandle())){notify("ABORT: a selected character is already in a test mission");return;}
    TestMission test;test.id=nextId++;
    std::stringstream begin;begin<<"TEST MISSION "<<test.id<<" start, selected="<<selected.size();log(begin.str());
    // Capture every origin before moving anyone: removals change squad indices.
    for(size_t i=0;i<selected.size();++i){
        Character* c=selected[i];ActivePlatoon* original=c->getPlatoon();Platoon* originalPlatoon=original?original->me:0;
        if(!original||!originalPlatoon){notify("ABORT: selected character has no active original platoon");return;}
        Member member;member.character=c->getHandle();member.originalPlatoon=hand(originalPlatoon);member.originalIndex=memberIndex(original,c);member.originalLeader=original->getSquadLeader_theRealOne()==c;member.returnPosition=c->getPosition();
        if(member.originalIndex<0){notify("ABORT: selected character was not found in its original platoon");return;}
        test.members.push_back(member);
    }
    Platoon* temporary=selected[0]->separateIntoMyOwnSquad(true);
    if(!temporary||!temporary->getActivePlatoon()||selected[0]->getPlatoon()!=temporary->getActivePlatoon()){notify("ABORT: native temporary platoon creation was not confirmed");return;}
    std::stringstream name;name<<"TEST MISSION "<<test.id;temporary->getActivePlatoon()->setName(name.str());temporary->setPersistentSquad(true);
    test.members[0].temporaryPlatoon=hand(temporary);
    bool transferComplete=true;
    for(size_t i=1;i<test.members.size();++i){
        Character* c=test.members[i].character.getCharacter();ActivePlatoon* original=c?c->getPlatoon():0;
        test.members[i].temporaryPlatoon=hand(temporary);
        if(!moveCharacter(c,original,temporary->getActivePlatoon(),(int)i)){log("STOP: multi-character transfer failed; rolling back");transferComplete=false;break;}
    }
    // Register before unload (and before any recoverable failure) so RETURN and save persistence can find it.
    missions.push_back(test);
    if(!transferComplete){
        bool repaired=true;for(size_t i=0;i<missions.back().members.size();++i){Character* c=missions.back().members[i].character.getCharacter();if(c&&c->getPlatoon()==temporary->getActivePlatoon())repaired=restoreMember(missions.back().members[i])&&repaired;}
        if(repaired){if(temporary->getCharacterCount()==0)ou->player->participant->removePlatoon(temporary);missions.pop_back();notify("ABORT: transfer failed and all moved characters were restored");}
        else notify("STOP: transfer failed and rollback was incomplete; use RETURN immediately");
        return;
    }
    for(size_t i=0;i<missions.back().members.size();++i){std::stringstream moved;Character* c=missions.back().members[i].character.getCharacter();moved<<(c?c->getName():"<invalid>")<<" transfer OK; original="<<missions.back().members[i].originalPlatoon.toString()<<" temporary="<<missions.back().members[i].temporaryPlatoon.toString();log(moved.str());}
    bool unloaded=unloadNative(temporary);std::stringstream state;state<<"temporary platoon "<<hand(temporary).toString()<<" unload confirmed="<<(unloaded?"YES":"NO");log(state.str());
    notify(unloaded?"departure complete; native unload confirmed":"STOP: transfer completed but native unload was not confirmed; use RETURN immediately");
}
inline void start(){if(!clientOptions.developerMode)return;
#if 0
    if(!ou||!ou->player||!ou->player->participant){notify("ABORT: player faction unavailable");return;}
    std::vector<Character*> selected=selectedPlayers();
    if(selected.size()!=1){notify("TEST 1 requires exactly one selected player character");return;}
    Character* c=selected[0];
    if(!c||c->isDead()){notify("ABORT: selected character is invalid or dead");return;}
    int playable=0;for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* p=ou->player->playerCharacters[i];if(p&&!p->isDead())++playable;}
    if(playable<2){notify("ABORT: at least one other playable character must remain");return;}
    log("PROTO2 step=pre-archive selected="+c->getName());
    GameDataContainer* archive=new GameDataContainer();
    if(!archive){notify("ABORT: archive container allocation failed");return;}
    GameSaveState state=c->serialise(archive,0,0);
    bool valid=state.baseData!=0&&state.dataSource==archive;
    std::stringstream s;s<<"PROTO2 step=archive result="<<(valid?"OK":"INVALID")<<" instance="<<state.instanceID.id;log(s.str());
    std::string path="mods/Guild Escort Contracts/CharacterReconstructionPrototype.archive";
    bool saved=valid&&archive->save(path,0);log(std::string("PROTO2 step=archive-file result=")+(saved?"OK":"FAILED")+" path="+path);
    delete archive;
    notify(saved?"PROTO2 archive complete; destructive dismissal is intentionally STOPPED pending native-safe API":"PROTO2 archive failed; no character mutation performed");
#else
    notify("DISABLED: use NATIVE DISMISS OBSERVER");
#endif
}
inline bool activate(Platoon* platoon){
    if(!platoon||!ou||!ou->player||!ou->player->participant)return false;
    if(!platoon->getActivePlatoon())ou->player->participant->restorePlatoon(platoon);
    if(!platoon->getActivePlatoon())platoon->activate();
    return platoon->getActivePlatoon()!=0;
}
inline void returnLatest(){if(!clientOptions.developerMode)return;
    if(missions.empty()){notify("no active test mission to return");return;}TestMission& test=missions.back();
    std::stringstream begin;begin<<"TEST MISSION "<<test.id<<" return requested";log(begin.str());bool complete=true;
    // Restore lower original indices first so relative squad order is preserved.
    std::vector<bool> attempted(test.members.size(),false);
    for(size_t done=0;done<test.members.size();++done){size_t best=test.members.size();for(size_t i=0;i<test.members.size();++i){if(attempted[i])continue;Character* c=test.members[i].character.getCharacter();Platoon* original=test.members[i].originalPlatoon.getPlatoon();if(c&&original&&c->getPlatoon()==original->getActivePlatoon()){attempted[i]=true;continue;}if(best==test.members.size()||test.members[i].originalIndex<test.members[best].originalIndex)best=i;}if(best==test.members.size())break;attempted[best]=true;
        Member& member=test.members[best];Character* c=member.character.getCharacter();
        if(!restoreMember(member)){log("STOP: character restoration failed");complete=false;continue;}
        std::stringstream restored;restored<<(c?c->getName():"<invalid>")<<" restored to original platoon="<<member.originalPlatoon.toString();log(restored.str());
    }
    Platoon* temporary=test.members.empty()?0:test.members[0].temporaryPlatoon.getPlatoon();if(complete&&temporary&&temporary->getCharacterCount()==0)ou->player->participant->removePlatoon(temporary);
    if(complete){missions.pop_back();notify("return complete; original characters restored");}else notify("STOP: return incomplete; keep the disposable save and inspect the prototype log");
}
inline void clearForImport(){missions.clear();nextId=1;log("all prototype missions cleared for Import Game without penalty");}
template<class Archive> void archiveState(Archive& a,unsigned int& nextId,std::vector<TestMission>& missions){
    std::string marker="MISSION-PLATOON-PROTOTYPE-1";a.field(marker);if(marker!="MISSION-PLATOON-PROTOTYPE-1")throw std::runtime_error("invalid platoon prototype extension");a.field(nextId);a.field(missions);
}
template<class Archive> void archive(Archive& a){archiveState(a,nextId,missions);}
}
