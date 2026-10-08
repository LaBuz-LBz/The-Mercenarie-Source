// New overview presentation, built directly in client pixels after legacy tabs scale.
GuildOverview::Layout overviewLayout;
MyGUI::TextBox *overviewStats[4]={0},*overviewMoney[5]={0},*overviewInfo[5]={0},*overviewRecent[3]={0},*overviewEmpty=0;
MyGUI::TextBox *overviewRecentResults[3]={0},*overviewRecentBonus[3]={0};
MyGUI::ImageBox *overviewRecentIcons[3]={0};
MyGUI::Widget *overviewHeaderTrack=0,*overviewHeaderFill=0,*overviewHeroTrack=0,*overviewHeroFill=0;
MyGUI::Widget *overviewChart=0,*overviewChartLines[3][192]={{0}},*overviewChartPoints[3][7]={{0}};
MyGUI::TextBox *overviewChartEmpty=0;
MyGUI::Widget* overviewSidebar=0;
int overviewFont=16,overviewChartPlotW=1,overviewRecentCapacity=3;
MyGUI::TextBox* overviewHeaderLevel=0;
MyGUI::Widget* overviewPanel(MyGUI::Widget* p,const GuildResponsive::Rect& r){return p->createWidget<MyGUI::Widget>("MercenarieOverviewPanel",r.x,r.y,r.w,r.h,MyGUI::Align::Default);}
MyGUI::Widget* overviewEmptyPanel(MyGUI::Widget* p,const GuildResponsive::Rect& r){return p->createWidget<MyGUI::Widget>("PanelEmpty",r.x,r.y,r.w,r.h,MyGUI::Align::Default);}
MyGUI::ImageBox* overviewIcon(MyGUI::Widget* p,int id,int x,int y,int size,const MyGUI::Colour& colour){MyGUI::ImageBox* icon=registerTextureIcon(p,"MercenarieOverviewIcons.png",x,y,size,colour);icon->setImageCoord(MyGUI::IntCoord((id%4)*128,(id/4)*128,128,128));return icon;}
MyGUI::TextBox* overviewText(MyGUI::Widget* p,int x,int y,int w,int h,int font,const std::string& text,const MyGUI::Colour& colour=registerIvory){MyGUI::TextBox* t=registerText(p,x,y,w,h,font,text,colour);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);if(w>1&&h>1)fitRegisterText(t,font);return t;}
MyGUI::Widget* overviewSection(MyGUI::Widget* p,const GuildResponsive::Rect& r,const std::string& title){MyGUI::Widget* panel=overviewPanel(p,r);int pad=overviewLayout.pad;overviewText(panel,pad,4,r.w-2*pad,overviewLayout.title+6,overviewLayout.title,title);registerLine(panel,5,overviewLayout.title+8,r.w-10);return panel;}
MyGUI::Button* overviewButton(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& text){MyGUI::Button* b=p->createWidget<MyGUI::Button>("MercenarieOverviewButton",x,y,w,h,MyGUI::Align::Default);if(!text.empty()){MyGUI::TextBox* t=overviewText(b,6,2,w-12,h-4,overviewFont,text,registerAmber);t->setTextAlign(MyGUI::Align::Center);}return b;}
void overviewBar(MyGUI::Widget* p,int x,int y,int w,int h,MyGUI::Widget*& track,MyGUI::Widget*& fill){track=registerSolid(p,x,y,w,h,MyGUI::Colour(.31f,.35f,.35f));registerSolid(track,1,1,w-2,h-2,MyGUI::Colour(.02f,.035f,.04f));fill=registerSolid(track,2,2,w-4,h-4,registerAmber);}
void overviewFill(MyGUI::Widget* track,MyGUI::Widget* fill,float fraction){int pixels=GuildOverview::fillPixels(track->getWidth()-4,fraction);fill->setVisible(pixels>0);fill->setSize(std::max(1,pixels),std::max(1,track->getHeight()-4));}
void overviewTimelineFill(float fraction){int pixels=GuildOverview::fillPixels(registerTimelineTrack->getWidth()-4,fraction);for(int i=0;i<16;++i){int width=std::max(0,std::min(registerTimelineWidths[i],pixels-(registerTimelineFill[i]->getLeft()-2)));registerTimelineFill[i]->setVisible(width>0);registerTimelineFill[i]->setSize(std::max(1,width),registerTimelineFill[i]->getHeight());}}
void overviewCaption(MyGUI::TextBox* t,const std::string& value){if(t){MercenarieFonts::caption(t,value);fitRegisterText(t,overviewFont);}}
void overviewRecentCaption(MyGUI::TextBox* t,const std::string& value){overviewCaption(t,value);int font=t->getFontHeight(),capacity=std::max(1,t->getHeight()/(font+2));std::stringstream lines(std::string(t->getCaption()));std::string line,out;for(int n=0;n<capacity&&std::getline(lines,line);++n){if(n==capacity-1&&lines.peek()!=EOF){MercenarieFonts::caption(t,line+"...");while(!line.empty()&&t->getTextSize().width>t->getWidth()-2){size_t end=line.size()-1;while(end>0&&((unsigned char)line[end]&0xc0)==0x80)--end;line.erase(end);MercenarieFonts::caption(t,line+"...");}line+="...";}if(!out.empty())out+="\n";out+=line;}MercenarieFonts::caption(t,out);}
void refreshRegisterNavigation(){
 if(!registerPayrollButton)return;
 const bool expanded=registerSelectedTab==4,selected=expanded&&payrollManagementIsOpen();
 registerPayrollButton->setVisible(expanded);registerPayrollButton->setStateSelected(selected);
 if(registerPayrollLabel)registerPayrollLabel->setTextColour(selected?registerAmber:registerIvory);
 if(registerEstateButton){bool estateSelected=expanded&&estateViewIsOpen()&&!selected;registerEstateButton->setVisible(expanded&&estateEnabled());registerEstateButton->setStateSelected(estateSelected);if(registerEstateLabel)registerEstateLabel->setTextColour(estateSelected?registerAmber:registerIvory);}
 const GuildOverview::Layout& l=overviewLayout;int bh=(l.sidebar.h-5*l.gap)/6;
 if(guildTabButtons[5])guildTabButtons[5]->setPosition(0,5*(bh+l.gap)+(expanded?2*(registerPayrollButton->getHeight()+l.gap):0));
 for(int i=0;i<2;++i)if(registerBrands[i])registerBrands[i]->setVisible(i==(expanded?1:0));
}
void buildOverviewBrand(int variant,int shift){
 const GuildOverview::Layout& l=overviewLayout;int p=l.pad,g=l.gap,h=l.brand.h-shift;
 MyGUI::Widget* brand=overviewPanel(overviewSidebar,GuildResponsive::Rect(0,l.brand.y-l.sidebar.y+shift,l.brand.w,h));registerBrands[variant]=brand;
 // Keep the mark and name readable when the child navigation takes more room.
 int textSpace=p*3+l.title+g+6*(l.font+3)+8;
 bool full=h>=l.px(40)+textSpace;
 int logo=full?std::min(std::min(l.px(135),h*39/100),h-textSpace):std::max(1,std::min(l.px(90),h-l.title-p*3-5));
 overviewIcon(brand,0,(l.brand.w-logo)/2,p,logo,registerAmber);
 MyGUI::TextBox* name=overviewText(brand,4,logo+p*2,l.brand.w-8,l.title+5,l.title,"THE MERCENARIE");name->setTextAlign(MyGUI::Align::Center);
 if(full){
 int ty=logo+p*2+l.title+g;const char* brandFr[]={"DISCIPLINE","SERVICES","REPUTATION","SURVIE"};const char* brandEn[]={"DISCIPLINE","SERVICES","REPUTATION","SURVIVAL"};
 for(int i=0;i<4;++i){registerSolid(brand,p*2,ty+i*(l.font+3)+l.font/2,l.px(12),3,registerAmber);overviewText(brand,p*2+l.px(20),ty+i*(l.font+3),l.brand.w-p*3-l.px(20),l.font+3,l.font,registerLanguage(brandFr[i],brandEn[i]));}
 overviewText(brand,p,h-2*l.font-8,l.brand.w-2*p,2*l.font+5,l.font,Loc::text("v8.literal.098"),registerAmber);
 }
}
void buildOverviewSidebar(MyGUI::Widget* c){
 const GuildOverview::Layout& l=overviewLayout;int p=l.pad,g=l.gap;
 overviewSidebar=overviewEmptyPanel(c,GuildResponsive::Rect(l.sidebar.x,l.sidebar.y,l.sidebar.w,l.height-l.sidebar.y-p));
 const char* fr[]={"VUE D'ENSEMBLE","CONTRATS","BUREAUX","REPUTATION","FINANCES","OPTIONS"};
 const char* en[]={"OVERVIEW","CONTRACTS","OFFICES","REPUTATION","FINANCES","OPTIONS"};const int ids[]={1,2,3,4,5,6};
 int bh=(l.sidebar.h-5*g)/6,icon=std::min(l.px(40),bh-12);
 for(int i=0;i<6;++i){MyGUI::Button* b=overviewSidebar->createWidget<MyGUI::Button>("MercenarieGuildNav",0,i*(bh+g),l.sidebar.w,bh,MyGUI::Align::Default);guildTabButtons[i]=b;b->eventMouseButtonClick+=MyGUI::newDelegate(guildTabClicked);b->eventMouseSetFocus+=MyGUI::newDelegate(registerTabFocus);b->eventMouseLostFocus+=MyGUI::newDelegate(registerTabBlur);
 overviewTabIcons[i]=overviewIcon(b,ids[i],p,(bh-icon)/2,icon,registerIvory);overviewTabLabels[i]=overviewText(b,p+icon+g,0,l.sidebar.w-icon-2*p-g,bh,l.font,registerLanguage(fr[i],en[i]));overviewTabLabels[i]->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
 registerTabMarks[i]=0;for(int e=0;e<3;++e)registerTabBorders[i][e]=0;}
 int childH=std::max(26,bh*3/4),indent=std::max(18,l.px(26)),childIcon=std::min(childH-8,l.px(24));
 registerPayrollButton=overviewSidebar->createWidget<MyGUI::Button>("MercenarieGuildNav",indent,5*(bh+g),l.sidebar.w-indent,childH,MyGUI::Align::Default);
 registerPayrollButton->eventMouseButtonClick+=MyGUI::newDelegate(openPayrollFinances);
 MyGUI::ImageBox* payrollIcon=registerTextureIcon(registerPayrollButton,"MercenariePayrollIcons.png",p,(childH-childIcon)/2,childIcon,registerAmber);payrollIcon->setImageCoord(MyGUI::IntCoord(6*64,0,64,64));
 registerPayrollLabel=overviewText(registerPayrollButton,p+childIcon+g,0,l.sidebar.w-indent-childIcon-2*p-g,childH,std::max(11,l.font-1),Loc::text("payroll.rates"));
 registerEstateButton=overviewSidebar->createWidget<MyGUI::Button>("MercenarieGuildNav",indent,5*(bh+g)+childH+g,l.sidebar.w-indent,childH,MyGUI::Align::Default);
 registerEstateButton->eventMouseButtonClick+=MyGUI::newDelegate(estateShowView);
 overviewIcon(registerEstateButton,3,p,(childH-childIcon)/2,childIcon,registerAmber);
 registerEstateLabel=overviewText(registerEstateButton,p+childIcon+g,0,l.sidebar.w-indent-childIcon-2*p-g,childH,std::max(11,l.font-1),Loc::text("estate.title"));
 buildOverviewBrand(0,0);buildOverviewBrand(1,2*(childH+g));
 registerSelectTab(0);
}
void buildRegisterOverview(MyGUI::Widget* c,int,int,int,int,int,int,int){
 overviewLayout=GuildOverview::calculate(c->getWidth(),c->getHeight());const GuildOverview::Layout& l=overviewLayout;overviewFont=l.font;int p=l.pad,g=l.gap,f=l.font,t=l.title;
 MyGUI::Widget* root=overviewEmptyPanel(c,GuildResponsive::Rect(0,0,l.width,l.height));overviewChrome=root;guildOverviewPanels[0]=root;for(int i=1;i<8;++i)guildOverviewPanels[i]=0;
 // This late-created sibling covers Options. Only its visible children may
 // receive input; the empty central area must pass through to the page below.
 root->setNeedMouseFocus(false);root->setInheritsPick(true);
 MyGUI::Widget* header=overviewPanel(root,l.header);header->setNeedMouseFocus(false);int sun=l.header.h-2*p;overviewIcon(header,0,p,p,sun,registerAmber);
 // MyGUI 3.2 resets the height to the font's native size in setFontName.
 // Apply the desired banner height AFTER the font, within the header's title lane.
 int titleH=l.header.h-2*p-f-8;
 guildHeroTitleText=overviewText(header,sun+3*p,p,l.header.w-sun-4*p,titleH,l.px(48),"THE MERCENARIE");guildHeroTitleText->setFontName("Kenshi_BannerTextFont");guildHeroTitleText->setFontHeight(l.px(48));guildStatsText=guildHeroTitleText;
 guildHeroDescriptionText=overviewText(header,sun+3*p,l.header.h-p-f-5,l.header.w-sun-4*p,f+5,f,Loc::text("v8.literal.099"));
 MyGUI::Widget* summary=overviewSection(root,l.summary,Loc::text("ui.guild_management"));summary->setNeedMouseFocus(false);int rowY=t+14,cw=(l.summary.w-2*p)/4;
 const int summaryIcons[]={3,1,4,5};const char* sf[]={"NIVEAU","BUREAUX","REPUTATION","FONDS"};const char* se[]={"LEVEL","OFFICES","REPUTATION","FUNDS"};
 for(int i=0;i<4;++i){int x=p+i*cw;if(i)registerSolid(summary,x-4,rowY,1,l.summary.h-rowY-p,MyGUI::Colour(.30f,.25f,.18f));overviewIcon(summary,summaryIcons[i],x,rowY,l.px(24),i==0||i==3?registerAmber:registerIvory);MyGUI::TextBox* label=overviewText(summary,x+l.px(28),rowY,cw-l.px(29),f+4,f,registerLanguage(sf[i],se[i]));if(i==0)overviewHeaderLevel=label;registerQuick[i]=overviewText(summary,x,rowY+f+2,cw-5,l.summary.h-rowY-f-4,f,"",i==2?registerAmber:registerIvory);registerQuick[i]->setTextAlign(MyGUI::Align::Center);}
 overviewBar(summary,p,l.summary.h-12,cw-6,8,overviewHeaderTrack,overviewHeaderFill);registerQuick[0]->setCoord(p,rowY+f+5,cw-5,l.summary.h-rowY-f-13);
 root=overviewEmptyPanel(root,GuildResponsive::Rect(0,0,l.width,l.height));guildOverviewPanels[0]=root;
 MyGUI::Widget* glance=overviewSection(root,l.glance,Loc::text("v8.literal.100"));int left=l.glance.w*27/100,tx=left+p,tw=l.glance.w-tx-p*2,y=t+p*2;
 overviewIcon(glance,0,p*2,y+l.px(20),left-p*3,registerAmber);
 guildHeroLevelText=overviewText(glance,tx,y,tw,t+5,t,"");y+=t+8;
 overviewBar(glance,tx,y,tw,l.px(20),overviewHeroTrack,overviewHeroFill);y+=l.px(24);
 guildHeroXpText=overviewText(glance,tx,y,tw,f+5,f,"");guildHeroXpText->setTextAlign(MyGUI::Align::Center);y+=f+g;
 const char* gf[]={"Membres :","Bureaux :","Contrats termines :","Fonds :"};const char* ge[]={"Members:","Offices:","Completed contracts:",Loc::text("overview.funds")};const int gi[]={9,1,2,5};
 int buttonH=std::max(30,l.px(43)),buttonY=l.glance.h-p-buttonH,rowH=(buttonY-g-y)/4;
 for(int i=0;i<4;++i){overviewIcon(glance,gi[i],tx,y+i*rowH+(rowH-std::min(f+4,rowH-2))/2,std::min(f+4,rowH-2),registerIvory);overviewText(glance,tx+f+g,y+i*rowH,tw*62/100-f-g,rowH,f,registerLanguage(gf[i],ge[i]));overviewStats[i]=overviewText(glance,tx+tw*63/100,y+i*rowH,tw*37/100,rowH,f,"");}
 MyGUI::Button* improve=overviewButton(glance,tx,buttonY,tw,buttonH,Loc::text("v8.literal.101"));improve->eventMouseButtonClick+=MyGUI::newDelegate(registerOpenInvestment);
 MyGUI::Widget* activity=overviewSection(root,l.activity,Loc::text("v8.literal.102"));int cy=t+p+4,ch=l.activity.h-cy-p,cardW=(l.activity.w-2*p-2*g)/3;const int ai[]={2,7,8};
 const char* af[]={"Contrats actifs",Loc::text("overview.expeditions"),"Contrats termines"};const char* ae[]={"Active contracts",Loc::text("overview.expeditions"),"Completed contracts"};
 for(int i=0;i<3;++i){MyGUI::Widget* card=overviewPanel(activity,GuildResponsive::Rect(p+i*(cardW+g),cy,cardW,ch));int icon=std::min(l.px(34),ch-2*f-10);overviewIcon(card,ai[i],(cardW-icon)/2,5,icon,registerIvory);registerValues[i]=overviewText(card,5,icon+7,cardW-10,f+6,f+4,"");registerValues[i]->setTextAlign(MyGUI::Align::Center);registerActivityLabels[i]=overviewText(card,3,ch-f-6,cardW-6,f+4,f,registerLanguage(af[i],ae[i]));registerActivityLabels[i]->setTextAlign(MyGUI::Align::Center);}registerValues[3]=0;
 MyGUI::Widget* finance=overviewSection(root,l.finance,Loc::text("v8.literal.103"));int fy=t+p+2,rh=(l.finance.h-fy-p-f-5)/4,fw=l.finance.w*36/100;
 const char* ff[]={"Revenus","Depenses","Salaires","Solde net","Fonds actuels :"};const char* fe[]={"Income","Expenses","Wages",Loc::text("overview.net_balance"),"Current funds:"};
 for(int i=0;i<5;++i){int yy=i<4?fy+i*rh:l.finance.h-p-f-2;overviewText(finance,p,yy,fw*58/100,rh,f,registerLanguage(ff[i],fe[i]));overviewMoney[i]=overviewText(finance,p+fw*58/100,yy,i<4?fw*42/100:l.finance.w-fw*58/100-p*2,rh,f,"",i==1?MyGUI::Colour(1,.22f,.16f):i==2?registerIvory:registerGreen);}
 int chartX=fw+p*2,chartW=l.finance.w-chartX-p,chartH=l.finance.h-fy-p-f-8;
 overviewChart=overviewEmptyPanel(finance,GuildResponsive::Rect(chartX,fy,chartW,chartH));int plotW=chartW-std::max(85,l.px(120));overviewChartPlotW=plotW;
 for(int i=0;i<=6;++i)registerSolid(overviewChart,i*(plotW-2)/6,0,1,chartH,MyGUI::Colour(.17f,.21f,.21f));for(int i=0;i<4;++i)registerSolid(overviewChart,0,i*(chartH-2)/3,plotW,1,MyGUI::Colour(.17f,.21f,.21f));
 const MyGUI::Colour colors[]={registerGreen,MyGUI::Colour(1,.22f,.16f),registerAmber};const char* lf[]={"Revenus","Depenses","Solde net"};const char* le[]={"Income","Expenses",Loc::text("overview.net")};
 for(int series=0;series<3;++series){for(int j=0;j<192;++j)overviewChartLines[series][j]=registerSolid(overviewChart,0,0,2,2,colors[series]);for(int j=0;j<7;++j)overviewChartPoints[series][j]=registerSolid(overviewChart,0,0,4,4,colors[series]);registerSolid(overviewChart,plotW+g,series*(f+8)+5,l.px(15),3,colors[series]);overviewText(overviewChart,plotW+g+l.px(19),series*(f+8),chartW-plotW-g-l.px(19),f+6,std::max(12,f-2),registerLanguage(lf[series],le[series]));}
 overviewChartEmpty=overviewText(overviewChart,4,chartH/2-f,plotW-8,f*2,f,Loc::text("v8.literal.104"));overviewChartEmpty->setTextAlign(MyGUI::Align::Center);
 MyGUI::Widget* mapPanel=overviewSection(root,l.map,Loc::text("v8.literal.105"));int my=t+10,mh=l.map.h-my-f-9,mapW=l.map.w*55/100-p;
 registerInfluenceMap=mapPanel->createWidget<MyGUI::ImageBox>("ImageBox",p,my,mapW,mh,MyGUI::Align::Default);registerInfluenceMap->castType<MyGUI::ImageBox>()->setImageTexture("GuildEscortMap.png");registerInfluenceMap->eventMouseWheel+=MyGUI::newDelegate(registerMapWheel);registerInfluenceMap->eventMouseButtonPressed+=MyGUI::newDelegate(registerMapPressed);registerInfluenceMap->eventMouseDrag+=MyGUI::newDelegate(registerMapDragged);
 const char* mf[]={Loc::text("v8.map.0"),Loc::text("v8.map.1"),Loc::text("v8.map.2"),Loc::text("v8.map.3"),Loc::text("v8.map.4"),Loc::text("v8.map.5")};const char** me=mf;const int mi[]={0,9,1,1,3,1};int legendX=p+mapW+g,legendH=mh/6;
 for(int i=0;i<6;++i){overviewIcon(mapPanel,mi[i],legendX,my+i*legendH+(legendH-std::min(f+2,legendH-2))/2,std::min(f+2,legendH-2),i==0?registerAmber:registerIvory);overviewText(mapPanel,legendX+f+g,my+i*legendH,l.map.w-legendX-f-g-p,legendH,std::max(12,f-2),registerLanguage(mf[i],me[i]));}
 registerInfluence=overviewText(mapPanel,p,l.map.h-f-5,l.map.w-2*p,f+3,std::max(12,f-2),Loc::text("v8.literal.106"));
 MyGUI::Widget* recent=overviewSection(root,l.recent,Loc::text("v8.literal.107"));int recentY=t+10,recentButtonH=std::max(28,l.px(41)),recentSpace=l.recent.h-recentY-recentButtonH-p-g;overviewRecentCapacity=std::max(1,std::min(3,recentSpace/(3*f+8)));int recentH=recentSpace/overviewRecentCapacity;
 for(int i=0;i<3;++i){overviewRecentIcons[i]=overviewIcon(recent,2,p,recentY+i*recentH+4,std::min(l.px(32),recentH-8),registerIvory);overviewRecent[i]=overviewText(recent,p+l.px(40),recentY+i*recentH,l.recent.w-2*p-l.px(40),recentH-f-6,f,"");overviewRecentResults[i]=overviewText(recent,p+l.px(40),recentY+i*recentH+recentH-f-5,l.recent.w/3-l.px(30),f+3,std::max(12,f-2),"");overviewRecentBonus[i]=overviewText(recent,l.recent.w/3+p,recentY+i*recentH+recentH-f-5,l.recent.w*2/3-2*p,f+3,std::max(12,f-2),"");if(i<overviewRecentCapacity&&i)registerLine(recent,p,recentY+i*recentH,l.recent.w-2*p);if(i>=overviewRecentCapacity){overviewRecent[i]->setVisible(false);overviewRecentIcons[i]->setVisible(false);overviewRecentResults[i]->setVisible(false);overviewRecentBonus[i]->setVisible(false);}}
 overviewEmpty=overviewText(recent,p,recentY,l.recent.w-2*p,recentSpace,f,Loc::text("v8.literal.108"));overviewEmpty->setTextAlign(MyGUI::Align::Center);
 MyGUI::Button* all=overviewButton(recent,p,l.recent.h-p-recentButtonH,l.recent.w-2*p,recentButtonH,Loc::text("v8.literal.109"));all->eventMouseButtonClick+=MyGUI::newDelegate(registerOpenContracts);
 MyGUI::Widget* info=overviewSection(root,l.information,Loc::text("guild.pages.information"));const char* inf[]={"Territoires connus :","Factions en contact :","Prochaine amelioration :","Conditions :","Bureaux enregistres :"};const char* ine[]={"Known territories:","Faction contacts:","Next improvement:","Requirements:","Registered offices:"};const int ii[]={14,9,1,3,2};int iy=t+p,ih=(l.information.h-iy-p)/5;
 for(int i=0;i<5;++i){overviewIcon(info,ii[i],p,iy+i*ih+(ih-f-3)/2,f+3,registerIvory);overviewText(info,p+f+g,iy+i*ih,l.information.w*61/100-p-f-g,ih,f,registerLanguage(inf[i],ine[i]));overviewInfo[i]=overviewText(info,l.information.w*62/100,iy+i*ih,l.information.w*38/100-p,ih,f,"",i>1&&i<4?registerAmber:registerIvory);if(i)registerLine(info,p,iy+i*ih-2,l.information.w-2*p);}
 registerProgression=overviewPanel(root,l.progression);registerProgressTitle=overviewText(registerProgression,0,0,1,1,t,Loc::text("v8.literal.110"));registerTimelineLevel=overviewText(registerProgression,0,0,1,1,f,"");registerTimelineDetail=overviewText(registerProgression,0,0,1,1,f,"");registerTimelineNext=overviewText(registerProgression,0,0,1,1,f,"");registerTimelineTrack=registerSolid(registerProgression,0,0,1,1,MyGUI::Colour(.25f,.30f,.30f));for(int i=0;i<16;++i)registerTimelineFill[i]=registerSolid(registerTimelineTrack,0,0,1,1,registerAmber);
 registerCarousel=overviewEmptyPanel(registerProgression,GuildResponsive::Rect());registerCarouselLeft=overviewButton(registerCarousel,0,0,1,1,"");registerCarouselRight=overviewButton(registerCarousel,0,0,1,1,"");MyGUI::Button* arrows[]={registerCarouselLeft,registerCarouselRight};for(int i=0;i<2;++i){arrows[i]->eventMouseButtonClick+=MyGUI::newDelegate(registerMoveLevelCarousel);arrows[i]->eventMouseWheel+=MyGUI::newDelegate(registerLevelCarouselWheel);for(int j=0;j<12;++j)registerArrowInk[i][j]=registerSolid(arrows[i],0,0,1,1,registerAmber);}
 for(int i=0;i<10;++i){registerLevelButtons[i]=overviewButton(registerCarousel,0,0,1,1,"");for(int e=0;e<4;++e)registerLevelAccents[i][e]=registerSolid(registerLevelButtons[i],0,0,1,1,registerAmber);registerLevelIcons[i]=overviewIcon(registerLevelButtons[i],0,0,0,1,registerIvory);registerSetProgressIcon(registerLevelIcons[i],i+1);registerLevelStates[i]=overviewText(registerLevelButtons[i],0,0,1,1,f,"");registerLevelStates[i]->setTextAlign(MyGUI::Align::Center);registerLevelTitles[i]=0;registerLevelButtons[i]->eventMouseButtonClick+=MyGUI::newDelegate(registerShowLevel);registerLevelButtons[i]->eventMouseSetFocus+=MyGUI::newDelegate(registerLevelHover);registerLevelButtons[i]->eventMouseLostFocus+=MyGUI::newDelegate(registerLevelLeave);registerLevelButtons[i]->eventMouseWheel+=MyGUI::newDelegate(registerLevelCarouselWheel);}
 registerNextPanel=overviewPanel(registerProgression,GuildResponsive::Rect());overviewNextTitle=overviewText(registerNextPanel,0,0,1,1,f,"");overviewNextLevel=overviewText(registerNextPanel,0,0,1,1,f,"",registerAmber);registerNextArt=overviewIcon(registerNextPanel,1,0,0,1,registerAmber);registerNextTier=overviewText(registerNextPanel,0,0,1,1,f,"");registerReflowProgression(l.scale);
 registerLevelPopup=overviewPanel(root,GuildResponsive::Rect(l.width/4,l.height/4,l.width/2,l.height/2));registerLevelPopup->setWidgetStyle(MyGUI::WidgetStyle::Popup,"Popup");registerLevelPopup->setAlpha(1);registerLevelPopup->setInheritsAlpha(false);registerLevelPopupIcon=overviewIcon(registerLevelPopup,0,p,p,l.px(64),registerAmber);registerLevelPopupText=overviewText(registerLevelPopup,p*2+l.px(64),p,registerLevelPopup->getWidth()-p*3-l.px(64),registerLevelPopup->getHeight()-p*3-40,f,"");MyGUI::Button* close=overviewButton(registerLevelPopup,registerLevelPopup->getWidth()/3,registerLevelPopup->getHeight()-p-34,registerLevelPopup->getWidth()/3,34,Loc::text("delegated.report.close"));close->eventMouseButtonClick+=MyGUI::newDelegate(registerCloseLevelPopup);registerLevelPopup->setVisible(false);
 registerLevelTip=root->createWidget<MyGUI::Widget>("MercenarieGuildTooltip",0,0,std::min(l.width/2,l.px(520)),l.px(235),MyGUI::Align::Default);registerLevelTip->setWidgetStyle(MyGUI::WidgetStyle::Popup,"Popup");registerLevelTip->setAlpha(1.0f);registerLevelTip->setInheritsAlpha(false);registerLevelTip->setNeedMouseFocus(false);registerLevelTipText=overviewText(registerLevelTip,p,p,registerLevelTip->getWidth()-2*p,registerLevelTip->getHeight()-2*p,f,"");registerLevelTip->setVisible(false);
 guildZonesText=guildSuccessText=guildCatsText=guildBenefitsText=0;guildLevelNumberText=guildHeroUnlockText=registerXpDetail=0;guildDetailsText=guildContractText=guildHistoryText=guildActivityText=guildHouseText=0;
 buildOverviewSidebar(c);registerCarouselInitialized=false;
}
