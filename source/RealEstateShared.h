#pragma once
#include "src/RealEstate.h"
#include "RealEstateBaseline.generated.h"
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>
namespace {
extern double currentGameHours;
RealEstate::State estateState;
std::string snapshotEstate;
bool estateBusy=false,estateHooksReady=false;
std::map<RealEstate::Money,hand> estateHandles;
bool estateEnabled();
void estateOptionChanged();
void estateTick(bool resolve=true);
void estateResetWorld();
void estateBuy(Building* b);
void estateRent(Building* b);
void estatePay(RealEstate::Money id,RealEstate::Money sequence);
void estateTerminate(RealEstate::Money id);
void estateInstallHooks();
void estateRefreshView();
void estateResetView();
void estateBuildView(MyGUI::Widget* parent);
void estateShowView(MyGUI::Widget*);
Building* estateResolve(RealEstate::Lease& l);
std::string estateNumber(RealEstate::Money n){std::ostringstream out;out<<n;std::string s=out.str();for(int i=(int)s.size()-3;i>0;i-=3)s.insert(i," ");return s;}
std::string estateText(const char* key){return Loc::text((std::string("estate.")+key).c_str());}
std::string estateWeekly(RealEstate::Money n){Loc::Catalogue a;a["amount"]=estateNumber(n);return Loc::format("estate.weekly",a);}
std::string estateDue(const RealEstate::Lease& l){if(l.suspended)return estateText("suspended");Loc::Catalogue a;a["hours"]=estateNumber((RealEstate::Money)std::ceil(std::max(0.0,l.next-currentGameHours)));return Loc::format("estate.hours",a);}
}
