
namespace OIS{enum{KC_J=36,KC_P=25};}
namespace MyGUI{struct Widget{std::string getUserString(const char*){return "0";}};}
namespace Loc{const char* text(const char*){return "Could not save this option";}}
struct World{int failures;World():failures(0){}void showPlayerAMessage(const char*,bool){++failures;}};World testWorld;World* ou=&testWorld;
void DebugLog(const std::string& message){std::cout<<message;}
ClientOptions::Settings clientOptions(false,36,25,0);bool guildKeyCapture=false;int pendingConflictAction=-1,pendingConflictKey=0;
bool validGuildKey(int value){return value>0&&value<256;}
void refreshOptionKeyLabels(){}

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
bool saveBinding(ClientOptions::Action action,int value,bool reassign){
        if(value!=0&&!validGuildKey(value))return false;ClientOptions::Settings next=clientOptions;int owner=ClientOptions::conflict(next,action,value);if(owner>=0&&!reassign){pendingConflictAction=owner;pendingConflictKey=value;return false;}if(owner>=0)next.bindings[owner]=0;next.bindings[action]=value;if(!saveClientOptions(next))return false;pendingConflictAction=-1;pendingConflictKey=0;refreshOptionKeyLabels();return true;
    }
void resetGuildKey(MyGUI::Widget* sender){guildKeyCapture=false;int action=sender?atoi(sender->getUserString("action").c_str()):0;action=std::max(0,std::min((int)ClientOptions::ActionCount-1,action));int defaults[]={OIS::KC_J,OIS::KC_P,0};saveBinding((ClientOptions::Action)action,defaults[action],true);}
namespace NewsPolicyTests {
inline std::string trim(const std::string& s){size_t a=0,b=s.size();while(a<b&&(s[a]==' '||s[a]=='\t'||s[a]=='\r'))++a;while(b>a&&(s[b-1]==' '||s[b-1]=='\t'||s[b-1]=='\r'))--b;return s.substr(a,b-a);}
inline std::string dismissedVersion(){
    ClientOptionsFile::Failure failure;std::wstring file=ClientOptionsFile::path(failure);std::string bytes;
    if(file.empty()||!ClientOptionsFile::read(file,bytes,failure))return "";
    return MainMenuNewsRules::dismissedPreference(bytes);
}
inline bool persistDismissed(){
    ClientOptionsFile::Failure failure;std::wstring file=ClientOptionsFile::path(failure);std::string bytes;
    if(file.empty()||(!ClientOptionsFile::read(file,bytes,failure)&&failure.code!=ERROR_FILE_NOT_FOUND))return false;
    std::istringstream in(bytes);std::ostringstream out;std::string line;
    while(std::getline(in,line)){size_t p=line.find('=');if(p!=std::string::npos&&trim(line.substr(0,p))=="lastDismissedNewsVersion")continue;out<<line<<"\n";}
    out<<"lastDismissedNewsVersion="<<MainMenuNewsRules::CurrentNewsVersion<<"\n";
    return ClientOptionsFile::write(file,out.str(),failure);
}


}
