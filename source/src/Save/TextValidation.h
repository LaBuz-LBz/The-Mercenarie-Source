#pragma once
#include <sstream>
#include <locale>
#include <vector>
#include <set>
#include <stdexcept>
#include <cmath>
namespace SaveText {
inline std::vector<std::string> fields(std::string line){if(!line.empty()&&line[line.size()-1]=='\r')line.erase(line.size()-1);std::vector<std::string> f;size_t at=0;for(;;){size_t end=line.find('|',at);f.push_back(line.substr(at,end==std::string::npos?end:end-at));if(end==std::string::npos)break;at=end+1;}return f;}
inline double number(const std::string& s,double low,double high,bool integer=false){double v=0;std::istringstream in(s);in.imbue(std::locale::classic());if(!(in>>v)||!(v>=low&&v<=high)||(integer&&std::floor(v)!=v))throw std::runtime_error("invalid numeric save field: "+s);in>>std::ws;if(!in.eof())throw std::runtime_error("trailing numeric save data");return v;}
inline void count(const std::vector<std::string>& f,size_t n){if(f.size()!=n)throw std::runtime_error("invalid field count for "+f[0]);}
inline void fiscal(const std::string& bytes){
    if(bytes.empty())return;if(bytes.size()>32u*1024u*1024u)throw std::runtime_error("fiscal byte limit");
    std::istringstream in(bytes);std::string line;bool version=false;std::set<std::string> orgs,ids;
    while(std::getline(in,line)){if(line.empty()||line=="\r")continue;std::vector<std::string> f=fields(line);
        if(f[0]=="@version"){count(f,3);if(version||f[1]!="1")throw std::runtime_error("unsupported fiscal version");number(f[2],1,4294967295.0,true);version=true;}
        else if(f[0]=="@org"){
            if(f.size()!=15&&f.size()!=16)throw std::runtime_error("truncated fiscal organisation");
            number(f[1],0,1,true);if(!orgs.insert(f[1]).second)throw std::runtime_error("duplicate fiscal organisation");number(f[2],0,16,true);
            for(int i=3;i<=6;++i)number(f[i],0,9e15,true);for(int i=7;i<=8;++i)number(f[i],0,1e9);
            for(int i=9;i<=11;++i)number(f[i],0,1,true);number(f[12],0,2147483647,true);
            if(f.size()==16){number(f[13],0,3,true);number(f[14],0,1,true);}else number(f[13],0,1,true);
        }else if(f[0]=="@entry"){
            count(f,24);if(f[1].empty()||!ids.insert(f[1]).second)throw std::runtime_error("duplicate fiscal identity");number(f[8],0,1e9);
            // Investments are certified, non-taxable debits. Only their gross
            // field is negative; income, bonuses, tips and taxes remain unsigned.
            const bool investment=f[3]=="guild.investment"&&f[5]=="guild.investment";
            number(f[9],investment?-2147483647.0:0,investment?-1:2147483647.0,true);
            for(int i=10;i<=12;++i)number(f[i],0,investment?0:2147483647,true);for(int i=13;i<=15;++i)number(f[i],0,investment?0:1,true);
            number(f[16],0,100,true);number(f[17],0,100,true);for(int i=18;i<=21;++i)number(f[i],0,2147483647,true);number(f[22],0,5,true);number(f[23],0,5,true);
            if(investment){for(int i=16;i<=21;++i)number(f[i],0,0,true);number(f[22],2,2,true);number(f[23],2,2,true);}
            if(number(f[20],0,2147483647)>number(f[18],0,2147483647)||number(f[21],0,2147483647)>number(f[19],0,2147483647))throw std::runtime_error("fiscal payment exceeds tax");
        }else if(f[0]!="@finance"&&f[0]!="@cash"&&f[0]!="@cashday"&&f[0]!="@cashmigration")throw std::runtime_error("unknown fiscal record: "+f[0]);
    }
    if(!version)throw std::runtime_error("missing fiscal version");
}
inline void boards(const std::string& bytes){
    if(bytes.empty())return;if(bytes.size()>16u*1024u*1024u)throw std::runtime_error("boards byte limit");
    std::istringstream in(bytes);std::string line;int version=0;std::set<std::string> cities,offers;bool rerolls=false;
    while(std::getline(in,line)){if(line.empty()||line=="\r")continue;std::vector<std::string> f=fields(line);
        if(f[0]=="@version"){count(f,2);if(version)throw std::runtime_error("duplicate board version");version=(int)number(f[1],1,3,true);}
        else if(f[0]=="@rerolls"){if(rerolls||f.size()<3)throw std::runtime_error("invalid rerolls record");rerolls=true;count(f,5);double remaining=number(f[1],0,2,true),started=number(f[2],-1,1e9),ends=number(f[3],-1,1e9);number(f[4],0,4294967295.0,true);if(remaining==2?(started!=-1||ends!=-1):!(started>=0&&ends==started+24&&ends<1e9))throw std::runtime_error("invalid reroll clock");}
        else if(f[0]=="@board"){count(f,3);if(f[1].empty()||!cities.insert(f[1]).second)throw std::runtime_error("duplicate board identity");number(f[2],0,1e9);}
        else if(f[0]=="@offer"){
            if(f.size()!=27&&!(version==1&&f.size()==19))throw std::runtime_error("invalid board offer fields");
            if(!cities.count(f[1])||!offers.insert(f[1]+"|"+f[2]).second)throw std::runtime_error("invalid board offer identity");number(f[2],0,5,true);number(f[3],0,4,true);number(f[4],0,100,true);
            number(f[12],0,1e9);number(f[13],0,1e6);number(f[14],0,2147483647,true);number(f[15],0,4,true);number(f[16],0,256,true);number(f[17],0,1,true);number(f[18],0,1,true);
            if(f.size()==27)for(int i=19;i<=25;++i)number(f[i],0,2147483647,true);
        }else throw std::runtime_error("unknown board record: "+f[0]);
    }
    if(!version)throw std::runtime_error("missing board version");
}
}
