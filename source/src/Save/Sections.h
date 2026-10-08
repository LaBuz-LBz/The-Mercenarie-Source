#pragma once
#include "../../MissionArchive.h"
#include <set>
namespace SaveSections {
struct Section {std::string id,bytes;unsigned int version;Section(const std::string& i="",unsigned int v=1,const std::string& b=""):id(i),bytes(b),version(v){}};
template<class A> void archive(A& a,std::vector<Section>& sections){
    std::string magic="MERCENARIE-SECTIONS-1";a.field(magic);if(magic!="MERCENARIE-SECTIONS-1")throw std::runtime_error("unsupported save sections");
    unsigned int n=(unsigned int)sections.size();a.field(n);if(n>32)throw std::runtime_error("too many save sections");
    if(a.reading){if(n>(a.bytes.size()-a.cursor)/12)throw std::runtime_error("truncated section table");sections.resize(n);}
    std::set<std::string> ids;
    for(unsigned int i=0;i<n;++i){Section& s=sections[i];a.field(s.id);a.field(s.version);a.field(s.bytes);unsigned int hash=missionChecksum(s.bytes);a.field(hash);
        if(s.id.empty()||s.id.size()>64||!ids.insert(s.id).second||hash!=missionChecksum(s.bytes))throw std::runtime_error("invalid save section");
    }
}
}
