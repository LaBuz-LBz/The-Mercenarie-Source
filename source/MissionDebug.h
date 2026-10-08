#pragma once
#include "MissionDebugRules.h"
// Only invoked from the developer menu. Mission observation remains unchanged.
namespace {
bool missionDebugBusy=false;
DWORD missionDebugReadyAt=0;
MyGUI::Window* missionDebugPicker=0;
bool missionDebugPickerRaid=false;
MyGUI::TextBox* missionDebugTip=0;
void missionDebugTooltip(MyGUI::Widget* sender,const MyGUI::ToolTipInfo& info){
    if(info.type==MyGUI::ToolTipInfo::Hide){if(missionDebugTip)missionDebugTip->setVisible(false);return;}
    if(!sender||!clientOptions.developerMode)return;
    if(!missionDebugTip){missionDebugTip=MyGUI::Gui::getInstance().createWidget<MyGUI::TextBox>("MercenarieDeveloperTooltip",10,10,560,80,MyGUI::Align::Default,"Window");missionDebugTip->setNeedMouseFocus(false);missionDebugTip->setFontHeight(14);}
    const std::string tipKey=sender->getUserString("debugTip");
    std::string text=tipKey.empty()?sender->getUserString("developerCaption"):Loc::text(tipKey.c_str());
    MercenarieFonts::caption(missionDebugTip,wrapRegisterCaption(missionDebugTip,text,14));
    const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();
    missionDebugTip->setPosition(std::max(0,std::min(info.point.left+12,view.width-560)),std::max(0,std::min(info.point.top+24,view.height-80)));missionDebugTip->setVisible(true);
}

bool debugMissionActive(){return MissionDebugRules::active(missionActive,missionPending,currentContract.settlementPaid,completedCleanupClock>0,escort&&!escort->isDead());}
bool debugNpcSafe(Character* c){
    if(!c||!MissionDebugRules::humanoidCandidate(c->isAnimal(),c->isPlayerCharacter())||c->isDead()||!c->getMedical())return false;
    MedicalSystem* m=c->getMedical();
    if(m->isUnconcious()||m->isDead()||m->currentBleedRate>0||m->blood<m->getMaxBlood()*0.75f)return false;
    int parts=0;
    for(int i=0;i<m->getPartCount();++i){
        MedicalSystem::HealthPartStatus* p=m->getPart((unsigned __int64)i);
        if(!p||p->isDead()||p->maxHealth()<=0)return false;
        if(MissionDebugRules::bluntDamage(p->maxHealth(),p->flesh,p->fleshStun)<=0)return false;
        if(!p->isRobotic())++parts;
    }
    return parts>=2;
}
std::vector<Character*> debugNpcCandidatesFor(const std::vector<hand>& members){
    std::vector<Character*> out;
    for(size_t i=0;i<members.size();++i){
        Character* c=members[i].getCharacter();
        if(debugNpcSafe(c)&&std::find(out.begin(),out.end(),c)==out.end())out.push_back(c);
    }
    return out;
}
std::vector<Character*> debugNpcCandidates(){return debugNpcCandidatesFor(progressMembers);}
bool debugMissionCompatible(bool raid){return debugMissionActive()&&(raid||debugNpcCandidates().size()>=2);}
std::vector<int> debugMissionSlots(bool raid){
    std::vector<int> slots;
    for(int i=0;i<maximumActiveQuests;++i){
        if(i==selectedEscortQuest){if(debugMissionCompatible(raid))slots.push_back(i);continue;}
        const EscortQuestContext& q=escortQuests[i];Character* leader=q.v_escortHandle.getCharacter();
        if(MissionDebugRules::active(q.v_missionActive,q.v_missionPending,q.v_currentContract.settlementPaid,q.v_completedCleanupClock>0,leader&&!leader->isDead())&&(raid||debugNpcCandidatesFor(q.v_progressMembers).size()>=2))slots.push_back(i);
    }
    return slots;
}
bool debugClickReady(){return MissionDebugRules::ready(missionDebugBusy,GetTickCount(),missionDebugReadyAt);}
void updateMissionDebugButtons(){
    if(!clientOptions.developerMode){if(missionDebugPicker)missionDebugPicker->setVisible(false);if(missionDebugTip)missionDebugTip->setVisible(false);return;}
    if(!developerWindow||!developerWindow->getVisible())return;
    bool ready=clientOptions.developerMode&&debugClickReady();
    if(developerRaidButton)developerRaidButton->setEnabled(ready&&!debugMissionSlots(true).empty());
    if(developerKoButton)developerKoButton->setEnabled(ready&&!debugMissionSlots(false).empty());
}
void logDebugHealth(std::ostringstream& out,MedicalSystem* m){
    out<<" blood="<<m->blood<<" alive="<<(!m->isDead())<<" KO="<<m->isUnconcious();
    for(int i=0;i<m->getPartCount();++i){MedicalSystem::HealthPartStatus* p=m->getPart((unsigned __int64)i);if(p)out<<" part["<<i<<"]="<<p->flesh<<"/stun="<<p->fleshStun<<"/bandaged="<<p->bandaging<<"/max="<<p->maxHealth();}
}
void executeMissionDebug(bool raid){
    if(!clientOptions.developerMode||!debugClickReady())return;
    missionDebugBusy=true;updateMissionDebugButtons();
    std::ostringstream log,message;
    log<<(raid?"CHEAT RAID TRIGGER":"CHEAT MISSION NPC KO")<<" mission="<<currentMissionFiscalId<<" type="<<(scientificMission?"scientific":caravanMission?"caravan":"escort")<<" difficulty="<<currentContract.dangerLevel;
    bool success=false;
    try{
        refreshQuestActors();
        if(!debugMissionCompatible(raid)){message<<Loc::text(raid?"debug.raid.need":"debug.health");log<<" result=refused inactive/cleanup/unsafe candidates";}
        else if(raid){
            DebugRaidResult report;success=spawnBanditAmbush(&report);
            log<<" anchor="<<escort->getPosition()<<" faction="<<report.faction<<" squad="<<report.squad<<" requested_max="<<report.requested<<" actual="<<report.actual<<" spawn="<<report.position<<" result="<<success<<" reason="<<report.reason;
            if(success)message<<Loc::text("debug.mission")<<currentMissionFiscalId<<"\n"<<Loc::text("debug.difficulty")<<currentContract.dangerLevel<<"\n"<<Loc::text("debug.enemies")<<report.actual<<"\n"<<Loc::text("debug.group")<<report.faction<<" / "<<report.squad;
            else message<<report.reason;
        }else{
            std::vector<Character*> candidates=debugNpcCandidates();
            int first=UtilityT::randomInt(0,(int)candidates.size()-1);Character* targets[2];targets[0]=candidates[first];candidates.erase(candidates.begin()+first);targets[1]=candidates[UtilityT::randomInt(0,(int)candidates.size()-1)];
            // Preflight BOTH before either receives damage. Two modest organic
            // wounds per actor; no severing, blood drain or fabricated mission flags.
            if(!debugNpcSafe(targets[0])||!debugNpcSafe(targets[1]))throw std::runtime_error("NPC health changed before operation");
            success=true;message<<Loc::text("debug.mission")<<currentMissionFiscalId;
            for(int n=0;n<2;++n){
                Character* c=targets[n];MedicalSystem* m=c->getMedical();log<<" npc="<<c->getHandle().toString()<<" name="<<c->getName()<<" BEFORE";logDebugHealth(log,m);
                bool wounded=false;int treatableParts=0;
                for(int i=0;i<m->getPartCount();++i){
                    MedicalSystem::HealthPartStatus* p=m->getPart((unsigned __int64)i);if(!p||p->isRobotic())continue;
                    float amount=MissionDebugRules::bluntDamage(p->maxHealth(),p->flesh,p->fleshStun);
                    float cut=treatableParts<2?MissionDebugRules::treatableCut(p->maxHealth(),p->flesh,p->fleshStun):0;
                    float before=p->flesh;Damages damage(cut,amount,0,0,0);p->applyDamage(damage);p->updateDerivedHealths();
                    if(cut>0&&p->flesh<before){++treatableParts;wounded=true;}log<<" applied_cut["<<i<<"]="<<cut<<" applied_blunt="<<amount;
                }
                m->validateHealthValues();m->knockout(1.0f);m->knockoutForceTimer(60.0f);
                // ForceTimer sets a duration; the native collapse assessment
                // consumes it and updates unconsciousness. Refresh care scores too.
                m->medicalUpdate(0.0f);m->reassessCollapseMode(false,false);
                log<<" AFTER";logDebugHealth(log,m);log<<" wounded="<<wounded<<" treatable_parts="<<treatableParts<<" first_aid="<<m->scoreFirstAidNeed(false);// The first-aid score can update on the next native tick; wounds and KO are already observable.
                bool confirmed=wounded&&treatableParts==2&&!c->isDead()&&!m->isDead()&&m->isUnconcious();success=success&&confirmed;
                message<<"\n- "<<c->getName()<<" : "<<(confirmed?"KO":Loc::text("debug.unconfirmed"));
            }
        }
    }catch(const std::exception& e){success=false;log<<" exception="<<e.what();message<<"\n"<<Loc::text("debug.unconfirmed");}
    catch(...){success=false;log<<" exception=unknown";message<<"\n"<<Loc::text("debug.unconfirmed");}
    DebugLog(log.str());
    if(missionDebugTip)missionDebugTip->setVisible(false);
    missionDebugBusy=false;missionDebugReadyAt=GetTickCount()+std::max<DWORD>(750,GetDoubleClickTime()+100);
    if(developerWindow)developerWindow->setVisible(false);
    if(ou)ou->showPlayerAMessage(std::string(Loc::text(raid?(success?"debug.raid.ok":"debug.raid.fail"):(success?"debug.ko.ok":"debug.ko.fail")))+"\n"+message.str(),true);
}
void debugPickerClose(MyGUI::Window*,const std::string&){if(missionDebugPicker)missionDebugPicker->setVisible(false);}
void debugPickerClick(MyGUI::Widget* sender){
    if(!sender||!missionDebugPicker||!missionDebugPicker->getVisible()||!clientOptions.developerMode)return;
    int slot=atoi(sender->getUserString("slot").c_str());
    if(slot<0||slot>=maximumActiveQuests)return;
    int previous=selectedEscortQuest;selectEscortQuest(slot);
    if(currentMissionFiscalId!=sender->getUserString("mission")){selectEscortQuest(previous);missionDebugPicker->setVisible(false);return;}
    missionDebugPicker->setVisible(false);executeMissionDebug(missionDebugPickerRaid);selectEscortQuest(previous);
}
void developerMissionDebug(bool raid){
    if(!clientOptions.developerMode||!debugClickReady())return;
    if(debugMissionCompatible(raid)){executeMissionDebug(raid);return;}
    std::vector<int> slots=debugMissionSlots(raid);
    if(slots.empty()){executeMissionDebug(raid);return;}
    if(slots.size()==1){int previous=selectedEscortQuest;selectEscortQuest(slots[0]);executeMissionDebug(raid);selectEscortQuest(previous);return;}
    if(missionDebugPicker){MyGUI::Gui::getInstance().destroyWidget(missionDebugPicker);missionDebugPicker=0;}
    missionDebugPickerRaid=raid;
    const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();
    missionDebugPicker=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-620)/2,(view.height-350)/2,620,350,MyGUI::Align::Default,"Window");
    MercenarieFonts::caption(missionDebugPicker,Loc::text("debug.mission"));missionDebugPicker->eventWindowButtonPressed+=MyGUI::newDelegate(debugPickerClose);
    int previous=selectedEscortQuest;
    for(size_t i=0;i<slots.size();++i){selectEscortQuest(slots[i]);MyGUI::Button* b=missionDebugPicker->getClientWidget()->createWidget<MyGUI::Button>("Kenshi_Button1",15,15+(int)i*48,570,40,MyGUI::Align::Default);MercenarieFonts::caption(b,currentMissionFiscalId+" / "+originCity);b->setUserString("slot",registerNumber(slots[i]));b->setUserString("mission",currentMissionFiscalId);b->eventMouseButtonClick+=MyGUI::newDelegate(debugPickerClick);}
    selectEscortQuest(previous);if(developerWindow)developerWindow->setVisible(false);
}
}
