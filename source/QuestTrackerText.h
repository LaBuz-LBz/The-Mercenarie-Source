#include "Localization.h"
#pragma once
#include <algorithm>
#include <sstream>
#include <string>
namespace QuestTrackerText {
inline std::string count(int n,bool){return Loc::count("quest.active_count",std::max(0,n));}
inline std::string distance(double metres,bool english){
    if(!(metres>=0)||metres>1e12)return "";
    std::ostringstream s;s.precision(1);s<<std::fixed<<metres/1000.0<<Loc::text("ui.km");std::string text=s.str();if(!english)std::replace(text.begin(),text.end(),'.',',');return text;
}
// Exo2 ships without the Unicode right-arrow glyph; keep the route readable.
inline const char* arrow(){return " > ";}
}
