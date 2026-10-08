#pragma once

// Never dereference a cached widget until it has been found in the live tree.
// The native menus can destroy their children before the save/new-game hooks.
inline MyGUI::Widget* mercenarieFindLiveWidget(MyGUI::EnumeratorWidgetPtr widgets, const MyGUI::Widget* target)
{
    while(widgets.next()){
        MyGUI::Widget* live=widgets.current();
        if(live==target)return live;
        MyGUI::Widget* child=mercenarieFindLiveWidget(live->getEnumerator(),target);
        if(child)return child;
    }
    return 0;
}
inline void mercenarieDestroyLiveWidget(MyGUI::Widget* cached)
{
    MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();
    if(!cached||!gui)return;
    MyGUI::Widget* live=mercenarieFindLiveWidget(gui->getEnumerator(),cached);
    if(!live)return;
    // Gui::destroyWidget owns roots only; a child belongs to its parent.
    if(live->getParent())live->getParent()->_destroyChildWidget(live);
    else gui->destroyWidget(live);
}
