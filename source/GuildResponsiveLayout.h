#pragma once
#include <algorithm>

namespace GuildResponsive {
enum Breakpoint { Compact, Standard, Wide, Large4K };
struct Rect { int x,y,w,h; Rect(int X=0,int Y=0,int W=0,int H=0):x(X),y(Y),w(W),h(H){} };
struct Metrics {
    int viewportW,viewportH,windowW,windowH,designW,designH;
    int outerMargin,gap,minButton,minFont,markerSize,levelColumns,levelRows;
    float scale;
    Breakpoint breakpoint;
    Rect header,navigation,content,popupLayer,tooltipLayer;
    bool compact()const{return breakpoint==Compact;}
};
struct OverviewGeometry {
    Rect map,activity,finance,recent,information,progression,nextTier;
    int levelCardW,levelCardH;
};
inline Metrics calculate(int vw,int vh){
    Metrics m={};m.viewportW=std::max(640,vw);m.viewportH=std::max(480,vh);
    const float aspect=float(m.viewportW)/std::max(1,m.viewportH);
    m.breakpoint=(m.viewportW>=3600&&m.viewportH>=1800)?Large4K:(aspect>=2.15f&&m.viewportW>=2560)?Wide:(m.viewportW>=1800&&m.viewportH>=900)?Standard:Compact;
    // Keep a visible ring of the game around the management window.  This is
    // deliberately a viewport ratio, not a second global scale pass.
    int capW=m.breakpoint==Large4K?2995:2683,capH=m.breakpoint==Large4K?1728:1536;
    m.windowW=std::min(m.viewportW-32,std::max(1060,std::min(capW,m.viewportW*78/100)*85/100));m.windowH=std::min(capH,m.viewportH*80/100);
    m.designH=1440;
    // Preserve one uniform scale while matching the real client aspect ratio.
    // A fixed 2560 canvas left an unused black strip on ultrawide viewports.
    m.designW=std::max(2560,(m.windowW*m.designH+m.windowH/2)/std::max(1,m.windowH));
    m.scale=std::min(m.windowW/float(m.designW),m.windowH/float(m.designH));
    m.outerMargin=m.breakpoint==Compact?16:24;m.gap=m.breakpoint==Compact?12:16;
    m.minButton=m.breakpoint==Compact?37:38;m.minFont=m.breakpoint==Compact?12:14;
    m.markerSize=std::max(16,(int)(20*m.scale+0.5f));
    m.levelColumns=(m.breakpoint==Wide||m.breakpoint==Large4K)?10:5;m.levelRows=10/m.levelColumns;
    int nav=std::min(500,std::max(300,m.designW*15/100)),headerH=m.breakpoint==Compact?156:180;
    m.header=Rect(nav+m.outerMargin+m.gap,m.outerMargin,m.designW-nav-m.outerMargin*2-m.gap,headerH);
    m.navigation=Rect(m.outerMargin,m.outerMargin,nav,m.designH-m.outerMargin*2);
    m.content=Rect(m.header.x,m.header.y+headerH+m.gap,m.header.w,m.designH-m.header.y-headerH-m.gap-m.outerMargin);
    m.popupLayer=Rect(0,0,m.designW,m.designH);m.tooltipLayer=m.popupLayer;return m;
}
inline Rect clampOverlay(const Rect& desired,const Rect& bounds){
    Rect r=desired;r.w=std::min(r.w,bounds.w);r.h=std::min(r.h,bounds.h);
    r.x=std::max(bounds.x,std::min(r.x,bounds.x+bounds.w-r.w));r.y=std::max(bounds.y,std::min(r.y,bounds.y+bounds.h-r.h));return r;
}
inline OverviewGeometry overview(const Metrics& m){
    OverviewGeometry g={};int mainX=m.outerMargin+m.navigation.w+m.gap+2,totalW=m.designW-mainX-m.outerMargin;
    int leftW=totalW*57/100,rightStart=leftW+16,rightArea=totalW-rightStart-8;
    g.activity=Rect(mainX+rightStart,202,rightArea,184);g.finance=Rect(mainX+rightStart,398,rightArea,184);
    int mapW=std::min(totalW*40/100,900),recentW=(totalW-mapW-30)*46/100,infoW=totalW-mapW-recentW-30;
    g.map=Rect(mainX+8,598,mapW,390);g.recent=Rect(mainX+18+mapW,598,recentW,390);g.information=Rect(mainX+28+mapW+recentW,598,infoW,390);
    int nextW=520,cardsW=totalW-nextW-34,progH=(m.designH-38)-1002-8;g.progression=Rect(mainX+8,1002,totalW-16,progH);
    g.levelCardW=cardsW/m.levelColumns-8;g.levelCardH=(progH-58)/m.levelRows-6;
    g.nextTier=Rect(mainX+cardsW+24,752,nextW-34,progH-60);return g;
}
struct InvestmentGeometry { Rect surface,left,middle,right,information; int trackW,confirmW,confirmH; };
inline InvestmentGeometry investment(const Metrics&){InvestmentGeometry g={};g.surface=Rect(34,232,2492,956);g.left=Rect(42,430,720,520);g.middle=Rect(770,430,980,520);g.right=Rect(1758,430,720,520);g.information=Rect(42,968,2476,200);g.trackW=626;g.confirmW=836;g.confirmH=70;return g;}
inline int scaledFill(int displayedTrackWidth,float fraction){return std::max(1,(int)(std::max(0.0f,std::min(1.0f,fraction))*displayedTrackWidth+0.5f));}
}
