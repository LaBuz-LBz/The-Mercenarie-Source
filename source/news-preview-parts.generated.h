struct NewsItem { const char* id; int icon; const char* title; const char* shortText; const char* tooltipTitle; const char* tooltipText; };
static const NewsItem Items[6]={
    {"contracts",0,"news.card.contracts.title","news.card.contracts.short","news.card.contracts.title","news.card.contracts.tooltip"},
    {"delegation",1,"news.card.delegation.title","news.card.delegation.short","news.card.delegation.title","news.card.delegation.tooltip"},
    {"finance",2,"news.card.finance.title","news.card.finance.short","news.card.finance.title","news.card.finance.tooltip"},
    {"options",3,"news.card.options.title","news.card.options.short","news.card.options.title","news.card.options.tooltip"},
    {"localization",4,"news.card.localization.title","news.card.localization.short","news.card.localization.title","news.card.localization.tooltip"},
    {"fixes",5,"news.card.fixes.title","news.card.fixes.short","news.card.fixes.title","news.card.fixes.tooltip"}
};

static const NewsItem V9Items[6]={
    {"reroll",0,"news.v9.reroll.title","news.v9.reroll.short","news.v9.reroll.title","news.v9.reroll.short"},
    {"translations",1,"news.card.localization.title","news.v9.translations.short","news.card.localization.title","news.v9.translations.short"},
    {"payroll",2,"news.v9.payroll.title","news.v9.payroll.short","news.v9.payroll.title","news.v9.payroll.short"},
    {"artisan",3,"news.v9.artisan.title","news.v9.artisan.short","news.v9.artisan.title","news.v9.artisan.short"},
    {"armour",4,"news.v9.armour.title","news.v9.armour.short","news.v9.armour.title","news.v9.armour.short"},
    {"buildings",5,"news.v9.buildings.title","news.v9.buildings.short","news.v9.buildings.title","news.v9.buildings.short"},
};
static int selectedVersion=MainMenuNewsRules::versionCount()-1;
static bool pendingNavigation=false;
inline const NewsItem* pageItems(){return selectedVersion==0?Items:V9Items;}
inline int pageItemCount(){return 6;}
inline const char* pageVersion(){return MainMenuNewsRules::Versions[selectedVersion];}
inline const char* pageChangelog(){return selectedVersion==0?"news.changelog_body":"news.v9.changelog_body";}
inline const char* pageChangelogTitle(){return selectedVersion==0?"news.changelog_title":"news.v9.changelog_title";}
inline std::string navigationCaption(int delta){Loc::Catalogue args;args["version"]=MainMenuNewsRules::Versions[MainMenuNewsRules::navigate(selectedVersion,delta,MainMenuNewsRules::versionCount())];return Loc::format(delta<0?"news.previous_version":"news.next_version",args);}

inline MyGUI::TextBox* text(MyGUI::Widget* parent,int x,int y,int w,int h,int size,const char* value,const MyGUI::Colour& colour){MyGUI::TextBox* t=parent->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",x,y,w,h,MyGUI::Align::Default);MercenarieFonts::caption(t,value);t->setFontHeight(size);t->setTextColour(colour);t->setNeedMouseFocus(false);return t;}
inline MyGUI::EditBox* wrappedText(MyGUI::Widget* parent,int x,int y,int w,int h,int size,const char* value,const MyGUI::Colour& colour){
    // MyGUI TextBox does not implement WordWrap. Kenshi's native wrapping widget is an EditBox.
    MyGUI::EditBox* t=parent->createWidget<MyGUI::EditBox>("Kenshi_WordWrapEmpty",x,y,w,h,MyGUI::Align::Default);
    t->setEditStatic(true);t->setEditReadOnly(true);t->setEditMultiLine(true);t->setEditWordWrap(true);
    t->setFontName(MercenarieFonts::newsFont("MercenarieNewsBody"));t->setFontHeight(size);
    t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setTextColour(colour);
    t->setNeedMouseFocus(false);t->setNeedKeyFocus(false);MercenarieFonts::caption(t,value);return t;
}
inline void border(MyGUI::Widget* parent,const MyGUI::Colour& colour){MyGUI::Widget* top=parent->createWidget<MyGUI::Widget>("WhiteSkin",0,0,parent->getWidth(),1,MyGUI::Align::HStretch|MyGUI::Align::Top);MyGUI::Widget* bottom=parent->createWidget<MyGUI::Widget>("WhiteSkin",0,parent->getHeight()-1,parent->getWidth(),1,MyGUI::Align::HStretch|MyGUI::Align::Bottom);MyGUI::Widget* left=parent->createWidget<MyGUI::Widget>("WhiteSkin",0,0,1,parent->getHeight(),MyGUI::Align::Left|MyGUI::Align::VStretch);MyGUI::Widget* right=parent->createWidget<MyGUI::Widget>("WhiteSkin",parent->getWidth()-1,0,1,parent->getHeight(),MyGUI::Align::Right|MyGUI::Align::VStretch);top->setColour(colour);bottom->setColour(colour);left->setColour(colour);right->setColour(colour);top->setNeedMouseFocus(false);bottom->setNeedMouseFocus(false);left->setNeedMouseFocus(false);right->setNeedMouseFocus(false);}
