#pragma once
#include "RoutePrototype.h"
#include <sstream>
#include <locale>
namespace ContractRouteVisual {
inline std::string encode(const std::vector<RoutePrototype::Point>& points){
 std::ostringstream s;s.imbue(std::locale::classic());s.precision(10);s<<";PATH=";
 for(size_t i=0;i<points.size();++i)s<<points[i].x<<','<<points[i].y<<','<<points[i].z<<';';return s.str();
}
inline std::vector<RoutePrototype::Point> decode(const std::string& metadata){
 std::vector<RoutePrototype::Point> result;size_t start=metadata.find(";PATH=");if(start==std::string::npos)return result;
 size_t end=metadata.find(";;",start+6);
 std::istringstream in(metadata.substr(start+6,end==std::string::npos?std::string::npos:end-(start+6)+1));in.imbue(std::locale::classic());
 while(in.peek()!=EOF){RoutePrototype::Point p;char a,b,c;if(!(in>>p.x>>a>>p.y>>b>>p.z>>c)||a!=','||b!=','||c!=';'||!p.valid()||result.size()>=200000){result.clear();return result;}result.push_back(p);}
 return result;
}
}
