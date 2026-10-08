#pragma once
#include "GuildOverviewLayout.h"
namespace GuildProgressLayout {
using GuildResponsive::Rect;
struct Layout {Rect title,current,bar,xp,next,carousel,panel,left,right,cards[5];int font,padding,icon;};
inline int first(int level){return GuildOverview::first(level);}
inline int move(int start,int delta,int level){return GuildOverview::move(start,delta,level);}
inline Layout calculate(int width,int height,float scale){
 Layout l={};l.font=std::max(12,(int)(18*scale));l.padding=std::max(5,(int)(10*scale));int p=l.padding,gap=std::max(6,(int)(10*scale));
 int inner=width-2*p,panelW=inner*27/100,carouselW=inner-panelW-gap,titleH=l.font+6,rowY=p+titleH,rowH=l.font+8;
 int y=rowY+rowH+gap,h=height-p-y;
 l.title=Rect(p,p,inner,titleH);l.current=Rect(p+carouselW*5/100,rowY,carouselW*14/100,rowH);
 l.bar=Rect(p+carouselW*20/100,rowY+3,carouselW*38/100,rowH-7);l.xp=Rect(p+carouselW*60/100,rowY,carouselW*20/100,rowH);l.next=Rect(p+carouselW*83/100,rowY,carouselW*17/100,rowH);
 l.carousel=Rect(p,y,carouselW,h);l.panel=Rect(p+carouselW+gap,rowY,panelW,height-p-rowY);
 int aw=std::max(24,(int)(38*scale)),ah=std::min(h,std::max(32,(int)(62*scale))),cg=std::max(5,(int)(10*scale)),ag=std::max(5,(int)(12*scale));
 int cw=(carouselW-2*aw-2*ag-4*cg)/5,used=2*aw+2*ag+4*cg+5*cw,x=(carouselW-used)/2;
 l.left=Rect(x,(h-ah)/2,aw,ah);for(int i=0;i<5;++i)l.cards[i]=Rect(x+aw+ag+i*(cw+cg),0,cw,h);
 l.right=Rect(l.cards[4].x+cw+ag,l.left.y,aw,ah);l.icon=std::min(std::max(18,(int)(30*scale)),h-2*l.font-8);return l;
}
}
