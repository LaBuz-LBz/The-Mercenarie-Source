#include "GuildBuildingTypes.h"
#include <cassert>
#include <iostream>
int main(){using namespace GuildBuildingTypes;
 assert(sizeof(types)/sizeof(types[0])==6);
 for(int level=0;level<=10;++level)for(int n=0;n<=4;++n){assert(limit(level)==(level<3?0:level==3?1:3));assert((state(types[0],level,n,false,true)==Available)==(level>=3&&n<limit(level)));assert(state(types[0],level,n,true,true)==Locked);assert(state(types[0],level,n,false,false)==Locked);for(int i=1;i<6;++i)assert(state(types[i],level,n,false,true)==ComingSoon);}
 assert(cleanName(" \t\n", "Default")=="Default");assert(cleanName("  A|B\nC#D  ","Default")=="ABCD");assert(cleanName(std::string(80,'A'),"D").size()==64);
 std::string unicode="\xC3\x89\xD0\x94\xC5\x81";assert(cleanName(unicode,"D")==unicode);std::string longName;for(int i=0;i<70;++i)longName+="\xC3\x89";assert(cleanName(longName,"D").size()==128);
 std::cout<<"PASS: 55 limit cases, 330 type states, occupied/invalid targets, names and UTF-8 cap\n";
}
