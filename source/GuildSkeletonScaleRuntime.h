#pragma once
#include <intrin.h>
#include <kenshi/Appearance.h>
#include <ogre/OgreOldBone.h>
#include <ogre/OgreSkeleton.h>

#pragma intrinsic(_ReturnAddress)

// AppearanceHuman::updateProportions rejects every skeleton whose bone count
// differs from 30. Allow our verified 30 + 15 finger rig at that one call only.
// In particular, never hide fingers from mesh skinning, animation or portraits.
namespace GuildSkeletonScale {
typedef unsigned short (*BoneCount)(const Ogre::Skeleton*);
static BoneCount originalBoneCount=0;
static const void* proportionsCountReturn=0;
static volatile LONG reported=0;

inline bool supportedRig(const Ogre::Skeleton* skeleton) {
    const std::string& path=skeleton->getName();
    const size_t slash=path.find_last_of("/\\");
    const std::string name=path.substr(slash==std::string::npos?0:slash+1);
    if(name!="human_male_fingers.skeleton"&&name!="human_female_fingers.skeleton")return false;
    const bool female=name=="human_female_fingers.skeleton";
    static const char* const names[]={
        "Bip01","Bip01 Pelvis","Bip01 L Thigh","Bip01 L Calf","Bip01 L Foot",
        "Bip01 L Toe0","Bip01 L Toe0Nub","Bip01 R Thigh","Bip01 R Calf","Bip01 R Foot",
        "Bip01 R Toe0","Bip01 R Toe0Nub","Bip01 Spine","Bip01 Spine1","Bip01 Spine2",
        "Bip01 L Clavicle","Bip01 L UpperArm","Bip01 L Forearm","Bip01 L Hand","Bip01 Prop1",
        "Bip01 Neck","Bip01 Head","Bip01 HeadNub","Bip01 Jaw","Bip01 JawNub",
        "Bip01 R Clavicle","Bip01 R UpperArm","Bip01 R Forearm","Bip01 R Hand","Bip01 Prop2",
        "Prototype R Little1","Prototype R Little2","Prototype R Little3",
        "Prototype R Ring1","Prototype R Ring2","Prototype R Ring3",
        "Prototype R Middle1","Prototype R Middle2","Prototype R Middle3",
        "Prototype R Index1","Prototype R Index2","Prototype R Index3",
        "Prototype R Thumb1","Prototype R Thumb2","Prototype R Thumb3"
    };
    static const int parents[]={-1,0,1,2,3,4,5,1,7,8,9,10,1,12,13,14,15,16,17,18,
        14,20,21,21,23,14,25,26,27,28,28,30,31,28,33,34,28,36,37,28,39,40,28,42,43};
    static const char* const femaleTail[]={"Bip01 R Clavicle","Bip01 R UpperArm",
        "Bip01 R Forearm","Bip01 R Hand","Bip01 Prop2","L Boob","R Boob"};
    static const int femaleParents[]={14,23,24,25,26,14,14};
    for(unsigned short i=0;i<45;++i){
        const char* expectedName=female&&i>=23&&i<30?femaleTail[i-23]:names[i];
        int expectedParent=female&&i>=23&&i<30?femaleParents[i-23]:parents[i];
        if(female&&i>=30&&(i-30)%3==0)expectedParent=26;
        const Ogre::OldBone* bone=skeleton->getBone(i);
        if(!bone||bone->getName()!=expectedName)return false;
        if(expectedParent<0){if(bone->getParent())return false;}
        else if(bone->getParent()!=skeleton->getBone(static_cast<unsigned short>(expectedParent)))return false;
    }
    return true;
}

__declspec(noinline) unsigned short boneCountHook(const Ogre::Skeleton* skeleton) {
    const void* caller=_ReturnAddress();
    const unsigned short count=originalBoneCount(skeleton);
    if(count!=45||caller!=proportionsCountReturn)return count;
    try {
        if(!supportedRig(skeleton))return count;
        if(InterlockedExchange(&reported,1)==0)
            DebugLog("Mercenarie skeleton scaling: verified articulated rig uses vanilla human proportions; actual bones=45");
        return 30;
    } catch(...) {return count;}
}

// The signature includes the getNumBones virtual call and the exact ==30 gate.
// Unsupported builds or another mod's changed gate leave the engine untouched.
inline const unsigned char* findCountGate(const unsigned char* code,size_t length) {
    static const unsigned char signature[]={0x48,0x8b,0xf8,0x48,0x8b,0x10,0x48,0x8b,0xc8,
        0xff,0x92,0x88,0x01,0x00,0x00,0x66,0x83,0xf8,0x1e,0x0f,0x85};
    const unsigned char* match=0;
    for(size_t i=0;i+sizeof(signature)+4<=length;++i){
        if(memcmp(code+i,signature,sizeof(signature))!=0)continue;
        int displacement=0;memcpy(&displacement,code+i+sizeof(signature),4);
        if(displacement<=0||displacement>0x4000)return 0;
        if(match)return 0;
        match=code+i+15; // return address of the virtual call, before cmp ax,30
    }
    return match;
}

inline void install() {
    if(originalBoneCount)return;
    const intptr_t address=KenshiLib::GetRealAddress(&AppearanceHuman::updateProportions);
    MEMORY_BASIC_INFORMATION region={0};
    const unsigned char* code=reinterpret_cast<const unsigned char*>(address);
    if(!code||!VirtualQuery(code,&region,sizeof(region))||region.State!=MEM_COMMIT||
       (region.Protect&(PAGE_GUARD|PAGE_NOACCESS))||
       !(region.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY))||
       reinterpret_cast<const unsigned char*>(region.BaseAddress)+region.RegionSize-code<0x400){
        ErrorLog("Mercenarie skeleton scaling: proportions function unavailable; compatibility hook not installed");return;
    }
    proportionsCountReturn=findCountGate(code,0x400);
    HMODULE ogre=GetModuleHandleA("OgreMain_x64.dll");
    FARPROC target=ogre?GetProcAddress(ogre,"?getNumBones@Skeleton@Ogre@@UEBAGXZ"):0;
    if(!proportionsCountReturn||!target){
        ErrorLog("Mercenarie skeleton scaling: unrecognised native count gate; compatibility hook not installed");return;
    }
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(target,&boneCountHook,&originalBoneCount)){
        proportionsCountReturn=0;
        ErrorLog("Mercenarie skeleton scaling: failed to install scoped compatibility hook");return;
    }
    DebugLog("Mercenarie skeleton scaling: installed scoped vanilla proportions compatibility");
}
}
