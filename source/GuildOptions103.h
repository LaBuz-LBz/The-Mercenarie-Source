// Accordion presentation; all controls keep their original persistence callbacks.
void refreshOptions103(){}
void openOptions108(){
 if(!o103Ready)return;
 for(int c=0;c<8;++c)optionsCategoryOpen[c]=false;
 optionsSelectedCategory=-1;optionsCategoryFilter->setIndexSelected(0);combo107(optionsCategoryFilter);
 optionsSearch->setCaption("");optionsSearchHint->setVisible(true);
 optionsScroll->setViewOffset(MyGUI::IntPoint(0,0));layoutOptions103();
}
void layoutOptions103(){
 if(!o103Ready||!optionsScroll)return;
 std::string query=optionsSearch?optionsLower(optionsSearch->getOnlyText()):std::string();int y=0;
 const int width=op95(1248),header=op95(44),rowH=op95(56),gap=op95(10);
 for(int c=0;c<8;++c){bool allowed=(optionsSelectedCategory<0||optionsSelectedCategory==c)&&(c!=7||clientOptions.developerMode);int rows=0,total=0;
  for(size_t r=0;r<optionsRows.size();++r){OptionsRow& e=optionsRows[r];if(e.category!=c)continue;++total;bool show=allowed&&(query.empty()||optionsLower(e.title+" "+e.description).find(query)!=std::string::npos);e.widget->setVisible(show);if(show){e.widget->setCoord(0,rows*rowH,width,rowH);++rows;}}
  bool visible=allowed&&rows>0;bool expanded=optionsCategoryOpen[c];optionsCategoryHeaders[c]->setVisible(visible);optionsCategoryBodies[c]->setVisible(visible&&expanded);if(!visible)continue;
  optionsCategoryHeaders[c]->setCoord(0,y,width,header);y+=header;
  o95Caption(optionsCategoryTitles[c],std::string(Loc::text(optionCategoryKey(c)))+" ("+registerNumber(total)+")",21);
  o95Caption(optionsCategoryChevrons[c],expanded?"v":">",22);
  if(expanded){optionsCategoryBodies[c]->setCoord(0,y,width,rows*rowH);y+=rows*rowH;}y+=gap;
 }
 int viewport=optionsScroll->getViewCoord().height;optionsScroll->setCanvasSize(width,std::max(viewport,y));optionsScroll->setVisibleVScroll(y>viewport);
 MyGUI::IntPoint offset=optionsScroll->getViewOffset();offset.left=0;offset.top=std::max(-std::max(0,y-viewport),std::min(0,offset.top));optionsScroll->setViewOffset(offset);
}
void option104Text(MyGUI::TextBox* t,const std::string& value,int size){
 t->setFontName((Loc::engine().language=="fr"||Loc::engine().language=="en")?"Negotiation74Serif32":MercenarieFonts::fontFor(Loc::engine().language));t->setFontHeight(std::max(12,op95(size)*40/32));t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);t->setCaption(wrapRegisterCaption(t,value,t->getFontHeight()));
 while(t->getFontHeight()>12&&t->getTextSize().height>t->getHeight()){t->setFontHeight(t->getFontHeight()-1);t->setCaption(wrapRegisterCaption(t,value,t->getFontHeight()));}t->setNeedMouseFocus(false);
}
void buildOptions103(){
 o103Ready=false;guildOptionsPanel->attachToWidget(overview95Root);guildOptionsPanel->setCoord(op95(228),op95(112),op95(1294),op95(674));guildOptionsPanel->changeWidgetSkin("Board77Panel");guildOptionsPanel->setNeedMouseFocus(false);guildOptionsPanel->setInheritsPick(true);
 // Old decoration only. Leave native skin clients, especially ComboBox labels, intact.
 for(size_t j=0;j<guildOptionsPanel->getChildCount();++j)guildOptionsPanel->getChildAt(j)->setVisible(false);
 optionsSearch->setCoord(op95(14),op95(12),op95(555),op95(44));optionsSearch->setVisible(true);optionsSearch->setFontName("MercenarieUnicode");optionsSearch->setFontHeight(std::max(12,op95(20)));
 for(size_t j=0;j<optionsSearch->getChildCount();++j)optionsSearch->getChildAt(j)->setVisible(false);
 optionsSearchHint->setCoord(op95(14),0,op95(520),op95(44));optionsSearchHint->setVisible(optionsSearch->getOnlyText().empty());o95Caption(optionsSearchHint,Loc::text("options.search"),19);optionKitBorder(optionsSearch,o95Edge);
 optionsCategoryFilter->setCoord(op95(589),op95(12),op95(385),op95(44));optionsCategoryFilter->setVisible(true);optionsCategoryFilter->setFontName("MercenarieUnicode");optionsCategoryFilter->setFontHeight(std::max(12,op95(19)));optionsCategoryFilter->setComboModeDrop(true);for(size_t j=0;j<optionsCategoryFilter->getChildCount();++j)optionsCategoryFilter->getChildAt(j)->setVisible(false);optionKitBorder(optionsCategoryFilter,o95Edge);
 optionsResetAllButton->changeWidgetSkin("Overview96Button");optionsResetAllButton->setCoord(op95(1010),op95(12),op95(268),op95(44));optionsResetAllButton->setVisible(true);for(size_t j=0;j<optionsResetAllButton->getChildCount();++j)optionsResetAllButton->getChildAt(j)->setVisible(false);optionsResetAllButton->setTextAlign(MyGUI::Align::Center);o95Caption(optionsResetAllButton,Loc::text("options.reset_all"),18);optionKitBorder(optionsResetAllButton,o95Edge);
 optionsScroll->setCoord(op95(14),op95(76),op95(1268),op95(580));optionsScroll->setVisible(true);optionsScroll->setInheritsPick(true);MercenarieNativeInput::bind(optionsScroll);
 for(int c=0;c<8;++c){MyGUI::Widget* head=optionsCategoryHeaders[c];head->changeWidgetSkin("Overview96Button");head->setSize(op95(1248),op95(44));head->setNeedMouseFocus(true);head->setInheritsPick(true);head->eventMouseButtonClick=MyGUI::newDelegate(optionsCategoryClicked);for(size_t j=0;j<head->getChildCount();++j)head->getChildAt(j)->setVisible(false);
  optionsCategoryChevrons[c]=ot95(head,12,0,26,44,"",22,true);optionsCategoryTitles[c]=ot95(head,88,0,290,44,"",21,true);optionsCategoryDescriptions[c]=ot95(head,410,0,800,44,Loc::text((std::string(optionCategoryKey(c))+".help").c_str()),16);oi95(head,c==0?6:c==1?7:c==3?2:c==6?5:6,43,10,24);optionKitBorder(head,o95Edge);
  optionsCategoryBodies[c]->changeWidgetSkin("PanelEmpty");optionsCategoryBodies[c]->setNeedMouseFocus(false);optionsCategoryBodies[c]->setInheritsPick(true);optionsCategoryOpen[c]=false;
 }
 for(size_t r=0;r<optionsRows.size();++r){MyGUI::Widget* row=optionsRows[r].widget;row->changeWidgetSkin("Board77Panel");row->setSize(op95(1248),op95(56));row->setNeedMouseFocus(false);row->setInheritsPick(true);
  MyGUI::TextBox* title=row->getChildAt(0)->castType<MyGUI::TextBox>();title->setCoord(op95(16),op95(3),op95(477),op95(50));option104Text(title,optionsRows[r].title,20);
  MyGUI::TextBox* description=row->getChildAt(1)->castType<MyGUI::TextBox>();description->setCoord(op95(876),op95(3),op95(350),op95(50));option104Text(description,optionsRows[r].description,15);
  row->getChildAt(2)->setVisible(false);size_t count=row->getChildCount();int controls=(int)count-3;
  for(size_t j=3;j<count;++j){MyGUI::Widget* child=row->getChildAt(j);child->setInheritsPick(true);MyGUI::Button* button=child->castType<MyGUI::Button>(false);MyGUI::ComboBox* combo=child->castType<MyGUI::ComboBox>(false);
   int x=510,w=340;if(controls>1){x=510+int(j-3)*116;w=106;}
   if(child==contractRewardSlider){x=510;w=245;}else if(child==contractRewardValue->getParent()){x=770;w=80;}
   if(button&&!child->getUserString("option").empty()){child->setCoord(op95(665),op95(13),op95(30),op95(30));bool checked=button->getStateSelected();child->changeWidgetSkin("TheMercenarie_CheckboxSkin");button->setStateSelected(checked);button->setCaption("");continue;}
   child->setCoord(op95(x),op95(9),op95(w),op95(38));
   if(combo){for(size_t k=0;k<combo->getChildCount();++k)combo->getChildAt(k)->setVisible(false);optionKitBorder(combo,o95Edge);combo->setFontName("MercenarieUnicode");combo->setFontHeight(std::max(12,op95(20)));combo->setComboModeDrop(true);MercenarieFonts::languageChoice(combo);combo107(combo);combo->setCaption(Loc::languageChoiceLabel(combo->getIndexSelected()));}
   else if(button){std::string text=button->getCaption();button->changeWidgetSkin("Overview96Button");for(size_t k=0;k<button->getChildCount();++k)button->getChildAt(k)->setVisible(false);o95Caption(button,text,16);optionKitBorder(button,o95Edge);}
   else if(child!=contractRewardSlider){for(size_t k=0;k<child->getChildCount();++k){MyGUI::TextBox* t=child->getChildAt(k)->castType<MyGUI::TextBox>(false);if(t){std::string text=t->getCaption();t->setCoord(op95(4),0,op95(w-8),op95(38));o95Caption(t,text,18);}else child->getChildAt(k)->setVisible(false);}}
  }
  registerSolid(row,0,op95(56)-1,op95(1248),1,o95Edge);
 }
 optionsSelectedCategory=-1;optionsCategoryFilter->setIndexSelected(0);combo107(optionsCategoryFilter);optionsViewScale=o95Scale;o103Ready=true;layoutOptions103();guildOptionsPanel->setVisible(false);
}
