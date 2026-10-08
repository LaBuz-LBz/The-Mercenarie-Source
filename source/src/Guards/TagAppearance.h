#pragma once
#include <string>
namespace GuildGuards {
// Bounded UTF-8 cosmetic text. Invalid sequences and control codes are discarded.
inline std::string cosmeticText(const std::string& text,size_t limit){
 std::string out;size_t count=0;
 for(size_t i=0;i<text.size()&&count<limit;){unsigned char c=(unsigned char)text[i];size_t n=c<0x80?1:(c>=0xc2&&c<=0xdf?2:(c>=0xe0&&c<=0xef?3:(c>=0xf0&&c<=0xf4?4:0)));bool valid=n&&i+n<=text.size();unsigned int cp=n?c&((1<<(8-n-1))-1):0;if(n==1)cp=c;
  for(size_t j=1;valid&&j<n;++j){unsigned char d=(unsigned char)text[i+j];if((d&0xc0)!=0x80)valid=false;else cp=(cp<<6)|(d&63);}
  if(valid&&((n==2&&cp<0x80)||(n==3&&cp<0x800)||(n==4&&cp<0x10000)||(cp>=0xd800&&cp<=0xdfff)||cp>0x10ffff))valid=false;
  if(!valid){++i;continue;}if((cp>=32&&cp!=127)||cp==10||cp==9){out.append(text,i,n);++count;}i+=n;
 }return out;
}
inline std::string tagName(const std::string& text){std::string s=cosmeticText(text,96);for(size_t i=0;i<s.size();++i)if(s[i]=='\n'||s[i]=='\t')s[i]=' ';size_t a=s.find_first_not_of(' '),b=s.find_last_not_of(' ');return a==std::string::npos?std::string():s.substr(a,b-a+1);}
inline std::string tagNameKey(const std::string& text){
 std::string s=tagName(text),out;
 for(size_t i=0;i<s.size();){unsigned char c=(unsigned char)s[i];size_t n=c<128?1:c<224?2:c<240?3:4;unsigned int cp=n==1?c:c&((1<<(7-n))-1);for(size_t j=1;j<n;++j)cp=(cp<<6)|((unsigned char)s[i+j]&63);i+=n;
  if((cp>='A'&&cp<='Z')||(cp>=0xc0&&cp<=0xde&&cp!=0xd7)||(cp>=0x410&&cp<=0x42f))cp+=32;
  else if(cp==0x401)cp=0x451;
  else if(cp==0x104||cp==0x106||cp==0x118||cp==0x141||cp==0x143||cp==0x15a||cp==0x179||cp==0x17b)++cp;
  if(cp<128)out+=(char)cp;else if(cp<2048){out+=(char)(0xc0|(cp>>6));out+=(char)(0x80|(cp&63));}else if(cp<65536){out+=(char)(0xe0|(cp>>12));out+=(char)(0x80|((cp>>6)&63));out+=(char)(0x80|(cp&63));}else {out+=(char)(0xf0|(cp>>18));out+=(char)(0x80|((cp>>12)&63));out+=(char)(0x80|((cp>>6)&63));out+=(char)(0x80|(cp&63));}
 }return out;
}
inline int tagColour(unsigned long long id,int colour){return colour>=0&&colour<5?colour:(int)(id%5);}
}
