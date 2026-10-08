#include "GuildContractsModel.h"
#include "GuildContractHistory.h"
#include <cassert>
#include <iostream>
int main(){using namespace GuildContracts;std::vector<Row> rows(5);
 for(int i=0;i<5;++i){rows[i].status=Completed;rows[i].rewardValue=i==4?-1:(i==3?10:30-i*10);rows[i].danger=i==4?0:i+1;rows[i].durationHours=i==4?-1:6-i;}
 std::vector<int> order=filter(rows,-1,"",2);assert(order[0]==2&&order[1]==3&&order[4]==4);
 order=filter(rows,-1,"",3);assert(order[0]==0&&order[4]==4);
 order=filter(rows,-1,"",4);assert(order[0]==0&&order[4]==4);
 order=filter(rows,-1,"",5);assert(order[0]==3&&order[4]==4);
 for(int n=0;n<=31;++n)for(int page=-3;page<40;++page){int p=clampPage(page,n,10);assert(p>=0&&p<pages(n,10));}
 GuildHistory::Summary no=GuildHistory::parse("Heng : REUSSI - sans prime");assert(no.status==Completed&&no.bonus==0);
 GuildHistory::Summary yes=GuildHistory::parse("Heng : SUCCESS - WITH BONUS");assert(yes.status==Completed&&yes.bonus==1);
 assert(GuildHistory::parse("Heng : REUSSI - primes demandees").bonus==2);
 assert(GuildHistory::parse("COURRIER | Heng | 0 Cats").reward==0);assert(GuildHistory::parse("GUILDE | Heng | 1 200 Cats").reward==1200);assert(GuildHistory::parse("Heng : SUCCESS").reward==-1);
 std::cout<<"PASS: numeric sorts, missing values last, stable ties, page bounds, successful missions with/without bonus\n";
}
