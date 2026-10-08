#pragma once
#include <climits>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// Pure, bounded ranking for candidates sampled through the existing mission
// and destination rules. Lower priorities never override a higher one.
namespace ContractOfferVariety {
struct Candidate {
    int type;
    std::string destinationId,destinationName;
    float distance;
    Candidate():type(0),distance(0){}
};
inline std::string pairKey(int type,const std::string& destinationId){std::ostringstream out;out<<type<<'|'<<destinationId;return out.str();}
inline int score(const Candidate& c,const int typeCounts[5],const std::set<std::string>& destinations,const std::set<std::string>& pairs){
    int result=0;
    if(pairs.count(pairKey(c.type,c.destinationId)))result+=100000;
    if(destinations.count(c.destinationId))result+=10000;
    if(c.type>=0&&c.type<5)result+=typeCounts[c.type]*100;
    return result;
}
inline int choose(const std::vector<Candidate>& candidates,const int typeCounts[5],const std::set<std::string>& destinations,const std::set<std::string>& pairs){
    int best=-1,bestScore=INT_MAX;
    for(size_t i=0;i<candidates.size();++i){int value=score(candidates[i],typeCounts,destinations,pairs);if(value<bestScore){best=(int)i;bestScore=value;}}
    return best;
}
}
