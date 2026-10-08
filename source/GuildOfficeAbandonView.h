#pragma once
struct GuildAbandonLayout{int w,h;GuildAbandonLayout(int vw,int vh):w(std::min(728,vw-32)),h(std::min(410,vh-32)){} };
void gbaParagraph(MyGUI::Widget* p,int y,int h,int size,const char* key){MyGUI::TextBox* t=gbpText(p,148,y,p->getWidth()-174,h,size,Loc::text(key),gbpIvory);gbpWrap(t,Loc::text(key));while(t->getTextSize().height>h&&size>14){size-=2;const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);if(!pack||pack->official){std::ostringstream f;f<<"BuildingBody"<<size;t->setFontName(f.str());}t->setFontHeight(size);gbpWrap(t,Loc::text(key));}}
void gbaDraw(MyGUI::Widget* p,const GuildAbandonLayout& l){
 gbpImage(p,"MercenarieBuildingSurface.png",0,0,l.w,l.h);gbnBorder(p,l.w,l.h,MyGUI::Colour(.42f,.38f,.30f));gbpImage(p,"LevelSun.png",12,6,70,70)->setColour(gbpGold);gbpSolid(p,1,76,l.w-2,1,gbpGold);
 int size=26;MyGUI::TextBox* a=gbpText(p,116,25,l.w-186,38,size,Loc::text("building.abandon_prefix"),gbpIvory,true);MyGUI::TextBox* b=gbpText(p,116,25,l.w-186,38,size,Loc::text("building.abandon_accent"),gbpGold,true);const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);while(a->getTextSize().width+b->getTextSize().width>l.w-186&&size>14){size-=2;if(!pack||pack->official){std::ostringstream f;f<<"BuildingBody"<<size;a->setFontName(f.str());b->setFontName(f.str());}a->setFontHeight(size);b->setFontHeight(size);}int first=a->getTextSize().width;a->setSize(first+1,38);b->setPosition(116+first,25);b->setSize(l.w-186-first,38);
 gbnAction(p,l.w-56,18,38,38,"v9.cancel",true,gbCancel,true);gbpImage(p,"MercenarieAbandonWarning.png",27,109,94,94);
 gbaParagraph(p,105,42,22,"building.abandon_question");gbaParagraph(p,155,104,20,"building.abandon_details");gbaParagraph(p,268,52,20,"building.abandon_warning");
 int left=(l.w-66)*58/100;gbmAction(p,20,l.h-76,left,58,"building.abandon_confirm","danger",2,gbConfirmAbandon);
 MyGUI::Button* cancel=gbmAction(p,40+left,l.h-76,l.w-left-60,58,"v9.cancel","secondary",-1,gbCancel);MyGUI::ImageBox* icon=gbpImage(cancel,"MercenariePopupIcons.png",22,14,30,30);icon->setImageCoord(MyGUI::IntCoord(256,0,128,128));icon->setColour(gbpIvory);
 for(size_t i=0;i<cancel->getChildCount();++i){MyGUI::TextBox* label=cancel->getChildAt(i)->castType<MyGUI::TextBox>(false);if(label){label->setPosition(62,label->getTop());label->setSize(cancel->getWidth()-74,label->getHeight());}}
}
