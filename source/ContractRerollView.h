// Shared textured control for both native contract-card presentations.
void bookRerollTooltip(MyGUI::Widget*,const MyGUI::ToolTipInfo&);
void rerollClassicRow(MyGUI::Widget*);
void rerollButtonPaint(MyGUI::Widget* button){
    const std::string state=button->getUserString("rerollVisual");
    button->setAlpha(!button->getEnabled()?.38f:state=="pressed"?.55f:state=="hover"?1.0f:.8f);
}
void rerollButtonHover(MyGUI::Widget* button,MyGUI::Widget*){button->setUserString("rerollVisual","hover");rerollButtonPaint(button);}
void rerollButtonLeave(MyGUI::Widget* button,MyGUI::Widget*){button->setUserString("rerollVisual","");rerollButtonPaint(button);}
void rerollButtonPress(MyGUI::Widget* button,int,int,MyGUI::MouseButton){button->setUserString("rerollVisual","pressed");rerollButtonPaint(button);}
void rerollButtonRelease(MyGUI::Widget* button,int,int,MyGUI::MouseButton){button->setUserString("rerollVisual","hover");rerollButtonPaint(button);}
MyGUI::Button* createContractRerollButton(MyGUI::Widget* row,int slot,bool book){
    MyGUI::Button* button=row->createWidget<MyGUI::Button>("TheMercenarie_ContractReroll",row->getWidth()-34,3,30,30,MyGUI::Align::Default);
    button->setUserString(book?"bookRow":"offerSlot",registerNumber(slot));
    button->eventMouseButtonClick+=MyGUI::newDelegate(book?rerollBookRow:rerollClassicRow);
    button->setNeedMouseFocus(true);button->setNeedToolTip(true);
    button->eventToolTip+=MyGUI::newDelegate(bookRerollTooltip);
    button->eventMouseSetFocus+=MyGUI::newDelegate(rerollButtonHover);
    button->eventMouseLostFocus+=MyGUI::newDelegate(rerollButtonLeave);
    button->eventMouseButtonPressed+=MyGUI::newDelegate(rerollButtonPress);
    button->eventMouseButtonReleased+=MyGUI::newDelegate(rerollButtonRelease);
    return button;
}
void refreshContractRerollButton(MyGUI::Button* button,bool offer,bool marked){
    button->setCoord(button->getParent()->getWidth()-34,3,30,30);
    button->setVisible(offer);button->setEnabled(offer&&guildRerolls.remaining>0);
    button->setUserString("rerolled",marked?"1":"0");rerollButtonPaint(button);
}
