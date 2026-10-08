#pragma once
#include <cmath>
#include <algorithm>
#include <vector>
// Engine-independent formation controller. It receives only the leader's
// position and roster state, never the mission's final destination or type.
namespace MissionFormation {
struct Point {float x,z;Point(float a=0,float b=0):x(a),z(b){}};
inline float distance2(Point a,Point b){float x=a.x-b.x,z=a.z-b.z;return x*x+z*z;}
inline Point slot(Point leader,Point heading,unsigned int index,bool narrow){
    float back=4.0f+(narrow?index:index/2)*3.5f;
    float side=narrow?0.0f:((index%2)?2.5f:-2.5f);
    return Point(leader.x-heading.x*back+heading.z*side,leader.z-heading.z*back-heading.x*side);
}
enum Action { None, Release, FollowLeader, MoveToSlot };
struct Member {bool issued,following;float retry,fallback,idle;Point target;Member():issued(false),following(false),retry(0),fallback(0),idle(0){}};
struct Group {
    Point previous,heading;bool positioned;std::vector<Member> members;
    Group():heading(0,1),positioned(false){}
    void reset(){*this=Group();}
    void position(Point leader){if(positioned){float d=distance2(leader,previous);if(d>1){float inv=1.0f/std::sqrt(d);heading=Point((leader.x-previous.x)*inv,(leader.z-previous.z)*inv);previous=leader;}}else{previous=leader;positioned=true;}}
    Action update(unsigned int index,float dt,Point leader,Point member,bool active,bool combat,bool narrow,bool failed,Point& target,bool moving=true){
        if(index>=members.size())members.resize(index+1);Member& state=members[index];
        if(!active||combat){bool owned=state.issued;state=Member();return owned?Release:None;}
        state.retry=std::max(0.0f,state.retry-dt);state.fallback=std::max(0.0f,state.fallback-dt);
        state.idle=moving?0:state.idle+dt;
        if(failed&&state.issued&&!state.following)state.fallback=12;
        bool catchup=distance2(leader,member)>45*45||state.fallback>0;
        target=slot(leader,heading,index,narrow);
        if(catchup){
            if(!state.issued||!state.following||(failed||state.idle>=8)&&state.retry<=0){state.issued=true;state.following=true;state.retry=8;state.idle=0;return FollowLeader;}
            return None;
        }
        if(state.following&&state.retry>0)return None;
        // Slots are relaxed targets. Do not reissue a path for tiny movements.
        if(state.issued&&!state.following&&distance2(member,target)<2.5f*2.5f)return None;
        if(!state.issued||state.following||state.retry<=0&&(failed||state.idle>=8||distance2(target,state.target)>6*6)){
            state.issued=true;state.following=false;state.target=target;state.retry=3;state.idle=0;return MoveToSlot;
        }
        return None;
    }
};
}
