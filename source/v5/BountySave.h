#pragma once
#include "BountyContracts.h"
#include "../MissionArchive.h"
#include <map>

namespace MercenarieV5 {
// A separate payload, embedded by the runtime save adapter, never raw engine pointers.
struct BountyWorldState {
    BountyContract contract;
    std::map<std::string,BountyBoard> boards;
    bool suspended;
    bool targetSpotted;
    bool campStarted;
    double searchSeconds;
    unsigned int searchAttempts;
    int encounter; // 0 undecided, 1 camp, 2 ambush
    bool ambushAnnounced;
    double missionSeconds;
    bool secretChosen;
    float secretX,secretY,secretZ;
    std::vector<ActorIdentity> campObjects;
    std::vector<ActorIdentity> campCleanup;
    BountyWorldState():suspended(false),targetSpotted(false),campStarted(false),searchSeconds(0),searchAttempts(0),encounter(0),ambushAnnounced(false),missionSeconds(0),secretChosen(false),secretX(0),secretY(0),secretZ(0){}
    std::string save(){
        if(suspended)throw std::runtime_error("bounty state suspended");
        MissionArchive a;std::string magic="MERCENARIE-BOUNTY-V5-6";a.field(magic);
        contract.archive(a);unsigned int count=(unsigned int)boards.size();
        if(count>1024)throw std::runtime_error("too many bounty boards");a.field(count);
        for(std::map<std::string,BountyBoard>::iterator i=boards.begin();i!=boards.end();++i){std::string key=i->first;a.field(key);i->second.archive(a);}
        a.field(targetSpotted);
        a.field(campStarted);
        archiveCamp(a,campObjects);archiveCamp(a,campCleanup);
        a.field(searchSeconds);a.field(searchAttempts);
        a.field(encounter);a.field(ambushAnnounced);
        a.field(missionSeconds);a.field(secretChosen);a.field(secretX);a.field(secretY);a.field(secretZ);
        MissionArchive envelope;envelope.field(a.bytes);unsigned int checksum=missionChecksum(a.bytes);envelope.field(checksum);BountyWorldState checked;checked.load(envelope.bytes);return envelope.bytes;
    }
    void load(const std::string& bytes){
        // Decode into a fresh world: malformed data never partially overwrites live state.
        BountyWorldState decoded;
        try{
            MissionArchive envelope(bytes);std::string payload;unsigned int checksum=0;
            envelope.field(payload);envelope.field(checksum);envelope.finish();
            if(missionChecksum(payload)!=checksum)throw std::runtime_error("bounty checksum mismatch");
            MissionArchive a(payload);std::string magic;a.field(magic);
            const bool v6=magic=="MERCENARIE-BOUNTY-V5-6";
            if(magic!="MERCENARIE-BOUNTY-V5-1"&&magic!="MERCENARIE-BOUNTY-V5-2"&&magic!="MERCENARIE-BOUNTY-V5-3"&&magic!="MERCENARIE-BOUNTY-V5-4"&&magic!="MERCENARIE-BOUNTY-V5-5"&&!v6)throw std::runtime_error("unsupported bounty save");
            decoded.contract.archive(a);unsigned int count=0;a.field(count);
            if(count>1024)throw std::runtime_error("too many bounty boards");
            for(unsigned int i=0;i<count;++i){std::string key;a.field(key);BountyBoard board;board.archive(a);
                if(key.empty()||!decoded.boards.insert(std::make_pair(key,board)).second)throw std::runtime_error("duplicate bounty board");
            }
            if(magic!="MERCENARIE-BOUNTY-V5-1")a.field(decoded.targetSpotted);
            if(magic=="MERCENARIE-BOUNTY-V5-3"||magic=="MERCENARIE-BOUNTY-V5-4"||magic=="MERCENARIE-BOUNTY-V5-5"||v6){
                a.field(decoded.campStarted);archiveCamp(a,decoded.campObjects);archiveCamp(a,decoded.campCleanup);
            }
            if(magic=="MERCENARIE-BOUNTY-V5-4"||magic=="MERCENARIE-BOUNTY-V5-5"||v6){a.field(decoded.searchSeconds);a.field(decoded.searchAttempts);}
            if(magic=="MERCENARIE-BOUNTY-V5-5"||v6){a.field(decoded.encounter);a.field(decoded.ambushAnnounced);}
            else if(decoded.contract.state!=BountySpawning)decoded.encounter=1;
            if(v6){a.field(decoded.missionSeconds);a.field(decoded.secretChosen);a.field(decoded.secretX);a.field(decoded.secretY);a.field(decoded.secretZ);}
            else {
                // Preserve old saved coordinates; never roll a new site on load.
                decoded.secretChosen=decoded.contract.occupied();
                decoded.secretX=decoded.contract.offer.x;decoded.secretY=decoded.contract.offer.y;decoded.secretZ=decoded.contract.offer.z;
            }
            if(!(decoded.missionSeconds>=0)||!(decoded.searchSeconds>=0))throw std::runtime_error("invalid bounty timer");
            if(decoded.encounter<0||decoded.encounter>2)throw std::runtime_error("invalid bounty encounter");
            a.finish();
        }catch(...){suspended=true;throw;}
        *this=decoded;
    }
    template<class Archive> static void archiveCamp(Archive& a,std::vector<ActorIdentity>& objects){
        unsigned int n=(unsigned int)objects.size();a.field(n);
        if(n>256)throw std::runtime_error("invalid camp object count");
        if(a.reading)objects.resize(n);
        for(unsigned int i=0;i<n;++i)objects[i].archive(a);
    }
};
}
