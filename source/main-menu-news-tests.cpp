#include "MainMenuNewsRules.h"
#include <cassert>
#include <iostream>
using namespace MainMenuNewsRules;
static bool inside(const Rect& r,int w,int h){return r.x>=0&&r.y>=0&&r.w>0&&r.h>0&&r.x+r.w<=w&&r.y+r.h<=h;}
int main(){
    assert(std::string(CurrentNewsVersion)=="V9"&&versionCount()==2);
    assert(shouldShowAutomatically("V8","V9",false));
    assert(!shouldShowAutomatically("V9","V9",false));
    assert(shouldShowAutomatically("V9","V10",false));
    std::string dismissal=dismissedPreference("dismissedNewsVersion=V8\n");assert(dismissal=="V8");
    assert(dismissedPreference("newsDismissedVersion=V8\nlastDismissedNewsVersion=V9\n")=="V9");
    int page=versionCount()-1;assert(std::string(Versions[page])=="V9");
    for(int i=0;i<100;++i){page=navigate(page,-1,versionCount());assert(std::string(Versions[page])=="V8");page=navigate(page,1,versionCount());assert(std::string(Versions[page])=="V9");assert(dismissal=="V8");}
    assert(navigate(0,-1,2)==0&&navigate(1,1,2)==1);
    assert(navigate(2,-1,3)==1&&navigate(1,-1,3)==0); // Future V10 retains older pages.
    assert(matchesLayoutName("00000000ABCDEF00_ImportGameButton","ImportGameButton"));
    assert(matchesLayoutName("ImportGameButton","ImportGameButton"));
    assert(!matchesLayoutName("OtherImportGameButton","ImportGameButton"));
    assert(!matchesLayoutName("00000000ABCDEF00_NewGameButton","ImportGameButton"));
    assert(!matchesLayoutName("","ImportGameButton"));
    assert(shouldShowAutomatically("", "V8", false));
    assert(!shouldShowAutomatically("V8", "V8", false));
    assert(shouldShowAutomatically("V8", "V8.1", false));
    assert(shouldShowAutomatically("V8.1", "V9", false));
    assert(!shouldShowAutomatically("", "V8", true));
    assert(isLegacyDismissed("dismissedNewsVersion","V8","V8"));
    const int views[][2]={{1280,720},{1920,1080},{2560,1440},{3440,1440},{3840,2160}};
    for(int i=0;i<5;++i){Layout l=calculate(views[i][0],views[i][1]);assert(inside(l.popup,views[i][0],views[i][1]));assert(l.popup.w==1120&&l.popup.h==660);assert(l.banner.x>=0&&l.content.x>l.banner.x+l.banner.w);assert(l.footer.y>=l.content.y+l.content.h);assert(l.cardHeight==108);assert((l.content.w-l.cardGap)/2==398);assert(l.header.y+l.header.h<l.content.y);assert(l.banner.y+l.banner.h<l.footer.y);assert(l.changelog.x+l.changelog.w<l.proceed.x);assert(inside(l.close,l.popup.w,l.popup.h));}
    Rect right=placeTooltip(Rect(1100,620,140,70),320,170,1280,720);assert(inside(right,1280,720));assert(right.x<1100&&right.y<620);
    Rect normal=placeTooltip(Rect(100,100,200,90),320,170,1920,1080);assert(normal.x>300&&normal.y==100);
    std::cout<<"PASS: news version policy, session guard, legacy preference, tooltip fallback and five responsive resolutions.\n";
}
