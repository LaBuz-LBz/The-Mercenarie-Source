#pragma once
#include <string>
namespace LanguageCode {
// Language tags only, never paths. Keep saved preferences even if a pack is absent.
inline bool valid(const std::string& value) {
    if(value.size()<2||value.size()>24)return false;
    bool start=true;
    for(size_t i=0;i<value.size();++i){
        unsigned char c=value[i];
        if(c=='-'||c=='_'){if(start)return false;start=true;continue;}
        bool letter=(c>='a'&&c<='z')||(c>='A'&&c<='Z');
        if(!letter&&!(i>0&&!start&&c>='0'&&c<='9'))return false;
        start=false;
    }
    return !start;
}
}
