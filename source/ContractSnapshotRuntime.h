// Included after mission state. Capturing information never changes Cats.
std::string historyType(int type){return type==MCT_CARAVAN?"caravan":type==MCT_SCIENCE?"science":type==MCT_MAIL?"mail":"escort";}
GuildHistory::Snapshot escortSnapshot(){
 GuildHistory::Snapshot s;s.id=currentMissionFiscalId;s.type=historyType(currentContract.type);s.giver=currentContract.source==MCS_TAVERN?1:5;
 s.origin=originCity;s.destination=destinationName;s.originId=currentContract.originId;s.destinationId=currentContract.destinationId;s.reward=missionReward;s.difficulty=currentContract.dangerLevel;
 return s;
}
void seedEscort(){GuildHistory::Snapshot s=escortSnapshot();s.status=0;s.accepted=currentGameHours;if(currentContract.source==MCS_TAVERN){Character* giver=resolveContractBarman();if(giver){s.giverId=giver->getHandle().toString();s.giverName=giver->getName();}}contractSeeds[s.id]=s;}
void archiveEscort(int status,int bonus,long long bonusReward){
 GuildHistory::Snapshot s=escortSnapshot();std::map<std::string,GuildHistory::Snapshot>::iterator seed=contractSeeds.find(s.id);if(seed!=contractSeeds.end())s=seed->second;
 s.status=status;s.bonus=bonus;s.bonusReward=bonusReward;s.completed=currentGameHours;if(reportStartHour>=0&&currentGameHours>=reportStartHour)s.duration=currentGameHours-reportStartHour;
 GuildHistory::append(contractHistory,s);contractSeeds.erase(s.id);
}
void archiveMail(const MailContracts::Contract& c,int status){
 GuildHistory::Snapshot s;s.id=c.contractId;s.type=c.steps.size()>1?"mail_multi":"mail";s.giver=c.senderRole;s.giverId=c.senderId;s.giverName=c.senderName;s.origin=c.originTownName;s.originId=c.originTownId;s.reward=c.reward;s.accepted=c.acceptedAtWorldHour;s.completed=currentGameHours;s.status=status;
 if(MailContracts::hasDeadline(c))s.deadline=c.deadlineWorldHour;
 for(size_t i=0;i<c.steps.size();++i){if(i)s.destination+=" > ";s.destination+=c.steps[i].townName;s.destinationId=c.steps[i].townId;s.routeIds+=(s.routeIds.empty()?"":"\n")+c.steps[i].townId;}
 GuildHistory::append(contractHistory,s);
}
void archiveDelegated(const DelegatedMissionState& c,int status){
 GuildHistory::Snapshot s;s.id=c.groupId+":"+c.offerIdentity;s.giverId=c.issuerIdentity;s.type=c.timing.activityType==DelegatedMissionTiming::ActivityBountyHunt?"bounty":c.timing.activityType==DelegatedMissionTiming::ActivityCaravan?"caravan":c.timing.activityType==DelegatedMissionTiming::ActivityScientificExpedition?"science":c.timing.activityType==DelegatedMissionTiming::ActivityMailDelivery?"mail":"escort";
 s.giver=c.kind==1?1:c.kind==2?2:0;s.origin=c.origin;s.destination=c.destination;s.reward=c.reward;s.accepted=c.timing.startedAtWorldHour;s.completed=currentGameHours;s.estimateMin=c.timing.estimateEarliestReturnWorldHour-s.accepted;s.estimateMax=c.timing.estimateLatestReturnWorldHour-s.accepted;std::map<std::string,GuildHistory::Snapshot>::iterator seed=contractSeeds.find(s.id);if(seed!=contractSeeds.end()){s=seed->second;s.completed=currentGameHours;}s.status=status;GuildHistory::append(contractHistory,s);contractSeeds.erase(s.id);
}
