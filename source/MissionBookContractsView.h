// Presentation only; the persistent offers, route and delegation rules remain shared.
// Book-only surfaces are siblings of the legacy information panel, never its children.
struct MissionBookOriginalWidget { MyGUI::Widget* widget;MyGUI::IntCoord coord;MissionBookOriginalWidget(MyGUI::Widget* w):widget(w),coord(w->getCoord()){} };
std::vector<MissionBookOriginalWidget> missionBookOriginalWidgets;
struct MissionBookOfferWidgets {MyGUI::Widget* row;MyGUI::Button* button;MyGUI::Button* reroll;MyGUI::ImageBox* icon;MyGUI::TextBox *name,*type,*duration,*price,*rerollBadge;MyGUI::Widget* edges[4];};
std::vector<MissionBookOfferWidgets> missionBookOfferRows;
MyGUI::Widget *missionBookDetail=0,*missionBookHeader=0,*missionBookBackground=0;
MyGUI::ScrollView *missionBookRosterScroll=0,*missionBookTeamScroll=0,*missionBookDetailScroll=0;
MyGUI::TextBox *missionBookAvailable=0,*missionBookOffersTitle=0,*missionBookDelegationTitle=0,*missionBookName=0,*missionBookType=0,*missionBookPrice=0,*missionBookDuration=0,*missionBookStory=0,*missionBookObjectives=0,*missionBookFacts=0,*missionBookEmpty=0;
MyGUI::ImageBox* missionBookStatsHeaders[3]={0};
MyGUI::ImageBox *missionBookCategoryIcon=0,*missionBookDelegateIcon=0,*missionBookSummaryIcons[4]={0};
MyGUI::Widget *missionBookSummaryRules[3]={0};
MyGUI::TextBox *missionBookTakeSubtitle=0,*missionBookDelegateSubtitle=0,*missionBookStatTooltip=0;
MyGUI::TextBox *missionBookObjectivesTitle=0,*missionBookFactsTitle=0;
MyGUI::TextBox *missionBookFactLabels[7]={0},*missionBookFactValues[7]={0};
// All new widgets belong to the existing window; no independent lifetime.
MyGUI::ImageBox* bookKitIcon(MyGUI::Widget* parent,int slot,const MyGUI::Colour& colour){
 MyGUI::ImageBox* icon=parent->createWidget<MyGUI::ImageBox>("ImageBox",0,0,24,24,MyGUI::Align::Default);
 icon->setImageTexture("TheMercenarieBookIcons.png");icon->setImageCoord(MyGUI::IntCoord(slot*64,0,64,64));icon->setColour(colour);icon->setNeedMouseFocus(false);return icon;
}
MyGUI::Colour bookCategoryColour(int i){return i==1?MyGUI::Colour(.15f,.85f,.25f):i==2?MyGUI::Colour(.96f,.12f,.22f):i==4?registerIvory:registerAmber;}
void bookStatTooltip(MyGUI::Widget* sender,const MyGUI::ToolTipInfo& info){
 if(!missionBookStatTooltip)return;
 MercenarieFonts::caption(missionBookStatTooltip,sender->getUserString("bookTooltip"));missionBookStatTooltip->setVisible(info.type!=MyGUI::ToolTipInfo::Hide);
}
void bookActionCaption(MyGUI::Button* button,MyGUI::TextBox* subtitle,const char* key){
 std::string text=button->getCaption().asUTF8();size_t newline=text.find('\n');
 std::string full=Loc::text(key);size_t split=full.find('\n');
 if(newline!=std::string::npos)MercenarieFonts::caption(button,text.substr(0,newline));
 MercenarieFonts::caption(subtitle,split==std::string::npos?"":full.substr(split+1));
 subtitle->setVisible(text!=Loc::text("options.confirm"));
 int font=subtitle->getFontHeight()+6;button->setFontHeight(font);button->setTextAlign(MyGUI::Align::Center);
 while(font>subtitle->getFontHeight()&&button->getTextSize().width>button->getWidth()-84)button->setFontHeight(--font);

}
void refreshMissionBookVisualDetails(){
 if(!missionBookCategoryIcon)return;
 int selected=std::max(0,std::min(4,(int)missionBookCategoryCombo->getIndexSelected()));
 missionBookCategoryIcon->setImageCoord(MyGUI::IntCoord(selected*64,0,64,64));missionBookCategoryIcon->setColour(bookCategoryColour(selected));
 // MyGUI recycles list widgets: resolve them anew, never retain a row pointer.
 MyGUI::Widget* popup=missionBookCategoryCombo->findWidget("List");
 MyGUI::ListBox* list=popup?popup->castType<MyGUI::ListBox>(false):0;
 if(list&&list->getVisible())for(size_t i=0;i<list->getItemCount();++i){
     if(!list->isItemVisibleAt(i))continue;MyGUI::Widget* row=list->getWidgetByIndex(i);if(!row)continue;
     MyGUI::Widget* child=row->findWidget("BookCategoryIcon");
     MyGUI::ImageBox* icon=child?child->castType<MyGUI::ImageBox>(false):0;
     if(!icon){icon=row->createWidget<MyGUI::ImageBox>("ImageBox",6,5,26,26,MyGUI::Align::Default,"BookCategoryIcon");icon->setImageTexture("TheMercenarieBookIcons.png");icon->setNeedMouseFocus(false);}
     icon->setCoord(6,(row->getHeight()-26)/2,26,26);icon->setImageCoord(MyGUI::IntCoord((int)i*64,0,64,64));icon->setColour(bookCategoryColour((int)i));icon->setVisible(true);
 }
 bookActionCaption(contractsAccept,missionBookTakeSubtitle,"missionbook.take");bookActionCaption(missionBookDelegateButton,missionBookDelegateSubtitle,"missionbook.delegate");
}
MyGUI::TextBox* missionBookSummaryLabels[4]={0};
MyGUI::TextBox* missionBookSummaryValues[4]={0};
MyGUI::ImageBox* missionBookDetailIcon=0;
MyGUI::ImageBox* missionBookTakeIcon=0;
MyGUI::Button* missionBookPageArrow=0;
std::vector<MyGUI::ImageBox*> missionBookTeamPortraits;
MyGUI::Button *missionBookPriceButton=0,*missionBookCloseButton=0;
bool missionBookPresentationActive=false;
int missionBookViewportW=0,missionBookViewportH=0;
MissionBookLayout::Layout missionBookLayout;

// Measure with the actual Kenshi font. Preserve whole UTF-8 characters when
// truncating; geometry clips the label without ever invading the next column.
void bookPopCharacter(std::string& text){if(text.empty())return;size_t i=text.size()-1;while(i>0&&((unsigned char)text[i]&0xC0)==0x80)--i;text.erase(i);}
void bookEllipsis(MyGUI::TextBox* label){
    std::string text=label->getCaption().asUTF8();
    if(label->getTextSize().width<=label->getWidth()-2)return;
    do{bookPopCharacter(text);MercenarieFonts::caption(label,text+"...");}while(!text.empty()&&label->getTextSize().width>label->getWidth()-2);
}
void bookWrap(MyGUI::TextBox* label,const std::string& text){
    std::istringstream paragraphs(text);std::string paragraph,result;
    while(std::getline(paragraphs,paragraph)){
        std::istringstream words(paragraph);std::string word,line;
        while(words>>word){std::string candidate=line.empty()?word:line+" "+word;MercenarieFonts::caption(label,candidate);
            if(!line.empty()&&label->getTextSize().width>label->getWidth()-4){result+=line+"\n";line=word;}else line=candidate;
        }
        result+=line+"\n";
    }
    if(!result.empty()&&result[result.size()-1]=='\n')result.erase(result.size()-1);
    MercenarieFonts::caption(label,result);
}

void rememberMissionBookGeometry(MyGUI::Widget* w){
    missionBookOriginalWidgets.push_back(MissionBookOriginalWidget(w));
    for(size_t i=0;i<w->getChildCount();++i)rememberMissionBookGeometry(w->getChildAt(i));
}
void bookCoord(MyGUI::Widget* w,const MissionBookLayout::Rect& r){w->setCoord(r.x,r.y,r.w,r.h);}
void bookPanelCoord(MyGUI::Widget* w,const MissionBookLayout::Rect& r){
    bookCoord(w,r);
    // boardPanelV6 owns four thin edge widgets before its content.
    w->getChildAt(0)->setCoord(0,0,r.w,1);w->getChildAt(1)->setCoord(0,r.h-1,r.w,1);
    w->getChildAt(2)->setCoord(0,0,1,r.h);w->getChildAt(3)->setCoord(r.w-1,0,1,r.h);
}
MyGUI::Widget* bookRerollTip=0;
MyGUI::TextBox* bookRerollTipText=0;
void bookRerollTooltip(MyGUI::Widget* sender,const MyGUI::ToolTipInfo& info){
    if(info.type==MyGUI::ToolTipInfo::Hide){if(bookRerollTip)bookRerollTip->setVisible(false);return;}
    if(!contractsWindow||!contractsWindow->getVisible())return;
    const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();int w=std::min(650,view.width-32),h=190;
    if(!bookRerollTip){bookRerollTip=MyGUI::Gui::getInstance().createWidget<MyGUI::Widget>("Kenshi_GenericTextBoxFlatSkin",0,0,w,h,MyGUI::Align::Default,"Popup");bookRerollTip->setNeedMouseFocus(false);bookRerollTipText=registerText(bookRerollTip,12,10,w-24,h-20,19,"",registerIvory);}
    const MyGUI::IntPoint mouse=MyGUI::InputManager::getInstance().getMousePosition();
    bookRerollTip->setCoord(std::max(8,std::min(mouse.left+16,view.width-w-8)),std::max(8,std::min(mouse.top+20,view.height-h-8)),w,h);
    std::string text=rerollTooltip();if(sender->getUserString("rerolled")=="1")text=std::string(Loc::text("v9.reroll.indicator"))+"\n"+text;
    bookWrap(bookRerollTipText,text);bookRerollTip->setVisible(true);
}
std::string bookDuration(double hours){
    int h=std::max(0,(int)ceil(hours));std::ostringstream s;
    if(h>=24)s<<h/24<<(Loc::text("ui.d_f380c84"));s<<h%24<<Loc::text("ui.h");return s.str();
}
DelegatedMissionTiming::State bookTiming(const BoardOffer& offer){
    DelegatedMissionTiming::ActivityType a=missionBookTab==1?DelegatedMissionTiming::ActivityBountyHunt:offer.missionType==MCT_ESCORT?DelegatedMissionTiming::ActivityEscort:offer.missionType==MCT_CARAVAN?DelegatedMissionTiming::ActivityCaravan:offer.missionType==MCT_MAIL?DelegatedMissionTiming::ActivityMailDelivery:DelegatedMissionTiming::ActivityScientificExpedition;
    double km=std::max(0.0f,offer.distance)*(missionBookTab==1||offer.missionType!=MCT_MAIL?(missionBookTab==1||offer.missionType!=MCT_ESCORT?2.0:1.0):1.0)/1000.0;
    return DelegatedMissionTiming::start(a,km,currentGameHours);
}
bool missionBookViewportChanged(){const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();return v.width!=missionBookViewportW||v.height!=missionBookViewportH;}
void missionBookPageArrowClicked(MyGUI::WidgetPtr){missionBookPageClicked(missionBookPageNext);}

void layoutMissionBookRoster(){
    if(!missionBookRosterScroll)return;
    const MissionBookLayout::Layout& l=missionBookLayout;
    int width=l.roster.w-22,h=l.rowH,font=l.font;
    while(missionBookSoldierRows.size()<delegationRosterCharacters.size()){
        MissionBookSoldierWidgets sw;
        sw.row=missionBookRosterScroll->createWidget<MyGUI::Button>("TheMercenarie_ListItem",0,0,width,h,MyGUI::Align::Default);
        sw.row->eventMouseButtonClick+=MyGUI::newDelegate(delegationRosterToggle);
        sw.portrait=sw.row->createWidget<MyGUI::ImageBox>("ImageBox",4,3,h-6,h-6,MyGUI::Align::Default);sw.portrait->setNeedMouseFocus(false);
        sw.name=registerText(sw.row,0,0,1,1,font,"",registerIvory);
        sw.stats=registerText(sw.row,0,0,1,1,font,"",registerIvory);
        sw.defence=registerText(sw.row,0,0,1,1,font,"",registerIvory);
        sw.endurance=registerText(sw.row,0,0,1,1,font,"",registerIvory);
        sw.check=sw.row->createWidget<MyGUI::Button>("TheMercenarie_Checkbox",0,0,26,26,MyGUI::Align::Default);
        sw.check->eventMouseButtonClick+=MyGUI::newDelegate(delegationRosterToggle);
        missionBookSoldierRows.push_back(sw);
    }
    int statW=std::max(37,width*14/100),check=28,statsX=width-check-3*statW-8;
    for(size_t i=0;i<missionBookSoldierRows.size();++i){
        MissionBookSoldierWidgets& sw=missionBookSoldierRows[i];
        sw.row->setCoord(0,(int)i*(h+2),width,h);sw.portrait->setCoord(4,3,h-6,h-6);
        sw.name->setCoord(h+4,2,statsX-h-8,h-4);sw.name->setFontHeight(font);sw.name->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
        MyGUI::TextBox* stats[]={sw.stats,sw.defence,sw.endurance};
        for(int j=0;j<3;++j){stats[j]->setCoord(statsX+j*statW,2,statW-4,h-4);stats[j]->setFontHeight(font);stats[j]->setTextAlign(MyGUI::Align::Center);}
        sw.stats->setCoord(statsX,2,3*statW-8,h-4);sw.defence->setVisible(false);sw.endurance->setVisible(false);
        sw.check->setCoord(width-check-8,(h-26)/2,26,26);
    }
    for(int j=0;j<3;++j)missionBookStatsHeaders[j]->setCoord(l.roster.x+statsX+j*statW+(statW-28)/2,73,24,24);
    for(int j=0;j<3;++j)missionBookStatsHeaders[j]->setVisible(false);
    missionBookStatTooltip->setCoord(12,74,std::max(80,statsX-12),24);
    int canvasH=std::max(l.roster.h-2,(int)delegationRosterCharacters.size()*(h+2));
    missionBookRosterScroll->setCanvasSize(width,canvasH);missionBookRosterScroll->setVisibleVScroll(canvasH>l.roster.h);missionBookRosterScroll->setVisibleHScroll(false);
    MercenarieFonts::caption(missionBookAvailable,std::string(Loc::text("missionbook.soldiers"))+" ("+registerNumber(delegationAvailableCount())+")");bookEllipsis(missionBookAvailable);
}
void layoutMissionBookDelegationGeometry(bool hasTeam){
    const MyGUI::IntSize& viewport=MyGUI::RenderManager::getInstance().getViewSize();
    missionBookLayout=MissionBookLayout::calculate(viewport.width,viewport.height,hasTeam);
    const MissionBookLayout::Layout& l=missionBookLayout;
    missionBookDelegationTitle->setCoord(12,6,l.right.w-24,32);missionBookDelegationTitle->setFontHeight(l.font+4);
    missionBookAvailable->setCoord(12,44,l.right.w*56/100-18,28);missionBookAvailable->setFontHeight(l.font-1);
    missionBookSoldierSort->setCoord(l.right.w*56/100,40,l.right.w*44/100-12,34);missionBookSoldierSort->setFontHeight(l.font-1);
    bookCoord(missionBookRosterScroll,l.roster);
    missionBookSelectedTeam->setCoord(l.team.x,l.team.y,l.team.w,28);missionBookSelectedTeam->setFontHeight(l.font);
    missionBookTeamScroll->setCoord(l.team.x,l.team.y+28,l.team.w,l.portraitH+16);missionBookTeamScroll->setVisible(hasTeam);
    const char* summaryKeys[]={"missionbook.group","missionbook.travel","missionbook.activity","missionbook.total"};
    for(int i=0;i<4;++i){MercenarieFonts::caption(missionBookSummaryLabels[i],Loc::text(summaryKeys[i]));int y=l.summary.y+i*(l.font+6);missionBookSummaryIcons[i]->setCoord(12,y+1,20,20);missionBookSummaryLabels[i]->setCoord(40,y,l.summary.w*66/100-28,l.font+6);missionBookSummaryLabels[i]->setFontHeight(l.font);missionBookSummaryValues[i]->setCoord(12+l.summary.w*66/100,y,l.summary.w*34/100-8,l.font+6);missionBookSummaryValues[i]->setFontHeight(l.font);bookEllipsis(missionBookSummaryLabels[i]);if(i<3)missionBookSummaryRules[i]->setCoord(12,y+l.font+5,l.summary.w-4,1);}
    bookCoord(missionBookDelegationStatus,l.status);missionBookDelegationStatus->setFontHeight(l.font-1);
    bookCoord(missionBookDelegateButton,l.delegate);missionBookDelegateButton->setFontHeight(l.font+6);
    missionBookDelegateIcon->setCoord(12,(l.delegate.h-52)/2,52,52);
    missionBookDelegateButton->getChildAt(2)->setCoord(0,0,l.delegate.w,1);missionBookDelegateButton->getChildAt(3)->setCoord(0,l.delegate.h-1,l.delegate.w,1);missionBookDelegateButton->getChildAt(4)->setCoord(0,0,1,l.delegate.h);missionBookDelegateButton->getChildAt(5)->setCoord(l.delegate.w-1,0,1,l.delegate.h);
    missionBookDelegateSubtitle->setCoord(70,l.delegate.h/2+4,l.delegate.w-82,30);missionBookDelegateSubtitle->setFontHeight(l.font);bookEllipsis(missionBookDelegateSubtitle);
    layoutMissionBookRoster();
}
void refreshMissionBookTeamPortraits(){
    if(!missionBookTeamScroll)return;
    int selected=delegationSelectedCount();
    // Only presentation changes with selection: collapse the empty portrait strip
    // and return its space to the roster. The footer never moves.
    layoutMissionBookDelegationGeometry(selected>0);
    int size=missionBookLayout.portraitH;
    MercenarieFonts::caption(missionBookSelectedTeam,std::string(Loc::text("missionbook.team"))+" ("+registerNumber(selected)+")");
    while((int)missionBookTeamPortraits.size()<selected){
        MyGUI::ImageBox* image=missionBookTeamScroll->createWidget<MyGUI::ImageBox>("ImageBox",0,0,size,size,MyGUI::Align::Default);image->setNeedMouseFocus(false);missionBookTeamPortraits.push_back(image);
    }
    PortraitManager* manager=PortraitManager::getInstance();int slot=0;
    for(size_t i=0;i<delegationRosterCharacters.size();++i)if(delegationRosterSelected[i]){
        MyGUI::ImageBox* image=missionBookTeamPortraits[slot];image->setCoord(slot*(size+10),0,size,size);image->setVisible(true);
        if(manager&&delegationRosterCharacters[i])manager->setImageWidget(delegationRosterCharacters[i]->getHandle(),image,true);++slot;
    }
    for(size_t i=slot;i<missionBookTeamPortraits.size();++i)missionBookTeamPortraits[i]->setVisible(false);
    missionBookTeamScroll->setCanvasSize(std::max(missionBookLayout.team.w-20,slot*(size+10)),size);
    missionBookTeamScroll->setVisibleVScroll(false);missionBookTeamScroll->setVisibleHScroll(slot*(size+10)>missionBookLayout.team.w-20);
    MercenarieFonts::caption(missionBookSummaryValues[0],registerNumber(selected)+" / 30");
}
void createMissionBookPresentation(MyGUI::Widget* c){
    const MyGUI::IntSize& viewport=MyGUI::RenderManager::getInstance().getViewSize();missionBookLayout=MissionBookLayout::calculate(viewport.width,viewport.height);
    // Capture before adding book-only widgets. No widget is destroyed or reparented
    // when switching modes; restoring coordinates cannot dereference a freed child.
    rememberMissionBookGeometry(contractsWindow);
    missionBookBackground=c->getChildAt(0);missionBookHeader=contractsHeading->getParent();
    missionBookCloseButton=missionBookHeader->getChildAt(missionBookHeader->getChildCount()-1)->castType<MyGUI::Button>();
    missionBookOffersTitle=contractsListPanel->getChildAt(4)->castType<MyGUI::TextBox>();
    // The price button is bound at creation; map overlays can add siblings.
    missionBookDelegationPanel=c->createWidget<MyGUI::Widget>("TheMercenarie_Panel",0,0,10,10,MyGUI::Align::Default,"MissionBookDelegationPanel");
    missionBookDelegationTitle=registerText(missionBookDelegationPanel,12,8,100,34,23,Loc::text("missionbook.delegation"),registerAmber);
    missionBookAvailable=registerText(missionBookDelegationPanel,12,46,100,26,16,"",registerAmber);
    missionBookSoldierSort=missionBookDelegationPanel->createWidget<MyGUI::ComboBox>("TheMercenarie_Combo",0,0,180,34,MyGUI::Align::Default);missionBookSoldierSort->setComboModeDrop(true);missionBookSoldierSort->setFontHeight(15);
    const char* fr[]={"Nom","Attaque","Defense","Endurance"};const char* en[]={"Name","Attack","Defence","Endurance"};
    for(int i=0;i<4;++i)missionBookSoldierSort->addItem(std::string(Loc::text("missionbook.sort"))+(gMercenarieEnglish?en[i]:fr[i]));
    missionBookSoldierSort->setIndexSelected(0);missionBookSoldierSort->eventComboChangePosition+=MyGUI::newDelegate(missionBookSoldierSortChanged);
    const int statSlots[]={5,4,6}; // crossed swords, shield, BREASTPLATE (native toughness unchanged).
    const char* statFr[]={"Attaque","Defense","Endurance"};const char* statEn[]={"Attack","Defence","Endurance"};
    for(int i=0;i<3;++i){missionBookStatsHeaders[i]=bookKitIcon(missionBookDelegationPanel,statSlots[i],i==2?registerAmber:registerIvory);missionBookStatsHeaders[i]->setNeedMouseFocus(true);missionBookStatsHeaders[i]->setNeedToolTip(true);missionBookStatsHeaders[i]->setUserString("bookTooltip",gMercenarieEnglish?statEn[i]:statFr[i]);missionBookStatsHeaders[i]->eventToolTip+=MyGUI::newDelegate(bookStatTooltip);}
    missionBookStatTooltip=registerText(missionBookDelegationPanel,12,74,120,24,16,"",registerIvory);missionBookStatTooltip->setVisible(false);
    missionBookRosterScroll=missionBookDelegationPanel->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",0,0,100,100,MyGUI::Align::Default);MercenarieNativeInput::bind(missionBookRosterScroll);missionBookRosterScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    missionBookSelectedTeam=registerText(missionBookDelegationPanel,0,0,100,28,18,"",registerAmber);
    missionBookTeamScroll=missionBookDelegationPanel->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",0,0,100,80,MyGUI::Align::Default);MercenarieNativeInput::bind(missionBookTeamScroll);missionBookTeamScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    const char* keys[]={"missionbook.group","missionbook.travel","missionbook.activity","missionbook.total"};
    for(int i=0;i<4;++i){missionBookSummaryLabels[i]=registerText(missionBookDelegationPanel,0,0,100,24,16,Loc::text(keys[i]),registerIvory);missionBookSummaryValues[i]=registerText(missionBookDelegationPanel,0,0,100,24,16,"",registerIvory);missionBookSummaryValues[i]->setTextAlign(MyGUI::Align::Right|MyGUI::Align::Top);}
    const int summarySlots[]={7,8,10,8};for(int i=0;i<4;++i){missionBookSummaryIcons[i]=bookKitIcon(missionBookDelegationPanel,summarySlots[i],i==3?registerAmber:registerIvory);if(i<3)missionBookSummaryRules[i]=registerSolid(missionBookDelegationPanel,0,0,1,1,MyGUI::Colour(.14f,.17f,.17f));}
    missionBookDelegationStatus=registerText(missionBookDelegationPanel,0,0,100,36,14,"",registerAmber);
    missionBookDelegateButton=missionBookDelegationPanel->createWidget<MyGUI::Button>("TheMercenarie_BookActionSkin",0,0,100,64,MyGUI::Align::Default);MercenarieFonts::caption(missionBookDelegateButton,Loc::text("missionbook.delegate"));missionBookDelegateButton->setTextColour(registerAmber);missionBookDelegateButton->eventMouseButtonClick+=MyGUI::newDelegate(missionBookDelegateClicked);
    missionBookDelegateIcon=bookKitIcon(missionBookDelegateButton,7,registerAmber);
    missionBookDelegateSubtitle=registerText(missionBookDelegateButton,70,54,300,28,19,"",registerIvory);missionBookDelegateSubtitle->setTextAlign(MyGUI::Align::Center);
    optionKitBorder(missionBookDelegateButton,registerAmber);
    missionBookDetail=c->createWidget<MyGUI::Widget>("TheMercenarie_Panel",0,0,100,100,MyGUI::Align::Default);
    missionBookDetailScroll=missionBookDetail->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",0,0,100,100,MyGUI::Align::Stretch);MercenarieNativeInput::bind(missionBookDetailScroll);missionBookDetailScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);
    missionBookDetailIcon=boardIconV6(missionBookDetailScroll,0,8,8,64,registerAmber);
    missionBookName=registerText(missionBookDetailScroll,0,0,100,30,23,"",registerIvory);missionBookType=registerText(missionBookDetailScroll,0,0,100,26,18,"",registerAmber);
    missionBookPrice=registerText(missionBookDetailScroll,0,0,100,30,23,"",registerAmber);missionBookPrice->setTextAlign(MyGUI::Align::Right|MyGUI::Align::Top);
    missionBookDuration=registerText(missionBookDetailScroll,0,0,100,28,16,"",registerIvory);missionBookDuration->setTextAlign(MyGUI::Align::Right|MyGUI::Align::Top);
    missionBookStory=registerText(missionBookDetailScroll,0,0,100,80,16,"",registerIvory);
    missionBookObjectives=registerText(missionBookDetailScroll,0,0,100,180,16,"",registerIvory);missionBookFacts=registerText(missionBookDetailScroll,0,0,100,180,16,"",registerIvory);
    missionBookEmpty=registerText(contractsListPanel,12,70,100,100,17,Loc::text("missionbook.none"),registerIvory);missionBookEmpty->setVisible(false);
    missionBookTakeIcon=bookKitIcon(contractsAccept,9,registerAmber);missionBookTakeIcon->setVisible(false);
    missionBookTakeSubtitle=registerText(contractsAccept,70,54,300,28,19,"",registerIvory);missionBookTakeSubtitle->setTextAlign(MyGUI::Align::Center);missionBookTakeSubtitle->setVisible(false);
    missionBookCategoryIcon=bookKitIcon(contractsListPanel,0,registerAmber);missionBookCategoryIcon->setVisible(false);
    missionBookObjectivesTitle=registerText(missionBookDetailScroll,0,0,100,26,20,Loc::text("missionbook.objectives"),registerAmber);
    missionBookFactsTitle=registerText(missionBookDetailScroll,0,0,100,26,20,Loc::text("missionbook.information"),registerAmber);
    const char* factKeys[]={"missionbook.departure","missionbook.arrival","missionbook.distance","missionbook.group","missionbook.danger","missionbook.reputation","missionbook.type"};
    for(int i=0;i<7;++i){missionBookFactLabels[i]=registerText(missionBookDetailScroll,0,0,100,24,18,Loc::text(factKeys[i]),registerIvory);missionBookFactValues[i]=registerText(missionBookDetailScroll,0,0,100,24,18,"",registerIvory);}
    missionBookPageArrow=contractsListPanel->createWidget<MyGUI::Button>("TheMercenarie_Button",106,0,40,36,MyGUI::Align::Default);MercenarieFonts::caption(missionBookPageArrow,">");missionBookPageArrow->eventMouseButtonClick+=MyGUI::newDelegate(missionBookPageArrowClicked);missionBookPageArrow->setVisible(false);
    for(int i=0;i<MissionBookContracts::PageSize;++i){
        MissionBookOfferWidgets w;w.row=boardPanelV6(contractsListPanel,8,54,100,90);
        w.button=w.row->createWidget<MyGUI::Button>("Kenshi_Button1",1,1,98,88,MyGUI::Align::Default);w.button->setAlpha(0);w.button->setUserString("bookRow",registerNumber(i));w.button->eventMouseButtonClick+=MyGUI::newDelegate(missionBookOfferRowClicked);
        w.icon=boardIconV6(w.row,0,8,8,48,registerAmber);
        w.reroll=createContractRerollButton(w.row,i,true);w.button->setNeedToolTip(true);w.button->eventToolTip+=MyGUI::newDelegate(bookRerollTooltip);
        w.name=registerText(w.row,64,4,100,24,20,"",registerIvory);w.type=registerText(w.row,64,28,100,24,18,"",registerAmber);
        w.duration=registerText(w.row,64,52,70,24,18,"",registerIvory);w.price=registerText(w.row,134,52,90,24,18,"",registerAmber);w.price->setTextAlign(MyGUI::Align::Right|MyGUI::Align::Top);w.rerollBadge=registerText(w.row,4,40,56,16,14,Loc::text("v9.reroll.short"),registerAmber);w.rerollBadge->setTextAlign(MyGUI::Align::Center);
        for(int j=0;j<4;++j)w.edges[j]=registerSolid(w.row,0,0,1,1,registerAmber);
        w.row->setVisible(false);missionBookOfferRows.push_back(w);
    }
    missionBookDetail->setVisible(false);missionBookDelegationPanel->setVisible(false);
}
void applyMissionBookPresentation(){
    if(!missionBookDetail)return;
    bool book=missionBookDelegationContext&&missionBookTab<2;
    if(!book){
        for(size_t i=0;i<missionBookOfferRows.size();++i)missionBookOfferRows[i].row->setVisible(false);
        if(missionBookPresentationActive){
            MyGUI::UString caption=contractsAccept->getCaption();contractsAccept->changeWidgetSkin("TheMercenarie_Button");MercenarieFonts::caption(contractsAccept,caption);contractsAccept->setFontHeight(24);missionBookTakeSubtitle->setVisible(false);
            for(size_t i=0;i<missionBookOriginalWidgets.size();++i)missionBookOriginalWidgets[i].widget->setCoord(missionBookOriginalWidgets[i].coord);
            for(int i=0;i<6;++i){contractButtons[i]->setAlpha(1);contractButtons[i]->getParent()->setVisible(true);}
            MercenarieFonts::caption(missionBookOffersTitle,Loc::text("ui.available_offers"));
            MercenarieFonts::caption(contractsHeading,Loc::text("ui.the_mercenarie_command_post_contracts_68e2811"));
        }
        missionBookPresentationActive=false;missionBookDetail->setVisible(false);missionBookDelegationPanel->setVisible(false);missionBookEmpty->setVisible(false);
        missionBookTakeIcon->setVisible(false);missionBookPageArrow->setVisible(false);
        missionBookCategoryCombo->setVisible(false);missionBookCategoryIcon->setVisible(false);missionBookPagePrevious->setVisible(false);missionBookPageNext->setVisible(false);missionBookPageLabel->setVisible(false);
        contractsInfoPanel->setVisible(true);contractsLegend->setVisible(true);return;
    }
    if(!missionBookPresentationActive)contractsAccept->changeWidgetSkin("TheMercenarie_BookActionSkin");
    const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();missionBookViewportW=v.width;missionBookViewportH=v.height;
    missionBookLayout=MissionBookLayout::calculate(v.width,v.height,delegationSelectedCount()>0);const MissionBookLayout::Layout& l=missionBookLayout;missionBookPresentationActive=true;
    contractsWindow->setCoord(l.x,l.y,l.w,l.h);bookPanelCoord(missionBookBackground,MissionBookLayout::Rect(0,0,l.w,l.h));bookPanelCoord(missionBookHeader,MissionBookLayout::Rect(0,0,l.w,54));
    contractsHeading->setCoord(12,9,l.w-64,36);contractsHeading->setFontHeight(std::max(20,std::min(30,l.font+7)));missionBookCloseButton->setPosition(l.w-38,7);
    bookPanelCoord(contractsListPanel,l.left);contractsInfoPanel->setVisible(false);bookCoord(missionBookDelegationPanel,l.right);missionBookDelegationPanel->setVisible(true);
    bookCoord(contractsMapPanel,l.map);contractsMapImage->setCoord(4,62,l.map.w-8,l.map.h-66);contractsMap->setCoord(8,8,l.map.w-190,48);contractsMap->setFontHeight(std::max(14,std::min(22,l.font-2)));MercenarieFonts::caption(contractsMap,Loc::text("missionbook.legend"));missionBookPriceButton->setCoord(l.map.w-174,12,166,34);missionBookPriceButton->setFontHeight(16);
    bookCoord(contractsAccept,l.accept);contractsAccept->setFontHeight(l.font+7);MercenarieFonts::caption(contractsAccept,contractAcceptArmed?Loc::text("options.confirm"):Loc::text("missionbook.take"));
    missionBookTakeIcon->setVisible(true);missionBookTakeIcon->setCoord(14,(l.accept.h-56)/2,56,56);missionBookTakeSubtitle->setCoord(76,l.accept.h/2+4,l.accept.w-88,30);missionBookTakeSubtitle->setFontHeight(l.font);
    // Resize the four existing orange action edges with the button.
    contractsAccept->getChildAt(0)->setCoord(0,0,l.accept.w,2);contractsAccept->getChildAt(1)->setCoord(0,l.accept.h-2,l.accept.w,2);contractsAccept->getChildAt(2)->setCoord(0,0,2,l.accept.h);contractsAccept->getChildAt(3)->setCoord(l.accept.w-2,0,2,l.accept.h);
    int fw=l.left.w*46/100;missionBookOffersTitle->setCoord(12,6,l.left.w-fw-24,44);missionBookOffersTitle->setFontHeight(std::min(26,l.font));
    missionBookCategoryCombo->setCoord(l.left.w-fw-10,8,fw,38);missionBookCategoryCombo->setFontHeight(l.font-1);missionBookCategoryCombo->setVisible(true);missionBookCategoryCombo->setMaxListLength(240);missionBookCategoryIcon->setCoord(l.left.w-fw-2,14,26,26);missionBookCategoryIcon->setVisible(true);
    for(int i=0;i<6;++i)contractButtons[i]->getParent()->setVisible(false);
    int shown=(int)missionBookPageEntries.size(),offerH=l.offerH;
    for(size_t row=0;row<missionBookOfferRows.size();++row){
        MissionBookOfferWidgets& w=missionBookOfferRows[row];w.row->setVisible(row<missionBookPageEntries.size());if(row>=missionBookPageEntries.size())continue;
        const MissionBookPoolEntry& e=missionBookPool[missionBookPageEntries[row]];const BoardOffer& o=e.offer;
        int width=l.left.w-16;bookPanelCoord(w.row,MissionBookLayout::Rect(8,54+(int)row*(offerH+6),width,offerH));w.button->setCoord(1,1,width-2,offerH-2);
        int icon=std::min(54,offerH-14),tx=icon+18,tw=width-tx-12,line=(offerH-10)/3;
        bool rerolled=ContractReroll::marked(o.routeRegions);int displayIcon=rerolled?std::min(icon,offerH-24):icon;w.icon->setCoord(8,rerolled?4:(offerH-icon)/2,displayIcon,displayIcon);setMissionIconV6(w.icon,o.missionType);w.rerollBadge->setCoord(4,offerH-19,icon+8,16);w.rerollBadge->setVisible(rerolled);
        refreshContractRerollButton(w.reroll,true,rerolled);w.button->setUserString("rerolled",ContractReroll::marked(o.routeRegions)?"1":"0");w.name->setCoord(tx,4,std::max(1,tw-34),line);w.type->setCoord(tx,4+line,std::max(1,tw-34),line);w.duration->setCoord(tx,4+line*2,tw*40/100,line);w.price->setCoord(tx+tw*40/100,4+line*2,tw*60/100,line);
        w.name->setFontHeight(std::min(l.font+1,line));w.type->setFontHeight(l.font);w.duration->setFontHeight(l.font);w.price->setFontHeight(l.font);
        MercenarieFonts::caption(w.name,mercenarieLocalize(o.townName));MercenarieFonts::caption(w.type,missionLabelV6(o.missionType));w.type->setTextColour(missionColourV6(o.missionType));MercenarieFonts::caption(w.price,registerNumber(displayedContractCats(o.estimatedPay))+Loc::text("ui.cats_3f70b99"));
        // Use the existing timing path with the entry's source context.
        int tab=missionBookTab;missionBookTab=e.security?1:0;try{DelegatedMissionTiming::State t=bookTiming(o);MercenarieFonts::caption(w.duration,bookDuration(t.travelHours+t.activityHours));}catch(...){MercenarieFonts::caption(w.duration,"--");}missionBookTab=tab;
        
        bookEllipsis(w.name);bookEllipsis(w.type);bookEllipsis(w.price);
        w.edges[0]->setCoord(0,0,width,2);w.edges[1]->setCoord(0,offerH-2,width,2);w.edges[2]->setCoord(0,0,2,offerH);w.edges[3]->setCoord(width-2,0,2,offerH);
        for(int j=0;j<4;++j)w.edges[j]->setVisible(e.id==missionBookSelectedId);
    }
    MercenarieFonts::caption(missionBookOffersTitle,std::string(Loc::text("missionbook.offers"))+" ("+registerNumber((int)missionBookFiltered.size())+")");missionBookEmpty->setCoord(12,70,l.left.w-24,90);missionBookEmpty->setVisible(shown==0);
    int pages=MissionBookContracts::pageCount(missionBookFiltered.size(),MissionBookContracts::PageSize);
    missionBookPagePrevious->setCoord(10,l.left.h-76,40,36);MercenarieFonts::caption(missionBookPagePrevious,"<");missionBookPagePrevious->setVisible(pages>1);missionBookPagePrevious->setEnabled(missionBookOfferPage>0);missionBookPagePrevious->setStateSelected(false);
    missionBookPageNext->setCoord(58,l.left.h-76,40,36);MercenarieFonts::caption(missionBookPageNext,">");missionBookPageNext->setVisible(pages>1);missionBookPageNext->setEnabled(missionBookOfferPage+1<pages);missionBookPageNext->setStateSelected(false);missionBookPageArrow->setVisible(false);
    missionBookPageLabel->setCoord(106,l.left.h-74,l.left.w-118,34);missionBookPageLabel->setFontHeight(l.font-1);missionBookPageLabel->setVisible(true);MercenarieFonts::caption(missionBookPageLabel,registerNumber(missionBookOfferPage+1)+" / "+registerNumber(pages)+" | "+registerNumber((int)missionBookFiltered.size())+" "+Loc::text("missionbook.displayed"));
    refreshMissionBookOfferCounts();
    contractsRefreshText->setCoord(l.left.x+10,l.left.y+l.left.h-36,l.left.w-20,30);contractsRefreshText->setFontHeight(l.font-1);contractsLegend->setVisible(false);
    layoutMissionBookDelegationGeometry(delegationSelectedCount()>0);
    MercenarieFonts::caption(missionBookDelegateButton,delegationFinalArmed?Loc::text("options.confirm"):Loc::text("missionbook.delegate"));
    bookCoord(missionBookDetail,l.detail);missionBookDetail->setVisible(shown>0);contractsAccept->setEnabled(shown>0&&missionBookPersonalAvailable());contractsMapPanel->setVisible(shown>0);
    int dw=l.detail.w-26,bodyFont=l.font,headH=bodyFont*2+20,storyH=bodyFont*3,factsY=headH+storyH+8,detailH=std::max(l.detail.h-4,factsY+(bodyFont+4)*9);
    missionBookDetailScroll->setCoord(0,0,l.detail.w,l.detail.h);missionBookDetailScroll->setCanvasSize(dw,detailH);missionBookDetailScroll->setVisibleHScroll(false);missionBookDetailScroll->setVisibleVScroll(detailH>l.detail.h);
    missionBookDetailIcon->setCoord(8,8,52,52);missionBookName->setCoord(70,6,dw*55/100-74,bodyFont+12);missionBookName->setFontHeight(bodyFont+5);missionBookType->setCoord(70,bodyFont+20,dw*55/100-74,bodyFont+8);missionBookType->setFontHeight(bodyFont);
    missionBookPrice->setCoord(dw*55/100,8,dw*45/100,bodyFont+12);missionBookPrice->setFontHeight(bodyFont+5);missionBookDuration->setCoord(dw*55/100,bodyFont+20,dw*45/100,bodyFont+8);missionBookDuration->setFontHeight(bodyFont);
    missionBookStory->setCoord(12,headH+6,dw-24,storyH);missionBookStory->setFontHeight(bodyFont);
    missionBookObjectives->setCoord(12,factsY,dw*47/100-18,detailH-factsY);missionBookFacts->setCoord(dw*47/100+8,factsY,dw*53/100-20,detailH-factsY);missionBookObjectives->setFontHeight(bodyFont);missionBookFacts->setFontHeight(bodyFont);
    BoardOffer& offer=boardOffers[selectedOffer];MercenarieFonts::caption(missionBookName,mercenarieLocalize(offer.townName));MercenarieFonts::caption(missionBookType,missionLabelV6(offer.missionType));missionBookType->setTextColour(missionColourV6(offer.missionType));setMissionIconV6(missionBookDetailIcon,offer.missionType);MercenarieFonts::caption(missionBookPrice,registerNumber(displayedContractCats(offer.estimatedPay))+Loc::text("ui.cats_3f70b99"));MercenarieFonts::caption(missionBookStory,(ContractReroll::marked(offer.routeRegions)?std::string(Loc::text("v9.reroll.indicator"))+"\n":"")+mercenarieLocalize(offer.story));
    const char* objectives[]={"missionbook.objective.escort","missionbook.objective.caravan","missionbook.objective.science","missionbook.objective.bounty","missionbook.objective.mail"};
    MercenarieFonts::caption(missionBookObjectives,Loc::text(objectives[std::max(0,std::min(4,offer.missionType))]));
    std::string factValues[]={mercenarieLocalize(originCity),mercenarieLocalize(offer.townName),registerNumber((int)(offer.distance/1000))+Loc::text("ui.km"),registerNumber(offer.groupSize),registerNumber(offer.dangerLevel)+" / 5",registerNumber(localReputation()),missionLabelV6(offer.missionType)};
    for(int i=0;i<7;++i)MercenarieFonts::caption(missionBookFactValues[i],factValues[i]);missionBookFacts->setVisible(false);
    try{DelegatedMissionTiming::State t=bookTiming(offer);MercenarieFonts::caption(missionBookSummaryValues[1],bookDuration(t.travelHours));MercenarieFonts::caption(missionBookSummaryValues[2],bookDuration(t.activityHours));MercenarieFonts::caption(missionBookSummaryValues[3],bookDuration(t.travelHours+t.activityHours));MercenarieFonts::caption(missionBookDuration,std::string(Loc::text("missionbook.estimate"))+bookDuration(t.travelHours+t.activityHours));}catch(...){MercenarieFonts::caption(missionBookDuration,"--");for(int i=1;i<4;++i)MercenarieFonts::caption(missionBookSummaryValues[i],"--");}
    bookEllipsis(missionBookName);bookEllipsis(missionBookPrice);bookEllipsis(missionBookType);bookEllipsis(missionBookDuration);bookWrap(missionBookOffersTitle,missionBookOffersTitle->getCaption().asUTF8());bookEllipsis(missionBookDelegationTitle);bookWrap(contractsMap,Loc::text("missionbook.legend"));
    bookWrap(missionBookStory,(ContractReroll::marked(offer.routeRegions)?std::string(Loc::text("v9.reroll.indicator"))+"\n":"")+(missionBookTab==1?missionBookBountyDossier():mercenarieLocalize(offer.story)));
    missionBookStory->setSize(missionBookStory->getWidth(),std::max(bodyFont+6,missionBookStory->getTextSize().height+4));
    factsY=missionBookStory->getTop()+missionBookStory->getHeight()+6;
    const int split=dw*40/100,infoX=split+12,infoW=dw-infoX-8,titleH=bodyFont+7;
    missionBookObjectivesTitle->setCoord(12,factsY,split-20,titleH);missionBookObjectivesTitle->setFontHeight(bodyFont+1);
    missionBookFactsTitle->setCoord(infoX,factsY,infoW,titleH);missionBookFactsTitle->setFontHeight(bodyFont+1);
    missionBookObjectives->setCoord(12,factsY+titleH,split-20,200);bookWrap(missionBookObjectives,missionBookObjectives->getCaption().asUTF8());
    int objectivesH=missionBookObjectives->getTextSize().height+4;missionBookObjectives->setSize(split-20,objectivesH);
    int fy=factsY+titleH,labelW=infoW*46/100;
    const char* factKeys[]={"missionbook.departure","missionbook.arrival","missionbook.distance","missionbook.group","missionbook.danger","missionbook.reputation","missionbook.type"};
    for(int i=0;i<7;++i){
        MyGUI::TextBox* label=missionBookFactLabels[i];MyGUI::TextBox* value=missionBookFactValues[i];
        label->setVisible(i<6);value->setVisible(i<6);if(i==6)continue; // Mission type is already displayed in the header.
        label->setCoord(infoX,fy,labelW-6,bodyFont+5);value->setCoord(infoX+labelW,fy,infoW-labelW,bodyFont+5);label->setFontHeight(bodyFont-1);value->setFontHeight(bodyFont-1);
        bookWrap(label,Loc::text(factKeys[i]));bookWrap(value,factValues[i]);int rh=std::max(bodyFont+3,std::max(label->getTextSize().height,value->getTextSize().height)+2);
        label->setSize(label->getWidth(),rh);value->setSize(value->getWidth(),rh);fy+=rh;
    }
    int contentH=std::max(fy,factsY+titleH+objectivesH)+8;
    missionBookDetailScroll->setCanvasSize(dw,std::max(l.detail.h-4,contentH));missionBookDetailScroll->setVisibleVScroll(MissionBookGeometry::scroll(contentH,l.detail.h-4));if(contentH<=l.detail.h-4)missionBookDetailScroll->setViewOffset(MyGUI::IntPoint(0,0));
    layoutMissionBookRoster();refreshMissionBookTeamPortraits();
    if(shown==0)missionBookDelegateButton->setEnabled(false);
    refreshMissionBookVisualDetails();
}
void resetMissionBookPresentation(){
    mercenarieDestroyLiveWidget(bookRerollTip);bookRerollTip=0;bookRerollTipText=0;
    resetV9PendingActions();
    // Invalidate BEFORE the owning window is destroyed during save/load/import.
    MyGUI::Window* oldRoster=delegationRosterWindow;
    delegationRosterWindow=0;delegationRosterScroll=0;delegationRosterCards.clear();
    delegationSelectionCount=delegationSelectionSummary=0;delegationDeselectAllButton=delegationConfirmButton=0;
    for(int i=0;i<6;++i)delegationFilterButtons[i]=0;
    delegationRosterCharacters.clear();delegationRosterSelected.clear();delegationRosterAvailable.clear();delegationRosterCategories.clear();
    missionBookDelegationContext=false;delegationConfirming=false;delegationFinalArmed=false;contractAcceptArmed=false;
    mercenarieDestroyLiveWidget(oldRoster);
    missionBookOfferRows.clear();missionBookPool.clear();missionBookFiltered.clear();missionBookPageEntries.clear();missionBookSelectedId.clear();missionBookPoolReady=false;missionBookSelectedCharacters.clear();missionBookLegalIssuer.setNull();missionBookSecurityIssuer.setNull();missionBookSecurityIssuers.clear();missionBookClassicSources.clear();missionBookTownId.clear();
    missionBookOriginalWidgets.clear();missionBookSoldierRows.clear();missionBookTeamPortraits.clear();missionBookSoldierOrder.clear();missionBookRosterIdentity.clear();
    missionBookDetail=missionBookHeader=missionBookBackground=missionBookDelegationPanel=0;
    missionBookRosterScroll=missionBookTeamScroll=missionBookDetailScroll=0;
    missionBookAvailable=missionBookOffersTitle=missionBookDelegationTitle=missionBookName=missionBookType=missionBookPrice=missionBookDuration=missionBookStory=missionBookObjectives=missionBookFacts=missionBookEmpty=0;
    missionBookSelectedTeam=missionBookTimingText=missionBookDelegationStatus=0;
    missionBookPriceButton=missionBookCloseButton=missionBookDelegateButton=missionBookPageArrow=0;missionBookDetailIcon=missionBookTakeIcon=0;missionBookSoldierSort=0;
    for(int i=0;i<3;++i)missionBookStatsHeaders[i]=0;
    for(int i=0;i<4;++i){missionBookSummaryLabels[i]=missionBookSummaryValues[i]=0;missionBookSummaryIcons[i]=0;}
    for(int i=0;i<3;++i)missionBookSummaryRules[i]=0;
    missionBookCategoryIcon=missionBookDelegateIcon=0;missionBookTakeSubtitle=missionBookDelegateSubtitle=missionBookStatTooltip=missionBookObjectivesTitle=missionBookFactsTitle=0;
    for(int i=0;i<7;++i)missionBookFactLabels[i]=missionBookFactValues[i]=0;
    missionBookCategoryCombo=0;missionBookPagePrevious=missionBookPageNext=0;missionBookPageLabel=0;
    contractsListPanel=contractsInfoPanel=0;contractsHeading=0;missionBookBoardPageText=0;for(int i=0;i<6;++i)missionBookBoardTabs[i]=0;
    missionBookPresentationActive=false;missionBookViewportW=missionBookViewportH=0;
}
