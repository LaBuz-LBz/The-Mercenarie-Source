#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "ContractReroll.h"
namespace MyGUI {
struct Widget;struct ToolTipInfo{};struct MouseButton{};
struct Colour {Colour(float=1,float=1,float=1){}};
struct Align {enum {Default=0,Right=1,Top=2};};
template<class T>T newDelegate(T value){return value;}
struct Event {template<class T>void operator+=(T){}};
struct Click {void(*callback)(Widget*);Click():callback(0){}void operator+=(void(*c)(Widget*)){callback=c;}};
struct Widget {
    Widget* parent;std::vector<Widget*> children;std::map<std::string,std::string> data;
    int x,y,w,h;bool visible,enabled,mouse,tooltip;float alpha;std::string skin;
    Click eventMouseButtonClick;Event eventToolTip,eventMouseSetFocus,eventMouseLostFocus,eventMouseButtonPressed,eventMouseButtonReleased;
    Widget():parent(0),x(0),y(0),w(0),h(0),visible(true),enabled(true),mouse(true),tooltip(false),alpha(1){}
    virtual ~Widget(){for(size_t i=0;i<children.size();++i)delete children[i];}
    template<class T>T* createWidget(const char* sk,int a,int b,int c,int d,int){T* t=new T;t->skin=sk;t->parent=this;t->setCoord(a,b,c,d);children.push_back(t);return t;}
    void setCoord(int a,int b,int c,int d){x=a;y=b;w=c;h=d;}void setVisible(bool v){visible=v;}void setEnabled(bool v){enabled=v;}bool getEnabled(){return enabled;}
    int getWidth(){return w;}Widget* getParent(){return parent;}void setAlpha(float v){alpha=v;}void setNeedMouseFocus(bool v){mouse=v;}void setNeedToolTip(bool v){tooltip=v;}
    void setUserString(const char* k,const std::string& v){data[k]=v;}std::string getUserString(const char* k){return data[k];}
    bool effectiveVisible(){return visible&&(!parent||parent->effectiveVisible());}
    Widget* hit(int a,int b){if(!visible)return 0;for(size_t i=children.size();i>0;--i){Widget* c=children[i-1];if(a>=c->x&&a<c->x+c->w&&b>=c->y&&b<c->y+c->h){Widget* found=c->hit(a-c->x,b-c->y);if(found)return found;}}return mouse&&enabled?this:0;}
};
struct Button:Widget {};
struct TextBox:Widget {void setTextAlign(int){}};
struct ImageBox:Widget {};
}
ContractReroll::Charges guildRerolls;
int clickedSlot=-1;bool clickedBook=false;
std::string registerNumber(int i){std::ostringstream s;s<<i;return s.str();}
void rerollBookRow(MyGUI::Widget* w){clickedSlot=atoi(w->getUserString("bookRow").c_str());clickedBook=true;}
void rerollClassicRow(MyGUI::Widget* w){clickedSlot=atoi(w->getUserString("offerSlot").c_str());clickedBook=false;}
void bookRerollTooltip(MyGUI::Widget*,const MyGUI::ToolTipInfo&){}
#include "ContractRerollView.h"
MyGUI::Colour registerIvory,registerAmber;
MyGUI::Button* contractButtons[6];MyGUI::Button* offerRerollV6[6];
MyGUI::TextBox *offerNameV6[6],*offerTypeV6[6],*offerRarityV6[6],*offerPriceV6[6];
MyGUI::ImageBox* offerIconV6[6];MyGUI::Widget* offerEdgesV6[6][4];
void contractOfferClicked(MyGUI::Widget*){}
MyGUI::Colour missionColourV6(int){return MyGUI::Colour();}
MyGUI::Widget* boardPanelV6(MyGUI::Widget* p,int x,int y,int w,int h){return p->createWidget<MyGUI::Widget>("WhiteSkin",x,y,w,h,0);}
MyGUI::TextBox* registerText(MyGUI::Widget* p,int x,int y,int w,int h,int,const char*,MyGUI::Colour){MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("text",x,y,w,h,0);t->setNeedMouseFocus(false);return t;}
MyGUI::ImageBox* boardIconV6(MyGUI::Widget* p,int,int x,int y,int w,MyGUI::Colour){MyGUI::ImageBox* t=p->createWidget<MyGUI::ImageBox>("icon",x,y,w,w,0);t->setNeedMouseFocus(false);return t;}
MyGUI::Widget* registerSolid(MyGUI::Widget* p,int x,int y,int w,int h,MyGUI::Colour){MyGUI::Widget* t=p->createWidget<MyGUI::Widget>("edge",x,y,w,h,0);t->setNeedMouseFocus(false);return t;}
#include "reroll-native-cards.generated.h"
int main(){
    const int widths[]={1024,1280,1920,2560},heights[]={768,720,1080,1440};
    for(int resolution=0;resolution<4;++resolution)for(int refresh=0;refresh<3;++refresh){
        MyGUI::Widget list;createActualClassicCards(&list,widths[resolution],heights[resolution]);
        for(int charges=2;charges>=0;--charges){guildRerolls.remaining=charges;
            for(int i=0;i<6;++i){MyGUI::Button* b=offerRerollV6[i];MyGUI::Widget* row=b->parent;
                bool available=i!=5;refreshContractRerollButton(b,available,false);
                assert(b->skin=="TheMercenarie_ContractReroll"&&b->parent==contractButtons[i]->parent);
                assert(b->effectiveVisible()==available&&b->enabled==(available&&charges>0)&&b->alpha>0);
                assert(b->w==30&&b->h==30&&b->x>=0&&b->y>=0&&b->x+b->w<=row->w&&b->y+b->h<=row->h);
                assert(offerNameV6[i]->x+offerNameV6[i]->w<=b->x&&offerTypeV6[i]->x+offerTypeV6[i]->w<=b->x);
                assert(offerPriceV6[i]->y>=b->y+b->h);
                if(available&&charges>0){MyGUI::Widget* hit=row->hit(b->x+10,b->y+10);assert(hit==b&&b->eventMouseButtonClick.callback);hit->eventMouseButtonClick.callback(hit);assert(clickedSlot==i&&!clickedBook);}
                if(available&&!charges)assert(row->hit(b->x+10,b->y+10)==contractButtons[i]);
            }
        }
        MyGUI::Widget row;row.w=300;row.h=75;MyGUI::Button* b=createContractRerollButton(&row,3,true);
        guildRerolls.remaining=2;refreshContractRerollButton(b,true,false);b->eventMouseButtonClick.callback(b);assert(clickedBook&&clickedSlot==3);
        rerollButtonHover(b,0);assert(b->alpha==1);rerollButtonPress(b,0,0,MyGUI::MouseButton());assert(b->alpha<1);rerollButtonRelease(b,0,0,MyGUI::MouseButton());assert(b->alpha==1);
        guildRerolls.remaining=0;refreshContractRerollButton(b,true,false);assert(b->visible&&!b->enabled&&b->alpha>.3f);
    }
    std::cout<<"PASS: actual classic-card creation at four resolutions, both textured buttons, 2/1/0 states, timer excluded, parent/alpha/clipping/hit-order, correct callback slot and rebuilds\n";
}
