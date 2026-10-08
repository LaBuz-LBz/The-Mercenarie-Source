#pragma once
// Fixed-price contract screen. All financial and mission callbacks remain shared.
float basicContractScale=1.0f;
MyGUI::ImageBox* basicContractTick=0;
void addBasicPersonnelDesign(MyGUI::Widget* parent);
int bcPx(int n){return static_cast<int>(n*basicContractScale+.5f);}
MyGUI::IntCoord bcRect(int x,int y,int w,int h){return MyGUI::IntCoord(bcPx(x-34),bcPx(y-70),bcPx(w),bcPx(h));}
void bcFont(MyGUI::TextBox* t,int size,bool sans){
 int pixels=std::max(8,std::min(96,(bcPx(size)+1)/2*2));
 if(pixels>56&&!sans&&(Loc::engine().language=="fr"||Loc::engine().language=="en")){
 std::string name="Basic76Serif"+registerNumber(pixels);if(!MyGUI::ResourceManager::getInstance().isExist(name))MyGUI::ResourceManager::getInstance().load(name+".xml");t->setFontName(name);t->setFontHeight(pixels);
 }else{float old=negotiationScale;negotiationScale=basicContractScale;negotiationFont(t,size,sans);negotiationScale=old;}
 t->setUserString("NegotiationFontHeight",registerNumber(t->getFontHeight()));
}
MyGUI::TextBox* bcText(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& caption,const MyGUI::Colour& color,bool sans=false,bool wrap=false){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Negotiation74Text",bcRect(x,y,w,h),MyGUI::Align::Default);bcFont(t,size,sans);t->setTextColour(color);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setNeedMouseFocus(false);
 std::string text=mercenarieLocalize(sans?negotiationTracked(caption):caption);
 if(wrap){int sizeNow=t->getFontHeight();for(;;){t->setCaption(RerollPopupLayout::wrap(text,std::max(8,t->getWidth()-8),LauncherMeasure(t)));if(t->getTextSize().height<=t->getHeight()||sizeNow<=8)break;t->setFontHeight(--sizeNow);}}
 else negotiationCaption(t,text);return t;
}
MyGUI::Button* bcButton(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& caption,const MyGUI::Colour& color){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>("Negotiation74Text",bcRect(x,y,w,h),MyGUI::Align::Default);bcFont(b,size,false);b->setTextAlign(MyGUI::Align::Center);b->setTextColour(color);negotiationCaption(b,caption);return b;
}
void basicContractClose(MyGUI::Widget*){suspendNegotiation();}
void createBasicContractDecisionUI(){
 if(!MyGUI::Gui::getInstancePtr())return;
 if(contractDecisionWindow)mercenarieDestroyLiveWidget(contractDecisionWindow);contractDecisionWindow=0;basicContractTick=0;
 launcherEnsureUi();MyGUI::ResourceManager& r=MyGUI::ResourceManager::getInstance();if(!r.isExist("NegotiationV9Window"))r.load("MercenarieNegotiation.xml");if(!r.isExist("Negotiation74Text"))r.load("MercenarieNegotiation74.xml");
 const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();basicContractScale=std::min(1.0f,std::min((view.width-24)/1470.0f,(view.height-24)/862.0f));int w=bcPx(1470),h=bcPx(862);
 contractDecisionWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("NegotiationV9Window",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","MercenarieContractDecision");contractDecisionWindow->eventWindowButtonPressed+=MyGUI::newDelegate(basicContractWindowPressed);
 MyGUI::Widget* c=contractDecisionWindow->getClientWidget();
 MyGUI::ImageBox* plate=c->createWidget<MyGUI::ImageBox>("ImageBox",0,0,w,h,MyGUI::Align::Default);plate->setImageTexture("MercenarieBasicPlate76.png");plate->setImageCoord(MyGUI::IntCoord(34,70,1470,862));plate->setNeedMouseFocus(false);
 bcText(c,167,120,298,38,22,"THE MERCENARIE",negotiationInk,true);
 bcText(c,521,94,872,64,48,Loc::text("basic.design.title"),negotiationInk);
 bcText(c,525,157,866,34,21,caravanMission?Loc::text("negotiation.design.caravan"):missionLabelV6(currentContract.type),negotiationGold,true);
 bcButton(c,1417,100,54,54,26,"X",negotiationGold)->eventMouseButtonClick+=MyGUI::newDelegate(basicContractClose);
 if(!caravanMission){MyGUI::ImageBox* cover=c->createWidget<MyGUI::ImageBox>("ImageBox",bcRect(78,222,80,80),MyGUI::Align::Default);cover->setImageTexture("MercenarieBasicPlate76.png");cover->setImageCoord(MyGUI::IntCoord(640,220,80,80));cover->setNeedMouseFocus(false);MyGUI::ImageBox* icon=c->createWidget<MyGUI::ImageBox>("ImageBox",bcRect(88,232,64,64),MyGUI::Align::Default);setMissionIconV6(icon,currentContract.type);icon->setNeedMouseFocus(false);}
 bcText(c,195,231,574,32,23,Loc::text("basic.design.route"),negotiationInk,true);
 std::string route=originCity+" \xE2\x86\x92 "+destinationName;if(caravanMission||scientificMission)route+=" \xE2\x86\x92 "+std::string(Loc::text("negotiation.design.return"));
 bcText(c,195,269,579,37,23,route,negotiationInk);
 bcText(c,899,245,167,42,27,negotiationDecimal(currentContract.distanceKm)+" km",negotiationInk);
 bcText(c,1216,244,247,43,26,std::string(Loc::text("basic.design.danger"))+" \xC2\xB7 "+negotiationDecimal(currentContract.danger)+" / 3",negotiationInk);
 bcText(c,81,349,730,38,25,Loc::text("basic.design.offer"),negotiationInk,true);
 contractDecisionText=bcText(c,77,397,744,105,88,registerNumber(baseReward)+" Cats",negotiationInk);
 bcText(c,85,503,730,40,27,Loc::text("basic.design.reward"),negotiationInk);
 bcText(c,85,586,730,40,22,Loc::text("basic.design.payment"),negotiationMuted);
 bcText(c,883,347,570,42,25,Loc::text("basic.design.fixed"),negotiationInk,true);
 bcText(c,982,433,451,82,25,Loc::text("basic.design.locked"),negotiationInk,false,true);
 bcText(c,982,521,451,91,25,Loc::text("basic.design.choice"),negotiationInk,false,true);
 addBasicPersonnelDesign(c);
 bcText(c,185,687,987,44,29,Loc::text("staff.assign"),negotiationInk);
 bcText(c,185,733,987,35,24,Loc::text("basic.design.staff_hint"),negotiationInk);
 contractDecisionRefuse=bcButton(c,68,804,686,90,33,Loc::text("ui.refuse_the_contract"),MyGUI::Colour(.85f,.36f,.27f));contractDecisionRefuse->eventMouseButtonClick+=MyGUI::newDelegate(basicContractRefused);
 contractDecisionAccept=bcButton(c,784,804,686,90,33,Loc::text("basic.design.accept"),MyGUI::Colour(.08f,.07f,.04f));contractDecisionAccept->eventMouseButtonClick+=MyGUI::newDelegate(basicContractAccepted);
 contractDecisionWindow->setVisible(false);
}
