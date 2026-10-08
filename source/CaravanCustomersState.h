#pragma once
#include "MissionArchive.h"
#include <set>
namespace CaravanCustomers {
inline bool identity(const std::string& s){int parts=1,digits=0;double n=0;for(size_t i=0;i<s.size();++i){if(s[i]=='-'){if(!digits)return false;++parts;digits=0;n=0;}else if(s[i]>='0'&&s[i]<='9'){++digits;n=n*10+(s[i]-'0');if(n>4294967295.0)return false;}else return false;}return parts==5&&digits>0;}
struct Client {
    std::string actor,home;int stage,line,order;float clock,retry,x,y,z;
    Client():stage(0),line(0),order(-1),clock(0),retry(0),x(0),y(0),z(0){}
    void archive(MissionArchive& a){a.field(actor);a.field(home);a.field(stage);a.field(line);a.field(clock);a.field(retry);a.field(order);a.field(x);a.field(y);a.field(z);
        if(!identity(actor)||!identity(home)||stage<0||stage>4||line<0||line>9||clock<0||clock>3600||retry<0||retry>3600)throw std::runtime_error("invalid commerce client");}
};
struct Group {
    std::string id,town,merchant;float x,y,z,busy;bool called,retiring;std::vector<Client> clients;
    Group():x(0),y(0),z(0),busy(0),called(false),retiring(false){}
    void archive(MissionArchive& a){a.field(id);a.field(town);a.field(merchant);a.field(x);a.field(y);a.field(z);a.field(busy);a.field(called);a.field(retiring);unsigned int n=(unsigned int)clients.size();a.field(n);
        if(id.empty()||id.size()>256||town.size()>256||!identity(merchant)||n>4||busy<0||busy>3600)throw std::runtime_error("invalid commerce group");
        a.requireElements(n,1);if(a.reading)clients.resize(n);for(unsigned int i=0;i<n;++i)clients[i].archive(a);}
};
inline std::vector<Group> load(const std::string& bytes){std::vector<Group> result;if(bytes.empty())return result;MissionArchive a(bytes,256*1024);unsigned int n=0;a.field(n);if(n>256)throw std::runtime_error("too many commerce groups");a.requireElements(n,1);result.resize(n);std::set<std::string> ids,actors;for(unsigned int i=0;i<n;++i){result[i].archive(a);if(!ids.insert(result[i].id).second)throw std::runtime_error("duplicate commerce phase");for(size_t j=0;j<result[i].clients.size();++j)if(!actors.insert(result[i].clients[j].actor).second)throw std::runtime_error("duplicate commerce actor");}a.finish();return result;}
inline std::string save(std::vector<Group>& groups){MissionArchive a(256*1024);unsigned int n=(unsigned int)groups.size();a.field(n);for(size_t i=0;i<groups.size();++i)groups[i].archive(a);return a.bytes;}
inline bool purchased(const Client& c){return c.stage==3&&c.line==9;}
inline bool complete(const Group& g){if(g.clients.size()<2||g.clients.size()>4)return false;for(size_t i=0;i<g.clients.size();++i)if(!purchased(g.clients[i]))return false;return true;}
inline int speaker(const Group& g){for(size_t i=0;i<g.clients.size();++i)if(!purchased(g.clients[i]))return (int)i;return -1;}
inline bool unseen(bool playerNearby,bool cameraNearby,bool onScreen,bool protectedActor){return !playerNearby&&!cameraNearby&&!onScreen&&!protectedActor;}
}
