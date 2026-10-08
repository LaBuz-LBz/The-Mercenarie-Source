#pragma once
MyGUI::Colour apOrange(1,.61f,.06f),apWhite(.9f,.91f,.87f);
MyGUI::ScrollView *apList=0,*apInfo=0;
MyGUI::Widget* apInfoPanel=0;int apInfoX=0,apInfoBottom=0,apInfoTop=0,apInfoMaxHeight=0;bool apInfoCompact=false;
MyGUI::Button* apReselect=0;
MyGUI::Widget *apListBody=0,*apMapArea=0,*apMarkerLayer=0,*apInfoBody=0;
MyGUI::ImageBox* apMap=0;
MyGUI::Widget* apRouteLayer=0;std::vector<MyGUI::Widget*> apRouteDots;MyGUI::TextBox* apRouteCaption=0;MyGUI::TextBox* apSelectedLabel=0;
std::vector<RoutePrototype::Point> apRoutePoints;Ogre::Vector3 apRouteOrigin;std::string apRouteTarget;
void updateAutopilotRoute(bool fit);void drawAutopilotRoute();
MyGUI::EditBox* apSearch=0;
MyGUI::TextBox *apPlaceholder=0,*apCount=0,*apSoldier=0;
MyGUI::Button *apConfirm=0,*apStop=0;
std::vector<MyGUI::Button*> apMarkers;
std::string apSignature,apLanguage;int apWidth=0,apHeight=0;
float apClock=0;bool apDirty=false,apScrollSelection=false,apDragged=false;
int apDragX=0,apDragY=0,apStartX=0,apStartY=0;
void rebuildAutopilotContents();void redrawAutopilotMap();void createAutopilotView();
#include "AutopilotRoutePreview.h"
MyGUI::TextBox* apText(MyGUI::Widget* p,int x,int y,int w,int size,const std::string& text,bool orange=false){
 MyGUI::TextBox* t=launcherV9Text(p,x,y,w,size+6,size,"",orange?apOrange:apWhite,false);
 if(apWidth>=1200&&apHeight>=700){int bigger=size==14?18:size==16?20:size==18?24:size==24?36:size;std::ostringstream font;font<<"AutopilotBody"<<bigger;const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);if(!pack||pack->official)t->setFontName(font.str());t->setFontHeight(bigger);}
 launcherWrap(t,text);return t;
}
MyGUI::Button* apButton(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& text,void(*click)(MyGUI::Widget*),bool orange=false){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>("AutopilotV9Row",x,y,w,h,MyGUI::Align::Default);
 MyGUI::TextBox* t=apText(b,8,6,w-16,16,text,orange);
 if(t->getHeight()>h-6){t->setFontName("LauncherBody14");t->setFontHeight(14);launcherWrap(t,text);}
 t->setPosition(8,std::max(3,(h-t->getHeight())/2));t->setTextAlign(MyGUI::Align::Center);
 if(click)b->eventMouseButtonClick+=MyGUI::newDelegate(click);return b;
}
void apNativeIcon(MyGUI::Widget* p,const std::string& marker,int x,int y,int box){
 std::map<std::string,AutopilotSprite>::const_iterator i=autopilotSprites.find(marker);if(i==autopilotSprites.end())return;
 const AutopilotSprite& s=i->second;int w=box*s.rect.width/std::max(s.rect.width,s.rect.height),h=box*s.rect.height/std::max(s.rect.width,s.rect.height);
 MyGUI::ImageBox* icon=p->createWidget<MyGUI::ImageBox>("ImageBox",x+(box-w)/2,y+(box-h)/2,w,h,MyGUI::Align::Default);
 icon->setImageInfo(s.texture,s.rect,MyGUI::IntSize(s.rect.width,s.rect.height));icon->setImageIndex(0);icon->setNeedMouseFocus(false);
}
void resetAutopilotView(){
 apList=apInfo=0;apInfoPanel=0;apReselect=0;apListBody=apMapArea=apMarkerLayer=apInfoBody=0;apMap=0;apSearch=0;apPlaceholder=apCount=apSoldier=0;apConfirm=apStop=0;
 apRouteLayer=0;apRouteCaption=0;apSelectedLabel=0;apRouteDots.clear();apRoutePoints.clear();apRouteTarget.clear();
 apMarkers.clear();apSignature.clear();apLanguage.clear();apWidth=apHeight=0;apClock=0;apDirty=false;apScrollSelection=false;apDragged=false;autopilotModel=AutopilotDestinations::Model();autopilotSprites.clear();
}
void apGroupClick(MyGUI::Widget* w){std::string id=w->getUserString("faction");if(autopilotModel.expanded.count(id))autopilotModel.expanded.erase(id);else autopilotModel.expanded.insert(id);apDirty=true;}
void apSelect(MyGUI::Widget* w){
 if(w->getUserString("source")=="map"&&apDragged)return;
 if(!autopilotModel.select(w->getUserString("place")))return;
 const AutopilotDestinations::Place* p=autopilotModel.find(autopilotModel.selected);
 if(p)updateAutopilotRoute(true);
 apDirty=true;apScrollSelection=true;
}
void apSearchChanged(MyGUI::EditBox* w){autopilotModel.search(w->getCaption().asUTF8());apDirty=true;apScrollSelection=false;apPlaceholder->setVisible(autopilotModel.query.empty());apList->setViewOffset(MyGUI::IntPoint(0,0));}
void apMapPress(MyGUI::Widget*,int x,int y,MyGUI::MouseButton b){if(b!=MyGUI::MouseButton::Left)return;apDragged=false;apStartX=apDragX=x;apStartY=apDragY=y;}
void apMapDrag(MyGUI::Widget*,int x,int y,MyGUI::MouseButton b){if(b!=MyGUI::MouseButton::Left)return;if(abs(x-apStartX)+abs(y-apStartY)>5)apDragged=true;if(apDragged){autopilotView.pan(x-apDragX,y-apDragY);redrawAutopilotMap();}apDragX=x;apDragY=y;}
void apMapWheel(MyGUI::Widget*,int delta){if(!delta)return;MyGUI::IntPoint p=MyGUI::InputManager::getInstance().getMousePosition();autopilotView.zoom(delta>0?.8:1.25,p.left-apMapArea->getAbsoluteLeft(),p.top-apMapArea->getAbsoluteTop());redrawAutopilotMap();}
void apZoomIn(MyGUI::Widget*){autopilotView.zoom(.8,autopilotView.width/2,autopilotView.height/2);redrawAutopilotMap();}
void apZoomOut(MyGUI::Widget*){autopilotView.zoom(1.25,autopilotView.width/2,autopilotView.height/2);redrawAutopilotMap();}
void apResetMap(MyGUI::Widget*){autopilotView.reset();redrawAutopilotMap();}
void apBindMap(MyGUI::Widget* w){w->eventMouseWheel+=MyGUI::newDelegate(apMapWheel);w->eventMouseButtonPressed+=MyGUI::newDelegate(apMapPress);w->eventMouseDrag+=MyGUI::newDelegate(apMapDrag);}
void redrawAutopilotMap(){
 if(!apMap)return;autopilotView.clamp();int side=(int)(2048*autopilotView.scale());apMap->setCoord((int)autopilotView.px(0),(int)autopilotView.py(0),side,side);
 for(size_t i=0;i<apMarkers.size();++i){MyGUI::Button* b=apMarkers[i];const AutopilotDestinations::Place* p=autopilotModel.find(b->getUserString("place"));if(!p){b->setVisible(false);continue;}
  int x=(int)autopilotView.px(p->mapX),y=(int)autopilotView.py(p->mapY),size=b->getWidth();b->setPosition(x-size/2,y-size/2);b->setVisible(x>=0&&y>=0&&x<autopilotView.width&&y<autopilotView.height);
 }
 if(apSelectedLabel){const AutopilotDestinations::Place* p=autopilotModel.find(autopilotModel.selected);if(p){int x=(int)autopilotView.px(p->mapX),y=(int)autopilotView.py(p->mapY);apSelectedLabel->setPosition(std::max(8,std::min(autopilotView.width-apSelectedLabel->getWidth()-8,x+38)),std::max(8,std::min(autopilotView.height-apSelectedLabel->getHeight()-8,y-12)));apSelectedLabel->setVisible(x>=0&&y>=0&&x<autopilotView.width&&y<autopilotView.height);}else apSelectedLabel->setVisible(false);}
 drawAutopilotRoute();
}
std::string apType(const AutopilotDestinations::Place& p){
 const char* key="other_type";
 switch(p.type){case TOWN_TOWN:key="Town";break;case TOWN_VILLAGE:key="Village";break;case TOWN_OUTPOST:case TOWN_MILITARY:key="Outpost";break;case TOWN_RUINS:key="Ruin";break;case TOWN_SLAVE_CAMP:key="Workcamp";break;case TOWN_NEST:case TOWN_NEST_MARKER:key="Nest";break;case TOWN_POI:key="SmallPlace";break;}
 return Loc::text((std::string("autopilot.v9.")+key).c_str());
}
void rebuildAutopilotInfo(){
 if(!apInfo)return;if(apInfoBody)MyGUI::Gui::getInstance().destroyWidget(apInfoBody);
 int width=apInfo->getWidth()-22;apInfoBody=apInfo->createWidget<MyGUI::Widget>("PanelEmpty",0,0,width,300,MyGUI::Align::Default);int y=8;
 const AutopilotDestinations::Place* p=autopilotModel.find(autopilotModel.selected);
 if(!p)y+=apText(apInfoBody,10,y,width-20,16,Loc::text("autopilot.v9.choose"))->getHeight();
 else{
  apNativeIcon(apInfoBody,p->marker,8,y,38);MyGUI::TextBox* name=apText(apInfoBody,54,y,width-64,18,p->name,true);y+=std::max(38,name->getHeight())+6;
  y+=apText(apInfoBody,10,y,width-20,16,Loc::named("autopilot.v9.owner","value",p->faction))->getHeight()+4;
  y+=apText(apInfoBody,10,y,width-20,16,Loc::named("autopilot.v9.type","value",apType(*p)))->getHeight()+4;
  Character* actor=selectedAutopilotActor();if(actor&&actor->getHandle()==autopilotActor){Ogre::Vector3 pos((float)p->x,(float)p->y,(float)p->z);std::string distance=QuestTrackerText::distance(Ogre::Math::Sqrt(actor->getPosition().squaredDistance(pos)),gMercenarieEnglish);y+=apText(apInfoBody,10,y,width-20,16,Loc::named("autopilot.v9.distance","value",distance))->getHeight();}
 }
 int panelHeight=std::min(apInfoMaxHeight,std::max(60,y+24));
 apInfoPanel->setSize(apInfoPanel->getWidth(),panelHeight);apInfoPanel->setPosition(apInfoX,apInfoCompact?apInfoTop:apInfoBottom-panelHeight);
 apInfo->setSize(apInfo->getWidth(),panelHeight-8);
 apInfoBody->setSize(width,y+8);apInfo->setCanvasSize(width,std::max(y+8,apInfo->getHeight()-2));
}
void rebuildAutopilotContents(){
 if(!apList)return;MyGUI::IntPoint offset=apList->getViewOffset();if(apListBody)MyGUI::Gui::getInstance().destroyWidget(apListBody);
 int width=apList->getWidth()-20,y=0,selectedY=-1,count=0;apListBody=apList->createWidget<MyGUI::Widget>("PanelEmpty",0,0,width,1,MyGUI::Align::Default);
 std::vector<AutopilotDestinations::Group> groups=autopilotModel.groups();
 for(size_t i=0;i<groups.size();++i){const AutopilotDestinations::Group& g=groups[i];count+=(int)g.places.size();bool open=autopilotModel.open(g.id);
  MyGUI::Button* group=apButton(apListBody,0,y,width,40,"",apGroupClick);group->setUserString("faction",g.id);group->eventMouseWheel+=MyGUI::newDelegate(MercenarieNativeInput::wheel);
  apText(group,8,8,18,18,open?"-":"+",true);std::ostringstream label;label<<g.name<<" ("<<g.places.size()<<")";
  int gh=std::max(40,apText(group,30,8,width-40,18,label.str())->getHeight()+16);group->setSize(width,gh);y+=gh+4;
  if(open)for(size_t j=0;j<g.places.size();++j){const AutopilotDestinations::Place& p=autopilotModel.places[g.places[j]];bool selected=p.id==autopilotModel.selected;
   MyGUI::Button* row=apButton(apListBody,8,y,width-8,38,"",apSelect);row->setStateSelected(selected);row->setUserString("place",p.id);row->setUserString("source","list");row->eventMouseWheel+=MyGUI::newDelegate(MercenarieNativeInput::wheel);
   apNativeIcon(row,p.marker,8,4,30);int rh=std::max(38,apText(row,46,8,width-64,18,p.name,selected)->getHeight()+16);row->setSize(width-8,rh);if(selected)selectedY=y;y+=rh+3;
  }
 }
 if(!count)y=apText(apListBody,8,20,width-16,16,Loc::text("autopilot.v9.empty"),true)->getHeight()+40;
 apListBody->setSize(width,y);apList->setCanvasSize(width,std::max(y,apList->getHeight()-2));
 if(apScrollSelection&&selectedY>=0){int top=-offset.top;if(selectedY<top||selectedY+48>top+apList->getHeight())offset.top=-selectedY;}
 offset.top=std::max(-std::max(0,y-apList->getHeight()+4),std::min(0,offset.top));apList->setViewOffset(offset);apCount->setCaption(Loc::count("autopilot.v9.count",count));
 if(apMarkerLayer)MyGUI::Gui::getInstance().destroyWidget(apMarkerLayer);apSelectedLabel=0;apMarkers.clear();
 apMarkerLayer=apMapArea->createWidget<MyGUI::Widget>("PanelEmpty",0,0,autopilotView.width,autopilotView.height,MyGUI::Align::Default);apMarkerLayer->setNeedMouseFocus(false);apMarkerLayer->setInheritsPick(true);
 for(size_t i=0;i<autopilotModel.places.size();++i){const AutopilotDestinations::Place& p=autopilotModel.places[i];if(!AutopilotDestinations::matches(p.name,autopilotModel.query))continue;bool selected=p.id==autopilotModel.selected;int size=selected?64:50;
  MyGUI::Button* marker=apMarkerLayer->createWidget<MyGUI::Button>(selected?"AutopilotV9Row":"PanelEmpty",0,0,size,size,MyGUI::Align::Default);marker->setNeedMouseFocus(true);marker->setStateSelected(selected);marker->setUserString("place",p.id);marker->setUserString("source","map");apNativeIcon(marker,p.marker,3,3,size-6);apBindMap(marker);marker->eventMouseButtonClick+=MyGUI::newDelegate(apSelect);apMarkers.push_back(marker);
 }
 const AutopilotDestinations::Place* selectedPlace=autopilotModel.find(autopilotModel.selected);
 if(selectedPlace)apSelectedLabel=apText(apMarkerLayer,0,0,std::min(260,autopilotView.width-16),18,selectedPlace->name,true);
 redrawAutopilotMap();rebuildAutopilotInfo();apDirty=false;apScrollSelection=false;
}
std::string apSnapshotSignature(){std::ostringstream s;for(size_t i=0;i<autopilotModel.places.size();++i){const AutopilotDestinations::Place& p=autopilotModel.places[i];s<<p.id<<'\n'<<p.name<<'\n'<<p.factionId<<'\n'<<p.faction<<'\n'<<p.marker<<'\n'<<p.type<<','<<p.x<<','<<p.y<<','<<p.z<<','<<p.mapX<<','<<p.mapY<<'\n';}return s.str();}
void apReselectSoldier(MyGUI::Widget*){
 Character* actor=selectedAutopilotActor();if(!actor||actor->isDead()||actor->isBeingCarried()){if(ou)ou->showPlayerAMessage(Loc::text("ui.select_one_member_of_your_squad_first"),true);return;}
 autopilotActor=actor->getHandle();updateAutopilotRoute(true);rebuildAutopilotInfo();redrawAutopilotMap();
}
void createAutopilotView(){
 launcherEnsureUi();MyGUI::ResourceManager& resources=MyGUI::ResourceManager::getInstance();if(!resources.isExist("AutopilotV9Window"))resources.load("MercenarieAutopilot.xml");
 if(mercenarieAutopilotWindow)MyGUI::Gui::getInstance().destroyWidget(mercenarieAutopilotWindow);apListBody=apInfoBody=apMarkerLayer=0;apRouteLayer=0;apRouteCaption=0;apSelectedLabel=0;apRouteDots.clear();apMarkers.clear();
 MyGUI::IntSize screen=MyGUI::RenderManager::getInstance().getViewSize();apWidth=screen.width;apHeight=screen.height;apLanguage=Loc::engine().language;AutopilotDestinations::Layout l(screen.width,screen.height);
 mercenarieAutopilotWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("AutopilotV9Window",(screen.width-l.w)/2,(screen.height-l.h)/2,l.w,l.h,MyGUI::Align::Default,"Window","MercenarieAutopilot");
 mercenarieAutopilotWindow->eventWindowButtonPressed+=MyGUI::newDelegate(closeMercenarieInterface);mercenarieAutopilotWindow->setNeedMouseFocus(true);MyGUI::Widget* c=mercenarieAutopilotWindow->getClientWidget();c->setNeedMouseFocus(true);MyGUI::Widget* frame=c->createWidget<MyGUI::Widget>("LauncherV9Frame",0,0,l.w,l.h,MyGUI::Align::Default);frame->setNeedMouseFocus(false);
 const bool large=apWidth>=1200&&apHeight>=700;int headerLine=large?112:91;
 launcherLogo(c,20,16,large?80:62);apText(c,large?116:98,18,l.w-185,24,std::string(Loc::text("ui.the_mercenarie"))+" | "+Loc::text("ui.auto_pilot"),true);
 apText(c,large?116:98,large?68:53,l.w-185,large?16:14,Loc::text("autopilot.v9.subtitle"));apButton(c,l.w-56,18,38,38,"X",closeAutopilot,true);
 launcherV9Solid(c,14,headerLine,l.w-28,1,apOrange);apSoldier=apText(c,20,headerLine+8,l.w-280,14,"");
 apStop=apButton(c,l.w-252,headerLine+5,232,30,Loc::text("autopilot.v9.stop"),cancelAutopilot);
 apReselect=apButton(c,l.w-252,headerLine+5,232,30,Loc::text("autopilot.v9.reselect"),apReselectSoldier,true);
 MyGUI::TextBox* heading=apText(c,20,l.top,l.left-8,18,Loc::text("autopilot.v9.factions"),true);int searchY=l.top+heading->getHeight()+9;
 c->createWidget<MyGUI::Widget>("AutopilotV9Row",20,searchY,l.left-8,36,MyGUI::Align::Default)->setNeedMouseFocus(false);
 apSearch=c->createWidget<MyGUI::EditBox>("AutopilotV9Search",20,searchY,l.left-8,36,MyGUI::Align::Default);apSearch->setCaption(autopilotModel.query);apSearch->setEditMultiLine(false);MercenarieFonts::caption(apSearch,autopilotModel.query);const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);if(!pack||pack->official)apSearch->setFontName("LauncherBody16");apSearch->setFontHeight(16);
 apSearch->eventEditTextChange+=MyGUI::newDelegate(apSearchChanged);apPlaceholder=apText(c,28,searchY+8,l.left-24,14,Loc::text("autopilot.v9.search"));apPlaceholder->setVisible(autopilotModel.query.empty());
 apList=c->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",20,searchY+44,l.left-8,l.h-searchY-92,MyGUI::Align::Default);MercenarieNativeInput::bind(apList);apList->setVisibleHScroll(false);apList->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);
 apCount=apText(c,20,l.h-38,l.left-8,14,"");launcherV9Solid(c,l.rightX-10,l.top,1,l.h-l.top-18,MyGUI::Colour(.25f,.27f,.25f));
 apMapArea=c->createWidget<MyGUI::Widget>("PanelEmpty",l.rightX,l.top,l.rightW,l.mapH,MyGUI::Align::Default);apMapArea->setNeedMouseFocus(true);apBindMap(apMapArea);
 apMap=apMapArea->createWidget<MyGUI::ImageBox>("ImageBox",0,0,1,1,MyGUI::Align::Default);apMap->setImageTexture("GuildEscortMap.png");apBindMap(apMap);
 autopilotView.width=l.rightW;autopilotView.height=l.mapH;autopilotView.clamp();
 apRouteLayer=apMapArea->createWidget<MyGUI::Widget>("PanelEmpty",0,0,l.rightW,l.mapH,MyGUI::Align::Default);apRouteLayer->setNeedMouseFocus(false);
 // Controls are siblings above the marker layer, so refreshes cannot cover them.
 apButton(c,l.rightX+l.rightW-42,l.top+8,34,34,"+",apZoomIn,true);apButton(c,l.rightX+l.rightW-42,l.top+46,34,34,"-",apZoomOut,true);
 apButton(c,l.rightX+8,l.top+8,large?190:142,large?40:32,Loc::text("autopilot.v9.reset"),apResetMap);
 int iw=l.compact?l.rightW:std::min(380,l.rightW-64),ix=l.compact?l.rightX:l.rightX+l.rightW-iw-10,iy=l.compact?l.top+l.mapH+8:l.top+l.mapH-l.infoH-10;
 MyGUI::Widget* panel=c->createWidget<MyGUI::Widget>("AutopilotV9Row",ix,iy,iw,l.infoH,MyGUI::Align::Default);
 apInfoPanel=panel;apInfoX=ix;apInfoTop=iy;apInfoBottom=iy+l.infoH;apInfoMaxHeight=l.infoH;apInfoCompact=l.compact;
 apInfo=panel->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",4,4,iw-8,l.infoH-8,MyGUI::Align::Default);MercenarieNativeInput::bind(apInfo);apInfo->setVisibleHScroll(false);apInfo->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);
 int bw=(l.rightW-12)/2;apConfirm=apButton(c,l.rightX,l.footerY,bw,44,Loc::text("ui.confirm"),launchAutopilot,true);apButton(c,l.rightX+bw+12,l.footerY,bw,44,Loc::text("ui.cancel"),closeAutopilot);
 apRouteCaption=apText(c,l.rightX+8,l.top+l.mapH-28,std::max(120,l.rightW-iw-28),14,"");if(l.compact)apRouteCaption->setVisible(false);
 apDirty=true;rebuildAutopilotContents();
}
void updateAutopilotUI(float elapsed){
 if(!mercenarieAutopilotWindow||!mercenarieAutopilotWindow->getVisible())return;
 MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();if(v.width!=apWidth||v.height!=apHeight||Loc::engine().language!=apLanguage)createAutopilotView();
 apClock-=std::max(0.0f,elapsed);if(apClock<=0){apClock=2;autopilotModel.replace(scanAutopilotPlaces());std::string signature=apSnapshotSignature();if(signature!=apSignature){apSignature=signature;apDirty=true;apScrollSelection=true;}else rebuildAutopilotInfo();updateAutopilotRoute(false);redrawAutopilotMap();}
 if(apDirty)rebuildAutopilotContents();Character* actor=selectedAutopilotActor();bool valid=actor&&actor->getHandle()==autopilotActor&&!actor->isDead()&&!actor->isBeingCarried();
 bool canConfirm=valid&&autopilotModel.find(autopilotModel.selected)!=0;apConfirm->setEnabled(canConfirm);
 apConfirm->getChildAt(0)->castType<MyGUI::TextBox>()->setTextColour(canConfirm?apOrange:MyGUI::Colour(.42f,.44f,.42f));
 apReselect->setVisible(!valid);
 apStop->setVisible(valid&&autopilotTrips.count(actor->getHandle().toString())!=0);
 apSoldier->setCaption(valid?Loc::named("autopilot.v9.soldier","name",actor->getName()):Loc::text("autopilot.v9.reopen"));
}
void buildPlayerAutopilot(){
 Character* actor=selectedAutopilotActor();if(!actor){if(ou)ou->showPlayerAMessage(Loc::text("ui.select_one_member_of_your_squad_first"),true);return;}
 if(!MyGUI::Gui::getInstancePtr()||!shou||!shou->townList||!ou->zoneMgr)return;
 autopilotActor=actor->getHandle();autopilotModel=AutopilotDestinations::Model();autopilotView=AutopilotDestinations::View();loadAutopilotSprites();autopilotModel.replace(scanAutopilotPlaces());
 if(!autopilotModel.places.empty())autopilotModel.expanded.insert(autopilotModel.places[0].factionId);apSignature=apSnapshotSignature();apClock=2;apDragged=false;createAutopilotView();updateAutopilotUI(0);
}
