    struct BoardOffer
    {
        std::string tier, townId, townName, profile, story, squadId, rarityName, routeRegions;
        float distance,danger;
        int estimatedPay, rarity, groupSize,missionType,tierIndex,source,dangerLevel,environmentTags,caravanSize,cargoClass,studyClass,studyDuration;
        bool available, prestigious;
        BoardOffer():distance(0),danger(1),estimatedPay(0),rarity(0),groupSize(1),missionType(0),tierIndex(0),source(MCS_TAVERN),dangerLevel(1),environmentTags(0),caravanSize(MCZ_SMALL),cargoClass(MCC_BASIC),studyClass(MSC_SMALL),studyDuration(MSD_SHORT),available(false),prestigious(false){}
    } boardOffers[6];
    bool boardOfferAvailable(const BoardOffer& offer){return offer.available;}
    struct CityContractBoard{BoardOffer offers[6];double expiresAt;CityContractBoard():expiresAt(0){}};
    std::map<std::string,CityContractBoard> savedContractBoards;
