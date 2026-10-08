#pragma once
#include "CleanupState.h"

// Non-persistent EN_MISSION prototype. Characters always remain in their
// original ActivePlatoon, faction and playerCharacters collection.
namespace MissionAbsencePrototype {

struct Member {
    std::string groupId;
    hand handle;
    Character* character;
    ActivePlatoon* platoon;
    Faction* faction;
    int originalIndex;
    bool wasLeader;
    Ogre::Vector3 returnPosition;
    Ogre::Vector3 technicalPosition;
    bool wasVisible;
    bool visualsWereActive;
    Member():character(0),platoon(0),faction(0),originalIndex(-1),wasLeader(false),
        returnPosition(Ogre::Vector3::ZERO),technicalPosition(Ogre::Vector3::ZERO),
        wasVisible(true),visualsWereActive(true){}
};

struct PlatoonOrder {
    ActivePlatoon* platoon;
    std::vector<Character*> original;
    PlatoonOrder():platoon(0){}
};

struct State {
    bool active;
    std::vector<Member> members;
    std::vector<PlatoonOrder> platoons;
    float elapsed;
    bool delayedSnapshotWritten;
    State():active(false),elapsed(0.0f),delayedSnapshotWritten(false){}
};

static State state;

struct PortraitWidgetVisibility { bool border,background,image,overlayBack,overlayFront,name; };
static std::map<PortraitMainCellView*,PortraitWidgetVisibility> hiddenPortraitCells;

inline void log(const std::string& line,bool reset=false)
{
    std::ofstream out("mods/Guild Escort Contracts/MissionAbsencePrototype.log",
        std::ios::out|(reset?std::ios::trunc:std::ios::app));
    if(out)out<<line<<"\n";
    DebugLog("Mission absence prototype: "+line);
}

inline bool sameHandleIdentity(const hand& a,const hand& b)
{
    return a.type==b.type&&a.container==b.container&&a.containerSerial==b.containerSerial&&
        a.index==b.index&&a.serial==b.serial;
}

inline bool samePersistentHandleIdentity(const hand& saved,const hand& current)
{
    return ZetaFixRules::restoredHandleMatches(saved.type,saved.index,saved.serial,
        current.type,current.index,current.serial);
}

inline bool isInPlayerCharacters(Character* character)
{
    if(!character||!ou||!ou->player)return false;
    for(size_t i=0;i<ou->player->playerCharacters.size();++i)
        if(ou->player->playerCharacters[i]==character)return true;
    return false;
}

inline int platoonIndex(ActivePlatoon* platoon,Character* character)
{
    lektor<RootObject*>* things=platoon?platoon->getThings():0;
    if(!things||!character)return -1;
    for(size_t i=0;i<things->size();++i)if((*things)[i]==character)return static_cast<int>(i);
    return -1;
}

inline Member* memberFor(Character* character)
{
    if(!state.active||!character)return 0;
    for(size_t i=0;i<state.members.size();++i)
        if(state.members[i].character==character&&sameHandleIdentity(character->getHandle(),state.members[i].handle))return &state.members[i];
    return 0;
}

inline Member* memberForHandle(const hand& handle)
{
    if(!state.active)return 0;
    for(size_t i=0;i<state.members.size();++i)
        if(sameHandleIdentity(handle,state.members[i].handle))return &state.members[i];
    return 0;
}

inline bool savedAbsent(Character* character);
inline bool isAbsent(Character* character){return memberFor(character)!=0||savedAbsent(character);}
inline bool isAbsent(RootObject* object){return object&&isAbsent(dynamic_cast<Character*>(object));}

inline bool identityIntact(const Member& member)
{
    Character* character=member.character;
    return character&&sameHandleIdentity(character->getHandle(),member.handle)&&
        character->getPlatoon()==member.platoon&&character->getFaction()==member.faction&&
        isInPlayerCharacters(character);
}

inline void refreshPortraits()
{
    if(gui&&gui->mainbar){
        gui->mainbar->updatePotraitsPlatoon();
        gui->mainbar->updatePortraits();
        for(size_t i=0;i<state.members.size();++i)gui->mainbar->updatePortrait(state.members[i].handle);
        gui->mainbar->refreshPortraitTabs();
    }
}

inline void restoreHiddenPortraitCell(PortraitMainCellView* cell)
{
    std::map<PortraitMainCellView*,PortraitWidgetVisibility>::iterator it=hiddenPortraitCells.find(cell);
    if(it==hiddenPortraitCells.end())return;
    const PortraitWidgetVisibility v=it->second;
    if(cell->border)cell->border->setVisible(v.border);
    if(cell->imageBackground)cell->imageBackground->setVisible(v.background);
    if(cell->imagePortrait)cell->imagePortrait->setVisible(v.image);
    if(cell->imageOverlayBack)cell->imageOverlayBack->setVisible(v.overlayBack);
    if(cell->imageOverlayFront)cell->imageOverlayFront->setVisible(v.overlayFront);
    if(cell->textName)cell->textName->setVisible(v.name);
    hiddenPortraitCells.erase(it);
}

inline void restoreAllHiddenPortraitCells()
{ while(!hiddenPortraitCells.empty())restoreHiddenPortraitCell(hiddenPortraitCells.begin()->first); }

inline bool memberIndexLess(Member* a,Member* b){return a->originalIndex<b->originalIndex;}

inline bool moveMember(Member& member,int targetIndex)
{
    int current=platoonIndex(member.platoon,member.character);
    lektor<RootObject*>* things=member.platoon?member.platoon->getThings():0;
    if(!things||!identityIntact(member)||current<0||targetIndex<0||targetIndex>=static_cast<int>(things->size()))return false;
    std::vector<std::pair<int,int> > completed;
    while(current!=targetIndex){
        const int next=current+(targetIndex>current?1:-1);
        {std::ostringstream s;s<<"SWAP "<<member.handle.toString()<<" | "<<current<<" <-> "<<next;log(s.str());}
        member.platoon->swapCharacters(current,next);
        completed.push_back(std::make_pair(current,next));
        const int observed=platoonIndex(member.platoon,member.character);
        {std::ostringstream s;s<<"SWAP CHECK | handle="<<member.handle.toString()<<" | expected_index="<<next
            <<" | observed_index="<<observed<<" | same_handle="<<(sameHandleIdentity(member.character->getHandle(),member.handle)?"YES":"NO")
            <<" | same_platoon="<<(member.character->getPlatoon()==member.platoon?"YES":"NO")
            <<" | same_faction="<<(member.character->getFaction()==member.faction?"YES":"NO")
            <<" | playerCharacters="<<(isInPlayerCharacters(member.character)?"YES":"NO");log(s.str());}
        if(observed!=next||!identityIntact(member)){
            for(std::vector<std::pair<int,int> >::reverse_iterator it=completed.rbegin();it!=completed.rend();++it)
                member.platoon->swapCharacters(it->second,it->first);
            return false;
        }
        current=next;
    }
    return true;
}

inline bool applyReferenceOrder(PlatoonOrder& saved,bool finalOrder)
{
    lektor<RootObject*>* things=saved.platoon?saved.platoon->getThings():0;
    if(!things||things->size()!=saved.original.size())return false;
    std::vector<Character*> desired;
    if(finalOrder)desired=saved.original;
    else{
        for(size_t i=0;i<saved.original.size();++i)if(!memberFor(saved.original[i]))desired.push_back(saved.original[i]);
        for(size_t i=0;i<saved.original.size();++i)if(memberFor(saved.original[i]))desired.push_back(saved.original[i]);
    }
    for(size_t target=0;target<desired.size();++target){
        Character* wanted=desired[target];
        int current=platoonIndex(saved.platoon,wanted);
        if(current<0)return false;
        while(current>static_cast<int>(target)){
            saved.platoon->swapCharacters(current,current-1);
            --current;
        }
        if(platoonIndex(saved.platoon,wanted)!=static_cast<int>(target))return false;
    }
    for(size_t i=0;i<state.members.size();++i)
        if(state.members[i].platoon==saved.platoon&&!identityIntact(state.members[i]))return false;
    return true;
}

inline bool applyAllReferenceOrders(bool finalOrder)
{
    bool ok=true;
    for(size_t i=0;i<state.platoons.size();++i)if(!applyReferenceOrder(state.platoons[i],finalOrder))ok=false;
    return ok;
}

inline bool isolatedNavmeshPosition(Character* character,const std::vector<Ogre::Vector3>& reserved,Ogre::Vector3& result)
{
    if(!character||!ou||!ou->navmesh||!ou->zoneMgr)return false;
    const Ogre::Vector3 origin=character->getPosition();
    const float radii[]={250.0f,400.0f,550.0f,700.0f};
    const Ogre::Vector2 dirs[]={Ogre::Vector2(1,0),Ogre::Vector2(-1,0),Ogre::Vector2(0,1),Ogre::Vector2(0,-1),
        Ogre::Vector2(.7071f,.7071f),Ogre::Vector2(-.7071f,.7071f),Ogre::Vector2(.7071f,-.7071f),Ogre::Vector2(-.7071f,-.7071f)};
    for(size_t r=0;r<4;++r)for(size_t d=0;d<8;++d){
        Ogre::Vector3 wanted(origin.x+dirs[d].x*radii[r],origin.y,origin.z+dirs[d].y*radii[r]);
        if(!ou->zoneMgr->isZoneLoadedT(wanted))continue;
        Ogre::Vector3 projected;
        if(!ou->navmesh->getClosestPoint(wanted,80.0f,2.0f,false,projected)||
            !ou->navmesh->getPositionValid(projected)||!ou->zoneMgr->isZoneLoadedT(projected))continue;
        bool occupied=false;
        for(size_t i=0;i<reserved.size();++i)if(projected.squaredDistance(reserved[i])<2500.0f){occupied=true;break;}
        if(occupied)continue;
        lektor<RootObject*> nearby;
        ou->getCharactersWithinSphere(nearby,projected,160.0f,160.0f,160.0f,64,64,character);
        if(nearby.size()!=0)continue;
        result=projected;return true;
    }
    return false;
}

inline bool reserveTechnicalPositions(Character* reference,size_t count,std::vector<Ogre::Vector3>& results)
{
    results.clear();
    if(!reference||count==0||count>30)return false;

    // Find one genuinely isolated, loaded and navigable anchor.  All mission
    // Characters then share that safe area instead of consuming one area each.
    std::vector<Ogre::Vector3> none;
    Ogre::Vector3 anchor;
    if(!isolatedNavmeshPosition(reference,none,anchor))return false;

    const float spacing=5.0f;
    for(int ring=0;ring<=5&&results.size()<count;++ring){
        for(int dz=-ring;dz<=ring&&results.size()<count;++dz){
            for(int dx=-ring;dx<=ring&&results.size()<count;++dx){
                if(ring!=0&&dx!=-ring&&dx!=ring&&dz!=-ring&&dz!=ring)continue;
                Ogre::Vector3 wanted(anchor.x+dx*spacing,anchor.y,anchor.z+dz*spacing);
                if(!ou->zoneMgr->isZoneLoadedT(wanted))continue;
                Ogre::Vector3 projected;
                if(!ou->navmesh->getClosestPoint(wanted,4.0f,2.0f,false,projected)||
                    !ou->navmesh->getPositionValid(projected)||!ou->zoneMgr->isZoneLoadedT(projected))continue;
                bool duplicate=false;
                for(size_t i=0;i<results.size();++i)
                    if(projected.squaredDistance(results[i])<4.0f){duplicate=true;break;}
                if(!duplicate)results.push_back(projected);
            }
        }
    }
    {std::ostringstream s;s<<"TECHNICAL AREA | requested="<<count<<" | reserved="<<results.size()
        <<" | anchor="<<anchor.x<<","<<anchor.y<<","<<anchor.z;log(s.str());}
    return results.size()==count;
}

inline void hide(Member& member)
{
    Character* character=member.character;if(!character)return;
    character->endCombatMode();
    if(character->getMovement())character->getMovement()->halt();
    character->setNameTagVisible(false);character->setVisible(false);
    AnimationClass* animation=character->getAnimationClass();
    if(animation&&animation->getIsActivated())animation->deactivateVisuals();
}

inline bool captureSelected(const std::string& groupId)
{
    if(!ou||!ou->player){log("CAPTURE REFUSED | reason=player interface unavailable");return false;}
    std::vector<Character*> selected;
    {std::ostringstream s;s<<"SELECTION SUMMARY | playerCharacters="<<ou->player->playerCharacters.size()
        <<" | selectedHandles="<<ou->player->selectedCharacters.size();log(s.str());}
    size_t rawIndex=0;
    for(ogre_unordered_set<hand>::type::const_iterator it=ou->player->selectedCharacters.begin();it!=ou->player->selectedCharacters.end();++it){
        Character* character=it->getCharacter();
        const bool inPlayers=isInPlayerCharacters(character);
        const bool duplicate=character&&std::find(selected.begin(),selected.end(),character)!=selected.end();
        {std::ostringstream s;s<<"SELECTED RAW ["<<rawIndex++<<"] | handle="<<it->toString()
            <<" | pointer="<<character<<" | in_playerCharacters="<<(inPlayers?"YES":"NO")
            <<" | duplicate_pointer="<<(duplicate?"YES":"NO")
            <<" | platoon_index="<<(character?platoonIndex(character->getPlatoon(),character):-1);log(s.str());}
        if(character&&inPlayers&&!duplicate&&!isAbsent(character))selected.push_back(character);
    }
    {std::ostringstream s;s<<"SELECTION FILTERED | valid_unique="<<selected.size()
        <<" | remaining="<<(ou->player->playerCharacters.size()>=selected.size()?ou->player->playerCharacters.size()-selected.size():0);log(s.str());}
    if(selected.empty()){log("CAPTURE REFUSED | reason=no valid selected Character");return false;}
    if(selected.size()>=ou->player->playerCharacters.size()){log("CAPTURE REFUSED | reason=no player Character would remain available");return false;}
    std::vector<Ogre::Vector3> technicalPositions;
    if(!reserveTechnicalPositions(selected[0],selected.size(),technicalPositions)){
        std::ostringstream s;s<<"CAPTURE REFUSED | reason=technical area cannot provide requested slots | requested="
            <<selected.size()<<" | available="<<technicalPositions.size();log(s.str());return false;
    }
    for(size_t i=0;i<selected.size();++i){
        Character* character=selected[i];
        {std::ostringstream s;s<<"VALIDATE SELECTED ["<<i<<"] | pointer="<<character
            <<" | handle="<<(character?character->getHandle().toString():"<null>")
            <<" | dead="<<(character&&character->isDead()?"YES":"NO")
            <<" | movement="<<(character&&character->getMovement()?"YES":"NO")
            <<" | platoon="<<(character?character->getPlatoon():0);log(s.str());}
        if(!character){log("CAPTURE REFUSED | reason=null Character");return false;}
        if(character->isDead()){log("CAPTURE REFUSED | reason=selected Character is dead");return false;}
        if(!character->getMovement()){log("CAPTURE REFUSED | reason=selected Character has no Movement");return false;}
        if(!character->getPlatoon()){log("CAPTURE REFUSED | reason=selected Character has no ActivePlatoon");return false;}
        Member member;member.groupId=groupId;member.character=character;member.handle=character->getHandle();member.platoon=character->getPlatoon();
        member.faction=character->getFaction();member.originalIndex=platoonIndex(member.platoon,character);
        // swapCharacters() does not change platoon membership or leadership.
        member.wasLeader=false;member.returnPosition=character->getPosition();
        member.wasVisible=character->getVisible();AnimationClass* animation=character->getAnimationClass();
        member.visualsWereActive=!animation||animation->getIsActivated();
        if(member.originalIndex<0){log("CAPTURE REFUSED | reason=selected Character absent from platoon ordered things");return false;}
        member.technicalPosition=technicalPositions[i];
        {std::ostringstream s;s<<"SELECTED VALID ["<<i<<"] | handle="<<member.handle.toString()
            <<" | original_index="<<member.originalIndex<<" | technical="<<member.technicalPosition.x<<","<<member.technicalPosition.y<<","<<member.technicalPosition.z;log(s.str());}
        state.members.push_back(member);
    }
    for(size_t m=0;m<state.members.size();++m){
        ActivePlatoon* platoon=state.members[m].platoon;bool known=false;
        for(size_t p=0;p<state.platoons.size();++p)if(state.platoons[p].platoon==platoon){known=true;break;}
        if(known)continue;
        PlatoonOrder saved;saved.platoon=platoon;lektor<RootObject*>* things=platoon->getThings();
        if(!things){log("CAPTURE REFUSED | reason=platoon getThings unavailable");return false;}
        for(size_t i=0;i<things->size();++i){Character* c=dynamic_cast<Character*>((*things)[i]);
            if(!c){log("CAPTURE REFUSED | reason=non-Character entry in platoon ordered things");return false;}saved.original.push_back(c);}
        state.platoons.push_back(saved);
    }
    return true;
}

inline bool reorderSelectedToEnd()
{
    return applyAllReferenceOrders(false);
}

inline void snapshot(const char* label)
{
    for(size_t i=0;i<state.members.size();++i){Member& m=state.members[i];std::ostringstream s;
        s<<label<<" ["<<i<<"] | handle="<<m.handle.toString()<<" | valid="<<(m.character?"YES":"NO")
         <<" | identity="<<(identityIntact(m)?"YES":"NO")<<" | original_index="<<m.originalIndex
         <<" | current_index="<<platoonIndex(m.platoon,m.character)<<" | visible="<<(m.character&&m.character->getVisible()?"YES":"NO");log(s.str());}
}

inline bool depart(const std::string& groupId)
{
    if(groupId.empty())return false;
    for(size_t i=0;i<state.members.size();++i)if(state.members[i].groupId==groupId)return false;
    const size_t previousMembers=state.members.size(),previousPlatoons=state.platoons.size();state.active=true;log("=== DEPART EN_MISSION GROUP ===",!previousMembers);
    if(!captureSelected(groupId)){
        state.members.resize(previousMembers);state.platoons.resize(previousPlatoons);state.active=!state.members.empty();log("DEPART CANCELLED | invalid selection or every character selected");return false;
    }
    snapshot("CAPTURED");
    if(!reorderSelectedToEnd()){
        log("DEPART CANCELLED | reorder failed; restoring reference order");state.members.resize(previousMembers);state.platoons.resize(previousPlatoons);applyAllReferenceOrders(state.members.empty());restoreAllHiddenPortraitCells();refreshPortraits();state.active=!state.members.empty();
        log("DEPART CANCELLED | native reference reorder not confirmed");return false;
    }
    snapshot("AFTER REORDER");restoreAllHiddenPortraitCells();refreshPortraits();
    for(size_t i=0;i<state.members.size();++i){
        Member& member=state.members[i];ou->player->unselectPlayerCharacter(member.character);hide(member);
        member.character->getMovement()->_setPositionAndTeleport(member.technicalPosition,0);hide(member);
    }
    refreshPortraits();snapshot("AFTER DEPART");
    return true;
}

inline void depart(){if(!clientOptions.developerMode)return;static unsigned int testSequence=0;std::ostringstream id;id<<"test-"<<++testSequence;depart(id.str());}

inline bool returnGroup(const std::string& groupId)
{
    if(!state.active)return false;
    std::vector<size_t> returning;for(size_t i=0;i<state.members.size();++i)if(state.members[i].groupId==groupId)returning.push_back(i);
    if(returning.empty())return false;
    for(size_t r=0;r<returning.size();++r)if(!identityIntact(state.members[returning[r]])||!state.members[returning[r]].character->getMovement()){
        snapshot("RETURN REFUSED");log("RETURN CANCELLED | character identity or validity changed");return false;}
    snapshot("BEFORE RETURN");
    for(size_t r=0;r<returning.size();++r){Member& m=state.members[returning[r]];
        m.character->getMovement()->_setPositionAndTeleport(m.returnPosition,0);
        AnimationClass* animation=m.character->getAnimationClass();if(animation&&m.visualsWereActive&&!animation->getIsActivated())animation->activateVisuals();
        m.character->setVisible(m.wasVisible);m.character->setNameTagVisible(true);
    }
    for(size_t r=returning.size();r>0;--r)state.members.erase(state.members.begin()+returning[r-1]);
    state.active=!state.members.empty();
    if(!applyAllReferenceOrders(!state.active)){snapshot("RETURN REORDER FAILED");return false;}
    restoreAllHiddenPortraitCells();refreshPortraits();snapshot("AFTER GROUP RETURN");
    if(!state.active)state.platoons.clear();
    return true;
}

inline void returnCharacters(){if(!state.active)return;std::set<std::string> groups;for(size_t i=0;i<state.members.size();++i)groups.insert(state.members[i].groupId);while(!groups.empty()){std::string id=*groups.begin();groups.erase(groups.begin());if(!returnGroup(id))return;}}

inline void returnCharacter(){if(!clientOptions.developerMode)return;returnCharacters();}

inline void update(float elapsed)
{
    if(!state.active)return;
    for(size_t i=0;i<state.members.size();++i){Member& m=state.members[i];
        if(!identityIntact(m))continue;if(ou&&ou->player)ou->player->unselectPlayerCharacter(m.character);hide(m);}
    state.elapsed+=elapsed;if(!state.delayedSnapshotWritten&&state.elapsed>=5.0f){state.delayedSnapshotWritten=true;snapshot("T+5s");}
}

inline void hidePortraitCell(PortraitMainCellView* cell)
{
    if(!cell||!memberForHandle(cell->characterHandle))return;
    PortraitWidgetVisibility v;
    v.border=cell->border&&cell->border->getVisible();v.background=cell->imageBackground&&cell->imageBackground->getVisible();
    v.image=cell->imagePortrait&&cell->imagePortrait->getVisible();v.overlayBack=cell->imageOverlayBack&&cell->imageOverlayBack->getVisible();
    v.overlayFront=cell->imageOverlayFront&&cell->imageOverlayFront->getVisible();v.name=cell->textName&&cell->textName->getVisible();
    hiddenPortraitCells[cell]=v;
    if(cell->border)cell->border->setVisible(false);if(cell->imageBackground)cell->imageBackground->setVisible(false);
    if(cell->imagePortrait)cell->imagePortrait->setVisible(false);if(cell->imageOverlayBack)cell->imageOverlayBack->setVisible(false);
    if(cell->imageOverlayFront)cell->imageOverlayFront->setVisible(false);if(cell->textName)cell->textName->setVisible(false);
}

struct PersistentMember {
    std::string groupId;
    hand handle;
    int originalIndex;
    Ogre::Vector3 returnPosition;
    Ogre::Vector3 technicalPosition;
    bool wasVisible;
    bool visualsWereActive;
    std::string factionId;
    PersistentMember():originalIndex(-1),returnPosition(Ogre::Vector3::ZERO),technicalPosition(Ogre::Vector3::ZERO),
        wasVisible(true),visualsWereActive(true){}
};

struct PersistentPlatoon {
    std::vector<hand> originalOrder;
};

struct PersistentState {
    int version;
    bool active;
    std::vector<PersistentMember> members;
    std::vector<PersistentPlatoon> platoons;
    PersistentState():version(2),active(false){}
};

static PersistentState persistent;
static float restoreElapsed=0,restoreRetry=0;
static bool restoreFault=false;
inline bool savedAbsent(Character* character){if(!character||!persistent.active)return false;const hand id=character->getHandle();for(size_t i=0;i<persistent.members.size();++i)if(samePersistentHandleIdentity(id,persistent.members[i].handle))return true;return false;}
static int restoreWaitLastLogSecond=-1;
static size_t restoreWaitLastPlayerCount=static_cast<size_t>(-1);
static const float restoreWaitLimitSeconds=30.0f;

inline void discardRuntimeReferences()
{
    state=State();
    hiddenPortraitCells.clear(); // UI widgets belong to the world being unloaded.
}

inline void clearPersistence()
{
    discardRuntimeReferences();persistent=PersistentState();restoreElapsed=restoreRetry=0;restoreFault=false;restoreWaitLastLogSecond=-1;
    restoreWaitLastPlayerCount=static_cast<size_t>(-1);
}

inline bool waitForRestore(float elapsed,const char* reason,const hand& handle)
{
    const size_t playerCount=ou&&ou->player?ou->player->playerCharacters.size():0;
    const int second=static_cast<int>(elapsed)/15;
    if(second!=restoreWaitLastLogSecond||playerCount!=restoreWaitLastPlayerCount){
        restoreWaitLastLogSecond=second;restoreWaitLastPlayerCount=playerCount;
        std::ostringstream s;s<<"RESTORE WAIT | elapsed="<<elapsed
            <<" | reason="<<reason<<" | handle="<<handle.toString()<<" | playerCharacters="<<playerCount;log(s.str());
    }
    // Streaming has no reliable deadline. Retain the domain and its identities.
    return false;
}

template<class Archive> inline void archive(Archive& a,PersistentState* isolated=0)
{
    PersistentState saved;
    if(!a.reading&&!persistent.active){
        saved.active=state.active;
        if(state.active){
            for(size_t i=0;i<state.members.size();++i){
                Member& m=state.members[i];PersistentMember p;p.groupId=m.groupId;p.handle=m.handle;p.originalIndex=m.originalIndex;
                p.returnPosition=m.returnPosition;p.technicalPosition=m.technicalPosition;p.wasVisible=m.wasVisible;
                p.visualsWereActive=m.visualsWereActive;
                if(m.faction&&m.faction->getData())p.factionId=m.faction->getData()->stringID;
                saved.members.push_back(p);
            }
            for(size_t i=0;i<state.platoons.size();++i){
                PersistentPlatoon p;
                for(size_t j=0;j<state.platoons[i].original.size();++j)
                    p.originalOrder.push_back(state.platoons[i].original[j]->getHandle());
                saved.platoons.push_back(p);
            }
        }
    }
    if(!a.reading&&persistent.active)saved=persistent;
    a.field(saved.version);a.field(saved.active);
    unsigned int memberCount=static_cast<unsigned int>(saved.members.size());a.field(memberCount);
    if(memberCount>30)throw std::runtime_error("oversized EN_MISSION roster");
    if(a.reading)saved.members.resize(memberCount);
    for(unsigned int i=0;i<memberCount;++i){PersistentMember& p=saved.members[i];
        a.field(p.handle);a.field(p.originalIndex);a.field(p.returnPosition);a.field(p.technicalPosition);
        a.field(p.wasVisible);a.field(p.visualsWereActive);a.field(p.factionId);if(!a.reading||saved.version>=2)a.field(p.groupId);else p.groupId="legacy";
    }
    unsigned int platoonCount=static_cast<unsigned int>(saved.platoons.size());a.field(platoonCount);
    if(platoonCount>30)throw std::runtime_error("oversized EN_MISSION platoons");
    if(a.reading)saved.platoons.resize(platoonCount);
    for(unsigned int i=0;i<platoonCount;++i){
        unsigned int n=static_cast<unsigned int>(saved.platoons[i].originalOrder.size());a.field(n);
        if(n>256)throw std::runtime_error("oversized EN_MISSION platoon order");
        if(a.reading)saved.platoons[i].originalOrder.resize(n);
        for(unsigned int j=0;j<n;++j)a.field(saved.platoons[i].originalOrder[j]);
    }
    if(saved.version!=1&&saved.version!=2)throw std::runtime_error("unsupported EN_MISSION version");
    if(saved.active&&(saved.members.empty()||saved.platoons.empty()))throw std::runtime_error("invalid EN_MISSION state");
    if(a.reading){if(isolated)*isolated=saved;else {persistent=saved;restoreElapsed=restoreRetry=0;restoreFault=false;}}
}

inline bool restorePersistentState(float restoreElapsed)
{
    if(!persistent.active)return true;
    State rebuilt;rebuilt.active=true;
    for(size_t i=0;i<persistent.members.size();++i){PersistentMember& p=persistent.members[i];
        Character* character=p.handle.getCharacter();
        if(!character)return waitForRestore(restoreElapsed,"handle not resolved",p.handle);
        const hand currentHandle=character->getHandle();
        const bool sameType=currentHandle.type==p.handle.type;
        const bool sameContainer=currentHandle.container==p.handle.container;
        const bool sameContainerSerial=currentHandle.containerSerial==p.handle.containerSerial;
        const bool sameIndex=currentHandle.index==p.handle.index;
        const bool sameSerial=currentHandle.serial==p.handle.serial;
        const bool sameHandle=samePersistentHandleIdentity(p.handle,currentHandle);
        const bool inPlayers=isInPlayerCharacters(character);
        ActivePlatoon* currentPlatoon=character->getPlatoon();
        const bool hasMovement=character->getMovement()!=0;
        if(!sameHandle){
            std::ostringstream s;s<<"RESTORE FATAL | saved_handle="<<p.handle.toString()<<" | current_handle="<<currentHandle.toString()
                <<" | components(type/container/containerSerial/index/serial)="
                <<(sameType?"Y":"N")<<"/"<<(sameContainer?"Y":"N")<<"/"<<(sameContainerSerial?"Y":"N")<<"/"<<(sameIndex?"Y":"N")<<"/"<<(sameSerial?"Y":"N");log(s.str());
            throw std::runtime_error("EN_MISSION Character durable identity mismatch");
        }
        if(!ou||!ou->player||ou->player->playerCharacters.size()==0)
            return waitForRestore(restoreElapsed,"playerCharacters not initialized",p.handle);
        if(!inPlayers)return waitForRestore(restoreElapsed,"Character not yet confirmed in playerCharacters",p.handle);
        if(!currentPlatoon)return waitForRestore(restoreElapsed,"platoon not ready",p.handle);
        if(!hasMovement)return waitForRestore(restoreElapsed,"movement not ready",p.handle);
        std::string factionId=character->getFaction()&&character->getFaction()->getData()?character->getFaction()->getData()->stringID:"";
        if(factionId.empty()&&!p.factionId.empty())return waitForRestore(restoreElapsed,"faction not ready",p.handle);
        if(factionId!=p.factionId)throw std::runtime_error("EN_MISSION Character faction mismatch");
        if(!sameContainer||!sameContainerSerial){
            std::ostringstream s;s<<"RESTORE REMAP | saved_handle="<<p.handle.toString()<<" | current_handle="<<currentHandle.toString()
                <<" | durable_identity=VALID";log(s.str());
        }
        Member m;m.groupId=p.groupId.empty()?"legacy":p.groupId;m.handle=currentHandle;m.character=character;m.platoon=character->getPlatoon();m.faction=character->getFaction();
        m.originalIndex=p.originalIndex;m.returnPosition=p.returnPosition;m.technicalPosition=p.technicalPosition;
        m.wasVisible=p.wasVisible;m.visualsWereActive=p.visualsWereActive;rebuilt.members.push_back(m);
    }
    for(size_t i=0;i<persistent.platoons.size();++i){PersistentPlatoon& p=persistent.platoons[i];
        if(p.originalOrder.empty())throw std::runtime_error("empty EN_MISSION platoon order");
        PlatoonOrder order;ActivePlatoon* expected=0;
        for(size_t j=0;j<p.originalOrder.size();++j){Character* character=p.originalOrder[j].getCharacter();
            if(!character)return waitForRestore(restoreElapsed,"platoon order Character not resolved",p.originalOrder[j]);
            if(!samePersistentHandleIdentity(p.originalOrder[j],character->getHandle())||!isInPlayerCharacters(character)||!character->getPlatoon())
                throw std::runtime_error("EN_MISSION platoon member identity mismatch");
            if(!expected)expected=character->getPlatoon();else if(character->getPlatoon()!=expected)
                throw std::runtime_error("EN_MISSION platoon changed");
            order.original.push_back(character);
        }
        order.platoon=expected;rebuilt.platoons.push_back(order);
    }
    state=rebuilt;
    if(!reorderSelectedToEnd()){discardRuntimeReferences();throw std::runtime_error("EN_MISSION reload reorder failed");}
    restoreAllHiddenPortraitCells();refreshPortraits();
    for(size_t i=0;i<state.members.size();++i){Member& m=state.members[i];
        if(ou&&ou->player)ou->player->unselectPlayerCharacter(m.character);
        hide(m);m.character->getMovement()->_setPositionAndTeleport(m.technicalPosition,0);hide(m);
    }
    refreshPortraits();snapshot("RESTORED AFTER LOAD");persistent=PersistentState();
    restoreWaitLastLogSecond=-1;restoreWaitLastPlayerCount=static_cast<size_t>(-1);
    log("EN_MISSION persistent state restored");return true;
}

}

void (*missionPortraitUpdateOriginal)(PortraitMainCellView*,const MyGUI::IBDrawItemInfo&,PortraitData*);
void missionPortraitUpdateHook(PortraitMainCellView* cell,const MyGUI::IBDrawItemInfo& info,PortraitData* data)
{MissionAbsencePrototype::restoreHiddenPortraitCell(cell);missionPortraitUpdateOriginal(cell,info,data);MissionAbsencePrototype::hidePortraitCell(cell);}

void (*missionSelectObjectOriginal)(PlayerInterface*,RootObject*,bool);
void missionSelectObjectHook(PlayerInterface* player,RootObject* object,bool modifier)
{if(!MissionAbsencePrototype::isAbsent(object))missionSelectObjectOriginal(player,object,modifier);}

void (*missionSelectPlayerCharacterOriginal)(PlayerInterface*,int,bool,bool);
void missionSelectPlayerCharacterHook(PlayerInterface* player,int index,bool modifier,bool track)
{if(!player||index<0||static_cast<size_t>(index)>=player->playerCharacters.size()||!MissionAbsencePrototype::isAbsent(player->playerCharacters[index]))missionSelectPlayerCharacterOriginal(player,index,modifier,track);}

void (*missionSelectPlayerObjectOriginal)(PlayerInterface*,RootObject*,bool,bool);
void missionSelectPlayerObjectHook(PlayerInterface* player,RootObject* object,bool modifier,bool track)
{if(!MissionAbsencePrototype::isAbsent(object))missionSelectPlayerObjectOriginal(player,object,modifier,track);}

void (*missionAddOrderOriginal)(Character*,Building*,TaskType,RootObject*,bool,bool,const Ogre::Vector3&);
void missionAddOrderHook(Character* character,Building* destination,TaskType task,RootObject* subject,bool shift,bool clear,const Ogre::Vector3& position)
{if(MercenarieCleanup::disabled){missionAddOrderOriginal(character,destination,task,subject,shift,clear,position);return;}if(!MissionAbsencePrototype::isAbsent(character)){if(observeMissionBookNativeOrder(character,destination,subject,task,true))return;missionAddOrderOriginal(character,destination,task,subject,shift,clear,position);}}

void (*missionAddJobOriginal)(Character*,TaskType,RootObject*,bool,bool,const Ogre::Vector3&);
void missionAddJobHook(Character* character,TaskType task,RootObject* subject,bool shift,bool dontClear,const Ogre::Vector3& position)
{if(MercenarieCleanup::disabled){missionAddJobOriginal(character,task,subject,shift,dontClear,position);return;}if(!MissionAbsencePrototype::isAbsent(character)){missionAddJobOriginal(character,task,subject,shift,dontClear,position);observeMissionBookNativeOrder(character,0,subject,task,false);}}

bool (*missionBookTryOperateOriginal)(UseableStuff*,const hand&);
bool missionBookTryOperateHook(UseableStuff* usable,const hand& actor)
{if(MercenarieCleanup::disabled)return missionBookTryOperateOriginal(usable,actor);bool accepted=missionBookTryOperateOriginal(usable,actor);observeMissionBookNativeTryOperate(usable,actor,accepted);return accepted;}
