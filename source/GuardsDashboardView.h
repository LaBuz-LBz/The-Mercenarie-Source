// Presentation only. Guard simulation and persistence remain in the existing services.
void fitLine(MyGUI::TextBox* t){
 std::string value=t->getCaption();t->setFontHeight(uiFont);
 if(t->getTextSize().width<=t->getWidth()-2)return;
 while(!value.empty()){size_t end=value.size()-1;while(end>0&&((unsigned char)value[end]&0xc0)==0x80)--end;value.erase(end);MercenarieFonts::caption(t,value+"...");if(t->getTextSize().width<=t->getWidth()-2)break;}
}
void windowClose(MyGUI::Window*,const std::string&){destroyWindow();}
MyGUI::TextBox* label(MyGUI::Widget* p,int x,int y,int w,const std::string& value,int height=28){MyGUI::TextBox* t=registerText(p,x,y,std::max(1,w),height,uiFont,value,registerIvory);fitLine(t);return t;}
MyGUI::TextBox* live(MyGUI::Widget* p,int y,const std::string& key){return liveLabels[key]=label(p,0,y,p->getWidth()-20,"");}
MyGUI::ScrollView* panel(MyGUI::Widget* p,int x,int y,int w,int h,const char* title){
 MyGUI::Widget* frame=p->createWidget<MyGUI::Widget>("MercenarieOverviewPanel",x,y,w,h,MyGUI::Align::Default);
 MyGUI::TextBox* heading=registerText(frame,10,5,w-20,26,uiFont,Loc::text(title),registerAmber);fitLine(heading);
 registerSolid(frame,8,32,w-16,1,registerAmber);
 MyGUI::ScrollView* body=frame->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",10,40,w-20,h-50,MyGUI::Align::Default);
 body->setVisibleHScroll(false);body->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);MercenarieNativeInput::bind(body);std::string key=number(page)+title;scrollPanels[key]=body;return body;
}
int bodyWidth(MyGUI::Widget* p){return p->getWidth()-20;}
void canvas(MyGUI::ScrollView* p,int height){for(size_t n=0;n<p->getChildCount();++n){MyGUI::Widget* c=p->getChildAt(n);height=std::max(height,c->getTop()+c->getHeight()+8);}p->setCanvasSize(bodyWidth(p),std::max(1,height));for(std::map<std::string,MyGUI::ScrollView*>::const_iterator i=scrollPanels.begin();i!=scrollPanels.end();++i)if(i->second==p&&scrollOffsets.count(i->first))p->setViewOffset(scrollOffsets[i->first]);}
void choice(MyGUI::Widget* p,int y,GuildGuards::Id chosen){
 tagChoice=p->createWidget<MyGUI::ComboBox>("TheMercenarie_Combo",0,y,bodyWidth(p),28,MyGUI::Align::Default);tagChoice->setComboModeDrop(true);tagChoice->setFontName("MercenarieUnicode");tagChoice->setFontHeight(uiFont);tagChoices.clear();
 for(std::map<GuildGuards::Id,GuildGuards::Tag>::const_iterator i=guildGuards.tags.begin();i!=guildGuards.tags.end();++i){tagChoice->addItem(i->second.name);tagChoices.push_back(i->first);}
 if(!tagChoices.empty())tagChoice->setIndexSelected(0);for(size_t n=0;n<tagChoices.size();++n)if(tagChoices[n]==chosen)tagChoice->setIndexSelected(n);
}
void help(MyGUI::Widget* p,int y,const char* key){
 MyGUI::TextBox* t=label(p,0,y,bodyWidth(p),"",96);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
 MercenarieFonts::caption(t,RerollPopupLayout::wrap(Loc::text(key),bodyWidth(p)-4,LauncherMeasure(t)));t->setSize(t->getWidth(),t->getTextSize().height+6);
}
GuildGuards::State guardState(const GuildGuards::Guard& g){
 if(guildGuards.paused||g.paused)return GuildGuards::Paused;
 std::map<std::string,GuildGuards::Runtime>::const_iterator i=allocator.runtime.find(g.actor);return i==allocator.runtime.end()?GuildGuards::Waiting:i->second.state;
}
MyGUI::Colour stateColour(GuildGuards::State s){if(s==GuildGuards::OnPost)return MyGUI::Colour(.40f,.74f,.22f);if(s==GuildGuards::Combat||s==GuildGuards::KnockedOut)return MyGUI::Colour(.93f,.29f,.16f);if(s==GuildGuards::Travelling||s==GuildGuards::Returning)return registerAmber;return registerIvory;}
std::string postName(const GuildGuards::Guard& g){std::map<GuildGuards::Id,GuildGuards::Post>::const_iterator p=guildGuards.posts.find(g.assigned);return p==guildGuards.posts.end()?Loc::text("guards.none"):p->second.name;}
void roster(MyGUI::ScrollView* p,bool overview){
 int row=0,w=bodyWidth(p);for(std::map<std::string,GuildGuards::Guard>::const_iterator i=guildGuards.guards.begin();i!=guildGuards.guards.end();++i){
 button(p,0,row,w,i->second.name,"guard:"+i->first)->setStateSelected(selectedGuard==i->first);
 statusLabels[i->first]=label(p,6,row+42,w-6,"");row+=overview?104:76;
 if(overview)liveLabels["post/"+i->first]=label(p,6,row-30,w-6,postName(i->second));
 }
 if(!row){help(p,0,"guards.empty_guards");row=110;}
 if(!overview){button(p,0,row,w,Loc::text("guards.add_guard"),"add_guard");row+=48;}canvas(p,row);
}
#include "GuardsOverviewView.h"
#include "GuardsPeopleView.h"
#include "GuardsPostsView.h"
#include "GuardsTagsView.h"
void build(){
 destroyWindow();viewport=MyGUI::RenderManager::getInstance().getViewSize();GuildResponsive::Metrics dimensions=GuildResponsive::calculate(viewport.width,viewport.height);int w=dimensions.windowW,h=dimensions.windowH;uiFont=viewport.width<1100?14:(viewport.width<1920?17:(viewport.width<2560?20:22));
 if(!MyGUI::ResourceManager::getInstance().isExist("MercenarieOverviewPanel"))MyGUI::ResourceManager::getInstance().load("MercenarieOverviewSkins.xml");
 if(!MyGUI::ResourceManager::getInstance().isExist("MercenarieGuildNav"))MyGUI::ResourceManager::getInstance().load("MercenarieGuildFinish.xml");
 window=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("MercenarieGuildWindow",(viewport.width-w)/2,(viewport.height-h)/2,w,h,MyGUI::Align::Default,"Window");MercenarieFonts::caption(window,Loc::text("guards.management"));window->eventWindowButtonPressed+=MyGUI::newDelegate(windowClose);
 MyGUI::Widget* p=window->getClientWidget();w=p->getWidth();h=p->getHeight();registerSolid(p,0,0,w,h,MyGUI::Colour(.025f,.04f,.045f));
 if(page==0){buildOverview(p,w,h);dirty=false;return;}
 if(page==1){buildPeople(p,w,h);dirty=false;return;}
 if(page==2){buildPosts(p,w,h);dirty=false;return;}
 buildTags(p,w,h);dirty=false;
}
void setLive(const std::string& key,const std::string& value){if(liveLabels.count(key)){MercenarieFonts::caption(liveLabels[key],value);fitLine(liveLabels[key]);}}
void countLive(const char* key,unsigned count){setLive(key,page==0?number(count):number(count)+"  "+Loc::text((std::string("guards.")+key).c_str()));}
void refreshUi(){
 if(!v9WidgetLive(window))return;const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();if(dirty)build();else if(v.width!=viewport.width||v.height!=viewport.height){
 // Preserve unsaved form input while adapting to the new viewport.
 std::string name=nameEdit?nameEdit->getOnlyText():"",priority=priorityEdit?priorityEdit->getOnlyText():"",angle=angleEdit?angleEdit->getOnlyText():"";GuildGuards::Id tag=chosenTag();double savedDraft=draftHeading;std::string savedDisplay=displayedAngle;build();draftHeading=savedDraft;displayedAngle=savedDisplay;
 if(nameEdit)nameEdit->setOnlyText(name);if(priorityEdit)priorityEdit->setOnlyText(priority);if(angleEdit)angleEdit->setOnlyText(angle);for(size_t n=0;tagChoice&&n<tagChoices.size();++n)if(tagChoices[n]==tag)tagChoice->setIndexSelected(n);
 }
 unsigned active=0,moving=0,resting=0,paused=0,waiting=0;
 for(std::map<std::string,GuildGuards::Guard>::const_iterator i=guildGuards.guards.begin();i!=guildGuards.guards.end();++i){GuildGuards::State s=guardState(i->second);if(s==GuildGuards::OnPost||s==GuildGuards::Combat||s==GuildGuards::Returning||s==GuildGuards::Travelling)++active;if(s==GuildGuards::Travelling||s==GuildGuards::Returning)++moving;if(s==GuildGuards::Resting||s==GuildGuards::SeekingBed)++resting;if(s==GuildGuards::Paused)++paused;if(s==GuildGuards::Waiting)++waiting;
 if(statusLabels.count(i->first)){MercenarieFonts::caption(statusLabels[i->first],Loc::text(page==1?peopleStateKey(s):stateKey(s)));statusLabels[i->first]->setTextColour(page==1?peopleColour(s):stateColour(s));fitLine(statusLabels[i->first]);}setLive("post/"+i->first,postName(i->second));
 }
 countLive("total",guildGuards.guards.size());countLive("serving",active);countLive("moving",moving);countLive("resting_count",resting);countLive("paused_count",paused);countLive("waiting_count",waiting);
 unsigned states[4]={0};for(std::map<GuildGuards::Id,GuildGuards::Post>::const_iterator i=guildGuards.posts.begin();i!=guildGuards.posts.end();++i){int state=allocator.status(guildGuards,i->first);++states[state];if(postRows.count(i->first)){MyGUI::Button* b=postRows[i->first];const char* keys[]={"guards.free","guards.occupied","guards.reserved","guards.invalid"};setButtonText(b,i->second.name+"\n"+Loc::text(keys[state]));}}
 if(overviewOccupancy){int filled=guildGuards.posts.empty()?0:(int)(overviewOccupancyWidth*states[GuildGuards::Occupied]/guildGuards.posts.size());overviewOccupancy->setVisible(filled>0);overviewOccupancy->setSize(std::max(1,filled),overviewOccupancy->getHeight());}
 countLive("post_total",guildGuards.posts.size());countLive("occupied",states[GuildGuards::Occupied]);countLive("free",states[GuildGuards::Free]);countLive("reserved",states[GuildGuards::Reserved]);unsigned tags=0;for(std::map<GuildGuards::Id,GuildGuards::Tag>::const_iterator i=guildGuards.tags.begin();i!=guildGuards.tags.end();++i)if(!i->second.paused)++tags;countLive("active_tags",tags);
 if(guildGuards.guards.count(selectedGuard)){const GuildGuards::Guard& g=guildGuards.guards[selectedGuard];setLive("detail_state",Loc::text(stateKey(guardState(g))));setLive("detail_post",postName(g));Character* c=resolve(selectedGuard);setLive("detail_health",std::string(Loc::text("guards.health"))+" : "+(c?decimal(std::floor(health(c)*100+.5))+" %":Loc::text("guards.unavailable")));}
 refreshPeople();refreshPosts();refreshTags();
 if(compass&&angleEdit){double heading=0;std::istringstream input(angleEdit->getOnlyText());input>>heading;if(angleEdit->getOnlyText()==displayedAngle)heading=draftHeading;double angle=heading*3.141592653589793/180,radius=compass->getWidth()*.36,center=compass->getWidth()/2.;for(int n=0;n<20;++n){double along=n<14?radius*n/13:radius-radius*.15*((n-14)/2+1),side=n<14?0:((n%2)?1:-1)*radius*.15*((n-14)/2+1);compassDots[n]->setPosition((int)(center+std::sin(angle)*along+std::cos(angle)*side)-2,(int)(center-std::cos(angle)*along+std::sin(angle)*side)-2);}
 if(guildGuards.posts.count(selectedPost)){const GuildGuards::Position& pos=guildGuards.posts[selectedPost].position;setLive("position",decimal(std::floor(pos.x+.5))+" / "+decimal(std::floor(pos.y+.5))+" / "+decimal(std::floor(pos.z+.5)));}}
}
void uiFrame(){if(!window)return;const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();if(dirty||v.width!=viewport.width||v.height!=viewport.height)refreshUi();}
