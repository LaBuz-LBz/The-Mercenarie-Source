#pragma once

namespace MissionBookNativeUseRules
{
struct Session
{
    unsigned long long started;
    unsigned long long consumed;
    bool confirmed;
    Session():started(0),consumed(0),confirmed(false){}
};

inline bool start(Session& session,unsigned long long action)
{
    if(!action)return false;
    const bool rearmed=session.consumed!=0;
    session.started=action;
    session.confirmed=false;
    return rearmed;
}

inline bool pending(const Session& session)
{
    return session.started!=0&&session.started!=session.consumed;
}

inline bool confirm(Session& session)
{
    if(!pending(session)||session.confirmed)return false;
    session.confirmed=true;
    return true;
}

inline bool ready(const Session& session)
{
    return pending(session)&&session.confirmed;
}

inline bool consume(Session& session)
{
    if(!ready(session))return false;
    session.consumed=session.started;
    return true;
}
}
