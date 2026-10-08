// Included after the runtime persistence primitives, and by their test double.
// These are working-session files. Player save slots are published by Lot 1.
bool persistGuildInvestment(const std::string& fiscal,const std::string& progress){
    const std::string oldFiscal=readMissionFile(fiscalFile);
    bool fiscalWritten=false;
    try {
        if(!atomicMissionFile(fiscalFile,fiscal))return false;
        fiscalWritten=true;
        if(atomicMissionFile(reputationFile,progress))return true;
    }catch(...){
        // The atomic file primitive never changes its target on a failed write.
        // Only the first successful write therefore needs compensation.
    }
    if(fiscalWritten){
        try {if(!atomicMissionFile(fiscalFile,oldFiscal))progressWriteBlocked=true;}
        catch(...){progressWriteBlocked=true;}
    }
    return false;
}
