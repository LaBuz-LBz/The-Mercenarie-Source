// Presentation only. Uses the same offer selection, pricing, map and reroll handlers.
// Reference coordinates are measured inside the approved 1536 x 928 frame.
float board77Scale=1.0f;
MyGUI::Button* board77Price=0;
MyGUI::TextBox *board77Brand=0,*board77Subtitle=0,*board77Route=0,*board77Distance=0,*board77Hint=0;
MyGUI::ImageBox* board77Logo=0;
MyGUI::Widget *board77HeaderRule=0,*board77BrandRule=0;
std::vector<MyGUI::Widget*> board77Decoration;
MyGUI::TextBox* board81RewardLabels[3]={0};
const MyGUI::Colour board77Ink(.88f,.85f,.79f),board77Gold(.90f,.71f,.42f),board77Edge(.44f,.37f,.25f);
void showContractPriceDetails(MyGUI::WidgetPtr);
int board77Px(int n){return static_cast<int>(n*board77Scale+.5f);}
void board77Coord(MyGUI::Widget* w,int x,int y,int width,int height){
 MyGUI::IntCoord r(board77Px(x),board77Px(y),board77Px(width),board77Px(height));
 if(w->getCoord()!=r)w->setCoord(r);
}
void board77Skin(MyGUI::Widget* w,const char* skin){
 if(w->getUserString("board77Skin")==skin)return;
 // MyGUI resets a TextBox caption when its skin changes. Restore it explicitly.
 MyGUI::TextBox* text=w->castType<MyGUI::TextBox>(false);
 std::string saved=text?text->getCaption().asUTF8():"";
 w->changeWidgetSkin(skin);w->setUserString("board77Skin",skin);
 if(text)text->setCaption(saved);
}
void board77Font(MyGUI::TextBox* t,int size,bool sans=false){
 // The font point size is NOT MyGUI's line height: native logs show 32pt ~54px.
 // Reuse two atlases instead of generating one texture for every label size.
 const char* name=sans?"Negotiation74Sans24":"Negotiation74Serif32";
 int nominal=sans?24:32;
 MyGUI::ResourceManager& rm=MyGUI::ResourceManager::getInstance();
 if(!rm.isExist(name))rm.load(std::string(name)+".xml");
 std::string selected=name;
 if(Loc::engine().language!="fr"&&Loc::engine().language!="en"){MercenarieFonts::ensure();selected=MercenarieFonts::fontFor(Loc::engine().language);}
 // A newly created Board77Text has no default font. Kenshi getFontName()
 // returns an invalid reference until setFontName() has initialized it.
 // Set it directly; never read the font of an uninitialized text widget.
 t->setFontName(selected);
 int pixels=std::max(10,board77Px(size));
 // Kenshi's font resource is not safely accessible through the SDK IFont ABI.
 // Use calibrated line metrics for these two fixed bundled fonts, without
 // dereferencing or calling virtual methods on the engine's font resource.
 const int atlasLineHeight=sans?40:40;
 int lineHeight=selected==name?std::max(10,(pixels*atlasLineHeight+nominal/2)/nominal):pixels;
 t->setFontHeight(lineHeight);
}
void board77Text(MyGUI::TextBox* t,int x,int y,int w,int h,int size,const MyGUI::Colour& ink,bool sans=false,MyGUI::Align align=MyGUI::Align::Left|MyGUI::Align::VCenter){
 board77Skin(t,"Board77Text");board77Font(t,size,sans);
 int height=std::max(board77Px(h),t->getFontHeight());
 t->setCoord(board77Px(x),board77Px(y)-(height-board77Px(h))/2,board77Px(w),height);
 t->setTextColour(ink);t->setTextAlign(align);
}
void board77Fit(MyGUI::TextBox* t){
 // One proportional fit plus a rounding check, not a pixel-by-pixel layout loop.
 MyGUI::IntSize text=t->getTextSize();MyGUI::IntCoord box=t->getTextRegion();
 if(text.width<=box.width&&text.height<=box.height)return;
 float scale=std::min(1.0f,std::min(box.width/(float)std::max(1,text.width),box.height/(float)std::max(1,text.height)));
 int minimum=std::max(12,board77Px(16));
 t->setFontHeight(std::max(minimum,(int)(t->getFontHeight()*scale)));
}
MyGUI::Widget* board77CreatePanel(MyGUI::Widget* p,int x,int y,int w,int h){
 MyGUI::Widget* panel=p->createWidget<MyGUI::Widget>("Board77Panel",x,y,w,h,MyGUI::Align::Default);
 panel->setUserString("board77Skin","Board77Panel");
 registerSolid(panel,0,0,w,1,board77Edge);registerSolid(panel,0,h-1,w,1,board77Edge);registerSolid(panel,0,0,1,h,board77Edge);registerSolid(panel,w-1,0,1,h,board77Edge);return panel;
}
MyGUI::TextBox* board77CreateText(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& caption,const MyGUI::Colour& color){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Board77Text",x,y,w,h,MyGUI::Align::Default);
 t->setUserString("board77Skin","Board77Text");MercenarieFonts::caption(t,caption);t->setFontHeight(size);t->setTextColour(color);t->setNeedMouseFocus(false);return t;
}
void board77Caption(MyGUI::TextBox* t,const std::string& value){t->setCaption(value);board77Fit(t);}
std::string board77Tracked(const std::string& value){std::string out;for(size_t i=0;i<value.size();++i){if(i&&((unsigned char)value[i]&0xc0)!=0x80)out+=' ';out+=value[i];}return out;}
void board77Section(MyGUI::TextBox* t,const char* key){board77Caption(t,board77Tracked(Loc::text(key)));}
void board77Panel(MyGUI::Widget* w,int x,int y,int width,int height,bool edges=true){
 board77Skin(w,"Board77Panel");w->setColour(MyGUI::Colour(1,1,1));board77Coord(w,x,y,width,height);
 int ww=w->getWidth(),hh=w->getHeight();
 for(int i=0;i<4;++i){MyGUI::Widget* e=w->getChildAt(i);e->setVisible(edges);e->setColour(board77Edge);}
 w->getChildAt(0)->setCoord(0,0,ww,1);w->getChildAt(1)->setCoord(0,hh-1,ww,1);w->getChildAt(2)->setCoord(0,0,1,hh);w->getChildAt(3)->setCoord(ww-1,0,1,hh);
}
MyGUI::TextBox* board77NewText(MyGUI::Widget* parent){
 MyGUI::TextBox* t=parent->createWidget<MyGUI::TextBox>("Board77Text",0,0,1,1,MyGUI::Align::Default);t->setUserString("board77Skin","Board77Text");t->setNeedMouseFocus(false);board77Decoration.push_back(t);return t;
}
void createBoard77(MyGUI::Widget* c){
 if(!MyGUI::ResourceManager::getInstance().isExist("Board77Text"))MyGUI::ResourceManager::getInstance().load("MercenarieBoard77.xml");
 board77Decoration.clear();
 MyGUI::TextBox* rewardValues[]={boardXpV6,contractsRewardReputation,contractsRewardBonus};
 for(int i=0;i<3;++i)board81RewardLabels[i]=board77NewText(rewardValues[i]->getParent());
 board77Brand=board77NewText(missionBookHeader);board77Subtitle=board77NewText(missionBookHeader);
 board77HeaderRule=missionBookHeader->createWidget<MyGUI::Widget>("WhiteSkin",0,0,1,1,MyGUI::Align::Default);board77HeaderRule->setColour(board77Edge);board77HeaderRule->setNeedMouseFocus(false);board77Decoration.push_back(board77HeaderRule);
 board77BrandRule=missionBookHeader->createWidget<MyGUI::Widget>("WhiteSkin",0,0,1,1,MyGUI::Align::Default);board77BrandRule->setColour(board77Edge);board77BrandRule->setNeedMouseFocus(false);board77Decoration.push_back(board77BrandRule);
 board77Logo=missionBookHeader->createWidget<MyGUI::ImageBox>("ImageBox",0,0,1,1,MyGUI::Align::Default);
 board77Logo->setImageTexture("MercenarieNegotiationPlate74.png");board77Logo->setImageCoord(MyGUI::IntCoord(48,52,76,94));board77Logo->setNeedMouseFocus(false);board77Decoration.push_back(board77Logo);
 board77Route=board77NewText(contractsMapPanel);board77Distance=board77NewText(contractsMapPanel);board77Hint=board77NewText(contractsMapPanel);
 board77Price=c->createWidget<MyGUI::Button>("Board77Card",0,0,1,1,MyGUI::Align::Default);board77Price->eventMouseButtonClick+=MyGUI::newDelegate(showContractPriceDetails);board77Decoration.push_back(board77Price);
}
void resetBoard77(){board77Decoration.clear();board77Price=0;board77Brand=board77Subtitle=board77Route=board77Distance=board77Hint=0;board77Logo=0;board77HeaderRule=board77BrandRule=0;for(int i=0;i<3;++i)board81RewardLabels[i]=0;}
void applyBoard77(){
 if(!board77Price)return;
 const bool active=!missionBookDelegationContext;
 for(size_t i=0;i<board77Decoration.size();++i)board77Decoration[i]->setVisible(active);
 missionBookPriceButton->setVisible(!active);
 if(!active){
  // Delegation changes the action skin itself; force the gold skin on return.
  contractsAccept->setUserString("board77Skin","");
  for(int i=0;i<4;++i)contractsAccept->getChildAt(i)->setVisible(true);
  return;
 }
 const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
 board77Scale=std::min((view.width-32)/1536.0f,(view.height-48)/928.0f);
 // Reduce large-screen coverage while preserving legibility on smaller displays.
 if(board77Scale>.85f)board77Scale=std::max(.85f,board77Scale*.85f);
 const int width=board77Px(1536),height=board77Px(928);
 contractsWindow->setCoord((view.width-width)/2,(view.height-height)/2,width,height);
 board77Coord(missionBookBackground,0,0,1536,928);board77Skin(missionBookBackground,"Board77Frame");missionBookBackground->setColour(MyGUI::Colour(1,1,1));
 for(int i=0;i<4;++i)missionBookBackground->getChildAt(i)->setVisible(false);
 board77Panel(missionBookHeader,8,8,1520,90,false);
 board77Coord(board77HeaderRule,0,89,1520,1);board77Coord(board77BrandRule,424,8,1,62);
 board77Coord(board77Logo,14,5,62,77);
 board77Text(board77Brand,92,26,316,32,20,board77Ink,true);board77Caption(board77Brand,"T H E   M E R C E N A R I E");
 board77Text(contractsHeading,460,7,968,60,44,board77Ink);board77Caption(contractsHeading,Loc::text("board.design.title"));
 board77Text(board77Subtitle,464,61,900,28,17,board77Gold,true);board77Section(board77Subtitle,"board.design.subtitle");
 board77Coord(missionBookCloseButton,1460,14,44,46);board77Skin(missionBookCloseButton,"Board77Card");board77Font(missionBookCloseButton,22,true);MercenarieFonts::caption(missionBookCloseButton,"X");missionBookCloseButton->setTextColour(board77Gold);missionBookCloseButton->setTextAlign(MyGUI::Align::Center);
 board77Panel(contractsListPanel,8,106,442,812);
 board77Text(missionBookOffersTitle,14,4,412,40,17,board77Ink,true);board77Section(missionBookOffersTitle,"ui.available_offers");
 for(int i=0;i<6;++i){
  MyGUI::Widget* row=contractButtons[i]->getParent();board77Panel(row,12,52+i*118,418,106,false);
  board77Coord(contractButtons[i],0,0,418,106);board77Skin(contractButtons[i],"Board77Card");
  // Keep reroll available as a small native control, including hover explanation.
  board77Coord(offerRerollV6[i],378,4,34,34);
  board77Coord(offerIconV6[i],18,26,52,52);offerIconV6[i]->setColour(board77Gold);offerIconV6[i]->setVisible(true);
  board77Text(offerNameV6[i],90,2,282,40,22,board77Ink);board77Caption(offerNameV6[i],boardOffers[i].available?mercenarieLocalize(boardOffers[i].townName):Loc::text("ui.new_mission_in"));
  board77Text(offerTypeV6[i],90,39,310,28,14,board77Gold,true);if(boardOffers[i].available)board77Caption(offerTypeV6[i],missionLabelV6(boardOffers[i].missionType));else board77Fit(offerTypeV6[i]);
  board77Text(offerRarityV6[i],90,70,122,30,13,boardOffers[i].rarity?MyGUI::Colour(.64f,.71f,.86f):board77Ink,true);board77Fit(offerRarityV6[i]);
  board77Text(offerPriceV6[i],206,64,198,40,22,board77Gold,false,MyGUI::Align::Right|MyGUI::Align::VCenter);board77Caption(offerPriceV6[i],registerNumber(displayedContractCats(boardOffers[i].estimatedPay))+" Cats");
  board77Coord(offerEdgesV6[i][0],0,0,418,2);board77Coord(offerEdgesV6[i][1],0,104,418,2);board77Coord(offerEdgesV6[i][2],0,0,2,106);board77Coord(offerEdgesV6[i][3],416,0,2,106);
  for(int edge=0;edge<4;++edge){offerEdgesV6[i][edge]->setColour(board77Gold);offerEdgesV6[i][edge]->setVisible(boardOffers[i].available&&i==selectedOffer);}
 }
 board77Coord(contractsMapPanel,460,106,688,740);board77Skin(contractsMapPanel,"Board77Panel");contractsMapPanel->setColour(MyGUI::Colour(1,1,1));
 board77Coord(contractsMapImage,8,42,672,666);
 board77Text(board77Route,12,0,432,40,17,board77Ink,true);board77Section(board77Route,"board.design.route");
 const BoardOffer& offer=boardOffers[selectedOffer];
 int km=(int)(offer.distance/1000*(offer.missionType==MCT_MAIL||offer.missionType==0?1:2));
 board77Text(board77Distance,472,0,196,40,20,board77Ink,false,MyGUI::Align::Right|MyGUI::Align::VCenter);board77Caption(board77Distance,registerNumber(km)+" km");
 board77Text(contractsMap,10,710,288,28,16,board77Ink);board77Caption(contractsMap,Loc::text("board.design.map_controls"));
 board77Text(board77Hint,292,710,388,28,16,board77Ink,false,MyGUI::Align::Right|MyGUI::Align::VCenter);board77Caption(board77Hint,Loc::text("board.design.map_hint"));
 board77Coord(contractsAccept,464,862,678,56);board77Skin(contractsAccept,"Board77Gold");board77Font(contractsAccept,27);contractsAccept->setTextAlign(MyGUI::Align::Center);contractsAccept->setTextColour(MyGUI::Colour(.09f,.08f,.04f));board77Caption(contractsAccept,Loc::text(contractAcceptArmed?"options.confirm":"board.design.select"));
 for(int i=0;i<4;++i)contractsAccept->getChildAt(i)->setVisible(false);
 board77Coord(board77Price,1162,862,358,56);board77Font(board77Price,23);board77Price->setTextAlign(MyGUI::Align::Center);board77Price->setTextColour(board77Ink);board77Caption(board77Price,Loc::text("board.design.price"));
 board77Panel(contractsInfoPanel,1156,106,372,740);
 MyGUI::TextBox* title=contractsInfoPanel->getChildAt(4)->castType<MyGUI::TextBox>();
 board77Text(title,14,0,344,42,17,board77Ink,true);board77Section(title,"board.design.details");
 const int rowY[]={48,130,218,304},rowH[]={82,88,80,80};
 for(int i=0;i<4;++i){
  MyGUI::Widget* card=contractsInfo[i]->getParent();board77Panel(card,12,rowY[i],348,rowH[i],false);
  MyGUI::Widget* icon=card->getChildAt(4);board77Coord(icon,6,14,48,48);icon->setColour(board77Gold);
  if(i==2){MyGUI::ImageBox* skull=icon->castType<MyGUI::ImageBox>();skull->setImageTexture("ContractIconsV6.png");skull->setImageCoord(MyGUI::IntCoord(7*96,0,96,96));}
  board77Text(contractsInfo[i],68,4,272,rowH[i]-8,i==0?25:17,board77Ink,i==2||i==3);
  if(i>0){card->getChildAt(1)->setVisible(true);card->getChildAt(1)->setColour(board77Edge);}
 }
 std::string mission=missionLabelV6(offer.missionType);
 if(Loc::engine().language=="fr"){const char* names[]={"Escorte","Caravane","Expedition scientifique","Chasse a la prime","Livraison de message"};if(offer.missionType>=0&&offer.missionType<5)mission=names[offer.missionType];}
 board77Caption(contractsInfo[0],mission);
 std::string route=contractsInfo[1]->getCaption().asUTF8();size_t newline=route.find('\n');if(newline!=std::string::npos)route=route.substr(newline+1);
 // Build readable lines from the raw itinerary, before legacy wrapping.
 board77Text(contractsInfo[1],68,0,272,88,18,board77Ink);
 bookWrap(contractsInfo[1],route);board77Fit(contractsInfo[1]);
 // Unknown route danger stays unknown; the renderer never invents a risk estimate.
 const bool dangerKnown=contractsInfo[2]->getCaption().asUTF8().find('?')==std::string::npos;
 board77Caption(contractsInfo[2],std::string(Loc::text("board.design.danger"))+"\n"+(dangerKnown?registerNumber(offer.dangerLevel):"?")+" / 5");
 for(int j=0;j<5;++j){board77Coord(boardDangerV6[j],152+j*34,43,23,23);boardDangerV6[j]->setColour(dangerKnown&&j<offer.dangerLevel?MyGUI::Colour(.84f,.16f,.25f):MyGUI::Colour(.30f,.29f,.26f));}
 board77Fit(contractsInfo[3]);
 MyGUI::TextBox* rewards=contractsInfoPanel->getChildAt(9)->castType<MyGUI::TextBox>();
 board77Text(rewards,14,395,344,32,17,board77Ink,true);board77Section(rewards,"board.design.reward");
 MyGUI::TextBox* values[]={contractsRewardAmount,boardXpV6,contractsRewardReputation,contractsRewardBonus};
 const int rewardY[]={433,519,587,653},rewardH[]={76,66,64,71};
 for(int i=0;i<4;++i){
  MyGUI::Widget* card=values[i]->getParent();board77Panel(card,12,rewardY[i],348,rewardH[i],false);
  board77Coord(card->getChildAt(4),6,12,46,46);card->getChildAt(4)->setColour(board77Gold);
  card->getChildAt(1)->setVisible(i<3);card->getChildAt(1)->setColour(board77Edge);
  board77Text(values[i],68,3,274,rewardH[i]-6,i==0?43:17,i==0?board77Gold:board77Ink);
 }
 board77Caption(contractsRewardAmount,registerNumber(displayedContractCats(offer.estimatedPay))+" Cats");
 const char* labels[]={"board.design.xp","board.design.reputation","board.design.tips"};
 for(int i=0;i<3;++i){
  board77Text(board81RewardLabels[i],68,3,274,24,14,board77Ink,true);board77Caption(board81RewardLabels[i],Loc::text(labels[i]));
  board77Text(values[i+1],68,28,274,30,17,board77Ink);board77Caption(values[i+1],Loc::text(i==2?"board.design.conditional":"board.design.variable"));
 }
 board77Text(contractsRefreshText,22,866,414,40,16,board77Ink);board77Fit(contractsRefreshText);contractsLegend->setVisible(false);
}
