#pragma once
#include <algorithm>
// Gross includes the advance, final balance, paid bonuses and tips.
// Never tax a bonus or tip, even if a caller passes an inflated contract value.
inline int contractTaxBase(int contractualPay,int gross,int bonuses,int tips)
{
    long long base=(long long)gross-std::max(0,bonuses)-std::max(0,tips);
    return static_cast<int>(std::max<long long>(0,std::min<long long>(std::max(0,contractualPay),base)));
}
