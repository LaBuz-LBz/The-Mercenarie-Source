#pragma once
#include "RoutePrototype.h"
#include <istream>
#include <locale>
#include <algorithm>

namespace RoutePrototype {
// A single offline-computed vanilla road route, not a general routing service.
// Parsing and compatibility checks complete before changing any runtime state.
struct RoadPreset {
    std::string originId,destinationId;
    Point origin,destination;
    double roadLength;
    std::vector<Point> points;
    RoadPreset():roadLength(0){}
    static void readPoint(std::istream& in,Point& p){
        if(!(in>>p.x>>p.y>>p.z)||!p.valid())throw std::runtime_error("invalid preset coordinate");
    }
    static RoadPreset read(std::istream& in){
        in.imbue(std::locale::classic());RoadPreset p;std::string magic;
        if(!(in>>magic)||magic!="MERCENARIE-ROAD-PRESET-1")throw std::runtime_error("invalid preset header");
        if(!(in>>p.originId>>p.destinationId)||p.originId.empty()||p.destinationId.empty()||p.originId.size()>200||p.destinationId.size()>200||p.originId==p.destinationId)throw std::runtime_error("invalid preset towns");
        readPoint(in,p.origin);readPoint(in,p.destination);
        unsigned int count=0;
        if(!(in>>p.roadLength>>count)||!_finite(p.roadLength)||p.roadLength<=0||p.roadLength>2000000||count<2||count>511)throw std::runtime_error("invalid preset length");
        double measured=0;Point last;
        for(unsigned int i=0;i<count;++i){Point point;readPoint(in,point);if(i)measured+=distance(last,point);last=point;
            if(p.points.empty()||distance(p.points.back(),point)>.01||fabs(p.points.back().y-point.y)>.01)p.points.push_back(point);
        }
        std::string extra;if(in>>extra)throw std::runtime_error("trailing preset data");
        if(in.bad()||p.points.size()<2||fabs(measured-p.roadLength)>.1)throw std::runtime_error("preset geometry mismatch");
        if(distance(p.origin,p.points.front())>500||distance(p.destination,p.points.back())>500)throw std::runtime_error("preset city access too long");
        return p;
    }
    bool matches(const std::string& from,const std::string& to,const Point& liveOrigin,const Point& liveDestination) const {
        return from==originId&&to==destinationId&&liveOrigin.valid()&&liveDestination.valid()&&distance(origin,liveOrigin)<=100&&distance(destination,liveDestination)<=100&&fabs(origin.y-liveOrigin.y)<=100&&fabs(destination.y-liveDestination.y)<=100;
    }
};
}
