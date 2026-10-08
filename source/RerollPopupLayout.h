#pragma once
#include <algorithm>
#include <string>
namespace RerollPopupLayout {
struct Layout {
    int w,h,pad,header,font,titleFont,buttonFont,left,textX,textW,contentY,contentH,buttonY,buttonW,buttonH,gap,icon;
    Layout(int vw,int vh,int textHeight=0,int buttonTextHeight=0,int headerTextHeight=0){
        w=std::max(240,std::min(900,vw-32));if(vw>=800)w=std::min(900,vw*3/4);
        pad=std::max(12,w/30);gap=std::max(12,w/42);font=std::max(16,w*29/1000);titleFont=font+5;buttonFont=font;
        header=std::max(font+38,headerTextHeight+20);left=w*28/100;textX=left+pad;textW=w-textX-pad-18;
        buttonH=std::max(font+46,buttonTextHeight+24);buttonW=(w-2*pad-gap)/2;
        contentY=header+pad;contentH=std::max(font*8,textHeight);
        h=std::min(vh-24,contentY+contentH+pad+buttonH+pad);
        contentH=std::max(1,h-contentY-pad-buttonH-pad);buttonY=h-pad-buttonH;icon=std::min(left-2*pad,contentH*7/10);
    }
};
// Measure with the actual selected MyGUI font. Split overlong words at UTF-8
// codepoint boundaries too, so CJK and unspaced community strings remain readable.
template<class Measure> std::string wrap(const std::string& text,int width,Measure measure){
    std::string out,line;size_t lastSpace=std::string::npos;
    for(size_t i=0;i<text.size();){
        unsigned char c=text[i];size_t n=c<128?1:c<224?2:c<240?3:4;n=std::min(n,text.size()-i);
        std::string ch=text.substr(i,n);i+=n;
        if(ch=="\r")continue;
        if(ch=="\n"){out+=line+"\n";line.clear();lastSpace=std::string::npos;continue;}
        std::string candidate=line+ch;
        if(!line.empty()&&measure(candidate)>width){
            if(lastSpace!=std::string::npos){out+=line.substr(0,lastSpace)+"\n";line=line.substr(lastSpace+1);}
            else {out+=line+"\n";line.clear();}
            if(ch==" "&&line.empty())continue;
            lastSpace=line.find_last_of(' ');
        }
        line+=ch;if(ch==" ")lastSpace=line.size()-1;
    }
    return out+line;
}
inline std::string penaltyColour(std::string body){
    size_t at=body.find("25");if(at==std::string::npos)return body;
    size_t end=body.find('%',at+2),wide=body.find("\xef\xbc\x85",at+2);
    if(wide!=std::string::npos&&(end==std::string::npos||wide<end)){end=wide+3;}else if(end!=std::string::npos)++end;
    if(end==std::string::npos||end-at>16)return body;
    body.insert(end,"#DDD5C2");body.insert(at,"#F5AD36");return body;
}
}
