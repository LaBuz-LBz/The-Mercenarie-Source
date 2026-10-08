#pragma once
// UI-only wheel routing. Native ComboBox/ListBox input and scroll-bar dragging
// remain owned by MyGUI. No cached widget pointers survive a refresh.
#ifndef MERCENARIE_NATIVE_INPUT_TEST
#include <mygui/MyGUI_InputManager.h>
#include <mygui/MyGUI_ScrollView.h>
#include <mygui/MyGUI_ScrollBar.h>
#include <mygui/MyGUI_ComboBox.h>
#include <mygui/MyGUI_ListBox.h>
#endif
namespace MercenarieNativeInput {
inline bool dropdownOpen(){
    for(MyGUI::Widget* p=MyGUI::InputManager::getInstance().getKeyFocusWidget();p;p=p->getParent()){
        if(p->castType<MyGUI::ListBox>(false)&&p->getVisible()){
            for(MyGUI::Widget* owner=p->getParent();owner;owner=owner->getParent())
                if(owner->castType<MyGUI::ComboBox>(false))return true;
        }
    }
    return false;
}
inline MyGUI::ScrollView* target(MyGUI::Widget* widget){
    for(MyGUI::Widget* p=widget;p;p=p->getParent()){
        // ListBox owns its row scrolling. Never forward it to a window behind.
        if(p->castType<MyGUI::ListBox>(false)||p->castType<MyGUI::ComboBox>(false))return 0;
        if(MyGUI::ScrollView* scroll=p->castType<MyGUI::ScrollView>(false))
            return scroll->getUserString("MercenarieWheelScope")=="1"?scroll:0;
    }
    return 0;
}
inline int wheelTop(int top,int canvas,int viewport,int delta){
    if(!delta)return top;
    const int limit=std::max(0,canvas-viewport);
    const int ticks=std::max(1,std::min(8,(delta<0?-delta:delta)/120));
    return std::max(-limit,std::min(0,top+(delta>0?50:-50)*ticks));
}
inline void wheel(MyGUI::Widget* sender,int delta){
    if(!delta||!sender||!sender->getInheritedEnabled()||dropdownOpen())return;
    MyGUI::ScrollView* scroll=target(sender);if(!scroll)return;
    const MyGUI::IntSize canvas=scroll->getCanvasSize();
    const MyGUI::IntCoord viewport=scroll->getViewCoord();
    // No horizontal fallback and no bubbling at the innermost view's limit.
    MyGUI::IntPoint offset=scroll->getViewOffset();
    if(canvas.height<=viewport.height)return;
    const int next=wheelTop(offset.top,canvas.height,viewport.height,delta);
    if(next==offset.top)return;
    offset.top=next;scroll->setViewOffset(offset);
}
inline void focus(MyGUI::Widget* widget){
    if(!widget)return;
    MyGUI::ScrollView* scroll=target(widget);if(!scroll)return;
    for(MyGUI::Widget* p=widget;p&&p!=scroll;p=p->getParent()){
        if(MyGUI::ScrollBar* bar=p->castType<MyGUI::ScrollBar>(false)){
            // Only a view's V/H bars: never turn an options/value slider into
            // a content scroll control. Native wheel step is zero; dragging
            // and button/page steps keep their original values and callbacks.
            MyGUI::Widget* owner=bar->getParent();
            while(owner&&owner!=scroll&&owner!=scroll->getClientWidget())owner=owner->getParent();
            if(owner!=scroll)return;
            bar->setScrollWheelPage(0);
            if(widget->getUserString("MercenarieWheelBound")!="1"){
                widget->eventMouseWheel+=MyGUI::newDelegate(wheel);
                widget->setUserString("MercenarieWheelBound","1");
            }
            return;
        }
        if(MyGUI::EditBox* edit=p->castType<MyGUI::EditBox>(false)){
            // Static wrapped labels (e.g. changelog) intercept wheel events
            // even when their own text fits. Real editors retain native input.
            if(!edit->getEditStatic()||edit->getVScrollRange()>1)return;
            widget->eventMouseWheel=MyGUI::newDelegate(wheel);return;
        }
    }
    if(widget->eventMouseWheel.empty())widget->eventMouseWheel+=MyGUI::newDelegate(wheel);
}
inline void bind(MyGUI::ScrollView* scroll){
    if(!scroll)return;
    scroll->setUserString("MercenarieWheelScope","1");
    // Replace ONLY the two native ScrollView canvas subscriptions. Adding a
    // second handler there would move twice and retain horizontal fallback.
    MyGUI::Widget* canvas=scroll->getClientWidget();
    if(canvas){
        canvas->eventMouseWheel=MyGUI::newDelegate(wheel);
        MyGUI::Widget* client=canvas->getParent();
        if(client&&client!=scroll)client->eventMouseWheel=MyGUI::newDelegate(wheel);
    }
    static bool listening=false;
    if(!listening){MyGUI::InputManager::getInstance().eventChangeMouseFocus+=MyGUI::newDelegate(focus);listening=true;}
    // Binding can occur while a view is being rebuilt under a stationary mouse.
    focus(MyGUI::InputManager::getInstance().getMouseFocusWidget());
}
}
