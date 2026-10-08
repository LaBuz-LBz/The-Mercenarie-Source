#pragma once
#include "MainMenuBackgroundRules.h"
#include <mygui/MyGUI_ISubWidgetRect.h>
namespace MainMenuBackground {
inline void update(){
    if(!MyGUI::Gui::getInstancePtr()||!MainMenuNews::mainMenuReady())return;
    MyGUI::Widget* button=MainMenuNews::findRootChild("ImportGameButton");
    if(!button||!button->getParent())return;
    MyGUI::ImageBox* background=button->getParent()->castType<MyGUI::ImageBox>(false);
    if(!background||background->getWidth()<=0||background->getHeight()<=0)return;
    MyGUI::ISubWidgetRect* image=background->getSubWidgetMain();
    if(!image)return;
    MyGUI::ImageBox* art=0;
    for(size_t i=0;i<background->getChildCount();++i){
        MyGUI::Widget* child=background->getChildAt(i);
        if(child->getName()=="MercenarieMainMenuArtwork")art=child->castType<MyGUI::ImageBox>(false);
    }
    if(!art){
        MainMenuNews::ensureResources();
        art=background->createWidget<MyGUI::ImageBox>("ImageBox",0,0,1,1,MyGUI::Align::Default,"MercenarieMainMenuArtwork");
        art->setImageTexture("MercenarieMainMenuV9.png");
        art->setNeedMouseFocus(false);art->setNeedKeyFocus(false);
        // Higher child depths render below native controls and the credits panel.
        art->setDepth(10000);
    }
    art->setCoord(0,0,background->getWidth(),background->getHeight());
    const MainMenuBackgroundRules::Crop p=MainMenuBackgroundRules::cover(background->getWidth(),background->getHeight());
    art->getSubWidgetMain()->_setUVSet(MyGUI::FloatRect(p.left,p.top,p.right,p.bottom));
    // No cached pointers; destruction/recreation follows the native menu parent.
}
}
