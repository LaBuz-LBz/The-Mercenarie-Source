#pragma once
#include <string>
#include <sstream>
#include "Platform/UnicodeFileSystem.h"
// Native save callbacks provide the authoritative location/name. In particular,
// getCurrentGame can still name the previous world during an import or Save As.
namespace GuildSavePaths {
inline bool uninitialized(const std::string& directory){
    const char* files[]={"Mission.v4","GuildEscortReputation.dat","GuildEscortReputation.dat.bak","GuildEscortFiscal.dat","GuildEscortBoards.dat"};
    for(size_t i=0;i<sizeof(files)/sizeof(files[0]);++i){
        if(UnicodeFileSystem::attributes(directory+"/"+files[i])!=INVALID_FILE_ATTRIBUTES)return false;
        DWORD error=GetLastError();if(error!=ERROR_FILE_NOT_FOUND&&error!=ERROR_PATH_NOT_FOUND)return false;
    }
    return true;
}
inline bool ensureDirectory(const std::string& path){return UnicodeFileSystem::ensureDirectory(path);}
inline std::string directory(const std::string& root,const std::string& name){
    std::string base=root.empty()?"save":root;
    while(!base.empty()&&(base[base.size()-1]=='/'||base[base.size()-1]=='\\'))base.erase(base.size()-1);
    return base+"/"+name+"/TheMercenarie";
}
inline std::string sessionDirectory(){
    const std::string temp=UnicodeFileSystem::temporaryDirectory();if(temp.empty())return "";
    static unsigned int sequence=0;std::ostringstream path;
    path<<temp<<"TheMercenarie/Session-"<<GetCurrentProcessId()<<"-"<<GetTickCount()<<"-"<<++sequence;
    return path.str();
}
// Working files are copied to a fresh session, never edited in a saved slot.
// A failed read is not an absent component. Publish runtime paths only after
// every copy succeeds; a partially prepared session is never used.
inline UnicodeFileSystem::Result stageSession(const std::string& source,const std::string& destination){
    if(destination.empty()||(!source.empty()&&source==destination))
        return UnicodeFileSystem::failure("write","session-path",destination,ERROR_INVALID_NAME,"invalid isolated session path");
    if(!ensureDirectory(destination))
        return UnicodeFileSystem::failure("write","session-directory",destination,GetLastError(),"cannot create isolated session");
    if(source.empty())return UnicodeFileSystem::Result();
    const char* files[]={"GuildEscortReputation.dat","GuildEscortReputation.dat.bak","GuildEscortFiscal.dat","GuildEscortBoards.dat"};
    for(size_t i=0;i<sizeof(files)/sizeof(files[0]);++i){
        std::string bytes;
        UnicodeFileSystem::Result result=UnicodeFileSystem::readDetailed(source+"/"+files[i],bytes,64ull*1024ull*1024ull);
        if(result.status==UnicodeFileSystem::ReadMissing)continue;
        if(!result.ok)return result;
        result=UnicodeFileSystem::atomicWrite(destination+"/"+files[i],bytes);
        if(!result.ok)return result;
    }
    return UnicodeFileSystem::Result();
}
}
