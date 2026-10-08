#pragma once
#include <string>
#include <vector>
namespace MailIdentity {
struct Step {std::string id,handle,carrier;Step(const std::string& a,const std::string& b,const std::string& c):id(a),handle(b),carrier(c){}};
struct Item {std::string id,handle,carrier;Item(const std::string& a,const std::string& b,const std::string& c):id(a),handle(b),carrier(c){}};
inline std::vector<int> bind(const std::vector<Step>& steps,const std::vector<Item>& items){
 std::vector<int> result(steps.size(),-1);std::vector<bool> used(items.size(),false);
 // New saves: only the stable token can override a changed runtime handle.
 // Legacy saves: exact handle, then a unique old letter on its saved carrier.
 for(int pass=0;pass<3;++pass)for(size_t s=0;s<steps.size();++s)if(result[s]<0){
  int match=-1,count=0;
  for(size_t i=0;i<items.size();++i)if(!used[i]){
   bool ok=pass==0?items[i].id==steps[s].id:items[i].id.empty()&&(pass==1?items[i].handle==steps[s].handle:items[i].carrier==steps[s].carrier);
   if(ok){match=(int)i;++count;}
  }
  if(count!=1)continue;
  if(pass==2){int peers=0;for(size_t t=0;t<steps.size();++t)if(result[t]<0&&steps[t].carrier==steps[s].carrier)++peers;if(peers!=1)continue;}
  result[s]=match;used[match]=true;
 }
 // A single unmatched legacy step and a single untagged letter are unambiguous.
 int missing=-1,available=-1,ns=0,ni=0;
 for(size_t s=0;s<steps.size();++s)if(result[s]<0){missing=(int)s;++ns;}
 for(size_t i=0;i<items.size();++i)if(!used[i]&&items[i].id.empty()){available=(int)i;++ni;}
 if(ns==1&&ni==1)result[missing]=available;
 return result;
}
}
