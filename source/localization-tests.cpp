#include "Localization.h"
#include "QuestTrackerText.h"
#include "GuildLevelUnlocks.h"
#include <cassert>
#include <iostream>
#include <direct.h>
void write(const char* name,const std::string& bytes){std::ofstream out((std::string("localization-test-data/")+name).c_str(),std::ios::binary);out<<bytes;}
int main(){
    assert(Loc::normalize("French")=="fr");assert(Loc::normalize("de-DE")=="de_de");assert(Loc::normalize("../../secret")=="en");
    assert(Loc::Json("{\"a\":\"\\u00e9\\ud83c\\udf0d\"}").read()["a"]=="\xC3\xA9\xF0\x9F\x8C\x8D");
    bool rejected=false;try{Loc::Json("{\"a\":\"x\",\"a\":\"y\"}").read();}catch(...){rejected=true;}assert(rejected);
    Loc::configure("Localization","fr_FR");assert(std::string(Loc::text("common.continue"))=="CONTINUER");
    std::vector<std::string> names;names.push_back("Soldat");std::string old="Bonjour. Nous sommes Soldat";
    Loc::configure("Localization","en");assert(Loc::legacy(old,names).find("Soldat")!=std::string::npos);
    names.clear();assert(Loc::legacy(old,names).find("Soldier")!=std::string::npos);
    assert(Loc::legacy("Sept jours. Pas un de plus. Nous reviendrons a cette date.",names)=="Seven days. Not one day more. We will return on that date.");
    _mkdir("localization-test-data");
    write("de.json","{\"common.continue\":\"WEITER\"}");
    Loc::configure("localization-test-data","german");assert(Loc::engine().language=="en");assert(std::string(Loc::text("common.continue"))=="CONTINUE");
    assert(Loc::resolveLanguage("fr-FR")=="fr");assert(Loc::resolveLanguage("en_GB")=="en");
    assert(Loc::resolveLanguage("polish")=="pl");assert(Loc::resolveLanguage("pl_PL")=="pl");
    assert(Loc::resolveLanguage("russian")=="ru");assert(Loc::resolveLanguage("RU-ru")=="ru");
    assert(Loc::resolveLanguage("Japanese")=="en");
    write("pl.json","{\"quest.active_count.one\":\"{count} zadanie\",\"quest.active_count.few\":\"{count} zadania\",\"quest.active_count.many\":\"{count} zadan\"}");Loc::configure("localization-test-data","polish");
    assert(QuestTrackerText::count(1,true)=="1 zadanie");assert(QuestTrackerText::count(2,true)=="2 zadania");assert(QuestTrackerText::count(12,true)=="12 zadan");assert(QuestTrackerText::count(22,true)=="22 zadania");
    assert(!Loc::compatible("%d Cats","%s Cats"));assert(!Loc::compatible("%d Cats","%n Cats"));assert(!Loc::compatible("{count} quests","quests"));
    write("ru.json","{\"common.continue\":\"SEGUIR\",\"common.continue\":\"OTHER\"}");Loc::configure("localization-test-data","ru");assert(std::string(Loc::text("common.continue"))=="CONTINUE");assert(!Loc::engine().error.empty());
    Loc::configure("localization-test-data","ja");assert(Loc::engine().language=="en");
    Loc::configure("Localization","fr");assert(std::string(GuildLevelUI::unlocks(1)[0].fr)=="PRIMES DE FIN DE MISSION");Loc::configure("Localization","en");assert(std::string(GuildLevelUI::unlocks(1)[0].en)=="MISSION BONUSES");
    Loc::Catalogue en;std::string error;assert(Loc::readFile("Localization/en.json",en,error));
    const char* languages[]={"en","fr","pl","ru"};
    for(int language=0;language<4;++language){
        Loc::Catalogue cat;assert(Loc::readFile(std::string("Localization/")+languages[language]+".json",cat,error));assert(cat.size()==en.size());
        Loc::configure("Localization",languages[language]);assert(Loc::engine().language==languages[language]);names.clear();
        for(Loc::Catalogue::iterator i=en.begin();i!=en.end();++i)if(i->first[0]!='_'){
            assert(cat.count(i->first));assert(!cat[i->first].empty());
            if(!Loc::compatible(i->second,cat[i->first])){std::cerr<<"FORMAT MISMATCH "<<languages[language]<<" "<<i->first<<"\n";return 1;}
            std::string translated=Loc::text(i->first.c_str());assert(translated==cat[i->first]);
            if(Loc::legacy(translated,names)!=translated){std::cerr<<"DOUBLE TRANSLATION "<<languages[language]<<" "<<i->first<<"\n";return 1;}
        }
        assert(std::string(Loc::text("missing.test.key"))==cat["common.text_unavailable"]);
    }
    Loc::configure("Localization","pl");std::string saved=Loc::named("mission.escort.story.0","destination","World's End");
    Loc::configure("Localization","ru");assert(Loc::legacy(saved,names)==Loc::named("mission.escort.story.0","destination","World's End"));
    names.push_back(saved);assert(Loc::legacy(saved,names)==saved);names.clear();
    Loc::configure("Localization","ru");assert(std::string(Loc::pluralForm(1))=="one");assert(std::string(Loc::pluralForm(21))=="one");assert(std::string(Loc::pluralForm(11))=="many");assert(std::string(Loc::pluralForm(22))=="few");assert(std::string(Loc::pluralForm(112))=="many");
    Loc::configure("Localization","pl");assert(std::string(Loc::pluralForm(1))=="one");assert(std::string(Loc::pluralForm(21))=="many");assert(std::string(Loc::pluralForm(22))=="few");assert(std::string(Loc::pluralForm(112))=="many");
    std::cout<<"PASS: four complete catalogues; UTF-8, formats, locale resolution, unsupported fallback, malformed/partial files, PL/RU plurals, legacy text and protected names\n";
}
