#define NOMINMAX
#include <windows.h>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include "src/UI/ClientOptions.h"
bool legacy(const ClientOptions::Settings& s){const char* temp="mods/Guild Escort Contracts/Options.ini.tmp";std::ofstream out(temp,std::ios::trunc);ClientOptions::write(out,s);out.flush();bool ok=out.good();out.close();return ok&&MoveFileExA(temp,"mods/Guild Escort Contracts/Options.ini",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);}
struct Valid{bool operator()(int v){return v>0&&v<256;}};
int main(){wchar_t original[32768];GetCurrentDirectoryW(32768,original);CreateDirectoryW(L"reports",0);CreateDirectoryW(L"reports\\options-persistence",0);CreateDirectoryW(L"reports\\options-persistence\\legacy",0);assert(SetCurrentDirectoryW(L"reports\\options-persistence\\legacy"));
const wchar_t* roots[]={L"ASCII User",L"\u6d4b\u8bd5\u7528\u6237 Latin \u4f63\u5175\u914d\u7f6e"};
for(int i=0;i<2;++i){CreateDirectoryW(roots[i],0);assert(SetCurrentDirectoryW(roots[i]));CreateDirectoryW(L"mods",0);CreateDirectoryW(L"mods\\Guild Escort Contracts",0);ClientOptions::Settings s(false,36,25,0);s.bindings[0]=66;assert(legacy(s));std::ifstream in("mods/Guild Escort Contracts/Options.ini");ClientOptions::Settings restored;ClientOptions::read(in,restored,Valid());assert(restored.bindings[0]==66);in.close();s=ClientOptions::Settings(false,36,25,0);assert(legacy(s));std::cout<<(i?"CJK":"ASCII")<<" legacy relative path: F8/write/reload/reset PASS\n";SetCurrentDirectoryW(L"..");}
CreateDirectoryW(L"Workshop-only",0);SetCurrentDirectoryW(L"Workshop-only");ClientOptions::Settings s(false,36,25,0);s.bindings[0]=66;assert(!legacy(s));s.bindings[0]=67;assert(!legacy(s));s=ClientOptions::Settings(false,36,25,0);assert(!legacy(s));std::cout<<"Workshop-only, no hardcoded mods directory: F8/other/reset ALL FAIL reproduced; ACP="<<GetACP()<<"\n";SetCurrentDirectoryW(original);}
