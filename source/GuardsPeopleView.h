// Phase 2: presentation of the existing guard model and native portraits only.
MyGUI::Button* peoplePause=0;
MyGUI::ImageBox* peopleStateDot=0;
const char* peopleStateKey(GuildGuards::State s){if(s==GuildGuards::Waiting)return "guards.people_waiting";if(s==GuildGuards::Travelling)return "guards.people_moving";if(s==GuildGuards::Returning)return "guards.people_returning";return stateKey(s);}
MyGUI::Colour peopleColour(GuildGuards::State s){
 if(s==GuildGuards::Resting||s==GuildGuards::SeekingBed)return MyGUI::Colour(.80f,.35f,.18f);
 if(s==GuildGuards::Waiting||s==GuildGuards::Paused||s==GuildGuards::Unavailable)return MyGUI::Colour(.60f,.64f,.63f);
 return stateColour(s);
}
MyGUI::ImageBox* peopleIcon(MyGUI::Widget* p,int slot,int x,int y,int size,const MyGUI::Colour& colour){MyGUI::ImageBox* i=overviewImage(p,"MercenarieGuardPeopleIcons.png",x,y,size,size);i->setImageCoord(MyGUI::IntCoord(slot*64,0,64,64));i->setColour(colour);return i;}
void peoplePortrait(MyGUI::Widget* p,const GuildGuards::Guard& g,int x,int y,int size){
 MyGUI::ImageBox* i=peopleIcon(p,g.robot?1:0,x,y,size,registerIvory);Character* c=resolve(g.actor);
 if(c&&PortraitManager::getInstance()){i->setColour(MyGUI::Colour(1,1,1));PortraitManager::getInstance()->setImageWidget(c->getHandle(),i,true);}
}
MyGUI::Button* peopleIconAction(MyGUI::Widget* p,int x,int y,int size,int slot,const char* key,const std::string& action){
 MyGUI::Button* b=p->createWidget<MyGUI::Button>("MercenarieGuildNav",x,y,size,size,MyGUI::Align::Default);b->setUserString("guardAction",action);b->setUserString("guardIconLabel",Loc::text(key));b->eventMouseButtonClick+=MyGUI::newDelegate(click);peopleIcon(b,slot,3,3,size-6,registerIvory);return b;
}
MyGUI::ScrollView* peopleScroll(MyGUI::Widget* p,int x,int y,int w,int h,const char* key){
 MyGUI::ScrollView* s=p->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",x,y,w,h,MyGUI::Align::Default);s->setVisibleHScroll(false);s->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);MercenarieNativeInput::bind(s);scrollPanels[key]=s;return s;
}
void peopleChoice(MyGUI::Widget* p,int y,int w,const GuildGuards::Guard& g){
 tagChoice=p->createWidget<MyGUI::ComboBox>("TheMercenarie_Combo",0,y,w,28,MyGUI::Align::Default);tagChoice->setComboModeDrop(true);tagChoice->setFontName("MercenarieUnicode");tagChoice->setFontHeight(uiFont);tagChoices.clear();
 for(std::map<GuildGuards::Id,GuildGuards::Tag>::const_iterator i=guildGuards.tags.begin();i!=guildGuards.tags.end();++i)if(std::find(g.tags.begin(),g.tags.end(),i->first)==g.tags.end()){tagChoice->addItem(i->second.name);tagChoices.push_back(i->first);}
 if(!tagChoices.empty())tagChoice->setIndexSelected(0);else MercenarieFonts::caption(tagChoice,Loc::text("guards.people_no_tags"));tagChoice->setEnabled(!tagChoices.empty());
}
void peopleDetail(MyGUI::Widget* p,int y,int w,int rh,const char* key,const char* liveKey){
 int left=w*43/100;overviewLabel(p,0,y,left-5,rh-2,Loc::text(key),registerIvory);liveLabels[liveKey]=overviewLabel(p,left,y,w-left,rh-2,"",registerIvory);registerSolid(p,0,y+rh-1,w,1,MyGUI::Colour(.13f,.17f,.17f));
}
void buildPeople(MyGUI::Widget* p,int w,int h){
 peoplePause=0;peopleStateDot=0;
 uiFont=std::max(12,std::min(22,(int)(16*std::min(w/1100.,h/660.))));int pad=std::max(6,w/140),gap=std::max(6,w/180),header=std::max(56,h*12/100),nav=std::max(28,h*6/100),footer=std::max(24,h*5/100),bh=std::max(30,std::min(50,h/14));
 int logo=header-2*pad;MyGUI::ImageBox* sun=overviewImage(p,"MercenarieOverviewIcons.png",pad,pad,logo,logo);sun->setImageCoord(MyGUI::IntCoord(0,0,128,128));sun->setColour(registerAmber);
 int tx=logo+3*pad,quote=w>=1000?w/5:0,tw=w-tx-quote-pad;
 MyGUI::TextBox* title=overviewLabel(p,tx,pad,tw,header/2-pad,std::string(Loc::text("ui.the_mercenarie"))+" | "+Loc::text("guards.management"),registerIvory);fitRegisterText(title,std::max(17,uiFont+5));
 MyGUI::TextBox* subtitle=registerText(p,tx,header/2,tw,header/2-pad,uiFont,Loc::text("guards.people_subtitle"),registerIvory);fitRegisterText(subtitle,uiFont);
 if(quote){MyGUI::TextBox* q=registerText(p,w-quote,pad,quote-pad,header-2*pad,uiFont-1,Loc::text("guards.people_quote"),MyGUI::Colour(.64f,.65f,.63f));fitRegisterText(q,uiFont-1);}
 registerSolid(p,pad,header,w-2*pad,1,registerAmber);
 const char* tabs[]={"guards.overview","guards.title","guards.posts","guards.tags"};int tabW=(w*70/100-3*gap)/4;for(int n=0;n<4;++n)overviewAction(p,pad+n*(tabW+gap),header+gap,tabW,nav,tabs[n],"tab:"+number(n))->setStateSelected(n==1);
 int top=header+gap+nav+gap,ch=h-top-footer-gap,total=w-2*pad,available=total-2*gap,aw=available*30/100,bw=available*30/100,cw=available-aw-bw,titleH=uiFont+12;
 MyGUI::Widget* a=overviewBox(p,pad,top,aw,ch,"guards.roster",pad,titleH);
 MyGUI::TextBox* rosterTitle=a->getChildAt(0)->castType<MyGUI::TextBox>();MercenarieFonts::caption(rosterTitle,std::string(Loc::text("guards.roster"))+" ("+number(guildGuards.guards.size())+")");fitLine(rosterTitle);
 list=peopleScroll(a,pad,titleH+gap,aw-2*pad,ch-titleH-bh-3*gap-pad,"people_roster");int rw=bodyWidth(list),rh=std::max(38,std::min(66,uiFont*3)),y=0;
 for(std::map<std::string,GuildGuards::Guard>::const_iterator i=guildGuards.guards.begin();i!=guildGuards.guards.end();++i){
  const GuildGuards::Guard& g=i->second;MyGUI::Button* pick=list->createWidget<MyGUI::Button>("MercenarieGuildNav",0,y,rw,rh-2,MyGUI::Align::Default);pick->setUserString("guardAction","guard:"+i->first);pick->eventMouseButtonClick+=MyGUI::newDelegate(click);pick->setStateSelected(selectedGuard==i->first);
  if(selectedGuard==i->first){MyGUI::Widget* highlight=registerSolid(pick,1,1,rw-2,rh-4,MyGUI::Colour(.025f,.20f,.29f));highlight->setNeedMouseFocus(false);}
  int portrait=rh-6,nx=portrait+gap,nw=(rw-nx)*44/100;peoplePortrait(pick,g,2,2,portrait);
  // Move the explicit caption above the row highlight; native nav skin has no text subskin.
  buttonCaptions.erase(pick);MyGUI::TextBox* name=overviewLabel(pick,nx,(rh-uiFont-6)/2,nw,uiFont+4,g.name,registerIvory);name->setNeedMouseFocus(false);buttonCaptions[pick]=name;
  statusLabels[i->first]=overviewLabel(pick,nx+nw+3,(rh-uiFont-6)/2,rw-nx-nw-5,uiFont+4,Loc::text(peopleStateKey(guardState(g))),peopleColour(guardState(g)));statusLabels[i->first]->setNeedMouseFocus(false);
  y+=rh;
 }
 if(!y){help(list,0,"guards.empty_guards");y=110;}canvas(list,y);
 overviewAction(a,pad,ch-bh-pad,aw-2*pad,bh,"guards.people_add_guard","add_guard");
 MyGUI::Widget* b=overviewBox(p,pad+aw+gap,top,bw,ch,"guards.guard_details",pad,titleH);
 MyGUI::ScrollView* details=peopleScroll(b,pad,titleH+gap,bw-2*pad,ch-titleH-gap-pad,"people_details");int dw=bodyWidth(details);
 int cx=pad+aw+bw+2*gap,notice=bh+2*pad,helpH=std::max(100,ch*27/100),tagH=ch-helpH-notice-2*gap;
 MyGUI::Widget* c=overviewBox(p,cx,top,cw,tagH,"guards.people_tags",pad,titleH);
 MyGUI::ScrollView* tags=peopleScroll(c,pad,titleH+gap,cw-2*pad,tagH-titleH-gap-pad,"people_tags");int tagW=bodyWidth(tags);
 MyGUI::Widget* info=p->createWidget<MyGUI::Widget>("MercenarieOverviewPanel",cx,top+tagH+gap,cw,helpH,MyGUI::Align::Default);int infoSize=std::max(22,uiFont*2);peopleIcon(info,5,pad,pad,infoSize,registerAmber);
 MyGUI::TextBox* hint=registerText(info,pad*2+infoSize,pad,cw-3*pad-infoSize,helpH-2*pad,uiFont,Loc::text("guards.people_help"),registerIvory);fitRegisterText(hint,uiFont);
 MyGUI::Widget* applied=p->createWidget<MyGUI::Widget>("MercenarieOverviewPanel",cx,top+ch-notice,cw,notice,MyGUI::Align::Default);MyGUI::TextBox* appliedText=registerText(applied,pad,pad,cw-2*pad,notice-2*pad,uiFont,Loc::text("guards.people_applied"),registerAmber);appliedText->setTextAlign(MyGUI::Align::Center);fitRegisterText(appliedText,uiFont);
 if(guildGuards.guards.count(selectedGuard)){
  const GuildGuards::Guard& g=guildGuards.guards[selectedGuard];int portrait=std::min(dw/3,std::max(58,ch*23/100)),nx=portrait+gap;
  peoplePortrait(details,g,0,0,portrait);liveLabels["people_name"]=overviewLabel(details,nx,2,dw-nx,uiFont+8,g.name,registerIvory);
  int dot=std::max(9,uiFont/2);peopleStateDot=peopleIcon(details,6,nx,portrait-uiFont-4,dot,peopleColour(guardState(g)));liveLabels["people_state"]=overviewLabel(details,nx+dot+4,portrait-uiFont-7,dw-nx-dot-4,uiFont+6,Loc::text(peopleStateKey(guardState(g))),peopleColour(guardState(g)));
  int rowH=std::max(uiFont+12,std::min(std::min(uiFont*3,ch*9/100),(details->getHeight()-portrait-3*bh-4*gap-8)/5));y=portrait+gap;const char* labels[]={"guards.people_state_label","guards.people_post","guards.health","guards.people_hunger","guards.people_type"};const char* values[]={"detail_state","detail_post","detail_health","people_hunger","people_type"};for(int n=0;n<5;++n){peopleDetail(details,y,dw,rowH,labels[n],values[n]);y+=rowH;}
  y+=gap;peoplePause=overviewAction(details,0,y,dw,bh,g.paused?"guards.resume":"guards.pause","pause_guard");y+=bh+gap;overviewAction(details,0,y,dw,bh,"guards.remove_service","delete_guard");y+=bh+gap;overviewAction(details,0,y,dw,bh,"guards.view_world","world_guard");canvas(details,y+bh);
  y=0;int size=std::max(24,uiFont+10),spacing=std::max(4,gap/2);for(size_t n=0;n<g.tags.size();++n){
   MyGUI::Widget* rank=tags->createWidget<MyGUI::Widget>("MercenarieOverviewPanel",0,y,size,size,MyGUI::Align::Default);MyGUI::TextBox* rankText=overviewLabel(rank,2,2,size-4,size-4,number(n+1),registerAmber);rankText->setTextAlign(MyGUI::Align::Center);
   int buttons=3*size+2*spacing,start=tagW-buttons;overviewLabel(tags,size+spacing,y,start-size-2*spacing,size,guildGuards.tags.find(g.tags[n])->second.name,registerIvory);
   peopleIconAction(tags,start,y,size,2,"guards.up","up:"+number(n))->setEnabled(n>0);peopleIconAction(tags,start+size+spacing,y,size,3,"guards.down","down:"+number(n))->setEnabled(n+1<g.tags.size());peopleIconAction(tags,start+2*(size+spacing),y,size,4,"guards.remove","untag:"+number(g.tags[n]));y+=size+gap;
  }
  peopleChoice(tags,y,tagW,g);y+=28+gap;overviewAction(tags,0,y,tagW,bh,"guards.people_add_tag","assign_tag")->setEnabled(!tagChoices.empty());canvas(tags,y+bh);
 }else{help(details,0,"guards.select_guard");canvas(details,110);canvas(tags,1);}
 int fy=h-footer;registerSolid(p,pad,fy,total,1,registerAmber);overviewLabel(p,pad,fy+3,total/4,footer-5,Loc::text("ui.the_mercenarie"),registerAmber);overviewLabel(p,pad+total/4,fy+3,total/2,footer-5,Loc::text("guards.overview_footer"),registerAmber);overviewAction(p,w-pad-total/6,fy+2,total/6,footer-3,"guards.close_window","close");
}
void refreshPeople(){
 if(page!=1||!guildGuards.guards.count(selectedGuard))return;const GuildGuards::Guard& g=guildGuards.guards[selectedGuard];GuildGuards::State s=guardState(g);Character* c=resolve(g.actor);
 const char* keys[]={"people_name","people_state","detail_state","detail_health","people_hunger","people_type"};std::string values[]={g.name,Loc::text(peopleStateKey(s)),Loc::text(peopleStateKey(s)),c?decimal(std::floor(health(c)*100+.5))+" %":Loc::text("guards.unavailable"),Loc::text(g.robot?"guards.people_not_applicable":"guards.unavailable"),Loc::text(g.robot?"guards.people_robot":"guards.people_human")};
 for(int n=0;n<6;++n)if(liveLabels.count(keys[n])){MercenarieFonts::caption(liveLabels[keys[n]],values[n]);fitLine(liveLabels[keys[n]]);}if(liveLabels.count("people_state"))liveLabels["people_state"]->setTextColour(peopleColour(s));if(peopleStateDot)peopleStateDot->setColour(peopleColour(s));if(peoplePause)setButtonText(peoplePause,Loc::text(g.paused?"guards.resume":"guards.pause"));
}
