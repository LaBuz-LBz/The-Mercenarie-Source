#include "MissionBookGeometry.h"
#include "MissionBookLayout.h"
#include "MissionBookContractsModel.h"
#include "v5/BountySave.h"
#include "DelegatedMissionTiming.h"
#include <cassert>
#include <cmath>
#include <iostream>
struct RandomAlpha {unsigned int operator()(){return 37;}};
int main(){
 using namespace MercenarieV5;
 using namespace MissionBookContracts;
 BountyWorldState world;RandomAlpha random;ActorIdentity officer;officer.index=11;officer.serial=7;
 world.boards["officer-a"].refresh(10,"officer-a",officer,HolyNation,random);
 officer.index=12;world.boards["officer-b"].refresh(10,"officer-b",officer,HolyNation,random);
 for(std::map<std::string,BountyBoard>::iterator b=world.boards.begin();b!=world.boards.end();++b)for(size_t i=0;i<b->second.offers.size();++i){b->second.offers[i].areaId="area";b->second.offers[i].targetName="target";}
 std::vector<Offer> pool;for(int i=0;i<5;++i){std::ostringstream id;id<<"classic-"<<i;pool.push_back(Offer(id.str(),categoryFor(i%3,false)));}
 std::set<std::string> identities;
 for(std::map<std::string,BountyBoard>::iterator b=world.boards.begin();b!=world.boards.end();++b)for(size_t i=0;i<b->second.offers.size();++i){assert(identities.insert(b->second.offers[i].id).second);pool.push_back(Offer(b->second.offers[i].id,Security));}
 assert(filtered(pool,All).size()==11&&filtered(pool,Security).size()==6);
 pool.push_back(Offer("future-illegal",Illegal));pool.push_back(Offer("future",(Category)9));assert(filtered(pool,All).size()==13);
 std::vector<size_t> all=filtered(pool,All);assert(page(all,0).size()==7&&page(all,1).size()==6);assert(clampPage(999,13)==1&&clampPage(-1,13)==0&&pageCount(7,7)==1);
 BountyWorldState restored;restored.load(world.save());assert(!restored.suspended&&restored.boards["officer-a"].offers[0].id==world.boards["officer-a"].offers[0].id);
 // Both frontends and delegation execute BountyBoard::consume on the same registry.
 for(int mode=0;mode<3;++mode){BountyWorldState scenario;scenario.load(world.save());BountyOffer chosen=scenario.boards["officer-a"].offers[1];
  if(mode<2)assert(scenario.contract.accept(chosen));
  assert(scenario.boards["officer-a"].consume(chosen.id,11));assert(!scenario.boards["officer-a"].consume(chosen.id,11));
  BountyWorldState loaded;loaded.load(scenario.save());assert(!loaded.suspended&&loaded.boards["officer-a"].offers.size()==2&&loaded.boards["officer-b"].offers.size()==3);
  for(size_t i=0;i<loaded.boards["officer-a"].offers.size();++i)assert(loaded.boards["officer-a"].offers[i].id!=chosen.id);
  assert(!loaded.boards["officer-a"].consume(chosen.id,12));assert(loaded.contract.occupied()==(mode<2));
  unsigned int rotation=loaded.boards["officer-a"].rotation;loaded.boards["officer-a"].refresh(12,"officer-a",officer,HolyNation,random);assert(loaded.boards["officer-a"].rotation==rotation);
  double deadline=loaded.boards["officer-a"].nextRefreshHour;assert(!loaded.boards["officer-a"].consume(loaded.boards["officer-a"].offers[0].id,deadline));
  loaded.boards["officer-a"].refresh(deadline,"officer-a",officer,HolyNation,random);for(size_t i=0;i<3;++i)assert(loaded.boards["officer-a"].offers[i].id!=chosen.id);
 }
 DelegatedMissionTiming::State timing=DelegatedMissionTiming::start(DelegatedMissionTiming::ActivityBountyHunt,80,10);assert(timing.activityHours==48);
 const int resolutions[][2]={{1280,720},{1920,1080},{2560,1440},{3440,1440},{3840,2160}};
 for(int r=0;r<5;++r){MissionBookLayout::Layout layout=MissionBookLayout::calculate(resolutions[r][0],resolutions[r][1]);int w=layout.map.w-8,h=layout.map.h-66;
  for(int crop=512;crop<=2048;crop+=64){double scale=MissionBookGeometry::scale(crop,w);double sampleH=h/scale;assert(MissionBookGeometry::cropHeight(crop,w,h)>=sampleH-1e-6);assert(fabs(w/(double)crop-h/sampleH)<1e-9);
   double x=300,y=200,dx=37,dy=-18;double screenX=x*scale,screenY=y*scale;assert(fabs((x+dx/scale)*scale-screenX-dx)<1e-9&&fabs((y+dy/scale)*scale-screenY-dy)<1e-9);
  }
  std::vector<MissionBookGeometry::Rect> labels;MissionBookGeometry::Rect first=MissionBookGeometry::label(MissionBookGeometry::Rect(w/2-140,h-20,280,30),w,h,labels);labels.push_back(first);
  MissionBookGeometry::Rect second=MissionBookGeometry::label(first,w,h,labels);assert(second.w>0&&!MissionBookGeometry::intersects(first,second));assert(second.x>=0&&second.y>=0&&second.x+second.w<=w&&second.y+second.h<=h);
  assert(!MissionBookGeometry::scroll(layout.detail.h-4,layout.detail.h-4)&&MissionBookGeometry::scroll(layout.detail.h+20,layout.detail.h-4));
  std::cout<<"PASS alpha geometry "<<resolutions[r][0]<<"x"<<resolutions[r][1]<<"\n";
 }
 std::cout<<"PASS shared bounty identity/consumption/save-load, multi-source categories, pages, expiry, rotation, 48h, isotropic crop and label collisions\n";
}
