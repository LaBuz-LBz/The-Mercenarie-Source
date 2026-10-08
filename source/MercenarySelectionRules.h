#pragma once
#include <algorithm>

namespace MercenarySelectionRules {
enum { MaximumSelected = 30 };
enum Category { All=0, Combat=1, Ranged=2, Guard=3, Work=4, Specialist=5 };
enum SelectResult { SelectAllowed, AlreadySelected, CharacterUnavailable, MaximumReached, LastCharacterRequired };

inline SelectResult maySelect(bool alreadySelected,bool available,int selectedCount,int availableCount)
{
    if(alreadySelected)return AlreadySelected;
    if(!available)return CharacterUnavailable;
    if(selectedCount>=MaximumSelected)return MaximumReached;
    if(selectedCount>=availableCount-1)return LastCharacterRequired;
    return SelectAllowed;
}

inline int categoryMask(float combat,float ranged,float guard,float work,float specialist)
{
    const float scores[5]={combat,ranged,guard,work,specialist};int mask=0;
    for(int i=0;i<5;++i)if(scores[i]>=20.0f)mask|=1<<(i+1);
    if(mask)return mask;int best=0;for(int i=1;i<5;++i)if(scores[i]>scores[best])best=i;
    return 1<<(best+1);
}

inline int primaryCategory(float combat,float ranged,float guard,float work,float specialist)
{
    const float scores[5]={combat,ranged,guard,work,specialist};int best=0;
    for(int i=1;i<5;++i)if(scores[i]>scores[best])best=i;
    return best+1;
}

inline bool visibleInFilter(int mask,int filter){return filter==All||(mask&(1<<filter))!=0;}
inline int columnsForWidth(int width,int preferredCardWidth=170,int gap=10){return std::max(1,width/(preferredCardWidth+gap));}
}
