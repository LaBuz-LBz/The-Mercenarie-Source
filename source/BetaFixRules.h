#pragma once
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
namespace BetaFixRules {
inline std::string trim(const std::string& v){size_t a=0,b=v.size();while(a<b&&std::isspace(static_cast<unsigned char>(v[a])))++a;while(b>a&&std::isspace(static_cast<unsigned char>(v[b-1])))--b;return v.substr(a,b-a);}
inline std::string donor(const std::string& realName,const std::string& role){const std::string n=trim(realName);return n.empty()?trim(role):n;}
inline bool interceptBarmanQuestReply(const std::string& id){return id=="880017-Guild Escort Contracts.mod";}
inline bool guildVisitorOfferDialogue(bool visitor){return visitor;}
inline bool showGuildVisitorOffer(bool visitor,const std::string& id){return visitor&&id=="mercenarie.guildvisitor.show";}
inline bool rejectWaitingVisitor(bool visitor,const std::string& id){return visitor&&id=="mercenarie.guildvisitor.refuse";}
inline bool missionChoiceVisible(bool visitor,bool pending,bool active,bool paused,bool following,const std::string& id){const bool intro=id=="880027-Guild Escort Contracts.mod"||id=="880035-Guild Escort Contracts.mod"||id=="880045-Guild Escort Contracts.mod";/* Walk-in visitors offer a contract before missionPending is set. */if(visitor)return intro||id=="184-gamedata.quack";if(intro)return pending;if(id=="880060-Guild Escort Contracts.mod")return active&&!paused;if(id=="880061-Guild Escort Contracts.mod")return active&&(paused||following);if(id=="880062-Guild Escort Contracts.mod")return active;return true;}
struct MailCandidate{std::string handle,carrierId;size_t order;MailCandidate(const std::string& h,const std::string& c,size_t o):handle(h),carrierId(c),order(o){}};
struct MailStep{std::string stepId,handle,carrierId;bool delivered;MailStep(const std::string& s,const std::string& h,const std::string& c,bool d=false):stepId(s),handle(h),carrierId(c),delivered(d){}};
inline std::vector<int> rebindMail(const std::vector<MailStep>& steps,const std::vector<MailCandidate>& items){std::vector<int> out(steps.size(),-1);std::vector<bool> used(items.size(),false);for(size_t s=0;s<steps.size();++s)if(!steps[s].delivered)for(size_t i=0;i<items.size();++i)if(!used[i]&&items[i].handle==steps[s].handle){out[s]=(int)i;used[i]=true;break;}std::vector<size_t> missing;for(size_t s=0;s<steps.size();++s)if(!steps[s].delivered&&out[s]<0)missing.push_back(s);std::sort(missing.begin(),missing.end(),[&](size_t a,size_t b){return steps[a].stepId<steps[b].stepId;});for(size_t m=0;m<missing.size();++m){size_t s=missing[m];int pick=-1;for(size_t i=0;i<items.size();++i)if(!used[i]&&items[i].carrierId==steps[s].carrierId&&(pick<0||items[i].order<items[pick].order))pick=(int)i;if(pick<0)for(size_t i=0;i<items.size();++i)if(!used[i]&&(pick<0||items[i].order<items[pick].order))pick=(int)i;if(pick>=0){out[s]=pick;used[pick]=true;}}return out;}
}
