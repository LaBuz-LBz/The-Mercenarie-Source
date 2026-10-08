#include "ArtisanClock.h"
#include <cassert>
#include <iostream>
using namespace ArtisanOrders;
Line item(int quantity){Line x;x.item="native-equipment";x.quality="armour:20";x.base=7000;x.unit=price(x.base);x.quantity=x.remaining=quantity;x.hours=series(x.base,quantity);return x;}
int main(){
 // A non-null GameWorld pointer during startup does NOT mean its clock exists.
 // GUI observation is allowed only after an Artisan screen in a loaded save.
 assert(!ArtisanClock::canObserveUi(false,false,false,false)); // Main menu.
 assert(!ArtisanClock::canObserveUi(false,true,false,false)); // No Artisan context.
 assert(!ArtisanClock::canObserveUi(true,false,false,false)); // Old screen, no save.
 assert(!ArtisanClock::canObserveUi(true,true,true,false)); // World unloading.
 assert(!ArtisanClock::canObserveUi(true,true,false,true)); // Restore pending.
 assert(ArtisanClock::canObserveUi(true,true,false,false)); // Loaded, including pause.
 // Requested exact scenarios, observing only the shared clock (no button hook).
 Ledger thirty;Line a=item(1);a.item="chest";a.base=6000;a.unit=price(a.base);a.hours=series(a.base,1);Line b=a;b.item="helmet";
 thirty.put("smith",a,1);thirty.put("smith",b,1);assert(thirty.confirm("smith","Smith",100,100000));assert(thirty.orders[0].end==130);
 for(int frame=0;frame<100;++frame)thirty.tick(WorldServiceClock::now(100,0));assert(thirty.orders[0].end-WorldServiceClock::now(100,0)==30); // Pause.
 thirty.tick(WorldServiceClock::now(101,0));assert(thirty.orders[0].end-WorldServiceClock::now(101,0)==29); // Normal world step.
 thirty.tick(WorldServiceClock::now(105,0));assert(thirty.orders[0].end-WorldServiceClock::now(105,0)==25); // Four world hours in one update.
 thirty.tick(WorldServiceClock::now(100,24));assert(thirty.orders[0].end-WorldServiceClock::now(100,24)==6&&thirty.orders[0].status==Making);
 Ledger six;Line shortItem=a;shortItem.base=500;shortItem.unit=price(500);shortItem.hours=series(500,1);six.put("smith",shortItem,1);assert(six.confirm("smith","Smith",100,100000));six.tick(WorldServiceClock::now(100,24));assert(six.orders[0].status==Ready);
 // Old private clock offset 24, shared offset 120: preserve 6 h, FIFO and data.
 Ledger migrated=thirty;double remaining=migrated.orders[0].end-WorldServiceClock::now(100,24);ArtisanClock::alignToShared(migrated,24,120);
 assert(migrated.orders[0].end-WorldServiceClock::now(100,120)==remaining&&migrated.orders[0].paid==thirty.orders[0].paid);
 double savedShared=0;Ledger again=ArtisanClock::load(ArtisanClock::save(migrated,120),savedShared);ArtisanClock::alignToShared(again,savedShared,120);assert(save(again)==save(migrated));
 Ledger old=thirty;ArtisanClock::alignToShared(old,0,120);assert(old.orders[0].end-WorldServiceClock::now(100,120)==30);ArtisanOrders::load(save(old));
 Ledger l;l.put("smith",item(3),3);assert(l.confirm("smith","Smith",100,100000));
 l.put("smith",item(1),1);assert(l.confirm("smith","Smith",100,100000));
 double offset=0,world=100;assert(l.orders[0].end==150&&l.orders[1].start==150&&l.orders[1].end==170);
 offset+=24;l.tick(ArtisanClock::now(world,offset));assert(l.orders[0].status==Making&&l.orders[1].status==Waiting);assert(l.orders[0].end-ArtisanClock::now(world,offset)==26);
 // Paused world; second +24 crosses no world frame but still advances orders.
 offset+=24;l.tick(ArtisanClock::now(world,offset));assert(l.orders[0].end-ArtisanClock::now(world,offset)==2);
 std::string bytes=ArtisanClock::save(l,offset);double loaded=999;Ledger restored=ArtisanClock::load(bytes,loaded);assert(loaded==48&&save(restored)==save(l));
 world+=2;restored.tick(ArtisanClock::now(world,loaded));assert(restored.orders[0].status==Ready&&restored.orders[1].status==Making);
 loaded+=48;restored.tick(ArtisanClock::now(world,loaded));assert(restored.orders[1].status==Ready);
 Ledger legacy=ArtisanClock::load(save(l),loaded);assert(loaded==0&&save(legacy)==save(l)); // No retroactive historical cheats.
 ArtisanClock::load("",loaded);assert(loaded==0); // A/B/A does not retain another world's offset.
 bool rejected=false;try{ArtisanClock::load("ARTISAN_CLOCK1 -1\n"+save(l),loaded);}catch(...){rejected=true;}assert(rejected);
 rejected=false;try{ArtisanClock::load("ARTISAN_CLOCK1 24x\n"+save(l),loaded);}catch(...){rejected=true;}assert(rejected);
 std::cout<<"PASS Artisan world time, +24 while paused, repeated jumps, +48, FIFO transitions, save/load, legacy and A/B/A, invalid clocks\n";
}
