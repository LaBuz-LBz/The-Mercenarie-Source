#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <clocale>
#include <mbctype.h>
#include "src/UI/ClientOptions.h"
#include "src/UI/ClientOptionsFile.h"
struct Valid{bool operator()(int v){return v>0&&v<256;}};
bool save(const std::wstring& path,const ClientOptions::Settings& settings,ClientOptionsFile::Failure& error){std::ostringstream bytes;bytes.imbue(std::locale::classic());ClientOptions::write(bytes,settings);return ClientOptionsFile::write(path,bytes.str(),error);}
ClientOptions::Settings reload(const std::wstring& path){ClientOptionsFile::Failure error;std::string bytes;assert(ClientOptionsFile::read(path,bytes,error));std::istringstream input(bytes);ClientOptions::Settings value(false,36,25,0);ClientOptions::read(input,value,Valid());return value;}
#include "options-persistence-callbacks.generated.h"
int main(){ClientOptionsFile::Failure error;std::wstring path=ClientOptionsFile::path(error);assert(!path.empty());std::cout<<"path="<<ClientOptionsFile::utf8(path)<<" ACP="<<GetACP()<<"\n";
const char* locales[]={"C","French_France.1252","Chinese_China.936"};
for(int l=0;l<3;++l){const char* active=setlocale(LC_ALL,locales[l]);std::cout<<"locale="<<locales[l]<<" available="<<(active!=0)<<"\n";if(!active)continue;
for(int cp=0;cp<2;++cp){int codepage=cp?65001:936;int result=_setmbcp(codepage);std::cout<<"CRT codepage="<<codepage<<" result="<<result<<"\n";
ClientOptions::Settings next(false,36,25,0);next.bindings[0]=66;next.language="zh_cn";assert(save(path,next,error));assert(reload(path).bindings[0]==66&&reload(path).language=="zh_cn");next.bindings[0]=67;assert(save(path,next,error));assert(reload(path).bindings[0]==67);ClientOptions::Settings defaults(false,36,25,0);assert(save(path,defaults,error));assert(reload(path).bindings[0]==36&&reload(path).language=="auto");}}
// Actual production callbacks, including Reset and rollback on failed persistence.
assert(saveBinding(ClientOptions::OpenGuildManagement,66,true));assert(reload(path).bindings[0]==66);
assert(saveBinding(ClientOptions::OpenGuildManagement,67,true));assert(reload(path).bindings[0]==67);resetGuildKey(0);assert(reload(path).bindings[0]==36);
assert(SetFileAttributesW(path.c_str(),FILE_ATTRIBUTE_READONLY));assert(!saveBinding(ClientOptions::OpenGuildManagement,66,true));assert(clientOptions.bindings[0]==36&&testWorld.failures==1);assert(SetFileAttributesW(path.c_str(),FILE_ATTRIBUTE_NORMAL));std::cout<<"PASS production callbacks F8/other/Reset/error rollback\n";
// UTF-8 BOM, CRLF, valid Chinese language code, Chinese comment/unknown field.
assert(ClientOptionsFile::write(path,"lastDismissedNewsVersion=V9\nguildManagementHotkey=36\n",error));
assert(saveBinding(ClientOptions::OpenGuildManagement,66,true));std::string newsBytes;assert(ClientOptionsFile::read(path,newsBytes,error));assert(MainMenuNewsRules::dismissedPreference(newsBytes)=="V9");
assert(saveClientOptions(ClientOptions::Settings()));assert(ClientOptionsFile::read(path,newsBytes,error));assert(MainMenuNewsRules::dismissedPreference(newsBytes)=="V9");
assert(ClientOptionsFile::write(path,"dismissedNewsVersion=V8\nrealEstateEnabled=0\n",error));
assert(NewsPolicyTests::dismissedVersion()=="V8");assert(MainMenuNewsRules::shouldShowAutomatically(NewsPolicyTests::dismissedVersion(),"V9",false));
assert(NewsPolicyTests::persistDismissed());assert(NewsPolicyTests::dismissedVersion()=="V9"&&!reload(path).realEstateEnabled);
assert(!MainMenuNewsRules::shouldShowAutomatically(NewsPolicyTests::dismissedVersion(),"V9",false));
assert(MainMenuNewsRules::shouldShowAutomatically(NewsPolicyTests::dismissedVersion(),"V10",false));
assert(ClientOptionsFile::write(path,"\xEF\xBB\xBFguildManagementHotkey=66\r\nlanguage=zh_cn\r\n# \xE8\xAF\xAD\xE8\xA8\x80 = \xE4\xB8\xAD\xE6\x96\x87\r\n",error));assert(reload(path).bindings[0]==66&&reload(path).language=="zh_cn");
std::string oldBytes;assert(ClientOptionsFile::read(path,oldBytes,error));ClientOptions::Settings next(false,67,25,0);
assert(SetFileAttributesW(path.c_str(),FILE_ATTRIBUTE_READONLY));assert(!save(path,next,error));assert(error.stage=="replace");std::string log=error.message();assert(log.find(ClientOptionsFile::utf8(path))!=std::string::npos);std::cout<<log;assert(SetFileAttributesW(path.c_str(),FILE_ATTRIBUTE_NORMAL));assert(reload(path).bindings[0]==66);
HANDLE locked=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);assert(locked!=INVALID_HANDLE_VALUE);assert(!save(path,next,error));assert(error.code==ERROR_SHARING_VIOLATION||error.code==ERROR_ACCESS_DENIED);CloseHandle(locked);assert(reload(path).bindings[0]==66);std::cout<<error.message();
assert(!ClientOptionsFile::write(path+L"\\missing\\Options.ini","x",error));assert(error.stage=="create-temp");std::cout<<error.message();
assert(save(path,next,error));assert(reload(path).bindings[0]==67);std::cout<<"PASS F8, other key, reload, reset, BOM/CJK, read-only, sharing lock, missing folder and preserved original\n";
}

