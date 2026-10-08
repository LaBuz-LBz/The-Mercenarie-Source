#include "ArtisanOrders.h"
#include "src/FinanceJournal.h"
#include <cassert>
#include <iostream>
using namespace ArtisanOrders;
Line make(const char* id,const char* quality,long long value,int n){Line l;l.item=id;l.quality=quality;l.base=value;l.quantity=l.remaining=n;l.unit=price(value);l.hours=series(value,n);return l;}
int main(){
 assert(unitHours(0)==6&&unitHours(1000)==6&&unitHours(1001)==12&&unitHours(3000)==12&&unitHours(3001)==15&&unitHours(6000)==15&&unitHours(6001)==20&&unitHours(10000)==20&&unitHours(10001)==24);
 assert(price(1000)==1250&&price(1)==2&&price(0)==0);
 assert(series(7000,1)==20&&series(7000,2)==35&&series(7000,10)==155);
 Ledger a;Line x=make("mod-item","grade-1",7000,10);assert(a.put("A",x,10));assert(!a.put("A",x,11));
 x.quality="grade-2";assert(a.put("A",x,10));assert(a.baskets["A"].size()==2);
 assert(a.confirm("A","Smith",100,1)==0&&a.orders.empty()&&a.baskets["A"].size()==2);
 unsigned long first=a.confirm("A","Smith",100,200000);assert(first&&a.orders[0].paid==175000&&a.orders[0].end==410);
 assert(a.confirm("A","Smith",100,200000)==0&&a.orders.size()==1);
 assert(a.put("A",make("mod-item","grade-1",1000,1),1));a.confirm("A","Smith",101,2000);
 assert(a.orders[1].start==410&&a.orders[1].end==416&&a.orders[1].status==Waiting);
 a.put("B",make("other","grade-1",0,1),1);a.confirm("B","Other",102,0);assert(a.orders[2].start==102);
 assert(!a.take(first,"A",0));a.tick(410);assert(a.orders[0].status==Ready&&a.orders[1].status==Making);
 assert(!a.take(first,"B",0));assert(a.take(first,"A",0));assert(a.orders[0].lines[0].remaining==9);
 a.put("B",make("saved basket","grade-1",100,1),1);const std::string bytes=save(a);Ledger restored=load(bytes);assert(save(restored)==bytes);
 restored.tick(416);assert(restored.orders[1].status==Ready&&restored.orders[0].lines[0].remaining==9);
 for(int i=0;i<9;++i)assert(restored.take(first,"A",0));assert(!restored.take(first,"A",0));
 for(int i=0;i<10;++i)assert(restored.take(first,"A",1));assert(restored.orders[0].status==Collected);
 assert(restored.dead("A"));assert(!restored.dead("A"));assert(restored.orders[0].status==Collected&&restored.orders[1].status==Lost&&restored.orders[2].status==Ready);
 assert(restored.baskets["B"].size()==1);assert(load("").orders.empty());
 bool rejected=false;try{load(bytes.substr(0,bytes.size()-3));}catch(...){rejected=true;}assert(rejected);
 rejected=false;try{load(bytes+"garbage");}catch(...){rejected=true;}assert(rejected);
 Ledger other;other.put("C",make("third","grade-1",42,1),1);restored=load(save(other));assert(restored.orders.empty()&&restored.baskets.count("A")==0);restored=load(bytes);assert(save(restored)==bytes);
 Ledger stress;double ends[2]={0,0};for(int i=0;i<1000;++i){const char* artisan=i%2?"A":"B";int n=1+i%10;Line l=make("fixture-modded","native-grade",i%11000,n);stress.put(artisan,l,n);double now=i/10.0;unsigned long id=stress.confirm(artisan,artisan,now,10000000);assert(id);double start=std::max(now,ends[i%2]);assert(stress.orders.back().start==start);ends[i%2]=start+l.hours;assert(stress.orders.back().end==ends[i%2]);}
 assert(save(load(save(stress)))==save(stress));stress.tick(1000000);for(size_t i=0;i<stress.orders.size();++i)assert(stress.orders[i].status==Ready);
 Ledger corrupt=a;corrupt.orders[1].start=corrupt.orders[0].start;corrupt.orders[1].end=corrupt.orders[1].start+duration(corrupt.orders[1].lines);rejected=false;try{load(save(corrupt));}catch(...){rejected=true;}assert(rejected);
 Finance::Journal live;live.baseline(10000,0);Finance::Journal pending=live;assert(pending.record(-1250,8750,1,Finance::Equipment,"artisan.payment","artisan:1"));assert(live.entries.empty());live.swap(pending);assert(live.entries.size()==1&&live.lastBalance==8750&&pending.entries.empty());assert(!live.observe(8750,1));assert(!live.record(-1250,7500,1,Finance::Equipment,"artisan.payment","artisan:1"));
 std::cout<<"Artisan orders: pricing, boundaries, series, quantity, FIFO, partial collection, death, corruption and A/B/A passed\n";
}
