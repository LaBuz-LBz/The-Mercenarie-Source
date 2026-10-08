#pragma once
#include "FinanceJournal.h"
namespace Finance {
inline bool inPeriod(double at,double now,int period){long d=Journal::day(at),today=Journal::day(now);return d<=today&&(period==0||d>today-period);}
struct DisplayRow {Entry entry;unsigned int movements;DisplayRow(const Entry& e):entry(e),movements(1){}};
inline std::vector<DisplayRow> recent(const Journal& j,double now,int period){
 std::vector<DisplayRow> rows;double newest=0,previous=0;
 for(size_t n=j.entries.size();n>0;--n){const Entry& e=j.entries[n-1];if(!inPeriod(e.hour,now,period))continue;
 bool unknown=e.category==Other&&e.description=="finance.unidentified";
 bool merge=!rows.empty()&&unknown&&rows.back().entry.description=="finance.unidentified"&&rows.back().entry.category==Other&&((e.amount>0)==(rows.back().entry.amount>0))&&Journal::day(e.hour)==Journal::day(newest)&&e.hour<=previous&&newest-e.hour<=1.0;
 if(merge){rows.back().entry.amount+=e.amount;++rows.back().movements;}else{rows.push_back(DisplayRow(e));newest=e.hour;}previous=e.hour;
 }return rows;
}
inline std::vector<Entry> displayEntries(const Journal& j,double now,int period){
 std::vector<Entry> rows;for(size_t i=0;i<j.entries.size();++i)if(inPeriod(j.entries[i].hour,now,period))rows.push_back(j.entries[i]);
 // Retention removes detail, not daily aggregates. Explicit archive rows
 // reconcile the journal with totals without inventing an individual payment.
 for(size_t d=0;d<j.days.size();++d){const Day& day=j.days[d];if(!inPeriod(day.index*24.0,now,period))continue;
  long long in[Count]={0},out[Count]={0};for(size_t i=0;i<j.entries.size();++i)if(Journal::day(j.entries[i].hour)==day.index){const Entry& e=j.entries[i];if(e.amount>0)in[e.category]+=e.amount;else out[e.category]-=e.amount;}
  for(int c=0;c<Count;++c)for(int sign=0;sign<2;++sign){long long amount=sign?day.out[c]-out[c]:day.in[c]-in[c];if(amount<=0)continue;Entry e;e.hour=day.index*24.0;e.category=c;e.amount=sign?-amount:amount;e.balance=-1;e.description="finance.archived";rows.push_back(e);}
 }
 return rows;
}
struct NewestFirst {bool operator()(const Entry& a,const Entry& b)const{return a.hour!=b.hour?a.hour>b.hour:a.id>b.id;}};
}
