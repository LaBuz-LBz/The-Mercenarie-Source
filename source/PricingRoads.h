#pragma once
#include "RoutePrototype.h"
#include <queue>
#include <map>
#include <locale>
namespace PricingRoads {
using RoutePrototype::Point;
using RoutePrototype::distance;
struct Edge {unsigned int to;double cost;Edge(unsigned int t,double c):to(t),cost(c){}};
struct Graph {
 std::vector<Point> points;std::vector<std::vector<Edge> > edges;
 unsigned int add(const Point& p){points.push_back(p);edges.push_back(std::vector<Edge>());return (unsigned int)points.size()-1;}
 void link(unsigned int a,unsigned int b){double d=distance(points[a],points[b]);edges[a].push_back(Edge(b,d));edges[b].push_back(Edge(a,d));}
 void read(std::istream& in){
  Graph g;std::map<int,unsigned int> junctions;std::string magic;int roads;
  in.imbue(std::locale::classic());if(!(in>>magic>>roads)||magic!="MERCENARIE-PRICING-ROADS-1"||roads<1||roads>10000)throw std::runtime_error("road header");
  for(int r=0;r<roads;++r){int a,b,n;if(!(in>>a>>b>>n)||n<2||n>10000||g.points.size()+n>200000)throw std::runtime_error("road bounds");unsigned int previous=0;
   for(int i=0;i<n;++i){Point p;if(!(in>>p.x>>p.y>>p.z)||!p.valid())throw std::runtime_error("road point");unsigned int id;
    if(i==0||i==n-1){int key=i==0?a:b;std::map<int,unsigned int>::iterator found=junctions.find(key);if(found!=junctions.end()){id=found->second;if(distance(g.points[id],p)>1)throw std::runtime_error("road junction");}else{id=g.add(p);junctions[key]=id;}}
    else id=g.add(p);if(i)g.link(previous,id);previous=id;
   }
  }std::string extra;if(in>>extra)throw std::runtime_error("road trailing data");*this=g;
 }
 unsigned int nearest(const Point& p)const{unsigned int best=0;double d=1e100;for(unsigned int i=0;i<points.size();++i){double next=distance(p,points[i]);if(next<d){best=i;d=next;}}return best;}
 bool route(const Point& from,const Point& to,std::vector<Point>& out)const{
  out.clear();if(points.empty()||!from.valid()||!to.valid())return false;
  unsigned int a=nearest(from),b=nearest(to);
  // Long off-network accesses are not represented as road routes.
  if(distance(from,points[a])>10000||distance(to,points[b])>10000)return false;
  typedef std::pair<double,unsigned int> Entry;std::priority_queue<Entry,std::vector<Entry>,std::greater<Entry> > q;
  std::vector<double> cost(points.size(),1e100);std::vector<unsigned int> prev(points.size(),(unsigned int)points.size());cost[a]=0;q.push(Entry(0,a));
  while(!q.empty()){Entry e=q.top();q.pop();if(e.first!=cost[e.second])continue;if(e.second==b)break;
   const std::vector<Edge>& es=edges[e.second];for(size_t i=0;i<es.size();++i){double c=e.first+es[i].cost;if(c<cost[es[i].to]){cost[es[i].to]=c;prev[es[i].to]=e.second;q.push(Entry(c,es[i].to));}}
  }
  if(cost[b]>=1e100)return false;std::vector<Point> reverse;unsigned int at=b;
  while(at!=a){if(reverse.size()>points.size())return false;reverse.push_back(points[at]);at=prev[at];}
  out.push_back(from);out.push_back(points[a]);for(size_t i=reverse.size();i>0;--i)out.push_back(reverse[i-1]);out.push_back(to);return true;
 }
};
}
