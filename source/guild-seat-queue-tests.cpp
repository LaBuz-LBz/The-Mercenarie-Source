#include "GuildSeatQueue.h"
#include "GuildVisitorTravel.h"
#include <cassert>
#include <iostream>
using namespace GuildSeatQueue;

static std::string assigned(const std::vector<Assignment>& a,const std::string& person)
{for(size_t i=0;i<a.size();++i)if(a[i].person==person)return a[i].seat;return "";}

int main()
{
    // The collector follows the same arrival -> assigned -> native seated cycle.
    for(int occupied=0;occupied<2;++occupied){
        GuildVisitorTravel::State travel;
        GuildVisitorTravel::update(travel,true,true,false,false,0);
        assert(travel.phase==GuildVisitorTravel::ArrivedAtOffice);
        std::vector<Seat> fiscalSeats;fiscalSeats.push_back(Seat("client",ClientSeat));fiscalSeats.push_back(Seat("waiting",WaitingSeat));
        if(occupied){fiscalSeats[0].blocked=true;fiscalSeats[0].nativeOwner="customer";}
        std::vector<Person> fiscalPeople;fiscalPeople.push_back(Person("collector",1,0,true));
        std::vector<Assignment> fiscalPlan=plan(fiscalPeople,fiscalSeats);
        assert(unique(fiscalPlan)&&assigned(fiscalPlan,"collector")== (occupied?"waiting":"client"));
        GuildVisitorTravel::assigned(travel,true,false);assert(travel.phase==GuildVisitorTravel::TravellingToSeat);
        GuildVisitorTravel::issued(travel);
        GuildVisitorTravel::assigned(travel,true,true);assert(travel.phase==GuildVisitorTravel::Seated);
        if(occupied){fiscalSeats[0].blocked=false;fiscalSeats[0].nativeOwner.clear();fiscalSeats[1].blocked=true;fiscalSeats[1].nativeOwner="collector";
            fiscalPeople.push_back(Person("later-customer",2,0,true));fiscalPlan=plan(fiscalPeople,fiscalSeats);
            assert(unique(fiscalPlan)&&assigned(fiscalPlan,"collector")=="client");
            GuildVisitorTravel::assigned(travel,true,false);assert(travel.phase==GuildVisitorTravel::TravellingToSeat);
            GuildVisitorTravel::assigned(travel,true,true);assert(travel.phase==GuildVisitorTravel::Seated);
        }
    }
    std::vector<Seat> seats;Seat client("client",ClientSeat);client.blocked=true;client.nativeOwner="A";seats.push_back(client);seats.push_back(Seat("wait1",WaitingSeat));seats.push_back(Seat("wait2",WaitingSeat));
    std::vector<Person> people;people.push_back(Person("B",2,0,true));people.push_back(Person("F",3,0,true));
    std::vector<Assignment> a=plan(people,seats);assert(assigned(a,"B")=="wait1");assert(assigned(a,"F")=="wait2");assert(unique(a));

    seats[0].blocked=false;seats[0].nativeOwner.clear();a=plan(people,seats);assert(assigned(a,"B")=="client");assert(assigned(a,"F")=="wait1");

    seats.push_back(Seat("client2",ClientSeat));people.push_back(Person("C",4,0,true));a=plan(people,seats);assert(assigned(a,"B")=="client");assert(assigned(a,"F")=="client2");assert(assigned(a,"C")=="wait1");assert(unique(a));

    people.push_back(Person("B_companion",2,1,false));a=plan(people,seats);assert(assigned(a,"B_companion")=="wait1");assert(assigned(a,"C")=="wait2");assert(unique(a));

    seats[0].blocked=true;seats[0].nativeOwner="external";seats[1].blocked=true;seats[1].nativeOwner="external2";a=plan(people,seats);assert(assigned(a,"B")!="client");assert(assigned(a,"F")!="client2");assert(unique(a));
    // Capture-sized group: six distinct actors, two Client + six Waiting chairs.
    seats.clear();people.clear();seats.push_back(Seat("c1",ClientSeat));seats.push_back(Seat("c2",ClientSeat));
    const char* ids[]={"w1","w2","w3","w4","w5","w6"};
    const char* actors[]={"clientA","clientB","collector","companionA","clientC","clientD"};
    for(int i=0;i<6;++i){seats.push_back(Seat(ids[i],WaitingSeat));people.push_back(Person(actors[i],i+1,0,i!=3));}
    a=plan(people,seats);assert(a.size()==6&&unique(a));assert(assigned(a,"clientA")=="c1");assert(assigned(a,"clientB")=="c2");
    // Confirmed inaccessible Client chair cannot pin the oldest visitor forever.
    people[0].excluded.push_back("c1");people[0].travelling=true;seats[0].nativeOwner="clientA";seats[0].blocked=true;
    a=plan(people,seats);assert(assigned(a,"clientA")=="c2");assert(unique(a));
    // Release the native reservation, then later actors can use that chair.
    seats[0].nativeOwner.clear();seats[0].blocked=false;a=plan(people,seats);assert(assigned(a,"clientB")=="c1");
    // Departing clients disappear from both the people and ownership lists.
    people.erase(people.begin(),people.begin()+2);a=plan(people,seats);assert(assigned(a,"collector")=="c1");assert(unique(a));
    // Occupied external chairs are never stolen, and a visitor can remain unassigned.
    for(size_t i=0;i<seats.size();++i){seats[i].blocked=true;seats[i].nativeOwner="external";}
    assert(plan(people,seats).empty());
    std::cout<<"cycle group: 2 Client + 6 Waiting, 6 actors, failed seat recovery, collector FIFO, departure passed\n";
    std::cout<<"guild seat queue tests passed\n";
    return 0;
}
