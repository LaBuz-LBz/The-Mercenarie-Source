// Guild management content only. Header/sidebar remain owned by the overview.
std::string gcText(const char* key){return Loc::text((std::string("contracts.ui.")+key).c_str());}
std::vector<GuildContracts::Row> gcRows;
std::vector<int> gcVisible;
std::string gcSelected,gcPending,gcFingerprint;
int gcFilter=-1,gcPage=0,gcCapacity=1,gcFont=16,gcTableWidth=1;
int gcSortMode=0;bool gcUpdating=false;
unsigned long gcLastRefresh=0;
MyGUI::Button *gcFilters[4]={0},*gcMap=0,*gcCancel=0,*gcNav[5]={0};
MyGUI::TextBox *gcFilterLabels[4]={0},*gcNavLabels[5]={0};
MyGUI::TextBox *gcCounts=0,*gcEmpty=0,*gcDetailTitle=0,*gcDetailStatus=0,*gcDetailText=0,*gcCancelNote=0;
MyGUI::ImageBox* gcDetailIcon=0;
MyGUI::TextBox* gcSearchHint=0;MyGUI::EditBox* gcSearch=0;
MyGUI::ComboBox* gcSort=0;
MyGUI::ScrollView *gcTable=0,*gcDetails=0;
MyGUI::Widget* gcConfirm=0;
struct GuildContractCard {MyGUI::Button* button;MyGUI::Widget* badge;MyGUI::TextBox* cells[8];MyGUI::ImageBox *donor,*type;};
std::vector<GuildContractCard> gcCards;
MyGUI::TextBox *gc99From=0,*gc99To=0,*gc99Reward=0,*gc99RewardLabel=0,*gc99Objective=0,*gc99Step=0;
// Invalidate non-owning references before MyGUI destroys the owning window.
// Destruction can emit focus callbacks; none may reach the old confirmation/tip.
void resetGuildMenuViewReferences(){
 overview95Root=0;o96LegacyClose=0;o97Tip=0;o97TipText=0;o95Journal=0;o99Body=0;o99Heading=0;for(int n=0;n<6;++n)o99NavMark[n]=0;
 registerPayrollButton=0;registerPayrollLabel=0;registerEstateButton=0;registerEstateLabel=0;registerBrands[0]=registerBrands[1]=0;
 gcRows.clear();gcVisible.clear();gcCards.clear();gcSelected.clear();gcPending.clear();gcFingerprint.clear();
 for(int i=0;i<4;++i){gcFilters[i]=0;gcFilterLabels[i]=0;}
 for(int i=0;i<5;++i){gcNav[i]=0;gcNavLabels[i]=0;}
 gc99From=gc99To=0;gc99Reward=gc99RewardLabel=gc99Objective=gc99Step=0;gcMap=gcCancel=0;gcCounts=gcEmpty=gcDetailTitle=gcDetailStatus=gcDetailText=gcCancelNote=0;
 gcSearchHint=0;gcDetailIcon=0;gcSearch=0;gcSort=0;gcTable=gcDetails=0;gcConfirm=0;guildRelationsPanel=0;
 gcPage=0;gcFilter=-1;gcSortMode=0;gcUpdating=false;gcLastRefresh=0;
 for(int i=0;i<4;++i){registerValues[i]=0;registerQuick[i]=0;}
 for(int i=0;i<10;++i){registerLevelButtons[i]=0;registerLevelIcons[i]=0;registerLevelStates[i]=0;registerLevelTitles[i]=0;for(int j=0;j<4;++j)registerLevelAccents[i][j]=0;}
 registerCarouselLeft=registerCarouselRight=0;registerProgression=registerCarousel=registerNextPanel=registerTimelineTrack=0;
 registerProgressTitle=registerTimelineNext=registerTimelineLevel=registerTimelineDetail=0;registerNextArt=0;overviewNextTitle=overviewNextLevel=0;
 for(int side=0;side<2;++side)for(int j=0;j<12;++j)registerArrowInk[side][j]=0;
 registerInfluenceMap=0;registerOfficeMarkers.clear();overviewChrome=0;registerLevelTip=registerLevelPopup=0;registerLevelTipText=registerLevelPopupText=0;registerLevelPopupIcon=0;
 for(int i=0;i<6;++i){guildTabButtons[i]=0;overviewTabLabels[i]=0;overviewTabIcons[i]=0;}
}
MyGUI::Colour gcStatusColour(int status){return status==GuildContracts::Active?registerAmber:status==GuildContracts::Completed?registerGreen:status==GuildContracts::Failed?MyGUI::Colour(1,.25f,.18f):registerIvory;}
MyGUI::Colour gcOutcomeColour(const GuildContracts::Row& r){return r.status==GuildContracts::Completed&&(r.bonus==0||(r.bonus<0&&r.rewardValue==0))?MyGUI::Colour(.85f,.25f,.16f):gcStatusColour(r.status);}
std::string gcStatus(int status){return gcText(status==GuildContracts::Active?"active":status==GuildContracts::Completed?"completed_single":status==GuildContracts::Failed?"failed_single":"archived");}
void gcSetIcon(MyGUI::ImageBox* icon,int id){if(id>=5&&id<=9){int slots[]={0,1,2,4,3};icon->setImageTexture("ContractIconsV6.png");icon->setImageCoord(MyGUI::IntCoord(slots[id-5]*96,0,96,96));return;}icon->setImageTexture("MercenarieContractIcons.png");icon->setImageCoord(MyGUI::IntCoord((id%4)*96,(id/4)*96,96,96));}
MyGUI::ImageBox* gcIcon(MyGUI::Widget* p,int x,int y,int size){MyGUI::ImageBox* icon=registerTextureIcon(p,"MercenarieContractIcons.png",x,y,size,registerIvory);gcSetIcon(icon,0);return icon;}
void closeGuildContractConfirm(){gcPending.clear();if(gcConfirm)gcConfirm->setVisible(false);}
void gcBack(MyGUI::Widget*){closeGuildContractConfirm();}
void gcConfirmCancel(MyGUI::Widget*){if(!gcConfirm||gcPending.empty())return;std::string key=gcPending;closeGuildContractConfirm();std::vector<GuildContracts::Row> live=collectGuildContracts();int i=GuildContracts::find(live,key);if(i>=0&&live[i].status==GuildContracts::Active&&live[i].canCancel)guildContractCancelAction(live[i]);refreshGuildContracts(true);}
void gcRequestCancel(MyGUI::Widget*){if(!gcConfirm)return;int i=GuildContracts::find(gcRows,gcSelected);if(i<0||!gcRows[i].canCancel||gcRows[i].status!=GuildContracts::Active)return;gcPending=gcRows[i].key;MyGUI::Widget* parent=gcConfirm->getParent();gcConfirm->setPosition(parent->getAbsoluteLeft()+(parent->getWidth()-gcConfirm->getWidth())/2,parent->getAbsoluteTop()+(parent->getHeight()-gcConfirm->getHeight())/2);gcConfirm->setVisible(true);}
void gcOpenMap(MyGUI::Widget*){std::vector<GuildContracts::Row> live=collectGuildContracts();int i=GuildContracts::find(live,gcSelected);if(i>=0&&live[i].canMap)guildContractMapAction(live[i]);}
void gcRender();
void gcSelect(MyGUI::Widget* sender){if(!gcTable||!sender)return;closeGuildContractConfirm();for(size_t i=0;i<gcCards.size();++i)if(gcCards[i].button==sender){size_t index=gcPage*gcCapacity+i;if(index<gcVisible.size()){gcSelected=gcRows[gcVisible[index]].key;gcRender();}return;}}
void gcFilterClick(MyGUI::Widget* sender){closeGuildContractConfirm();for(int i=0;i<4;++i)if(sender==gcFilters[i])gcFilter=i-1;gcPage=0;gcSelected.clear();gcRender();}
void gcSearchChanged(MyGUI::EditBox*){if(gcUpdating||!gcSearch)return;closeGuildContractConfirm();gcPage=0;gcSelected.clear();gcRender();}
void gcSortChanged(MyGUI::ComboBox*,size_t index){if(gcUpdating||!gcSort)return;closeGuildContractConfirm();gcSortMode=std::min(5,(int)index);gcPage=0;gcRender();}
void gcPageClick(MyGUI::Widget* sender){closeGuildContractConfirm();int last=GuildContracts::pages((int)gcVisible.size(),gcCapacity)-1;if(sender==gcNav[0])gcPage=0;else if(sender==gcNav[1])--gcPage;else if(sender==gcNav[3])++gcPage;else if(sender==gcNav[4])gcPage=last;gcSelected.clear();gcRender();}
std::string gcValue(const std::string& s){return s.empty()?"-":s;}
// Keep complete values in Details; long table cells use a bounded preview.
std::string gcWrap(MyGUI::TextBox* t,const std::string& value,int font){std::string wrapped=wrapRegisterCaption(t,value,font),out;std::stringstream input(wrapped);std::string line;while(std::getline(input,line)){std::string part;for(size_t i=0;i<line.size();){size_t end=i+1;while(end<line.size()&&((unsigned char)line[end]&0xc0)==0x80)++end;std::string glyph=line.substr(i,end-i);MercenarieFonts::caption(t,part+glyph);if(!part.empty()&&t->getTextSize().width>t->getWidth()-2){out+=part+"\n";part.clear();}part+=glyph;i=end;}out+=part+"\n";}if(!out.empty())out.erase(out.size()-1);return out;}
void gcCell(MyGUI::TextBox* t,const std::string& value,int font){t->setFontHeight(font);std::stringstream lines(gcWrap(t,value,font));std::string line,out;int capacity=std::max(1,t->getHeight()/(font+2));for(int n=0;n<capacity&&std::getline(lines,line);++n){if(n==capacity-1&&lines.peek()!=EOF){MercenarieFonts::caption(t,line+"...");while(!line.empty()&&t->getTextSize().width>t->getWidth()-2){size_t end=line.size()-1;while(end>0&&((unsigned char)line[end]&0xc0)==0x80)--end;line.erase(end);MercenarieFonts::caption(t,line+"...");}line+="...";}if(!out.empty())out+="\n";out+=line;}MercenarieFonts::caption(t,out);}
void gcRender(){
 if(!gcSearch||!gcCounts)return;if(gcSearchHint)gcSearchHint->setVisible(gcSearch->getOnlyText().asUTF8().empty());GuildContracts::Counts counts=GuildContracts::count(gcRows);int numbers[]={counts.total,counts.active,counts.completed,counts.failed};const char* names[]={"all","active","completed","failed"};
 for(int i=0;i<4;++i){o95Caption(gcFilterLabels[i],gcText(names[i])+" ("+registerNumber(numbers[i])+")",18);gcFilterLabels[i]->setTextColour(gcFilter==i-1?o95Gold:o95Ink);gcFilters[i]->setStateSelected(gcFilter==i-1);}
 MercenarieFonts::caption(gcCounts,registerNumber(counts.total)+" "+gcText("total")+"   |   "+registerNumber(counts.active)+" "+gcText("active")+"   |   "+registerNumber(counts.completed)+" "+gcText("completed")+"   |   "+registerNumber(counts.failed)+" "+gcText("failed"));fitRegisterText(gcCounts,gcFont);
 gcVisible=GuildContracts::filter(gcRows,gcFilter,gcSearch->getOnlyText().asUTF8(),gcSortMode);gcPage=GuildContracts::clampPage(gcPage,(int)gcVisible.size(),gcCapacity);int last=GuildContracts::pages((int)gcVisible.size(),gcCapacity)-1;
 MercenarieFonts::caption(gcNavLabels[2],registerNumber(gcPage+1)+" / "+registerNumber(last+1));gcNav[0]->setEnabled(gcPage>0);gcNav[1]->setEnabled(gcPage>0);gcNav[3]->setEnabled(gcPage<last);gcNav[4]->setEnabled(gcPage<last);
 for(int i=0;i<5;++i)gcNavLabels[i]->setTextColour((i==2||(i<2?gcPage>0:gcPage<last))?registerAmber:MyGUI::Colour(.35f,.35f,.35f));
 bool found=false;for(size_t i=0;i<gcVisible.size();++i)if(gcRows[gcVisible[i]].key==gcSelected)found=true;if(!found)gcSelected=gcVisible.empty()?"":gcRows[gcVisible[gcPage*gcCapacity]].key;
 gcEmpty->setVisible(gcVisible.empty());
 for(size_t c=0;c<gcCards.size();++c){size_t n=gcPage*gcCapacity+c;GuildContractCard& card=gcCards[c];card.button->setVisible(n<gcVisible.size());if(n>=gcVisible.size())continue;const GuildContracts::Row& r=gcRows[gcVisible[n]];card.button->setStateSelected(r.key==gcSelected);card.badge->setVisible(r.key==gcSelected);o95Caption(card.cells[1],r.name,23);o95Caption(card.cells[3],r.destination,17);o95Caption(card.cells[4],r.reward.empty()?"\342\200\224":r.reward,22);o95Caption(card.cells[7],gcStatus(r.status),17);card.cells[7]->setTextColour(gcStatusColour(r.status));gcSetIcon(card.type,r.typeIcon);card.type->setColour(o95Gold);}
 int selected=GuildContracts::find(gcRows,gcSelected);gcMap->setEnabled(selected>=0&&gcRows[selected].canMap);bool cancellable=selected>=0&&gcRows[selected].status==GuildContracts::Active&&gcRows[selected].canCancel;gcCancel->setVisible(cancellable);gcCancel->setEnabled(cancellable);
 if(selected<0){o95Caption(gcDetailTitle,gcText("select"),24);o95Caption(gcDetailStatus,"",18);o95Caption(gcDetailText,"",18);o95Caption(gc99Reward,"\342\200\224",35);o95Caption(gc99Objective,"",18);o95Caption(gc99From,"",18);o95Caption(gc99To,"",18);o95Caption(gc99Step,"",18);gcDetailIcon->setVisible(false);return;}
 const GuildContracts::Row& r=gcRows[selected];gcDetailIcon->setVisible(true);gcSetIcon(gcDetailIcon,r.typeIcon);gcDetailIcon->setColour(o95Gold);o95Caption(gcDetailTitle,r.name,26);o95Caption(gcDetailStatus,r.state.empty()?gcStatus(r.status):r.state,18);gcDetailStatus->setTextColour(gcStatusColour(r.status));
 size_t split=r.destination.find(" > ");o95Caption(gc99From,split==std::string::npos?r.destination:r.destination.substr(0,split),19);o95Caption(gc99To,split==std::string::npos?"":r.destination.substr(split+3),19);
 std::string facts=gcText("donor")+" : "+gcValue(r.donor);if(!r.target.empty())facts+="\n"+gcText("target")+" : "+r.target;if(!r.difficulty.empty())facts+="\n"+gcText("difficulty")+" : "+r.difficulty;if(!r.duration.empty())facts+="\n"+gcText("duration")+" : "+r.duration;
 gcDetailText->setFontName("Negotiation74Serif32");gcDetailText->setFontHeight(op95(23));gcDetailText->setCaption(facts);gcDetailText->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);gcDetailText->setCaption(gcWrap(gcDetailText,facts,op95(23)));gcDetailText->setFontName("Negotiation74Serif32");gcDetailText->setFontHeight(op95(23));int height=std::max(op95(95),gcDetailText->getTextSize().height+op95(12));gcDetailText->setSize(gcDetailText->getWidth(),height);gcDetails->setCanvasSize(gcDetailText->getWidth()+op95(12),height);
 o95Caption(gc99RewardLabel,r.status==GuildContracts::Active?o95("R\303\211COMPENSE ESTIM\303\211E","ESTIMATED REWARD"):o95("R\303\211COMPENSE ENREGISTR\303\211E","RECORDED REWARD"),17);o95Caption(gc99Reward,r.reward.empty()?o95("Non renseign\303\251e","Not recorded"):r.reward,34);
 o95Caption(gc99Step,r.status==GuildContracts::Active?o95("PROCHAINE \303\211TAPE","NEXT STEP"):o95("R\303\211SULTAT DU CONTRAT","CONTRACT RESULT"),17);
 std::string objective=r.status==GuildContracts::Active?(r.objectives.empty()?r.state:r.objectives):(r.state+(r.description.empty()?"":"\n"+r.description));
 if(r.source==GuildContracts::Mail&&r.status==GuildContracts::Active&&r.slot>=0&&r.slot<(int)mailContracts.size()){const MailContracts::Contract& m=mailContracts[r.slot];if(m.contractId==r.id){for(size_t k=0;k<m.steps.size();++k)if(!m.steps[k].delivered){objective=o95("Remettre le courrier \303\240 : ","Deliver the letter to: ")+mercenarieLocalize(m.steps[k].townName)+"\n"+o95("Destinataire : ","Recipient: ")+gcText(m.steps[k].recipientRole==1?"barman":m.steps[k].recipientRole==2?"police":m.steps[k].recipientRole==3?"merchant":m.steps[k].recipientRole==4?"shinobi":m.steps[k].recipientRole==5?"guild":"missing");break;}}}
 o95Caption(gc99Objective,objective.empty()?gcStatus(r.status):objective,19);o95Caption(gcMap,o95("Voir la destination","View destination"),22);o95Caption(gcCancel,gcText("cancel"),17);
}

void refreshGuildContracts(bool force){if(!guildRelationsPanel||(!force&&!guildRelationsPanel->getVisible()))return;unsigned long now=GetTickCount();if(!force&&now-gcLastRefresh<500)return;gcLastRefresh=now;std::vector<GuildContracts::Row> fresh=collectGuildContracts();std::string signature=GuildContracts::fingerprint(fresh);if(!force&&signature==gcFingerprint)return;gcFingerprint=signature;gcRows.swap(fresh);gcRender();}
void buildGuildContracts(MyGUI::Widget* c){
 gcFont=std::max(12,op95(17));gcCapacity=4;gcRows.clear();gcCards.clear();gcSelected.clear();gcPending.clear();gcFingerprint.clear();gcPage=0;gcFilter=-1;gcSortMode=0;
 guildRelationsPanel=c->createWidget<MyGUI::Widget>("PanelEmpty",0,0,c->getWidth(),c->getHeight(),MyGUI::Align::Default);guildRelationsPanel->setNeedMouseFocus(false);guildRelationsPanel->setInheritsPick(true);
 MyGUI::Widget* list=panel95(guildRelationsPanel,228,114,794,672);MyGUI::Widget* detail=panel95(guildRelationsPanel,1034,114,486,672);
 ot95(list,20,12,750,34,o95("REGISTRE DES MISSIONS","MISSION REGISTER"),25);ot95(list,20,47,750,26,o95("Suivi et historique de vos contrats.","Track your missions and their history."),17);
 for(int i=0;i<4;++i){gcFilters[i]=ob95(list,20+i*190,88,182,44,"");gcFilterLabels[i]=ot95(gcFilters[i],3,0,176,44,"",18,true);gcFilterLabels[i]->setTextAlign(MyGUI::Align::Center);gcFilters[i]->eventMouseButtonClick+=MyGUI::newDelegate(gcFilterClick);}
 gcSearch=list->createWidget<MyGUI::EditBox>("MercenarieContractSearch",op95(20),op95(146),op95(458),op95(44),MyGUI::Align::Default);gcSearch->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);gcSearch->setFontName("Negotiation74Serif32");gcSearch->setFontHeight(op95(23));gcSearch->setTextColour(o95Ink);gcSearch->eventEditTextChange+=MyGUI::newDelegate(gcSearchChanged);gcSearchHint=ot95(gcSearch,12,0,430,44,gcText("search"),18);
 gcSort=list->createWidget<MyGUI::ComboBox>("MercenarieContractSort",op95(490),op95(146),op95(284),op95(44),MyGUI::Align::Default);gcSort->setComboModeDrop(true);gcSort->setFontName("Negotiation74Serif32");gcSort->setFontHeight(op95(23));gcSort->setTextColour(o95Ink);const char* sorts[]={"recent_sort","oldest","reward_asc","reward_desc","difficulty_sort","duration_sort"};for(int i=0;i<6;++i)gcSort->addItem(gcText(sorts[i]));gcSort->setIndexSelected(0);combo107(gcSort);gcSort->eventComboChangePosition+=MyGUI::newDelegate(gcSortChanged);
 gcTable=list->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",op95(14),op95(207),op95(766),op95(366),MyGUI::Align::Default);MercenarieNativeInput::bind(gcTable);gcTable->setVisibleVScroll(false);gcTable->setVisibleHScroll(false);gcTable->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);gcTableWidth=op95(756);gcTable->setCanvasSize(gcTableWidth,op95(360));
 for(int row=0;row<4;++row){GuildContractCard card;card.donor=0;for(int i=0;i<8;++i)card.cells[i]=0;card.button=ob95(gcTable,0,row*90,756,82,"");card.button->eventMouseButtonClick+=MyGUI::newDelegate(gcSelect);card.badge=card.button->createWidget<MyGUI::Widget>("PanelEmpty",0,0,op95(756),op95(82),MyGUI::Align::Default);card.badge->setNeedMouseFocus(false);registerSolid(card.badge,0,0,op95(756),1,o95Gold);registerSolid(card.badge,0,op95(82)-1,op95(756),1,o95Gold);registerSolid(card.badge,0,0,1,op95(82),o95Gold);registerSolid(card.badge,op95(756)-1,0,1,op95(82),o95Gold);card.type=gcIcon(card.button,op95(18),op95(22),op95(36));card.type->setNeedMouseFocus(false);card.cells[1]=ot95(card.button,72,9,359,32,"",23);card.cells[3]=ot95(card.button,72,44,370,27,"",17);card.cells[4]=ot95(card.button,452,20,170,42,"",22,true);MyGUI::Widget* status=panel95(card.button,628,23,116,37);card.cells[7]=ot95(status,0,0,116,37,"",17);card.cells[7]->setTextAlign(MyGUI::Align::Center);gcCards.push_back(card);}
 gcEmpty=ot95(list,30,240,730,200,gcText("empty"),22);gcEmpty->setTextAlign(MyGUI::Align::Center);gcCounts=ot95(list,20,583,754,30,"",17);
 const char* nav[]={"<<","<","",">",">>"};for(int i=0;i<5;++i){gcNav[i]=ob95(list,438+i*65,625,i==2?76:54,32,"");gcNavLabels[i]=ot95(gcNav[i],0,0,i==2?76:54,32,nav[i],18,true);gcNavLabels[i]->setTextAlign(MyGUI::Align::Center);gcNav[i]->eventMouseButtonClick+=MyGUI::newDelegate(gcPageClick);}gcNav[2]->setEnabled(false);gcNav[0]->setVisible(false);gcNav[4]->setVisible(false);
 ot95(detail,20,12,446,32,o95("D\303\211TAILS DU CONTRAT","CONTRACT DETAILS"),22);registerSolid(detail,op95(20),op95(52),op95(446),1,o95Edge);gcDetailIcon=gcIcon(detail,op95(24),op95(77),op95(56));gcDetailTitle=ot95(detail,95,70,367,55,"",26);gcDetailStatus=ot95(detail,95,128,363,35,"",18,true);registerSolid(detail,op95(20),op95(177),op95(446),1,o95Edge);
 ot95(detail,20,189,446,26,o95("ITIN\303\211RAIRE","ROUTE"),17);gc99From=ot95(detail,20,223,204,48,"",19);gc99From->setTextColour(registerGreen);ot95(detail,223,223,24,48,">",19,true);gc99To=ot95(detail,254,223,212,48,"",19);gc99To->setTextColour(MyGUI::Colour(.86f,.33f,.27f));gcDetails=detail->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",op95(20),op95(278),op95(446),op95(95),MyGUI::Align::Default);MercenarieNativeInput::bind(gcDetails);gcDetails->setVisibleHScroll(false);gcDetails->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);gcDetailText=ot95(gcDetails,2,0,419,95,"",19);
 registerSolid(detail,op95(20),op95(381),op95(446),1,o95Edge);gc99RewardLabel=ot95(detail,20,391,446,28,"",17);gc99Reward=ot95(detail,20,424,446,52,"",34,true);
 MyGUI::Widget* step=panel95(detail,20,480,446,92);gc99Step=ot95(step,12,5,422,25,"",17,true);gc99Objective=ot95(step,12,34,422,60,"",19);
 gcMap=ob95(detail,20,582,446,44,o95("Voir la destination","View destination"));gcMap->changeWidgetSkin("Board77Gold");gcMap->setTextAlign(MyGUI::Align::Center);o95Caption(gcMap,o95("Voir la destination","View destination"),20);gcMap->setTextColour(MyGUI::Colour(.08f,.07f,.04f));gcMap->eventMouseButtonClick+=MyGUI::newDelegate(gcOpenMap);gcCancel=ob95(detail,20,634,446,32,gcText("cancel"));gcCancel->setTextColour(MyGUI::Colour(.75f,.32f,.25f));gcCancel->eventMouseButtonClick+=MyGUI::newDelegate(gcRequestCancel);gcCancelNote=ot95(detail,0,0,1,1,"",12);gcCancelNote->setVisible(false);
 gcConfirm=panel95(guildRelationsPanel,460,265,650,260);gcConfirm->setWidgetStyle(MyGUI::WidgetStyle::Popup,"Popup");gcConfirm->setNeedMouseFocus(true);ot95(gcConfirm,20,16,610,40,gcText("confirm_title"),28);ot95(gcConfirm,20,67,610,112,gcText("confirm_body"),20);MyGUI::Button* yes=ob95(gcConfirm,20,194,295,44,gcText("confirm"));yes->eventMouseButtonClick+=MyGUI::newDelegate(gcConfirmCancel);MyGUI::Button* no=ob95(gcConfirm,335,194,295,44,gcText("back"));no->eventMouseButtonClick+=MyGUI::newDelegate(gcBack);gcConfirm->setVisible(false);guildRelationsPanel->setVisible(false);
}

