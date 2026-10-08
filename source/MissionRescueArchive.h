
    if(!a.reading||a.cursor<a.bytes.size()){
        for(int i=0;i<maximumActiveQuests;++i){
            if(i==selectedEscortQuest){a.field(missionRescue.retiredLeaders);a.field(missionPace);a.field(missionForcedPace);}
            else {a.field(escortQuests[i].v_missionRescue.retiredLeaders);a.field(escortQuests[i].v_missionPace);a.field(escortQuests[i].v_missionForcedPace);}
        }
    }
    else if(a.reading){missionRescue.retiredLeaders.clear();for(int i=0;i<maximumActiveQuests;++i){escortQuests[i].v_missionRescue=MissionRescueState();escortQuests[i].v_missionPace=EscortPace::Normal;escortQuests[i].v_missionForcedPace=false;}}
    if(!EscortPace::valid((int)missionPace))throw std::runtime_error("invalid rescue pace");
    for(int i=0;i<maximumActiveQuests;++i)if(!EscortPace::valid((int)escortQuests[i].v_missionPace))throw std::runtime_error("invalid quest rescue pace");
    if(a.reading){missionRescue.resetMotion();missionRescue.recoveryPending=false;missionRescue.roadPlanned=false;missionRescue.roadPoints.clear();missionRescue.roadNext=0;missionRescue.holds.clear();missionRescue.regrouping=false;missionRescue.lastSpeed=-1;missionRescue.tasks.clear();missionRescue.safeSeconds=0;missionCasualtyWaiting=missionActive;missionTemporaryLeader=0;
        for(int i=0;i<maximumActiveQuests;++i){escortQuests[i].v_missionRescue.resetMotion();escortQuests[i].v_missionRescue.recoveryPending=false;escortQuests[i].v_missionRescue.roadPlanned=false;escortQuests[i].v_missionRescue.roadPoints.clear();escortQuests[i].v_missionRescue.roadNext=0;escortQuests[i].v_missionRescue.holds.clear();escortQuests[i].v_missionRescue.tasks.clear();escortQuests[i].v_missionRescue.safeSeconds=0;escortQuests[i].v_missionCasualtyWaiting=escortQuests[i].v_missionActive;escortQuests[i].v_missionTemporaryLeader=0;}}
