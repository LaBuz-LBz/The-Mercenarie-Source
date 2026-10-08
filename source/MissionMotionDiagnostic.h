#pragma once
// Read-only bounded tracing for the native stop/start reproduction.
namespace {
bool missionMotionDiagnosticEnabled=false;
struct MissionMotionSample {unsigned long last;unsigned int samples,halts;MissionMotionSample():last(0),samples(0),halts(0){}};
MissionMotionSample missionMotionSamples[maximumActiveQuests];
void missionMotionSample(){
    if(!missionMotionDiagnosticEnabled||missionWorldChanging||missionRestorePending||!missionActive||!escort||!escort->getMovement()||selectedEscortQuest<0||selectedEscortQuest>=maximumActiveQuests)return;
    MissionMotionSample& state=missionMotionSamples[selectedEscortQuest];
    const unsigned long now=GetTickCount();
    if(state.samples>=600||now-state.last<100)return;
    state.last=now;++state.samples;
    AbstractMovementBase* movement=escort->getMovement();
    std::ostringstream s;s<<"TM-MOTION id="<<currentMissionFiscalId<<" sample="<<state.samples<<" ms="<<now
        <<" pos="<<escort->getPosition()<<" speed="<<movement->currentSpeed<<" desired="<<movement->desiredSpeed
        <<" motion="<<movement->currentMotion<<" moving="<<movement->currentlyMoving<<" stopped="<<movement->officiallyStopped
        <<" visible_update="<<escort->isVisibleUpdateMode<<" offscreen_dt="<<escort->offscreenFrameTime
        <<" goal="<<(escort->getAI()&&escort->getAI()->getTaskSystem()?(int)escort->getAI()->getTaskSystem()->getCurrentGoal().key():-1)
        <<" target="<<movement->getDestination()<<" path="<<movement->pathDestination<<" path_failed="<<movement->pathFailed()
        <<" road_index="<<missionRescue.roadNext<<" regroup="<<missionRescue.regrouping<<" rescue="<<missionCasualtyWaiting<<" paused="<<missionPaused;
    Tasker* action=escort->getBody()?escort->getBody()->currentAction:0;
    if(action)s<<" body_action="<<(int)action->key()<<" body_target="<<action->location;
    DebugLog(s.str());
}
void (*missionMotionHaltOriginal)(CharMovement*);
void missionMotionHaltHook(CharMovement* movement){
    if(missionMotionDiagnosticEnabled&&!missionWorldChanging&&!missionRestorePending&&movement){
        for(int slot=0;slot<maximumActiveQuests;++slot){
            const bool selected=slot==selectedEscortQuest;
            if(!(selected?missionActive:escortQuests[slot].v_missionActive))continue;
            Character* leader=(selected?escortHandle:escortQuests[slot].v_escortHandle).getCharacter();
            if(!leader||leader->getMovement()!=movement)continue;
            MissionMotionSample& state=missionMotionSamples[slot];
            if(state.halts>=80)break;
            ++state.halts;
            void* frames[12]={0};USHORT count=CaptureStackBackTrace(0,12,frames,0);
            std::ostringstream s;s<<"TM-MOTION HALT id="<<(selected?currentMissionFiscalId:escortQuests[slot].v_currentMissionFiscalId)<<" pos="<<leader->getPosition()<<" speed="<<movement->currentSpeed<<" stack=";
            for(USHORT i=0;i<count;++i){HMODULE module=0;char name[MAX_PATH]={0};
                if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(frames[i]),&module)){
                    GetModuleFileNameA(module,name,MAX_PATH);const char* base=strrchr(name,'\\');
                    s<<(base?base+1:name)<<"+"<<std::hex<<(reinterpret_cast<size_t>(frames[i])-reinterpret_cast<size_t>(module))<<",";
                }
            }
            DebugLog(s.str());break;
        }
    }
    missionMotionHaltOriginal(movement);
}
}
