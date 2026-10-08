#pragma once
#include <algorithm>
#include <cmath>
#include <vector>

// Pure geometry used while an offer is generated. The first destination fixes
// the broad heading; later stops must continue forward without requiring a
// perfectly straight line. Runtime delivery order remains unrestricted.
namespace MailRoutePlanner {
struct Point { double x,z;Point(double px=0,double pz=0):x(px),z(pz){} };
struct Candidate { int index;Point point;double legKm,score;Candidate(int i=0,const Point& p=Point(),double km=0):index(i),point(p),legKm(km),score(0){} };
inline double length(double x,double z){return std::sqrt(x*x+z*z);}
inline double distance(const Point& a,const Point& b){return length(a.x-b.x,a.z-b.z);}
inline double dot(double ax,double az,double bx,double bz){return ax*bx+az*bz;}

inline bool coherent(const Point& origin,const Point& before,const Point& previous,const Point& first,const Point& candidate){
    const double hx=first.x-origin.x,hz=first.z-origin.z,headingLength=length(hx,hz);
    const double sx=previous.x-before.x,sz=previous.z-before.z,segmentLength=length(sx,sz);
    const double nx=candidate.x-previous.x,nz=candidate.z-previous.z,nextLength=length(nx,nz);
    if(headingLength<1e-6||segmentLength<1e-6||nextLength<1e-6)return false;
    const double previousProjection=dot(previous.x-origin.x,previous.z-origin.z,hx,hz)/headingLength;
    const double candidateProjection=dot(candidate.x-origin.x,candidate.z-origin.z,hx,hz)/headingLength;
    // Require measurable global progress along the initial heading.
    if(candidateProjection<=previousProjection+std::max(.001,headingLength*.04))return false;
    // Reject a turn beyond roughly 100 degrees relative to the latest leg.
    if(dot(sx,sz,nx,nz)/(segmentLength*nextLength)<-.17)return false;
    // A side step is fine, but a large return toward the origin is not.
    if(distance(origin,candidate)<distance(origin,previous)*.88)return false;
    return true;
}

inline double continuationScore(const Point& origin,const Point& previous,const Point& first,const Point& candidate){
    const double hx=first.x-origin.x,hz=first.z-origin.z,h=std::max(1e-6,length(hx,hz));
    const double advance=dot(candidate.x-previous.x,candidate.z-previous.z,hx,hz)/h;
    const double lateral=std::fabs((candidate.x-origin.x)*hz-(candidate.z-origin.z)*hx)/h;
    return advance-lateral*.18-distance(previous,candidate)*.06;
}
inline bool better(const Candidate& a,const Candidate& b){return a.score>b.score;}
inline std::vector<Candidate> coherentCandidates(const Point& origin,const Point& before,const Point& previous,const Point& first,const std::vector<Candidate>& input){
    std::vector<Candidate> result;for(size_t i=0;i<input.size();++i)if(coherent(origin,before,previous,first,input[i].point)){Candidate c=input[i];c.score=continuationScore(origin,previous,first,c.point);result.push_back(c);}std::sort(result.begin(),result.end(),better);return result;
}
inline double totalDistanceKm(const std::vector<double>& successiveLegs){double total=0;for(size_t i=0;i<successiveLegs.size();++i)total+=std::max(0.0,successiveLegs[i]);return total;}
}
