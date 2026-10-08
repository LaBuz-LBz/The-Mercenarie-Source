#pragma once
#include <algorithm>
#include <string>
#include <sstream>

namespace MainMenuNewsRules {

static const char* const CurrentNewsVersion = "V9";
static const char* const Versions[]={"V8","V9"};
inline int versionCount(){return sizeof(Versions)/sizeof(Versions[0]);}
inline int navigate(int page,int delta,int count){return std::max(0,std::min(count-1,page+delta));}
inline std::string trimPreference(const std::string& s){size_t a=s.find_first_not_of(" \t\r"),b=s.find_last_not_of(" \t\r");return a==std::string::npos?"":s.substr(a,b-a+1);}
inline std::string dismissedPreference(const std::string& bytes){
    std::istringstream in(bytes);std::string line,legacy,current;bool found=false;
    while(std::getline(in,line)){
        if(line.compare(0,3,"\xEF\xBB\xBF")==0)line.erase(0,3);
        size_t p=line.find('=');if(p==std::string::npos)continue;
        std::string key=trimPreference(line.substr(0,p)),value=trimPreference(line.substr(p+1));
        if(key=="lastDismissedNewsVersion"){current=value;found=true;}
        else if(key=="dismissedNewsVersion"||key=="newsDismissedVersion")legacy=value;
    }
    return found?current:legacy;
}

struct Rect { int x,y,w,h; Rect(int px=0,int py=0,int pw=0,int ph=0):x(px),y(py),w(pw),h(ph){} };
struct Layout {
    Rect popup,banner,content,footer,header,check,changelog,proceed,close;
    int scale,cardGap,cardHeight;
};

inline Layout calculate(int viewportW,int viewportH){
    Layout out;
    out.scale=std::min(100,std::min((viewportW-32)*100/1120,(viewportH-32)*100/660));
    out.popup.w=1120*out.scale/100;
    out.popup.h=660*out.scale/100;
    out.popup.x=(viewportW-out.popup.w)/2;out.popup.y=(viewportH-out.popup.h)/2;
    const int s=out.scale;
    out.banner=Rect(24*s/100,24*s/100,240*s/100,548*s/100);
    out.header=Rect(288*s/100,34*s/100,764*s/100,142*s/100);
    out.content=Rect(288*s/100,208*s/100,808*s/100,348*s/100);
    out.footer=Rect(24*s/100,596*s/100,1072*s/100,44*s/100);
    out.check=Rect(24*s/100,600*s/100,36*s/100,36*s/100);
    out.changelog=Rect(604*s/100,596*s/100,292*s/100,44*s/100);
    out.proceed=Rect(912*s/100,596*s/100,184*s/100,44*s/100);
    out.close=Rect(1064*s/100,8*s/100,44*s/100,44*s/100);
    out.cardGap=12*out.scale/100;
    out.cardHeight=(out.content.h-out.cardGap*2)/3;
    return out;
}

inline Rect placeTooltip(const Rect& anchor,int width,int height,int screenW,int screenH){
    const int gap=12,edge=8;
    int x=anchor.x+anchor.w+gap,y=anchor.y;
    if(x+width>screenW-edge)x=anchor.x-width-gap;
    if(x<edge)x=std::max(edge,std::min(screenW-width-edge,anchor.x));
    if(y+height>screenH-edge)y=anchor.y-height-gap;
    if(y<edge)y=edge;
    return Rect(x,y,width,height);
}

inline bool shouldShowAutomatically(const std::string& dismissed,const std::string& current,bool handledThisSession){
    return !handledThisSession && dismissed!=current;
}

// BaseLayout prefixes every widget name with its instance address and '_'.
inline bool matchesLayoutName(const std::string& actual,const std::string& name){
    return actual==name||(actual.size()>name.size()&&
        actual[actual.size()-name.size()-1]=='_'&&
        actual.compare(actual.size()-name.size(),name.size(),name)==0);
}

inline bool isLegacyDismissed(const std::string& key,const std::string& value,const std::string& current){
    return (key=="dismissedNewsVersion"||key=="newsDismissedVersion")&&value==current;
}

}
