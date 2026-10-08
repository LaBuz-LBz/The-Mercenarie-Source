#pragma once
#include <cstddef>
#include <string>

namespace ZetaFixRules {
inline bool fiscalChoiceKeepsDialogue(const char* id)
{
    return id && (std::string(id)=="880234-Guild Escort Contracts.mod" ||
        std::string(id)=="880235-Guild Escort Contracts.mod");
}
inline bool closeGiverAfterSelection(bool selectionValidated,bool openedFromDialogue)
{
    return selectionValidated && openedFromDialogue;
}
inline std::size_t routeMarkerCount(std::size_t destinationCount)
{
    return 1 + destinationCount;
}
inline bool mayDestroyDepartingVisitor(bool delayElapsed,bool indoors,bool nearPlayer)
{
    return delayElapsed && !indoors && !nearPlayer;
}
inline bool mayDestroyTemporaryNpc(bool roleFinished,bool indoors,double nearestPlayerDistanceSquared)
{
    return roleFinished && !indoors && nearestPlayerDistanceSquared>=1000000.0;
}
inline bool mayStartVisitorDialogue(bool departing){return !departing;}
inline bool restoredHandleMatches(int savedType,unsigned int savedIndex,unsigned int savedSerial,
                                  int currentType,unsigned int currentIndex,unsigned int currentSerial)
{
    // Kenshi rebuilds container/containerSerial while loading a save.  The
    // object kind, object index and serial are the durable identity tuple.
    return savedType==currentType&&savedIndex==currentIndex&&savedSerial==currentSerial;
}
inline bool shouldRequeuePaymentVisitor(bool paymentVisitor,bool leaderResolved,bool allMembersResolved)
{
    return paymentVisitor&&(!leaderResolved||!allMembersResolved);
}
}
