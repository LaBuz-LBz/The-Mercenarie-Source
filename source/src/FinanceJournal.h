#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace Finance {
enum Category { Contract, Purchase, Sale, Wages, Office, Maintenance, Equipment, Travel, Tax, Investment, Other, Rent, Count };
enum { MaxEntries=4096, RetainedDays=366 };
struct Entry {
 unsigned long id; double hour; long long amount,balance; int category; std::string description,event;
 Entry():id(0),hour(0),amount(0),balance(0),category(Other){}
};
struct Day {
 long index; long long in[Count],out[Count];
 Day(long n=0):index(n){for(int i=0;i<Count;++i)in[i]=out[i]=0;}
};
struct Totals {
 long long income,expense,in[Count],out[Count];
 Totals():income(0),expense(0){for(int i=0;i<Count;++i)in[i]=out[i]=0;}
};
struct Journal {
 std::vector<Entry> entries; std::vector<Day> days;
 unsigned long nextId,revision; double startedHour; bool started,truncated,available;
 // Observation baseline is deliberately NOT restored across world loads.
 bool observed; long long lastBalance;bool legacyInvestmentsImported;
 Journal():nextId(1),revision(1),startedHour(0),started(false),truncated(false),available(true),observed(false),lastBalance(0),legacyInvestmentsImported(false){}
 void swap(Journal& other){
  entries.swap(other.entries);days.swap(other.days);
  std::swap(nextId,other.nextId);std::swap(revision,other.revision);std::swap(startedHour,other.startedHour);
  std::swap(started,other.started);std::swap(truncated,other.truncated);std::swap(available,other.available);
  std::swap(observed,other.observed);std::swap(lastBalance,other.lastBalance);std::swap(legacyInvestmentsImported,other.legacyInvestmentsImported);
 }
 static long day(double h){return (long)std::floor(h/24.0);}
 void baseline(long long balance,double hour){lastBalance=balance;observed=true;if(!started){started=true;startedHour=hour;++revision;}}
 bool record(long long amount,long long balance,double hour,int category,const std::string& description,const std::string& event="") {
  if(!available||!amount||hour<0||hour>1e9||category<0||category>=Count)return false;
  if(!event.empty())for(size_t i=0;i<entries.size();++i)if(entries[i].event==event)return false;
  if(!started){started=true;startedHour=hour;}
  Entry e;e.id=nextId++;e.hour=hour;e.amount=amount;e.balance=balance;e.category=category;e.description=description;e.event=event;entries.push_back(e);
  if(entries.size()>MaxEntries){entries.erase(entries.begin());truncated=true;}
  long today=day(hour),latest=today;for(size_t i=0;i<days.size();++i)latest=std::max(latest,days[i].index);
  if(today>=latest-RetainedDays+1){size_t n=0;for(;n<days.size();++n)if(days[n].index==today)break;
   if(n==days.size())days.push_back(Day(today));
   if(amount>0)days[n].in[category]+=amount;else days[n].out[category]-=amount;
  }
  for(size_t i=0;i<days.size();)if(days[i].index<latest-RetainedDays+1)days.erase(days.begin()+i);else ++i;
  lastBalance=balance;observed=true;++revision;return true;
 }
 bool observe(long long balance,double hour) {
  if(!available)return false;
  if(!observed){baseline(balance,hour);return false;}
  return record(balance-lastBalance,balance,hour,Other,"finance.unidentified");
 }
 Totals totals(double hour,int period)const {
  Totals t;long today=day(hour);for(size_t d=0;d<days.size();++d)if(days[d].index<=today&&days[d].index>today-period)
   for(int c=0;c<Count;++c){t.in[c]+=days[d].in[c];t.out[c]+=days[d].out[c];t.income+=days[d].in[c];t.expense+=days[d].out[c];}
  return t;
 }
};
// One calendar-day window for all financial screens, including their charts.
struct Summary {
 Totals totals;long long income[90],expense[90],net[90];int period;bool available;
 Summary():period(0),available(false){for(int i=0;i<90;++i)income[i]=expense[i]=net[i]=0;}
};
inline Summary summary(const Journal& journal,double hour,int period){
 Summary s;s.period=std::max(1,std::min(90,period));s.available=journal.available;
 s.totals=journal.totals(hour,s.period);long first=Journal::day(hour)-s.period+1;
 for(size_t i=0;i<journal.days.size();++i){long n=journal.days[i].index-first;if(n<0||n>=s.period)continue;for(int c=0;c<Count;++c){s.income[n]+=journal.days[i].in[c];s.expense[n]+=journal.days[i].out[c];}}
 long long running=0;for(int i=0;i<s.period;++i){running+=s.income[i]-s.expense[i];s.net[i]=running;}return s;
}
// Future providers must explicitly declare availability. Missing components
// are never interpreted as zero-cost wages or guaranteed recurring revenue.
struct ForecastComponent {bool available;long long amount;double dueHour;ForecastComponent():available(false),amount(0),dueHour(0){}};
struct Forecast {ForecastComponent missions,offices,other,wages,maintenance;};
inline bool matches(const Entry& e,int filter){return filter==0||(filter==1&&e.amount>0)||(filter==2&&e.amount<0)||(filter>=3&&e.category==filter-3);}
inline std::string clean(std::string s){if(s.size()>240)s.resize(240);for(size_t i=0;i<s.size();++i)if(s[i]=='|'||s[i]=='\n'||s[i]=='\r')s[i]=' ';return s;}
inline void write(std::ostream& out,const Journal& j){
 std::streamsize precision=out.precision();out<<std::setprecision(17);
 if(j.legacyInvestmentsImported)out<<"@cashmigration|1\n";
 out<<"@finance|1|"<<j.nextId<<'|'<<j.started<<'|'<<j.startedHour<<'|'<<j.truncated<<'|'<<j.available<<'\n';
 for(size_t i=0;i<j.entries.size();++i){const Entry& e=j.entries[i];out<<"@cash|"<<e.id<<'|'<<e.hour<<'|'<<e.amount<<'|'<<e.balance<<'|'<<e.category<<'|'<<clean(e.description)<<'|'<<clean(e.event)<<'\n';}
 for(size_t d=0;d<j.days.size();++d){out<<"@cashday|"<<j.days[d].index;for(int c=0;c<Count;++c)out<<'|'<<j.days[d].in[c]<<'|'<<j.days[d].out[c];out<<'\n';}
 out.precision(precision);
}
inline bool number(const std::string& s,long long& n){std::istringstream in(s);in>>n;return !in.fail()&&in.eof();}
inline void readLine(const std::string& line,Journal& j){
 std::istringstream in(line);std::vector<std::string> f;std::string s;while(std::getline(in,s,'|'))f.push_back(s);
 if(f.empty())return;
 if(f[0]=="@cashmigration"){if(f.size()==2&&f[1]=="1")j.legacyInvestmentsImported=true;else j.available=false;return;}
 if(f[0]=="@finance"){
  if(f.size()!=7||f[1]!="1"){j.available=false;return;}
  long long id=0,start=0,trunc=0,avail=0;double hour=0;std::istringstream h(f[4]);h>>hour;
  if(!number(f[2],id)||id<1||id>0x7fffffff||!number(f[3],start)||h.fail()||!h.eof()||hour<0||hour>1e9||!number(f[5],trunc)||!number(f[6],avail)){j.available=false;return;}
  j.nextId=(unsigned long)id;j.started=start!=0;j.startedHour=hour;j.truncated=trunc!=0;j.available=j.available&&avail!=0;
 }else if(f[0]=="@cash"){
  if(f.size()<7||f.size()>8||j.entries.size()>=MaxEntries){j.available=false;return;}
  Entry e;long long id=0,c=0;std::istringstream h(f[2]);h>>e.hour;
  if(!number(f[1],id)||id<1||id>=j.nextId||h.fail()||!h.eof()||e.hour<0||e.hour>1e9||!number(f[3],e.amount)||e.amount==0||e.amount>2147483647LL||e.amount<-2147483647LL||!number(f[4],e.balance)||e.balance<-1||e.balance>2147483647LL||!number(f[5],c)||c<0||c>=Count||f[6].size()>240){j.available=false;return;}
  if(!j.entries.empty()&&j.entries.back().id>=(unsigned long)id){j.available=false;return;}
  e.id=(unsigned long)id;e.category=(int)c;e.description=f[6];if(f.size()==8)e.event=f[7];j.entries.push_back(e);
 }else if(f[0]=="@cashday"){
  if((f.size()!=2+2*Count&&f.size()!=2+2*Rent)||j.days.size()>=RetainedDays){j.available=false;return;}
  long long index=0;if(!number(f[1],index)||index<0||index>41666667){j.available=false;return;}Day d((long)index);
  for(size_t i=0;i<j.days.size();++i)if(j.days[i].index==index){j.available=false;return;}
  for(int c=0;c<(int)(f.size()-2)/2;++c)if(!number(f[2+c*2],d.in[c])||!number(f[3+c*2],d.out[c])||d.in[c]<0||d.out[c]<0||d.in[c]>1000000000000LL||d.out[c]>1000000000000LL){j.available=false;return;}
  j.days.push_back(d);
 }
}
}
