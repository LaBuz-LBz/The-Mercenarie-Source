#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
namespace GuildHistory {
// Versioned value object. Never owns a live actor, widget, payment or mission.
struct Snapshot {
 std::string id,giverId,giverName,type,origin,destination,target,originId,destinationId,routeIds;
 int giver,status,bonus,difficulty;long long reward,bonusReward;
 double accepted,completed,duration,deadline,estimateMin,estimateMax;
 Snapshot():giver(0),status(3),bonus(-1),difficulty(-1),reward(-1),bonusReward(-1),accepted(-1),completed(-1),duration(-1),deadline(-1),estimateMin(-1),estimateMax(-1){}
};
inline std::string hex(const std::string& s){const char* digits="0123456789abcdef";std::string out;for(size_t i=0;i<s.size();++i){unsigned char c=s[i];out+=digits[c>>4];out+=digits[c&15];}return out.empty()?"-":out;}
inline bool unhex(const std::string& s,std::string& out){out.clear();if(s=="-")return true;if(s.size()%2||s.size()>32768)return false;for(size_t i=0;i<s.size();i+=2){int n=0;for(int j=0;j<2;++j){char c=s[i+j];int d=c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;if(d<0)return false;n=n*16+d;}if(!n)return false;out+=(char)n;}return true;}
inline std::string encode(const Snapshot& s){std::ostringstream o;o<<"@contract1 "<<hex(s.id)<<' '<<hex(s.giverId)<<' '<<hex(s.giverName)<<' '<<hex(s.type)<<' '<<hex(s.origin)<<' '<<hex(s.destination)<<' '<<hex(s.target)<<' '<<hex(s.originId)<<' '<<hex(s.destinationId)<<' '<<hex(s.routeIds)<<' '<<s.giver<<' '<<s.status<<' '<<s.bonus<<' '<<s.difficulty<<' '<<s.reward<<' '<<s.bonusReward<<' '<<std::setprecision(17)<<s.accepted<<' '<<s.completed<<' '<<s.duration<<' '<<s.deadline<<' '<<s.estimateMin<<' '<<s.estimateMax;return o.str();}
inline bool decode(const std::string& raw,Snapshot& result){if(raw.compare(0,11,"@contract1 ")!=0)return false;Snapshot s;std::istringstream in(raw.substr(11));std::string f[10];for(int i=0;i<10;++i)if(!(in>>f[i]))return false;if(!unhex(f[0],s.id)||!unhex(f[1],s.giverId)||!unhex(f[2],s.giverName)||!unhex(f[3],s.type)||!unhex(f[4],s.origin)||!unhex(f[5],s.destination)||!unhex(f[6],s.target)||!unhex(f[7],s.originId)||!unhex(f[8],s.destinationId)||!unhex(f[9],s.routeIds))return false;
 if(!(in>>s.giver>>s.status>>s.bonus>>s.difficulty>>s.reward>>s.bonusReward>>s.accepted>>s.completed>>s.duration>>s.deadline>>s.estimateMin>>s.estimateMax))return false;in>>std::ws;if(!in.eof()||s.id.empty()||s.type.empty()||s.status<0||s.status>4||s.giver<0||s.giver>5||s.reward<-1||s.bonusReward<-1)return false;double times[]={s.accepted,s.completed,s.duration,s.deadline,s.estimateMin,s.estimateMax};for(int i=0;i<6;++i)if(!(times[i]>=-1&&times[i]<=1e9))return false;result=s;return true;}
inline bool contains(const std::vector<std::string>& history,const std::string& id){for(size_t i=0;i<history.size();++i){Snapshot s;if(decode(history[i],s)&&s.id==id)return true;}return false;}
inline bool append(std::vector<std::string>& history,const Snapshot& s){if(s.id.empty()||contains(history,s.id))return false;history.insert(history.begin(),encode(s));if(history.size()>50)history.resize(50);return true;}
}
