// Included after register presentation helpers. Runtime transaction is defined
// after the existing save/load integration, never serialized as a pending action.
MyGUI::Widget* guildInvestmentOverlay=0;
MyGUI::TextBox *investmentAmount=0,*investmentReward=0,*investmentBalance=0;
MyGUI::TextBox *investmentAvailableText=0,*investmentCurrentText=0,*investmentCostText=0,*investmentMaxText=0,*investmentPreviewText=0;
MyGUI::Widget *investmentCurrentFill=0,*investmentPreviewFill=0;
int investmentCurrentWidth=1,investmentPreviewWidth=1;
MyGUI::Button *investmentConfirm=0,*investmentMinus=0,*investmentPlus=0,*investmentQuick[4]={0};
int investmentCats=MercenarieConfig::Progression::CatsToXpCost;
bool investmentArmed=false,investmentBusy=false;
std::string investmentSlot;
std::vector<std::pair<MyGUI::Widget*,bool> > investmentPreviousPages;
bool investmentPageActive=false;
int investmentAvailable();
void confirmGuildInvestment(MyGUI::Widget*);
void refreshGuildInvestment();
void closeGuildInvestment(MyGUI::Widget*){
    investmentArmed=false;
    if(guildInvestmentOverlay)guildInvestmentOverlay->setVisible(false);
    if(investmentPageActive){
        for(size_t i=0;i<investmentPreviousPages.size();++i)
            if(investmentPreviousPages[i].first)investmentPreviousPages[i].first->setVisible(investmentPreviousPages[i].second);
        investmentPreviousPages.clear();investmentPageActive=false;
        if(guildWindow)MercenarieFonts::caption(guildWindow,Loc::text("ui.the_mercenarie_guild_management"));
    }
}
MyGUI::Button* investmentButton(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& caption){
    MyGUI::Button* b=p->createWidget<MyGUI::Button>("GuildRegisterButton",x,y,w,h,MyGUI::Align::Default);
    MercenarieFonts::caption(b,caption);b->setFontHeight(24);b->setTextColour(registerAmber);return b;
}
MyGUI::Widget* investmentSurface(MyGUI::Widget* p,int x,int y,int w,int h){
    // Unlike decorative registerSolid widgets, this surface owns interactive
    // descendants and must remain in the mouse-picking hierarchy.
    MyGUI::Widget* surface=p->createWidget<MyGUI::Widget>("WhiteSkin",x,y,w,h,MyGUI::Align::Default);
    surface->setColour(MyGUI::Colour(0.055f,0.052f,0.047f));return surface;
}
std::string investmentTitle(){
    // Kenshi's painted font has no Unicode arrow glyph.
    std::string title=Loc::text("guild.exchange_title");size_t arrow=title.find("\xE2\x86\x92");if(arrow!=std::string::npos)title.replace(arrow,3,">");return title;
}
void changeGuildInvestment(MyGUI::Widget* sender){
    if(!investmentArmed||investmentBusy)return;
    const int step=MercenarieConfig::Progression::CatsToXpCost;
    int batches=sender==investmentMinus?-1:1;
    const int quick[]={1,2,5,10};for(int i=0;i<4;++i)if(sender==investmentQuick[i])batches=quick[i];
    long long amount=(long long)investmentCats+(long long)batches*step;
    if(amount==step&&amount<=INT_MAX&&(batches<0||amount<=investmentAvailable()))investmentCats=(int)amount;
    refreshGuildInvestment();
}
void buildGuildInvestment(){
    MyGUI::Widget* c=guildWindow->getClientWidget();
    guildInvestmentOverlay=c->createWidget<MyGUI::Widget>("WhiteSkin",0,0,guildLayout.designW,guildLayout.designH,MyGUI::Align::Default);
    guildInvestmentOverlay->setColour(MyGUI::Colour(0.025f,0.03f,0.03f));
    {
    MyGUI::Widget* p=investmentSurface(guildInvestmentOverlay,(guildLayout.designW-2492)/2,232,2492,956);
    registerArt(p,"GuildManagementScene.png",2,2,2488,180,0.68f);registerArt(p,"LevelSun.png",34,35,100,100,1.0f);
    registerText(p,158,30,1680,50,40,Loc::text("v8.literal.065"),registerIvory);
    registerText(p,158,84,1720,36,25,Loc::text("v8.literal.066"),registerBlue);
    registerText(p,158,126,1720,30,20,Loc::text("v8.literal.067"),registerIvory);
    const int y=198,h=520,leftW=720,midW=980,rightW=720;
    MyGUI::Widget* left=registerPanel(p,8,y,leftW,h);registerText(left,24,18,leftW-48,32,24,Loc::text("v8.literal.068"),registerAmber);registerLine(left,20,62,leftW-40);
    investmentAvailableText=registerText(left,42,92,leftW-84,118,38,"",registerIvory);investmentCurrentText=registerText(left,42,220,leftW-84,172,31,"",registerIvory);
    MyGUI::Widget* currentBar=registerSolid(left,42,420,leftW-84,42,MyGUI::Colour(0.06f,0.06f,0.055f));investmentCurrentWidth=leftW-94;investmentCurrentFill=registerSolid(currentBar,5,5,1,32,registerAmber);
    MyGUI::Widget* mid=registerPanel(p,16+leftW,y,midW,h);registerText(mid,24,18,midW-48,32,24,Loc::text("v8.literal.069"),registerAmber);registerLine(mid,20,62,midW-40);
    std::stringstream rate;rate<<registerNumber(MercenarieConfig::Progression::CatsToXpCost)<<" Cats = "<<MercenarieConfig::Progression::CatsToXpReward<<Loc::text("v8.literal.070");registerText(mid,32,78,midW-64,64,30,rate.str(),registerIvory)->setTextAlign(MyGUI::Align::Center);
    registerText(mid,48,178,340,42,25,Loc::text("v8.literal.071"),registerIvory);
    MyGUI::Widget* amount=registerPanel(mid,410,160,510,78);investmentMinus=investmentButton(amount,8,8,72,62,"-");investmentMinus->eventMouseButtonClick+=MyGUI::newDelegate(changeGuildInvestment);investmentPlus=investmentButton(amount,430,8,72,62,"+");investmentPlus->eventMouseButtonClick+=MyGUI::newDelegate(changeGuildInvestment);investmentAmount=registerText(amount,92,15,326,48,34,"",registerIvory);investmentAmount->setTextAlign(MyGUI::Align::Center);
    investmentCostText=registerText(mid,48,266,860,102,27,"",registerIvory);investmentMaxText=registerText(mid,48,370,860,44,23,"",registerAmber);
    investmentConfirm=investmentButton(mid,72,430,836,70,Loc::text("v8.literal.072"));investmentConfirm->setFontHeight(31);investmentConfirm->eventMouseButtonClick+=MyGUI::newDelegate(confirmGuildInvestment);
    MyGUI::Widget* right=registerPanel(p,24+leftW+midW,y,rightW,h);registerText(right,24,18,rightW-48,36,26,Loc::text("v8.literal.073"),registerAmber);registerLine(right,20,66,rightW-40);investmentBalance=registerText(right,42,92,rightW-84,82,31,"",registerIvory);investmentPreviewText=registerText(right,42,190,rightW-84,198,30,"",registerIvory);MyGUI::Widget* previewBar=registerSolid(right,42,420,rightW-84,42,MyGUI::Colour(0.06f,0.06f,0.055f));investmentPreviewWidth=rightW-94;investmentPreviewFill=registerSolid(previewBar,5,5,1,32,registerAmber);
    MyGUI::Widget* info=registerPanel(p,8,736,2476,200);registerText(info,30,18,2416,36,27,Loc::text("guild.pages.information"),registerAmber);registerText(info,42,66,2380,118,23,Loc::text("v8.literal.074"),registerIvory);
    for(int i=0;i<4;++i)investmentQuick[i]=0;
    guildInvestmentOverlay->setVisible(false);return;
    }
    // One centered, frameless composition inside the native Kenshi window.
    MyGUI::Widget* p=investmentSurface(guildInvestmentOverlay,90,48,1420,744);
    registerArt(p,"GuildManagementScene.png",12,128,420,602,0.70f);
    registerArt(p,"LevelSun.png",30,20,88,88,1.0f);
    registerText(p,140,25,1110,48,34,investmentTitle(),registerIvory);
    registerText(p,140,78,1110,36,22,Loc::text("guild.exchange_subtitle"),registerIvory);
    registerLine(p,24,120,1372);
    registerIcon(p,4,478,166,54,registerAmber);
    registerText(p,550,178,400,40,25,Loc::text("guild.cats_exchange"),registerAmber);
    registerText(p,1060,178,330,40,25,Loc::text("guild.xp_received"),registerBlue);
    MyGUI::Widget* amount=registerPanel(p,470,246,444,84);
    investmentMinus=investmentButton(amount,8,8,66,68,"-");investmentMinus->eventMouseButtonClick+=MyGUI::newDelegate(changeGuildInvestment);
    investmentPlus=investmentButton(amount,370,8,66,68,"+");investmentPlus->eventMouseButtonClick+=MyGUI::newDelegate(changeGuildInvestment);
    investmentAmount=registerText(amount,82,20,278,48,34,"",registerIvory);investmentAmount->setTextAlign(MyGUI::Align::Center);
    registerText(p,945,260,76,60,42,">>",registerAmber);
    MyGUI::Widget* reward=registerPanel(p,1040,246,350,84);
    investmentReward=registerText(reward,16,18,318,50,36,"",registerBlue);investmentReward->setTextAlign(MyGUI::Align::Center);
    investmentBalance=registerText(p,480,350,500,52,23,"",registerIvory);
    std::stringstream rate;rate<<registerNumber(MercenarieConfig::Progression::CatsToXpCost)<<" Cats = "<<MercenarieConfig::Progression::CatsToXpReward<<Loc::text("ui.xp_67b2c80");
    registerText(p,1040,350,350,50,21,rate.str(),registerIvory);
    MyGUI::Widget* info=registerPanel(p,470,425,920,106);
    registerText(info,22,14,876,30,22,Loc::text("guild.exchange_info_title"),registerBlue);
    registerText(info,22,48,876,52,20,Loc::text("guild.exchange_info"),registerIvory);
    const int quick[]={1,2,5,10};
    for(int i=0;i<4;++i){std::stringstream s;s<<"+ "<<registerNumber(quick[i]*MercenarieConfig::Progression::CatsToXpCost)<<"\n+ "<<quick[i]*MercenarieConfig::Progression::CatsToXpReward<<Loc::text("ui.xp_67b2c80");
        investmentQuick[i]=investmentButton(p,470+i*234,554,218,76,s.str());investmentQuick[i]->eventMouseButtonClick+=MyGUI::newDelegate(changeGuildInvestment);}
    investmentConfirm=investmentButton(p,710,660,450,60,Loc::text("guild.confirm_exchange"));investmentConfirm->eventMouseButtonClick+=MyGUI::newDelegate(confirmGuildInvestment);
    registerText(p,36,644,390,44,29,Loc::text("ui.the_mercenarie"),registerIvory);
    guildInvestmentOverlay->setVisible(false);
}
void openGuildInvestment(MyGUI::Widget*){
    if(!guildInvestmentOverlay||investmentBusy||progressWriteBlocked||investmentPageActive)return;
    investmentCats=MercenarieConfig::Progression::CatsToXpCost;
    investmentSlot=activeMercenarieSaveSlot;investmentArmed=true;
    // Cats -> XP is a page, not a translucent modal. Preserve the current
    // root-page state and hide every sibling so no navigation, card, text or
    // button from Guild Management can remain visible behind it.
    investmentPreviousPages.clear();MyGUI::Widget* c=guildWindow->getClientWidget();
    for(size_t i=0;i<c->getChildCount();++i){MyGUI::Widget* child=c->getChildAt(i);if(child==guildInvestmentOverlay)continue;investmentPreviousPages.push_back(std::make_pair(child,child->getVisible()));child->setVisible(false);}
    investmentPageActive=true;MercenarieFonts::caption(guildWindow,investmentTitle());
    guildInvestmentOverlay->setVisible(true);refreshGuildInvestment();
}
void refreshGuildInvestmentPreview(int available,int step,int earned){
    GuildProgressView before=currentGuildProgressView();int previewXp=guildPoints,previewPrestige=guildPrestige;GuildProgression::award(previewXp,previewPrestige,earned);GuildProgressView after(previewXp,previewPrestige);
    if(investmentAvailableText)MercenarieFonts::caption(investmentAvailableText,Loc::text("v8.literal.075")+registerNumber(std::max(0,available)));
    if(investmentCurrentText){std::stringstream s;s<<Loc::text("v8.literal.076")<<registerNumber(before.xp)<<"\n"<<Loc::text("v8.literal.077")<<before.level<<"\n"<<Loc::text("v8.literal.078")<<registerNumber(before.next)<<Loc::text("ui.xp_67b2c80");MercenarieFonts::caption(investmentCurrentText,s.str());}
    if(investmentCostText){std::stringstream s;s<<Loc::text("v8.literal.079")<<registerNumber(investmentCats)<<" Cats\n"<<Loc::text("v8.literal.080")<<registerNumber(earned)<<Loc::text("ui.xp_67b2c80");MercenarieFonts::caption(investmentCostText,s.str());}
    if(investmentMaxText)MercenarieFonts::caption(investmentMaxText,Loc::text("v8.literal.081")+registerNumber(step>0?std::max(0,available)/step:0)+Loc::text("v8.literal.082"));
    if(investmentPreviewText){std::stringstream s;s<<Loc::text("ui.guild_xp")<<registerNumber(after.xp)<<"\n"<<Loc::text("v8.literal.083")<<after.level<<"\n"<<registerNumber(after.xp)<<" / "<<registerNumber(after.next)<<Loc::text("ui.xp_67b2c80");MercenarieFonts::caption(investmentPreviewText,s.str());}
    float beforePart=before.level>=10?1.0f:float(before.xp-before.previous)/std::max(1,before.next-before.previous),afterPart=after.level>=10?1.0f:float(after.xp-after.previous)/std::max(1,after.next-after.previous);if(investmentCurrentFill){int displayed=std::max(1,investmentCurrentFill->getParent()->getWidth()-10);investmentCurrentFill->setSize(GuildResponsive::scaledFill(displayed,beforePart),investmentCurrentFill->getHeight());}if(investmentPreviewFill){int displayed=std::max(1,investmentPreviewFill->getParent()->getWidth()-10);investmentPreviewFill->setSize(GuildResponsive::scaledFill(displayed,afterPart),investmentPreviewFill->getHeight());}
}
