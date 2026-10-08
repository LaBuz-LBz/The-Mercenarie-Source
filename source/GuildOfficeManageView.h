#pragma once
// Presentation only: the existing office callbacks and registry remain authoritative.
struct GuildManageLayout{int w,h;GuildManageLayout(int vw,int vh):w(std::min(716,vw-32)),h(std::min(430,vh-32)){} };
void gbmPaint(MyGUI::Widget* w,int state){
 const std::string role=w->getUserString("role");bool danger=role=="danger",primary=role=="primary";MyGUI::Colour edge=danger?MyGUI::Colour(.56f,.10f,.07f):(primary?gbpGold:MyGUI::Colour(.40f,.41f,.39f));MyGUI::Colour fill=danger?MyGUI::Colour(.095f,.008f,.004f):(primary?MyGUI::Colour(.12f,.10f,.075f):MyGUI::Colour(.07f,.075f,.075f));
 if(state==1){edge=danger?MyGUI::Colour(1,.27f,.20f):(primary?MyGUI::Colour(1,.75f,.35f):MyGUI::Colour(.72f,.73f,.70f));fill=danger?MyGUI::Colour(.20f,.018f,.012f):MyGUI::Colour(.18f,.16f,.12f);}if(state==2)fill=danger?MyGUI::Colour(.055f,0,0):MyGUI::Colour(.025f,.025f,.02f);
 for(size_t i=0;i<w->getChildCount();++i){MyGUI::Widget* c=w->getChildAt(i);if(c->getUserString("nameEdge")=="1")c->setColour(edge);if(c->getUserString("nameFill")=="1")c->setColour(fill);}
}
void gbmHover(MyGUI::Widget* w,MyGUI::Widget*){gbmPaint(w,1);}void gbmLeave(MyGUI::Widget* w,MyGUI::Widget*){gbmPaint(w,0);}void gbmPress(MyGUI::Widget* w,int,int,MyGUI::MouseButton){gbmPaint(w,2);}void gbmRelease(MyGUI::Widget* w,int,int,MyGUI::MouseButton){gbmPaint(w,1);}
MyGUI::TextBox* gbmFit(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& caption,const MyGUI::Colour& colour){MyGUI::TextBox* t=gbpText(p,x,y,w,h,size,caption,colour,true);const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);while(t->getTextSize().width>w&&size>14){size-=2;if(!pack||pack->official){std::ostringstream font;font<<"BuildingBody"<<size;t->setFontName(font.str());}t->setFontHeight(size);}t->setPosition(x,y+(h-size-2)/2);t->setSize(w,size+4);return t;}
MyGUI::Button* gbmAction(MyGUI::Widget* p,int x,int y,int w,int h,const char* key,const char* role,int icon,void(*callback)(MyGUI::Widget*),bool enabled=true){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>("PanelEmpty",x,y,w,h,MyGUI::Align::Default);b->setUserString("role",role);gbpImage(b,"MercenarieBuildingWhite.png",0,0,w,h)->setUserString("nameFill","1");gbnBorder(b,w,h,gbpGold);
 MyGUI::Colour colour=std::string(role)=="danger"?MyGUI::Colour(1,.22f,.16f):(std::string(role)=="primary"?gbpGold:gbpIvory);
 if(icon>=0){MyGUI::ImageBox* image=gbpImage(b,icon==3?"MercenariePopupIcons.png":"MercenarieManageActions.png",18,(h-38)/2,38,38);image->setImageCoord(MyGUI::IntCoord((icon==3?1:icon)*128,0,128,128));if(icon==3)image->setColour(MyGUI::Colour(.65f,.77f,.49f));}
 MyGUI::TextBox* t=gbmFit(b,icon>=0?72:12,0,w-(icon>=0?92:24),h,22,Loc::text(key),colour);t->setTextAlign(MyGUI::Align::Center);
 if(enabled){b->eventMouseButtonClick+=MyGUI::newDelegate(callback);b->eventMouseSetFocus+=MyGUI::newDelegate(gbmHover);b->eventMouseLostFocus+=MyGUI::newDelegate(gbmLeave);b->eventMouseButtonPressed+=MyGUI::newDelegate(gbmPress);b->eventMouseButtonReleased+=MyGUI::newDelegate(gbmRelease);}else b->setEnabled(false);gbmPaint(b,0);return b;
}
void gbmDraw(MyGUI::Widget* p,const GuildManageLayout& l,const std::string& name,bool active){
 gbpImage(p,"MercenarieBuildingSurface.png",0,0,l.w,l.h);gbnBorder(p,l.w,l.h,MyGUI::Colour(.42f,.38f,.30f));gbpImage(p,"LevelSun.png",17,12,66,66)->setColour(gbpGold);gbpSolid(p,1,82,l.w-2,1,gbpGold);
 std::string prefix=Loc::text("building.manage_prefix"),accent=Loc::text("building.manage_accent");int size=26;MyGUI::TextBox* a=gbpText(p,119,26,l.w-190,38,size,prefix,gbpIvory,true);MyGUI::TextBox* b=gbpText(p,119,26,l.w-190,38,size,accent,gbpGold,true);
 const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);while(a->getTextSize().width+b->getTextSize().width>l.w-190&&size>14){size-=2;if(!pack||pack->official){std::ostringstream f;f<<"BuildingBody"<<size;a->setFontName(f.str());b->setFontName(f.str());}a->setFontHeight(size);b->setFontHeight(size);}int first=a->getTextSize().width;a->setSize(first+1,38);b->setPosition(119+first,26);b->setSize(l.w-190-first,38);
 gbnAction(p,l.w-62,24,42,42,"v9.cancel",true,gbCancel,true);
 MyGUI::TextBox* title=gbpText(p,20,99,l.w-40,48,24,name,gbpIvory); if(title->getTextSize().width>l.w-40){const Loc::LanguagePack* language=Loc::registry().find(Loc::engine().language);if(!language||language->official)title->setFontName("BuildingBody18");title->setFontHeight(18);std::string row,wrapped;for(size_t i=0;i<name.size();){size_t end=i+1;while(end<name.size()&&(static_cast<unsigned char>(name[end])&0xC0)==0x80)++end;std::string next=row+name.substr(i,end-i);title->setCaption(next);if(!row.empty()&&title->getTextSize().width>l.w-40){wrapped+=row+"\n";row=name.substr(i,end-i);}else row=next;i=end;}title->setCaption(wrapped+row);}
 // Three equal action rows. Already-active status is derived from designatedGuildHouseKey.
 gbmAction(p,28,151,l.w-56,58,"building.rename","primary",0,gbRename);
 gbmAction(p,28,223,l.w-56,58,active?"ui.active_office_b188703":"ui.set_as_active_office","secondary",active?3:1,gbActivate,!active);
 gbmAction(p,28,295,l.w-56,58,"building.abandon","danger",2,gbAskAbandon);
 gbmAction(p,(l.w-310)/2,375,310,42,"v9.cancel","primary",-1,gbCancel);
}
