#pragma once
#include "GuildOverviewLayout.h"
namespace GuildPages {
using GuildResponsive::Rect;
struct Layout {Rect page,heading,list,details,bottom,info;int font,pad,gap,row;};
inline Layout calculate(const GuildOverview::Layout& shell,bool offices){
 Layout l;l.font=shell.font;l.pad=shell.pad;l.gap=shell.gap;
 int p=l.pad,g=l.gap,w=shell.width-shell.glance.x-p,h=shell.height-shell.glance.y-p;
 l.page=Rect(shell.glance.x,shell.glance.y,w,h);
 int header=std::max(60,3*l.font+2*p),body=h-header-g;
 int top=offices?body*57/100:body*78/100;
 int left=(w-g)*(offices?59:67)/100;
 l.heading=Rect(0,0,w,header);l.list=Rect(0,header+g,left,top);
 l.details=Rect(left+g,header+g,w-left-g,top);
 int bottomY=header+g+top+g,bottomLeft=(w-g)*(offices?67:53)/100;
 l.bottom=Rect(0,bottomY,bottomLeft,h-bottomY);l.info=Rect(bottomLeft+g,bottomY,w-bottomLeft-g,h-bottomY);
 l.row=offices?std::max(4*l.font+14,(top-shell.title-3*p-20)/3-p/2):std::max(34,2*l.font+6);return l;
}
struct Bar {int x,width;};
inline Bar signedBar(float value,int width){float v=std::max(-100.f,std::min(100.f,value));int center=width/2;Bar b;b.width=int((v<0?-v:v)/100.f*(v<0?center:width-center)+.5f);b.x=v<0?center-b.width:center;return b;}
}
