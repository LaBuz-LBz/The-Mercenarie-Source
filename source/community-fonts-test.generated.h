#pragma once
static std::string mercenarieLocalize(const std::string& source);
namespace MercenarieFonts {
inline bool extended(){return Loc::engine().language!="en"&&Loc::engine().language!="fr";}
inline void ensure(){
    MyGUI::ResourceManager& manager=MyGUI::ResourceManager::getInstance();
    if(manager.isExist("MercenarieUnicode"))return;
    Ogre::ResourceGroupManager& r=Ogre::ResourceGroupManager::getSingleton();
    if(!r.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))r.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");
    manager.load("MercenarieUnicodeFonts.xml");
}
inline std::string fontFor(const std::string& code){
    const Loc::LanguagePack* p=Loc::registry().find(code);
    if(p&&!p->official&&!p->fontResource.empty()){
        MyGUI::IResource* resource=MyGUI::ResourceManager::getInstance().getByName(p->fontResource,false);
        if(resource&&resource->castType<MyGUI::IFont>(false))return p->fontResource;
    }
    return "MercenarieUnicode";
}
inline void writeDiagnostics(std::ostream& out){
    const Loc::LanguagePack* p=Loc::registry().find(Loc::engine().language);if(!p||p->official)return;
    std::string name=fontFor(p->code);out<<"localization.fontRequested="<<p->fontResource<<"\nlocalization.fontUsed="<<name<<"\n";
    MyGUI::IResource* resource=MyGUI::ResourceManager::getInstance().getByName(name,false);
    MyGUI::IFont* font=resource?resource->castType<MyGUI::IFont>(false):0;
    std::set<unsigned int> points;std::string bytes=p->name;
    for(Loc::Catalogue::const_iterator i=Loc::engine().selected.begin();i!=Loc::engine().selected.end();++i)bytes+=i->second;
    for(size_t i=0;i<bytes.size();){unsigned char c=bytes[i++];unsigned int point=c;int extra=0;
        if(c>=240){point=c&7;extra=3;}else if(c>=224){point=c&15;extra=2;}else if(c>=192){point=c&31;extra=1;}
        while(extra--&&i<bytes.size())point=(point<<6)|((unsigned char)bytes[i++]&63);
        if(point>32)points.insert(point);
    }
    size_t absent=0;for(std::set<unsigned int>::const_iterator i=points.begin();i!=points.end();++i){MyGUI::GlyphInfo* glyph=font?font->getGlyphInfo(*i):0;if(!glyph||glyph->codePoint!=*i)++absent;}
    out<<"localization.fontCheckedCodepoints="<<points.size()<<"\nlocalization.fontMissingCodepoints="<<absent<<"\n";
}
inline void setFont(MyGUI::TextBox* text,const std::string& font){
    if(text&&text->getFontName()!=font){int height=text->getFontHeight();text->setFontName(font);text->setFontHeight(height);}
}
inline void languageChoice(MyGUI::ComboBox* choice){
    if(!choice)return;
    setFont(choice,fontFor(Loc::languageChoiceCode(choice->getIndexSelected())));
    MyGUI::Widget* child=choice->findWidget("List");MyGUI::ListBox* list=child?child->castType<MyGUI::ListBox>(false):0;
    if(!list||!list->getVisible())return;
    // MyGUI recycles visible rows as the list scrolls. No filesystem work here.
    for(size_t i=0;i<Loc::languageChoiceCount()&&i<list->getItemCount();++i){MyGUI::Widget* row=list->getWidgetByIndex(i);if(row)setFont(row->castType<MyGUI::TextBox>(false),fontFor(Loc::languageChoiceCode(i)));}
}
inline void prepare(MyGUI::Widget* widget){
    if(!widget)return;
    MyGUI::TextBox* text=widget->castType<MyGUI::TextBox>(false);
    MyGUI::Window* window=widget->castType<MyGUI::Window>(false);
    if(window)text=window->getCaptionWidget();
    if(!text)return;
    if(extended()){ensure();if(!text->isUserString("MercenarieOriginalFont"))text->setUserString("MercenarieOriginalFont",text->getFontName());setFont(text,fontFor(Loc::engine().language));}
    else if(text->isUserString("MercenarieOriginalFont"))setFont(text,text->getUserString("MercenarieOriginalFont"));
}
template<class T> inline void caption(T* widget,const std::string& value){prepare(widget);widget->setCaption(mercenarieLocalize(value));}
inline const char* newsFont(const char* original){
    if(!extended())return original;
    const Loc::LanguagePack* p=Loc::registry().find(Loc::engine().language);
    if(p&&p->official&&std::string(original)=="MercenarieNewsTitle")return original;
    ensure();return Loc::engine().intern(fontFor(Loc::engine().language));
}
}