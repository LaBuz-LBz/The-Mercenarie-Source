#include "GuildResponsiveLayout.h"
#include "GuildOverviewLayout.h"
#include "GuildMapViewport.h"
#include "GuildContractHistory.h"
#include "GuildProgressLayout.h"
#include "GuildOverviewDataModel.h"
#include <cassert>
#include <iostream>
using namespace GuildResponsive;
bool intersects(const Rect& a,const Rect& b){return a.x<b.x+b.w&&a.x+a.w>b.x&&a.y<b.y+b.h&&a.y+a.h>b.y;}
int main(){
 for(int repeat=0;repeat<30;++repeat)for(int zoom=512;zoom<=2048;zoom+=64)for(int pan=-5000;pan<=5000;pan+=250){GuildMapViewport::Crop c=GuildMapViewport::clamp(pan,-pan,zoom,220,180);assert(c.x>=0&&c.y>=0&&c.x+c.w<=2048&&c.y+c.h<=2048&&c.w>0&&c.h>0);assert(c.w/c.w*c.h/c.h==1);}
 assert(GuildHistory::parse("A -> B : REUSSI - sans prime").bonus==0);assert(GuildHistory::parse("A -> B : SUCCESS - WITH BONUS").bonus==1);assert(GuildHistory::parse("A -> B : REUSSI - primes demandees").bonus==2);assert(GuildHistory::parse("COURRIER | Heng | ANNULE").status==2);
 const int views[][2]={{1280,720},{1366,768},{1920,1080},{2560,1440},{3440,1440},{3840,2160}};
 for(int i=0;i<6;++i){Metrics m=calculate(views[i][0],views[i][1]);assert(m.windowW<=m.viewportW&&m.windowH<=m.viewportH);assert(m.windowW>=m.viewportW*65/100&&m.windowW<=std::max(1060,m.viewportW*78/100));
 for(int inset=0;inset<=40;inset+=20){GuildOverview::Layout o=GuildOverview::calculate(m.windowW-16,m.windowH-24-inset);Rect panels[]={o.header,o.summary,o.sidebar,o.brand,o.glance,o.activity,o.finance,o.map,o.recent,o.information,o.progression};
 for(int a=0;a<11;++a){assert(panels[a].x>=0&&panels[a].y>=0&&panels[a].w>0&&panels[a].h>0);assert(panels[a].x+panels[a].w<=o.width&&panels[a].y+panels[a].h<=o.height);for(int b=a+1;b<11;++b)assert(!intersects(panels[a],panels[b]));}
 GuildProgressLayout::Layout g=GuildProgressLayout::calculate(o.progression.w,o.progression.h,o.scale);assert(g.carousel.x+g.carousel.w<g.panel.x);assert(g.panel.y+g.panel.h<=o.progression.h);assert(g.cards[0].h>=g.icon+2*g.font+8);assert(g.icon>=18);assert(g.right.x+g.right.w<=g.carousel.w);assert(g.cards[4].x+g.cards[4].w<g.right.x);assert(g.left.x+g.left.w<g.cards[0].x);
 for(int c=1;c<5;++c){assert(g.cards[c].w==g.cards[0].w&&g.cards[c].h==g.cards[0].h&&g.cards[c].y==0);assert(g.cards[c].x-g.cards[c-1].x==g.cards[1].x-g.cards[0].x);}
 for(int level=1;level<=10;++level){int start=GuildOverview::first(level);assert(start<=level-1&&start+5>level-1);for(int delta=-1;delta<=1;delta+=2)for(int j=0;j<15;++j){start=GuildOverview::move(start,delta,level);assert(start>=0&&start<=5&&start<=level-1&&start+5>level-1);}}
 }
 }
 const int percent[]={0,25,50,70,75,99,100};
 for(int i=0;i<7;++i){float f=GuildOverview::xpFraction(percent[i]*55,5500,5);assert(std::abs(f-percent[i]/100.0f)<.0001f);for(int w=1;w<1500;++w){int n=GuildOverview::fillPixels(w,f);assert(n>=0&&n<=w);assert(std::abs(n-w*f)<=.501f);}}
 assert(GuildOverview::xpFraction(3850,5500,5)==.7f);assert(GuildOverview::xpFraction(-1,5500,5)==0);assert(GuildOverview::xpFraction(6000,5500,5)==1);assert(GuildOverview::xpFraction(0,0,5)==0);assert(GuildOverview::xpFraction(42000,42000,10)==1);
 Finance::Journal journal;journal.record(125,1000,200,Finance::Contract,"finance.contract");journal.record(-30,970,176,Finance::Investment,"finance.investment");journal.record(999,1969,32,Finance::Contract,"finance.contract");journal.record(999,2968,240,Finance::Contract,"finance.contract");GuildOverview::Finances money=GuildOverview::finances(journal,200);assert(money.income==125&&money.expense==30&&money.days[0][6]==125&&money.days[1][5]==-30&&money.days[2][5]==-30);assert(!GuildOverview::finances(Finance::Journal(),200).available);
 std::cout<<"PASS: new overview panel separation, typography space, five equal cards, current level visibility, 5 resolutions, 7-day real ledger projection.\n";
}
