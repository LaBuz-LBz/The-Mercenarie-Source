// Production picker drawing shared by the preview harness. No game mutation here.
#include "GuildBuildingPickerLayout.h"
MyGUI::Colour gbpGold(1.f,.60f,.18f),gbpIvory(.80f,.79f,.74f),gbpGrey(.36f,.37f,.35f);
MyGUI::TextBox *gbpDescriptionTitle=0,*gbpDescriptionBody=0;
MyGUI::ImageBox* gbpDescriptionArt=0;
MyGUI::Button* gbpCards[6]={0,0,0,0,0,0};
void gbpEnsure(){
 Ogre::ResourceGroupManager& resources=Ogre::ResourceGroupManager::getSingleton();if(!resources.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))resources.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");
 if(!MyGUI::ResourceManager::getInstance().isExist("BuildingBody18"))MyGUI::ResourceManager::getInstance().load("MercenarieBuildingPicker.xml");
}
MyGUI::ImageBox* gbpImage(MyGUI::Widget* p,const char* file,int x,int y,int w,int h){MyGUI::ImageBox* image=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,w,h,MyGUI::Align::Default);image->setImageTexture(file);image->setNeedMouseFocus(false);return image;}
void gbpSolid(MyGUI::Widget* p,int x,int y,int w,int h,const MyGUI::Colour& colour){MyGUI::ImageBox* image=gbpImage(p,"MercenarieBuildingWhite.png",x,y,w,h);image->setColour(colour);}
MyGUI::TextBox* gbpText(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& caption,const MyGUI::Colour& colour,bool title=false){
 MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",x,y,w,h,MyGUI::Align::Default);t->setCaption(mercenarieLocalize(caption));
 const Loc::LanguagePack* pack=Loc::registry().find(Loc::engine().language);
 if(!pack||pack->official){std::ostringstream name;name<<"Building"<<(title&&Loc::engine().language!="ru"&&Loc::engine().language!="pl"?"Brand":"Body")<<size;t->setFontName(name.str());}
 else MercenarieFonts::prepare(t);
 t->setFontHeight(size);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setTextColour(colour);t->setNeedMouseFocus(false);return t;
}
void gbpCaption(MyGUI::TextBox* t,const std::string& value){t->setCaption(mercenarieLocalize(value));}
void gbpWrap(MyGUI::TextBox* t,const std::string& value){
 std::istringstream paragraphs(value);std::string line,out;
 while(std::getline(paragraphs,line)){std::istringstream words(line);std::string word,row;while(words>>word){std::string next=row.empty()?word:row+" "+word;gbpCaption(t,next);if(!row.empty()&&t->getTextSize().width>t->getWidth()){out+=row+"\n";row=word;}else row=next;}out+=row+"\n";}
 if(!out.empty())out.erase(out.size()-1);gbpCaption(t,out);
}
void gbpFrame(MyGUI::Widget* p,int w,int h,int border,const MyGUI::Colour& colour,bool transparent=false){
 const int cuts[4]={0,8,56,64};const int xs[4]={0,border,w-border,w},ys[4]={0,border,h-border,h};
 for(int y=0;y<3;++y)for(int x=0;x<3;++x){if(transparent&&x==1&&y==1)continue;MyGUI::ImageBox* part=gbpImage(p,"MercenarieBuildingChrome.png",xs[x],ys[y],xs[x+1]-xs[x],ys[y+1]-ys[y]);part->setImageCoord(MyGUI::IntCoord(cuts[x],cuts[y],cuts[x+1]-cuts[x],cuts[y+1]-cuts[y]));part->setColour(colour);part->setUserString("pickerEdge","1");}
}
void gbpPaint(MyGUI::Widget* sender,int state){
 if(!sender)return;bool locked=sender->getUserString("locked")=="1";const MyGUI::Colour colour=locked?gbpGrey:state==2?MyGUI::Colour(.75f,.4f,.1f):state==1?MyGUI::Colour(1.f,.78f,.38f):gbpGold;
 for(size_t i=0;i<sender->getChildCount();++i){MyGUI::Widget* child=sender->getChildAt(i);if(child->getUserString("pickerRing")=="1")child->setColour(colour);}
}
void gbpEnter(MyGUI::Widget* sender,MyGUI::Widget*){
 gbpPaint(sender,1);if(sender&&gbpDescriptionTitle&&!sender->getUserString("descriptionTitle").empty()){
  gbpCaption(gbpDescriptionTitle,sender->getUserString("descriptionTitle"));gbpWrap(gbpDescriptionBody,sender->getUserString("description"));gbpDescriptionArt->setVisible(sender->getUserString("type")=="office");
 }
}
void gbpLeave(MyGUI::Widget* sender,MyGUI::Widget*){gbpPaint(sender,0);}
void gbpPress(MyGUI::Widget* sender,int,int,MyGUI::MouseButton){gbpPaint(sender,2);}
void gbpRelease(MyGUI::Widget* sender,int,int,MyGUI::MouseButton){gbpPaint(sender,1);}
void gbpBind(MyGUI::Button* b){b->eventMouseSetFocus+=MyGUI::newDelegate(gbpEnter);b->eventMouseLostFocus+=MyGUI::newDelegate(gbpLeave);b->eventMouseButtonPressed+=MyGUI::newDelegate(gbpPress);b->eventMouseButtonReleased+=MyGUI::newDelegate(gbpRelease);}
void gbpDraw(MyGUI::Widget* p,const GuildBuildingPicker::Layout& l,const std::string& reason){
 gbpEnsure();gbpImage(p,"MercenarieBuildingSurface.png",0,0,l.w,l.h);gbpFrame(p,l.w,l.h,l.px(8),MyGUI::Colour(.8f,.8f,.76f),true);
 gbpImage(p,"LevelSun.png",l.px(32),l.px(12),l.px(78),l.px(78))->setColour(gbpGold);
 const int titleFont=l.font(38);MyGUI::TextBox* title=gbpText(p,l.px(152),l.px(28),l.px(1090),l.px(56),titleFont,Loc::text("building.choose_prefix"),gbpIvory,true);
 int prefixWidth=title->getTextSize().width;gbpText(p,l.px(152)+prefixWidth,l.px(28),l.px(1090)-prefixWidth,l.px(56),titleFont,Loc::text("building.choose_accent"),gbpGold,true);
 MyGUI::Button* close=p->createWidget<MyGUI::Button>("PanelEmpty",l.w-l.px(78),l.px(24),l.px(54),l.px(54),MyGUI::Align::Default);gbpFrame(close,close->getWidth(),close->getHeight(),l.px(5),MyGUI::Colour(.75f,.71f,.60f));gbpImage(close,"MercenarieBuildingClose.png",l.px(13),l.px(13),l.px(28),l.px(28));close->eventMouseButtonClick+=MyGUI::newDelegate(gbCancel);
 for(int y=0;y<3;++y)gbpSolid(p,l.px(12),l.px(y==0?96:y==1?780:950),l.w-l.px(24),1,gbpGold);
 for(int i=0;i<6;++i){
  int x=l.px(76+(i%3)*443),y=l.px(112+(i/3)*334),cw=l.px(330),ch=l.px(322),diameter=l.px(286),cx=(cw-diameter)/2;
  const GuildBuildingTypes::Type& type=GuildBuildingTypes::types[i];bool locked=i!=0||!reason.empty();
  MyGUI::Button* card=p->createWidget<MyGUI::Button>("PanelEmpty",x,y,cw,ch,MyGUI::Align::Default);gbpCards[i]=card;card->setUserString("type",type.id);card->setUserString("locked",locked?"1":"0");card->setUserString("descriptionTitle",i?Loc::text("building.soon"):Loc::text("building.office_title"));card->setUserString("description",i?Loc::text("building.future_description"):(reason.empty()?std::string(Loc::text("building.office_body")):reason));
  gbpImage(card,"MercenarieBuildingMedallion.png",cx,0,diameter,diameter);
  if(i){const char* files[]={"MercenarieBuildingFuture1.png","MercenarieBuildingFuture2.png","MercenarieBuildingFuture3.png","MercenarieBuildingFuture4.png","MercenarieBuildingFuture5.png"};gbpImage(card,files[i-1],cx+l.px(24),l.px(24),diameter-l.px(48),diameter-l.px(48))->setColour(MyGUI::Colour(.28f,.28f,.28f));}
  else gbpImage(card,"MercenarieBuildingOfficeCutout.png",cx+l.px(38),l.px(35),diameter-l.px(76),diameter-l.px(70));
  if(locked)gbpImage(card,"MercenarieBuildingLock.png",(cw-l.px(88))/2,l.px(98),l.px(88),l.px(88));
  MyGUI::ImageBox* ring=gbpImage(card,"MercenarieBuildingRing.png",cx,0,diameter,diameter);ring->setUserString("pickerRing","1");ring->setColour(locked?gbpGrey:gbpGold);
  MyGUI::Widget* label=card->createWidget<MyGUI::Widget>("PanelEmpty",0,l.px(270),cw,l.px(52),MyGUI::Align::Default);label->setNeedMouseFocus(false);gbpFrame(label,cw,l.px(52),l.px(4),locked?MyGUI::Colour(.52f,.53f,.50f):gbpGold);
  MyGUI::TextBox* text=gbpText(label,l.px(8),l.px(9),cw-l.px(16),l.px(36),l.font(27),Loc::text(type.name),locked?gbpIvory:gbpGold,true);text->setTextAlign(MyGUI::Align::Center);
  gbpBind(card);card->eventMouseButtonClick+=MyGUI::newDelegate(gbSelectType);
 }
 gbpDescriptionArt=gbpImage(p,"MercenarieBuildingOfficeScene.png",l.px(1110),l.px(781),l.px(276),l.px(168));gbpDescriptionArt->setAlpha(.20f);
 gbpSolid(p,l.px(40),l.px(804),l.px(5),l.px(124),gbpGold);
 gbpDescriptionTitle=gbpText(p,l.px(68),l.px(800),l.px(1130),l.px(38),l.font(29),Loc::text("building.office_title"),gbpGold,true);
 gbpDescriptionBody=gbpText(p,l.px(68),l.px(840),l.px(1130),l.px(100),l.font(23),"",gbpIvory);gbpWrap(gbpDescriptionBody,(reason.empty()?std::string(Loc::text("building.office_body")):reason));
 MyGUI::Button* cancel=p->createWidget<MyGUI::Button>("PanelEmpty",l.px(460),l.px(974),l.px(480),l.px(64),MyGUI::Align::Default);gbpFrame(cancel,cancel->getWidth(),cancel->getHeight(),l.px(5),gbpGold);MyGUI::TextBox* caption=gbpText(cancel,0,l.px(8),cancel->getWidth(),l.px(48),l.font(30),Loc::text("v9.cancel"),gbpGold,true);caption->setTextAlign(MyGUI::Align::Center);cancel->eventMouseButtonClick+=MyGUI::newDelegate(gbCancel);gbpBind(cancel);
}

