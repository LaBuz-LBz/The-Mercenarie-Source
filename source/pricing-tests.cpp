#include "PricingRoads.h"
#include "ContractRouteVisual.h"
#include "ContinuousMapAxis.h"
#include "ContractGroupPlan.h"
#include "ContractFactors.h"
#include <fstream>
#include <cassert>
#include <iostream>
struct TestMapAxis {int sign;TestMapAxis(int s):sign(s){}int operator()(double x)const{return sign*(int)floor((x+1400)/3000);}};
int main(){
 {using namespace ContractGroupPlan;Plan p;p.entries.push_back(Entry('L',"leader.mod",1));p.entries.push_back(Entry('S',"members.mod",3));p.entries.push_back(Entry('A',"animals.mod",2));assert(p.valid());assert(p.people()==4&&p.animals()==2);
  ContractFactors::Quote factor;factor.fragility=10;factor.count=8;factor.unitValue=400;factor.cargoPercent=15;factor.item="585-gamedata.base";
  std::string saved="V6EST:1.25:ROAD"+encode(p)+ContractFactors::encode(factor)+";PATH=1,2,3;4,5,6;";Plan restored;assert(decode(saved,restored));assert(restored.people()==4&&restored.animals()==2);assert(ContractRouteVisual::decode(saved).size()==2);
  MissionArchive writer;writer.field(saved);MissionArchive reader(writer.bytes);std::string loaded;reader.field(loaded);reader.finish();assert(decode(loaded,restored));assert(encode(restored)==encode(p));ContractFactors::Quote after;assert(ContractFactors::decode(loaded,after));assert(after.cargoValue()==3200&&after.fragility==10);
  assert(saved.find('|')==std::string::npos);assert(!decode("old offer",restored));assert(!decode(";GROUP1=L,1,l:A,99,a;",restored));assert(!decode(";GROUP1=L,1,l:L,1,l;",restored));assert(!decode(";GROUP1=S,2,s;",restored));assert(!decode(";GROUP1=L,1,l",restored));
  int members[]={2,4,0},animals[]={2,3,100};std::vector<Binding> bindings;bindings.push_back(Binding(members,3));bindings.push_back(Binding(animals,2));
  {ScopedCounts guard(bindings);assert(members[0]==3&&members[1]==3&&animals[1]==2);assert(members[2]==0&&animals[2]==100);}assert(members[0]==2&&members[1]==4&&animals[1]==3);
  try{ScopedCounts guard(bindings);throw 1;}catch(int){}assert(members[0]==2&&members[1]==4&&animals[1]==3);
 }
 for(int sign=-1;sign<=1;sign+=2){ContinuousMapAxis axis;assert(axis.calibrate(500,TestMapAxis(sign)));for(int x=-20000;x<20000;x+=173){double expected=sign>0?(x+1400)/3000.0:1-(x+1400)/3000.0;assert(fabs(axis.project(x)-expected)<.00001);}}
 PricingRoads::Graph graph;std::ifstream in("pricing-roads.dat");graph.read(in);assert(graph.points.size()>10000);
 RoutePrototype::Point hub(-50978.80078125,1533.082763671875,2932.4765625),hill(-37262.41015625,674.07830810546875,-43586.984375);
 std::vector<RoutePrototype::Point> a,b;assert(graph.route(hub,hill,a));assert(graph.route(hill,hub,b));
 double la=0,lb=0;for(size_t i=1;i<a.size();++i)la+=RoutePrototype::distance(a[i-1],a[i]);for(size_t i=1;i<b.size();++i)lb+=RoutePrototype::distance(b[i-1],b[i]);
 assert(fabs(la-lb)<.01);assert(la>=RoutePrototype::distance(hub,hill));assert(la<100000);
 std::string metadata="V6EST:1.25:ROAD;PRICE=2000,500,0,0,625,0,0,3125;"+ContractRouteVisual::encode(a);
 MissionArchive writer;writer.field(metadata);MissionArchive reader(writer.bytes);std::string restored;reader.field(restored);reader.finish();
 std::vector<RoutePrototype::Point> decoded=ContractRouteVisual::decode(restored);assert(decoded.size()==a.size());
 for(size_t i=0;i<a.size();++i)assert(RoutePrototype::distance(a[i],decoded[i])<.001);
 assert(ContractRouteVisual::decode("old-region-list").empty());assert(ContractRouteVisual::decode(";PATH=1,2,bad;").empty());
 for(size_t i=0;i<graph.points.size();i+=500){assert(graph.route(hub,graph.points[i],b));assert(!b.empty());}
 assert(!graph.route(hub,RoutePrototype::Point(9000000,0,9000000),b));
 std::istringstream bad("MERCENARIE-PRICING-ROADS-1 1 1 2 -1");bool rejected=false;try{PricingRoads::Graph g;g.read(bad);}catch(...){rejected=true;}assert(rejected);
 std::cout<<"PASS: road graph, multiple destinations, reverse symmetry, distance, off-network fallback, invalid input; Hub-Hill metres="<<la<<"\n";
}
