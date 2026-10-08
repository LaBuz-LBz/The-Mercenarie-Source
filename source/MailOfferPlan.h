#pragma once
#include "MailContracts.h"
#include <sstream>

namespace MailOfferPlan {
struct Destination { std::string townId,townName;double distanceKm;MailContracts::SenderRole role;Destination():distanceKm(0),role(MailContracts::RoleUndefined){} };
struct Plan { bool urgent;std::vector<Destination> destinations;Plan():urgent(false){} };
inline std::string encode(const Plan& p){std::ostringstream o;o<<";MAIL1="<<(p.urgent?1:0)<<','<<p.destinations.size();for(size_t i=0;i<p.destinations.size();++i)o<<','<<p.destinations[i].townId<<'~'<<p.destinations[i].townName<<'~'<<p.destinations[i].distanceKm<<'~'<<(int)p.destinations[i].role;o<<';';return o.str();}
inline bool decode(const std::string& text,Plan& p){size_t at=text.find(";MAIL1=");if(at==std::string::npos)return false;size_t end=text.find(';',at+7);if(end==std::string::npos)return false;std::string body=text.substr(at+7,end-at-7);std::istringstream in(body);std::string field;if(!std::getline(in,field,','))return false;p=Plan();p.urgent=atoi(field.c_str())!=0;if(!std::getline(in,field,','))return false;int count=atoi(field.c_str());if(count<1||count>3)return false;for(int i=0;i<count;++i){if(!std::getline(in,field,','))return false;std::istringstream row(field);Destination d;std::string km,role;if(!std::getline(row,d.townId,'~')||!std::getline(row,d.townName,'~')||!std::getline(row,km,'~')||!std::getline(row,role,'~'))return false;d.distanceKm=atof(km.c_str());d.role=(MailContracts::SenderRole)atoi(role.c_str());if(d.townId.empty()||d.townName.empty()||d.distanceKm<=0||d.role==MailContracts::RoleUndefined)return false;p.destinations.push_back(d);}std::set<std::string> towns;for(size_t i=0;i<p.destinations.size();++i)if(!towns.insert(p.destinations[i].townId).second)return false;return true;}
}
