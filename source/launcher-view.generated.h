#pragma once

#include "RerollPopupLayout.h"
// Legacy helpers are shared by QuestTrackerView: preserve their exact styling.
MyGUI::Colour launcherSand(.81f,.72f,.56f),launcherOrange(1,.59f,.12f);
MyGUI::Widget* launcherSolid(MyGUI::Widget* p,int x,int y,int w,int h,const MyGUI::Colour& color){MyGUI::Widget* t=p->createWidget<MyGUI::Widget>("WhiteSkin",x,y,w,h,MyGUI::Align::Default);t->setColour(color);t->setNeedMouseFocus(false);return t;}
MyGUI::TextBox* launcherText(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& caption,const MyGUI::Colour& color){MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",x,y,w,h,MyGUI::Align::Default);MercenarieFonts::caption(t,caption);t->setFontHeight(size);t->setTextColour(color);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setNeedMouseFocus(false);return t;}
void launcherIcon(MyGUI::Widget* p,int slot,int x,int y,int size){MyGUI::ImageBox* t=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);t->setImageTexture("LauncherIcons.png");t->setImageCoord(MyGUI::IntCoord(slot*96,0,96,96));t->setColour(launcherSand);t->setNeedMouseFocus(false);}
// V9 scoped launcher presentation.
// Presentation only: existing click callbacks and persistence own lifecycle.
MyGUI::Button* launcherCards[3]={0};
MyGUI::TextBox* launcherArrows[3]={0};
MyGUI::Colour launcherV9Sand(.85f,.79f,.62f),launcherV9Orange(.94f,.65f,.15f);
void launcherEnsureUi(){
    MyGUI::ResourceManager& m=MyGUI::ResourceManager::getInstance();
    if(m.isExist("LauncherV9Card"))return;
    Ogre::ResourceGroupManager& r=Ogre::ResourceGroupManager::getSingleton();
    if(!r.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))r.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");
    m.load("MercenarieLauncher.xml");
}
MyGUI::Widget* launcherV9Solid(MyGUI::Widget* p,int x,int y,int w,int h,const MyGUI::Colour& c){
    MyGUI::Widget* t=p->createWidget<MyGUI::Widget>("WhiteSkin",x,y,w,h,MyGUI::Align::Default);t->setColour(c);t->setNeedMouseFocus(false);return t;
}
MyGUI::TextBox* launcherV9Text(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& caption,const MyGUI::Colour& c,bool title=false){
    MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",x,y,w,h,MyGUI::Align::Default);
    std::ostringstream name;name<<"Launcher"<<(title&&(Loc::engine().language=="en"||Loc::engine().language=="fr")?"Title":"Body")<<size;
    t->setFontName(name.str());MercenarieFonts::caption(t,caption);
    // Community packs retain their chosen font; official packs use sized glyph atlases.
    const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);
    if(!pack||pack->official)t->setFontName(name.str());
    t->setFontHeight(size);t->setTextColour(c);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setNeedMouseFocus(false);return t;
}
struct LauncherMeasure{MyGUI::TextBox* t;LauncherMeasure(MyGUI::TextBox* v):t(v){}int operator()(const std::string& s)const{t->setCaption(s);return t->getTextSize().width;}};
int launcherWrap(MyGUI::TextBox* t,const std::string& s){t->setCaption(RerollPopupLayout::wrap(s,t->getWidth()-2,LauncherMeasure(t)));int h=t->getTextSize().height+4;t->setSize(t->getWidth(),h);return h;}
void launcherLogo(MyGUI::Widget* p,int x,int y,int size){
    // Preserve the original 1230 x 1278 source aspect ratio, centered in its box.
    int width=size*1230/1278;
    MyGUI::ImageBox* t=p->createWidget<MyGUI::ImageBox>("ImageBox",x+(size-width)/2,y,width,size,MyGUI::Align::Default);t->setImageTexture("MercenarieLauncherLogo.png");t->setNeedMouseFocus(false);
}
void launcherV9Icon(MyGUI::Widget* p,int slot,int x,int y,int size){
    MyGUI::ImageBox* t=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);t->setImageTexture("MercenarieLauncherIcons.png");t->setImageCoord(MyGUI::IntCoord(slot*96,0,96,96));t->setColour(launcherV9Sand);t->setNeedMouseFocus(false);
}
void launcherHighlight(MyGUI::Widget* sender,bool on){for(int i=0;i<3;++i)if(sender==launcherCards[i])launcherArrows[i]->setTextColour(on?MyGUI::Colour(1,.83f,.35f):launcherV9Orange);}
void launcherFocus(MyGUI::Widget* s,MyGUI::Widget*){launcherHighlight(s,true);}
void launcherBlur(MyGUI::Widget* s,MyGUI::Widget*){launcherHighlight(s,false);}
void launcherPress(MyGUI::Widget* s,int,int,MyGUI::MouseButton){for(int i=0;i<3;++i)if(s==launcherCards[i])launcherArrows[i]->setTextColour(MyGUI::Colour(1,.94f,.70f));}
void launcherRelease(MyGUI::Widget* s,int,int,MyGUI::MouseButton){launcherHighlight(s,true);}
void buildMercenarieLauncher(MyGUI::Gui* gui){
    if(mercenarieLauncherMenu)return;
    launcherEnsureUi();
    const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
    const int w=std::min(500,view.width-24),rowW=w-32;
    mercenarieLauncherMenu=gui->createWidget<MyGUI::Widget>("LauncherV9Frame",12,82,w,356,MyGUI::Align::Default,"Window","MercenarieLauncherMenu");
    MyGUI::Widget* p=mercenarieLauncherMenu;
    launcherLogo(p,18,14,64);
    MyGUI::TextBox* title=launcherV9Text(p,94,17,w-145,30,24,Loc::text("ui.the_mercenarie"),MyGUI::Colour(.94f,.94f,.90f),true);
    int titleH=launcherWrap(title,Loc::text("ui.the_mercenarie"));
    MyGUI::TextBox* tagline=launcherV9Text(p,94,20+titleH,w-116,36,14,Loc::text("ui.contracts_a_guild_a_lawless_world"),MyGUI::Colour(.71f,.72f,.69f));
    int tagH=launcherWrap(tagline,Loc::text("ui.contracts_a_guild_a_lawless_world"));
    int header=std::max(94,28+titleH+tagH);
    MyGUI::Button* close=p->createWidget<MyGUI::Button>("LauncherV9Close",w-42,16,26,26,MyGUI::Align::Default);
    MyGUI::TextBox* cross=launcherV9Text(close,5,2,20,23,18,"X",launcherV9Sand);cross->setTextAlign(MyGUI::Align::Center);
    close->eventMouseButtonClick+=MyGUI::newDelegate(closeMercenarieLauncher);
    launcherV9Solid(p,12,header-8,w-24,1,launcherV9Orange);
    const char* titles[]={"ui.quest_tracker","ui.auto_pilot","ui.guild_management"};
    const char* descriptions[]={"ui.view_your_active_contracts","ui.send_your_soldiers_automatically","ui.offices_members_options_and_more"};
    MyGUI::TextBox* headings[3];MyGUI::TextBox* bodies[3];int titleHeights[3],bodyHeights[3];
    int rowH=78;
    for(int i=0;i<3;++i){
        MyGUI::Button* row=p->createWidget<MyGUI::Button>("LauncherV9Card",16,header+i*86,rowW,78,MyGUI::Align::Default);launcherCards[i]=row;
        headings[i]=launcherV9Text(row,86,10,rowW-126,26,20,Loc::text(titles[i]),MyGUI::Colour(.91f,.91f,.87f),true);
        titleHeights[i]=launcherWrap(headings[i],Loc::text(titles[i]));
        bodies[i]=launcherV9Text(row,86,36,rowW-126,36,14,Loc::text(descriptions[i]),MyGUI::Colour(.75f,.77f,.74f));
        bodyHeights[i]=launcherWrap(bodies[i],Loc::text(descriptions[i]));
        rowH=std::max(rowH,18+titleHeights[i]+bodyHeights[i]);
    }
    for(int i=0;i<3;++i){
        MyGUI::Button* row=launcherCards[i];row->setPosition(16,header+i*(rowH+8));row->setSize(rowW,rowH);
        int y=(rowH-titleHeights[i]-bodyHeights[i]-2)/2;
        headings[i]->setPosition(86,y);bodies[i]->setPosition(86,y+titleHeights[i]+2);
        launcherV9Icon(row,i,18,(rowH-48)/2,48);
        launcherArrows[i]=launcherV9Text(row,rowW-29,(rowH-28)/2,20,28,24,">",launcherV9Orange);
        row->eventMouseSetFocus+=MyGUI::newDelegate(launcherFocus);row->eventMouseLostFocus+=MyGUI::newDelegate(launcherBlur);
        row->eventMouseButtonPressed+=MyGUI::newDelegate(launcherPress);row->eventMouseButtonReleased+=MyGUI::newDelegate(launcherRelease);
        if(i==0)row->eventMouseButtonClick+=MyGUI::newDelegate(openMercenarieQuests);else if(i==1)row->eventMouseButtonClick+=MyGUI::newDelegate(openMercenarieAutopilot);else row->eventMouseButtonClick+=MyGUI::newDelegate(toggleGuildManagement);
    }
    int h=header+3*rowH+16+14;
    if(routeTestEnabled()){
        int y=h-4;MyGUI::Button* test=p->createWidget<MyGUI::Button>("Kenshi_Button1",16,y,w-32,36,MyGUI::Align::Default);MercenarieFonts::caption(test,Loc::text("ui.v6_test_route"));test->eventMouseButtonClick+=MyGUI::newDelegate(openRouteTest);
        test=p->createWidget<MyGUI::Button>("Kenshi_Button1",16,y+42,w-32,36,MyGUI::Align::Default);MercenarieFonts::caption(test,Loc::text("ui.v6_create_test_escort"));test->eventMouseButtonClick+=MyGUI::newDelegate(prepareRoadTestContract);h+=88;
    }
    p->setSize(w,h);p->setPosition(12,std::max(6,std::min(82,view.height-h-12)));p->setVisible(false);
}
