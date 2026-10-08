#pragma once
namespace GammaFixRules {
enum VisitOutcome{VisitWaiting,VisitAccepted,VisitRefused,VisitPaid,VisitExtended,VisitEscalated};
inline bool businessFinished(VisitOutcome outcome){return outcome!=VisitWaiting;}
inline bool mayReseat(bool conversationEnded,bool businessActive){return conversationEnded&&businessActive;}
}
