#include <cassert>
#include <iostream>
#include "ContractOfferVariety.h"
using namespace ContractOfferVariety;
Candidate candidate(int type,const char* id){Candidate c;c.type=type;c.destinationId=id;return c;}
int main(){
    int counts[3]={2,0,0};std::set<std::string> destinations,pairs;destinations.insert("A");pairs.insert(pairKey(0,"A"));
    std::vector<Candidate> pool;pool.push_back(candidate(0,"A"));pool.push_back(candidate(0,"B"));pool.push_back(candidate(1,"A"));pool.push_back(candidate(1,"C"));
    assert(choose(pool,counts,destinations,pairs)==3); // new pair, destination and underused type
    destinations.insert("C");assert(choose(pool,counts,destinations,pairs)==1); // unused destination beats type count
    destinations.insert("B");pairs.insert(pairKey(0,"B"));pairs.insert(pairKey(1,"A"));pairs.insert(pairKey(1,"C"));
    assert(choose(pool,counts,destinations,pairs)>=0); // duplicates remain a bounded fallback
    std::vector<Candidate> six;int generated[3]={0,0,0};std::set<std::string> usedDest,usedPairs;
    for(int slot=0;slot<6;++slot){six.clear();for(int t=0;t<3;++t)for(int d=0;d<6;++d){std::ostringstream id;id<<char('A'+d);six.push_back(candidate(t,id.str().c_str()));}int pick=choose(six,generated,usedDest,usedPairs);assert(pick>=0);Candidate c=six[pick];++generated[c.type];usedDest.insert(c.destinationId);usedPairs.insert(pairKey(c.type,c.destinationId));}
    assert(generated[0]>0&&generated[1]>0&&generated[2]>0&&usedDest.size()==6&&usedPairs.size()==6);
    std::cout<<"contract offer variety tests: OK\n";
}
