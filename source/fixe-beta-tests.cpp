#include "BetaFixRules.h"
#include <cassert>
#include <iostream>
// A walk-in client offers work before missionPending becomes true. Filtering
// every reply at that point prevents the native conversation from opening.
struct VisitorDialogueRegression {
    VisitorDialogueRegression() {
        const char* introductions[]={"880027-Guild Escort Contracts.mod","880035-Guild Escort Contracts.mod","880045-Guild Escort Contracts.mod"};
        for(int pending=0;pending<2;++pending)for(int active=0;active<2;++active){
            for(int i=0;i<3;++i)
                assert(BetaFixRules::missionChoiceVisible(true,pending!=0,active!=0,false,false,introductions[i]));
            assert(BetaFixRules::missionChoiceVisible(true,pending!=0,active!=0,false,false,"184-gamedata.quack"));
            assert(!BetaFixRules::missionChoiceVisible(true,pending!=0,active!=0,false,false,"880060-Guild Escort Contracts.mod"));
            assert(!BetaFixRules::missionChoiceVisible(true,pending!=0,active!=0,false,false,"880061-Guild Escort Contracts.mod"));
            assert(!BetaFixRules::missionChoiceVisible(true,pending!=0,active!=0,false,false,"880062-Guild Escort Contracts.mod"));
        }
        assert(BetaFixRules::guildVisitorOfferDialogue(true));
        assert(BetaFixRules::showGuildVisitorOffer(true,"mercenarie.guildvisitor.show"));
        assert(!BetaFixRules::showGuildVisitorOffer(false,"mercenarie.guildvisitor.show"));
        assert(BetaFixRules::rejectWaitingVisitor(true,"mercenarie.guildvisitor.refuse"));
        assert(!BetaFixRules::rejectWaitingVisitor(false,"mercenarie.guildvisitor.refuse"));
        assert(!BetaFixRules::rejectWaitingVisitor(true,"880063-Guild Escort Contracts.mod"));
        // An escort introduced by a tavern still needs a pending contract.
        assert(!BetaFixRules::missionChoiceVisible(false,false,false,false,false,introductions[0]));
    }
} visitorDialogueRegression;
int main(){using namespace BetaFixRules;assert(donor("Barman Sacre","Barman")=="Barman Sacre");assert(donor("  ","Police")=="Police");assert(missionChoiceVisible(true,true,false,false,false,"880027-Guild Escort Contracts.mod"));assert(!missionChoiceVisible(true,true,false,false,false,"880060-Guild Escort Contracts.mod"));assert(missionChoiceVisible(false,false,true,false,false,"880060-Guild Escort Contracts.mod"));assert(missionChoiceVisible(false,false,true,true,false,"880061-Guild Escort Contracts.mod"));std::vector<MailStep>s;s.push_back(MailStep("A#0","old-a","c1"));s.push_back(MailStep("A#1","old-b","c2"));s.push_back(MailStep("B#0","old-c","c1"));std::vector<MailCandidate>i;i.push_back(MailCandidate("new-c","c1",2));i.push_back(MailCandidate("new-a","c1",1));i.push_back(MailCandidate("new-b","c2",0));std::vector<int>m=rebindMail(s,i);assert(m[0]==1&&m[1]==2&&m[2]==0&&m[0]!=m[2]);i[1].handle="old-a";m=rebindMail(s,i);assert(m[0]==1);std::cout<<"fixe beta tests: OK\n";}
