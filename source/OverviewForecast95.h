#pragma once
// Read-only forecast. Amounts due already are kept separate from the next cycle.
namespace OverviewForecast95 {
struct Result {long long salary,rent,tax,overdue,total;double next;int kind;bool unknownTax;Result():salary(0),rent(0),tax(0),overdue(0),total(0),next(-1),kind(-1),unknownTax(false){}void due(double h,int k,double now){h=std::max(now,h);if(next<0||h<next){next=h;kind=k;}}};
inline Result calculate(const GuildPayroll::State& pay,const RealEstate::State& estate,const FiscalLedger& fiscal,double now){
 Result r;const double end=now+168;
 r.overdue=pay.debt()+estate.debt();if(r.overdue>0)r.due(now,3,now);
 if(pay.enabled&&pay.initialized&&pay.next<=end){
  double first=pay.next;long long cycles=(long long)std::floor((end-first)/168)+1;
  for(std::map<std::string,GuildPayroll::Member>::const_iterator i=pay.members.begin();i!=pay.members.end();++i){const GuildPayroll::Member& m=i->second;if(m.robot||m.former)continue;r.salary+=GuildPayroll::rounded(m.accrued()+std::max(0.0,first-m.last)*m.rate/24)+(cycles-1)*GuildPayroll::rounded(7.0*m.rate);}
  if(r.salary>0)r.due(first,0,now);
 }
 for(std::map<RealEstate::Money,RealEstate::Lease>::const_iterator i=estate.leases.begin();i!=estate.leases.end();++i){const RealEstate::Lease& l=i->second;if(l.status!=RealEstate::Active||l.suspended||l.next>end)continue;long long n=(long long)std::floor((end-l.next)/168)+1;r.rent+=n*l.rent;if(l.rent>0)r.due(l.next,1,now);}
 for(int i=0;i<2;++i){const FiscalOrganisationState& f=fiscal.organisations[i];if(f.rebel||f.debt<=0)continue;double h=f.deadlineHour>0?f.deadlineHour:f.nextCollectionHour;if(h<=0){r.unknownTax=true;continue;}if(h<=end){r.tax+=f.debt;r.due(h,2,now);}}
 r.total=r.salary+r.rent+r.tax+r.overdue;return r;
}
}
