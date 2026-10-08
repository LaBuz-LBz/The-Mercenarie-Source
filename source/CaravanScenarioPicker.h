#pragma once
// Automatic per-visit plans; legacy picker widgets are removed on update.
namespace {
const char* caravanScenarioWindowName="MercenarieCaravanScenarioPicker";
int caravanNegotiationVariant(const std::string& id,int client){
    for(int i=0;i<maximumActiveQuests;++i){bool selected=i==selectedEscortQuest;
        if((selected?currentMissionFiscalId:escortQuests[i].v_currentMissionFiscalId)!=id)continue;
        std::string& metadata=selected?currentContract.routeRegions:escortQuests[i].v_currentContract.routeRegions;
        if(CaravanNegotiation::read(metadata,0)<0)CaravanNegotiation::choose(metadata,UtilityT::randomInt(0,19),UtilityT::randomInt(0,18),UtilityT::randomInt(0,17),UtilityT::randomInt(0,16));
        return CaravanNegotiation::read(metadata,client);
    }return 0;
}
bool caravanDeliveryInstructionPending(const std::string& id){
    for(int slot=0;slot<maximumActiveQuests;++slot){bool selected=slot==selectedEscortQuest;
        if((selected?currentMissionFiscalId:escortQuests[slot].v_currentMissionFiscalId)!=id)continue;
        const std::string& metadata=selected?currentContract.routeRegions:escortQuests[slot].v_currentContract.routeRegions;
        CaravanDelivery::State s=CaravanDelivery::read(metadata);if(s.scenario<2||CaravanHomeAmbush::read(metadata).triggered)return false;
        int busy=0;for(int j=0;j<4;++j)if(CaravanDelivery::active(s.parcels[j]))++busy;if(busy>=2)return false;
        const std::vector<hand>& members=selected?progressMembers:escortQuests[slot].v_progressMembers;
        Character* leader=selected?escort:escortQuests[slot].v_escortHandle.getCharacter();bool free=false;
        for(size_t j=0;j<members.size();++j){Character* c=members[j].getCharacter();if(c&&c!=leader&&!c->isAnimal()&&!c->isPlayerCharacter()&&houseFighter(c)&&!CaravanDelivery::owns(s,c->getHandle().toString()))free=true;}if(!free)return false;
        const std::vector<CaravanCustomers::Group>& groups=caravanCustomerSnapshot();
        for(size_t i=0;i<groups.size();++i)if(groups[i].id==id)for(size_t j=0;j<groups[i].clients.size();++j)if(j<4&&CaravanCustomers::purchased(groups[i].clients[j])&&s.parcels[j].stage==CaravanDelivery::Waiting)return true;
    }return false;
}
bool caravanCrewSpeaking(const std::string& id){
    for(int i=0;i<maximumActiveQuests;++i){const bool selected=i==selectedEscortQuest;
        if((selected?currentMissionFiscalId:escortQuests[i].v_currentMissionFiscalId)!=id)continue;
        const std::vector<hand>& members=selected?progressMembers:escortQuests[i].v_progressMembers;
        for(size_t j=0;j<members.size();++j)if(caravanSpeaking(members[j].getCharacter()))return true;
    }return false;
}
bool caravanDeliveryFor(const std::string& id,int client){
    for(int i=0;i<maximumActiveQuests;++i){
        const bool selected=i==selectedEscortQuest;
        if((selected?currentMissionFiscalId:escortQuests[i].v_currentMissionFiscalId)==id)
        {const std::string& m=selected?currentContract.routeRegions:escortQuests[i].v_currentContract.routeRegions;
            CaravanVisit::Plan p=CaravanVisit::read(m);return CaravanDelivery::read(m).scenario>=2&&(!p.exists||CaravanVisit::delivery(p,client));}
    }return false;
}

void initializeCaravanVisit(){
 if(CaravanVisit::read(currentContract.routeRegions).exists)return;
 int rolls[4];for(int i=0;i<4;++i)rolls[i]=UtilityT::randomInt(0,3);
 int countRoll=UtilityT::randomInt(0,2);
 CaravanVisit::Plan p=CaravanVisit::draw(countRoll,UtilityT::randomInt(0,3),rolls,UtilityT::randomInt(0,countRoll+1),caravanHasDeliveryGuard());
 CaravanVisit::write(currentContract.routeRegions,p);
 CaravanDelivery::State delivery;delivery.scenario=p.trap>=0?3:p.mask?2:1;
 for(int i=0;i<4;++i)delivery.parcels[i].stage=CaravanVisit::delivery(p,i)?CaravanDelivery::Waiting:CaravanDelivery::Done;
 CaravanDelivery::write(currentContract.routeRegions,delivery);
 DebugLog("CARAVAN VISIT automatic plan saved");
}
void maintainCaravanScenarioPicker(){
 MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();MyGUI::Window* window=gui?gui->findWidget<MyGUI::Window>(caravanScenarioWindowName,false):0;
 if(window)gui->destroyWidget(window);
}
void showCaravanScenarioPicker(){initializeCaravanVisit();}
}
