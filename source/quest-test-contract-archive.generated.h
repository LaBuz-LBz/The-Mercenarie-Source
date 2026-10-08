    void field(MercRewardBreakdown& v){field(v.base);field(v.distance);field(v.mission);field(v.environment);field(v.beforeDanger);field(v.afterDanger);field(v.afterRarity);field(v.guildHouseBonus);field(v.beforeNegotiation);field(v.negotiation);field(v.finalBeforeMissionBonuses);}
    void field(EscortContractData& v){field(v.schemaVersion);field(v.origin);field(v.destination);field(v.originId);field(v.destinationId);field(v.routeRegions);
#ifdef MERCENARIE_CARAVAN_DELIVERY
        if(reading)CaravanDelivery::read(v.routeRegions);
#ifdef MERCENARIE_CARAVAN_VISIT
        if(reading)CaravanVisit::read(v.routeRegions);
#endif // Validate before committing any restored quest.
#ifdef MERCENARIE_CARAVAN_HOME_AMBUSH
        if(reading)CaravanHomeAmbush::read(v.routeRegions),CaravanBattleResolution::read(v.routeRegions);
#ifdef MERCENARIE_CARAVAN_SPEECH
        if(reading)CaravanNegotiation::read(v.routeRegions,0);
#endif
#endif
#endif
        field(v.type);field(v.source);field(v.rarity);field(v.caravanSize);field(v.cargoClass);field(v.studyClass);field(v.studyDuration);field(v.environmentTags);field(v.dangerLevel);field(v.groupSize);field(v.cargoValue);field(v.selectedBonuses);field(v.bonusPay);field(v.advance);field(v.finalPay);field(v.totalPay);field(v.counterOffers);field(v.negotiationPercent);field(v.basePay);field(v.distanceKm);field(v.danger);field(v.initialRate);field(v.negotiatedRate);field(v.wealth);field(v.personality);field(v.prestigious);field(v.urgent);field(v.advancePaidOnce);field(v.settlementPaid);field(v.reward);}
    void field(EscortJourneyData& v){field(v.seconds);field(v.distanceTravelled);field(v.combats);field(v.ambushes);field(v.dangerousRegions);field(v.detours);field(v.knockedOut);field(v.gravelyInjured);field(v.mutilated);field(v.arrivedOnTime);}