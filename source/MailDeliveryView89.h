namespace {
float mailDeliveryScale89=1.0f;

std::string mailDeliveryContract89;
MyGUI::TextBox* mailDeliveryInfo89=0;
void closeMailDelivery89(MyGUI::Widget*){if(mailDeliveryWindow89)mailDeliveryWindow89->setVisible(false);}
void closeMailDeliveryWindow89(MyGUI::Window*,const std::string&){closeMailDelivery89(0);}
int mailDeliveryIndex89(){for(size_t i=0;i<mailContracts.size();++i)if(mailContracts[i].contractId==mailDeliveryContract89)return (int)i;return -1;}
Character* nearbyMailRecipient89(MailContracts::Contract& mission,MailContracts::Step& step){
 Character* carrier=0;Item* item=0;InventorySection* section=0;if(!mailResolveCarrier(step,carrier,item,section)||!carrier)return 0;
 lektor<RootObject*> nearby;ou->getCharactersWithinSphere(nearby,carrier->getPosition(),25.0f,25.0f,25.0f,64,64,carrier);
 Character* best=0;float distance=626;
 for(unsigned int i=0;i<nearby.size();++i){Character* c=dynamic_cast<Character*>(nearby[i]);if(!c||!mailRecipientEligible(c,mission,step))continue;float d=c->getPosition().squaredDistance(carrier->getPosition());if(d<distance){distance=d;best=c;}}return best;
}
void refreshMailDelivery89(){
 int index=mailDeliveryIndex89();if(index<0){closeMailDelivery89(0);return;}
 MailContracts::Contract& m=mailContracts[index];std::ostringstream text;text<<Loc::text("mail89.help")<<"\n\n";
 for(size_t i=0;i<m.steps.size();++i){MailContracts::Step& step=m.steps[i];text<<(step.delivered?"[OK] ":"[ ] ")<<step.townName<<" - "<<mailRoleName(step.recipientRole)<<"\n";if(!step.delivered){Character* carrier=0;Item* item=0;InventorySection* section=0;bool found=mailResolveCarrier(step,carrier,item,section);text<<Loc::text("mail89.carrier")<<(found?carrier->getName():Loc::text("mail89.missing"))<<"\n";}}
 mailDeliveryInfo89->setCaption(text.str());float oldScale=reportScaleX;reportScaleX=mailDeliveryScale89;r88Fit(mailDeliveryInfo89,20);reportScaleX=oldScale;
}
void deliverNearbyMail89(MyGUI::Widget*){
 if(!mailDeliveryWindow89||!mailDeliveryWindow89->getVisible())return;
 if(missionWorldChanging||missionRestorePending||progressWriteBlocked||progressLoadFault||!ou||!ou->player)return;
 int index=mailDeliveryIndex89();if(index<0)return;MailContracts::Contract& mission=mailContracts[index];
 if(mission.status!=MailContracts::MailActive||mission.delegated){ou->showPlayerAMessage(Loc::text("mail89.inactive"),true);return;}
 bool missing=false;
 for(size_t i=0;i<mission.steps.size();++i){MailContracts::Step& step=mission.steps[i];if(step.delivered)continue;
  Character* carrier=0;Item* item=0;InventorySection* section=0;if(!mailResolveCarrier(step,carrier,item,section)){missing=true;continue;}
  Character* recipient=nearbyMailRecipient89(mission,step);if(recipient){performMailDelivery89(recipient,mission,step);refreshMailDelivery89();return;}}
 ou->showPlayerAMessage(Loc::text(missing?"mail89.letter":"mail89.none"),true);refreshMailDelivery89();
}
void cancelFromMailDelivery89(MyGUI::Widget*){int index=mailDeliveryIndex89();closeMailDelivery89(0);if(index>=0)requestMailCancel(index);}
void openMailDelivery89(int index){
 if(index<0||index>=(int)mailContracts.size()||missionWorldChanging||missionRestorePending||!MyGUI::Gui::getInstancePtr())return;
 mailDeliveryContract89=mailContracts[index].contractId;
 if(!mailDeliveryWindow89){
  const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();float oldX=reportScaleX,oldY=reportScaleY;
  reportScaleX=reportScaleY=std::min(1.25f,std::min((view.width-32)/760.0f,(view.height-32)/510.0f));mailDeliveryScale89=reportScaleX;int w=rx(760),h=ry(510);
  MyGUI::ResourceManager::getInstance().load("ContractBoardV6.xml");MyGUI::ResourceManager::getInstance().load("MercenarieBoard77.xml");
  mailDeliveryWindow89=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("ContractBoardWindowV6",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","MailDelivery89");mailDeliveryWindow89->eventWindowButtonPressed+=MyGUI::newDelegate(closeMailDeliveryWindow89);
  MyGUI::Widget* c=mailDeliveryWindow89->getClientWidget();MyGUI::Widget* bg=c->createWidget<MyGUI::Widget>("Board77Frame",0,0,w,h,MyGUI::Align::Default);bg->setNeedMouseFocus(false);
  r88Text(c,26,22,652,48,32,Loc::text("mail89.title"),true);
  MyGUI::Button* x=r88Button(c,690,22,42,42,"X");x->eventMouseButtonClick+=MyGUI::newDelegate(closeMailDelivery89);
  mailDeliveryInfo89=r88Text(c,30,84,700,290,20,"");mailDeliveryInfo89->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
  MyGUI::Button* deliver=r88Button(c,30,393,700,50,Loc::text("mail89.deliver"),true);deliver->eventMouseButtonClick+=MyGUI::newDelegate(deliverNearbyMail89);
  MyGUI::Button* cancel=r88Button(c,30,453,340,34,Loc::text("mail.cancel.confirm"));cancel->eventMouseButtonClick+=MyGUI::newDelegate(cancelFromMailDelivery89);
  MyGUI::Button* back=r88Button(c,390,453,340,34,Loc::text("common.back"));back->eventMouseButtonClick+=MyGUI::newDelegate(closeMailDelivery89);
  reportScaleX=oldX;reportScaleY=oldY;
 }
 refreshMailDelivery89();mailDeliveryWindow89->setVisible(true);
}

}
