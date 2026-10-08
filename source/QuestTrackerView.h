#pragma once
#include "Localization.h"
#include "QuestTrackerText.h"
#include "QuestDelegatedProgress.h"
struct QuestTrackerItem {
 int type,slot,percent;bool bounty,delegated,timingValid;double remaining,deadline;
 std::string typeLabel,objective,state,distance,location,identity;
 void (*click)(MyGUI::WidgetPtr);bool clickable;
 QuestTrackerItem():type(0),slot(0),percent(0),bounty(false),delegated(false),timingValid(false),remaining(0),deadline(0),click(0),clickable(false){}
};
std::vector<QuestTrackerItem> questTrackerItems;
struct QuestTrackerCard {
 MyGUI::Button* row;MyGUI::ImageBox *icon,*location;
 MyGUI::TextBox *type,*objective,*state,*distance,*place,*percent;
 MyGUI::Widget *bar,*fill;
};
std::vector<QuestTrackerCard> questTrackerCards;
MyGUI::ScrollView* questTrackerScroll=0;
MyGUI::Widget* questTrackerBackground=0;
MyGUI::TextBox *questTrackerEmpty=0,*questTrackerCount=0,*questTrackerTitle=0,*questTrackerSubtitle=0;
MyGUI::Button* questTrackerTabs[2]={0};
MyGUI::TextBox *questTrackerTabTitles[2]={0},*questTrackerTabSubs[2]={0};
MyGUI::ImageBox* questTrackerTabIcons[2]={0};
int questTrackerTab=0,questTrackerViewportW=0,questTrackerViewportH=0,questTrackerContentY=0;
MyGUI::IntPoint questTrackerOffsets[2];
std::string questTrackerLanguage,questTrackerSignature;
void refreshDynamicTracker();
void appendBountyTrackerItem();void appendMailTrackerItems();
void resetQuestTrackerView(){
 questTrackerCards.clear();questTrackerItems.clear();questTrackerScroll=0;questTrackerBackground=0;questTrackerEmpty=0;questTrackerCount=0;questTrackerTitle=questTrackerSubtitle=0;
 for(int i=0;i<2;++i){questTrackerTabs[i]=0;questTrackerTabTitles[i]=questTrackerTabSubs[i]=0;questTrackerTabIcons[i]=0;questTrackerOffsets[i]=MyGUI::IntPoint(0,0);}
 questTrackerTab=0;questTrackerViewportW=questTrackerViewportH=0;questTrackerLanguage.clear();questTrackerSignature.clear();
}
void questTrackerClose(MyGUI::WidgetPtr){if(trackerWindow)trackerWindow->setVisible(false);}
void questTrackerClick(MyGUI::WidgetPtr sender){
 for(size_t i=0;i<questTrackerItems.size()&&i<questTrackerCards.size();++i)if(questTrackerCards[i].row==sender){
  const QuestTrackerItem item=questTrackerItems[i];if(item.delegated||!item.clickable||!item.click||questPanelLocked())return;
  // Mail owns its own index space; never switch escort context for a letter.
  if(item.type!=4)selectTrackerQuest(item.slot,item.bounty);item.click(sender);return;
 }
}
MyGUI::ImageBox* questIcon(MyGUI::Widget* p,int slot,int x,int y,int size){
 MyGUI::ImageBox* t=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);t->setImageTexture("MercenarieQuestIcons.png");t->setImageCoord(MyGUI::IntCoord(slot*96,0,96,96));t->setColour(launcherV9Sand);t->setNeedMouseFocus(false);return t;
}
int questText(MyGUI::TextBox* t,const std::string& text){return launcherWrap(t,text);}
std::string questHours(double remaining){
 int hours=(int)std::ceil(std::max(0.0,remaining));std::ostringstream s;
 if(hours>=24)s<<hours/24<<Loc::text("quest.v9.day")<<" ";s<<hours%24<<Loc::text("quest.v9.hour");return s.str();
}
void questTrackerChooseTab(MyGUI::WidgetPtr sender){
 int tab=sender==questTrackerTabs[1]?1:0;if(tab==questTrackerTab)return;
 questTrackerOffsets[questTrackerTab]=questTrackerScroll->getViewOffset();questTrackerTab=tab;
 questTrackerScroll->setViewOffset(questTrackerOffsets[tab]);questTrackerSignature.clear();refreshDynamicTracker();
}
void createDynamicTracker(){
 if(trackerWindow)return;launcherEnsureUi();
 MyGUI::ResourceManager& resources=MyGUI::ResourceManager::getInstance();if(!resources.isExist("QuestTrackerV9Window"))resources.load("MercenarieQuestTracker.xml");
 const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();questTrackerViewportW=view.width;questTrackerViewportH=view.height;questTrackerLanguage=Loc::engine().language;
 int w=std::min(720,view.width-32),h=std::min(900,view.height-32);
 int x=std::max(16,std::min((int)(view.width*.045f),view.width-w-16)),y=std::max(16,std::min((int)(view.height*.075f),view.height-h-16));
 trackerWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("QuestTrackerV9Window",x,y,w,h,MyGUI::Align::Default,"Window","GuildEscortTrackerWindow");
 trackerWindow->eventWindowButtonPressed+=MyGUI::newDelegate(closeMercenarieInterface);
 MyGUI::Widget* c=trackerWindow->getClientWidget();questTrackerBackground=c->createWidget<MyGUI::Widget>("LauncherV9Frame",0,0,w,h,MyGUI::Align::Default);questTrackerBackground->setNeedMouseFocus(false);
 launcherLogo(c,24,17,66);
 MyGUI::TextBox* brand=launcherV9Text(c,112,24,w-170,32,24,Loc::text("ui.the_mercenarie"),MyGUI::Colour(.94f,.94f,.90f),true);
 int brandH=questText(brand,Loc::text("ui.the_mercenarie"));
 MyGUI::TextBox* slogan=launcherV9Text(c,112,28+brandH,w-140,36,14,Loc::text("ui.contracts_a_guild_a_lawless_world"),launcherV9Sand);
 int sloganH=questText(slogan,Loc::text("ui.contracts_a_guild_a_lawless_world"));int header=std::max(104,40+brandH+sloganH);
 MyGUI::Button* close=c->createWidget<MyGUI::Button>("LauncherV9Close",w-50,24,28,28,MyGUI::Align::Default);
 launcherV9Text(close,6,3,20,24,18,"X",launcherV9Sand);close->eventMouseButtonClick+=MyGUI::newDelegate(questTrackerClose);
 launcherV9Solid(c,22,header,w-44,1,launcherV9Orange);launcherV9Icon(c,0,28,header+12,52);
 questTrackerTitle=launcherV9Text(c,98,header+14,w-122,32,24,Loc::text("quest.v9.title"),launcherV9Orange,true);
 int titleH=questText(questTrackerTitle,Loc::text("quest.v9.title"));
 questTrackerSubtitle=launcherV9Text(c,98,header+18+titleH,w-122,24,16,Loc::text("quest.v9.subtitle"),MyGUI::Colour(.88f,.89f,.85f));
 int subtitleH=questText(questTrackerSubtitle,Loc::text("quest.v9.subtitle"));int tabsY=header+std::max(78,titleH+subtitleH+26),tabW=(w-54)/2,tabH=76;
 const char* names[]={"quest.v9.player","quest.v9.guild"};const char* subs[]={"quest.v9.player_sub","quest.v9.guild_sub"};
 for(int i=0;i<2;++i){
  MyGUI::Button* b=c->createWidget<MyGUI::Button>("QuestTrackerV9Tab",22+i*(tabW+10),tabsY,tabW,76,MyGUI::Align::Default);questTrackerTabs[i]=b;b->eventMouseButtonClick+=MyGUI::newDelegate(questTrackerChooseTab);
  questTrackerTabIcons[i]=questIcon(b,i?6:4,16,16,42);
  questTrackerTabTitles[i]=launcherV9Text(b,72,13,tabW-84,28,18,Loc::text(names[i]),launcherV9Sand);
  int th=questText(questTrackerTabTitles[i],Loc::text(names[i]));
  questTrackerTabSubs[i]=launcherV9Text(b,72,17+th,tabW-84,24,16,Loc::text(subs[i]),launcherV9Sand);
  int sh=questText(questTrackerTabSubs[i],Loc::text(subs[i]));tabH=std::max(tabH,th+sh+30);
 }
 for(int i=0;i<2;++i)questTrackerTabs[i]->setSize(tabW,tabH);
 questTrackerContentY=tabsY+tabH+18;
 questTrackerScroll=c->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",22,questTrackerContentY,w-44,h-questTrackerContentY-48,MyGUI::Align::Default);
 MercenarieNativeInput::bind(questTrackerScroll);questTrackerScroll->setVisibleHScroll(false);questTrackerScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);
 questTrackerEmpty=launcherV9Text(c,38,questTrackerContentY+28,w-76,70,20,"",launcherV9Sand);questTrackerEmpty->setTextAlign(MyGUI::Align::Center);
 questTrackerCount=launcherV9Text(c,24,h-36,w-48,26,18,"",MyGUI::Colour(.85f,.87f,.84f));
 trackerWindow->setVisible(false);questTrackerSignature.clear();
}
#include "QuestTrackerPlayerAdapter.h"
struct QuestReturnSooner{bool operator()(const QuestTrackerItem& a,const QuestTrackerItem& b)const{return a.deadline<b.deadline;}};
void appendDelegatedTrackerItems(){
 for(size_t i=0;i<delegatedMissions.size();++i){
  const DelegatedMissionState& m=delegatedMissions[i];if(m.completed||!m.timing.active)continue;
  QuestTrackerItem item;item.delegated=true;item.slot=(int)i;item.identity=m.groupId;item.deadline=m.timing.exactReturnWorldHour;
  item.type=QuestDelegatedProgress::missionType(m.timing.activityType);item.typeLabel=mercenarieLocalize(m.title);
  item.objective=mercenarieLocalize(m.origin)+QuestTrackerText::arrow()+mercenarieLocalize(m.destination);item.location=mercenarieLocalize(m.destination);
  QuestDelegatedProgress::Value p=QuestDelegatedProgress::read(m.timing,currentGameHours);item.timingValid=p.valid;item.percent=p.percent;item.remaining=p.remaining;
  questTrackerItems.push_back(item);
 }
 std::stable_sort(questTrackerItems.begin(),questTrackerItems.end(),QuestReturnSooner());
}
void refreshDynamicTracker(){
 if(!trackerWindow||!questTrackerScroll)return;
 const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
 if(view.width!=questTrackerViewportW||view.height!=questTrackerViewportH||questTrackerLanguage!=Loc::engine().language){
  bool visible=trackerWindow->getVisible();int tab=questTrackerTab,x=trackerWindow->getLeft(),y=trackerWindow->getTop();MyGUI::IntPoint offset=questTrackerScroll->getViewOffset();
  mercenarieDestroyLiveWidget(trackerWindow);trackerWindow=0;resetQuestTrackerView();questTrackerTab=tab;createDynamicTracker();
  trackerWindow->setPosition(std::max(0,std::min(x,view.width-trackerWindow->getWidth())),std::max(0,std::min(y,view.height-trackerWindow->getHeight())));trackerWindow->setVisible(visible);questTrackerScroll->setViewOffset(offset);
 }
 questTrackerItems.clear();if(questTrackerTab)appendDelegatedTrackerItems();else{appendAllQuestItems();appendMailTrackerItems();}
 std::ostringstream signature;signature<<questTrackerTab<<'|'<<questTrackerLanguage;
 for(size_t i=0;i<questTrackerItems.size();++i){const QuestTrackerItem& q=questTrackerItems[i];signature<<'\n'<<q.type<<'|'<<q.slot<<'|'<<q.bounty<<q.clickable<<q.delegated<<q.timingValid<<'|'<<q.typeLabel<<'|'<<q.objective<<'|'<<q.location<<'|'<<q.state<<'|'<<q.distance<<'|'<<q.identity<<'|'<<q.percent<<'|'<<(q.timingValid?(int)std::ceil(q.remaining):0);}
 if(signature.str()==questTrackerSignature)return;questTrackerSignature=signature.str();
 int count=(int)questTrackerItems.size(),rowW=trackerWindow->getWidth()-68;
 for(int i=0;i<2;++i){bool selected=i==questTrackerTab;questTrackerTabs[i]->setStateSelected(selected);MyGUI::Colour color=selected?launcherV9Orange:launcherV9Sand;questTrackerTabIcons[i]->setColour(color);questTrackerTabTitles[i]->setTextColour(color);questTrackerTabSubs[i]->setTextColour(color);}
 questText(questTrackerCount,Loc::count(questTrackerTab?"quest.v9.guild_count":"quest.v9.player_count",count));
 questText(questTrackerEmpty,Loc::text(questTrackerTab?"quest.v9.empty_guild":"quest.v9.empty_player"));questTrackerEmpty->setVisible(count==0);questTrackerScroll->setVisible(count>0);
 while(questTrackerCards.size()<questTrackerItems.size()){
  QuestTrackerCard card;card.row=questTrackerScroll->createWidget<MyGUI::Button>("LauncherV9Card",0,0,rowW,112,MyGUI::Align::Default);card.row->eventMouseButtonClick+=MyGUI::newDelegate(questTrackerClick);
  card.icon=questIcon(card.row,0,12,29,48);card.location=questIcon(card.row,5,0,0,23);
  card.type=launcherV9Text(card.row,78,14,rowW-100,26,20,"",MyGUI::Colour(.93f,.93f,.90f));
  card.objective=launcherV9Text(card.row,78,40,rowW-100,24,18,"",MyGUI::Colour(.84f,.86f,.83f));
  card.distance=launcherV9Text(card.row,78,68,rowW-100,24,16,"",MyGUI::Colour(.79f,.82f,.79f));
  card.place=launcherV9Text(card.row,0,0,100,24,16,"",MyGUI::Colour(.89f,.89f,.85f));
  card.state=launcherV9Text(card.row,0,0,100,30,14,"",launcherV9Orange);
  card.percent=launcherV9Text(card.row,0,0,64,26,18,"",MyGUI::Colour(.94f,.93f,.88f));
  card.bar=launcherV9Solid(card.row,0,0,100,14,MyGUI::Colour(.32f,.34f,.31f));launcherV9Solid(card.bar,1,1,98,12,MyGUI::Colour(.025f,.035f,.035f));
  card.fill=launcherV9Solid(card.bar,1,1,1,12,launcherV9Orange);questTrackerCards.push_back(card);
 }
 int y=0;
 for(size_t i=0;i<questTrackerCards.size();++i){
  QuestTrackerCard& c=questTrackerCards[i];bool active=i<questTrackerItems.size();c.row->setVisible(active);if(!active)continue;
  const QuestTrackerItem& item=questTrackerItems[i];int right=rowW*62/100,mainW=right-90,rightW=rowW-right-16;
  c.type->setSize(mainW,26);c.objective->setSize(mainW,24);c.distance->setSize(mainW,24);
  std::string label=item.typeLabel.empty()?missionLabelV6(item.type):item.typeLabel;
  int th=questText(c.type,label);c.type->setPosition(78,14);
  int oh=questText(c.objective,item.objective);c.objective->setPosition(78,18+th);
  std::string detail=item.delegated?(item.timingValid?Loc::named("quest.v9.return","time",questHours(item.remaining)):Loc::text("quest.v9.unknown_time")):(item.distance.empty()?Loc::text("quest.v9.unknown_distance"):Loc::named("quest.v9.distance","distance",item.distance));
  int dh=questText(c.distance,detail);c.distance->setPosition(78,22+th+oh);
  c.location->setPosition(right,16);c.location->setVisible(!item.location.empty());c.place->setSize(rightW-28,24);c.place->setPosition(right+28,16);int ph=questText(c.place,item.location);
  c.state->setSize(rightW,26);c.state->setPosition(right,24+ph);int sh=questText(c.state,item.delegated?"":item.state);c.state->setVisible(!item.delegated);
  c.bar->setVisible(item.delegated&&item.timingValid);c.percent->setVisible(item.delegated&&item.timingValid);
  if(item.delegated&&item.timingValid){int barW=rightW-72;c.bar->setPosition(right,28+ph);c.bar->setSize(barW,14);c.bar->getChildAt(0)->setSize(barW-2,12);c.fill->setSize(std::max(1,(barW-2)*item.percent/100),12);c.fill->setVisible(item.percent>0);c.percent->setPosition(right+barW+8,24+ph);std::ostringstream percent;percent<<item.percent<<" %";questText(c.percent,percent.str());}
  int rowH=std::max(112,std::max(34+th+oh+dh,42+ph+(item.delegated?26:sh)));c.row->setPosition(0,y);c.row->setSize(rowW,rowH);c.icon->setPosition(12,(rowH-48)/2);setMissionIconV6(c.icon,item.type);
  y+=rowH+10;
 }
 // Grow with actual rows; cap the window to the viewport and scroll beyond it.
 int contentHeight=count?std::max(1,y-10):std::max(92,questTrackerEmpty->getHeight()+40);
 int height=std::min(std::min(900,view.height-32),questTrackerContentY+contentHeight+48);
 trackerWindow->setSize(trackerWindow->getWidth(),height);questTrackerBackground->setSize(trackerWindow->getWidth(),height);
 trackerWindow->setPosition(trackerWindow->getLeft(),std::max(0,std::min(trackerWindow->getTop(),view.height-height-16)));
 questTrackerCount->setPosition(24,height-36);questTrackerScroll->setSize(questTrackerScroll->getWidth(),std::max(1,height-questTrackerContentY-48));
 int body=questTrackerScroll->getHeight(),canvas=std::max(body,y?y-10:0);questTrackerScroll->setCanvasSize(rowW,canvas);questTrackerScroll->setVisibleVScroll(canvas>body);
 MyGUI::IntPoint offset=questTrackerScroll->getViewOffset();offset.top=std::max(body-canvas,std::min(0,offset.top));offset.left=0;questTrackerScroll->setViewOffset(offset);questTrackerOffsets[questTrackerTab]=offset;
}
