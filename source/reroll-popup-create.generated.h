void v9Confirm(const std::string& title,const std::string& body,const char* action,void (*callback)(int)){
    if(v9WidgetLive(bookRerollTip))bookRerollTip->setVisible(false);
    hideV9Confirmation();mercenarieDestroyLiveWidget(v9Confirmation);v9Confirmation=0;
    rerollPopupActive=callback==confirmReroll;
    if(rerollPopupActive){
        if(!MyGUI::ResourceManager::getInstance().isExist("MercenariePopupWindow"))MyGUI::ResourceManager::getInstance().load("MercenariePopup.xml");
        v9Confirmation=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("MercenariePopupWindow",0,0,900,460,MyGUI::Align::Default,"Popup");
        layoutRerollPopup();v9ConfirmationCallback=callback;
        MyGUI::InputManager::getInstance().addWidgetModal(v9Confirmation);return;
    }
    const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();int w=std::min(900,view.width-40),h=std::min(body.size()>320?520:340,view.height-40);
    v9Confirmation=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window");
    MercenarieFonts::caption(v9Confirmation,Loc::text("v9.confirm"));v9Confirmation->eventWindowButtonPressed+=MyGUI::newDelegate(closeV9Confirmation);
    MyGUI::Widget* parent=v9Confirmation->getClientWidget();applyMercenarieFrame(parent,true);
    MyGUI::TextBox* text=registerText(parent,20,20,parent->getWidth()-40,parent->getHeight()-100,21,body,registerIvory);text->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);bookWrap(text,title+"\n\n"+body);
    for(int i=0;i<2;++i){MyGUI::Button* button=parent->createWidget<MyGUI::Button>("Kenshi_Button1",20+i*(parent->getWidth()-30)/2,parent->getHeight()-64,(parent->getWidth()-50)/2,48,MyGUI::Align::Default);MercenarieFonts::caption(button,Loc::text(i?"v9.cancel":action));button->setFontHeight(20);button->setUserString("answer",i?"0":"1");button->eventMouseButtonClick+=MyGUI::newDelegate(answerV9Confirmation);}
    v9ConfirmationCallback=callback;MyGUI::InputManager::getInstance().addWidgetModal(v9Confirmation);
}