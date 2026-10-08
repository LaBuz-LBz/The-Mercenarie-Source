#include "Localization.h"
#include <cassert>
#include <iostream>
int main(){
 std::vector<std::string> names;
 const char* languages[]={"fr","en","pl","ru"};
 for(int l=0;l<4;++l){
  Loc::configure("Localization",languages[l]);assert(Loc::engine().language==languages[l]);
  std::string expected=Loc::text("common.continue");
  assert(Loc::legacy("CONTINUER",names)==expected);
  names.push_back("CONTINUER");assert(Loc::legacy("CONTINUER",names)=="CONTINUER");names.clear();
  for(int i=0;i<10000;++i){
   std::ostringstream value;value<<"Destination "<<i;
   std::string source=kLocalizationTemplates[0].value;
   for(size_t at=source.find('{');at!=std::string::npos;at=source.find('{')){size_t end=source.find('}',at);assert(end!=std::string::npos);source.replace(at,end-at+1,value.str());}
   Loc::legacy(source,names);
   assert(Loc::engine().cache.size()<=4096&&Loc::engine().rendered.size()<=4096);
  }
  const char* retained=Loc::text("common.continue");Loc::select("en");assert(std::string(retained)==expected);
 }
 std::cout<<"PASS: production cache FR/EN/PL/RU, names, switches, 40000 parameterized messages, bounded cache and stable pointers.\n";
}
