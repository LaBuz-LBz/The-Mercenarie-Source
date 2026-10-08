#pragma once
// Native widgets aligned to the approved 1536 x 1024 mockup, cropped at (100,52).
float personnelPickerScale=1.0f;
std::vector<size_t> personnelSelectionOrder;
std::vector<MyGUI::ImageBox*> personnelChecks;
std::vector<MyGUI::TextBox*> personnelRoles;
MyGUI::TextBox* personnelCount=0;
MyGUI::Button *personnelConfirmButton=0,*personnelAllButton=0;
MyGUI::ComboBox* personnelFormationChoice=0;
MyGUI::ScrollView* personnelDiagram=0;
std::vector<MyGUI::Widget*> personnelDiagramItems;
std::string personnelFormationId="classic";
int ppPx(int v){return static_cast<int>(v*personnelPickerScale+.5f);}
MyGUI::IntCoord ppRect(int x,int y,int w,int h){return MyGUI::IntCoord(ppPx(x),ppPx(y),ppPx(w),ppPx(h));}
void ppFont(MyGUI::TextBox* t,int size,bool sans=false){float old=negotiationScale;negotiationScale=personnelPickerScale;negotiationFont(t,size,sans);negotiationScale=old;t->setUserString("NegotiationFontHeight",registerNumber(t->getFontHeight()));}
MyGUI::TextBox* ppText(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text,const MyGUI::Colour& color,bool sans=false,MyGUI::Align align=MyGUI::Align::Left|MyGUI::Align::Top){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Negotiation74Text",ppRect(x,y,w,h),MyGUI::Align::Default);ppFont(t,size,sans);t->setTextAlign(align);t->setTextColour(color);t->setNeedMouseFocus(false);negotiationCaption(t,sans?negotiationTracked(text):text);return t;
}
MyGUI::Button* ppButton(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text,const MyGUI::Colour& color,const char* skin="Negotiation74Text"){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>(skin,ppRect(x,y,w,h),MyGUI::Align::Default);ppFont(b,size);b->setTextAlign(MyGUI::Align::Center);b->setTextColour(color);negotiationCaption(b,text);return b;
}
MyGUI::ImageBox* ppImage(MyGUI::Widget* p,int x,int y,int w,int h,int sx,int sy,int sw,int sh){MyGUI::ImageBox* im=p->createWidget<MyGUI::ImageBox>("ImageBox",ppRect(x,y,w,h),MyGUI::Align::Default);im->setImageTexture("MercenariePersonnelReference75.png");im->setImageCoord(MyGUI::IntCoord(sx,sy,sw,sh));im->setNeedMouseFocus(false);return im;}
void ppNode(int x,int y,int kind,const std::string& caption){
 int sx=1090,sy=421,sw=39,sh=39;
 if(kind==1){sx=1140;sy=480;sw=39;sh=34;}else if(kind==2){sx=1043;sy=537;sw=36;sh=34;}else if(kind==3){sx=1136;sy=555;sw=45;sh=44;}
 personnelDiagramItems.push_back(ppImage(personnelDiagram,x-sw/2,y,sw,sh,sx,sy,sw,sh));
 personnelDiagramItems.push_back(ppText(personnelDiagram,x-75,y+sh+3,150,27,17,caption,kind==0?negotiationGold:negotiationInk,false,MyGUI::Align::HCenter|MyGUI::Align::Top));
}
void personnelRefreshDiagram(){
 if(!personnelDiagram)return;
 for(size_t i=0;i<personnelDiagramItems.size();++i)mercenarieDestroyLiveWidget(personnelDiagramItems[i]);personnelDiagramItems.clear();
 int animals=0,guards=0;std::vector<Character*> seen;const std::vector<Character*>& members=scientificMission?scientificMembers:caravanMembers;
 for(size_t i=0;i<members.size();++i){Character* c=members[i];if(!c||c==escort||c->isDead()||std::find(seen.begin(),seen.end(),c)!=seen.end())continue;seen.push_back(c);if(c->isAnimal())++animals;else ++guards;}
 const int cx=228;
 ppNode(cx,140,3,Loc::text("staff.design.leader"));
 for(int i=0;i<std::min(4,guards);++i)ppNode(cx+(i%2?100:-100),i<2?122:195,2,Loc::text("staff.design.guard"));
 if(animals>1)ppNode(cx,66,1,Loc::text("staff.design.animal_front"));
 if(animals>0)ppNode(cx,232,1,Loc::text("staff.design.animal_rear"));
 size_t count=personnelSelectionOrder.size();
 for(size_t slot=0;slot<count;++slot){size_t index=personnelSelectionOrder[slot];if(index>=personnelCandidates.size())continue;Character* c=personnelCandidates[index].getCharacter();
 EscortPersonnel::Position pos=EscortPersonnel::formationPosition(personnelFormationId,slot,count,animals>0);
 int x=cx+static_cast<int>(pos.side*(slot<2?2.8f:2.8f));int y=slot<2?6:290+static_cast<int>((slot-2)/3)*82;
 ppNode(x,y,0,c?c->getName():"?");}
 int rows=count>2?static_cast<int>((count-2+2)/3):0;
 personnelDiagram->setCanvasSize(ppPx(456),ppPx(std::max(360,290+rows*82)));
}
void personnelRefreshPicker(){
 for(size_t i=0;i<personnelRows.size();++i){bool selected=personnelChosen[i];personnelRows[i]->setStateSelected(selected);personnelChecks[i]->setImageCoord(selected?MyGUI::IntCoord(158,347,37,38):MyGUI::IntCoord(158,633,37,38));
 std::vector<size_t>::iterator it=std::find(personnelSelectionOrder.begin(),personnelSelectionOrder.end(),i);std::string role;
 if(it!=personnelSelectionOrder.end()){size_t slot=it-personnelSelectionOrder.begin();role=std::string(Loc::text(slot<2?"staff.design.front":"staff.design.rear"))+" \xC2\xB7 "+registerNumber(static_cast<int>(slot<2?slot+1:slot-1));}
 negotiationCaption(personnelRoles[i],role);}
 negotiationCaption(personnelCount,registerNumber(static_cast<int>(personnelSelectionOrder.size()))+" "+Loc::text("staff.design.selected")+" "+registerNumber(static_cast<int>(personnelCandidates.size())));
 personnelConfirmButton->setEnabled(!personnelSelectionOrder.empty());
 negotiationCaption(personnelAllButton,Loc::text(!personnelCandidates.empty()&&personnelSelectionOrder.size()==personnelCandidates.size()?"staff.design.none":"staff.design.all"));
 personnelRefreshDiagram();
}
void personnelSelectAll(MyGUI::Widget*){bool on=personnelSelectionOrder.size()!=personnelCandidates.size();for(size_t i=0;i<personnelCandidates.size();++i){personnelChosen[i]=on;EscortPersonnel::choose(personnelSelectionOrder,i,on);}personnelRefreshPicker();}
void personnelFormationChanged(MyGUI::ComboBox*,size_t index){size_t count=0;const EscortPersonnel::Formation* list=EscortPersonnel::formations(count);if(index<count)personnelFormationId=list[index].id;personnelRefreshDiagram();}
void personnelBuildPicker(){
 launcherEnsureUi();MyGUI::ResourceManager& resources=MyGUI::ResourceManager::getInstance();
 if(!resources.isExist("NegotiationV9Window"))resources.load("MercenarieNegotiation.xml");if(!resources.isExist("Negotiation74Text"))resources.load("MercenarieNegotiation74.xml");if(!resources.isExist("Personnel75Row"))resources.load("MercenariePersonnel75.xml");
 personnelChecks.clear();personnelRoles.clear();personnelSelectionOrder.clear();personnelDiagramItems.clear();personnelFormationId="classic";
 const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();personnelPickerScale=std::min(1.0f,std::min((view.width-24)/1336.0f,(view.height-24)/920.0f));int w=ppPx(1336),h=ppPx(920);
 personnelPicker=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("NegotiationV9Window",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","MercenariePersonnelPicker");personnelPicker->eventWindowButtonPressed+=MyGUI::newDelegate(personnelPickerWindowClose);
 MyGUI::Widget* parent=personnelPicker->getClientWidget();
 MyGUI::ImageBox* plate=parent->createWidget<MyGUI::ImageBox>("ImageBox",0,0,w,h,MyGUI::Align::Default);plate->setImageTexture("MercenariePersonnelPlate75.png");plate->setImageCoord(MyGUI::IntCoord(100,52,1336,920));plate->setNeedMouseFocus(false);
 ppText(parent,113,42,239,35,19,"THE MERCENARIE",negotiationInk,true);
 ppText(parent,403,20,814,61,42,Loc::text("staff.title"),negotiationInk);
 ppText(parent,405,72,815,31,19,caravanMission?Loc::text("negotiation.design.caravan"):missionLabelV6(currentContract.type),negotiationGold,true);
 ppButton(parent,1260,24,44,44,23,"X",negotiationGold)->eventMouseButtonClick+=MyGUI::newDelegate(personnelPickerCancel);
 if(!caravanMission){MyGUI::ImageBox* cover=parent->createWidget<MyGUI::ImageBox>("ImageBox",ppRect(35,126,80,68),MyGUI::Align::Default);cover->setImageTexture("MercenariePersonnelPlate75.png");cover->setImageCoord(MyGUI::IntCoord(700,178,80,68));cover->setNeedMouseFocus(false);MyGUI::ImageBox* icon=parent->createWidget<MyGUI::ImageBox>("ImageBox",ppRect(52,134,56,56),MyGUI::Align::Default);setMissionIconV6(icon,currentContract.type);icon->setNeedMouseFocus(false);}
 ppText(parent,147,126,1115,31,23,std::string(Loc::text("negotiation.design.client"))+" "+(escort?escort->getName():""),negotiationInk);
 std::string route=originCity+" \xE2\x86\x92 "+destinationName;if(caravanMission||scientificMission)route+=" \xE2\x86\x92 "+std::string(Loc::text("negotiation.design.return"));ppText(parent,147,159,1116,30,22,route,negotiationInk);
 ppText(parent,42,222,504,30,21,Loc::text("staff.design.available"),negotiationInk,true);ppText(parent,42,250,495,27,18,Loc::text("staff.design.town"),negotiationMuted);
 personnelAllButton=ppButton(parent,594,221,186,32,18,Loc::text("staff.design.all"),negotiationGold);personnelAllButton->eventMouseButtonClick+=MyGUI::newDelegate(personnelSelectAll);
 MyGUI::ScrollView* scroll=parent->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",ppRect(37,282,766,422),MyGUI::Align::Default);scroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);scroll->setVisibleHScroll(false);MercenarieNativeInput::bind(scroll);
 if(ou&&ou->player)for(unsigned int i=0;i<ou->player->playerCharacters.size()&&personnelCandidates.size()<256;++i){Character* c=ou->player->playerCharacters[i];if(!personnelSameTown(c)||personnelAssignment(c)>=0)continue;
 int row=static_cast<int>(personnelCandidates.size());MyGUI::Button* button=ppButton(scroll,0,row*72,748,60,23,"",negotiationInk,"Personnel75Row");button->eventMouseButtonClick+=MyGUI::newDelegate(personnelRowClicked);
 personnelChecks.push_back(ppImage(button,21,13,37,38,158,633,37,38));
 MyGUI::ImageBox* portrait=button->createWidget<MyGUI::ImageBox>("ImageBox",ppRect(78,4,52,52),MyGUI::Align::Default);portrait->setNeedMouseFocus(false);if(PortraitManager::getInstance())PortraitManager::getInstance()->setImageWidget(c->getHandle(),portrait,true);
 ppText(button,157,13,464,39,24,c->getName(),negotiationInk);
 personnelRoles.push_back(ppText(button,620,18,113,32,18,"",negotiationGold,false,MyGUI::Align::Right|MyGUI::Align::Top));personnelCandidates.push_back(c->getHandle());personnelRows.push_back(button);personnelChosen.push_back(false);}
 scroll->setCanvasSize(ppPx(748),ppPx(std::max(422,static_cast<int>(personnelRows.size())*72-12)));
 if(personnelRows.empty())ppText(scroll,12,14,711,90,20,Loc::text("staff.none"),negotiationMuted);
 personnelCount=ppText(parent,40,716,739,31,19,"",negotiationInk);
 ppText(parent,831,216,453,32,22,Loc::text("staff.design.formation"),negotiationInk,true);
 personnelFormationChoice=parent->createWidget<MyGUI::ComboBox>("Personnel75Formation",ppRect(836,248,459,46),MyGUI::Align::Default);personnelFormationChoice->setComboModeDrop(true);ppFont(personnelFormationChoice,22);personnelFormationChoice->setTextColour(negotiationInk);personnelFormationChoice->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
 size_t count=0;const EscortPersonnel::Formation* list=EscortPersonnel::formations(count);for(size_t i=0;i<count;++i)personnelFormationChoice->addItem(Loc::text(list[i].label));personnelFormationChoice->setIndexSelected(0);personnelFormationChoice->eventComboChangePosition+=MyGUI::newDelegate(personnelFormationChanged);
 ppText(parent,831,304,465,28,16,Loc::text("staff.design.hint"),negotiationMuted);
 ppText(parent,831,341,465,27,16,Loc::text("staff.design.direction"),negotiationInk,false,MyGUI::Align::HCenter|MyGUI::Align::Top);
 personnelDiagram=parent->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",ppRect(834,363,466,361),MyGUI::Align::Default);personnelDiagram->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);personnelDiagram->setVisibleHScroll(false);MercenarieNativeInput::bind(personnelDiagram);
 ppText(parent,831,726,465,25,17,Loc::text("staff.design.legend"),negotiationGold,false,MyGUI::Align::HCenter|MyGUI::Align::Top);
 ppText(parent,95,782,1202,36,19,Loc::text("staff.design.help"),negotiationInk);
 ppButton(parent,37,836,611,60,24,Loc::text("staff.design.back"),negotiationInk)->eventMouseButtonClick+=MyGUI::newDelegate(personnelPickerCancel);
 personnelConfirmButton=ppButton(parent,687,836,611,60,24,Loc::text("staff.design.confirm"),MyGUI::Colour(.08f,.07f,.04f));personnelConfirmButton->eventMouseButtonClick+=MyGUI::newDelegate(personnelPickerConfirm);
 personnelRefreshPicker();
}
