#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <string>
#include <sstream>
#include <map>
#include <cstdio>
#include <iomanip>
#include "SaveBuild.generated.h"
// Temporary candidate instrumentation. No frame polling, no persistent pointers.
// Define MERCENARIE_PERF_DISABLED for a production build without instrumentation.
namespace MercenariePerf {
inline double clockMs(){LARGE_INTEGER t,f;QueryPerformanceCounter(&t);QueryPerformanceFrequency(&f);return 1000.0*(double)t.QuadPart/(double)f.QuadPart;}
struct Data {
 const char* kind;const char* operation;std::string incident;double start,last,mod,native;bool nativeMode;
 std::map<std::string,double> phases;std::map<std::string,unsigned long> calls;
 Data(const char* k,const char* o):kind(k),operation(o),start(clockMs()),last(start),mod(0),native(0),nativeMode(false){}
 void tick(){double n=clockMs();if(nativeMode)native+=n-last;else mod+=n-last;last=n;}
};
__declspec(thread) static Data* active=0;
struct Mode {
 Data* data;bool previous;
 explicit Mode(bool native):data(active),previous(false){if(data){data->tick();previous=data->nativeMode;data->nativeMode=native;}}
 ~Mode(){if(data){data->tick();data->nativeMode=previous;}}
};
struct Event {
 Data* data;bool owner,previous;
 Event(const char* kind,const char* operation,bool enabled=true):data(0),owner(false),previous(false){
#ifndef MERCENARIE_PERF_DISABLED
  try{if(active){data=active;data->tick();previous=data->nativeMode;data->nativeMode=false;}
  else if(enabled){data=new Data(kind,operation);active=data;owner=true;}}catch(...){}
#endif
 }
 void id(const std::string& value){if(data&&owner)data->incident=value;}
 ~Event(){if(!data)return;data->tick();if(!owner){data->nativeMode=previous;return;}active=0;
  try{std::ostringstream s;s<<std::fixed<<std::setprecision(3);const std::string prefix=std::string("[PERF][")+data->kind+"] ";
   s<<prefix<<"begin build="<<MercenarieSaveBuild<<" operation="<<data->operation<<" incident="<<data->incident<<" pid="<<GetCurrentProcessId()<<" tid="<<GetCurrentThreadId()<<" start_qpc_ms="<<data->start<<"\n";
   for(std::map<std::string,double>::const_iterator i=data->phases.begin();i!=data->phases.end();++i)s<<prefix<<i->first<<": "<<i->second<<" ms calls="<<data->calls[i->first]<<"\n";
   s<<prefix<<"native exclusive: "<<data->native<<" ms\n"<<prefix<<"total mod-side: "<<data->mod<<" ms\n"<<prefix<<"end (nested phase timings overlap; asynchronous wait excluded)\n";
   FILE* f=0;if(fopen_s(&f,"mods/Guild Escort Contracts/performance-audit.log","ab")==0&&f){std::string bytes=s.str();fwrite(bytes.data(),1,bytes.size(),f);fclose(f);}
  }catch(...){}delete data;
 }
};
struct Phase {
 Data* data;const char* name;double begin;
 explicit Phase(const char* n):data(active),name(n),begin(0){if(data){data->tick();begin=data->mod;}}
 ~Phase(){if(data){data->tick();try{data->phases[name]+=data->mod-begin;++data->calls[name];}catch(...){}}}
};
}
