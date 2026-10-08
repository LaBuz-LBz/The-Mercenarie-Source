#pragma once
#include "GuildPayroll.h"
namespace GuildPayroll {
template<class Archive>void archiveOptional(Archive& a,State& live,bool apply){
    State restored;if(!a.reading)restored=live;
    if(!a.reading||a.cursor<a.bytes.size())restored.archive(a);
    if(a.reading&&apply)live=restored;
}
}
