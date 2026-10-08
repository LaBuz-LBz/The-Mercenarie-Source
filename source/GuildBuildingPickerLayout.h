#pragma once
#include <algorithm>
namespace GuildBuildingPicker {
struct Layout {
 int w,h;float scale;
 Layout(int vw,int vh){scale=std::min(1.0f,std::min((vw-32)/1400.f,(vh-32)/1080.f));w=px(1400);h=px(1080);}
 int px(int v)const{return std::max(1,int(v*scale+.5f));}
 int font(int v)const{return std::max(14,std::min(38,(px(v)/2)*2));}
};
}
