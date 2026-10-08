#include "Localization.h"
#include "src/UI/ClientOptions.h"
#include <cassert>
#include <iostream>
#include <climits>
struct ModInfo {std::string path;bool isBaseMod;ModInfo(const std::string& p):path(p),isBaseMod(false){}};
struct GameWorld {std::vector<ModInfo*> activeMods;};
#include "community-adapter-test.generated.h"
struct ValidKey {bool operator()(int n)const{return n>=0;}};
int main(int argc,char** argv){
    assert(argc==2);std::string root=argv[1];
    Loc::configure(root+"/local","en");
    assert(Loc::registry().packs.size()==4&&Loc::languageChoiceCount()==5);
    assert(Loc::resolveLanguage("Japanese")=="en");
    for(size_t i=0;i<4;++i)assert(Loc::registry().packs[i].official);
    GameWorld world;CommunityTranslations::discover(0);CommunityTranslations::discover(&world);
    ModInfo first(root+"/external"),second(root+"/later"),base(root+"/disabled");base.isBaseMod=true;
    world.activeMods.push_back(&base);
    CommunityTranslations::discover(&world); // non-empty partial list must not freeze discovery
    world.activeMods.push_back(&first);world.activeMods.push_back(&second);
    CommunityTranslations::discover(&world);
    assert(Loc::registry().find("zh_tw")&&!Loc::registry().find("es"));
    assert(Loc::registry().packs.size()==6); // zh-TW plus a Unicode parent path pack added below
    assert(Loc::registry().find("de"));
    assert(Loc::languageChoiceCode(0)=="auto");
    assert(Loc::resolveLanguage("tchinese")=="zh_tw");
    assert(Loc::resolveLanguage("fr-FR")=="fr");
    Loc::select("zh-TW");
    assert(Loc::engine().language=="zh_tw");
    assert(std::string(Loc::text("common.continue"))=="\xE7\xB9\xBC\xE7\xBA\x8C");
    assert(std::string(Loc::text("contract.refuse"))==Loc::engine().english["contract.refuse"]);
    assert(std::string(Loc::text("common.text_unavailable"))!="common.text_unavailable");
    assert(std::string(Loc::text("not.a.real.key"))=="Text unavailable.");
    assert(std::string(Loc::text("contract.accept"))!="ACCEPT %s"); // format injection rejected
    assert(std::string(Loc::text("options.subtitle"))!=""); // blank fallback
    assert(Loc::count("quest.active_count",1)=="1 \xE5\x80\x8B\xE4\xBB\xBB\xE5\x8B\x99");
    assert(std::string(Loc::pluralForm(INT_MIN))=="other");
    const Loc::LanguagePack* pack=Loc::registry().find("zh_tw");
    assert(pack&&pack->catalogueVersion=="V8-old"&&pack->source==root+"/external/Localization/zh-TW");
    assert(Loc::engine().selected.size()<Loc::engine().english.size());
    assert(!Loc::registry().diagnostics.empty());
    unsigned int scans=Loc::registry().scanCount;
    for(int i=0;i<10000;++i){CommunityTranslations::discover(&world);Loc::select("tchinese");}
    assert(scans==Loc::registry().scanCount);
    std::vector<std::string> names;Loc::legacy("CONTINUE",names);assert(!Loc::engine().cache.empty());
    const char* stable=Loc::text("common.continue");Loc::select("en");assert(Loc::engine().cache.empty());
    assert(std::string(stable)=="\xE7\xB9\xBC\xE7\xBA\x8C");Loc::select("zh_tw");
    ClientOptions::Settings saved;saved.language="zh_tw";std::ostringstream bytes;ClientOptions::write(bytes,saved);
    ClientOptions::Settings loaded;std::istringstream input(bytes.str());ClientOptions::read(input,loaded,ValidKey());assert(loaded.language=="zh_tw");
    assert(!ClientOptions::validLanguage("../zh-TW")&&!ClientOptions::validLanguage("C:\\evil")&&!ClientOptions::validLanguage("zh--TW"));
    assert(!Loc::compatible("{count}","count")&&!Loc::compatible("%d","%s")&&!Loc::compatible("ok","%n"));
    assert(!Loc::compatible("ok","%1$n")&&!Loc::compatible("ok","%1$s"));
    assert(!Loc::compatible("ok",std::string("ok\0evil",7)));
    assert(Loc::Json("{\"x\":\"\\ud83c\\udf0d\"}").read()["x"]=="\xF0\x9F\x8C\x8D");
    bool bad=false;try{Loc::Json("{\"x\":\"\xC0\xAF\"}").read();}catch(...){bad=true;}assert(bad);
    ModInfo late(root+"/\xE4\xB8\xAD\xE6\x96\x87");world.activeMods.push_back(&late);
    CommunityTranslations::discover(&world);assert(Loc::registry().find("ja"));
    assert(Loc::registry().find("zh_cn"));Loc::select("schinese");assert(Loc::engine().language=="zh_cn");
    assert(std::string(Loc::text("common.continue"))=="ZH TEST");
    assert(Loc::Json("{\"schemaVersion\":1}").read(true)["schemaVersion"]=="1");
    bad=false;try{Loc::Json("{\"value\":1}").read();}catch(...){bad=true;}assert(bad);
    Loc::select("ja_JP");assert(Loc::engine().language=="ja"&&std::string(Loc::pluralForm(1))=="other");
    assert(!Loc::registry().find("it")&&!Loc::registry().find("pt")&&!Loc::registry().find("nl")&&!Loc::registry().find("ko"));
    assert(!Loc::registry().find("sv")&&!Loc::registry().find("fi")&&!Loc::registry().find("da")&&!Loc::registry().find("cs"));
    Loc::registry()=Loc::LanguageRegistry();Loc::engine().loaded=false;Loc::select(loaded.language);
    assert(Loc::engine().language=="en"&&loaded.language=="zh_tw"); // restart without provider
    CommunityTranslations::discover(&world);Loc::select(loaded.language);assert(Loc::engine().language=="zh_tw");
    Loc::configure(root+"/internal/Localization","ZH-CN");assert(Loc::engine().language=="zh_cn");
    std::cout<<"PASS: production active-mod adapter, official entries, external discovery/order, UTF-8 paths, cached discovery, incomplete/old packs, selection, EN fallback, formats, plural rules, persistence and invalid packs\n";
}
