// Event-driven contract harness, NOT the Kenshi executable. Models MyGUI 3.2
// focus-only wheel delivery, no parent bubbling, popup layers and modal roots.
// Routing under test is MercenarieNativeInput.h, not a test reimplementation.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "native-input-fixtures.generated.h"
namespace MyGUI {
struct Widget;
struct IntPoint{int left,top;IntPoint(int x=0,int y=0):left(x),top(y){}};
struct IntSize{int width,height;IntSize(int w=0,int h=0):width(w),height(h){}};
struct IntCoord{int left,top,width,height;IntCoord(int x=0,int y=0,int w=0,int h=0):left(x),top(y),width(w),height(h){}};
template<class T>T newDelegate(T f){return f;}
struct WheelEvent{
    typedef void(*Callback)(Widget*,int);std::vector<Callback> callbacks;
    bool empty()const{return callbacks.empty();}
    void operator+=(Callback f){callbacks.push_back(f);}
    void operator=(Callback f){callbacks.clear();callbacks.push_back(f);}
    void operator()(Widget* w,int delta){std::vector<Callback> copy=callbacks;for(size_t i=0;i<copy.size();++i)copy[i](w,delta);}
};
struct FocusEvent{std::vector<void(*)(Widget*)> callbacks;void operator+=(void(*f)(Widget*)){callbacks.push_back(f);}void operator()(Widget* w){for(size_t i=0;i<callbacks.size();++i)callbacks[i](w);}};
struct Widget{
    Widget* parent;std::vector<Widget*> children;std::map<std::string,std::string> data;
    bool visible,enabled,pick,popup;int x,y,w,h,layer,z;std::string name;WheelEvent eventMouseWheel;
    Widget(Widget* p=0,int a=0,int b=0,int c=300,int d=200):parent(p),visible(true),enabled(true),pick(true),popup(false),x(a),y(b),w(c),h(d),layer(0),z(0){if(p)p->children.push_back(this);}
    virtual ~Widget(){if(parent){std::vector<Widget*>& v=parent->children;v.erase(std::find(v.begin(),v.end(),this));}}
    template<class T>T* castType(bool){return dynamic_cast<T*>(this);}
    Widget* getParent(){return parent;}bool getVisible(){return visible;}
    bool getInheritedEnabled(){return enabled&&(!parent||parent->getInheritedEnabled());}
    void setUserString(const char* k,const std::string& v){data[k]=v;}std::string getUserString(const char* k){return data[k];}
    Widget* findWidget(const std::string& n){for(size_t i=0;i<children.size();++i){if(children[i]->name==n)return children[i];if(Widget* w=children[i]->findWidget(n))return w;}return 0;}
    virtual void onWheel(int delta){eventMouseWheel(this,delta);}
    int left(){return x+(parent&&!popup?parent->left():0);}int top(){return y+(parent&&!popup?parent->top():0);}
    bool inside(int a,int b){return a>=left()&&b>=top()&&a<left()+w&&b<top()+h;}
    Widget* hit(int a,int b){if(!visible||!inside(a,b))return 0;for(size_t i=children.size();i>0;--i)if(!children[i-1]->popup)if(Widget* t=children[i-1]->hit(a,b))return t;return pick?this:0;}
    Widget* root(){Widget* r=this;while(r->parent)r=r->parent;return r;}
};
struct InputManager{
    Widget *mouse,*key,*modal;FocusEvent eventChangeMouseFocus;std::vector<Widget*> layerRoots;int serial;
    InputManager():mouse(0),key(0),modal(0),serial(0){}
    static InputManager& getInstance(){static InputManager i;return i;}
    Widget* getMouseFocusWidget(){return mouse;}Widget* getKeyFocusWidget(){return key;}
    void focus(Widget* w){mouse=w;eventChangeMouseFocus(w);}
    void move(int x,int y){Widget* best=0;Widget* surface=0;for(size_t i=0;i<layerRoots.size();++i){Widget* r=layerRoots[i];Widget* hit=r->hit(x,y);if(hit&&(!surface||r->layer>surface->layer||(r->layer==surface->layer&&r->z>surface->z))){best=hit;surface=r;}}if(best&&modal&&best->root()!=modal)best=0;focus(best);}
    void injectWheel(int delta){if(mouse&&mouse->getInheritedEnabled())mouse->onWheel(delta);}
    void raise(Widget* w){w->z=++serial;}void addModal(Widget* w){modal=w;raise(w);key=w;}
};
struct EditBox:Widget{bool textStatic;size_t range;EditBox(Widget* p=0):Widget(p),textStatic(false),range(0){}bool getEditStatic(){return textStatic;}size_t getVScrollRange(){return range;}};
struct ScrollBar:Widget{
    size_t wheelPage,position,page;int* linked;
    ScrollBar(Widget* p,const char* n,int* value):Widget(p),wheelPage(16),position(0),page(16),linked(value){name=n;}
    void setScrollWheelPage(size_t n){wheelPage=n;}
    void drag(int pos){position=pos;if(linked)*linked=-pos;}
    virtual void onWheel(int delta){if(wheelPage)drag(std::max(0,(int)position+(delta<0?1:-1)*(int)wheelPage));Widget::onWheel(delta);}
};
struct ScrollView:Widget{
    Widget client,canvas;IntPoint offset;IntSize extent;int updates;ScrollBar vertical,horizontal;
    ScrollView(Widget* p=0,int a=0,int b=0,int c=300,int d=200):Widget(p,a,b,c,d),client(this,0,0,c,d),canvas(&client,0,0,c,d),extent(c,1000),updates(0),vertical(this,"VScroll",&offset.top),horizontal(this,"HScroll",&offset.left){vertical.visible=horizontal.visible=false;client.eventMouseWheel+=legacy;canvas.eventMouseWheel+=legacy;}
    static void legacy(Widget* w,int delta){ScrollView* s=0;for(;w&&!s;w=w->parent)s=dynamic_cast<ScrollView*>(w);if(s){if(s->extent.height>s->h)s->offset.top+=delta<0?-50:50;else s->offset.left+=delta<0?-50:50;++s->updates;}}
    Widget* getClientWidget(){return &canvas;}IntPoint getViewOffset(){return offset;}IntSize getCanvasSize(){return extent;}IntCoord getViewCoord(){return IntCoord(0,0,w,h);}
    void setViewOffset(IntPoint p){offset=p;++updates;}
};
struct ListBox:Widget{int first;ListBox(Widget* p=0):Widget(p),first(0){}virtual void onWheel(int delta){first=std::max(0,std::min(20,first+(delta<0?1:-1)));Widget::onWheel(delta);}};
struct ComboBox:EditBox{
    Widget button,client;ListBox list;std::vector<std::string> items;size_t selected;int changed,accepted;
    ComboBox(Widget* p,int listLayer):EditBox(p),button(this),client(this),list(this),selected(0),changed(0),accepted(0){x=10;y=10;w=200;h=32;button.x=180;button.w=20;button.h=32;client.w=180;client.h=32;list.popup=true;list.layer=listLayer;list.x=10;list.y=42;list.w=200;list.h=150;list.visible=false;InputManager::getInstance().layerRoots.push_back(&list);}
    void press(){if(items.empty())return;list.visible=!list.visible;InputManager::getInstance().key=list.visible?static_cast<Widget*>(&list):static_cast<Widget*>(this);InputManager::getInstance().raise(root());}
    void accept(size_t n){assert(n<items.size());selected=n;++changed;++accepted;list.visible=false;InputManager::getInstance().key=this;}
};
}
#define MERCENARIE_NATIVE_INPUT_TEST
#include "MercenarieNativeInput.h"
int zoomCalls=0;
void zoom(MyGUI::Widget*,int){++zoomCalls;}
int main(){
 using namespace MyGUI;using namespace MercenarieNativeInput;
 InputManager& input=InputManager::getInstance();Widget window(0,0,0,900,700);window.layer=2;input.layerRoots.push_back(&window);input.addModal(&window);
 ScrollView a(&window,0,200,400,200),b(&window,420,200,400,200);Widget row(&a.canvas,0,0,400,200),rowB(&b.canvas,0,0,400,200);
 // Reproduce the old failure: wheel reaches a card, not the canvas.
 input.move(20,220);input.injectWheel(-120);assert(a.offset.top==0);
 bind(&a);bind(&b);input.move(20,220);input.injectWheel(-120);assert(a.offset.top==-50&&b.offset.top==0&&a.updates==1);
 input.injectWheel(120);assert(a.offset.top==0);input.injectWheel(120);assert(a.offset.top==0&&a.updates==2);
 for(int i=0;i<50;++i)input.injectWheel(-120);assert(a.offset.top==-800);int updates=a.updates;input.injectWheel(-120);assert(a.updates==updates);
 input.move(450,220);input.injectWheel(-120);assert(b.offset.top==-50&&a.offset.top==-800);
 input.focus(&a.canvas);a.offset=IntPoint(-70,-100);updates=a.updates;input.injectWheel(-120);assert(a.offset.top==-150&&a.offset.left==-70&&a.updates==updates+1);
 // Rebinding does not add a second canvas handler or another global listener.
 bind(&a);assert(a.canvas.eventMouseWheel.callbacks.size()==1&&input.eventChangeMouseFocus.callbacks.size()==1);
 a.extent=IntSize(900,100);input.injectWheel(-120);assert(a.offset.left==-70);a.extent.height=1000;
 ScrollView nested(&a.canvas,0,0,150,100);Widget inner(&nested.canvas);bind(&nested);input.focus(&inner);a.offset.top=-200;input.injectWheel(-120);assert(nested.offset.top==-50&&a.offset.top==-200);nested.offset.top=-900;input.injectWheel(-120);assert(nested.offset.top==-900&&a.offset.top==-200);
 // Bar wheel is vertical even over the horizontal thumb. Manual dragging lives.
 input.focus(&a.horizontal);int horizontal=a.offset.left;input.injectWheel(-120);assert(a.offset.left==horizontal&&a.offset.top==-250);a.horizontal.drag(130);assert(a.offset.left==-130);a.vertical.drag(340);assert(a.offset.top==-340&&a.vertical.page==16);
 Widget thumb(&a.vertical);input.focus(&thumb);input.injectWheel(120);assert(a.offset.top==-290);input.focus(&thumb);assert(thumb.eventMouseWheel.callbacks.size()==1);
 ScrollBar valueSlider(&a.canvas,"value",0);input.focus(&valueSlider);assert(valueSlider.wheelPage==16&&valueSlider.eventMouseWheel.empty());
 Widget map(&a.canvas);map.eventMouseWheel+=zoom;input.focus(&map);updates=a.updates;input.injectWheel(-120);assert(zoomCalls==1&&updates==a.updates);
 EditBox label(&a.canvas);label.textStatic=true;Widget text(&label);text.eventMouseWheel+=zoom;input.focus(&text);input.injectWheel(-120);assert(zoomCalls==1&&a.offset.top==-340);
 EditBox editor(&a.canvas);input.focus(&editor);assert(editor.eventMouseWheel.empty());
 // Native popup list retains its parent for modal eligibility. Same-layer
 // raising reproduces the invisible list; the shipped layer must fix picking.
 {ComboBox old(&window,2);old.items.push_back("All");old.press();input.move(20,60);assert(input.mouse!=&old.list);old.list.visible=false;input.layerRoots.pop_back();input.key=0;input.focus(0);}
 for(int menu=0;menu<6;++menu){ComboBox combo(&window,fixtureDropdownLayer);combo.items.push_back("All");combo.items.push_back("Second");combo.press();assert(combo.list.visible&&combo.list.parent==&combo);input.move(20,60);assert(input.mouse==&combo.list);input.injectWheel(-120);assert(combo.list.first==1);int before=a.offset.top;input.move(20,220);input.injectWheel(-120);assert(a.offset.top==before);combo.accept(1);assert(combo.selected==1&&combo.changed==1&&combo.accepted==1&&!combo.list.visible);input.key=&combo;input.layerRoots.pop_back();input.key=0;input.focus(0);}
 ListBox standalone(&a.canvas);Widget listRow(&standalone);input.focus(&listRow);assert(target(&listRow)==0&&listRow.eventMouseWheel.empty());
 // Refresh: only offsets and filter selection are kept; new cards get a new
 // binding, with no pointer registry retaining dead widgets.
 IntPoint saved=b.getViewOffset();{Widget transient(&b.canvas);input.focus(&transient);input.injectWheel(-120);saved=b.getViewOffset();input.focus(0);}b.setViewOffset(saved);{Widget replacement(&b.canvas);input.focus(&replacement);input.injectWheel(-120);assert(b.offset.top==saved.top-50);input.focus(0);}
 input.modal=&window;Widget behind(0,0,0,900,700);behind.layer=1;input.layerRoots.push_back(&behind);input.move(880,680);assert(input.mouse==&window);input.layerRoots.pop_back();
 std::cout<<"PASS production wheel router: focus delivery, up/down, bounds, two zones, nested priority, no horizontal fallback, bar drag, sliders/map/editor preserved, static text, dropdown suppression, refresh and no duplicate handlers\n";
 std::cout<<"PASS six dropdown event sequences with XML layer fixture: old-layer reproduction, modal parent, open, items, hit test, wheel, accept, callback count, close, selection retained\n";
 std::cout<<"LIMIT: event contract harness; not a Kenshi input-injection or GPU test\n";
}
