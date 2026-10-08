#pragma once
#include "src/GuildPayroll.h"
#include "ArtisanOrders.h"
namespace ImportRecovery {
struct Person {std::string id,name;bool robot;unsigned int loss;Person(const std::string& i,const std::string& n,bool r,unsigned int l):id(i),name(n),robot(r),loss(l){}};
inline bool payrollPlan(const GuildPayroll::State& state,const std::vector<Person>& people,std::map<std::string,std::string>& links){
 links.clear();std::set<std::string> used;
 for(std::map<std::string,GuildPayroll::Member>::const_iterator i=state.members.begin();i!=state.members.end();++i){if(i->second.former)continue;const Person* found=0;size_t count=0;
  for(size_t j=0;j<people.size();++j)if(people[j].name==i->second.name&&people[j].robot==i->second.robot){found=&people[j];++count;}
  if(count!=1||found->id.empty()||!used.insert(found->id).second)return false;links[i->first]=found->id;
 }
 for(std::map<std::string,std::string>::const_iterator i=links.begin();i!=links.end();++i)if(state.members.count(i->second)&&!links.count(i->second))return false;
 return true;
}
inline void relink(std::string& id,const std::map<std::string,std::string>& links){std::map<std::string,std::string>::const_iterator i=links.find(id);if(i!=links.end())id=i->second;}
inline GuildPayroll::State resumePayroll(const GuildPayroll::State& old,const std::vector<Person>& people,double hour){
 if(!(hour>=0&&hour<=1e9))throw std::runtime_error("invalid recovery clock");std::map<std::string,std::string> links;if(!payrollPlan(old,people,links))throw std::runtime_error("ambiguous payroll recovery");
 GuildPayroll::State next=old;next.members.clear();double delta=hour-old.now;next.start=std::max(0.0,old.start+delta);next.next=std::max(hour,old.next+delta);next.now=hour;if(!old.initialized)next.init(hour);
 for(std::map<std::string,GuildPayroll::Member>::const_iterator i=old.members.begin();i!=old.members.end();++i){GuildPayroll::Member m=i->second;relink(m.id,links);if(!m.former){m.last=hour;for(size_t j=0;j<people.size();++j)if(people[j].id==m.id)m.observedLoss|=people[j].loss;}next.members[m.id]=m;}
 for(size_t i=0;i<next.debts.size();++i)relink(next.debts[i].member,links);
 for(size_t i=0;i<next.bills.size();++i)for(size_t j=0;j<next.bills[i].lines.size();++j)relink(next.bills[i].lines[j].member,links);
 for(std::map<std::string,std::vector<std::string> >::iterator i=next.participants.begin();i!=next.participants.end();++i)for(size_t j=0;j<i->second.size();++j)relink(i->second[j],links);
 return next;
}
inline bool autoResumePayroll(GuildPayroll::State& state,bool& suspended,const std::vector<Person>& people,double hour){
 if(!suspended||!(hour>=0&&hour<=1e9))return false;
 std::map<std::string,std::string> links;if(!payrollPlan(state,people,links))return false;
 GuildPayroll::State recovered=resumePayroll(state,people,hour);
 state=recovered;suspended=false;return true;
}
inline bool held(const std::vector<std::string>& keys,const std::string& key){return std::find(keys.begin(),keys.end(),key)!=keys.end();}
inline std::vector<std::string> artisanKeys(const ArtisanOrders::Ledger& ledger){std::set<std::string> keys;for(size_t i=0;i<ledger.orders.size();++i)keys.insert(ledger.orders[i].artisan);for(std::map<std::string,std::vector<ArtisanOrders::Line> >::const_iterator i=ledger.baskets.begin();i!=ledger.baskets.end();++i)if(!i->second.empty())keys.insert(i->first);return std::vector<std::string>(keys.begin(),keys.end());}
inline ArtisanOrders::Ledger resumeArtisan(const ArtisanOrders::Ledger& old,const std::string& from,const std::string& to,const std::string& name,int roles,double now,double suspendedAt){
 if(from.empty()||to.empty()||!(now>=0&&now<=1e9))throw std::runtime_error("invalid artisan recovery");
 if(from!=to){for(size_t i=0;i<old.orders.size();++i)if(old.orders[i].artisan==to)throw std::runtime_error("artisan already has orders");std::map<std::string,std::vector<ArtisanOrders::Line> >::const_iterator basket=old.baskets.find(to);if(basket!=old.baskets.end()&&!basket->second.empty())throw std::runtime_error("artisan already has basket");}
 ArtisanOrders::Ledger next=old;bool found=false;double delta=suspendedAt>=0?std::max(0.0,now-suspendedAt):0;
 // Older suspended saves have no suspension timestamp. Restart unfinished work,
 // preserving paid value, quantities and completed deliveries.
 if(suspendedAt<0)for(size_t i=0;i<old.orders.size();++i)if(old.orders[i].artisan==from&&(old.orders[i].status==ArtisanOrders::Making||old.orders[i].status==ArtisanOrders::Waiting)){delta=std::max(0.0,now-old.orders[i].start);break;}
 for(size_t i=0;i<next.orders.size();++i){ArtisanOrders::Order& o=next.orders[i];if(o.artisan!=from)continue;found=true;for(size_t j=0;j<o.lines.size();++j)if(!(roles&(o.lines[j].kind?2:1)))throw std::runtime_error("incompatible artisan trade");o.artisan=to;o.name=name;o.created+=delta;o.start+=delta;o.end+=delta;}
 std::map<std::string,std::vector<ArtisanOrders::Line> >::iterator basket=next.baskets.find(from);if(basket!=next.baskets.end()){for(size_t j=0;j<basket->second.size();++j)if(!(roles&(basket->second[j].kind?2:1)))throw std::runtime_error("incompatible basket trade");found=found||!basket->second.empty();if(from!=to){next.baskets[to]=basket->second;next.baskets.erase(basket);}}
 if(!found)throw std::runtime_error("missing artisan recovery source");ArtisanOrders::load(ArtisanOrders::save(next));return next;
}
inline void tickArtisans(ArtisanOrders::Ledger& ledger,const std::vector<std::string>& heldKeys,double hour){if(!(hour>=0&&hour<=1e9))return;for(size_t i=0;i<ledger.orders.size();++i){ArtisanOrders::Order& o=ledger.orders[i];if(held(heldKeys,o.artisan))continue;if(o.status==ArtisanOrders::Waiting&&hour>=o.start)o.status=ArtisanOrders::Making;if(o.status==ArtisanOrders::Making&&hour>=o.end)o.status=ArtisanOrders::Ready;}}
}
