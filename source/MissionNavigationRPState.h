#pragma once
namespace MissionNavigationRP {
template<class V> struct State {
    bool thinking,spoken,awaitingGuide;float moving;V origin;
    State():thinking(false),spoken(false),awaitingGuide(false),moving(0){}
};
}
