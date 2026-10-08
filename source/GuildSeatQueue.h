#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace GuildSeatQueue
{
    enum SeatKind { ClientSeat = 1, WaitingSeat = 2 };

    struct Seat
    {
        std::string id;
        SeatKind kind;
        std::string nativeOwner;
        bool blocked;
        Seat(const std::string& i=std::string(),SeatKind k=WaitingSeat):id(i),kind(k),blocked(false){}
    };

    struct Person
    {
        std::string id;
        unsigned long ticket;
        unsigned int memberOrder;
        bool leader;
        bool travelling;
        std::vector<std::string> excluded;
        Person(const std::string& i=std::string(),unsigned long t=0,unsigned int m=0,bool l=false,bool moving=false):id(i),ticket(t),memberOrder(m),leader(l),travelling(moving){}
    };

    struct Assignment
    {
        std::string person;
        std::string seat;
        SeatKind kind;
        Assignment(const std::string& p=std::string(),const std::string& s=std::string(),SeatKind k=WaitingSeat):person(p),seat(s),kind(k){}
    };

    inline bool personBefore(const Person& a,const Person& b)
    {
        if(a.ticket!=b.ticket)return a.ticket<b.ticket;
        if(a.leader!=b.leader)return a.leader;
        if(a.memberOrder!=b.memberOrder)return a.memberOrder<b.memberOrder;
        return a.id<b.id;
    }

    inline bool seatAvailable(const Seat& seat,const std::string& person)
    {
        return (!seat.blocked||seat.nativeOwner==person)&&(seat.nativeOwner.empty()||seat.nativeOwner==person);
    }

    inline bool seatAvailable(const Seat& seat,const Person& person)
    {return std::find(person.excluded.begin(),person.excluded.end(),seat.id)==person.excluded.end()&&seatAvailable(seat,person.id);}

    // Pure deterministic plan. The runtime publishes the complete result as one
    // reservation transaction before it changes any native Kenshi job.
    inline std::vector<Assignment> plan(std::vector<Person> people,const std::vector<Seat>& seats)
    {
        std::sort(people.begin(),people.end(),personBefore);
        std::vector<Assignment> out;
        std::map<std::string,bool> used;
        std::map<std::string,bool> assigned;

        // Do not promote a visitor away from a still-valid seat while walking to it.
        for(size_t p=0;p<people.size();++p)if(people[p].travelling)
            for(size_t s=0;s<seats.size();++s)if(!used[seats[s].id]&&seats[s].nativeOwner==people[p].id&&seatAvailable(seats[s],people[p]))
            {out.push_back(Assignment(people[p].id,seats[s].id,seats[s].kind));used[seats[s].id]=true;assigned[people[p].id]=true;break;}

        // Prefer the existing reservation within each tier. A newly free earlier
        // chair must not make actors abandon a valid destination every refresh.
        bool earlierLeaderInTransit=false;
        for(size_t p=0;p<people.size();++p)if(people[p].leader)
        {
            if(people[p].travelling&&assigned[people[p].id])
                for(size_t a=0;a<out.size();++a)if(out[a].person==people[p].id&&out[a].kind==WaitingSeat)earlierLeaderInTransit=true;
            if(!assigned[people[p].id])
                for(int own=1;own>=0&&!assigned[people[p].id];--own)
                    for(size_t s=0;s<seats.size();++s)if(seats[s].kind==ClientSeat&&!used[seats[s].id]&&seatAvailable(seats[s],people[p])&&(!own||seats[s].nativeOwner==people[p].id)&&(own||!earlierLeaderInTransit))
                    {out.push_back(Assignment(people[p].id,seats[s].id,ClientSeat));used[seats[s].id]=true;assigned[people[p].id]=true;break;}
        }

        for(size_t p=0;p<people.size();++p)if(!assigned[people[p].id])
            for(int own=1;own>=0&&!assigned[people[p].id];--own)
                for(size_t s=0;s<seats.size();++s)if(seats[s].kind==WaitingSeat&&!used[seats[s].id]&&seatAvailable(seats[s],people[p])&&(!own||seats[s].nativeOwner==people[p].id))
                {out.push_back(Assignment(people[p].id,seats[s].id,WaitingSeat));used[seats[s].id]=true;assigned[people[p].id]=true;break;}
        return out;
    }

    inline bool unique(const std::vector<Assignment>& assignments)
    {
        std::map<std::string,bool> people,seats;
        for(size_t i=0;i<assignments.size();++i){if(people[assignments[i].person]||seats[assignments[i].seat])return false;people[assignments[i].person]=seats[assignments[i].seat]=true;}
        return true;
    }
}
