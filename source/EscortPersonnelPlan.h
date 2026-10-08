#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
namespace EscortPersonnel {
struct Member {std::string id,target;bool enabled;Member(const std::string& i="",bool e=true):id(i),enabled(e){}};
inline bool identity(const std::string& s){return !s.empty()&&s.size()<160&&s.find_first_of(";:,|\r\n\t")==std::string::npos&&s.find_first_not_of("0123456789-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._ ")==std::string::npos;}
inline std::vector<Member> read(const std::string& text){
 std::vector<Member> out;size_t a=text.find(";STAFF1=");if(a==std::string::npos)return out;size_t b=text.find(';',a+8);if(b==std::string::npos||b-a>100000)return out;
 std::istringstream in(text.substr(a+8,b-a-8));std::string row;
 while(std::getline(in,row,':')){std::istringstream parts(row);Member m;std::string enabled;if(!std::getline(parts,m.id,',')||!std::getline(parts,enabled,',')||!std::getline(parts,m.target)||!identity(m.id)||(enabled!="0"&&enabled!="1")||(m.target!="-"&&!identity(m.target)))return std::vector<Member>();
 m.enabled=enabled=="1";if(m.target=="-")m.target.clear();for(size_t i=0;i<out.size();++i)if(out[i].id==m.id)return std::vector<Member>();if(out.size()>=256)return std::vector<Member>();out.push_back(m);}
 return out;
}
inline void write(std::string& text,const std::vector<Member>& members){
 size_t a=text.find(";STAFF1=");if(a!=std::string::npos){size_t b=text.find(';',a+8);if(b!=std::string::npos)text.erase(a,b-a+1);}
 if(members.empty())return;std::ostringstream out;out<<";STAFF1=";for(size_t i=0;i<members.size();++i){if(i)out<<':';out<<members[i].id<<','<<(members[i].enabled?1:0)<<','<<(members[i].target.empty()?"-":members[i].target);}out<<';';text+=out.str();
}
struct Position{float forward,side;Position(float f,float s):forward(f),side(s){}};
inline Position position(size_t index,size_t count,bool animals){float front=animals?64.0f:28.0f,rear=animals?88.0f:40.0f;
 if(index<2)return Position(-front,count==1?0:index==0?-18.0f:18.0f);
 size_t row=(index-2)/3,col=(index-2)%3;size_t columns=std::min<size_t>(3,count-2-row*3);float side=columns==1?0:columns==2?(col==0?-18.0f:18.0f):18.0f*(static_cast<float>(col)-1);return Position(rear+18.0f*row,side);}
}

#include "EscortPersonnelFormations.h"
