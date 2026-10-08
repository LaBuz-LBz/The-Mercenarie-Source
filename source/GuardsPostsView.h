// Post editor: native V10 actions remain the single path to model mutations.
MyGUI::ImageBox* postDirection=0;
std::map<GuildGuards::Id,MyGUI::ImageBox*> postIndicators;
void postClick(MyGUI::Widget* w){
 const std::string action=w->getUserString("guardAction");
 if(action=="priority_less"||action=="priority_more"){int value=1;std::istringstream in(priorityEdit->getOnlyText());in>>value;value=std::max(1,std::min(999999,value));priorityEdit->setOnlyText(number(std::max(1,value+(action=="priority_less"?-1:1))));return;}
 if(action=="position"||action=="add_post"){
  Character* c=selected();if(!c||!c->isPlayerCharacter()||c->isAnimal()||c->isDead()){if(ou)ou->showPlayerAMessage(Loc::text("guards.select_character"),true);return;}
  if(action=="position"&&guildGuards.posts.count(selectedPost)&&chosenTag()){
   // Capturing position must not erase the name, tag or priority being edited.
   std::string name=nameEdit->getOnlyText(),priority=priorityEdit->getOnlyText();GuildGuards::Id tag=chosenTag();click(w);build();nameEdit->setOnlyText(name);priorityEdit->setOnlyText(priority);for(size_t n=0;n<tagChoices.size();++n)if(tagChoices[n]==tag)tagChoice->setIndexSelected(n);return;
  }
 }
 bool firstDelete=action=="delete_post"&&guildGuards.posts.count(selectedPost)&&confirmation!=action+number(selectedPost)+":"+number(selectedTag)+":"+selectedGuard;
 click(w);if(firstDelete&&ou)ou->showPlayerAMessage(Loc::text("guards.posts_delete_explanation"),true);
}
MyGUI::Button* postAction(MyGUI::Widget* p,int x,int y,int w,int h,const char* key,const std::string& action){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>("MercenarieGuildNav",x,y,w,h,MyGUI::Align::Default);setButtonText(b,Loc::text(key));b->setUserString("guardAction",action);b->eventMouseButtonClick+=MyGUI::newDelegate(postClick);return b;
}
MyGUI::ImageBox* postIcon(MyGUI::Widget* p,int slot,int x,int y,int size){MyGUI::ImageBox* i=overviewImage(p,"MercenarieGuardPostIcons.png",x,y,size,size);i->setImageCoord(MyGUI::IntCoord(slot*64,0,64,64));i->setColour(registerAmber);return i;}
MyGUI::Button* postIconAction(MyGUI::Widget* p,int x,int y,int size,int slot,const char* key,const char* action){MyGUI::Button* b=p->createWidget<MyGUI::Button>("MercenarieGuildNav",x,y,size,size,MyGUI::Align::Default);b->setUserString("guardAction",action);b->setUserString("guardIconLabel",Loc::text(key));b->eventMouseButtonClick+=MyGUI::newDelegate(postClick);postIcon(b,slot,3,3,size-6);return b;}
MyGUI::Colour postColour(int state){if(state==GuildGuards::Occupied)return MyGUI::Colour(.4f,.74f,.22f);if(state==GuildGuards::Reserved)return registerAmber;if(state==GuildGuards::Invalid)return MyGUI::Colour(.85f,.30f,.20f);return MyGUI::Colour(.52f,.59f,.60f);}
void postFieldLabel(MyGUI::Widget* p,int y,int w,int h,const char* key){MyGUI::TextBox* t=registerText(p,0,y,w,h,uiFont,Loc::text(key),registerIvory);fitRegisterText(t,uiFont);while(t->getFontHeight()>10&&(t->getTextSize().width>w||t->getTextSize().height>h))t->setFontHeight(t->getFontHeight()-1);}
void buildPosts(MyGUI::Widget* p,int w,int h){
 postDirection=0;postIndicators.clear();
 uiFont=std::max(12,std::min(22,(int)(16*std::min(w/1100.,h/660.))));int pad=std::max(6,w/140),gap=std::max(6,w/180),header=std::max(56,h*12/100),nav=std::max(28,h*6/100),footer=std::max(24,h*5/100),bh=std::max(30,std::min(50,h/14));
 int logo=header-2*pad;MyGUI::ImageBox* sun=overviewImage(p,"MercenarieOverviewIcons.png",pad,pad,logo,logo);sun->setImageCoord(MyGUI::IntCoord(0,0,128,128));sun->setColour(registerAmber);
 int tx=logo+3*pad,quote=w>=1000?w/5:0,tw=w-tx-quote-pad;
 MyGUI::TextBox* title=overviewLabel(p,tx,pad,tw,header/2-pad,std::string(Loc::text("ui.the_mercenarie"))+" | "+Loc::text("guards.management"),registerIvory);fitRegisterText(title,std::max(17,uiFont+5));
 MyGUI::TextBox* subtitle=registerText(p,tx,header/2,tw,header/2-pad,uiFont,Loc::text("guards.posts_subtitle"),registerIvory);fitRegisterText(subtitle,uiFont);
 if(quote){MyGUI::TextBox* q=registerText(p,w-quote,pad,quote-pad,header-2*pad,uiFont-1,Loc::text("guards.posts_quote"),MyGUI::Colour(.64f,.65f,.63f));fitRegisterText(q,uiFont-1);}
 registerSolid(p,pad,header,w-2*pad,1,registerAmber);
 const char* tabs[]={"guards.overview","guards.title","guards.posts","guards.tags"};int tabW=(w*70/100-3*gap)/4;for(int n=0;n<4;++n)overviewAction(p,pad+n*(tabW+gap),header+gap,tabW,nav,tabs[n],"tab:"+number(n))->setStateSelected(n==2);
 int top=header+gap+nav+gap,ch=h-top-footer-gap,total=w-2*pad,left=(total-gap)*30/100,right=total-left-gap,titleH=uiFont+12,helpH=std::max(76,ch*18/100);
 MyGUI::Widget* tree=overviewBox(p,pad,top,left,ch,"guards.tags",pad,titleH);list=peopleScroll(tree,pad,titleH+gap,left-2*pad,ch-titleH-bh-3*gap-pad,"posts_tree");int rw=bodyWidth(list),row=std::max(26,uiFont+12),y=0;
 for(std::map<GuildGuards::Id,GuildGuards::Tag>::const_iterator t=guildGuards.tags.begin();t!=guildGuards.tags.end();++t){
  unsigned count=0;for(std::map<GuildGuards::Id,GuildGuards::Post>::const_iterator i=guildGuards.posts.begin();i!=guildGuards.posts.end();++i)if(i->second.tag==t->first)++count;
  MyGUI::Button* fold=postAction(list,0,y,rw,row,"guards.tag","fold:"+number(t->first));setButtonText(fold,t->second.name+" ("+number(count)+")");int icon=std::min(20,row-6);postIcon(fold,collapsedTags[t->first]?1:0,3,3,icon);buttonCaptions[fold]->setCoord(icon+6,2,rw-icon-10,row-4);buttonCaptions[fold]->setTextAlign(MyGUI::Align::Left);fitLine(buttonCaptions[fold]);y+=row+2;
  if(!collapsedTags[t->first])for(std::map<GuildGuards::Id,GuildGuards::Post>::const_iterator i=guildGuards.posts.begin();i!=guildGuards.posts.end();++i)if(i->second.tag==t->first){
   MyGUI::Button* b=list->createWidget<MyGUI::Button>("MercenarieGuildNav",12,y,rw-12,row,MyGUI::Align::Default);b->setUserString("guardAction","post:"+number(i->first));b->eventMouseButtonClick+=MyGUI::newDelegate(postClick);b->setStateSelected(selectedPost==i->first);if(selectedPost==i->first){MyGUI::Widget* mark=registerSolid(b,1,1,b->getWidth()-2,row-2,MyGUI::Colour(.025f,.20f,.29f));mark->setNeedMouseFocus(false);}
   MyGUI::TextBox* name=overviewLabel(b,24,2,b->getWidth()-28,row-4,i->second.name,registerIvory);name->setNeedMouseFocus(false);buttonCaptions[b]=name;postIndicators[i->first]=peopleIcon(b,6,6,(row-10)/2,10,postColour(allocator.status(guildGuards,i->first)));y+=row;
  }
 }
 if(!y){help(list,0,"guards.posts_need_tag");y=100;}canvas(list,y);int createW=(left-2*pad-gap)*62/100;
 postAction(tree,pad,ch-bh-pad,createW,bh,"guards.posts_create","add_post");postAction(tree,pad+createW+gap,ch-bh-pad,left-2*pad-createW-gap,bh,"guards.delete","delete_post")->setEnabled(guildGuards.posts.count(selectedPost)!=0);
 int rx=pad+left+gap,detailH=ch-helpH-gap;MyGUI::Widget* box=overviewBox(p,rx,top,right,detailH,"guards.post_details",pad,titleH);
 MyGUI::ScrollView* body=peopleScroll(box,pad,titleH+gap,right-2*pad,detailH-titleH-gap-pad,"posts_details");int width=bodyWidth(body),formW=(width-gap)*54/100,previewX=formW+gap,previewW=width-previewX,lw=std::max(84,formW*26/100),fx=lw+gap,fw=formW-fx,field=std::max(32,uiFont*2+6),step=field+gap;
 GuildGuards::Post value;if(guildGuards.posts.count(selectedPost))value=guildGuards.posts[selectedPost];bool exists=value.id!=0;
 postFieldLabel(body,0,lw,field,"guards.name");nameEdit=edit(body,fx,0,fw,value.name);nameEdit->setSize(fw,field);
 postFieldLabel(body,step,lw,field,"guards.tag");tagChoices.clear();tagChoice=body->createWidget<MyGUI::ComboBox>("TheMercenarie_Combo",fx,step,fw,field,MyGUI::Align::Default);tagChoice->setComboModeDrop(true);tagChoice->setFontName("MercenarieUnicode");tagChoice->setFontHeight(uiFont);
 for(std::map<GuildGuards::Id,GuildGuards::Tag>::const_iterator i=guildGuards.tags.begin();i!=guildGuards.tags.end();++i){tagChoices.push_back(i->first);tagChoice->addItem(i->second.name);}if(!tagChoices.empty())tagChoice->setIndexSelected(0);for(size_t n=0;n<tagChoices.size();++n)if(tagChoices[n]==value.tag)tagChoice->setIndexSelected(n);
 postFieldLabel(body,2*step,lw,field,"guards.priority");int square=std::min(42,field),editW=std::max(26,fw-2*(square+4));priorityEdit=edit(body,fx,2*step,editW,number(value.priority));priorityEdit->setSize(editW,field);postIconAction(body,fx+editW+4,2*step,square,4,"guards.down","priority_less");postIconAction(body,fx+editW+square+8,2*step,square,5,"guards.up","priority_more");
 postFieldLabel(body,3*step,lw,bh,"guards.posts_position");postAction(body,fx,3*step,fw,bh,"guards.posts_use_character","position")->setEnabled(exists);
 int sourceY=3*step+bh+gap;peopleIcon(body,0,0,sourceY,uiFont+8,registerIvory);liveLabels["posts_source"]=overviewLabel(body,uiFont+14,sourceY,formW-uiFont-14,uiFont+8,"",registerIvory);
 int angleY=sourceY+uiFont+8+gap;postFieldLabel(body,angleY,lw,field,"guards.posts_orientation");draftHeading=value.heading;displayedAngle=decimal(std::floor(value.heading+.5));int angleW=fw-2*(square+4);postIconAction(body,fx,angleY,square,2,"guards.left","left");angleEdit=edit(body,fx+square+4,angleY,angleW,displayedAngle);angleEdit->setSize(angleW,field);postIconAction(body,fx+square+8+angleW,angleY,square,3,"guards.right","right");
 int formBottom=angleY+field,previewSize=std::min(previewW,formBottom-uiFont*4-8);MyGUI::Widget* diagram=body->createWidget<MyGUI::Widget>("MercenarieOverviewPanel",previewX,0,previewW,formBottom,MyGUI::Align::Default);int px=(previewW-previewSize)/2;
 overviewImage(diagram,"MercenarieGuardPostPreview.png",px,0,previewSize,previewSize);postDirection=overviewImage(diagram,"MercenarieGuardPostDirections.png",px,0,previewSize,previewSize);postDirection->setImageCoord(MyGUI::IntCoord(0,0,128,128));peopleIcon(diagram,0,px+previewSize*40/100,previewSize*40/100,previewSize/5,registerIvory);
 MyGUI::TextBox* caption=registerText(diagram,4,previewSize+2,previewW-8,formBottom-previewSize-4,uiFont,Loc::text("guards.posts_diagram"),registerIvory);fitRegisterText(caption,uiFont);
 int actionsY=formBottom+gap,half=(width-gap)/2;postAction(body,0,actionsY,half,bh,"guards.posts_recapture","position")->setEnabled(exists);postAction(body,half+gap,actionsY,width-half-gap,bh,"guards.posts_delete","delete_post")->setEnabled(exists);
 actionsY+=bh+gap;postAction(body,0,actionsY,half,bh,"guards.posts_save","save_post")->setEnabled(exists);postAction(body,half+gap,actionsY,width-half-gap,bh,"guards.view_world","world_post")->setEnabled(exists);
 MyGUI::TextBox* note=registerText(body,0,actionsY+bh+gap,width,uiFont*3,uiFont,Loc::text("guards.posts_save_hint"),registerIvory);fitRegisterText(note,uiFont);canvas(body,actionsY+bh+gap+uiFont*3);
 MyGUI::Widget* info=p->createWidget<MyGUI::Widget>("MercenarieOverviewPanel",rx,top+ch-helpH,right,helpH,MyGUI::Align::Default);int infoSize=std::max(24,uiFont*2);peopleIcon(info,5,pad,pad,infoSize,registerAmber);MyGUI::TextBox* hint=registerText(info,pad*2+infoSize,pad,right-3*pad-infoSize,helpH-2*pad,uiFont,Loc::text("guards.posts_help"),registerIvory);fitRegisterText(hint,uiFont);
 int fy=h-footer;registerSolid(p,pad,fy,total,1,registerAmber);overviewLabel(p,pad,fy+3,total/4,footer-5,Loc::text("ui.the_mercenarie"),registerAmber);overviewLabel(p,pad+total/4,fy+3,total/2,footer-5,Loc::text("guards.overview_footer"),registerAmber);overviewAction(p,w-pad-total/6,fy+2,total/6,footer-3,"guards.close_window","close");
}
void refreshPosts(){
 if(page!=2)return;for(std::map<GuildGuards::Id,MyGUI::ImageBox*>::iterator i=postIndicators.begin();i!=postIndicators.end();++i)i->second->setColour(postColour(allocator.status(guildGuards,i->first)));
 if(postDirection&&angleEdit){double value=draftHeading;std::istringstream input(angleEdit->getOnlyText());if(angleEdit->getOnlyText()!=displayedAngle)input>>value;if(value>=-1e9&&value<=1e9){double angle=std::fmod(value,360.);if(angle<0)angle+=360;int frame=((int)std::floor(angle*64/360.+.5))%64;postDirection->setImageCoord(MyGUI::IntCoord((frame%8)*128,(frame/8)*128,128,128));}}
 if(liveLabels.count("posts_source")){Character* c=selected();std::string value=std::string(Loc::text("guards.posts_source"))+" "+(c&&c->isPlayerCharacter()?c->getName():Loc::text("guards.none"));MercenarieFonts::caption(liveLabels["posts_source"],value);fitLine(liveLabels["posts_source"]);}
}
