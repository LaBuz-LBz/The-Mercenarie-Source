// TAG appearance and editing. Allocation, pauses and deletion keep their V10 services.
std::map<GuildGuards::Id,MyGUI::ImageBox*> tagSwatches;
std::vector<MyGUI::Button*> tagColourButtons;
MyGUI::Colour tagUiColour(int n){const MyGUI::Colour colours[]={MyGUI::Colour(.38f,.70f,.25f),MyGUI::Colour(.76f,.20f,.17f),MyGUI::Colour(.16f,.48f,.66f),MyGUI::Colour(.96f,.61f,.16f),MyGUI::Colour(.45f,.51f,.53f)};return colours[n>=0&&n<5?n:0];}
MyGUI::ImageBox* tagIcon(MyGUI::Widget* p,int slot,int x,int y,int size,const MyGUI::Colour& colour){MyGUI::ImageBox* i=overviewImage(p,"MercenarieGuardTagIcons.png",x,y,size,size);i->setImageCoord(MyGUI::IntCoord(slot*64,0,64,64));i->setColour(colour);return i;}
bool tagNameExists(const std::string& name,GuildGuards::Id except){std::string key=GuildGuards::tagNameKey(name);for(std::map<GuildGuards::Id,GuildGuards::Tag>::const_iterator i=guildGuards.tags.begin();i!=guildGuards.tags.end();++i)if(i->first!=except&&GuildGuards::tagNameKey(i->second.name)==key)return true;return false;}
std::string uniqueTagName(const std::string& base){std::string stem=GuildGuards::cosmeticText(base,64),name=stem;for(unsigned int n=2;tagNameExists(name,0);++n)name=stem+" "+number(n);return name;}
void tagDescriptionChanged(MyGUI::EditBox* e){if(page==3&&guildGuards.tags.count(selectedTag))guildGuards.tags[selectedTag].description=GuildGuards::cosmeticText(e->getOnlyText(),512);}
void refreshTags(){
 if(page!=3)return;for(std::map<GuildGuards::Id,MyGUI::ImageBox*>::iterator i=tagSwatches.begin();i!=tagSwatches.end();++i)if(guildGuards.tags.count(i->first))i->second->setColour(tagUiColour(GuildGuards::tagColour(i->first,guildGuards.tags[i->first].colour)));
 for(size_t n=0;n<tagColourButtons.size();++n)tagColourButtons[n]->setStateSelected(guildGuards.tags.count(selectedTag)&&GuildGuards::tagColour(selectedTag,guildGuards.tags[selectedTag].colour)==(int)n);
}
void tagClick(MyGUI::Widget* w){
 const std::string action=w->getUserString("guardAction");
 if(action=="add_tag"){
  std::string name=GuildGuards::tagName(nameEdit->getOnlyText());if(name.empty()||(guildGuards.tags.count(selectedTag)&&GuildGuards::tagNameKey(name)==GuildGuards::tagNameKey(guildGuards.tags[selectedTag].name)))name=uniqueTagName(Loc::text("guards.tag"));
  if(tagNameExists(name,0)){if(ou)ou->showPlayerAMessage(Loc::text("guards.tags_name_exists"),true);return;}nameEdit->setOnlyText(name);click(w);return;
 }
 if(action=="save_tag"&&guildGuards.tags.count(selectedTag)){
  std::string name=GuildGuards::tagName(nameEdit->getOnlyText());if(name.empty()||tagNameExists(name,selectedTag)){if(ou)ou->showPlayerAMessage(Loc::text(name.empty()?"guards.tags_name_empty":"guards.tags_name_exists"),true);return;}nameEdit->setOnlyText(name);click(w);return;
 }
 if(action=="duplicate_tag"&&guildGuards.tags.count(selectedTag)){
  GuildGuards::Tag copy=guildGuards.tags[selectedTag];copy.id=guildGuards.next();copy.name=uniqueTagName(GuildGuards::cosmeticText(copy.name,48)+" - "+Loc::text("guards.tags_copy"));guildGuards.tags[copy.id]=copy;selectedTag=copy.id;confirmation.clear();dirty=true;return;
 }
 if(action.compare(0,11,"tag_colour:")==0&&guildGuards.tags.count(selectedTag)){int colour=atoi(action.substr(11).c_str());if(colour>=0&&colour<5)guildGuards.tags[selectedTag].colour=colour;refreshTags();return;}
 if(action=="pause_tag"||action=="global"){std::string name=nameEdit?nameEdit->getOnlyText():"";click(w);build();if(nameEdit)nameEdit->setOnlyText(name);return;}
 bool firstDelete=action=="delete_tag"&&guildGuards.tags.count(selectedTag)&&confirmation!=action+number(selectedPost)+":"+number(selectedTag)+":"+selectedGuard;
 click(w);if(firstDelete&&ou)ou->showPlayerAMessage(Loc::text("guards.tags_delete_explanation"),true);
}
MyGUI::Button* tagAction(MyGUI::Widget* p,int x,int y,int w,int h,const char* key,const std::string& action){MyGUI::Button* b=p->createWidget<MyGUI::Button>("MercenarieGuildNav",x,y,w,h,MyGUI::Align::Default);setButtonText(b,Loc::text(key));b->setUserString("guardAction",action);b->eventMouseButtonClick+=MyGUI::newDelegate(tagClick);return b;}
void buildTags(MyGUI::Widget* p,int w,int h){
 tagSwatches.clear();tagColourButtons.clear();
 uiFont=std::max(12,std::min(22,(int)(16*std::min(w/1100.,h/660.))));int pad=std::max(6,w/140),gap=std::max(6,w/180),header=std::max(56,h*12/100),nav=std::max(28,h*6/100),footer=std::max(24,h*5/100),bh=std::max(30,std::min(50,h/14));
 int logo=header-2*pad;MyGUI::ImageBox* sun=overviewImage(p,"MercenarieOverviewIcons.png",pad,pad,logo,logo);sun->setImageCoord(MyGUI::IntCoord(0,0,128,128));sun->setColour(registerAmber);
 int tx=logo+3*pad,quote=w>=1000?w/5:0,tw=w-tx-quote-pad;
 MyGUI::TextBox* title=overviewLabel(p,tx,pad,tw,header/2-pad,std::string(Loc::text("ui.the_mercenarie"))+" | "+Loc::text("guards.management"),registerIvory);fitRegisterText(title,std::max(17,uiFont+5));
 MyGUI::TextBox* subtitle=registerText(p,tx,header/2,tw,header/2-pad,uiFont,Loc::text("guards.tags_subtitle"),registerIvory);fitRegisterText(subtitle,uiFont);
 if(quote){MyGUI::TextBox* q=registerText(p,w-quote,pad,quote-pad,header-2*pad,uiFont-1,Loc::text("guards.tags_quote"),MyGUI::Colour(.64f,.65f,.63f));fitRegisterText(q,uiFont-1);}
 registerSolid(p,pad,header,w-2*pad,1,registerAmber);
 const char* tabs[]={"guards.overview","guards.title","guards.posts","guards.tags"};int tabW=(w*70/100-3*gap)/4;for(int n=0;n<4;++n)overviewAction(p,pad+n*(tabW+gap),header+gap,tabW,nav,tabs[n],"tab:"+number(n))->setStateSelected(n==3);
 int top=header+gap+nav+gap,ch=h-top-footer-gap,total=w-2*pad,available=total-2*gap,aw=available*29/100,bw=available*34/100,cw=available-aw-bw,titleH=uiFont+12,helpH=std::max(76,ch*17/100),detailH=ch-helpH-gap;
 MyGUI::Widget* a=overviewBox(p,pad,top,aw,ch,"guards.tags_list",pad,titleH);MyGUI::ScrollView* tags=peopleScroll(a,pad,titleH+gap,aw-2*pad,ch-titleH-3*bh-4*gap-pad,"tags_list");int row=std::max(36,std::min(64,uiFont*3)),rw=bodyWidth(tags),y=0;
 for(std::map<GuildGuards::Id,GuildGuards::Tag>::const_iterator i=guildGuards.tags.begin();i!=guildGuards.tags.end();++i){
  MyGUI::Button* b=tags->createWidget<MyGUI::Button>("MercenarieGuildNav",0,y,rw,row-2,MyGUI::Align::Default);b->setUserString("guardAction","tag:"+number(i->first));b->eventMouseButtonClick+=MyGUI::newDelegate(tagClick);b->setStateSelected(selectedTag==i->first);if(selectedTag==i->first)registerSolid(b,1,1,rw-2,row-4,MyGUI::Colour(.025f,.20f,.29f));int size=std::min(26,row-10);tagSwatches[i->first]=tagIcon(b,0,5,(row-size)/2,size,tagUiColour(GuildGuards::tagColour(i->first,i->second.colour)));buttonCaptions[b]=overviewLabel(b,size+12,3,rw-size-18,row-6,i->second.name,registerIvory);y+=row;
 }if(!y){help(tags,0,"guards.tags_empty");y=80;}canvas(tags,y);
 int actionsY=ch-3*bh-2*gap-pad;tagAction(a,pad,actionsY,aw-2*pad,bh,"guards.tags_create","add_tag");tagAction(a,pad,actionsY+bh+gap,aw-2*pad,bh,"guards.tags_duplicate","duplicate_tag")->setEnabled(guildGuards.tags.count(selectedTag)!=0);tagAction(a,pad,actionsY+2*(bh+gap),aw-2*pad,bh,guildGuards.paused?"guards.resume_all":"guards.pause_all","global");
 int bx=pad+aw+gap,cx=bx+bw+gap;MyGUI::Widget* b=overviewBox(p,bx,top,bw,detailH,"guards.tag_details",pad,titleH);MyGUI::ScrollView* details=peopleScroll(b,pad,titleH+gap,bw-2*pad,detailH-titleH-gap-pad,"tags_details");int dw=bodyWidth(details);bool exists=guildGuards.tags.count(selectedTag)!=0;GuildGuards::Tag value;if(exists)value=guildGuards.tags[selectedTag];
 int field=std::max(30,uiFont+10),labelW=std::max(52,dw*26/100),fx=labelW+gap,fw=dw-fx;
 postFieldLabel(details,0,labelW,field,"guards.name");nameEdit=edit(details,fx,0,fw,value.name);nameEdit->setSize(fw,field);nameEdit->setMaxTextLength(96);
 y=field+gap;postFieldLabel(details,y,labelW,field,"guards.tags_colour");int colourSize=std::min(field,(fw-16)/5);const char* colourNames[]={"guards.tags_green","guards.tags_red","guards.tags_blue","guards.tags_orange","guards.tags_grey"};
 for(int n=0;n<5;++n){MyGUI::Button* pick=details->createWidget<MyGUI::Button>("MercenarieGuildNav",fx+n*(colourSize+4),y,colourSize,colourSize,MyGUI::Align::Default);pick->setUserString("guardAction","tag_colour:"+number(n));pick->setUserString("guardIconLabel",Loc::text(colourNames[n]));pick->eventMouseButtonClick+=MyGUI::newDelegate(tagClick);pick->setEnabled(exists);tagIcon(pick,0,2,2,colourSize-4,tagUiColour(n));tagColourButtons.push_back(pick);}
 y+=field+gap;overviewLabel(details,0,y,dw,uiFont+7,Loc::text("guards.tags_description"),registerIvory);y+=uiFont+7;
 int descH=std::max(64,std::min(150,detailH*25/100));tagDescriptionEdit=edit(details,0,y,dw,"");tagDescriptionEdit->setSize(dw,descH);tagDescriptionEdit->setEditMultiLine(true);tagDescriptionEdit->setVisibleVScroll(true);tagDescriptionEdit->setVisibleHScroll(true);tagDescriptionEdit->setOnlyText(value.description);tagDescriptionEdit->setMaxTextLength(512);tagDescriptionEdit->setEnabled(exists);tagDescriptionEdit->eventEditTextChange+=MyGUI::newDelegate(tagDescriptionChanged);y+=descH+gap;
 MyGUI::Button* active=tagAction(details,0,y,dw,bh,value.paused?"guards.tags_paused":"guards.tags_active","pause_tag");active->setEnabled(exists);int check=std::min(24,bh-6);tagIcon(active,value.paused?2:1,4,(bh-check)/2,check,registerAmber);buttonCaptions[active]->setCoord(check+10,2,dw-check-14,bh-4);fitLine(buttonCaptions[active]);y+=bh+gap;
 int half=(dw-gap)/2;tagAction(details,0,y,half,bh,"guards.rename","save_tag")->setEnabled(exists);tagAction(details,half+gap,y,dw-half-gap,bh,"guards.tags_duplicate_short","duplicate_tag")->setEnabled(exists);y+=bh+gap;
 MyGUI::Button* remove=tagAction(details,0,y,dw,bh,"guards.tags_delete","delete_tag");remove->setEnabled(exists);buttonCaptions[remove]->setTextColour(MyGUI::Colour(.81f,.45f,.33f));y+=bh+gap;
 MyGUI::TextBox* saved=registerText(details,0,y,dw,uiFont*4,uiFont,Loc::text("guards.tags_immediate"),registerIvory);fitRegisterText(saved,uiFont);canvas(details,y+uiFont*4);
 MyGUI::Widget* c=overviewBox(p,cx,top,cw,detailH,"guards.tag_posts",pad,titleH);MyGUI::ScrollView* posts=peopleScroll(c,pad,titleH+gap,cw-2*pad,detailH-titleH-gap-pad,"tags_posts");int pw=bodyWidth(posts),postRow=std::max(48,uiFont*3),count=0;y=0;
 for(std::map<GuildGuards::Id,GuildGuards::Post>::const_iterator i=guildGuards.posts.begin();i!=guildGuards.posts.end();++i)if(i->second.tag==selectedTag){
  MyGUI::Button* pick=posts->createWidget<MyGUI::Button>("MercenarieGuildNav",0,y,pw,postRow-2,MyGUI::Align::Default);pick->setUserString("guardAction","post:"+number(i->first));pick->eventMouseButtonClick+=MyGUI::newDelegate(tagClick);int size=std::min(30,postRow-8),nx=size+gap,nw=(pw-nx)*56/100;guardIcon(pick,5,3,(postRow-size)/2,size,registerAmber);buttonCaptions[pick]=overviewLabel(pick,nx,4,nw,postRow-8,i->second.name,registerIvory);
  MyGUI::TextBox* priority=registerText(pick,nx+nw+gap,4,pw-nx-nw-gap-3,postRow-8,uiFont,std::string(Loc::text("guards.priority"))+" : "+number(i->second.priority),registerIvory);fitRegisterText(priority,uiFont);y+=postRow;++count;
 }if(!count){help(posts,0,"guards.tags_no_posts");y=80;}canvas(posts,y);MyGUI::TextBox* heading=c->getChildAt(0)->castType<MyGUI::TextBox>();MercenarieFonts::caption(heading,std::string(Loc::text("guards.tag_posts"))+" ("+number(count)+")");fitLine(heading);liveLabels["tags_post_count"]=heading;
 MyGUI::Widget* info=p->createWidget<MyGUI::Widget>("MercenarieOverviewPanel",bx,top+ch-helpH,bw+gap+cw,helpH,MyGUI::Align::Default);int size=std::max(24,uiFont*2);peopleIcon(info,5,pad,pad,size,registerAmber);MyGUI::TextBox* hint=registerText(info,2*pad+size,pad,bw+gap+cw-3*pad-size,helpH-2*pad,uiFont,Loc::text("guards.tags_help"),registerIvory);fitRegisterText(hint,uiFont);
 int fy=h-footer;registerSolid(p,pad,fy,total,1,registerAmber);overviewLabel(p,pad,fy+3,total/4,footer-5,Loc::text("ui.the_mercenarie"),registerAmber);overviewLabel(p,pad+total/4,fy+3,total/2,footer-5,Loc::text("guards.overview_footer"),registerAmber);overviewAction(p,w-pad-total/6,fy+2,total/6,footer-3,"guards.close_window","close");
 refreshTags();
}
