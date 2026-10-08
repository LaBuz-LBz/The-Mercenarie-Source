#pragma once
#include "CleanupState.h"
// Step identity lives in the native item's saved GameData, not its display name
// or the session handle. The contract sidecar already persists the same stepId.
std::map<std::string,std::string> mailItemSteps;
std::set<std::string> mailRestorationPending;
void resetMailItemIdentities(){mailItemSteps.clear();mailRestorationPending.clear();}
std::string mailItemStep(Item* item){if(!item)return "";std::map<std::string,std::string>::const_iterator i=mailItemSteps.find(item->getHandle().toString());return i==mailItemSteps.end()?std::string():i->second;}
void bindMailItem(Item* item,const std::string& step){if(item&&!step.empty()){mailItemSteps[item->getHandle().toString()]=step;item->isUnique=true;}}
GameData* (*mailItemSaveOriginal)(Item*,GameDataContainer*,GameData*);
GameData* mailItemSaveHook(Item* item,GameDataContainer* container,GameData* refs){
 GameData* state=mailItemSaveOriginal(item,container,refs);
 if(MercenarieCleanup::disabled){if(state){state->sdata.erase("TheMercenarie.MailStep.V1");state->sdata.erase("TheMercenarie.MailCaption.V1");}return state;}
 const std::string step=mailItemStep(item);
 if(state&&!step.empty()){
  state->sdata["TheMercenarie.MailStep.V1"]=step;
  // Cosmetic only. The caption is never used to identify or bind the item.
  state->sdata["TheMercenarie.MailCaption.V1"]=item->getName();
 }
 return state;
}
void (*mailItemLoadOriginal)(Item*,GameDataContainer*,GameData*);
void mailItemLoadHook(Item* item,GameDataContainer* container,GameData* state){
 if(MercenarieCleanup::disabled&&state){state->sdata.erase("TheMercenarie.MailStep.V1");state->sdata.erase("TheMercenarie.MailCaption.V1");}
 std::string step;if(state&&state->sdata.find("TheMercenarie.MailStep.V1")!=state->sdata.end())step=state->sdata["TheMercenarie.MailStep.V1"];
 mailItemLoadOriginal(item,container,state);
 // A reused handle must not inherit an identity from an earlier object.
 mailItemSteps.erase(item->getHandle().toString());bindMailItem(item,step);
 if(!step.empty()&&state&&state->sdata.find("TheMercenarie.MailCaption.V1")!=state->sdata.end()){const std::string caption=state->sdata["TheMercenarie.MailCaption.V1"];if(!caption.empty())item->setName(caption);}
}
