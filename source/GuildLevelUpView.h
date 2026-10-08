#include "Localization.h"
#pragma once
// Presentation state only. Nothing is serialized or applied to progression.
std::vector<GuildLevelUI::Transition> guildLevelQueue;
MyGUI::Widget* guildLevelCardsCanvas=0;
int guildLevelCardTop=0;
float guildLevelScale=1;
const MyGUI::Colour levelGold(1.0f,.66f,.30f),levelIvory(.89f,.86f,.78f);
int lu(int n){return (int)(n*guildLevelScale+.5f);}
std::string levelLang(const char* fr,const char* en){return gMercenarieEnglish?en:fr;}
MyGUI::TextBox* levelText(MyGUI::Widget* parent,int x,int y,int w,int h,int size,const std::string& value,const MyGUI::Colour& color,bool center=false){
    MyGUI::TextBox* t=parent->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",x,y,w,h,MyGUI::Align::Default);
    MercenarieFonts::caption(t,value);t->setFontHeight(std::max(12,lu(size)));t->setTextColour(color);t->setTextAlign(center?MyGUI::Align::Center:MyGUI::Align::Left|MyGUI::Align::Top);t->setNeedMouseFocus(false);return t;
}
MyGUI::ImageBox* levelImage(MyGUI::Widget* parent,const std::string& texture,int x,int y,int w,int h,const MyGUI::Colour& tint){
    MyGUI::ImageBox* image=parent->createWidget<MyGUI::ImageBox>("ImageBox",x,y,w,h,MyGUI::Align::Default);image->setImageTexture(texture);image->setColour(tint);image->setNeedMouseFocus(false);return image;
}
void levelLine(MyGUI::Widget* parent,int x,int y,int w,const MyGUI::Colour& color){MyGUI::Widget* line=parent->createWidget<MyGUI::Widget>("WhiteSkin",x,y,w,std::max(1,lu(1)),MyGUI::Align::Default);line->setColour(color);line->setNeedMouseFocus(false);}
void levelBorder(MyGUI::Widget* p,int w,int h,const MyGUI::Colour& c){levelLine(p,0,0,w,c);levelLine(p,0,h-1,w,c);for(int x=0;x<=w-1;x+=std::max(1,w-1)){MyGUI::Widget* edge=p->createWidget<MyGUI::Widget>("WhiteSkin",x,0,1,h,MyGUI::Align::Default);edge->setColour(c);edge->setNeedMouseFocus(false);}}
void guildLevelScroll(MyGUI::ScrollBar*,size_t value){if(guildLevelCardsCanvas)guildLevelCardsCanvas->setPosition(0,-(int)value);}
void levelButtonFocus(MyGUI::Widget* button,MyGUI::Widget*){button->setColour(MyGUI::Colour(1,.85f,.62f));}
void levelButtonBlur(MyGUI::Widget* button,MyGUI::Widget*){button->setColour(MyGUI::Colour(1,1,1));}
void updateGuildLevelUpUI();
void guildLevelUpClosed(MyGUI::WidgetPtr){
    if(!guildLevelUpWindow||!guildLevelUpWindow->getVisible())return;
    guildLevelUpWindow->setVisible(false);
    if(!guildLevelQueue.empty())guildLevelQueue.erase(guildLevelQueue.begin());
    if(guildLevelQueue.empty()&&ou&&!guildLevelUpWasPaused)ou->userPause(false);
}
void guildLevelUpWindowPressed(MyGUI::Window*,const std::string&){guildLevelUpClosed(0);}
void buildGuildLevelUp(int oldLevel,int newLevel){
    if(guildLevelUpWindow){mercenarieDestroyLiveWidget(guildLevelUpWindow);guildLevelUpWindow=0;guildLevelUpText=0;guildLevelUpClose=0;guildLevelCardsCanvas=0;}
    const std::vector<GuildLevelUI::Unlock> cards=GuildLevelUI::unlocks(newLevel);
    const MyGUI::IntSize& screen=MyGUI::RenderManager::getInstance().getViewSize();GuildLevelUI::Layout layout(screen.width,screen.height,(int)cards.size());guildLevelScale=layout.scale;
    MyGUI::ResourceManager::getInstance().load("ContractBoardV6.xml");
    guildLevelUpWindow=MyGUI::Gui::getInstancePtr()->createWidget<MyGUI::Window>("ContractBoardWindowV6",(screen.width-layout.width)/2,(screen.height-layout.height)/2,layout.width,layout.height,MyGUI::Align::Default,"Window","MercenarieGuildLevelUp");
    guildLevelUpWindow->eventWindowButtonPressed+=MyGUI::newDelegate(guildLevelUpWindowPressed);
    MyGUI::Widget* parent=guildLevelUpWindow->getClientWidget();
    levelImage(parent,"GuildLevelBackground.png",0,0,layout.width,layout.height,MyGUI::Colour(1,1,1));
    levelImage(parent,"LevelSun.png",lu(46),lu(26),lu(86),lu(86),levelIvory);
    levelText(parent,lu(166),lu(25),lu(1060),lu(57),45,Loc::text("ui.the_mercenarie"),levelIvory);
    levelText(parent,lu(169),lu(84),lu(1050),lu(30),19,levelLang(Loc::text("ui.contracts_a_guild_a_lawless_world_285cea2"),Loc::text("ui.contracts_a_guild_a_lawless_world_285cea2")),MyGUI::Colour(.65f,.65f,.61f));
    MyGUI::Button* close=parent->createWidget<MyGUI::Button>("Kenshi_Button1",layout.width-lu(83),lu(29),lu(48),lu(48),MyGUI::Align::Default);MercenarieFonts::caption(close,"X");close->eventMouseButtonClick+=MyGUI::newDelegate(guildLevelUpClosed);
    levelLine(parent,lu(25),lu(137),layout.width-lu(50),MyGUI::Colour(.60f,.40f,.20f));
    levelText(parent,lu(70),lu(158),layout.width-lu(140),lu(63),43,levelLang(Loc::text("ui.guild_level_up"),Loc::text("ui.guild_level_up")),levelGold,true);
    levelLine(parent,lu(385),lu(226),lu(610),MyGUI::Colour(.6f,.41f,.23f));
    std::ostringstream levelNumber;levelNumber<<newLevel;std::string announcement=Loc::named("guild.level.announcement","level",levelNumber.str());
    guildLevelUpText=levelText(parent,lu(80),lu(241),layout.width-lu(160),lu(38),24,announcement,levelIvory,true);
    int badgeSize=lu(186),badgeY=lu(282);int oldX=layout.width/2-lu(236),newX=layout.width/2+lu(50);
    for(int i=0;i<2;++i){int x=i?newX:oldX,level=i?newLevel:oldLevel;std::ostringstream asset;asset<<"GuildLevel"<<level<<".png";
        const MyGUI::Colour color=i?levelGold:MyGUI::Colour(.52f,.53f,.53f);
        levelImage(parent,asset.str(),x,badgeY,badgeSize,badgeSize,color);
        levelText(parent,x+lu(49),badgeY+lu(63),lu(88),lu(26),19,levelLang(Loc::text("ui.level_33337d2"),Loc::text("ui.level_33337d2")),i?levelGold:levelIvory,true);
        std::ostringstream number;number<<level;levelText(parent,x+lu(46),badgeY+lu(86),lu(94),lu(72),59,number.str(),i?levelGold:MyGUI::Colour(.7f,.7f,.67f),true);
    }
    levelText(parent,layout.width/2-lu(52),lu(341),lu(104),lu(69),56,">>>",levelGold,true);
    levelText(parent,lu(310),lu(446),layout.width-lu(620),lu(38),25,levelLang(Loc::text("ui.newly_unlocked_benefits"),Loc::text("ui.newly_unlocked_benefits")),levelIvory,true);
    levelLine(parent,lu(70),lu(465),lu(230),levelGold);levelLine(parent,layout.width-lu(300),lu(465),lu(230),levelGold);
    const int areaW=layout.width-lu(180),contentH=std::max(1,(int)cards.size())*layout.cardHeight;
    MyGUI::Widget* clip=parent->createWidget<MyGUI::Widget>("PanelEmpty",lu(90),layout.areaY,areaW,layout.areaHeight,MyGUI::Align::Default);
    guildLevelCardsCanvas=clip->createWidget<MyGUI::Widget>("PanelEmpty",0,0,areaW-lu(22),contentH,MyGUI::Align::Default);
    const int cardW=areaW-lu(22);
    if(cards.empty())levelText(guildLevelCardsCanvas,lu(28),lu(32),cardW-lu(56),lu(80),23,levelLang(Loc::text("ui.your_guild_grows_in_standing_no_new_feature_unlocks"),Loc::text("ui.your_guild_grows_in_standing_no_new_feature_unlocks")),levelIvory,true);
    for(size_t i=0;i<cards.size();++i){const GuildLevelUI::Unlock& card=cards[i];
        MyGUI::Widget* panel=guildLevelCardsCanvas->createWidget<MyGUI::Widget>("WhiteSkin",0,(int)i*layout.cardHeight,cardW,layout.cardHeight-lu(12),MyGUI::Align::Default);panel->setColour(MyGUI::Colour(.065f,.065f,.065f));levelBorder(panel,cardW,layout.cardHeight-lu(12),MyGUI::Colour(.44f,.44f,.42f));
        levelImage(panel,card.icon,lu(30),lu(12),lu(106),lu(106),levelIvory);
        MyGUI::Widget* divider=panel->createWidget<MyGUI::Widget>("WhiteSkin",lu(166),1,1,layout.cardHeight-lu(14),MyGUI::Align::Default);divider->setColour(MyGUI::Colour(.28f,.28f,.27f));
        levelText(panel,lu(194),lu(12),cardW-lu(214),lu(38),27,levelLang(card.fr,card.en),levelGold);
        levelText(panel,lu(194),lu(54),cardW-lu(214),lu(36),21,levelLang(card.detailFr,card.detailEn),levelIvory);
        levelText(panel,lu(194),lu(93),cardW-lu(214),lu(30),18,levelLang(card.noteFr,card.noteEn),MyGUI::Colour(.62f,.62f,.60f));
    }
    if(contentH>layout.areaHeight){MyGUI::ScrollBar* scroll=parent->createWidget<MyGUI::ScrollBar>("Kenshi_ScrollBarV",layout.width-lu(106),layout.areaY,lu(20),layout.areaHeight,MyGUI::Align::Default);scroll->setScrollRange(contentH-layout.areaHeight+1);scroll->setScrollPage(std::max(1,lu(40)));scroll->setScrollPosition(0);scroll->eventScrollChangePosition+=MyGUI::newDelegate(guildLevelScroll);}
    levelText(parent,lu(80),layout.quoteY,layout.width-lu(160),lu(36),20,levelLang(Loc::text("ui.your_reputation_grows_and_new_opportunities_await_you"),Loc::text("ui.your_reputation_grows_and_new_opportunities_await_you")),MyGUI::Colour(.69f,.69f,.65f),true);
    levelImage(parent,"LevelSun.png",layout.width/2-lu(15),layout.buttonY-lu(40),lu(30),lu(30),levelGold);
    guildLevelUpClose=parent->createWidget<MyGUI::Button>("Kenshi_Button1",layout.width/2-lu(245),layout.buttonY,lu(490),lu(56),MyGUI::Align::Default);MercenarieFonts::caption(guildLevelUpClose,levelLang(Loc::text("common.continue"),Loc::text("common.continue")));guildLevelUpClose->setFontHeight(std::max(12,lu(29)));guildLevelUpClose->setTextColour(levelGold);levelBorder(guildLevelUpClose,lu(490),lu(56),levelGold);guildLevelUpClose->eventMouseButtonClick+=MyGUI::newDelegate(guildLevelUpClosed);guildLevelUpClose->eventMouseSetFocus+=MyGUI::newDelegate(levelButtonFocus);guildLevelUpClose->eventMouseLostFocus+=MyGUI::newDelegate(levelButtonBlur);
    guildLevelUpWindow->setVisible(true);
}
void updateGuildLevelUpUI(){
    if(guildLevelQueue.empty()||(guildLevelUpWindow&&guildLevelUpWindow->getVisible())||!MyGUI::Gui::getInstancePtr())return;
    if(finalWindow&&finalWindow->getVisible())return;
    if(ou)ou->userPause(true);
    buildGuildLevelUp(guildLevelQueue[0].oldLevel,guildLevelQueue[0].newLevel);
}
void showGuildLevelUpWindow(int oldLevel,int newLevel){
    if(newLevel<=oldLevel)return;
    if(guildLevelQueue.empty())guildLevelUpWasPaused=ou?ou->isPaused():false;
    GuildLevelUI::enqueue(guildLevelQueue,oldLevel,newLevel);updateGuildLevelUpUI();
}
