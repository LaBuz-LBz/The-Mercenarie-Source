#include "GuildVisitorTravel.h"
#include "GuildSeatQueue.h"
#include "MissionArchive.h"
#include <cassert>
#include <iostream>
using namespace GuildVisitorTravel;
int main(){
    State s;assert(update(s,true,false,false,false,0)==InitialOrder);issued(s);
    // Near a wall or a wrong building is still outside the target interior.
    for(int i=0;i<10000;++i){assert(update(s,true,false,true,false,0.1f)==None);assert(s.phase==TravellingToOffice);assigned(s,false,false);assert(s.phase==TravellingToOffice);}
    // A transient failure must not cause a frame-by-frame replacement.
    assert(update(s,true,false,false,true,1)==None);
    assert(update(s,true,false,false,false,1)==None);
    assert(update(s,true,false,false,true,2)==None);
    assert(update(s,true,false,false,true,1)==NativePathFailure);issued(s);
    assert(update(s,true,false,false,true,0.01f)==None);
    assert(update(s,true,false,true,false,0.1f)==None);
    assert(update(s,true,false,false,false,11.9f)==None);
    assert(update(s,true,false,false,false,0.2f)==Stagnation);issued(s);
    assert(update(s,true,true,false,false,0)==None);assert(s.phase==ArrivedAtOffice);
    assigned(s,true,false);assert(s.phase==TravellingToSeat);
    MissionArchive out;s.archive(out);State restored;MissionArchive in(out.bytes);restored.archive(in);in.finish();
    // Repair old radius-only arrivals saved on the other side of a wall.
    assert(update(restored,true,false,false,false,0)==RestoredOrder);assert(restored.phase==TravellingToOffice);assert(!eligible(restored));
    issued(restored);assert(update(restored,true,false,true,false,1)==None);
    using namespace GuildSeatQueue;
    std::vector<Seat> seats;seats.push_back(Seat("client",ClientSeat));seats.push_back(Seat("waiting",WaitingSeat));seats[1].blocked=true;seats[1].nativeOwner="B";
    std::vector<Person> people;people.push_back(Person("B",1,0,true,true));
    std::vector<Assignment> a=plan(people,seats);assert(a.size()==1&&a[0].seat=="waiting");
    people.push_back(Person("collector",2,0,true));
    a=plan(people,seats);assert(a.size()==1&&a[0].person=="B"&&a[0].seat=="waiting");
    people.pop_back();
    people[0].travelling=false;a=plan(people,seats);assert(a.size()==1&&a[0].seat=="client");
    people[0].travelling=true;seats.erase(seats.begin()+1);a=plan(people,seats);assert(a.size()==1&&a[0].seat=="client");assert(unique(a));
    std::cout<<"native path policy: stable orders, true arrival, failure grace, stagnation, restore, in-flight seat pin passed\n";
}
