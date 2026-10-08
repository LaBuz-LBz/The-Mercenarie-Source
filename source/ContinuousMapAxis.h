#pragma once
#include <cmath>
// Recover the continuous coordinate from the engine's integer sector mapping.
// Calibrated with read-only queries. No hardcoded world size or origin.
struct ContinuousMapAxis {
 double boundary,width;int sector,step;bool valid;
 ContinuousMapAxis():boundary(0),width(0),sector(0),step(0),valid(false){}
 template<class Query> bool calibrate(double origin,Query query){
  valid=false;sector=query(origin);double lo=origin,hi=origin+1;
  while(query(hi)==sector&&hi-origin<1048576)hi=origin+(hi-origin)*2;
  step=query(hi)-sector;if(step!=1&&step!=-1)return false;
  for(int i=0;i<30;++i){double mid=(lo+hi)/2;if(query(mid)==sector)lo=mid;else hi=mid;}
  boundary=hi;lo=boundary+1;hi=lo+1;if(query(lo)!=sector+step)return false;
  while(query(hi)==sector+step&&hi-boundary<1048576)hi=boundary+(hi-boundary)*2;
  if(query(hi)!=sector+2*step)return false;
  for(int i=0;i<30;++i){double mid=(lo+hi)/2;if(query(mid)==sector+step)lo=mid;else hi=mid;}
  width=hi-boundary;valid=width>2&&width<1048576;return valid;
 }
 double project(double coordinate)const{return sector+(step>0?1.0:0.0)+step*(coordinate-boundary)/width;}
};
