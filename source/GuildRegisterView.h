MyGUI::Widget* overview95Root=0;
void refreshOverview95();
void syncOverview96(int tab);
#include "Localization.h"
// Included inside the plugin namespace. Presentation and investment controls.
MyGUI::TextBox* registerValues[4]={0,0,0,0};
MyGUI::TextBox* registerXpDetail=0;
MyGUI::TextBox *registerQuick[4]={0},*registerFinance=0,*registerInfluence=0,*registerNextTier=0,*registerGlanceStats=0;
MyGUI::Button* registerLevelButtons[10]={0};
MyGUI::ImageBox* registerLevelIcons[10]={0};
MyGUI::TextBox* registerLevelStates[10]={0};
MyGUI::TextBox* registerLevelTitles[10]={0};
MyGUI::Widget* registerLevelAccents[10][4]={{0}};
MyGUI::Button *registerCarouselLeft=0,*registerCarouselRight=0;
int registerCarouselStart=0;
bool registerCarouselInitialized=false;
MyGUI::Widget *registerProgression=0,*registerCarousel=0,*registerNextPanel=0,*registerTimelineTrack=0;
MyGUI::TextBox *registerProgressTitle=0,*registerTimelineNext=0;
MyGUI::ImageBox *registerNextArt=0;
MyGUI::TextBox *overviewNextTitle=0,*overviewNextLevel=0;
MyGUI::ImageBox* overviewTabIcons[6]={0};
MyGUI::TextBox* overviewTabLabels[6]={0};
MyGUI::Button* registerPayrollButton=0;
MyGUI::Button* registerEstateButton=0;
MyGUI::TextBox* registerEstateLabel=0;
void estateShowView(MyGUI::Widget*);
bool estateViewIsOpen();
MyGUI::TextBox* registerPayrollLabel=0;
MyGUI::Widget* registerBrands[2]={0,0};
void openPayrollFinances(MyGUI::Widget*);
bool payrollManagementIsOpen();
void refreshRegisterNavigation();
MyGUI::Widget* registerArrowInk[2][12]={{0}};
GuildProgressLayout::Layout registerProgressGeometry;
void fitRegisterText(MyGUI::TextBox*,int);
MyGUI::TextBox* registerActivityLabels[3]={0};
MyGUI::Widget* registerLevelPopup=0;
MyGUI::TextBox* registerLevelPopupText=0;
MyGUI::ImageBox* registerLevelPopupIcon=0;
MyGUI::Widget* registerLevelTip=0;
MyGUI::TextBox* registerLevelTipText=0;
MyGUI::Widget* registerInfluenceMap=0;
std::vector<MyGUI::ImageBox*> registerOfficeMarkers;
int registerMapCropX=0,registerMapCropY=0,registerMapCropSize=2048;
int registerMapDragX=0,registerMapDragY=0;
MyGUI::Widget* registerXpFill[16]={0};
MyGUI::Widget* registerTimelineFill[16]={0};
MyGUI::TextBox* registerTimelineLevel=0;
MyGUI::TextBox* registerTimelineDetail=0;
MyGUI::Widget* registerTabMarks[6]={0};
MyGUI::Widget* registerTabBorders[6][3]={{0}};
int registerXpWidths[16]={0};
int registerTimelineWidths[16]={0};
int registerSelectedTab=0;
const MyGUI::Colour registerIvory(0.90f,0.87f,0.78f);
const MyGUI::Colour registerAmber(1.0f,0.67f,0.23f);
const MyGUI::Colour registerBlue(0.40f,0.78f,0.88f);
const MyGUI::Colour registerGreen(0.65f,0.83f,0.49f);
std::string registerLanguage(const char* fr,const char* en){return gMercenarieEnglish?en:fr;}
std::string registerNumber(long long n){std::stringstream out;out<<n;std::string s=out.str();for(int i=(int)s.size()-3;i>(n<0?1:0);i-=3)s.insert(i," ");return s;}
MyGUI::TextBox* registerText(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text,const MyGUI::Colour& colour)
{
    MyGUI::TextBox* t=p->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",x,y,w,h,MyGUI::Align::Default);
    MercenarieFonts::caption(t,text);t->setFontHeight(size);t->setTextColour(colour);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);t->setNeedMouseFocus(false);return t;
}
MyGUI::Widget* registerPanel(MyGUI::Widget* p,int x,int y,int w,int h){return p->createWidget<MyGUI::Widget>("GuildRegisterPanel",x,y,w,h,MyGUI::Align::Default);}
MyGUI::Widget* registerSolid(MyGUI::Widget* p,int x,int y,int w,int h,const MyGUI::Colour& colour)
{
    MyGUI::Widget* r=p->createWidget<MyGUI::Widget>("WhiteSkin",x,y,w,h,MyGUI::Align::Default);r->setColour(colour);r->setNeedMouseFocus(false);return r;
}
void registerLine(MyGUI::Widget* p,int x,int y,int w){registerSolid(p,x,y,w,1,MyGUI::Colour(0.40f,0.31f,0.20f));}
void registerArt(MyGUI::Widget* p,const char* texture,int x,int y,int w,int h,float alpha){
    MyGUI::ImageBox* art=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,w,h,MyGUI::Align::Default);
    art->setImageTexture(texture);
    if(std::string(texture)=="GuildManagementScene.png"){int cropW=1536,cropH=1024;if(w*1024>h*1536)cropH=1536*h/w;else cropW=1024*w/h;art->setImageCoord(MyGUI::IntCoord(1536-cropW,0,cropW,cropH));}
    if(std::string(texture)=="LevelSun.png")art->setColour(registerAmber);
    art->setAlpha(alpha);art->setNeedMouseFocus(false);
}
void registerIcon(MyGUI::Widget* p,int id,int x,int y,int size,const MyGUI::Colour& colour)
{
    MyGUI::ImageBox* icon=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);
    icon->setImageTexture("GuildMenuIcons.png");icon->setImageCoord(MyGUI::IntCoord(id*434,135,434,454));icon->setColour(colour);icon->setNeedMouseFocus(false);
}
void registerTabFocus(MyGUI::Widget* sender,MyGUI::Widget*){for(int i=0;i<6;++i)if(sender==guildTabButtons[i]&&overviewTabLabels[i])overviewTabLabels[i]->setTextColour(i==registerSelectedTab?registerAmber:MyGUI::Colour(.98f,.95f,.86f));}
void registerTabBlur(MyGUI::Widget* sender,MyGUI::Widget*){for(int i=0;i<6;++i)if(sender==guildTabButtons[i]&&overviewTabLabels[i])overviewTabLabels[i]->setTextColour(i==registerSelectedTab?registerAmber:registerIvory);}
void registerSelectTab(int tab){syncOverview96(tab);registerSelectedTab=tab;for(int i=0;i<6;++i)if(guildTabButtons[i]){guildTabButtons[i]->setStateSelected(i==tab);if(overviewTabLabels[i])overviewTabLabels[i]->setTextColour(i==tab?registerAmber:registerIvory);if(overviewTabIcons[i])overviewTabIcons[i]->setColour(i==tab?registerAmber:registerIvory);}refreshRegisterNavigation();}
MyGUI::ImageBox* registerSidebarIcon(MyGUI::Widget* p,int id,int x,int y,int size,const MyGUI::Colour& colour)
{
    MyGUI::ImageBox* icon=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);
    const int cellW=2171/6;icon->setImageTexture("GuildSidebarIcons.png");icon->setImageCoord(MyGUI::IntCoord(id*cellW,150,cellW,424));icon->setColour(colour);icon->setNeedMouseFocus(false);return icon;
}
void registerCoverArt(MyGUI::Widget* p,const char* texture,int srcW,int srcH,int x,int y,int w,int h,float alpha)
{
    MyGUI::ImageBox* art=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,w,h,MyGUI::Align::Default);int cropW=srcW,cropH=srcH;if(w*srcH>h*srcW)cropH=srcW*h/w;else cropW=srcH*w/h;art->setImageTexture(texture);art->setImageCoord(MyGUI::IntCoord((srcW-cropW)/2,(srcH-cropH)/2,cropW,cropH));art->setAlpha(alpha);art->setNeedMouseFocus(false);
}
MyGUI::ImageBox* registerTextureIcon(MyGUI::Widget* p,const std::string& texture,int x,int y,int size,const MyGUI::Colour& colour)
{
    MyGUI::ImageBox* icon=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);
    icon->setImageTexture(texture);icon->setColour(colour);icon->setNeedMouseFocus(false);return icon;
}
std::string registerLevelIconTexture(int level)
{
    std::stringstream path;path<<"GuildProgressLevel"<<(level<10?"0":"")<<level<<".png";return path.str();
}
void registerSetProgressIcon(MyGUI::ImageBox* icon,int level){const int ids[]={5,10,1,3,12,13,14,11,0,15};int id=ids[std::max(0,std::min(9,level-1))];if(icon){icon->setImageTexture("MercenarieOverviewIcons.png");icon->setImageCoord(MyGUI::IntCoord(id%4*128,id/4*128,128,128));}}
void guildTabClicked(MyGUI::WidgetPtr);
void openGuildInvestment(MyGUI::Widget*);
void registerOpenContracts(MyGUI::Widget*){if(guildTabButtons[1])guildTabClicked(guildTabButtons[1]);}
void registerOpenInvestment(MyGUI::Widget*){openGuildInvestment(0);}
void registerCloseLevelPopup(MyGUI::Widget*){if(registerLevelPopup)registerLevelPopup->setVisible(false);}
std::string registerUnlockText(int level,bool detailed){std::vector<GuildLevelUI::Unlock> rows=GuildLevelUI::unlocks(level);if(rows.empty())return Loc::text("v8.literal.112");std::stringstream out;for(size_t i=0;i<rows.size();++i){if(i)out<<"\n\n";out<<(gMercenarieEnglish?rows[i].en:rows[i].fr);if(detailed)out<<"\n"<<(gMercenarieEnglish?rows[i].detailEn:rows[i].detailFr)<<"\n"<<(gMercenarieEnglish?rows[i].noteEn:rows[i].noteFr);}return out.str();}
void registerProgressCoord(MyGUI::Widget* w,const GuildResponsive::Rect& r){w->setCoord(r.x,r.y,r.w,r.h);}
void registerLayoutLevelCarousel(){
    for(int i=0;i<10;++i)if(registerLevelButtons[i]){bool visible=i>=registerCarouselStart&&i<registerCarouselStart+5;registerLevelButtons[i]->setVisible(visible);if(visible)registerProgressCoord(registerLevelButtons[i],registerProgressGeometry.cards[i-registerCarouselStart]);}
    if(registerCarouselLeft)registerCarouselLeft->setEnabled(registerCarouselStart>GuildOverview::minStart(std::max(1,currentGuildProgressView().level)));
    if(registerCarouselRight)registerCarouselRight->setEnabled(registerCarouselStart<GuildOverview::maxStart(std::max(1,currentGuildProgressView().level)));
    for(int side=0;side<2;++side)for(int j=0;j<12;++j)if(registerArrowInk[side][j])registerArrowInk[side][j]->setAlpha((side==0?registerCarouselStart>GuildOverview::minStart(std::max(1,currentGuildProgressView().level)):registerCarouselStart<GuildOverview::maxStart(std::max(1,currentGuildProgressView().level)))?1.0f:0.3f);
}
void registerCenterLevelCarousel(int level){registerCarouselStart=GuildProgressLayout::first(level);registerCarouselInitialized=true;registerLayoutLevelCarousel();}
void registerMoveLevelCarousel(MyGUI::Widget* sender){if(!registerCarousel)return;registerCarouselStart=GuildProgressLayout::move(registerCarouselStart,sender==registerCarouselLeft?-1:1,std::max(1,currentGuildProgressView().level));registerCarouselInitialized=true;if(registerLevelTip)registerLevelTip->setVisible(false);registerLayoutLevelCarousel();}
void registerLevelCarouselWheel(MyGUI::WidgetPtr,int rel){if(!registerCarousel)return;registerCarouselStart=GuildProgressLayout::move(registerCarouselStart,rel>0?-1:1,std::max(1,currentGuildProgressView().level));registerCarouselInitialized=true;if(registerLevelTip)registerLevelTip->setVisible(false);registerLayoutLevelCarousel();}
// Run once after the global scale. Navigation subsequently uses these same pixel slots.
void registerReflowProgression(float scale){
    if(!registerProgression)return;
    registerProgressGeometry=GuildProgressLayout::calculate(registerProgression->getWidth(),registerProgression->getHeight(),scale);
    const GuildProgressLayout::Layout& g=registerProgressGeometry;
    registerProgressCoord(registerProgressTitle,g.title);registerProgressCoord(registerTimelineLevel,g.current);
    registerProgressCoord(registerTimelineDetail,g.xp);registerProgressCoord(registerTimelineNext,g.next);
    registerProgressCoord(registerTimelineTrack,g.bar);registerTimelineTrack->setColour(MyGUI::Colour(.02f,.035f,.04f));registerSolid(registerTimelineTrack,0,0,g.bar.w,1,MyGUI::Colour(.31f,.35f,.35f));registerSolid(registerTimelineTrack,0,g.bar.h-1,g.bar.w,1,MyGUI::Colour(.31f,.35f,.35f));registerProgressCoord(registerCarousel,g.carousel);registerProgressCoord(registerNextPanel,g.panel);
    MyGUI::TextBox* labels[]={registerProgressTitle,registerTimelineLevel,registerTimelineDetail,registerTimelineNext};for(int i=0;i<4;++i)labels[i]->setFontHeight(g.font);
    for(int i=0;i<16;++i){int l=2+(g.bar.w-4)*i/16,r=2+(g.bar.w-4)*(i+1)/16;registerTimelineWidths[i]=std::max(1,r-l);registerTimelineFill[i]->setCoord(l,2,registerTimelineWidths[i],g.bar.h-4);}
    registerProgressCoord(registerCarouselLeft,g.left);registerProgressCoord(registerCarouselRight,g.right);
    for(int side=0;side<2;++side)for(int j=0;j<12;++j){int step=std::max(1,g.left.h/24),dx=abs(j-5);registerArrowInk[side][j]->setCoord(g.left.w/2+(side==0?dx-3:3-dx)*step,g.left.h/2+(j-6)*step,step*2,step+1);}
    int w=g.cards[0].w,h=g.cards[0].h,iconY=4,stateY=iconY+g.icon+2;
    for(int i=0;i<10;++i){registerLevelButtons[i]->setSize(w,h);registerLevelIcons[i]->setCoord((w-g.icon)/2,iconY,g.icon,g.icon);
        registerLevelStates[i]->setCoord(4,stateY,w-8,2*g.font+3);registerLevelStates[i]->setFontHeight(g.font);

        registerLevelAccents[i][0]->setCoord(1,1,w-2,1);registerLevelAccents[i][1]->setCoord(1,1,1,h-2);registerLevelAccents[i][2]->setCoord(w-2,1,1,h-2);registerLevelAccents[i][3]->setCoord(1,h-2,w-2,1);
    }
    int p=g.padding,icon=std::min(g.panel.h-2*g.font-3*p,g.panel.w/4);
    overviewNextTitle->setCoord(p,4,g.panel.w-2*p,g.font+6);overviewNextTitle->setFontHeight(g.font);
    registerNextArt->setCoord(p,g.font+2*p,icon,icon);
    overviewNextLevel->setCoord(icon+2*p,g.font+2*p,g.panel.w-icon-3*p,g.font+5);overviewNextLevel->setFontHeight(g.font);
    registerNextTier->setCoord(icon+2*p,2*g.font+2*p+6,g.panel.w-icon-3*p,g.panel.h-2*g.font-3*p-6);registerNextTier->setFontHeight(g.font);
    registerLayoutLevelCarousel();
}
void registerShowLevel(MyGUI::Widget* sender){if(!registerLevelPopup||!sender)return;
    int level=1;for(int i=0;i<10;++i)if(sender==registerLevelButtons[i])level=i+1;
    int current=currentGuildProgressView().level;std::string state=level<current?Loc::text("v8.literal.113"):level==current?Loc::text("v8.literal.114"):level==current+1?Loc::text("v8.literal.115"):Loc::text("v8.literal.116");
    std::stringstream out;out<<(Loc::text("v8.literal.111"))<<level<<" - "<<state<<"\n\n"<<registerUnlockText(level,true);
    if(registerLevelPopupIcon){registerSetProgressIcon(registerLevelPopupIcon,level);registerLevelPopupIcon->setColour(level<current?registerGreen:level==current?registerAmber:level==current+1?registerIvory:MyGUI::Colour(0.48f,0.47f,0.44f));}
    if(registerLevelPopupText){MercenarieFonts::caption(registerLevelPopupText,out.str());fitRegisterText(registerLevelPopupText,registerProgressGeometry.font);}if(registerLevelPopup){MyGUI::Widget* root=registerLevelPopup->getParent();if(root)registerLevelPopup->setPosition(root->getAbsoluteLeft()+(root->getWidth()-registerLevelPopup->getWidth())/2,root->getAbsoluteTop()+(root->getHeight()-registerLevelPopup->getHeight())/2);registerLevelPopup->setVisible(true);}
}
void registerLevelHover(MyGUI::Widget* sender,MyGUI::Widget*){if(!registerLevelTip||!sender)return;int level=1;for(int i=0;i<10;++i)if(sender==registerLevelButtons[i])level=i+1;if(registerLevelTipText){std::stringstream tip;tip<<Loc::text("v8.literal.084")<<level<<"\n"<<registerUnlockText(level,false);std::vector<GuildLevelUI::Unlock> rows=GuildLevelUI::unlocks(level);for(size_t i=0;i<rows.size();++i)tip<<"\n"<<(gMercenarieEnglish?rows[i].detailEn:rows[i].detailFr);MercenarieFonts::caption(registerLevelTipText,level==3?GuildLevelUI::level3Tooltip():tip.str());}if(registerLevelTipText){fitRegisterText(registerLevelTipText,registerProgressGeometry.font);if(registerLevelTip&&registerLevelTip->getParent()){int pad=registerProgressGeometry.padding,needed=std::min(registerLevelTip->getParent()->getHeight()-16,registerLevelTipText->getTextSize().height+2*pad+8);registerLevelTip->setSize(registerLevelTip->getWidth(),needed);registerLevelTipText->setCoord(pad,pad,registerLevelTip->getWidth()-2*pad,needed-2*pad);}}if(registerLevelTip&&sender){MyGUI::Widget* root=registerLevelTip->getParent();if(!root)return;int x=sender->getAbsoluteLeft()-root->getAbsoluteLeft()+sender->getWidth()/2-registerLevelTip->getWidth()/2,y=sender->getAbsoluteTop()-root->getAbsoluteTop()-registerLevelTip->getHeight()-8;if(root){x=std::max(0,std::min(x,root->getWidth()-registerLevelTip->getWidth()));y=std::max(0,std::min(y,root->getHeight()-registerLevelTip->getHeight()));}registerLevelTip->setPosition(root->getAbsoluteLeft()+x,root->getAbsoluteTop()+y);registerLevelTip->setVisible(true);}}
void registerLevelLeave(MyGUI::Widget*,MyGUI::Widget*){if(registerLevelTip)registerLevelTip->setVisible(false);}
void refreshRegisterInfluenceMap();
int registerMapCropHeight(){return registerInfluenceMap?std::min(2048,registerMapCropSize*registerInfluenceMap->getHeight()/std::max(1,registerInfluenceMap->getWidth())):registerMapCropSize;}
void registerMapWheel(MyGUI::WidgetPtr,int rel){
    if(!registerInfluenceMap||!rel)return;int oldH=registerMapCropHeight();
    int old=registerMapCropSize,step=std::max(64,registerMapCropSize/8);
    registerMapCropSize=std::max(512,std::min(2048,registerMapCropSize+(rel>0?-step:step)));
    registerMapCropX+=(old-registerMapCropSize)/2;registerMapCropY+=(oldH-registerMapCropHeight())/2;
    registerMapCropX=std::max(0,std::min(2048-registerMapCropSize,registerMapCropX));registerMapCropY=std::max(0,std::min(2048-registerMapCropHeight(),registerMapCropY));
    refreshRegisterInfluenceMap();
}
void registerMapPressed(MyGUI::WidgetPtr,int left,int top,MyGUI::MouseButton id){if(id==MyGUI::MouseButton::Left){registerMapDragX=left;registerMapDragY=top;}}
void registerMapDragged(MyGUI::WidgetPtr,int left,int top,MyGUI::MouseButton id){
    if(id!=MyGUI::MouseButton::Left||!registerInfluenceMap)return;MyGUI::IntSize s=registerInfluenceMap->getSize();
    registerMapCropX-=(left-registerMapDragX)*registerMapCropSize/std::max(1,s.width);registerMapCropY-=(top-registerMapDragY)*registerMapCropHeight()/std::max(1,s.height);
    registerMapDragX=left;registerMapDragY=top;registerMapCropX=std::max(0,std::min(2048-registerMapCropSize,registerMapCropX));registerMapCropY=std::max(0,std::min(2048-registerMapCropHeight(),registerMapCropY));refreshRegisterInfluenceMap();
}
#include "GuildInvestmentView.h"
void registerCloseClicked(MyGUI::Widget*){closeGuildContractConfirm();registerCloseLevelPopup(0);if(registerLevelTip)registerLevelTip->setVisible(false);closeGuildInvestment(0);if(guildWindow)guildWindow->setVisible(false);}
void scaleRegisterChildren(MyGUI::Widget* parent,float sx,float sy)
{
    for(size_t i=0;i<parent->getChildCount();++i){MyGUI::Widget* w=parent->getChildAt(i);scaleRegisterChildren(w,sx,sy);MyGUI::IntCoord r=w->getCoord();w->setCoord((int)(r.left*sx),(int)(r.top*sy),std::max(1,(int)(r.width*sx)),std::max(1,(int)(r.height*sy)));MyGUI::TextBox* t=w->castType<MyGUI::TextBox>(false);if(t)t->setFontHeight(std::max(12,(int)(t->getFontHeight()*std::min(sx,sy))));}
}
// MyGUI TextBox has no WordWrap property. Wrap by the real font metrics
// before fitting height, so long sentences do not shrink to microscopic text.
std::string wrapRegisterCaption(MyGUI::TextBox* t,const std::string& raw,int font)
{
    std::stringstream paragraphs(raw);std::string paragraph,result;
    while(std::getline(paragraphs,paragraph)){
        std::stringstream words(paragraph);std::string word,line;
        while(words>>word){std::string candidate=line.empty()?word:line+" "+word;MercenarieFonts::caption(t,candidate);t->setFontHeight(font);
            if(!line.empty()&&t->getTextSize().width>t->getWidth()-2){result+=line+"\n";line=word;}else line=candidate;
        }
        result+=line+"\n";
    }
    if(!result.empty())result.erase(result.size()-1);return result;
}
void fitRegisterText(MyGUI::TextBox* t,int maximum)
{
    if(!t)return;std::string raw=t->getCaption();int minimum=std::min(maximum,std::max(14,maximum*4/5));
    for(int font=maximum;font>=minimum;--font){MercenarieFonts::caption(t,wrapRegisterCaption(t,raw,font));t->setFontHeight(font);if(t->getTextSize().height<=t->getHeight())break;}
}
std::string registerActivity(const std::string& value){
    if(value.compare(0,12,"@investment:")==0)return std::string(Loc::text("guild.investment"))+" - "+value.substr(12);
    return mercenarieLocalize(value);
}
std::string registerBountySummary();
void refreshRegisterInfluenceMap()
{
    if(!registerInfluenceMap)return;MyGUI::ImageBox* map=registerInfluenceMap->castType<MyGUI::ImageBox>();
    // One explicit image tile spans the clamped source rectangle.
    GuildMapViewport::Crop crop=GuildMapViewport::clamp(registerMapCropX,registerMapCropY,registerMapCropSize,map->getWidth(),map->getHeight());
    registerMapCropX=crop.x;registerMapCropY=crop.y;registerMapCropSize=crop.w;
    // ImageBox retains its old tile size when only the crop changes. A smaller
    // crop then contains zero tiles and MyGUI clears its texture. Set both.
    map->setImageInfo("GuildEscortMap.png",MyGUI::IntCoord(crop.x,crop.y,crop.w,crop.h),MyGUI::IntSize(crop.w,crop.h));map->setItemSelect(0);
    for(size_t i=0;i<registerOfficeMarkers.size();++i)registerOfficeMarkers[i]->setVisible(false);size_t marker=0;
    if(!shou||!shou->townList||!ou||!ou->zoneMgr)return;
    int markerSize=guildLayout.markerSize;for(std::map<std::string,std::string>::const_iterator office=guildHouseCities.begin();office!=guildHouseCities.end();++office)if(guildHouseNames.count(office->first))for(int t=0;t<14;++t){std::string city=office->second;if(city!=mapTownNames[t]&&mercenarieLocalize(city)!=mercenarieLocalize(mapTownNames[t]))continue;Town* town=shou->townList->getTownBySID(mapTownIds[t]);if(!town)break;iVector2 sector=ou->zoneMgr->getMapSector(town->getPosition());float tx=(sector.x+0.5f)*32.0f,ty=(sector.y+0.5f)*32.0f;int px=(int)((tx-registerMapCropX)*map->getWidth()/registerMapCropSize),py=(int)((ty-registerMapCropY)*map->getHeight()/registerMapCropHeight());MyGUI::ImageBox* icon=0;if(marker<registerOfficeMarkers.size())icon=registerOfficeMarkers[marker];else{icon=map->createWidget<MyGUI::ImageBox>("ImageBox",0,0,markerSize,markerSize,MyGUI::Align::Default);icon->setImageTexture("MercenarieOverviewIcons.png");icon->setImageCoord(MyGUI::IntCoord(0,0,128,128));icon->setColour(registerAmber);icon->setNeedMouseFocus(false);registerOfficeMarkers.push_back(icon);}icon->setSize(markerSize,markerSize);icon->setPosition(px-markerSize/2,py-markerSize/2);icon->setVisible(px>=0&&py>=0&&px<=map->getWidth()&&py<=map->getHeight());++marker;break;}
}

#include "GuildOverviewView.h"
#include "GuildOverview95.h"
#include "GuildOverviewData.h"

#include "GuildContractsView.h"

#include "GuildOfficesReputationView.h"
#include "FinanceView.h"
