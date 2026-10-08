#pragma once
#include "ContractRegionGrid.h"
#include "ContractRouteVisual.h"
#include "src/MercenarieRegions.h"
#include <set>
#include <algorithm>

// Shared by the difficulty calculation and the map. Coordinates use the
// native 64 x 4608-unit sectors, with the same orientation as GuildEscortMap.
namespace ContractRegionRoute {
struct Result {
    std::set<int> regions;
    int maximum,tags; bool unknown;
    Result():maximum(1),tags(0),unknown(false){}
    void add(int i){
        if(i<0||i>=MercenarieRegions::count()){unknown=true;return;}
        regions.insert(i);maximum=std::max(maximum,MercenarieRegions::Table[i].level);tags|=MercenarieRegions::Table[i].tags;
    }
    void merge(const Result& r){regions.insert(r.regions.begin(),r.regions.end());maximum=std::max(maximum,r.maximum);tags|=r.tags;unknown=unknown||r.unknown;}
};
inline int cell(int x,int z){return x<0||z<0||x>=256||z>=256?-1:ContractRegionGrid::Cells[z*256+x];}
inline double grid(double world){return world/1152.0+128.0;}
inline void at(Result& r,double x,double z){r.add(cell((int)floor(x),(int)floor(z)));}
inline void segment(Result& r,const RoutePrototype::Point& a,const RoutePrototype::Point& b){
    if(!a.valid()||!b.valid()){r.unknown=true;return;}
    double x=grid(a.x),z=grid(a.z),dx=grid(b.x)-x,dz=grid(b.z)-z;
    // Split at EVERY native cell boundary, not one midpoint per kilometre.
    // Midpoints then identify every nonzero-length region crossing exactly.
    std::vector<double> cuts;cuts.push_back(0);cuts.push_back(1);
    for(int k=0;k<=256;++k){
        if(fabs(dx)>1e-12){double t=(k-x)/dx;if(t>0&&t<1)cuts.push_back(t);}
        if(fabs(dz)>1e-12){double t=(k-z)/dz;if(t>0&&t<1)cuts.push_back(t);}
    }
    std::sort(cuts.begin(),cuts.end());at(r,x,z);at(r,x+dx,z+dz);
    for(size_t j=1;j<cuts.size();++j)if(cuts[j]-cuts[j-1]>1e-12){double t=(cuts[j]+cuts[j-1])*.5;at(r,x+t*dx,z+t*dz);}
}
inline Result analyse(const std::vector<RoutePrototype::Point>& path){
    Result r;if(path.empty()){r.unknown=true;return r;}
    if(path.size()==1){if(path[0].valid())at(r,grid(path[0].x),grid(path[0].z));else r.unknown=true;}
    for(size_t i=1;i<path.size();++i)segment(r,path[i-1],path[i]);return r;
}
inline std::vector<RoutePrototype::Point> returnPath(const std::string& s){
    size_t a=s.find(";BACKPATH=");return a==std::string::npos?std::vector<RoutePrototype::Point>():ContractRouteVisual::decode(";PATH="+s.substr(a+10));
}
inline Result metadata(const std::string& s){Result r=analyse(ContractRouteVisual::decode(s));std::vector<RoutePrototype::Point> back=returnPath(s);if(!back.empty())r.merge(analyse(back));return r;}
inline unsigned int colour(int level){static const unsigned int c[]={0,0x76bd7b,0xbcc267,0xdcad55,0xd78250,0xc95c58};return c[std::max(1,std::min(5,level))];}
inline void pixel(std::vector<unsigned char>& rgba,int x,int y,unsigned int c,int alpha){
    if(x<0||y<0||x>=2048||y>=2048)return;size_t p=(y*2048+x)*4;
    rgba[p]=(unsigned char)(c>>16);rgba[p+1]=(unsigned char)(c>>8);rgba[p+2]=(unsigned char)c;rgba[p+3]=(unsigned char)alpha;
}
inline void raster(const Result& r,std::vector<unsigned char>& rgba){
    rgba.assign(2048*2048*4,0);
    for(int y=0;y<256;++y)for(int x=0;x<256;++x){int i=cell(x,y);
        int neighbours[]={cell(x+1,y),cell(x,y+1)};
        for(int side=0;side<2;++side){int other=neighbours[side];if(i==other||((i<0||i==255)&&(other<0||other==255)))continue;
            const int alpha=180;
            for(int n=0;n<8;++n){int px=side==0?(x+1)*8:x*8+n,py=side==0?y*8+n:(y+1)*8;
                pixel(rgba,px,py,0xdccfb1,alpha);
                pixel(rgba,px-(side==0?1:0),py-(side==1?1:0),0xdccfb1,alpha);
            }
        }
    }
}
}
