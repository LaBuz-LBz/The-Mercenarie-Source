#pragma once
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <stdexcept>

namespace GuildPayroll {
typedef long long Money;
inline int minimum(int){return 10;}
inline int defaultRate(int rank){const int v[]={50,60,80,110,140};return v[std::max(0,std::min(4,rank))];}
inline int rank(double strength,double toughness,double dex,double melee,double martial,double defence,double dodge){
    double c[]={strength,toughness,dex,std::max(melee,martial),std::max(defence,dodge)};
    double total=0,low=c[0];for(int i=0;i<5;++i){if(c[i]!=c[i])return 0;total+=c[i];low=std::min(low,c[i]);}
    double score=total/5;int r=score<25?0:score<40?1:score<60?2:score<80?3:4;
    if(r==4&&low<60)r=3;if(r>=3&&low<40)r=2;return r;
}
inline Money rounded(double n){return n<=0?0:static_cast<Money>(std::floor(n+.5));}
struct Segment {
    int rank,rate;double hours;
    Segment(int r=0,int v=0,double h=0):rank(r),rate(v),hours(h){}
    template<class A>void archive(A& a){a.field(rank);a.field(rate);a.field(hours);}
};
struct Member {
    std::string id,name;double joined,last,ended;int rank,rate;unsigned int observedLoss;
    bool robot,former;Money income,wages,compensation,contracts;std::vector<Segment> cycle;
    Member():joined(0),last(0),ended(-1),rank(0),rate(50),observedLoss(0),robot(false),former(false),income(0),wages(0),compensation(0),contracts(0){}
    double accrued()const{double n=0;for(size_t i=0;i<cycle.size();++i)n+=cycle[i].hours*cycle[i].rate/24;return n;}
    void add(double h){if(h<=0||former)return;if(!cycle.empty()&&cycle.back().rank==rank&&cycle.back().rate==rate)cycle.back().hours+=h;else cycle.push_back(Segment(rank,rate,h));}
    template<class A>void archive(A& a){a.field(id);a.field(name);a.field(joined);a.field(last);a.field(ended);a.field(rank);a.field(rate);a.field(observedLoss);a.field(robot);a.field(former);a.field(income);a.field(wages);a.field(compensation);a.field(contracts);list(a,cycle);}
    template<class A,class T>static void list(A& a,std::vector<T>& items){unsigned int n=(unsigned int)items.size();a.field(n);if(n>2000000)throw std::runtime_error("payroll collection too large");if(a.reading){if(n>(a.bytes.size()-a.cursor)/16)throw std::runtime_error("truncated payroll collection");items.resize(n);}for(unsigned int i=0;i<n;++i)items[i].archive(a);}
};
enum Category{Salary=0,Limb=1};
struct Repayment{double hour;Money amount;std::string source;Repayment(double h=0,Money v=0,const std::string& s=""):hour(h),amount(v),source(s){}template<class A>void archive(A& a){a.field(hour);a.field(amount);a.field(source);}};
struct Debt {
    Money id,initial,remaining;double hour;int category;std::string member,origin;std::vector<Repayment> repayments;
    Debt():id(0),initial(0),remaining(0),hour(0),category(0){}
    template<class A>void archive(A& a){a.field(id);a.field(initial);a.field(remaining);a.field(hour);a.field(category);a.field(member);a.field(origin);Member::list(a,repayments);}
};
struct PayLine {
    std::string member,name;int rank;bool robot;Money amount;std::vector<Segment> segments;
    PayLine():rank(0),robot(false),amount(0){}
    template<class A>void archive(A& a){a.field(member);a.field(name);a.field(rank);a.field(robot);a.field(amount);Member::list(a,segments);}
};
struct Bill {
    Money id,total;double start,end;bool answered;std::vector<PayLine> lines;
    Bill():id(0),total(0),start(0),end(0),answered(false){}
    template<class A>void archive(A& a){a.field(id);a.field(total);a.field(start);a.field(end);a.field(answered);Member::list(a,lines);}
};
struct Receipt {
    std::string id;Money gross,repaid,net,remaining;double hour;
    Receipt():gross(0),repaid(0),net(0),remaining(0),hour(0){}
    template<class A>void archive(A& a){a.field(id);a.field(gross);a.field(repaid);a.field(net);a.field(remaining);a.field(hour);}
};
struct State {
    bool enabled,initialized,warned;int rates[5];double start,next,now;Money sequence;
    std::map<std::string,Member> members;
    std::vector<Debt> debts;std::vector<Bill> bills;std::vector<Receipt> receipts;
    std::map<std::string,std::vector<std::string> > participants;
    std::set<std::string> completed;
    State():enabled(true),initialized(false),warned(false),start(0),next(0),now(0),sequence(0){for(int i=0;i<5;++i)rates[i]=defaultRate(i);}
    void init(double h){if(initialized)return;initialized=true;now=start=h;next=h+168;}
    Money debt(int category=-1,const std::string& member="")const{Money n=0;for(size_t i=0;i<debts.size();++i)if((category<0||debts[i].category==category)&&(member.empty()||debts[i].member==member))n+=debts[i].remaining;return n;}
    Money addDebt(const std::string& who,int category,Money value,double hour,const std::string& origin){if(value<=0)return 0;Debt d;d.id=++sequence;d.member=who;d.category=category;d.initial=d.remaining=value;d.hour=hour;d.origin=origin;debts.push_back(d);return d.id;}
    Money repay(Money budget,double h,const std::string& source,Money only=0){
        Money paid=0;while(budget>0){size_t best=debts.size();for(size_t i=0;i<debts.size();++i)if(debts[i].remaining>0&&(!only||debts[i].id==only)&&(best==debts.size()||debts[i].hour<debts[best].hour||(debts[i].hour==debts[best].hour&&debts[i].id<debts[best].id)))best=i;
            if(best==debts.size())break;Debt& d=debts[best];Money take=std::min(budget,d.remaining);d.remaining-=take;budget-=take;paid+=take;d.repayments.push_back(Repayment(h,take,source));
            std::map<std::string,Member>::iterator m=members.find(d.member);if(m!=members.end()){if(d.category==Salary)m->second.wages+=take;else m->second.compensation+=take;}
        }return paid;
    }
    void accrue(double h){for(std::map<std::string,Member>::iterator i=members.begin();i!=members.end();++i){Member& m=i->second;if(!m.former){m.add(std::max(0.0,h-m.last));m.last=h;}}}
    void advance(double h){init(h);if(h<now)return;if(!enabled){now=h;return;}
        while(next<=h){accrue(next);Bill b;b.id=++sequence;b.start=start;b.end=next;
            for(std::map<std::string,Member>::iterator i=members.begin();i!=members.end();++i){Member& m=i->second;if(m.former)continue;PayLine line;line.member=m.id;line.name=m.name;line.rank=m.rank;line.robot=m.robot;line.segments=m.cycle;line.amount=m.robot?0:rounded(m.accrued());b.total+=line.amount;b.lines.push_back(line);addDebt(m.id,Salary,line.amount,next,"payroll.salary");m.cycle.clear();}
            if(!b.lines.empty()){if(!b.total)b.answered=true;bills.push_back(b);}start=next;next+=168;warned=false;
        }accrue(h);now=h;
    }
    // First observation establishes a limb baseline: no invented pre-recruitment claims.
    std::vector<Money> observe(const std::string& id,const std::string& name,int rank,bool robot,bool dead,unsigned int loss,double h){
        advance(h);std::vector<Money> claims;std::map<std::string,Member>::iterator it=members.find(id);
        if(it==members.end()){if(dead)return claims;Member m;m.id=id;m.name=name;m.joined=m.last=h;m.rank=rank;m.robot=robot;m.rate=robot?0:rates[rank];m.observedLoss=loss;if(enabled)m.add(h-start);members[id]=m;return claims;}
        Member& m=it->second;m.name=name;if(m.former)return claims;
        unsigned int added=loss&~m.observedLoss;m.observedLoss|=loss;
        if(enabled)for(int limb=0;limb<4;++limb)if(added&(1<<limb))claims.push_back(addDebt(id,Limb,1000,h,limb==0?"payroll.left_arm":limb==1?"payroll.right_arm":limb==2?"payroll.left_leg":"payroll.right_leg"));
        m.rank=rank;m.robot=robot;m.rate=robot?0:rates[rank];
        if(dead){if(enabled)addDebt(id,Salary,m.robot?0:rounded(m.accrued()),h,"payroll.final_salary");m.cycle.clear();m.former=true;m.ended=h;}
        return claims;
    }
    void rate(int rank,int value,double h){advance(h);rates[rank]=std::max(minimum(rank),std::min(200,value));for(std::map<std::string,Member>::iterator i=members.begin();i!=members.end();++i)if(i->second.rank==rank&&!i->second.robot)i->second.rate=rates[rank];}
    void enable(bool value,double h){if(value==enabled)return;advance(h);if(!value)for(std::map<std::string,Member>::iterator i=members.begin();i!=members.end();++i)if(!i->second.former&&!i->second.robot)addDebt(i->first,Salary,rounded(i->second.accrued()),h,"payroll.suspended_balance");enabled=value;start=h;next=h+168;warned=false;for(std::map<std::string,Member>::iterator i=members.begin();i!=members.end();++i){i->second.cycle.clear();i->second.last=h;}for(size_t i=0;i<bills.size();++i)bills[i].answered=true;}
    Money estimate()const{Money n=0;for(std::map<std::string,Member>::const_iterator i=members.begin();i!=members.end();++i)if(!i->second.former&&!i->second.robot)n+=rounded(i->second.accrued()+std::max(0.0,next-now)*i->second.rate/24);return enabled?n:0;}
    bool alert(Money cash){if(!enabled||warned||next-now>24||cash>=estimate()+debt())return false;warned=true;return true;}
    Bill* pending(){if(!enabled)return 0;for(size_t i=0;i<bills.size();++i)if(!bills[i].answered)return &bills[i];return 0;}
    bool answer(Money id){for(size_t i=0;i<bills.size();++i)if(bills[i].id==id&&!bills[i].answered){bills[i].answered=true;return true;}return false;}
    const Receipt* receipt(const std::string& id)const{for(size_t i=0;i<receipts.size();++i)if(receipts[i].id==id)return &receipts[i];return 0;}
    Money withholding(Money gross)const{return enabled?std::min(std::max((Money)0,gross)/2,debt()):0;}
    void recordReward(const std::string& id,Money gross,Money paid,double h,const std::vector<std::string>& people){
        if(receipt(id))return;Receipt r;r.id=id;r.gross=gross;r.repaid=repay(paid,h,id);r.net=gross-r.repaid;r.remaining=debt();r.hour=h;receipts.push_back(r);
        std::set<std::string> unique;for(size_t i=0;i<people.size();++i)if(members.count(people[i]))unique.insert(people[i]);if(unique.empty())return;
        Money share=gross/unique.size(),remainder=gross%unique.size();for(std::set<std::string>::iterator i=unique.begin();i!=unique.end();++i){Member& m=members[*i];m.income+=share+(remainder-->0?1:0);}
    }
    void complete(const std::string& group){if(completed.count(group))return;completed.insert(group);std::map<std::string,std::vector<std::string> >::const_iterator p=participants.find(group);if(p==participants.end())return;std::set<std::string> unique(p->second.begin(),p->second.end());for(std::set<std::string>::const_iterator i=unique.begin();i!=unique.end();++i)if(members.count(*i))++members[*i].contracts;}
    template<class A>void archive(A& a){
        int version=1;a.field(version);if(version!=1)throw std::runtime_error("unsupported payroll");a.field(enabled);a.field(initialized);a.field(warned);for(int i=0;i<5;++i)a.field(rates[i]);a.field(start);a.field(next);a.field(now);a.field(sequence);
        unsigned int count=(unsigned int)members.size();a.field(count);if(count>200000)throw std::runtime_error("oversized payroll roster");
        if(a.reading){if(count>(a.bytes.size()-a.cursor)/64)throw std::runtime_error("truncated payroll roster");members.clear();for(unsigned int i=0;i<count;++i){Member m;m.archive(a);if(m.id.empty()||!members.insert(std::make_pair(m.id,m)).second)throw std::runtime_error("duplicate payroll member");}}else for(std::map<std::string,Member>::iterator i=members.begin();i!=members.end();++i)i->second.archive(a);
        Member::list(a,debts);Member::list(a,bills);Member::list(a,receipts);
        count=(unsigned int)participants.size();a.field(count);if(count>200000)throw std::runtime_error("oversized payroll missions");
        if(a.reading){if(count>(a.bytes.size()-a.cursor)/8)throw std::runtime_error("truncated payroll participants");participants.clear();for(unsigned int i=0;i<count;++i){std::string id;std::vector<std::string> p;a.field(id);a.field(p);if(id.empty()||!participants.insert(std::make_pair(id,p)).second)throw std::runtime_error("duplicate payroll participants");}}else for(std::map<std::string,std::vector<std::string> >::iterator i=participants.begin();i!=participants.end();++i){std::string id=i->first;a.field(id);a.field(i->second);}
        count=(unsigned int)completed.size();a.field(count);if(count>2000000)throw std::runtime_error("oversized payroll completion history");if(a.reading){if(count>(a.bytes.size()-a.cursor)/4)throw std::runtime_error("truncated payroll history");completed.clear();for(unsigned int i=0;i<count;++i){std::string id;a.field(id);if(id.empty()||!completed.insert(id).second)throw std::runtime_error("duplicate payroll completion");}}else for(std::set<std::string>::const_iterator i=completed.begin();i!=completed.end();++i){std::string id=*i;a.field(id);}
        {for(int i=0;i<5;++i)if(rates[i]<minimum(i)||rates[i]>200)throw std::runtime_error("invalid payroll rate");if(next<start||now!=now||now<0)throw std::runtime_error("invalid payroll clock");for(size_t i=0;i<debts.size();++i)if(debts[i].remaining<0||debts[i].remaining>debts[i].initial||debts[i].category<0||debts[i].category>1)throw std::runtime_error("invalid payroll debt");}
    }
};
}
