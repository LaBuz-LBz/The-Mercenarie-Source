#pragma once

// Active-contract overlays for Kenshi's main map. This is deliberately rebuilt
// from persisted quest state when MapScreen::update runs; no background polling.
namespace {
int selectedMissionMapSlot=-1;

MyGUI::Colour missionMapColour(int slot){
    static const MyGUI::Colour colours[5]={
        MyGUI::Colour(1.00f,0.72f,0.12f),MyGUI::Colour(0.20f,0.78f,1.00f),
        MyGUI::Colour(0.88f,0.30f,1.00f),MyGUI::Colour(0.18f,0.92f,0.58f),
        MyGUI::Colour(1.00f,0.42f,0.58f)};
    return colours[std::max(0,std::min(4,slot))];
}

void missionMapSelected(MyGUI::Widget* sender){
    if(!sender)return;const std::string name=sender->getName();
    if(name.empty()||name[name.size()-1]<'0'||name[name.size()-1]>'4')return;
    selectedMissionMapSlot=name[name.size()-1]-'0';selectEscortQuest(selectedMissionMapSlot);
}

MyGUI::Widget* missionMapChild(MyGUI::Widget* parent,const std::string& name,int w,int h){
    MyGUI::Widget* child=parent->findWidget(name);
    if(!child){child=parent->createWidget<MyGUI::Widget>("WhiteSkin",0,0,w,h,MyGUI::Align::Default,name);child->setNeedMouseFocus(false);}
    return child;
}

void missionMapCircle(MapScreen* map,MyGUI::Widget* root,const char* prefix,const Ogre::Vector3& world,const MyGUI::Colour& colour){
    const MyGUI::IntPoint centre=map->worldToMapCoords(world);const double pi=3.14159265358979323846;
    for(int i=0;i<24;++i){std::ostringstream n;n<<prefix<<i;MyGUI::Widget* dot=missionMapChild(root,n.str(),4,4);double a=i*pi*2.0/24.0;dot->setColour(colour);dot->setPosition(centre.left+(int)(cos(a)*12)-2,centre.top+(int)(sin(a)*12)-2);dot->setVisible(true);}
}

std::vector<RoutePrototype::Point> missionMapPath(const EscortQuestContext& q,bool selected){
    const EscortContractData& contract=selected?currentContract:q.v_currentContract;
    const Ogre::Vector3 target=selected?destination:q.v_destination;
    const Ogre::Vector3 start=selected?journeyStart:q.v_journeyStart;
    // Render the same road plan that drives this mission, including after
    // leader succession. Old saves need not contain a contract preview polyline.
    const MissionRescueState& rescue=selected?missionRescue:q.v_missionRescue;
    if(rescue.roadPlanned&&!rescue.roadFailed&&!rescue.roadPoints.empty()
        &&rescue.roadGoal.squaredDistance(target)<1.0f){
        std::vector<RoutePrototype::Point> actual;
        for(size_t i=0;i<rescue.roadPoints.size();++i){const Ogre::Vector3& p=rescue.roadPoints[i];actual.push_back(RoutePrototype::Point(p.x,p.y,p.z));}
        return actual;
    }
    std::vector<RoutePrototype::Point> path=ContractRouteVisual::decode(contract.routeRegions);
    if(path.empty())return path;
    const RoutePrototype::Point a(start.x,start.y,start.z),b(target.x,target.y,target.z);
    if(RoutePrototype::distance(path.front(),b)<RoutePrototype::distance(path.back(),b))std::reverse(path.begin(),path.end());
    if(RoutePrototype::distance(a,path.front())>10.0)path.insert(path.begin(),a);
    if(RoutePrototype::distance(path.back(),b)>10.0)path.push_back(b);
    return path;
}

void updateMissionMapOverlay(MapScreen* map,int slot){
    if(!map||!map->mapImage||slot<0||slot>=maximumActiveQuests)return;
    const bool selected=slot==selectedEscortQuest;const EscortQuestContext& q=escortQuests[slot];
    const bool active=selected?missionActive:q.v_missionActive;
    const int lifecycle=selected?contractLifecycle:q.v_contractLifecycle;
    std::ostringstream rootName;rootName<<"MercenarieMissionRoute"<<slot;
    MyGUI::Widget* root=map->mapImage->findWidget(rootName.str());
    const bool visible=active&&lifecycle==CONTRACT_ACTIVE;
    if(!visible){if(root)root->setVisible(false);return;}
    if(!root){root=map->mapImage->createWidget<MyGUI::Widget>("",0,0,map->mapImage->getWidth(),map->mapImage->getHeight(),MyGUI::Align::Stretch,rootName.str());root->setNeedMouseFocus(false);}
    root->setVisible(true);const MyGUI::Colour colour=missionMapColour(slot);
    const Ogre::Vector3 start=selected?journeyStart:q.v_journeyStart;
    const Ogre::Vector3 target=selected?destination:q.v_destination;
    const EscortContractData& contract=selected?currentContract:q.v_currentContract;
    const hand& leaderHandle=selected?escortHandle:q.v_escortHandle;
    Character* leader=leaderHandle.isNull()?0:leaderHandle.getCharacter();
    const Ogre::Vector3 leaderPosition=leader?leader->getPosition():(selected?lastJourneyPosition:q.v_lastJourneyPosition);

    // Accepted contract knowledge reveals only its endpoints, even if the town
    // has not otherwise been discovered on the world map.
    missionMapCircle(map,root,"MissionOriginDot",start,MyGUI::Colour(1.0f,0.12f,0.10f));
    const bool destinationKnown=!contract.destinationId.empty();
    if(destinationKnown)missionMapCircle(map,root,"MissionDestinationDot",target,MyGUI::Colour(0.12f,1.0f,0.20f));
    else for(int i=0;i<24;++i){std::ostringstream n;n<<"MissionDestinationDot"<<i;if(MyGUI::Widget* dot=root->findWidget(n.str()))dot->setVisible(false);}

    std::vector<RoutePrototype::Point> path=missionMapPath(q,selected);std::vector<MyGUI::IntPoint> pixels;
    if(destinationKnown&&!path.empty())for(size_t i=0;i<path.size();++i)pixels.push_back(map->worldToMapCoords(Ogre::Vector3((float)path[i].x,(float)path[i].y,(float)path[i].z)));
    int used=0;double carried=0;const double spacing=9.0;
    for(size_t i=1;i<pixels.size()&&used<180;++i){double dx=pixels[i].left-pixels[i-1].left,dy=pixels[i].top-pixels[i-1].top,len=sqrt(dx*dx+dy*dy);if(len<.1)continue;double at=carried;
        while(at<=len&&used<180){double t=at/len;int x=(int)(pixels[i-1].left+dx*t),y=(int)(pixels[i-1].top+dy*t);double ox=len>0?-dy/len:0,oy=len>0?dx/len:0;int spread=(slot-2)*2;
            std::ostringstream n;n<<"MissionRouteDot"<<used;MyGUI::Widget* dot=missionMapChild(root,n.str(),selectedMissionMapSlot==slot?7:5,selectedMissionMapSlot==slot?7:5);int dotSize=selectedMissionMapSlot==slot?7:5;dot->setSize(dotSize,dotSize);dot->setColour(colour);dot->setPosition(x+(int)(ox*spread)-dot->getWidth()/2,y+(int)(oy*spread)-dot->getHeight()/2);dot->setVisible(true);++used;at+=spacing;}
        carried=at-len;
    }
    for(int i=used;i<180;++i){std::ostringstream n;n<<"MissionRouteDot"<<i;if(MyGUI::Widget* dot=root->findWidget(n.str()))dot->setVisible(false);}

    // A compact flag follows the actual leader position, independently of the
    // planned line (detour, KO, cage, blocked movement, or manual displacement).
    const MyGUI::IntPoint lp=map->worldToMapCoords(leaderPosition);MyGUI::Widget* pole=missionMapChild(root,"MissionLeaderPole",3,24);pole->setColour(colour);pole->setPosition(lp.left+5+(slot-2)*3,lp.top-25);pole->setVisible(true);
    MyGUI::Widget* flag=missionMapChild(root,"MissionLeaderFlag",18,11);flag->setColour(colour);flag->setPosition(lp.left+8+(slot-2)*3,lp.top-25);flag->setVisible(true);
    std::ostringstream selectName;selectName<<"MissionRouteSelect"<<slot;MyGUI::Button* choose=static_cast<MyGUI::Button*>(root->findWidget(selectName.str()));
    if(!choose){choose=root->createWidget<MyGUI::Button>("Kenshi_Button1",8,8+slot*30,210,26,MyGUI::Align::Default,selectName.str());choose->eventMouseButtonClick+=MyGUI::newDelegate(missionMapSelected);}
    MercenarieFonts::caption(choose,std::string("#")+registerNumber(slot+1)+"  "+contract.origin+" > "+contract.destination);choose->setTextColour(colour);choose->setFontHeight(13);choose->setStateSelected(selectedMissionMapSlot==slot);choose->setVisible(true);
}

#include "MailMapOverlay.h"
void updateAllQuestMissionMaps(MapScreen* map){
    updateSelectedMailMap(map);
    bool any=false;for(int i=0;i<maximumActiveQuests;++i){const bool active=i==selectedEscortQuest?missionActive:escortQuests[i].v_missionActive;if(active)any=true;updateMissionMapOverlay(map,i);}
    if(!any)selectedMissionMapSlot=-1;
}
}
