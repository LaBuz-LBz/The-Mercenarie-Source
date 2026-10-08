#pragma once
#include "Model.h"
#include "../../MissionArchive.h"
namespace GuildGuards {
inline void recordTagBase(MissionArchive& a,Tag& t){a.field(t.id);a.field(t.name);a.field(t.paused);}
inline void record(MissionArchive& a,Tag& t){
 recordTagBase(a,t);
 if(a.reading){t.colour=tagColour(t.id,-1);t.description.clear();if(a.remaining()){
  try{int colour=-1;std::string description;a.field(colour);a.field(description);a.finish();t.colour=tagColour(t.id,colour);t.description=cosmeticText(description,512);}
  catch(const std::runtime_error&){/* A damaged cosmetic tail never discards the tag or its references. */a.cursor=a.bytes.size();}
 }}else{int colour=tagColour(t.id,t.colour);std::string description=cosmeticText(t.description,512);a.field(colour);a.field(description);}
}
inline void record(MissionArchive& a,Post& p){a.field(p.id);a.field(p.tag);a.field(p.name);a.field(p.position.x);a.field(p.position.y);a.field(p.position.z);a.field(p.heading);a.field(p.priority);a.field(p.enabled);}
inline void record(MissionArchive& a,Guard& g){a.field(g.actor);a.field(g.name);a.field(g.robot);a.field(g.paused);a.field(g.recoveryRequired);a.field(g.recoveryReleased);a.field(g.assigned);unsigned int n=(unsigned int)g.tags.size();a.field(n);a.requireElements(n,sizeof(Id));if(a.reading)g.tags.resize(n);for(size_t i=0;i<n;++i)a.field(g.tags[i]);}
template<class K,class V>void records(MissionArchive& out,const std::map<K,V>& values){
    for(typename std::map<K,V>::const_iterator i=values.begin();i!=values.end();++i){
        try{V v=i->second;MissionArchive row;record(row,v);out.field(row.bytes);}catch(const std::runtime_error&){/* One invalid entry never aborts the other entries. */}
    }
}
inline std::string saveVersion(Model model,bool appearance,size_t budget=MissionArchive::MaximumBytes){
 model.clean();MissionArchive out(budget);unsigned int version=appearance?2:1;out.field(version);out.field(model.paused);out.field(model.sequence);
 MissionArchive tags,posts,guards;
 for(std::map<Id,Tag>::const_iterator i=model.tags.begin();i!=model.tags.end();++i){Tag t=i->second;MissionArchive base;try{recordTagBase(base,t);}catch(const std::runtime_error&){continue;}
  if(appearance){MissionArchive full=base;int colour=tagColour(t.id,t.colour);std::string description=cosmeticText(t.description,512);try{full.field(colour);full.field(description);tags.field(full.bytes);}catch(const std::runtime_error&){tags.field(base.bytes);}}
  else tags.field(base.bytes);
 }
 records(posts,model.posts);records(guards,model.guards);out.field(tags.bytes);out.field(posts.bytes);out.field(guards.bytes);return out.bytes;
}
inline std::string save(Model model,size_t budget=MissionArchive::MaximumBytes){
 // If cosmetic data exhausts the archive budget, preserve the complete business data in v1.
 try{return saveVersion(model,true,budget);}catch(const std::runtime_error&){return saveVersion(model,false,budget);}
}
template<class V>std::vector<V> rows(const std::string& bytes){
    std::vector<V> values;MissionArchive in(bytes);
    while(in.remaining()){
        std::string framed;try{in.field(framed);}catch(const std::runtime_error&){break;}
        try{MissionArchive row(framed);V v;record(row,v);row.finish();values.push_back(v);}catch(const std::runtime_error&){}
    }return values;
}
inline Model load(const std::string& bytes){
    Model result;
    try{
        MissionArchive a(bytes);unsigned int version=0;a.field(version);if(version!=1&&version!=2)return result;a.field(result.paused);a.field(result.sequence);
        std::string t,p,g;a.field(t);a.field(p);a.field(g);a.finish();
        std::vector<Tag> tags=rows<Tag>(t);for(size_t i=0;i<tags.size();++i)result.tags.insert(std::make_pair(tags[i].id,tags[i]));
        std::vector<Post> posts=rows<Post>(p);for(size_t i=0;i<posts.size();++i)result.posts.insert(std::make_pair(posts[i].id,posts[i]));
        std::vector<Guard> guards=rows<Guard>(g);for(size_t i=0;i<guards.size();++i)result.guards.insert(std::make_pair(guards[i].actor,guards[i]));
        result.clean();
    }catch(const std::runtime_error&){result=Model();}
    return result;
}
}
