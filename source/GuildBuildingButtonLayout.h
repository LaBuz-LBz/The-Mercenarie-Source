#pragma once
#include <algorithm>

// Coordinates come from the live native information and action panels.
struct GuildBuildingButtonLayout
{
    int x,y,w,h;
    GuildBuildingButtonLayout(int left,int width,int contentBottom,int panelBottom,int desiredHeight)
    {
        const int gap=std::max(1,panelBottom-contentBottom);
        const int margin=std::max(1,std::min(width/40,gap/6));
        w=std::max(1,width-2*margin);
        h=std::max(1,std::min(desiredHeight,gap-2*margin));
        x=left+(width-w)/2;
        y=contentBottom+(gap-h)/2;
    }
};
