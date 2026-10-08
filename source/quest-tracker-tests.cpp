#include "QuestTrackerLayout.h"
#include <cassert>
#include <iostream>
int main(){using namespace QuestTrackerLayout;assert(forCount(0,1000).height==200);assert(forCount(1,1000).height==212);for(int i=1;i<=5;++i){Size s=forCount(i,1000);assert(s.height==108+i*104&&!s.scroll);}for(int i=6;i<100;++i){Size s=forCount(i,1000);assert(s.height==628&&s.scroll);}assert(forCount(3,1000).height<forCount(4,1000).height);assert(forCount(5,250).body==250&&forCount(5,250).scroll);assert(forCount(2,250).body==200&&!forCount(2,250).scroll);std::cout<<"PASS: 0/1/2/3/4/5/6/99 quests, shrink, screen bounds, scroll threshold; language-independent sizing\n";}
