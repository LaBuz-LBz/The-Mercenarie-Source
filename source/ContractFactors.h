#pragma once
#include <string>
#include <sstream>
#include <locale>
namespace ContractFactors {
struct Quote {
 int fragility,unknown,count,unitValue,cargoPercent;std::string item;
 Quote():fragility(0),unknown(0),count(0),unitValue(0),cargoPercent(0),item("-"){}
 int cargoValue()const{return count*unitValue;}
 bool valid()const{return fragility>=0&&fragility<=15&&unknown>=0&&unknown<=8&&count>=0&&count<=64&&unitValue>=0&&unitValue<=100000&&cargoPercent>=0&&cargoPercent<=20&&!item.empty()&&item.size()<200&&item.find_first_of(",;|\r\n") == std::string::npos&&(count?item!="-":item=="-")&&(count||(!unitValue&&!cargoPercent));}
};
inline int fragility(double defence,double toughness,double athletics){
 // Estimated protection need from template skills, not current wounds.
 double resilience=defence*.45+toughness*.40+athletics*.15;
 return resilience<10?15:resilience<25?10:resilience<40?5:0;
}
inline int combatFragility(double combat){return combat<10?15:combat<25?10:combat<40?5:0;}
inline std::string encode(const Quote& q){if(!q.valid())return "";std::ostringstream s;s.imbue(std::locale::classic());s<<";FACT1="<<q.fragility<<','<<q.unknown<<','<<q.item<<','<<q.count<<','<<q.unitValue<<','<<q.cargoPercent<<';';return s.str();}
inline bool decode(const std::string& metadata,Quote& q){
 size_t pos=metadata.find(";FACT1=");if(pos==std::string::npos)return false;size_t end=metadata.find(';',pos+7);if(end==std::string::npos)return false;
 std::istringstream s(metadata.substr(pos+7,end-pos-7));s.imbue(std::locale::classic());Quote p;char a,b,c,d;
 if(!(s>>p.fragility>>a>>p.unknown>>b)||a!=','||b!=','||!std::getline(s,p.item,','))return false;
 if(!(s>>p.count>>c>>p.unitValue>>d>>p.cargoPercent)||c!=','||d!=','||s.peek()!=EOF||!p.valid())return false;q=p;return true;
}
inline int supplement(int distanceBase,int percent){return distanceBase*percent/100;}
}
