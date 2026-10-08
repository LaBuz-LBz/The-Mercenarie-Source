#pragma once
#include "GuildPayroll.h"
#include <sstream>
namespace PayrollPresentation {
struct Rank {int count;GuildPayroll::Money total;std::set<int> rates;bool changed;Rank():count(0),total(0),changed(false){}};
struct Summary {Rank ranks[5];int count;GuildPayroll::Money total;Summary():count(0),total(0){}};
// Group by rank at billing. Sum the engine's already rounded individual amounts,
// including every earlier rank/rate segment. Never recalculate current rate * 7.
inline Summary summarize(const GuildPayroll::Bill& bill){
 Summary s;s.total=bill.total;
 for(size_t i=0;i<bill.lines.size();++i){const GuildPayroll::PayLine& line=bill.lines[i];Rank& r=s.ranks[std::max(0,std::min(4,line.rank))];++r.count;++s.count;r.total+=line.amount;
  if(line.robot)r.rates.insert(0);
  else for(size_t j=0;j<line.segments.size();++j){r.rates.insert(line.segments[j].rate);if(line.segments[j].rank!=line.rank)r.changed=true;}
 }return s;
}
inline long long minutes(double hours){return hours<=0?0:(long long)std::floor(hours*60+.5);}
}
