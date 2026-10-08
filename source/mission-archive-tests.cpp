#include "MissionReportState.h"
#include "MissionArchive.h"
#include "GuildProgression.h"
#include "GuildSavePaths.h"
#include <cassert>
#include <iostream>
#include <sstream>

enum Phase { MEETING, ACTIVE, WAITING, FOLLOWING, REPORT, REFUSED };
int main(){
    {
        std::ostringstream unique;unique<<UnicodeFileSystem::temporaryDirectory()<<"GEC-Unicode-"<<GetCurrentProcessId()<<"-"<<GetTickCount();
        const std::string root=unique.str();
        const char* names[]={"ASCII-save","Fran\xc3\xa7""ais-\xc3\xa9t\xc3\xa9","\xe4\xb8\xad\xe6\x96\x87-\xe5\xad\x98\xe6\xa1\xa3","emoji-\xf0\x9f\xa7\xaa"};
        for(size_t i=0;i<sizeof(names)/sizeof(names[0]);++i){
            const std::string directory=root+"/"+names[i]+"/TheMercenarie";
            assert(GuildSavePaths::ensureDirectory(directory));
            const std::string file=directory+"/Mission.v4",copy=directory+"/GuildEscortReputation.dat",moved=directory+"/GuildEscortBoards.dat";
            const std::string payload=std::string("payload-")+names[i];assert(UnicodeFileSystem::writeFile(file,payload));
            std::string restored;assert(UnicodeFileSystem::readFile(file,restored,1048576)==UnicodeFileSystem::ReadOk&&restored==payload);
            assert(UnicodeFileSystem::copyFile(file,copy,false));assert(UnicodeFileSystem::moveReplace(copy,moved));
            restored.clear();assert(UnicodeFileSystem::readFile(moved,restored,1048576)==UnicodeFileSystem::ReadOk&&restored==payload);
            assert(UnicodeFileSystem::removeFile(file)&&UnicodeFileSystem::removeFile(moved));
            assert(UnicodeFileSystem::removeDirectory(directory));assert(UnicodeFileSystem::removeDirectory(root+"/"+names[i]));
        }
        const std::string legacy=root+"/legacy-\xc3\xa9";assert(GuildSavePaths::ensureDirectory(root));
        assert(CreateDirectoryA(legacy.c_str(),0)||GetLastError()==ERROR_ALREADY_EXISTS);
        assert(UnicodeFileSystem::attributes(legacy)!=INVALID_FILE_ATTRIBUTES);
        assert(UnicodeFileSystem::removeDirectory(legacy));assert(UnicodeFileSystem::removeDirectory(root));
    }
    {
        MissionArchive out;std::vector<int> roster;roster.push_back(123);roster.push_back(456);roster.push_back(789);
        bool known=true,ko=true;int local=1,global=0;std::set<unsigned int> losses;losses.insert(1);
        GuildProgression::archiveOutcome(out,roster,known,ko,local,global,losses);
        MissionArchive in(openMissionArchive(sealMissionArchive(out.bytes)));roster.clear();losses.clear();known=ko=false;local=global=999;
        GuildProgression::archiveOutcome(in,roster,known,ko,local,global,losses);in.finish();
        assert(roster.size()==3&&roster[1]==456&&known&&ko&&local==1&&global==0&&losses.count(1));
        MissionArchive second;GuildProgression::archiveOutcome(second,roster,known,ko,local,global,losses);assert(second.bytes==out.bytes);
    }
    for(int phase=MEETING;phase<=REFUSED;++phase){
        MissionArchive out;int state=phase,paid=850;bool settled=false;float research=123.5f;
        std::string target="soldier-2|stable-id",label="Client\navec caracteres UTF-8: \xc3\xa9";
        std::vector<int> group;group.push_back(11);group.push_back(22);group.push_back(33);
        out.field(state);out.field(paid);out.field(settled);out.field(research);out.field(target);out.field(label);out.field(group);
        const std::string sealed=sealMissionArchive(out.bytes);
        MissionArchive in(openMissionArchive(sealed));int s=0,p=0;bool done=true;float r=0;std::string t,l;std::vector<int> g;
        in.field(s);in.field(p);in.field(done);in.field(r);in.field(t);in.field(l);in.field(g);in.finish();
        assert(s==phase&&p==850&&!done&&r==research&&t==target&&l==label&&g==group);
        for(size_t i=0;i<sealed.size();++i){bool rejected=false;try{openMissionArchive(sealed.substr(0,i));}catch(const std::exception&){rejected=true;}assert(rejected);}
        std::string damaged=sealed;damaged[damaged.size()-5]^=0x10;bool rejected=false;
        try{openMissionArchive(damaged);}catch(const std::exception&){rejected=true;}assert(rejected);
        rejected=false;try{openMissionArchive(sealed+"garbage");}catch(const std::exception&){rejected=true;}assert(rejected);
    }
    bool rejected=false;try{MissionArchive in(std::string(4,'\xff'));std::string value;in.field(value);}catch(const std::exception&){rejected=true;}assert(rejected);
    rejected=false;try{MissionArchive in(std::string(1,'\x02'));bool value=false;in.field(value);}catch(const std::exception&){rejected=true;}assert(rejected);
    {MissionArchive out;double start=12.5,end=98.5;archiveReportTime(out,start,end);
     MissionArchive in(out.bytes);double a=0,b=0;archiveReportTime(in,a,b);in.finish();assert(a==12.5&&b==98.5);
     MissionArchive legacy((std::string()));archiveReportTime(legacy,a,b);legacy.finish();assert(a==-1&&b==-1);
     for(size_t n=1;n<out.bytes.size();++n){bool bad=false;try{MissionArchive cut(out.bytes.substr(0,n));archiveReportTime(cut,a,b);}catch(...){bad=true;}assert(bad);}}
    std::cout<<"PASS: Unicode filesystem ASCII/French/Chinese/emoji and ANSI fallback; report time/legacy/truncation; 6 phase round-trips, amounts, flags, target, group, UTF-8, checksum and bounds.\n";
}
