#pragma once
namespace WorldServiceClock {
// All services observe the same Kenshi world hours plus the existing debug
// simulation offset. Wall-clock time and UI frame rate never enter this value.
inline double now(double worldHours,double debugHours){return worldHours+debugHours;}
}
