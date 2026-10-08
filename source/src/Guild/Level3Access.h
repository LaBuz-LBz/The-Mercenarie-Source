#pragma once

namespace GuildLevel3Access {
inline bool available(int level) { return level >= 3; }
inline const char* researchId() { return "880141-Guild Escort Contracts.mod"; }

// Reconcile both native sets, including old saves and research cache rebuilds.
// Do not cache by level: two different loaded saves can have the same level.
template<class ResearchState, class Record>
bool reconcile(ResearchState& state, Record* research, Record* book, int level) {
    if (!research || !book) return false;
    bool changed = false;
    if (available(level)) {
        changed = state.finished.insert(research).second;
        changed = state.enabledObjects.insert(book).second || changed;
    } else {
        changed = state.finished.erase(research) != 0;
        changed = state.enabledObjects.erase(book) != 0 || changed;
    }
    if (changed) state.changedSoUpdateGUI = true;
    return changed;
}
}
