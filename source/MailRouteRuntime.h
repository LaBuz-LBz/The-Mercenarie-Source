#pragma once
// Snapshot the actual contract endpoints once. Never derive the origin from
// the current player, the first road node, or another selected delivery.
bool mailRoutePositionKnown(const Ogre::Vector3& p){return (p.x!=0||p.y!=0||p.z!=0)&&p.x>=-10000000&&p.x<=10000000&&p.y>=-10000000&&p.y<=10000000&&p.z>=-10000000&&p.z<=10000000;}
bool captureMailRoute(MailContracts::Contract& contract,TownBase* origin){
 if(!origin||!origin->getGameData()||origin->getGameData()->stringID!=contract.originTownId||!shou||!shou->townList)return false;
 std::vector<double> points;Ogre::Vector3 p=origin->getPosition();if(!mailRoutePositionKnown(p))return false;
 points.push_back(p.x);points.push_back(p.y);points.push_back(p.z);
 for(size_t i=0;i<contract.steps.size();++i){Town* town=shou->townList->getTownBySID(contract.steps[i].townId);if(!town)return false;p=town->getPosition();if(!mailRoutePositionKnown(p))return false;points.push_back(p.x);points.push_back(p.y);points.push_back(p.z);}
 contract.routePoints=points;contract.originInstanceId=hand(origin).toString();contract.schemaVersion=2;return true;
}
void restoreMailRouteSnapshots(){
 if(!shou||!shou->townList||!ou)return;
 for(size_t i=0;i<mailContracts.size();++i){MailContracts::Contract& c=mailContracts[i];if(c.status!=MailContracts::MailActive||c.delegated||!c.routePoints.empty())continue;
  // The old archive retained the last board's physical TownBase handle.
  // Use it only when both the issuer and board identity belong to this mail.
  TownBase* origin=0;
  if(c.senderId==contractBarmanHandle.toString()&&c.offerId.find(currentBoardKey+"#")==0)origin=contractOriginTown;
  if(origin&&captureMailRoute(c,origin))continue;
  // Legacy fallback is permitted only for an unambiguous town template.
  GameData* data=ou->gamedata.getData(c.originTownId,TOWN);lektor<Town*> towns;
  if(data)shou->townList->getAllTowsByGameData(towns,data);
  if(towns.size()==1)captureMailRoute(c,towns[0]);
 }
}
