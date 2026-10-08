#include "MercenarySelectionRules.h"
#include <cassert>
#include <iostream>
using namespace MercenarySelectionRules;
int main()
{
    assert(maySelect(false,true,0,1)==LastCharacterRequired);
    assert(maySelect(false,true,0,2)==SelectAllowed);
    assert(maySelect(false,false,0,5)==CharacterUnavailable);
    assert(maySelect(false,true,30,50)==MaximumReached);
    assert(maySelect(true,true,30,31)==AlreadySelected);
    const int hybrid=categoryMask(45,35,15,4,2);assert(visibleInFilter(hybrid,Combat));assert(visibleInFilter(hybrid,Ranged));assert(!visibleInFilter(hybrid,Work));assert(visibleInFilter(hybrid,All));
    const int novice=categoryMask(2,3,4,5,6);assert(novice==(1<<Specialist));assert(primaryCategory(2,3,4,5,6)==Specialist);
    assert(columnsForWidth(160)==1);assert(columnsForWidth(720)==4);
    std::cout<<"mercenary selection rules: OK\n";
}
