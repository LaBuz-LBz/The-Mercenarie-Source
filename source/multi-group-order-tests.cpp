#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

struct Registry {
    std::vector<std::string> original,current;
    std::map<std::string,std::set<std::string> > groups;
    Registry(const char* names[],size_t n){for(size_t i=0;i<n;++i)original.push_back(names[i]);current=original;}
    bool absent(const std::string& id)const{for(std::map<std::string,std::set<std::string> >::const_iterator g=groups.begin();g!=groups.end();++g)if(g->second.count(id))return true;return false;}
    void reorder(){current.clear();for(size_t i=0;i<original.size();++i)if(!absent(original[i]))current.push_back(original[i]);for(size_t i=0;i<original.size();++i)if(absent(original[i]))current.push_back(original[i]);}
    bool depart(const std::string& group,const std::vector<std::string>& members){if(group.empty()||groups.count(group)||members.empty())return false;std::set<std::string> unique;for(size_t i=0;i<members.size();++i){if(absent(members[i])||std::find(original.begin(),original.end(),members[i])==original.end()||!unique.insert(members[i]).second)return false;}size_t absentCount=0;for(size_t i=0;i<original.size();++i)if(absent(original[i]))++absentCount;if(absentCount+members.size()>=original.size())return false;groups[group]=unique;reorder();return true;}
    bool back(const std::string& group){if(!groups.erase(group))return false;reorder();return true;}
};
static std::vector<std::string> v(const char* a,const char* b=0){std::vector<std::string> r;r.push_back(a);if(b)r.push_back(b);return r;}
int main(){
    const char* order[]={"A","B","C","D","E","F","G","H"};
    Registry first(order,8);assert(first.depart("M1",v("B","D")));assert(first.depart("M2",v("F","G")));assert(first.back("M1"));assert(first.absent("F")&&first.absent("G")&&!first.absent("B"));const char* midNames[]={"A","B","C","D","E","H","F","G"};Registry midRegistry(midNames,8);assert(first.current==midRegistry.original);assert(first.back("M2"));assert(first.current==first.original);
    Registry inverse(order,8);assert(inverse.depart("M1",v("B","D")));assert(inverse.depart("M2",v("F","G")));assert(inverse.back("M2"));assert(inverse.absent("B")&&inverse.absent("D"));assert(inverse.back("M1"));assert(inverse.current==inverse.original);
    Registry three(order,8);assert(three.depart("M1",v("B")));assert(three.depart("M2",v("D")));assert(three.depart("M3",v("F")));assert(!three.depart("DUP",v("F")));assert(three.back("M2")&&three.absent("B")&&three.absent("F"));assert(three.back("M1")&&three.back("M3")&&three.current==three.original);
    Registry last(order,8);std::vector<std::string> all=last.original;assert(!last.depart("all",all));all.pop_back();assert(last.depart("seven",all));assert(!last.depart("last",v("H")));
    std::cout<<"PASS: multi-group reference order, both return orders, three groups, unique membership, last playable character.\n";
}
