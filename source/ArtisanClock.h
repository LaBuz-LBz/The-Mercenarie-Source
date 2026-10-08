#pragma once
#include "ArtisanOrders.h"
#include "WorldServiceClock.h"

// Compatibility envelope for absolute order timestamps. Runtime observes the
// shared service clock; this offset only records the saved timestamps' origin.
namespace ArtisanClock {
inline bool canObserveUi(bool window,bool saveSlot,bool changing,bool restoring){return window&&saveSlot&&!changing&&!restoring;}
inline double now(double world,double simulated){return WorldServiceClock::now(world,simulated);}
// Preserve remaining durations when reading older saves whose Artisan clock
// had a different origin from the common service clock. Runtime has no counter.
inline void alignToShared(ArtisanOrders::Ledger& ledger,double savedOffset,double sharedOffset){
 double delta=sharedOffset-savedOffset;
 if(delta==0)return;
 for(size_t i=0;i<ledger.orders.size();++i){ledger.orders[i].created+=delta;ledger.orders[i].start+=delta;ledger.orders[i].end+=delta;}
}
inline std::string save(const ArtisanOrders::Ledger& ledger,double simulated){
 if(!(simulated>=0&&simulated<=1e9))throw std::runtime_error("Invalid artisan clock");
 std::ostringstream s;s<<std::setprecision(17)<<"ARTISAN_CLOCK1 "<<simulated<<'\n'<<ArtisanOrders::save(ledger);return s.str();
}
inline ArtisanOrders::Ledger load(const std::string& bytes,double& simulated){
 simulated=0;
 if(bytes.compare(0,15,"ARTISAN_CLOCK1 ")!=0)return ArtisanOrders::load(bytes);
 size_t end=bytes.find('\n');if(end==std::string::npos)throw std::runtime_error("Truncated artisan clock");
 std::istringstream s(bytes.substr(15,end-15));double offset=0;
 if(!(s>>offset)||!(offset>=0&&offset<=1e9))throw std::runtime_error("Invalid artisan clock");
 s>>std::ws;if(!s.eof())throw std::runtime_error("Unexpected artisan clock data");
 ArtisanOrders::Ledger ledger=ArtisanOrders::load(bytes.substr(end+1));simulated=offset;return ledger;
}
}
