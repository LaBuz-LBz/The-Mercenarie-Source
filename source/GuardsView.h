#pragma once
namespace GuardsNative {
MyGUI::Window* window=0;
MyGUI::ScrollView* list=0;
MyGUI::EditBox *nameEdit=0,*priorityEdit=0,*angleEdit=0;
MyGUI::ComboBox* tagChoice=0;
MyGUI::EditBox* tagDescriptionEdit=0;
std::vector<GuildGuards::Id> tagChoices;
GuildGuards::Id selectedPost=0,selectedTag=0;
std::string selectedGuard,confirmation;
int page=0;
std::map<GuildGuards::Id,bool> collapsedTags;
std::map<std::string,MyGUI::TextBox*> liveLabels;
MyGUI::Widget* compass=0;
std::vector<MyGUI::Widget*> compassDots;
std::string displayedAngle;
double draftHeading=0;
std::map<std::string,MyGUI::ScrollView*> scrollPanels;
std::map<std::string,MyGUI::IntPoint> scrollOffsets;
int uiFont=15;
std::map<MyGUI::Button*,MyGUI::TextBox*> buttonCaptions;
MyGUI::Widget* overviewOccupancy=0;
int overviewOccupancyWidth=0;
bool preview=false,dirty=true;
MyGUI::IntSize viewport;
std::map<std::string,MyGUI::TextBox*> statusLabels;
std::map<GuildGuards::Id,MyGUI::Button*> postRows,tagRows;
std::string number(GuildGuards::Id n){std::ostringstream s;s<<n;return s.str();}
GuildGuards::Id id(const std::string& s){std::istringstream in(s);GuildGuards::Id n=0;in>>n;return n;}
std::string decimal(double n){std::ostringstream s;s.precision(12);s<<n;return s.str();}
const char* stateKey(GuildGuards::State s){const char* keys[]={"guards.on_post","guards.travelling","guards.combat","guards.returning","guards.eating","guards.seeking_bed","guards.resting","guards.waiting","guards.paused","guards.ko","guards.unavailable"};return keys[(int)s];}
void click(MyGUI::Widget*);
void build();
void clearPreview();
void setButtonText(MyGUI::Button* b,const std::string& value){
 MyGUI::TextBox*& t=buttonCaptions[b];if(!t)t=registerText(b,6,2,std::max(1,b->getWidth()-12),std::max(1,b->getHeight()-4),uiFont,"",registerAmber);
 t->setCoord(6,2,std::max(1,b->getWidth()-12),std::max(1,b->getHeight()-4));t->setTextAlign(MyGUI::Align::Center);t->setNeedMouseFocus(false);
 b->setUserString("guardCaption",value);std::string visible=value;MercenarieFonts::caption(t,visible);fitRegisterText(t,uiFont);
 while(!visible.empty()&&(t->getTextSize().width>t->getWidth()-2||t->getTextSize().height>t->getHeight())){size_t end=visible.size()-1;while(end>0&&((unsigned char)visible[end]&0xc0)==0x80)--end;visible.erase(end);MercenarieFonts::caption(t,visible+"...");fitRegisterText(t,uiFont);}
}
MyGUI::Button* button(MyGUI::Widget* p,int x,int y,int w,const std::string& text,const std::string& action){MyGUI::Button* b=p->createWidget<MyGUI::Button>("MercenarieGuildNav",x,y,std::max(1,w),40,MyGUI::Align::Default);setButtonText(b,text);b->setUserString("guardAction",action);b->eventMouseButtonClick+=MyGUI::newDelegate(click);if(action.compare(0,5,"post:")==0)postRows[id(action.substr(5))]=b;if(action.compare(0,4,"tag:")==0)tagRows[id(action.substr(4))]=b;return b;}
MyGUI::TextBox* text(MyGUI::Widget* p,int x,int y,int w,const std::string& caption){return registerText(p,x,y,std::max(1,w),28,15,caption,registerIvory);}
void destroyWindow(){for(std::map<std::string,MyGUI::ScrollView*>::iterator i=scrollPanels.begin();i!=scrollPanels.end();++i)scrollOffsets[i->first]=i->second->getViewOffset();scrollPanels.clear();if(v9WidgetLive(window))mercenarieDestroyLiveWidget(window);window=0;list=0;nameEdit=priorityEdit=angleEdit=tagDescriptionEdit=0;tagChoice=0;buttonCaptions.clear();overviewOccupancy=0;statusLabels.clear();postRows.clear();tagRows.clear();liveLabels.clear();compass=0;compassDots.clear();}
void closeUi(){destroyWindow();clearPreview();preview=false;selectedPost=selectedTag=0;selectedGuard.clear();confirmation.clear();scrollOffsets.clear();dirty=true;}
void open(MyGUI::Widget*){if(mercenarieGameplayUnavailable()){showMercenarieUnavailable();return;}closeMercenarieLauncher(0);if(!hooksReady){if(ou)ou->showPlayerAMessage(Loc::text("guards.hooks_unavailable"),true);return;}page=0;build();}
Character* selected(){return ou&&ou->player?ou->player->selectedCharacter.getCharacter():0;}
GuildGuards::Id chosenTag(){size_t n=tagChoice?tagChoice->getIndexSelected():MyGUI::ITEM_NONE;return n<tagChoices.size()?tagChoices[n]:0;}
void click(MyGUI::Widget* w){
    const std::string action=w->getUserString("guardAction");
    if(action=="close"){destroyWindow();return;}
    if(action=="preview"){preview=!preview;if(!preview)clearPreview();else destroyWindow();dirty=true;return;}
    if(action=="global"){guildGuards.paused=!guildGuards.paused;if(guildGuards.paused)for(std::map<std::string,GuildGuards::Guard>::const_iterator i=guildGuards.guards.begin();i!=guildGuards.guards.end();++i)releaseOrder(i->first);dirty=true;return;}
    if(action.compare(0,4,"tab:")==0){page=atoi(action.substr(4).c_str());confirmation.clear();dirty=true;return;}
    if(action.compare(0,5,"post:")==0){selectedPost=id(action.substr(5));page=2;confirmation.clear();dirty=true;if(!window)build();return;}
    if(action.compare(0,4,"tag:")==0){selectedTag=id(action.substr(4));confirmation.clear();dirty=true;return;}
    if(action.compare(0,6,"guard:")==0){selectedGuard=action.substr(6);page=1;confirmation.clear();dirty=true;return;}
    if(action=="add_guard"){Character* c=selected();if(c&&c->isPlayerCharacter()&&!c->isAnimal()&&!c->isDead()){std::string key=c->getHandle().toString();if(!guildGuards.guards.count(key)){GuildGuards::Guard g;g.actor=key;g.name=c->getName();g.robot=c->getRace()&&c->getRace()->robot;guildGuards.guards[key]=g;}selectedGuard=key;page=1;}else if(ou)ou->showPlayerAMessage(Loc::text("guards.select_character"),true);}
    else if(action=="add_tag"){GuildGuards::Tag t;t.id=guildGuards.next();t.name=nameEdit?nameEdit->getOnlyText():"";if(t.name.empty())t.name=Loc::text("guards.tag");guildGuards.tags[t.id]=t;selectedTag=t.id;}
    else if(action=="save_tag"&&guildGuards.tags.count(selectedTag)){guildGuards.tags[selectedTag].name=nameEdit->getOnlyText();}
    else if(action=="pause_tag"&&guildGuards.tags.count(selectedTag)){guildGuards.tags[selectedTag].paused=!guildGuards.tags[selectedTag].paused;}
    else if(action=="add_post"||action=="position"){
        Character* c=selected();GuildGuards::Id tag=chosenTag();if(!c||!c->isPlayerCharacter()||!tag){if(ou)ou->showPlayerAMessage(Loc::text("guards.select_post_source"),true);return;}
        GuildGuards::Post p;if(action=="position"){if(!guildGuards.posts.count(selectedPost))return;p=guildGuards.posts[selectedPost];}else{p.id=guildGuards.next();p.tag=tag;p.name=nameEdit->getOnlyText();if(p.name.empty())p.name=Loc::text("guards.post");}
        p.position=position(c->getPosition());Ogre::Vector3 facing=c->getMovement()->getFacingDirection();p.heading=std::atan2(facing.x,facing.z)*180/3.141592653589793;guildGuards.posts[p.id]=p;selectedPost=p.id;
    }else if(action=="save_post"&&guildGuards.posts.count(selectedPost)){
        GuildGuards::Post& p=guildGuards.posts[selectedPost];double angle;int priority;std::istringstream a(angleEdit->getOnlyText()),r(priorityEdit->getOnlyText());if(!(a>>angle)||!(r>>priority)||!(angle>=-1e9&&angle<=1e9)||!chosenTag()){if(ou)ou->showPlayerAMessage(Loc::text("guards.invalid_fields"),true);return;}p.name=nameEdit->getOnlyText();p.tag=chosenTag();p.priority=std::max(1,priority);p.heading=angleEdit->getOnlyText()==displayedAngle?draftHeading:angle;
    }else if((action=="left"||action=="right")&&angleEdit){double angle=0;std::istringstream value(angleEdit->getOnlyText());value>>angle;if(angleEdit->getOnlyText()==displayedAngle)angle=draftHeading;draftHeading=angle+(action=="left"?-1:1);displayedAngle=decimal(std::floor(draftHeading+.5));angleEdit->setOnlyText(displayedAngle);return;}
    else if(action.compare(0,5,"fold:")==0){GuildGuards::Id tag=id(action.substr(5));collapsedTags[tag]=!collapsedTags[tag];}
    else if(action.compare(0,3,"up:")==0||action.compare(0,5,"down:")==0){if(guildGuards.guards.count(selectedGuard)){std::vector<GuildGuards::Id>& tags=guildGuards.guards[selectedGuard].tags;int from=atoi(action.substr(action[0]=='u'?3:5).c_str()),to=from+(action[0]=='u'?-1:1);if(from>=0&&to>=0&&from<(int)tags.size()&&to<(int)tags.size())std::swap(tags[from],tags[to]);}}
    else if(action=="world_guard"||action=="world_post"){if(ou&&ou->player&&ou->player->getCamera()){Character* c=action=="world_guard"?resolve(selectedGuard):0;if(c)ou->player->getCamera()->teleport(c->getPosition());else if(action=="world_post"&&guildGuards.posts.count(selectedPost)){ou->player->getCamera()->teleport(position(guildGuards.posts[selectedPost].position));preview=true;}else return;destroyWindow();}return;}
    else if(action=="pause_guard"&&guildGuards.guards.count(selectedGuard)){guildGuards.guards[selectedGuard].paused=!guildGuards.guards[selectedGuard].paused;if(guildGuards.guards[selectedGuard].paused)releaseOrder(selectedGuard);}
    else if(action=="assign_tag"&&guildGuards.guards.count(selectedGuard)&&chosenTag()){std::vector<GuildGuards::Id>& t=guildGuards.guards[selectedGuard].tags;if(std::find(t.begin(),t.end(),chosenTag())==t.end())t.push_back(chosenTag());}
    else if(action.compare(0,6,"untag:")==0&&guildGuards.guards.count(selectedGuard)){std::vector<GuildGuards::Id>& t=guildGuards.guards[selectedGuard].tags;GuildGuards::Id remove=id(action.substr(6));t.erase(std::remove(t.begin(),t.end(),remove),t.end());}
    else if(action=="delete_post"||action=="delete_tag"||action=="delete_guard"){
        std::string token=action+number(selectedPost)+":"+number(selectedTag)+":"+selectedGuard;
        if(confirmation!=token){confirmation=token;MyGUI::Button* b=w->castType<MyGUI::Button>();setButtonText(b,Loc::text(action=="delete_tag"?"guards.confirm_delete_tag":"guards.confirm_delete"));return;}
        if(action=="delete_post")guildGuards.erasePost(selectedPost);else if(action=="delete_tag")guildGuards.eraseTag(selectedTag);else {releaseOrder(selectedGuard);guildGuards.guards.erase(selectedGuard);}confirmation.clear();
    }
    guildGuards.clean();dirty=true;
}
MyGUI::EditBox* edit(MyGUI::Widget* p,int x,int y,int width,const std::string& value){MyGUI::EditBox* e=p->createWidget<MyGUI::EditBox>("Kenshi_EditBox",x,y,width,28,MyGUI::Align::Default);e->setFontName("MercenarieUnicode");e->setFontHeight(uiFont);e->setOnlyText(value);return e;}
#include "GuardsDashboardView.h"
}
