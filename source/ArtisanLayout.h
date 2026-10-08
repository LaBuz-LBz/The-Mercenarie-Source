#pragma once
#include <algorithm>
#include <kenshi/Enums.h>
namespace ArtisanLayout {
inline int category(int slot){
 switch(slot){
 case ATTACH_HAT:case ATTACH_HAIR:case ATTACH_EYES:return 1;
 case ATTACH_BODY:return 2;case ATTACH_SHIRT:return 3;case ATTACH_LEGS:return 4;
 case ATTACH_BOOTS:case ATTACH_LEFT_LEG:case ATTACH_RIGHT_LEG:return 5;
 case ATTACH_GLOVES:case ATTACH_LEFT_ARM:case ATTACH_RIGHT_ARM:return 7;
 case ATTACH_BACKPACK:return 8;default:return 6;
 }
}
struct Layout {
 int width,height,side,content,bodyHeight;
 bool wideRows,sideSummary;
 // Fixed preferred footprint: ultrawide displays must not stretch columns.
 // Smaller viewports reduce available space, not font or control scale.
 Layout(int vw,int vh):width(std::min(1612,vw-24)),height(std::min(907,vh-24)) {
  side=width>=1200?width*32/100:152;
  content=width-side-56;
  bodyHeight=height-186;
  wideRows=content>=900;
  sideSummary=width>=1000;
 }
};
struct Row {
 int qualityX,qualityWidth,priceX,priceWidth,quantityX,addX,addWidth;
 Row(int width,bool wide){
  qualityX=wide?width-624:8;
  qualityWidth=224;
  priceX=qualityX+qualityWidth+8;priceWidth=wide?140:88;
  quantityX=priceX+priceWidth+6;addX=quantityX+104;addWidth=width-addX-8;
 }
};
// Native attachment slots; the caller supplies the engine enum values.
template<class Map,class Data> bool known(const Map& groups,Data* data) {
 if(!data)return false;
 typename Map::const_iterator group=groups.find(data->type);
 if(group==groups.end())return false;
 for(size_t i=0;i<group->second.size();++i)if(group->second[i]==data)return true;
 return false;
}
}
