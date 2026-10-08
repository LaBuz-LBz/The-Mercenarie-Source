#pragma once
#include "Localization.h"
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
