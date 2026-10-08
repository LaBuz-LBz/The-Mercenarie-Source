#pragma once
#include "GuildResponsiveLayout.h"
namespace GuildOverview {
using GuildResponsive::Rect;
struct Layout {
 int width,height,font,title,pad,gap; float scale;
 Rect header,summary,sidebar,brand,glance,activity,finance,map,recent,information,progression;
 int px(int n)const{return std::max(1,(int)(n*scale+0.5f));}
};
inline Layout calculate(int w,int h){
 Layout l={};l.width=w;l.height=h;l.scale=std::min(w/(1675.0f*.85f),h/900.0f);
 l.font=std::max(12,l.px(19));l.title=std::max(14,l.px(22));l.pad=std::max(6,l.px(12));l.gap=std::max(6,l.px(10));
 int p=l.pad,g=l.gap,headerH=h*125/1000,navW=std::max(170,l.px(230)),top=p+headerH+g;
 int mainX=p+navW+g,mainW=w-mainX-p,bodyH=h-top-p;
 int topH=bodyH*44/100,midH=bodyH*31/100,progH=bodyH-topH-midH-2*g;
 int minProg=4*l.font+64;if(progH<minProg){midH-=minProg-progH;progH=minProg;}
 int minRecent=l.title+10+std::max(28,l.px(41))+p+g+3*(2*(l.font+2)+3);
 if(midH<minRecent){topH-=minRecent-midH;midH=minRecent;}
 int glanceW=(mainW-g)*43/100,rightX=mainX+glanceW+g,rightW=mainW-glanceW-g;
 int headerW=(w-2*p-g)*53/100;
 l.header=Rect(p,p,headerW,headerH);l.summary=Rect(p+headerW+g,p,w-2*p-headerW-g,headerH);
 int buttonH=std::max(37,(bodyH*54/100-5*g)/6),navH=6*buttonH+5*g;
 l.sidebar=Rect(p,top,navW,navH);l.brand=Rect(p,top+navH+g,navW,bodyH-navH-g);
 l.glance=Rect(mainX,top,glanceW,topH);int activityH=(topH-g)*46/100;
 l.activity=Rect(rightX,top,rightW,activityH);l.finance=Rect(rightX,top+activityH+g,rightW,topH-activityH-g);
 int midY=top+topH+g,mapW=(mainW-2*g)*38/100,recentW=(mainW-2*g)*29/100;
 l.map=Rect(mainX,midY,mapW,midH);l.recent=Rect(mainX+mapW+g,midY,recentW,midH);l.information=Rect(mainX+mapW+recentW+2*g,midY,mainW-mapW-recentW-2*g,midH);
 l.progression=Rect(mainX,midY+midH+g,mainW,progH);return l;
}
inline int first(int level){return std::max(0,std::min(5,level-3));}
// Match the cumulative XP numerator and denominator printed beside every bar.
inline float xpFraction(int xp,int required,int level){return level>=10?1.0f:required<=0?0.0f:std::max(0.0f,std::min(1.0f,float(xp)/required));}
inline int fillPixels(int width,float fraction){return std::max(0,std::min(width,int(std::max(0.0f,std::min(1.0f,fraction))*std::max(0,width)+.5f)));}
inline int minStart(int level){return std::max(0,std::min(5,level-5));}
inline int maxStart(int level){return std::max(0,std::min(5,level-1));}
inline int move(int start,int delta,int level){return std::max(minStart(level),std::min(maxStart(level),start+delta));}
}
