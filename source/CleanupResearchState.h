#pragma once
#include "CleanupState.h"
namespace MercenarieCleanup {
// Only audited mod-owned identities. No native currency refund or global FCS edit.
struct ResearchCounts {unsigned finished,enabled,known,queued,paid;ResearchCounts():finished(0),enabled(0),known(0),queued(0),paid(0){}unsigned total()const{return finished+enabled+known+queued+paid;}};
inline ResearchCounts clearResearch(Research& state){
    ResearchCounts result;
    for(ogre_unordered_set<GameData*>::type::iterator i=state.finished.begin();i!=state.finished.end();){if(*i&&ownedDefinition((*i)->stringID)){state.finished.erase(i++);++result.finished;}else ++i;}
    for(ogre_unordered_set<GameData*>::type::iterator i=state.enabledObjects.begin();i!=state.enabledObjects.end();){if(*i&&ownedDefinition((*i)->stringID)){state.enabledObjects.erase(i++);++result.enabled;}else ++i;}
    for(Ogre::map<itemType,lektor<GameData*> >::type::iterator i=state.knownObjectsByType.begin();i!=state.knownObjectsByType.end();++i){
        lektor<GameData*>& list=i->second;unsigned int kept=0,old=list.count;
        for(unsigned int n=0;n<old;++n){GameData* d=list[n];if(d&&ownedDefinition(d->stringID))++result.known;else list.stuff[kept++]=d;}
        list.count=kept;for(unsigned int n=kept;n<old;++n)list.stuff[n]=0;
    }
    for(Ogre::deque<ResearchItem>::type::iterator i=state.researchQueue.begin();i!=state.researchQueue.end();){if(i->gamedata&&ownedDefinition(i->gamedata->stringID)){i=state.researchQueue.erase(i);++result.queued;}else ++i;}
    unsigned int kept=0,old=state.paid.count;
    for(unsigned int n=0;n<old;++n){if(ownedDefinition(state.paid[n]))++result.paid;else state.paid.stuff[kept++]=state.paid[n];}
    state.paid.count=kept;for(unsigned int n=kept;n<old;++n)state.paid.stuff[n].clear();
    if(result.total())state.changedSoUpdateGUI=true;
    return result;
}
}
