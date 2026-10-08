#pragma once
#include <string>
#include <cstddef>
namespace GuildBuildingTypes {
enum State { Available, Locked, ComingSoon };
struct Type {const char* id;const char* name;const char* description;int icon;bool enabled;};
static const Type types[]={
 {"office","building.office","building.description",0,true},
 {"future.1","building.soon","building.soon",3,false},
 {"future.2","building.soon","building.soon",3,false},
 {"future.3","building.soon","building.soon",3,false},
 {"future.4","building.soon","building.soon",3,false},
 {"future.5","building.soon","building.soon",3,false}};
inline int limit(int level){return level<3?0:level==3?1:3;}
inline State state(const Type& t,int level,size_t count,bool occupied,bool valid){return !t.enabled?ComingSoon:(!valid||level<3||count>=size_t(limit(level))||occupied)?Locked:Available;}
inline std::string cleanName(const std::string& value,const std::string& fallback){
 std::string result;size_t chars=0;
 for(size_t i=0;i<value.size();++i){unsigned char c=value[i];if(c<32||c=='|'||c==127||c=='#')continue;if((c&0xc0)!=0x80){if(chars==64)break;++chars;}result+=value[i];}
 size_t a=result.find_first_not_of(" \t\r\n"),b=result.find_last_not_of(" \t\r\n");return a==std::string::npos?fallback:result.substr(a,b-a+1);
}
}
