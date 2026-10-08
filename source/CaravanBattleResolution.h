#pragma once
namespace CaravanBattleResolution {
struct State {bool anchored,ended;double x,y,z,quiet;State():anchored(false),ended(false),x(0),y(0),z(0),quiet(0){} };
inline State read(const std::string& m){State s;size_t p=m.find(";HOUSERESOLVE1=");if(p==std::string::npos)return s;p+=15;
    std::istringstream in(m.substr(p,m.find(';',p)-p));if(!(in>>s.anchored>>s.ended>>s.x>>s.y>>s.z>>s.quiet)||!(s.quiet>=0&&s.quiet<=30)||!(s.x>=-10000000&&s.x<=10000000)||!(s.y>=-10000000&&s.y<=10000000)||!(s.z>=-10000000&&s.z<=10000000))throw std::runtime_error("invalid house resolution");return s;}
inline void write(std::string& m,const State& s){size_t p=m.find(";HOUSERESOLVE1=");if(p!=std::string::npos){size_t e=m.find(';',p+1);m.erase(p,e==std::string::npos?std::string::npos:e-p+1);}std::ostringstream out;out<<std::setprecision(15)<<";HOUSERESOLVE1="<<s.anchored<<' '<<s.ended<<' '<<s.x<<' '<<s.y<<' '<<s.z<<' '<<s.quiet<<';';m+=out.str();}
}
