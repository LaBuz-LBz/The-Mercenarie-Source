#pragma once
namespace CaravanNegotiation {
inline int read(const std::string& metadata,int client){
    size_t p=metadata.find(";SALEVAR2=");bool modern=p!=std::string::npos;if(!modern)p=metadata.find(";SALEVAR1=");if(p==std::string::npos)return -1;
    p+=10;std::istringstream in(metadata.substr(p,metadata.find(';',p)-p));int ids[4]={0,0,0,0};
    for(int i=0;i<(modern?4:3);++i)if(!(in>>ids[i])||ids[i]<0||ids[i]>=20)throw std::runtime_error("invalid sale variant");
    return ids[std::max(0,std::min(modern?3:2,client))];
}
inline void choose(std::string& metadata,int first,int second,int third,int fourth=0){
    if(read(metadata,0)>=0)return;
    int pool[20],draws[4]={first,second,third,fourth},ids[4];for(int i=0;i<20;++i)pool[i]=i;
    for(int i=0;i<4;++i){int j=std::max(0,std::min(19-i,draws[i]));ids[i]=pool[j];for(int k=j;k<19-i;++k)pool[k]=pool[k+1];}
    std::ostringstream out;out<<";SALEVAR2="<<ids[0]<<' '<<ids[1]<<' '<<ids[2]<<' '<<ids[3]<<';';metadata+=out.str();
}
}
