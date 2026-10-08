// Appended after the complete legacy payload. Old files migrate in memory.
if(!a.reading||a.cursor<a.bytes.size()){
    std::vector<SaveSections::Section> sections;
    if(!a.reading){
        MissionArchive meta;std::string producer=MercenarieSaveBuild;meta.field(producer);
        sections.push_back(SaveSections::Section("producer",1,meta.bytes));
        MissionArchive profiles;unsigned int count=(unsigned int)guildVisitors.size();profiles.field(count);
        for(size_t i=0;i<guildVisitors.size();++i){std::string key=guildVisitors[i].offerKey;int profile=guildVisitors[i].profile;if(profile<0||profile>6)throw std::runtime_error("invalid visitor profile");profiles.field(key);profiles.field(profile);}
        sections.push_back(SaveSections::Section("visitor-profiles",1,profiles.bytes));
        MissionArchive policy;policy.field(importedDomainsSuspended);
        sections.push_back(SaveSections::Section("import-policy",1,policy.bytes));
    }
    if(!a.reading){MissionArchive recovery;recovery.field(importedArtisans);recovery.field(importedArtisanHour);sections.push_back(SaveSections::Section("import-recovery",1,recovery.bytes));}
#ifdef MERCENARIE_CARAVAN_CUSTOMERS
    if(!a.reading){CaravanCustomers::load(caravanCustomerState);MissionArchive customers;customers.field(caravanCustomerState);sections.push_back(SaveSections::Section("caravan-customers",2,customers.bytes));}
    else caravanCustomerState.clear();
#endif
    SaveSections::archive(a,sections);
    if(a.reading){
        bool producer=false,profiles=false,policy=false,recovery=false;
        for(size_t i=0;i<sections.size();++i){const SaveSections::Section& section=sections[i];
            if(section.version!=1
#ifdef MERCENARIE_CARAVAN_CUSTOMERS
                && !(section.id=="caravan-customers"&&section.version==2)
#endif
            )throw std::runtime_error("unsupported section version: "+section.id);
            MissionArchive data(section.bytes);
            if(section.id=="producer"){std::string build;data.field(build);if(build.empty()||build.size()>128)throw std::runtime_error("invalid producer build");producer=true;}
            else if(section.id=="visitor-profiles"){
                unsigned int count=0;data.field(count);if(count!=guildVisitors.size())throw std::runtime_error("visitor profile count mismatch");
                for(size_t j=0;j<guildVisitors.size();++j){std::string key;int profile=0;data.field(key);data.field(profile);if(key!=guildVisitors[j].offerKey||profile<0||profile>6)throw std::runtime_error("invalid visitor profile identity");guildVisitors[j].profile=profile;}profiles=true;
            }else if(section.id=="import-policy"){data.field(importedDomainsSuspended);policy=true;}
            else if(section.id=="import-recovery"){data.field(importedArtisans);data.field(importedArtisanHour);if(!(importedArtisanHour>=-1&&importedArtisanHour<=1e9))throw std::runtime_error("invalid recovery clock");std::set<std::string> unique;for(size_t k=0;k<importedArtisans.size();++k)if(importedArtisans[k].empty()||!unique.insert(importedArtisans[k]).second)throw std::runtime_error("invalid recovery identities");recovery=true;}
#ifdef MERCENARIE_CARAVAN_CUSTOMERS
            else if(section.id=="caravan-customers"){data.field(caravanCustomerState);CaravanCustomers::load(caravanCustomerState);}
#endif
            else throw std::runtime_error("unsupported save section: "+section.id);
            data.finish();
        }
        if(!recovery){importedArtisans=importedDomainsSuspended?ImportRecovery::artisanKeys(artisanLedger):std::vector<std::string>();importedArtisanHour=-1;}
        if(!producer||!profiles||!policy)throw std::runtime_error("missing required save section");
    }
}else {importedDomainsSuspended=false;importedArtisans.clear();importedArtisanHour=-1;}
