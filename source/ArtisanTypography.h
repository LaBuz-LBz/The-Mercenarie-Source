// Font resources encode actual glyph sizes at 72 dpi. This avoids relying on
// line-height changes to enlarge a small native Kenshi font atlas.
void artisanEnsureUi(){
 MyGUI::ResourceManager& manager=MyGUI::ResourceManager::getInstance();
 if(!manager.isExist("ArtisanBody18")){Ogre::ResourceGroupManager& resources=Ogre::ResourceGroupManager::getSingleton();if(!resources.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))resources.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");manager.load("MercenarieArtisan.xml");}
}
bool artisanCommunityFont(){const Loc::LanguagePack* p=Loc::registry().find(Loc::engine().language);return p&&!p->official;}
void artisanSetFont(MyGUI::TextBox* t,int size,bool title=false){
 // Final readability pass; preserve fitting loops and each role's hierarchy.
 const int before[]={12,14,16,18,20,22,24,28,32,34};
 const int after []={12,16,18,20,22,24,26,32,36,36};
 for(int i=0;i<10;++i)if(size==before[i]){size=after[i];break;}
 if(artisanCommunityFont()){MercenarieFonts::prepare(t);t->setFontHeight(size);return;}
 const int sizes[]={12,14,16,18,20,22,24,26,28,30,32,34,36};int chosen=12;
 for(int i=0;i<13;++i)if(sizes[i]<=size)chosen=sizes[i];
 std::string name=std::string(title&&Loc::engine().language!="ru"&&Loc::engine().language!="pl"?"ArtisanTitle":"ArtisanBody")+artisanNumber(chosen);
 t->setFontName(name);t->setFontHeight(chosen);t->setUserString("ArtisanFontSize",artisanNumber(chosen));
}
void artisanCaption(MyGUI::TextBox* t,const std::string& value){
 std::string text=mercenarieLocalize(value),safe;
 for(size_t i=0;i<text.size();++i){safe+=text[i];if(text[i]=='#')safe+='#';}
 t->setCaption(safe);
}
struct ArtisanTextMeasure{MyGUI::TextBox* t;ArtisanTextMeasure(MyGUI::TextBox* p):t(p){}int operator()(const std::string& s)const{artisanCaption(t,s);return t->getTextSize().width;}};
int artisanWrap(MyGUI::TextBox* t,const std::string& text,int width){
 std::string wrapped=RerollPopupLayout::wrap(text,std::max(8,width),ArtisanTextMeasure(t));artisanCaption(t,wrapped);
 return std::max(t->getFontHeight()+4,t->getTextSize().height+6);
}
