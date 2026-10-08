#include "Localization.h"
#include <cassert>
#include <iostream>
namespace MyGUI {
struct GlyphInfo {unsigned int codePoint;GlyphInfo(unsigned int c):codePoint(c){}};
struct IResource {virtual ~IResource(){}template<class T>T* castType(bool){return dynamic_cast<T*>(this);}};
struct IFont:IResource {bool cjk;GlyphInfo glyph;IFont(bool c=false):cjk(c),glyph(0){}GlyphInfo* getGlyphInfo(unsigned int c){glyph.codePoint=(c<128||cjk)?c:0;return &glyph;}};
struct ResourceManager {std::map<std::string,IResource*> resources;static ResourceManager& getInstance(){static ResourceManager r;return r;}
 bool isExist(const char* n){return resources.count(n)!=0;}void load(const char*){static IFont unicode;resources["MercenarieUnicode"]=&unicode;}
 IResource* getByName(const std::string& n,bool){return resources.count(n)?resources[n]:0;}};
struct Widget {virtual ~Widget(){}virtual Widget* findWidget(const std::string&){return 0;}template<class T>T* castType(bool){return dynamic_cast<T*>(this);}};
struct TextBox:Widget {std::string font,caption;int height;std::map<std::string,std::string> data;TextBox():font("Original"),height(21){}
 const std::string& getFontName(){return font;}void setFontName(const std::string& n){font=n;height=99;}int getFontHeight(){return height;}void setFontHeight(int h){height=h;}
 bool isUserString(const char* k){return data.count(k)!=0;}void setUserString(const char* k,const std::string& v){data[k]=v;}const std::string& getUserString(const char* k){return data[k];}void setCaption(const std::string& c){caption=c;}};
struct Window:Widget {TextBox title;TextBox* getCaptionWidget(){return &title;}};
struct ListBox:Widget {bool visible;std::vector<TextBox*> rows;ListBox():visible(true){}bool getVisible(){return visible;}size_t getItemCount(){return rows.size();}Widget* getWidgetByIndex(size_t i){return i<rows.size()?rows[i]:0;}};
struct ComboBox:TextBox {ListBox list;size_t index;ComboBox():index(0){}size_t getIndexSelected(){return index;}Widget* findWidget(const std::string&){return &list;}};
}
namespace Ogre {struct ResourceGroupManager {static ResourceGroupManager& getSingleton(){static ResourceGroupManager r;return r;}bool resourceLocationExists(const char*,const char*){return true;}void addResourceLocation(const char*,const char*,const char*){}};}
static std::string mercenarieLocalize(const std::string& s){return s;}
#include "community-fonts-test.generated.h"
int main(){
    Loc::LanguagePack p;p.code="zh_tw";p.name="\xE7\xB9\x81";p.fontResource="CJK";Loc::registry().packs.push_back(p);
    MyGUI::IFont cjk(true);MyGUI::IResource wrongType;MyGUI::ResourceManager& resources=MyGUI::ResourceManager::getInstance();resources.resources["CJK"]=&cjk;resources.resources["notFont"]=&wrongType;
    MyGUI::TextBox text;Loc::engine().language="fr";MercenarieFonts::prepare(&text);assert(text.font=="Original");
    Loc::engine().language="pl";MercenarieFonts::prepare(&text);assert(text.font=="MercenarieUnicode"&&text.height==21);
    Loc::engine().language="zh_tw";MercenarieFonts::caption(&text,"hello");assert(text.font=="CJK"&&text.caption=="hello"&&text.height==21);
    assert(std::string(MercenarieFonts::newsFont("MercenarieNewsBody"))=="CJK");
    MyGUI::Window window;MercenarieFonts::prepare(&window);assert(window.title.font=="CJK");
    std::ostringstream diagnostic;MercenarieFonts::writeDiagnostics(diagnostic);assert(diagnostic.str().find("fontMissingCodepoints=0")!=std::string::npos);
    MyGUI::ComboBox combo;MyGUI::TextBox recycled;combo.index=5;combo.list.rows.resize(6,0);combo.list.rows[5]=&recycled;
    MercenarieFonts::languageChoice(&combo);assert(combo.font=="CJK"&&recycled.font=="CJK");
    combo.list.rows[5]=0;combo.list.rows[4]=&recycled;MercenarieFonts::languageChoice(&combo);assert(recycled.font=="MercenarieUnicode");
    Loc::registry().packs.back().fontResource="absent";MercenarieFonts::prepare(&text);assert(text.font=="MercenarieUnicode");
    std::ostringstream missing;MercenarieFonts::writeDiagnostics(missing);assert(missing.str().find("fontMissingCodepoints=1")!=std::string::npos);
    Loc::registry().packs.back().fontResource="notFont";assert(MercenarieFonts::fontFor("zh_tw")=="MercenarieUnicode");
    Loc::engine().language="en";MercenarieFonts::prepare(&text);assert(text.font=="Original"&&text.height==21);
    Loc::engine().language="ru";assert(std::string(MercenarieFonts::newsFont("MercenarieNewsTitle"))=="MercenarieNewsTitle");
    std::cout<<"PASS: production font adapter, existing IFont validation, missing/wrong-type fallback, official styling, restored font and height, CJK resource selection, recycled dropdown rows and glyph diagnostics\n";
}
