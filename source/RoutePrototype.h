#pragma once
#include "MissionArchive.h"
#include <cmath>
#include <float.h>

// Experimental only: consumes explicit waypoints, never invents a native route.
// No game objects, orders, payments or existing save formats are changed here.
namespace RoutePrototype {
struct Point {
    double x,y,z;
    Point(double a=0,double b=0,double c=0):x(a),y(b),z(c){}
    bool valid() const {return _finite(x)&&_finite(y)&&_finite(z)&&fabs(x)<10000000&&fabs(y)<10000000&&fabs(z)<10000000;}
};
inline double distance(const Point& a,const Point& b){double x=a.x-b.x,z=a.z-b.z;return sqrt(x*x+z*z);}
// Records observed samples only. A discontinuity must be reviewed, not joined
// into a supposedly traversable segment (teleport, unloaded actor or long gap).
enum SampleResult { SampleIgnored, SampleAdded, SampleGap, SampleFull };
inline SampleResult recordSample(std::vector<Point>& points,const Point& p){
    if(!p.valid())return SampleGap;
    if(points.size()>=511)return SampleFull;
    if(!points.empty()){
        double d=distance(points.back(),p);
        if(d>500||fabs(points.back().y-p.y)>100)return SampleGap;
        if(d<60&&fabs(points.back().y-p.y)<10)return SampleIgnored;
    }
    points.push_back(p);return SampleAdded;
}
enum Status { Idle, Travelling, Blocked, Complete };
enum Action { NoOrder, MoveToPoint, StopForReview, Arrived };
struct Route {
    std::string missionId;
    std::vector<Point> points;
    unsigned int next;
    Status status;
    double stalled,best;
    bool issued;
    Route():next(0),status(Idle),stalled(0),best(-1),issued(false){}
    void validate() const {
        if(points.size()>512||next>points.size()||status<Idle||status>Complete||!_finite(stalled)||stalled<0||!_finite(best)||best< -1)throw std::runtime_error("invalid prototype route");
        if(status!=Idle&&(missionId.empty()||points.empty()))throw std::runtime_error("missing route identity");
        if((status==Travelling||status==Blocked)&&next>=points.size())throw std::runtime_error("invalid route cursor");
        if(status==Complete&&next!=points.size())throw std::runtime_error("unfinished route");
        for(size_t i=0;i<points.size();++i)if(!points[i].valid())throw std::runtime_error("invalid route coordinate");
    }
    void start(const std::string& id,const std::vector<Point>& p){
        Route candidate;candidate.missionId=id;candidate.points=p;candidate.status=Travelling;candidate.validate();*this=candidate;
    }
    double lengthFrom(const Point& origin) const {
        if(!origin.valid())throw std::runtime_error("invalid origin");validate();
        double total=0;Point last=origin;for(size_t i=0;i<points.size();++i){total+=distance(last,points[i]);last=points[i];}return total;
    }
    // Call only with a resolved actor and elapsed game time. Suspensions include
    // Wait Here, Follow Me, combat, unconsciousness and waiting for the player.
    Action tick(const Point& position,double dt,bool suspended,bool pathFailed){
        validate();if(!position.valid()||!_finite(dt)||dt<0)throw std::runtime_error("invalid route tick");
        if(status!=Travelling)return NoOrder;
        if(suspended){issued=false;stalled=0;best=-1;return NoOrder;}
        while(next<points.size()&&distance(position,points[next])<=40&&fabs(position.y-points[next].y)<=20){++next;issued=false;stalled=0;best=-1;}
        if(next==points.size()){status=Complete;return Arrived;}
        // Ignore failure of an old/manual order before our current order exists.
        if(issued&&pathFailed){status=Blocked;return StopForReview;}
        double d=distance(position,points[next]);
        if(best<0||d<best-2){best=d;stalled=0;}else if(issued)stalled+=dt;
        if(stalled>=90){status=Blocked;return StopForReview;}
        if(!issued){issued=true;return MoveToPoint;}return NoOrder;
    }
    void retry(){if(status==Blocked){status=Travelling;issued=false;best=-1;stalled=0;}}
    void cancel(){*this=Route();}
    std::string save() const {
        validate();MissionArchive a;std::string magic="MERCENARIE-ROUTE-PROTOTYPE-1",id=missionId;a.field(magic);a.field(id);
        unsigned int n=(unsigned int)points.size(),cursor=next;int state=status;a.field(n);
        for(size_t i=0;i<points.size();++i){double x=points[i].x,y=points[i].y,z=points[i].z;a.field(x);a.field(y);a.field(z);}
        a.field(cursor);a.field(state);unsigned int hash=missionChecksum(a.bytes);a.field(hash);return a.bytes;
    }
    static Route load(const std::string& bytes,const std::string& expectedMission){
        MissionArchive a(bytes);Route r;std::string magic;a.field(magic);if(magic!="MERCENARIE-ROUTE-PROTOTYPE-1")throw std::runtime_error("route version");
        a.field(r.missionId);if(r.missionId!=expectedMission)throw std::runtime_error("wrong mission route");unsigned int n=0;a.field(n);if(n>512)throw std::runtime_error("route too long");
        r.points.resize(n);for(size_t i=0;i<n;++i){a.field(r.points[i].x);a.field(r.points[i].y);a.field(r.points[i].z);}
        a.field(r.next);int state=0;a.field(state);r.status=(Status)state;size_t payload=a.cursor;unsigned int hash=0;a.field(hash);a.finish();
        if(hash!=missionChecksum(bytes.substr(0,payload)))throw std::runtime_error("route checksum");r.validate();return r;
    }
};
}
