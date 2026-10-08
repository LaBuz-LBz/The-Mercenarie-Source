#include <cassert>
#include <iostream>
#include "GuildLevelUnlocks.h"
int main(){
    std::vector<GuildLevelUI::Transition> q;
    GuildLevelUI::enqueue(q,0,4);
    assert(q.size()==4);
    for(int i=0;i<4;++i){assert(q[i].oldLevel==i);assert(q[i].newLevel==i+1);}
    GuildLevelUI::enqueue(q,2,4);assert(q.size()==4);
    GuildLevelUI::enqueue(q,4,3);assert(q.size()==4);
    q.clear();GuildLevelUI::enqueue(q,-3,12);assert(q.size()==10);
    assert(GuildLevelUI::unlocks(1).size()==3);
    assert(std::string(GuildLevelUI::unlocks(1)[2].fr)==Loc::text("artisan.unlock.title"));
    assert(GuildLevelUI::unlocks(0).empty());
    assert(GuildLevelUI::unlocks(3).size()==2);
    assert(GuildLevelUI::unlocks(5).empty());
    assert(GuildLevelUI::unlocks(10).size()==1);
    const int sizes[][2]={{800,600},{1280,720},{1366,768},{1920,1080},{2560,1440},{3840,2160}};
    for(int r=0;r<6;++r)for(int count=0;count<=10;++count){
        GuildLevelUI::Layout l(sizes[r][0],sizes[r][1],count);
        assert(l.width<=sizes[r][0]-28 && l.height<=sizes[r][1]-28);
        assert(l.areaHeight>=l.cardHeight);
        assert(l.quoteY+int(36*l.scale)<=l.buttonY-int(40*l.scale));
        assert(l.buttonY+int(56*l.scale)<=l.height);
    }
    std::cout<<"Level-up transitions, unlocks and layout: PASS\n";
}
