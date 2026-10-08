
#include "GuildBuildingTypes.h"
#include <map>
#include <vector>
#include <sstream>
#include <cassert>
#include <iostream>
namespace Loc {const char* text(const char*){return "Guild House of ";}}
struct GameData{std::string name;};
struct Building{std::string key,name;GameData data;bool destroyed,player;int furniture;Building():destroyed(false),player(true),furniture(7){data.name="Shack";}bool isDestroyed(){return destroyed;}bool isThePlayer(){return player;}void setName(const std::string& v){name=v;}std::string getName(){return name;}GameData* getGameData(){return &data;}};
struct hand{Building* b;hand():b(0){}hand(Building* x):b(x){}hand& operator=(Building* x){b=x;return *this;}bool isNull(){return !b;}Building* getBuilding(){return b;}void setNull(){b=0;}};
namespace MyGUI{struct Widget{};typedef Widget* WidgetPtr;struct EditBox {std::string value;std::string getCaption(){return value;}};struct Window {bool visible;void setVisible(bool b){visible=b;}};}
struct World {void showPlayerAMessage(const std::string&,bool){}}world,*ou=&world;
struct FiscalState {std::string targetHouseId;long long debt;double deadline;int penalty;FiscalState():debt(9000),deadline(123),penalty(2){}};
struct Ledger{FiscalState organisations[2];}fiscalLedger;
struct Visitor{std::string houseKey;int leader;};std::vector<Visitor> guildVisitors;
std::map<std::string,std::string> guildHouseNames,guildHouseCities,guildHouseOriginalNames;
std::string pendingGuildHouseKey,designatedGuildHouseKey,currentGuildHouseKey,currentGuildHouseName,operationalAnnouncementKey;
hand pendingGuildHouseHandle,designatedGuildHouseHandle,fiscalTargetOffice;
Building *guildBuilding=0,*guildClientChair=0,*guildFiscalChair=0;
std::vector<Building*> guildClientChairs,guildWaitingChairs;
bool guildNameIsRename=false,guildClientSystemActive=false,guildHouseOperational=false;
MyGUI::EditBox edit,*guildNameEdit=&edit;MyGUI::Window win,*fiscalCollectorWindow=&win;
int level=3,saved=0,departed=0,collectors=0,refreshes=0;
std::string guildHouseKey(Building* b){return b?b->key:"";}
std::string gbReason(Building* b){return !b||b->destroyed||!b->player||guildHouseNames.count(b->key)||guildHouseNames.size()>=size_t(GuildBuildingTypes::limit(level))?"blocked":"";}
bool ensureGuildHouseCityAvailable(Building*,const std::string&,std::string& city,bool){city="The Hub";return true;}
void saveReputations(){++saved;}void saveFiscalLedger(){}void refreshOfficesView(bool){++refreshes;}void refreshGuildFurniture(bool){}
void gbClose(void*,const std::string&){pendingGuildHouseKey.clear();pendingGuildHouseHandle.setNull();guildNameIsRename=false;}
void removeGuildVisitor(int leader,bool penalty){assert(!penalty);for(size_t i=0;i<guildVisitors.size();++i)if(guildVisitors[i].leader==leader){guildVisitors.erase(guildVisitors.begin()+i);++departed;return;}}
void departFiscalParty(){++collectors;}
void confirmGuildHouseName(MyGUI::WidgetPtr)
    {
        Building* building=pendingGuildHouseHandle.isNull()?0:pendingGuildHouseHandle.getBuilding();
        if(!guildNameEdit||pendingGuildHouseKey.empty())return;
        const std::string key=pendingGuildHouseKey;
        if(guildNameIsRename){
            if(!guildHouseNames.count(key)) {gbClose(0,"");return;}
            std::string city=guildHouseCities.count(key)?guildHouseCities[key]:Loc::text("ui.unknown_city");
            std::string name=GuildBuildingTypes::cleanName(guildNameEdit->getCaption(),std::string(Loc::text("ui.guild_house_of"))+city);
            guildHouseNames[key]=name;if(building&&!building->isDestroyed()&&building->isThePlayer()&&guildHouseKey(building)==key)building->setName(name);
            if(currentGuildHouseKey==key)currentGuildHouseName=name;
            saveReputations();gbClose(0,"");refreshOfficesView(true);return;
        }
        std::string reason=gbReason(building);if(!reason.empty()){if(ou)ou->showPlayerAMessage(reason,true);gbClose(0,"");return;}
        std::string city;if(!ensureGuildHouseCityAvailable(building,key,city,false))return;
        std::string name=GuildBuildingTypes::cleanName(guildNameEdit->getCaption(),std::string(Loc::text("ui.guild_house_of"))+city);
        guildHouseOriginalNames[key]=GuildBuildingTypes::cleanName(building->getName(),"");building->setName(name);
        designatedGuildHouseKey=key;designatedGuildHouseHandle=building;guildBuilding=building;
        currentGuildHouseKey=key;guildHouseNames[key]=name;guildHouseCities[key]=city;currentGuildHouseName=name;
        operationalAnnouncementKey.clear();saveReputations();gbClose(0,"");refreshGuildFurniture(false);refreshOfficesView(true);
    }
void abandonGuildOffice(const std::string& key,Building* building)
    {
        if(!guildHouseNames.count(key))return;
        // Reuse the normal departure route: release seats and exit before cleanup.
        for(int i=int(guildVisitors.size())-1;i>=0;--i)if(guildVisitors[i].houseKey==key)removeGuildVisitor(guildVisitors[i].leader,false);
        Building* fiscalOffice=fiscalTargetOffice.isNull()?0:fiscalTargetOffice.getBuilding();
        if(fiscalOffice&&guildHouseKey(fiscalOffice)==key){departFiscalParty();fiscalTargetOffice.setNull();if(fiscalCollectorWindow)fiscalCollectorWindow->setVisible(false);}
        // Preserve debt, deadlines, penalties and fiscal history; detach only the destination.
        for(int i=0;i<2;++i)if(fiscalLedger.organisations[i].targetHouseId==key)fiscalLedger.organisations[i].targetHouseId.clear();
        if(building&&!building->isDestroyed()&&building->isThePlayer()&&guildHouseKey(building)==key){std::string original=guildHouseOriginalNames.count(key)?guildHouseOriginalNames[key]:(building->getGameData()?building->getGameData()->name:std::string());if(!original.empty())building->setName(original);}
        guildHouseNames.erase(key);guildHouseCities.erase(key);guildHouseOriginalNames.erase(key);
        if(designatedGuildHouseKey==key){designatedGuildHouseKey.clear();designatedGuildHouseHandle.setNull();guildBuilding=0;guildClientChair=0;guildClientChairs.clear();guildFiscalChair=0;guildWaitingChairs.clear();guildClientSystemActive=false;guildHouseOperational=false;currentGuildHouseKey.clear();currentGuildHouseName.clear();operationalAnnouncementKey.clear();}
        saveReputations();saveFiscalLedger();refreshGuildFurniture(false);refreshOfficesView(true);
    }
std::string serialize(){std::ostringstream out;for(std::map<std::string,std::string>::const_iterator h=guildHouseNames.begin();h!=guildHouseNames.end();++h)out<<"@house|"<<h->first<<'|'<<h->second<<'|'<<(guildHouseCities.count(h->first)?guildHouseCities[h->first]:Loc::text("ui.unknown_city"))<<'\n';for(std::map<std::string,std::string>::const_iterator h=guildHouseOriginalNames.begin();h!=guildHouseOriginalNames.end();++h)if(guildHouseNames.count(h->first))out<<"@housemeta|"<<h->first<<"|office|"<<h->second<<'\n';return out.str();}
void reload(const std::string& data){guildHouseNames.clear();guildHouseCities.clear();guildHouseOriginalNames.clear();std::istringstream in(data);std::string line;while(std::getline(in,line)){if(line.find("@housemeta|")==0){std::stringstream row(line.substr(11));std::string key,type,original;if(std::getline(row,key,'|')&&std::getline(row,type,'|')&&std::getline(row,original))guildHouseOriginalNames[key]=original;continue;}if(line.find("@house|")==0){std::stringstream house(line.substr(7));std::string key,name,city;if(std::getline(house,key,'|')&&std::getline(house,name,'|')){guildHouseNames[key]=name;if(std::getline(house,city,'|')&&!city.empty())guildHouseCities[key]=city;if(name.empty())guildHouseNames[key]=std::string(Loc::text("ui.guild_house_of"))+city;}continue;}}}

int main(){Building b;b.key="1:2";b.name="Original Shack";
 pendingGuildHouseKey=b.key;pendingGuildHouseHandle=&b;edit.value="  ";confirmGuildHouseName(0);assert(guildHouseNames[b.key]=="Guild House of The Hub");assert(guildHouseOriginalNames[b.key]=="Original Shack");
 reload(serialize());assert(guildHouseNames.size()==1&&guildHouseOriginalNames[b.key]=="Original Shack");
 guildNameIsRename=true;pendingGuildHouseKey=b.key;pendingGuildHouseHandle=&b;edit.value="\xC3\x89lite";std::string active=designatedGuildHouseKey;confirmGuildHouseName(0);assert(guildHouseNames[b.key]==edit.value&&designatedGuildHouseKey==active);
 reload(serialize());assert(guildHouseNames[b.key]==edit.value);
 Visitor v;v.houseKey=b.key;v.leader=1;guildVisitors.push_back(v);v.leader=2;guildVisitors.push_back(v);v.houseKey="remote";v.leader=3;guildVisitors.push_back(v);
 fiscalTargetOffice=&b;fiscalLedger.organisations[0].targetHouseId=b.key;fiscalLedger.organisations[1].targetHouseId="remote";
 abandonGuildOffice(b.key,&b);assert(guildHouseNames.empty()&&guildHouseCities.empty()&&guildHouseOriginalNames.empty());assert(b.name=="Original Shack"&&!b.destroyed&&b.player&&b.furniture==7);assert(departed==2&&collectors==1&&guildVisitors.size()==1&&guildVisitors[0].houseKey=="remote");
 assert(fiscalTargetOffice.isNull()&&designatedGuildHouseKey.empty()&&!guildBuilding&&!guildClientSystemActive);assert(fiscalLedger.organisations[0].targetHouseId.empty());assert(fiscalLedger.organisations[1].targetHouseId=="remote");for(int i=0;i<2;++i)assert(fiscalLedger.organisations[i].debt==9000&&fiscalLedger.organisations[i].deadline==123&&fiscalLedger.organisations[i].penalty==2);
 reload(serialize());assert(guildHouseNames.empty());assert(gbReason(&b).empty());
 reload("@house|old|Legacy Name|Squin\n@house|empty||Heft\n");assert(guildHouseNames["old"]=="Legacy Name");assert(guildHouseNames["empty"]=="Guild House of Heft");
 level=2;pendingGuildHouseKey=b.key;pendingGuildHouseHandle=&b;edit.value="No";confirmGuildHouseName(0);assert(!guildHouseNames.count(b.key));
 std::cout<<"PASS production handlers: creation, rename, abandonment, legacy/save round trips, furniture, client departures, fiscal destination and debt invariants\n";
}
