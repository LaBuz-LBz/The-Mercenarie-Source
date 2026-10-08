#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>
namespace MyGUI {
struct Widget;
struct EnumeratorWidgetPtr {
 std::vector<Widget*> values;size_t pos;
 EnumeratorWidgetPtr(const std::vector<Widget*>& v):values(v),pos(0){}
 bool next(){return pos<values.size()?++pos,true:false;}
 Widget* current(){return values[pos-1];}
};
struct Widget {
 Widget* parent;std::vector<Widget*> children;int destroyed;
 Widget():parent(0),destroyed(0){}
 EnumeratorWidgetPtr getEnumerator(){assert(!destroyed);return EnumeratorWidgetPtr(children);}
 Widget* getParent(){assert(!destroyed);return parent;}
 void retire(){++destroyed;for(size_t i=0;i<children.size();++i)children[i]->retire();children.clear();}
 void _destroyChildWidget(Widget* w){assert(w->parent==this);children.erase(std::find(children.begin(),children.end(),w));w->retire();}
};
struct Gui {
 static Gui* instance;std::vector<Widget*> roots;
 static Gui* getInstancePtr(){return instance;}
 EnumeratorWidgetPtr getEnumerator(){return EnumeratorWidgetPtr(roots);}
 void destroyWidget(Widget* w){assert(!w->parent);roots.erase(std::find(roots.begin(),roots.end(),w));w->retire();}
};Gui* Gui::instance=0;
}
#include "SafeGuiCleanup.h"
int main(){
 MyGUI::Gui gui;MyGUI::Gui::instance=&gui;
 MyGUI::Widget root,child,other;
 gui.roots.push_back(&root);child.parent=&root;root.children.push_back(&child);
 mercenarieDestroyLiveWidget(&child);assert(child.destroyed==1&&root.destroyed==0);
 mercenarieDestroyLiveWidget(&child);assert(child.destroyed==1);
 other.parent=&root;root.children.push_back(&other);
 mercenarieDestroyLiveWidget(&root);assert(root.destroyed==1&&other.destroyed==1);
 mercenarieDestroyLiveWidget(&other);mercenarieDestroyLiveWidget(&root);
 mercenarieDestroyLiveWidget(reinterpret_cast<MyGUI::Widget*>(1));
 mercenarieDestroyLiveWidget(0);MyGUI::Gui::instance=0;mercenarieDestroyLiveWidget(&root);
 std::cout<<"PASS: root, child, native parent removal, repeated cleanup, stale pointer, null GUI\n";
}
