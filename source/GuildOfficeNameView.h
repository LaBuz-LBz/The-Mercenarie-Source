#pragma once
// Shared presentation for creating and renaming an office. Existing callbacks own mutations.
struct GuildNameLayout {int w,h;GuildNameLayout(int vw,int vh):w(std::min(760,vw-32)),h(std::min(380,vh-32)){} };
MyGUI::Widget* gbnFieldFrame=0;
void gbnEnsure(){gbpEnsure();if(!MyGUI::ResourceManager::getInstance().isExist("MercenarieOfficeNameEdit"))MyGUI::ResourceManager::getInstance().load("MercenarieOfficeName.xml");}
void gbnBorder(MyGUI::Widget* p,int w,int h,const MyGUI::Colour& colour){
 const int coords[4][4]={{0,0,w,1},{0,h-1,w,1},{0,0,1,h},{w-1,0,1,h}};
 for(int i=0;i<4;++i){MyGUI::ImageBox* edge=gbpImage(p,"MercenarieBuildingWhite.png",coords[i][0],coords[i][1],coords[i][2],coords[i][3]);edge->setColour(colour);edge->setUserString("nameEdge","1");}
}
void gbnPaint(MyGUI::Widget* w,int state){
 bool primary=w->getUserString("primary")=="1";MyGUI::Colour edge=primary?gbpGold:MyGUI::Colour(.55f,.56f,.54f);MyGUI::Colour fill=primary?MyGUI::Colour(.12f,.10f,.075f):MyGUI::Colour(.075f,.08f,.08f);
 if(state==1){edge=primary?MyGUI::Colour(1,.76f,.36f):MyGUI::Colour(.8f,.81f,.78f);fill=primary?MyGUI::Colour(.21f,.15f,.085f):MyGUI::Colour(.14f,.15f,.15f);}
 if(state==2){edge=primary?MyGUI::Colour(.78f,.44f,.12f):MyGUI::Colour(.42f,.43f,.42f);fill=MyGUI::Colour(.035f,.035f,.035f);}
 for(size_t i=0;i<w->getChildCount();++i){MyGUI::Widget* c=w->getChildAt(i);if(c->getUserString("nameEdge")=="1")c->setColour(edge);if(c->getUserString("nameFill")=="1")c->setColour(fill);}
}
void gbnHover(MyGUI::Widget* w,MyGUI::Widget*){gbnPaint(w,1);}void gbnLeave(MyGUI::Widget* w,MyGUI::Widget*){gbnPaint(w,0);}
void gbnPress(MyGUI::Widget* w,int,int,MyGUI::MouseButton){gbnPaint(w,2);}void gbnRelease(MyGUI::Widget* w,int,int,MyGUI::MouseButton){gbnPaint(w,1);}
void gbnFocus(bool active){if(!gbnFieldFrame)return;for(size_t i=0;i<gbnFieldFrame->getChildCount();++i){MyGUI::Widget* c=gbnFieldFrame->getChildAt(i);if(c->getUserString("nameEdge")=="1")c->setColour(active?gbpGold:MyGUI::Colour(.40f,.41f,.39f));}}
void gbnFocusIn(MyGUI::Widget*,MyGUI::Widget*){gbnFocus(true);}void gbnFocusOut(MyGUI::Widget*,MyGUI::Widget*){gbnFocus(false);}
MyGUI::Button* gbnAction(MyGUI::Widget* p,int x,int y,int w,int h,const char* key,bool primary,void (*callback)(MyGUI::Widget*),bool close=false){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>("PanelEmpty",x,y,w,h,MyGUI::Align::Default);b->setUserString("primary",primary?"1":"0");
 MyGUI::ImageBox* fill=gbpImage(b,"MercenarieBuildingWhite.png",0,0,w,h);fill->setUserString("nameFill","1");gbnBorder(b,w,h,gbpGold);
 if(close){MyGUI::ImageBox* icon=gbpImage(b,"MercenariePopupIcons.png",9,9,w-18,h-18);icon->setImageCoord(MyGUI::IntCoord(256,0,128,128));icon->setColour(gbpGold);}
 else {
  const MyGUI::Colour colour=primary?gbpGold:gbpIvory;MyGUI::TextBox* label=gbpText(b,0,0,w-60,h,22,Loc::text(key),colour,true);int font=22;
  while(label->getTextSize().width>w-74&&font>18){font-=2;std::ostringstream resource;resource<<"BuildingBrand"<<font;const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);if(!pack||pack->official)label->setFontName(resource.str());label->setFontHeight(font);}
  int tw=label->getTextSize().width,start=std::max(12,(w-tw-46)/2);label->setPosition(start+46,(h-font-4)/2);label->setSize(w-start-54,font+8);
  MyGUI::ImageBox* icon=gbpImage(b,"MercenariePopupIcons.png",start,(h-30)/2,30,30);icon->setImageCoord(MyGUI::IntCoord((primary?1:2)*128,0,128,128));icon->setColour(colour);
 }
 b->eventMouseButtonClick+=MyGUI::newDelegate(callback);b->eventMouseSetFocus+=MyGUI::newDelegate(gbnHover);b->eventMouseLostFocus+=MyGUI::newDelegate(gbnLeave);b->eventMouseButtonPressed+=MyGUI::newDelegate(gbnPress);b->eventMouseButtonReleased+=MyGUI::newDelegate(gbnRelease);gbnPaint(b,0);return b;
}
void gbnDraw(MyGUI::Widget* p,const GuildNameLayout& l,bool rename){
 gbnEnsure();gbpImage(p,"MercenarieBuildingSurface.png",0,0,l.w,l.h);gbnBorder(p,l.w,l.h,MyGUI::Colour(.46f,.45f,.40f));
 gbpImage(p,"LevelSun.png",26,17,70,70)->setColour(gbpGold);gbpSolid(p,1,98,l.w-2,1,gbpGold);
 std::string prefix=Loc::text(rename?"building.rename_prefix":"building.name_prefix"),accent=Loc::text("building.name_accent");
 MyGUI::TextBox* left=gbpText(p,120,35,l.w-204,42,26,prefix,gbpIvory,true);MyGUI::TextBox* right=gbpText(p,120,35,l.w-204,42,26,accent,gbpGold,true);
 int font=26;while(left->getTextSize().width+right->getTextSize().width>l.w-204&&font>18){font-=2;std::ostringstream resource;resource<<"BuildingBrand"<<font;const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);if(!pack||pack->official){left->setFontName(resource.str());right->setFontName(resource.str());}left->setFontHeight(font);right->setFontHeight(font);}
 int prefixW=left->getTextSize().width;left->setSize(prefixW+2,42);right->setPosition(120+prefixW,35);right->setSize(l.w-204-prefixW,42);
 gbnAction(p,l.w-66,30,42,42,"v9.cancel",true,gbCancel,true);
 MyGUI::TextBox* hint=gbpText(p,36,128,l.w-72,30,20,Loc::text("building.name_hint"),gbpIvory);gbpWrap(hint,Loc::text("building.name_hint"));
 gbnFieldFrame=p->createWidget<MyGUI::Widget>("PanelEmpty",36,174,l.w-72,64,MyGUI::Align::Default);gbpSolid(gbnFieldFrame,0,0,l.w-72,64,MyGUI::Colour(.045f,.05f,.05f));gbnBorder(gbnFieldFrame,l.w-72,64,gbpGold);
 guildNameEdit=gbnFieldFrame->createWidget<MyGUI::EditBox>("MercenarieOfficeNameEdit",2,2,l.w-76,60,MyGUI::Align::Default);guildNameEdit->setMaxTextLength(64);guildNameEdit->setEditMultiLine(false);guildNameEdit->setEditWordWrap(false);
 const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);if(!pack||pack->official)guildNameEdit->setFontName("BuildingBody24");else MercenarieFonts::prepare(guildNameEdit);guildNameEdit->setFontHeight(24);guildNameEdit->setTextColour(MyGUI::Colour(.9f,.9f,.86f));
 guildNameEdit->eventKeyButtonPressed+=MyGUI::newDelegate(gbNameKey);guildNameEdit->eventKeySetFocus+=MyGUI::newDelegate(gbnFocusIn);guildNameEdit->eventKeyLostFocus+=MyGUI::newDelegate(gbnFocusOut);
 const int bw=(l.w-108)/2;guildNameConfirm=gbnAction(p,36,l.h-86,bw,60,"v9.confirm",true,confirmGuildHouseName);gbnAction(p,72+bw,l.h-86,bw,60,"v9.cancel",false,gbCancel);
}
