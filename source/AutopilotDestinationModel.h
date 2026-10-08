#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cmath>
namespace AutopilotDestinations {
inline unsigned int lower(unsigned int c){
 if(c>='A'&&c<='Z')return c+32;
 if((c>=0xC0&&c<=0xD6)||(c>=0xD8&&c<=0xDE)||(c>=0x410&&c<=0x42F))return c+32;
 if(c==0x401)return 0x451;
 const unsigned int upper[]={0x104,0x106,0x118,0x141,0x143,0x15A,0x179,0x17B};
 for(int i=0;i<8;++i)if(c==upper[i])return c+1;return c;
}
inline std::vector<unsigned int> folded(const std::string& s){
 std::vector<unsigned int> out;
 for(size_t i=0;i<s.size();){unsigned int c=(unsigned char)s[i++];int extra=0;if(c>=240){c&=7;extra=3;}else if(c>=224){c&=15;extra=2;}else if(c>=192){c&=31;extra=1;}while(extra--&&i<s.size())c=(c<<6)|((unsigned char)s[i++]&63);out.push_back(lower(c));}return out;
}
inline bool matches(const std::string& text,const std::string& query){std::vector<unsigned int> a=folded(text),b=folded(query);return b.empty()||std::search(a.begin(),a.end(),b.begin(),b.end())!=a.end();}
struct Place {
 std::string id,name,factionId,faction,marker;int type;double x,y,z,mapX,mapY;
 Place():type(0),x(0),y(0),z(0),mapX(0),mapY(0){}
};
inline bool finite(double n){return n==n&&n>-1e10&&n<1e10;}
inline bool valid(const Place& p){return !p.id.empty()&&!p.name.empty()&&!p.marker.empty()&&finite(p.x)&&finite(p.y)&&finite(p.z)&&finite(p.mapX)&&finite(p.mapY)&&p.mapX>=0&&p.mapX<=2048&&p.mapY>=0&&p.mapY<=2048;}
struct Order {
 bool operator()(const Place& a,const Place& b)const{
  if(a.factionId=="__other"&&b.factionId!="__other")return false;if(b.factionId=="__other"&&a.factionId!="__other")return true;
  if(folded(a.faction)!=folded(b.faction))return folded(a.faction)<folded(b.faction);
  if(a.factionId!=b.factionId)return a.factionId<b.factionId;
  if(folded(a.name)!=folded(b.name))return folded(a.name)<folded(b.name);return a.id<b.id;
 }
};
struct Group {std::string id,name;std::vector<size_t> places;};
struct Model {
 std::vector<Place> places;std::string selected,query;std::set<std::string> expanded;
 const Place* find(const std::string& id)const{for(size_t i=0;i<places.size();++i)if(places[i].id==id)return &places[i];return 0;}
 void replace(const std::vector<Place>& incoming){
  const Place* old=find(selected);std::string oldFaction=old?old->factionId:std::string();
  places.clear();std::set<std::string> seen;for(size_t i=0;i<incoming.size();++i)if(valid(incoming[i])&&seen.insert(incoming[i].id).second)places.push_back(incoming[i]);std::stable_sort(places.begin(),places.end(),Order());
  const Place* p=find(selected);if(!p)selected.clear();else if(p->factionId!=oldFaction)expanded.insert(p->factionId);
 }
 bool select(const std::string& id){const Place* p=find(id);if(!p||!matches(p->name,query))return false;selected=id;expanded.insert(p->factionId);return true;}
 void search(const std::string& q){query=q;const Place* p=find(selected);if(p&&!matches(p->name,query))selected.clear();}
 std::vector<Group> groups()const{
  std::vector<Group> result;for(size_t i=0;i<places.size();++i){const Place& p=places[i];if(!matches(p.name,query))continue;
   if(result.empty()||result.back().id!=p.factionId){Group g;g.id=p.factionId;g.name=p.faction;result.push_back(g);}result.back().places.push_back(i);
  }return result;
 }
 bool open(const std::string& id)const{return !query.empty()||expanded.count(id)!=0;}
};
// Isotropic projection of the existing 2048px map; overview letterboxes instead of stretching.
struct View {
 int width,height;double crop,cx,cy;
 View():width(1),height(1),crop(2048),cx(1024),cy(1024){}
 double scale()const{return std::min(width,height)/crop;}
 void clamp(){crop=std::max(256.0,std::min(2048.0,crop));double sx=width/(2*scale()),sy=height/(2*scale());cx=sx>=1024?1024:std::max(sx,std::min(2048-sx,cx));cy=sy>=1024?1024:std::max(sy,std::min(2048-sy,cy));}
 double px(double world)const{return width*.5+(world-cx)*scale();}double py(double world)const{return height*.5+(world-cy)*scale();}
 void zoom(double factor,double x,double y){double wx=cx+(x-width*.5)/scale(),wy=cy+(y-height*.5)/scale();crop=std::max(256.0,std::min(2048.0,crop*factor));cx=wx-(x-width*.5)/scale();cy=wy-(y-height*.5)/scale();clamp();}
 void pan(double dx,double dy){cx-=dx/scale();cy-=dy/scale();clamp();}
 void center(double x,double y){cx=x;cy=y;clamp();}
 void reset(){crop=2048;cx=cy=1024;clamp();}
};
struct Layout {
 int w,h,left,rightX,rightW,top,mapH,infoH,footerY;bool compact;
 Layout(int vw,int vh){w=std::min(1500,vw-24);h=std::min(950,vh-24);left=w<1000?242:std::max(270,std::min(390,w*27/100));rightX=left+28;rightW=w-rightX-16;top=(vw>=1200&&vh>=700)?154:126;footerY=h-64;compact=w<1050||h<680;infoH=compact?142:250;mapH=footerY-top-12-(compact?infoH+8:0);}
};
}
