#pragma once
// Read-only preview. Never supplies movement orders or replaces native navigation.
bool apRouteOnRoads=false;
void updateAutopilotRoute(bool fit){
 Character* actor=selectedAutopilotActor();const AutopilotDestinations::Place* destination=autopilotModel.find(autopilotModel.selected);
 if(!actor||actor->getHandle()!=autopilotActor||actor->isDead()||actor->isBeingCarried()||!destination){apRoutePoints.clear();apRouteTarget.clear();return;}
 const Ogre::Vector3 start=actor->getPosition();
 if(apRouteTarget!=destination->id||apRoutePoints.empty()||start.squaredDistance(apRouteOrigin)>62500||fit){
  static PricingRoads::Graph roads;static bool loaded=false;
  if(!loaded){loaded=true;try{std::ifstream file("mods/Guild Escort Contracts/pricing-roads.dat");roads.read(file);}catch(const std::exception&){}}
  apRouteOrigin=start;apRouteTarget=destination->id;
  RoutePrototype::Point a(start.x,start.y,start.z),b(destination->x,destination->y,destination->z);
  apRouteOnRoads=roads.route(a,b,apRoutePoints);
  if(!apRouteOnRoads){apRoutePoints.clear();apRoutePoints.push_back(a);apRoutePoints.push_back(b);}
 }
 if(!apRoutePoints.empty())apRoutePoints.front()=RoutePrototype::Point(start.x,start.y,start.z);
 if(fit&&!apRoutePoints.empty()){
  double left=2048,right=0,top=2048,bottom=0;
  for(size_t i=0;i<apRoutePoints.size();++i){const RoutePrototype::Point& p=apRoutePoints[i];iVector2 sector=ou->zoneMgr->getMapSector(Ogre::Vector3((float)p.x,(float)p.y,(float)p.z));double x=(sector.x+.5)*32,y=(sector.y+.5)*32;left=std::min(left,x);right=std::max(right,x);top=std::min(top,y);bottom=std::max(bottom,y);}
  double scale=std::min(std::max(1,autopilotView.width-140)/std::max(200.0,right-left),std::max(1,autopilotView.height-140)/std::max(200.0,bottom-top));
  autopilotView.crop=std::max(512.0,std::min(2048.0,std::min(autopilotView.width,autopilotView.height)/scale));autopilotView.center((left+right)/2,(top+bottom)/2);
 }
}
void drawAutopilotRoute(){
 if(!apRouteLayer)return;size_t used=0;Character* actor=selectedAutopilotActor();
 bool visible=actor&&actor->getHandle()==autopilotActor&&!actor->isDead()&&!actor->isBeingCarried()&&!autopilotModel.selected.empty()&&autopilotModel.selected==apRouteTarget;
 if(apRouteCaption)apRouteCaption->setCaption(visible?Loc::text(apRouteOnRoads?"autopilot.v9.route_preview":"autopilot.v9.route_direct"):"");
 if(visible)for(size_t i=1;i<apRoutePoints.size();++i){
  const RoutePrototype::Point& a=apRoutePoints[i-1];const RoutePrototype::Point& b=apRoutePoints[i];iVector2 sa=ou->zoneMgr->getMapSector(Ogre::Vector3((float)a.x,(float)a.y,(float)a.z)),sb=ou->zoneMgr->getMapSector(Ogre::Vector3((float)b.x,(float)b.y,(float)b.z));
  double x=autopilotView.px((sa.x+.5)*32),y=autopilotView.py((sa.y+.5)*32),dx=autopilotView.px((sb.x+.5)*32)-x,dy=autopilotView.py((sb.y+.5)*32)-y;int steps=std::max(1,(int)(std::sqrt(dx*dx+dy*dy)/7));
  for(int j=0;j<=steps&&used<768;++j){double t=j/(double)steps;int px=(int)(x+dx*t),py=(int)(y+dy*t);if(px<3||py<3||px>=autopilotView.width-3||py>=autopilotView.height-3)continue;
   if(used==apRouteDots.size()){MyGUI::Widget* dot=apRouteLayer->createWidget<MyGUI::Widget>("WhiteSkin",0,0,4,4,MyGUI::Align::Default);dot->setNeedMouseFocus(false);apRouteDots.push_back(dot);}
   MyGUI::Widget* dot=apRouteDots[used++];bool origin=i==1&&j==0;dot->setSize(origin?10:4,origin?10:4);dot->setPosition(px-(origin?5:2),py-(origin?5:2));dot->setColour(origin?MyGUI::Colour(.2f,1,.65f):apOrange);dot->setVisible(true);
  }
 }
 for(size_t i=used;i<apRouteDots.size();++i)apRouteDots[i]->setVisible(false);
}
