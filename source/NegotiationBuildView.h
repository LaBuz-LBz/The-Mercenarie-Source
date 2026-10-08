#pragma once
void createNegotiationUI()
{
 if(!MyGUI::Gui::getInstancePtr())return;
 size_t position=negotiationWindow&&priceSlider?priceSlider->getScrollPosition():20;
 if(negotiationWindow)mercenarieDestroyLiveWidget(negotiationWindow);
 negotiationWindow=0;negotiationContent=0;launcherEnsureUi();
 MyGUI::ResourceManager& resources=MyGUI::ResourceManager::getInstance();
 if(!resources.isExist("NegotiationV9Button"))resources.load("MercenarieNegotiation.xml");
 if(!resources.isExist("Negotiation74Text"))resources.load("MercenarieNegotiation74.xml");
 const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
 negotiationScale=std::min(1.0f,std::min((view.width-24)/1496.0f,(view.height-24)/956.0f));
 int width=negotiationPx(1496),height=negotiationPx(956);
 negotiationWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("NegotiationV9Window",(view.width-width)/2,(view.height-height)/2,width,height,MyGUI::Align::Default,"Window","GuildEscortNegotiation");
 negotiationWindow->eventWindowButtonPressed+=MyGUI::newDelegate(negotiationWindowButtonPressed);
 MyGUI::Widget* c=negotiationWindow->getClientWidget();c->setNeedMouseFocus(true);
 MyGUI::ImageBox* plate=c->createWidget<MyGUI::ImageBox>("ImageBox",0,0,width,height,MyGUI::Align::Default);plate->setImageTexture("MercenarieNegotiationPlate74.png");plate->setImageCoord(MyGUI::IntCoord(20,34,1496,956));plate->setNeedMouseFocus(false);negotiationFrame=plate;
 negotiationLabel(c,150,80,280,34,22,"THE MERCENARIE",negotiationInk,true);
 negotiationLabel(c,482,58,908,62,48,Loc::text("negotiation.design.title"),negotiationInk);
 negotiationLabel(c,484,118,908,30,20,caravanMission?Loc::text("negotiation.design.caravan"):missionLabelV6(currentContract.type),negotiationGold,true);
 MyGUI::Button* close=negotiationAction(c,1442,62,47,48,24,"X",negotiationGold);close->eventMouseButtonClick+=MyGUI::newDelegate(negotiationCloseV9);
 negotiationLabel(c,180,188,580,34,24,std::string(Loc::text("negotiation.design.client"))+" "+escort->getName(),negotiationInk);
 std::string route=originCity+" \xE2\x86\x92 "+destinationName;if(caravanMission||scientificMission)route+=" \xE2\x86\x92 "+std::string(Loc::text("negotiation.design.return"));
 offerText=negotiationLabel(c,180,225,579,40,22,route,negotiationInk);
 // The approved plate depicts a caravan. Other escort types keep their own mission icon.
 if(!caravanMission){MyGUI::ImageBox* cover=c->createWidget<MyGUI::ImageBox>("ImageBox",negotiationRect(65,184,85,78),MyGUI::Align::Default);cover->setImageTexture("MercenarieNegotiationPlate74.png");cover->setImageCoord(MyGUI::IntCoord(630,184,85,78));cover->setNeedMouseFocus(false);MyGUI::ImageBox* icon=c->createWidget<MyGUI::ImageBox>("ImageBox",negotiationRect(77,193,61,62),MyGUI::Align::Default);setMissionIconV6(icon,currentContract.type);icon->setNeedMouseFocus(false);}
 negotiationLabel(c,877,203,166,40,24,negotiationDecimal(currentContract.distanceKm)+" km",negotiationInk);
 const char* risk=currentContract.danger<1.5f?"negotiation.design.danger_low":currentContract.danger<2.5f?"negotiation.design.danger_medium":"negotiation.design.danger_high";
 negotiationLabel(c,1185,203,291,42,23,std::string(Loc::text(risk))+" \xC2\xB7 "+negotiationDecimal(currentContract.danger)+" / 3",negotiationInk);
 negotiationSliderTitle=negotiationLabel(c,62,307,884,32,22,Loc::text("negotiation.design.proposal"),negotiationInk,true);
 proposalText=negotiationLabel(c,58,350,323,65,54,registerNumber(baseReward)+" Cats",negotiationInk);
 launcherV9Solid(c,negotiationPx(377-20),negotiationPx(353-34),std::max(1,negotiationPx(1)),negotiationPx(48),MyGUI::Colour(.40f,.36f,.27f));
 negotiationLabel(c,398,370,560,34,22,Loc::text("negotiation.design.offer"),negotiationMuted);
 sliderMinusButton=negotiationAction(c,62,417,68,64,36,"",negotiationInk);sliderPlusButton=negotiationAction(c,896,417,68,64,36,"",negotiationInk);
 negotiationSliderPanel=c;negotiationRatePanel=c;negotiationConditions=c;
 negotiationSliderTrack=negotiationAction(c,163,434,702,28,18,"",negotiationInk);negotiationSliderTrack->setNeedMouseFocus(false);
 negotiationFill=launcherV9Solid(c,negotiationPx(171-20),negotiationPx(443-34),1,std::max(1,negotiationPx(8)),negotiationGold);
 priceSlider=c->createWidget<MyGUI::ScrollBar>("Negotiation74Slider",negotiationRect(162,430,704,36),MyGUI::Align::Default,"NegotiationSlider");priceSlider->setTrackSize(negotiationPx(28));priceSlider->setMoveToClick(true);priceSlider->setScrollRange(46);priceSlider->setScrollPage(1);priceSlider->setScrollPosition(position);priceSlider->eventScrollChangePosition+=MyGUI::newDelegate(negotiationSliderChanged);
 sliderMinusButton->eventMouseButtonClick+=MyGUI::newDelegate(negotiationMinusClicked);sliderPlusButton->eventMouseButtonClick+=MyGUI::newDelegate(negotiationPlusClicked);
 MyGUI::Colour green(.62f,.78f,.36f);
 negotiationLabel(c,62,489,340,30,23,"- 20 %",green);negotiationLabel(c,62,515,425,32,22,Loc::text("negotiation.design.low"),green);
 sliderPercentText=negotiationLabel(c,453,474,120,31,22,"0 %",negotiationInk,false,MyGUI::Align::HCenter|MyGUI::Align::Top);
 negotiationLabel(c,620,489,340,30,23,"+ 25 %",negotiationGold,false,MyGUI::Align::Right|MyGUI::Align::Top);
 negotiationLabel(c,527,515,434,32,22,Loc::text("negotiation.design.high"),negotiationGold,false,MyGUI::Align::Right|MyGUI::Align::Top);
 negotiationLabel(c,68,548,889,31,18,Loc::text("negotiation.design.budget"),negotiationMuted,false,MyGUI::Align::HCenter|MyGUI::Align::Top);
 negotiationLabel(c,62,598,896,34,22,Loc::text("negotiation.design.advance"),negotiationInk,true);
 const int choices[]={0,10,25,50},xs[]={69,295,520,745};
 for(int i=0;i<4;++i){MyGUI::Button* b=negotiationAction(c,xs[i],639,211,57,24,registerNumber(choices[i])+" %",negotiationInk,"Negotiation74Choice");b->eventMouseButtonClick+=MyGUI::newDelegate(negotiationAdvanceChosen);negotiationAdvanceChoices[i]=b;}
 negotiationSummary=c;
 negotiationLabel(c,1022,307,453,32,22,Loc::text("negotiation.design.summary"),negotiationInk,true);
 negotiationLabel(c,1025,387,223,42,24,Loc::text("negotiation.design.total"),negotiationInk);
 negotiationTotalValue=negotiationLabel(c,1233,381,246,46,28,"0 Cats",negotiationInk,false,MyGUI::Align::Right|MyGUI::Align::Top);
 negotiationAdvanceLabel=negotiationLabel(c,1025,449,253,39,24,"",negotiationInk);
 negotiationAdvanceValue=negotiationLabel(c,1268,445,211,43,28,"0 Cats",negotiationInk,false,MyGUI::Align::Right|MyGUI::Align::Top);
 negotiationLabel(c,1025,561,215,40,24,Loc::text("negotiation.design.balance"),negotiationInk);
 negotiationBalanceValue=negotiationLabel(c,1233,551,246,61,40,"0 Cats",negotiationGold,false,MyGUI::Align::Right|MyGUI::Align::Top);
 negotiationRateValue=negotiationLabel(c,1025,658,450,34,21,"",negotiationMuted);paymentBreakdownText=negotiationRateValue;
 for(int i=0;i<6;++i){bonusButtons[i]=negotiationAction(c,20,34,1,1,8,"",negotiationInk);bonusButtons[i]->setVisible(false);}
 negotiationStaff=c;addPersonnelDesign(c,width);
 // Counteroffer overlay is hidden during ordinary negotiation and does not move the approved layout.
 negotiationCounterPanel=c->createWidget<MyGUI::Widget>("LauncherV9Card",negotiationRect(62,416,902,165),MyGUI::Align::Default);
 reactionText=negotiationCounterPanel->createWidget<MyGUI::TextBox>("Negotiation74Text",negotiationPx(12),negotiationPx(8),negotiationPx(878),negotiationPx(98),MyGUI::Align::Default);negotiationFont(reactionText,20);reactionText->setTextColour(negotiationInk);reactionText->setNeedMouseFocus(false);
 counterButton=negotiationCounterPanel->createWidget<MyGUI::Button>("Negotiation74Choice",negotiationPx(12),negotiationPx(112),negotiationPx(878),negotiationPx(44),MyGUI::Align::Default);negotiationFont(counterButton,22);counterButton->setTextAlign(MyGUI::Align::Center);counterButton->setTextColour(negotiationGold);counterButton->setCaption(Loc::text("ui.accept_the_counteroffer"));counterButton->setUserString("NegotiationFontHeight",registerNumber(counterButton->getFontHeight()));negotiationCounterPanel->setVisible(false);
 refuseButton=negotiationAction(c,50,889,418,68,24,Loc::text("ui.refuse_the_contract"),MyGUI::Colour(.85f,.36f,.27f));
 returnButton=negotiationAction(c,506,889,454,68,24,Loc::text("ui.back_to_contracts"),negotiationInk);
 proposeButton=negotiationAction(c,998,889,490,68,27,Loc::text("negotiation.design.validate"),MyGUI::Colour(.08f,.07f,.04f));
 proposeButton->eventMouseButtonClick+=MyGUI::newDelegate(proposePriceClicked);counterButton->eventMouseButtonClick+=MyGUI::newDelegate(acceptCounterClicked);returnButton->eventMouseButtonClick+=MyGUI::newDelegate(returnToContractsClicked);refuseButton->eventMouseButtonClick+=MyGUI::newDelegate(refuseNegotiationClicked);
 negotiationWindow->setVisible(false);
}
