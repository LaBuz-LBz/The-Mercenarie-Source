void updateMissionBookLevelAccess()
    {
        if(!ou||!ou->player||!ou->player->technology||progressLoadFault||progressWriteBlocked)return;
        GameData* book=ou->gamedata.getData(kMissionBookBuildingId,BUILDING);
        GameData* unlock=ou->gamedata.getData(GuildLevel3Access::researchId(),RESEARCH);
        GuildLevel3Access::reconcile(*ou->player->technology,unlock,book,guildLevel());
    }
