#pragma once
#include <cstring>
#include <float.h>
// The SDK's ZoneManager::getBiome returns BIOMES (terrain), not BIOME_GROUP.
// Resolve the same AreasList query used by the native region-name HUD through
// an exported method. No fixed executable RVA, writes, or camera dependency.
namespace NativeRegionLookup {
typedef void* (*Query)(void*,const Ogre::Vector3&);
struct Binding {void** manager;Query query;Binding():manager(0),query(0){} };
inline const unsigned char* jump(const unsigned char* p){
    for(int i=0;i<4&&p&&p[0]==0xe9;++i){int d;memcpy(&d,p+1,4);p+=5+d;}return p;
}
inline bool decode(const unsigned char* entry,Binding& out){
    out=Binding();const unsigned char* p=jump(entry);if(!p)return false;
    // sub rsp,28; mov rcx,[rip+manager]; call query; mov rax,[rax+98];
    // add rax,90; add rsp,28; ret. Displacements are relocatable.
    const unsigned char head[]={0x48,0x83,0xec,0x28,0x48,0x8b,0x0d};
    const unsigned char tail[]={0x48,0x8b,0x80,0x98,0,0,0,0x48,0x05,0x90,0,0,0,0x48,0x83,0xc4,0x28,0xc3};
    if(memcmp(p,head,7)||p[11]!=0xe8||memcmp(p+16,tail,sizeof(tail)))return false;
    int global,call;memcpy(&global,p+7,4);memcpy(&call,p+12,4);
    const unsigned char* q=jump(p+16+call);
    const unsigned char qhead[]={0x48,0x83,0xec,0x28,0xe8};
    const unsigned char qtail[]={0x48,0x85,0xc0,0x74,0x09,0x48,0x8b,0x40,0x28,0x48,0x83,0xc4,0x28,0xc3,0x48,0x83,0xc4,0x28,0xc3};
    if(!q||memcmp(q,qhead,5)||memcmp(q+9,qtail,sizeof(qtail)))return false;
    out.manager=(void**)(p+11+global);out.query=(Query)q;return true;
}
inline bool bind(Binding& out){
    __try {HMODULE sdk=GetModuleHandleA("KenshiLib.dll");
        void* exported=(void*)(sdk?GetProcAddress(sdk,"?getPositionGlobalEffects@WeatherSystem@@QEBAAEBV?$vector@U?$pair@W4Enum@EffectType@@M@std@@V?$STLAllocator@U?$pair@W4Enum@EffectType@@M@std@@V?$CategorisedAllocPolicy@$0A@@Ogre@@@Ogre@@@std@@AEBVVector3@Ogre@@@Z"):0);
        return exported&&decode((const unsigned char*)KenshiLib::GetRealAddress(exported),out);}
    __except(EXCEPTION_EXECUTE_HANDLER){out=Binding();return false;}
}
inline GameData* read(const Binding& b,const Ogre::Vector3& position){
    __try {
        if(!b.manager||!b.query||!*b.manager)return 0;
        void* area=b.query(*b.manager,position);if(!area)return 0;
        // Verified against MainBarGUI::_NV_update: AreaBiomeGroup + 0x10.
        GameData* data=*(GameData**)((unsigned char*)area+0x10);
        return data&&data->type==BIOME_GROUP?data:0;
    }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
inline GameData* at(const Ogre::Vector3& p){
    if(!_finite(p.x)||!_finite(p.y)||!_finite(p.z))return 0;
    static Binding binding;static bool tried=false;
    if(!tried){tried=true;if(!bind(binding))ErrorLog("REGION LOOKUP unavailable: unsupported native signature; terrain fallback disabled");}
    return read(binding,p);
}
}
