#pragma once
#include <windows.h>
#include <algorithm>
#include <string>
#include <sstream>

namespace UnicodeFileSystem {

enum ReadResult { ReadMissing, ReadOk, ReadError, ReadTooLarge, ReadAccessDenied, ReadLocked, ReadTruncated };
struct Result {
    bool ok;ReadResult status;DWORD error;std::string operation,stage,path,detail;unsigned long long size;
    Result():ok(true),status(ReadOk),error(0),size(0){}
};
inline bool absent(DWORD error){return error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND;}
inline ReadResult readStatus(DWORD error){return absent(error)?ReadMissing:error==ERROR_ACCESS_DENIED?ReadAccessDenied:(error==ERROR_SHARING_VIOLATION||error==ERROR_LOCK_VIOLATION)?ReadLocked:error==ERROR_HANDLE_EOF?ReadTruncated:ReadError;}
inline Result failure(const char* op,const char* stage,const std::string& path,DWORD error,const char* detail){Result r;r.ok=false;r.status=readStatus(error);r.error=error;r.operation=op;r.stage=stage;r.path=path;r.detail=detail;return r;}
struct Handle {HANDLE value;explicit Handle(HANDLE h):value(h){}~Handle(){if(value!=INVALID_HANDLE_VALUE)CloseHandle(value);}private:Handle(const Handle&);Handle& operator=(const Handle&);};

inline std::wstring decode(const std::string& value, UINT codePage, DWORD flags) {
    if(value.empty()) return std::wstring();
    int count=MultiByteToWideChar(codePage,flags,value.data(),static_cast<int>(value.size()),0,0);
    if(count<=0) return std::wstring();
    std::wstring result(static_cast<size_t>(count),L'\0');
    if(!MultiByteToWideChar(codePage,flags,value.data(),static_cast<int>(value.size()),&result[0],count))return std::wstring();
    std::replace(result.begin(),result.end(),L'/',L'\\');
    return result;
}

inline std::wstring preferredPath(const std::string& path) {
    std::wstring result=decode(path,CP_UTF8,MB_ERR_INVALID_CHARS);
    return result.empty()&&!path.empty()?decode(path,CP_ACP,0):result;
}

inline std::wstring legacyPath(const std::string& path) {
    return decode(path,CP_ACP,0);
}

inline std::string utf8(const std::wstring& value) {
    if(value.empty())return std::string();
    int count=WideCharToMultiByte(CP_UTF8,0,value.data(),static_cast<int>(value.size()),0,0,0,0);
    if(count<=0)return std::string();
    std::string result(static_cast<size_t>(count),'\0');
    if(!WideCharToMultiByte(CP_UTF8,0,value.data(),static_cast<int>(value.size()),&result[0],count,0,0))return std::string();
    return result;
}

inline bool attributesFor(const std::wstring& path,DWORD& attributes) {
    if(path.empty()){attributes=INVALID_FILE_ATTRIBUTES;SetLastError(ERROR_INVALID_NAME);return false;}
    attributes=path.empty()?INVALID_FILE_ATTRIBUTES:GetFileAttributesW(path.c_str());
    return attributes!=INVALID_FILE_ATTRIBUTES;
}

inline std::wstring existingPath(const std::string& path,bool* found=0) {
    DWORD attributes=INVALID_FILE_ATTRIBUTES;
    const std::wstring preferred=preferredPath(path);
    if(attributesFor(preferred,attributes)){if(found)*found=true;return preferred;}
    DWORD error=GetLastError();if(!absent(error)){if(found)*found=false;SetLastError(error);return preferred;}
    const std::wstring legacy=legacyPath(path);
    if(legacy!=preferred&&attributesFor(legacy,attributes)){if(found)*found=true;return legacy;}
    if(found)*found=false;
    return preferred;
}

inline std::wstring pathForWrite(std::string path) {
    bool found=false;std::wstring existing=existingPath(path,&found);if(found)return existing;
    while(path.size()>3&&(path[path.size()-1]=='/'||path[path.size()-1]=='\\'))path.erase(path.size()-1);
    size_t slash=path.find_last_of("/\\");
    if(slash==std::string::npos)return preferredPath(path);
    const std::string parent=path.substr(0,slash),leaf=path.substr(slash+1);
    if(parent.empty())return preferredPath(path);
    std::wstring resolvedParent=pathForWrite(parent),resolvedLeaf=preferredPath(leaf);
    if(resolvedParent.empty()||resolvedLeaf.empty())return preferredPath(path);
    if(resolvedParent[resolvedParent.size()-1]!=L'\\')resolvedParent+=L'\\';
    return resolvedParent+resolvedLeaf;
}

inline DWORD attributes(const std::string& path) {
    DWORD result=INVALID_FILE_ATTRIBUTES;attributesFor(existingPath(path),result);return result;
}

inline bool exists(const std::string& path) {return attributes(path)!=INVALID_FILE_ATTRIBUTES;}

inline bool ensureDirectory(std::string path) {
    while(path.size()>3&&(path[path.size()-1]=='/'||path[path.size()-1]=='\\'))path.erase(path.size()-1);
    if(path.empty()){SetLastError(ERROR_INVALID_NAME);return false;}
    DWORD current=attributes(path);
    if(current!=INVALID_FILE_ATTRIBUTES){if(current&FILE_ATTRIBUTE_DIRECTORY)return true;SetLastError(ERROR_DIRECTORY);return false;}
    DWORD error=GetLastError();if(!absent(error)){SetLastError(error);return false;}
    size_t slash=path.find_last_of("/\\");
    if(slash!=std::string::npos){std::string parent=path.substr(0,slash);if(parent.size()==2&&parent[1]==':')parent+='\\';if(!parent.empty()&&!ensureDirectory(parent))return false;}
    const std::wstring target=pathForWrite(path);
    if(!target.empty()&&CreateDirectoryW(target.c_str(),0))return true;
    error=GetLastError();if(error!=ERROR_ALREADY_EXISTS){SetLastError(error);return false;}
    current=attributes(path);if(current!=INVALID_FILE_ATTRIBUTES&&(current&FILE_ATTRIBUTE_DIRECTORY))return true;SetLastError(error);return false;
}

inline Result readDetailed(const std::string& path,std::string& bytes,unsigned long long maximum) {
    bytes.clear();std::wstring source=preferredPath(path);
    HANDLE raw=CreateFileW(source.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    DWORD error=raw==INVALID_HANDLE_VALUE?GetLastError():0;
    if(raw==INVALID_HANDLE_VALUE&&absent(error)){std::wstring legacy=legacyPath(path);if(legacy!=source){raw=CreateFileW(legacy.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);error=raw==INVALID_HANDLE_VALUE?GetLastError():0;}}
    if(raw==INVALID_HANDLE_VALUE)return failure("read","open",path,error,"cannot open sidecar");
    Handle file(raw);LARGE_INTEGER size;if(!GetFileSizeEx(raw,&size)){error=GetLastError();return failure("read","size",path,error,"cannot query size");}
    if(size.QuadPart<0||static_cast<unsigned long long>(size.QuadPart)>maximum){Result r=failure("read","limit",path,ERROR_FILE_TOO_LARGE,"sidecar exceeds byte budget");r.status=ReadTooLarge;r.size=size.QuadPart;return r;}
    bytes.assign(static_cast<size_t>(size.QuadPart),'\0');size_t offset=0;
    while(offset<bytes.size()){
        DWORD chunk=static_cast<DWORD>(std::min<size_t>(bytes.size()-offset,1024u*1024u)),done=0;
        if(!ReadFile(raw,&bytes[offset],chunk,&done,0)){error=GetLastError();bytes.clear();return failure("read","bytes",path,error,"ReadFile failed");}
        if(done!=chunk){bytes.clear();return failure("read","bytes",path,ERROR_HANDLE_EOF,"truncated sidecar");}offset+=done;
    }
    if(!CloseHandle(raw)){error=GetLastError();file.value=INVALID_HANDLE_VALUE;return failure("read","close",path,error,"close failed");}file.value=INVALID_HANDLE_VALUE;
    Result result;result.operation="read";result.path=path;result.size=bytes.size();return result;
}
inline ReadResult readFile(const std::string& path,std::string& bytes,unsigned long long maximum){return readDetailed(path,bytes,maximum).status;}

// Unique same-directory temporary; no backup or validated file mutation before replace.
inline Result atomicWrite(const std::string& path,const std::string& bytes){
    const size_t slash=path.find_last_of("/\\");
    if(path.empty())return failure("write","path",path,ERROR_INVALID_NAME,"empty path");
    if(slash!=std::string::npos&&!ensureDirectory(path.substr(0,slash))){DWORD e=GetLastError();return failure("write","directory",path,e,"cannot create parent");}
    const std::wstring target=pathForWrite(path);static LONG sequence=0;
    std::wostringstream suffix;suffix<<L".tmp-"<<GetCurrentProcessId()<<L"-"<<GetTickCount()<<L"-"<<InterlockedIncrement(&sequence);
    const std::wstring temp=target+suffix.str();
    HANDLE raw=CreateFileW(temp.c_str(),GENERIC_WRITE,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
    if(raw==INVALID_HANDLE_VALUE){DWORD e=GetLastError();return failure("write","create-temp",path,e,"cannot create temporary");}
    struct Temporary {const std::wstring& path;bool active;Temporary(const std::wstring& p):path(p),active(true){}~Temporary(){if(active)DeleteFileW(path.c_str());}} cleanup(temp);
    Handle file(raw);Result result;result.operation="write";result.path=path;result.size=bytes.size();
    size_t offset=0;while(offset<bytes.size()){
        DWORD chunk=static_cast<DWORD>(std::min<size_t>(bytes.size()-offset,1024u*1024u)),done=0;
        if(!WriteFile(raw,bytes.data()+offset,chunk,&done,0)){DWORD e=GetLastError();result=failure("write","write-temp",path,e,"WriteFile failed");break;}
        if(done!=chunk){result=failure("write","write-temp",path,ERROR_WRITE_FAULT,"short write");break;}offset+=done;
    }
    if(result.ok&&!FlushFileBuffers(raw)){DWORD e=GetLastError();result=failure("write","flush-temp",path,e,"flush failed");}
    if(!CloseHandle(raw)){DWORD e=GetLastError();if(result.ok)result=failure("write","close-temp",path,e,"close failed");}file.value=INVALID_HANDLE_VALUE;
    if(result.ok&&!MoveFileExW(temp.c_str(),target.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)){DWORD e=GetLastError();result=failure("write","replace",path,e,"replace failed");}
    if(!result.ok&&!DeleteFileW(temp.c_str())){DWORD e=GetLastError();if(!absent(e)){std::ostringstream extra;extra<<"; cleanup-temp failed win32="<<e;result.detail+=extra.str();}}
    cleanup.active=false;
    result.size=bytes.size();return result;
}

inline bool writeFile(const std::string& path,const std::string& bytes) {
    const std::wstring target=pathForWrite(path);if(target.empty())return false;
    HANDLE file=CreateFileW(target.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    if(file==INVALID_HANDLE_VALUE)return false;
    size_t offset=0;bool ok=true;
    while(offset<bytes.size()){
        DWORD chunk=static_cast<DWORD>(std::min<size_t>(bytes.size()-offset,1024u*1024u)),done=0;
        if(!WriteFile(file,bytes.data()+offset,chunk,&done,0)||done!=chunk){ok=false;break;}offset+=done;
    }
    if(ok)ok=FlushFileBuffers(file)!=0;
    if(!CloseHandle(file))ok=false;
    return ok;
}

inline bool copyFile(const std::string& source,const std::string& destination,bool failIfExists) {
    bool found=false;const std::wstring from=existingPath(source,&found);if(!found)return false;
    const std::wstring to=pathForWrite(destination);
    return !to.empty()&&CopyFileW(from.c_str(),to.c_str(),failIfExists?TRUE:FALSE)!=0;
}

inline bool moveReplace(const std::string& source,const std::string& destination) {
    bool found=false;const std::wstring from=existingPath(source,&found);if(!found)return false;
    const std::wstring to=pathForWrite(destination);
    return !to.empty()&&MoveFileExW(from.c_str(),to.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
}

inline bool removeFile(const std::string& path) {
    bool found=false;const std::wstring target=existingPath(path,&found);return !found||DeleteFileW(target.c_str())!=0;
}

inline bool removeDirectory(const std::string& path) {
    bool found=false;const std::wstring target=existingPath(path,&found);return !found||RemoveDirectoryW(target.c_str())!=0;
}

inline std::string temporaryDirectory() {
    DWORD needed=GetTempPathW(0,0);if(!needed)return std::string();
    std::wstring path(static_cast<size_t>(needed),L'\0');DWORD written=GetTempPathW(needed,&path[0]);
    if(!written||written>=needed)return std::string();path.resize(written);return utf8(path);
}

}
