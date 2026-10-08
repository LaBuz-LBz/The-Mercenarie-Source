#include "ZetaFixRules.h"
#include <cassert>
#include <iostream>

int main()
{
    using namespace ZetaFixRules;
    assert(fiscalChoiceKeepsDialogue("880234-Guild Escort Contracts.mod"));
    assert(fiscalChoiceKeepsDialogue("880235-Guild Escort Contracts.mod"));
    assert(!fiscalChoiceKeepsDialogue("880236-Guild Escort Contracts.mod"));
    assert(closeGiverAfterSelection(true,true));
    assert(!closeGiverAfterSelection(false,true));
    assert(!closeGiverAfterSelection(true,false));
    assert(routeMarkerCount(1)==2);
    assert(routeMarkerCount(2)==3);
    assert(routeMarkerCount(3)==4);
    assert(mayDestroyDepartingVisitor(true,false,false));
    assert(!mayDestroyDepartingVisitor(false,false,false));
    assert(!mayDestroyDepartingVisitor(true,true,false));
    assert(!mayDestroyDepartingVisitor(true,false,true));
    assert(!mayDestroyTemporaryNpc(false,false,4000000.0));
    assert(!mayDestroyTemporaryNpc(true,true,4000000.0));
    assert(!mayDestroyTemporaryNpc(true,false,40000.0));
    assert(!mayDestroyTemporaryNpc(true,false,250000.0));
    assert(!mayDestroyTemporaryNpc(true,false,998001.0));
    assert(mayDestroyTemporaryNpc(true,false,1000000.0));
    assert(mayDestroyTemporaryNpc(true,false,1440000.0));
    assert(mayStartVisitorDialogue(false));
    assert(!mayStartVisitorDialogue(true));
    assert(restoredHandleMatches(22,1140869120u,1u,22,1140869120u,1u));
    assert(!restoredHandleMatches(22,1140869120u,1u,22,1140869121u,1u));
    assert(!restoredHandleMatches(22,1140869120u,1u,22,1140869120u,2u));
    assert(shouldRequeuePaymentVisitor(true,false,false));
    assert(shouldRequeuePaymentVisitor(true,true,false));
    assert(!shouldRequeuePaymentVisitor(true,true,true));
    assert(!shouldRequeuePaymentVisitor(false,false,false));
    std::cout<<"fixe zeta tests: OK\n";
}
