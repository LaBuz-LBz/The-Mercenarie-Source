#pragma once
namespace CaravanCustomerSpawn {
// V1 marked an attempt, including failures. V2 means at least one actor exists.
inline bool completed(const std::string& metadata){return metadata.find(";RPCLIENTS=2;")!=std::string::npos;}
inline int attempts(const std::string& metadata){
    size_t p=metadata.find(";RPTRY4=");if(p==std::string::npos)return 0;p+=8;
    int n=0;for(;p<metadata.size()&&metadata[p]>='0'&&metadata[p]<='9';++p){n=n*10+metadata[p]-'0';if(n>=10)return 10;}return n;
}
inline void attempt(std::string& metadata){
    int n=attempts(metadata)+1;size_t p=metadata.find(";RPTRY4=");
    if(p!=std::string::npos){size_t end=metadata.find(';',p+1);if(end!=std::string::npos)metadata.erase(p,end-p+1);}
    char value[32];sprintf_s(value,";RPTRY4=%d;",n);metadata+=value;
}
inline void complete(std::string& metadata){if(!completed(metadata))metadata+=";RPCLIENTS=2;";}
}
