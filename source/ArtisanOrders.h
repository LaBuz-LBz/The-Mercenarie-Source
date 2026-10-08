#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <algorithm>

// Engine-independent order ledger. Times are absolute Kenshi world hours.
namespace ArtisanOrders {
enum Status { Waiting, Making, Ready, Collected, Lost };
struct Line {
 std::string item,quality; int kind,quantity,remaining; long long base,unit; double hours;
 Line():kind(0),quantity(1),remaining(1),base(0),unit(0),hours(0){}
};
inline int unitHours(long long value){return value<=1000?6:value<=3000?12:value<=6000?15:value<=10000?20:24;}
inline long long price(long long value){if(value<0||value>1717986916LL)throw std::runtime_error("Invalid equipment value");return (value*5+3)/4;}
inline double series(long long value,int quantity){if(quantity<1||quantity>10)throw std::runtime_error("Invalid equipment quantity");return unitHours(value)*(1.0+0.75*(quantity-1));}
inline bool valid(const Line& l){return !l.item.empty()&&!l.quality.empty()&&l.kind>=0&&l.kind<=1&&l.quantity>=1&&l.quantity<=10&&l.remaining>=0&&l.remaining<=l.quantity&&l.base>=0&&l.base<=1717986916LL&&l.unit==price(l.base)&&l.hours==series(l.base,l.quantity);}
inline long long total(const std::vector<Line>& rows){long long n=0;for(size_t i=0;i<rows.size();++i){if(!valid(rows[i]))throw std::runtime_error("Invalid equipment line");n+=rows[i].unit*rows[i].quantity;if(n>2147483647LL)throw std::runtime_error("Order exceeds wallet capacity");}return n;}
inline double duration(const std::vector<Line>& rows){double h=0;for(size_t i=0;i<rows.size();++i)h+=rows[i].hours;return h;}
struct Order {
 unsigned long id; std::string artisan,name; std::vector<Line> lines;
 long long paid; double created,start,end; Status status; bool notified;
 Order():id(0),paid(0),created(0),start(0),end(0),status(Waiting),notified(false){}
};
struct Ledger {
 std::map<std::string,std::vector<Line> > baskets;
 std::vector<Order> orders; unsigned long nextId;
 Ledger():nextId(1){}
 bool put(const std::string& artisan,Line line,int quantity){
  if(artisan.empty()||quantity<0||quantity>10)return false;
  line.quantity=std::max(1,quantity);line.remaining=line.quantity;line.unit=price(line.base);line.hours=series(line.base,line.quantity);
  if(!valid(line))return false;
  std::vector<Line>& b=baskets[artisan];size_t i=0;
  for(;i<b.size();++i)if(b[i].item==line.item&&b[i].quality==line.quality&&b[i].kind==line.kind)break;
  if(quantity==0){if(i<b.size())b.erase(b.begin()+i);return true;}
  if(i<b.size())b[i]=line;else b.push_back(line);return true;
 }
 // Prepare all allocations before the native debit. The caller commits by swap
 // on the game thread only after the wallet confirms the exact debit.
 unsigned long confirm(const std::string& artisan,const std::string& name,double now,long long funds){
  std::map<std::string,std::vector<Line> >::iterator b=baskets.find(artisan);
  if(b==baskets.end()||b->second.empty()||!(now>=0&&now<=1e9)||nextId==0||nextId==std::numeric_limits<unsigned long>::max())return 0;
  long long cost=total(b->second);if(cost>funds)return 0;
  Order o;o.id=nextId;o.artisan=artisan;o.name=name;o.created=now;o.paid=cost;o.lines=b->second;o.start=now;
  for(size_t i=0;i<orders.size();++i)if(orders[i].artisan==artisan&&(orders[i].status==Waiting||orders[i].status==Making))o.start=std::max(o.start,orders[i].end);
  o.end=o.start+duration(o.lines);o.status=o.start<=now?Making:Waiting;
  orders.push_back(o);b->second.clear();++nextId;return o.id;
 }
 void tick(double now){if(!(now>=0&&now<=1e9))return;for(size_t i=0;i<orders.size();++i){Order& o=orders[i];if(o.status==Waiting||o.status==Making)o.status=now>=o.end?Ready:now>=o.start?Making:Waiting;}}
 bool take(unsigned long id,const std::string& artisan,size_t line){for(size_t i=0;i<orders.size();++i){Order& o=orders[i];if(o.id!=id||o.artisan!=artisan||o.status!=Ready||line>=o.lines.size()||o.lines[line].remaining<=0)continue;--o.lines[line].remaining;bool empty=true;for(size_t j=0;j<o.lines.size();++j)if(o.lines[j].remaining)empty=false;if(empty)o.status=Collected;return true;}return false;}
 bool dead(const std::string& artisan){bool changed=false;for(size_t i=0;i<orders.size();++i){Order& o=orders[i];if(o.artisan==artisan&&o.status!=Collected&&o.status!=Lost){o.status=Lost;o.notified=true;for(size_t j=0;j<o.lines.size();++j)o.lines[j].remaining=0;changed=true;}}baskets.erase(artisan);return changed;}
 void swap(Ledger& other){baskets.swap(other.baskets);orders.swap(other.orders);std::swap(nextId,other.nextId);}
};
// Length-prefixed strings preserve mod identifiers and arbitrary UTF-8 names.
inline void str(std::ostream& s,const std::string& v){s<<v.size()<<' ';s.write(v.data(),v.size());s<<'\n';}
inline std::string str(std::istream& s){size_t n=0;if(!(s>>n)||n>1048576||s.get()!=' ')throw std::runtime_error("Invalid artisan string");std::string v(n,'\0');if(n)s.read(&v[0],n);if(!s||s.get()!='\n')throw std::runtime_error("Truncated artisan string");return v;}
inline void lines(std::ostream& s,const std::vector<Line>& v){s<<v.size()<<'\n';for(size_t i=0;i<v.size();++i){const Line& l=v[i];str(s,l.item);str(s,l.quality);s<<l.kind<<' '<<l.quantity<<' '<<l.remaining<<' '<<l.base<<' '<<l.unit<<' '<<l.hours<<'\n';}}
inline std::vector<Line> lines(std::istream& s){size_t n=0;if(!(s>>n)||n>100000)throw std::runtime_error("Invalid artisan line count");std::vector<Line> v;for(size_t i=0;i<n;++i){Line l;l.item=str(s);l.quality=str(s);if(!(s>>l.kind>>l.quantity>>l.remaining>>l.base>>l.unit>>l.hours)||!valid(l))throw std::runtime_error("Invalid saved artisan line");for(size_t j=0;j<v.size();++j)if(v[j].item==l.item&&v[j].quality==l.quality&&v[j].kind==l.kind)throw std::runtime_error("Duplicate artisan line");v.push_back(l);}return v;}
inline std::string save(const Ledger& l){std::ostringstream s;s<<std::setprecision(17)<<"ARTISAN1 "<<l.nextId<<' '<<l.baskets.size()<<'\n';for(std::map<std::string,std::vector<Line> >::const_iterator i=l.baskets.begin();i!=l.baskets.end();++i){str(s,i->first);lines(s,i->second);}s<<l.orders.size()<<'\n';for(size_t i=0;i<l.orders.size();++i){const Order& o=l.orders[i];s<<o.id<<' '<<o.paid<<' '<<o.created<<' '<<o.start<<' '<<o.end<<' '<<(int)o.status<<' '<<o.notified<<'\n';str(s,o.artisan);str(s,o.name);lines(s,o.lines);}return s.str();}
inline Ledger load(const std::string& bytes){Ledger l;if(bytes.empty())return l;std::istringstream s(bytes);std::string magic;size_t n=0;if(!(s>>magic>>l.nextId>>n)||magic!="ARTISAN1"||!l.nextId||n>100000)throw std::runtime_error("Invalid artisan archive");for(size_t i=0;i<n;++i){std::string key=str(s);if(key.empty()||l.baskets.count(key))throw std::runtime_error("Invalid artisan identity");l.baskets[key]=lines(s);}if(!(s>>n)||n>1000000)throw std::runtime_error("Invalid order count");unsigned long last=0;std::map<std::string,double> ends;for(size_t i=0;i<n;++i){Order o;int status=0;if(!(s>>o.id>>o.paid>>o.created>>o.start>>o.end>>status>>o.notified)||status<Waiting||status>Lost||o.id<=last||o.id>=l.nextId||!(o.created>=0&&o.start>=o.created&&o.end>=o.start&&o.end<=1e9))throw std::runtime_error("Invalid saved order");last=o.id;o.status=(Status)status;o.artisan=str(s);o.name=str(s);o.lines=lines(s);if(o.artisan.empty()||o.lines.empty()||o.paid!=total(o.lines)||std::fabs(o.end-o.start-duration(o.lines))>1e-6)throw std::runtime_error("Invalid saved order totals");int remaining=0;for(size_t j=0;j<o.lines.size();++j){remaining+=o.lines[j].remaining;if((o.status==Waiting||o.status==Making)&&o.lines[j].remaining!=o.lines[j].quantity)throw std::runtime_error("Unfinished order already collected");}
 if(((o.status==Collected||o.status==Lost)&&remaining!=0)||(o.status==Ready&&remaining==0)||((o.status==Waiting||o.status==Making)&&o.notified))throw std::runtime_error("Invalid saved order status");
 if(o.status!=Lost){if(ends.count(o.artisan)&&o.start<ends[o.artisan])throw std::runtime_error("Overlapping artisan queue");ends[o.artisan]=o.end;}
 l.orders.push_back(o);}s>>std::ws;if(!s.eof())throw std::runtime_error("Unexpected artisan data");return l;}
}
