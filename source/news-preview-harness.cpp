#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include "MainMenuNewsRules.h"
#include "Localization.h"
namespace MyGUI {
struct Colour {float r,g,b;Colour(float x=1,float y=1,float z=1):r(x),g(y),b(z){}};
struct IntCoord {int left,top,width,height;IntCoord(int x=0,int y=0,int w=0,int h=0):left(x),top(y),width(w),height(h){}};
struct IntSize {int width,height;IntSize(int w=1920,int h=1080):width(w),height(h){}};
struct Align {enum {Default=0,Left=1,Top=2,Right=4,Bottom=8,HStretch=16,VStretch=32,Stretch=48,VCenter=64,Center=128};};
struct Event {int count;Event():count(0){}void operator+=(int){++count;}};
template<class T> int newDelegate(T){return 0;}
struct Widget;std::vector<Widget*> widgets;
struct Widget {
 int x,y,w,h,font,align;float alpha;std::string kind,caption,texture,fontName,name;Colour colour;IntCoord crop;bool visible,selected,mouse,wordWrap,multiline,readOnly,staticEdit;Widget* parent;std::map<std::string,std::string> userdata;
 Event eventMouseButtonClick,eventMouseSetFocus,eventMouseLostFocus;
 Widget():x(0),y(0),w(0),h(0),font(18),align(0),alpha(1),colour(),visible(true),selected(false),mouse(true),parent(0),wordWrap(false),multiline(false),readOnly(false),staticEdit(false){}
 template<class T>T* createWidget(const char* skin,int xx,int yy,int ww,int hh,int,const std::string& n="",const std::string& rootName=""){
 T* t=new T;t->parent=this;t->kind=skin;t->x=x+xx;t->y=y+yy;t->w=ww;t->h=hh;t->name=rootName.empty()?n:rootName;widgets.push_back(t);return t;}
 void setCaption(const std::string& s){caption=s;}void setFontHeight(int n){font=n;}void setFontName(const char* s){fontName=s;}
 void setTextColour(Colour c){colour=c;}void setTextAlign(int n){align=n;}void setNeedMouseFocus(bool b){mouse=b;}void setColour(Colour c){colour=c;}
 void setImageTexture(const char* s){texture=s;}void setImageCoord(IntCoord c){crop=c;}void setStateSelected(bool b){selected=b;}void setVisible(bool b){visible=b;}
 void setAlpha(float a){alpha=a;}void setInheritsAlpha(bool){}void setInheritsPick(bool){}void setUserString(const char* k,const std::string& v){userdata[k]=v;}void setProperty(const char*,const char*){assert(false && "Unsupported widget property in strict news harness");}void setNeedKeyFocus(bool){}
 void setEnabled(bool){}
 std::string getUserString(const char* k){return userdata[k];}IntCoord getAbsoluteCoord(){return IntCoord(x,y,w,h);}bool getStateSelected(){return selected;}template<class T>T* castType(bool){return static_cast<T*>(this);}
 int getWidth(){return w;}int getHeight(){return h;}bool shown(){return visible&&(!parent||parent->shown());}
};
struct Button:Widget{};struct TextBox:Widget{};struct ImageBox:Widget{};
struct EditBox:TextBox {void setEditWordWrap(bool v){wordWrap=v;}void setEditMultiLine(bool v){multiline=v;}void setEditReadOnly(bool v){readOnly=v;}void setEditStatic(bool v){staticEdit=v;}};
struct ScrollView:Widget{void setVisibleHScroll(bool){}void setCanvasAlign(int){}void setCanvasSize(int,int){}};
struct Gui:Widget {static Gui& getInstance(){static Gui g;return g;}static Gui* getInstancePtr(){return &getInstance();}void destroyWidget(Widget* w){w->visible=false;}};
struct RenderManager {IntSize view;static RenderManager& getInstance(){static RenderManager r;return r;}const IntSize& getViewSize(){return view;}};
struct InputManager {Widget* modal;InputManager():modal(0){}static InputManager& getInstance(){static InputManager m;return m;}static InputManager* getInstancePtr(){return &getInstance();}void addWidgetModal(Widget* w){modal=w;}void removeWidgetModal(Widget* w){if(modal==w)modal=0;}};
}
namespace MercenarieFonts {inline void caption(MyGUI::Widget* w,const std::string& s){if(Loc::engine().language=="pl"||Loc::engine().language=="ru")w->setFontName("MercenarieUnicode");w->setCaption(s);}inline const char* newsFont(const char* original){return (Loc::engine().language=="pl"||Loc::engine().language=="ru")&&std::string(original)!="MercenarieNewsTitle"?"MercenarieUnicode":original;}}
namespace MainMenuNews {
#include "news-preview-parts.generated.h"
MyGUI::Widget *modalRoot=0,*popup=0,*summaryPanel=0,*changelogPanel=0,*tooltip=0;
MyGUI::Button *dismissCheck=0,*continueButton=0,*changelogButton=0,*returnButton=0,*manualButton=0;
int lastWidth=0,lastHeight=0;
bool pendingClose=false;int persisted=0;
bool persistDismissed(){++persisted;return true;}void ErrorLog(const char*){}
void ensureResources(){}void DebugLog(const char*){}
#include "news-preview-interactions.generated.h"
#include "tests/NativeInputLayoutStub.h"
#include "MainMenuNewsView.h"
}
int main(int argc,char** argv){
    Loc::configure("Localization",argc>1?argv[1]:"fr");
    if(argc>4)MyGUI::RenderManager::getInstance().view=MyGUI::IntSize(atoi(argv[3]),atoi(argv[4]));
    if(argc>5)MainMenuNews::selectedVersion=atoi(argv[5]);
    MainMenuNews::createModal();
    assert(MyGUI::InputManager::getInstance().modal==MainMenuNews::modalRoot);
    int cards=0,tooltips=0;
    std::ofstream out(argc>2?argv[2]:"news-preview.tsv");
    for(size_t i=0;i<MyGUI::widgets.size();++i){MyGUI::Widget* t=MyGUI::widgets[i];
        if(t->kind=="MercenarieNewsCard"){++cards;tooltips+=t->eventMouseSetFocus.count;assert(t->eventMouseLostFocus.count==1);}
        if(t->kind=="Kenshi_WordWrapEmpty"){assert(t->wordWrap&&t->multiline&&t->readOnly&&t->staticEdit&&!t->mouse);if(t->parent&&t->parent->kind=="MercenarieNewsCard"){assert(t->x>=t->parent->x&&t->y>=t->parent->y&&t->x+t->w<=t->parent->x+t->parent->w&&t->y+t->h<=t->parent->y+t->parent->h);}}
        if(!t->shown())continue;
        std::string value=t->caption;for(size_t j=0;j<value.size();++j)if(value[j]=='\n')value[j]='~';
        out<<t->kind<<'\t'<<t->x<<'\t'<<t->y<<'\t'<<t->w<<'\t'<<t->h<<'\t'<<t->font<<'\t'<<t->align<<'\t'<<(int)(t->colour.r*255)<<'\t'<<(int)(t->colour.g*255)<<'\t'<<(int)(t->colour.b*255)<<'\t'<<t->texture<<'\t'<<t->crop.left<<'\t'<<t->crop.top<<'\t'<<t->crop.width<<'\t'<<t->crop.height<<'\t'<<value<<'\t'<<t->fontName<<'\t'<<t->alpha<<'\t'<<t->wordWrap<<'\t'<<t->multiline<<'\n';
    }
    assert(cards==MainMenuNews::pageItemCount()&&tooltips==cards);
    assert(MainMenuNews::continueButton->eventMouseButtonClick.count==1&&MainMenuNews::dismissCheck->eventMouseButtonClick.count==1&&MainMenuNews::changelogButton->eventMouseButtonClick.count==1);
    using namespace MainMenuNews;
    std::vector<MyGUI::Widget*> cardWidgets;
    for(size_t i=0;i<MyGUI::widgets.size();++i)if(MyGUI::widgets[i]->kind=="MercenarieNewsCard")cardWidgets.push_back(MyGUI::widgets[i]);
    for(int i=0;i<pageItemCount();++i){
        showTooltip(cardWidgets[i],0);assert(tooltip&&tooltip->shown());
        assert(tooltip->x>=0&&tooltip->y>=0&&tooltip->x+tooltip->w<=MyGUI::RenderManager::getInstance().view.width&&tooltip->y+tooltip->h<=MyGUI::RenderManager::getInstance().view.height);
        int live=0;bool content=false;
        for(size_t j=0;j<MyGUI::widgets.size();++j){MyGUI::Widget* w=MyGUI::widgets[j];if(w->name=="MercenarieNewsTooltip"&&w->shown())++live;if(w->parent==tooltip&&w->caption==Loc::text(pageItems()[i].tooltipText))content=true;}
        assert(live==1&&content); // Includes A -> B replacement without leave.
    }
    hideTooltip(cardWidgets[5],0);assert(!tooltip);
    showTooltip(cardWidgets[0],0);showChangelog(0);assert(!tooltip&&!summaryPanel->visible&&changelogPanel->visible&&returnButton->visible&&modalRoot->shown());
    bool localizedChangelog=false;for(size_t j=0;j<MyGUI::widgets.size();++j)if(MyGUI::widgets[j]->caption==Loc::text(pageChangelog()))localizedChangelog=true;
    assert(localizedChangelog);showSummary(0);assert(summaryPanel->visible&&!changelogPanel->visible&&!returnButton->visible&&continueButton->visible);
    checkPressed(dismissCheck);assert(dismissCheck->selected);checkPressed(dismissCheck);assert(!dismissCheck->selected);
    showTooltip(cardWidgets[0],0);closePressed(0);assert(!tooltip&&pendingClose&&!modalRoot->shown()&&persisted==0);destroyModal();assert(!modalRoot&&!MyGUI::InputManager::getInstance().modal);
    createModal();checkPressed(dismissCheck);closePressed(0);assert(persisted==1);destroyModal();
    std::cout<<"PASS: actual view created; six cards and hover bindings, checkbox, continue, changelog, native modal.\n";
    std::cout<<"PASS: production callbacks: six localized tooltips, A/B switch, one tooltip, leave, close cleanup; changelog/return; checkbox/close persistence.\n";
}
