#pragma once
// Export signature verified in KenshiLib and native follow caller. Offset is
// leader-local (+X forward, -Z right); the native task rotates it and controls speed/path prediction.
const Ogre::Vector3& (*caravanOffsetOriginal)(Blackboard*,Character*)=0;
const Ogre::Vector3& caravanOffsetHook(Blackboard* board,Character* actor){
    if(!MercenarieCleanup::disabled&&!missionWorldChanging&&!missionRestorePending&&caravanNativeFormationReady){
        const Ogre::Vector3* staff=personnelNativeOffset(actor);if(staff)return *staff;
        for(int i=0;i<maximumActiveQuests;++i){const bool selected=i==selectedEscortQuest;
            if(!(selected?missionActive:escortQuests[i].v_missionActive)||!(selected?caravanMission:escortQuests[i].v_caravanMission))continue;
            CaravanFormationController& controller=selected?missionGroup.caravan:escortQuests[i].v_missionGroup.caravan;
            const Ogre::Vector3* offset=controller.offsetFor(actor);
            if(offset){static bool logged=false;if(!logged){DebugLog("CARAVAN NATIVE FOLLOW offset hook consumed by engine");logged=true;}return *offset;}
        }
    }
    return caravanOffsetOriginal(board,actor);
}
// Native follow only selects its rotated formation path for same-platoon
// actors. Assigned player escorts belong to a different platoon. Opt only
// their owned FOLLOW order into that path, without changing either platoon.
bool (*personnelFormationGroupOriginal)(void*,Character*,Character**)=0;
bool personnelFormationGroupHook(void* task,Character* actor,Character** target){
    if(!MercenarieCleanup::disabled&&!missionWorldChanging&&!missionRestorePending&&caravanNativeFormationReady&&target&&*target){
        int slot=personnelAssignment(actor);
        if(slot>=0&&personnelLeader(slot)==*target&&personnelNativeOffset(actor)){
            static bool logged=false;if(!logged){DebugLog("PERSONNEL NATIVE FOLLOW cross-platoon formation consumed");logged=true;}
            return true;
        }
    }
    return personnelFormationGroupOriginal(task,actor,target);
}
void installPersonnelFormationGroup(){
    // Unique instruction signature verified against the installed executable.
    // Do not patch an unknown engine build or accept ambiguous matches.
    static const unsigned char signature[]={0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x4d,0x8b,0x08,0x49,0x8b,0xd8,0x48,0x8b,0xfa,0x48,0x8d,0x0d};
    static const unsigned char tail[]={0x4d,0x85,0xc9,0x74,0x04,0x49,0x8d,0x49,0x58,0x8b,0x82,0x34,0x03,0x00,0x00,0x8b,0x92,0x38,0x03,0x00,0x00,0x3b,0x41,0x0c};
    unsigned char* base=reinterpret_cast<unsigned char*>(GetModuleHandleW(0));if(!base)return;
    IMAGE_DOS_HEADER* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);if(dos->e_magic!=IMAGE_DOS_SIGNATURE)return;
    IMAGE_NT_HEADERS* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+dos->e_lfanew);if(nt->Signature!=IMAGE_NT_SIGNATURE)return;
    IMAGE_SECTION_HEADER* sections=IMAGE_FIRST_SECTION(nt);unsigned char* found=0;int matches=0;
    for(unsigned int i=0;i<nt->FileHeader.NumberOfSections;++i){if(!(sections[i].Characteristics&IMAGE_SCN_MEM_EXECUTE))continue;
        unsigned char* start=base+sections[i].VirtualAddress;size_t size=sections[i].Misc.VirtualSize;
        for(size_t j=0;j+26+sizeof(tail)<=size;++j)if(!memcmp(start+j,signature,sizeof(signature))&&!memcmp(start+j+26,tail,sizeof(tail))){found=start+j;++matches;}
    }
    if(matches==1&&KenshiLib::SUCCESS==KenshiLib::AddHook(found,reinterpret_cast<void*>(&personnelFormationGroupHook),reinterpret_cast<void**>(&personnelFormationGroupOriginal)))DebugLog("PERSONNEL NATIVE FOLLOW cross-platoon formation hook installed");
    else ErrorLog("PERSONNEL NATIVE FOLLOW cross-platoon hook unavailable; regular follow retained");
}
void installCaravanNativeFollow(){
    HMODULE library=GetModuleHandleW(L"KenshiLib.dll");
    FARPROC symbol=library?GetProcAddress(library,"?getFormationPositionOffset@Blackboard@@QEAAAEBVVector3@Ogre@@PEAVCharacter@@@Z"):0;
    if(!symbol){ErrorLog("CARAVAN NATIVE FOLLOW export missing; regular follow retained");return;}
    intptr_t address=KenshiLib::GetRealAddress(reinterpret_cast<void*>(symbol));
    if(address&&KenshiLib::SUCCESS==KenshiLib::AddHook(reinterpret_cast<void*>(address),reinterpret_cast<void*>(&caravanOffsetHook),reinterpret_cast<void**>(&caravanOffsetOriginal))){caravanNativeFormationReady=true;installPersonnelFormationGroup();DebugLog("CARAVAN NATIVE FOLLOW offset hook installed");}
    else ErrorLog("CARAVAN NATIVE FOLLOW hook unavailable; regular follow retained");
}
