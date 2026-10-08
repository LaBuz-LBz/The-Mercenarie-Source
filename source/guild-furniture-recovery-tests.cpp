#include "GuildFurnitureRecovery.h"
#include <cassert>
#include <iostream>
int main(){
    GuildFurnitureRecovery::State state;GuildFurnitureRecovery::begin(state);
    assert(state.active&&!GuildFurnitureRecovery::tick(state,.5f));
    assert(GuildFurnitureRecovery::tick(state,.5f));GuildFurnitureRecovery::completeAttempt(state,false);
    assert(state.active&&state.attempts==1&&!GuildFurnitureRecovery::tick(state,1.0f));
    assert(GuildFurnitureRecovery::tick(state,1.0f));GuildFurnitureRecovery::completeAttempt(state,true);assert(!state.active);
    GuildFurnitureRecovery::begin(state);for(int i=0;i<15;++i){assert(GuildFurnitureRecovery::tick(state,i?2.0f:1.0f));GuildFurnitureRecovery::completeAttempt(state,false);}assert(!state.active&&state.attempts==15);
    assert(GuildFurnitureRecovery::sameHouse("office-a","office-a"));
    assert(!GuildFurnitureRecovery::sameHouse("office-a","office-b"));
    assert(!GuildFurnitureRecovery::sameHouse("","office-a"));
    std::cout<<"guild furniture recovery tests: OK\n";
}
