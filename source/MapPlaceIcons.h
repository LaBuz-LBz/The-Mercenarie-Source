// Shared visual policy. Does not alter projection, discovery or movement orders.
MyGUI::IntCoord mercenariePlaceSprite(TownBase* town){
    // Native Kenshi_MapMarkers definitions from data/gui/images/kenshi_images.xml.
    if(!town)return MyGUI::IntCoord(1405,27,32,32);
    std::string name=town->getGameData()?town->getGameData()->name:"";
    const std::string id=town->getGameData()?town->getGameData()->stringID:"";
    std::transform(name.begin(),name.end(),name.begin(),::tolower);
    // Stable vanilla record: names can already be translated when this runs.
    if(id=="48446-rebirth.mod"||town->townType==TOWN_RUINS||name.find("ruin")!=std::string::npos||name.find("workshop")!=std::string::npos||name.find("atelier")!=std::string::npos||name.find("ancient lab")!=std::string::npos||name.find("laboratoire")!=std::string::npos||name.find("armoury")!=std::string::npos)return MyGUI::IntCoord(1245,23,22,34);
    if(town->townType==TOWN_SLAVE_CAMP||name.find("mine")!=std::string::npos)return MyGUI::IntCoord(1003,25,32,32);
    if(name.find("farm")!=std::string::npos)return MyGUI::IntCoord(1083,25,32,32);
    if(name.find("tower")!=std::string::npos)return MyGUI::IntCoord(950,6,48,108);
    if(town->townType==TOWN_TOWN)return MyGUI::IntCoord(835,9,105,79);
    if(town->townType==TOWN_VILLAGE)return MyGUI::IntCoord(1083,25,32,32);
    if(town->townType==TOWN_OUTPOST||town->townType==TOWN_MILITARY||town->townType==TOWN_PRISON)return MyGUI::IntCoord(950,6,48,108);
    if(town->townType==TOWN_NEST||town->townType==TOWN_NEST_MARKER)return MyGUI::IntCoord(1202,25,32,32);
    // Unknown nests/POIs are not automatically towns or slave camps.
    return MyGUI::IntCoord(1405,27,32,32);
}
void mercenariePlaceImage(MyGUI::ImageBox* image,TownBase* town){
    const MyGUI::IntCoord sprite=mercenariePlaceSprite(town);
    // Reset tile dimensions too: the previous town tile can exceed a ruin crop.
    image->setImageInfo("Kenshi_UI.png",sprite,MyGUI::IntSize(sprite.width,sprite.height));
    image->setImageIndex(0);
}
