#pragma once
namespace CaravanVisit {
struct Plan{bool exists;int count,mask,trap;Plan():exists(false),count(3),mask(7),trap(-1){} };
inline Plan read(const std::string& m){Plan s;size_t p=m.find(";VISIT1=");if(p==std::string::npos)return s;p+=8;
 std::istringstream in(m.substr(p,m.find(';',p)-p));if(!(in>>s.count>>s.mask>>s.trap)||s.count<2||s.count>4||s.mask<0||s.mask>=(1<<s.count)||s.trap< -1||s.trap>=s.count||(s.trap>=0&&!(s.mask&(1<<s.trap))))throw std::runtime_error("invalid caravan visit");s.exists=true;return s;}
inline void write(std::string& m,const Plan& s){size_t p=m.find(";VISIT1=");if(p!=std::string::npos){size_t e=m.find(';',p+1);m.erase(p,e==std::string::npos?std::string::npos:e-p+1);}std::ostringstream out;out<<";VISIT1="<<s.count<<' '<<s.mask<<' '<<s.trap<<';';m+=out.str();}
inline Plan draw(int countRoll,int ambushRoll,const int* deliveryRolls,int forcedClient,bool guard){
 Plan p;p.exists=true;p.count=2+countRoll;p.mask=0;p.trap=-1;
 if(guard){for(int i=0;i<p.count;++i)if(deliveryRolls[i]==0)p.mask|=1<<i;
 if(ambushRoll==0){p.trap=forcedClient%p.count;p.mask|=1<<p.trap;}}
 return p;
}
inline bool delivery(const Plan& p,int i){return i>=0&&i<p.count&&(p.mask&(1<<i))!=0;}
}
