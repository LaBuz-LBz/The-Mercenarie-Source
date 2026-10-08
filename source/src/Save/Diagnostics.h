#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "../../SaveBuild.generated.h"

// Stable support codes. Never renumber an existing reason; unknown errors retain
// their full detail in the log. The prefix identifies the operation, not the cause.
namespace SaveDiagnostics {
enum Reason {
    ProgressUnavailable=1, EstateUnavailable=2, EstateUpdateFailed=3,
    InvalidMission=4, InvalidQuest=5, InvalidSelection=6, TooManyQuests=7,
    InvalidBonus=8, InvalidPace=9, BountySuspended=10, UnsupportedVersion=11,
    Checksum=12, Truncated=13, TrailingData=14, SizeLimit=15,
    InvalidBoolean=16, ActorTimeout=17, ActorIdentity=18,
    FinancialRestore=19, ProgressInvalid=20, ArtisanInvalid=21,
    EstateInvalid=22, PayrollInvalid=23, ArchiveInvalid=24,
    ReadFailed=25, DirectoryFailed=26, WriteFailed=27,
    ArtisanBusy=28, HookUnavailable=29, NativeFailed=30,
    ImportStageFailed=31, FiscalWriteFailed=33, ProgressWriteFailed=34,
    BoardsWriteFailed=35, AccessDenied=36, Locked=37, PrepareWrite=38,
    PendingPublication=39, ProgressLimit=40, Unknown=99
};
inline bool has(const std::string& s,const char* value){return s.find(value)!=std::string::npos;}
inline Reason classify(const std::string& s){
    if(has(s,"publication still pending")||has(s,"restoration not complete"))return PendingPublication;
    if(has(s,"rewardedContractIds limit")||has(s,"progression byte limit"))return ProgressLimit;
    if(s=="guild progress unavailable: refusing to save zero over original")return ProgressUnavailable;
    if(s=="estate state unavailable during save"||s=="estate unavailable")return EstateUnavailable;
    if(s=="estate update failed during save")return EstateUpdateFailed;
    if(has(s,"invalid mission state"))return InvalidMission;
    if(s=="invalid quest context"||s=="invalid bounty quest")return InvalidQuest;
    if(s=="invalid selected quest")return InvalidSelection;
    if(s=="too many active quests")return TooManyQuests;
    if(s=="bounty state suspended")return BountySuspended;
    if(has(s,"unsupported")||has(s,"unknown guild progression extension")||has(s,"invalid bonus version")||has(s,"invalid platoon prototype extension")||has(s,"estate schema"))return UnsupportedVersion;
    if(has(s,"checksum"))return Checksum;
    if(has(s,"truncated")||has(s,"Truncated"))return Truncated;
    if(has(s,"trailing data")||has(s,"Unexpected artisan"))return TrailingData;
    if(has(s,"oversized")||has(s,"too large")||has(s,"too many bounty boards")||s=="invalid sidecar size")return SizeLimit;
    if(s=="invalid boolean")return InvalidBoolean;
    if(has(s,"invalid bonus"))return InvalidBonus;
    if(has(s,"rescue pace"))return InvalidPace;
    if(has(s,"EN_MISSION restore timeout"))return ActorTimeout;
    if(has(s,"EN_MISSION"))return ActorIdentity;
    if(s=="cannot restore financial snapshot")return FinancialRestore;
    if(has(s,"progression")||has(s,"guild progress"))return ProgressInvalid;
    if(has(s,"artisan")||has(s,"saved order")||has(s,"Order exceeds")||has(s,"equipment"))return ArtisanInvalid;
    if(has(s,"estate"))return EstateInvalid;
    if(has(s,"payroll"))return PayrollInvalid;
    if(s=="sidecar read failed")return ReadFailed;
    if(has(s,"could not be staged")||has(s,"session directory unavailable"))return ImportStageFailed;
    if(has(s,"invalid")||has(s,"Invalid")||has(s,"duplicate"))return ArchiveInvalid;
    return Unknown;
}
struct Failure {
    std::string operation,stage,component,path,detail,data,session;
    unsigned long win32;
    unsigned long long size,count;
    Reason reason;
    Failure():win32(0),size(0),count(0),reason(Unknown){}
};
inline std::string describe(const Failure& f){std::ostringstream s;s<<f.detail<<" operation="<<f.operation<<" stage="<<f.stage<<" component="<<f.component<<" data="<<f.data<<" path="<<f.path<<" win32="<<f.win32<<" size="<<f.size<<" count="<<f.count;return s.str();}
struct Error:std::runtime_error {
    Failure failure;
    explicit Error(const Failure& f):std::runtime_error(describe(f)),failure(f){}
};
struct Operation {
    std::string id,phase,component,data,path;
    Failure first;
    bool failed,nativeCalled,nativeSucceeded,notified;
    Operation():phase("prepare"),component("missions"),failed(false),nativeCalled(false),nativeSucceeded(false),notified(false){}
    void remember(Failure f){if(failed)return;if(f.operation.empty())f.operation="save";if(f.stage.empty())f.stage=phase;if(f.component.empty())f.component=component;if(f.data.empty())f.data=data;if(f.path.empty())f.path=path;f.session=id;first=f;failed=true;}
};
inline Failure exceptionFailure(const std::exception& e){const Error* structured=dynamic_cast<const Error*>(&e);if(structured)return structured->failure;Failure f;f.detail=e.what();f.reason=classify(f.detail);return f;}
inline std::string code(const std::string& operation,Reason reason){
    std::ostringstream out;out<<"TM-"<<operation<<"-"<<std::setfill('0')<<std::setw(3)<<(int)reason;return out.str();
}
inline Reason writeReason(const std::string& path){
    if(has(path,"GuildEscortFiscal.dat"))return FiscalWriteFailed;
    if(has(path,"GuildEscortReputation.dat"))return ProgressWriteFailed;
    if(has(path,"GuildEscortBoards.dat"))return BoardsWriteFailed;
    return WriteFailed;
}
inline std::string clean(std::string s){for(size_t i=0;i<s.size();++i)if(s[i]=='\r'||s[i]=='\n'||s[i]=='\t')s[i]=' ';return s;}
inline std::string playerMessage(const std::string& message,const std::string& id){return message+"\n["+id+"]";}
inline std::string record(const std::string& id,const std::string& incident,const std::string& detail,const std::string& path){
    return std::string("Mercenarie persistence: build=")+MercenarieSaveBuild+" code="+id+" incident="+incident+" detail="+clean(detail)+" path="+clean(path);
}
}
