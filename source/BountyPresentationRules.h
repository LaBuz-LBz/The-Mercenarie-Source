#pragma once
#include <algorithm>

namespace BountyPresentationRules {
inline bool usesSearchArea(int missionType){return missionType==3;}
inline bool showsExactRoute(int missionType){return !usesSearchArea(missionType);}
inline double roundTripKilometres(double oneWayMetres){return std::max(0.0,oneWayMetres)*2.0/1000.0;}
}
