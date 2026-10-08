MyGUI::TextBox* artisanText(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text,const MyGUI::Colour& colour){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("ArtisanText",x,y,w,h,MyGUI::Align::Default);artisanSetFont(t,size);t->setTextColour(colour);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setNeedMouseFocus(false);int height=artisanWrap(t,text,w);t->setSize(w,std::max(h,height));return t;
}
MyGUI::Button* artisanButton(MyGUI::Widget* p,int x,int y,int w,const std::string& text,const char* action,int row=-1,const char* skin="ArtisanButton"){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>(skin,x,y,w,36,MyGUI::Align::Default);artisanSetFont(b,16);artisanWrap(b,text,w-16);b->setTextAlign(MyGUI::Align::Center);b->setUserString("action",action);b->setUserString("row",artisanNumber(row));b->eventMouseButtonClick+=MyGUI::newDelegate(artisanClick);return b;
}