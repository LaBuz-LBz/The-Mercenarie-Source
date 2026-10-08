#pragma once
#include "src/Save/Diagnostics.h"
#include "SaveBuild.generated.h"
static SaveDiagnostics::Operation* activeSaveOperation=0;
static bool savePreparing=false;
static SaveDiagnostics::Failure lastPersistenceIO;
static std::string estateRootError;
static SaveDiagnostics::Failure worldPersistenceRoot;
static bool hasWorldPersistenceRoot=false;
static void saveStage(const char* component,const char* data=""){
    if(activeSaveOperation){if(activeSaveOperation->component!=component&&!activeSaveOperation->failed){std::ostringstream s;s<<"MercenarieSave: incident="<<activeSaveOperation->id<<" phase="<<activeSaveOperation->phase<<" component="<<activeSaveOperation->component<<" result=OK";DebugLog(s.str());}activeSaveOperation->component=component;activeSaveOperation->data=data;}
}
static void saveLog(const SaveDiagnostics::Operation& op,const char* result,const SaveDiagnostics::Failure* error=0){
    std::ostringstream s;s<<"MercenarieSave: build="<<MercenarieSaveBuild<<" session="<<GetCurrentProcessId()<<" incident="<<op.id<<" phase="<<op.phase<<" component="<<(error?error->component:op.component)<<" result="<<result<<" native_called="<<op.nativeCalled<<" native_succeeded="<<op.nativeSucceeded;
    if(error)s<<" operation="<<error->operation<<" stage="<<error->stage<<" data="<<SaveDiagnostics::clean(error->data)<<" code="<<SaveDiagnostics::code("SAVE",error->reason)<<" win32="<<error->win32<<" size="<<error->size<<" count="<<error->count<<" path="<<SaveDiagnostics::clean(error->path)<<" detail="<<SaveDiagnostics::clean(error->detail);
    DebugLog(s.str());
}
static SaveDiagnostics::Failure saveIOFailure(const UnicodeFileSystem::Result& r){
    SaveDiagnostics::Failure f;f.operation=r.operation;f.stage=r.stage;f.path=r.path;f.win32=r.error;f.detail=r.detail;f.size=r.size;
    f.component=r.path.find("Reputation")!=std::string::npos?"reputation":r.path.find("Fiscal")!=std::string::npos?"fiscal":r.path.find("Boards")!=std::string::npos?"boards":"missions";
    f.reason=r.status==UnicodeFileSystem::ReadAccessDenied?SaveDiagnostics::AccessDenied:r.status==UnicodeFileSystem::ReadLocked?SaveDiagnostics::Locked:r.status==UnicodeFileSystem::ReadTruncated?SaveDiagnostics::Truncated:r.status==UnicodeFileSystem::ReadTooLarge?SaveDiagnostics::SizeLimit:r.operation=="read"?SaveDiagnostics::ReadFailed:SaveDiagnostics::writeReason(r.path);return f;
}
static void saveRemember(const SaveDiagnostics::Failure& f){
    if(activeSaveOperation){bool first=!activeSaveOperation->failed;activeSaveOperation->remember(f);saveLog(*activeSaveOperation,first?"ROOT_ERROR":"CONSEQUENCE",first?&activeSaveOperation->first:&f);}
}
static void saveNotify(SaveDiagnostics::Operation& op,const char* key){
    if(op.notified)return;op.notified=true;if(!ou)return;
    Loc::Catalogue a;a["incident"]=op.id;a["code"]=SaveDiagnostics::code("SAVE",op.first.reason);
    ou->showPlayerAMessage(Loc::format(key,a),true);
}
struct SaveOperationGuard {
    SaveDiagnostics::Operation* previous;bool preparing;
    explicit SaveOperationGuard(SaveDiagnostics::Operation& op):previous(activeSaveOperation),preparing(savePreparing){activeSaveOperation=&op;savePreparing=true;}
    ~SaveOperationGuard(){activeSaveOperation=previous;savePreparing=preparing;}
};
