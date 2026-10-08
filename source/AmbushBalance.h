#pragma once

// The source faction only decides whether a group is lore-consistent.  The
// contract danger decides which of our explicitly reviewed vanilla archetypes
// may be used.  Keeping that policy here makes it testable without Kenshi.
namespace AmbushBalance {
enum Archetype { Cannibal, DustRaid, DustSquad, StarvingMob, SandJounin, BloodRaider };

struct Profile {
    int minimum, maximum;
    Profile(int lo=5,int hi=15):minimum(lo),maximum(hi){}
};

inline Profile profileForDifficulty(int difficulty) {
    static const Profile profiles[] = {
        Profile(5,15), Profile(15,25), Profile(20,40),
        Profile(35,50), Profile(50,60)
    };
    if(difficulty<1)difficulty=1;if(difficulty>5)difficulty=5;
    return profiles[difficulty-1];
}

inline bool allowed(int difficulty, Archetype type) {
    if(difficulty<1)difficulty=1;if(difficulty>5)difficulty=5;
    // Low/normal contracts are deliberately restricted to basic bandits.
    if(difficulty<=2)return type==DustRaid||type==DustSquad||type==StarvingMob;
    if(difficulty==3)return type==Cannibal||type==DustRaid||type==DustSquad||type==StarvingMob;
    if(difficulty==4)return type!=SandJounin;
    return true;
}

inline bool valid(Profile p) { return p.minimum>=0 && p.maximum>=p.minimum && p.maximum<=100; }
}
