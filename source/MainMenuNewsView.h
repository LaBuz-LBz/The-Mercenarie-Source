// Visual composition only. Included inside MainMenuNews; lifecycle stays in MainMenuNews.h.
inline int px(int value,const MainMenuNewsRules::Layout& l){return value*l.scale/100;}
inline MyGUI::TextBox* newsText(MyGUI::Widget* parent,int x,int y,int w,int h,int size,const char* caption,const MyGUI::Colour& colour,const char* font="MercenarieNewsBody"){
    MyGUI::TextBox* t=text(parent,x,y,w,h,size,caption,colour);t->setFontName(MercenarieFonts::newsFont(font));t->setFontHeight(size);return t;
}
inline void createCards(MyGUI::Widget* parent,const MainMenuNewsRules::Layout& l){
    const int gap=l.cardGap,cardW=(l.content.w-gap)/2;
    const bool compact=selectedVersion!=0;const int rows=3,cardGap=gap,cardH=(l.content.h-cardGap*(rows-1))/rows;
    for(int i=0;i<pageItemCount();++i){
        // Reference order is column-major: contracts/delegation/finance on left.
        int col=compact?i%2:i/3,row=compact?i/2:i%3,x=col*(cardW+gap),y=row*(cardH+cardGap);
        MyGUI::Button* card=parent->createWidget<MyGUI::Button>("MercenarieNewsCard",x,y,cardW,cardH,MyGUI::Align::Default,std::string("MercenarieNewsCard")+char('0'+i));
        MercenarieFonts::caption(card,"");card->setUserString("newsIndex",std::string(1,char('0'+i)));
        card->eventMouseSetFocus+=MyGUI::newDelegate(showTooltip);card->eventMouseLostFocus+=MyGUI::newDelegate(hideTooltip);
        border(card,MyGUI::Colour(.30f,.23f,.15f));
        MyGUI::ImageBox* icon=card->createWidget<MyGUI::ImageBox>("ImageBox",px(12,l),px(22,l),px(64,l),px(64,l),MyGUI::Align::Default);
        icon->setImageTexture("MercenarieNewsIcons.png");icon->setImageCoord(MyGUI::IntCoord(pageItems()[i].icon*96,0,96,96));icon->setNeedMouseFocus(false);
        MyGUI::TextBox* title=newsText(card,px(88,l),px(17,l),cardW-px(100,l),px(28,l),px(21,l),Loc::text(pageItems()[i].title),MyGUI::Colour(.94f,.51f,.17f),"MercenarieNewsHeading");title->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
        MyGUI::EditBox* body=wrappedText(card,px(88,l),px(47,l),cardW-px(102,l),cardH-px(52,l),px(18,l),Loc::text(pageItems()[i].shortText),MyGUI::Colour(.80f,.75f,.65f));body->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    }
}

inline void createModal(){
    ensureResources();const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();lastWidth=view.width;lastHeight=view.height;MainMenuNewsRules::Layout l=MainMenuNewsRules::calculate(view.width,view.height);
    modalRoot=MyGUI::Gui::getInstance().createWidget<MyGUI::Widget>("WhiteSkin",0,0,view.width,view.height,MyGUI::Align::Stretch,"Popup","MercenarieNewsModal");modalRoot->setColour(MyGUI::Colour(0,0,0));modalRoot->setAlpha(.72f);modalRoot->setNeedMouseFocus(true);modalRoot->setInheritsPick(true);
    popup=modalRoot->createWidget<MyGUI::Widget>("MercenarieNewsPanel",l.popup.x,l.popup.y,l.popup.w,l.popup.h,MyGUI::Align::Default,"MercenarieNewsPopup");popup->setInheritsAlpha(false);popup->setAlpha(1);border(popup,MyGUI::Colour(.47f,.30f,.15f));
    // Warm material backdrop; native widgets above remain independent and interactive.
    MyGUI::ImageBox* backdrop=popup->createWidget<MyGUI::ImageBox>("ImageBox",0,0,l.popup.w,l.popup.h,MyGUI::Align::Stretch);backdrop->setImageTexture("GuildSidebarBackdrop.png");backdrop->setAlpha(.12f);backdrop->setNeedMouseFocus(false);
    summaryPanel=popup->createWidget<MyGUI::Widget>("PanelEmpty",0,0,l.popup.w,l.popup.h,MyGUI::Align::Default,"MercenarieNewsSummary");summaryPanel->setNeedMouseFocus(false);summaryPanel->setInheritsPick(true);
    MyGUI::ImageBox* banner=summaryPanel->createWidget<MyGUI::ImageBox>("ImageBox",l.banner.x,l.banner.y,l.banner.w,l.banner.h,MyGUI::Align::Default);banner->setImageTexture("MercenarieNewsBanner.png");banner->setNeedMouseFocus(false);border(banner,MyGUI::Colour(.38f,.27f,.16f));
    MyGUI::Widget* header=popup->createWidget<MyGUI::Widget>("MercenarieNewsHeader",l.header.x,l.header.y,l.header.w,l.header.h,MyGUI::Align::Default);header->setNeedMouseFocus(false);
    MyGUI::TextBox* brand=newsText(header,px(18,l),px(9,l),px(510,l),px(82,l),px(66,l),"THE MERCENARIE",MyGUI::Colour(.91f,.85f,.72f),"MercenarieNewsTitle");brand->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
    MyGUI::TextBox* version=newsText(header,px(550,l),px(0,l),px(178,l),px(100,l),px(92,l),pageVersion(),MyGUI::Colour(.96f,.44f,.12f),"MercenarieNewsTitle");version->setTextAlign(MyGUI::Align::Center);
    MyGUI::Widget* ribbon=header->createWidget<MyGUI::Widget>("WhiteSkin",px(18,l),px(102,l),px(506,l),px(28,l),MyGUI::Align::Default);ribbon->setColour(MyGUI::Colour(.10f,.085f,.06f));ribbon->setNeedMouseFocus(false);border(ribbon,MyGUI::Colour(.32f,.23f,.13f));
    MyGUI::TextBox* subtitle=newsText(ribbon,0,0,ribbon->getWidth(),ribbon->getHeight(),px(20,l),Loc::text("news.subtitle"),MyGUI::Colour(.84f,.77f,.64f),"MercenarieNewsHeading");subtitle->setTextAlign(MyGUI::Align::Center);
    MyGUI::TextBox* major=newsText(header,px(538,l),px(106,l),px(214,l),px(26,l),px(18,l),Loc::text("news.major_update"),MyGUI::Colour(.78f,.68f,.50f),"MercenarieNewsHeading");major->setTextAlign(MyGUI::Align::Center);
    MyGUI::Button* previous=popup->createWidget<MyGUI::Button>("MercenarieNewsSecondary",l.content.x,px(175,l),px(200,l),px(30,l),MyGUI::Align::Default,"MercenarieNewsPrevious");
    MercenarieFonts::caption(previous,navigationCaption(-1));previous->setFontHeight(px(16,l));previous->setEnabled(selectedVersion>0);previous->eventMouseButtonClick+=MyGUI::newDelegate(previousVersion);
    MyGUI::Button* next=popup->createWidget<MyGUI::Button>("MercenarieNewsSecondary",l.content.x+l.content.w-px(200,l),px(175,l),px(200,l),px(30,l),MyGUI::Align::Default,"MercenarieNewsNext");
    MercenarieFonts::caption(next,navigationCaption(1));next->setFontHeight(px(16,l));next->setEnabled(selectedVersion+1<MainMenuNewsRules::versionCount());next->eventMouseButtonClick+=MyGUI::newDelegate(nextVersion);
    MyGUI::TextBox* page=newsText(popup,l.content.x+px(220,l),px(175,l),l.content.w-px(440,l),px(30,l),px(18,l),pageVersion(),MyGUI::Colour(.94f,.51f,.17f));page->setTextAlign(MyGUI::Align::Center);
    MyGUI::Widget* cards=summaryPanel->createWidget<MyGUI::Widget>("PanelEmpty",l.content.x,l.content.y,l.content.w,l.content.h,MyGUI::Align::Default);createCards(cards,l);
    MyGUI::Widget* footer=popup->createWidget<MyGUI::Widget>("MercenarieNewsHeader",0,px(584,l),l.popup.w,px(76,l),MyGUI::Align::Default);footer->setNeedMouseFocus(false);
    dismissCheck=popup->createWidget<MyGUI::Button>("TheMercenarie_Checkbox",l.check.x,l.check.y,l.check.w,l.check.h,MyGUI::Align::Default,"MercenarieNewsDismissCheck");dismissCheck->setStateSelected(false);dismissCheck->eventMouseButtonClick+=MyGUI::newDelegate(checkPressed);border(dismissCheck,MyGUI::Colour(.68f,.49f,.28f));
    std::string checkCaption=Loc::text("news.dismiss_version");size_t marker=checkCaption.find("{version}");if(marker!=std::string::npos)checkCaption.replace(marker,9,MainMenuNewsRules::CurrentNewsVersion);
    MyGUI::TextBox* checkText=newsText(popup,px(76,l),l.footer.y,px(508,l),l.footer.h,px(16,l),checkCaption.c_str(),MyGUI::Colour(.84f,.79f,.68f));checkText->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
    changelogButton=popup->createWidget<MyGUI::Button>("MercenarieNewsSecondary",l.changelog.x,l.changelog.y,l.changelog.w,l.changelog.h,MyGUI::Align::Default,"MercenarieNewsChangelogButton");MercenarieFonts::caption(changelogButton,Loc::text("news.full_changelog"));changelogButton->setFontHeight(px(17,l));changelogButton->eventMouseButtonClick+=MyGUI::newDelegate(showChangelog);border(changelogButton,MyGUI::Colour(.38f,.30f,.21f));
    continueButton=popup->createWidget<MyGUI::Button>("MercenarieNewsPrimary",l.proceed.x,l.proceed.y,l.proceed.w,l.proceed.h,MyGUI::Align::Default,"MercenarieNewsContinueButton");MercenarieFonts::caption(continueButton,Loc::text("news.continue"));continueButton->setFontHeight(px(23,l));continueButton->eventMouseButtonClick+=MyGUI::newDelegate(closePressed);
    changelogPanel=popup->createWidget<MyGUI::Widget>("MercenarieNewsPanel",px(24,l),px(210,l),l.popup.w-px(48,l),px(362,l),MyGUI::Align::Default,"MercenarieNewsChangelog");changelogPanel->setVisible(false);border(changelogPanel,MyGUI::Colour(.45f,.29f,.12f));
    MyGUI::TextBox* changeTitle=newsText(changelogPanel,px(24,l),px(12,l),changelogPanel->getWidth()-px(48,l),px(42,l),px(28,l),Loc::text(pageChangelogTitle()),MyGUI::Colour(.94f,.52f,.12f),"MercenarieNewsHeading");changeTitle->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
    MyGUI::ScrollView* scroll=changelogPanel->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",px(22,l),px(64,l),changelogPanel->getWidth()-px(44,l),changelogPanel->getHeight()-px(82,l),MyGUI::Align::Default);MercenarieNativeInput::bind(scroll);scroll->setVisibleHScroll(false);scroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);scroll->setCanvasSize(scroll->getWidth()-24,selectedVersion==0?760:1200);
    MyGUI::EditBox* changeBody=wrappedText(scroll,18,10,scroll->getWidth()-60,selectedVersion==0?730:1170,18,Loc::text(pageChangelog()),MyGUI::Colour(.90f,.87f,.78f));changeBody->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    returnButton=popup->createWidget<MyGUI::Button>("MercenarieNewsSecondary",l.proceed.x,l.proceed.y,l.proceed.w,l.proceed.h,MyGUI::Align::Default,"MercenarieNewsReturnButton");MercenarieFonts::caption(returnButton,Loc::text("news.back"));returnButton->setFontHeight(px(21,l));returnButton->eventMouseButtonClick+=MyGUI::newDelegate(showSummary);returnButton->setVisible(false);
    MyGUI::Button* close=popup->createWidget<MyGUI::Button>("MercenarieNewsSecondary",l.close.x,l.close.y,l.close.w,l.close.h,MyGUI::Align::Default,"MercenarieNewsClose");MercenarieFonts::caption(close,"X");close->setFontHeight(px(30,l));close->eventMouseButtonClick+=MyGUI::newDelegate(closePressed);
    MyGUI::InputManager::getInstance().addWidgetModal(modalRoot);
    DebugLog("Mercenarie news: popup created on Popup layer; native input modal active");
}
