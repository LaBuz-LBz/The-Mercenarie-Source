#pragma once
// The reference uses a 1536 x 1024 canvas. Crop only its outer margin (20,34).
// Scale both axes together so fonts, artwork, hit targets and spacing never drift.
float negotiationScale=1.0f;
MyGUI::ScrollView* negotiationContent=0; // Legacy handle; the faithful layout fits as one page.
MyGUI::Widget *negotiationFill=0,*negotiationFrame=0,*negotiationConditions=0,*negotiationRatePanel=0;
MyGUI::Widget *negotiationSummary=0,*negotiationStaff=0,*negotiationCounterPanel=0;
MyGUI::TextBox *negotiationTotalValue=0,*negotiationAdvanceValue=0,*negotiationBalanceValue=0,*negotiationRateValue=0,*negotiationAdvanceLabel=0;
MyGUI::Button* negotiationAdvanceChoices[4]={0};
void layoutNegotiationButtons(bool showCounter);
void updateNegotiationPrice(size_t position);
void negotiationAdvanceChosen(MyGUI::Widget* sender){
 const int choices[]={0,10,25,50};for(int i=0;i<4;++i)if(sender==negotiationAdvanceChoices[i]){
 negotiatedAdvancePercent=choices[i];if(negotiatedAdvancePercent)currentContract.selectedBonuses|=EB_HALF_NOW;else currentContract.selectedBonuses&=~EB_HALF_NOW;
 updateNegotiationPrice(priceSlider?priceSlider->getScrollPosition():20);return;}
}
MyGUI::Colour negotiationInk(.88f,.85f,.79f),negotiationGold(.90f,.71f,.42f),negotiationMuted(.65f,.63f,.59f);
void negotiationCloseV9(MyGUI::WidgetPtr){suspendNegotiation();}
std::string negotiationDecimal(double value){
 std::ostringstream s;s.imbue(std::locale::classic());s<<std::fixed<<std::setprecision(1)<<value;
 std::string result=s.str();if(Loc::engine().language=="fr"||Loc::engine().language=="pl"||Loc::engine().language=="ru")std::replace(result.begin(),result.end(),'.',',');return result;
}
int negotiationPx(int n){return static_cast<int>(n*negotiationScale+.5f);}
MyGUI::IntCoord negotiationRect(int x,int y,int w,int h){return MyGUI::IntCoord(negotiationPx(x-20),negotiationPx(y-34),negotiationPx(w),negotiationPx(h));}
int negotiationFontSize(int size){return std::max(8,std::min(56,(negotiationPx(size)+1)/2*2));}
void negotiationFont(MyGUI::TextBox* t,int size,bool sans=false){
 int pixels=negotiationFontSize(size);std::ostringstream name;name<<"Negotiation74"<<(sans?"Sans":"Serif")<<pixels;
 if(!MyGUI::ResourceManager::getInstance().isExist(name.str()))MyGUI::ResourceManager::getInstance().load(name.str()+".xml");
 t->setFontName(name.str());
 // Preserve Cyrillic and community-font support rather than replacing missing glyphs.
 if(Loc::engine().language!="fr"&&Loc::engine().language!="en") {MercenarieFonts::ensure();t->setFontName(MercenarieFonts::fontFor(Loc::engine().language));}
 t->setFontHeight(pixels);
}
void negotiationCaption(MyGUI::TextBox* t,const std::string& caption){
 t->setCaption(mercenarieLocalize(caption));
 // Keep the reserved rectangle fixed. Very long names/translations shrink instead of overlapping.
 int original=t->isUserString("NegotiationFontHeight")?atoi(t->getUserString("NegotiationFontHeight").c_str()):t->getFontHeight();
 t->setFontHeight(original);
 while(t->getFontHeight()>8&&t->getTextSize().width>t->getTextRegion().width)t->setFontHeight(t->getFontHeight()-1);
}
std::string negotiationTracked(const std::string& text){
 std::string out;for(size_t i=0;i<text.size();++i){unsigned char c=static_cast<unsigned char>(text[i]);if(i&&(c&0xc0)!=0x80)out+=" ";out+=text[i];}return out;
}
MyGUI::TextBox* negotiationLabel(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text,const MyGUI::Colour& color,bool sans=false,MyGUI::Align align=MyGUI::Align::Left|MyGUI::Align::Top){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Negotiation74Text",negotiationRect(x,y,w,h),MyGUI::Align::Default);
 negotiationFont(t,size,sans);t->setUserString("NegotiationFontHeight",registerNumber(t->getFontHeight()));t->setTextAlign(align);t->setTextColour(color);t->setNeedMouseFocus(false);negotiationCaption(t,sans?negotiationTracked(text):text);return t;
}
MyGUI::Button* negotiationAction(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text,const MyGUI::Colour& color,const char* skin="Negotiation74Text"){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>(skin,negotiationRect(x,y,w,h),MyGUI::Align::Default);negotiationFont(b,size);b->setUserString("NegotiationFontHeight",registerNumber(b->getFontHeight()));b->setTextAlign(MyGUI::Align::Center);b->setTextColour(color);negotiationCaption(b,text);return b;
}
void negotiationFitButton(MyGUI::Button* b){if(!b)return;negotiationCaption(b,b->getCaption().asUTF8());b->setTextAlign(MyGUI::Align::Center);}
void layoutNegotiationV9Content(){layoutNegotiationButtons(counterOfferActive);}
void negotiationReaction(const std::string& text){
 if(!reactionText)return;
 // Counteroffers need an explicit response area; ordinary hints are already on the plate.
 reactionText->setCaption(RerollPopupLayout::wrap(mercenarieLocalize(text),reactionText->getWidth()-8,LauncherMeasure(reactionText)));
 layoutNegotiationV9Content();
}
void refreshNegotiationV9Price(int percent){
 negotiationCaption(proposalText,registerNumber(currentContract.totalPay)+" Cats");
 negotiationCaption(sliderPercentText,(percent>0?"+":"")+registerNumber(percent)+" %");
 negotiationCaption(negotiationTotalValue,registerNumber(currentContract.totalPay)+" Cats");
 negotiationCaption(negotiationAdvanceValue,registerNumber(currentContract.advance)+" Cats");
 negotiationCaption(negotiationAdvanceLabel,std::string(Loc::text("negotiation.design.advance_label"))+" \xC2\xB7 "+registerNumber(negotiatedAdvancePercent)+" %");
 negotiationCaption(negotiationBalanceValue,registerNumber(currentContract.finalPay)+" Cats");
 negotiationCaption(negotiationRateValue,std::string(Loc::text("negotiation.design.rate"))+" : "+negotiationDecimal(currentContract.negotiatedRate)+" Cats/km");
 const int choices[]={0,10,25,50};for(int i=0;i<4;++i){bool selected=negotiatedAdvancePercent==choices[i];negotiationAdvanceChoices[i]->setStateSelected(selected);negotiationAdvanceChoices[i]->setTextColour(selected?MyGUI::Colour(.08f,.07f,.04f):negotiationInk);}
 int fill=negotiationPx(688)*(percent+20)/45;negotiationFill->setSize(std::max(1,fill),std::max(1,negotiationPx(8)));negotiationFill->setVisible(percent>-20);
 sliderMinusButton->setEnabled(percent>-20);sliderPlusButton->setEnabled(percent<25);
 layoutNegotiationV9Content();
}
