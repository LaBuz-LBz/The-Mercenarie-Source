#pragma once
// Reuse the existing map circles/dots and road graph. Keep an identity, never
// a pointer/index into mailContracts across save loads or cancellations.
std::string selectedMailMapId; // Retained for contract-list navigation/save compatibility.
struct MailMapRouteCache {std::string key;std::vector<RoutePrototype::Point> points;};
std::map<std::string,MailMapRouteCache> mailMapRoutes;
void resetMailMapSelection(){selectedMailMapId.clear();mailMapRoutes.clear();}
void updateSelectedMailMap(MapScreen* map){
 if(!map||!map->mapImage)return;
 restoreMailRouteSnapshots(); // Legacy town objects may arrive after the sidecar.
 MyGUI::Widget* container=map->mapImage->findWidget("MercenarieMailRoute");
 if(!container){container=map->mapImage->createWidget<MyGUI::Widget>("",0,0,map->mapImage->getWidth(),map->mapImage->getHeight(),MyGUI::Align::Stretch,"MercenarieMailRoute");container->setNeedMouseFocus(false);}
 std::set<std::string> visible;
 for(size_t i=0;i<mailContracts.size();++i){
 const MailContracts::Contract* contract=&mailContracts[i];
 if(contract->status!=MailContracts::MailActive||contract->delegated||contract->routePoints.size()!=(contract->steps.size()+1)*3)continue;
 size_t targetIndex=contract->steps.size();
 for(size_t j=0;j<contract->steps.size();++j)if(!contract->steps[j].delivered){targetIndex=j;break;}
 if(targetIndex==contract->steps.size())continue;
 const std::vector<double>& points=contract->routePoints;size_t offset=(targetIndex+1)*3;
 Ogre::Vector3 origin((float)points[0],(float)points[1],(float)points[2]),target((float)points[offset],(float)points[offset+1],(float)points[offset+2]);
 if(!mailRoutePositionKnown(origin)||!mailRoutePositionKnown(target))continue;
 const std::string name="MailContract:"+contract->contractId;
 visible.insert(name);
 MyGUI::Widget* root=container->findWidget(name);
 if(!root){root=container->createWidget<MyGUI::Widget>("",0,0,container->getWidth(),container->getHeight(),MyGUI::Align::Stretch,name);root->setNeedMouseFocus(false);}
 MailMapRouteCache& cache=mailMapRoutes[contract->contractId];
 std::vector<RoutePrototype::Point>& mailMapRoute=cache.points;
 root->setVisible(true);
 missionMapCircle(map,root,"MailOrigin",origin,MyGUI::Colour(1,.12f,.10f));
 missionMapCircle(map,root,"MailTarget",target,MyGUI::Colour(.12f,1,.20f));
 std::string key=contract->contractId+":"+contract->originTownId;
 for(size_t j=0;j<contract->steps.size();++j)key+=":"+contract->steps[j].townId+(contract->steps[j].delivered?"+":"-");
 std::ostringstream endpoints;endpoints.precision(17);for(size_t j=0;j<points.size();++j)endpoints<<":"<<points[j];key+=endpoints.str();
 if(key!=cache.key){
  cache.key=key;mailMapRoute.clear();
  static PricingRoads::Graph roads;static bool loaded=false;
  if(!loaded){loaded=true;try{std::ifstream in("mods/Guild Escort Contracts/pricing-roads.dat");roads.read(in);}catch(const std::exception&){}}
  Ogre::Vector3 a=origin,b=target;
  // No fabricated straight line if roads cannot be resolved.
  roads.route(RoutePrototype::Point(a.x,a.y,a.z),RoutePrototype::Point(b.x,b.y,b.z),mailMapRoute);
  // One line per route rebuild gives native evidence of the exact endpoints.
  std::ostringstream trace;trace.precision(9);trace<<"Mail route: contract="<<contract->contractId<<" originSid="<<contract->originTownId<<" originInstance="<<contract->originInstanceId<<" from="<<a.x<<","<<a.y<<","<<a.z<<" to="<<b.x<<","<<b.y<<","<<b.z<<" roadPoints="<<mailMapRoute.size();DebugLog(trace.str());
 }
 int used=0;double carried=0;
 for(size_t j=1;j<mailMapRoute.size()&&used<240;++j){
  const RoutePrototype::Point& a=mailMapRoute[j-1];const RoutePrototype::Point& b=mailMapRoute[j];
  MyGUI::IntPoint p=map->worldToMapCoords(Ogre::Vector3((float)a.x,(float)a.y,(float)a.z)),q=map->worldToMapCoords(Ogre::Vector3((float)b.x,(float)b.y,(float)b.z));
  double dx=q.left-p.left,dy=q.top-p.top,len=sqrt(dx*dx+dy*dy);if(len<.1)continue;double at=carried;
  while(at<=len&&used<240){std::ostringstream n;n<<"MailRoad"<<used++;MyGUI::Widget* dot=missionMapChild(root,n.str(),5,5);dot->setColour(registerAmber);dot->setPosition(p.left+(int)(dx*at/len)-2,p.top+(int)(dy*at/len)-2);dot->setVisible(true);at+=9;}carried=at-len;
 }
 for(int j=used;j<240;++j){std::ostringstream n;n<<"MailRoad"<<j;if(MyGUI::Widget* dot=root->findWidget(n.str()))dot->setVisible(false);}
 }
 // Remove only obsolete delivery children, including those from a prior world.
 for(size_t i=container->getChildCount();i>0;--i){MyGUI::Widget* child=container->getChildAt(i-1);if(!visible.count(child->getName()))container->_destroyChildWidget(child);}
 for(std::map<std::string,MailMapRouteCache>::iterator it=mailMapRoutes.begin();it!=mailMapRoutes.end();){if(!visible.count("MailContract:"+it->first))mailMapRoutes.erase(it++);else ++it;}
 container->setVisible(!visible.empty());
}
