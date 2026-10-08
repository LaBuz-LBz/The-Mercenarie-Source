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
