#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "../MissionArchive.h"

// Pure ledger. Engine objects, UI lifetime and fiscal organisations never own it.
namespace RealEstate {
typedef long long Money;
const double Week=168.0;
enum Status { Active, Terminated, Destroyed, Purchased };
inline bool hour(double h){return h==h&&h>=0&&h<=1e9;}
template<class T> inline std::runtime_error invalid(const char* field,T value,Money lease=0){std::ostringstream s;s<<"invalid estate "<<field<<" value="<<std::setprecision(17)<<value<<" lease="<<lease;return std::runtime_error(s.str());}
inline std::runtime_error invalidClock(double now,double observed,bool available){std::ostringstream s;s<<"estate clock unavailable now="<<std::setprecision(17)<<now<<" observedHour="<<observed<<" available="<<available;return std::runtime_error(s.str());}
inline Money add(Money a,Money b){if(a<0||b<0||a>(std::numeric_limits<Money>::max)()-b)throw std::runtime_error("estate amount overflow");return a+b;}
inline Money multiply(Money a,Money b){if(a<0||b<0||(b&&a>(std::numeric_limits<Money>::max)()/b))throw std::runtime_error("estate amount overflow");return a*b;}
inline bool prices(Money vanilla,Money& rent,Money& buy){if(vanilla<=0||vanilla>214748364LL)return false;rent=(vanilla+9)/10;buy=vanilla*10;return true;}
struct Identity {
    std::string uid,layout,town,building,owner,name,city,ownerName;
    double x,y,z;
    Identity():x(0),y(0),z(0){}
    bool valid()const{return !uid.empty()&&!town.empty()&&!building.empty()&&!owner.empty()&&x==x&&y==y&&z==z&&std::fabs(x)<1e7&&std::fabs(y)<1e7&&std::fabs(z)<1e7;}
    bool matches(const Identity& b)const{return uid==b.uid&&layout==b.layout&&town==b.town&&building==b.building&&std::fabs(x-b.x)<0.1&&std::fabs(y-b.y)<0.1&&std::fabs(z-b.z)<0.1;}
    template<class A>void archive(A& a){a.field(uid);a.field(layout);a.field(town);a.field(building);a.field(owner);a.field(name);a.field(city);a.field(ownerName);a.field(x);a.field(y);a.field(z);}
};
// An unpaid contiguous range avoids millions of objects after a large time jump.
struct Due {Money first,count;Due(Money f=0,Money n=0):first(f),count(n){} };
struct Lease {
    Money id,base,rent,buy,sequence;
    Identity identity;
    Status status;
    double next,remaining;
    bool suspended,initialRuin;
    std::vector<Due> due;
    Lease():id(0),base(0),rent(0),buy(0),sequence(1),status(Active),next(0),remaining(0),suspended(false),initialRuin(false){}
    Money debt()const{Money n=0;for(size_t i=0;i<due.size();++i)n=add(n,multiply(due[i].count,rent));return n;}
    Money oldest()const{return due.empty()?0:due.front().first;}
    void unpaid(Money first,Money count){if(!count)return;if(!due.empty()&&due.back().first+due.back().count==first)due.back().count=add(due.back().count,count);else due.push_back(Due(first,count));}
    template<class A>void archive(A& a){
        a.field(id);identity.archive(a);a.field(base);a.field(rent);a.field(buy);a.field(sequence);a.field(status);a.field(next);a.field(remaining);a.field(suspended);a.field(initialRuin);
        unsigned int n=(unsigned int)due.size();a.field(n);if(n>MissionArchive::MaximumBytes/16)throw std::runtime_error("estate debt list too large");if(a.reading)due.resize(n);
        Money previous=0;for(unsigned int i=0;i<n;++i){a.field(due[i].first);a.field(due[i].count);if(due[i].first<1||due[i].first<=previous||due[i].count<=0)throw std::runtime_error("estate debt range invalid");previous=add(due[i].first,due[i].count)-1;if(previous>=sequence)throw std::runtime_error("estate future debt");}
        Money r=0,b=0;
        if(id<1)throw invalid("id",id,id);
        if(!identity.valid())throw invalid("identity (uid/town/building/owner/coordinates)",identity.uid,id);
        if(!prices(base,r,b))throw invalid("base",base,id);
        if(r!=rent)throw invalid("rent",rent,id);
        if(b!=buy)throw invalid("buy",buy,id);
        if(sequence<1)throw invalid("sequence",sequence,id);
        if(status<Active||status>Purchased)throw invalid("status",(int)status,id);
        if(!hour(next))throw invalid("next",next,id);
        if(remaining!=remaining||std::fabs(remaining)>1e9)throw invalid("remaining",remaining,id);
        if(status==Purchased&&!due.empty())throw invalid("purchased lease with debt count",due.size(),id);
        debt();
    }
};
struct Payment {Money lease,sequence,count,amount;Payment(Money l,Money s,Money c,Money a):lease(l),sequence(s),count(c),amount(a){} };
struct State {
    std::map<Money,Lease> leases;
    Money nextId;
    double observedHour;
    bool available;
    unsigned long revision;
    State():nextId(1),observedHour(0),available(true),revision(1){}
    void swap(State& b){leases.swap(b.leases);std::swap(nextId,b.nextId);std::swap(observedHour,b.observedHour);std::swap(available,b.available);std::swap(revision,b.revision);}
    Money debt()const{Money n=0;for(std::map<Money,Lease>::const_iterator i=leases.begin();i!=leases.end();++i)n=add(n,i->second.debt());return n;}
    bool canBuy()const{return available&&debt()==0;}
    Money activeFor(const Identity& b)const{for(std::map<Money,Lease>::const_iterator i=leases.begin();i!=leases.end();++i)if(i->second.status==Active&&i->second.identity.matches(b))return i->first;return 0;}
    Money sign(const Identity& b,Money vanilla,double now,Money funds,bool ruin=false){
        Money r=0,p=0;if(!available||!b.valid()||!hour(now)||!prices(vanilla,r,p)||funds<r||activeFor(b))return 0;
        Lease l;l.id=nextId;l.identity=b;l.base=vanilla;l.rent=r;l.buy=p;l.next=now+Week;l.initialRuin=ruin;Money id=nextId;nextId=add(nextId,1);leases[id]=l;observedHour=now;++revision;return id;
    }
    std::vector<Payment> advance(double now,Money funds){
        if(!available||!hour(now))throw invalidClock(now,observedHour,available);
        // Native quick.save stores day/hour/minute, while our sidecar retains
        // fractional seconds. Loading can therefore rewind less than one minute.
        // Keep the last observed instant until native time catches up: do not
        // move lease deadlines or charge the same interval again.
        if(now<observedHour){
            if(observedHour-now>1.0/60.0+1e-9)throw invalidClock(now,observedHour,available);
            now=observedHour;
        }
        std::vector<Payment> due;Money total=0;
        for(std::map<Money,Lease>::const_iterator i=leases.begin();i!=leases.end();++i){const Lease& l=i->second;if(l.status!=Active||l.suspended||l.next>now)continue;Money count=(Money)std::floor((now-l.next)/Week)+1;Money amount=multiply(count,l.rent);total=add(total,amount);due.push_back(Payment(l.id,l.sequence,count,amount));}
        const bool autoPay=total<=funds;for(size_t i=0;i<due.size();++i){Lease& l=leases[due[i].lease];l.sequence=add(l.sequence,due[i].count);l.next+=due[i].count*Week;if(!autoPay)l.unpaid(due[i].sequence,due[i].count);}
        observedHour=now;if(!due.empty())++revision;if(!autoPay)due.clear();return due;
    }
    bool pay(Money id,Money expectedSequence,Money funds){
        std::map<Money,Lease>::iterator i=leases.find(id);if(!available||i==leases.end())return false;Lease& l=i->second;
        if(l.suspended||l.due.empty()||l.oldest()!=expectedSequence||funds<l.rent)return false;
        ++l.due.front().first;if(--l.due.front().count==0)l.due.erase(l.due.begin());++revision;return true;
    }
    bool close(Money id,Status reason){std::map<Money,Lease>::iterator i=leases.find(id);if(!available||i==leases.end()||i->second.status!=Active||i->second.suspended||reason==Active||(reason==Purchased&&!canBuy()))return false;i->second.status=reason;++revision;return true;}
    void suspendAll(double now){if(!hour(now))throw std::runtime_error("estate import clock");for(std::map<Money,Lease>::iterator i=leases.begin();i!=leases.end();++i){Lease& l=i->second;if(l.status==Active&&!l.suspended){l.remaining=l.next-observedHour;l.suspended=true;}}observedHour=now;++revision;}
    bool resume(Money id,double now){std::map<Money,Lease>::iterator i=leases.find(id);if(!available||!hour(now)||i==leases.end()||!i->second.suspended)return false;i->second.next=std::max(0.0,now+i->second.remaining);i->second.suspended=false;++revision;return true;}
    Money forecast(double now)const{Money total=0;for(std::map<Money,Lease>::const_iterator i=leases.begin();i!=leases.end();++i){const Lease& l=i->second;if(l.status!=Active||l.suspended||l.next>now+Week)continue;double first=std::max(now,l.next);Money count=(Money)std::floor((now+Week-first)/Week)+1;total=add(total,multiply(l.rent,count));}return total;}
};
// Closed, debt-free records are history; ownership is already native.
// An unavailable ledger cannot prove that a transition is safe.
inline bool requiresSystem(const State& s){
    if(!s.available)return true;
    for(std::map<Money,Lease>::const_iterator i=s.leases.begin();i!=s.leases.end();++i)
        if(i->second.status==Active||!i->second.due.empty())return true;
    return false;
}
inline std::string save(const State& state){
    if(!state.available)throw std::runtime_error("estate unavailable");State s=state;
    // No lease uses the clock: an unset sentinel is not corrupt financial data.
    // Canonicalize only this unused clock in the snapshot, never clear the ledger.
    if(s.leases.empty()&&!hour(s.observedHour))s.observedHour=0;
    if(s.nextId<1)throw invalid("nextId",s.nextId);
    if(!hour(s.observedHour))throw invalid("observedHour",s.observedHour);
    MissionArchive a;std::string magic="MERCENARIE-ESTATE-1";a.field(magic);a.field(s.nextId);a.field(s.observedHour);
    unsigned int n=(unsigned int)s.leases.size();a.field(n);for(std::map<Money,Lease>::iterator i=s.leases.begin();i!=s.leases.end();++i)i->second.archive(a);
    if(a.bytes.size()>MissionArchive::MaximumBytes)throw std::runtime_error("estate snapshot too large");return sealMissionArchive(a.bytes);
}
inline State load(const std::string& bytes){
    State s;if(bytes.empty())return s;MissionArchive a(openMissionArchive(bytes));std::string magic;a.field(magic);if(magic!="MERCENARIE-ESTATE-1")throw std::runtime_error("estate schema");a.field(s.nextId);a.field(s.observedHour);unsigned int n=0;a.field(n);
    if(n>MissionArchive::MaximumBytes/100||s.nextId<1||!hour(s.observedHour))throw std::runtime_error("estate invalid header");
    for(unsigned int i=0;i<n;++i){Lease l;l.archive(a);if(l.id>=s.nextId||s.leases.count(l.id)||(l.status==Active&&s.activeFor(l.identity)))throw std::runtime_error("estate duplicate lease");s.leases[l.id]=l;}a.finish();s.debt();return s;
}
}
