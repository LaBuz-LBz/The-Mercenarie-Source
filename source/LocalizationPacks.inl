// Included inside namespace Loc, after the JSON and format validators.
struct LanguagePack {
    std::string code,name,englishName,author,version,catalogueVersion,fontResource,pluralRule,source;
    Catalogue messages;
    bool official;
    LanguagePack():official(false){}
};
inline bool displayName(const std::string& s){
    if(s.empty()||s.size()>160)return false;
    for(size_t i=0;i<s.size();++i)if((unsigned char)s[i]<32||s[i]=='#'||s[i]==127)return false;
    return true;
}
inline bool fontIdentifier(const std::string& s){
    if(s.size()>96)return false;
    for(size_t i=0;i<s.size();++i)if(!((s[i]>='a'&&s[i]<='z')||(s[i]>='A'&&s[i]<='Z')||(s[i]>='0'&&s[i]<='9')||s[i]=='_'||s[i]=='-'))return false;
    return true;
}
inline bool ordinaryPath(const std::string& path,bool directory){
    DWORD a=UnicodeFileSystem::attributes(path);
    return a!=INVALID_FILE_ATTRIBUTES&&!(a&FILE_ATTRIBUTE_REPARSE_POINT)&&((a&FILE_ATTRIBUTE_DIRECTORY)!=0)==directory;
}
struct LanguageRegistry {
    std::vector<LanguagePack> packs;
    std::set<std::string> scanned;
    std::set<std::string> requestedRoots;
    std::vector<std::string> diagnostics;
    unsigned int revision,scanCount;
    LanguageRegistry():revision(0),scanCount(0){
        const char* codes[]={"fr","en","pl","ru"};
        for(int i=0;i<4;++i){LanguagePack p;p.code=codes[i];p.official=true;packs.push_back(p);}
    }
    const LanguagePack* find(const std::string& code) const {
        for(size_t i=0;i<packs.size();++i)if(packs[i].code==code)return &packs[i];return 0;
    }
    std::string resolve(const std::string& code)const {
        std::string normalized=normalize(code);
        if(find(normalized))return normalized;
        std::string base=normalized.substr(0,normalized.find('_'));
        return find(base)?base:"en";
    }
    void scan(const std::string& root){
        // Only caller-supplied roots; manifest fields never supply filesystem paths.
        if(!requestedRoots.insert(root).second)return;
        std::wstring wide=UnicodeFileSystem::existingPath(root);
        wchar_t absolute[32768];DWORD length=GetFullPathNameW(wide.c_str(),32768,absolute,0);
        if(length&&length<32768)wide.assign(absolute,length);
        while(wide.size()>3&&(wide[wide.size()-1]==L'\\'||wide[wide.size()-1]==L'/'))wide.erase(wide.size()-1);
        std::string identity=UnicodeFileSystem::utf8(wide);
        for(size_t i=0;i<identity.size();++i)if(identity[i]>='A'&&identity[i]<='Z')identity[i]+='a'-'A';
        if(!scanned.insert(identity).second)return;
        ++scanCount;
        if(!ordinaryPath(root,true))return;
        WIN32_FIND_DATAW data;HANDLE search=FindFirstFileW((wide+L"\\*").c_str(),&data);
        if(search==INVALID_HANDLE_VALUE)return;
        std::vector<std::string> folders;
        do {if((data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)&&!(data.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)){
            std::string leaf=UnicodeFileSystem::utf8(data.cFileName);
            if(LanguageCode::valid(leaf)&&normalize(leaf)!="auto")folders.push_back(leaf);
            else if(leaf!="."&&leaf!="..")diagnostics.push_back(root+"/"+leaf+": rejected: invalid language folder/code");
        }else if(!(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)){
            std::string leaf=UnicodeFileSystem::utf8(data.cFileName);
            if(leaf.size()>5&&leaf.substr(leaf.size()-5)==".json"){
                std::string code=leaf.substr(0,leaf.size()-5);
                if(LanguageCode::valid(code)&&!find(normalize(code)))
                    diagnostics.push_back(root+"/"+leaf+": ignored flat catalogue; use Localization/<code>/language.json and messages.json");
            }
        }}while(FindNextFileW(search,&data));FindClose(search);
        std::sort(folders.begin(),folders.end());
        for(size_t i=0;i<folders.size()&&packs.size()<132;++i){
            std::string folder=root+"/"+folders[i],error;
            Catalogue manifest,messages;
            if(!ordinaryPath(folder+"/language.json",false)||!ordinaryPath(folder+"/messages.json",false)){
                diagnostics.push_back(folder+": rejected: missing, inaccessible or reparse language.json/messages.json");continue;
            }
            if(!readFile(folder+"/language.json",manifest,error,16384,true)||!readFile(folder+"/messages.json",messages,error)){
                diagnostics.push_back(error);continue;
            }
            LanguagePack p;p.code=normalize(manifest["code"]);p.name=manifest["name"];p.englishName=manifest["englishName"];
            p.author=manifest["author"];p.version=manifest["version"];p.catalogueVersion=manifest["catalogueVersion"];
            p.fontResource=manifest["fontResource"];p.pluralRule=manifest["pluralRule"];p.source=folder;
            if(manifest["schemaVersion"]!="1"){
                diagnostics.push_back(folder+": rejected: schemaVersion must be 1 (string or integer)");continue;
            }
            if(!LanguageCode::valid(manifest["code"])||p.code!=normalize(folders[i])||p.code=="auto"||
               !displayName(p.name)||!displayName(p.englishName)||!displayName(p.author)||!displayName(p.version)||!displayName(p.catalogueVersion)||
               !fontIdentifier(p.fontResource)||!(p.pluralRule.empty()||p.pluralRule=="one-other"||p.pluralRule=="other"||p.pluralRule=="fr"||p.pluralRule=="pl"||p.pluralRule=="ru")){
                diagnostics.push_back(folder+": rejected: invalid code, metadata, pluralRule or fontResource");continue;
            }
            const LanguagePack* existing=find(p.code);
            // The first valid provider wins. Official languages cannot be shadowed.
            if(existing){diagnostics.push_back(folder+": duplicate language "+p.code+" ignored; retained="+(existing->official?"official":existing->source));continue;}
            p.messages.swap(messages);packs.push_back(p);++revision;
            std::ostringstream info;info<<folder<<": accepted code="<<p.code<<" name="<<p.name<<" author="<<p.author<<" version="<<p.version<<" catalogueVersion="<<p.catalogueVersion<<" entries="<<p.messages.size();diagnostics.push_back(info.str());
        }
    }
};
inline LanguageRegistry& registry(){static LanguageRegistry value;return value;}
inline std::string resolveLanguage(const std::string& code){return registry().resolve(code);}
