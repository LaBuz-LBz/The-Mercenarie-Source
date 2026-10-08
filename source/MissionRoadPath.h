#pragma once
#include "PricingRoads.h"
namespace MissionRoadPath {
// Local level data: road 424-Newwworld, vertex 3 jumps from the road at
// (-50932,873) to the town gate at (-51032,2782), then back to (-51602,1023).
// Repair only this identified spur, only in the mission's copied graph. Do
// not flatten arbitrary bends or change the world/pricing data.
inline void repairKnownTownSpur(PricingRoads::Graph& graph){
    const RoutePrototype::Point bad(-51032.72,1539.615,2782.943),a(-50932.81,1181.951,873.4854),b(-51602.33,1140.679,1023.788);
    for(size_t i=0;i<graph.points.size();++i){
        if(RoutePrototype::distance(graph.points[i],bad)>1||graph.edges[i].size()!=2)continue;
        RoutePrototype::Point x=graph.points[graph.edges[i][0].to],y=graph.points[graph.edges[i][1].to];
        if(!((RoutePrototype::distance(x,a)<1&&RoutePrototype::distance(y,b)<1)||(RoutePrototype::distance(x,b)<1&&RoutePrototype::distance(y,a)<1)))continue;
        graph.points[i]=RoutePrototype::Point((x.x+y.x)/2,(x.y+y.y)/2,(x.z+y.z)/2);
        for(size_t j=0;j<2;++j){unsigned int k=graph.edges[i][j].to;double cost=RoutePrototype::distance(graph.points[i],graph.points[k]);graph.edges[i][j].cost=cost;for(size_t n=0;n<graph.edges[k].size();++n)if(graph.edges[k][n].to==i)graph.edges[k][n].cost=cost;}
    }
}
// Join the closest road segment, not its potentially distant town endpoint.
inline bool attach(PricingRoads::Graph& graph,const RoutePrototype::Point& p,RoutePrototype::Point& projected){
    double best=1e100;unsigned int a=0,b=0;
    for(unsigned int i=0;i<graph.points.size();++i)for(size_t j=0;j<graph.edges[i].size();++j){
        unsigned int k=graph.edges[i][j].to;if(k<=i)continue;
        const RoutePrototype::Point& x=graph.points[i];const RoutePrototype::Point& y=graph.points[k];
        double dx=y.x-x.x,dz=y.z-x.z,n=dx*dx+dz*dz;if(n<=0)continue;
        double t=std::max(0.0,std::min(1.0,((p.x-x.x)*dx+(p.z-x.z)*dz)/n));
        RoutePrototype::Point q(x.x+t*dx,x.y+t*(y.y-x.y),x.z+t*dz);
        double dist=RoutePrototype::distance(p,q);
        if(dist<best){best=dist;a=i;b=k;projected=q;}
    }
    if(best>10000)return false;
    for(size_t i=0;i<graph.edges[a].size();++i)if(graph.edges[a][i].to==b){graph.edges[a].erase(graph.edges[a].begin()+i);break;}
    for(size_t i=0;i<graph.edges[b].size();++i)if(graph.edges[b][i].to==a){graph.edges[b].erase(graph.edges[b].begin()+i);break;}
    unsigned int node=graph.add(projected);graph.link(a,node);graph.link(b,node);return true;
}
inline bool plan(const PricingRoads::Graph& network,const RoutePrototype::Point& from,const RoutePrototype::Point& to,std::vector<RoutePrototype::Point>& points){
    points.clear();if(!from.valid()||!to.valid())return false;
    PricingRoads::Graph graph=network;repairKnownTownSpur(graph);RoutePrototype::Point start,end;
    if(!attach(graph,from,start)||!attach(graph,to,end))return false;
    std::vector<RoutePrototype::Point> path;if(!graph.route(start,end,path))return false;
    path.insert(path.begin(),from);path.push_back(to);
    for(size_t i=1;i<path.size();++i){
        double length=RoutePrototype::distance(path[i-1],path[i]);if(length<0.1)continue;
        int steps=std::max(1,(int)ceil(length/60.0));
        if(points.size()+steps>50000)return false;
        for(int j=1;j<=steps;++j){double t=(double)j/steps;const RoutePrototype::Point& a=path[i-1];const RoutePrototype::Point& b=path[i];points.push_back(RoutePrototype::Point(a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t));}
    }
    return !points.empty();
}
}
