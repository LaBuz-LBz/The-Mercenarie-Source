#pragma once
#include <cstdlib>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include "../Localization/LanguageCode.h"
#include "../../MainMenuNewsRules.h"

namespace ClientOptions {

enum Scope { GlobalPreference, PerSaveSetting, SessionPreference };
enum Type { Toggle, Choice, Range, KeyBinding, Command };
enum Category { General, Gameplay, Interface, Contracts, Notifications, Keys, Data, Development, CategoryCount };
enum Action { OpenGuildManagement, OpenCheatMenu, OpenAutopilot, ActionCount };

struct OptionDefinition {
    const char* id; Category category; Type type; Scope scope;
    const char* titleKey; const char* descriptionKey;
    OptionDefinition(const char* i,Category c,Type t,Scope s,const char* title,const char* description)
        :id(i),category(c),type(t),scope(s),titleKey(title),descriptionKey(description){}
};

inline const std::vector<OptionDefinition>& definitions(){
    static std::vector<OptionDefinition> value;
    if(value.empty()){
        value.push_back(OptionDefinition("general.client_tracker",General,Toggle,GlobalPreference,"options.client_tracker.title","options.client_tracker.description"));
        value.push_back(OptionDefinition("gameplay.reward_difficulty",Gameplay,Range,PerSaveSetting,"options.reward.title","options.reward.description"));
        value.push_back(OptionDefinition("general.language",General,Choice,GlobalPreference,"options.language.title","options.language.description"));
        value.push_back(OptionDefinition("general.cheat_menu",General,Toggle,SessionPreference,"options.cheat_menu.title","options.cheat_menu.description"));
        value.push_back(OptionDefinition("gameplay.payroll",Gameplay,Toggle,PerSaveSetting,"payroll.enabled","payroll.enabled_tip"));
        value.push_back(OptionDefinition("gameplay.real_estate",Gameplay,Toggle,GlobalPreference,"options.real_estate.title","options.real_estate.description"));
        value.push_back(OptionDefinition("interface.tooltips",Interface,Toggle,GlobalPreference,"options.tooltips.title","options.tooltips.description"));
        value.push_back(OptionDefinition("interface.routes",Interface,Toggle,GlobalPreference,"options.routes.title","options.routes.description"));
        value.push_back(OptionDefinition("contracts.details",Contracts,Toggle,GlobalPreference,"options.details.title","options.details.description"));
        value.push_back(OptionDefinition("contracts.confirm_accept",Contracts,Toggle,GlobalPreference,"options.confirm_accept.title","options.confirm_accept.description"));
        value.push_back(OptionDefinition("contracts.confirm_delegate",Contracts,Toggle,GlobalPreference,"options.confirm_delegate.title","options.confirm_delegate.description"));
        value.push_back(OptionDefinition("notifications.master",Notifications,Toggle,GlobalPreference,"options.notifications.title","options.notifications.description"));
        value.push_back(OptionDefinition("notifications.duration",Notifications,Range,GlobalPreference,"options.notification_duration.title","options.notification_duration.description"));
        value.push_back(OptionDefinition("notifications.mission",Notifications,Toggle,GlobalPreference,"options.mission_notification.title","options.mission_notification.description"));
        value.push_back(OptionDefinition("notifications.delegated",Notifications,Toggle,GlobalPreference,"options.delegated_notification.title","options.delegated_notification.description"));
        value.push_back(OptionDefinition("keys.guild",Keys,KeyBinding,GlobalPreference,"options.key.guild","options.key.description"));
        value.push_back(OptionDefinition("keys.cheats",Keys,KeyBinding,GlobalPreference,"options.key.cheats","options.key.description"));
        value.push_back(OptionDefinition("keys.autopilot",Keys,KeyBinding,GlobalPreference,"options.key.autopilot","options.key.description"));
        value.push_back(OptionDefinition("data.diagnostic",Data,Command,GlobalPreference,"options.diagnostic.title","options.diagnostic.description"));
        value.push_back(OptionDefinition("data.reset_ui",Data,Command,GlobalPreference,"options.reset_ui.title","options.reset_ui.description"));
        value.push_back(OptionDefinition("development.logs",Development,Toggle,GlobalPreference,"options.dev_logs.title","options.dev_logs.description"));
    }return value;
}

struct Settings {
    bool realEstateEnabled;
    bool showClientTracker,showTooltips,showRoutes,detailedOffers,confirmAccept,confirmDelegate;
    bool showNotifications,notifyMissionComplete,notifyDelegatedComplete;
    bool developerMode,detailedLogs,showInternalIds,debugRoutes,debugPersistence;
    int notificationDuration,bindings[ActionCount]; int guildManagementHotkey; std::string language;
    Settings(bool showTracker=true,int guildHotkey=0,int cheatHotkey=0,int autopilotHotkey=0)
        :realEstateEnabled(true),showClientTracker(showTracker),showTooltips(true),showRoutes(true),detailedOffers(true),confirmAccept(true),confirmDelegate(true),
         showNotifications(true),notifyMissionComplete(true),notifyDelegatedComplete(true),developerMode(false),detailedLogs(false),showInternalIds(false),debugRoutes(false),debugPersistence(false),notificationDuration(6),guildManagementHotkey(guildHotkey),language("auto"){
        bindings[OpenGuildManagement]=guildHotkey;bindings[OpenCheatMenu]=cheatHotkey;bindings[OpenAutopilot]=autopilotHotkey;
    }
};

inline bool parseBool(const std::string& value,bool fallback){return value=="1"?true:value=="0"?false:fallback;}
inline int clampDuration(int value){return value<3?3:value>15?15:value;}
inline bool validLanguage(const std::string& value){return LanguageCode::valid(value);}

template<class ValidKey>
void read(std::istream& input,Settings& settings,ValidKey validKey){
    std::string line;bool first=true;while(std::getline(input,line)){
        if(first&&line.compare(0,3,"\xEF\xBB\xBF")==0)line.erase(0,3);first=false;
        if(!line.empty()&&line[line.size()-1]=='\r')line.erase(line.size()-1);
        const size_t equals=line.find('=');if(equals==std::string::npos)continue;
        const std::string key=line.substr(0,equals),value=line.substr(equals+1);const int number=std::atoi(value.c_str());
        if(key=="showClientTracker")settings.showClientTracker=parseBool(value,settings.showClientTracker);
        else if(key=="realEstateEnabled")settings.realEstateEnabled=parseBool(value,settings.realEstateEnabled);
        else if(key=="showTooltips")settings.showTooltips=parseBool(value,settings.showTooltips);
        else if(key=="showRoutes")settings.showRoutes=parseBool(value,settings.showRoutes);
        else if(key=="detailedOffers")settings.detailedOffers=parseBool(value,settings.detailedOffers);
        else if(key=="confirmAccept")settings.confirmAccept=parseBool(value,settings.confirmAccept);
        else if(key=="confirmDelegate")settings.confirmDelegate=parseBool(value,settings.confirmDelegate);
        else if(key=="showNotifications")settings.showNotifications=parseBool(value,settings.showNotifications);
        else if(key=="notifyMissionComplete")settings.notifyMissionComplete=parseBool(value,settings.notifyMissionComplete);
        else if(key=="notifyDelegatedComplete")settings.notifyDelegatedComplete=parseBool(value,settings.notifyDelegatedComplete);
        else if(key=="notificationDuration")settings.notificationDuration=clampDuration(number);
        else if(key=="language"&&validLanguage(value))settings.language=value;
        else if(key=="developerMode"){/* Session-only: never enable cheats from a previous launch. */}
        else if(key=="detailedLogs")settings.detailedLogs=parseBool(value,settings.detailedLogs);
        else if(key=="showInternalIds")settings.showInternalIds=parseBool(value,settings.showInternalIds);
        else if(key=="debugRoutes")settings.debugRoutes=parseBool(value,settings.debugRoutes);
        else if(key=="debugPersistence")settings.debugPersistence=parseBool(value,settings.debugPersistence);
        else if(key=="guildManagementHotkey"&&validKey(number)){settings.bindings[OpenGuildManagement]=number;settings.guildManagementHotkey=number;}
        else if(key=="cheatMenuHotkey"&&validKey(number))settings.bindings[OpenCheatMenu]=number;
        else if(key=="autopilotHotkey"&&(number==0||validKey(number)))settings.bindings[OpenAutopilot]=number;
    }
}
inline void write(std::ostream& output,bool showClientTracker,int guildManagementHotkey){output<<"showClientTracker="<<(showClientTracker?1:0)<<"\nguildManagementHotkey="<<guildManagementHotkey<<"\n";}

inline void write(std::ostream& output,const Settings& s){
    output<<"realEstateEnabled="<<(s.realEstateEnabled?1:0)<<"\n";
    output<<"showClientTracker="<<(s.showClientTracker?1:0)<<"\nshowTooltips="<<(s.showTooltips?1:0)<<"\nshowRoutes="<<(s.showRoutes?1:0)
          <<"\ndetailedOffers="<<(s.detailedOffers?1:0)<<"\nconfirmAccept="<<(s.confirmAccept?1:0)<<"\nconfirmDelegate="<<(s.confirmDelegate?1:0)
          <<"\nshowNotifications="<<(s.showNotifications?1:0)<<"\nnotificationDuration="<<s.notificationDuration
          <<"\nnotifyMissionComplete="<<(s.notifyMissionComplete?1:0)<<"\nnotifyDelegatedComplete="<<(s.notifyDelegatedComplete?1:0)
          <<"\nlanguage="<<s.language<<"\nguildManagementHotkey="<<s.bindings[OpenGuildManagement]
          <<"\ncheatMenuHotkey="<<s.bindings[OpenCheatMenu]<<"\nautopilotHotkey="<<s.bindings[OpenAutopilot]
          <<"\ndeveloperMode=0"<<"\ndetailedLogs="<<(s.detailedLogs?1:0)
          <<"\nshowInternalIds="<<(s.showInternalIds?1:0)<<"\ndebugRoutes="<<(s.debugRoutes?1:0)
          <<"\ndebugPersistence="<<(s.debugPersistence?1:0)<<"\n";
}
inline int conflict(const Settings& s,Action action,int key){if(key==0)return -1;for(int i=0;i<ActionCount;++i)if(i!=(int)action&&s.bindings[i]==key)return i;return -1;}
inline const char* actionId(Action action){return action==OpenGuildManagement?"guild":action==OpenCheatMenu?"cheats":"autopilot";}

}
