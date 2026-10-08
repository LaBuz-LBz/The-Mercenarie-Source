#pragma once
#include <algorithm>
namespace QuestTrackerLayout {
struct Size {int body,height;bool scroll;};
inline Size forCount(int count,int room){Size s;count=std::max(0,count);s.body=std::min(count?std::min(5,count)*104-8:84,std::max(96,room));s.height=76+s.body+40;s.scroll=count*104-8>s.body;return s;}
}
