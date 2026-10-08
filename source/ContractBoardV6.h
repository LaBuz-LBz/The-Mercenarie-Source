#include "Localization.h"
#include "ContractRerollView.h"
// Presentation only. Included after the board model and common GUI helpers.
MyGUI::TextBox* offerNameV6[6]={0};
MyGUI::TextBox* offerTypeV6[6]={0};
MyGUI::TextBox* offerRarityV6[6]={0};
MyGUI::TextBox* offerPriceV6[6]={0};
MyGUI::ImageBox* offerIconV6[6]={0};
MyGUI::Button* offerRerollV6[6]={0};
MyGUI::Widget* offerEdgesV6[6][4]={{0}};
MyGUI::TextBox* boardXpV6=0;
MyGUI::ImageBox* boardMissionIconV6=0;
MyGUI::ImageBox* boardDangerV6[5]={0};
// Shared registry for every mission surface; append future types and atlas slots here.
struct MissionVisualV6 { int slot; unsigned char r,g,b; const char* fr; const char* en; };
const MissionVisualV6& missionVisualV6(int type){
    static MissionVisualV6 visuals[]={
        {0,55,210,54,Loc::text("ui.escort"),Loc::text("ui.escort")},
        {1,205,146,65,Loc::text("ui.caravan"),Loc::text("ui.caravan")},
        {2,57,205,235,Loc::text("ui.scientific_expedition"),Loc::text("ui.scientific_expedition")},
        {3,206,76,83,Loc::text("ui.bounty_hunt"),Loc::text("ui.bounty_hunt")},
        {4,210,183,98,Loc::text("ui.message_delivery"),Loc::text("ui.message_delivery")},
        {5,160,126,191,Loc::text("ui.long_distance"),Loc::text("ui.long_distance")}
    };
    static MissionVisualV6 unknown={6,153,163,166,Loc::text("common.mission"),Loc::text("common.mission")};
    const char* keys[]={"ui.escort","ui.caravan","ui.scientific_expedition","ui.bounty_hunt","ui.message_delivery","ui.long_distance"};
    for(int i=0;i<6;++i)visuals[i].fr=visuals[i].en=Loc::text(keys[i]);
    unknown.fr=unknown.en=Loc::text("common.mission");
    return type>=0&&type<static_cast<int>(sizeof(visuals)/sizeof(visuals[0]))?visuals[type]:unknown;
}
MyGUI::Colour missionColourV6(int type){
    const MissionVisualV6& v=missionVisualV6(type);return MyGUI::Colour(v.r/255.0f,v.g/255.0f,v.b/255.0f);
}
std::string missionLabelV6(int type){
    const MissionVisualV6& v=missionVisualV6(type);return registerLanguage(v.fr,v.en);
}
void setMissionIconV6(MyGUI::ImageBox* icon,int type){
    icon->setImageTexture("ContractIconsV6.png");
    icon->setImageCoord(MyGUI::IntCoord(missionVisualV6(type).slot*96,0,96,96));
    icon->setColour(missionColourV6(type));
}
MyGUI::ImageBox* boardIconV6(MyGUI::Widget* p,int id,int x,int y,int size,const MyGUI::Colour& colour){
    MyGUI::ImageBox* icon=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);
    icon->setImageTexture("ContractIconsV6.png");icon->setImageCoord(MyGUI::IntCoord(id*96,0,96,96));icon->setColour(colour);icon->setNeedMouseFocus(false);return icon;
}
MyGUI::Widget* boardPanelV6(MyGUI::Widget* p,int x,int y,int w,int h){
    MyGUI::Widget* panel=p->createWidget<MyGUI::Widget>("WhiteSkin",x,y,w,h,MyGUI::Align::Default);panel->setColour(MyGUI::Colour(.075f,.085f,.09f));
    const MyGUI::Colour edge(.46f,.49f,.50f);
    registerSolid(panel,0,0,w,1,edge);registerSolid(panel,0,h-1,w,1,edge);registerSolid(panel,0,0,1,h,edge);registerSolid(panel,w-1,0,1,h,edge);return panel;
}
void refreshOfferRowsV6(){
    guildRerolls.refresh(currentGameHours);
    for(int i=0;i<6;++i){if(!offerNameV6[i])continue;const BoardOffer& o=boardOffers[i];bool available=o.available;
        MercenarieFonts::caption(contractButtons[i],"");contractButtons[i]->setEnabled(true);contractButtons[i]->setNeedMouseFocus(available);
        refreshContractRerollButton(offerRerollV6[i],available&&!missionBookDelegationContext,ContractReroll::marked(o.routeRegions));
        contractButtons[i]->setNeedToolTip(available);contractButtons[i]->setUserString("rerolled",ContractReroll::marked(o.routeRegions)?"1":"0");
        for(int e=0;e<4;++e)offerEdgesV6[i][e]->setVisible(available&&i==selectedOffer);
        setMissionIconV6(offerIconV6[i],available?o.missionType:-1);
        MercenarieFonts::caption(offerNameV6[i],available?mercenarieLocalize(o.townName):registerLanguage(Loc::text("ui.new_mission_in"),Loc::text("ui.new_mission_in")));
        offerTypeV6[i]->setTextColour(available?missionColourV6(o.missionType):MyGUI::Colour(.85f,.88f,.90f));
        if(available)MercenarieFonts::caption(offerTypeV6[i],missionLabelV6(o.missionType));else{double deadline=0;std::map<std::string,CityContractBoard>::const_iterator b=savedContractBoards.find(currentBoardKey);if(b!=savedContractBoards.end())deadline=b->second.expiresAt;int minutes=std::max(0,(int)ceil((deadline-currentGameHours)*60));std::ostringstream t;t<<minutes/1440<<registerLanguage(Loc::text("ui.d"),Loc::text("ui.d"))<<(minutes/60)%24<<Loc::text("ui.h_d346c2c")<<minutes%60<<Loc::text("ui.min");MercenarieFonts::caption(offerTypeV6[i],t.str());}
        offerRarityV6[i]->setVisible(available);offerPriceV6[i]->setVisible(available);
        MercenarieFonts::caption(offerRarityV6[i],o.rarity>=2?registerLanguage(Loc::text("ui.epic"),Loc::text("ui.epic")):o.rarity==1?registerLanguage(Loc::text("ui.rare"),Loc::text("ui.rare")):registerLanguage(Loc::text("ui.common_a840b15"),Loc::text("ui.common_a840b15")));
        offerRarityV6[i]->setTextColour(o.rarity>=2?missionColourV6(5):o.rarity==1?MyGUI::Colour(.5f,.65f,1):MyGUI::Colour(.72f,.75f,.77f));
        MercenarieFonts::caption(offerPriceV6[i],registerNumber(o.estimatedPay)+Loc::text("ui.cats_3f70b99"));
        fitRegisterText(offerNameV6[i],23);fitRegisterText(offerTypeV6[i],20);fitRegisterText(offerPriceV6[i],23);fitRegisterText(offerRarityV6[i],17);
    }
}
void contractsWindowButtonPressed(MyGUI::Window*,const std::string&);
void closeBoardV6(MyGUI::WidgetPtr){contractsWindowButtonPressed(contractsWindow,"");}
