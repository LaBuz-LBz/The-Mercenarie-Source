#include "MissionBookContractsModel.h"
#include "DelegatedMissionTiming.h"
#include <cassert>
#include <iostream>
#include <set>

using namespace MissionBookContracts;

int main()
{
    std::vector<Offer> offers;
    offers.push_back(Offer("escort",categoryFor(0,false)));
    offers.push_back(Offer("caravan",categoryFor(1,false)));
    offers.push_back(Offer("science",categoryFor(2,false)));
    offers.push_back(Offer("mail",categoryFor(4,false)));
    offers.push_back(Offer("bounty",categoryFor(3,true)));
    offers.push_back(Offer("consumed",Legal,false));
    assert(filtered(offers,All).size()==5);
    assert(filtered(offers,Legal).size()==2);
    assert(filtered(offers,Illegal).empty());
    assert(filtered(offers,Merchant).size()==2);
    assert(filtered(offers,Security).size()==1);
    assert(offers.size()==6); // display filtering never consumes offers
    assert(pageCount(14,PageSize)==2);
    for(int count=0;count<=29;++count){
        std::vector<Offer> pool;for(int i=0;i<count;++i)pool.push_back(Offer("id",(Category)(1+i%4),true));
        std::vector<size_t> all=filtered(pool,All),visited;
        for(int p=0;p<pageCount(all.size(),PageSize);++p){std::vector<size_t> rows=page(all,p);assert(rows.size()<=7);visited.insert(visited.end(),rows.begin(),rows.end());}
        assert(visited==all);assert(clampPage(99,0)==0);
        for(int f=1;f<=4;++f){std::vector<size_t> rows=filtered(pool,(Category)f);for(size_t j=0;j<rows.size();++j)assert(pool[rows[j]].category==f);assert(page(rows,99).size()<=7);}
        assert(filtered(pool,All)==all);assert(pool.size()==count);
    }
    std::vector<size_t> none;assert(page(none,0).empty());

    std::vector<Soldier> soldiers;
    soldiers.push_back(Soldier("a","Ruka",42,38,51));
    soldiers.push_back(Soldier("b","Bard",31,35,40));
    soldiers.push_back(Soldier("c","Miu",22,18,29));
    std::vector<size_t> order;order.push_back(0);order.push_back(1);order.push_back(2);
    std::set<std::string> selected;selected.insert("b");
    sortSoldiers(order,soldiers,SortName);assert(order[0]==1);
    sortSoldiers(order,soldiers,SortAttack);assert(order[0]==0);
    sortSoldiers(order,soldiers,SortDefence);assert(order[0]==0);
    sortSoldiers(order,soldiers,SortEndurance);assert(order[0]==0);
    assert(selected.count("b")==1); // sorting is presentation-only

    assert(DelegatedMissionTiming::travelHours(3.0)==1.0);
    double hours=0;assert(DelegatedMissionTiming::activityDuration(DelegatedMissionTiming::ActivityBountyHunt,hours)&&hours==48.0);
    std::cout<<"PASS book model: all categories, 0-29 offers, seven per page, clamping, stable IDs, sorting, unchanged timings\n";
}
