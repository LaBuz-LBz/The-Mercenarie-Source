// Existing player-state projection; location exposed for the new cards.
void appendEscortTrackerItem(){
    Character* tracked=escortHandle.isNull()?0:escortHandle.getCharacter();
    if((missionActive||missionPending)&&!currentContract.settlementPaid&&(!tracked||!tracked->isDead())){
        QuestTrackerItem item;item.type=(int)currentContract.type;item.objective=mercenarieLocalize(originCity)+QuestTrackerText::arrow()+mercenarieLocalize(currentContract.destination);
        if(scientificReturning||caravanReturning)item.objective=mercenarieLocalize(currentContract.destination)+QuestTrackerText::arrow()+mercenarieLocalize(originCity);
        item.location=mercenarieLocalize((scientificReturning||caravanReturning)?originCity:currentContract.destination);
        bool returning=false;for(size_t i=0;i<waitingHere.size();++i)if(waitingHere[i].returning)returning=true;
        bool fighting=tracked&&tracked->isInCombatMode(true,true);
        item.state=registerLanguage(missionPending?Loc::text("ui.awaiting_acceptance"):fighting?(missionPaused?Loc::text("ui.in_combat_will_wait_afterwards"):Loc::text("ui.in_combat")):missionPaused?(returning?Loc::text("ui.returning_to_waiting_point"):Loc::text("ui.waiting_your_order")):waitingForPlayer?Loc::text("ui.waiting_for_your_company"):scientificResearching?Loc::text("ui.research_in_progress"):(scientificReturning||caravanReturning)?Loc::text("ui.return_journey_e07c7f6"):Loc::text("ui.on_the_way"),missionPending?Loc::text("ui.awaiting_acceptance"):fighting?(missionPaused?Loc::text("ui.in_combat_will_wait_afterwards"):Loc::text("ui.in_combat")):missionPaused?(returning?Loc::text("ui.returning_to_waiting_point"):Loc::text("ui.waiting_your_order")):waitingForPlayer?Loc::text("ui.waiting_for_your_company"):scientificResearching?Loc::text("ui.research_in_progress"):(scientificReturning||caravanReturning)?Loc::text("ui.return_journey_e07c7f6"):Loc::text("ui.on_the_way"));
        if(missionFollowing){Character* followed=missionFollowTarget.getCharacter();item.state+=registerLanguage(Loc::text("ui.following"),Loc::text("ui.following"))+(followed?followed->getName():registerLanguage(Loc::text("ui.unavailable"),Loc::text("ui.unavailable")));}
        if(tracked)item.distance=QuestTrackerText::distance(Ogre::Math::Sqrt(tracked->getPosition().squaredDistance(destination)),gMercenarieEnglish);
        item.click=trackerEntryClicked;item.clickable=tracked&&tracked==escort;questTrackerItems.push_back(item);
    }
}
