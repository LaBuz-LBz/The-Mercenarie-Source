#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <locale>

// Versioned in route metadata, before PATH: old offers keep their old behaviour.
namespace ContractGroupPlan {
struct Entry {
    char role; std::string id; int count;
    Entry(char r='L',const std::string& s="",int n=1):role(r),id(s),count(n){}
};
struct Plan {
    std::vector<Entry> entries;
    int people()const {int n=0;for(size_t i=0;i<entries.size();++i)if(entries[i].role!='A')n+=entries[i].count;return n;}
    int animals()const {int n=0;for(size_t i=0;i<entries.size();++i)if(entries[i].role=='A')n+=entries[i].count;return n;}
    bool valid()const {
        if(entries.empty()||entries.size()>16||people()<1||people()+animals()>8)return false;
        int leaders=0;
        for(size_t i=0;i<entries.size();++i){const Entry& e=entries[i];
            if(e.role!='L'&&e.role!='S'&&e.role!='A')return false;
            if(e.count<1||e.count>8||e.id.empty()||e.id.size()>200||e.id.find_first_of(",;:|\r\n")!=std::string::npos)return false;
            if(e.role=='L'){++leaders;if(e.count!=1)return false;}
            for(size_t j=0;j<i;++j)if(e.role==entries[j].role&&e.id==entries[j].id)return false;
        }return leaders==1;
    }
};
// Apply only to mission caravans, retaining legacy decoding for existing saves.
inline void limitRole(Plan& p,char role,int maximum){
    int left=maximum>0?maximum:0;
    for(size_t i=0;i<p.entries.size();){Entry& e=p.entries[i];if(e.role!=role){++i;continue;}
        if(left==0){p.entries.erase(p.entries.begin()+i);continue;}
        if(e.count>left)e.count=left;left-=e.count;++i;
    }
}
inline void limitAnimals(Plan& p,int maximum){limitRole(p,'A',maximum);}
inline void limitCaravan(Plan& p){limitRole(p,'A',2);limitRole(p,'S',4);}
inline std::string encode(const Plan& p){
    if(!p.valid())return "";std::ostringstream s;s.imbue(std::locale::classic());s<<";GROUP1=";
    for(size_t i=0;i<p.entries.size();++i){if(i)s<<':';s<<p.entries[i].role<<','<<p.entries[i].count<<','<<p.entries[i].id;}s<<';';return s.str();
}
inline bool decode(const std::string& metadata,Plan& out){
    out.entries.clear();size_t start=metadata.find(";GROUP1=");if(start==std::string::npos)return false;
    size_t end=metadata.find(';',start+8);if(end==std::string::npos||end-start>4096)return false;
    std::istringstream in(metadata.substr(start+8,end-start-8));std::string part;Plan p;
    while(std::getline(in,part,':')){std::istringstream fields(part);fields.imbue(std::locale::classic());Entry e;char a,b;
        if(!(fields>>e.role>>a>>e.count>>b)||a!=','||b!=','||!std::getline(fields,e.id)||p.entries.size()>=16)return false;p.entries.push_back(e);
    }if(!p.valid())return false;out=p;return true;
}
struct Binding {int* values;int count;Binding(int* v,int n):values(v),count(n){}};
inline int extraClientPercent(const Plan& p){int n=(p.people()-1)*5;return n>25?25:n;}
// Allocate backups before touching the shared template. Restore on normal return
// and C++ exceptions; no disk writes and no reference-list reallocation.
class ScopedCounts {
    struct Saved {int* values;int a,b;Saved(int* v):values(v),a(v[0]),b(v[1]){}};
    std::vector<Saved> saved;
    ScopedCounts(const ScopedCounts&);ScopedCounts& operator=(const ScopedCounts&);
public:
    explicit ScopedCounts(const std::vector<Binding>& bindings){
        saved.reserve(bindings.size());for(size_t i=0;i<bindings.size();++i)saved.push_back(Saved(bindings[i].values));
        for(size_t i=0;i<bindings.size();++i)bindings[i].values[0]=bindings[i].values[1]=bindings[i].count;
    }
    ~ScopedCounts(){for(size_t i=0;i<saved.size();++i){saved[i].values[0]=saved[i].a;saved[i].values[1]=saved[i].b;}}
};
}
