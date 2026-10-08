#pragma once
#include <algorithm>
namespace MainMenuBackgroundRules {
static const int ImageWidth=2172, ImageHeight=724;
struct Crop {float left,top,right,bottom;};
inline Crop cover(int width,int height){
    Crop p={0,0,1,1};if(width<=0||height<=0)return p;
    const float ratio=float(width)/height,source=float(ImageWidth)/ImageHeight;
    if(ratio<source){float span=ratio/source;p.left=std::max(0.f,std::min(1.f-span,0.75f-0.72f*span));p.right=p.left+span;}
    else {p.bottom=source/ratio;}
    return p;
}
}
