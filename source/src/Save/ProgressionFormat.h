#pragma once
#include <algorithm>
#include <cmath>
#include <climits>
#include <sstream>
#include <string>
#include <locale>
#include <vector>
#include <set>
#include <stdexcept>

// Existing on-disk protocol, deliberately unchanged (including legacy versions).
// This codec does not open files or own live guild/mission state.
namespace GuildProgression {
enum { MaximumProgressBytes=16*1024*1024,MaximumPaidIds=250000 };
template<class Archive,class Handle>
void archiveOutcome(Archive& a,std::vector<Handle>& members,bool& known,bool& ko,int& local,int& global,std::set<unsigned int>& losses){
 std::string marker="GUILD-PROGRESSION-2";a.field(marker);if(marker!="GUILD-PROGRESSION-2")throw std::runtime_error("unknown guild progression extension");
 a.field(members);a.field(known);a.field(ko);a.field(local);a.field(global);
 std::vector<unsigned int> dead;if(!a.reading)dead.assign(losses.begin(),losses.end());a.field(dead);
 for(size_t i=0;i<dead.size();++i)if(dead[i]>=members.size())throw std::runtime_error("invalid guild roster loss");
 if(a.reading){losses.clear();for(size_t i=0;i<dead.size();++i)losses.insert(dead[i]);}
 if(local< -100||local>100||global< -100||global>100)throw std::runtime_error("invalid guild report reputation");
}
inline bool number(const std::string& text,double& value){std::istringstream in(text);in.imbue(std::locale::classic());if(!(in>>value)||value!=value||value>9e15||value< -9e15)return false;in>>std::ws;return in.eof();}
inline float decimal(const std::string& text){double n=0;return number(text,n)?(float)n:0;}
inline unsigned int checksum(const std::string& bytes){unsigned int h=2166136261u;for(size_t i=0;i<bytes.size();++i){h^=(unsigned char)bytes[i];h*=16777619u;}return h;}
inline std::string sealProgress(const std::string& bytes){std::ostringstream out;out.imbue(std::locale::classic());out<<bytes<<"@checksum|"<<checksum(bytes)<<"\n";return out.str();}
inline bool validProgress(const std::string& bytes){
 // Normalise text-mode Windows line endings before checking the stable bytes.
 if(bytes.find('\r')!=std::string::npos){std::string normalized=bytes;normalized.erase(std::remove(normalized.begin(),normalized.end(),'\r'),normalized.end());return validProgress(normalized);}
 if(bytes.empty()||bytes.size()>MaximumProgressBytes)return false;std::istringstream in(bytes);std::string line;int stats=0,version=0,paid=0,statsFields=0;
 while(std::getline(in,line)){if(!line.empty()&&line[line.size()-1]=='\r')line.erase(line.size()-1);
  if(line.find("@progress|")==0){if((line!="@progress|2"&&line!="@progress|3"&&line!="@progress|4")||++version>1)return false;}
  if(line.find("@paid|")==0){if(line.size()<=6||line.size()>300||++paid>MaximumPaidIds)return false;}
  if(line.find("@stats|")!=0)continue;if(++stats>1)return false;
  std::istringstream fields(line.substr(7));std::string field;int n=0;
  while(std::getline(fields,field,'|')){double value;if(!number(field,value))return false;if(n!=3&&(value<0||floor(value)!=value))return false;if(n==3&&(value< -100||value>100))return false;if((n==0||n==1||n==4||n==5)&&value>INT_MAX)return false;++n;}
  if(n<4||n>9)return false;statsFields=n;
 }
 if(stats!=1||version&&statsFields!=9)return false;
 if(version){size_t at=bytes.rfind("@checksum|");if(at==std::string::npos||at&&bytes[at-1]!='\n')return false;double stored=0;if(!number(bytes.substr(at+10),stored)||stored!=checksum(bytes.substr(0,at)))return false;}
 return true;
}
}
