#pragma once
#include "src/FiscalSystem.h"
namespace GuildOverview {
struct Finances {long long income,expense,days[3][7];bool available;Finances():income(0),expense(0),available(false){for(int s=0;s<3;++s)for(int d=0;d<7;++d)days[s][d]=0;}};
inline Finances finances(const Finance::Journal& journal,double now){
 Finance::Summary summary=Finance::summary(journal,now,7);Finances out;out.income=summary.totals.income;out.expense=summary.totals.expense;out.available=summary.available&&(out.income||out.expense);
 for(int d=0;d<7;++d){out.days[0][d]=summary.income[d];out.days[1][d]=-summary.expense[d];out.days[2][d]=summary.net[d];}return out;
}
}
