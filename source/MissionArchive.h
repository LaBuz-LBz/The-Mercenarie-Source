#pragma once
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

// Versioned little-endian x64 save sidecar. Never serialise an object layout,
// pointer, STL container or engine handle as raw memory.
struct MissionArchive {
    // The payroll extension retains individual bills and repayment history.
    // Keep a bounded parser while allowing long-running guild saves.
    enum { MaximumBytes=64*1024*1024 };
    bool reading;
    std::string bytes;
    size_t cursor;
    size_t budget;
    explicit MissionArchive(size_t limit=MaximumBytes):reading(false),cursor(0),budget(limit){}
    explicit MissionArchive(const std::string& b,size_t limit=MaximumBytes):reading(true),bytes(b),cursor(0),budget(limit){if(b.size()>budget)throw std::runtime_error("oversized total archive");}
    size_t remaining() const {return cursor<=bytes.size()?bytes.size()-cursor:0;}
    void requireElements(size_t n,size_t minimum)const{if(reading&&(minimum==0||n>remaining()/minimum))throw std::runtime_error("collection exceeds remaining archive bytes");}
    void reserveWrite(size_t n){if(bytes.size()>budget||n>budget-bytes.size())throw std::runtime_error("oversized total archive");}
    template<class T> void field(T& v) {
        BOOST_STATIC_ASSERT((boost::is_arithmetic<T>::value || boost::is_enum<T>::value));
        if(reading){if(cursor+sizeof(T)>bytes.size())throw std::runtime_error("truncated mission");std::memcpy(&v,bytes.data()+cursor,sizeof(T));cursor+=sizeof(T);}
        else {reserveWrite(sizeof(T));bytes.append(reinterpret_cast<const char*>(&v),sizeof(T));}
    }
    void field(float& v){if(!reading&&!(v>=-3.402823466e38F&&v<=3.402823466e38F))throw std::runtime_error("nonfinite float");field<float>(v);if(reading&&!(v>=-3.402823466e38F&&v<=3.402823466e38F))throw std::runtime_error("nonfinite float");}
    void field(double& v){if(!reading&&!(v>=-1.7976931348623157e308&&v<=1.7976931348623157e308))throw std::runtime_error("nonfinite double");field<double>(v);if(reading&&!(v>=-1.7976931348623157e308&&v<=1.7976931348623157e308))throw std::runtime_error("nonfinite double");}
    void field(bool& v){unsigned char b=v?1:0;field(b);if(b>1)throw std::runtime_error("invalid boolean");if(reading)v=b!=0;}
    void field(std::string& v){if(!reading&&v.size()>MaximumBytes)throw std::runtime_error("oversized string");unsigned int n=static_cast<unsigned int>(v.size());field(n);if(n>MaximumBytes)throw std::runtime_error("oversized string");if(reading){if(cursor+n>bytes.size())throw std::runtime_error("truncated string");v.assign(bytes,cursor,n);cursor+=n;}else{reserveWrite(n);bytes.append(v);}}
    template<class T> void field(std::vector<T>& v){unsigned int n=static_cast<unsigned int>(v.size());field(n);if(n>4096)throw std::runtime_error("oversized list");requireElements(n,1);if(reading)v.resize(n);for(unsigned int i=0;i<n;++i)field(v[i]);}
    void finish(){if(reading&&cursor!=bytes.size())throw std::runtime_error("unexpected trailing data");}
};
inline unsigned int missionChecksum(const std::string& s){unsigned int h=2166136261u;for(size_t i=0;i<s.size();++i){h^=static_cast<unsigned char>(s[i]);h*=16777619u;}return h;}
inline std::string sealMissionArchive(const std::string& payload){MissionArchive a(MissionArchive::MaximumBytes+1024);std::string magic="MERCENARIE-MISSION-V4-1";a.field(magic);std::string p=payload;a.field(p);unsigned int hash=missionChecksum(p);a.field(hash);return a.bytes;}
inline std::string openMissionArchive(const std::string& bytes){MissionArchive a(bytes,MissionArchive::MaximumBytes+1024);std::string magic,p;a.field(magic);if(magic!="MERCENARIE-MISSION-V4-1")throw std::runtime_error("unsupported mission version");a.field(p);unsigned int hash=0;a.field(hash);a.finish();if(hash!=missionChecksum(p))throw std::runtime_error("mission checksum mismatch");return p;}
