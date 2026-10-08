
    // The selected contexts retain their legacy fields above this extension.
    if(a.reading&&a.cursor==a.bytes.size()){resetQuestContexts();questOrdersNeedRestore[0]=true;return;}
    unsigned int version=1;a.field(version);if(version!=1)throw std::runtime_error("unsupported quest pool");
    a.field(questFiscalSequence);
    unsigned int retiredCount=(unsigned int)retiredQuestGroups.size();a.field(retiredCount);if(retiredCount>4096)throw std::runtime_error("invalid cleanup queue");if(a.reading)retiredQuestGroups.resize(retiredCount);for(unsigned int r=0;r<retiredCount;++r){a.field(retiredQuestGroups[r].members);a.field(retiredQuestGroups[r].seconds);}
    a.field(selectedEscortQuest);a.field(selectedBountyQuest);
    if(selectedEscortQuest<0||selectedEscortQuest>=5||selectedBountyQuest<0||selectedBountyQuest>=5)throw std::runtime_error("invalid selected quest");
    if(!isolated){escortQuests[selectedEscortQuest].capture();captureBountyQuest();}
    for(int i=0;i<maximumActiveQuests;++i){
        if(i!=selectedEscortQuest)escortQuests[i].archive(a);
        if(i!=selectedBountyQuest){std::string bytes;if(!a.reading){MercenarieV5::BountyWorldState saved=bountyQuests[i];saved.boards.clear();bytes=saved.save();}a.field(bytes);if(a.reading){bountyQuests[i].load(bytes);if(bountyQuests[i].suspended)throw std::runtime_error("invalid bounty quest");}}
    }
    if(activeQuestCount()>maximumActiveQuests)throw std::runtime_error("too many active quests");
    if(a.reading){for(int i=0;i<maximumActiveQuests;++i)questOrdersNeedRestore[i]=true;}
