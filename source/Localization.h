#pragma once
#include "PerformanceAudit.h"
// Catalogues are authored in Localization/*.json. This header contains no UI prose.
#include <string>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "src/Platform/UnicodeFileSystem.h"
#include "src/Localization/LanguageCode.h"
#include "LocalizationDefaults.generated.h"
namespace Loc {
typedef std::map<std::string,std::string> Catalogue;
inline bool validUtf8(const std::string& s){
    for(size_t i=0;i<s.size();){unsigned char c=s[i++];if(c<128)continue;
        unsigned int n=0,minimum=0;int more=0;
        if(c>=0xC2&&c<=0xDF){n=c&31;more=1;minimum=128;}
        else if(c>=0xE0&&c<=0xEF){n=c&15;more=2;minimum=2048;}
        else if(c>=0xF0&&c<=0xF4){n=c&7;more=3;minimum=65536;}else return false;
        while(more--){if(i>=s.size())return false;unsigned char d=s[i++];if((d&192)!=128)return false;n=(n<<6)|(d&63);}
        if(n<minimum||n>0x10FFFF||(n>=0xD800&&n<=0xDFFF))return false;
    }return true;
}
inline void utf8(std::string& s,unsigned int n){
    if(n<128)s+=(char)n;
    else if(n<2048){s+=(char)(192|(n>>6));s+=(char)(128|(n&63));}
    else if(n<65536){s+=(char)(224|(n>>12));s+=(char)(128|((n>>6)&63));s+=(char)(128|(n&63));}
    else{s+=(char)(240|(n>>18));s+=(char)(128|((n>>12)&63));s+=(char)(128|((n>>6)&63));s+=(char)(128|(n&63));}
}
class Json {
    const std::string& s;size_t p;
    void ws(){while(p<s.size()&&(s[p]==' '||s[p]=='\n'||s[p]=='\r'||s[p]=='\t'))++p;}
    void expect(char c){ws();if(p>=s.size()||s[p++]!=c)throw std::runtime_error("invalid JSON syntax");}
    unsigned int hex(){unsigned int n=0;for(int i=0;i<4;++i){if(p>=s.size())throw std::runtime_error("incomplete unicode escape");char c=s[p++];int d=c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:c>='A'&&c<='F'?c-'A'+10:-1;if(d<0)throw std::runtime_error("invalid unicode escape");n=n*16+d;}return n;}
    std::string str(){expect('"');std::string out;while(p<s.size()){
        unsigned char c=s[p++];if(c=='"')return out;if(c<32)throw std::runtime_error("unescaped control character");
        if(c!='\\'){out+=(char)c;continue;}if(p>=s.size())break;c=s[p++];
        if(c=='"'||c=='\\'||c=='/')out+=(char)c;
        else if(c=='n')out+='\n';else if(c=='r')out+='\r';else if(c=='t')out+='\t';else if(c=='b')out+='\b';else if(c=='f')out+='\f';
        else if(c=='u'){unsigned int n=hex();if(n>=0xD800&&n<=0xDBFF){if(p+2>s.size()||s[p++]!='\\'||s[p++]!='u')throw std::runtime_error("missing low surrogate");unsigned int l=hex();if(l<0xDC00||l>0xDFFF)throw std::runtime_error("invalid low surrogate");n=0x10000+((n-0xD800)<<10)+(l-0xDC00);}else if(n>=0xDC00&&n<=0xDFFF)throw std::runtime_error("unpaired surrogate");utf8(out,n);}
        else throw std::runtime_error("invalid escape");
    }throw std::runtime_error("unterminated JSON string");}
public:
    Json(const std::string& bytes):s(bytes),p(bytes.compare(0,3,"\xEF\xBB\xBF")==0?3:0){if(!validUtf8(bytes))throw std::runtime_error("invalid UTF-8");}
    Catalogue read(bool manifest=false){Catalogue out;expect('{');ws();if(p<s.size()&&s[p]=='}')++p;else for(;;){std::string k=str();expect(':');ws();std::string v;
        // Existing manifests use strings. Also accept the JSON integer schema
        // version commonly written by translators; messages remain string-only.
        if(manifest&&k=="schemaVersion"&&p<s.size()&&s[p]>='0'&&s[p]<='9'){
            size_t begin=p;while(p<s.size()&&s[p]>='0'&&s[p]<='9')++p;
            v=s.substr(begin,p-begin);if(v.size()>1&&v[0]=='0')throw std::runtime_error("invalid schemaVersion integer");
        }else v=str();
        if(out.count(k))throw std::runtime_error("duplicate JSON key");out[k]=v;ws();if(p<s.size()&&s[p]=='}'){++p;break;}expect(',');}ws();if(p!=s.size())throw std::runtime_error("trailing JSON data");return out;}
};
inline std::string normalize(std::string code){
    std::string out;for(size_t i=0;i<code.size();++i){unsigned char c=code[i];if(c=='-')c='_';if(!std::isspace(c))out+=(char)std::tolower(c);}
    const char* aliases[][2]={{"english","en"},{"french","fr"},{"german","de"},{"spanish","es"},{"latam","es_mx"},{"polish","pl"},{"italian","it"},{"russian","ru"},{"portuguese","pt"},{"brazilian","pt_br"},{"japanese","ja"},{"koreana","ko"},{"schinese","zh_cn"},{"tchinese","zh_tw"},{"turkish","tr"},{"ukrainian","uk"}};
    for(size_t i=0;i<sizeof(aliases)/sizeof(aliases[0]);++i)if(out==aliases[i][0])out=aliases[i][1];
    if(out.empty()||out.size()>24)return "en";for(size_t i=0;i<out.size();++i)if(!(out[i]>='a'&&out[i]<='z')&&!(out[i]>='0'&&out[i]<='9')&&out[i]!='_')return "en";return out;
}
inline bool readFile(const std::string& path,Catalogue& out,std::string& error,unsigned long maximum=8*1024*1024,bool manifest=false){
    std::string bytes;UnicodeFileSystem::ReadResult status=UnicodeFileSystem::readFile(path,bytes,maximum);
    if(status!=UnicodeFileSystem::ReadOk){error=path+(status==UnicodeFileSystem::ReadMissing?": missing file":": unreadable or too large");return false;}
    try{out=Json(bytes).read(manifest);return true;}catch(const std::exception& e){error=path+": "+e.what();return false;}
}
inline std::vector<std::string> printfArgs(const std::string& s){
    std::vector<std::string> out;for(size_t i=0;i<s.size();++i)if(s[i]=='%'){
        if(i+1<s.size()&&s[i+1]=='%'){++i;continue;}size_t j=i+1;
        while(j<s.size()&&std::string("-+#0.123456789hlLIzjt*$").find(s[j])!=std::string::npos)++j;
        if(j<s.size()&&std::string("diuoxXfFeEgGaAcspn").find(s[j])!=std::string::npos){out.push_back(s.substr(i+1,j-i));i=j;}
    }return out;
}
inline std::set<std::string> namedArgs(const std::string& s){std::set<std::string> out;for(size_t p=0;(p=s.find('{',p))!=std::string::npos;){size_t e=s.find('}',p);if(e==std::string::npos)break;out.insert(s.substr(p,e-p+1));p=e+1;}return out;}
inline bool compatible(const std::string& base,const std::string& candidate){
    if(candidate.find('\0')!=std::string::npos)return false;
    for(size_t p=0;p<candidate.size();++p)if(candidate[p]=='%'){
        if(p+1<candidate.size()&&candidate[p+1]=='%'){++p;continue;}size_t q=p+1;while(q<candidate.size()&&std::string("-+ #0.123456789hlLIzjt*$").find(candidate[q])!=std::string::npos)++q;if(q<candidate.size()&&candidate[q]=='n')return false;
    }
    return printfArgs(base)==printfArgs(candidate)&&namedArgs(base)==namedArgs(candidate);
}
#include "LocalizationPacks.inl"
struct Engine {
    Catalogue english,selected,cache;std::set<std::string> rendered,interned;
    std::vector<std::string> protectedNames;
    std::vector<std::string> targets[256];
    std::string directory,language,requested,error;bool loaded;unsigned int registryRevision;
    Engine():directory("mods/Guild Escort Contracts/Localization"),language("en"),loaded(false),registryRevision(0){}
    void load(std::string code){
        registry().scan(directory);registryRevision=registry().revision;
        requested=resolveLanguage(code);english.clear();selected.clear();cache.clear();rendered.clear();error.clear();
        for(size_t i=0;i<sizeof(kLocalizationEnglish)/sizeof(kLocalizationEnglish[0]);++i)english[kLocalizationEnglish[i].key]=kLocalizationEnglish[i].value;
        Catalogue file;if(readFile(directory+"/en.json",file,error))merge(english,file);
        language="en";const LanguagePack* pack=registry().find(requested);
        if(pack&&!pack->official){merge(selected,pack->messages);language=pack->code;}
        else if(requested!="en"&&readFile(directory+"/"+requested+".json",file,error)){merge(selected,file);language=requested;}
        for(int i=0;i<256;++i)targets[i].clear();
        for(Catalogue::const_iterator i=english.begin();i!=english.end();++i){Catalogue::const_iterator t=selected.find(i->first);std::string value=t==selected.end()?i->second:t->second;if(!value.empty())targets[(unsigned char)value[0]].push_back(value);}
        loaded=true;
    }
    void merge(Catalogue& dest,const Catalogue& file){for(Catalogue::const_iterator i=file.begin();i!=file.end();++i){Catalogue::const_iterator base=english.find(i->first);if(base!=english.end()&&!i->second.empty()&&compatible(base->second,i->second))dest[i->first]=i->second;}}
    const char* get(const std::string& key){if(!loaded)load("en");Catalogue::const_iterator i=selected.find(key);if(i!=selected.end())return intern(i->second);i=english.find(key);if(i!=english.end())return intern(i->second);i=selected.find("common.text_unavailable");return intern(i!=selected.end()?i->second:"Text unavailable.");}
    const char* intern(const std::string& s){return interned.insert(s).first->c_str();}
};
inline Engine& engine(){static Engine e;return e;}
inline const char* text(const char* key){MercenariePerf::Phase perf("localization");return engine().get(key);}
inline size_t languageChoiceCount(){return registry().packs.size()+1;}
inline std::string languageChoiceCode(size_t index){return index==0?"auto":index<=registry().packs.size()?registry().packs[index-1].code:"";}
inline std::string languageChoiceLabel(size_t index){
    if(index==0)return text("options.language.auto");
    if(index>registry().packs.size())return "";
    const LanguagePack& p=registry().packs[index-1];
    return p.official?text(("options.language."+p.code).c_str()):p.name+" ("+p.englishName+")";
}
inline std::string named(const char* key,const char* name,const std::string& value);
inline void configure(const std::string& directory,const std::string& code){if(engine().directory!=directory)registry()=LanguageRegistry();engine().directory=directory;engine().load(code);}
inline void select(const std::string& code){if(!engine().loaded||engine().registryRevision!=registry().revision||engine().requested!=resolveLanguage(code))engine().load(code);}
inline void writeDiagnostics(std::ostream& out){
    out<<"\nlocalization.catalogue=V9-2361\nlocalization.scans="<<registry().scanCount;
    const LanguagePack* p=registry().find(engine().language);
    if(p&&!p->official)out<<"\nlocalization.pack="<<p->source<<"\nlocalization.packCode="<<p->code<<"\nlocalization.packName="<<p->name<<"\nlocalization.packAuthor="<<p->author<<"\nlocalization.packVersion="<<p->version<<"\nlocalization.packCatalogue="<<p->catalogueVersion<<"\nlocalization.translated="<<engine().selected.size()<<"\nlocalization.englishFallback="<<(engine().english.size()-engine().selected.size())<<"\nlocalization.ignoredOrInvalidEntries="<<(p->messages.size()-engine().selected.size());
    for(size_t i=0;i<registry().diagnostics.size();++i)out<<"\nlocalization.notice="<<registry().diagnostics[i];
    out<<"\n";
}
inline std::string format(const char* key,const Catalogue& args){std::string s=text(key),out;for(size_t p=0;p<s.size();){if(s[p]=='{'){size_t e=s.find('}',p);if(e!=std::string::npos){Catalogue::const_iterator i=args.find(s.substr(p+1,e-p-1));if(i!=args.end()){out+=i->second;p=e+1;continue;}}}out+=s[p++];}return out;}
inline std::string named(const char* key,const char* name,const std::string& value){Catalogue args;args[name]=value;return format(key,args);}
inline const char* pluralForm(int value){std::string lang=engine().language.substr(0,2);const LanguagePack* p=registry().find(engine().language);if(p&&!p->pluralRule.empty())lang=p->pluralRule;unsigned int n=value<0?0u-static_cast<unsigned int>(value):static_cast<unsigned int>(value);if(lang=="other"||lang=="zh"||lang=="ja"||lang=="ko")return "other";if(lang=="pl"){if(n==1)return "one";if(n%10>=2&&n%10<=4&&(n%100<12||n%100>14))return "few";return "many";}if(lang=="ru"||lang=="uk"){if(n%10==1&&n%100!=11)return "one";if(n%10>=2&&n%10<=4&&(n%100<12||n%100>14))return "few";return "many";}return (lang=="fr"?n<=1:n==1)?"one":"other";}
inline std::string count(const char* key,int n){std::string k=std::string(key)+"."+pluralForm(n);Catalogue args;std::ostringstream s;s<<n;args["count"]=s.str();return format(k.c_str(),args);}
inline bool word(unsigned char c){return c>=128||std::isalnum(c)||c=='_';}
struct AliasIndex {
    std::vector<std::string> values;std::vector<size_t> buckets[256];
    AliasIndex(){for(size_t i=0;i<sizeof(kLocalizationAliases)/sizeof(kLocalizationAliases[0]);++i){values.push_back(kLocalizationAliases[i].value);if(!values.back().empty())buckets[(unsigned char)values.back()[0]].push_back(i);}}
};
// Compatibility for old saved descriptions and histories. New strings use text/format.
inline bool matchTemplate(const std::string& pattern,const std::string& source,Catalogue& args){
    size_t p=0,s=0;
    while(p<pattern.size()){
        size_t open=pattern.find('{',p);
        if(open==std::string::npos)return source.substr(s)==pattern.substr(p);
        std::string literal=pattern.substr(p,open-p);
        if(source.compare(s,literal.size(),literal)!=0)return false;s+=literal.size();
        size_t close=pattern.find('}',open);if(close==std::string::npos)return false;
        size_t next=pattern.find('{',close+1);std::string delimiter=pattern.substr(close+1,next==std::string::npos?std::string::npos:next-close-1);
        size_t end=delimiter.empty()?source.size():source.find(delimiter,s);
        if(end==std::string::npos||end==s)return false;
        args[pattern.substr(open+1,close-open-1)]=source.substr(s,end-s);s=end;p=close+1;
    }return s==source.size();
}
inline std::string legacy(const std::string& source,const std::vector<std::string>& names){
    MercenariePerf::Phase perf("localization-legacy");
    Engine& e=engine();if(!e.loaded)e.load("en");if(e.protectedNames!=names){e.protectedNames=names;e.cache.clear();e.rendered.clear();}if(source.empty()||e.rendered.count(source))return source;
    Catalogue::const_iterator cached=e.cache.find(source);if(cached!=e.cache.end())return cached->second;
    for(size_t n=0;n<names.size();++n)if(source==names[n])return source;
    if(e.cache.size()>=4096||e.rendered.size()>=4096){e.cache.clear();e.rendered.clear();}
    for(size_t i=0;i<sizeof(kLocalizationTemplates)/sizeof(kLocalizationTemplates[0]);++i){Catalogue args;if(matchTemplate(kLocalizationTemplates[i].value,source,args)){std::string result=format(kLocalizationTemplates[i].key,args);e.cache[source]=result;e.rendered.insert(result);return result;}}
    static AliasIndex index;
    // Names are checked before aliases so player-created names never become UI text.
    std::string result;for(size_t p=0;p<source.size();){size_t keep=0;for(size_t i=0;i<names.size();++i)if(!names[i].empty()&&source.compare(p,names[i].size(),names[i])==0&&(p==0||!word(source[p-1]))&&(p+names[i].size()==source.size()||!word(source[p+names[i].size()])))keep=std::max(keep,names[i].size());
        if(keep){result.append(source,p,keep);p+=keep;continue;}size_t best=0;const char* key=0;
        const std::vector<size_t>& candidates=index.buckets[(unsigned char)source[p]];
        for(size_t c=0;c<candidates.size();++c){size_t i=candidates[c];const std::string& a=index.values[i];if(a.size()<=best||source.compare(p,a.size(),a)!=0)continue;if(word(a[0])&&p&&word(source[p-1]))continue;if(word(a[a.size()-1])&&p+a.size()<source.size()&&word(source[p+a.size()]))continue;best=a.size();key=kLocalizationAliases[i].key;}
        // Already translated fragments must not undergo a second substitution.
        const std::vector<std::string>& targets=e.targets[(unsigned char)source[p]];size_t translated=0;
        for(size_t t=0;t<targets.size();++t){const std::string& a=targets[t];if(a.size()<best||a.size()<=translated||source.compare(p,a.size(),a)!=0)continue;if(word(a[0])&&p&&word(source[p-1]))continue;if(word(a[a.size()-1])&&p+a.size()<source.size()&&word(source[p+a.size()]))continue;translated=a.size();}
        if(translated){result.append(source,p,translated);p+=translated;continue;}
        if(key){result+=e.get(key);p+=best;}else result+=source[p++];
    }if(e.rendered.size()>=4096||e.cache.size()>=4096){e.rendered.clear();e.cache.clear();}e.rendered.insert(result);e.cache[source]=result;return result;
}
}
