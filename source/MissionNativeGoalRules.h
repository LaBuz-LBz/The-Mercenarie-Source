#pragma once
// Only autonomous travel competes with the mission controller. Combat, recovery,
// hunger and first aid remain native. Explicit orders bypass this filter.
inline bool missionConflictingNativeGoal(TaskType type){
    switch(type){
    case PATROL: case WANDERER: case PATROL_TOWN: case WANDER_TOWN:
    case WANDERING_TRADER: case SHOPPING: case RELAX_IN_TOWN_PACKAGE:
    case GO_HOMEBUILDING: case STAY_IN_HOME: case TRAVEL_TO_TARGET_TOWN:
    case FOLLOW_SQUADLEADER: return true;
    default: return false;
    }
}
