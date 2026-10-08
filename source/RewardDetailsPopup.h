// Dynamic reward sheet. All displayed amounts come from the selected offer.
MyGUI::Widget* rewardPopup87=0;
void closeRewardPopup87(MyGUI::Widget*){if(rewardPopup87){MyGUI::Gui::getInstance().destroyWidget(rewardPopup87);rewardPopup87=0;}}
std::string reward87Text(const char* fr,const char* en){return Loc::engine().language=="fr"?fr:en;}
MyGUI::TextBox* reward87Label(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& value,bool gold=false,bool right=false){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Board77Text",0,0,1,1,MyGUI::Align::Default);
 t->setUserString("board77Skin","Board77Text");t->setNeedMouseFocus(false);
 board77Text(t,x,y,w,h,size,gold?board77Gold:board77Ink,false,right?MyGUI::Align::Right|MyGUI::Align::VCenter:MyGUI::Align::Left|MyGUI::Align::VCenter);
 t->setCaption(value);board77Fit(t);return t;
}
void reward87Rule(MyGUI::Widget* p,int x,int y,int w,int h=1){registerSolid(p,board77Px(x),board77Px(y),board77Px(w),std::max(1,board77Px(h)),board77Edge);}
void reward87Row(MyGUI::Widget* p,int x,int y,int width,const std::string& label,const std::string& value){reward87Label(p,x,y,width*66/100,36,21,label);reward87Label(p,x+width*66/100,y,width*34/100,36,22,value,true,true);}
void openRewardPopup87(){
 if(!contractsWindow||selectedOffer<0||selectedOffer>=6)return;
 closeRewardPopup87(0);
 const BoardOffer& o=boardOffers[selectedOffer];
 float savedScale=board77Scale;
 board77Scale=std::min(contractsWindow->getWidth()/1536.0f,contractsWindow->getHeight()/928.0f);
 rewardPopup87=contractsWindow->createWidget<MyGUI::Widget>("PanelEmpty",0,0,contractsWindow->getWidth(),contractsWindow->getHeight(),MyGUI::Align::Stretch);
 // Isolate the sheet in the popup render layer: Child widgets share draw batches
 // with the board and can let its text render above this opaque frame.
 // Keep parent ownership for automatic visibility and teardown with the board.
 rewardPopup87->setWidgetStyle(MyGUI::WidgetStyle::Popup,"Popup");
 // Popup coordinates are screen-relative, even while owned by the board.
 rewardPopup87->setCoord(contractsWindow->getAbsoluteLeft(),contractsWindow->getAbsoluteTop(),contractsWindow->getWidth(),contractsWindow->getHeight());
 rewardPopup87->setInheritsAlpha(false);rewardPopup87->setAlpha(1.0f);
 rewardPopup87->setNeedMouseFocus(true);
 MyGUI::Widget* p=rewardPopup87->createWidget<MyGUI::Widget>("Board77Frame",0,0,1,1,MyGUI::Align::Default);p->setCoord((contractsWindow->getWidth()-board77Px(930))/2,(contractsWindow->getHeight()-board77Px(760))/2,board77Px(930),board77Px(760));
 reward87Label(p,42,24,806,62,40,reward87Text("D\303\251tails des r\303\251compenses","Reward details"))->setTextAlign(MyGUI::Align::Center);
 reward87Label(p,40,90,850,36,22,missionLabelV6(o.missionType),true)->setTextAlign(MyGUI::Align::Center);
 reward87Label(p,40,139,850,40,25,mercenarieLocalize(originCity)+" > "+mercenarieLocalize(o.townName))->setTextAlign(MyGUI::Align::Center);
 const float km=o.distance/1000.0f*(o.missionType==MCT_CARAVAN||o.missionType==MCT_SCIENCE?2:1);
 std::ostringstream summary;summary<<(int)km<<" km"<<(o.missionType==MCT_CARAVAN||o.missionType==MCT_SCIENCE?reward87Text(" aller-retour"," round trip"):"")<<"  \302\267  "<<reward87Text("Danger ","Danger ")<<o.dangerLevel<<"/5";
 reward87Label(p,40,184,850,34,20,summary.str())->setTextAlign(MyGUI::Align::Center);
 reward87Rule(p,22,234,886);reward87Rule(p,535,234,1,384);
 reward87Label(p,42,246,474,38,22,reward87Text("R\303\211COMPENSE EN CATS","CATS REWARD"));reward87Label(p,562,246,330,38,22,reward87Text("XP DE GUILDE","GUILD XP"));
 int q[8];bool detailed=FrozenPriceTerms::read(o.routeRegions,q);int y=292;
 if(detailed){
  reward87Row(p,42,y,474,reward87Text("Tarif de base","Base reward"),registerNumber(q[0]));y+=32;
  reward87Row(p,42,y,474,(o.routeRegions.find(";PRICE85;")!=std::string::npos?reward87Text("Distance \302\267 ","Distance \302\267 ")+registerNumber((int)km)+" \303\227 "+registerNumber(o.missionType==MCT_MAIL?75:o.missionType==MCT_ESCORT?130:90):reward87Text("Distance","Distance")),"+"+registerNumber(q[1]));y+=32;
  if(q[2]||q[3]){reward87Row(p,42,y,474,reward87Text("Suppl\303\251ments de mission","Mission supplements"),"+"+registerNumber(q[2]+q[3]));y+=32;}
  reward87Row(p,42,y,474,reward87Text("Sous-total","Subtotal"),registerNumber(q[0]+q[1]+q[2]+q[3]));y+=32;
  reward87Row(p,42,y,474,(o.routeRegions.find(";PRICE85;")!=std::string::npos?reward87Text("Danger \302\267 +","Danger \302\267 +")+registerNumber(o.dangerLevel==1?0:o.dangerLevel==2?15:o.dangerLevel==3?35:o.dangerLevel==4?60:100)+" %":reward87Text("Majoration de danger","Danger premium")),"+"+registerNumber(q[4]));y+=32;
  if(q[5]||q[6]){reward87Row(p,42,y,474,reward87Text("Raret\303\251 / maison de guilde","Rarity / guild office"),"+"+registerNumber(q[5]+q[6]));y+=32;}
  if(o.estimatedPay!=q[7]){reward87Row(p,42,y,474,reward87Text("Offre apr\303\250s ajustement","Adjusted offer"),registerNumber(o.estimatedPay));y+=32;}
 }else{reward87Row(p,42,y,474,reward87Text("Offre enregistr\303\251e","Saved offer"),registerNumber(o.estimatedPay));y+=40;}
 reward87Row(p,42,y,474,reward87Text("R\303\251glage des gains","Reward setting"),registerNumber(contractRewardPercent)+" %");
 reward87Rule(p,42,548,474);
 reward87Label(p,42,557,240,44,29,reward87Text("Total estim\303\251","Estimated total"),true);reward87Label(p,277,557,239,44,31,registerNumber(displayedContractCats(o.estimatedPay))+" Cats",true,true);
 reward87Label(p,42,597,474,30,16,reward87Text("Avant taxes \303\251ventuelles","Before applicable taxes"));
 ContractFactors::Quote cargo;bool intact=o.missionType==MCT_CARAVAN&&ContractFactors::decode(o.routeRegions,cargo)&&cargo.count>0;
 GuildProgression::Result xp=GuildProgression::success(o.missionType,km,o.dangerLevel,0,true,intact);
 const char* multipliers[]={"\303\2271","\303\2271,10","\303\2271,25","\303\2271,50","\303\2271,75"};
 reward87Row(p,562,298,326,reward87Text("Base de mission","Mission base"),registerNumber(xp.base)+" XP");
 reward87Row(p,562,344,326,reward87Text("Distance","Distance"),"+"+registerNumber(xp.distance)+" XP");
 reward87Row(p,562,390,326,reward87Text("Danger","Danger"),multipliers[std::max(1,std::min(5,o.dangerLevel))-1]);
 reward87Row(p,562,436,326,o.missionType==MCT_MAIL?reward87Text("Livraison r\303\251ussie","Successful delivery"):reward87Text("Sans perte ni KO","No losses or KO"),"+"+registerNumber(xp.quality)+" XP");
 reward87Rule(p,562,490,326);
 reward87Label(p,562,505,211,43,25,reward87Text("XP \303\240 la r\303\251ussite","XP on success"),true);reward87Label(p,768,505,120,43,29,registerNumber(xp.xp)+" XP",true,true);
 reward87Label(p,562,554,326,64,16,o.missionType==MCT_MAIL?reward87Text("Les primes d\342\200\231argent ne r\303\251duisent pas l\342\200\231XP.","Cash bonuses do not reduce XP."):reward87Text("Estimation sans perte ni KO.\nBilan final variable.","Estimate with no losses or KO.\nFinal outcome may vary."));
 MyGUI::Button* close=p->createWidget<MyGUI::Button>("Board77Card",0,0,1,1,MyGUI::Align::Default);board77Coord(close,316,650,298,54);board77Font(close,26);close->setCaption(reward87Text("Fermer","Close"));close->setTextColour(board77Gold);close->setTextAlign(MyGUI::Align::Center);close->eventMouseButtonClick+=MyGUI::newDelegate(closeRewardPopup87);
 MyGUI::Button* cross=p->createWidget<MyGUI::Button>("Board77Card",0,0,1,1,MyGUI::Align::Default);board77Coord(cross,858,22,48,48);board77Font(cross,25);cross->setCaption("X");cross->setTextColour(board77Gold);cross->setTextAlign(MyGUI::Align::Center);cross->eventMouseButtonClick+=MyGUI::newDelegate(closeRewardPopup87);
 reward87Label(p,42,715,846,28,16,o.routeRegions.find(";PRICE85;")!=std::string::npos?reward87Text("Nouveau bar\303\250me \302\267 Valeurs du contrat s\303\251lectionn\303\251","New rates \302\267 Selected contract values"):reward87Text("Offre enregistr\303\251e \302\267 Tarif conserv\303\251","Saved offer \302\267 Original price retained"))->setTextAlign(MyGUI::Align::Center);
 board77Scale=savedScale;
}
