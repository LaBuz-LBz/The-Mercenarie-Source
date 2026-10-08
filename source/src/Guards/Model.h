#pragma once
#include <map>
#include <set>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include "TagAppearance.h"

namespace GuildGuards {
typedef unsigned long long Id;
struct Position {
    double x,y,z;
    Position(double X=0,double Y=0,double Z=0):x(X),y(Y),z(Z){}
    double distance2(const Position& p)const {double a=x-p.x,b=y-p.y,c=z-p.z;return a*a+b*b+c*c;}
    bool valid()const {return x>=-1e9&&x<=1e9&&y>=-1e9&&y<=1e9&&z>=-1e9&&z<=1e9;}
};
struct Tag {Id id;std::string name;bool paused;int colour;std::string description;Tag():id(0),paused(false),colour(-1){}};
struct Post {Id id,tag;std::string name;Position position;double heading;int priority;bool enabled;Post():id(0),tag(0),heading(0),priority(1),enabled(true){}};
struct Guard {
    std::string actor,name;bool robot,paused,recoveryRequired,recoveryReleased;std::vector<Id> tags;Id assigned;
    Guard():robot(false),paused(false),recoveryRequired(false),recoveryReleased(false),assigned(0){}
};
struct Model {
    std::map<Id,Tag> tags;std::map<Id,Post> posts;std::map<std::string,Guard> guards;
    bool paused;Id sequence;
    Model():paused(false),sequence(0){}
    Id next(){do {++sequence;}while(!sequence||tags.count(sequence)||posts.count(sequence));return sequence;}
    void erasePost(Id id){posts.erase(id);for(std::map<std::string,Guard>::iterator i=guards.begin();i!=guards.end();++i)if(i->second.assigned==id)i->second.assigned=0;}
    void eraseTag(Id id){
        tags.erase(id);
        for(std::map<Id,Post>::iterator i=posts.begin();i!=posts.end();){Id p=i->first;++i;if(posts[p].tag==id)erasePost(p);}
        for(std::map<std::string,Guard>::iterator i=guards.begin();i!=guards.end();++i){std::vector<Id>& t=i->second.tags;t.erase(std::remove(t.begin(),t.end(),id),t.end());}
    }
    void clean(){
        for(std::map<Id,Tag>::iterator i=tags.begin();i!=tags.end();)if(!i->first||i->first!=i->second.id){tags.erase(i++);}else {i->second.colour=tagColour(i->second.id,i->second.colour);i->second.description=cosmeticText(i->second.description,512);++i;}
        for(std::map<Id,Post>::iterator i=posts.begin();i!=posts.end();){Post& p=i->second;Id id=i->first;++i;if(!id||p.id!=id||!tags.count(p.tag)||!p.position.valid()||!(p.heading>=-1e9&&p.heading<=1e9))erasePost(id);else {p.heading=std::fmod(p.heading+360.0,360.0);if(p.heading<0)p.heading+=360;p.priority=std::max(1,p.priority);}}
        std::set<Id> occupied;
        for(std::map<std::string,Guard>::iterator i=guards.begin();i!=guards.end();){Guard& g=i->second;if(i->first.empty()||g.actor!=i->first){guards.erase(i++);continue;}std::set<Id> seen;std::vector<Id> valid;for(size_t k=0;k<g.tags.size();++k)if(tags.count(g.tags[k])&&seen.insert(g.tags[k]).second)valid.push_back(g.tags[k]);g.tags.swap(valid);
            if(!posts.count(g.assigned)||!seen.count(posts[g.assigned].tag)||!occupied.insert(g.assigned).second)g.assigned=0;
            ++i;
        }
        // Never create a placeholder entry while validating an absent reference.
        posts.erase(0);
    }
};
enum State {OnPost,Travelling,Combat,Returning,Eating,SeekingBed,Resting,Waiting,Paused,KnockedOut,Unavailable};
enum PostState {Free,Occupied,Reserved,Invalid};
struct Observation {
    std::set<Id> inaccessible;
    Position position;bool available,ko,combat,eating,bedAvailable,inBed,manualOrder,enemyOutside;
    double health;
    Observation():available(false),ko(false),combat(false),eating(false),bedAvailable(false),inBed(false),manualOrder(false),enemyOutside(false),health(1){}
};
struct Runtime {State state;bool recovering;double vacancySince;Runtime():state(Waiting),recovering(false),vacancySince(0){}};
inline int rank(const Model& m,const Guard& g,Id tag){for(size_t i=0;i<g.tags.size();++i){std::map<Id,Tag>::const_iterator t=m.tags.find(g.tags[i]);if(g.tags[i]==tag&&t!=m.tags.end()&&!t->second.paused)return (int)i;}return -1;}
inline bool better(const Model& m,const Guard& g,const Post& a,const Post& b){int ar=rank(m,g,a.tag),br=rank(m,g,b.tag);return ar<br||(ar==br&&(a.priority<b.priority||(a.priority==b.priority&&a.id<b.id)));}
}
