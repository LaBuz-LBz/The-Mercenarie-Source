inline void destroyTooltip(){if(tooltip&&MyGUI::Gui::getInstancePtr())MyGUI::Gui::getInstance().destroyWidget(tooltip);tooltip=0;}
inline void destroyModal(){destroyTooltip();if(modalRoot&&MyGUI::Gui::getInstancePtr()){if(MyGUI::InputManager::getInstancePtr())MyGUI::InputManager::getInstance().removeWidgetModal(modalRoot);MyGUI::Gui::getInstance().destroyWidget(modalRoot);}modalRoot=popup=summaryPanel=changelogPanel=0;dismissCheck=continueButton=changelogButton=returnButton=0;pendingClose=false;}
inline void destroyManual(){if(manualButton&&MyGUI::Gui::getInstancePtr())MyGUI::Gui::getInstance().destroyWidget(manualButton);manualButton=0;}
inline void cleanup(){destroyModal();destroyManual();lastWidth=lastHeight=0;}

inline void closePressed(MyGUI::Widget*){if(dismissCheck&&dismissCheck->getStateSelected()){if(!persistDismissed())ErrorLog("Mercenarie news: could not persist dismissed version");}destroyTooltip();if(modalRoot)modalRoot->setVisible(false);pendingClose=true;}
inline void previousVersion(MyGUI::Widget*){selectedVersion=MainMenuNewsRules::navigate(selectedVersion,-1,MainMenuNewsRules::versionCount());pendingNavigation=true;}
inline void nextVersion(MyGUI::Widget*){selectedVersion=MainMenuNewsRules::navigate(selectedVersion,1,MainMenuNewsRules::versionCount());pendingNavigation=true;}
inline void checkPressed(MyGUI::Widget* sender){MyGUI::Button* b=sender->castType<MyGUI::Button>(false);if(b)b->setStateSelected(!b->getStateSelected());}
inline void showSummary(MyGUI::Widget*){destroyTooltip();if(summaryPanel)summaryPanel->setVisible(true);if(changelogPanel)changelogPanel->setVisible(false);if(dismissCheck)dismissCheck->setVisible(true);if(continueButton)continueButton->setVisible(true);if(changelogButton)changelogButton->setVisible(true);if(returnButton)returnButton->setVisible(false);}
inline void showChangelog(MyGUI::Widget*){destroyTooltip();if(summaryPanel)summaryPanel->setVisible(false);if(changelogPanel)changelogPanel->setVisible(true);if(dismissCheck)dismissCheck->setVisible(false);if(continueButton)continueButton->setVisible(false);if(changelogButton)changelogButton->setVisible(false);if(returnButton)returnButton->setVisible(true);}
inline void hideTooltip(MyGUI::Widget*,MyGUI::Widget*){destroyTooltip();}
inline void showTooltip(MyGUI::Widget* sender,MyGUI::Widget*){
    destroyTooltip();int index=std::atoi(sender->getUserString("newsIndex").c_str());if(index<0||index>=pageItemCount())return;
    const MyGUI::IntSize& screen=MyGUI::RenderManager::getInstance().getViewSize();int width=std::min(390,screen.width-24);std::string description=Loc::text(pageItems()[index].tooltipText);int lines=3+(int)description.size()/48,height=std::min(250,92+lines*18);
    MyGUI::IntCoord a=sender->getAbsoluteCoord();MainMenuNewsRules::Rect placed=MainMenuNewsRules::placeTooltip(MainMenuNewsRules::Rect(a.left,a.top,a.width,a.height),width,height,screen.width,screen.height);
    tooltip=MyGUI::Gui::getInstance().createWidget<MyGUI::Widget>("TheMercenarie_Panel",placed.x,placed.y,placed.w,placed.h,MyGUI::Align::Default,"ToolTip","MercenarieNewsTooltip");tooltip->setNeedMouseFocus(false);border(tooltip,MyGUI::Colour(.90f,.43f,.08f));
    MyGUI::TextBox* title=text(tooltip,18,12,width-36,32,20,Loc::text(pageItems()[index].tooltipTitle),MyGUI::Colour(.95f,.55f,.15f));title->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
    MyGUI::EditBox* body=wrappedText(tooltip,18,50,width-36,height-62,17,description.c_str(),MyGUI::Colour(.91f,.87f,.77f));body->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);
}

