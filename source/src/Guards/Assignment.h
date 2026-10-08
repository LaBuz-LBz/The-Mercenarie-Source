#pragma once
#include "Model.h"
namespace GuildGuards {
// Called on a paced service tick, never per rendered frame. Ownership changes
// are committed here before the native adapter submits any orders.
struct Assignment {
    std::map<std::string,Runtime> runtime;
    std::map<Id,double> freeSince;
    std::map<Id,std::string> owners;
    void reset(){runtime.clear();freeSince.clear();owners.clear();}
    void update(Model& m,const std::map<std::string,Observation>& observations,double seconds){
        m.clean();owners.clear();
        for(std::map<std::string,Guard>::iterator i=m.guards.begin();i!=m.guards.end();++i){
            Guard& g=i->second;Runtime& r=runtime[i->first];
            std::map<std::string,Observation>::const_iterator oi=observations.find(i->first);
            Observation missing;const Observation& o=oi==observations.end()?missing:oi->second;
            if(o.manualOrder)g.paused=true;
            r.recovering=g.recoveryRequired||(o.inBed&&o.health<1);
            if(o.ko){g.assigned=0;g.recoveryReleased=true;g.recoveryRequired=r.recovering=true;r.state=KnockedOut;continue;}
            if(!o.available){g.assigned=0;r.state=Unavailable;continue;}
            if(m.paused||g.paused){g.assigned=0;r.state=Paused;continue;}
            if(!(o.health>=0.60))r.recovering=true;
            if(r.recovering&&o.health>=1.0){r.recovering=false;g.recoveryReleased=false;}
            g.recoveryRequired=r.recovering;
            if(g.assigned){const Post& p=m.posts.find(g.assigned)->second;if(!p.enabled||rank(m,g,p.tag)<0)g.assigned=0;}
            if(r.recovering&&(o.inBed||o.bedAvailable||g.recoveryReleased)&&(!o.combat||!g.assigned)){g.assigned=0;g.recoveryReleased=true;r.state=o.combat?Combat:o.inBed?Resting:SeekingBed;continue;}
            if(g.assigned){
                owners[g.assigned]=g.actor;
                const Post& p=m.posts.find(g.assigned)->second;
                r.state=o.combat?(o.enemyOutside||o.position.distance2(p.position)>10000?Returning:Combat):o.eating?Eating:o.position.distance2(p.position)<=1?OnPost:Returning;
            }else r.state=o.combat?Combat:o.eating?Eating:Waiting;
        }
        for(std::map<Id,Post>::const_iterator p=m.posts.begin();p!=m.posts.end();++p){if(owners.count(p->first))freeSince.erase(p->first);else if(!freeSince.count(p->first))freeSince[p->first]=seconds;}
        // Deferred acceptance: each guard proposes to its best available post;
        // a contested post keeps the nearest proposer, with an identity tie-break.
        // A rejected guard tries the next post. Existing owners are never evicted.
        std::map<std::string,std::set<Id> > rejected;
        std::map<Id,std::string> proposed;
        std::set<std::string> pending;
        for(std::map<std::string,Guard>::const_iterator i=m.guards.begin();i!=m.guards.end();++i){const Runtime& r=runtime[i->first];if(r.state==Waiting||r.state==OnPost)pending.insert(i->first);}
        while(!pending.empty()){
            std::string id=*pending.begin();pending.erase(pending.begin());Guard& g=m.guards[id];const Observation& o=observations.find(id)->second;
            const Post* best=0;
            for(std::map<Id,Post>::const_iterator i=m.posts.begin();i!=m.posts.end();++i){const Post& p=i->second;
                if(!p.enabled||owners.count(p.id)||rejected[id].count(p.id)||o.inaccessible.count(p.id)||rank(m,g,p.tag)<0||o.position.distance2(p.position)>1000000)continue;
                if(g.assigned&&(!better(m,g,p,m.posts.find(g.assigned)->second)||seconds-freeSince[p.id]<10))continue;
                if(!best||better(m,g,p,*best))best=&p;
            }
            if(!best)continue;
            std::map<Id,std::string>::iterator held=proposed.find(best->id);
            if(held!=proposed.end()){
                const double d=o.position.distance2(best->position),old=observations.find(held->second)->second.position.distance2(best->position);
                if(d>old||(d==old&&id>held->second)){rejected[id].insert(best->id);pending.insert(id);continue;}
                rejected[held->second].insert(best->id);pending.insert(held->second);
            }
            proposed[best->id]=id;
        }
        for(std::map<Id,std::string>::const_iterator i=proposed.begin();i!=proposed.end();++i){Guard& g=m.guards[i->second];if(g.assigned){owners.erase(g.assigned);freeSince[g.assigned]=seconds;}g.assigned=i->first;owners[i->first]=i->second;freeSince.erase(i->first);runtime[i->second].state=Travelling;}
        for(std::map<std::string,Runtime>::iterator i=runtime.begin();i!=runtime.end();)if(!m.guards.count(i->first))runtime.erase(i++);else ++i;
        for(std::map<Id,double>::iterator i=freeSince.begin();i!=freeSince.end();)if(!m.posts.count(i->first))freeSince.erase(i++);else ++i;
    }
    PostState status(const Model& m,Id id)const{
        std::map<Id,Post>::const_iterator p=m.posts.find(id);if(p==m.posts.end()||!p->second.enabled)return Invalid;
        std::map<Id,std::string>::const_iterator o=owners.find(id);if(o==owners.end())return Free;
        std::map<std::string,Runtime>::const_iterator r=runtime.find(o->second);return r!=runtime.end()&&r->second.state==OnPost?Occupied:Reserved;
    }
};
}
