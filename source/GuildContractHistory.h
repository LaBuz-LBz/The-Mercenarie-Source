#pragma once
#include <string>
#include <algorithm>
#include <sstream>
#include "ContractSnapshot.h"
namespace GuildHistory {
struct Summary {std::string route,result;int status,bonus;double reward;Summary():status(3),bonus(-1),reward(-1){}};
inline bool has(const std::string& s,const char* word){return s.find(word)!=std::string::npos;}
inline Summary parse(const std::string& raw){Summary s;Snapshot snapshot;if(decode(raw,snapshot)){s.route=snapshot.origin+" -> "+snapshot.destination;s.status=snapshot.status==4?2:snapshot.status;s.bonus=snapshot.bonus;s.reward=(double)snapshot.reward;return s;}s.route=raw;size_t split=raw.rfind(" : ");if(split!=std::string::npos){s.route=raw.substr(0,split);s.result=raw.substr(split+3);}else{split=raw.rfind(" | ");if(split!=std::string::npos){s.route=raw.substr(0,split);s.result=raw.substr(split+3);}}
 if(has(raw,"ECHEC")||has(raw,"FAILURE")||has(raw,"FAILED")||has(raw,"ANNULE")||has(raw,"CANCELLED"))s.status=2;
 else if(has(raw,"REUSSI")||has(raw,"SUCCESS")||((has(raw,"COURRIER | ")||has(raw,"GUILDE | "))&&has(raw," Cats")))s.status=1;
 if(has(raw,"sans prime")||has(raw,"no bonus")||has(raw,"SANS PRIME")||has(raw,"WITHOUT BONUS"))s.bonus=0;
 else if(has(raw,"AVEC PRIME")||has(raw,"WITH BONUS"))s.bonus=1;
 else if(has(raw,"primes demandees")||has(raw,"bonuses requested"))s.bonus=2;
 size_t currency=raw.rfind(" Cats"),field=raw.rfind(" | ");
 if(currency!=std::string::npos&&field!=std::string::npos&&currency>field+3){std::string amount=raw.substr(field+3,currency-field-3);bool valid=!amount.empty();std::string digits;for(size_t i=0;i<amount.size();++i){if(amount[i]>='0'&&amount[i]<='9')digits+=amount[i];else if(amount[i]!=' ')valid=false;}if(valid&&!digits.empty()){std::istringstream value(digits);value>>s.reward;}}
 return s;}
}
