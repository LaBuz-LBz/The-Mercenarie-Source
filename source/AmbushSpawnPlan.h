#pragma once
#include <algorithm>
#include <vector>

// Engine-independent ambush sizing. Runtime code translates FCS references to
// these entries, applies the returned fixed counts only for the duration of the
// native factory call, then restores the original template through RAII.
namespace AmbushSpawnPlan {
// Vanilla leader tuples use both (1, 0, 0) and (1, 100, 0).
// Zero is a valid native default, not a missing/disabled leader.
inline bool validLeaderTuple(int count,int probability){return count>=1&&count<=1024&&probability>=0&&probability<=100;}
enum Role { Leader, Member, Animal };

struct Entry {
    Role role;
    int minimum;
    int maximum;
    Entry(Role r=Member,int lo=0,int hi=0):role(r),minimum(lo),maximum(hi){}
};

struct Plan {
    int cap;
    int maximumActors;
    bool overrideCounts;
    std::vector<int> fixedCounts;
    Plan():cap(0),maximumActors(0),overrideCounts(false){}
};

inline int capForDifficulty(int difficulty)
{
    static const int caps[]={7,12,16,20,20};
    difficulty=std::max(1,std::min(5,difficulty));
    return caps[difficulty-1];
}

inline int minimumForDifficulty(int difficulty){
    static const int counts[]={5,8,12,16,20};
    return counts[std::max(1,std::min(5,difficulty))-1];
}
inline bool build(const std::vector<Entry>& entries,int difficulty,Plan& out,int requested=0)
{
    out=Plan();out.cap=capForDifficulty(difficulty);
    if(entries.empty()||entries.size()>64)return false;
    const int wanted=requested?requested:out.cap;
    if(wanted<minimumForDifficulty(difficulty)||wanted>out.cap)return false;
    int leaders=0,nonLeaderKinds=0;
    for(size_t i=0;i<entries.size();++i){
        const Entry& e=entries[i];
        if(e.minimum<0||e.maximum<e.minimum||e.maximum>1024)return false;
        if(e.role==Leader){
            // Leaders are never rewritten: their FCS count/probability tuple has
            // different semantics from member min/max tuples.
            if(e.minimum<1||e.maximum!=e.minimum)return false;
            leaders+=e.maximum;
        }else if(e.maximum>0)++nonLeaderKinds;
    }
    if(leaders<1||leaders>wanted||leaders+nonLeaderKinds>wanted)return false;
    if(!nonLeaderKinds&&leaders!=wanted)return false;

    out.overrideCounts=true;
    out.fixedCounts.assign(entries.size(),0);
    int remaining=wanted-leaders;
    for(size_t i=0;i<entries.size();++i){
        if(entries[i].role==Leader)out.fixedCounts[i]=entries[i].maximum;
        else if(entries[i].maximum>0){out.fixedCounts[i]=1;--remaining;}
    }
    // Preserve every enabled member/animal kind and all leaders. Scale only
    // ordinary member counts to the requested contract population (at most 20).
    while(remaining>0){
        bool advanced=false;
        for(size_t i=0;i<entries.size()&&remaining>0;++i){
            if(entries[i].role==Leader||entries[i].maximum==0)continue;
            ++out.fixedCounts[i];--remaining;advanced=true;
        }
        if(!advanced)break;
    }
    out.maximumActors=leaders;
    for(size_t i=0;i<entries.size();++i)if(entries[i].role!=Leader)out.maximumActors+=out.fixedCounts[i];
    return out.maximumActors==wanted&&out.maximumActors<=out.cap&&out.maximumActors<=20;
}
}
