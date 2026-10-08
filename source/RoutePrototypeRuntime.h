#include "Localization.h"
// Included inside the mission namespace, after issueTravelOrder.
MyGUI::Window* routeTestWindow=0;
MyGUI::TextBox* routeTestText=0;
MyGUI::ImageBox* routeTestMap=0;
std::vector<MyGUI::TextBox*> routeTestDots;
std::vector<RoutePrototype::Point> routeTestDraft;
std::string routeTestDraftMission;
hand routeRecorderActor;
bool routeRecording=false;
bool routeDraftFromRoad=false;
RoutePrototype::RoadPreset routeDraftPreset;
std::string routeTestDiagnostic;
// Road estimates are for pricing only. Never enable waypoint driving in missions.
bool routeTestEnabled(){return false;}
void refreshRouteTest(){
    if(!routeTestText)return;
    std::stringstream s;
    s<<(gMercenarieEnglish?Loc::text("ui.experimental_not_verified_in_game"):Loc::text("ui.experimental_not_verified_in_game"));
    s<<(gMercenarieEnglish?Loc::text("ui.draft_points"):Loc::text("ui.draft_points"))<<routeTestDraft.size()<<"\n";
    if(routeRecording)s<<(gMercenarieEnglish?Loc::text("ui.recording_selected_soldier_s_travel"):Loc::text("ui.recording_selected_soldier_s_travel"));
    if(routeDraftFromRoad&&routeTestDraftMission==currentMissionFiscalId)s<<(gMercenarieEnglish?Loc::text("ui.road_preset_the_hub_blister_hill"):Loc::text("ui.road_preset_the_hub_blister_hill"));
    if(testRoute.missionId==currentMissionFiscalId&&testRoute.status!=RoutePrototype::Idle){
        s<<(gMercenarieEnglish?Loc::text("ui.next_point"):Loc::text("ui.next_point"))<<std::min(testRoute.next+1,(unsigned int)testRoute.points.size())<<" / "<<testRoute.points.size()<<"\n";
        s<<(testRoute.status==RoutePrototype::Blocked?(gMercenarieEnglish?Loc::text("ui.blocked_review_then_retry"):Loc::text("ui.blocked_review_then_retry")):testRoute.status==RoutePrototype::Complete?(gMercenarieEnglish?Loc::text("ui.route_complete"):Loc::text("ui.route_complete")):(gMercenarieEnglish?Loc::text("ui.route_active"):Loc::text("ui.route_active")));
    }
    s<<"\n"<<routeTestDiagnostic;
    MercenarieFonts::caption(routeTestText,s.str());
    if(routeTestMap&&ou&&ou->zoneMgr){
        for(size_t i=0;i<routeTestDots.size();++i)routeTestDots[i]->setVisible(false);
        std::vector<RoutePrototype::Point> points;
        if(escort){Ogre::Vector3 p=escort->getPosition();points.push_back(RoutePrototype::Point(p.x,p.y,p.z));}
        if(testRoute.missionId==currentMissionFiscalId&&testRoute.status!=RoutePrototype::Idle){for(size_t i=testRoute.next;i<testRoute.points.size();++i)points.push_back(testRoute.points[i]);}
        else if(routeTestDraftMission==currentMissionFiscalId)points.insert(points.end(),routeTestDraft.begin(),routeTestDraft.end());
        // Same sector projection as existing contract maps. This is a schematic
        // polyline, NOT a native path and NOT metre-accurate screen positioning.
        size_t dot=0;
        for(size_t i=1;i<points.size()&&dot<routeTestDots.size();++i){
            iVector2 a=ou->zoneMgr->getMapSector(Ogre::Vector3((float)points[i-1].x,(float)points[i-1].y,(float)points[i-1].z));
            iVector2 b=ou->zoneMgr->getMapSector(Ogre::Vector3((float)points[i].x,(float)points[i].y,(float)points[i].z));
            int budget=(int)((routeTestDots.size()-dot)/(points.size()-i));
            int steps=std::max(1,std::min(budget,std::max(abs(a.x-b.x),abs(a.y-b.y))*2));
            for(int j=1;j<=steps&&dot<routeTestDots.size();++j){float t=(float)j/steps;int x=(int)((a.x+.5f+(b.x-a.x)*t)*routeTestMap->getWidth()/64.0f),y=(int)((a.y+.5f+(b.y-a.y)*t)*routeTestMap->getHeight()/64.0f);MyGUI::TextBox* d=routeTestDots[dot++];if(x<0||y<0||x>=routeTestMap->getWidth()-12||y>=routeTestMap->getHeight()-16)continue;d->setPosition(x,y);MercenarieFonts::caption(d,j==steps?"+":".");d->setVisible(true);}
        }
    }
}
bool routeTestEligible(){return missionActive&&!missionPending&&escort&&!escort->isDead()&&!caravanMission&&!scientificMission&&!currentMissionFiscalId.empty();}
RoutePrototype::Point routePoint(const Ogre::Vector3& p){return RoutePrototype::Point(p.x,p.y,p.z);}
bool roadDraftCompatible(const RoutePrototype::RoadPreset& preset){
    return routeTestEligible()&&contractOriginTown&&preset.matches(currentContract.originId,currentContract.destinationId,routePoint(contractOriginTown->getPosition()),routePoint(destination))&&RoutePrototype::distance(routePoint(escort->getPosition()),preset.origin)<=1500;
}
void routeTestLoadRoad(MyGUI::Widget*){
    if(!routeTestEligible())return;
    if(routeRecording||(testRoute.missionId==currentMissionFiscalId&&testRoute.status!=RoutePrototype::Idle)){
        ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.stop_recording_and_cancel_the_current_route_test_first"):Loc::text("ui.stop_recording_and_cancel_the_current_route_test_first"),true);return;
    }
    try{
        std::ifstream in("mods/Guild Escort Contracts/hub-blister.route");
        RoutePrototype::RoadPreset preset=RoutePrototype::RoadPreset::read(in);
        if(!roadDraftCompatible(preset)){
            ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.this_preset_requires_a_simple_the_hub_blister_hill"):Loc::text("ui.this_preset_requires_a_simple_the_hub_blister_hill"),true);return;
        }
        routeTestDraft=preset.points;routeTestDraftMission=currentMissionFiscalId;routeDraftPreset=preset;routeDraftFromRoad=true;
        refreshRouteTest();
        ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.road_route_prepared_58_8_km_on_roads_click"):Loc::text("ui.road_route_prepared_58_8_km_on_roads_click"),true);
    }catch(const std::exception&){ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.road_preset_missing_or_invalid_no_route_was_changed"):Loc::text("ui.road_preset_missing_or_invalid_no_route_was_changed"),true);}
}
void routeTestRecord(MyGUI::Widget*){
    if(routeRecording){routeRecording=false;routeRecorderActor.setNull();refreshRouteTest();return;}
    if(!routeTestEligible())return;
    Character* chosen=0;unsigned int count=0;
    for(size_t i=0;ou&&ou->player&&i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(c&&c->getFaction()==ou->player->getFaction()&&ou->player->isObjectSelected(c)){chosen=c;++count;}}
    if(count!=1||!chosen||chosen->isDead()){
        if(ou)ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.select_one_living_player_soldier"):Loc::text("ui.select_one_living_player_soldier"),true);return;
    }
    if(testRoute.missionId==currentMissionFiscalId&&testRoute.status!=RoutePrototype::Idle){ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.cancel_the_route_test_before_recording"):Loc::text("ui.cancel_the_route_test_before_recording"),true);return;}
    routeDraftFromRoad=false;routeTestDraft.clear();routeTestDraftMission=currentMissionFiscalId;routeRecorderActor=chosen->getHandle();routeRecording=true;
    Ogre::Vector3 p=chosen->getPosition();RoutePrototype::recordSample(routeTestDraft,RoutePrototype::Point(p.x,p.y,p.z));refreshRouteTest();
}
void routeTestUndo(MyGUI::Widget*){if(!routeRecording&&!routeTestDraft.empty()){routeDraftFromRoad=false;routeTestDraft.pop_back();refreshRouteTest();}}
void updateRouteRecorder(){
    if(!routeRecording)return;
    Character* c=routeRecorderActor.getCharacter();
    if(!routeTestEligible()||routeTestDraftMission!=currentMissionFiscalId||!c||c->isDead()||!ou||!ou->player||c->getFaction()!=ou->player->getFaction()){
        routeRecording=false;routeRecorderActor.setNull();if(ou)ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.route_recording_stopped_soldier_or_mission_unavailable"):Loc::text("ui.route_recording_stopped_soldier_or_mission_unavailable"),true);refreshRouteTest();return;
    }
    Ogre::Vector3 p=c->getPosition();RoutePrototype::SampleResult result=RoutePrototype::recordSample(routeTestDraft,RoutePrototype::Point(p.x,p.y,p.z));
    if(result==RoutePrototype::SampleGap||result==RoutePrototype::SampleFull){routeRecording=false;routeRecorderActor.setNull();ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.recording_stopped_position_gap_or_point_limit_review_the"):Loc::text("ui.recording_stopped_position_gap_or_point_limit_review_the"),true);}
    if(result!=RoutePrototype::SampleIgnored)refreshRouteTest();
}
void routeTestAdd(MyGUI::Widget*){
    if(!routeTestEligible()||routeRecording)return;
    Character* selected=0;unsigned int count=0;
    if(!ou||!ou->player)return;
    for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(c&&c->getFaction()==ou->player->getFaction()&&ou->player->isObjectSelected(c)){selected=c;++count;}}
    if(count!=1||!selected){ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.select_one_player_soldier"):Loc::text("ui.select_one_player_soldier"),true);return;}
    if(routeTestDraftMission!=currentMissionFiscalId){routeTestDraft.clear();routeTestDraftMission=currentMissionFiscalId;}
    if(routeTestDraft.size()>=511)return;
    routeDraftFromRoad=false;const Ogre::Vector3 p=selected->getPosition();routeTestDraft.push_back(RoutePrototype::Point(p.x,p.y,p.z));refreshRouteTest();
}
void routeTestStart(MyGUI::Widget*){
    if(routeRecording){ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.stop_recording_before_starting"):Loc::text("ui.stop_recording_before_starting"),true);return;}
    if(!routeTestEligible()||routeTestDraftMission!=currentMissionFiscalId||routeTestDraft.empty())return;
    if(routeDraftFromRoad&&!roadDraftCompatible(routeDraftPreset)){ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.the_client_or_towns_no_longer_match_the_road"):Loc::text("ui.the_client_or_towns_no_longer_match_the_road"),true);return;}
    std::vector<RoutePrototype::Point> points=routeTestDraft;points.push_back(RoutePrototype::Point(destination.x,destination.y,destination.z));
    testRoute.start(currentMissionFiscalId,points);
    // Existing wait/follow states are deliberately not cleared.
    // The next mission tick issues the order, after checking combat/health/wait.
    refreshRouteTest();
}
void routeTestRetry(MyGUI::Widget*){if(routeTestEligible()&&testRoute.missionId==currentMissionFiscalId){testRoute.retry();refreshRouteTest();}}
void routeTestCancel(MyGUI::Widget*){
    routeRecording=false;routeRecorderActor.setNull();
    bool own=routeTestEligible()&&testRoute.missionId==currentMissionFiscalId;
    testRoute.cancel();routeTestDraft.clear();routeTestDraftMission.clear();routeDraftFromRoad=false;
    if(own&&!missionPaused&&!missionFollowing&&!waitingForPlayer)issueTravelOrder(destination);
    refreshRouteTest();
}
void routeTestClose(MyGUI::Window* w,const std::string&){w->setVisible(false);}
void openRouteTest(MyGUI::Widget*){
    if(!routeTestEnabled())return;
    if(!routeTestEligible()){if(ou)ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.accept_a_simple_escort_first"):Loc::text("ui.accept_a_simple_escort_first"),true);return;}
    if(!routeTestWindow){
        MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();if(!gui)return;
        routeTestWindow=gui->createWidget<MyGUI::Window>("Kenshi_WindowCX",20,40,960,560,MyGUI::Align::Default,"Window","RoutePrototypeWindow");
        MercenarieFonts::caption(routeTestWindow,Loc::text("ui.v6_test_route"));routeTestWindow->eventWindowButtonPressed+=MyGUI::newDelegate(routeTestClose);
        MyGUI::Widget* c=routeTestWindow->getClientWidget();
        routeTestText=c->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",16,16,570,170,MyGUI::Align::Default);routeTestText->setFontHeight(17);
        routeTestMap=c->createWidget<MyGUI::ImageBox>("ImageBox",600,16,320,320,MyGUI::Align::Default);routeTestMap->setImageTexture("GuildEscortMap.png");routeTestMap->setNeedMouseFocus(false);
        for(int i=0;i<512;++i){MyGUI::TextBox* d=routeTestMap->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",0,0,12,18,MyGUI::Align::Default);d->setFontHeight(16);d->setTextColour(MyGUI::Colour(1.0f,.7f,.15f));d->setNeedMouseFocus(false);d->setVisible(false);routeTestDots.push_back(d);}
        MyGUI::TextBox* legend=c->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",16,344,880,70,MyGUI::Align::Default);legend->setFontHeight(16);MercenarieFonts::caption(legend,gMercenarieEnglish?Loc::text("ui.schematic_remaining_route_waypoints_dots_connect_them_not_a"):Loc::text("ui.schematic_remaining_route_waypoints_dots_connect_them_not_a"));
        MyGUI::Button* add=c->createWidget<MyGUI::Button>("Kenshi_Button1",16,190,570,38,MyGUI::Align::Default);MercenarieFonts::caption(add,gMercenarieEnglish?Loc::text("ui.add_selected_soldier_position"):Loc::text("ui.add_selected_soldier_position"));add->eventMouseButtonClick+=MyGUI::newDelegate(routeTestAdd);
        MyGUI::Button* start=c->createWidget<MyGUI::Button>("Kenshi_Button1",16,236,180,38,MyGUI::Align::Default);MercenarieFonts::caption(start,gMercenarieEnglish?Loc::text("ui.start"):Loc::text("ui.start"));start->eventMouseButtonClick+=MyGUI::newDelegate(routeTestStart);
        MyGUI::Button* retry=c->createWidget<MyGUI::Button>("Kenshi_Button1",210,236,180,38,MyGUI::Align::Default);MercenarieFonts::caption(retry,gMercenarieEnglish?Loc::text("ui.retry"):Loc::text("ui.retry"));retry->eventMouseButtonClick+=MyGUI::newDelegate(routeTestRetry);
        MyGUI::Button* cancel=c->createWidget<MyGUI::Button>("Kenshi_Button1",16,282,570,38,MyGUI::Align::Default);MercenarieFonts::caption(cancel,gMercenarieEnglish?Loc::text("ui.cancel_test_normal_travel"):Loc::text("ui.cancel_test_normal_travel"));cancel->eventMouseButtonClick+=MyGUI::newDelegate(routeTestCancel);
        MyGUI::Button* record=c->createWidget<MyGUI::Button>("Kenshi_Button1",16,425,570,38,MyGUI::Align::Default);MercenarieFonts::caption(record,gMercenarieEnglish?Loc::text("ui.record_travel_stop_new_draft"):Loc::text("ui.record_travel_stop_new_draft"));record->eventMouseButtonClick+=MyGUI::newDelegate(routeTestRecord);
        MyGUI::Button* undo=c->createWidget<MyGUI::Button>("Kenshi_Button1",600,425,320,38,MyGUI::Align::Default);MercenarieFonts::caption(undo,gMercenarieEnglish?Loc::text("ui.remove_last_point"):Loc::text("ui.remove_last_point"));undo->eventMouseButtonClick+=MyGUI::newDelegate(routeTestUndo);
        MyGUI::Button* road=c->createWidget<MyGUI::Button>("Kenshi_Button1",16,470,904,36,MyGUI::Align::Default);MercenarieFonts::caption(road,gMercenarieEnglish?Loc::text("ui.load_game_roads_the_hub_blister_hill"):Loc::text("ui.load_game_roads_the_hub_blister_hill"));road->eventMouseButtonClick+=MyGUI::newDelegate(routeTestLoadRoad);
    }
    refreshRouteTest();routeTestWindow->setVisible(true);
}
void clearRouteTestUI(){
    routeRecording=false;routeRecorderActor.setNull();routeDraftFromRoad=false;
    mercenarieDestroyLiveWidget(routeTestWindow);
    routeTestWindow=0;routeTestText=0;routeTestMap=0;routeTestDots.clear();routeTestDraft.clear();routeTestDraftMission.clear();testRoute.cancel();
}
bool tickRouteTest(float elapsed){
    if(!routeTestEnabled()){
        if(testRoute.status!=RoutePrototype::Idle){testRoute.cancel();}
        return false;
    }
    updateRouteRecorder();
    if(testRoute.status==RoutePrototype::Idle||testRoute.missionId!=currentMissionFiscalId)return false;
    if(!routeTestEligible()){testRoute.cancel();return false;}
    if(testRoute.status==RoutePrototype::Complete)return false;
    const Ogre::Vector3 p=escort->getPosition();
    bool suspended=missionPaused||missionFollowing||waitingForPlayer||leavingBuilding||escort->isInCombatMode(true,true)||escort->getMedical()->isUnconcious()||playerIsCarryingEscort();
    RoutePrototype::Action action=testRoute.tick(RoutePrototype::Point(p.x,p.y,p.z),elapsed,suspended,escort->getMovement()->pathFailed());
    if(action==RoutePrototype::MoveToPoint)issueTravelOrder(destination);
    std::stringstream detail;
    if(suspended)detail<<(gMercenarieEnglish?Loc::text("ui.suspended"):Loc::text("ui.suspended"))
        <<(missionPaused?"pause ":"")<<(missionFollowing?"follow ":"")<<(waitingForPlayer?"player ":"")<<(leavingBuilding?"exit ":"")
        <<(escort->isInCombatMode(true,true)?"combat ":"")<<(escort->getMedical()->isUnconcious()?"KO ":"")<<(playerIsCarryingEscort()?"carried ":"");
    else detail<<(gMercenarieEnglish?Loc::text("ui.no_progress"):Loc::text("ui.no_progress"))<<(int)testRoute.stalled<<" / 90 s";
    if(testRoute.next<testRoute.points.size()){
        const RoutePrototype::Point& next=testRoute.points[testRoute.next];
        detail<<"\nDistance XZ: "<<(int)RoutePrototype::distance(routePoint(p),next)<<" m | Delta Y: "<<(int)(p.y-next.y)<<Loc::text("ui.m");
        detail<<"\n"<<(gMercenarieEnglish?Loc::text("ui.moving"):Loc::text("ui.moving"))<<escort->getMovement()->isCurrentlyMoving()<<" | PathFailed: "<<escort->getMovement()->pathFailed();
        if(action==RoutePrototype::MoveToPoint||action==RoutePrototype::StopForReview){
            std::stringstream log;log<<"Route test action="<<(int)action<<" step="<<testRoute.next+1<<" actor="<<p.x<<","<<p.y<<","<<p.z<<" target="<<next.x<<","<<next.y<<","<<next.z<<" "<<detail.str();DebugLog(log.str());
        }
    }
    routeTestDiagnostic=detail.str();
    if(action==RoutePrototype::StopForReview){escort->removeJob(MOVE_CUS_ORDERED);escort->getMovement()->halt();ou->showPlayerAMessage(gMercenarieEnglish?Loc::text("ui.route_test_blocked_review_the_next_point_then_retry"):Loc::text("ui.route_test_blocked_review_the_next_point_then_retry"),true);}
    refreshRouteTest();return testRoute.status!=RoutePrototype::Complete;
}
