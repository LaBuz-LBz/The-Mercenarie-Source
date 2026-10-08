#pragma once
#include <algorithm>
namespace GuildMapViewport {
struct Crop {int x,y,w,h;};
inline Crop clamp(int x,int y,int width,int viewW,int viewH){Crop c;c.w=std::max(512,std::min(2048,width));c.h=std::max(1,std::min(2048,int((long long)c.w*std::max(1,viewH)/std::max(1,viewW))));c.x=std::max(0,std::min(2048-c.w,x));c.y=std::max(0,std::min(2048-c.h,y));return c;}
}
