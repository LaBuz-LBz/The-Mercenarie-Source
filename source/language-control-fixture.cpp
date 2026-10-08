
#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <iostream>
#include "Localization.h"
#include "src/UI/ClientOptions.h"
#include "src/UI/ClientOptionsFile.h"
void DebugLog(const std::string& s){std::cerr<<s;}
namespace MyGUI {
struct ComboBox;
typedef void (*Callback)(ComboBox*,size_t);
Callback newDelegate(Callback cb){return cb;}
struct Event {Callback callback;Event():callback(0){}void operator+=(Callback c){callback=c;}};
struct Align {enum {Default=0};};
struct Widget {Widget* parent;int x,y,w,h;bool visible,enabled,mouse;std::string skin,name;Widget():parent(0),x(0),y(0),w(1000),h(92),visible(true),enabled(true),mouse(true){}int getWidth(){return w;}
 template<class T>T* createWidget(const char* sk,int a,int b,int c,int d,int,const char* n){T* t=new T;t->parent=this;t->x=a;t->y=b;t->w=c;t->h=d;t->skin=sk;t->name=n;return t;}
};
ComboBox* created=0;
struct ComboBox:Widget {Event eventComboChangePosition;bool drop,open;size_t selected;std::vector<std::string> items;ComboBox():drop(false),open(false),selected(0){created=this;}
 void setVisible(bool v){visible=v;}void setEnabled(bool v){enabled=v;}void setNeedMouseFocus(bool v){mouse=v;}void setComboModeDrop(bool v){drop=v;}void setFontName(const char*){}void setFontHeight(int){}
 void addItem(const std::string& v){items.push_back(v);}void setIndexSelected(size_t i){selected=i;}
 void click(){assert(visible&&enabled&&mouse&&drop);open=true;}
 void choose(size_t i){assert(open&&i<items.size()&&eventComboChangePosition.callback);selected=i;eventComboChangePosition.callback(this,i);open=false;}
};
}
namespace MercenarieFonts {void ensure(){}void languageChoice(MyGUI::ComboBox*){}}
struct LocaleInfo {std::string id,steamCode;};
struct LocaleManager {LocaleInfo locale;static LocaleManager* getInstance(){static LocaleManager m;return &m;}LocaleInfo* getCurrentLocale(){return &locale;}};
struct World {void showPlayerAMessage(const char*,bool){assert(false && "save failure");}};World world;World* ou=&world;
namespace CommunityTranslations {void discover(World*){}}
ClientOptions::Settings clientOptions;bool optionsRebuildRequested=false,gMercenarieEnglish=false;std::string gMercenarieLanguageOverride="auto";
struct Valid {bool operator()(int n)const{return n>0&&n<200;}};

static std::string readClientOptionsBytes(){ClientOptionsFile::Failure failure;std::wstring file=ClientOptionsFile::path(failure);std::string bytes;if(file.empty()||!ClientOptionsFile::read(file,bytes,failure)){if(failure.code!=ERROR_FILE_NOT_FOUND)DebugLog(failure.message());}return bytes;}
static void detectMercenarieLanguage()
{
    // Read the global preference before the first menu frame, including fresh launches.
    CommunityTranslations::discover(ou);
    static bool preferenceLoaded=false;
    if(!preferenceLoaded){preferenceLoaded=true;std::istringstream options(readClientOptionsBytes());std::string line;while(std::getline(options,line)){if(!line.empty()&&line[line.size()-1]=='\r')line.erase(line.size()-1);if(line.compare(0,9,"language=")==0){std::string value=line.substr(9);if(ClientOptions::validLanguage(value))gMercenarieLanguageOverride=value;}}}
    LocaleManager* manager=LocaleManager::getInstance();
    LocaleInfo* locale=manager?manager->getCurrentLocale():0;
    std::string language="en";
    if(locale)language=locale->id.empty()?locale->steamCode:locale->id;
    else {std::ifstream settings("settings.cfg");std::string line;while(std::getline(settings,line))if(line.compare(0,9,"language=")==0){language=line.substr(9);break;}}
    Loc::select(gMercenarieLanguageOverride=="auto"?language:gMercenarieLanguageOverride);
    static size_t notices=0;
    const std::vector<std::string>& log=Loc::registry().diagnostics;
    while(notices<log.size())DebugLog("Mercenarie translations: "+log[notices++]);
    static std::string lastSelection;
    std::ostringstream selection;selection<<gMercenarieLanguageOverride<<"/"<<language<<"/"<<Loc::engine().language<<"/"<<Loc::registry().revision;
    if(lastSelection!=selection.str()){
        lastSelection=selection.str();std::ostringstream summary;
        summary<<"Mercenarie translations: requested="<<gMercenarieLanguageOverride<<" gameLocale="<<language<<" selected="<<Loc::engine().language<<" translated="<<Loc::engine().selected.size()<<" englishFallback="<<(Loc::engine().english.size()-Loc::engine().selected.size());
        DebugLog(summary.str());
    }
    gMercenarieEnglish=Loc::engine().language.substr(0,2)!="fr";
}
bool saveClientOptions(const ClientOptions::Settings& next){
        ClientOptionsFile::Failure failure;std::wstring file=ClientOptionsFile::path(failure);
        std::string previous;
        if(!file.empty()&&!ClientOptionsFile::read(file,previous,failure)&&failure.code!=ERROR_FILE_NOT_FOUND){DebugLog(failure.message());if(ou)ou->showPlayerAMessage(Loc::text("ui.could_not_save_this_option"),true);return false;}
        std::ostringstream out;out.imbue(std::locale::classic());ClientOptions::write(out,next);
        const std::string dismissed=MainMenuNewsRules::dismissedPreference(previous);
        if(!dismissed.empty())out<<"lastDismissedNewsVersion="<<dismissed<<"\n";
        if(file.empty()||!out.good()||!ClientOptionsFile::write(file,out.str(),failure)){
            if(!out.good())failure.set("save","serialize",file,ERROR_INVALID_DATA);
            DebugLog(failure.message());if(ou)ou->showPlayerAMessage(Loc::text("ui.could_not_save_this_option"),true);return false;
        }
        clientOptions=next;return true;
    }
void optionLanguageChanged(MyGUI::ComboBox*,size_t index){if(index>=Loc::languageChoiceCount())return;ClientOptions::Settings next=clientOptions;next.language=Loc::languageChoiceCode(index);if(!saveClientOptions(next))return;gMercenarieLanguageOverride=next.language;detectMercenarieLanguage();optionsRebuildRequested=true;}
void buildLanguageChoice(MyGUI::Widget* row){MercenarieFonts::ensure();MyGUI::ComboBox* choice=row->createWidget<MyGUI::ComboBox>("TheMercenarie_LanguageCombo",row->getWidth()*45/100,22,row->getWidth()*22/100,48,MyGUI::Align::Default,"MercenarieLanguageChoice");choice->setVisible(true);choice->setEnabled(true);choice->setNeedMouseFocus(true);choice->setComboModeDrop(true);choice->setFontName("MercenarieUnicode");choice->setFontHeight(21);std::string selectedCode=clientOptions.language=="auto"?"auto":Loc::resolveLanguage(clientOptions.language);size_t selected=0;for(size_t i=0;i<Loc::languageChoiceCount();++i){choice->addItem(Loc::languageChoiceLabel(i));if(selectedCode==Loc::languageChoiceCode(i))selected=i;}choice->setIndexSelected(selected);choice->eventComboChangePosition+=MyGUI::newDelegate(optionLanguageChanged);MercenarieFonts::languageChoice(choice);}

int main(int argc,char** argv){
 Loc::configure(argv[1],"en");LocaleManager::getInstance()->locale.id=argv[2];
 std::istringstream file(readClientOptionsBytes());ClientOptions::read(file,clientOptions,Valid());
 detectMercenarieLanguage();
 MyGUI::Widget row;buildLanguageChoice(&row);MyGUI::ComboBox* choice=MyGUI::created;
 assert(choice&&choice->parent==&row&&choice->visible&&choice->enabled&&choice->mouse);
 assert(choice->x>=0&&choice->y>=0&&choice->x+choice->w<=700&&choice->y+choice->h<=row.h&&choice->w>=200);
 assert(choice->items.size()==Loc::languageChoiceCount()&&choice->skin=="TheMercenarie_LanguageCombo");
 const char* values[]={"auto","fr","en","pl","ru"};for(size_t i=0;i<5;++i){std::string key=std::string("options.language.")+values[i];assert(choice->items[i]==Loc::text(key.c_str()));}
 choice->click();assert(choice->open);
 if(argc>3){size_t index=atoi(argv[3]);choice->choose(index);assert(optionsRebuildRequested&&!choice->open);assert(clientOptions.language==Loc::languageChoiceCode(index));}
 else if(clientOptions.language!="auto"&&Loc::registry().find(Loc::normalize(clientOptions.language)))assert(Loc::languageChoiceCode(choice->selected)==Loc::normalize(clientOptions.language));
 else if(clientOptions.language!="auto")assert(Loc::languageChoiceCode(choice->selected)==Loc::resolveLanguage(clientOptions.language));
 if(choice->items.size()>5)assert(choice->items[5]==Loc::languageChoiceLabel(5));
 std::cout<<Loc::engine().language<<"\n"<<clientOptions.language<<"\n"<<Loc::text("common.continue")<<"\n";
}
