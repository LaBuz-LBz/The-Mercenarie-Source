#pragma once
#include <vector>
// Runtime-only ownership of unresolved identities. The wire format stays a
// numeric hand (or vector of hands). Never retain addresses inside STL vectors.
namespace SavedActorResolution {
inline Character* find(const hand& id){
    MercenariePerf::Phase perf("references");
    if(id.isNull())return 0;
    Character* actor=id.getCharacter();if(!actor)return 0;
    const hand current=actor->getHandle();
    return current.type==id.type&&current.index==id.index&&current.serial==id.serial?actor:0;
}
struct Actor {
    hand identity;bool pending;float retry;
    Actor():pending(false),retry(0){}
    bool resolve(Character*& actor,float elapsed){
        if(!pending)return true;retry-=elapsed;if(retry>0)return false;retry=1;
        Character* found=find(identity);if(!identity.isNull()&&!found)return false;
        actor=found;pending=false;return true;
    }
};
struct Group {
    std::vector<hand> identities;bool pending;float retry;
    Group():pending(false),retry(0){}
    bool resolve(std::vector<Character*>& actors,float elapsed){
        if(!pending)return true;retry-=elapsed;if(retry>0)return false;retry=1;
        std::vector<Character*> found;found.reserve(identities.size());
        for(size_t i=0;i<identities.size();++i){Character* c=find(identities[i]);if(!identities[i].isNull()&&!c)return false;found.push_back(c);}
        actors.swap(found);pending=false;return true;
    }
};
template<class A> void archive(A& a,Character*& actor,Actor& saved){
    hand id=saved.identity;if(!a.reading&&!saved.pending){if(actor)id=actor->getHandle();else id.setNull();}
    a.field(id);
    if(a.reading){actor=0;saved.identity=id;saved.pending=!id.isNull();saved.retry=0;if(a.resolve)saved.resolve(actor,0);}
}
template<class A> void archive(A& a,std::vector<Character*>& actors,Group& saved){
    std::vector<hand> ids=saved.identities;if(!a.reading&&!saved.pending){ids.clear();for(size_t i=0;i<actors.size();++i)ids.push_back(actors[i]?actors[i]->getHandle():hand());}
    a.field(ids);
    if(a.reading){actors.clear();saved.identities.swap(ids);saved.pending=!saved.identities.empty();saved.retry=0;if(a.resolve)saved.resolve(actors,0);}
}
}
