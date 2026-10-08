#pragma once
#include <string>
#include <sstream>
#include <locale>
namespace FrozenPriceTerms {
inline bool read(const std::string& metadata,int (&out)[8]){
 size_t pos=metadata.find(";PRICE=");if(pos==std::string::npos)return false;
 std::istringstream in(metadata.substr(pos+7));in.imbue(std::locale::classic());int values[8];char separator;
 for(int i=0;i<8;++i)if(!(in>>values[i]>>separator)||values[i]<0||values[i]>100000000||separator!=(i==7?';':','))return false;
 for(int i=0;i<8;++i)out[i]=values[i];return true;
}
template<class Reward>inline bool restore(const std::string& metadata,int frozenBase,Reward& r){
 int v[8];if(!read(metadata,v)||v[7]!=frozenBase)return false;
 r.base=v[0];r.distance=v[1];r.mission=v[2];r.environment=v[3];r.beforeDanger=v[0]+v[1]+v[2]+v[3];r.afterDanger=r.beforeDanger+v[4];r.afterRarity=r.afterDanger+v[5];r.guildHouseBonus=v[6];return true;
}
}
