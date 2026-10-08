#pragma once
#include <algorithm>
namespace EscortPersonnel {
struct Formation {const char* id;const char* label;};
inline const Formation* formations(size_t& count){static const Formation values[]={{"classic","staff.design.classic"}};count=sizeof(values)/sizeof(values[0]);return values;}
inline std::string formation(const std::string& text){size_t a=text.find(";STAFF_FORM=");if(a==std::string::npos)return "classic";size_t b=text.find(';',a+12);std::string id=text.substr(a+12,b==std::string::npos?0:b-a-12);size_t count=0;const Formation* list=formations(count);for(size_t i=0;i<count;++i)if(id==list[i].id)return id;return "classic";}
inline void writeFormation(std::string& text,const std::string& id){size_t a=text.find(";STAFF_FORM=");if(a!=std::string::npos){size_t b=text.find(';',a+12);if(b!=std::string::npos)text.erase(a,b-a+1);}size_t count=0;const Formation* list=formations(count);for(size_t i=0;i<count;++i)if(id==list[i].id){text+=";STAFF_FORM="+id+";";return;}text+=";STAFF_FORM=classic;";}
// The order of selection is also the order written to the mission's personnel roster.
inline void choose(std::vector<size_t>& order,size_t index,bool on){std::vector<size_t>::iterator it=std::find(order.begin(),order.end(),index);if(on&&it==order.end())order.push_back(index);else if(!on&&it!=order.end())order.erase(it);}
inline Position formationPosition(const std::string& id,size_t index,size_t count,bool animals){return position(index,count,animals);}
}
