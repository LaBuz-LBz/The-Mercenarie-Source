#include "GuildVisitorTravel.h"
#include "GuildSeatQueue.h"
#include "MissionArchive.h"
#include <cassert>
#include <iostream>
using namespace GuildVisitorTravel;

static State reload(State s){MissionArchive out;s.archive(out);MissionArchive in(out.bytes);State r;r.archive(in);in.finish();return r;}
static std::string seat(const std::vector<GuildSeatQueue::Assignment>& a,const std::string& who){for(size_t i=0;i<a.size();++i)if(a[i].person==who)return a[i].seat;return "";}
int main(){
    State client;assert(client.phase==Spawned);
    assert(update(client,false,false,false,false,1)==None);assert(client.phase==Spawned);
    assert(update(client,true,false,false,false,0)==InitialOrder);issued(client);assert(client.phase==TravellingToOffice);
    assigned(client,false,false);assert(client.phase==TravellingToOffice);assert(!eligible(client));
    for(int i=0;i<100;++i)assert(update(client,true,false,true,false,0.1f)==None);
    State travelling=reload(client);assert(travelling.phase==TravellingToOffice);
    assert(update(travelling,true,false,false,false,0)==RestoredOrder);issued(travelling);
    assert(update(travelling,true,false,true,false,1)==None);
    assert(update(travelling,true,false,false,false,11)==None);assert(update(travelling,true,false,false,false,1)==Stagnation);issued(travelling);
    assert(update(travelling,true,true,false,false,0)==None);assert(travelling.phase==ArrivedAtOffice);assert(eligible(travelling));
    assigned(travelling,false,false);assert(travelling.phase==WaitingForSeat);
    State waiting=reload(travelling);assert(waiting.phase==WaitingForSeat);assert(update(waiting,true,true,false,false,0)==None);
    assigned(waiting,true,false);assert(waiting.phase==TravellingToSeat);
    State transfer=reload(waiting);assert(transfer.phase==TravellingToSeat);assert(transfer.restoreOrder);
    assigned(transfer,true,true);assert(transfer.phase==Seated);
    State collector;assert(update(collector,true,false,false,false,0)==InitialOrder);issued(collector);assert(!eligible(collector));
    assert(update(collector,true,true,true,false,2)==None);assert(eligible(collector));
    using namespace GuildSeatQueue;
    std::vector<Person> people;people.push_back(Person("B",2,0,true));people.push_back(Person("collector",3,0,true));
    std::vector<Seat> chairs;chairs.push_back(Seat("officeA/client",ClientSeat));chairs.push_back(Seat("officeA/wait1",WaitingSeat));chairs.push_back(Seat("officeA/wait2",WaitingSeat));
    chairs[0].blocked=true;chairs[0].nativeOwner="external";
    std::vector<Assignment> a=plan(people,chairs);assert(seat(a,"B")=="officeA/wait1");assert(seat(a,"collector")=="officeA/wait2");assert(unique(a));
    chairs[1].blocked=chairs[2].blocked=true;chairs[1].nativeOwner="B";chairs[2].nativeOwner="collector";
    chairs[0].blocked=false;chairs[0].nativeOwner.clear();a=plan(people,chairs);assert(seat(a,"B")=="officeA/client");assert(seat(a,"collector")=="officeA/wait2");assert(unique(a));
    chairs[0].blocked=true;chairs[0].nativeOwner="B";
    a=plan(people,chairs);assert(seat(a,"B")=="officeA/client");assert(unique(a));
    for(size_t i=0;i<chairs.size();++i){chairs[i].blocked=true;chairs[i].nativeOwner="external";}assert(plan(people,chairs).empty());
    std::vector<Seat> stable;stable.push_back(Seat("freeEarlier",ClientSeat));stable.push_back(Seat("reserved",ClientSeat));stable[1].blocked=true;stable[1].nativeOwner="B";
    a=plan(people,stable);assert(seat(a,"B")=="reserved");assert(seat(a,"collector")=="freeEarlier");assert(unique(a));
    // Old sidecar has no optional travel block: default phase reconstructs travel.
    State legacy;MissionArchive old;MissionArchive oldRead(old.bytes);if(oldRead.cursor<oldRead.bytes.size())legacy.archive(oldRead);oldRead.finish();assert(update(legacy,true,false,false,false,0)==InitialOrder);
    for(int phase=Spawned;phase<=Seated;++phase){State saved;saved.phase=(Phase)phase;State restored=reload(saved);assert(restored.phase==saved.phase);}
    std::cout<<"visitor travel, recovery, FIFO and seat transfer tests passed\n";
}
