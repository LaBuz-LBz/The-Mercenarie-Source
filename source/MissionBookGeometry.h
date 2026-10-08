#pragma once
#include <algorithm>
#include <vector>
namespace MissionBookGeometry {
struct Rect { int x,y,w,h; Rect(int X=0,int Y=0,int W=0,int H=0):x(X),y(Y),w(W),h(H){} };
inline bool intersects(const Rect& a,const Rect& b){return a.x<b.x+b.w+4&&b.x<a.x+a.w+4&&a.y<b.y+b.h+4&&b.y<a.y+a.h+4;}
inline Rect bounded(Rect r,int w,int h){r.w=std::min(r.w,w);r.h=std::min(r.h,h);r.x=std::max(0,std::min(w-r.w,r.x));r.y=std::max(0,std::min(h-r.h,r.y));return r;}
// Move captions only. Deterministic search prefers the closest free row.
inline Rect label(Rect desired,int w,int h,const std::vector<Rect>& placed){
 Rect base=bounded(desired,w,h);
 for(int step=0;step<=h/(base.h+4)+1;++step)for(int side=0;side<(step?2:1);++side){
  Rect r=base;r.y+=(side?-1:1)*step*(base.h+4);r=bounded(r,w,h);
  bool free=true;for(size_t i=0;i<placed.size();++i)if(intersects(r,placed[i])){free=false;break;}
  if(free)return r;
 }
 // Dense multi-stop routes can exceed the viewport's label capacity.
 return Rect(0,0,0,0);
}
inline int cropHeight(int width,int vw,int vh){return std::max(1,(int)((double)width*vh/std::max(1,vw)+.999999));}
inline double scale(int cropWidth,int viewportWidth){return (double)viewportWidth/std::max(1,cropWidth);}
inline bool scroll(int content,int viewport){return content>viewport;}
}
