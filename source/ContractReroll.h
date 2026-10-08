#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <locale>
#include <algorithm>
#include <cmath>
namespace ContractReroll {
struct Charges {
    int remaining; double started,ends; unsigned int sequence;
    Charges():remaining(2),started(-1),ends(-1),sequence(0){}
    void refresh(double now){if(ends>=0&&now>=ends){remaining=2;started=ends=-1;}}
    bool consume(double now){refresh(now);if(remaining<=0||!(now>=0))return false;if(ends<0){started=now;ends=now+24;}--remaining;++sequence;return true;}
    int hoursLeft(double now)const{return ends<0?0:std::max(0,(int)ceil(ends-now));}
    std::string encode()const{std::ostringstream o;o.imbue(std::locale::classic());o<<remaining<<'|'<<std::setprecision(17)<<started<<'|'<<ends<<'|'<<sequence;return o.str();}
    bool decode(std::string raw){std::replace(raw.begin(),raw.end(),'|',' ');std::istringstream in(raw);in.imbue(std::locale::classic());Charges c;if(!(in>>c.remaining>>c.started>>c.ends>>c.sequence))return false;in>>std::ws;if(!in.eof()||c.remaining<0||c.remaining>2)return false;if(c.remaining==2){if(c.started!=-1||c.ends!=-1)return false;}else if(!(c.started>=0&&c.ends==c.started+24&&c.ends<1e9))return false;*this=c;return true;}
};
inline bool marked(const std::string& metadata){return metadata.find(";REROLL=1;")!=std::string::npos;}
inline int reward(int normal,bool rerolled){return rerolled?(int)((static_cast<long long>(normal)*3)/4):normal;}
inline bool bountyMarked(const std::string& id){return id.find(":reroll:")!=std::string::npos;}
inline int bountyReward(int normal,const std::string& id){return reward(normal,bountyMarked(id));}
// Preserve both the undiscounted quote and the unique offer revision in existing
// versioned metadata; existing V8 save/archive layouts remain readable.
inline void mark(std::string& metadata,int normal,unsigned int revision){
    if(marked(metadata))return;
    std::ostringstream s;s<<";REROLL=1;NORMAL="<<normal<<";REV="<<revision<<';';metadata+=s.str();
}
}
