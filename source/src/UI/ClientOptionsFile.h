#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <locale>
namespace ClientOptionsFile {
inline std::string utf8(const std::wstring& value){
 if(value.empty())return std::string();int n=WideCharToMultiByte(CP_UTF8,0,value.data(),(int)value.size(),0,0,0,0);if(n<=0)return std::string();std::string out(n,'\0');WideCharToMultiByte(CP_UTF8,0,value.data(),(int)value.size(),&out[0],n,0,0);return out;
}
struct Failure {
 std::string operation,stage;std::wstring path;DWORD code;
 Failure():code(0){}
 bool set(const char* op,const char* step,const std::wstring& file,DWORD error){operation=op;stage=step;path=file;code=error;return false;}
 std::string message()const{wchar_t buffer[1024]={0};FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,0,code,0,buffer,1024,0);std::ostringstream text;text.imbue(std::locale::classic());text<<"Options persistence: operation="<<operation<<" stage="<<stage<<" path="<<utf8(path)<<" win32="<<code<<" detail="<<utf8(buffer);return text.str();}
};
inline std::wstring path(Failure& error){
 HMODULE module=0;
 if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&path),&module)){error.set("resolve","module",L"",GetLastError());return L"";}
 std::vector<wchar_t> buffer(32768);DWORD count=GetModuleFileNameW(module,&buffer[0],(DWORD)buffer.size());
 if(!count||count>=buffer.size()){error.set("resolve","module-path",L"",count?ERROR_INSUFFICIENT_BUFFER:GetLastError());return L"";}
 std::wstring value(&buffer[0],count);size_t slash=value.find_last_of(L"\\/");if(slash==std::wstring::npos){error.set("resolve","parent",value,ERROR_PATH_NOT_FOUND);return L"";}return value.substr(0,slash+1)+L"Options.ini";
}
inline bool read(const std::wstring& file,std::string& bytes,Failure& error){
 bytes.clear();HANDLE in=CreateFileW(file.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(in==INVALID_HANDLE_VALUE)return error.set("read","open",file,GetLastError());
 char buffer[4096];DWORD count=0;bool ok=true;
 while(true){if(!ReadFile(in,buffer,sizeof(buffer),&count,0)){error.set("read","read",file,GetLastError());ok=false;break;}if(!count)break;if(bytes.size()+count>1024*1024){error.set("read","size-limit",file,ERROR_FILE_TOO_LARGE);ok=false;break;}bytes.append(buffer,count);}
 if(!CloseHandle(in)&&ok)ok=error.set("read","close",file,GetLastError());if(!ok){bytes.clear();return false;}
 if(bytes.size()>=3&&bytes.compare(0,3,"\xEF\xBB\xBF")==0)bytes.erase(0,3);
 return true;
}
inline bool write(const std::wstring& file,const std::string& bytes,Failure& error){
 size_t slash=file.find_last_of(L"\\/");if(slash==std::wstring::npos)return error.set("save","parent",file,ERROR_PATH_NOT_FOUND);
 std::wstring directory=file.substr(0,slash);wchar_t temp[MAX_PATH]={0};
 if(!GetTempFileNameW(directory.c_str(),L"MCO",0,temp))return error.set("save","create-temp",file,GetLastError());
 HANDLE out=CreateFileW(temp,GENERIC_WRITE,0,0,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(out==INVALID_HANDLE_VALUE){DWORD code=GetLastError();DeleteFileW(temp);return error.set("save","open-temp",file,code);}
 DWORD count=0;bool ok=WriteFile(out,bytes.data(),(DWORD)bytes.size(),&count,0)!=0;
 if(!ok)error.set("save","write-temp",file,GetLastError());else if(count!=bytes.size())ok=error.set("save","short-write",file,ERROR_WRITE_FAULT);
 if(ok&&!FlushFileBuffers(out))ok=error.set("save","flush-temp",file,GetLastError());
 if(!CloseHandle(out)&&ok)ok=error.set("save","close-temp",file,GetLastError());
 if(ok&&!MoveFileExW(temp,file.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))ok=error.set("save","replace",file,GetLastError());
 if(!ok){if(!DeleteFileW(temp)){DWORD cleanup=GetLastError();std::ostringstream extra;extra<<error.stage<<"; cleanup-temp="<<cleanup;error.stage=extra.str();}return false;}
 return true;
}
}
