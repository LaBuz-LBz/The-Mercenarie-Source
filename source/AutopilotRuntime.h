#pragma once
#include "AutopilotDestinationModel.h"
struct AutopilotSprite {std::string texture;MyGUI::IntCoord rect;};
std::map<std::string,AutopilotSprite> autopilotSprites;
void loadAutopilotSprites(){
 autopilotSprites.clear();MyGUI::IResource* resource=MyGUI::ResourceManager::getInstance().getByName("Kenshi_MapMarkers",false);
 MyGUI::ResourceImageSet* images=resource?resource->castType<MyGUI::ResourceImageSet>(false):0;if(!images)return;
 MyGUI::EnumeratorGroupImage groups=images->getEnumerator();
 while(groups.next()){const MyGUI::GroupImage& g=groups.current();if(g.size.width<=0||g.size.height<=0||g.texture.empty())continue;
  for(size_t i=0;i<g.indexes.size();++i){const MyGUI::IndexImage& index=g.indexes[i];if(index.frames.empty())continue;
   AutopilotSprite s;s.texture=g.texture;s.rect=MyGUI::IntCoord(index.frames[0].left,index.frames[0].top,g.size.width,g.size.height);
   if(i==0)autopilotSprites[g.name]=s;if(!index.name.empty())autopilotSprites[index.name]=s;
  }
 }
}
std::string autopilotIdentity(TownBase* town){
 if(!town->getHandle().isNull())return town->getHandle().toString();
 // Position stays stable when world states swap the underlying GameData record.
 const Ogre::Vector3& p=town->getPosition();std::ostringstream id;id.precision(12);id<<"position:"<<p.x<<":"<<p.y<<":"<<p.z;return id.str();
}
bool autopilotReadPlace(TownBase* town,AutopilotDestinations::Place& p){
 if(!town||!town->isDiscovered()||!town->getGameData()||town->townType==TOWN_NULL||!ou||!ou->zoneMgr)return false;
 p.name=town->getName();p.marker=town->getMapMarker();if(autopilotSprites.find(p.marker)==autopilotSprites.end())return false;
 const Ogre::Vector3& pos=town->getPosition();p.x=pos.x;p.y=pos.y;p.z=pos.z;
 if(!AutopilotDestinations::finite(p.x)||!AutopilotDestinations::finite(p.y)||!AutopilotDestinations::finite(p.z))return false;
 p.id=autopilotIdentity(town);p.type=(int)town->townType;iVector2 sector=ou->zoneMgr->getMapSector(pos);p.mapX=(sector.x+.5)*32;p.mapY=(sector.y+.5)*32;
 Faction* faction=town->getFaction();p.faction=faction?faction->name:std::string();
 p.factionId=faction&&faction->data&&!faction->data->stringID.empty()?faction->data->stringID:p.faction;
 if(p.faction.empty()||p.factionId.empty()){p.factionId="__other";p.faction=Loc::text("autopilot.v9.unknown");}
 return AutopilotDestinations::valid(p);
}
std::vector<AutopilotDestinations::Place> scanAutopilotPlaces(){
 std::vector<AutopilotDestinations::Place> result;if(!shou||!shou->townList)return result;
 for(int source=0;source<2;++source){lektor<RootObject*>& objects=source?shou->townList->nests:shou->townList->getAllTowns();
  for(unsigned int i=0;i<objects.size();++i){AutopilotDestinations::Place p;if(autopilotReadPlace(dynamic_cast<TownBase*>(objects[i]),p))result.push_back(p);}
 }return result;
}
TownBase* resolveAutopilotPlace(const std::string& id){
 if(id.empty()||!shou||!shou->townList)return 0;
 for(int source=0;source<2;++source){lektor<RootObject*>& objects=source?shou->townList->nests:shou->townList->getAllTowns();
  for(unsigned int i=0;i<objects.size();++i){TownBase* town=dynamic_cast<TownBase*>(objects[i]);AutopilotDestinations::Place p;if(autopilotReadPlace(town,p)&&p.id==id)return town;}
 }return 0;
}
