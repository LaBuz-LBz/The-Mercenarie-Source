#pragma once
#include "../Platform/UnicodeFileSystem.h"
#include "../../MissionArchive.h"
#include "Diagnostics.h"
#include "../../CleanupState.h"
#include <iomanip>
#include <algorithm>
#include <vector>
#include "../../PerformanceAudit.h"

// Durable journal lives OUTSIDE the native slot: Kenshi may replace its files.
// All fingerprints cover bytes, not timestamps or the existence of quick.save.
namespace SaveTransaction {
#ifdef MERCENARIE_TRANSACTION_TEST
static void (*testCheckpoint)(const char*)=0;
inline void checkpoint(const char* phase){if(testCheckpoint)testCheckpoint(phase);}
#else
inline void checkpoint(const char*){}
#endif
typedef unsigned long long Hash;
inline Hash hashBytes(Hash h,const char* bytes,size_t count){for(size_t i=0;i<count;++i){h^=(unsigned char)bytes[i];h*=1099511628211ull;}return h;}
inline std::string hex(Hash h){std::ostringstream out;out<<std::hex<<std::setw(16)<<std::setfill('0')<<h;return out.str();}
inline std::string canonical(const std::string& path){
    const std::wstring p=UnicodeFileSystem::preferredPath(path);DWORD size=GetFullPathNameW(p.c_str(),0,0,0);
    if(!size)throw std::runtime_error("transaction: invalid absolute path");
    std::vector<wchar_t> buffer(size+1);DWORD n=GetFullPathNameW(p.c_str(),(DWORD)buffer.size(),&buffer[0],0);
    if(!n||n>=buffer.size())throw std::runtime_error("transaction: path resolution failed");
    std::wstring full(&buffer[0],n);while(full.size()>3&&full[full.size()-1]==L'\\')full.resize(full.size()-1);
    std::wstring lower(full.size(),L'\0');if(!LCMapStringW(LOCALE_INVARIANT,LCMAP_LOWERCASE,full.data(),(int)full.size(),&lower[0],(int)lower.size()))throw std::runtime_error("transaction: path case mapping failed");return UnicodeFileSystem::utf8(lower);
}
inline void require(const UnicodeFileSystem::Result& r){if(!r.ok){
    SaveDiagnostics::Failure f;f.operation=r.operation;f.stage=r.stage;f.path=r.path;f.win32=r.error;f.detail=r.detail;f.size=r.size;f.component="transaction";
    f.reason=r.status==UnicodeFileSystem::ReadAccessDenied?SaveDiagnostics::AccessDenied:r.status==UnicodeFileSystem::ReadLocked?SaveDiagnostics::Locked:r.status==UnicodeFileSystem::ReadTooLarge?SaveDiagnostics::SizeLimit:r.status==UnicodeFileSystem::ReadTruncated?SaveDiagnostics::Truncated:r.operation=="read"?SaveDiagnostics::ReadFailed:SaveDiagnostics::WriteFailed;
    throw SaveDiagnostics::Error(f);
}}
inline std::string read(const std::string& path,bool optional=false){std::string bytes;UnicodeFileSystem::Result r=UnicodeFileSystem::readDetailed(path,bytes,MissionArchive::MaximumBytes+1024);if(optional&&r.status==UnicodeFileSystem::ReadMissing)return "";require(r);return bytes;}
inline void write(const std::string& path,const std::string& bytes){checkpoint("before-write");require(UnicodeFileSystem::atomicWrite(path,bytes));checkpoint("after-write");}
inline std::string parent(const std::string& path){return path.substr(0,path.find_last_of("/\\"));}
inline bool exists(const std::string& path){DWORD a=UnicodeFileSystem::attributes(path);if(a!=INVALID_FILE_ATTRIBUTES)return true;DWORD e=GetLastError();if(!UnicodeFileSystem::absent(e))require(UnicodeFileSystem::failure("read","attributes",path,e,"cannot inspect transaction path"));return false;}
inline void list(const std::string& base,const std::string& relative,std::vector<std::string>& files,bool nativeOnly){
    const std::string directory=relative.empty()?base:base+"/"+relative;
    DWORD attrs=UnicodeFileSystem::attributes(directory);
    if(attrs==INVALID_FILE_ATTRIBUTES){DWORD e=GetLastError();if(relative.empty()&&UnicodeFileSystem::absent(e))return;require(UnicodeFileSystem::failure("read","scan",directory,e,"cannot inspect directory"));}
    if((attrs&FILE_ATTRIBUTE_REPARSE_POINT)||!(attrs&FILE_ATTRIBUTE_DIRECTORY))throw std::runtime_error("transaction: unsafe save directory "+directory);
    WIN32_FIND_DATAW entry;HANDLE raw=FindFirstFileW(UnicodeFileSystem::existingPath(directory+"/*").c_str(),&entry);
    if(raw==INVALID_HANDLE_VALUE){DWORD e=GetLastError();if(e==ERROR_FILE_NOT_FOUND)return;require(UnicodeFileSystem::failure("read","scan",directory,e,"cannot list save"));}
    struct Search {HANDLE h;Search(HANDLE value):h(value){}~Search(){FindClose(h);}} search(raw);
    do{
        std::wstring name=entry.cFileName;if(name==L"."||name==L"..")continue;
        const std::string utf=UnicodeFileSystem::utf8(name),path=relative.empty()?utf:relative+"/"+utf;
        if(nativeOnly&&relative.empty()&&_wcsicmp(name.c_str(),L"TheMercenarie")==0)continue;
        if(!nativeOnly&&_stricmp(path.c_str(),"TheMercenarie/Generation.v1")==0)continue;
        if(entry.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)throw std::runtime_error("transaction: reparse point in save "+path);
        if(entry.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)list(base,path,files,nativeOnly);else files.push_back(path);
        if(files.size()>200000)throw std::runtime_error("transaction: too many save files");
    }while(FindNextFileW(raw,&entry));
    DWORD error=GetLastError();if(error!=ERROR_NO_MORE_FILES)require(UnicodeFileSystem::failure("read","scan",directory,error,"save enumeration incomplete"));
}
inline Hash fileHash(const std::string& path){
    HANDLE raw=CreateFileW(UnicodeFileSystem::existingPath(path).c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(raw==INVALID_HANDLE_VALUE)require(UnicodeFileSystem::failure("read","fingerprint",path,GetLastError(),"cannot lock save file for reading"));
    UnicodeFileSystem::Handle file(raw);Hash h=14695981039346656037ull;char buffer[65536];DWORD n=0;
    do{if(!ReadFile(raw,buffer,sizeof(buffer),&n,0))require(UnicodeFileSystem::failure("read","fingerprint",path,GetLastError(),"cannot fingerprint save file"));h=hashBytes(h,buffer,n);}while(n);
    return h;
}
inline std::string fingerprint(const std::string& slot,bool nativeOnly=false){
    MercenariePerf::Phase perf("fingerprint");
    std::vector<std::string> files;list(slot,"",files,nativeOnly);std::sort(files.begin(),files.end());
    Hash h=14695981039346656037ull;
    for(size_t i=0;i<files.size();++i){h=hashBytes(h,files[i].data(),files[i].size());h=hashBytes(h,"\0",1);const std::string digest=hex(fileHash(slot+"/"+files[i]));h=hashBytes(h,digest.data(),digest.size());}
    return hex(h);
}
struct Fingerprints {std::string complete,native;};
inline Fingerprints fingerprints(const std::string& slot){
    MercenariePerf::Phase perf("fingerprints-paired");
    std::vector<std::string> files;list(slot,"",files,false);std::sort(files.begin(),files.end());
    Hash full=14695981039346656037ull,native=full;
    for(size_t i=0;i<files.size();++i){
        const std::string& name=files[i];const std::string digest=hex(fileHash(slot+"/"+name));
        full=hashBytes(full,name.data(),name.size());full=hashBytes(full,"\0",1);full=hashBytes(full,digest.data(),digest.size());
        if(_strnicmp(name.c_str(),"TheMercenarie/",sizeof("TheMercenarie/")-1)!=0){native=hashBytes(native,name.data(),name.size());native=hashBytes(native,"\0",1);native=hashBytes(native,digest.data(),digest.size());}
    }
    Fingerprints result;result.complete=hex(full);result.native=hex(native);return result;
}
enum Phase { Prepared=1, NativeSaving=2, NativeConfirmed=3, Committed=4, NativeFailed=5 };
struct Record {
    std::string slot,generation,previousGeneration,nativeDigest,completeDigest,parts[4];int phase;bool hasPrevious;
    Record():phase(Prepared),hasPrevious(false){}
    std::string encode()const{
        MissionArchive a;std::string magic="MERCENARIE-TRANSACTION-1";a.field(magic);Record r=*this;
        a.field(r.slot);a.field(r.generation);a.field(r.previousGeneration);a.field(r.phase);a.field(r.hasPrevious);a.field(r.nativeDigest);a.field(r.completeDigest);
        for(int i=0;i<4;++i)a.field(r.parts[i]);return sealMissionArchive(a.bytes);
    }
    static bool validGeneration(const std::string& value){return !value.empty()&&value.size()<100&&value.find_first_not_of("0123456789-")==std::string::npos;}
    static Record decode(const std::string& bytes){
        MissionArchive a(openMissionArchive(bytes));std::string magic;Record r;a.field(magic);
        if(magic!="MERCENARIE-TRANSACTION-1")throw std::runtime_error("transaction: unsupported journal");
        a.field(r.slot);a.field(r.generation);a.field(r.previousGeneration);a.field(r.phase);a.field(r.hasPrevious);a.field(r.nativeDigest);a.field(r.completeDigest);
        for(int i=0;i<4;++i)a.field(r.parts[i]);a.finish();
        if(!validGeneration(r.generation)||(!r.previousGeneration.empty()&&!validGeneration(r.previousGeneration))||r.phase<Prepared||r.phase>NativeFailed)throw std::runtime_error("transaction: invalid journal state");return r;
    }
};
inline const char* partName(int i){static const char* names[]={"Mission.v4","GuildEscortReputation.dat","GuildEscortFiscal.dat","GuildEscortBoards.dat"};return names[i];}
inline std::string home(const std::string& slot){const std::string path=canonical(slot);return parent(path)+"/.TheMercenarieRecovery/"+hex(hashBytes(14695981039346656037ull,path.data(),path.size()));}
inline std::string directory(const Record& r){return home(r.slot)+"/"+r.generation;}
inline void journal(const Record& r){write(home(r.slot)+"/Current.v1",r.encode());}
inline Record current(const std::string& slot,bool& found){const std::string path=home(slot)+"/Current.v1";found=exists(path);Record r;if(found){r=Record::decode(read(path));if(r.slot!=canonical(slot))throw std::runtime_error("transaction: journal slot mismatch");}return r;}
inline bool validCommitted(const std::string& slot){
    const std::string path=slot+"/TheMercenarie/Generation.v1";if(!exists(path)){if(exists(slot+"/TheMercenarie/NativeGeneration.v1"))throw std::runtime_error("transaction: native generation has no completed companions: "+slot);return false;}const std::string bytes=read(path);
    Record r=Record::decode(bytes);const Fingerprints observed=fingerprints(slot);if(r.phase!=Committed||r.completeDigest!=observed.complete||r.nativeDigest!=observed.native)throw std::runtime_error("transaction: saved generation is incomplete or modified: "+slot);
    return true; // Slot can be copied/renamed: bind to bytes, not its old path.
}
inline void backup(const std::string& source,const std::string& destination){
    MercenariePerf::Phase perf("backup-verified");
    const std::string before=fingerprint(source);std::vector<std::string> files;list(source,"",files,false);
    if(exists(source+"/TheMercenarie/Generation.v1"))files.push_back("TheMercenarie/Generation.v1");
    for(size_t i=0;i<files.size();++i){checkpoint("before-copy");const std::string target=destination+"/"+files[i];if(!UnicodeFileSystem::ensureDirectory(parent(target))||!UnicodeFileSystem::copyFile(source+"/"+files[i],target,true))require(UnicodeFileSystem::failure("write","backup",target,GetLastError(),"complete backup failed"));
        HANDLE raw=CreateFileW(UnicodeFileSystem::pathForWrite(target).c_str(),GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(raw==INVALID_HANDLE_VALUE)require(UnicodeFileSystem::failure("write","backup-flush",target,GetLastError(),"cannot open backup"));UnicodeFileSystem::Handle file(raw);if(!FlushFileBuffers(raw))require(UnicodeFileSystem::failure("write","backup-flush",target,GetLastError(),"cannot flush backup"));
        checkpoint("after-copy");
    }
    if(fingerprint(source)!=before||fingerprint(destination)!=before)throw std::runtime_error("transaction: source changed or backup verification failed");
}
inline Record begin(const std::string& slot,const std::string values[4]){
    MercenariePerf::Phase perf("transaction-prepare");
    Record r;r.slot=canonical(slot);bool found=false;Record previous=current(slot,found);
    if(found&&previous.phase!=Committed&&previous.phase!=Prepared)throw std::runtime_error(previous.hasPrevious ? "transaction: unresolved previous save; recovery: "+directory(previous)+"/previous" : "transaction: unresolved native save; use another Kenshi save");
    static LONG counter=0;FILETIME time;GetSystemTimeAsFileTime(&time);std::ostringstream id;id<<time.dwHighDateTime<<'-'<<time.dwLowDateTime<<'-'<<GetCurrentProcessId()<<'-'<<InterlockedIncrement(&counter);r.generation=id.str();
    if(exists(directory(r)))throw std::runtime_error("transaction: generation already exists");
    if(found)r.previousGeneration=previous.generation;
    // Never overwrite a pre-existing inconsistent modern generation.
    validCommitted(slot);
    // Native save backups are managed by Kenshi; stage only the mod companions.
    r.hasPrevious=false;
    for(int i=0;i<4;++i){r.parts[i]=hex(hashBytes(14695981039346656037ull,values[i].data(),values[i].size()));write(directory(r)+"/prepared/"+partName(i),values[i]);if(hex(fileHash(directory(r)+"/prepared/"+partName(i)))!=r.parts[i])throw std::runtime_error("transaction: prepared bytes verification failed");}
    journal(r);return r;
}
inline void markNative(Record& r){
    r.phase=NativeSaving;journal(r);
    // Travels with the slot during native autosave rotations or manual copies.
    write(r.slot+"/TheMercenarie/NativeGeneration.v1",r.encode());
}
inline void verifyNativeGeneration(const Record& r){
    const Record native=Record::decode(read(r.slot+"/TheMercenarie/NativeGeneration.v1"));
    if(native.generation!=r.generation)throw std::runtime_error("transaction: native generation identity mismatch");
    for(int i=0;i<4;++i)if(native.parts[i]!=r.parts[i])throw std::runtime_error("transaction: native companion identity mismatch");
}
inline void confirmNative(Record& r){
    MercenariePerf::Phase perf("confirm-native");
    if(!exists(r.slot+"/quick.save"))throw std::runtime_error("transaction: native completion without quick.save");
    verifyNativeGeneration(r);
    r.nativeDigest=fingerprint(r.slot,true);r.phase=NativeConfirmed;journal(r);
}
inline void failNative(Record& r){r.phase=NativeFailed;journal(r);}
inline void removeOwnedGeneration(const Record& currentRecord){
    // Only the previous journal-owned numeric generation is eligible. The new
    // generation contains the committed mod companions. Legacy full backups are retained.
    if(currentRecord.previousGeneration.empty()||currentRecord.previousGeneration==currentRecord.generation)return;
    const std::string root=home(currentRecord.slot)+"/"+currentRecord.previousGeneration;
    if(exists(root+"/previous"))return; // Preserve backups made by older builds.
    std::vector<std::string> pending(1,root),directories;
    for(size_t i=0;i<pending.size();++i){
        DWORD a=UnicodeFileSystem::attributes(pending[i]);if(a==INVALID_FILE_ATTRIBUTES)continue;
        if((a&FILE_ATTRIBUTE_REPARSE_POINT)||!(a&FILE_ATTRIBUTE_DIRECTORY))return;
        directories.push_back(pending[i]);WIN32_FIND_DATAW e;
        HANDLE raw=FindFirstFileW(UnicodeFileSystem::preferredPath(pending[i]+"/*").c_str(),&e);if(raw==INVALID_HANDLE_VALUE)continue;
        do{std::wstring n=e.cFileName;if(n==L"."||n==L"..")continue;
            const std::string p=pending[i]+"/"+UnicodeFileSystem::utf8(n);
            if(e.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT){FindClose(raw);return;}
            if(e.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)pending.push_back(p);else UnicodeFileSystem::removeFile(p);
        }while(FindNextFileW(raw,&e));FindClose(raw);
    }
    for(size_t i=directories.size();i>0;--i)UnicodeFileSystem::removeDirectory(directories[i-1]);
}
inline bool disabledRecord(const Record& r){
    const std::string marker=MercenarieCleanup::marker();
    if(r.parts[0]!=hex(hashBytes(14695981039346656037ull,marker.data(),marker.size())))return false;
    for(int i=1;i<4;++i)if(r.parts[i]!=hex(hashBytes(14695981039346656037ull,"",0)))return false;
    return true;
}
inline bool disabledSlot(const std::string& slot){
    const std::string path=slot+"/TheMercenarie/"+MercenarieCleanup::markerName();
    if(!exists(path))return false;
    if(!MercenarieCleanup::validMarker(read(path)))throw std::runtime_error("cleanup: invalid disabled marker; refusing reactivation");
    return true;
}
inline void removeCompanion(const std::string& directory,const char* name){
    const std::string path=directory+"/"+name;
    if(!exists(path))return;
    DWORD attrs=UnicodeFileSystem::attributes(path);
    if(attrs&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))throw std::runtime_error("cleanup: unsafe companion "+path);
    checkpoint("before-remove-companion");
    if(!UnicodeFileSystem::removeFile(path))throw std::runtime_error("cleanup: cannot remove companion "+path);
    checkpoint("after-remove-companion");
}
inline void commit(Record& r){
    MercenariePerf::Phase perf("publication");
    verifyNativeGeneration(r);
    if(r.phase!=NativeConfirmed||r.nativeDigest!=fingerprint(r.slot,true))throw std::runtime_error("transaction: native generation changed before publication");
    const bool cleanup=disabledRecord(r);const std::string target=r.slot+"/TheMercenarie";
    // Validate every prepared part before publishing any of them.
    std::string values[4];for(int i=0;i<4;++i){values[i]=read(directory(r)+"/prepared/"+partName(i));if(hex(hashBytes(14695981039346656037ull,values[i].data(),values[i].size()))!=r.parts[i])throw std::runtime_error("transaction: prepared component damaged");}
    if(cleanup){
        write(target+"/"+MercenarieCleanup::markerName(),values[0]);
        for(int i=0;i<4;++i)removeCompanion(target,partName(i));
        removeCompanion(target,"GuildEscortReputation.dat.bak");
    }else{
        for(int i=0;i<4;++i)write(target+"/"+partName(i),values[i]);
        removeCompanion(target,MercenarieCleanup::markerName());
    }
    const Fingerprints observed=fingerprints(r.slot);
    if(r.nativeDigest!=observed.native)throw std::runtime_error("transaction: native generation changed during publication");
    Record committed=r;committed.completeDigest=observed.complete;committed.phase=Committed;
    write(r.slot+"/TheMercenarie/Generation.v1",committed.encode());journal(committed);r=committed;
    // Cleanup is best effort; it cannot turn a completed save into a failure.
    try{removeOwnedGeneration(r);}catch(...){}
}
inline void preflight(const std::string& slot,bool resetMod=false,bool recover=true){
    MercenariePerf::Phase perf("preflight");
    bool found=false;Record r=current(slot,found);
    if(found&&r.phase==NativeConfirmed){if(!recover)throw std::runtime_error("transaction: sidecar recovery required before import: "+slot);commit(r);return;}
    if(found&&(r.phase==NativeSaving||r.phase==NativeFailed))
        throw std::runtime_error(r.hasPrevious ? "transaction: interrupted native save; preserved complete backup: "+directory(r)+"/previous" : "transaction: interrupted native save; use another Kenshi save");
    if(resetMod){
        const std::string path=slot+"/TheMercenarie/Generation.v1";
        if(exists(path)){Record saved=Record::decode(read(path));saved.slot=canonical(slot);verifyNativeGeneration(saved);if(saved.nativeDigest!=fingerprint(slot,true))throw std::runtime_error("transaction: native generation damaged; reset cannot repair the world");}
        else if(exists(slot+"/TheMercenarie/NativeGeneration.v1"))throw std::runtime_error("transaction: incomplete native generation; reset cannot repair the world");
        return;
    }
    const bool valid=validCommitted(slot);
    if(found&&r.phase==Committed&&!valid)throw std::runtime_error("transaction: committed generation marker missing: "+slot);
}
}
