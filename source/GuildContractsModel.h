#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
namespace GuildContracts {
enum Status {Active,Completed,Failed,Archived};
enum Source {Escort,Bounty,Mail,Delegated,History};
struct Row {
 std::string mapTownId;std::string key,id,name,donor,type,destination,target,reward,difficulty,duration,state,description,objectives,search;
 int bonus;int source,slot,status,donorIcon,typeIcon,danger;bool canMap,canCancel;double started,rewardValue,durationHours;
 Row():bonus(-1),source(History),slot(-1),status(Archived),donorIcon(0),typeIcon(0),danger(0),canMap(false),canCancel(false),started(-1),rewardValue(-1),durationHours(-1){}
};
inline std::string fold(std::string s){for(size_t i=0;i<s.size();++i){unsigned char c=s[i];if(c<128)s[i]=(char)std::tolower(c);else if(c==0xc3&&i+1<s.size()){unsigned char d=s[i+1];if(d>=0x80&&d<=0x9e&&d!=0x97)s[++i]=(char)(d+32);}}return s;}
struct Counts {int total,active,completed,failed;Counts():total(0),active(0),completed(0),failed(0){}};
inline Counts count(const std::vector<Row>& rows){Counts c;c.total=(int)rows.size();for(size_t i=0;i<rows.size();++i){if(rows[i].status==Active)++c.active;else if(rows[i].status==Completed)++c.completed;else if(rows[i].status==Failed)++c.failed;}return c;}
struct RecentOrder {const std::vector<Row>* rows;RecentOrder(const std::vector<Row>& r):rows(&r){}bool operator()(int a,int b)const{const Row& x=(*rows)[a];const Row& y=(*rows)[b];if((x.status==Active)!=(y.status==Active))return x.status==Active;if(x.status==Active&&y.status==Active){if((x.started>=0)!=(y.started>=0))return x.started>=0;if(x.started>=0&&y.started>=0&&x.started!=y.started)return x.started>y.started;}return false;}};
struct ValueOrder {const std::vector<Row>* rows;int mode;ValueOrder(const std::vector<Row>& r,int m):rows(&r),mode(m){}double value(const Row& r)const{return mode==2||mode==3?r.rewardValue:mode==4?(r.danger>0?r.danger:-1):r.durationHours;}bool operator()(int a,int b)const{double x=value((*rows)[a]),y=value((*rows)[b]);if((x>=0)!=(y>=0))return x>=0;if(x<0)return false;return mode==3?x>y:x<y;}};
inline std::vector<int> filter(const std::vector<Row>& rows,int status,const std::string& query,int sort){std::vector<int> out;std::string q=fold(query);for(size_t i=0;i<rows.size();++i){const Row& r=rows[i];if(status>=0&&r.status!=status)continue;std::string hay=r.name+" "+r.donor+" "+r.type+" "+r.destination+" "+r.target+" "+r.search;if(!q.empty()&&fold(hay).find(q)==std::string::npos)continue;out.push_back((int)i);}std::stable_sort(out.begin(),out.end(),RecentOrder(rows));if(sort==1)std::reverse(out.begin(),out.end());else if(sort>=2&&sort<=5)std::stable_sort(out.begin(),out.end(),ValueOrder(rows,sort));return out;}
inline int pages(int count,int capacity){return std::max(1,(count+std::max(1,capacity)-1)/std::max(1,capacity));}
inline int clampPage(int page,int count,int capacity){return std::max(0,std::min(pages(count,capacity)-1,page));}
inline int find(const std::vector<Row>& rows,const std::string& key){for(size_t i=0;i<rows.size();++i)if(rows[i].key==key)return (int)i;return -1;}
inline std::string fingerprint(const std::vector<Row>& rows){std::string s;for(size_t i=0;i<rows.size();++i){const Row& r=rows[i];s+=r.key+'\x1f'+r.name+'\x1f'+r.donor+'\x1f'+r.type+'\x1f'+r.destination+'\x1f'+r.reward+'\x1f'+r.difficulty+'\x1f'+r.duration+'\x1f'+r.state+'\x1f'+r.description+'\x1f'+r.objectives;s+=(char)('0'+r.status);s+=(char)('0'+r.bonus+1);s+=r.canCancel?'C':'-';s+=r.canMap?'M':'-';}return s;}
}
