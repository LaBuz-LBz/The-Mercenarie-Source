#pragma once
// Follow the native text lifetime, including forced sayALine bubbles. No raw
// UI pointers are retained and no native bubble is dismissed by the mission.
bool caravanSpeaking(Character* c){
    return c&&c->dialogue&&(c->dialogue->speechTextTimer>0||c->dialogue->speechTextTimer_forced>0);
}
template<class T> bool caravanSpeechReady(T& gap,bool speaking,float elapsed){
    if(speaking){gap=.3f;return false;}
    gap=std::max(T(0),std::min(gap,T(.3))-T(elapsed));return gap<=0;
}
bool caravanCrewSpeaking(const std::string&);
bool caravanConversationBusy();

bool caravanDeliveryInstructionPending(const std::string&);

int caravanNegotiationVariant(const std::string&,int);
