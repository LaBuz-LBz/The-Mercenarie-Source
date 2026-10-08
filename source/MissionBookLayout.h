#pragma once
#include <algorithm>
namespace MissionBookLayout {
struct Rect { int x,y,w,h; Rect(int X=0,int Y=0,int W=0,int H=0):x(X),y(Y),w(W),h(H){} };
struct Layout {
 int x,y,w,h,font,rowH,portraitH,offerH; Rect left,center,right,map,detail,accept,roster,team,summary,status,delegate;
};
inline Layout calculate(int vw,int vh,bool hasTeam=true) {
 // Window growth is capped independently of typography. Never scale the tree.
 Layout l; l.w=std::min(vw-48,1620+std::min(180,std::max(0,vw-1920)/4));
 l.h=std::min(vh-40,900+std::min(100,std::max(0,vh-1080)/4));
 l.x=(vw-l.w)/2;l.y=(vh-l.h)/2;
 const int gap=12,top=62,bottom=12,body=l.h-top-bottom;
 const int usable=l.w-20-2*gap;
 int lw=usable*275/1000,rw=usable*275/1000,cw=usable-lw-rw;
 l.left=Rect(10,top,lw,body);l.center=Rect(10+lw+gap,top,cw,body);l.right=Rect(l.center.x+cw+gap,top,rw,body);
 l.offerH=std::min(94,(body-138)/7-6);
 l.font=l.w<1450?16:l.w<1760?19:20;l.rowH=40;l.portraitH=l.w<1450?44:50;
 int button=l.h<800?84:l.h<960?104:108;int mapH=(body-button-16)*430/706;
 l.map=Rect(l.center.x,top,cw,mapH);l.detail=Rect(l.center.x,top+mapH+8,cw,body-mapH-button-16);l.accept=Rect(l.center.x,top+body-button,cw,button);
 int header=100,teamH=hasTeam?l.portraitH+44:30,summaryH=4*(l.font+6)+8,statusH=36;
 int rosterH=body-header-teamH-summaryH-statusH-button-24;
 l.rowH=std::max(40,std::min(46,(rosterH-16)/8));
 l.roster=Rect(10,header,rw-20,rosterH);l.team=Rect(10,header+rosterH+6,rw-20,teamH);
 l.summary=Rect(10,l.team.y+teamH+4,rw-20,summaryH);l.status=Rect(10,body-button-statusH-6,rw-20,statusH);
 l.delegate=Rect(8,body-button,rw-16,button);
 return l;
}
}
