// Presentation only. All three buttons use the existing V9 answer dispatcher.
#include "RerollPopupLayout.h"
MyGUI::ScrollView* rerollPopupScroll=0;
MyGUI::TextBox *rerollPopupTitle=0,*rerollPopupBody=0;
MyGUI::IntSize rerollPopupViewport;
std::string rerollPopupLanguage;
bool rerollPopupActive=false;
void popupBorder(MyGUI::Widget* p,int w,int h,const MyGUI::Colour& c){
    registerSolid(p,0,0,w,1,c);registerSolid(p,0,h-1,w,1,c);registerSolid(p,0,0,1,h,c);registerSolid(p,w-1,0,1,h,c);
}
struct PopupMeasure {
    MyGUI::TextBox* label;
    explicit PopupMeasure(MyGUI::TextBox* p):label(p){}
    int operator()(const std::string& s)const{MercenarieFonts::caption(label,s);return label->getTextSize().width;}
};
int popupWrap(MyGUI::TextBox* label,const std::string& text,int width,bool penalty=false){
    std::string wrapped=RerollPopupLayout::wrap(text,width,PopupMeasure(label));
    MercenarieFonts::caption(label,penalty?RerollPopupLayout::penaltyColour(wrapped):wrapped);
    return std::max(label->getFontHeight()+4,label->getTextSize().height+6);
}
MyGUI::ImageBox* popupIcon(MyGUI::Widget* parent,int slot,int x,int y,int size,const MyGUI::Colour& colour){
    MyGUI::ImageBox* icon=parent->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);
    icon->setImageTexture("MercenariePopupIcons.png");icon->setImageCoord(MyGUI::IntCoord(slot*128,0,128,128));icon->setColour(colour);icon->setNeedMouseFocus(false);return icon;
}
MyGUI::Button* popupButton(MyGUI::Widget* p,int x,int y,int w,int h,int answer,const std::string& caption,int font,bool close=false){
    MyGUI::Button* b=p->createWidget<MyGUI::Button>("PanelEmpty",x,y,w,h,MyGUI::Align::Default);
    registerSolid(b,0,0,w,h,MyGUI::Colour(.055f,.065f,.070f));
    MyGUI::Colour edge=close?MyGUI::Colour(.58f,.57f,.49f):answer?MyGUI::Colour(.64f,.69f,.32f):MyGUI::Colour(.80f,.16f,.20f);
    popupBorder(b,w,h,edge);
    if(close)popupIcon(b,2,4,4,w-8,registerIvory);
    else {
        MyGUI::TextBox* t=registerText(b,0,0,w-64,h,font,"",registerIvory);
        int th=popupWrap(t,caption,w-70);int tw=std::min(w-70,t->getTextSize().width);
        int group=tw+font+12,start=(w-group)/2;
        popupIcon(b,answer?1:2,start,(h-font)/2,font,edge);
        t->setCoord(start+font+12,(h-th)/2,tw+4,th);t->setTextAlign(MyGUI::Align::Center);
    }
    b->setUserString("answer",answer?"1":"0");b->eventMouseButtonClick+=MyGUI::newDelegate(answerV9Confirmation);
    b->eventMouseSetFocus+=MyGUI::newDelegate(rerollButtonHover);b->eventMouseLostFocus+=MyGUI::newDelegate(rerollButtonLeave);
    b->eventMouseButtonPressed+=MyGUI::newDelegate(rerollButtonPress);b->eventMouseButtonReleased+=MyGUI::newDelegate(rerollButtonRelease);
    rerollButtonPaint(b);return b;
}
void layoutRerollPopup(){
    const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
    MyGUI::Widget* p=v9Confirmation->getClientWidget();
    while(p->getChildCount())MyGUI::Gui::getInstance().destroyWidget(p->getChildAt(0));
    RerollPopupLayout::Layout base(view.width,view.height);
    // Measure before drawing. The right-hand text remains scrollable if its
    // translation exceeds the viewport; no ellipsis or truncated translations.
    MyGUI::TextBox* measure=registerText(p,0,0,base.textW,2000,base.titleFont,"",registerIvory);
    int titleH=popupWrap(measure,Loc::text("v9.reroll.confirm_title"),base.textW);
    measure->setFontHeight(base.font);
    int bodyH=popupWrap(measure,Loc::text("v9.reroll.confirm_body"),base.textW);
    int buttonH=std::max(popupWrap(measure,Loc::text("v9.confirm"),base.buttonW-70),popupWrap(measure,Loc::text("v9.cancel"),base.buttonW-70));
    int headerH=popupWrap(measure,Loc::text("v9.confirm"),base.w-base.pad*3-36);
    MyGUI::Gui::getInstance().destroyWidget(measure);
    int textH=titleH+base.gap+bodyH;
    RerollPopupLayout::Layout l(view.width,view.height,textH,buttonH,headerH);
    v9Confirmation->setCoord((view.width-l.w)/2,(view.height-l.h)/2,l.w,l.h);
    registerSolid(p,0,0,l.w,l.h,MyGUI::Colour(.040f,.052f,.059f));
    popupBorder(p,l.w,l.h,registerAmber);
    registerSolid(p,1,l.header-1,l.w-2,1,MyGUI::Colour(.35f,.34f,.28f));
    MyGUI::TextBox* heading=registerText(p,l.pad,(l.header-headerH)/2,l.w-l.pad*3-36,headerH,l.font,"",registerAmber);
    popupWrap(heading,Loc::text("v9.confirm"),heading->getWidth());
    popupButton(p,l.w-l.pad-30,(l.header-30)/2,30,30,0,"",l.font,true);
    popupIcon(p,0,(l.left-l.icon)/2,l.contentY+(l.contentH-l.icon)/2,l.icon,registerAmber);
    registerSolid(p,l.left,l.contentY,1,l.contentH,MyGUI::Colour(.36f,.36f,.32f));
    rerollPopupScroll=p->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",l.textX,l.contentY,l.w-l.textX-l.pad,l.contentH,MyGUI::Align::Default);MercenarieNativeInput::bind(rerollPopupScroll);
    rerollPopupScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);rerollPopupScroll->setVisibleHScroll(false);
    rerollPopupScroll->setCanvasSize(l.textW,textH);
    rerollPopupTitle=registerText(rerollPopupScroll,0,0,l.textW,titleH,l.titleFont,"",registerIvory);
    popupWrap(rerollPopupTitle,Loc::text("v9.reroll.confirm_title"),l.textW);
    rerollPopupBody=registerText(rerollPopupScroll,0,titleH+l.gap,l.textW,bodyH,l.font,"",registerIvory);
    popupWrap(rerollPopupBody,Loc::text("v9.reroll.confirm_body"),l.textW,true);
    popupButton(p,l.pad,l.buttonY,l.buttonW,l.buttonH,1,Loc::text("v9.confirm"),l.buttonFont);
    popupButton(p,l.pad+l.buttonW+l.gap,l.buttonY,l.buttonW,l.buttonH,0,Loc::text("v9.cancel"),l.buttonFont);
    rerollPopupViewport=view;rerollPopupLanguage=Loc::engine().language;
}
void updateRerollPopupLayout(){
    if(!rerollPopupActive||!v9WidgetLive(v9Confirmation)||!v9Confirmation->getVisible())return;
    const MyGUI::IntSize v=MyGUI::RenderManager::getInstance().getViewSize();
    if(v.width!=rerollPopupViewport.width||v.height!=rerollPopupViewport.height||rerollPopupLanguage!=Loc::engine().language)layoutRerollPopup();
}
