#include "MissionBookLayout.h"
#include <cassert>
#include <fstream>
#include <iostream>
using namespace MissionBookLayout;
bool inside(const Rect& a,int w,int h){return a.x>=0&&a.y>=0&&a.w>0&&a.h>0&&a.x+a.w<=w&&a.y+a.h<=h;}
bool before(const Rect& a,const Rect& b){return a.y+a.h<=b.y;}
void json(std::ostream& o,const char* name,const Rect& r,bool comma=true){o<<"\""<<name<<"\":["<<r.x<<","<<r.y<<","<<r.w<<","<<r.h<<"]"<<(comma?",":"");}
int main(){
 // Validate the reference first, before exercising responsive variants.
 Layout reference=calculate(1920,1080);
 assert(reference.w==1620&&reference.h==900&&reference.x==150&&reference.y==90);
 assert(reference.font>=18&&reference.offerH==92&&reference.map.h>=420&&reference.map.h<=440);
 assert(reference.detail.h>=260&&reference.detail.h<=280&&reference.accept.h>=100);
 std::cout<<"PASS reference 1920x1080: centered 1620x900, readable type, larger map and compact details\n";
 const int sizes[][2]={{1280,720},{1920,1080},{2560,1440},{3440,1440},{3840,2160}};
 std::ofstream out("mission-book-layout-dump.json");out<<"[";
 for(int i=0;i<5;++i){Layout l=calculate(sizes[i][0],sizes[i][1]);
  assert(l.x==(sizes[i][0]-l.w)/2&&l.y==(sizes[i][1]-l.h)/2);
  assert(l.x>=24&&l.y>=20&&l.w<=1800&&l.h<=1000);
  if(i>0){assert(l.w*100/sizes[i][0]<=85&&l.h*100/sizes[i][1]<=85);}
  assert(inside(l.left,l.w,l.h)&&inside(l.center,l.w,l.h)&&inside(l.right,l.w,l.h));
  assert(l.left.x+l.left.w<l.center.x&&l.center.x+l.center.w<l.right.x);
  assert(l.center.w>l.left.w&&l.center.w>l.right.w);
  assert(inside(l.map,l.w,l.h)&&inside(l.detail,l.w,l.h)&&inside(l.accept,l.w,l.h));
  assert(before(l.map,l.detail)&&before(l.detail,l.accept));
  assert(inside(l.roster,l.right.w,l.right.h)&&inside(l.team,l.right.w,l.right.h));
  assert(inside(l.summary,l.right.w,l.right.h)&&inside(l.status,l.right.w,l.right.h)&&inside(l.delegate,l.right.w,l.right.h));
  assert(before(l.roster,l.team)&&before(l.team,l.summary)&&before(l.summary,l.status)&&before(l.status,l.delegate));
  assert(l.team.y+28+l.portraitH+16<=l.summary.y);
  assert(4*(l.portraitH+10)<=l.team.w-20); // Four selected portraits fit at every supported resolution.
  int width=l.roster.w-22,statW=std::max(37,width*14/100),statsX=width-28-3*statW-8;
  assert(statsX-l.rowH-8>=32);assert(statsX+3*statW<=width-28);
  assert(l.font>=16&&l.font<=20&&l.roster.h>=l.rowH);
  assert(l.offerH>=60&&l.offerH<=94&&54+7*(l.offerH+6)-6<=l.left.h-84);
  assert(l.rowH>=40&&l.rowH<=46);
  // Book action skin title and separate subtitle remain disjoint and contained.
  const Rect actions[]={l.accept,l.delegate};
  for(int a=0;a<2;++a){int bh=actions[a].h,bw=actions[a].w;
   Rect title(70,19+(bh-104)/2,bw-82,32),subtitle(70,bh/2+4,bw-82,30);
   assert(inside(title,bw,bh)&&inside(subtitle,bw,bh)&&before(title,subtitle));
  }

  Layout empty=calculate(sizes[i][0],sizes[i][1],false);
  assert(empty.team.h==30&&empty.roster.h>l.roster.h);
  assert(empty.summary.y==l.summary.y&&empty.delegate.y==l.delegate.y);
  assert(before(empty.roster,empty.team)&&before(empty.team,empty.summary));
  assert(empty.w==l.w&&empty.h==l.h&&empty.map.h==l.map.h);
  if(i==1){assert(l.left.w>=425&&l.left.w<=445);assert(l.right.w==l.left.w);assert(l.center.w>=700);}
  if(i)out<<",";out<<"{\"viewport\":["<<sizes[i][0]<<","<<sizes[i][1]<<"],\"window\":["<<l.x<<","<<l.y<<","<<l.w<<","<<l.h<<"],\"font\":"<<l.font<<",\"offerH\":"<<l.offerH<<",\"rowH\":"<<l.rowH<<",";
  json(out,"emptyRoster",empty.roster);json(out,"emptyTeam",empty.team);
  json(out,"left",l.left);json(out,"center",l.center);json(out,"right",l.right);json(out,"map",l.map);json(out,"detail",l.detail);json(out,"accept",l.accept);json(out,"roster",l.roster);json(out,"team",l.team);json(out,"summary",l.summary);json(out,"status",l.status);json(out,"delegate",l.delegate,false);out<<"}";
  std::cout<<"PASS "<<sizes[i][0]<<"x"<<sizes[i][1]<<": columns, containment, fixed actions, scroll areas, stat columns\n";
 }
 out<<"]\n";
}
