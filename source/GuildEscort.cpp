#include "PerformanceAudit.h"
#include "GuildOfficesReputationLayout.h"
#include <iomanip>
#include "GuildBuildingButtonLayout.h"
class MapScreen;
#include <Debug.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/GameDataManager.h>
#include <kenshi/Dialogue.h>
#include <kenshi/LocaleInfo.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/CameraClass.h>
#define CharacterMessage AICharacterMessage
#include <kenshi/AI/AI.h>
#undef CharacterMessage
#include <kenshi/CharMovement.h>
#include <kenshi/MedicalSystem.h>
#include <kenshi/AI/AITaskSystem.h>
#include <kenshi/Tasker.h>
#include "MissionNativeGoalRules.h"
#include <kenshi/Damages.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/Faction.h>
#include <kenshi/FactionRelations.h>
#include <kenshi/WorldEventStateQuery.h>
#include <kenshi/Building/Building.h>
#include <kenshi/Building/UseableStuff.h>
#include <kenshi/Building/DoorStuff.h>
#include <kenshi/util/UtilityT.h>
#include <kenshi/CharBody.h>
#include <kenshi/StateBroadcastData.h>
#include <kenshi/combat/CombatClass.h>
#include <kenshi/CameraClass.h>
#include "v5/NavMeshCompat.h"
#include <kenshi/Animation/AnimationClass.h>
#include <kenshi/gui/PortraitManager.h>
#include <kenshi/gui/MainBarGUI.h>
#include <kenshi/gui/ForgottenGUI.h>
#define ParticlePool ZoneManagerParticlePool
#include <kenshi/ZoneManager.h>
#undef ParticlePool
#include <mygui/MyGUI_Gui.h>
#include <mygui/MyGUI_Button.h>
#include <mygui/MyGUI_Window.h>
#include <mygui/MyGUI_Delegate.h>
#include <mygui/MyGUI_ScrollBar.h>
#include <mygui/MyGUI_ScrollView.h>
#include <mygui/MyGUI_ResourceImageSet.h>
#include <mygui/MyGUI_TextBox.h>
#include <mygui/MyGUI_RenderManager.h>
#include <mygui/MyGUI_ProgressBar.h>
#include <mygui/MyGUI_ResourceManager.h>
#include <mygui/MyGUI_ImageBox.h>
#include <mygui/MyGUI_EditBox.h>
#include "SafeGuiCleanup.h"
#include "GuildResponsiveLayout.h"
#include "GuildProgressLayout.h"
#include "GuildOverviewDataModel.h"
#include "GuildMapViewport.h"
#include "GuildContractHistory.h"
#include "GuildContractsModel.h"
#include <mygui/MyGUI_ComboBox.h>
#include "MercenarieNativeInput.h"
#include <kenshi/gui/ManagementScreen.h>
#include "PricingRoads.h"
#include "CleanupState.h"
#include "MissionRoadPath.h"
#include "ContractRouteVisual.h"
#include "ContractRegionRoute.h"
#include "ContractDestinationRules.h"
#include "MailPresentation.h"
#include "MissionRouteRecovery.h"
#include "MissionMotionPolicy.h"
#include "ContractGroupPlan.h"
#include "AmbushSpawnPlan.h"
#include "AmbushBalance.h"
#include "FrozenPriceTerms.h"
#include "ContractReroll.h"
#include "ContractFactors.h"
#include "GuildProgression.h"
#include "GuildLevelUnlocks.h"
#include "GuildSeatQueue.h"
#include "GuildVisitorTravel.h"
#include "ReputationIdentity.h"
#include "GuildSavePaths.h"
#include "MissionBonuses.h"
#include "MissionFormation.h"
#include "EscortPace.h"
#include "ContractRewards.h"
#include "ContractOfferVariety.h"
#include "EscortMissionRules.h"
#include "DelegatedMissionTiming.h"
#include "MissionBookContractsModel.h"
#include "MissionBookLayout.h"
#include "MissionBookGeometry.h"
#include "MissionBookNativeUseRules.h"
#include "MailContracts.h"
#include "BetaFixRules.h"
#include "GuildFurnitureRecovery.h"
#include "GuildVisitorOfferRules.h"
#include "GammaFixRules.h"
#include "ZetaFixRules.h"
#include "MailOfferPlan.h"
#include "MailRoutePlanner.h"
#include "MercenarySelectionRules.h"
#include "MissionOfferViewRules.h"
#include "BountyPresentationRules.h"
#include <set>
#include "ContinuousMapAxis.h"
#include <ogre/OgreResourceGroupManager.h>
#include <cstdio>
#define BuildingDesignation PlatoonBuildingDesignation
#define BD_NONE PBD_NONE
#define BD_SHOP PBD_SHOP
#define BD_BARRACKS PBD_BARRACKS
#define BD_BAR PBD_BAR
#define BD_HOSPITAL PBD_HOSPITAL
#define BD_ARMOURY PBD_ARMOURY
#define BD_TREASURE PBD_TREASURE
#define BD_PRISON PBD_PRISON
#define BD_HQ PBD_HQ
#define BD_RESIDENTIAL PBD_RESIDENTIAL
#define BD_SLAVE_STORAGE PBD_SLAVE_STORAGE
#define BD_RESIDENTIAL_SMALL PBD_RESIDENTIAL_SMALL
#include <kenshi/Platoon.h>
#undef BuildingDesignation
#undef BD_NONE
#undef BD_SHOP
#undef BD_BARRACKS
#undef BD_BAR
#undef BD_HOSPITAL
#undef BD_ARMOURY
#undef BD_TREASURE
#undef BD_PRISON
#undef BD_HQ
#undef BD_RESIDENTIAL
#undef BD_SLAVE_STORAGE
#undef BD_RESIDENTIAL_SMALL
#include <kenshi/RootObjectFactory.h>
#include <kenshi/Town.h>
#include <kenshi/SharedKing.h>
#include "NativeRegionLookup.h"
#include <kenshi/InputHandler.h>
#include <kenshi/Item.h>
#include <kenshi/Inventory.h>
#include <kenshi/gui/InventoryGUI.h>
#include "ArtisanOrders.h"
#define private public
#define protected public
#include <kenshi/SaveManager.h>
#include <kenshi/gui/LoadSaveWindow.h>
#include <kenshi/gui/NewGameOptionsWindow.h>
#undef protected
#undef private
#include "ResearchCompat.h"
#include "src/Guild/Level3Access.h"
#include <algorithm>
#include <cctype>
#include <map>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include "Localization.h"
#include "MercenarieFonts.h"
#include "src/Integration/FcsLocalization.h"
#include "src/Integration/CommunityTranslations.h"
#include "EscortConfig.h"
#include "EscortContract.h"
#include "EscortEconomy.h"
#include "src/Contracts/EstimatedPricing.h"
#include "EscortReputation.h"
#include "MercenarieRegions.h"
#include "FiscalSystem.h"
#include "src/Save/FiscalLedgerFormat.h"
#include "src/Save/Diagnostics.h"
#include "src/FinancePresentation.h"
#include "src/UI/ClientOptions.h"
#include "src/UI/ClientOptionsFile.h"
static std::string readClientOptionsBytes(){ClientOptionsFile::Failure failure;std::wstring file=ClientOptionsFile::path(failure);std::string bytes;if(file.empty()||!ClientOptionsFile::read(file,bytes,failure)){if(failure.code!=ERROR_FILE_NOT_FOUND)DebugLog(failure.message());}return bytes;}
#include "src/GuildPayroll.h"
#include "src/GuildPayrollPersistence.h"
#include <kenshi/RaceData.h>
#include "ContractQualityRules.h"
#include "RoutePrototype.h"
#include "RoadRoutePreset.h"
#include "MainMenuNews.h"
#include "MainMenuBackground.h"

static bool importedDomainsSuspended=false;
static std::vector<std::string> importedArtisans;
static double importedArtisanHour=-1;
static bool gMercenarieEnglish = true;
static bool diagnosticEnabled=false;
static std::string gMercenarieLanguageOverride="auto";
static bool gMercenarieLanguagePreferenceLoaded=false;
static const char* kMissionBookBuildingId="880137-Guild Escort Contracts.mod";
#include "SaveOperationRuntime.h"
#include "SavedActorResolution.h"
static void reportPersistenceError(const char* operation,SaveDiagnostics::Reason reason,const std::string& detail,const std::string& path,const char* messageKey=0){
    if(activeSaveOperation){SaveDiagnostics::Failure f;f.operation=operation;f.reason=reason;f.detail=detail;f.path=path;saveRemember(f);return;}
    const std::string code=SaveDiagnostics::code(operation,reason);
    static unsigned long sequence=0;
    std::ostringstream incident;incident<<GetCurrentProcessId()<<'-'<<GetTickCount()<<'-'<<++sequence;
    // ErrorLog is enabled independently of the optional dialogue diagnostics.
    ErrorLog(SaveDiagnostics::record(code,incident.str(),detail,path));
    if(messageKey&&ou)ou->showPlayerAMessage(SaveDiagnostics::playerMessage(Loc::text(messageKey),code),true);
}
static void reportPersistenceException(const char* operation,const std::exception& e,const std::string& path,const char* messageKey=0){
    SaveDiagnostics::Failure f=SaveDiagnostics::exceptionFailure(e);if(f.path.empty())f.path=path;
    if(!hasWorldPersistenceRoot){worldPersistenceRoot=f;hasWorldPersistenceRoot=true;}
    reportPersistenceError(operation,f.reason,SaveDiagnostics::describe(f),f.path,messageKey);
}
static void diagnosticTrace(const char* stage,const char* id="")
{
    if(!diagnosticEnabled)return;
    FILE* log=0;
    if(fopen_s(&log,"mods/Guild Escort Contracts/dialogue-diagnostic.log","ab")!=0||!log)return;
    fprintf(log,"[V4-DIAG] %s | %s\n",stage,id);fflush(log);fclose(log);
}

static void detectMercenarieLanguage()
{
    // Read the global preference before the first menu frame, including fresh launches.
    CommunityTranslations::discover(ou);
    static bool preferenceLoaded=false;
    if(!preferenceLoaded){preferenceLoaded=true;std::istringstream options(readClientOptionsBytes());std::string line;while(std::getline(options,line)){if(!line.empty()&&line[line.size()-1]=='\r')line.erase(line.size()-1);if(line.compare(0,9,"language=")==0){std::string value=line.substr(9);if(ClientOptions::validLanguage(value))gMercenarieLanguageOverride=value;}}}
    LocaleManager* manager=LocaleManager::getInstance();
    LocaleInfo* locale=manager?manager->getCurrentLocale():0;
    std::string language="en";
    if(locale)language=locale->id.empty()?locale->steamCode:locale->id;
    else {std::ifstream settings("settings.cfg");std::string line;while(std::getline(settings,line))if(line.compare(0,9,"language=")==0){language=line.substr(9);break;}}
    Loc::select(gMercenarieLanguageOverride=="auto"?language:gMercenarieLanguageOverride);
    static size_t notices=0;
    const std::vector<std::string>& log=Loc::registry().diagnostics;
    while(notices<log.size())DebugLog("Mercenarie translations: "+log[notices++]);
    static std::string lastSelection;
    std::ostringstream selection;selection<<gMercenarieLanguageOverride<<"/"<<language<<"/"<<Loc::engine().language<<"/"<<Loc::registry().revision;
    if(lastSelection!=selection.str()){
        lastSelection=selection.str();std::ostringstream summary;
        summary<<"Mercenarie translations: requested="<<gMercenarieLanguageOverride<<" gameLocale="<<language<<" selected="<<Loc::engine().language<<" translated="<<Loc::engine().selected.size()<<" englishFallback="<<(Loc::engine().english.size()-Loc::engine().selected.size());
        DebugLog(summary.str());
    }
    gMercenarieEnglish=Loc::engine().language.substr(0,2)!="fr";
}
static std::vector<std::string> mercenarieProtectedNames;
static std::string mercenarieLocalize(const std::string& source){return Loc::legacy(source,mercenarieProtectedNames);}

// Captions pass through MercenarieFonts::caption for localization and glyph coverage.
#define showPlayerAMessage(message, ...) showPlayerAMessage(mercenarieLocalize(message), __VA_ARGS__)
#define sayALine(message, ...) sayALine(mercenarieLocalize(message), __VA_ARGS__)

#include "RealEstateShared.h"
#include "GuardRestRuntime.h"
#include "GuardSaluteRuntime.h"
namespace
{
    int activeQuestCount();
    std::string allocateQuestFiscalId();
    const char* questEscortFactionId();
    int bountyQuestSlot();
    bool isAnyQuestBountyPlatoon(Platoon*,bool);
    void observeQuestPrison(Character*);
    void updateAllQuestBountyMaps(MapScreen*);
    void updateAllQuestMissionMaps(MapScreen*);
    bool questCapacityAvailable();
    bool prepareEscortQuest();
    bool prepareBountyQuest();
    bool questPanelLocked();
    void selectQuestActor(Character*);
    void resetQuestContexts();
    void queueQuestCleanup();
    void refreshQuestActors();
    void appendAllQuestItems();
    void selectTrackerQuest(int,bool);
    const char* escortSquad = "880010-Guild Escort Contracts.mod";

    Character* escort = 0;
    hand escortHandle;
    Ogre::Vector3 destination;
    const char* destinationTown = "49386-rebirth.mod";
    const char* destinationName = "World's End";
    std::string destinationNameStorage = Loc::text("ui.world_s_end");
    int missionReward = 4000;
    int healthBonus = 1000;
    bool missionActive = false;
    bool missionPending = false;
    EscortPace::State missionPace = EscortPace::Normal;
    bool missionForcedPace = false;
    bool missionCasualtyWaiting = false;
    Character* missionTemporaryLeader = 0;
    EscortMissionRules::Separation missionSeparation;
    float carriedDestinationDistance = -1.0f;
    enum ContractLifecycleState
    {
        CONTRACT_NONE,
        CONTRACT_CLIENT_MEETING,
        CONTRACT_NEGOTIATING,
        CONTRACT_ACTIVE,
        CONTRACT_COMPLETED,
        CONTRACT_FAILED
    };
    ContractLifecycleState contractLifecycle = CONTRACT_NONE;
    bool waitingForPlayer = false;
    bool leavingBuilding = false;
    Ogre::Vector3 exitWaypoint;
    float updateClock = 0.0f;
    float stationaryClock = 0.0f;
    Ogre::Vector3 journeyStart;
    float journeyDistanceSquared = 0.0f;
    bool missionPaused = false;
    bool missionFollowing=false;
    hand missionFollowTarget;
    std::vector<hand> missionFollowers;
    struct WaitingHereState {
        hand actor;
        Ogre::Vector3 position;
        AI::AIManuverabilityOrders movement;
        Ogre::Vector3 center;
        hand centerTarget;
        bool returning;
    };
    std::vector<WaitingHereState> waitingHere;
    bool midpointChecked = false;
    bool quarterSpeech = false;
    bool finalSpeech = false;
    bool rareContract = false;
    Faction* contractOriginFaction = 0;
    float selectedDistance = 0.0f;
    bool wasInCombat = false;
    float unconsciousSeconds = 0.0f;
    float carriedByPlayerSeconds = 0.0f;
    MyGUI::Button* trackerIcon = 0;
    MyGUI::Window* trackerWindow = 0;
    MyGUI::Widget* mercenarieLauncherMenu = 0;
    MyGUI::Window* mercenarieAutopilotWindow = 0;
    MyGUI::Button* trackerEntry = 0;
    bool requestPersonnelSelection(int reward,bool goodwill);
    void acceptNegotiatedContractFinal(int reward,bool goodwill);
    void addPersonnelDesign(MyGUI::Widget* parent,int width);
    void addPersonnelOption(MyGUI::Widget* parent,int y,int width,bool basic);
    void clearCurrentPersonnel();
    void releaseAllPersonnel();
    void restoreCurrentPersonnel();
    void closePersonnelDialog();
    void resetPersonnelChoice();
    MyGUI::Window* negotiationWindow = 0;
    MyGUI::TextBox* offerText = 0;
    MyGUI::TextBox* proposalText = 0;
    MyGUI::TextBox* sliderPercentText = 0;
    MyGUI::TextBox* reactionText = 0;
    MyGUI::ScrollBar* priceSlider = 0;
    MyGUI::Widget* negotiationSliderPanel = 0;
    MyGUI::Button* negotiationSliderTrack = 0;
    MyGUI::TextBox* negotiationSliderTitle = 0;
    MyGUI::Button* sliderMinusButton = 0;
    MyGUI::Button* sliderPlusButton = 0;
    MyGUI::Button* proposeButton = 0;
    MyGUI::Button* counterButton = 0;
    MyGUI::Button* refuseButton = 0;
    MyGUI::Button* returnButton = 0;
    MyGUI::Window* contractDecisionWindow = 0;
    MyGUI::TextBox* contractDecisionText = 0;
    MyGUI::Button* contractDecisionAccept = 0;
    MyGUI::Button* contractDecisionRefuse = 0;
    int baseReward = 0;
    int proposedReward = 0;
    int counterOffer = 0;
    int advancePaid = 0;
    int negotiatedAdvancePercent = 0;
    float escortReputation = 0.0f;
    int guildPoints = 0;
    int guildPrestige = 0;
    bool progressWriteBlocked=false,progressVirginWorld=false;
    extern bool progressLoadFault; // Defined by MissionPersistence.h after the mission types.
    std::set<std::string> rewardedContractIds;
    std::vector<hand> progressMembers;
    std::set<unsigned int> progressDeadMembers;
    bool progressAnyClientKo=false,progressRosterKnown=false;
    int progressReportLocal=0,progressReportGlobal=0;
    MissionBonuses::Choice finalBonusChoice;
    MyGUI::Button* finalBonusButtons[MissionBonuses::Count]={0};
    MyGUI::TextBox* finalBonusPreview=0;
    int negotiationInsistence = 0;
    long long totalAdvances = 0, totalBonuses = 0, totalTips = 0;
    int missionTierIndex = 0;
    int pendingGuildXp = 0;
    float clientBudgetMultiplier = 1.15f;
    float personalityAcceptance = 0.0f;
    bool counterOfferActive = false;
    bool negotiationOpen = false;
    bool negotiationWasPaused = false;
    std::string originCity = Loc::text("ui.unknown_city");
    struct CityMemory { int abuses; int recovery; CityMemory():abuses(0),recovery(0){} };
    std::map<std::string, CityMemory> cityMemories;
    std::map<std::string, float> localReputations;
    std::vector<std::string> archivedReputationAliases;
    float missionElapsed = 0.0f;
    double reportStartHour=-1,reportEndHour=-1;
    float expectedTravelTime = 0.0f;
    bool escortWasKnockedOut = false;
    int journeyCombatCount = 0;
    float proximitySeconds=0.0f, journeySeconds=0.0f, incidentClock=0.0f, importantAlertClock=0.0f;
    std::string lastImportantAlert;
    Ogre::Vector3 lastJourneyPosition;
    int successfulContracts = 0;
    int failedContracts = 0;
    long long totalContractCats = 0;
    std::vector<std::string> contractHistory;
    std::map<std::string,GuildHistory::Snapshot> contractSeeds;
    MyGUI::Window* guildWindow = 0;
    GuildResponsive::Metrics guildLayout=GuildResponsive::calculate(1920,1080);
    int guildLayoutViewportW=0,guildLayoutViewportH=0;
    MyGUI::TextBox* guildZonesText = 0;
    MyGUI::TextBox* guildDetailsText = 0;
    MyGUI::TextBox* guildStatsText = 0;
    MyGUI::TextBox* guildHeroTitleText=0;MyGUI::TextBox* guildHeroLevelText=0;MyGUI::TextBox* guildHeroXpText=0;MyGUI::TextBox* guildHeroUnlockText=0;MyGUI::TextBox* guildHeroDescriptionText=0;
    MyGUI::TextBox* guildHistoryText = 0;
    MyGUI::TextBox* guildSuccessText=0;MyGUI::TextBox* guildCatsText=0;MyGUI::TextBox* guildBenefitsText=0;MyGUI::TextBox* guildContractText=0;MyGUI::TextBox* guildHouseText=0;MyGUI::TextBox* guildActivityText=0;
    MyGUI::ProgressBar* guildLevelProgress=0;
    MyGUI::TextBox* guildLevelNumberText=0;
    MyGUI::Widget* guildOverviewPanels[8] = {0,0,0,0,0,0,0,0};
    MyGUI::Widget* guildReputationPanel = 0;
    MyGUI::Button* guildTabButtons[6] = {0};
    ClientOptions::Settings clientOptions(true,OIS::KC_J,OIS::KC_P,0);
    bool clientOptionsLoaded=false;
    bool estateEnabled(){return !MercenarieCleanup::disabled&&(clientOptions.realEstateEnabled||RealEstate::requiresSystem(estateState));}
    double guildInvestmentNextHour=0;
    int contractRewardPercent=100; // per-save; absent in legacy saves = Standard
    MyGUI::ScrollBar* contractRewardSlider=0;
    MyGUI::TextBox* contractRewardValue=0;
    MyGUI::Widget* contractRewardTip=0;
    bool guildKeyCapture=false,guildKeyStates[256]={false};
    ClientOptions::Action capturedAction=ClientOptions::OpenGuildManagement;
    int pendingConflictAction=-1,pendingConflictKey=0;
    MyGUI::TextBox* guildKeyLabel=0;
    MyGUI::Widget* guildKeyTip=0;
    bool validGuildKey(int k){return (k>=OIS::KC_Q&&k<=OIS::KC_P)||(k>=OIS::KC_A&&k<=OIS::KC_L)||(k>=OIS::KC_Z&&k<=OIS::KC_M)||(k>=OIS::KC_F1&&k<=OIS::KC_F10)||k==OIS::KC_F11||k==OIS::KC_F12;}
    void toggleGuildManagement(MyGUI::Widget*);
    void openMissionBookManagementFor(Building* book);
    struct BoardOffer;
    void openContractsBoard(Character*,bool,bool);
    void analyseContractRoute(const Ogre::Vector3&,const Ogre::Vector3&,BoardOffer&);
    bool refreshOfferRegions(BoardOffer&);
    void openBountyOffers(Character*);
    void updateBountyDossier();
    void updateContractsBoard();
    std::string missionBookSecurityText(const std::string&);
    std::string registerBountySummary();
    int overviewBountyCount();
    int overviewEscortCount(bool expeditions);
    std::vector<GuildContracts::Row> collectGuildContracts();
    void refreshGuildContracts(bool force=false);
    void buildGuildContracts(MyGUI::Widget*);
    void closeGuildContractConfirm();
    void guildContractMapAction(const GuildContracts::Row&);
    void guildContractCancelAction(const GuildContracts::Row&);
    MyGUI::Widget* overviewChrome=0;
    MyGUI::Widget* guildOptionsPanel=0;
    MyGUI::Button* clientTrackerCheck=0;
    MyGUI::Widget* clientOptionTip=0;
    MyGUI::ScrollView* optionsScroll=0;
    MyGUI::EditBox* optionsSearch=0;
    MyGUI::TextBox* optionsSearchHint=0;
    MyGUI::ComboBox* optionsCategoryFilter=0;
    MyGUI::Button* optionsResetAllButton=0;
    MyGUI::Button* optionsCategoryHeaders[ClientOptions::CategoryCount]={0,0,0,0,0,0,0};
    MyGUI::TextBox* optionsCategoryChevrons[ClientOptions::CategoryCount]={0,0,0,0,0,0,0};
    MyGUI::TextBox* optionsCategoryTitles[ClientOptions::CategoryCount]={0,0,0,0,0,0,0};
    MyGUI::TextBox* optionsCategoryDescriptions[ClientOptions::CategoryCount]={0,0,0,0,0,0,0};
    MyGUI::Widget* optionsCategoryBodies[ClientOptions::CategoryCount]={0,0,0,0,0,0,0};
    bool optionsCategoryOpen[ClientOptions::CategoryCount]={false,false,false,false,false,false,false,false};
    float optionsViewScale=1.0f;
    bool o103Ready=false;void layoutOptions103();void refreshOptions103();void openOptions108();
    int optionsSelectedCategory=-1;
    bool optionsResetConfirmation=false,optionsUiResetConfirmation=false;
    struct OptionsRow {MyGUI::Widget* widget;int category;std::string title,description;OptionsRow(MyGUI::Widget* w=0,int c=0,const std::string& t="",const std::string& d=""):widget(w),category(c),title(t),description(d){}};
    std::vector<OptionsRow> optionsRows;
    MyGUI::TextBox* optionKeyLabels[ClientOptions::ActionCount]={0,0,0};
    bool optionsRebuildRequested=false;
    void loadClientOptions(){if(clientOptionsLoaded)return;clientOptionsLoaded=true;std::istringstream in(readClientOptionsBytes());ClientOptions::read(in,clientOptions,validGuildKey);}
    MyGUI::Widget* guildRelationsPanel = 0;


    MyGUI::Widget* guildOfficesPanel = 0;
    MyGUI::TextBox* guildOfficesText = 0;
    MyGUI::TextBox* guildOfficeCountText=0;MyGUI::TextBox* guildOfficeDetailText=0;MyGUI::TextBox* guildOfficeCreateText=0;
    MyGUI::Button* guildOfficeDefineButton=0;
    MyGUI::Widget* guildFinancesPanel = 0;
    MyGUI::TextBox* guildFinancesText = 0;
    MyGUI::TextBox* guildFinanceGrossText=0;MyGUI::TextBox* guildFinancePaidText=0;MyGUI::TextBox* guildFinanceDebtText=0;MyGUI::TextBox* guildFinanceUcText=0;MyGUI::TextBox* guildFinanceGuildText=0;MyGUI::TextBox* guildFinanceHistoryText=0;
    MyGUI::Button* fiscalPayButtons[2] = {0,0};
    MyGUI::Button* fiscalRulesButton = 0;
    bool fiscalShowRules = false;
    MyGUI::Window* fiscalCollectorWindow=0;
    MyGUI::TextBox* fiscalCollectorText=0;
    MyGUI::Button* fiscalCollectorPay=0;
    MyGUI::Button* fiscalCollectorDetail=0;
    MyGUI::Button* fiscalCollectorExtension=0;
    MyGUI::Button* fiscalCollectorRefuse=0;
    int activeFiscalOrganisation=-1;
    bool fiscalRefusalConfirm=false;
    struct FiscalParty{int organisation,raid;hand leader;std::vector<hand> members;float stuckClock,seatRefresh,restoreRetry;bool waitingForConversation;Ogre::Vector3 lastPosition;FiscalParty():organisation(-1),raid(0),stuckClock(0),seatRefresh(0),restoreRetry(0),waitingForConversation(false),lastPosition(Ogre::Vector3::ZERO){}};
    FiscalParty fiscalParty;
    MyGUI::Button* reputationFilterButtons[4] = {0,0,0,0};
    MyGUI::Button* defineGuildHouseButton = 0;
    MyGUI::Window* missionBookTestWindow = 0;
    MyGUI::Window* automaticQuestWindow = 0;
    MyGUI::Button* missionBookTabs[6] = {0,0,0,0,0,0};
    MyGUI::TextBox* missionBookOfficeText = 0;
    MyGUI::TextBox* missionBookContentText = 0;
    MyGUI::Widget* missionBookOffersPanel = 0;
    MyGUI::Button* missionBookOfferButtons[6] = {0,0,0,0,0,0};
    struct MissionBookEntry{int kind,index;std::string label;MissionBookEntry(int k=0,int i=0,const std::string& l=std::string()):kind(k),index(i),label(l){}};
    std::vector<MissionBookEntry> missionBookEntries;
    MyGUI::Window* mailCancelWindow=0;MyGUI::TextBox* mailCancelWarning=0;MyGUI::Button* mailCancelConfirmButton=0;MyGUI::Button* mailCancelBackButton=0;int mailCancelIndex=-1;
    void requestMailCancel(int index);
    void mailTrackerClicked(MyGUI::WidgetPtr sender);
    void openMailDelivery89(int index);
    MyGUI::Window* mailDeliveryWindow89=0;
    bool missionBookDelegationContext=false;
    MyGUI::Window* delegationRosterWindow=0;
    struct DelegationCardWidgets{MyGUI::Button* card;MyGUI::ImageBox* portrait;MyGUI::TextBox* check;MyGUI::TextBox* details;MyGUI::TextBox* unavailable;DelegationCardWidgets():card(0),portrait(0),check(0),details(0),unavailable(0){}};
    std::vector<DelegationCardWidgets> delegationRosterCards;
    MyGUI::ScrollView* delegationRosterScroll=0;
    MyGUI::Widget* delegationRosterCanvas=0;
    MyGUI::Button* delegationFilterButtons[6]={0,0,0,0,0,0};
    MyGUI::TextBox* delegationSelectionCount=0;
    MyGUI::TextBox* delegationSelectionSummary=0;
    MyGUI::Button* delegationDeselectAllButton=0;
    MyGUI::Button* delegationConfirmButton=0;
    std::vector<Character*> delegationRosterCharacters;
    std::vector<bool> delegationRosterSelected;
    std::vector<bool> delegationRosterAvailable;
    std::vector<int> delegationRosterCategories;
    int delegationRosterFilter=0;
    bool delegationConfirming=false;
    int delegationOfferKind=0,delegationOfferIndex=-1;
    std::string delegationOfferIdentity,delegationIssuerIdentity,delegationBoardKey;
    struct DelegatedMissionState;
    void openMissionBookOffer(MyGUI::WidgetPtr);
    void beginDelegationSelection(int,int);
    void delegationRosterToggle(MyGUI::WidgetPtr);
    void delegationRosterFilterClicked(MyGUI::WidgetPtr);
    void delegationRosterDeselectAll(MyGUI::WidgetPtr);
    void refreshDelegationRosterCards();
    void delegationRosterConfirm(MyGUI::WidgetPtr);
    void showDelegatedReport(int);
    bool delegatedAbsenceActive();
    bool delegatedCharacterAbsent(Character*);
    void openMissionBookBountyDetail(int);
    void missionBookBoardTabClicked(MyGUI::WidgetPtr);
    void refreshMissionBookContractHub();
    void initializeMissionBookPool();
    std::string missionBookBountyDossier();
    void tickMissionBookPool();
    void selectMissionBookEntry(size_t);
    void missionBookOfferRowClicked(MyGUI::WidgetPtr);
    void refreshMissionBookOfferCounts();
    bool missionBookPersonalAvailable();
    bool missionBookSelectionValid();
    void refreshMissionBookDelegationPanel();
    void missionBookDelegateClicked(MyGUI::WidgetPtr);
    void missionBookCategoryChanged(MyGUI::ComboBox*,size_t);
    void missionBookSoldierSortChanged(MyGUI::ComboBox*,size_t);
    void missionBookPageClicked(MyGUI::WidgetPtr);
    bool beginMissionBookSecurityDelegation(int);
    bool acceptMissionBookSecurityPersonal(int);
    bool prepareMissionBookSecurityIdentity(int);
    void synchronizeMissionBookSecurityBoard();
    bool prepareDelegatedBounty(DelegatedMissionState&,double&,DelegatedMissionTiming::ActivityType&);
    void seedDelegated(const DelegatedMissionState&);
    void consumeDelegatedBounty();
    int missionBookTab = 0;
    std::string missionBookOfficeKey,missionBookOfficeCity,missionBookOfficeName,missionBookTownId;
    struct MissionBookNativeWatch
    {
        hand book;
        hand orderedActor;
        MissionBookNativeUseRules::Session session;
        TaskType lastTask;
        float age;
        MissionBookNativeWatch():lastTask(NULL_TASK),age(0.0f){}
    };
    std::map<std::string,MissionBookNativeWatch> missionBookNativeWatches;
    unsigned long long missionBookNativeNextAction=0;
    std::string designatedGuildHouseKey;
    std::string operationalAnnouncementKey;
    hand designatedGuildHouseHandle;
    bool guildHouseOperational = false;
    bool fcsLanguageApplied = false;
    MyGUI::Button* resetMercenarieImportCheck = 0;
    MyGUI::TextBox* resetMercenarieImportLabel = 0;
    bool resetMercenarieOnImport = false;
    bool cleanupMercenarieOnImport=false;
    MyGUI::Button* cleanupMercenarieImportCheck=0;
    MyGUI::TextBox* cleanupMercenarieImportLabel=0;
    void clearMercenarieForCleanup(); void cleanupNativeOwnedResearch();
    ImportGameMenu* configuredMercenarieImportMenu = 0;
    const int RESET_MERCENARIE_IMPORT_FLAG = 0x40000000;
    MyGUI::Window* guildLevelUpWindow = 0;
    MyGUI::TextBox* guildLevelUpText = 0;
    MyGUI::Button* guildLevelUpClose = 0;
    int appliedGuildUnlockLevel = -1;
    bool guildLevelUpWasPaused = false;
    class MercenarieLayoutAccess : public wraps::BaseLayout{public:MyGUI::Widget* rootWidget(){return mMainWidget;}};

    void applyFcsRuntimeLanguage()
    {
        static std::string appliedLanguage;
        if (!ou || (fcsLanguageApplied && appliedLanguage==Loc::engine().language)) return;
        appliedLanguage=Loc::engine().language;
        fcsLanguageApplied = true;
        MercenarieFcsLocalization::apply(ou,gMercenarieEnglish);
    }
    MyGUI::Button* reputationRows[16] = {0};
    MyGUI::ImageBox* reputationIcons[16] = {0};
    MyGUI::TextBox* reputationPageDetails = 0;
    MyGUI::TextBox* reputationPageSummary=0;MyGUI::TextBox* reputationPageEmpty=0;
    int reputationFilter = 0;
    std::vector<std::string> visibleReputationNames;
    bool jWasDown = false;
    bool pWasDown = false;
    bool autopilotWasDown = false;
    bool escapeWasDown = false;
    MyGUI::Window* developerWindow = 0;
    MyGUI::TextBox* developerStatus = 0;
    int developerFurnitureOverride = -1;
    float developerTimeOffsetHours = 0.0f;
    EscortContractData currentContract;
    EscortJourneyData journeyData;
    MyGUI::Button* bonusButtons[6] = {0,0,0,0,0,0};
    MyGUI::TextBox* paymentBreakdownText = 0;
    MyGUI::Window* finalWindow = 0;
    MyGUI::TextBox* finalSummaryText = 0;
    MyGUI::Button* finalCashButton = 0;
    MyGUI::Button* finalHalfCashButton = 0;
    MyGUI::Button* finalReputationButton = 0;
    int earnedFinalBonus = 0;
    int earnedFinalBonusCount = 0;
    int finalClientTip = 0;
    int requestedFinalBonusPercent = 0;
    bool finalHealthy = false;
    bool finalFast = false;
    bool boardCaravan = false;
    bool boardScientific = false;
    bool caravanMission = false;
    bool caravanReturning = false;
    Ogre::Vector3 caravanOrigin;
    std::vector<Character*> caravanMembers;
    std::string caravanCargoType=Loc::text("ui.miscellaneous_goods");
    int caravanCargoValue=0, caravanInitialMembers=0;
    bool travelIncidentTriggered=false;
    bool scientificMission=false,scientificResearching=false,scientificReturning=false,scientificEntryAttempted=false,scientificInsideDiscovery=false,scientificWillEnter=false,scientificEntryResolved=false;
    float scientificResearchSeconds=0.0f,scientificMoveClock=0.0f,scientificCommentClock=0.0f,scientificEntryClock=0.0f;
    Ogre::Vector3 scientificOrigin,scientificRuinCenter;
    std::vector<Character*> scientificMembers;
    bool guildClientSystemActive=false;
    float guildFacilityScanClock=0.0f;
    GuildFurnitureRecovery::State guildFurnitureRecovery;
    Building* guildBuilding=0;
    Building* guildClientChair=0;
    std::vector<Building*> guildClientChairs;
    Building* guildFiscalChair=0;
    std::vector<Building*> guildWaitingChairs;
    struct GuildSeatReservation
    {
        hand actor,seat;int kind;std::string queueKey;
        GuildSeatReservation():kind(0){actor.setNull();seat.setNull();}
    };
    struct GuildSeatTicketState
    {
        std::string key;unsigned long ticket;GuildSeatTicketState():ticket(0){}
    };
    struct GuildTravelRecord
    {
        hand actor,office,commandedSeat;GuildVisitorTravel::State state;
        std::vector<std::string> rejectedSeats;float rejectCooldown;
        float seatRetry,resolveRetry;Ogre::Vector3 lastPosition,officeDestination;bool destinationValid,missingLogged;GuildVisitorTravel::Reason pendingSeatRetry;
        GuildTravelRecord():rejectCooldown(0),seatRetry(0),resolveRetry(0),lastPosition(Ogre::Vector3::ZERO),officeDestination(Ogre::Vector3::ZERO),destinationValid(false),missingLogged(false),pendingSeatRetry(GuildVisitorTravel::None){actor.setNull();office.setNull();commandedSeat.setNull();}
    };
    std::vector<GuildTravelRecord> guildTravelRecords;
    hand fiscalTargetOffice;
    std::vector<GuildSeatReservation> guildSeatReservations;
    std::map<std::string,unsigned long> guildSeatTickets;
    unsigned long guildSeatNextTicket=1;
    std::map<std::string,std::string> guildHouseNames;
    std::map<std::string,std::string> guildHouseCities;
    std::map<std::string,std::string> guildHouseOriginalNames;
    FiscalLedger fiscalLedger;
    std::string activeMercenarieSaveSlot;
    std::string fiscalFile="mods/Guild Escort Contracts/GuildEscortFiscal.dat";
    std::string reputationFile="mods/Guild Escort Contracts/GuildEscortReputation.dat";
    std::string currentMissionFiscalId;
    double fiscalUpdateClock=0;
    std::string currentGuildHouseKey,currentGuildHouseName;
    hand pendingGuildHouseHandle;
    std::string pendingGuildHouseKey;
    MyGUI::Window* guildNameWindow=0;
    MyGUI::EditBox* guildNameEdit=0;
    MyGUI::Button* guildNameConfirm=0;
    struct GuildVisitor
    {
        Character* leader;std::vector<Character*> members;SavedActorResolution::Actor restoredLeader;SavedActorResolution::Group restoredMembers;float patience;float remarkClock;float orderRefresh;std::string type;std::string houseKey;std::string offerKey;hand targetHouse;std::vector<hand> assignedSeats;int profile;bool urgent;bool vip;bool exceptional;bool arrivalNotified;
        GuildVisitor():leader(0),patience(0),remarkClock(0),orderRefresh(0),profile(GuildVisitorOfferRules::Civilian),urgent(false),vip(false),exceptional(false),arrivalNotified(false){}
    };
    std::vector<GuildVisitor> guildVisitors;
    unsigned long guildVisitorSequence=1;
    enum VisitorDeparturePhase{VISITOR_LEAVING,VISITOR_CLEANUP};
    struct DepartingVisitors{std::vector<Character*> members;SavedActorResolution::Group restoredMembers;float cleanup;VisitorDeparturePhase phase;DepartingVisitors():cleanup(45.0f),phase(VISITOR_LEAVING){}};
    std::vector<DepartingVisitors> departingVisitors;
    float guildVisitorSpawnClock=45.0f;
    bool visitorOfferUrgent=false,visitorOfferVip=false,visitorOfferExceptional=false;
    int visitorOfferProfile=GuildVisitorOfferRules::Civilian;
    std::vector<int> recentGuildVisitorOfferTypes;
    bool isDepartingVisitor(Character* who){if(!who)return false;for(size_t d=0;d<departingVisitors.size();++d)for(size_t m=0;m<departingVisitors[d].members.size();++m)if(departingVisitors[d].members[m]==who)return true;return false;}
    void disableDepartingVisitorDialogue(Character* who){if(who&&who->dialogue)who->dialogue->pacakgesIHave.clear();}
    bool negotiationSuspended = false;
    Character* contractBarman = 0;
    hand contractBarmanHandle;
    Character* resolveContractBarman(){return contractBarmanHandle.isNull()?0:contractBarmanHandle.getCharacter();}
    TownBase* contractOriginTown = 0;
    float completedCleanupClock = 0.0f;
    Character* completedEscort = 0;
    SavedActorResolution::Actor completedEscortRestore;
    SavedActorResolution::Group completedCaravanRestore,caravanRestore,scientificRestore;
    struct RefusedDeparture {hand actor;Ogre::Vector3 rendezvous,target;float retry,resolutionRetry;RefusedDeparture():retry(0),resolutionRetry(0){}};
    std::vector<RefusedDeparture> refusedDepartures;
    std::vector<Character*> completedCaravan;
    std::string selectedProfileSquad;
    std::string selectedProfileName=Loc::text("ui.civilian");
    int selectedProfileGroupSize=1;
    int selectedProfileRarity=0;
    std::map<std::string, int> recentDestinations;
    MyGUI::Window* contractsWindow = 0;
    MyGUI::Button* contractButtons[6] = {0,0,0,0,0,0};
    MyGUI::TextBox* contractsDetails = 0;
    MyGUI::TextBox* contractsInfo[4] = {0,0,0,0};
    MyGUI::TextBox* contractsRewardAmount = 0;
    MyGUI::TextBox* contractsRewardReputation = 0;
    MyGUI::TextBox* contractsRewardBonus = 0;
    bool routeTestEnabled();
    void openRouteTest(MyGUI::Widget*);
    void prepareRoadTestContract(MyGUI::Widget*);
    bool playerIsCarryingEscort();
    MyGUI::TextBox* contractsMap = 0;
    MyGUI::ImageBox* contractsMapImage = 0;
    MyGUI::Widget* contractsMapPanel = 0;
    MyGUI::TextBox* contractsLegend = 0;
    MyGUI::Button* contractsAccept = 0;
    MyGUI::TextBox* contractsHeading=0;
    MyGUI::Button* missionBookBoardTabs[6]={0};
    MyGUI::ComboBox* missionBookCategoryCombo=0;
    MyGUI::Button* missionBookPagePrevious=0;MyGUI::Button* missionBookPageNext=0;
    MyGUI::TextBox* missionBookPageLabel=0;
    int missionBookCategoryFilter=MissionBookContracts::All,missionBookOfferPage=0;
    MyGUI::Widget* contractsListPanel=0;MyGUI::Widget* contractsInfoPanel=0;
    MyGUI::TextBox* missionBookBoardPageText=0;
    std::string missionBookSecurityOfferIds[6];
    std::string missionBookSecurityAreaNames[6];
    Ogre::Vector3 missionBookSecurityAreaCenters[6];
    float missionBookSecurityAreaRadii[6]={0,0,0,0,0,0};
    MyGUI::TextBox* contractsRefreshText = 0;
    MyGUI::Button* cityMarkers[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    MyGUI::ImageBox* routeCityIcons[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    hand contractDialogueGiver;
    bool contractBoardOpenedFromDialogue=false;
    std::vector<MyGUI::Widget*> contractRouteDots;
    void showContractPriceDetails(MyGUI::Widget*);
    struct MapAxisQuery {
        bool xAxis;Ogre::Vector3 fixed;
        MapAxisQuery(bool x,const Ogre::Vector3& p):xAxis(x),fixed(p){}
        int operator()(double value)const{Ogre::Vector3 p=fixed;if(xAxis)p.x=(float)value;else p.z=(float)value;iVector2 s=ou->zoneMgr->getMapSector(p);return xAxis?s.x:s.y;}
    };
    int selectedOffer = 0;
    bool contractAcceptArmed=false,delegationFinalArmed=false;
    bool missionBookPersonalAction=false;
    MyGUI::Widget* missionBookDelegationPanel=0;
    MyGUI::ComboBox* missionBookSoldierSort=0;
    MyGUI::Button* missionBookDelegateButton=0;
    MyGUI::TextBox* missionBookDelegationStatus=0;MyGUI::TextBox* missionBookSelectedTeam=0;MyGUI::TextBox* missionBookTimingText=0;
    struct MissionBookSoldierWidgets{MyGUI::Button* row;MyGUI::ImageBox* portrait;MyGUI::TextBox* name;MyGUI::TextBox* stats;MyGUI::TextBox* defence;MyGUI::TextBox* endurance;MyGUI::Button* check;MissionBookSoldierWidgets():row(0),portrait(0),name(0),stats(0),defence(0),endurance(0),check(0){}};
    std::vector<MissionBookSoldierWidgets> missionBookSoldierRows;
    void layoutMissionBookRoster();
    void refreshMissionBookTeamPortraits();
    void bookEllipsis(MyGUI::TextBox*);
    std::vector<size_t> missionBookSoldierOrder;
    int missionBookSoldierSortMode=MissionBookContracts::SortName;
    std::string missionBookRosterIdentity;
    int mapCropX=0,mapCropY=0,mapCropSize=2048,mapDragX=0,mapDragY=0;
    struct BoardOffer
    {
        std::string tier, townId, townName, profile, story, squadId, rarityName, routeRegions;
        float distance,danger;
        int estimatedPay, rarity, groupSize,missionType,tierIndex,source,dangerLevel,environmentTags,caravanSize,cargoClass,studyClass,studyDuration;
        bool available, prestigious;
        BoardOffer():distance(0),danger(1),estimatedPay(0),rarity(0),groupSize(1),missionType(0),tierIndex(0),source(MCS_TAVERN),dangerLevel(1),environmentTags(0),caravanSize(MCZ_SMALL),cargoClass(MCC_BASIC),studyClass(MSC_SMALL),studyDuration(MSD_SHORT),available(false),prestigious(false){}
    } boardOffers[6];
    bool boardOfferAvailable(const BoardOffer& offer){return offer.available;}
    struct CityContractBoard{BoardOffer offers[6];double expiresAt;CityContractBoard():expiresAt(0){}};
    std::map<std::string,CityContractBoard> savedContractBoards;
    std::string currentBoardKey;
    ContractReroll::Charges guildRerolls;
    void rerollBookRow(MyGUI::Widget*);
    std::string rerollTooltip();
    void openFactionDiplomacy(MyGUI::Widget*);
    void resetV9PendingActions();
    struct MissionBookPoolEntry {
        BoardOffer offer; int sourceIndex; bool security; std::string id,boardKey,areaName; hand issuer; float radius;
        MissionBookPoolEntry():sourceIndex(0),security(false),radius(0){}
    };
    std::vector<MissionBookPoolEntry> missionBookPool;
    std::vector<size_t> missionBookFiltered,missionBookPageEntries;
    std::string missionBookSelectedId,missionBookLegalBoardKey;
    hand missionBookLegalIssuer,missionBookSecurityIssuer;
    std::vector<hand> missionBookSecurityIssuers;
    std::map<std::string,hand> missionBookClassicSources;
    bool missionBookPoolReady=false;
    std::set<std::string> missionBookSelectedCharacters;
    double currentGameHours=0.0;
    struct DelegatedMissionState {
        int version,kind,reward,difficulty,sent,returned,injured,amputations,dead,guildXp,reputationDelta,paymentState;
        bool resultRolled,success,completed,reportShown;
        float successChance,successRoll;
        double completedAt;
        std::string groupId,offerIdentity,issuerIdentity,boardKey,title,origin,destination,officeKey,clientName,outcomeCode;
        DelegatedMissionTiming::State timing;
        DelegatedMissionState():version(4),kind(0),reward(0),difficulty(0),sent(0),returned(0),injured(0),amputations(0),dead(0),guildXp(0),reputationDelta(0),paymentState(0),resultRolled(false),success(false),completed(false),reportShown(false),successChance(0),successRoll(0),completedAt(0){}
        template<class Archive> void archive(Archive& a){a.field(version);a.field(kind);a.field(reward);a.field(offerIdentity);a.field(issuerIdentity);a.field(boardKey);a.field(title);a.field(origin);a.field(destination);timing.archive(a);if(!a.reading||version>=2){a.field(resultRolled);a.field(success);}else if(a.reading){resultRolled=true;success=true;}if(!a.reading||version>=3)a.field(groupId);else if(a.reading)groupId="legacy";if(!a.reading||version>=4){a.field(difficulty);a.field(sent);a.field(returned);a.field(injured);a.field(amputations);a.field(dead);a.field(guildXp);a.field(reputationDelta);a.field(paymentState);a.field(completed);a.field(reportShown);a.field(successChance);a.field(successRoll);a.field(completedAt);a.field(officeKey);a.field(clientName);a.field(outcomeCode);}else if(a.reading){difficulty=0;sent=returned=injured=amputations=dead=0;guildXp=35;reputationDelta=success?1:-1;paymentState=completed=reportShown=0;successChance=successRoll=0;completedAt=0;version=4;}if(a.reading&&((version<1||version>4)||kind<0||kind>2||reward<0||paymentState<0||paymentState>3||(timing.active&&!resultRolled)))throw std::runtime_error("invalid delegated mission");}
        void clear(){*this=DelegatedMissionState();}
    } delegatedMission;
    std::vector<DelegatedMissionState> delegatedMissions;
    MyGUI::Window* delegatedReportWindow=0;
    MyGUI::TextBox* delegatedReportText=0;
    MyGUI::Button* delegatedReportCloseButton=0;
    int delegatedReportIndex=-1;
    bool delegatedReportWasPaused=false;
    std::vector<MailContracts::Contract> mailContracts;
    bool contractBoardsLoaded=false;
    int displayedContractCats(int baseCats){return ContractRewards::apply(baseCats,contractRewardPercent);}
    void applyContractRewardSnapshot(EscortContractData& contract){
        const int percent=ContractRewards::snapshot(contract.routeRegions);
        contract.basePay=ContractRewards::apply(contract.basePay,percent);
        contract.totalPay=ContractRewards::apply(contract.totalPay,percent);
        contract.advance=ContractRewards::apply(contract.advance,percent);
        contract.finalPay=contract.totalPay-contract.advance;
    }
    float contractBoardUiClock=0.0f;
    std::string contractBoardsFile="mods/Guild Escort Contracts/GuildEscortBoards.dat";
    const char* mapTownIds[14]={"49386-rebirth.mod","49368-rebirth.mod","12628-Newwworld.mod","18021-Newwworld.mod","18022-Newwworld.mod","18925-Newwworld.mod","18919-Newwworld.mod","44854-rebirth.mod","55656-rebirth.mod","1078-gamedata.base","1076-gamedata.base","55651-rebirth.mod","1079-gamedata.base","2608-gamedata.base"};
    const char* mapTownNames[14]={"World's End","Flotsam","Blister Hill","Bad Teeth","Mongrel","Stack","The Hub","Waystation","Black Scratch","Heft","Bark","Sho-Battai","Squin","Admag"};

    std::string cleanBoardField(std::string value){std::replace(value.begin(),value.end(),'|','/');std::replace(value.begin(),value.end(),'\n',' ');return value;}
    std::string readMissionFile(const std::string&);
    bool atomicMissionFile(const std::string&,const std::string&);
    bool mercenarieFreshWorld=false;
    bool mercenarieExplicitSlot=false;
    std::string mercenarieExplicitRoot,mercenarieExplicitName;
    void requestMercenarieDataSlot(const std::string& root,const std::string& name){
        mercenarieExplicitSlot=true;mercenarieExplicitRoot=root;mercenarieExplicitName=name;
        mercenarieFreshWorld=false;
    }
    bool configureMercenarieDataSlot()
    {
        if(MercenarieCleanup::disabled)return false;
        SaveManager* manager=SaveManager::getSingleton();std::string game=mercenarieExplicitSlot?mercenarieExplicitName:manager?manager->getCurrentGame():"";
        std::string root=mercenarieExplicitSlot?mercenarieExplicitRoot:manager?manager->getSavePath():"";
        if(mercenarieFreshWorld)game="__unsaved__";
        if(game.empty())game="__unsaved__";
        const std::string slot=game=="__unsaved__"?game:GuildSavePaths::directory(root,game);
        if(slot==activeMercenarieSaveSlot)return false;
        // Save As changes the destination, not the working session. Load/new
        // game/import explicitly clear activeMercenarieSaveSlot for a new world.
        if(!activeMercenarieSaveSlot.empty()&&!reputationFile.empty()&&!fiscalFile.empty()&&!contractBoardsFile.empty()){
            activeMercenarieSaveSlot=slot;return false;
        }
        activeMercenarieSaveSlot=slot;
        const std::string directory=GuildSavePaths::sessionDirectory();
        const UnicodeFileSystem::Result staged=GuildSavePaths::stageSession(game=="__unsaved__"?"":slot,directory);
        if(!staged.ok){
            progressWriteBlocked=true;progressLoadFault=true;
            const SaveDiagnostics::Error error(saveIOFailure(staged));
            reportPersistenceException("LOAD",error,staged.path);
            // Never fall back to a previous world's files after a path failure.
            fiscalFile.clear();contractBoardsFile.clear();reputationFile.clear();return true;
        }
        fiscalFile=directory+"/GuildEscortFiscal.dat";contractBoardsFile=directory+"/GuildEscortBoards.dat";reputationFile=directory+"/GuildEscortReputation.dat";
        // No implicit migration from a different world's global files.
        return true;
    }
    const char* fiscalOrgName(FiscalOrganisation o){return o==FISCAL_UC?Loc::text("ui.united_cities_4393b61"):Loc::text("ui.mercenary_guild");}
    const char* fiscalStateName(FiscalState s){switch(s){case FSTATE_NORMAL:return Loc::text("common.normale");case FSTATE_COLLECTION_DUE:return Loc::text("ui.collection_due");case FSTATE_EXTENSION:return Loc::text("ui.extension_granted");case FSTATE_WARNING:return Loc::text("ui.final_warning");case FSTATE_WAITING_FOR_PLAYER:return Loc::text("ui.waiting_for_player");case FSTATE_COLLECTOR_TRAVELLING:return Loc::text("ui.collector_on_the_way");case FSTATE_DIALOGUE:return Loc::text("ui.payment_request");case FSTATE_RAID_1_PENDING:return Loc::text("ui.raid_1_approaching");case FSTATE_RAID_1_COMBAT:return Loc::text("ui.raid_1_in_combat");case FSTATE_RAID_1_DEFEATED:return Loc::text("ui.raid_1_repelled");case FSTATE_RAID_2_PENDING:return Loc::text("ui.raid_2_approaching");case FSTATE_RAID_2_COMBAT:return Loc::text("ui.raid_2_in_combat");case FSTATE_RAID_2_DEFEATED:return Loc::text("ui.raid_2_repelled");case FSTATE_RAID_3_PENDING:return Loc::text("ui.raid_3_approaching");case FSTATE_RAID_3_COMBAT:return Loc::text("ui.raid_3_in_combat");case FSTATE_PLAYER_DEFEATED:return Loc::text("ui.player_defeated");case FSTATE_REBEL:return Loc::text("ui.tax_rebel");default:return Loc::text("common.inconnu");}}
    void saveFiscalLedger()
    {
        if(savePreparing||MercenarieCleanup::disabled)return;
        if(fiscalFile.empty())return;std::ostringstream out;
        FiscalLedgerFormat::write(out,fiscalLedger);
        atomicMissionFile(fiscalFile,out.str());
    }

#include "FinanceRuntime.h"
#include "GuildPayrollRuntime.h"
    void loadFiscalLedger()
    {
        try{
        std::string bytes=readMissionFile(fiscalFile);if(bytes.empty())return;std::istringstream in(bytes);
        FiscalLedgerFormat::read(in,fiscalLedger);
    
        }catch(const std::exception& e){progressWriteBlocked=true;progressLoadFault=true;reportPersistenceException("LOAD",e,fiscalFile);}
    }
    FiscalRelation actualFiscalRelation(FiscalOrganisation o){FiscalOrganisationState& state=fiscalLedger.organisations[(int)o];if(state.rebel)return FREL_REBEL;if(!ou||!ou->player||!ou->player->participant||!ou->factionMgr)return FREL_NORMAL;Faction* target=ou->factionMgr->getFactionByStringID(o==FISCAL_UC?"defaultEmpireFactionSID":"1214-gamedata.base");return target&&target->relations&&target->relations->isAlly(ou->player->participant)?FREL_ALLIED:FREL_NORMAL;}
    bool townOwnedByUC(TownBase* town){if(!town||!ou||!ou->factionMgr)return false;Faction* uc=ou->factionMgr->getFactionByStringID("defaultEmpireFactionSID");return uc&&town->getFaction()==uc;}
    std::string makeMissionFiscalId(){return allocateQuestFiscalId();}
    void createFiscalEntryForSuccess(int gross,int bonuses,int tips){if(currentMissionFiscalId.empty())currentMissionFiscalId=makeMissionFiscalId();if(fiscalLedger.contains(currentMissionFiscalId))return;TownBase* destinationTaxTown=shou&&shou->townList?shou->townList->getTownBySID(destinationTown):0;bool ucTaxable=townOwnedByUC(contractOriginTown)||townOwnedByUC(destinationTaxTown);FiscalRelation uc=actualFiscalRelation(FISCAL_UC),mg=actualFiscalRelation(FISCAL_MERCENARY_GUILD);int taxable=contractTaxBase(currentContract.totalPay,gross,bonuses,tips);fiscalLedger.create(currentMissionFiscalId,scientificMission?Loc::text("ui.scientific_expedition_e4b8223"):caravanMission?Loc::text("ui.caravan_escort_a4c7a93"):Loc::text("ui.escort_2e246e4"),currentContract.rarity==MCR_EPIC?"Epique":currentContract.rarity==MCR_RARE?"Rare":Loc::text("common.commun"),currentContract.source==MCS_GUILD_HOUSE?Loc::text("ui.guild_house_6620bc7"):Loc::text("common.taverne"),originCity,destinationName,currentGameHours,gross,bonuses,tips,taxable,ucTaxable,uc,mg);for(int i=0;i<2;++i){FiscalOrganisationState& s=fiscalLedger.organisations[i];if(s.debt>0&&s.nextCollectionHour<=0)s.nextCollectionHour=currentGameHours+(guildHouseOperational?48.0:168.0);}saveFiscalLedger();}
    void recordFiscalFailure(FiscalEntryStatus status){if(currentMissionFiscalId.empty())currentMissionFiscalId=makeMissionFiscalId();fiscalLedger.recordNoTax(currentMissionFiscalId,scientificMission?Loc::text("ui.scientific_expedition_e4b8223"):caravanMission?Loc::text("ui.caravan_escort_a4c7a93"):Loc::text("ui.escort_2e246e4"),originCity,destinationName,currentGameHours,status);saveFiscalLedger();}
    #include "FactionWarRuntime.h"
    std::string serializeContractBoards()
    {
        std::ostringstream out;
        out.imbue(std::locale::classic());out<<std::setprecision(17)<<"@version|3\n";
        out<<"@rerolls|"<<guildRerolls.encode()<<'\n';
        for(std::map<std::string,CityContractBoard>::const_iterator it=savedContractBoards.begin();it!=savedContractBoards.end();++it){out<<"@board|"<<cleanBoardField(it->first)<<'|'<<it->second.expiresAt<<'\n';for(int i=0;i<6;++i){const BoardOffer& o=it->second.offers[i];out<<"@offer|"<<cleanBoardField(it->first)<<'|'<<i<<'|'<<o.missionType<<'|'<<o.tierIndex<<'|'<<cleanBoardField(o.tier)<<'|'<<cleanBoardField(o.townId)<<'|'<<cleanBoardField(o.townName)<<'|'<<cleanBoardField(o.profile)<<'|'<<cleanBoardField(o.story)<<'|'<<cleanBoardField(o.squadId)<<'|'<<cleanBoardField(o.rarityName)<<'|'<<o.distance<<'|'<<o.danger<<'|'<<o.estimatedPay<<'|'<<o.rarity<<'|'<<o.groupSize<<'|'<<(o.available?1:0)<<'|'<<(o.prestigious?1:0)<<'|'<<o.source<<'|'<<o.dangerLevel<<'|'<<o.environmentTags<<'|'<<o.caravanSize<<'|'<<o.cargoClass<<'|'<<o.studyClass<<'|'<<o.studyDuration<<'|'<<cleanBoardField(o.routeRegions)<<'\n';}}
        if(!out.good())throw std::runtime_error("boards serialization failed");SaveText::boards(out.str());return out.str();
    }
    void saveContractBoards(){if(savePreparing||MercenarieCleanup::disabled||contractBoardsFile.empty())return;atomicMissionFile(contractBoardsFile,serializeContractBoards());}
    void loadContractBoards()
    {
        try{
        if(contractBoardsLoaded)return;contractBoardsLoaded=true;savedContractBoards.clear();guildRerolls=ContractReroll::Charges();std::string bytes=readMissionFile(contractBoardsFile);SaveText::boards(bytes);std::istringstream in(bytes);std::string line;
        while(std::getline(in,line)){std::stringstream s(line);std::string tag,city,v;std::getline(s,tag,'|');std::getline(s,city,'|');if(tag=="@rerolls"){std::string rest;std::getline(s,rest);if(!guildRerolls.decode(city+"|"+rest)){guildRerolls.remaining=0;guildRerolls.started=currentGameHours;guildRerolls.ends=currentGameHours+24;ErrorLog("V9 invalid reroll state: fail closed");}continue;}if(tag=="@version")continue;if(tag=="@board"){std::getline(s,v,'|');savedContractBoards[city].expiresAt=atof(v.c_str());continue;}if(tag!="@offer")continue;std::string fields[25];for(int i=0;i<25;++i)std::getline(s,fields[i],'|');int index=atoi(fields[0].c_str());if(index<0||index>=6)continue;BoardOffer& o=savedContractBoards[city].offers[index];o.missionType=atoi(fields[1].c_str());o.tierIndex=atoi(fields[2].c_str());o.tier=fields[3];o.townId=fields[4];o.townName=fields[5];o.profile=fields[6];o.story=fields[7];o.squadId=fields[8];o.rarityName=fields[9];o.distance=std::max(0.0f,static_cast<float>(atof(fields[10].c_str())));o.danger=static_cast<float>(atof(fields[11].c_str()));o.estimatedPay=std::max(0,atoi(fields[12].c_str()));o.rarity=std::max(0,std::min(2,atoi(fields[13].c_str())));o.groupSize=std::max(1,std::min(EscortConfig::MaxCaravanMembers,atoi(fields[14].c_str())));o.available=atoi(fields[15].c_str())!=0;o.prestigious=atoi(fields[16].c_str())!=0;if(!fields[17].empty()){o.source=atoi(fields[17].c_str());o.dangerLevel=std::max(1,std::min(5,atoi(fields[18].c_str())));o.environmentTags=atoi(fields[19].c_str());o.caravanSize=atoi(fields[20].c_str());o.cargoClass=atoi(fields[21].c_str());o.studyClass=atoi(fields[22].c_str());o.studyDuration=atoi(fields[23].c_str());o.routeRegions=fields[24];}else{o.source=city.find("#GUILD_HOUSE")!=std::string::npos?MCS_GUILD_HOUSE:MCS_TAVERN;o.dangerLevel=o.danger>=1.8f?5:o.danger>=1.48f?4:o.danger>=1.25f?3:o.danger>=1.08f?2:1;o.rarity=o.rarity>1?MCR_EPIC:o.rarity>0?MCR_RARE:MCR_COMMON;}if(o.rarity==MCR_LEGENDARY)o.rarity=MCR_EPIC;
            refreshOfferRegions(o);
            // Expire invalid unaccepted legacy offers without touching active quests.
            if(o.available&&(ContractDestinationRules::blocked(o.townId)||(o.missionType==MCT_SCIENCE&&!ContractDestinationRules::scientific(o.townId)))){o.available=false;savedContractBoards[city].expiresAt=0;}
        }
    
        }catch(const std::exception& e){progressWriteBlocked=true;progressLoadFault=true;reportPersistenceException("LOAD",e,contractBoardsFile);}
    }

    void resetContractBoardWorld(){
        savedContractBoards.clear();contractBoardsLoaded=false;guildRerolls=ContractReroll::Charges();
        currentBoardKey.clear();for(int i=0;i<6;++i)boardOffers[i]=BoardOffer();
        currentGameHours=0;developerTimeOffsetHours=0;resetV9PendingActions();
    }

    std::string frenchPlaceName(const std::string& name)
    {
        // TownBase supplies the active native Kenshi name.
        return name;
    }

    bool atomicMissionFile(const std::string&,const std::string&);
    std::string serializeReputations()
    {
        std::ostringstream out;out.imbue(std::locale::classic());
        out<<"@progress|4\n";
        out<<"@investmentNext|"<<std::setprecision(17)<<guildInvestmentNextHour<<'\n';
        out<<"@contractreward|"<<contractRewardPercent<<'\n';
        for(size_t i=0;i<archivedReputationAliases.size();++i)out<<"@legacycity|"<<archivedReputationAliases[i]<<'\n';
        for(std::set<std::string>::const_iterator paid=rewardedContractIds.begin();paid!=rewardedContractIds.end();++paid)out<<"@paid|"<<*paid<<'\n';
        out << "@stats|" << successfulContracts << '|' << failedContracts << '|' << totalContractCats << '|' << escortReputation << '|' << guildPoints << '|' << guildPrestige << '|' << totalAdvances << '|' << totalBonuses << '|' << totalTips << '\n';
        if(!designatedGuildHouseKey.empty())out<<"@designated|"<<designatedGuildHouseKey<<'\n';
        if(!operationalAnnouncementKey.empty())out<<"@operational|"<<operationalAnnouncementKey<<'\n';
        for(std::map<std::string,std::string>::const_iterator h=guildHouseNames.begin();h!=guildHouseNames.end();++h)out<<"@house|"<<h->first<<'|'<<h->second<<'|'<<(guildHouseCities.count(h->first)?guildHouseCities[h->first]:Loc::text("ui.unknown_city"))<<'\n';
        for(std::map<std::string,std::string>::const_iterator h=guildHouseOriginalNames.begin();h!=guildHouseOriginalNames.end();++h)if(guildHouseNames.count(h->first))out<<"@housemeta|"<<h->first<<"|office|"<<h->second<<'\n';
        for (unsigned int i = 0; i < contractHistory.size(); ++i) out << "@history|" << contractHistory[i] << '\n';
        for(std::map<std::string,GuildHistory::Snapshot>::const_iterator i=contractSeeds.begin();i!=contractSeeds.end();++i)out<<"@contractseed|"<<GuildHistory::encode(i->second)<<'\n';
        for (std::map<std::string, float>::const_iterator it = localReputations.begin(); it != localReputations.end(); ++it)
        {
            CityMemory memory = cityMemories[it->first];
            out << it->first << '|' << it->second << '|' << memory.abuses << '|' << memory.recovery << '\n';
        }
        return GuildProgression::sealProgress(out.str());
    }

    void saveReputations()
    {
        if(savePreparing||progressWriteBlocked||MercenarieCleanup::disabled)return;
        static bool warned=false;
        if(atomicMissionFile(reputationFile,serializeReputations())){warned=false;return;}
        ErrorLog("Guild Escort: guild progression write failed: "+reputationFile);
        if(!warned){warned=true;reportPersistenceError("PROGRESS",SaveDiagnostics::ProgressWriteFailed,"guild progression write failed",reputationFile,"ui.warning_guild_progress_could_not_be_written_keep_your");}
    }

    void readReputations(std::istream& in)
    {
        std::string bytes((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
        if(!GuildProgression::validProgress(bytes)){progressWriteBlocked=true;ErrorLog("Guild progress: invalid stats; refusing reset/write");reportPersistenceError("PROGRESS",SaveDiagnostics::ProgressInvalid,"invalid stats/checksum/version or progression limits",reputationFile);return;}
        int progressVersion=bytes.find("@progress|4")!=std::string::npos?4:bytes.find("@progress|3")!=std::string::npos?3:bytes.find("@progress|2")!=std::string::npos?2:1;bool modern=progressVersion>=2;
        if(bytes.find("@progress|")!=std::string::npos&&!modern){progressWriteBlocked=true;ErrorLog("Guild progress: unsupported version");return;}
        std::istringstream parsed(bytes);parsed.imbue(std::locale::classic());
        contractRewardPercent=100;guildInvestmentNextHour=0;rewardedContractIds.clear();
        contractHistory.clear();contractSeeds.clear();localReputations.clear();cityMemories.clear();archivedReputationAliases.clear();guildHouseNames.clear();guildHouseCities.clear();guildHouseOriginalNames.clear();designatedGuildHouseKey.clear();operationalAnnouncementKey.clear();
        std::string line;
        while (std::getline(parsed, line))
        {
            if(!line.empty()&&line[line.size()-1]=='\r')line.erase(line.size()-1);
            if(line.find("@progress|")==0)continue;
            if(line.find("@checksum|")==0)continue;
            if(line.find("@legacycity|")==0){archivedReputationAliases.push_back(line.substr(12));continue;}
            if(line.find("@paid|")==0){rewardedContractIds.insert(line.substr(6));continue;}
            if(line.find("@investmentNext|")==0){double value=0;if(GuildProgression::number(line.substr(16),value)&&value>=0)guildInvestmentNextHour=value;continue;}
            if(line.find("@contractreward|")==0){int value=atoi(line.substr(16).c_str());contractRewardPercent=(value>=50&&value<=150&&value%10==0)?value:100;continue;}
            if (line.find("@stats|") == 0)
            {
                std::stringstream stats(line.substr(7)); std::string a,b,c,d,e,f,g,h,i;
                std::getline(stats,a,'|'); std::getline(stats,b,'|'); std::getline(stats,c,'|'); std::getline(stats,d,'|');std::getline(stats,e,'|');
                std::getline(stats,f,'|');std::getline(stats,g,'|');std::getline(stats,h,'|');std::getline(stats,i,'|');
                successfulContracts=atoi(a.c_str()); failedContracts=atoi(b.c_str()); totalContractCats=_atoi64(c.c_str()); escortReputation=GuildProgression::decimal(d);guildPoints=e.empty()?(int)std::min((long long)INT_MAX,(long long)successfulContracts*50):std::max(0,atoi(e.c_str()));guildPrestige=f.empty()?0:atoi(f.c_str());totalAdvances=g.empty()?0:_atoi64(g.c_str());totalBonuses=h.empty()?0:_atoi64(h.c_str());totalTips=i.empty()?0:_atoi64(i.c_str()); continue;
            }
            if(line.find("@housemeta|")==0){std::stringstream row(line.substr(11));std::string key,type,original;if(std::getline(row,key,'|')&&std::getline(row,type,'|')&&std::getline(row,original))guildHouseOriginalNames[key]=original;continue;}
            if(line.find("@house|")==0){std::stringstream house(line.substr(7));std::string key,name,city;if(std::getline(house,key,'|')&&std::getline(house,name,'|')){guildHouseNames[key]=name;if(std::getline(house,city,'|')&&!city.empty())guildHouseCities[key]=city;if(name.empty())guildHouseNames[key]=std::string(Loc::text("ui.guild_house_of"))+city;}continue;}
            if(line.find("@designated|")==0){designatedGuildHouseKey=line.substr(12);continue;}
            if(line.find("@operational|")==0){operationalAnnouncementKey=line.substr(13);continue;}
            if(line.find("@contractseed|")==0){GuildHistory::Snapshot seed;if(GuildHistory::decode(line.substr(14),seed)&&seed.status==0)contractSeeds[seed.id]=seed;continue;}
            if (line.find("@history|") == 0) { contractHistory.push_back(line.substr(9)); continue; }
            std::stringstream parser(line); std::string city, rep, abuses, recovery;
            if (!std::getline(parser, city, '|') || !std::getline(parser, rep, '|')) continue;
            std::getline(parser, abuses, '|'); std::getline(parser, recovery, '|');
            localReputations[city] = GuildProgression::decimal(rep);
            cityMemories[city].abuses = abuses.empty() ? 0 : atoi(abuses.c_str());
            cityMemories[city].recovery = recovery.empty() ? 0 : atoi(recovery.c_str());
        }
        if(progressVersion<4){int oldCap=progressVersion==3?42000:progressVersion==2?12500:10000;if(guildPoints>oldCap)guildPrestige=(int)std::min((long long)INT_MAX,(long long)guildPrestige+guildPoints-oldCap);guildPoints=GuildProgression::migrate(guildPoints,progressVersion);}
        guildPoints=std::max(0,std::min(GuildProgression::MaxXp,guildPoints));guildPrestige=std::max(0,guildPrestige);
        escortReputation=GuildProgression::clampRep(escortReputation);
        for(std::map<std::string,float>::iterator r=localReputations.begin();r!=localReputations.end();++r)r->second=GuildProgression::clampRep(r->second);
        ReputationIdentity::migrate(localReputations,cityMemories,archivedReputationAliases);
        progressWriteBlocked=false;
    }

    void loadReputations()
    {
        try{
        if(reputationFile.empty()){progressWriteBlocked=true;return;}
        std::string primary=readMissionFile(reputationFile);bool hasPrimary=UnicodeFileSystem::exists(reputationFile);
        if(hasPrimary){std::istringstream in(primary);readReputations(in);if(!progressWriteBlocked)return;}
        std::string backupBytes=readMissionFile(reputationFile+".bak");bool hasBackup=UnicodeFileSystem::exists(reputationFile+".bak");
        if(hasBackup){std::istringstream backup(backupBytes);readReputations(backup);if(!progressWriteBlocked){ErrorLog("Guild progress: recovered backup "+reputationFile);return;}}
        // A virgin flag may be stale, but it must never hide an existing file.
        if(!hasPrimary&&!hasBackup&&(progressVirginWorld||mercenarieFreshWorld||activeMercenarieSaveSlot=="__unsaved__")){
            progressWriteBlocked=false;
            if(!atomicMissionFile(reputationFile,serializeReputations())){progressWriteBlocked=true;ErrorLog("Guild Escort: cannot initialize progression: "+reputationFile);return;}
            saveFiscalLedger();saveContractBoards();progressVirginWorld=false;return;
        }
        progressWriteBlocked=true;ErrorLog("Guild progress: no readable progress file "+reputationFile);reportPersistenceError("PROGRESS",SaveDiagnostics::ProgressUnavailable,"no readable progress file",reputationFile);
    
        }catch(const std::exception& e){progressWriteBlocked=true;progressLoadFault=true;reportPersistenceException("LOAD",e,reputationFile);}
    }

    #include "ContractSnapshotRuntime.h"
    void addHistory(const std::string& result)
    {
        GuildHistory::Summary legacy=GuildHistory::parse(result);
        archiveEscort(legacy.status==1?1:2,legacy.bonus,-1);
    }

    float localReputation()
    {
        std::map<std::string,float>::const_iterator found=localReputations.find(ReputationIdentity::key(originCity));
        return found==localReputations.end()?0.0f:found->second;
    }

    std::string reputationRegionFor(const std::string& place)
    {
        if(place=="The Hub"||place=="Le Hub"||place=="Stack"||place=="Waystation"||place=="Avant-poste")return "Zone: Zone frontaliere";
        if(place=="Mongrel")return "Zone: Iles brumeuses";
        if(place=="Squin"||place=="Admag")return "Zone: Desert de Stenn";
        if(place=="Heft"||place=="Bark"||place=="Sho-Battai")return "Zone: Grand Desert";
        if(place=="Black Scratch"||place=="Griffure Noire")return "Zone: Terres exterieures";
        if(place=="World's End"||place=="Flotsam"||place=="Flotsam Village"||place=="Village Pecheurs"||place=="Village Pêcheurs")return "Zone: Cote Nord";
        if(place=="Blister Hill"||place=="Bad Teeth")return "Zone: Territoire d'Okran";
        return "";
    }

    void changeLocalReputation(float amount, const char* reason, bool persist=true, bool notify=true)
    {
        float& rep = localReputations[ReputationIdentity::key(originCity)];
        rep += amount; if (rep > 100.0f) rep = 100.0f; if (rep < -100.0f) rep = -100.0f;
        // Region entries from legacy saves are retained, but new results affect only the origin town.
        if(persist)saveReputations();
        if(!notify)return;
        char message[260];
        sprintf_s(message, mercenarieLocalize(Loc::text("ui.local_reputation_s_1f_s_total_1f")).c_str(), originCity.c_str(), amount, reason, rep);
        if (ou) ou->showPlayerAMessage(message, true);
    }

    int guildLevel();

    std::string guildHouseKey(Building* building)
    {
        if(!building)return "";Ogre::Vector3 p=building->getPosition();char keyText[96];sprintf_s(keyText,"%d:%d",static_cast<int>(p.x/10.0f),static_cast<int>(p.z/10.0f));return keyText;
    }

    void scanGuildFurniture();
    std::string guildFiscalQueueKey();
    void refreshGuildSeatQueue(bool developerForced=false);
    void seatGuildVisitors(bool developerForced=false);
    void refreshGuildFurniture(bool rebindOffice);
    void scheduleGuildFurnitureRecovery();
    bool ensureGuildHouseCityAvailable(Building* candidate,const std::string& candidateKey,std::string& city,bool cleanStale);

    void addMercenarieFramePart(MyGUI::Widget* parent,const char* name,const MyGUI::IntCoord& destination,const MyGUI::IntCoord& source,MyGUI::Align align)
    {
        MyGUI::ImageBox* part=parent->createWidget<MyGUI::ImageBox>("ImageBox",destination,align,name);
        part->setImageTexture("GuildMenuFrame.png");part->setImageCoord(source);part->setNeedMouseFocus(false);
    }

    void applyMercenarieFrame(MyGUI::Widget* parent,bool decorated)
    {
        if(!parent)return;
        Ogre::ResourceGroupManager& resources=Ogre::ResourceGroupManager::getSingleton();
        if(!resources.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))resources.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");
        const int w=parent->getWidth(),h=parent->getHeight();
        const int border=std::max(12,std::min(32,std::min(w,h)/10));
        const int sourceBorder=58,sourceWidth=1672,sourceHeight=941;
        addMercenarieFramePart(parent,"MercFrameCentre",MyGUI::IntCoord(border,border,w-border*2,h-border*2),MyGUI::IntCoord(sourceBorder,sourceBorder,sourceWidth-sourceBorder*2,sourceHeight-sourceBorder*2),MyGUI::Align::Stretch);
        addMercenarieFramePart(parent,"MercFrameTop",MyGUI::IntCoord(border,0,w-border*2,border),MyGUI::IntCoord(sourceBorder,0,sourceWidth-sourceBorder*2,sourceBorder),MyGUI::Align::HStretch|MyGUI::Align::Top);
        addMercenarieFramePart(parent,"MercFrameBottom",MyGUI::IntCoord(border,h-border,w-border*2,border),MyGUI::IntCoord(sourceBorder,sourceHeight-sourceBorder,sourceWidth-sourceBorder*2,sourceBorder),MyGUI::Align::HStretch|MyGUI::Align::Bottom);
        addMercenarieFramePart(parent,"MercFrameLeft",MyGUI::IntCoord(0,border,border,h-border*2),MyGUI::IntCoord(0,sourceBorder,sourceBorder,sourceHeight-sourceBorder*2),MyGUI::Align::VStretch|MyGUI::Align::Left);
        addMercenarieFramePart(parent,"MercFrameRight",MyGUI::IntCoord(w-border,border,border,h-border*2),MyGUI::IntCoord(sourceWidth-sourceBorder,sourceBorder,sourceBorder,sourceHeight-sourceBorder*2),MyGUI::Align::VStretch|MyGUI::Align::Right);
        addMercenarieFramePart(parent,"MercFrameTL",MyGUI::IntCoord(0,0,border,border),MyGUI::IntCoord(0,0,sourceBorder,sourceBorder),MyGUI::Align::Left|MyGUI::Align::Top);
        addMercenarieFramePart(parent,"MercFrameTR",MyGUI::IntCoord(w-border,0,border,border),MyGUI::IntCoord(sourceWidth-sourceBorder,0,sourceBorder,sourceBorder),MyGUI::Align::Right|MyGUI::Align::Top);
        addMercenarieFramePart(parent,"MercFrameBL",MyGUI::IntCoord(0,h-border,border,border),MyGUI::IntCoord(0,sourceHeight-sourceBorder,sourceBorder,sourceBorder),MyGUI::Align::Left|MyGUI::Align::Bottom);
        addMercenarieFramePart(parent,"MercFrameBR",MyGUI::IntCoord(w-border,h-border,border,border),MyGUI::IntCoord(sourceWidth-sourceBorder,sourceHeight-sourceBorder,sourceBorder,sourceBorder),MyGUI::Align::Right|MyGUI::Align::Bottom);
        if(decorated&&w>=700&&h>=360){int logoSize=std::min(120,h/4);MyGUI::ImageBox* logo=parent->createWidget<MyGUI::ImageBox>("ImageBox",w-border-logoSize-8,border+8,logoSize,logoSize,MyGUI::Align::Right|MyGUI::Align::Top,"MercFrameGuildLogo");logo->setImageTexture("GuildTeamLogo.png");logo->setAlpha(0.18f);logo->setNeedMouseFocus(false);}
    }

    #include "GuildBuildingDialogs.h"

    void confirmGuildHouseName(MyGUI::WidgetPtr)
    {
        Building* building=pendingGuildHouseHandle.isNull()?0:pendingGuildHouseHandle.getBuilding();
        if(!guildNameEdit||pendingGuildHouseKey.empty())return;
        const std::string key=pendingGuildHouseKey;
        if(guildNameIsRename){
            if(!guildHouseNames.count(key)) {gbClose(0,"");return;}
            std::string city=guildHouseCities.count(key)?guildHouseCities[key]:Loc::text("ui.unknown_city");
            std::string name=GuildBuildingTypes::cleanName(guildNameEdit->getCaption(),std::string(Loc::text("ui.guild_house_of"))+city);
            guildHouseNames[key]=name;if(building&&!building->isDestroyed()&&building->isThePlayer()&&guildHouseKey(building)==key)building->setName(name);
            if(currentGuildHouseKey==key)currentGuildHouseName=name;
            saveReputations();gbClose(0,"");refreshOfficesView(true);return;
        }
        std::string reason=gbReason(building);if(!reason.empty()){if(ou)ou->showPlayerAMessage(reason,true);gbClose(0,"");return;}
        std::string city;if(!ensureGuildHouseCityAvailable(building,key,city,false))return;
        std::string name=GuildBuildingTypes::cleanName(guildNameEdit->getCaption(),std::string(Loc::text("ui.guild_house_of"))+city);
        guildHouseOriginalNames[key]=GuildBuildingTypes::cleanName(building->getName(),"");building->setName(name);
        designatedGuildHouseKey=key;designatedGuildHouseHandle=building;guildBuilding=building;
        currentGuildHouseKey=key;guildHouseNames[key]=name;guildHouseCities[key]=city;currentGuildHouseName=name;
        operationalAnnouncementKey.clear();saveReputations();gbClose(0,"");refreshGuildFurniture(false);refreshOfficesView(true);
    }
    void closeGuildHouseName(MyGUI::Window*,const std::string&){gbClose(0,"");}
    void requestGuildHouseName()
    {
        if(pendingGuildHouseKey.empty()||!MyGUI::Gui::getInstancePtr())return;
        gbnEnsure();if(gbLive(guildNameWindow)){gbHide(guildNameWindow);gbnFieldFrame=0;MyGUI::Gui::getInstance().destroyWidget(guildNameWindow);}
        const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();GuildNameLayout l(view.width,view.height);
        guildNameWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("MercenarieBuildingWindow",(view.width-l.w)/2,(view.height-l.h)/2,l.w,l.h,MyGUI::Align::Default,"Popup");
        gbnDraw(guildNameWindow->getClientWidget(),l,guildNameIsRename);MyGUI::InputManager::getInstance().addWidgetModal(guildNameWindow);
        Building* target=pendingGuildHouseHandle.isNull()?0:pendingGuildHouseHandle.getBuilding();TownBase* town=target?target->getCurrentTownLocation():0;
        std::string city=town?frenchPlaceName(town->getName()):Loc::text("ui.unknown_city");
        std::string suggested=guildNameIsRename?guildHouseNames[pendingGuildHouseKey]:std::string(Loc::text("ui.guild_house_of"))+city;
        guildNameEdit->setCaption(suggested);guildNameEdit->setTextCursor(guildNameEdit->getTextLength());MyGUI::InputManager::getInstance().setKeyFocusWidget(guildNameEdit);
    }

    Building* selectedPlayerBuilding()
    {
        if(!ou||!ou->player||ou->player->selectedObject.isNull())return 0;
        Building* selected=ou->player->selectedObject.getBuilding();
        if(!selected||selected->isDestroyed()||!selected->isThePlayer())return 0;
        Building* parent=selected->furnitureParentBuilding();
        Building* candidate=parent&&parent->isThePlayer()?parent:selected;
        if(candidate->isFurnitureOrDoor()||candidate->isGate()||!candidate->getBuildState()||!candidate->getBuildState()->isComplete)return 0;
        return candidate;
    }

    bool ensureGuildHouseCityAvailable(Building* candidate,const std::string& candidateKey,std::string& city,bool cleanStale)
    {
        city.clear();if(!candidate||!ou||!ou->zoneMgr)return false;TownBase* location=candidate->getCurrentTownLocation();
        if(!location||!location->isTown()){if(ou)ou->showPlayerAMessage(Loc::text("ui.this_office_cannot_be_registered_kenshi_does_not_recognize"),true);return false;}
        city=frenchPlaceName(location->getName());if(city.empty()||city=="Ville inconnue"){if(ou)ou->showPlayerAMessage(Loc::text("ui.this_office_cannot_be_registered_kenshi_does_not_recognize"),true);return false;}
        std::vector<std::string> staleKeys;lektor<Building*> townBuildings;ou->zoneMgr->findAllBuildings(townBuildings,location,ou->player?ou->player->getFaction():0,false,0,0);
        for(std::map<std::string,std::string>::const_iterator it=guildHouseNames.begin();it!=guildHouseNames.end();++it)
        {
            if(it->first==candidateKey||!guildHouseCities.count(it->first)||guildHouseCities[it->first]!=city)continue;
            bool alive=false;if(it->first==designatedGuildHouseKey&&!designatedGuildHouseHandle.isNull()){Building* known=designatedGuildHouseHandle.getBuilding();alive=known&&!known->isDestroyed()&&known->isThePlayer()&&guildHouseKey(known)==it->first;}
            for(unsigned int b=0;!alive&&b<townBuildings.size();++b){Building* known=townBuildings[b];if(known&&!known->isDestroyed()&&known->isThePlayer()&&guildHouseKey(known)==it->first)alive=true;}
            if(alive){if(ou)ou->showPlayerAMessage(Loc::text("ui.a_guild_house_is_already_registered_in_this_city"),true);return false;}
            if(cleanStale)staleKeys.push_back(it->first);
        }
        for(unsigned int i=0;i<staleKeys.size();++i){guildHouseNames.erase(staleKeys[i]);guildHouseCities.erase(staleKeys[i]);if(designatedGuildHouseKey==staleKeys[i]){designatedGuildHouseKey.clear();designatedGuildHouseHandle.setNull();}}
        if(!staleKeys.empty())saveReputations();return true;
    }

    void scanGuildFurniture();
    void defineGuildHouseClicked(MyGUI::WidgetPtr){requestGuildBuildingType();}

    // Measure actual native text rows, independent of language and UI scale.
    int guildBuildingInfoBottom(MyGUI::Widget* widget,int bottom)
    {
        if(!widget||!widget->getVisible())return bottom;
        MyGUI::TextBox* text=widget->castType<MyGUI::TextBox>(false);
        if(text&&!text->getCaption().empty())bottom=std::max(bottom,widget->getAbsoluteTop()+widget->getHeight());
        for(size_t i=0;i<widget->getChildCount();++i)bottom=guildBuildingInfoBottom(widget->getChildAt(i),bottom);
        return bottom;
    }

    void layoutDefineGuildHouseButton()
    {
        MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();if(!gui||!defineGuildHouseButton)return;
        if(!::gui||!::gui->mainbar)return;
        MyGUI::Widget* info=0;MyGUI::Widget* actions=0;
        // BaseLayout prefixes every native widget name. Never use a global
        // unprefixed lookup or create a fake widget when an anchor is absent.
        ::gui->mainbar->assignWidget(info,"ExtendedStatsPanel",false,false);
        ::gui->mainbar->assignWidget(actions,"StatsPanel",false,false);
        if(!info||!actions)return;
        const int bottom=std::min(info->getAbsoluteTop()+info->getHeight(),actions->getAbsoluteTop());
        const int textBottom=guildBuildingInfoBottom(info,info->getAbsoluteTop());
        const GuildBuildingButtonLayout l(info->getAbsoluteLeft(),info->getWidth(),textBottom,bottom,
            std::max(1,int(MyGUI::RenderManager::getInstance().getViewSize().height*0.024f)));
        defineGuildHouseButton->setCoord(l.x,l.y,l.w,l.h);
    }

    void updateDefineGuildHouseButton()
    {
        MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();if(!gui)return;
        if(!defineGuildHouseButton){defineGuildHouseButton=gui->createWidget<MyGUI::Button>("Kenshi_Button1",0,0,1,1,MyGUI::Align::Default,"Window","DefineMercenarieGuildHouse");defineGuildHouseButton->setTextColour(MyGUI::Colour(0.68f,0.68f,0.40f));defineGuildHouseButton->eventMouseButtonClick+=MyGUI::newDelegate(defineGuildHouseClicked);}
        Building* selected=selectedPlayerBuilding();bool alreadyRegistered=selected&&guildHouseNames.count(guildHouseKey(selected))!=0;bool visible=selected!=0&&!alreadyRegistered;defineGuildHouseButton->setVisible(visible);if(!visible)return;
        MercenarieFonts::caption(defineGuildHouseButton,Loc::text("ui.define_guild_building"));defineGuildHouseButton->setStateSelected(false);
        layoutDefineGuildHouseButton();
    }

    bool textContainsInsensitive(const std::string& value, const char* token)
    {
        std::string lower(value);
        std::transform(lower.begin(), lower.end(), lower.begin(),
            static_cast<int(*)(int)>(std::tolower));
        return lower.find(token) != std::string::npos;
    }

    void appendUniqueChair(std::vector<Building*>& chairs,Building* chair)
    {
        if(chair&&!chair->isDestroyed()&&std::find(chairs.begin(),chairs.end(),chair)==chairs.end())chairs.push_back(chair);
    }

    void scanGuildFurniture()
    {
        if(!ou||!ou->zoneMgr)return;GameData* clientData=ou->gamedata.getData("880130-Guild Escort Contracts.mod",BUILDING);GameData* waitingData=ou->gamedata.getData("880131-Guild Escort Contracts.mod",BUILDING);GameData* fiscalData=ou->gamedata.getData("880133-Guild Escort Contracts.mod",BUILDING);if(!clientData||!waitingData)return;
        lektor<Building*> clients,waiting,fiscal;ou->zoneMgr->getBuildingsThatLinkTo(clients,clientData);ou->zoneMgr->getBuildingsThatLinkTo(waiting,waitingData);if(fiscalData)ou->zoneMgr->getBuildingsThatLinkTo(fiscal,fiscalData);
        Building* selectedBuilding=designatedGuildHouseHandle.isNull()?0:designatedGuildHouseHandle.getBuilding();Building* selectedFiscal=0;std::vector<Building*> selectedClients,selectedWaiting;
        if(selectedBuilding&&(selectedBuilding->isDestroyed()||!selectedBuilding->isThePlayer()||guildHouseKey(selectedBuilding)!=designatedGuildHouseKey)){if(selectedBuilding->isDestroyed()||!selectedBuilding->isThePlayer()){guildHouseNames.erase(designatedGuildHouseKey);guildHouseCities.erase(designatedGuildHouseKey);designatedGuildHouseKey.clear();saveReputations();}selectedBuilding=0;designatedGuildHouseHandle.setNull();}
        for(unsigned int i=0;i<clients.size();++i){Building* chair=clients[i];if(!chair||chair->isDestroyed())continue;Building* parent=chair->furnitureParentBuilding();if(!parent||!parent->isThePlayer()||guildHouseKey(parent)!=designatedGuildHouseKey)continue;if(!selectedBuilding){selectedBuilding=parent;designatedGuildHouseHandle=parent;}if(parent==selectedBuilding)appendUniqueChair(selectedClients,chair);}
        if(selectedBuilding){for(unsigned int j=0;j<waiting.size();++j){Building* seat=waiting[j];if(seat&&seat->furnitureParentBuilding()==selectedBuilding)appendUniqueChair(selectedWaiting,seat);}for(unsigned int j=0;j<fiscal.size();++j){Building* seat=fiscal[j];if(seat&&!seat->isDestroyed()&&seat->furnitureParentBuilding()==selectedBuilding){selectedFiscal=seat;break;}}}
        // Shift+F12 furniture is attached to the interior but is not always indexed by
        // getBuildingsThatLinkTo(). Inspect the designated building's mounted objects too.
        if(selectedBuilding)
        {
            lektor<Building*> mounted;selectedBuilding->getMountedBuildings(&mounted);
            for(unsigned int i=0;i<mounted.size();++i)
            {
                Building* furniture=mounted[i];if(!furniture||furniture->isDestroyed())continue;GameData* data=furniture->getGameData();if(!data)continue;
                if(data->stringID==clientData->stringID)appendUniqueChair(selectedClients,furniture);
                if(data->stringID==waitingData->stringID)appendUniqueChair(selectedWaiting,furniture);
                if(fiscalData&&data->stringID==fiscalData->stringID)selectedFiscal=furniture;
            }
        }
        // Furniture built normally is registered as an interior building and can be
        // absent from both link and mounted-object indexes. Fall back to the complete
        // player-building list for the current town, then keep only furniture whose
        // parent is the designated Guild House.
        if(selectedBuilding)
        {
            lektor<Building*> localBuildings;TownBase* town=selectedBuilding->getCurrentTownLocation();
            ou->zoneMgr->findAllBuildings(localBuildings,town,ou->player?ou->player->getFaction():0,false,0,0);
            for(unsigned int i=0;i<localBuildings.size();++i)
            {
                Building* furniture=localBuildings[i];if(!furniture||furniture->isDestroyed()||furniture==selectedBuilding)continue;
                if(furniture->furnitureParentBuilding()!=selectedBuilding)continue;
                GameData* data=furniture->getGameData();if(!data)continue;
                const bool isClient=data->stringID==clientData->stringID||textContainsInsensitive(data->name,"chaise client")||textContainsInsensitive(data->name,"client chair");
                const bool isWaiting=data->stringID==waitingData->stringID||textContainsInsensitive(data->name,"chaise d'attente")||textContainsInsensitive(data->name,"waiting chair");
                const bool isFiscal=fiscalData&&(data->stringID==fiscalData->stringID||textContainsInsensitive(data->name,"collecteur fiscal")||textContainsInsensitive(data->name,"tax collector"));
                if(isClient)appendUniqueChair(selectedClients,furniture);
                if(isWaiting)appendUniqueChair(selectedWaiting,furniture);
                if(!selectedFiscal&&isFiscal)selectedFiscal=furniture;
            }
        }
        bool wasActive=guildClientSystemActive;bool wasOperational=guildHouseOperational;Building* oldBuilding=guildBuilding;guildBuilding=selectedBuilding;guildClientChairs=selectedClients;guildClientChair=guildClientChairs.empty()?0:guildClientChairs[0];guildFiscalChair=selectedFiscal;guildWaitingChairs=selectedWaiting;bool furnitureReady=guildBuilding&&!guildClientChairs.empty()&&!guildWaitingChairs.empty();guildHouseOperational=furnitureReady&&!designatedGuildHouseKey.empty();guildClientSystemActive=developerFurnitureOverride==0?false:(guildLevel()>=3&&furnitureReady);currentGuildHouseKey=guildHouseKey(guildBuilding);currentGuildHouseName=guildHouseNames.count(currentGuildHouseKey)?guildHouseNames[currentGuildHouseKey]:Loc::text("ui.unnamed_guild_house");if(guildBuilding&&guildHouseNames.count(currentGuildHouseKey)){guildBuilding->setName(guildHouseNames[currentGuildHouseKey]);if(!guildHouseCities.count(currentGuildHouseKey)){TownBase* officeTown=guildBuilding->getCurrentTownLocation();guildHouseCities[currentGuildHouseKey]=officeTown?frenchPlaceName(officeTown->getName()):originCity;saveReputations();}}
        if(guildHouseOperational&&operationalAnnouncementKey!=designatedGuildHouseKey){operationalAnnouncementKey=designatedGuildHouseKey;saveReputations();ou->showPlayerAMessage(Loc::text("ui.operational_guild_house_the_client_chair_and_waiting_chair"),true);}
        if(!wasOperational&&guildHouseOperational){for(int i=0;i<2;++i){FiscalOrganisationState& s=fiscalLedger.organisations[i];if(s.debt>0&&!s.rebel){if(s.nextCollectionHour<=0){s.nextCollectionHour=currentGameHours+48.0;s.preArrivalNotified=false;}s.targetHouseId=designatedGuildHouseKey;}}saveFiscalLedger();}
        if(guildClientSystemActive&&!wasActive)ou->showPlayerAMessage(Loc::text("ui.guild_reception_active_a_client_chair_and_a_waiting"),true);
        else if(!guildClientSystemActive&&wasActive)ou->showPlayerAMessage(Loc::text("ui.guild_reception_disabled_at_least_1_client_chair_and"),true);
    }

    bool rebindDesignatedGuildHouse()
    {
        if(designatedGuildHouseKey.empty()||!ou||!ou->zoneMgr||!ou->player||!shou||!shou->townList)return false;
        Building* known=designatedGuildHouseHandle.isNull()?0:designatedGuildHouseHandle.getBuilding();
        if(known&&!known->isDestroyed()&&known->isThePlayer()&&GuildFurnitureRecovery::sameHouse(designatedGuildHouseKey,guildHouseKey(known))){guildBuilding=known;return true;}
        designatedGuildHouseHandle.setNull();guildBuilding=0;
        const std::string expectedCity=guildHouseCities.count(designatedGuildHouseKey)?guildHouseCities[designatedGuildHouseKey]:"";
        lektor<RootObject*>& towns=shou->townList->getAllTowns();
        for(unsigned int i=0;i<towns.size();++i)
        {
            Town* town=dynamic_cast<Town*>(towns[i]);if(!town)continue;
            if(!expectedCity.empty()&&frenchPlaceName(town->getName())!=expectedCity)continue;
            lektor<Building*> buildings;ou->zoneMgr->findAllBuildings(buildings,town,ou->player->getFaction(),false,0,0);
            for(unsigned int b=0;b<buildings.size();++b)
            {
                Building* candidate=buildings[b];if(!candidate||candidate->isDestroyed()||!candidate->isThePlayer())continue;
                Building* parent=candidate->furnitureParentBuilding();if(parent&&parent->isThePlayer())candidate=parent;
                if(!candidate->isDestroyed()&&GuildFurnitureRecovery::sameHouse(designatedGuildHouseKey,guildHouseKey(candidate))){designatedGuildHouseHandle=candidate;guildBuilding=candidate;return true;}
            }
        }
        return false;
    }

    void refreshGuildFurniture(bool rebindOffice)
    {
        if(rebindOffice)rebindDesignatedGuildHouse();
        scanGuildFurniture();
    }

    void scheduleGuildFurnitureRecovery(){GuildFurnitureRecovery::begin(guildFurnitureRecovery);}

    void updateGuildFurnitureRecovery(float elapsed)
    {
        if(!GuildFurnitureRecovery::tick(guildFurnitureRecovery,elapsed))return;
        refreshGuildFurniture(true);
        const bool noRegisteredOffice=designatedGuildHouseKey.empty()&&guildHouseNames.empty();
        GuildFurnitureRecovery::completeAttempt(guildFurnitureRecovery,guildHouseOperational||noRegisteredOffice);
    }

    bool isBarman(Character* character)
    {
        if (!character) return false;
        if(character->getGameData()&&character->getGameData()->stringID=="43356-Dialogue.mod")return true; // Holy bartender, verified from dialogue log.
        ActivePlatoon* active = character->getPlatoon();
        Platoon* platoon = active ? active->me : 0;
        GameData* squad = platoon ? platoon->squadTemplate : 0;
        if (!active || !squad) return false;
        if(!active->getIsTrader()&&active->getSquadLeader_theRealOne()!=character)return false;

        const std::string& name = squad->name;
        if (textContainsInsensitive(name, "bartender") ||
            textContainsInsensitive(name, "barman") ||
            textContainsInsensitive(name, "bar squad") ||
            textContainsInsensitive(name, "big bar") ||
            textContainsInsensitive(name, "small bar") ||
            textContainsInsensitive(name, "bar residents"))
            return true;

        static const char* knownBars[] = {
            "95581-__Mourn.mod", "44856-rebirth.mod", "14487-rebirth.mod",
            "44422-rebirth.mod", "40560-rebirth.mod", "51484-rebirth.mod",
            "95554-Mourn.mod", "51483-rebirth.mod", "65826-rebirth.mod",
            "50345-rebirth.mod", "13241-Escapes.mod",
            "1533545-__world reactions Slavers.mod", "64853-rebirth.mod",
            "43355-Dialogue.mod", "1193-gamedata.base", "56158-rebirth.mod",
            "54546-rebirth.mod", "56164-rebirth.mod", "56130-rebirth.mod",
            "1533534-__world reactions Slavers.mod", "58699-rebirth.mod"
        };
        for (unsigned int i = 0; i < sizeof(knownBars) / sizeof(knownBars[0]); ++i)
            if (squad->stringID == knownBars[i]) return true;
        return false;
    }

    // Extra replies are appended to the caller-owned native result below.
    // Never alter a shared conversation or attach packages during sendEvent.

    std::string playerGuildName()
    {
        if (ou && ou->player && ou->player->getFaction() &&
            !ou->player->getFaction()->name.empty())
            return ou->player->getFaction()->name;
        return Loc::text("ui.our_mercenary_company");
    }

    std::string contractMeetingQuestion()
    {
        if (scientificMission)
            return Loc::text("ui.are_you_the_company_sent_by_the_bartender_to");
        if (caravanMission)
            return Loc::text("ui.are_you_the_company_sent_by_the_bartender_to_654c107");
        return Loc::text("ui.are_you_the_company_sent_by_the_bartender_to_6b83ad5");
    }

    std::string contractIntroduction()
    {
        std::string text = Loc::text("ui.hello_we_are") + playerGuildName() + ". ";
        if (scientificMission)
            text += Loc::text("ui.the_bartender_told_us_about_your_scientific_expedition_and");
        else if (caravanMission)
            text += Loc::text("ui.the_bartender_told_us_about_your_caravan_and_we");
        else
            text += Loc::text("ui.the_bartender_sent_us_to_secure_your_escort");
        text += Loc::text("ui.before_we_leave_i_would_like_to_discuss_payment");
        return text;
    }

    bool isContractIntroduction(const std::string& id)
    {
        return id == "880027-Guild Escort Contracts.mod" ||
               id == "880035-Guild Escort Contracts.mod" ||
               id == "880045-Guild Escort Contracts.mod";
    }

    bool isContractGreeting(const std::string& id)
    {
        return id == "880026-Guild Escort Contracts.mod" ||
               id == "880034-Guild Escort Contracts.mod" ||
               id == "880044-Guild Escort Contracts.mod";
    }

    bool isGuildVisitor(Character* who);
    std::string guildVisitorOfferKey(Character* who);

    bool contractChoiceVisible(const std::string& id)
    {
        if (isContractIntroduction(id))
            return missionPending && (contractLifecycle == CONTRACT_CLIENT_MEETING ||
                                      contractLifecycle == CONTRACT_NEGOTIATING);
        if (id == "880060-Guild Escort Contracts.mod")
            return missionActive && contractLifecycle == CONTRACT_ACTIVE && !missionPaused;
        if (id == "880061-Guild Escort Contracts.mod")
            return missionActive && contractLifecycle == CONTRACT_ACTIVE && (missionPaused||missionFollowing);
        if (id == "880062-Guild Escort Contracts.mod")
            return missionActive && contractLifecycle == CONTRACT_ACTIVE;
        return true;
    }

    void closeMissionBookTest(MyGUI::WidgetPtr)
    {
        if(missionBookTestWindow)missionBookTestWindow->setVisible(false);
    }

    void openMissionBookTest(MyGUI::WidgetPtr)
    {
        MyGUI::Gui* gui=MyGUI::Gui::getInstancePtr();if(!gui)return;
        if(!missionBookTestWindow)
        {
            const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();
            missionBookTestWindow=gui->createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-520)/2,(view.height-230)/2,520,230,MyGUI::Align::Default,"Window","MissionBookTechnicalTest");
            MyGUI::Widget* content=missionBookTestWindow->getClientWidget();applyMercenarieFrame(content,false);
            MyGUI::TextBox* message=content->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",24,30,472,78,MyGUI::Align::Default,"MissionBookTestMessage");
            message->setTextAlign(MyGUI::Align::Center);message->setFontHeight(20);MercenarieFonts::caption(message,Loc::text("v8.literal.000"));
            MyGUI::Button* close=content->createWidget<MyGUI::Button>("Kenshi_Button1",160,126,200,42,MyGUI::Align::Default,"MissionBookTestClose");MercenarieFonts::caption(close,Loc::text("v8.literal.001"));close->eventMouseButtonClick+=MyGUI::newDelegate(closeMissionBookTest);
        }
        MercenarieFonts::caption(missionBookTestWindow,Loc::text("v8.literal.002"));missionBookTestWindow->setVisible(true);
    }

    bool canUseMissionBook(Building* selected)
    {
        if(!selected)return false;
        GameData* data=selected->getGameData();Building::ConstructionState* state=selected->getBuildState();Building* parent=selected->furnitureParentBuilding();
        return data&&data->stringID==kMissionBookBuildingId&&state&&state->isComplete&&!selected->isDestroyed()&&selected->isThePlayer()&&parent&&!parent->isDestroyed()&&parent->isThePlayer()&&guildHouseNames.count(guildHouseKey(parent))!=0;
    }

    bool missionBookPlayerCharacter(Character* character)
    {
        if(!character||!ou||!ou->player)return false;
        for(unsigned int i=0;i<ou->player->playerCharacters.size();++i)if(ou->player->playerCharacters[i]==character)return true;
        return false;
    }

    Building* missionBookOrderTarget(Building* destination,RootObject* subject)
    {
        Building* candidates[2]={destination,dynamic_cast<Building*>(subject)};
        for(int i=0;i<2;++i){Building* book=candidates[i];GameData* data=book?book->getGameData():0;if(data&&data->stringID==kMissionBookBuildingId)return book;}
        return 0;
    }

    std::string missionBookNativeWatchKey(Building* book,Character* character)
    {
        return book->getHandle().toString()+"|"+character->getHandle().toString();
    }

    void missionBookNativeTrace(const char* event,const MissionBookNativeWatch& watch)
    {
        FILE* log=0;if(fopen_s(&log,"mods/Guild Escort Contracts/MissionBookNativeUse.log","ab")!=0||!log)return;
        Building* book=watch.book.isNull()?0:watch.book.getBuilding();
        fprintf(log,"%s action=%llu task=%d book=%s actor=%s office=%s\n",event,watch.session.started,(int)watch.lastTask,watch.book.toString().c_str(),watch.orderedActor.toString().c_str(),book?guildHouseKey(book->furnitureParentBuilding()).c_str():"");
        fflush(log);fclose(log);
    }

    bool observeMissionBookNativeOrder(Character* character,Building* destination,RootObject* subject,TaskType task,bool directOrder)
    {
        Building* book=missionBookOrderTarget(destination,subject);if(!book||!missionBookPlayerCharacter(character)||task!=OPERATE_MACHINERY)return false;
        MissionBookNativeWatch& watch=missionBookNativeWatches[missionBookNativeWatchKey(book,character)];watch.book=book;watch.orderedActor=character;watch.lastTask=task;watch.age=0.0f;
        // A BF_CHAIR operator is aligned to the usable building's origin.  The
        // Mission Book origin is on the tabletop, unlike a chair origin at floor
        // level.  Consume only this direct native command before Kenshi creates
        // the movement/job; the native cursor and command selection remain, but
        // no invalid tabletop destination is ever submitted to pathfinding.
        if(!directOrder)return true;
        const bool rearmed=MissionBookNativeUseRules::start(watch.session,++missionBookNativeNextAction);
        if(rearmed)missionBookNativeTrace("USE_REARMED",watch);
        missionBookNativeTrace("USE_STARTED",watch);
        if(MissionBookNativeUseRules::confirm(watch.session))missionBookNativeTrace("USE_COMMAND_CONFIRMED",watch);
        return true;
    }

    void observeMissionBookNativeTryOperate(UseableStuff* usable,const hand& actor,bool accepted)
    {
        if(!accepted||!usable)return;GameData* data=usable->getGameData();if(!data||data->stringID!=kMissionBookBuildingId)return;
        Character* character=actor.isNull()?0:actor.getCharacter();if(!missionBookPlayerCharacter(character))return;
        std::map<std::string,MissionBookNativeWatch>::iterator found=missionBookNativeWatches.find(missionBookNativeWatchKey(usable,character));
        if(found==missionBookNativeWatches.end())return;
        MissionBookNativeWatch& watch=found->second;
        if(MissionBookNativeUseRules::confirm(watch.session))missionBookNativeTrace("USE_NATIVE_CONFIRMED",watch);
    }

    void resetMissionBookNativeUse(){missionBookNativeWatches.clear();missionBookNativeNextAction=0;}

    void updateMissionBookNativeUse(float elapsed)
    {
        for(std::map<std::string,MissionBookNativeWatch>::iterator it=missionBookNativeWatches.begin();it!=missionBookNativeWatches.end();)
        {
            MissionBookNativeWatch& watch=it->second;watch.age+=elapsed;Building* book=watch.book.isNull()?0:watch.book.getBuilding();
            if(!book||book->isDestroyed()){missionBookNativeWatches.erase(it++);continue;}
            const bool missionInterfaceVisible=contractsWindow&&contractsWindow->getVisible()&&missionBookDelegationContext;
            Character* character=watch.orderedActor.isNull()?0:watch.orderedActor.getCharacter();
            if(!missionInterfaceVisible&&MissionBookNativeUseRules::ready(watch.session)&&missionBookPlayerCharacter(character)&&canUseMissionBook(book))
            {
                // Consume before opening.  Closing, pausing, occupant flicker and
                // continued native jobs can therefore never replay this action.
                MissionBookNativeUseRules::consume(watch.session);
                missionBookNativeTrace("USE_CONSUMED",watch);
                openMissionBookManagementFor(book);
            }
            ++it;
        }
    }

    bool isMissionCommandSpeaker(Character* speaker)
    {
        return speaker&&speaker==escort;
    }

    void prepareContractClientDialogue(Dialogue* dialogue)
    {
        if(!dialogue||!dialogue->currentLine)return;Character* speaker=dialogue->getCharacter();bool directGuildVisitor=isGuildVisitor(speaker);bool activeEscort=isMissionCommandSpeaker(speaker);if(!directGuildVisitor&&!activeEscort)return;

        if (isContractGreeting(dialogue->currentLine->getStringID()) &&
            dialogue->currentLine->texts && dialogue->currentLine->lineCount > 0)
        {
            const char* frGreetings[]={
                Loc::text("ui.are_you_the_mercenaries_sent_by_the_bartender_i"),
                Loc::text("ui.you_are_the_mercenaries_good_let_us_talk_business"),
                Loc::text("ui.the_bartender_told_me_a_company_would_come_i"),
                Loc::text("ui.finally_i_have_been_waiting_for_the_mercenaries_sent")
            };
            const char* enGreetings[]={
                Loc::text("ui.are_you_the_mercenaries_sent_by_the_bartender_i"),
                Loc::text("ui.you_are_the_mercenaries_good_let_us_talk_business"),
                Loc::text("ui.the_bartender_told_me_a_company_would_come_i"),
                Loc::text("ui.finally_i_have_been_waiting_for_the_mercenaries_sent")
            };
            int greetingIndex=UtilityT::randomInt(0,3);
            dialogue->currentLine->texts[0] = directGuildVisitor?(Loc::text("ui.i_came_directly_to_your_guild_house_because_i")):missionActive?(gMercenarieEnglish?(missionPaused?Loc::text("ui.i_am_waiting_here_until_you_tell_me_to"):Loc::text("ui.we_are_travelling_under_your_protection")):(missionPaused?Loc::text("ui.i_am_waiting_here_until_you_tell_me_to"):Loc::text("ui.we_are_travelling_under_your_protection"))):(gMercenarieEnglish?enGreetings[greetingIndex]:frGreetings[greetingIndex]);
        }

        DialogChoiceList* choices = dialogue->currentLine->children;
        if (!choices) return;
        for (unsigned int i = 0; i < choices->conversationChoices.size(); ++i)
        {
            DialogLineData* choice = choices->conversationChoices[i];
            if (choice && isContractIntroduction(choice->getStringID()) &&
                choice->texts && choice->lineCount > 0)
                choice->texts[0] = directGuildVisitor?(Loc::text("ui.very_well_tell_me_about_the_contract")):mercenarieLocalize(contractIntroduction());
        }
    }

    unsigned long mercenarieUISession=0;
    bool mercenarieGameplayUnavailable();
    void showMercenarieUnavailable();
    void trackerIconClicked(MyGUI::WidgetPtr)
    {
        if(mercenarieLauncherMenu)mercenarieLauncherMenu->setVisible(!mercenarieLauncherMenu->getVisible());
    }
    void closeMercenarieLauncher(MyGUI::Widget*){if(mercenarieLauncherMenu)mercenarieLauncherMenu->setVisible(false);}
    void closeMercenarieInterface(MyGUI::Window* window,const std::string&){if(window)window->setVisible(false);}
    void openMercenarieQuests(MyGUI::Widget*){
        closeMercenarieLauncher(0);
        if(mercenarieGameplayUnavailable()){showMercenarieUnavailable();return;}
        if(trackerWindow)trackerWindow->setVisible(true);
    }
    #define MERCENARIE_PLACE_ICONS
    #include "MapPlaceIcons.h"
    #include "PlayerAutopilot.h"
    void openMercenarieAutopilot(MyGUI::Widget*){
        closeMercenarieLauncher(0);if(mercenarieGameplayUnavailable()){showMercenarieUnavailable();return;}buildPlayerAutopilot();return;
        closeMercenarieLauncher(0);
        if(!MyGUI::Gui::getInstancePtr())return;
        if(!mercenarieAutopilotWindow){
            const MyGUI::IntSize view=MyGUI::RenderManager::getInstance().getViewSize();
            int width=std::min(520,view.width-24);
            mercenarieAutopilotWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-width)/2,(view.height-200)/2,width,200,MyGUI::Align::Default,"Window","MercenarieAutopilot");
            MercenarieFonts::caption(mercenarieAutopilotWindow,Loc::text("ui.the_mercenarie_autopilot"));
            mercenarieAutopilotWindow->eventWindowButtonPressed+=MyGUI::newDelegate(closeMercenarieInterface);
            MyGUI::Widget* client=mercenarieAutopilotWindow->getClientWidget();applyMercenarieFrame(client,false);
            MyGUI::TextBox* text=client->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",24,28,client->getWidth()-48,100,MyGUI::Align::Default);
            text->setFontHeight(18);text->setTextColour(MyGUI::Colour(0.9f,0.84f,0.72f));
            MercenarieFonts::caption(text,Loc::text("ui.autopilot_is_not_available_yet_this_entry_is_reserved"));
        }
        mercenarieAutopilotWindow->setVisible(true);
    }
    #include "LauncherView.h"

    void trackerEntryClicked(MyGUI::WidgetPtr)
    {
        Character* tracked=escortHandle.isNull()?0:escortHandle.getCharacter();
        if (!tracked || tracked!=escort || tracked->isDead() || !ou || !ou->player) return;
        ou->player->getCamera()->teleport(tracked->getPosition());
        ou->player->getCamera()->followObject(escortHandle);
    }

    void createDynamicTracker();
    void refreshDynamicTracker();
    void initialiseTrackerUI()
    {
        loadClientOptions();
        if (!MyGUI::Gui::getInstancePtr()) return;
        MyGUI::Gui* gui = MyGUI::Gui::getInstancePtr();
        launcherEnsureUi();
        if(!trackerIcon){
        trackerIcon = gui->createWidget<MyGUI::Button>(
            "LauncherV9Toggle", 6, 6, 68, 68,
            MyGUI::Align::Default, "Window", "GuildEscortTrackerIcon");
        MercenarieFonts::caption(trackerIcon,"");
        launcherLogo(trackerIcon,10,10,48);
        trackerIcon->eventMouseButtonClick += MyGUI::newDelegate(trackerIconClicked);
        }
        buildMercenarieLauncher(gui);
        trackerIcon->setVisible(true);
    }

    void updateTrackerUI(){if(trackerIcon)trackerIcon->setVisible(true);refreshDynamicTracker();}

    std::string reputationRank(float rep)
    {
        if(rep>=80)return Loc::text("ui.renowned");
        if(rep>=50)return Loc::text("ui.respected");
        if(rep>=20)return Loc::text("ui.reliable");
        if(rep> -20)return Loc::text("ui.unknown_mixed");
        if(rep> -50)return Loc::text("ui.unreliable");
        return Loc::text("ui.discredited");
    }

    int guildLevelForXp(int xp){return GuildProgression::level(xp);}
    int guildLevel(){return guildLevelForXp(guildPoints);}
    const char* guildNextUnlock(int level){
        const std::vector<GuildLevelUI::Unlock>& rows=GuildLevelUI::catalog();
        static std::string next;next.clear();int nearest=11;
        for(size_t i=0;i<rows.size();++i)if(rows[i].level>level)nearest=std::min(nearest,rows[i].level);
        for(size_t i=0;i<rows.size();++i)if(rows[i].level==nearest){if(!next.empty())next+=" / ";next+=rows[i].en;}
        if(next.empty())next=Loc::text("ui.unlimited_prestige_score");return next.c_str();
    }
    int guildThresholdForLevel(int level){return GuildProgression::threshold(level);}
    int guildNextThreshold(int level){return level>=10?GuildProgression::MaxXp:guildThresholdForLevel(level+1);}
    struct GuildProgressView{
        int xp,prestige,level,previous,next,nextLevel;
        GuildProgressView(int value,int prestigeValue):xp(value),prestige(prestigeValue),level(GuildProgression::level(value)),previous(GuildProgression::threshold(level)),next(level>=10?GuildProgression::MaxXp:GuildProgression::threshold(level+1)),nextLevel(std::min(10,level+1)){}
    };
    // UI projection only: every progression label/bar is derived from the
    // same live, persisted guild XP used by gameplay and Cats -> XP.
    GuildProgressView currentGuildProgressView(){return GuildProgressView(guildPoints,guildPrestige);}
    const char* guildLevelTitle(int level){const char* fr[]={Loc::text("ui.fledgling_guild"),Loc::text("ui.unknown_guild"),Loc::text("ui.local_guild"),Loc::text("ui.recognised_guild"),Loc::text("ui.experienced_company"),Loc::text("ui.proven_protectors"),Loc::text("ui.guild_veterans"),Loc::text("ui.renowned_guild"),Loc::text("ui.great_company"),Loc::text("ui.prestigious_guild"),Loc::text("ui.legendary_guild")};const char* en[]={Loc::text("ui.fledgling_guild"),Loc::text("ui.unknown_guild"),Loc::text("ui.local_guild"),Loc::text("ui.recognised_guild"),Loc::text("ui.experienced_company"),Loc::text("ui.proven_protectors"),Loc::text("ui.guild_veterans"),Loc::text("ui.renowned_guild"),Loc::text("ui.great_company"),Loc::text("ui.prestigious_guild"),Loc::text("ui.legendary_guild")};int i=std::max(0,std::min(10,level));return gMercenarieEnglish?en[i]:fr[i];}

#include "GuildLevelUpView.h"

    void updateMissionBookLevelAccess()
    {
        if(!ou||!ou->player||!ou->player->technology||progressLoadFault||progressWriteBlocked)return;
        GameData* book=ou->gamedata.getData(kMissionBookBuildingId,BUILDING);
        GameData* unlock=ou->gamedata.getData(GuildLevel3Access::researchId(),RESEARCH);
        GuildLevel3Access::reconcile(*ou->player->technology,unlock,book,guildLevel());
    }

    void updateGuildResearchAccess()
    {
        int level=guildLevel();if(level==appliedGuildUnlockLevel||!ou||!ou->player||!ou->player->technology)return;appliedGuildUnlockLevel=level;lektor<GameData*> researches;ou->gamedata.getDataOfType(researches,RESEARCH);for(unsigned int i=0;i<researches.size();++i){GameData* research=researches[i];if(!research||research->stringID!="940001-Holy Nation Mercenary Plastron.mod")continue;if(ou->player->technology->finished.find(research)==ou->player->technology->finished.end())research->idata["level"]=1;}ou->player->technology->changedSoUpdateGUI=true;
    }
    const char* relationStanding(float rep){return rep>=70?Loc::text("common.alliee"):rep>=40?Loc::text("common.respectee"):rep>=20?Loc::text("common.amicale"):rep>=0?Loc::text("common.neutre"):rep>=-25?Loc::text("common.mefiante"):Loc::text("common.hostile");}
    int reputationPlaceType(const std::string& name);
    const char* reputationTypeName(int type);

    std::string registerActivity(const std::string& value);
    void updateRelationsPage()
    {
        refreshGuildContracts();
    }

    void updateOfficesPage();

#include "GuildRegisterView.h"
#include "GuildReputation105.h"
#include "GuildFinance106.h"
    bool saveClientOptions(const ClientOptions::Settings& next){
        ClientOptionsFile::Failure failure;std::wstring file=ClientOptionsFile::path(failure);
        std::string previous;
        if(!file.empty()&&!ClientOptionsFile::read(file,previous,failure)&&failure.code!=ERROR_FILE_NOT_FOUND){DebugLog(failure.message());if(ou)ou->showPlayerAMessage(Loc::text("ui.could_not_save_this_option"),true);return false;}
        std::ostringstream out;out.imbue(std::locale::classic());ClientOptions::write(out,next);
        const std::string dismissed=MainMenuNewsRules::dismissedPreference(previous);
        if(!dismissed.empty())out<<"lastDismissedNewsVersion="<<dismissed<<"\n";
        if(file.empty()||!out.good()||!ClientOptionsFile::write(file,out.str(),failure)){
            if(!out.good())failure.set("save","serialize",file,ERROR_INVALID_DATA);
            DebugLog(failure.message());if(ou)ou->showPlayerAMessage(Loc::text("ui.could_not_save_this_option"),true);return false;
        }
        clientOptions=next;return true;
    }
    std::string optionKeyName(ClientOptions::Action action){int value=clientOptions.bindings[action];return value&&key&&key->keyboard?key->keyboard->getAsString((OIS::KeyCode)value):Loc::text("options.key.unassigned");}
    void refreshOptionKeyLabels(){for(int i=0;i<ClientOptions::ActionCount;++i)if(optionKeyLabels[i])MercenarieFonts::caption(optionKeyLabels[i],optionKeyName((ClientOptions::Action)i));if(guildKeyLabel)MercenarieFonts::caption(guildKeyLabel,optionKeyName(ClientOptions::OpenGuildManagement));}
    void refreshGuildKeyLabel(){refreshOptionKeyLabels();}
    bool saveBinding(ClientOptions::Action action,int value,bool reassign){
        if(value!=0&&!validGuildKey(value))return false;ClientOptions::Settings next=clientOptions;int owner=ClientOptions::conflict(next,action,value);if(owner>=0&&!reassign){pendingConflictAction=owner;pendingConflictKey=value;return false;}if(owner>=0)next.bindings[owner]=0;next.bindings[action]=value;if(!saveClientOptions(next))return false;pendingConflictAction=-1;pendingConflictKey=0;refreshOptionKeyLabels();return true;
    }
    bool saveGuildKey(int value){return saveBinding(ClientOptions::OpenGuildManagement,value,true);}
    void captureGuildKey(MyGUI::Widget* sender){if(!key||!key->keyboard)return;int action=sender?atoi(sender->getUserString("action").c_str()):0;capturedAction=(ClientOptions::Action)std::max(0,std::min((int)ClientOptions::ActionCount-1,action));for(int i=0;i<256;++i)guildKeyStates[i]=key->keyboard->isKeyDown((OIS::KeyCode)i);guildKeyCapture=true;pendingConflictAction=-1;pendingConflictKey=0;if(optionKeyLabels[capturedAction])MercenarieFonts::caption(optionKeyLabels[capturedAction],Loc::text("options.key.press"));}
    void resetGuildKey(MyGUI::Widget* sender){guildKeyCapture=false;int action=sender?atoi(sender->getUserString("action").c_str()):0;action=std::max(0,std::min((int)ClientOptions::ActionCount-1,action));int defaults[]={OIS::KC_J,OIS::KC_P,0};saveBinding((ClientOptions::Action)action,defaults[action],true);}
    void guildKeyHover(MyGUI::Widget*,MyGUI::Widget*){if(guildKeyTip&&clientOptions.showTooltips)guildKeyTip->setVisible(true);}
    void guildKeyLeave(MyGUI::Widget*,MyGUI::Widget*){if(guildKeyTip)guildKeyTip->setVisible(false);}
    void clientOptionHover(MyGUI::Widget*,MyGUI::Widget*){if(clientOptionTip&&clientOptions.showTooltips)clientOptionTip->setVisible(true);}
    void clientOptionLeave(MyGUI::Widget*,MyGUI::Widget*){if(clientOptionTip)clientOptionTip->setVisible(false);}
    void contractRewardHover(MyGUI::Widget*,MyGUI::Widget*){if(contractRewardTip&&clientOptions.showTooltips)contractRewardTip->setVisible(true);}
    void contractRewardLeave(MyGUI::Widget*,MyGUI::Widget*){if(contractRewardTip)contractRewardTip->setVisible(false);}
    std::string contractRewardVisualLabel(int percent){if(percent==100)return "0 %";return ContractRewards::label(percent);}
    void contractRewardChanged(MyGUI::ScrollBar*,size_t index){
        if(index>=11)return;const int previous=contractRewardPercent;contractRewardPercent=50+(int)index*10;
        if(contractRewardValue)MercenarieFonts::caption(contractRewardValue,contractRewardVisualLabel(contractRewardPercent));
        if(o103Ready)refreshOptions103();if(previous==contractRewardPercent)return;saveReputations();
        std::ostringstream log;log<<"Contract rewards setting: "<<previous<<" -> "<<contractRewardPercent<<" percent";DebugLog(log.str());
    }
    void clientOptionToggle(MyGUI::Widget*){
        const bool next=!clientOptions.showClientTracker;
        ClientOptions::Settings updated=clientOptions;updated.showClientTracker=next;if(!saveClientOptions(updated))return;
        if(clientTrackerCheck)clientTrackerCheck->setStateSelected(next);
        if(trackerIcon)trackerIcon->setVisible(true);
        if(trackerWindow)trackerWindow->setVisible(next);
    }
    std::string optionsLower(std::string value){for(size_t i=0;i<value.size();++i)value[i]=(char)tolower((unsigned char)value[i]);return value;}
    const char* optionCategoryKey(int category){static const char* keys[]={"options.category.general","options.category.gameplay","options.category.interface","options.category.contracts","options.category.notifications","options.category.keys","options.category.data","options.category.development"};return keys[std::max(0,std::min(7,category))];}
    void fitClientOptionRow(OptionsRow& entry){
        MyGUI::Widget* row=entry.widget;
        if(optionsViewScale==1.0f||!row)return;
        const int w=row->getWidth();int h=row->getHeight();
        for(int i=0;i<2;++i){MyGUI::TextBox* text=row->getChildAt(i)->castType<MyGUI::TextBox>(false);if(text){int font=std::max(12,int((i==0?30:25)*optionsViewScale));text->setSize(i==0?w*41/100-12:w*30/100-8,h-16);MercenarieFonts::caption(text,wrapRegisterCaption(text,i==0?entry.title:entry.description,font));h=std::max(h,text->getTextSize().height+16);}}
        row->setSize(w,h);
        for(size_t i=0;i<row->getChildCount();++i){MyGUI::Widget* child=row->getChildAt(i);
            if(i<2){MyGUI::TextBox* text=child->castType<MyGUI::TextBox>(false);if(text){text->setCoord(i==0?8:w*70/100,8,i==0?w*41/100-12:w*30/100-8,h-16);fitRegisterText(text,std::max(12,int((i==0?30:25)*optionsViewScale)));}}
            else if(i==2)child->setCoord(6,h-1,w-12,1);
            else {int ch=child->getHeight();if(child->getUserString("option")!=""){ch=std::max(22,ch);child->setSize(ch,ch);}child->setPosition(child->getLeft(),std::max(0,(h-ch)/2));}
        }
    }
    void layoutClientOptions(){
        if(o103Ready){layoutOptions103();return;}if(!optionsScroll)return;std::string query=optionsSearch?optionsLower(optionsSearch->getOnlyText()):std::string();const int rowH=std::max(84,int(92*optionsViewScale)),rowStep=rowH+2,headerH=std::max(46,int(68*optionsViewScale)),categoryGap=std::max(6,int(12*optionsViewScale));int y=0,canvasW=optionsScroll->getWidth()-20;
        for(int category=0;category<ClientOptions::CategoryCount;++category){if(!optionsCategoryHeaders[category])continue;bool allowed=(optionsSelectedCategory<0||optionsSelectedCategory==category)&&(category!=ClientOptions::Development||clientOptions.developerMode);int visibleRows=0,rowsHeight=0;
            int totalRows=0;for(size_t r=0;r<optionsRows.size();++r)if(optionsRows[r].category==category){++totalRows;bool match=query.empty()||optionsLower(optionsRows[r].title+" "+optionsRows[r].description).find(query)!=std::string::npos;optionsRows[r].widget->setVisible(allowed&&match);if(allowed&&match){optionsRows[r].widget->setCoord(2,rowsHeight,canvasW-4,rowH);fitClientOptionRow(optionsRows[r]);rowsHeight+=optionsRows[r].widget->getHeight()+2;++visibleRows;}}
            bool showHeader=allowed&&visibleRows>0;optionsCategoryHeaders[category]->setVisible(showHeader);optionsCategoryBodies[category]->setVisible(showHeader&&optionsCategoryOpen[category]);if(!showHeader)continue;
            if(optionsCategoryTitles[category])MercenarieFonts::caption(optionsCategoryTitles[category],std::string(Loc::text(optionCategoryKey(category)))+" ("+registerNumber(totalRows)+")");
            if(optionsCategoryChevrons[category])MercenarieFonts::caption(optionsCategoryChevrons[category],optionsCategoryOpen[category]?"v":">");
            optionsCategoryHeaders[category]->setCoord(0,y,canvasW,headerH);y+=headerH+4;if(optionsCategoryOpen[category]){optionsCategoryBodies[category]->setCoord(0,y,canvasW,rowsHeight+2);y+=rowsHeight+categoryGap;}
            if(optionsViewScale!=1.0f){int tx=std::max(40,int(94*optionsViewScale)),tw=canvasW<600?canvasW-tx-8:canvasW*42/100-tx;
                optionsCategoryTitles[category]->setCoord(tx,2,tw,headerH-4);fitRegisterText(optionsCategoryTitles[category],std::max(12,int(30*optionsViewScale)));
                optionsCategoryDescriptions[category]->setVisible(canvasW>=600);if(canvasW>=600){optionsCategoryDescriptions[category]->setCoord(canvasW*44/100,2,canvasW*56/100-8,headerH-4);fitRegisterText(optionsCategoryDescriptions[category],std::max(12,int(24*optionsViewScale)));}}
        }optionsScroll->setCanvasSize(canvasW,std::max(optionsScroll->getHeight(),y));optionsScroll->setVisibleVScroll(y>optionsScroll->getHeight());
    }
    void optionsSearchChanged(MyGUI::EditBox*){if(o103Ready&&optionsSearch&&!optionsSearch->getOnlyText().empty())for(int i=0;i<ClientOptions::CategoryCount;++i)optionsCategoryOpen[i]=true;if(optionsSearchHint&&optionsSearch)optionsSearchHint->setVisible(optionsSearch->getOnlyText().empty());layoutClientOptions();}
    void optionsCategoryChanged(MyGUI::ComboBox*,size_t index){optionsSelectedCategory=index==0?-1:(int)index-1;layoutClientOptions();}
    void optionsCategoryClicked(MyGUI::Widget* sender){int category=-1;for(int i=0;i<ClientOptions::CategoryCount;++i)if(sender==optionsCategoryHeaders[i]){category=i;break;}if(category>=0&&category<ClientOptions::CategoryCount){optionsCategoryOpen[category]=!optionsCategoryOpen[category];if(optionsCategoryChevrons[category])MercenarieFonts::caption(optionsCategoryChevrons[category],optionsCategoryOpen[category]?"v":">");layoutClientOptions();}}
    std::string optionBoolCaption(bool value){return Loc::text(value?"options.on":"options.off");}
    void closeDeveloperFinishPicker(MyGUI::WidgetPtr);
    void setCheatMenuEnabled(bool enabled){
        clientOptions.developerMode=enabled;
        pWasDown=true; // Require a fresh key press after enabling.
        if(!enabled){
            resetV9PendingActions();
            if(developerWindow)developerWindow->setVisible(false);
            closeDeveloperFinishPicker(0);
            if(capturedAction==ClientOptions::OpenCheatMenu){guildKeyCapture=false;pendingConflictAction=-1;pendingConflictKey=0;}
        }
        optionsRebuildRequested=true; // Rebuild after the checkbox callback returns.
    }
    void optionToggleClicked(MyGUI::Widget* sender){
        std::string id=sender->getUserString("option");
        if(id=="realEstate"){
            if(estateEnabled()&&(estateBusy||RealEstate::requiresSystem(estateState))){if(ou)ou->showPlayerAMessage(Loc::text("options.real_estate.blocked"),true);return;}
            ClientOptions::Settings next=clientOptions;next.realEstateEnabled=!estateEnabled();
            if(saveClientOptions(next)){estateOptionChanged();optionsRebuildRequested=true;}return;
        }
        if(id=="payroll"){payrollSetEnabled(!guildPayroll.enabled);MyGUI::Button* button=sender->castType<MyGUI::Button>(false);if(button)button->setStateSelected(guildPayroll.enabled);return;}
        if(id=="cheatMenu"){setCheatMenuEnabled(!clientOptions.developerMode);MyGUI::Button* button=sender->castType<MyGUI::Button>(false);if(button)button->setStateSelected(clientOptions.developerMode);return;}
        ClientOptions::Settings next=clientOptions;bool* value=0;
        if(id=="tooltips")value=&next.showTooltips;else if(id=="routes")value=&next.showRoutes;else if(id=="details")value=&next.detailedOffers;else if(id=="confirmAccept")value=&next.confirmAccept;else if(id=="confirmDelegate")value=&next.confirmDelegate;else if(id=="notifications")value=&next.showNotifications;else if(id=="notifyMission")value=&next.notifyMissionComplete;else if(id=="notifyDelegated")value=&next.notifyDelegatedComplete;else if(id=="detailedLogs")value=&next.detailedLogs;
        if(!value)return;*value=!*value;if(saveClientOptions(next)){MyGUI::Button* button=sender->castType<MyGUI::Button>(false);if(button)button->setStateSelected(*value);if(id=="routes"&&contractsWindow&&contractsWindow->getVisible())updateContractsBoard();if(id=="detailedLogs")diagnosticEnabled=*value;}
    }
    void optionLanguageChanged(MyGUI::ComboBox*,size_t index){if(index>=Loc::languageChoiceCount())return;ClientOptions::Settings next=clientOptions;next.language=Loc::languageChoiceCode(index);if(!saveClientOptions(next))return;gMercenarieLanguageOverride=next.language;detectMercenarieLanguage();optionsRebuildRequested=true;}
    void buildLanguageChoice(MyGUI::Widget* row){MercenarieFonts::ensure();MyGUI::ComboBox* choice=row->createWidget<MyGUI::ComboBox>("Options108LanguageCombo",row->getWidth()*45/100,22,row->getWidth()*22/100,48,MyGUI::Align::Default,"MercenarieLanguageChoice");choice->setVisible(true);choice->setEnabled(true);choice->setNeedMouseFocus(true);choice->setComboModeDrop(true);choice->setFontName("MercenarieUnicode");choice->setFontHeight(21);std::string selectedCode=clientOptions.language=="auto"?"auto":Loc::resolveLanguage(clientOptions.language);size_t selected=0;for(size_t i=0;i<Loc::languageChoiceCount();++i){choice->addItem(Loc::languageChoiceLabel(i));if(selectedCode==Loc::languageChoiceCode(i))selected=i;}choice->setIndexSelected(selected);choice->eventComboChangePosition+=MyGUI::newDelegate(optionLanguageChanged);MercenarieFonts::languageChoice(choice);}
    void exportOptionsDiagnostic(MyGUI::Widget*){std::ofstream out("mods/Guild Escort Contracts/TheMercenarie-Diagnostic.txt",std::ios::trunc);out<<"The Mercenarie diagnostic\nformat=4\nlanguage="<<Loc::engine().language<<"\nguildLevel="<<guildLevel()<<"\nactiveContracts="<<overviewEscortCount(false)<<"\nhistory="<<contractHistory.size()<<"\nfinancialEntries="<<fiscalLedger.entries.size()<<"\noffices="<<guildHouseNames.size()<<"\nrewardPercent="<<contractRewardPercent<<"\n";Loc::writeDiagnostics(out);MercenarieFonts::writeDiagnostics(out);out.close();if(ou)ou->showPlayerAMessage(Loc::text("options.diagnostic.done"),true);}
    void resetOptionsInterface(MyGUI::Widget* sender){if(!optionsUiResetConfirmation){optionsUiResetConfirmation=true;sender->castType<MyGUI::Button>()->setCaption(Loc::text("options.confirm"));return;}optionsUiResetConfirmation=false;for(int i=0;i<ClientOptions::CategoryCount;++i)optionsCategoryOpen[i]=false;if(optionsSearch)MercenarieFonts::caption(optionsSearch,"");if(optionsCategoryFilter)optionsCategoryFilter->setIndexSelected(0);optionsSelectedCategory=-1;sender->castType<MyGUI::Button>()->setCaption(Loc::text("options.reset_ui.action"));layoutClientOptions();}
    void resetAllOptions(MyGUI::Widget*){if(!optionsResetConfirmation){optionsResetConfirmation=true;if(optionsResetAllButton)MercenarieFonts::caption(optionsResetAllButton,Loc::text("options.confirm"));return;}ClientOptions::Settings defaults(false,OIS::KC_J,OIS::KC_P,0);if(!saveClientOptions(defaults))return;estateOptionChanged();setCheatMenuEnabled(false);for(int i=0;i<ClientOptions::CategoryCount;++i)optionsCategoryOpen[i]=false;optionsSelectedCategory=-1;contractRewardPercent=100;saveReputations();gMercenarieLanguageOverride="auto";detectMercenarieLanguage();if(trackerWindow)trackerWindow->setVisible(defaults.showClientTracker);optionsResetConfirmation=false;optionsRebuildRequested=true;}
    void optionKitBorder(MyGUI::Widget* widget,const MyGUI::Colour& colour){if(!widget)return;const int w=widget->getWidth(),h=widget->getHeight();registerSolid(widget,0,0,w,1,colour);registerSolid(widget,0,h-1,w,1,colour);registerSolid(widget,0,0,1,h,colour);registerSolid(widget,w-1,0,1,h,colour);}
    MyGUI::ImageBox* optionKitIcon(MyGUI::Widget* parent,int id,int x,int y,int size){MyGUI::ImageBox* icon=parent->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);icon->setImageTexture("TheMercenarieUI.png");icon->setImageCoord(MyGUI::IntCoord(std::max(0,std::min(8,id))*32,64,32,32));icon->setNeedMouseFocus(false);return icon;}
    void scaleOptionButtonFonts(MyGUI::Widget* parent,float scale){if(!parent)return;for(size_t i=0;i<parent->getChildCount();++i){MyGUI::Widget* child=parent->getChildAt(i);scaleOptionButtonFonts(child,scale);MyGUI::Button* button=child->castType<MyGUI::Button>(false);if(button)button->setFontHeight(std::max(13,(int)(button->getFontHeight()*scale+0.5f)));}}
    MyGUI::Widget* optionRow(MyGUI::Widget* body,int category,const char* titleKey,const char* descriptionKey){
        const int w=body->getWidth();MyGUI::Widget* row=body->createWidget<MyGUI::Widget>("TheMercenarie_Row",0,0,w,92,MyGUI::Align::Default);
        registerText(row,22,22,w*41/100,46,30,Loc::text(titleKey),registerIvory)->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
        MyGUI::TextBox* description=registerText(row,w*70/100,12,w*29/100-20,68,25,Loc::text(descriptionKey),MyGUI::Colour(.68f,.70f,.68f));description->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);description->setNeedMouseFocus(false);fitRegisterText(description,25);
        registerSolid(row,16,91,w-32,1,MyGUI::Colour(.12f,.16f,.17f));optionsRows.push_back(OptionsRow(row,category,Loc::text(titleKey),Loc::text(descriptionKey)));return row;
    }
    MyGUI::Button* optionToggle(MyGUI::Widget* row,const char* id,bool value){int x=row->getWidth()*55/100;MyGUI::Button* button=row->createWidget<MyGUI::Button>("TheMercenarie_Checkbox",x,27,38,38,MyGUI::Align::Default);button->setUserString("option",id);MercenarieFonts::caption(button,"");button->setStateSelected(value);button->eventMouseButtonClick+=MyGUI::newDelegate(optionToggleClicked);return button;}
    void buildClientOptions(MyGUI::Widget* parent,int x,int y,int w,int h){
        o103Ready=false;optionsViewScale=1.0f;loadClientOptions();guildOptionsPanel=parent->createWidget<MyGUI::Widget>("TheMercenarie_Panel",x,y,w,h,MyGUI::Align::Default);optionKitBorder(guildOptionsPanel,MyGUI::Colour(.48f,.29f,.09f));optionsRows.clear();
        registerText(guildOptionsPanel,26,10,480,48,38,Loc::text("common.options"),registerAmber);registerText(guildOptionsPanel,26,56,w-52,34,25,Loc::text("options.subtitle"),registerIvory);
        const int searchW=w*38/100,filterW=w*22/100,resetW=330,toolbarY=106,toolbarH=52;
        optionsSearch=guildOptionsPanel->createWidget<MyGUI::EditBox>("TheMercenarie_Input",26,toolbarY,searchW,toolbarH,MyGUI::Align::Default);MercenarieFonts::caption(optionsSearch,"");optionsSearch->setTextColour(registerIvory);optionsSearch->setFontHeight(26);optionsSearch->eventEditTextChange+=MyGUI::newDelegate(optionsSearchChanged);optionKitBorder(optionsSearch,MyGUI::Colour(.39f,.32f,.22f));optionKitIcon(optionsSearch,7,12,10,32);optionsSearchHint=registerText(optionsSearch,52,0,searchW-66,toolbarH,24,Loc::text("options.search"),MyGUI::Colour(.58f,.60f,.58f));optionsSearchHint->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);optionsSearchHint->setNeedMouseFocus(false);
        int filterX=44+searchW;optionsCategoryFilter=guildOptionsPanel->createWidget<MyGUI::ComboBox>("Options108Combo",filterX,toolbarY,filterW,toolbarH,MyGUI::Align::Default);optionsCategoryFilter->setTextColour(registerIvory);optionsCategoryFilter->setFontHeight(25);optionsCategoryFilter->addItem(Loc::text("options.all_categories"));for(int i=0;i<ClientOptions::CategoryCount;++i)if(i<ClientOptions::Development||clientOptions.developerMode)optionsCategoryFilter->addItem(Loc::text(optionCategoryKey(i)));optionsCategoryFilter->setIndexSelected(0);optionsCategoryFilter->eventComboChangePosition+=MyGUI::newDelegate(optionsCategoryChanged);optionKitBorder(optionsCategoryFilter,MyGUI::Colour(.39f,.32f,.22f));
        optionsResetAllButton=guildOptionsPanel->createWidget<MyGUI::Button>("TheMercenarie_Button",w-resetW-26,toolbarY,resetW,toolbarH,MyGUI::Align::Default);MercenarieFonts::caption(optionsResetAllButton,Loc::text("options.reset_all"));optionsResetAllButton->setFontHeight(24);optionsResetAllButton->eventMouseButtonClick+=MyGUI::newDelegate(resetAllOptions);optionKitBorder(optionsResetAllButton,MyGUI::Colour(.76f,.43f,.09f));optionKitIcon(optionsResetAllButton,8,14,10,32);
        optionsScroll=guildOptionsPanel->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",26,176,w-52,h-202,MyGUI::Align::Default);MercenarieNativeInput::bind(optionsScroll);optionsScroll->setVisibleHScroll(false);optionsScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);int canvasW=w-74;
        const char* helpKeys[]={"options.category.general.help","options.category.gameplay.help","options.category.interface.help","options.category.contracts.help","options.category.notifications.help","options.category.keys.help","options.category.data.help","options.category.development.help"};
        for(int c=0;c<ClientOptions::CategoryCount;++c){optionsCategoryHeaders[c]=optionsScroll->createWidget<MyGUI::Button>("TheMercenarie_HeaderSkin",0,0,canvasW,68,MyGUI::Align::Default);MercenarieFonts::caption(optionsCategoryHeaders[c],"");optionsCategoryHeaders[c]->setUserString("category",registerNumber(c));optionsCategoryHeaders[c]->eventMouseButtonClick+=MyGUI::newDelegate(optionsCategoryClicked);optionKitBorder(optionsCategoryHeaders[c],MyGUI::Colour(.82f,.45f,.07f));optionsCategoryChevrons[c]=registerText(optionsCategoryHeaders[c],14,14,26,40,28,optionsCategoryOpen[c]?"v":">",registerAmber);optionsCategoryChevrons[c]->setTextAlign(MyGUI::Align::Center);optionKitIcon(optionsCategoryHeaders[c],c==1?2:(c>1?c-1:c),48,18,32);optionsCategoryTitles[c]=registerText(optionsCategoryHeaders[c],94,10,350,48,30,Loc::text(optionCategoryKey(c)),registerAmber);optionsCategoryTitles[c]->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);optionsCategoryDescriptions[c]=registerText(optionsCategoryHeaders[c],450,12,canvasW-470,44,24,Loc::text(helpKeys[c]),MyGUI::Colour(.70f,.71f,.68f));optionsCategoryDescriptions[c]->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);optionsCategoryBodies[c]=optionsScroll->createWidget<MyGUI::Widget>("TheMercenarie_Panel",0,0,canvasW,92,MyGUI::Align::Default);}
        MyGUI::Widget* row=optionRow(optionsCategoryBodies[0],0,"options.client_tracker.title","options.client_tracker.description");clientTrackerCheck=optionToggle(row,"tracker",clientOptions.showClientTracker);clientTrackerCheck->eventMouseButtonClick.clear();clientTrackerCheck->eventMouseButtonClick+=MyGUI::newDelegate(clientOptionToggle);
        row=optionRow(optionsCategoryBodies[1],1,"options.reward.title","options.reward.description");{int cx=row->getWidth()*45/100,cw=row->getWidth()*22/100;contractRewardSlider=row->createWidget<MyGUI::ScrollBar>("TheMercenarie_Slider",cx,28,cw-104,36,MyGUI::Align::Default);contractRewardSlider->setScrollRange(11);contractRewardSlider->setScrollPage(1);contractRewardSlider->setScrollPosition((contractRewardPercent-50)/10);contractRewardSlider->eventScrollChangePosition+=MyGUI::newDelegate(contractRewardChanged);MyGUI::Widget* valueBox=row->createWidget<MyGUI::Widget>("TheMercenarie_InputSkin",cx+cw-94,24,88,44,MyGUI::Align::Default);optionKitBorder(valueBox,MyGUI::Colour(.82f,.45f,.07f));contractRewardValue=registerText(valueBox,4,3,80,38,22,contractRewardVisualLabel(contractRewardPercent),registerIvory);contractRewardValue->setTextAlign(MyGUI::Align::Center);}
        row=optionRow(optionsCategoryBodies[0],0,"options.language.title","options.language.description");buildLanguageChoice(row);
        row=optionRow(optionsCategoryBodies[0],0,"options.cheat_menu.title","options.cheat_menu.description");optionToggle(row,"cheatMenu",clientOptions.developerMode);
        row=optionRow(optionsCategoryBodies[2],2,"options.tooltips.title","options.tooltips.description");optionToggle(row,"tooltips",clientOptions.showTooltips);row=optionRow(optionsCategoryBodies[2],2,"options.routes.title","options.routes.description");optionToggle(row,"routes",clientOptions.showRoutes);
        row=optionRow(optionsCategoryBodies[3],3,"options.details.title","options.details.description");optionToggle(row,"details",clientOptions.detailedOffers);row=optionRow(optionsCategoryBodies[3],3,"options.confirm_accept.title","options.confirm_accept.description");optionToggle(row,"confirmAccept",clientOptions.confirmAccept);row=optionRow(optionsCategoryBodies[3],3,"options.confirm_delegate.title","options.confirm_delegate.description");optionToggle(row,"confirmDelegate",clientOptions.confirmDelegate);
        row=optionRow(optionsCategoryBodies[4],4,"options.notifications.title","options.notifications.description");optionToggle(row,"notifications",clientOptions.showNotifications);row=optionRow(optionsCategoryBodies[4],4,"options.mission_notification.title","options.mission_notification.description");optionToggle(row,"notifyMission",clientOptions.notifyMissionComplete);row=optionRow(optionsCategoryBodies[4],4,"options.delegated_notification.title","options.delegated_notification.description");optionToggle(row,"notifyDelegated",clientOptions.notifyDelegatedComplete);
        const char* actionKeys[]={"options.key.guild","options.key.cheats","options.key.autopilot"};for(int action=0;action<ClientOptions::ActionCount;++action){if(action==ClientOptions::OpenCheatMenu&&!clientOptions.developerMode)continue;row=optionRow(optionsCategoryBodies[5],5,actionKeys[action],"options.key.description");int cx=row->getWidth()*43/100,keyW=230,modifyW=140,resetKeyW=140,g=10;MyGUI::Widget* keyBox=row->createWidget<MyGUI::Widget>("TheMercenarie_InputSkin",cx,24,keyW,44,MyGUI::Align::Default);optionKitBorder(keyBox,MyGUI::Colour(.54f,.36f,.13f));optionKeyLabels[action]=registerText(keyBox,8,3,keyW-16,38,23,"",registerAmber);optionKeyLabels[action]->setTextAlign(MyGUI::Align::Center);MyGUI::Button* modify=row->createWidget<MyGUI::Button>("TheMercenarie_Button",cx+keyW+g,24,modifyW,44,MyGUI::Align::Default);MercenarieFonts::caption(modify,Loc::text("options.key.modify"));modify->setFontHeight(19);modify->setUserString("action",registerNumber(action));modify->eventMouseButtonClick+=MyGUI::newDelegate(captureGuildKey);optionKitBorder(modify,MyGUI::Colour(.76f,.43f,.09f));MyGUI::Button* reset=row->createWidget<MyGUI::Button>("TheMercenarie_Button",cx+keyW+g+modifyW+g,24,resetKeyW,44,MyGUI::Align::Default);MercenarieFonts::caption(reset,Loc::text("ui.reset"));reset->setFontHeight(19);reset->setUserString("action",registerNumber(action));reset->eventMouseButtonClick+=MyGUI::newDelegate(resetGuildKey);optionKitBorder(reset,MyGUI::Colour(.54f,.36f,.13f));}
        row=optionRow(optionsCategoryBodies[6],6,"options.diagnostic.title","options.diagnostic.description");{int cx=row->getWidth()*45/100,cw=row->getWidth()*22/100;MyGUI::Button* diagnostic=row->createWidget<MyGUI::Button>("TheMercenarie_Button",cx,24,cw,44,MyGUI::Align::Default);MercenarieFonts::caption(diagnostic,Loc::text("options.diagnostic.export"));diagnostic->setFontHeight(19);diagnostic->eventMouseButtonClick+=MyGUI::newDelegate(exportOptionsDiagnostic);optionKitBorder(diagnostic,MyGUI::Colour(.76f,.43f,.09f));}row=optionRow(optionsCategoryBodies[6],6,"options.reset_ui.title","options.reset_ui.description");{int cx=row->getWidth()*45/100,cw=row->getWidth()*22/100;MyGUI::Button* resetUi=row->createWidget<MyGUI::Button>("TheMercenarie_Button",cx,24,cw,44,MyGUI::Align::Default);MercenarieFonts::caption(resetUi,Loc::text("options.reset_ui.action"));resetUi->setFontHeight(18);resetUi->eventMouseButtonClick+=MyGUI::newDelegate(resetOptionsInterface);optionKitBorder(resetUi,MyGUI::Colour(.76f,.43f,.09f));}
        if(clientOptions.developerMode){MyGUI::Widget* warRow=optionRow(optionsCategoryBodies[7],7,"v9.war.title","v9.war.description");MyGUI::Button* warButton=warRow->createWidget<MyGUI::Button>("TheMercenarie_Button",warRow->getWidth()*60/100,24,warRow->getWidth()*38/100,44,MyGUI::Align::Default);MercenarieFonts::caption(warButton,Loc::text("v9.war.title"));warButton->setFontHeight(18);warButton->eventMouseButtonClick+=MyGUI::newDelegate(openFactionDiplomacy);row=optionRow(optionsCategoryBodies[7],7,"options.dev_logs.title","options.dev_logs.description");optionToggle(row,"detailedLogs",clientOptions.detailedLogs);}
        row=optionRow(optionsCategoryBodies[1],1,"payroll.enabled","payroll.enabled_tip");optionToggle(row,"payroll",guildPayroll.enabled);
        row=optionRow(optionsCategoryBodies[1],1,"options.real_estate.title","options.real_estate.description");optionToggle(row,"realEstate",estateEnabled());
        refreshOptionKeyLabels();layoutClientOptions();guildOptionsPanel->setVisible(false);
    }

    #include "GuildOptions103.h"
    void guildTabClicked(MyGUI::WidgetPtr sender);
    long long fiscalGrossTotal(){long long n=0;for(size_t i=0;i<fiscalLedger.entries.size();++i)n+=std::max(0,fiscalLedger.entries[i].grossIncome);return n;}
    void updateFinancesPage(){refreshFinanceView();refreshFinance106();}
    bool guildClientReady(Character* actor);
    void releaseGuildVisitorSeat(Character* actor);
    void cleanupFiscalParty();void startFiscalCombat();
    void departFiscalParty()
    {
        const std::string seatQueueKey=guildFiscalQueueKey();
        if(fiscalParty.members.empty()){fiscalParty=FiscalParty();guildSeatTickets.erase(seatQueueKey);refreshGuildSeatQueue();return;}
        Building* office=fiscalTargetOffice.getBuilding();DepartingVisitors departing;TownBase* location=office?office->getCurrentTownLocation():0;Town* town=location?location->isTown():0;Ogre::Vector3 exit=town?town->getPositionOutsideTownGates(1200.0f):(office?office->getPosition()+Ogre::Vector3(1200,0,1200):Ogre::Vector3::ZERO);
        for(size_t i=0;i<fiscalParty.members.size();++i){Character* who=fiscalParty.members[i].isNull()?0:fiscalParty.members[i].getCharacter();if(!who)continue;departing.members.push_back(who);releaseGuildVisitorSeat(who);disableDepartingVisitorDialogue(who);who->removeJob(SIT_AROUND);who->removeJob(OPERATE_MACHINERY);who->removeJob(FOLLOW_WHILE_TALKING);who->removeJob(MOVE_CUS_ORDERED);who->addJob(MOVE_CUS_ORDERED,0,false,false,exit);}
        if(!departing.members.empty())departingVisitors.push_back(departing);fiscalParty=FiscalParty();guildSeatTickets.erase(seatQueueKey);refreshGuildSeatQueue();
    }
    void fiscalPayOrganisation(FiscalOrganisation o){if(!ou||!ou->player||!ou->player->participant||!ou->player->participant->factionOwnerships)return;long long debt=fiscalLedger.debt(o);int money=ou->player->participant->factionOwnerships->getMoney();if(debt<=0){ou->showPlayerAMessage(Loc::text("ui.no_debt_is_currently_owed_to_this_organization"),true);return;}if(money<debt){char b[4096];sprintf_s(b,mercenarieLocalize(Loc::text("ui.insufficient_funds_you_own_d_cats_but_the_collector")).c_str(),money,debt);ou->showPlayerAMessage(b,true);return;}if(!financePayDebt(o)){ou->showPlayerAMessage(Loc::text("guild.exchange_failed"),true);return;}departFiscalParty();saveFiscalLedger();ou->showPlayerAMessage(Loc::text("ui.tax_debt_fully_paid_the_recovery_procedure_is_completed"),true);if(fiscalCollectorWindow)fiscalCollectorWindow->setVisible(false);updateFinancesPage();}
    void fiscalFinanceButtonClicked(MyGUI::WidgetPtr sender){if(sender==fiscalRulesButton){fiscalShowRules=!fiscalShowRules;updateFinancesPage();}}
    void fiscalCollectorPressed(MyGUI::WidgetPtr sender)
    {
        if(activeFiscalOrganisation<0)return;FiscalOrganisation o=(FiscalOrganisation)activeFiscalOrganisation;FiscalOrganisationState& s=fiscalLedger.organisations[activeFiscalOrganisation];
        if(sender==fiscalCollectorPay){fiscalPayOrganisation(o);return;}
        if(sender==fiscalCollectorDetail){fiscalShowRules=false;updateFinancesPage();if(guildWindow){guildWindow->setVisible(true);guildTabClicked(guildTabButtons[4]);}return;}
        if(sender==fiscalCollectorExtension){if(fiscalParty.raid>0||s.extensionUsed){ou->showPlayerAMessage(Loc::text("ui.no_we_have_already_given_you_enough_time"),true);return;}s.extensionUsed=true;s.state=FSTATE_EXTENSION;s.deadlineHour=currentGameHours+168.0;s.nextCollectionHour=s.deadlineHour;s.preArrivalNotified=false;saveFiscalLedger();departFiscalParty();ou->showPlayerAMessage(Loc::text("ui.seven_days_not_one_day_more_we_will_return"),true);fiscalCollectorWindow->setVisible(false);return;}
        if(sender!=fiscalCollectorRefuse)return;if(!fiscalRefusalConfirm){fiscalRefusalConfirm=true;MercenarieFonts::caption(fiscalCollectorRefuse,Loc::text("ui.confirm_refusal"));MercenarieFonts::caption(fiscalCollectorText,fiscalCollectorText->getCaption()+mercenarieLocalize(Loc::text("ui.do_you_confirm_your_refusal_an_armed_procedure_will")));return;}
        fiscalRefusalConfirm=false;if(fiscalParty.raid>0){fiscalCollectorWindow->setVisible(false);startFiscalCombat();return;}
        if(s.debt<5000){s.state=FSTATE_NORMAL;s.nextCollectionHour=currentGameHours+168.0;s.extensionUsed=false;s.preArrivalNotified=false;saveFiscalLedger();departFiscalParty();ou->showPlayerAMessage(Loc::text("ui.your_debt_is_recorded_we_will_come_back"),true);}
        else{s.state=FSTATE_WARNING;s.deadlineHour=currentGameHours+48.0;s.nextCollectionHour=s.deadlineHour;s.preArrivalNotified=false;saveFiscalLedger();departFiscalParty();ou->showPlayerAMessage(Loc::text("ui.refusal_confirmed_a_recovery_team_will_arrive_in_two"),true);}fiscalCollectorWindow->setVisible(false);
    }
    void fiscalCollectorWindowClosed(MyGUI::Window*,const std::string&){if(fiscalCollectorWindow)fiscalCollectorWindow->setVisible(false);if(fiscalParty.organisation>=0&&fiscalParty.organisation<2){fiscalParty.waitingForConversation=true;FiscalOrganisationState& s=fiscalLedger.organisations[fiscalParty.organisation];s.state=FSTATE_DIALOGUE;saveFiscalLedger();}if(ou)ou->showPlayerAMessage(Loc::text("ui.the_collector_remains_seated_at_the_guild_house_speak"),true);}
    void showFiscalCollector(FiscalOrganisation o)
    {
        if(!guildClientReady(fiscalParty.leader.getCharacter()))return;
        if(!MyGUI::Gui::getInstancePtr())return;FiscalOrganisationState& s=fiscalLedger.organisations[(int)o];activeFiscalOrganisation=(int)o;if(!fiscalCollectorWindow){const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();int w=850,h=560;fiscalCollectorWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(v.width-w)/2,(v.height-h)/2,w,h,MyGUI::Align::Default,"Window","FiscalCollectorWindow");fiscalCollectorWindow->eventWindowButtonPressed+=MyGUI::newDelegate(fiscalCollectorWindowClosed);MyGUI::Widget* c=fiscalCollectorWindow->getClientWidget();applyMercenarieFrame(c,true);fiscalCollectorText=c->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",24,22,w-48,350,MyGUI::Align::Default,"FiscalCollectorText");int bw=(w-72)/4;fiscalCollectorPay=c->createWidget<MyGUI::Button>("Kenshi_Button1",18,410,bw,58,MyGUI::Align::Default,"FiscalPay");fiscalCollectorDetail=c->createWidget<MyGUI::Button>("Kenshi_Button1",30+bw,410,bw,58,MyGUI::Align::Default,"FiscalDetail");fiscalCollectorExtension=c->createWidget<MyGUI::Button>("Kenshi_Button1",42+bw*2,410,bw,58,MyGUI::Align::Default,"FiscalExtension");fiscalCollectorRefuse=c->createWidget<MyGUI::Button>("Kenshi_Button1",54+bw*3,410,bw,58,MyGUI::Align::Default,"FiscalRefuse");fiscalCollectorPay->eventMouseButtonClick+=MyGUI::newDelegate(fiscalCollectorPressed);fiscalCollectorDetail->eventMouseButtonClick+=MyGUI::newDelegate(fiscalCollectorPressed);fiscalCollectorExtension->eventMouseButtonClick+=MyGUI::newDelegate(fiscalCollectorPressed);fiscalCollectorRefuse->eventMouseButtonClick+=MyGUI::newDelegate(fiscalCollectorPressed);}
        MercenarieFonts::caption(fiscalCollectorWindow,mercenarieLocalize(Loc::text("ui.tax_collection"))+mercenarieLocalize(fiscalOrgName(o)));std::stringstream t;fiscalRefusalConfirm=false;if(fiscalParty.raid>0){int soldiers=fiscalParty.raid==1?10:fiscalParty.raid==2?15:20;t<<mercenarieLocalize(Loc::text("ui.collection_team"))<<fiscalParty.raid<<"/3\n\n"<<mercenarieLocalize(Loc::text("ui.the_deadline_has_passed_the_collector_is_accompanied_by"))<<soldiers<<mercenarieLocalize(Loc::text("ui.soldiers_pay_in_full_now_or_decline_and_face"));}else if(!s.firstIntroductionDone){t<<mercenarieLocalize(Loc::text("ui.your_company_now_has_an_official_establishment"))<<mercenarieLocalize(fiscalOrgName(o))<<mercenarieLocalize(Loc::text("ui.keeps_the_accounts_of_your_contracts_since_their_payment"))<<(o==FISCAL_UC?20:10)<<mercenarieLocalize(Loc::text("ui.reduced_to_5_for_an_ally_a_collector_will"));s.firstIntroductionDone=true;}else t<<mercenarieLocalize(Loc::text("ui.we_came_back_for_the_collection_here_s_what"));t<<mercenarieLocalize(Loc::text("ui.total_of"))<<s.debt<<Loc::text("ui.cats");MercenarieFonts::caption(fiscalCollectorText,t.str());MercenarieFonts::caption(fiscalCollectorPay,mercenarieLocalize(Loc::text("ui.pay"))+FiscalLedger::toString((unsigned long)s.debt)+Loc::text("ui.cats_3f70b99"));MercenarieFonts::caption(fiscalCollectorDetail,Loc::text("ui.view_details"));MercenarieFonts::caption(fiscalCollectorExtension,fiscalParty.raid>0?Loc::text("ui.no_delay"):Loc::text("ui.7_day_extension"));fiscalCollectorExtension->setEnabled(fiscalParty.raid==0&&!s.extensionUsed);MercenarieFonts::caption(fiscalCollectorRefuse,fiscalParty.raid>0?Loc::text("ui.refuse_and_fight"):Loc::text("common.refuser"));s.state=FSTATE_DIALOGUE;s.newTaxesAtLastVisit=s.debt;saveFiscalLedger();fiscalCollectorWindow->setVisible(true);
    }

    int reputationPlaceType(const std::string& name)
    {
        std::string lower=name;for(size_t i=0;i<lower.size();++i)lower[i]=static_cast<char>(tolower(static_cast<unsigned char>(lower[i])));
        if(lower.find("zone:")==0||lower.find("region:")==0) return 3;
        if(lower.find("ferme")!=std::string::npos||lower.find("farm")!=std::string::npos||lower.find("village")!=std::string::npos||lower.find("waystation")!=std::string::npos||lower.find("avant-poste")!=std::string::npos) return 2;
        return 1;
    }

    const char* reputationTypeName(int type){return type==2?"VILLAGE":type==3?"ZONE":Loc::text("ui.city_e168e26");}
    const char* reputationTypeTexture(int type){return type==2?"GuildReputationVillage.png":type==3?"GuildReputationZone.png":"GuildReputationCity.png";}

    std::string reputationDetails(const std::string& name,float local,int type)
    {
        std::stringstream s;s<<frenchPlaceName(name)<<"\n"<<reputationRank(local)<<" : "<<local<<"\n";
        if(type==3){s<<(Loc::text("ui.legacy_region_record_no_gameplay_effect"));return s.str();}
        s<<(Loc::text("ui.global_reputation_aaa7942"))<<escortReputation<<"\n\n"
         <<(Loc::text("ui.negotiation"))<<std::showpos<<GuildProgression::chanceBonus(local,escortReputation)*100<<Loc::text("ui.pts")
         <<(Loc::text("ui.client_frequency"))<<(GuildProgression::visitorRate(local,escortReputation)-1)*100<<" %\n\n"
         <<(Loc::text("ui.base_prices_and_level_unlocks_unchanged"));return s.str();
    }

    void updateOfficesPage(){refreshOfficesView();}
    void updateReputationPage(){refreshReputation105();}

    void guildTabClicked(MyGUI::WidgetPtr sender){int tab=sender==guildTabButtons[5]?5:sender==guildTabButtons[1]?1:sender==guildTabButtons[2]?2:sender==guildTabButtons[3]?3:sender==guildTabButtons[4]?4:0;for(int i=0;i<8;++i)if(guildOverviewPanels[i])guildOverviewPanels[i]->setVisible(tab==0);closeGuildContractConfirm();if(overviewChrome)overviewChrome->setVisible(true);if(registerLevelTip)registerLevelTip->setVisible(false);if(registerLevelPopup)registerLevelPopup->setVisible(false);if(guildRelationsPanel)guildRelationsPanel->setVisible(tab==1);if(guildOfficesPanel)guildOfficesPanel->setVisible(tab==2);if(guildReputationPanel)guildReputationPanel->setVisible(tab==3);if(guildFinancesPanel)guildFinancesPanel->setVisible(tab==4);if(guildOptionsPanel)guildOptionsPanel->setVisible(tab==5);if(clientOptionTip)clientOptionTip->setVisible(false);if(contractRewardTip)contractRewardTip->setVisible(false);for(int i=0;i<6;++i)if(guildTabButtons[i])guildTabButtons[i]->setStateSelected(i==tab);registerSelectTab(tab);if(tab==1)refreshGuildContracts(true);if(tab==2)updateOfficesPage();if(tab==3)updateReputationPage();if(tab==4)updateFinancesPage();if(tab==5)openOptions108();refreshOverview95();}

    void updateGuildMenu(){
        if(!guildWindow)return;refreshRegisterOverview();
        if(guildReputationPanel&&guildReputationPanel->getVisible())updateReputationPage();
        if(guildRelationsPanel&&guildRelationsPanel->getVisible())updateRelationsPage();
        if(guildOfficesPanel&&guildOfficesPanel->getVisible())updateOfficesPage();
        if(guildFinancesPanel&&guildFinancesPanel->getVisible())updateFinancesPage();
    }

    void guildWindowButtonPressed(MyGUI::Window* window, const std::string&)
    {
        if(guildInvestmentOverlay&&guildInvestmentOverlay->getVisible()){closeGuildInvestment(0);return;}
        closeGuildInvestment(0);
        closeGuildContractConfirm();registerCloseLevelPopup(0);if(registerLevelTip)registerLevelTip->setVisible(false);
        if (window) window->setVisible(false);
    }

    void createGuildMenu()
    {
        if (guildWindow || !MyGUI::Gui::getInstancePtr()) return;
        MyGUI::Gui* g=MyGUI::Gui::getInstancePtr();
        const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();
        guildLayout=GuildResponsive::calculate(view.width,view.height);guildLayoutViewportW=view.width;guildLayoutViewportH=view.height;
        int height=guildLayout.windowH,width=height*1536/800;
        if(width>guildLayout.windowW){width=guildLayout.windowW;height=width*800/1536;}
        Ogre::ResourceGroupManager& resources=Ogre::ResourceGroupManager::getSingleton();
        if(!resources.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))resources.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");
        MyGUI::ResourceManager::getInstance().load("MercenarieOverviewSkins.xml");
        MyGUI::ResourceManager::getInstance().load("MercenarieGuildFinish.xml");
        if(!MyGUI::ResourceManager::getInstance().isExist("ContractBoardWindowV6"))MyGUI::ResourceManager::getInstance().load("ContractBoardV6.xml");
        guildWindow=g->createWidget<MyGUI::Window>("ContractBoardWindowV6",(view.width-width)/2,(view.height-height)/2,width,height,MyGUI::Align::Default,"Window","GuildContractsMenu");
        MercenarieFonts::caption(guildWindow,Loc::text("ui.the_mercenarie_guild_management"));
        guildWindow->eventWindowButtonPressed+=MyGUI::newDelegate(guildWindowButtonPressed);
        MyGUI::Widget* c=guildWindow->getClientWidget();
        MyGUI::ResourceManager::getInstance().load("GuildRegisterSkins.xml");
        // Canonical layout: the full tree is scaled once at the end.
        int cw=guildLayout.designW,ch=guildLayout.designH;
        // The native Kenshi window already owns the title and close button.
        // Start the application content directly below its client edge.
        int margin=guildLayout.outerMargin,gap=guildLayout.gap,tabH=140,navW=guildLayout.navigation.w,contentY=18,contentH=ch-contentY-20,mainX=margin+navW+gap+2,mainW=cw-mainX-margin;
        MyGUI::ImageBox* contractsBg=0;int pagePad=22,pageGap=14;

        float optionsScale=std::min(c->getWidth()/float(guildLayout.designW),c->getHeight()/float(guildLayout.designH));
        GuildOverview::Layout optionsLayout=GuildOverview::calculate(c->getWidth(),c->getHeight());
        int optionsX=optionsLayout.glance.x,optionsY=optionsLayout.sidebar.y;
        buildClientOptions(c,int(optionsX/optionsScale),int(optionsY/optionsScale),int((c->getWidth()-optionsX-optionsLayout.pad)/optionsScale),int((c->getHeight()-optionsY-optionsLayout.pad)/optionsScale));
        buildGuildInvestment();
        float registerScale=std::min(c->getWidth()/float(guildLayout.designW),c->getHeight()/float(guildLayout.designH));
        // MyGUI's generic scaler only sees TextBox widgets. Custom Button
        // captions live in a SimpleText subwidget, so scale those explicitly.
        scaleOptionButtonFonts(guildOptionsPanel,registerScale);
        scaleRegisterChildren(c,registerScale,registerScale);
        buildRegisterOverview(c,0,0,0,0,0,0,0);

        buildGuildOfficesReputation(c);
        buildFinanceView(c);
        buildOverview95(c);
        buildGuildContracts(overview95Root);
        buildOffices101(overview95Root);
        buildOptions103();
        buildReputation105();
        buildFinance106();
        guildWindow->setVisible(false);
    }

    void rebuildGuildMenuForViewport(){
        if(!guildWindow)return;if(MyGUI::Gui::getInstancePtr()&&mercenarieFindLiveWidget(MyGUI::Gui::getInstance().getEnumerator(),guildWindow))closeGuildInvestment(0);else{guildInvestmentOverlay=0;investmentPreviousPages.clear();investmentPageActive=false;}resetFinanceView();resetGuildPages();resetGuildMenuViewReferences();mercenarieDestroyLiveWidget(guildWindow);guildWindow=0;
        registerOfficeMarkers.clear();investmentPreviousPages.clear();investmentPageActive=false;guildInvestmentOverlay=0;
        for(int i=0;i<8;++i)guildOverviewPanels[i]=0;for(int i=0;i<6;++i)guildTabButtons[i]=0;
        guildRelationsPanel=guildOfficesPanel=guildReputationPanel=guildFinancesPanel=guildOptionsPanel=0;
        o103Ready=false;optionsScroll=0;optionsSearch=0;optionsSearchHint=0;optionsCategoryFilter=0;optionsResetAllButton=0;optionsRows.clear();guildKeyLabel=0;guildKeyTip=0;clientTrackerCheck=0;clientOptionTip=0;contractRewardSlider=0;contractRewardValue=0;contractRewardTip=0;for(int i=0;i<ClientOptions::CategoryCount;++i){optionsCategoryHeaders[i]=0;optionsCategoryBodies[i]=0;optionsCategoryChevrons[i]=0;optionsCategoryTitles[i]=0;optionsCategoryDescriptions[i]=0;}for(int i=0;i<ClientOptions::ActionCount;++i)optionKeyLabels[i]=0;
    }
    void toggleGuildManagement(MyGUI::Widget*){if(mercenarieGameplayUnavailable()){showMercenarieUnavailable();return;}if(negotiationOpen)return;guildKeyCapture=false;closeMercenarieLauncher(0);const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();if(guildWindow&&(v.width!=guildLayoutViewportW||v.height!=guildLayoutViewportH))rebuildGuildMenuForViewport();createGuildMenu();refreshGuildKeyLabel();if(guildWindow){bool show=!guildWindow->getVisible();if(show){registerCarouselInitialized=false;openOptions108();updateGuildMenu();}else{closeGuildInvestment(0);closeGuildContractConfirm();registerCloseLevelPopup(0);if(registerLevelTip)registerLevelTip->setVisible(false);}guildWindow->setVisible(show);}}
    void closeAutomaticQuestWindow(MyGUI::WidgetPtr){if(automaticQuestWindow)automaticQuestWindow->setVisible(false);}
    void missionBookWindowButtonPressed(MyGUI::Window*,const std::string&){closeAutomaticQuestWindow(0);}
    std::string missionBookTypeName(int type){if(type==MCT_CARAVAN)return Loc::text("ui.caravan_879cb28");if(type==MCT_SCIENCE)return Loc::text("ui.scientific_expedition_e4b8223");if(type==MCT_MAIL)return Loc::text("v8.literal.003");return Loc::text("ui.escort_2e246e4");}
    std::string missionBookDifficulty(int level){const char* keys[]={"v8.difficulty.0","v8.difficulty.1","v8.difficulty.2","v8.difficulty.3","v8.difficulty.4","v8.difficulty.5"};return Loc::text(keys[std::max(0,std::min(5,level))]);}
    void setMissionBookOffice(Building* book){Building* house=book?book->furnitureParentBuilding():0;TownBase* stableTown=house?house->getCurrentTownLocation():0;missionBookTownId=stableTown&&stableTown->getGameData()?stableTown->getGameData()->stringID:std::string();missionBookOfficeKey=house?guildHouseKey(house):std::string();missionBookOfficeName=guildHouseNames.count(missionBookOfficeKey)?guildHouseNames[missionBookOfficeKey]:(Loc::text("ui.guild_house_6620bc7"));if(guildHouseCities.count(missionBookOfficeKey))missionBookOfficeCity=guildHouseCities[missionBookOfficeKey];else{TownBase* town=house?house->getCurrentTownLocation():0;missionBookOfficeCity=town?frenchPlaceName(town->getName()):Loc::text("ui.unknown_city");}}
    bool shinobiAllied(){if(!ou||!ou->player||!ou->player->participant||!ou->factionMgr)return false;lektor<GameData*> factions;ou->gamedata.getDataOfType(factions,FACTION);for(unsigned int i=0;i<factions.size();++i){GameData* data=factions[i];if(!data)continue;std::string n=data->name;std::transform(n.begin(),n.end(),n.begin(),::tolower);if(n.find("shinobi")!=std::string::npos){Faction* faction=ou->factionMgr->getFactionByStringID(data->stringID);if(faction&&faction->relations&&faction->relations->isAlly(ou->player->participant))return true;}}return false;}
    Character* missionBookTownBarman(){if(!ou||!ou->factionMgr||missionBookTownId.empty())return 0;const lektor<Faction*>* factions=ou->factionMgr->getAllFactions();if(!factions)return 0;for(unsigned int f=0;f<factions->size();++f){Faction* faction=(*factions)[f];if(!faction)continue;for(unsigned int p=0;p<faction->activePlatoons.size();++p){Platoon* platoon=faction->activePlatoons[p];if(!platoon||!platoon->activePlatoon)continue;for(unsigned int i=0;i<platoon->activePlatoon->things.size();++i){Character* actor=dynamic_cast<Character*>(platoon->activePlatoon->things[i]);TownBase* town=actor?actor->getCurrentTownLocation():0;if(actor&&town&&town->getGameData()&&town->getGameData()->stringID==missionBookTownId&&isBarman(actor))return actor;}}}return 0;}
    void synchronizeMissionBookLegalBoard(){Character* barman=missionBookTownBarman();if(!barman)return;openContractsBoard(barman,false,false);if(contractsWindow)contractsWindow->setVisible(false);}
    std::string missionBookLegalText(){missionBookEntries.clear();synchronizeMissionBookLegalBoard();loadContractBoards();std::string key=missionBookOfficeCity+"#TAVERN";std::map<std::string,CityContractBoard>::const_iterator found=savedContractBoards.find(key);if(found==savedContractBoards.end())return Loc::text("v8.literal.004");for(int i=0;i<6;++i){const BoardOffer& offer=found->second.offers[i];if(!offer.available)continue;std::ostringstream row;row<<missionBookTypeName(offer.missionType)<<" | "<<(Loc::text("contracts.ui.barman"))<<" | "<<offer.townName<<" | "<<missionBookDifficulty(offer.dangerLevel)<<" | "<<displayedContractCats(offer.estimatedPay)<<Loc::text("ui.cats");missionBookEntries.push_back(MissionBookEntry(1,i,row.str()));}return missionBookEntries.empty()?(Loc::text("v8.literal.005")):std::string();}
    std::string missionBookClientText(){std::ostringstream out;int shown=0;for(size_t i=0;i<guildVisitors.size();++i){const GuildVisitor& v=guildVisitors[i];if(v.houseKey!=missionBookOfficeKey)continue;Character* client=v.leader;++shown;out<<shown<<". "<<(client?client->getName():(Loc::text("v8.literal.006")))<<" | "<<v.type<<"\n";}if(!shown)out<<(Loc::text("v8.literal.007"));out<<"\n\n"<<(Loc::text("v8.literal.008"));return out.str();}
    std::string missionBookActiveText(){
        std::ostringstream out;bool shown=false;
        for(size_t i=0;i<delegatedMissions.size();++i){const DelegatedMissionState& mission=delegatedMissions[i];if(!mission.timing.active&&mission.paymentState!=1&&mission.paymentState!=2)continue;if(shown)out<<"\n\n";out<<(Loc::text("v8.literal.009"))<<" | "<<mission.title<<"\n"<<(Loc::text("v8.literal.010"))<<mission.origin<<"\n"<<(Loc::text("ui.destination_c0903ba"))<<mission.destination<<"\n"<<(Loc::text("v8.literal.011"))<<mission.reward<<" Cats\n";if(mission.timing.active){DelegatedMissionTiming::PublicEstimate estimate=DelegatedMissionTiming::remainingEstimate(mission.timing,currentGameHours);out<<(Loc::text("v8.literal.012"))<<(int)std::floor(estimate.minimumHours)<<" h - "<<(int)std::ceil(estimate.maximumHours)<<(std::string(Loc::text("ui.h"))+"\n")<<(Loc::text("v8.literal.013"));}else out<<Loc::text("delegated.report.payment")<<" "<<Loc::text("delegated.report.pending");shown=true;}
        if(missionPending||missionActive){if(shown)out<<"\n\n";out<<(Loc::text("v8.literal.014"))<<" | "<<missionBookTypeName(currentContract.type)<<"\n"<<(Loc::text("v8.literal.015"))<<currentContract.origin<<"\n"<<(Loc::text("ui.destination_c0903ba"))<<currentContract.destination<<"\n"<<(Loc::text("v8.literal.011"))<<currentContract.totalPay<<Loc::text("ui.cats");shown=true;}
        for(size_t c=0;c<mailContracts.size();++c){const MailContracts::Contract& mail=mailContracts[c];if(mail.status!=MailContracts::MailActive)continue;if(shown)out<<"\n\n";out<<(Loc::text("v8.literal.016"))<<"\n"<<(Loc::text("v8.literal.015"))<<mail.originTownName<<"\n"<<(Loc::text("v8.literal.017"))<<MailContracts::deliveredCount(mail)<<" / "<<mail.steps.size()<<"\n"<<(Loc::text("v8.literal.018"));for(size_t s=0;s<mail.steps.size();++s){if(s)out<<", ";out<<mail.steps[s].townName;}out<<"\n"<<(Loc::text("v8.literal.011"))<<mail.reward<<Loc::text("ui.cats");if(MailContracts::hasDeadline(mail))out<<"\n"<<(Loc::text("v8.literal.019"))<<std::max(0,(int)ceil(mail.deadlineWorldHour-currentGameHours))<<Loc::text("ui.h");shown=true;}
        if(!shown)out<<(Loc::text("v8.literal.020"));std::string bounty=registerBountySummary();if(!bounty.empty())out<<"\n\n"<<(Loc::text("v8.literal.021"))<<bounty;return out.str();}
    std::string missionBookHistoryText(){missionBookEntries.clear();for(int i=(int)delegatedMissions.size()-1;i>=0&&missionBookEntries.size()<6;--i)if(delegatedMissions[i].completed){const DelegatedMissionState& m=delegatedMissions[i];std::ostringstream row;row<<m.title<<" | "<<m.origin<<" > "<<m.destination<<" | "<<(m.success?Loc::text("delegated.report.success"):Loc::text("delegated.report.failure"));missionBookEntries.push_back(MissionBookEntry(3,i,row.str()));}if(!missionBookEntries.empty())return Loc::text("delegated.report.history_help");std::ostringstream history;if(contractHistory.empty())history<<(Loc::text("v8.literal.022"));else for(size_t i=0;i<contractHistory.size()&&i<50;++i)history<<i+1<<". "<<GuildHistory::parse(contractHistory[i]).route<<"\n";return history.str();}
    void refreshMissionBook(){if(!missionBookContentText)return;missionBookEntries.clear();const char* fr[]={Loc::text("v8.book.tab.0"),Loc::text("v8.book.tab.1"),Loc::text("v8.book.tab.2"),Loc::text("v8.book.tab.3"),Loc::text("v8.book.tab.4"),Loc::text("v8.book.tab.5")};const char** en=fr;for(int i=0;i<6;++i){MercenarieFonts::caption(missionBookTabs[i],gMercenarieEnglish?en[i]:fr[i]);missionBookTabs[i]->setStateSelected(i==missionBookTab);}std::ostringstream office;office<<(Loc::text("v8.literal.010"))<<missionBookOfficeName<<"  |  "<<(Loc::text("v8.literal.023"))<<missionBookOfficeCity;MercenarieFonts::caption(missionBookOfficeText,office.str());std::string text;if(missionBookTab==0)text=missionBookLegalText();else if(missionBookTab==1)text=missionBookSecurityText(missionBookOfficeCity);else if(missionBookTab==2)text=shinobiAllied()?(Loc::text("v8.literal.024")):(Loc::text("v8.literal.025"));else if(missionBookTab==3)text=missionBookClientText();else if(missionBookTab==4)text=missionBookActiveText();else text=missionBookHistoryText();MercenarieFonts::caption(missionBookContentText,text);for(int i=0;i<6;++i){bool visible=i<(int)missionBookEntries.size();missionBookOfferButtons[i]->setVisible(visible);if(visible){MercenarieFonts::caption(missionBookOfferButtons[i],missionBookEntries[i].label);missionBookOfferButtons[i]->setUserString("entry",registerNumber(i));}}}
    void missionBookTabClicked(MyGUI::WidgetPtr sender){for(int i=0;i<6;++i)if(sender==missionBookTabs[i]){missionBookTab=i;break;}refreshMissionBook();}
    void createAutomaticQuestWindow(){if(automaticQuestWindow||!MyGUI::Gui::getInstancePtr())return;const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();int w=std::min(1180,view.width-30),h=std::min(760,view.height-30);automaticQuestWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","MissionBookManagementWindow");automaticQuestWindow->eventWindowButtonPressed+=MyGUI::newDelegate(missionBookWindowButtonPressed);MercenarieFonts::caption(automaticQuestWindow,Loc::text("v8.literal.026"));MyGUI::Widget* c=automaticQuestWindow->getClientWidget();applyMercenarieFrame(c,true);missionBookOfficeText=registerText(c,22,16,w-44,34,21,"",registerAmber);int gap=6,bw=(w-44-gap*5)/6;for(int i=0;i<6;++i){missionBookTabs[i]=c->createWidget<MyGUI::Button>("Kenshi_Button1",22+i*(bw+gap),58,bw,46,MyGUI::Align::Default);missionBookTabs[i]->setFontHeight(16);missionBookTabs[i]->eventMouseButtonClick+=MyGUI::newDelegate(missionBookTabClicked);}missionBookOffersPanel=registerPanel(c,22,116,w-44,h-194);missionBookContentText=registerText(missionBookOffersPanel,22,18,missionBookOffersPanel->getWidth()-44,missionBookOffersPanel->getHeight()-36,18,"",registerIvory);missionBookContentText->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);for(int i=0;i<6;++i){missionBookOfferButtons[i]=missionBookOffersPanel->createWidget<MyGUI::Button>("Kenshi_Button1",18,14+i*70,missionBookOffersPanel->getWidth()-36,62,MyGUI::Align::Default);missionBookOfferButtons[i]->setFontHeight(17);missionBookOfferButtons[i]->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);missionBookOfferButtons[i]->eventMouseButtonClick+=MyGUI::newDelegate(openMissionBookOffer);missionBookOfferButtons[i]->setVisible(false);}MyGUI::Button* close=c->createWidget<MyGUI::Button>("Kenshi_Button1",w-242,h-68,220,42,MyGUI::Align::Default);MercenarieFonts::caption(close,Loc::text("delegated.report.close"));close->eventMouseButtonClick+=MyGUI::newDelegate(closeAutomaticQuestWindow);automaticQuestWindow->setVisible(false);}
    void openMissionBookManagementFor(Building* book){if(negotiationOpen||!canUseMissionBook(book))return;if(contractsWindow&&contractsWindow->getVisible()&&missionBookDelegationContext)return;setMissionBookOffice(book);guildKeyCapture=false;closeMercenarieLauncher(0);missionBookDelegationContext=true;missionBookTab=0;missionBookCategoryFilter=MissionBookContracts::All;missionBookOfferPage=0;missionBookRosterIdentity.clear();missionBookSelectedCharacters.clear();missionBookPoolReady=false;missionBookSelectedId.clear();Character* giver=missionBookTownBarman();if(!giver){missionBookDelegationContext=false;return;}openContractsBoard(giver,false,false);initializeMissionBookPool();if(missionBookCategoryCombo)missionBookCategoryCombo->setIndexSelected(0);refreshMissionBookContractHub();if(contractsAccept)MercenarieFonts::caption(contractsAccept,Loc::text("v8.literal.027"));}
    void openMissionBookOffer(MyGUI::WidgetPtr sender){int row=atoi(sender->getUserString("entry").c_str());if(row<0||row>=(int)missionBookEntries.size())return;MissionBookEntry entry=missionBookEntries[row];missionBookDelegationContext=true;if(automaticQuestWindow)automaticQuestWindow->setVisible(false);if(entry.kind==1){Character* giver=missionBookTownBarman();if(!giver){missionBookDelegationContext=false;return;}openContractsBoard(giver,false,false);selectedOffer=entry.index;updateContractsBoard();if(contractsAccept)MercenarieFonts::caption(contractsAccept,Loc::text("missionbook.take"));}else if(entry.kind==2)openMissionBookBountyDetail(entry.index);else if(entry.kind==3)showDelegatedReport(entry.index);}
    void delegationRosterWindowClosed(MyGUI::Window*,const std::string&){missionBookDelegationContext=false;delegationConfirming=false;delegationFinalArmed=false;if(delegationRosterWindow)delegationRosterWindow->setVisible(false);}
    int delegationSelectedCount(){int count=0;for(size_t i=0;i<delegationRosterSelected.size();++i)if(delegationRosterSelected[i])++count;return count;}
    int delegationAvailableCount(){int count=0;for(size_t i=0;i<delegationRosterAvailable.size();++i)if(delegationRosterAvailable[i])++count;return count;}
    int delegationCategory(Character* character)
    {
        CharStats* s=character?character->getStats():0;if(!s)return 32;
        const float combat=std::max(s->getMeleeAttack(),std::max(s->getMeleeDefence(false),s->toughness()));
        const float ranged=std::max(s->bows,s->turrets),guard=(s->getMeleeDefence(false)+s->toughness())*.5f;
        const float work=std::max(s->labouring,std::max(s->engineer,std::max(s->farming,s->cooking)));
        const float specialist=std::max(s->science,std::max(s->medic,std::max(s->robotics,std::max(s->stealth,s->assassin))));
        return MercenarySelectionRules::categoryMask(combat,ranged,guard,work,specialist);
    }
    std::string delegationRole(Character* character)
    {
        CharStats* s=character?character->getStats():0;if(!s)return Loc::text("v8.literal.028");
        float score[5]={std::max(s->getMeleeAttack(),s->getMeleeDefence(false)),std::max(s->bows,s->turrets),(s->getMeleeDefence(false)+s->toughness())*.5f,std::max(s->labouring,std::max(s->engineer,std::max(s->farming,s->cooking))),std::max(s->science,std::max(s->medic,std::max(s->robotics,std::max(s->stealth,s->assassin))))};
        int best=MercenarySelectionRules::primaryCategory(score[0],score[1],score[2],score[3],score[4])-1;const char* keys[]={"v8.role.0","v8.role.1","v8.role.2","v8.role.3","v8.role.4"};return Loc::text(keys[best]);
    }
    std::string delegationStats(Character* character)
    {
        CharStats* s=character?character->getStats():0;if(!s)return std::string();std::ostringstream out;
        if(delegationOfferKind==1&&delegationOfferIndex>=0){std::map<std::string,CityContractBoard>::const_iterator board=savedContractBoards.find(delegationBoardKey);if(board!=savedContractBoards.end()&&delegationOfferIndex<6&&board->second.offers[delegationOfferIndex].missionType==2){out<<(Loc::text("v8.literal.029"))<<(int)s->science<<"  "<<(Loc::text("v8.literal.030"))<<(int)s->medic;return out.str();}}
        out<<(Loc::text("v8.literal.031"))<<(int)s->getMeleeAttack()<<"  "<<(Loc::text("v8.literal.032"))<<(int)s->getMeleeDefence(false);return out.str();
    }
    void prepareMissionBookRoster()
    {
        if(!missionBookDelegationContext||!ou||!ou->player)return;
        // The roster comes from live player handles, independently of offer availability.
        std::string identity=missionBookSelectedId;
        if(identity!=missionBookRosterIdentity){missionBookSelectedCharacters.clear();delegationConfirming=false;delegationFinalArmed=false;}
        missionBookRosterIdentity=identity;
        delegationOfferKind=0;delegationOfferIndex=-1;
        if(missionBookSelectionValid()){
            if(missionBookTab==1)prepareMissionBookSecurityIdentity(selectedOffer);
            else {delegationOfferKind=1;delegationOfferIndex=selectedOffer;delegationBoardKey=currentBoardKey;delegationOfferIdentity=identity;}
        }
        delegationRosterCharacters.clear();delegationRosterSelected.clear();delegationRosterAvailable.clear();delegationRosterCategories.clear();missionBookSoldierOrder.clear();
        int viable=0;for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* actor=ou->player->playerCharacters[i];if(actor&&!actor->isDead()&&!delegatedCharacterAbsent(actor))++viable;}
        std::set<std::string> retained;
        for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* actor=ou->player->playerCharacters[i];if(!actor)continue;
            const std::string id=actor->getHandle().toString();bool available=!actor->isDead()&&!delegatedCharacterAbsent(actor)&&viable>1;
            bool selected=available&&missionBookSelectedCharacters.count(id)!=0;
            delegationRosterCharacters.push_back(actor);delegationRosterSelected.push_back(selected);delegationRosterAvailable.push_back(available);delegationRosterCategories.push_back(delegationCategory(actor));missionBookSoldierOrder.push_back(delegationRosterCharacters.size()-1);
            if(selected)retained.insert(id);
        }
        missionBookSelectedCharacters.swap(retained);
    }
    void refreshMissionBookDelegationPanel()
    {
        if(!missionBookDelegationPanel)return;missionBookDelegationPanel->setVisible(missionBookDelegationContext&&missionBookTab<2);if(!missionBookDelegationPanel->getVisible())return;prepareMissionBookRoster();
        std::vector<MissionBookContracts::Soldier> model;for(size_t i=0;i<delegationRosterCharacters.size();++i){Character* actor=delegationRosterCharacters[i];CharStats* stats=actor?actor->getStats():0;model.push_back(MissionBookContracts::Soldier(actor?actor->getHandle().toString():registerNumber((int)i),actor?actor->getName():std::string(),stats?(int)stats->getMeleeAttack():0,stats?(int)stats->getMeleeDefence(false):0,stats?(int)stats->toughness():0));}
        if(missionBookSoldierOrder.size()!=model.size()){missionBookSoldierOrder.clear();for(size_t i=0;i<model.size();++i)missionBookSoldierOrder.push_back(i);}MissionBookContracts::sortSoldiers(missionBookSoldierOrder,model,(MissionBookContracts::SoldierSort)missionBookSoldierSortMode);
        layoutMissionBookRoster();PortraitManager* portraits=PortraitManager::getInstance();for(size_t row=0;row<missionBookSoldierRows.size();++row){MissionBookSoldierWidgets& widget=missionBookSoldierRows[row];if(!widget.row)continue;bool shown=row<(int)missionBookSoldierOrder.size();widget.row->setVisible(shown);if(!shown)continue;size_t index=missionBookSoldierOrder[row];Character* actor=delegationRosterCharacters[index];const MissionBookContracts::Soldier& soldier=model[index];widget.row->setUserString("rosterIndex",registerNumber((int)index));widget.row->setUserString("rosterId",soldier.stableId);widget.check->setUserString("rosterId",soldier.stableId);if(portraits)portraits->setImageWidget(actor->getHandle(),widget.portrait,true);MercenarieFonts::caption(widget.name,soldier.name);bookEllipsis(widget.name);MercenarieFonts::caption(widget.stats,payrollRankLabel(actor));for(int f=widget.stats->getFontHeight();f>10&&widget.stats->getTextSize().width>widget.stats->getWidth();--f)widget.stats->setFontHeight(f-1);widget.defence->setVisible(false);widget.endurance->setVisible(false);widget.check->setStateSelected(delegationRosterSelected[index]);widget.check->setUserString("rosterIndex",registerNumber((int)index));MercenarySelectionRules::SelectResult decision=MercenarySelectionRules::maySelect(delegationRosterSelected[index],delegationRosterAvailable[index],delegationSelectedCount(),delegationAvailableCount());widget.row->setEnabled(decision==MercenarySelectionRules::SelectAllowed||decision==MercenarySelectionRules::AlreadySelected);widget.row->setStateSelected(delegationRosterSelected[index]);widget.row->setAlpha(delegationRosterAvailable[index]?1.0f:.38f);}
        refreshMissionBookTeamPortraits();
        if(missionBookTimingText&&selectedOffer>=0&&selectedOffer<6){BoardOffer& offer=boardOffers[selectedOffer];DelegatedMissionTiming::ActivityType activity=missionBookTab==1?DelegatedMissionTiming::ActivityBountyHunt:offer.missionType==MCT_ESCORT?DelegatedMissionTiming::ActivityEscort:offer.missionType==MCT_CARAVAN?DelegatedMissionTiming::ActivityCaravan:offer.missionType==MCT_MAIL?DelegatedMissionTiming::ActivityMailDelivery:DelegatedMissionTiming::ActivityScientificExpedition;double km=std::max(0.0f,offer.distance)*(missionBookTab==1||offer.missionType!=MCT_MAIL?(missionBookTab==1||offer.missionType!=MCT_ESCORT?2.0:1.0):1.0)/1000.0;try{DelegatedMissionTiming::State timing=DelegatedMissionTiming::start(activity,km,currentGameHours);std::ostringstream text;text<<(Loc::text("v8.literal.033"))<<(int)ceil(timing.travelHours)<<(std::string(Loc::text("ui.h"))+"\n")<<(Loc::text("v8.literal.034"))<<(int)ceil(timing.activityHours)<<(std::string(Loc::text("ui.h"))+"\n")<<(Loc::text("v8.literal.035"))<<(int)ceil(timing.travelHours+timing.activityHours)<<Loc::text("ui.h");MercenarieFonts::caption(missionBookTimingText,text.str());}catch(...){MercenarieFonts::caption(missionBookTimingText,Loc::text("v8.literal.036"));}}
        bool unlocked=GuildLevel3Access::available(guildLevel()),valid=unlocked&&missionBookSelectionValid()&&delegationSelectedCount()>0&&!delegationConfirming;if(missionBookDelegateButton)missionBookDelegateButton->setEnabled(valid);if(missionBookDelegationStatus)MercenarieFonts::caption(missionBookDelegationStatus,!unlocked?Loc::text("options.delegation.level3"):(delegationAvailableCount()==0?Loc::text("missionbook.roster.empty"):(delegationSelectedCount()==0?Loc::text("missionbook.select_member"):std::string())));
    }
    void refreshDelegationRosterCards()
    {
        const int selected=delegationSelectedCount(),available=delegationAvailableCount();
        if(delegationSelectionCount){std::ostringstream s;s<<selected<<" / 30";MercenarieFonts::caption(delegationSelectionCount,s.str());}
        if(delegationSelectionSummary){std::ostringstream s;s<<selected<<(Loc::text("v8.literal.037"));MercenarieFonts::caption(delegationSelectionSummary,s.str());}
        if(delegationConfirmButton)delegationConfirmButton->setEnabled(selected>0&&!delegationConfirming);
        if(delegationDeselectAllButton)delegationDeselectAllButton->setEnabled(selected>0);
        for(int f=0;f<6;++f)if(delegationFilterButtons[f])delegationFilterButtons[f]->setStateSelected(f==delegationRosterFilter);
        refreshMissionBookDelegationPanel();if(missionBookDelegationContext||!delegationRosterScroll)return;int width=delegationRosterScroll->getWidth()-24,cardW=250,gap=18,columns=MercenarySelectionRules::columnsForWidth(width,cardW,gap);cardW=(width-gap*(columns-1))/columns;int shown=0;
        for(size_t i=0;i<delegationRosterCards.size();++i){bool visible=MercenarySelectionRules::visibleInFilter(delegationRosterCategories[i],delegationRosterFilter);DelegationCardWidgets& cw=delegationRosterCards[i];cw.card->setVisible(visible);if(!visible)continue;int row=shown/columns,col=shown%columns;++shown;cw.card->setCoord(col*(cardW+gap),row*304,cardW,286);cw.portrait->setPosition((cardW-cw.portrait->getWidth())/2,14);cw.details->setCoord(12,182,cardW-24,86);cw.unavailable->setCoord(8,65,cardW-16,44);cw.card->setStateSelected(delegationRosterSelected[i]);MercenarySelectionRules::SelectResult decision=MercenarySelectionRules::maySelect(delegationRosterSelected[i],delegationRosterAvailable[i],selected,available);cw.card->setEnabled(decision==MercenarySelectionRules::SelectAllowed||decision==MercenarySelectionRules::AlreadySelected);MercenarieFonts::caption(cw.check,delegationRosterSelected[i]?"[X]":"[ ]");cw.check->setTextColour(delegationRosterSelected[i]?MyGUI::Colour(1.0f,.55f,.12f):MyGUI::Colour(.75f,.75f,.70f));bool mustRemain=decision==MercenarySelectionRules::LastCharacterRequired;cw.unavailable->setVisible(!delegationRosterAvailable[i]||mustRemain);if(mustRemain)MercenarieFonts::caption(cw.unavailable,Loc::text("v8.literal.038"));cw.portrait->setAlpha((delegationRosterAvailable[i]&&!mustRemain)?1.0f:.28f);cw.details->setAlpha((delegationRosterAvailable[i]&&!mustRemain)?1.0f:.42f);Character* rankActor=delegationRosterCharacters[i];if(rankActor)MercenarieFonts::caption(cw.details,rankActor->getName()+"\n"+delegationRole(rankActor)+"\n"+payrollRankLabel(rankActor));}
        int rows=(shown+columns-1)/columns;delegationRosterScroll->setCanvasSize(width,std::max(delegationRosterScroll->getHeight(),rows*304));delegationRosterScroll->setVisibleVScroll(rows*304>delegationRosterScroll->getHeight());
    }
    void delegationRosterToggle(MyGUI::WidgetPtr sender)
    {
        int index=atoi(sender->getUserString("rosterIndex").c_str());
        if(missionBookDelegationContext){const std::string id=sender->getUserString("rosterId");prepareMissionBookRoster();index=-1;for(size_t i=0;i<delegationRosterCharacters.size();++i)if(delegationRosterCharacters[i]->getHandle().toString()==id){index=(int)i;break;}}
        if(index<0||index>=(int)delegationRosterCharacters.size())return;
        MercenarySelectionRules::SelectResult decision=MercenarySelectionRules::maySelect(delegationRosterSelected[index],delegationRosterAvailable[index],delegationSelectedCount(),delegationAvailableCount());
        if(decision==MercenarySelectionRules::AlreadySelected)delegationRosterSelected[index]=false;
        else if(decision==MercenarySelectionRules::MaximumReached){if(ou)ou->showPlayerAMessage(Loc::text("v8.literal.039"),true);return;}
        else if(decision==MercenarySelectionRules::LastCharacterRequired){if(ou)ou->showPlayerAMessage(Loc::text("v8.literal.040"),true);return;}
        else if(decision!=MercenarySelectionRules::SelectAllowed)return;else delegationRosterSelected[index]=true;
        if(missionBookDelegationContext&&delegationRosterCharacters[index]){std::string id=delegationRosterCharacters[index]->getHandle().toString();if(delegationRosterSelected[index])missionBookSelectedCharacters.insert(id);else missionBookSelectedCharacters.erase(id);}
        refreshDelegationRosterCards();
    }
    void delegationRosterFilterClicked(MyGUI::WidgetPtr sender){delegationRosterFilter=atoi(sender->getUserString("filter").c_str());refreshDelegationRosterCards();}
    void missionBookSoldierSortChanged(MyGUI::ComboBox*,size_t index){missionBookSoldierSortMode=(int)std::min<size_t>(3,index);refreshMissionBookDelegationPanel();}
    void missionBookCategoryChanged(MyGUI::ComboBox*,size_t index){missionBookCategoryFilter=(int)std::min<size_t>(4,index);missionBookOfferPage=0;contractAcceptArmed=false;delegationFinalArmed=false;refreshMissionBookContractHub();}
    void missionBookPageClicked(MyGUI::WidgetPtr sender){missionBookOfferPage+=sender==missionBookPagePrevious?-1:1;contractAcceptArmed=false;delegationFinalArmed=false;refreshMissionBookContractHub();}
    void missionBookDelegateClicked(MyGUI::WidgetPtr){if(!GuildLevel3Access::available(guildLevel())){if(ou)ou->showPlayerAMessage(Loc::text("options.delegation.level3"),true);return;}prepareMissionBookRoster();if(!missionBookSelectionValid()||delegationSelectedCount()==0)return;delegationRosterConfirm(0);}
    void delegationRosterDeselectAll(MyGUI::WidgetPtr){missionBookSelectedCharacters.clear();for(size_t i=0;i<delegationRosterSelected.size();++i)delegationRosterSelected[i]=false;refreshDelegationRosterCards();}
    void closeDelegationRoster(MyGUI::WidgetPtr){missionBookDelegationContext=false;delegationConfirming=false;if(delegationRosterWindow)delegationRosterWindow->setVisible(false);}
    void beginDelegationSelection(int kind,int index)
    {
        if(!GuildLevel3Access::available(guildLevel())){if(ou)ou->showPlayerAMessage(Loc::text("options.delegation.level3"),true);return;}
        if(!ou||!ou->player)return;delegationOfferKind=kind;delegationOfferIndex=index;delegationRosterFilter=0;delegationConfirming=false;delegationFinalArmed=false;delegationRosterCharacters.clear();delegationRosterSelected.clear();delegationRosterAvailable.clear();delegationRosterCategories.clear();
        int viable=0;for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(c&&!c->isDead()&&!delegatedCharacterAbsent(c))++viable;}
        for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(!c)continue;bool available=!c->isDead()&&!delegatedCharacterAbsent(c)&&viable>1;delegationRosterCharacters.push_back(c);delegationRosterSelected.push_back(false);delegationRosterAvailable.push_back(available);delegationRosterCategories.push_back(delegationCategory(c));}
        MyGUI::Gui* guiInstance=MyGUI::Gui::getInstancePtr();if(!guiInstance)return;const MyGUI::IntSize& v=MyGUI::RenderManager::getInstance().getViewSize();int w=std::min(1240,v.width-24),h=std::min(900,v.height-24);
        if(!delegationRosterWindow){delegationRosterWindow=guiInstance->createWidget<MyGUI::Window>("Kenshi_WindowCX",(v.width-w)/2,(v.height-h)/2,w,h,MyGUI::Align::Default,"Window","DelegationRoster");delegationRosterWindow->eventWindowButtonPressed+=MyGUI::newDelegate(delegationRosterWindowClosed);MercenarieFonts::caption(delegationRosterWindow,Loc::text("v8.literal.041"));MyGUI::Widget* c=delegationRosterWindow->getClientWidget();const int cw=c->getWidth(),ch=c->getHeight();applyMercenarieFrame(c,true);delegationSelectionCount=registerText(c,cw-154,14,120,36,23,"0 / 30",registerAmber);delegationSelectionCount->setTextAlign(MyGUI::Align::Right|MyGUI::Align::VCenter);const char* fr[]={Loc::text("v8.roster.filter.0"),Loc::text("v8.roster.filter.1"),Loc::text("v8.roster.filter.2"),Loc::text("v8.roster.filter.3"),Loc::text("v8.roster.filter.4"),Loc::text("v8.roster.filter.5")};const char** en=fr;int filterGap=8,filterW=(cw-64-filterGap*5)/6;for(int f=0;f<6;++f){delegationFilterButtons[f]=c->createWidget<MyGUI::Button>("Kenshi_Button1",32+f*(filterW+filterGap),56,filterW,38,MyGUI::Align::Default);MercenarieFonts::caption(delegationFilterButtons[f],gMercenarieEnglish?en[f]:fr[f]);delegationFilterButtons[f]->setFontHeight(15);delegationFilterButtons[f]->setUserString("filter",registerNumber(f));delegationFilterButtons[f]->eventMouseButtonClick+=MyGUI::newDelegate(delegationRosterFilterClicked);}const int footerH=58,footerY=ch-32-footerH;delegationRosterScroll=c->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",32,108,cw-64,footerY-120,MyGUI::Align::Default);MercenarieNativeInput::bind(delegationRosterScroll);delegationRosterScroll->setVisibleHScroll(false);delegationRosterScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);delegationDeselectAllButton=c->createWidget<MyGUI::Button>("Kenshi_Button1",32,footerY,240,46,MyGUI::Align::Default);MercenarieFonts::caption(delegationDeselectAllButton,Loc::text("v8.literal.042"));delegationDeselectAllButton->eventMouseButtonClick+=MyGUI::newDelegate(delegationRosterDeselectAll);delegationSelectionSummary=registerText(c,288,footerY,cw-576,46,18,"",registerIvory);delegationSelectionSummary->setTextAlign(MyGUI::Align::Center);delegationConfirmButton=c->createWidget<MyGUI::Button>("Kenshi_Button1",cw-272,footerY,240,46,MyGUI::Align::Default);MercenarieFonts::caption(delegationConfirmButton,Loc::text("ui.confirm"));delegationConfirmButton->eventMouseButtonClick+=MyGUI::newDelegate(delegationRosterConfirm);}
        else{delegationRosterWindow->setCoord((v.width-w)/2,(v.height-h)/2,w,h);}
        for(size_t i=0;i<delegationRosterCards.size();++i)if(delegationRosterCards[i].card)guiInstance->destroyWidget(delegationRosterCards[i].card);delegationRosterCards.clear();PortraitManager* portraits=PortraitManager::getInstance();
        int portraitW=210,portraitH=160;if(portraits&&portraits->texturePortraitSize.x>0&&portraits->texturePortraitSize.y>0){portraitH=portraitW*portraits->texturePortraitSize.y/portraits->texturePortraitSize.x;if(portraitH>160){portraitH=160;portraitW=portraitH*portraits->texturePortraitSize.x/portraits->texturePortraitSize.y;}}
        for(size_t i=0;i<delegationRosterCharacters.size();++i){Character* character=delegationRosterCharacters[i];DelegationCardWidgets cw;cw.card=delegationRosterScroll->createWidget<MyGUI::Button>("Kenshi_Button1",0,0,250,286,MyGUI::Align::Default);cw.card->setUserString("rosterIndex",registerNumber((int)i));cw.card->eventMouseButtonClick+=MyGUI::newDelegate(delegationRosterToggle);cw.portrait=cw.card->createWidget<MyGUI::ImageBox>("ImageBox",(250-portraitW)/2,14,portraitW,portraitH,MyGUI::Align::Top);cw.portrait->setNeedMouseFocus(false);if(portraits)portraits->setImageWidget(character->getHandle(),cw.portrait,true);cw.check=cw.card->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",14,12,42,30,MyGUI::Align::Left|MyGUI::Align::Top);cw.check->setNeedMouseFocus(false);cw.check->setFontHeight(20);cw.details=cw.card->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",12,182,226,86,MyGUI::Align::HStretch|MyGUI::Align::Bottom);cw.details->setNeedMouseFocus(false);cw.details->setTextAlign(MyGUI::Align::Center);cw.details->setFontHeight(17);std::ostringstream info;info<<character->getName()<<"\n"<<delegationRole(character)<<"\n"<<payrollRankLabel(character);MercenarieFonts::caption(cw.details,info.str());cw.unavailable=cw.card->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",8,65,234,44,MyGUI::Align::HStretch|MyGUI::Align::Top);cw.unavailable->setNeedMouseFocus(false);cw.unavailable->setTextAlign(MyGUI::Align::Center);cw.unavailable->setFontHeight(15);cw.unavailable->setTextColour(MyGUI::Colour(1.0f,.55f,.12f));MercenarieFonts::caption(cw.unavailable,delegatedCharacterAbsent(character)?(Loc::text("v8.literal.043")):(character->isDead()?(Loc::text("v8.literal.044")):(Loc::text("v8.literal.038"))));delegationRosterCards.push_back(cw);}
        refreshDelegationRosterCards();delegationRosterWindow->setVisible(true);
    }
    void updateGuildMenuHotkey()
    {
        if (!key || !key->keyboard) return;
        if(guildKeyCapture){
            jWasDown=key->keyboard->isKeyDown((OIS::KeyCode)clientOptions.bindings[ClientOptions::OpenGuildManagement]);
            if(!guildWindow||!guildWindow->getVisible()||!guildOptionsPanel||!guildOptionsPanel->getVisible()){guildKeyCapture=false;refreshGuildKeyLabel();return;}
            for(int k=1;k<256;++k){bool pressed=key->keyboard->isKeyDown((OIS::KeyCode)k);bool edge=pressed&&!guildKeyStates[k];guildKeyStates[k]=pressed;
                if(!edge)continue;
                if(k==OIS::KC_ESCAPE){guildKeyCapture=false;pendingConflictAction=-1;pendingConflictKey=0;refreshGuildKeyLabel();return;}
                if(!validGuildKey(k)){if(optionKeyLabels[capturedAction])MercenarieFonts::caption(optionKeyLabels[capturedAction],Loc::text("ui.invalid_key_choose_a_letter_or_f1_f12"));continue;}
                bool reassign=pendingConflictKey==k&&pendingConflictAction>=0;if(saveBinding(capturedAction,k,reassign)){guildKeyCapture=false;jWasDown=capturedAction==ClientOptions::OpenGuildManagement;pWasDown=capturedAction==ClientOptions::OpenCheatMenu;autopilotWasDown=capturedAction==ClientOptions::OpenAutopilot;refreshOptionKeyLabels();}else if(pendingConflictAction>=0&&optionKeyLabels[capturedAction])MercenarieFonts::caption(optionKeyLabels[capturedAction],Loc::text("options.key.conflict"));return;
            }return;
        }
        bool down=key->keyboard->isKeyDown((OIS::KeyCode)clientOptions.bindings[ClientOptions::OpenGuildManagement]);
        if(down && !jWasDown && (!negotiationOpen||mercenarieGameplayUnavailable()) && !gbModalOpen())
        {
            toggleGuildManagement(0);
        }
        jWasDown=down;
    }

    struct DestinationOption { const char* id; const char* name; };

    bool invalidContractDestination(const std::string& id)
    {
        return ContractDestinationRules::blocked(id);
    }

    void chooseDestination(const std::string& choice, const Ogre::Vector3& origin)
    {
        MercenariePerf::Phase perf("destinations");
        destinationTown="";destinationNameStorage.clear();destinationName=destinationNameStorage.c_str();selectedDistance=0;
        struct Ranked { Town* town; float distance; };
        std::vector<Ranked> ranked;
        if(!shou||!shou->townList)return;
        lektor<RootObject*>& towns=shou->townList->getAllTowns();
        for(unsigned int i=0;i<towns.size();++i){
            Town* town=dynamic_cast<Town*>(towns[i]);
            if(!town||!town->getGameData()||town->getName().empty())continue;
            // Exact Kenshi types only: small ruins/outposts never become villages.
            if(town->townType!=TOWN_TOWN&&town->townType!=TOWN_VILLAGE)continue;
            if(invalidContractDestination(town->getGameData()->stringID)||destinationAtWar(town))continue;
            float d = origin.squaredDistance(town->getPosition());
            if (d < 1000000.0f) continue;
            Ranked candidate={town,d};ranked.push_back(candidate);
        }
        const int count=(int)ranked.size();if(!count){ErrorLog("Guild Escort: no valid town/village destination");return;}
        for (int i = 0; i < count; ++i) for (int j = i + 1; j < count; ++j)
            if (ranked[j].distance < ranked[i].distance) { Ranked t = ranked[i]; ranked[i] = ranked[j]; ranked[j] = t; }
        int third = count / 3; if (third < 1) third = 1;
        int lo = 0, hi = third - 1;
        if (choice == "tier_medium") { lo = third; hi = third * 2 - 1; }
        else if (choice == "tier_long") { lo = third * 2; hi = count - 1; }
        if (lo >= count) lo = 0; if (hi >= count) hi = count - 1; if (hi < lo) hi = lo;
        Town* selectedTown=ranked[UtilityT::randomInt(lo, hi)].town;
        destinationTown=selectedTown->getGameData()->stringID.c_str();
        destinationNameStorage=frenchPlaceName(selectedTown->getName());
        destinationName=destinationNameStorage.c_str();
        selectedDistance = Ogre::Math::Sqrt(origin.squaredDistance(
            selectedTown->getPosition()));
        std::ostringstream log;log<<"Contract destination selected: type="<<(selectedTown->townType==TOWN_VILLAGE?"village":"city")<<" id="<<destinationTown<<" name="<<destinationNameStorage;DebugLog(log.str());
    }

    void chooseRuinDestination(const std::string& choice,const Ogre::Vector3& origin)
    {
        MercenariePerf::Phase perf("destinations");
        destinationTown="";destinationNameStorage.clear();destinationName=destinationNameStorage.c_str();selectedDistance=0;
        if(!shou||!shou->townList)return;
        static const DestinationOption ruins[]={
            {"51398-rebirth.mod",Loc::text("ui.ancient_labs")},{"1075-gamedata.base",Loc::text("ui.ancient_tech_lab")},{"49765-rebirth.mod",Loc::text("ui.ruined_armoury")},{"58703-rebirth.mod",Loc::text("ui.collapsed_labs")},{"62796-rebirth.mod",Loc::text("ui.abandoned_workshop")},{"55644-rebirth.mod",Loc::text("ui.deadlands_workshop")},{"49367-rebirth.mod",Loc::text("ui.sunken_ruins")},{"50993-rebirth.mod",Loc::text("ui.empty_lab")},{"51423-rebirth.mod",Loc::text("ui.ruined_library")},{"48796-rebirth.mod",Loc::text("ui.lost_library")},{"48446-rebirth.mod",Loc::text("ui.post_ancient_workshop")},{"51383-rebirth.mod",Loc::text("ui.swamp_laboratory")}
        };
        struct RankedRuin{const DestinationOption* option;float distance;Town* town;}ranked[12];int count=0;
        for(int i=0;i<12;++i){if(invalidContractDestination(ruins[i].id)||!ContractDestinationRules::scientific(ruins[i].id))continue;Town* town=shou->townList->getTownBySID(ruins[i].id);if(!town||destinationAtWar(town))continue;float d=origin.squaredDistance(town->getPosition());if(d<4000000.0f)continue;ranked[count].option=&ruins[i];ranked[count].distance=d;ranked[count].town=town;++count;}
        for(int i=0;i<count;++i)for(int j=i+1;j<count;++j)if(ranked[j].distance<ranked[i].distance){RankedRuin t=ranked[i];ranked[i]=ranked[j];ranked[j]=t;}
        if(count==0){ErrorLog("V9 science: no eligible scientific site; no city fallback");return;}int third=std::max(1,count/3),lo=0,hi=std::min(count-1,third-1);if(choice=="tier_medium"){lo=std::min(count-1,third);hi=std::min(count-1,third*2-1);}else if(choice=="tier_long"){lo=std::min(count-1,third*2);hi=count-1;}RankedRuin& selected=ranked[UtilityT::randomInt(lo,std::max(lo,hi))];destinationTown=selected.option->id;destinationNameStorage=frenchPlaceName(selected.town->getName());if(destinationNameStorage.empty()||destinationNameStorage==selected.town->getName())destinationNameStorage=selected.option->name;destinationName=destinationNameStorage.c_str();selectedDistance=Ogre::Math::Sqrt(selected.distance);
    }

    void startMission(Character* guildMaster, const std::string& choice, bool destinationAlreadyChosen);
    void openNegotiation();
    bool isGuildVisitor(Character* who);
    void removeGuildVisitor(Character* leader,bool reputationPenalty);

    const char* profileFor(int index)
    {
        const char* profiles[] = {Loc::text("ui.traveling_merchant"), "Pelerin", "Messager", Loc::text("ui.scientist"), "Artisan", Loc::text("ui.traveler_ff746db")};
        return profiles[index % 6];
    }

    std::string storyFor(int index, const std::string& town)
    {
        std::ostringstream key;key<<"mission.escort.story."<<(index%6);
        return Loc::named(key.str().c_str(),"destination",town);
    }

    #include "ContractBoardV6.h"
    #include "MissionBookContractsView.h"
    #include "ContractBoardDesign.h"
    #include "RewardDetailsPopup.h"
    #include "QuestTrackerView.h"
    #include "ContractRegionMapUI.h"

    void updateContractsBoard()
    {
        contractBarman=resolveContractBarman();
        if (!contractsMap || !contractsLegend || selectedOffer<0 || selectedOffer>=6) return;
        BoardOffer& offer = boardOffers[selectedOffer];
        const bool bountySearchView=missionBookDelegationContext&&missionBookTab==1&&BountyPresentationRules::usesSearchArea(offer.missionType)&&!missionBookSecurityOfferIds[selectedOffer].empty();
        refreshOfferRowsV6();
        const char* typeName=offer.missionType==MCT_MAIL?Loc::text("ui.message_delivery"):offer.missionType==3?Loc::text("ui.bounty_hunt"):offer.missionType==2?Loc::text("ui.scientific_expedition"):offer.missionType==1?Loc::text("ui.caravan_escort"):Loc::text("ui.escort_contract");
        std::stringstream details;
        details << typeName << "\n" << offer.tier << "  |  "<<offer.rarityName<<Loc::text("ui.customer") << offer.profile << "\n\n" << offer.story
                << Loc::text("ui.departure") << originCity << Loc::text("ui.arrival_dd572cb") << offer.townName
                << Loc::text("ui.distance") << static_cast<int>(offer.distance/1000.0f) << Loc::text("ui.km");
        if(offer.missionType!=0)details << Loc::text("ui.go") << static_cast<int>(offer.distance/1000.0f) << " km retour = " << static_cast<int>(offer.distance/500.0f) << Loc::text("ui.km");
        details
                << Loc::text("ui.hazard") << offer.danger << "/3"
                << Loc::text("ui.rate")<<(offer.missionType==MCT_MAIL?25:offer.missionType==2?60:50)<<Loc::text("ui.cats_km")
                << Loc::text("ui.estimate_6d44dc0") << displayedContractCats(offer.estimatedPay) << Loc::text("ui.cats");
        details<<Loc::text("ui.local_reputation_a8c82e9")<<localReputation()<<"  |  "<<reputationRank(localReputation());
        if (offer.prestigious) details << Loc::text("ui.prestigious_contract");
        if (!offer.available) details << (offer.story.find("CONTRAT ACCEPTE")==0?Loc::text("ui.unavailable_contract_already_accepted"):Loc::text("ui.unavailable_insufficient_local_level_or_reputation"));
        if(contractsDetails)MercenarieFonts::caption(contractsDetails,details.str());
        if(contractsInfo[0]){std::stringstream s;s<<mercenarieLocalize(Loc::text("common.mission"))<<"\n"<<mercenarieLocalize(typeName);MercenarieFonts::caption(contractsInfo[0],s.str());}
        if(contractsInfo[1]){std::stringstream s;s<<mercenarieLocalize(Loc::text("ui.route_125aac2"))<<"\n"<<originCity<<"  >  "<<offer.townName;MercenarieFonts::caption(contractsInfo[1],s.str());}
        if(contractsInfo[2]){std::stringstream s;s<<(Loc::text("ui.estimated_distance_risk"))<<"\n"<<static_cast<int>(offer.distance/1000.0f*(offer.missionType==0?1:2))<<(offer.missionType==0?Loc::text("ui.km_efeb0e5"):(Loc::text("ui.km_round_trip")))<<mercenarieLocalize("Danger")<<" "<<offer.dangerLevel<<"/5";if(offer.routeRegions.find("V6EST:")==0)s<<(offer.routeRegions.find(":DIRECT")!=std::string::npos?(Loc::text("ui.direct_fallback")):(Loc::text("ui.roads")));MercenarieFonts::caption(contractsInfo[2],s.str());}
        if(contractsInfo[3]){std::stringstream s;s<<mercenarieLocalize(Loc::text("ui.group"))<<"\n"<<offer.groupSize<<" "<<mercenarieLocalize(offer.groupSize>1?Loc::text("ui.people"):Loc::text("ui.person"));ContractGroupPlan::Plan p;if(ContractGroupPlan::decode(offer.routeRegions,p)&&p.animals())s<<" + "<<p.animals()<<(Loc::text("ui.animals"));MercenarieFonts::caption(contractsInfo[3],s.str());}
        if(contractsInfo[2])contractsInfo[2]->setVisible(clientOptions.detailedOffers);if(contractsInfo[3])contractsInfo[3]->setVisible(clientOptions.detailedOffers);for(int detailStar=0;detailStar<5;++detailStar)if(boardDangerV6[detailStar])boardDangerV6[detailStar]->setVisible(clientOptions.detailedOffers);
        if(contractsRewardAmount){std::stringstream s;s<<displayedContractCats(offer.estimatedPay)<<Loc::text("ui.cats_5e56227")<<(Loc::text("ui.estimate_a401125"));MercenarieFonts::caption(contractsRewardAmount,s.str());}
        if(contractsRewardReputation){std::stringstream s;s<<mercenarieLocalize(Loc::text("ui.reputation"))<<"     "<<(offer.available?"+5":"--");MercenarieFonts::caption(contractsRewardReputation,s.str());contractsRewardReputation->setTextColour(offer.available?MyGUI::Colour(0.18f,0.92f,0.76f):MyGUI::Colour(0.95f,0.34f,0.25f));}
        if(contractsRewardBonus)MercenarieFonts::caption(contractsRewardBonus,Loc::text("ui.bonuses_tips_conditional_not_taxed"));
        std::stringstream map;
        map << (bountySearchView?(Loc::text("v8.literal.045")):Loc::text("ui.wheel_zoom_drag_pan_green_departure_red_arrival"));
        if(!bountySearchView)map<<"\n"<<Loc::text("contracts.region.legend");
        MercenarieFonts::caption(contractsMap,map.str());
        std::stringstream dossier;dossier<<mercenarieLocalize(Loc::text("ui.selected_file"))<<" : "<<(offer.missionType==MCT_MAIL?"MAIL":offer.missionType==3?"BNT":offer.missionType==2?"EXP":offer.missionType==1?"CAR":"ESC")<<"-"<<static_cast<int>(offer.distance/1000.0f)<<"-"<<offer.rarityName;MercenarieFonts::caption(contractsLegend,dossier.str());
        if(contractsAccept){contractsAccept->setEnabled(missionBookDelegationContext?missionBookPersonalAvailable():offer.available);MercenarieFonts::caption(contractsAccept,missionBookDelegationContext?(Loc::text("v8.literal.027")):registerLanguage(Loc::text("ui.select_this_contract"),Loc::text("ui.select_this_contract")));}
        if(missionBookDelegationContext)refreshMissionBookDelegationPanel();
        if(missionBookDelegationContext)for(int i=0;i<4;++i)fitRegisterText(contractsInfo[i],14);
        fitRegisterText(contractsRewardAmount,24);fitRegisterText(contractsRewardBonus,16);
        fitRegisterText(contractsMap,17);
        if(boardMissionIconV6){
            setMissionIconV6(boardMissionIconV6,offer.missionType);
            MercenarieFonts::caption(contractsInfo[0],registerLanguage(Loc::text("ui.mission"),Loc::text("ui.mission"))+missionLabelV6(offer.missionType));contractsInfo[0]->setTextColour(missionColourV6(offer.missionType));
            std::ostringstream itinerary;if(bountySearchView)itinerary<<(Loc::text("v8.literal.046"))<<mercenarieLocalize(missionBookSecurityAreaNames[selectedOffer])<<"\n"<<(Loc::text("v8.literal.047"))<<(int)(offer.distance/1000)<<Loc::text("ui.km");else if(offer.missionType==MCT_MAIL){MailOfferPlan::Plan mail;itinerary<<(Loc::text("ui.route_d3b4476"))<<mercenarieLocalize(originCity);if(MailOfferPlan::decode(offer.routeRegions,mail)){for(size_t m=0;m<mail.destinations.size();++m)itinerary<<" > "<<mercenarieLocalize(mail.destinations[m].townName);itinerary<<"\n"<<(int)(offer.distance/1000)<<(std::string(Loc::text("ui.km"))+" (")<<(Loc::text("v8.literal.048"))<<")";itinerary<<"\n"<<Loc::text("contracts.ui.no_deadline");}}else itinerary<<registerLanguage(Loc::text("ui.route_d3b4476"),Loc::text("ui.route_d3b4476"))<<mercenarieLocalize(originCity)<<" > "<<mercenarieLocalize(offer.townName)<<"\n"<<(int)(offer.distance/1000*(offer.missionType==0?1:2))<<(offer.missionType==0?Loc::text("ui.km"):Loc::text("ui.km_round_trip_d654d18"))<<registerLanguage(Loc::text("ui.estimate"),Loc::text("ui.estimate"));MercenarieFonts::caption(contractsInfo[1],itinerary.str());
            bool mappedRegionDanger=bountySearchView||offer.routeRegions.find(";REGION3;")!=std::string::npos;std::ostringstream danger;danger<<Loc::text("ui.danger")<<(mappedRegionDanger?registerNumber(offer.dangerLevel):"?")<<"/5";MercenarieFonts::caption(contractsInfo[2],danger.str());
            for(int j=0;j<5;++j)boardDangerV6[j]->setColour(mappedRegionDanger&&j<offer.dangerLevel?MyGUI::Colour(.95f,.1f,.22f):MyGUI::Colour(.35f,.37f,.38f));
            ContractGroupPlan::Plan group;int people=offer.groupSize,animals=0;if(ContractGroupPlan::decode(offer.routeRegions,group)){people=group.people();animals=group.animals();}
            std::ostringstream roster;if(offer.missionType==MCT_MAIL)roster<<(Loc::text("v8.literal.050"))<<people<<" "<<(Loc::text("v8.literal.051"));else{roster<<registerLanguage(Loc::text("ui.group_f46c64b"),Loc::text("ui.group_f46c64b"))<<people<<registerLanguage(Loc::text("ui.people_39997fa"),Loc::text("ui.people_39997fa"));if(animals)roster<<" + "<<animals<<registerLanguage(Loc::text("ui.animals"),Loc::text("ui.animals"));roster<<"\n"<<registerLanguage(Loc::text("ui.1_leader"),Loc::text("ui.1_leader"))<<std::max(0,people-1)<<registerLanguage(Loc::text("ui.companion_s"),Loc::text("ui.companion_s"));}MercenarieFonts::caption(contractsInfo[3],roster.str());
            MercenarieFonts::caption(contractsRewardAmount,registerNumber(displayedContractCats(offer.estimatedPay))+Loc::text("ui.cats_5e56227")+registerLanguage(Loc::text("ui.estimate_9f8e888"),Loc::text("ui.estimate_9f8e888")));
            MercenarieFonts::caption(boardXpV6,registerLanguage(Loc::text("ui.guild_xp_depends_on_mission_report"),Loc::text("ui.guild_xp_depends_on_mission_report")));
            MercenarieFonts::caption(contractsRewardReputation,registerLanguage(Loc::text("ui.reputation_depends_on_mission_report"),Loc::text("ui.reputation_depends_on_mission_report")));contractsRewardReputation->setTextColour(registerIvory);
            for(int i=0;i<4;++i){if(missionBookDelegationContext)fitRegisterText(contractsInfo[i],21);contractsInfo[i]->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);}
            fitRegisterText(boardXpV6,21);fitRegisterText(contractsRewardReputation,21);fitRegisterText(contractsRewardAmount,32);fitRegisterText(contractsRewardBonus,20);
            if(boardDangerV6[0]){int x=contractsInfo[2]->getLeft()+52;int y=contractsInfo[2]->getTop()+(contractsInfo[2]->getHeight()-contractsInfo[2]->getTextSize().height)/2+contractsInfo[2]->getFontHeight();for(int j=0;j<5;++j)boardDangerV6[j]->setPosition(x+j*25,y);}
        }
        for (int i=0;i<6;++i) if(contractButtons[i]) contractButtons[i]->setStateSelected(i==selectedOffer);
        if(missionBookDelegationContext){if(contractsHeading){contractsHeading->setVisible(true);MercenarieFonts::caption(contractsHeading,Loc::text("missionbook.title"));}for(int i=0;i<6;++i)if(missionBookBoardTabs[i])missionBookBoardTabs[i]->setVisible(false);if(missionBookCategoryCombo)missionBookCategoryCombo->setVisible(true);}

        applyMissionBookPresentation();
        {MercenariePerf::Phase boardLayoutPhase("board-layout");applyBoard77();}
        if(contractsMapImage && contractsMapPanel)
        {
            MyGUI::IntSize panel=contractsMapImage->getSize();int mapLeft=contractsMapImage->getLeft(),mapTop=contractsMapImage->getTop();
            int cropHeight=missionBookDelegationContext?MissionBookGeometry::cropHeight(mapCropSize,panel.width,panel.height):mapCropSize;
            mapCropY=std::max(0,std::min(2048-cropHeight,mapCropY));
            const double mapScale=missionBookDelegationContext?MissionBookGeometry::scale(mapCropSize,panel.width):0;
            contractsMapImage->setImageInfo("GuildEscortMap.png",MyGUI::IntCoord(mapCropX,mapCropY,mapCropSize,cropHeight),MyGUI::IntSize(mapCropSize,cropHeight));contractsMapImage->setImageIndex(0);
            if(missionBookDelegationContext&&contractsMapImage->getSubWidgetMain())contractsMapImage->getSubWidgetMain()->_setUVSet(MyGUI::FloatRect(mapCropX/2048.0f,mapCropY/2048.0f,(mapCropX+mapCropSize)/2048.0f,(float)((mapCropY+panel.height/mapScale)/2048.0)));
            for(int i=2;i<14;++i)if(cityMarkers[i])cityMarkers[i]->setVisible(false);
            updateContractRegionMap(offer,bountySearchView,cropHeight,missionBookDelegationContext?mapScale:(double)panel.height/mapCropSize);
            Town* arrivalTown=shou->townList->getTownBySID(offer.townId);
            std::vector<TownBase*> routePlaces;std::vector<Ogre::Vector3> routePositions;std::vector<std::string> routeNames;
            routePlaces.push_back(contractOriginTown);routePositions.push_back(contractOriginTown?contractOriginTown->getPosition():(contractBarman?contractBarman->getPosition():Ogre::Vector3::ZERO));routeNames.push_back(originCity);
            MailOfferPlan::Plan mapMail;bool multiMail=offer.missionType==MCT_MAIL&&MailOfferPlan::decode(offer.routeRegions,mapMail);
            if(multiMail){for(size_t m=0;m<mapMail.destinations.size()&&routePositions.size()<14;++m){Town* stop=shou->townList->getTownBySID(mapMail.destinations[m].townId);if(!stop)continue;routePlaces.push_back(stop);routePositions.push_back(stop->getPosition());routeNames.push_back(mapMail.destinations[m].townName);}}
            else{routePlaces.push_back(arrivalTown);routePositions.push_back(arrivalTown?arrivalTown->getPosition():Ogre::Vector3::ZERO);routeNames.push_back(offer.townName);}
            // Saved price-estimation geometry only. Never connected to movement.
            std::vector<RoutePrototype::Point> preview=ContractRouteVisual::decode(offer.routeRegions);
            bool directMailLeg=false;
            if(multiMail&&offer.routeRegions.find(";REGION3;")==std::string::npos){preview.clear();for(size_t m=1;m<routePositions.size();++m){BoardOffer leg;analyseContractRoute(routePositions[m-1],routePositions[m],leg);if(leg.routeRegions.find(":DIRECT")!=std::string::npos)directMailLeg=true;std::vector<RoutePrototype::Point> detailed=ContractRouteVisual::decode(leg.routeRegions);if(preview.empty()&&!detailed.empty())preview.push_back(detailed.front());for(size_t p=preview.empty()?0:1;p<detailed.size();++p)preview.push_back(detailed[p]);}}
            size_t outwardPointCount=preview.size();
            std::vector<RoutePrototype::Point> backPreview=ContractRegionRoute::returnPath(offer.routeRegions);
            if(!backPreview.empty())preview.insert(preview.end(),backPreview.begin(),backPreview.end());
            bool legacyPreview=preview.size()<2||directMailLeg||offer.routeRegions.find(":DIRECT")!=std::string::npos;
            if(preview.size()<2){preview.clear();for(size_t j=0;j<routePositions.size();++j)preview.push_back(RoutePrototype::Point(routePositions[j].x,routePositions[j].y,routePositions[j].z));}
            for(size_t j=0;j<contractRouteDots.size();++j)contractRouteDots[j]->setVisible(false);
            if(bountySearchView&&ou&&ou->zoneMgr){
                for(int i=0;i<2;++i){if(routeCityIcons[i])routeCityIcons[i]->setVisible(false);if(cityMarkers[i])cityMarkers[i]->setVisible(false);}
                const Ogre::Vector3 center=missionBookSecurityAreaCenters[selectedOffer];iVector2 sector=ou->zoneMgr->getMapSector(center);int tx=std::max(0,std::min(2047,(int)((sector.x+.5f)*32))),ty=std::max(0,std::min(2047,(int)((sector.y+.5f)*32)));
                iVector2 west=ou->zoneMgr->getMapSector(center+Ogre::Vector3(-32000,0,0)),east=ou->zoneMgr->getMapSector(center+Ogre::Vector3(32000,0,0));float texelsPerUnit=abs(east.x-west.x)*32.0f/64000.0f;
                int px=(tx-mapCropX)*panel.width/mapCropSize,py=(ty-mapCropY)*(missionBookDelegationContext?mapScale:(double)panel.height/mapCropSize),radius=std::max(2,(int)(missionBookSecurityAreaRadii[selectedOffer]*texelsPerUnit*panel.width/mapCropSize));size_t dash=0;for(int i=0;i<180&&dash<contractRouteDots.size();++i)if(i%9<6){double angle=i*6.283185307179586/180;MyGUI::Widget* mark=contractRouteDots[dash++];mark->setPosition(mapLeft+px+(int)(cos(angle)*radius)-2,mapTop+py+(int)(sin(angle)*radius)-2);mark->setVisible(clientOptions.showRoutes&&mark->getLeft()>=mapLeft&&mark->getTop()>=mapTop&&mark->getLeft()+4<=mapLeft+panel.width&&mark->getTop()+4<=mapTop+panel.height);}
                Town* area=shou&&shou->townList?shou->townList->getTownBySID(offer.townId):0;if(routeCityIcons[1]&&area){MyGUI::IntCoord sprite=mercenariePlaceSprite(area);int iw=64*sprite.width/std::max(sprite.width,sprite.height),ih=64*sprite.height/std::max(sprite.width,sprite.height);mercenariePlaceImage(routeCityIcons[1],area);routeCityIcons[1]->setCoord(mapLeft+px-iw/2,mapTop+py-ih/2,iw,ih);routeCityIcons[1]->setVisible(px>=iw/2&&py>=ih/2&&px+iw/2<=panel.width&&py+ih/2<=panel.height);}if(cityMarkers[1]){const std::string& areaName=missionBookSecurityAreaNames[selectedOffer];int labelWidth=std::max(150,std::min(310,(int)areaName.size()*10+28));cityMarkers[1]->setCoord(mapLeft+px-labelWidth/2,mapTop+py+38,labelWidth,30);MercenarieFonts::caption(cityMarkers[1],areaName);cityMarkers[1]->setTextColour(registerAmber);cityMarkers[1]->setVisible(px>=0&&py>=0&&px<panel.width&&py<panel.height);if(missionBookDelegationContext){MissionBookGeometry::Rect r=MissionBookGeometry::bounded(MissionBookGeometry::Rect(px-labelWidth/2,py+38,labelWidth,30),panel.width,panel.height);cityMarkers[1]->setCoord(mapLeft+r.x,mapTop+r.y,r.w,r.h);}}if(contractsLegend)MercenarieFonts::caption(contractsLegend,Loc::text("ui.approximate_search_area"));return;
            }
            if(outwardPointCount<2)outwardPointCount=preview.size();
            std::vector<std::pair<float,float> > pixels;double screenLength=0;
            ContinuousMapAxis axisX,axisZ;bool smooth=ou&&ou->zoneMgr&&axisX.calibrate(routePositions.front().x,MapAxisQuery(true,routePositions.front()))&&axisZ.calibrate(routePositions.front().z,MapAxisQuery(false,routePositions.front()));
            if(smooth)for(size_t j=0;j<preview.size();++j){iVector2 cell=ou->zoneMgr->getMapSector(Ogre::Vector3((float)preview[j].x,(float)preview[j].y,(float)preview[j].z));if(fabs(axisX.project(preview[j].x)-(cell.x+.5))>.51||fabs(axisZ.project(preview[j].z)-(cell.y+.5))>.51){smooth=false;break;}}
            if(ou&&ou->zoneMgr)for(size_t j=0;j<preview.size();++j){iVector2 sector=ou->zoneMgr->getMapSector(Ogre::Vector3((float)preview[j].x,(float)preview[j].y,(float)preview[j].z));double sx=sector.x+.5,sz=sector.y+.5;
                if(smooth){sx=axisX.project(preview[j].x);sz=axisZ.project(preview[j].z);}
                float x=(float)((sx*32-mapCropX)*panel.width/mapCropSize),y=(float)((sz*32-mapCropY)*(missionBookDelegationContext?mapScale:(double)panel.height/mapCropSize));
                if(!pixels.empty()){float dx=x-pixels.back().first,dy=y-pixels.back().second;screenLength+=sqrt(dx*dx+dy*dy);}pixels.push_back(std::make_pair(x,y));}
            double spacing=std::max(10.0,screenLength/450.0),nextDot=0,travelled=0;size_t dot=0;
            for(size_t j=1;j<pixels.size()&&dot<contractRouteDots.size();++j){double dx=pixels[j].first-pixels[j-1].first,dy=pixels[j].second-pixels[j-1].second,len=sqrt(dx*dx+dy*dy);if(len<=0)continue;
                while(nextDot<=travelled+len&&dot<contractRouteDots.size()){double t=(nextDot-travelled)/len;int x=(int)(pixels[j-1].first+t*dx),y=(int)(pixels[j-1].second+t*dy);nextDot+=spacing;
                    if(x<4||y<4||x>panel.width-8||y>panel.height-10)continue;
                    MyGUI::Widget* mark=contractRouteDots[dot++];mark->setPosition(mapLeft+x-2,mapTop+y-2);mark->setVisible(clientOptions.showRoutes);
                }travelled+=len;
            }
            // Draw one chevron along the outward route, using the existing marker pool.
            // Its direction follows route order and its points stay inside the map.
            if(!missionBookDelegationContext&&clientOptions.showRoutes&&pixels.size()>1){
                size_t end=std::min(outwardPointCount,pixels.size());double length=0;
                for(size_t j=1;j<end;++j){double dx=pixels[j].first-pixels[j-1].first,dy=pixels[j].second-pixels[j-1].second;length+=sqrt(dx*dx+dy*dy);}
                double target=length*.5,passed=0;
                for(size_t j=1;length>60&&j<end;++j){
                    double dx=pixels[j].first-pixels[j-1].first,dy=pixels[j].second-pixels[j-1].second,len=sqrt(dx*dx+dy*dy);
                    if(len<=0)continue;
                    if(passed+len>=target){
                        double t=(target-passed)/len,ux=dx/len,uy=dy/len;
                        double cx=pixels[j-1].first+t*dx,cy=pixels[j-1].second+t*dy;
                        double arm=std::max(12,board77Px(18));
                        if(cx>arm+8&&cy>arm+8&&cx<panel.width-arm-8&&cy<panel.height-arm-8){
                            for(int side=-1;side<=1;side+=2)for(int k=0;k<=6&&dot<contractRouteDots.size();++k){
                                double step=arm*k/6.0;
                                int x=(int)(cx-ux*step-uy*step*.65*side),y=(int)(cy-uy*step+ux*step*.65*side);
                                MyGUI::Widget* mark=contractRouteDots[dot++];mark->setPosition(mapLeft+x-3,mapTop+y-3);mark->setVisible(true);
                            }
                        }
                        break;
                    }
                    passed+=len;
                }
            }
            if(contractsLegend)MercenarieFonts::caption(contractsLegend,gMercenarieEnglish?(legacyPreview?Loc::text("ui.illustrative_direct_route_npc_path_may_differ"):Loc::text("ui.estimated_route_npc_path_may_differ")):(legacyPreview?Loc::text("ui.illustrative_direct_route_npc_path_may_differ"):Loc::text("ui.estimated_route_npc_path_may_differ")));
            for(size_t i=0;i<14;++i){if(routeCityIcons[i])routeCityIcons[i]->setVisible(false);if(cityMarkers[i])cityMarkers[i]->setVisible(false);}
            std::vector<MissionBookGeometry::Rect> labelRects;
            for(size_t i=0;i<routePositions.size()&&i<14;++i){
                bool valid=ou&&ou->zoneMgr&&(i==0?(contractOriginTown!=0||contractBarman!=0):routePlaces[i]!=0);
                iVector2 sector=valid?ou->zoneMgr->getMapSector(routePositions[i]):iVector2(0,0);
                float tx=(static_cast<float>(sector.x)+0.5f)*32.0f;
                float ty=(static_cast<float>(sector.y)+0.5f)*32.0f;
                int x=mapLeft+static_cast<int>((tx-mapCropX)*panel.width/mapCropSize),y=mapTop+static_cast<int>((ty-mapCropY)*(missionBookDelegationContext?mapScale:(double)panel.height/mapCropSize));
                bool visible=valid&&x>=mapLeft&&y>=mapTop&&x<=panel.width+mapLeft&&y<=panel.height+mapTop;
if(routeCityIcons[i]){TownBase* place=routePlaces[i];MyGUI::IntCoord sprite=mercenariePlaceSprite(place);int iw=64*sprite.width/std::max(sprite.width,sprite.height),ih=64*sprite.height/std::max(sprite.width,sprite.height);mercenariePlaceImage(routeCityIcons[i],place);routeCityIcons[i]->setCoord(x-iw/2,y-ih/2,iw,ih);routeCityIcons[i]->setVisible(visible);}
                if(cityMarkers[i]){int labelWidth=std::max(130,std::min(310,static_cast<int>(routeNames[i].size())*10+28));cityMarkers[i]->setCoord(x-labelWidth/2,y+38,labelWidth,30);cityMarkers[i]->setVisible(visible);MercenarieFonts::caption(cityMarkers[i],routeNames[i]);cityMarkers[i]->setFontHeight(18);cityMarkers[i]->setTextColour(i==0?MyGUI::Colour(0.25f,1.0f,0.32f):MyGUI::Colour(1.0f,0.22f,0.16f));}
                if(!missionBookDelegationContext&&cityMarkers[i]){board77Skin(cityMarkers[i],"Board77Card");int labelWidth=board77Px(std::max(130,std::min(310,static_cast<int>(routeNames[i].size())*10+28)));cityMarkers[i]->setCoord(x-labelWidth/2,y+board77Px(38),labelWidth,board77Px(32));board77Font(cityMarkers[i],16,true);cityMarkers[i]->setTextColour(i==0?MyGUI::Colour(.25f,1.0f,.32f):MyGUI::Colour(1.0f,.46f,.40f));cityMarkers[i]->setTextAlign(MyGUI::Align::Center);board77Fit(cityMarkers[i]);}
                if(missionBookDelegationContext&&visible&&cityMarkers[i]){MissionBookGeometry::Rect r=MissionBookGeometry::label(MissionBookGeometry::Rect(cityMarkers[i]->getLeft()-mapLeft,cityMarkers[i]->getTop()-mapTop,cityMarkers[i]->getWidth(),cityMarkers[i]->getHeight()),panel.width,panel.height,labelRects);cityMarkers[i]->setCoord(mapLeft+r.x,mapTop+r.y,r.w,r.h);cityMarkers[i]->setVisible(r.w>0);if(r.w>0)labelRects.push_back(r);}

            }
        }
    }

    void mapMouseWheel(MyGUI::WidgetPtr,int rel)
    {
        if(missionBookDelegationContext&&contractsMapImage){int oldH=MissionBookGeometry::cropHeight(mapCropSize,contractsMapImage->getWidth(),contractsMapImage->getHeight()),oldW=mapCropSize;int step=std::max(64,mapCropSize/8);mapCropSize=std::max(512,std::min(2048,mapCropSize+(rel>0?-step:step)));int newH=MissionBookGeometry::cropHeight(mapCropSize,contractsMapImage->getWidth(),contractsMapImage->getHeight());mapCropX=std::max(0,std::min(2048-mapCropSize,mapCropX+(oldW-mapCropSize)/2));mapCropY=std::max(0,std::min(2048-newH,mapCropY+(oldH-newH)/2));updateContractsBoard();return;}
        int old=mapCropSize;int step=std::max(64,mapCropSize/8);mapCropSize+=rel>0?-step:step;mapCropSize=std::max(512,std::min(2048,mapCropSize));mapCropX+=(old-mapCropSize)/2;mapCropY+=(old-mapCropSize)/2;mapCropX=std::max(0,std::min(2048-mapCropSize,mapCropX));mapCropY=std::max(0,std::min(2048-mapCropSize,mapCropY));updateContractsBoard();
    }

    void mapMousePressed(MyGUI::WidgetPtr,int left,int top,MyGUI::MouseButton id){if(id==MyGUI::MouseButton::Left){mapDragX=left;mapDragY=top;}}
    void mapMouseDragged(MyGUI::WidgetPtr,int left,int top,MyGUI::MouseButton id)
    {
        if(id!=MyGUI::MouseButton::Left||!contractsMapPanel)return;
        if(missionBookDelegationContext&&contractsMapImage){double scale=MissionBookGeometry::scale(mapCropSize,contractsMapImage->getWidth());int cropH=MissionBookGeometry::cropHeight(mapCropSize,contractsMapImage->getWidth(),contractsMapImage->getHeight());mapCropX=std::max(0,std::min(2048-mapCropSize,mapCropX-(int)((left-mapDragX)/scale)));mapCropY=std::max(0,std::min(2048-cropH,mapCropY-(int)((top-mapDragY)/scale)));mapDragX=left;mapDragY=top;updateContractsBoard();return;}
MyGUI::IntSize s=contractsMapImage->getSize();mapCropX-=(left-mapDragX)*mapCropSize/std::max(1,s.width);mapCropY-=(top-mapDragY)*mapCropSize/std::max(1,s.height);mapDragX=left;mapDragY=top;mapCropX=std::max(0,std::min(2048-mapCropSize,mapCropX));mapCropY=std::max(0,std::min(2048-mapCropSize,mapCropY));updateContractsBoard();
    }

    void cityMarkerHovered(MyGUI::WidgetPtr sender,MyGUI::WidgetPtr)
    {
        for(int i=0;i<14;++i)if(sender==cityMarkers[i]||sender==routeCityIcons[i]){
            BoardOffer& offer=boardOffers[selectedOffer];MailOfferPlan::Plan mail;bool multi=offer.missionType==MCT_MAIL&&MailOfferPlan::decode(offer.routeRegions,mail);std::string name=i==0?originCity:(multi&&i-1<(int)mail.destinations.size()?mail.destinations[i-1].townName:offer.townName);float km=i==0?0.0f:offer.distance/1000.0f;char tip[4096];sprintf_s(tip,mercenarieLocalize(Loc::text("ui.s_s_distance_from_s_1f_km_s")).c_str(),name.c_str(),mercenarieLocalize(i==0?Loc::text("ui.departure_526b83a"):Loc::text("ui.arrival")).c_str(),originCity.c_str(),km,i==0?"":mercenarieLocalize(Loc::text("ui.danger_estimated_according_to_the_contract")).c_str());MercenarieFonts::caption(contractsLegend,tip);break;
        }
    }

    void contractOfferClicked(MyGUI::WidgetPtr sender)
    {
        contractAcceptArmed=false;
        for(int i=0;i<6;++i) if(sender==contractButtons[i]) { selectedOffer=i; break; }
        if(selectedOffer<0||selectedOffer>=6||!boardOffers[selectedOffer].available)return;
        Town* arrival=shou&&shou->townList?shou->townList->getTownBySID(boardOffers[selectedOffer].townId):0;
        if(arrival&&contractOriginTown&&ou&&ou->zoneMgr){
            iVector2 origin=ou->zoneMgr->getMapSector(contractOriginTown->getPosition());int minX=origin.x*32+16,maxX=minX,minY=origin.y*32+16,maxY=minY;MailOfferPlan::Plan mail;bool multi=boardOffers[selectedOffer].missionType==MCT_MAIL&&MailOfferPlan::decode(boardOffers[selectedOffer].routeRegions,mail);if(multi){for(size_t i=0;i<mail.destinations.size();++i){Town* stop=shou->townList->getTownBySID(mail.destinations[i].townId);if(!stop)continue;iVector2 s=ou->zoneMgr->getMapSector(stop->getPosition());int x=s.x*32+16,y=s.y*32+16;minX=std::min(minX,x);maxX=std::max(maxX,x);minY=std::min(minY,y);maxY=std::max(maxY,y);}}else{iVector2 s=ou->zoneMgr->getMapSector(arrival->getPosition());minX=std::min(minX,s.x*32+16);maxX=std::max(maxX,s.x*32+16);minY=std::min(minY,s.y*32+16);maxY=std::max(maxY,s.y*32+16);}
            mapCropSize=std::min(2048,std::max(512,std::max(maxX-minX,maxY-minY)+384));
            mapCropX=std::max(0,std::min(2048-mapCropSize,(minX+maxX-mapCropSize)/2));
            mapCropY=std::max(0,std::min(2048-mapCropSize,(minY+maxY-mapCropSize)/2));
        }
        if(missionBookDelegationContext&&contractsMapImage){
            int vw=contractsMapImage->getWidth(),vh=contractsMapImage->getHeight();
            int cx=mapCropX+mapCropSize/2,cy=mapCropY+mapCropSize/2;
            if(missionBookTab==1&&ou&&ou->zoneMgr){iVector2 q=ou->zoneMgr->getMapSector(missionBookSecurityAreaCenters[selectedOffer]);cx=q.x*32+16;cy=q.y*32+16;const Ogre::Vector3 center=missionBookSecurityAreaCenters[selectedOffer];iVector2 west=ou->zoneMgr->getMapSector(center+Ogre::Vector3(-32000,0,0)),east=ou->zoneMgr->getMapSector(center+Ogre::Vector3(32000,0,0));double texels=abs(east.x-west.x)*32.0/64000.0;mapCropSize=std::max(512,std::min(2048,(int)ceil(missionBookSecurityAreaRadii[selectedOffer]*texels*2.5)));}
            mapCropSize=std::min(2048,std::max(mapCropSize,(int)((double)mapCropSize*vw/std::max(1,vh))));
            int h=MissionBookGeometry::cropHeight(mapCropSize,vw,vh);mapCropX=std::max(0,std::min(2048-mapCropSize,cx-mapCropSize/2));mapCropY=std::max(0,std::min(2048-h,cy-h/2));
        }
        updateContractsBoard();
    }

    void contractsWindowButtonPressed(MyGUI::Window* window,const std::string&)
    {
        missionBookDelegationContext=false;
        if(window) window->setVisible(false);
        contractDialogueGiver.setNull();contractBoardOpenedFromDialogue=false;
        if(ou && !negotiationWasPaused) ou->userPause(false);
    }

#include "MailRouteRuntime.h"
#include "MailItemIdentity.h"
    bool restoreMailLetterIdentities();
#include "MailIdentityRules.h"
    bool mailFindItemOn(Character* carrier,const std::string& handle,Item*& found,InventorySection*& section)
    {
        found=0;section=0;if(!carrier||handle.empty())return false;Inventory* inventory=carrier->getInventory();if(!inventory)return false;lektor<InventorySection*>& sections=inventory->getAllSections();for(unsigned int s=0;s<sections.size();++s){InventorySection* current=sections[s];if(!current)continue;const Ogre::vector<InventorySection::SectionItem>::type& items=current->getItems();for(size_t i=0;i<items.size();++i){Item* item=items[i].item;if(item&&item->getHandle().toString()==handle){found=item;section=current;return true;}}}return false;
    }
    bool mailFindCarrier(const std::string& handle,Character*& carrier,Item*& item,InventorySection*& section)
    {carrier=0;item=0;section=0;if(!ou||!ou->player)return false;for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* candidate=ou->player->playerCharacters[i];if(mailFindItemOn(candidate,handle,item,section)){carrier=candidate;return true;}}return false;}
    bool mailResolveCarrier(MailContracts::Step& step,Character*& carrier,Item*& item,InventorySection*& section,bool* recoveryAttempted=0)
    {MercenariePerf::Phase perf("references-mail");if(!mailFindCarrier(step.letterHandle,carrier,item,section)||mailItemStep(item)!=step.stepId){
        // A reply list is one synchronous interaction. Recover once, retaining
        // immediate recovery on the next interaction or explicit delivery.
        if(recoveryAttempted&&*recoveryAttempted)return false;
        if(recoveryAttempted)*recoveryAttempted=true;
        restoreMailLetterIdentities();if(!mailFindCarrier(step.letterHandle,carrier,item,section)||mailItemStep(item)!=step.stepId)return false;}step.carrierId=carrier->getHandle().toString();step.carrierName=carrier->getName();mailRestorationPending.erase(step.stepId);return true;}
    std::string mailLetterName(const MailContracts::Step& step);
    bool restoreMailLetterIdentities()
    {
        MercenariePerf::Phase perf("mail-recovery");
        mailRestorationPending.clear();
        struct RuntimeCandidate{Character* carrier;Item* item;InventorySection* section;RuntimeCandidate(Character* c,Item* i,InventorySection* s):carrier(c),item(i),section(s){}};
        std::vector<MailIdentity::Step> steps;std::vector<MailContracts::Step*> runtimeSteps;
        for(size_t c=0;c<mailContracts.size();++c)if(mailContracts[c].status==MailContracts::MailActive&&!mailContracts[c].delegated)for(size_t s=0;s<mailContracts[c].steps.size();++s){MailContracts::Step& step=mailContracts[c].steps[s];if(!step.delivered){steps.push_back(MailIdentity::Step(step.stepId,step.letterHandle,step.carrierId));runtimeSteps.push_back(&step);mailRestorationPending.insert(step.stepId);}}
        if(steps.empty())return true;
        if(!ou||!ou->player)return false;
        std::vector<MailIdentity::Item> candidates;std::vector<RuntimeCandidate> runtimeCandidates;size_t order=0;
        for(size_t p=0;p<ou->player->playerCharacters.size();++p){Character* carrier=ou->player->playerCharacters[p];Inventory* inventory=carrier?carrier->getInventory():0;if(!inventory)continue;lektor<InventorySection*>& sections=inventory->getAllSections();for(unsigned int s=0;s<sections.size();++s){InventorySection* section=sections[s];if(!section)continue;const Ogre::vector<InventorySection::SectionItem>::type& items=section->getItems();for(size_t i=0;i<items.size();++i){Item* item=items[i].item;if(!item)continue;std::string token=mailItemStep(item);if(token.empty()&&(!item->getGameData()||item->getGameData()->stringID!="49380-rebirth.mod"))continue;candidates.push_back(MailIdentity::Item(token,item->getHandle().toString(),carrier->getHandle().toString()));runtimeCandidates.push_back(RuntimeCandidate(carrier,item,section));}}}
        bool complete=true;std::vector<int> binding=MailIdentity::bind(steps,candidates);for(size_t i=0;i<binding.size();++i){if(binding[i]<0){complete=false;continue;}MailContracts::Step& step=*runtimeSteps[i];RuntimeCandidate& found=runtimeCandidates[binding[i]];step.letterHandle=found.item->getHandle().toString();step.carrierId=found.carrier->getHandle().toString();step.carrierName=found.carrier->getName();bindMailItem(found.item,step.stepId);mailRestorationPending.erase(step.stepId);found.item->setName(mailLetterName(step));found.item->setPersistant(found.carrier->getHandle());}
        return complete;
    }
    bool mailPoliceIssuer(Character* c){if(!c||c->isDead()||!c->getGameData()||!c->getFaction()||!c->getFaction()->data)return false;const std::string actor=c->getGameData()->stringID,fac=c->getFaction()->data->stringID;if(actor=="1833-gamedata.base"&&fac=="defaultEmpireFactionSID")return true;if(!c->platoon||!c->platoon->me)return false;Platoon* p=c->platoon->me;if(!p->squadTemplate||c->platoon->getSquadLeader_theRealOne()!=c)return false;const std::string squad=p->squadTemplate->stringID;return (actor=="18882-rebirth.mod"&&fac=="1083-gamedata.base"&&squad=="18886-rebirth.mod")||(actor=="11623-Dialogue (10).mod"&&fac=="11624-Dialogue (10).mod"&&squad=="59292-rebirth.mod");}
    MailContracts::SenderRole mailRole(Character* actor){if(isBarman(actor))return MailContracts::RoleBarman;if(mailPoliceIssuer(actor))return MailContracts::RolePolice;return MailContracts::RoleUndefined;}
    std::string mailRoleName(MailContracts::SenderRole role){if(role==MailContracts::RolePolice)return Loc::text("contracts.ui.police");if(role==MailContracts::RoleMerchant)return Loc::text("ui.merchant");if(role==MailContracts::RoleShinobi)return Loc::text("contracts.ui.shinobi");return Loc::text("contracts.ui.barman");}
    std::string mailLetterName(const MailContracts::Step& step){return std::string(Loc::text("v8.literal.052"))+step.townName+" "+mailRoleName(step.recipientRole);}
    void removeMailLetter(MailContracts::Step& step){Character* carrier=0;Item* item=0;InventorySection* section=0;if(mailFindCarrier(step.letterHandle,carrier,item,section)&&carrier&&carrier->getInventory())carrier->getInventory()->removeItemAutoDestroy(item,1);step.letterHandle.clear();step.carrierId.clear();step.carrierName.clear();}
    bool startPersonalMail(const BoardOffer& offer)
    {
        MailOfferPlan::Plan plan;if(!MailOfferPlan::decode(offer.routeRegions,plan)||!contractBarman||!ou||!ou->player||!ou->theFactory)return false;Character* receiver=contractBarman->dialogue&&!contractBarman->dialogue->conversationTarget.isNull()?contractBarman->dialogue->conversationTarget.getCharacter():0;if(!receiver&&!ou->player->selectedCharacter.isNull())receiver=ou->player->selectedCharacter.getCharacter();if(!receiver)return false;GameData* templateData=ou->gamedata.getData("49380-rebirth.mod",ITEM);if(!templateData)return false;
        TownBase* mailOrigin=contractBarman->getCurrentTownLocation();if(!mailOrigin||!mailOrigin->getGameData())return false;
        MailContracts::Contract mission;mission.contractId=allocateQuestFiscalId();mission.offerId=currentBoardKey+"#"+registerNumber(selectedOffer)+"#"+registerNumber((int)currentGameHours);mission.senderId=contractBarman->getHandle().toString();mission.senderName=contractBarman->getName();mission.originTownId=mailOrigin->getGameData()->stringID;mission.originTownName=frenchPlaceName(mailOrigin->getName());mission.senderRole=MailContracts::RoleBarman;mission.status=MailContracts::MailActive;mission.urgent=false;mission.acceptedAtWorldHour=currentGameHours;mission.reward=displayedContractCats(offer.estimatedPay);mission.guildXp=GuildProgression::success(MCT_MAIL,offer.distance/1000.0f,offer.dangerLevel,0,true,false).xp;mission.reputation=4;double estimatedHours=DelegatedMissionTiming::travelHours(std::max(0.0f,offer.distance/1000.0f));mission.deadlineWorldHour=mission.urgent?currentGameHours+MailContracts::urgentDeadlineHours(estimatedHours):0;
        for(size_t i=0;i<plan.destinations.size();++i){MailContracts::Step step;step.stepId=mission.contractId+"#"+registerNumber((int)i);step.recipientId="UNBOUND:"+plan.destinations[i].townId+":"+registerNumber((int)plan.destinations[i].role);step.recipientName=mailRoleName(plan.destinations[i].role);step.townId=plan.destinations[i].townId;step.townName=plan.destinations[i].townName;step.recipientRole=plan.destinations[i].role;step.distanceKm=plan.destinations[i].distanceKm;Item* letter=ou->theFactory->createItem(templateData,hand(),0,0,0,0);if(!letter){for(size_t j=0;j<mission.steps.size();++j)removeMailLetter(mission.steps[j]);return false;}letter->setName(mailLetterName(step));letter->isUnique=true;letter->setPersistant(receiver->getHandle());if(!receiver->giveItem(letter,false,true)){for(size_t j=0;j<mission.steps.size();++j)removeMailLetter(mission.steps[j]);return false;}bindMailItem(letter,step.stepId);step.letterHandle=letter->getHandle().toString();step.carrierId=receiver->getHandle().toString();step.carrierName=receiver->getName();mission.steps.push_back(step);mission.suggestedRoute.push_back((int)i);}
        if(!MailContracts::validDefinition(mission)||!captureMailRoute(mission,mailOrigin)){for(size_t j=0;j<mission.steps.size();++j)removeMailLetter(mission.steps[j]);return false;}mailContracts.push_back(mission);ou->showPlayerAMessage(Loc::text("v8.literal.053"),true);return true;
    }
    void appendMailTrackerItems()
    {
        for(size_t c=0;c<mailContracts.size();++c){MailContracts::Contract& mission=mailContracts[c];if(mission.status!=MailContracts::MailActive)continue;QuestTrackerItem item;item.type=MCT_MAIL;std::ostringstream objective,state;objective<<mission.originTownName<<QuestTrackerText::arrow();double remainingKm=0;for(size_t s=0;s<mission.steps.size();++s){MailContracts::Step& step=mission.steps[s];if(s) objective<<", ";objective<<step.townName;if(step.delivered)continue;if(item.location.empty())item.location=mercenarieLocalize(step.townName);remainingKm+=step.distanceKm;Character* carrier=0;Item* letter=0;InventorySection* section=0;if(mailFindCarrier(step.letterHandle,carrier,letter,section)&&carrier){step.carrierId=carrier->getHandle().toString();step.carrierName=carrier->getName();}}
            state<<(Loc::text("v8.literal.054"))<<MailContracts::deliveredCount(mission)<<"/"<<mission.steps.size();for(size_t s=0;s<mission.steps.size();++s)if(!mission.steps[s].delivered&&!mission.steps[s].carrierName.empty()){state<<" | "<<(Loc::text("v8.literal.055"))<<mission.steps[s].carrierName;break;}if(MailContracts::hasDeadline(mission))state<<" | "<<(Loc::text("v8.literal.019"))<<std::max(0,(int)ceil(mission.deadlineWorldHour-currentGameHours))<<Loc::text("ui.h");item.objective=objective.str();item.state=state.str();item.distance=QuestTrackerText::distance((float)(remainingKm*1000.0),gMercenarieEnglish);item.slot=(int)c;item.clickable=true;item.click=mailTrackerClicked;questTrackerItems.push_back(item);}
    }
    void mailCancelBack(MyGUI::WidgetPtr){mailCancelIndex=-1;if(mailCancelWindow)mailCancelWindow->setVisible(false);}
    void mailCancelWindowPressed(MyGUI::Window*,const std::string&){mailCancelBack(0);}
    void mailCancelConfirm(MyGUI::WidgetPtr)
    {if(mailCancelIndex>=0&&mailCancelIndex<(int)mailContracts.size()){MailContracts::Contract& mission=mailContracts[mailCancelIndex];if(mission.status==MailContracts::MailActive){for(size_t s=0;s<mission.steps.size();++s)if(!mission.steps[s].delivered)removeMailLetter(mission.steps[s]);MailContracts::cancel(mission);++failedContracts;escortReputation=GuildProgression::clampRep(escortReputation+MercenarieConfig::Reputation::CancellationGlobal);float& local=localReputations[ReputationIdentity::key(mission.originTownName)];local=GuildProgression::clampRep(local+MercenarieConfig::Reputation::CancellationLocal);archiveMail(mission,4);if(contractHistory.size()>50)contractHistory.resize(50);saveReputations();if(ou)ou->showPlayerAMessage(Loc::text("mail.cancelled"),true);}}mailCancelBack(0);updateTrackerUI();}
    void requestMailCancel(int index)
    {if(index<0||index>=(int)mailContracts.size()||mailContracts[index].status!=MailContracts::MailActive)return;mailCancelIndex=index;if(!mailCancelWindow&&MyGUI::Gui::getInstancePtr()){const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();mailCancelWindow=MyGUI::Gui::getInstance().createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-620)/2,(view.height-300)/2,620,300,MyGUI::Align::Default,"Window","MailCancelWindow");mailCancelWindow->eventWindowButtonPressed+=MyGUI::newDelegate(mailCancelWindowPressed);MyGUI::Widget* c=mailCancelWindow->getClientWidget();applyMercenarieFrame(c,false);mailCancelWarning=c->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",28,34,564,130,MyGUI::Align::Default);mailCancelWarning->setFontHeight(20);mailCancelWarning->setTextAlign(MyGUI::Align::Center);mailCancelConfirmButton=c->createWidget<MyGUI::Button>("Kenshi_Button1",38,190,250,52,MyGUI::Align::Default);mailCancelConfirmButton->eventMouseButtonClick+=MyGUI::newDelegate(mailCancelConfirm);mailCancelBackButton=c->createWidget<MyGUI::Button>("Kenshi_Button1",332,190,250,52,MyGUI::Align::Default);mailCancelBackButton->eventMouseButtonClick+=MyGUI::newDelegate(mailCancelBack);}MercenarieFonts::caption(mailCancelWarning,Loc::text("mail.cancel.warning"));MercenarieFonts::caption(mailCancelConfirmButton,Loc::text("mail.cancel.confirm"));MercenarieFonts::caption(mailCancelBackButton,Loc::text("common.back"));mailCancelWindow->setVisible(true);}
    void mailTrackerClicked(MyGUI::WidgetPtr sender){for(size_t i=0;i<questTrackerCards.size()&&i<questTrackerItems.size();++i)if(questTrackerCards[i].row==sender&&questTrackerItems[i].type==MCT_MAIL){openMailDelivery89(questTrackerItems[i].slot);return;}}

    void closeValidatedContractInterfaces(bool selectionValidated)
    {
        missionBookDelegationContext=false;missionBookPoolReady=false;contractAcceptArmed=false;delegationFinalArmed=false;
        if(contractsWindow)contractsWindow->setVisible(false);
        if(!ZetaFixRules::closeGiverAfterSelection(selectionValidated,contractBoardOpenedFromDialogue))return;
        Character* giver=contractDialogueGiver.isNull()?0:contractDialogueGiver.getCharacter();
        if(giver&&giver->dialogue&&!giver->dialogue->conversationHasEndedPrettyMuch())giver->dialogue->threadMessages.push_back(Dialogue::DT_END_DIALOG);
        contractDialogueGiver.setNull();contractBoardOpenedFromDialogue=false;
    }

    void contractsAcceptClicked(MyGUI::WidgetPtr)
    {
        if(missionBookDelegationContext&&!missionBookPersonalAvailable())return;
        if(missionBookDelegationContext&&missionBookTab==1){if(clientOptions.confirmAccept&&!contractAcceptArmed){contractAcceptArmed=true;if(contractsAccept)MercenarieFonts::caption(contractsAccept,Loc::text("options.confirm"));return;}contractAcceptArmed=false;if(acceptMissionBookSecurityPersonal(selectedOffer)){missionBookDelegationContext=false;missionBookPoolReady=false;if(contractsWindow)contractsWindow->setVisible(false);}return;}
        contractBarman=resolveContractBarman();
        if(selectedOffer<0||selectedOffer>=6)return;
        if(missionPending && escort){if(contractsWindow)contractsWindow->setVisible(false);openNegotiation();return;}
        if(!contractBarman || !boardOffers[selectedOffer].available) return;
        if(clientOptions.confirmAccept&&!contractAcceptArmed){contractAcceptArmed=true;if(contractsAccept)MercenarieFonts::caption(contractsAccept,Loc::text("options.confirm"));return;}contractAcceptArmed=false;
        BoardOffer& offer=boardOffers[selectedOffer];
        if(ContractDestinationRules::blocked(offer.townId))return;
        if(offer.missionType==MCT_SCIENCE&&!ContractDestinationRules::scientific(offer.townId))return;
        Character* physicalClient=contractBarman;const bool removePhysicalClient=isGuildVisitor(physicalClient)&&GuildVisitorOfferRules::giverMayDepart(offer.missionType);
        if(offer.missionType==MCT_MAIL){if(startPersonalMail(offer)){savedContractBoards[currentBoardKey].offers[selectedOffer].available=false;savedContractBoards[currentBoardKey].offers[selectedOffer].story="COURRIER ACCEPTE";saveContractBoards();closeValidatedContractInterfaces(true);if(removePhysicalClient)removeGuildVisitor(physicalClient,false);if(ou&&!negotiationWasPaused)ou->userPause(false);}else ou->showPlayerAMessage(Loc::text("v8.literal.056"),true);return;}
        destinationTown=offer.townId.c_str();
        destinationNameStorage=offer.townName;destinationName=destinationNameStorage.c_str();
        selectedDistance=offer.distance;
        selectedProfileSquad=offer.squadId;selectedProfileName=offer.profile;selectedProfileGroupSize=offer.groupSize;selectedProfileRarity=offer.rarity;
        missionTierIndex=offer.tierIndex;
        caravanMission=offer.missionType==1;
        scientificMission=offer.missionType==2;
        if(contractsWindow) contractsWindow->setVisible(false);
        if(ou && !negotiationWasPaused) ou->userPause(false);
        const char* tierChoice=offer.tierIndex==0?"tier_near":offer.tierIndex==1?"tier_medium":"tier_long";startMission(contractBarman,tierChoice,true);if(missionPending){savedContractBoards[currentBoardKey].offers[selectedOffer].available=false;savedContractBoards[currentBoardKey].offers[selectedOffer].story="CONTRAT ACCEPTE";saveContractBoards();closeValidatedContractInterfaces(true);}if(removePhysicalClient&&missionPending)removeGuildVisitor(physicalClient,false);
    }

    void createContractsBoard()
    {
        MercenariePerf::Phase boardCreationPhase("board-widgets");
        if(contractsWindow || !MyGUI::Gui::getInstancePtr())return;
        MyGUI::Gui* g=MyGUI::Gui::getInstancePtr();const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();
        Ogre::ResourceGroupManager& resources=Ogre::ResourceGroupManager::getSingleton();
        if(!resources.resourceLocationExists("mods/Guild Escort Contracts/gui/gfx","GUI"))resources.addResourceLocation("mods/Guild Escort Contracts/gui/gfx","FileSystem","GUI");
        int w=std::min(1540,view.width-24),h=std::min(980,view.height-24);
        if(!MyGUI::ResourceManager::getInstance().isExist("ContractBoardWindowV6"))MyGUI::ResourceManager::getInstance().load("ContractBoardV6.xml");
        if(!MyGUI::ResourceManager::getInstance().isExist("Board77Text"))MyGUI::ResourceManager::getInstance().load("MercenarieBoard77.xml");
        if(!MyGUI::ResourceManager::getInstance().isExist("GuildRegisterPanel"))MyGUI::ResourceManager::getInstance().load("GuildRegisterSkins.xml");
        contractsWindow=g->createWidget<MyGUI::Window>("ContractBoardWindowV6",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","GuildContractsBoard");
        contractsWindow->eventWindowButtonPressed+=MyGUI::newDelegate(contractsWindowButtonPressed);
        MyGUI::Widget* c=contractsWindow->getClientWidget();
        MyGUI::Widget* backdrop=board77CreatePanel(c,0,0,w,h);board77Skin(backdrop,"Board77Frame");
        // Foreground content is attached to this single surface, without metal nesting.
        MyGUI::Widget* header=board77CreatePanel(c,0,0,w,54);
        contractsHeading=board77CreateText(header,12,9,w-60,36,27,registerLanguage(Loc::text("ui.the_mercenarie_command_post_contracts_68e2811"),Loc::text("ui.the_mercenarie_command_post_contracts_68e2811")),registerAmber);contractsHeading->setTextAlign(MyGUI::Align::Center);
        {int tabX=12,tabW=(w-66)/6;for(int i=0;i<6;++i){missionBookBoardTabs[i]=header->createWidget<MyGUI::Button>("Board77Card",tabX+i*tabW,5,tabW-4,40,MyGUI::Align::Default,"MissionBookBoardTab");missionBookBoardTabs[i]->setFontHeight(14);missionBookBoardTabs[i]->eventMouseButtonClick+=MyGUI::newDelegate(missionBookBoardTabClicked);missionBookBoardTabs[i]->setVisible(false);}}
        MyGUI::Button* close=header->createWidget<MyGUI::Button>("Board77Card",w-38,7,30,30,MyGUI::Align::Default);close->setUserString("board77Skin","Board77Card");MercenarieFonts::caption(close,"X");close->eventMouseButtonClick+=MyGUI::newDelegate(closeBoardV6);
        int pad=10,gap=14,bodyY=66,bodyH=h-106,leftW=w*28/100,infoW=w*23/100,centerX=pad+leftW+gap,centerW=w-2*pad-leftW-infoW-2*gap,infoX=centerX+centerW+gap;
        MyGUI::Widget* list=board77CreatePanel(c,pad,bodyY,leftW,bodyH);contractsListPanel=list;
        board77CreateText(list,12,10,leftW-24,28,20,registerLanguage(Loc::text("ui.available_offers"),Loc::text("ui.available_offers")),registerAmber);
        missionBookCategoryCombo=list->createWidget<MyGUI::ComboBox>("TheMercenarie_BookCategoryCombo",leftW-190,7,178,38,MyGUI::Align::Default);missionBookCategoryCombo->setComboModeDrop(true);missionBookCategoryCombo->setFontHeight(16);missionBookCategoryCombo->setTextColour(registerIvory);missionBookCategoryCombo->addItem(Loc::text("missionbook.category.all"));missionBookCategoryCombo->addItem(Loc::text("missionbook.category.legal"));missionBookCategoryCombo->addItem(Loc::text("missionbook.category.illegal"));missionBookCategoryCombo->addItem(Loc::text("missionbook.category.merchant"));missionBookCategoryCombo->addItem(Loc::text("missionbook.category.security"));missionBookCategoryCombo->setIndexSelected(0);missionBookCategoryCombo->eventComboChangePosition+=MyGUI::newDelegate(missionBookCategoryChanged);
        int offersY=52,offerH=(bodyH-offersY-54)/6-6;
        for(int i=0;i<6;++i){
            MyGUI::Widget* row=board77CreatePanel(list,8,offersY+i*(offerH+6),leftW-16,offerH);
            contractButtons[i]=row->createWidget<MyGUI::Button>("Board77Card",1,1,row->getWidth()-2,offerH-2,MyGUI::Align::Default);
            contractButtons[i]->setUserString("board77Skin","Board77Card");
            contractButtons[i]->eventMouseButtonClick+=MyGUI::newDelegate(contractOfferClicked);
            int iconSize=std::min(60,offerH-22),textX=iconSize+20,tw=row->getWidth()-textX-20;
            offerIconV6[i]=boardIconV6(row,0,8,(offerH-iconSize)/2,iconSize,missionColourV6(0));
            offerNameV6[i]=board77CreateText(row,textX,9,std::max(1,tw-34),offerH/3-2,18,"",registerIvory);
            offerTypeV6[i]=board77CreateText(row,textX,offerH/3+5,std::max(1,tw-34),offerH/3-4,16,"",registerIvory);
            offerRarityV6[i]=board77CreateText(row,textX,offerH*2/3+2,tw*42/100,offerH/3-6,14,"",registerIvory);
            offerPriceV6[i]=board77CreateText(row,textX+tw*42/100,offerH*2/3+2,tw*58/100,offerH/3-6,19,"",registerAmber);offerPriceV6[i]->setTextAlign(MyGUI::Align::Right|MyGUI::Align::Top);
            offerEdgesV6[i][0]=registerSolid(row,0,0,row->getWidth(),2,registerAmber);offerEdgesV6[i][1]=registerSolid(row,0,offerH-2,row->getWidth(),2,registerAmber);offerEdgesV6[i][2]=registerSolid(row,0,0,2,offerH,registerAmber);offerEdgesV6[i][3]=registerSolid(row,row->getWidth()-2,0,2,offerH,registerAmber);
            offerRerollV6[i]=createContractRerollButton(row,i,false);
            // Reroll tooltip belongs to its small button, not the whole offer card.
        }
        missionBookPagePrevious=list->createWidget<MyGUI::Button>("TheMercenarie_Button",10,bodyH-45,48,36,MyGUI::Align::Default);MercenarieFonts::caption(missionBookPagePrevious,"<");missionBookPagePrevious->eventMouseButtonClick+=MyGUI::newDelegate(missionBookPageClicked);missionBookPageLabel=board77CreateText(list,64,bodyH-45,leftW-128,36,15,"1 / 1",registerIvory);missionBookPageLabel->setTextAlign(MyGUI::Align::Center);missionBookPageNext=list->createWidget<MyGUI::Button>("TheMercenarie_Button",leftW-58,bodyH-45,48,36,MyGUI::Align::Default);MercenarieFonts::caption(missionBookPageNext,">");missionBookPageNext->eventMouseButtonClick+=MyGUI::newDelegate(missionBookPageClicked);
        contractsDetails=board77CreateText(list,0,0,1,1,12,"",registerIvory);contractsDetails->setVisible(false);
        MyGUI::Widget* mapPanel=c->createWidget<MyGUI::Widget>("Board77Panel",centerX,bodyY,centerW,bodyH-72,MyGUI::Align::Default,"ContractMapPanel");contractsMapPanel=mapPanel;mapPanel->setUserString("board77Skin","Board77Panel");
        contractsMapImage=mapPanel->createWidget<MyGUI::ImageBox>("ImageBox",4,4,centerW-8,bodyH-80,MyGUI::Align::Default,"ContractWorldMap");contractsMapImage->setImageInfo("GuildEscortMap.png",MyGUI::IntCoord(0,0,2048,2048),MyGUI::IntSize(2048,2048));contractsMapImage->setImageIndex(0);{MyGUI::IntSize loaded=contractsMapImage->getImageSize();char mapLog[160];sprintf_s(mapLog,"Guild Escort map ready: %dx%d, items=%u",loaded.width,loaded.height,static_cast<unsigned int>(contractsMapImage->getItemCount()));DebugLog(mapLog);}contractsMapImage->eventMouseWheel+=MyGUI::newDelegate(mapMouseWheel);contractsMapImage->eventMouseButtonPressed+=MyGUI::newDelegate(mapMousePressed);contractsMapImage->eventMouseDrag+=MyGUI::newDelegate(mapMouseDragged);
        createContractRegionMap(mapPanel);
        contractsMap=mapPanel->createWidget<MyGUI::TextBox>("Board77Text",12,8,centerW-24,68,MyGUI::Align::Default,"ContractMap");contractsMap->setUserString("board77Skin","Board77Text");contractsMap->setNeedMouseFocus(false);contractsMap->setFontHeight(13);contractsMap->setTextColour(MyGUI::Colour(0.86f,0.80f,0.68f));
        contractsMap->setSize(centerW-24,48);
        contractsMapImage->setCoord(4,62,centerW-8,mapPanel->getHeight()-66);
        MyGUI::Button* priceDetails=mapPanel->createWidget<MyGUI::Button>("Board77Card",centerW-210,8,196,34,MyGUI::Align::Default);missionBookPriceButton=priceDetails;MercenarieFonts::caption(priceDetails,Loc::text("ui.price_details"));priceDetails->setTextColour(registerIvory);priceDetails->setFontHeight(19);priceDetails->eventMouseButtonClick+=MyGUI::newDelegate(showContractPriceDetails);
        contractRouteDots.clear();
        // Solid GUI primitives, above the map and below place icons. No glyph clipping.
        for(int i=0;i<512;++i){MyGUI::Widget* dot=mapPanel->createWidget<MyGUI::Widget>("WhiteSkin",0,0,7,7,MyGUI::Align::Default);dot->setColour(MyGUI::Colour(1.0f,.78f,.18f));dot->setNeedMouseFocus(false);dot->setVisible(false);contractRouteDots.push_back(dot);}
        for(int i=0;i<14;++i){routeCityIcons[i]=mapPanel->createWidget<MyGUI::ImageBox>("ImageBox",0,0,68,68,MyGUI::Align::Default,"RouteCityIcon");routeCityIcons[i]->setImageTexture(i==0?"GuildEscortCityGreen.png":"GuildEscortCityRed.png");routeCityIcons[i]->eventMouseSetFocus+=MyGUI::newDelegate(cityMarkerHovered);routeCityIcons[i]->eventMouseWheel+=MyGUI::newDelegate(mapMouseWheel);routeCityIcons[i]->eventMouseButtonPressed+=MyGUI::newDelegate(mapMousePressed);routeCityIcons[i]->eventMouseDrag+=MyGUI::newDelegate(mapMouseDragged);routeCityIcons[i]->setVisible(false);cityMarkers[i]=mapPanel->createWidget<MyGUI::Button>("Board77Card",0,0,140,28,MyGUI::Align::Default,"RouteTownName");cityMarkers[i]->setUserString("board77Skin","Board77Card");cityMarkers[i]->eventMouseSetFocus+=MyGUI::newDelegate(cityMarkerHovered);cityMarkers[i]->eventMouseWheel+=MyGUI::newDelegate(mapMouseWheel);cityMarkers[i]->eventMouseButtonPressed+=MyGUI::newDelegate(mapMousePressed);cityMarkers[i]->eventMouseDrag+=MyGUI::newDelegate(mapMouseDragged);cityMarkers[i]->setVisible(false);}

        mapPanel->setColour(MyGUI::Colour(.065f,.075f,.08f));
        contractsMap->setSize(std::max(80,centerW-218),48);
        contractsAccept=c->createWidget<MyGUI::Button>("Board77Gold",centerX,bodyY+bodyH-62,centerW,54,MyGUI::Align::Default,"ContractAccept");
        contractsAccept->setUserString("board77Skin","Board77Gold");
        MercenarieFonts::caption(contractsAccept,registerLanguage(Loc::text("ui.select_this_contract"),Loc::text("ui.select_this_contract")));contractsAccept->setFontHeight(24);contractsAccept->setTextColour(registerAmber);contractsAccept->eventMouseButtonClick+=MyGUI::newDelegate(contractsAcceptClicked);
        registerSolid(contractsAccept,0,0,centerW,2,registerAmber);registerSolid(contractsAccept,0,52,centerW,2,registerAmber);registerSolid(contractsAccept,0,0,2,54,registerAmber);registerSolid(contractsAccept,centerW-2,0,2,54,registerAmber);
        MyGUI::Widget* info=board77CreatePanel(c,infoX,bodyY,infoW,bodyH);contractsInfoPanel=info;
        board77CreateText(info,12,10,infoW-24,30,24,registerLanguage(Loc::text("ui.information"),Loc::text("ui.information")),registerAmber);
        int cardH=(bodyH-98)/8,yy=46;
        for(int i=0;i<4;++i){
            MyGUI::Widget* card=board77CreatePanel(info,8,yy,infoW-16,cardH);int sz=std::min(60,cardH-16);
            if(i==0)boardMissionIconV6=boardIconV6(card,2,9,(cardH-sz)/2,sz,missionColourV6(2));
            else registerIcon(card,i==1?2:3,9,(cardH-sz)/2,sz,registerIvory);
            contractsInfo[i]=board77CreateText(card,sz+20,6,card->getWidth()-sz-28,cardH-12,15,"",registerIvory);
            if(i==2)for(int j=0;j<5;++j)boardDangerV6[j]=boardIconV6(card,7,sz+72+j*25,cardH/2,23,MyGUI::Colour(.4f,.4f,.4f));
            yy+=cardH+5;
        }
        board77CreateText(info,12,yy+2,infoW-24,28,24,registerLanguage(Loc::text("ui.rewards"),Loc::text("ui.rewards")),registerAmber);yy+=32;
        for(int i=0;i<4;++i){
            MyGUI::Widget* card=board77CreatePanel(info,8,yy,infoW-16,cardH);int sz=std::min(64,cardH-16);registerIcon(card,i==0||i==3?4:i==1?1:3,9,(cardH-sz)/2,sz,registerIvory);
            MyGUI::TextBox* text=board77CreateText(card,sz+20,6,card->getWidth()-sz-28,cardH-12,i==0?23:16,"",i==0||i==3?registerAmber:registerIvory);
            if(i==0)contractsRewardAmount=text;else if(i==1)boardXpV6=text;else if(i==2)contractsRewardReputation=text;else contractsRewardBonus=text;
            text->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
            yy+=cardH+1;
        }
        contractsRefreshText=board77CreateText(c,pad,h-30,w/2-10,24,14,"",registerIvory);
        contractsLegend=board77CreateText(c,w/2,h-30,w/2-10,24,14,"",registerIvory);contractsLegend->setTextAlign(MyGUI::Align::Right);
        missionBookBoardPageText=board77CreateText(c,pad+18,bodyY+18,w-pad*2-36,bodyH-36,19,"",registerIvory);missionBookBoardPageText->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);missionBookBoardPageText->setVisible(false);
        createMissionBookPresentation(c);
        createBoard77(c);
        contractsWindow->setVisible(false);
    }

    void analyseRegionPath(const std::vector<RoutePrototype::Point>& path,bool road,BoardOffer& offer)
    {
        double length=0,weighted=0,levels=0;int tags=0;bool unknown=false;std::string last;std::stringstream regions;
        const double bonuses[]={0,.10,.25,.50,.75};
        for(size_t segment=1;segment<path.size();++segment){double d=RoutePrototype::distance(path[segment-1],path[segment]);if(d<=0)continue;int samples=std::max(1,(int)ceil(d/1000.0));
            for(int j=0;j<samples;++j){double t=(j+.5)/samples;const RoutePrototype::Point& p=path[segment-1];const RoutePrototype::Point& q=path[segment];Ogre::Vector3 point((float)(p.x+(q.x-p.x)*t),(float)(p.y+(q.y-p.y)*t),(float)(p.z+(q.z-p.z)*t));
                GameData* biome=NativeRegionLookup::at(point);const MercRegionRisk* risk=biome?MercenarieRegions::find(biome->stringID,biome->name):0;
                int level=risk?std::max(1,std::min(5,risk->level)):3;if(!risk)unknown=true;
                double weight=d/samples;weighted+=weight*bonuses[level-1];levels+=weight*level;length+=weight;
                if(risk){tags|=risk->tags;if(last!=risk->id){regions<<','<<risk->id;last=risk->id;}}
            }
        }
        offer.distance=(float)length;offer.danger=1.0f+(float)(length>0?weighted/length:0);offer.dangerLevel=length>0?std::max(1,std::min(5,(int)(levels/length+.5))):1;offer.environmentTags=tags;
            std::stringstream version;version.imbue(std::locale::classic());version<<"V6EST:"<<offer.danger<<":"<<(road?"ROAD":"DIRECT")<<(unknown?":UNKNOWN":"")<<regions.str();offer.routeRegions=version.str()+(unknown?"":";REGION2;")+ContractRouteVisual::encode(path);
    }

    void analyseContractRoute(const Ogre::Vector3& from,const Ogre::Vector3& to,BoardOffer& offer)
    {
        MercenariePerf::Phase perf("routes");
        static PricingRoads::Graph graph;static bool loaded=false;
        if(!loaded){loaded=true;try{std::ifstream in("mods/Guild Escort Contracts/pricing-roads.dat");graph.read(in);}catch(const std::exception& e){ErrorLog(std::string(Loc::text("ui.pricing_roads_unavailable"))+e.what());}}
        std::vector<RoutePrototype::Point> path;RoutePrototype::Point a(from.x,from.y,from.z),b(to.x,to.y,to.z);
        bool road=graph.route(a,b,path);if(!road){path.push_back(a);path.push_back(b);}
        analyseRegionPath(path,road,offer);
    }

    int rollContractRarity(int level,bool guildHouse,float reputation)
    {
        if(level<=0)return MCR_COMMON;float rare=guildHouse?35.0f:22.0f,epic=guildHouse?10.0f:3.0f;if(level==1){rare=5.0f;epic=0;}else if(level==2){rare=18.0f;epic=1.0f;}float shift=std::max(-5.0f,std::min(5.0f,reputation*0.05f));rare=std::max(0.0f,rare+shift);epic=std::max(0.0f,epic+std::max(0.0f,shift*.35f));float roll=UtilityT::random(0.0f,100.0f);if(roll<epic)return MCR_EPIC;if(roll<epic+rare)return MCR_RARE;return MCR_COMMON;
    }

    using ContractPricing::calculateEstimatedContract;

    bool prepareOfferGroup(BoardOffer& offer)
    {
        GameData* data=ou->gamedata.getData(offer.squadId,SQUAD_TEMPLATE);if(!data)return false;
        const char* lists[]={"leader","squad","animals"};const char roles[]={'L','S','A'};
        // Only the single-leader templates used by these contracts are supported.
        if(data->getListSize("leader")!=1)return false;
        for(int attempt=0;attempt<16;++attempt){ContractGroupPlan::Plan p;
            for(int list=0;list<3;++list){const Ogre::vector<GameDataReference>::type* refs=data->getReferenceListIfExists(lists[list]);if(!refs)continue;
                for(size_t i=0;i<refs->size();++i){const GameDataReference& ref=(*refs)[i];int n=1;
                    if(list){int low=ref.values.value[0],high=ref.values.value[1];if(low<1||high<low||high>8||ref.values.value[2]!=(list==2?100:0))return false;n=UtilityT::randomInt(low,high);}
                    else if(ref.values.value[0]!=1||ref.values.value[1]!=100)return false;
                    p.entries.push_back(ContractGroupPlan::Entry(roles[list],ref.sid,n));
                }
            }
            if(offer.missionType==MCT_CARAVAN)ContractGroupPlan::limitCaravan(p);
            if(!p.valid())continue;offer.groupSize=p.people();if(offer.missionType==MCT_CARAVAN)offer.caravanSize=EscortEconomy::caravanSizeFor(p.people()+p.animals());
            size_t path=offer.routeRegions.find(";PATH=");offer.routeRegions.insert(path==std::string::npos?offer.routeRegions.size():path,ContractGroupPlan::encode(p));return true;
        }return false;
    }

    bool bindOfferGroup(GameData* data,const ContractGroupPlan::Plan& p,std::vector<ContractGroupPlan::Binding>& bindings,bool cappedCaravan)
    {
        const char* lists[]={"leader","squad","animals"};const char roles[]={'L','S','A'};size_t found=0;bindings.clear();
        for(int list=0;list<3;++list){if(!data->getReferenceListIfExists(lists[list]))continue;Ogre::vector<GameDataReference>::type* refs=data->_getReferenceList_nonConst(lists[list]);if(!refs)continue;
            for(size_t i=0;i<refs->size();++i){GameDataReference& ref=(*refs)[i];const ContractGroupPlan::Entry* entry=0;
                for(size_t j=0;j<p.entries.size();++j)if(p.entries[j].role==roles[list]&&p.entries[j].id==ref.sid){entry=&p.entries[j];break;}
                if(!entry){if(cappedCaravan&&list>0){bindings.push_back(ContractGroupPlan::Binding(ref.values.value,0));continue;}return false;}if(list==0&&(ref.values.value[0]!=1||ref.values.value[1]!=100))return false;if(list&&ref.values.value[2]!=(list==2?100:0))return false;++found;
                // Leader tuple is count/probability, NOT min/max. Preserve it.
                if(list)bindings.push_back(ContractGroupPlan::Binding(ref.values.value,entry->count));
            }
        }return found==p.entries.size();
    }

    bool templateNumber(GameData* data,const char* name,float& value)
    {
        if(!data)return false;auto i=data->idata.find(name);if(i!=data->idata.end()){value=(float)i->second;return true;}auto f=data->fdata.find(name);if(f!=data->fdata.end()&&_finite(f->second)){value=f->second;return true;}return false;
    }

    bool prepareContractFactors(BoardOffer& offer)
    {
        ContractGroupPlan::Plan group;if(!ContractGroupPlan::decode(offer.routeRegions,group))return false;
        ContractFactors::Quote factors;double weighted=0;
        for(size_t i=0;i<group.entries.size();++i){const ContractGroupPlan::Entry& e=group.entries[i];if(e.role=='A')continue;
            GameData* actor=ou->gamedata.getData(e.id,CHARACTER);float defence=0,toughness=0,athletics=0;bool known=false;
            const Ogre::vector<GameDataReference>::type* stats=actor?actor->getReferenceListIfExists("stats"):0;
            if(stats&&stats->size()==1){GameData* stat=(*stats)[0].getPtr(actor->getSourceContainer());known=templateNumber(stat,"defence",defence)&&templateNumber(stat,"toughness2",toughness)&&templateNumber(stat,"athletics",athletics);}
            if(known)weighted+=e.count*ContractFactors::fragility(defence,toughness,athletics);
            else if((!stats||stats->empty())&&templateNumber(actor,"combat stats",defence)){
                // Some templates specify only the engine's general combat level.
                // Use it as an estimated index, not as claimed toughness/athletics.
                weighted+=e.count*ContractFactors::combatFragility(defence);
            }else factors.unknown+=e.count;
        }
        factors.fragility=(int)(weighted/group.people()+.5); // Unknown profiles receive no invented premium.
        if(offer.missionType==MCT_CARAVAN){
            static const char* cargoIds[]={"584-gamedata.base","42159-gamedata.base","42164-gamedata.base","585-gamedata.base","585-gamedata.base"};
            int kind=std::max(0,std::min(4,offer.cargoClass));factors.item=cargoIds[kind];factors.count=group.people()*(kind==4?6:kind==3?4:2);
            GameData* item=ou->gamedata.getData(factors.item,ITEM);float value=0;if(!templateNumber(item,"value",value)||value<0||value>100000)return false;
            factors.unitValue=(int)value;factors.cargoPercent=kind==0?0:kind<3?5:factors.cargoValue()>=10000?20:15;
            offer.profile=kind==0?Loc::text("ui.fabric_caravan"):kind==1?Loc::text("ui.iron_plate_caravan"):kind==2?Loc::text("ui.component_caravan"):Loc::text("ui.luxury_goods_caravan");
        }
        if(!factors.valid())return false;size_t path=offer.routeRegions.find(";PATH=");offer.routeRegions.insert(path==std::string::npos?offer.routeRegions.size():path,ContractFactors::encode(factors));return true;
    }

    int countContractCargo(const std::vector<Character*>& members,const std::string& id)
    {
        int count=0;std::set<Item*> seen;
        for(size_t m=0;m<members.size();++m){Inventory* inventory=members[m]&&!members[m]->isDead()?members[m]->getInventory():0;if(!inventory)continue;lektor<InventorySection*>& sections=inventory->getAllSections();
            for(unsigned int s=0;s<sections.size();++s){if(!sections[s])continue;const Ogre::vector<InventorySection::SectionItem>::type& items=sections[s]->getItems();for(size_t i=0;i<items.size();++i){Item* item=items[i].item;if(item&&seen.insert(item).second&&item->getGameData()&&item->getGameData()->stringID==id)count+=std::max(0,item->quantity);}}
        }return count;
    }

    void provisionContractCargo(const ContractFactors::Quote& q)
    {
        if(!q.count||caravanMembers.empty())return;GameData* data=ou->gamedata.getData(q.item,ITEM);if(!data)return;
        int missing=std::max(0,q.count-countContractCargo(caravanMembers,q.item));
        for(int i=0;i<missing;++i){bool given=false;for(size_t j=0;j<caravanMembers.size()&&!given;++j){Character* member=caravanMembers[(i+j)%caravanMembers.size()];if(!member)continue;Item* item=ou->theFactory->createItem(data,hand(),0,0,0,0);if(item)given=member->giveItem(item,false,true);}if(!given)break;}
        int actual=countContractCargo(caravanMembers,q.item);char log[180];sprintf_s(log,"Guild Escort cargo: planned=%d present=%d nominalValue=%d",q.count,actual,q.cargoValue());DebugLog(log);
        if(actual<q.count)ou->showPlayerAMessage(Loc::text("ui.cargo_incomplete_no_cargo_premium_will_be_charged_please"),true);
    }

    void calculateBoardOffer(BoardOffer& offer,EscortClientWealth wealth)
    {
        if((offer.missionType==MCT_MAIL||offer.missionType==MCT_ESCORT||offer.missionType==MCT_CARAVAN)&&offer.routeRegions.find(";PRICE85;")==std::string::npos)offer.routeRegions+=";PRICE85;";
        EscortContractData c;c.type=(MercContractType)offer.missionType;c.source=(MercContractSource)offer.source;c.rarity=(MercContractRarity)offer.rarity;c.distanceKm=std::max(1.0f,offer.distance/1000.0f)*(offer.missionType==MCT_ESCORT||offer.missionType==MCT_MAIL?1.0f:2.0f);c.dangerLevel=offer.dangerLevel;c.environmentTags=offer.environmentTags;c.groupSize=offer.groupSize;c.caravanSize=(MercCaravanSize)offer.caravanSize;c.cargoClass=(MercCargoClass)offer.cargoClass;c.studyClass=(MercStudyClass)offer.studyClass;c.studyDuration=(MercStudyDuration)offer.studyDuration;c.wealth=wealth;c.routeRegions=offer.routeRegions;MailOfferPlan::Plan mail;if(offer.missionType==MCT_MAIL&&MailOfferPlan::decode(offer.routeRegions,mail))c.urgent=mail.urgent;calculateEstimatedContract(c,0);offer.estimatedPay=c.totalPay;offer.danger=c.danger;
        if(offer.routeRegions.find("V6EST:")==0){std::ostringstream terms;terms.imbue(std::locale::classic());terms<<";PRICE="<<c.reward.base<<','<<c.reward.distance<<','<<c.reward.mission<<','<<c.reward.environment<<','<<c.reward.afterDanger-c.reward.beforeDanger<<','<<c.reward.afterRarity-c.reward.afterDanger<<','<<c.reward.guildHouseBonus<<','<<c.totalPay<<';';size_t pos=offer.routeRegions.find(";PATH=");offer.routeRegions.insert(pos==std::string::npos?offer.routeRegions.size():pos,terms.str());}
    }

    void showContractPriceDetails(MyGUI::Widget*){openRewardPopup87();}

    bool analyseMailRegions(const RoutePrototype::Point& origin,const MailOfferPlan::Plan& mail,BoardOffer& risk){
        if(!shou||!shou->townList)return false;
        std::vector<RoutePrototype::Point> all;Ogre::Vector3 previous((float)origin.x,(float)origin.y,(float)origin.z);bool road=true;
        for(size_t i=0;i<mail.destinations.size();++i){
            Town* town=shou->townList->getTownBySID(mail.destinations[i].townId);if(!town)return false;
            BoardOffer leg;analyseContractRoute(previous,town->getPosition(),leg);previous=town->getPosition();
            std::vector<RoutePrototype::Point> path=ContractRouteVisual::decode(leg.routeRegions);if(path.size()<2)return false;
            if(leg.routeRegions.find(":DIRECT")!=std::string::npos)road=false;
            all.insert(all.end(),path.begin()+(all.empty()?0:1),path.end());
        }
        if(all.size()<2)return false;analyseRegionPath(all,road,risk);return true;
    }
    std::string refreshedRegionMetadata(const std::string& previous,const std::string& revised){
        std::string metadata=previous;size_t prefix=metadata.find(';');if(prefix==std::string::npos)return previous;metadata.erase(0,prefix);
        size_t marker=metadata.find(";REGION2;");if(marker!=std::string::npos)metadata.erase(marker,8);
        size_t price=metadata.find(";PRICE=");if(price!=std::string::npos){size_t stop=metadata.find(';',price+7);if(stop==std::string::npos)return previous;metadata.erase(price,stop-price);}
        size_t route=metadata.find(";PATH=");if(route==std::string::npos)return previous;
        size_t routeEnd=metadata.find(";;",route+6);metadata.erase(route,routeEnd==std::string::npos?std::string::npos:routeEnd-route+1);
        std::string result=revised;size_t newPath=result.find(";PATH=");if(newPath==std::string::npos)return previous;result.insert(newPath,metadata);return result;
    }

    bool refreshOfferRegions(BoardOffer& offer){
        if(!offer.available||offer.routeRegions.find("V6EST:")!=0||offer.routeRegions.find(";REGION3;")!=std::string::npos)return false;
        std::vector<RoutePrototype::Point> path=ContractRouteVisual::decode(offer.routeRegions);if(path.size()<2)return false;
        // Older multi-stop mail offers may store only the first leg. Repair
        // that preview, retaining every financial term byte for byte.
        MailOfferPlan::Plan mail;
        if(offer.missionType==MCT_MAIL&&offer.routeRegions.find(";REGION2;")==std::string::npos&&MailOfferPlan::decode(offer.routeRegions,mail)){
            BoardOffer full;if(!analyseMailRegions(path.front(),mail,full))return false;
            path=ContractRouteVisual::decode(full.routeRegions);if(path.size()<2)return false;
            size_t start=offer.routeRegions.find(";PATH="),end=offer.routeRegions.find(";;",start+6);
            offer.routeRegions.replace(start,end==std::string::npos?std::string::npos:end-start+1,ContractRouteVisual::encode(path));
        }
        ContractRegionRoute::Result risk=ContractRegionRoute::analyse(path);
        if(offer.missionType==MCT_CARAVAN||offer.missionType==MCT_SCIENCE){
            std::vector<RoutePrototype::Point> back=ContractRegionRoute::returnPath(offer.routeRegions);
            if(back.empty()){
                const RoutePrototype::Point& a=path.back();const RoutePrototype::Point& b=path.front();BoardOffer returning;
                analyseContractRoute(Ogre::Vector3((float)a.x,(float)a.y,(float)a.z),Ogre::Vector3((float)b.x,(float)b.y,(float)b.z),returning);
                back=ContractRouteVisual::decode(returning.routeRegions);if(back.size()<2)return false;
                std::string encoded=ContractRouteVisual::encode(back);encoded.replace(1,4,"BACKPATH");offer.routeRegions+=";"+encoded+";";
            }
            risk.merge(ContractRegionRoute::analyse(back));
        }
        if(risk.unknown)return false; // Do not invent a danger for unmapped cells.
        int previous=offer.dangerLevel;offer.dangerLevel=risk.maximum;
        if(offer.routeRegions.find(";PAYRISK=")==std::string::npos){std::ostringstream frozenRisk;frozenRisk<<";PAYRISK="<<previous<<';';offer.routeRegions+=frozenRisk.str();}
        offer.routeRegions+=";REGION3;";
        // estimatedPay, PRICE, rarity, distance and reward multipliers remain frozen.
        std::ostringstream log;log<<"REGION MAX destination="<<offer.townName<<" old="<<previous<<" new="<<offer.dangerLevel<<" crossed="<<risk.regions.size();DebugLog(log.str());return true;
    }

    void generateContractOffers(Character* barman,CityContractBoard& cityBoard,int firstSlot,int offerCount,bool reroll){
        MercenariePerf::Phase perf("offers");
        const char* tiers[]={Loc::text("ui.near_contract"),Loc::text("ui.medium_contract"),Loc::text("ui.long_contract")};
        const char* choices[]={"tier_near","tier_medium","tier_long"};
        int level=guildLevel();bool guildHouseSource=isGuildVisitor(barman);
        bool unitedOrigin=originCity=="Heft"||originCity=="Bark"||originCity=="Sho-Battai"||originCity=="Stoat"||originCity=="Heng"||originCity=="Clownsteady"||originCity=="Drifter's Last";
        int rotationTypeCounts[5]={0,0,0,0,0};std::set<std::string> rotationDestinations,rotationPairs;
        DebugLog("Contract rotation: generating 6 varied offers (bounded candidate search)");
        for(int i=firstSlot;i<firstSlot+offerCount;++i){
            BoardOffer& generated=cityBoard.offers[i];generated=BoardOffer();generated.source=guildHouseSource?MCS_GUILD_HOUSE:MCS_TAVERN;generated.rarity=rollContractRarity(level,guildHouseSource,0.0f);int tier=generated.rarity==MCR_EPIC?2:generated.rarity==MCR_RARE?UtilityT::randomInt(1,2):UtilityT::randomInt(0,1);
            const int visitorMissionType=guildHouseSource?GuildVisitorOfferRules::choose(visitorOfferProfile,recentGuildVisitorOfferTypes,UtilityT::randomInt(0,9999)):MCT_MAIL;
            const int rerollTypeOffset=reroll?UtilityT::randomInt(0,3):0;std::vector<ContractOfferVariety::Candidate> candidates;candidates.reserve(18);
            for(int attempt=0;attempt<18;++attempt){const int candidateType=(attempt+rerollTypeOffset)%4;ContractOfferVariety::Candidate c;c.type=guildHouseSource?visitorMissionType:(reroll?(candidateType==3?MCT_MAIL:candidateType):(i==0?MCT_MAIL:UtilityT::randomInt(0,2)));if(c.type==MCT_MAIL){int city=UtilityT::randomInt(0,13);if(city==1||city==7)continue;Town* target=shou&&shou->townList?shou->townList->getTownBySID(mapTownIds[city]):0;std::string originId=contractOriginTown&&contractOriginTown->getGameData()?contractOriginTown->getGameData()->stringID:"";if(!target||invalidContractDestination(mapTownIds[city])||destinationAtWar(target)||originId==mapTownIds[city])continue;BoardOffer probe;analyseContractRoute(barman->getPosition(),target->getPosition(),probe);c.destinationId=mapTownIds[city];c.destinationName=frenchPlaceName(target->getName());c.distance=probe.distance;}else{if(c.type==MCT_SCIENCE)chooseRuinDestination(choices[tier],barman->getPosition());else chooseDestination(choices[tier],barman->getPosition());c.destinationId=destinationTown;c.destinationName=destinationName;c.distance=selectedDistance;}if(!c.destinationId.empty()&&(c.type!=MCT_SCIENCE||ContractDestinationRules::scientific(c.destinationId)))candidates.push_back(c);}
            int selected=ContractOfferVariety::choose(candidates,rotationTypeCounts,rotationDestinations,rotationPairs);if(selected<0){ErrorLog("Contract rotation: no valid candidate produced");continue;}const ContractOfferVariety::Candidate& chosen=candidates[selected];int type=chosen.type;destinationTown=chosen.destinationId.c_str();destinationNameStorage=chosen.destinationName;destinationName=destinationNameStorage.c_str();selectedDistance=chosen.distance;
            const bool repeatedPair=rotationPairs.count(ContractOfferVariety::pairKey(type,chosen.destinationId))!=0,repeatedDestination=rotationDestinations.count(chosen.destinationId)!=0;++rotationTypeCounts[type];rotationDestinations.insert(chosen.destinationId);rotationPairs.insert(ContractOfferVariety::pairKey(type,chosen.destinationId));
            std::ostringstream varietyLog;varietyLog<<"Contract rotation slot="<<(i+1)<<" type="<<type<<" destination="<<chosen.destinationName;if(repeatedPair)varietyLog<<" duplicate-pair-fallback";else if(repeatedDestination)varietyLog<<" repeated-destination-fallback";else varietyLog<<" varied";DebugLog(varietyLog.str());
            generated.missionType=type;generated.tierIndex=tier;bool isCaravan=type==MCT_CARAVAN,isScientific=type==MCT_SCIENCE,isMail=type==MCT_MAIL;
            generated.tier=tiers[tier];generated.townId=destinationTown;generated.townName=destinationName;generated.distance=selectedDistance;Town* generatedTown=shou->townList->getTownBySID(destinationTown);analyseContractRoute(barman->getPosition(),generatedTown?generatedTown->getPosition():barman->getPosition(),generated);
            generated.rarityName=generated.rarity==MCR_EPIC?"EPIQUE":generated.rarity==MCR_RARE?"RARE":Loc::text("ui.common");
            if(visitorOfferVip&&generated.rarity<MCR_RARE){generated.rarity=MCR_RARE;generated.rarityName=Loc::text("ui.rare_vip_customer");}if(visitorOfferExceptional){generated.rarity=MCR_EPIC;generated.rarityName=Loc::text("ui.epic_exceptional_customer");}
            if(isScientific){generated.studyClass=generated.rarity==MCR_EPIC?UtilityT::randomInt(2,3):generated.rarity==MCR_RARE?UtilityT::randomInt(1,2):UtilityT::randomInt(0,1);generated.studyDuration=generated.rarity==MCR_EPIC?UtilityT::randomInt(2,3):generated.rarity==MCR_RARE?UtilityT::randomInt(1,2):UtilityT::randomInt(0,1);generated.profile=generated.studyClass==3?Loc::text("ui.great_scientific_expedition"):generated.studyClass==2?Loc::text("ui.major_study"):generated.studyClass==1?Loc::text("ui.standard_study"):Loc::text("ui.small_study");generated.squadId="880121-Guild Escort Contracts.mod";generated.groupSize=std::min(4,generated.studyClass+1);}
            else if(isCaravan){generated.groupSize=generated.rarity==MCR_EPIC?UtilityT::randomInt(5,8):generated.rarity==MCR_RARE?UtilityT::randomInt(3,6):UtilityT::randomInt(1,4);generated.caravanSize=EscortEconomy::caravanSizeFor(generated.groupSize);generated.cargoClass=generated.rarity==MCR_EPIC?UtilityT::randomInt(MCC_PRECIOUS,MCC_EXCEPTIONAL):generated.rarity==MCR_RARE?UtilityT::randomInt(MCC_COMMON,MCC_PRECIOUS):UtilityT::randomInt(MCC_BASIC,MCC_COMMON);const char* caravanProfiles[]={Loc::text("ui.basic_caravan"),Loc::text("ui.common_caravan"),Loc::text("ui.large_caravan"),Loc::text("ui.precious_caravan"),Loc::text("ui.exceptional_caravan")};generated.profile=caravanProfiles[generated.cargoClass];generated.squadId="880080-Guild Escort Contracts.mod";}
            else if(isMail){MailOfferPlan::Plan plan;plan.urgent=false;int deliveries=generated.rarity==MCR_EPIC?3:generated.rarity==MCR_RARE?2:1;std::set<std::string> towns;MailOfferPlan::Destination first;first.townId=generated.townId;first.townName=generated.townName;first.distanceKm=std::max(1.0f,generated.distance/1000.0f);first.role=MailContracts::RoleBarman;plan.destinations.push_back(first);towns.insert(first.townId);Town* firstTown=shou&&shou->townList?shou->townList->getTownBySID(first.townId):0;Ogre::Vector3 originPosition=barman->getPosition(),beforePosition=originPosition,previousPosition=firstTown?firstTown->getPosition():originPosition,firstPosition=previousPosition;std::string originId=contractOriginTown&&contractOriginTown->getGameData()?contractOriginTown->getGameData()->stringID:"";for(int d=1;d<deliveries&&firstTown;++d){std::vector<MailRoutePlanner::Candidate> candidates;std::vector<BoardOffer> legs;for(int city=0;city<14;++city){if(city==1||city==7||towns.count(mapTownIds[city])||originId==mapTownIds[city])continue;Town* target=shou&&shou->townList?shou->townList->getTownBySID(mapTownIds[city]):0;if(!target||destinationAtWar(target))continue;BoardOffer leg;analyseContractRoute(previousPosition,target->getPosition(),leg);MailRoutePlanner::Candidate candidate(city,MailRoutePlanner::Point(target->getPosition().x,target->getPosition().z),std::max(1.0f,leg.distance/1000.0f));candidates.push_back(candidate);legs.push_back(leg);}std::vector<MailRoutePlanner::Candidate> coherent=MailRoutePlanner::coherentCandidates(MailRoutePlanner::Point(originPosition.x,originPosition.z),MailRoutePlanner::Point(beforePosition.x,beforePosition.z),MailRoutePlanner::Point(previousPosition.x,previousPosition.z),MailRoutePlanner::Point(firstPosition.x,firstPosition.z),candidates);if(coherent.empty())break;int pool=std::min(3,(int)coherent.size()),pick=UtilityT::randomInt(0,pool-1),city=coherent[pick].index;Town* target=shou->townList->getTownBySID(mapTownIds[city]);if(!target)break;MailOfferPlan::Destination next;next.townId=mapTownIds[city];next.townName=frenchPlaceName(target->getName());next.distanceKm=coherent[pick].legKm;next.role=MailContracts::RoleBarman;plan.destinations.push_back(next);towns.insert(next.townId);beforePosition=previousPosition;previousPosition=target->getPosition();}generated.groupSize=(int)plan.destinations.size();generated.profile=plan.destinations.size()>1?(Loc::text("v8.literal.057")):(Loc::text("v8.literal.003"));generated.routeRegions+=MailOfferPlan::encode(plan);generated.distance=0;for(size_t d=0;d<plan.destinations.size();++d)generated.distance+=(float)(plan.destinations[d].distanceKm*1000.0);}
            else{int maxProfile=level>=6?4:level>=4?3:level>=2?2:1;int profile=UtilityT::randomInt(0,maxProfile);if(generated.rarity==2&&unitedOrigin)profile=5;const char* names[]={Loc::text("ui.civilian"),Loc::text("ui.merchant"),Loc::text("ui.soldier"),Loc::text("ui.family"),Loc::text("ui.mercenary_squad"),Loc::text("ui.noble")};const char* squads[]={"880101-Guild Escort Contracts.mod","880103-Guild Escort Contracts.mod","880105-Guild Escort Contracts.mod","880107-Guild Escort Contracts.mod","880109-Guild Escort Contracts.mod","880111-Guild Escort Contracts.mod"};const int groups[]={1,1,1,4,3,1};if(guildHouseSource){const char* visitorNames[]={Loc::text("ui.civilian"),Loc::text("ui.merchant"),Loc::text("ui.soldier"),Loc::text("ui.family"),Loc::text("ui.scientist"),Loc::text("ui.mercenary_squad"),Loc::text("ui.noble")};const char* visitorSquads[]={"880101-Guild Escort Contracts.mod","880103-Guild Escort Contracts.mod","880105-Guild Escort Contracts.mod","880107-Guild Escort Contracts.mod","880121-Guild Escort Contracts.mod","880109-Guild Escort Contracts.mod","880111-Guild Escort Contracts.mod"};const int visitorGroups[]={1,1,1,4,3,3,1};profile=std::max(0,std::min(6,visitorOfferProfile));generated.profile=visitorNames[profile];generated.squadId=visitorSquads[profile];generated.groupSize=visitorGroups[profile];}else{generated.profile=names[profile];generated.squadId=squads[profile];generated.groupSize=groups[profile];}}
        Loc::Catalogue storyArgs;storyArgs["destination"]=generated.townName;storyArgs["origin"]=originCity;generated.story=isMail?(Loc::text("v8.literal.058")):isScientific?Loc::format("mission.science.description",storyArgs):isCaravan?Loc::format("mission.caravan.description",storyArgs):storyFor(UtilityT::randomInt(0,5),generated.townName);
            bool groupPrepared=isMail||prepareOfferGroup(generated);if(groupPrepared&&!isMail)groupPrepared=prepareContractFactors(generated);
            // Mail danger covers every delivery leg, not only the first town.
            if(isMail){MailOfferPlan::Plan mail;BoardOffer risk;const Ogre::Vector3& p=barman->getPosition();if(MailOfferPlan::decode(generated.routeRegions,mail)&&analyseMailRegions(RoutePrototype::Point(p.x,p.y,p.z),mail,risk)){generated.routeRegions=refreshedRegionMetadata(generated.routeRegions,risk.routeRegions);generated.dangerLevel=risk.dangerLevel;generated.environmentTags=risk.environmentTags;}}
            EscortClientWealth previewWealth=generated.profile=="Noble"?ECW_NOBLE:generated.profile.find("Caravane")!=std::string::npos||generated.profile=="Marchand"?ECW_MERCHANT:ECW_TRAVELLER;calculateBoardOffer(generated,previewWealth);
            generated.prestigious=generated.rarity>=MCR_RARE;if(visitorOfferUrgent)generated.story=Loc::text("ui.urgent")+generated.story;
            generated.available=true;if(visitorOfferExceptional)generated.available=true;
            if(!groupPrepared){generated.available=false;generated.story=Loc::text("ui.composition_indisponible_modele_de_groupe_incompatible");ErrorLog("Guild Escort: unsupported offer group template");}
            else if(guildHouseSource&&!reroll)GuildVisitorOfferRules::remember(recentGuildVisitorOfferTypes,type);
            refreshOfferRegions(generated);
        }
    }

    void openContractsBoard(Character* barman,bool caravan,bool scientific=false)
    {
        if(isGuildVisitor(barman)&&!guildClientReady(barman))return;
        // Mission Management is a read/delegate view over the same persistent
        // city board. Its visible window must not make questPanelLocked()
        // reject a harmless refresh when the player changes tabs.
        if(!barman || (MissionOfferViewRules::needsQuestPreparation(missionBookDelegationContext)&&!prepareEscortQuest()))return;
        contractBarman=barman;contractBarmanHandle=barman->getHandle();contractBoardOpenedFromDialogue=barman->dialogue&&!barman->dialogue->conversationHasEndedPrettyMuch();contractDialogueGiver=contractBoardOpenedFromDialogue?barman->getHandle():hand();boardCaravan=false;boardScientific=false;contractOriginTown=barman->getCurrentTownLocation();originCity=contractOriginTown?frenchPlaceName(contractOriginTown->getName()):Loc::text("ui.unknown_location");loadContractBoards();
        const char* tiers[]={Loc::text("ui.near_contract"),Loc::text("ui.medium_contract"),Loc::text("ui.long_contract")};
        const char* choices[]={"tier_near","tier_medium","tier_long"};
        int level=guildLevel();bool guildHouseSource=isGuildVisitor(barman);std::string visitorKey=guildHouseSource?guildVisitorOfferKey(barman):std::string();currentBoardKey=guildHouseSource&&!visitorKey.empty()?visitorKey:originCity+(guildHouseSource?"#GUILD_HOUSE":"#TAVERN");bool unitedOrigin=originCity=="Heft"||originCity=="Bark"||originCity=="Sho-Battai"||originCity=="Stoat"||originCity=="Heng"||originCity=="Clownsteady"||originCity=="Drifter's Last";
        CityContractBoard& cityBoard=savedContractBoards[currentBoardKey];bool blockedOffer=false;for(int i=0;i<6;++i)if(invalidContractDestination(cityBoard.offers[i].townId)||(cityBoard.offers[i].missionType==MCT_SCIENCE&&!cityBoard.offers[i].townId.empty()&&!ContractDestinationRules::scientific(cityBoard.offers[i].townId)))blockedOffer=true;bool renew=blockedOffer||cityBoard.expiresAt<=currentGameHours||cityBoard.expiresAt>currentGameHours+48.1||cityBoard.offers[0].townId.empty();
        const int offerCount=guildHouseSource?1:6;
        if(renew){
        for(int clear=0;clear<6;++clear)cityBoard.offers[clear]=BoardOffer();
        generateContractOffers(barman,cityBoard,0,offerCount,false);
        cityBoard.expiresAt=currentGameHours+48.0;saveContractBoards();}
        bool regionRefresh=false;for(int i=0;i<6;++i)if(refreshOfferRegions(cityBoard.offers[i]))regionRefresh=true;if(regionRefresh)saveContractBoards();
        for(int i=0;i<6;++i)boardOffers[i]=cityBoard.offers[i];
        createContractsBoard(); if(!contractsWindow)return; contractAcceptArmed=false; selectedOffer=0;MercenarieFonts::caption(contractsWindow,mercenarieLocalize(Loc::text("ui.the_mercenarie_command_post_contracts")));if(!missionBookDelegationContext){if(contractsHeading)contractsHeading->setVisible(true);for(int i=0;i<6;++i)if(missionBookBoardTabs[i])missionBookBoardTabs[i]->setVisible(false);if(missionBookBoardPageText)missionBookBoardPageText->setVisible(false);if(contractsListPanel)contractsListPanel->setVisible(true);if(contractsMapPanel)contractsMapPanel->setVisible(true);if(contractsInfoPanel)contractsInfoPanel->setVisible(true);if(contractsAccept)contractsAccept->setVisible(true);}
        for(int i=0;i<6;++i){if(guildHouseSource&&i>0){contractButtons[i]->setVisible(false);continue;}contractButtons[i]->setVisible(true);std::stringstream label;if(!boardOffers[i].available){int waitHours=std::max(0,static_cast<int>(cityBoard.expiresAt-currentGameHours));label<<Loc::text("ui.new_mission_available_in")<<(waitHours/24)<<Loc::text("ui.d_f380c84")<<(waitHours%24)<<Loc::text("ui.h");contractButtons[i]->setEnabled(false);contractButtons[i]->setTextColour(MyGUI::Colour(0.78f,0.78f,0.72f));contractButtons[i]->setColour(MyGUI::Colour(0.72f,0.72f,0.68f));}else{MailOfferPlan::Plan mailPlan;const char* shortType=boardOffers[i].missionType==MCT_MAIL&&MailOfferPlan::decode(boardOffers[i].routeRegions,mailPlan)?Loc::text(MailPresentation::typeKey(mailPlan.destinations.size())):boardOffers[i].missionType==2?Loc::text("ui.expedition"):boardOffers[i].missionType==1?Loc::text("ui.caravan"):Loc::text("ui.escort");label<<"  "<<boardOffers[i].townName<<"\n  "<<mercenarieLocalize(shortType)<<"  -  "<<mercenarieLocalize(boardOffers[i].rarityName)<<"\n  "<<displayedContractCats(boardOffers[i].estimatedPay)<<Loc::text("ui.cats");contractButtons[i]->setEnabled(true);contractButtons[i]->setTextColour(MyGUI::Colour(0.86f,0.82f,0.68f));contractButtons[i]->setColour(MyGUI::Colour(1.0f,1.0f,1.0f));}MercenarieFonts::caption(contractButtons[i],label.str());}
        int remaining=std::max(0,static_cast<int>(cityBoard.expiresAt-currentGameHours));if(contractsRefreshText){std::stringstream refresh;refresh<<std::count_if(boardOffers,boardOffers+6,boardOfferAvailable)<<" "<<mercenarieLocalize(offerCount==1?Loc::text("ui.offer"):Loc::text("ui.offers"))<<"  -  "<<mercenarieLocalize(Loc::text("ui.next_rotation_in"))<<" "<<(remaining/24)<<" "<<mercenarieLocalize(Loc::text("ui.days"))<<" "<<(remaining%24)<<Loc::text("ui.h");MercenarieFonts::caption(contractsRefreshText,refresh.str());}
        if(missionBookDelegationContext&&missionBookPoolReady)refreshMissionBookContractHub();else updateContractsBoard(); negotiationWasPaused=ou->isPaused();ou->userPause(true);contractsWindow->setVisible(true);
    }

    RoutePrototype::Route testRoute;
    Ogre::Vector3 testRouteTarget(const Ogre::Vector3& target)
    {
        if(!routeTestEnabled())return target;
        if(testRoute.missionId!=currentMissionFiscalId||caravanMission||scientificMission||leavingBuilding||target.squaredDistance(destination)>1.0f)return target;
        if(testRoute.status==RoutePrototype::Travelling&&testRoute.next<testRoute.points.size()){
            const RoutePrototype::Point& p=testRoute.points[testRoute.next];return Ogre::Vector3((float)p.x,(float)p.y,(float)p.z);
        }
        return target;
    }
    #include "CaravanCustomersState.h"
    std::string caravanCustomerState;
    #define MERCENARIE_CARAVAN_CUSTOMERS 1
    void createCaravanCustomers();
    void callCaravanCustomers();
    void endCaravanCustomers();
    bool caravanCustomersBusy();
    bool caravanCustomersComplete();
    #include "CaravanTradeState.h"
    #include "CaravanDeliveryState.h"
    #include "CaravanVisitPlan.h"
    #define MERCENARIE_CARAVAN_VISIT 1
    void initializeCaravanVisit();
    #include "CaravanHomeAmbushState.h"
    #define MERCENARIE_CARAVAN_HOME_AMBUSH 1
    bool triggerCaravanHomeAmbush(Character*,Building*);
    void tickCaravanHomeAmbushFlight(float);
    bool caravanHomeAmbushFleeing(Character*);
    bool deliveryExit(Character*,Ogre::Vector3&);
    #define MERCENARIE_CARAVAN_DELIVERY 1
    bool caravanDeliveryOwns(Character*);
    bool caravanHasDeliveryGuard();
    bool tickCaravanDelivery(float);
    void showCaravanScenarioPicker();
    void maintainCaravanScenarioPicker();
    bool caravanDeliveryFor(const std::string&,int);
    #define MERCENARIE_CARAVAN_TRADE 1
    bool caravanTradeActive();
    #include "MissionRescueState.h"
    #include "MissionMovementOrders.h"
    #include "MissionCombatResponse.h"
    bool missionNavigationNotice(MissionMotionPolicy::Reason reason);
    bool missionRequestPlayerGuide();
    bool missionGuidancePending();
    bool missionPlayerGuided();
    void missionClearGuidanceTag();
    #include "MissionMotionRuntime.h"
    #define MERCENARIE_CARAVAN_FORMATION 1
    #include "MissionFormationRuntime.h"

    Character* missionGroupCarrierOf(Character* member)
    {
        if(!member)return 0;
        for(size_t i=0;i<progressMembers.size();++i){Character* carrier=progressMembers[i].getCharacter();if(carrier&&carrier!=member&&!carrier->isAnimal()&&carrier->isCarryingSomething&&carrier->getCarryingObject()==member)return carrier;}
        return 0;
    }

    void applyMissionPace();
    bool missionProjectExteriorRoadPoint(const Ogre::Vector3& wanted,Ogre::Vector3& projected){
        if(!ou||!ou->navmesh)return false;
        return ou->navmesh->getClosestExteriorPoint(wanted,30.0f,1.0f,projected)==1;
    }
    #include "MissionRoadSurface.h"
    #include "MissionRoadRuntime.h"
    #include "MissionGateRuntime.h"
    void issueTravelOrder(const Ogre::Vector3& requestedTarget,const char* reason="mission travel")
    {
        if(caravanTradeActive())return;
        if(missionGuidancePending())return;
        if (!escort || missionPending || missionPaused || (missionFollowing&&!leavingBuilding) || missionCasualtyWaiting || missionRescue.motionSuspended) return;
        if(routeTestEnabled()&&testRoute.missionId==currentMissionFiscalId&&testRoute.status==RoutePrototype::Blocked)return;
        Ogre::Vector3 target;
        missionRescue.legacy.select(destination);
        const bool legacy=missionRescue.legacy.active&&!leavingBuilding&&requestedTarget.squaredDistance(destination)<=1;
        if(legacy)target=requestedTarget;
        else if(!missionRoadTarget(requestedTarget,target)){missionHoldTravel(escort,"road route unavailable");missionWaitReason(MissionMotionPolicy::Inaccessible);return;}
        Character* routeLeader=escort;
        const bool continued=std::string(reason)=="next road point"&&missionContinueTravel(routeLeader,target);
        if(!continued)missionClearTravel(routeLeader,reason);
        std::ostringstream routeLog;routeLog<<"MISSION ROUTE id="<<currentMissionFiscalId<<" reason="<<reason<<" leader="<<routeLeader->getHandle().toString()<<" requested="<<requestedTarget<<" waypoint="<<target<<" destination="<<destination;DebugLog(routeLog.str());
        // A new waypoint/retry must not reuse the previous failed/completed path.
        // Keep the normal escort behaviour unchanged outside the prototype.
        if(routeTestEnabled()&&testRoute.missionId==currentMissionFiscalId&&testRoute.status==RoutePrototype::Travelling&&!leavingBuilding)
            routeLeader->getMovement()->invalidatePath();
        if(!continued)missionIssueOrder(routeLeader,MOVE_CUS_ORDERED,0,target);
        if(legacy){routeLeader->getMovement()->setRoadPreference(1.0f);routeLeader->getMovement()->setRoadDestination(target);}
        // Keep the ordinary navigation restored by missionClearTravel (preference
        // zero / roadWeight one). Preference one makes road distance cost zero
        // and selected a waypoint back at the origin town in the captured run.
        // MOVE owns the destination; do not reactivate that road detour here.
        const MoveSpeed routeSpeed=missionPace==EscortPace::Accelerated?RUN:WALK;
        routeLeader->getMovement()->setDesiredSpeedOrders(routeSpeed);
        routeLeader->getMovement()->setDesiredSpeed(routeSpeed);
        // MOVE_CUS_ORDERED alone owns the destination; no competing road override.
        missionOrderTrace(routeLeader,"route active");
        applyMissionPace();
        // Only the leader receives the route destination.
        updateMissionFormation(0);
        stationaryClock = 0.0f;
        incidentClock = escort->getPosition().squaredDistance(target);
        missionRescue.localRecoveryTarget=target;
    }

#include "RoutePrototypeRuntime.h"

    int guildVisitorPeopleCount()
    {
        int total=0;for(unsigned int i=0;i<guildVisitors.size();++i)total+=static_cast<int>(guildVisitors[i].restoredMembers.pending?guildVisitors[i].restoredMembers.identities.size():guildVisitors[i].members.size());return total;
    }

    bool isGuildVisitor(Character* who)
    {
        for(unsigned int i=0;i<guildVisitors.size();++i)if(guildVisitors[i].leader==who)return true;return false;
    }

    std::string guildVisitorOfferKey(Character* who)
    {
        for(unsigned int i=0;i<guildVisitors.size();++i)if(guildVisitors[i].leader==who)return guildVisitors[i].offerKey;return std::string();
    }

    void selectGuildVisitorOffer(Character* who)
    {
        visitorOfferUrgent=visitorOfferVip=visitorOfferExceptional=false;visitorOfferProfile=GuildVisitorOfferRules::Civilian;for(unsigned int i=0;i<guildVisitors.size();++i)if(guildVisitors[i].leader==who){visitorOfferUrgent=guildVisitors[i].urgent;visitorOfferVip=guildVisitors[i].vip;visitorOfferExceptional=guildVisitors[i].exceptional;visitorOfferProfile=guildVisitors[i].profile;return;}
    }

    bool guildSameHandle(const hand& a,const hand& b)
    {return a.isNull()||b.isNull()?a.isNull()&&b.isNull():a.type==b.type&&a.container==b.container&&a.containerSerial==b.containerSerial&&a.index==b.index&&a.serial==b.serial;}
    hand guildObjectHandle(RootObjectBase* object){if(object)return object->getHandle();hand empty;empty.setNull();return empty;}
    bool guildActuallySeated(Character* actor,Building* target)
    {
        if(!actor||!target||!actor->getStateBroadcast()||!actor->getStateBroadcast()->isSitting||!actor->getBody()||actor->getBody()->getCurrentSubject().getBuilding()!=target)return false;
        UseableStuff* usable=target->getUseableStuff();return usable&&(guildSameHandle(usable->getOccupant(),actor->getHandle())||usable->currentOperators.find(actor->getHandle())!=usable->currentOperators.end());
    }
    bool guildClientReady(Character* actor)
    {
        if(!actor)return false;
        for(size_t i=0;i<guildSeatReservations.size();++i)if(guildSameHandle(guildSeatReservations[i].actor,actor->getHandle())&&guildSeatReservations[i].kind==GuildSeatQueue::ClientSeat)return guildActuallySeated(actor,guildSeatReservations[i].seat.getBuilding());
        return false;
    }
    void releaseGuildVisitorSeat(Character* actor)
    {
        if(!actor)return;
        for(int i=(int)guildSeatReservations.size()-1;i>=0;--i)if(guildSameHandle(guildSeatReservations[i].actor,actor->getHandle())){
            Building* seat=guildSeatReservations[i].seat.getBuilding();if(seat&&seat->getUseableStuff())seat->getUseableStuff()->stopOperating(actor->getHandle());
            guildSeatReservations.erase(guildSeatReservations.begin()+i);
        }
        Building* current=actor->getBody()?actor->getBody()->getCurrentSubject().getBuilding():0;
        if(current&&current->getUseableStuff())current->getUseableStuff()->stopOperating(actor->getHandle());
        for(int i=(int)guildTravelRecords.size()-1;i>=0;--i)if(guildSameHandle(guildTravelRecords[i].actor,actor->getHandle()))guildTravelRecords.erase(guildTravelRecords.begin()+i);
    }
    std::string guildSeatActorKey(const hand& actor){return actor.isNull()?std::string():actor.toString();}
    std::string guildFiscalQueueKey(){std::ostringstream key;key<<"FISCAL:"<<fiscalParty.organisation;return key.str();}
    bool guildFiscalMember(const hand& actor)
    {
        // hand's virtual equality is not reliable across plugin-owned copies.
        for(size_t i=0;i<fiscalParty.members.size();++i)if(guildSameHandle(fiscalParty.members[i],actor))return true;
        return false;
    }
    unsigned long guildSeatTicket(const std::string& key){std::map<std::string,unsigned long>::iterator it=guildSeatTickets.find(key);if(it!=guildSeatTickets.end())return it->second;unsigned long ticket=guildSeatNextTicket++;guildSeatTickets[key]=ticket;return ticket;}
    bool guildManagedChair(Building* seat){return std::find(guildClientChairs.begin(),guildClientChairs.end(),seat)!=guildClientChairs.end()||std::find(guildWaitingChairs.begin(),guildWaitingChairs.end(),seat)!=guildWaitingChairs.end();}

    struct GuildSeatRuntimePerson
    {
        Character* actor;std::string actorKey,queueKey;unsigned long ticket;unsigned int memberOrder;bool leader;GuildVisitor* visitor;
        GuildSeatRuntimePerson():actor(0),ticket(0),memberOrder(0),leader(false),visitor(0){}
    };

    GuildTravelRecord& guildTravelRecord(Character* actor,Building* office)
    {
        for(size_t i=0;i<guildTravelRecords.size();++i)if(guildSameHandle(guildTravelRecords[i].actor,actor->getHandle())&&guildSameHandle(guildTravelRecords[i].office,office->getHandle()))return guildTravelRecords[i];
        GuildTravelRecord r;r.actor=actor->getHandle();r.office=office->getHandle();guildTravelRecords.push_back(r);return guildTravelRecords.back();
    }

    bool guildInsideTargetOffice(Character* actor,Building* office)
    {
        return actor&&office&&!office->isDestroyed()&&actor->getMovement()->isIndoors()&&actor->getMovement()->building.getBuilding()==office;
    }

    void guildTravelLog(Character* actor,Building* office,GuildTravelRecord& r,const char* event,int reason)
    {
        std::ostringstream out;out<<"GuildVisitors actor="<<actor->getHandle().toString()<<" office="<<office->getHandle().toString()<<" phase="<<(int)r.state.phase<<" event="<<event<<" reason="<<reason<<" seat="<<(r.commandedSeat.isNull()?"none":r.commandedSeat.toString());DebugLog(out.str());
    }

    bool guildResolveOfficeDestination(Character* actor,Building* office,Ogre::Vector3& destination)
    {
        if(!ou||!ou->navmesh||!ou->zoneMgr||!actor->getMovement()->havokCharacter)return false;
        if(!ou->navmesh->isLoaded(ou->zoneMgr->getMapSector(office->getPosition())))return false;
        // Native building marker first. The centre is only a query seed, never an order.
        const Ogre::Vector3 seeds[]={office->getPositionMarker(actor->getPosition()),office->getPosition()};
        for(int i=0;i<2;++i){Ogre::Vector3 projected;unsigned int face=0;
            if(!ou->navmesh->getClosestPoint(seeds[i],20.0f,1.0f,false,projected,face))continue;
            if(!ou->navmesh->getPositionValid(projected)||!ou->navmesh->isInterior(face)||!guildSameHandle(ou->navmesh->getHandle(face),office->getHandle()))continue;
            if(!ou->navmesh->pathExists(actor->getMovement()->havokCharacter,projected))continue;
            destination=projected;return true;
        }
        return false;
    }

    bool guildOfficeArrived(Character* actor,Building* office)
    {
        if(!actor||!office||office->isDestroyed())return false;
        GuildTravelRecord& r=guildTravelRecord(actor,office);
        // Queue refresh is read-only with respect to travel. The travel update owns arrival.
        return GuildVisitorTravel::eligible(r.state)&&(!r.state.restoreOrder||guildInsideTargetOffice(actor,office));
    }

    void updateGuildOfficeTravel(Character* actor,Building* office,float elapsed)
    {
        if(!actor||actor->isDead()||!office||office->isDestroyed())return;
        GuildTravelRecord& r=guildTravelRecord(actor,office);
        if(r.rejectCooldown>0){r.rejectCooldown-=elapsed;if(r.rejectCooldown<=0)r.rejectedSeats.clear();}
        const bool progressed=actor->getPosition().squaredDistance(r.lastPosition)>0.25f;
        if(progressed)r.lastPosition=actor->getPosition();
        const GuildVisitorTravel::Phase before=r.state.phase;
        const GuildVisitorTravel::Reason why=GuildVisitorTravel::update(r.state,true,guildInsideTargetOffice(actor,office),progressed,actor->getMovement()->pathFailed(),elapsed);
        if(before!=r.state.phase&&r.state.phase==GuildVisitorTravel::ArrivedAtOffice){r.state.restoreOrder=true;guildTravelLog(actor,office,r,"arrived-interior",0);}
        if(GuildVisitorTravel::eligible(r.state)){
            if(r.state.phase==GuildVisitorTravel::TravellingToSeat&&!r.state.restoreOrder&&!guildActuallySeated(actor,r.commandedSeat.getBuilding()))
                r.pendingSeatRetry=GuildVisitorTravel::retryReason(r.state,progressed,actor->getMovement()->pathFailed(),elapsed);
            return;
        }
        r.resolveRetry-=elapsed;
        if(why==GuildVisitorTravel::None&&r.destinationValid)return;
        if(r.resolveRetry>0)return;
        // Revalidate/reproject only at initial issue, restore or an actual retry.
        Ogre::Vector3 goal;
        if(!guildResolveOfficeDestination(actor,office,goal)){
            r.destinationValid=false;r.resolveRetry=3.0f;
            if(!r.missingLogged){guildTravelLog(actor,office,r,"no-reachable-office-point",(int)why);r.missingLogged=true;}
            return;
        }
        r.officeDestination=goal;r.destinationValid=true;r.missingLogged=false;r.resolveRetry=0;
        guildTravelLog(actor,office,r,"office-order",(int)why);
        if(why==GuildVisitorTravel::NativePathFailure||why==GuildVisitorTravel::Stagnation)actor->getMovement()->invalidatePath();
        actor->removeJob(SIT_AROUND);actor->removeJob(OPERATE_MACHINERY);actor->removeJob(MOVE_CUS_ORDERED);
        actor->getMovement()->setRoadPreference(0.0f);
        actor->addJob(MOVE_CUS_ORDERED,0,false,false,goal);
        actor->getMovement()->setDesiredSpeedOrders(WALK);
        // One owner: MOVE_CUS_ORDERED drives the native path, no second road override.
        GuildVisitorTravel::issued(r.state);
    }

    void collectGuildOfficeChairs(Building* office,std::vector<Building*>& clients,std::vector<Building*>& waiting)
    {
        // Rescan actual furniture, including newly built chairs, in the target office.
        if(!office||!ou||!ou->zoneMgr)return;
        lektor<Building*> mounted;office->getMountedBuildings(&mounted);std::vector<Building*> found;for(unsigned int i=0;i<mounted.size();++i)found.push_back(mounted[i]);
        lektor<Building*> local;ou->zoneMgr->findAllBuildings(local,office->getCurrentTownLocation(),ou->player?ou->player->getFaction():0,false,0,0);
        for(unsigned int i=0;i<local.size();++i)found.push_back(local[i]);
        const char* ids[]={"880130-Guild Escort Contracts.mod","880131-Guild Escort Contracts.mod"};
        for(int kind=0;kind<2;++kind){GameData* data=ou->gamedata.getData(ids[kind],BUILDING);if(data){lektor<Building*> linked;ou->zoneMgr->getBuildingsThatLinkTo(linked,data);for(unsigned int i=0;i<linked.size();++i)found.push_back(linked[i]);}}
        for(unsigned int i=0;i<found.size();++i){Building* seat=found[i];if(!seat||seat->isDestroyed()||seat->furnitureParentBuilding()!=office)continue;GameData* data=seat->getGameData();if(!data)continue;
            if(data->stringID==ids[0]||textContainsInsensitive(data->name,"chaise client")||textContainsInsensitive(data->name,"client chair"))appendUniqueChair(clients,seat);
            if(data->stringID==ids[1]||textContainsInsensitive(data->name,"chaise d'attente")||textContainsInsensitive(data->name,"waiting chair"))appendUniqueChair(waiting,seat);
        }
    }

    void refreshGuildOfficeSeatQueue(Building* office,bool developerForced)
    {
        for(int r=(int)guildSeatReservations.size()-1;r>=0;--r){Building* seat=guildSeatReservations[r].seat.getBuilding();Character* actor=guildSeatReservations[r].actor.getCharacter();if(!seat||seat->isDestroyed()||!actor||actor->isDead())guildSeatReservations.erase(guildSeatReservations.begin()+r);}
        if(!office||office->isDestroyed())return;
        for(size_t r=0;r<guildSeatReservations.size();++r){guildSeatReservations[r].actor=guildSeatReservations[r].actor.getCharacter()->getHandle();guildSeatReservations[r].seat=guildSeatReservations[r].seat.getBuilding()->getHandle();}
        std::vector<Building*> officeClients,officeWaiting;collectGuildOfficeChairs(office,officeClients,officeWaiting);
        std::vector<GuildSeatRuntimePerson> runtime;
        for(size_t v=0;v<guildVisitors.size();++v)
        {
            GuildVisitor& visitor=guildVisitors[v];if(visitor.targetHouse.getBuilding()!=office)continue;const std::string queueKey="VISITOR:"+visitor.offerKey;visitor.assignedSeats.clear();
            for(size_t m=0;m<visitor.members.size();++m)
            {
                Character* who=visitor.members[m];if(!who||who->isDead()||!guildOfficeArrived(who,office))continue;
                const unsigned long ticket=guildSeatTicket(queueKey);
                if(m==0&&who->dialogue){GameData* package=ou?ou->gamedata.getData("880003-Guild Escort Contracts.mod",DIALOGUE_PACKAGE):0;if(package&&who->dialogue->pacakgesIHave.find(package)==who->dialogue->pacakgesIHave.end())who->dialogue->addDialoguePackage(package);/* Active dialogue keeps its reservation and native seat job. */}
                GuildSeatRuntimePerson p;p.actor=who;p.actorKey=guildSeatActorKey(who->getHandle());p.queueKey=queueKey;p.ticket=ticket;p.memberOrder=(unsigned int)m;p.leader=m==0;p.visitor=&visitor;runtime.push_back(p);
            }
            visitor.orderRefresh=1.0f;
        }
        if(fiscalParty.waitingForConversation&&!fiscalParty.leader.isNull()&&fiscalTargetOffice.getBuilding()==office)
        {
            Character* who=fiscalParty.leader.getCharacter();if(who&&!who->isDead()&&guildOfficeArrived(who,office)){GuildSeatRuntimePerson p;p.actor=who;p.actorKey=guildSeatActorKey(who->getHandle());p.queueKey=guildFiscalQueueKey();p.ticket=guildSeatTicket(p.queueKey);p.leader=true;runtime.push_back(p);}
        }

        std::map<std::string,GuildSeatRuntimePerson*> peopleByKey;std::vector<GuildSeatQueue::Person> people;
        for(size_t i=0;i<runtime.size();++i){
            Character* actor=runtime[i].actor;GuildTravelRecord& travel=guildTravelRecord(actor,office);
            if(travel.pendingSeatRetry!=GuildVisitorTravel::None&&!travel.commandedSeat.isNull()){
                travel.rejectedSeats.push_back(travel.commandedSeat.toString());travel.rejectCooldown=60.0f;
                Building* failed=travel.commandedSeat.getBuilding();if(failed&&failed->getUseableStuff())failed->getUseableStuff()->stopOperating(actor->getHandle());
                for(int r=(int)guildSeatReservations.size()-1;r>=0;--r)if(guildSameHandle(guildSeatReservations[r].actor,actor->getHandle()))guildSeatReservations.erase(guildSeatReservations.begin()+r);
                guildTravelLog(actor,office,travel,"seat-released-after-failure",(int)travel.pendingSeatRetry);
            }
            peopleByKey[runtime[i].actorKey]=&runtime[i];
            const bool interacting=actor->dialogue&&!actor->dialogue->conversationHasEndedPrettyMuch();
            GuildSeatQueue::Person person(runtime[i].actorKey,runtime[i].ticket,runtime[i].memberOrder,runtime[i].leader,(travel.state.phase==GuildVisitorTravel::TravellingToSeat&&travel.pendingSeatRetry==GuildVisitorTravel::None)||interacting);
            person.excluded=travel.rejectedSeats;people.push_back(person);
        }
        std::vector<GuildSeatQueue::Seat> seats;std::map<std::string,Building*> seatsByKey;
        for(int kind=GuildSeatQueue::ClientSeat;kind<=GuildSeatQueue::WaitingSeat;++kind)
        {
            const std::vector<Building*>& source=kind==GuildSeatQueue::ClientSeat?officeClients:officeWaiting;
            for(size_t i=0;i<source.size();++i)
            {
                Building* seat=source[i];if(!seat||seat->isDestroyed()||seat->furnitureParentBuilding()!=office)continue;const std::string key=seat->getHandle().toString();GuildSeatQueue::Seat model(key,(GuildSeatQueue::SeatKind)kind);UseableStuff* usable=seat->getUseableStuff();if(!usable)continue;
                if(usable){hand occupant=usable->getOccupant();if(occupant.isNull()&&!usable->currentOperators.empty())occupant=*usable->currentOperators.begin();if(!occupant.isNull()){model.nativeOwner=guildSeatActorKey(occupant);model.blocked=true;}}
                for(size_t r=0;r<guildSeatReservations.size();++r)if(guildSameHandle(guildSeatReservations[r].seat,seat->getHandle())&&model.nativeOwner.empty()){model.nativeOwner=guildSeatActorKey(guildSeatReservations[r].actor);model.blocked=true;break;}
                seats.push_back(model);seatsByKey[key]=seat;
            }
        }
        std::ostringstream census;census<<"GuildVisitors office="<<office->getHandle().toString()<<" clients="<<officeClients.size()<<" waiting="<<officeWaiting.size();
        for(size_t i=0;i<seats.size();++i){Building* chair=seatsByKey[seats[i].id];census<<" [seat="<<seats[i].id<<" id="<<chair->getGameData()->stringID<<" kind="<<(int)seats[i].kind<<" occupant="<<(chair->getUseableStuff()->getOccupant().isNull()?"none":chair->getUseableStuff()->getOccupant().toString())<<" owner-or-reservation="<<seats[i].nativeOwner<<" operators="<<chair->getUseableStuff()->currentOperators.size()<<"]";}
        static std::map<std::string,std::string> lastCensus;const std::string officeId=office->getHandle().toString();if(lastCensus[officeId]!=census.str()){lastCensus[officeId]=census.str();DebugLog(census.str());}
        const std::vector<GuildSeatQueue::Assignment> plan=GuildSeatQueue::plan(people,seats);

        // Publish every new target first. Native jobs are changed only after this
        // complete reservation set exists, so an Attente -> Client promotion cannot
        // expose the Client chair to another participant during the transfer.
        std::vector<GuildSeatReservation> nextReservations;
        for(size_t r=0;r<guildSeatReservations.size();++r){Building* seat=guildSeatReservations[r].seat.getBuilding();if(seat&&seat->furnitureParentBuilding()!=office)nextReservations.push_back(guildSeatReservations[r]);}
        for(size_t i=0;i<plan.size();++i)
        {
            std::map<std::string,GuildSeatRuntimePerson*>::iterator p=peopleByKey.find(plan[i].person);std::map<std::string,Building*>::iterator s=seatsByKey.find(plan[i].seat);if(p==peopleByKey.end()||s==seatsByKey.end())continue;
            GuildSeatReservation reservation;reservation.actor=p->second->actor->getHandle();reservation.seat=s->second->getHandle();reservation.kind=(int)plan[i].kind;reservation.queueKey=p->second->queueKey;nextReservations.push_back(reservation);if(p->second->visitor)p->second->visitor->assignedSeats.push_back(reservation.seat);
        }
        guildSeatReservations.swap(nextReservations);

        for(size_t i=0;i<runtime.size();++i)
        {
            Building* target=0;for(size_t r=0;r<guildSeatReservations.size();++r)if(guildSameHandle(guildSeatReservations[r].actor,runtime[i].actor->getHandle())){target=guildSeatReservations[r].seat.getBuilding();break;}
            Character* actor=runtime[i].actor;GuildTravelRecord& travel=guildTravelRecord(actor,office);
            UseableStuff* usable=target?target->getUseableStuff():0;
            const bool seated=guildActuallySeated(actor,target);
            const GuildVisitorTravel::Phase previousPhase=travel.state.phase;
            GuildVisitorTravel::assigned(travel.state,target!=0,seated);
            if(previousPhase!=travel.state.phase)guildTravelLog(actor,office,travel,"seat-phase",0);
            if(!travel.state.restoreOrder&&guildSameHandle(travel.commandedSeat,guildObjectHandle(target))&&(seated||!target||travel.pendingSeatRetry==GuildVisitorTravel::None))continue;
            Building* current=actor->getBody()?actor->getBody()->getCurrentSubject().getBuilding():0;
            if(current&&current!=target&&current->furnitureParentBuilding()==office){UseableStuff* old=current->getUseableStuff();if(old)old->stopOperating(actor->getHandle());}
            actor->removeJob(MOVE_CUS_ORDERED);actor->removeJob(SIT_AROUND);actor->removeJob(OPERATE_MACHINERY);
            travel.commandedSeat=guildObjectHandle(target);
            const int retryReason=(int)travel.pendingSeatRetry;
            if(travel.pendingSeatRetry!=GuildVisitorTravel::None)actor->getMovement()->invalidatePath();
            GuildVisitorTravel::issued(travel.state);travel.pendingSeatRetry=GuildVisitorTravel::None;travel.seatRetry=0;travel.lastPosition=actor->getPosition();
            if(!target){guildTravelLog(actor,office,travel,"waiting-without-seat",retryReason);continue;}
            actor->getMovement()->setRoadPreference(0.0f);actor->addJob(OPERATE_MACHINERY,target,false,false,target->getPositionMarker(actor->getPosition()));actor->getMovement()->setDesiredSpeedOrders(WALK);
            guildTravelLog(actor,office,travel,"seat-order-sent",retryReason);
            // OPERATE_MACHINERY resolves the native use position; do not override it with the mesh origin.
        }
    }

    void refreshGuildSeatQueue(bool developerForced)
    {
        std::vector<Building*> offices;
        for(size_t i=0;i<guildVisitors.size();++i){Building* office=guildVisitors[i].targetHouse.getBuilding();if(office&&!office->isDestroyed()&&std::find(offices.begin(),offices.end(),office)==offices.end())offices.push_back(office);}
        Building* fiscalOffice=fiscalTargetOffice.getBuilding();if(fiscalOffice&&!fiscalOffice->isDestroyed()&&std::find(offices.begin(),offices.end(),fiscalOffice)==offices.end())offices.push_back(fiscalOffice);
        for(int r=(int)guildSeatReservations.size()-1;r>=0;--r){Character* actor=guildSeatReservations[r].actor.getCharacter();bool active=actor&&!actor->isDead()&&fiscalParty.waitingForConversation&&guildSameHandle(guildSeatReservations[r].actor,fiscalParty.leader);for(size_t v=0;actor&&v<guildVisitors.size();++v)if(std::find(guildVisitors[v].members.begin(),guildVisitors[v].members.end(),actor)!=guildVisitors[v].members.end())active=true;Building* reservedSeat=guildSeatReservations[r].seat.getBuilding();if(!active||!reservedSeat||reservedSeat->isDestroyed()||!reservedSeat->furnitureParentBuilding()||reservedSeat->furnitureParentBuilding()->isDestroyed())guildSeatReservations.erase(guildSeatReservations.begin()+r);}
        for(int r=(int)guildTravelRecords.size()-1;r>=0;--r){Character* actor=guildTravelRecords[r].actor.getCharacter();bool active=actor&&!actor->isDead()&&guildFiscalMember(guildTravelRecords[r].actor);for(size_t v=0;actor&&v<guildVisitors.size();++v)if(std::find(guildVisitors[v].members.begin(),guildVisitors[v].members.end(),actor)!=guildVisitors[v].members.end())active=true;if(!active)guildTravelRecords.erase(guildTravelRecords.begin()+r);}
        for(size_t i=0;i<offices.size();++i)refreshGuildOfficeSeatQueue(offices[i],developerForced);
    }

    void seatFiscalCollector(){refreshGuildSeatQueue();}
    void seatGuildVisitors(bool developerForced){refreshGuildSeatQueue(developerForced);}

    void removeGuildVisitor(Character* leader,bool reputationPenalty)
    {
        for(unsigned int i=0;i<guildVisitors.size();++i)if(leader&&guildVisitors[i].leader==leader)
        {
            if(guildVisitors[i].restoredLeader.pending||guildVisitors[i].restoredMembers.pending)return;
            Building* office=guildVisitors[i].targetHouse.getBuilding();const std::string seatQueueKey="VISITOR:"+guildVisitors[i].offerKey;DepartingVisitors departing;departing.members=guildVisitors[i].members;departing.restoredMembers=guildVisitors[i].restoredMembers;guildVisitors.erase(guildVisitors.begin()+i);guildSeatTickets.erase(seatQueueKey);TownBase* location=office?office->getCurrentTownLocation():0;Town* town=location?location->isTown():0;Ogre::Vector3 exit=town?town->getPositionOutsideTownGates(1200.0f):(office?office->getPosition()+Ogre::Vector3(1200,0,1200):Ogre::Vector3::ZERO);
            for(unsigned int j=0;j<departing.members.size();++j){Character* who=departing.members[j];if(!who)continue;releaseGuildVisitorSeat(who);disableDepartingVisitorDialogue(who);who->removeJob(SIT_AROUND);who->removeJob(OPERATE_MACHINERY);who->removeJob(FOLLOW_WHILE_TALKING);who->removeJob(MOVE_CUS_ORDERED);who->addJob(MOVE_CUS_ORDERED,0,false,false,exit);}
            departingVisitors.push_back(departing);
            // Waiting visitors have not accepted a contract: no reputation penalty.
            seatGuildVisitors();return;
        }
    }

    bool findExteriorWaypoint(Character* who,Ogre::Vector3& waypoint);

    void abandonGuildOffice(const std::string& key,Building* building)
    {
        if(!guildHouseNames.count(key))return;
        // Reuse the normal departure route: release seats and exit before cleanup.
        for(int i=int(guildVisitors.size())-1;i>=0;--i)if(guildVisitors[i].houseKey==key)removeGuildVisitor(guildVisitors[i].leader,false);
        Building* fiscalOffice=fiscalTargetOffice.isNull()?0:fiscalTargetOffice.getBuilding();
        if(fiscalOffice&&guildHouseKey(fiscalOffice)==key){departFiscalParty();fiscalTargetOffice.setNull();if(fiscalCollectorWindow)fiscalCollectorWindow->setVisible(false);}
        // Preserve debt, deadlines, penalties and fiscal history; detach only the destination.
        for(int i=0;i<2;++i)if(fiscalLedger.organisations[i].targetHouseId==key)fiscalLedger.organisations[i].targetHouseId.clear();
        if(building&&!building->isDestroyed()&&building->isThePlayer()&&guildHouseKey(building)==key){std::string original=guildHouseOriginalNames.count(key)?guildHouseOriginalNames[key]:(building->getGameData()?building->getGameData()->name:std::string());if(!original.empty())building->setName(original);}
        guildHouseNames.erase(key);guildHouseCities.erase(key);guildHouseOriginalNames.erase(key);
        if(designatedGuildHouseKey==key){designatedGuildHouseKey.clear();designatedGuildHouseHandle.setNull();guildBuilding=0;guildClientChair=0;guildClientChairs.clear();guildFiscalChair=0;guildWaitingChairs.clear();guildClientSystemActive=false;guildHouseOperational=false;currentGuildHouseKey.clear();currentGuildHouseName.clear();operationalAnnouncementKey.clear();}
        saveReputations();saveFiscalLedger();refreshGuildFurniture(false);refreshOfficesView(true);
    }

    void spawnGuildVisitor(bool developerForced=false)
    {
        if(!guildBuilding||!guildClientChair||guildWaitingChairs.empty()){if(developerForced&&ou)ou->showPlayerAMessage(Loc::text("ui.impossible_test_define_the_guild_building_and_place_a"),true);return;}
        if((!developerForced&&(!guildClientSystemActive||guildLevel()<3))||missionPending){if(developerForced&&ou)ou->showPlayerAMessage(Loc::text("ui.test_impossible_a_contract_is_already_awaiting_validation"),true);return;}int capacity=static_cast<int>(guildClientChairs.size()+guildWaitingChairs.size());int freeSeats=capacity-guildVisitorPeopleCount();if(freeSeats<=0){if(developerForced&&ou)ou->showPlayerAMessage(Loc::text("ui.impossible_test_all_the_places_in_the_guild_house"),true);return;}
        int level=guildLevel();struct Candidate{const char* squad;const char* type;int maximum;float patience;};Candidate candidates[]={
            {"880101-Guild Escort Contracts.mod",Loc::text("ui.civilian"),1,1200},{"880103-Guild Escort Contracts.mod",Loc::text("ui.merchant"),1,900},{"880105-Guild Escort Contracts.mod",Loc::text("ui.soldier"),1,850},{"880107-Guild Escort Contracts.mod",Loc::text("ui.family"),5,1300},{"880121-Guild Escort Contracts.mod",Loc::text("ui.scientist"),3,1200},{"880109-Guild Escort Contracts.mod",Loc::text("ui.mercenary"),4,700},{"880111-Guild Escort Contracts.mod",Loc::text("ui.noble"),1,500}};
        int maxIndex=level>=8?6:level>=6?5:level>=5?4:level>=4?3:level>=2?2:level>=1?1:0;int pick=UtilityT::randomInt(0,maxIndex);for(int tries=0;tries<10&&candidates[pick].maximum>freeSeats;++tries)pick=UtilityT::randomInt(0,maxIndex);if(candidates[pick].maximum>freeSeats)pick=0;
        bool united=false;TownBase* location=guildBuilding->getCurrentTownLocation();std::string place=location?frenchPlaceName(location->getName()):"";united=place=="Heft"||place=="Bark"||place=="Sho-Battai"||place=="Stoat"||place=="Heng"||place=="Clownsteady"||place=="Drifter's Last";if(pick==6&&!united)pick=std::min(5,maxIndex);
        GameData* squadData=ou->gamedata.getData(candidates[pick].squad,SQUAD_TEMPLATE);Faction* faction=ou->factionMgr->getFactionByStringID("880050-Guild Escort Contracts.mod");Town* town=location?location->isTown():0;if(!squadData||!faction)return;
        Ogre::Vector3 spawn=town?town->getPositionOutsideTownGates(35.0f):guildBuilding->getPosition()+Ogre::Vector3(30,0,30);Platoon* platoon=ou->theFactory->createRandomSquad(faction,spawn,0,1,0,squadData,0,0,0,true,hand(),town,1.0f,SQ_ROAMING,false);if(!platoon||!platoon->activePlatoon||platoon->activePlatoon->things.size()==0)return;
        GuildVisitor visitor;visitor.type=candidates[pick].type;visitor.profile=pick;visitor.houseKey=currentGuildHouseKey;do{std::stringstream key;key<<currentGuildHouseKey<<"#VISITOR_"<<static_cast<unsigned long>(currentGameHours*100.0)<<"_"<<guildVisitorSequence++;visitor.offerKey=key.str();}while(savedContractBoards.count(visitor.offerKey));visitor.targetHouse=guildBuilding->getHandle();visitor.patience=candidates[pick].patience;visitor.remarkClock=UtilityT::random(120.0f,240.0f);visitor.urgent=UtilityT::random(0.0f,100.0f)<8.0f;visitor.vip=level>=6&&UtilityT::random(0.0f,100.0f)<3.0f;visitor.exceptional=level>=7&&UtilityT::random(0.0f,100.0f)<1.0f;if(visitor.urgent)visitor.patience*=0.55f;
        for(unsigned int i=0;i<platoon->activePlatoon->things.size();++i)visitor.members.push_back(static_cast<Character*>(platoon->activePlatoon->things[i]));visitor.leader=visitor.members[0];
        if(guildVisitorPeopleCount()+static_cast<int>(visitor.members.size())>capacity){for(unsigned int i=0;i<visitor.members.size();++i)faction->destroyObject(visitor.members[i]);return;}
        if(visitor.leader&&visitor.leader->dialogue){GameData* package=ou->gamedata.getData("880003-Guild Escort Contracts.mod",DIALOGUE_PACKAGE);if(package)visitor.leader->dialogue->addDialoguePackage(package);}
        guildVisitors.push_back(visitor);for(size_t m=0;m<visitor.members.size();++m)updateGuildOfficeTravel(visitor.members[m],visitor.targetHouse.getBuilding(),0);seatGuildVisitors(developerForced);if(visitor.leader&&successfulContracts>0){float roll=UtilityT::random(0.0f,100.0f);if(roll<12.0f)visitor.leader->sayALine(Loc::text("ui.your_last_escort_ended_well_so_i_ll_come"),true);else if(roll<32.0f)visitor.leader->sayALine(Loc::text("ui.a_former_satisfied_customer_recommended_your_services_to_me"),true);else if(roll<38.0f)visitor.leader->sayALine(Loc::text("ui.it_is_said_that_a_nearby_road_has_become"),true);else if(guildWaitingChairs.size()<3)visitor.leader->sayALine(Loc::text("ui.your_waiting_room_is_modest_but_your_reputation_led"),true);else visitor.leader->sayALine(Loc::text("ui.this_guild_house_inspires_more_confidence_than_the_taverns"),true);}std::string notice=visitor.exceptional?Loc::text("ui.a_client_offers_an_exceptional_contract"):visitor.vip?Loc::text("ui.a_vip_client_wants_to_meet_the_guild"):visitor.urgent?Loc::text("ui.a_client_has_just_arrived_with_an_urgent_contract"):std::string(Loc::text("ui.a_new_customer"))+visitor.type+Loc::text("ui.arrived_at")+currentGuildHouseName+".";// Arrival notification is emitted once the client reaches the building.
    }

    void updateGuildVisitors(float elapsed)
    {
        for(size_t i=0;i<guildVisitors.size();++i){
            GuildVisitor& v=guildVisitors[i];if(!v.restoredMembers.resolve(v.members,elapsed))continue;if(!v.restoredLeader.resolve(v.leader,elapsed))continue;Building* house=v.targetHouse.getBuilding();
            bool newlyArrived=false;
            for(size_t m=0;m<v.members.size();++m){const bool arrived=house&&v.members[m]&&guildOfficeArrived(v.members[m],house);updateGuildOfficeTravel(v.members[m],house,elapsed);if(!arrived&&house&&v.members[m]&&guildOfficeArrived(v.members[m],house))newlyArrived=true;}
            if(newlyArrived)v.orderRefresh=0;
            if(!v.arrivalNotified&&v.leader&&house&&guildOfficeArrived(v.leader,house)){
                v.arrivalNotified=true;
                std::string name=guildHouseNames.count(v.houseKey)?guildHouseNames[v.houseKey]:house->getName();
                std::string notice=v.offerKey.find("PAY:")==0?Loc::text("delegated.payment.arrived"):(v.leader->getName()+" ("+mercenarieLocalize(v.type)+")"+(Loc::text("ui.is_waiting_at"))+name+(Loc::text("ui.speak_to_them_to_review_their_offer")));
                if(ou)ou->showPlayerAMessage(notice,true);
            }
        }
        for(int d=static_cast<int>(departingVisitors.size())-1;d>=0;--d){
            DepartingVisitors& departing=departingVisitors[d];if(!departing.restoredMembers.resolve(departing.members,elapsed))continue;departing.cleanup-=elapsed;bool remaining=false;
            for(int j=(int)departing.members.size()-1;j>=0;--j){
                Character* who=departing.members[j];if(!who){departing.members.erase(departing.members.begin()+j);continue;}disableDepartingVisitorDialogue(who);
                bool nearPlayer=false;Character* nearest=0;float nearestDistance=3.4e38f;if(ou&&ou->player)for(size_t p=0;p<ou->player->playerCharacters.size();++p){Character* player=ou->player->playerCharacters[p];if(!player||player->isDead())continue;float distance=player->getPosition().squaredDistance(who->getPosition());if(distance<nearestDistance){nearestDistance=distance;nearest=player;}if(distance<1000000.0f)nearPlayer=true;}
                const bool indoors=who->getMovement()&&who->getMovement()->isIndoors();
                if(!indoors&&!nearPlayer){departing.phase=VISITOR_CLEANUP;who->removeJob(SIT_AROUND);who->removeJob(OPERATE_MACHINERY);who->removeJob(FOLLOW_WHILE_TALKING);who->removeJob(MOVE_CUS_ORDERED);if(who->getMovement()){who->getMovement()->halt();who->getMovement()->invalidatePath();}if(AI* ai=who->getAI()){hand none;none.setNull();ai->setCenterOfMovementTarget(none);}Faction* owner=who->getFaction();if(owner)owner->destroyObject(who);departing.members.erase(departing.members.begin()+j);continue;}
                remaining=true;if(departing.cleanup<=0&&!who->isDead()&&!who->isInCombatMode(true,true)){Ogre::Vector3 waypoint;if(indoors&&findExteriorWaypoint(who,waypoint)){}else{Ogre::Vector3 away=nearest?who->getPosition()-nearest->getPosition():Ogre::Vector3(1,0,1);away.y=0;if(away.squaredLength()<1.0f)away=Ogre::Vector3(1,0,1);away.normalise();waypoint=who->getPosition()+away*1200.0f;}who->removeJob(SIT_AROUND);who->removeJob(OPERATE_MACHINERY);who->removeJob(FOLLOW_WHILE_TALKING);who->removeJob(MOVE_CUS_ORDERED);who->getMovement()->invalidatePath();who->addJob(MOVE_CUS_ORDERED,0,false,false,waypoint);departing.cleanup=10.0f;}
            }
            if(!remaining&&departing.members.empty())departingVisitors.erase(departingVisitors.begin()+d);
        }
        if(guildClientSystemActive&&guildLevel()>=3){guildVisitorSpawnClock-=elapsed;if(guildVisitorSpawnClock<=0.0f){spawnGuildVisitor();guildVisitorSpawnClock=(UtilityT::random(420.0f,900.0f)-guildLevel()*20.0f)/GuildProgression::visitorRate(guildBuilding&&guildBuilding->getCurrentTownLocation()?localReputations[ReputationIdentity::key(guildBuilding->getCurrentTownLocation()->getName())]:0,escortReputation);}}
        bool refreshSeats=false;for(int i=static_cast<int>(guildVisitors.size())-1;i>=0;--i){GuildVisitor& visitor=guildVisitors[i];if(visitor.restoredLeader.pending||visitor.restoredMembers.pending)continue;const bool payment=visitor.offerKey.find("PAY:")==0;if(!payment&&visitor.arrivalNotified){visitor.patience-=elapsed;visitor.remarkClock-=elapsed;}visitor.orderRefresh-=elapsed;if(visitor.orderRefresh<=0)refreshSeats=true;if(!payment&&visitor.remarkClock<=0&&visitor.leader){const char* lines[]={Loc::text("ui.i_hope_someone_can_see_me_soon"),Loc::text("ui.this_guild_seems_in_high_demand_today"),Loc::text("ui.i_traveled_a_long_way_to_get_here"),Loc::text("ui.how_much_longer_will_we_have_to_wait"),Loc::text("ui.i_was_recommended_your_services_i_hope_i_didn")};visitor.leader->sayALine(lines[UtilityT::randomInt(0,4)],true);visitor.remarkClock=UtilityT::random(150.0f,260.0f);}if(!payment&&visitor.patience<=0){Character* leaving=visitor.leader;if(leaving)leaving->sayALine(Loc::text("ui.i_ve_waited_long_enough_i_will_offer_this"),true);removeGuildVisitor(leaving,true);}}if(refreshSeats)seatGuildVisitors();
    }

    bool findExteriorWaypoint(Character* who, Ogre::Vector3& waypoint)
    {
        if (!who || !who->getMovement()->isIndoors()) return false;
        Building* building = who->getMovement()->building.getBuilding();
        if (!building || building->doors.size() == 0) return false;

        bool found=false;float best=3.4e38f;
        // Door state + exterior projection before geometric distance. Bound the
        // local query to this building; Kenshi still decides full reachability.
        for(unsigned int i=0;i<building->doors.size()&&i<16;++i){
            Building* candidate=building->doors[i];if(!candidate)continue;
            bool rejected=false;if(who==escort)for(size_t j=0;j<missionRescue.failedExitDoors.size();++j)if(missionRescue.failedExitDoors[j].toString()==candidate->getHandle().toString())rejected=true;
            DoorStuff* door=candidate->getDoor();if(rejected||!door||door->isLocked())continue;
            Ogre::Vector3 raw=door->getDoorPosOutside_extraFarOut(3.0f),point;
            if(!missionProjectExteriorRoadPoint(raw,point)||point.squaredDistance(raw)>900.0f)continue;
            if(!ou||!ou->navmesh||!who->getMovement()->havokCharacter||ou->navmesh->pathExists(who->getMovement()->havokCharacter,point)!=1)continue;
            const float score=who->getPosition().squaredDistance(point)+(door->isOpen()?0.0f:10000.0f);
            if(score<best){best=score;waypoint=point;found=true;}
        }
        if(!found)return false;
        return true;
    }

    void copyOriginEnemies(Faction* missionFaction, Faction* originFaction)
    {
        if (!missionFaction || !originFaction || !missionFaction->relations || !originFaction->relations)
            return;

        const lektor<Faction*>* factions = ou->factionMgr->getAllFactions();
        if (!factions) return;

        for (unsigned int i = 0; i < factions->size(); ++i)
        {
            Faction* other = (*factions)[i];
            if (!other || other == missionFaction) continue;

            // All currently implemented contracts are cooperative missions.
            // Their dedicated Mission Escort faction must be a true ally of
            // the player, not merely neutral. Future war contracts may opt
            // out explicitly when that mission type is introduced.
            if (ou && ou->player && other == ou->player->participant)
            {
                missionFaction->relations->setRelation(other, 100.0f);
                continue;
            }

            // The mission faction remains neutral to everyone else, except
            // the current enemies of the faction that issued the contract.
            const float relation = originFaction->relations->getFactionRelation(other);
            missionFaction->relations->setRelation(other,
                originFaction->relations->isEnemy(other)
                    ? (relation < 0.0f ? relation : -100.0f)
                    : 0.0f);
        }
    }

    Character* nearestPlayer(const Ogre::Vector3& from, float& distanceSquared)
    {
        Character* result = 0;
        distanceSquared = 3.4e38f;
        for (unsigned int i = 0; i < ou->player->playerCharacters.size(); ++i)
        {
            Character* candidate = ou->player->playerCharacters[i];
            if (!candidate || candidate->isDead()) continue;
            float d = candidate->getPosition().squaredDistance(from);
            if (d < distanceSquared)
            {
                result = candidate;
                distanceSquared = d;
            }
        }
        return result;
    }

    bool usableAmbushTemplate(GameData* squad,std::string& reason)
    {
        if(!squad){reason="missing squad template";return false;}
        const char* lists[]={"leader","squad","animals"};
        bool hasCharacter=false;
        size_t references=0;
        for(size_t list=0;list<3;++list){
            const Ogre::vector<GameDataReference>::type* refs=squad->getReferenceListIfExists(lists[list]);
            if(!refs)continue;
            references+=refs->size();
            if(references>64){reason="oversized reference list";return false;}
            for(size_t i=0;i<refs->size();++i){
                const GameDataReference& ref=(*refs)[i];
                GameData* actor=ref.sid.empty()?0:ref.getPtr(squad->getSourceContainer());
                if(!actor||actor->type!=CHARACTER){reason=std::string("invalid ")+lists[list]+" reference";return false;}
                hasCharacter=true;
            }
        }
        if(!hasCharacter){reason="template contains no character";return false;}
        return true;
    }

    bool prepareAmbushPlan(GameData* squad,int difficulty,AmbushSpawnPlan::Plan& plan,
                           std::vector<ContractGroupPlan::Binding>& bindings,std::string& reason,int requested=0)
    {
        const char* lists[]={"leader","squad","animals"};
        std::vector<AmbushSpawnPlan::Entry> entries;
        std::vector<int*> values;
        bindings.clear();
        for(int list=0;list<3;++list){
            const Ogre::vector<GameDataReference>::type* checked=squad?squad->getReferenceListIfExists(lists[list]):0;
            if(!checked)continue;
            Ogre::vector<GameDataReference>::type* refs=squad->_getReferenceList_nonConst(lists[list]);
            if(!refs){reason=std::string("unavailable ")+lists[list]+" list";return false;}
            for(size_t i=0;i<refs->size();++i){
                GameDataReference& ref=(*refs)[i];
                if(list==0){
                    const int count=ref.values.value[0],chance=ref.values.value[1];
                    if(!AmbushSpawnPlan::validLeaderTuple(count,chance)){reason="invalid leader count/probability";return false;}
                    entries.push_back(AmbushSpawnPlan::Entry(AmbushSpawnPlan::Leader,count,count));
                }else{
                    const int low=ref.values.value[0],high=ref.values.value[1];
                    entries.push_back(AmbushSpawnPlan::Entry(list==1?AmbushSpawnPlan::Member:AmbushSpawnPlan::Animal,low,high));
                }
                values.push_back(ref.values.value);
            }
        }
        if(!AmbushSpawnPlan::build(entries,difficulty,plan,requested)){reason="composition cannot be capped safely";return false;}
        if(plan.overrideCounts)for(size_t i=0;i<entries.size();++i)
            if(entries[i].role!=AmbushSpawnPlan::Leader)bindings.push_back(ContractGroupPlan::Binding(values[i],plan.fixedCounts[i]));
        return true;
    }

    void rejectOversizedAmbush(Platoon* attackers,const std::string& reason)
    {
        ErrorLog(std::string("Guild Escort: ambush spawn rejected: ")+reason);
        if(!attackers||!attackers->activePlatoon)return;
        std::vector<Character*> spawned;
        for(unsigned int i=0;i<attackers->activePlatoon->things.size();++i)
            spawned.push_back(static_cast<Character*>(attackers->activePlatoon->things[i]));
        for(size_t i=0;i<spawned.size();++i)if(spawned[i]&&spawned[i]->getFaction())spawned[i]->getFaction()->destroyObject(spawned[i]);
    }

    #include "MissionRegionalAmbush.h"
    #include "RoadAmbushRuntime.h"

    struct DebugRaidResult {
        std::string reason, faction, squad; int requested, actual; Ogre::Vector3 position;
        DebugRaidResult():reason(Loc::text("debug.raid.spawn_unavailable")),requested(0),actual(0),position(Ogre::Vector3::ZERO){}
    };
    bool spawnBanditAmbush(DebugRaidResult* report=0)
    {
        if (!ou || !ou->factionMgr || !ou->theFactory || !ou->zoneMgr || !shou || !shou->townList ||
            !contractOriginFaction || !contractOriginFaction->relations || !escort) return false;
        GameData* biome=NativeRegionLookup::at(escort->getPosition());
        std::vector<RegionalAmbushOption> options;
        regionalAmbushOptions(biome,contractOriginFaction,currentContract.dangerLevel,options);
        if(options.empty()){
            DebugLog(std::string("MISSION REGIONAL RAID skipped region=")+(biome?biome->stringID:"unknown")+" reason=no eligible local enemy");
            if(report)report->reason=Loc::text("debug.raid.no_faction");return false;
        }
        int totalWeight=0;for(size_t i=0;i<options.size();++i)totalWeight+=options[i].weight;
        const size_t chosen=regionalAmbushPick(options,UtilityT::randomInt(0,totalWeight-1));
        if(chosen>=options.size())return false;
        GameData* squad=options[chosen].squad;Faction* bandits=options[chosen].faction;
        const std::string squadId=squad->stringID;
        const std::string factionId=squad->getFromList("faction",0);
        if(report){report->faction=factionId;report->squad=squadId;}
        DebugLog("MISSION REGIONAL RAID selected region="+biome->stringID+" name="+biome->name+" squad="+squadId+" faction="+factionId);
        std::string invalidReason;
        if(report)report->reason=Loc::text("debug.raid.no_composition");
        AmbushSpawnPlan::Plan ambushPlan;
        std::vector<ContractGroupPlan::Binding> ambushBindings;
        const int requestedCount=UtilityT::randomInt(AmbushSpawnPlan::minimumForDifficulty(currentContract.dangerLevel),AmbushSpawnPlan::capForDifficulty(currentContract.dangerLevel));
        if(!prepareAmbushPlan(squad,currentContract.dangerLevel,ambushPlan,ambushBindings,invalidReason,requestedCount)){
            ErrorLog(std::string("Guild Escort: ambush skipped: ")+squadId+" ("+invalidReason+")");
            return false;
        }
        Ogre::Vector3 pos;
        if(!roadAmbushPoint(pos)){
            DebugLog("MISSION REGIONAL RAID skipped reason=no reachable exterior spawn");
            if(report)report->reason=Loc::text("debug.raid.invalid_position");return false;
        }
        if(report){report->requested=ambushPlan.maximumActors;report->position=pos;report->reason=Loc::text("debug.raid.factory_failed");}
        Town* target = shou->townList->getTownBySID(destinationTown);
        Platoon* attackers=0;
        try{
            ContractGroupPlan::ScopedCounts cappedCounts(ambushBindings);
            attackers = ou->theFactory->createRandomSquad(
                bandits, pos, 0, 1, 0, squad, 0, 0, 0,
                false, hand(escort), target, 1.0f, SQ_ROAMING, false);
        }catch(const std::exception& e){
            ErrorLog(std::string("Guild Escort: ambush spawn rejected: ")+squadId+" ("+e.what()+")");
            return false;
        }catch(...){
            ErrorLog(std::string("Guild Escort: ambush spawn rejected: ")+squadId+" (unknown exception)");
            return false;
        }
        if (!attackers){ErrorLog("MISSION REGIONAL RAID factory returned no squad");return false;}
        const unsigned int actual=attackers->activePlatoon?attackers->activePlatoon->things.size():0;
        if(report){report->actual=(int)actual;report->reason=Loc::text("debug.raid.invalid_group");}
        Character* leader=attackers->getSquadLeader_theRealOne().getCharacter();
        if(!leader||actual!=static_cast<unsigned int>(ambushPlan.maximumActors)||actual>static_cast<unsigned int>(ambushPlan.cap)||actual>20){
            std::ostringstream rejected;rejected<<squadId<<" (actual="<<actual<<", cap="<<ambushPlan.cap<<", leader="<<(leader?"yes":"no")<<")";
            rejectOversizedAmbush(attackers,rejected.str());return false;
        }
        for(unsigned int i=0;i<actual;++i){
            Character* enemy=static_cast<Character*>(attackers->activePlatoon->things[i]);
            if(!roadAmbushActorReady(enemy,pos)){
                rejectOversizedAmbush(attackers,"road raid actor missing, misplaced or unable to reach convoy");
                return false;
            }
        }
        // Native templates provide appearance and basic gear only.  Combat
        // values are overwritten after creation, so vanilla/third-party edits
        // to their character records cannot make a low contract elite-level.
        const AmbushBalance::Profile profile=AmbushBalance::profileForDifficulty(currentContract.dangerLevel);
        for(unsigned int i=0;i<actual;++i){
            Character* enemy=static_cast<Character*>(attackers->activePlatoon->things[i]);
            CharStats* stats=enemy?enemy->getStats():0;if(!stats)continue;
            const float value=UtilityT::random((float)profile.minimum,(float)profile.maximum);
            stats->_strength=value;stats->_dexterity=value;stats->_toughness=value;
            stats->__meleeAttack=value;stats->_meleeDefence=value;stats->dodging=value;
            stats->katanas=value;stats->sabres=value;stats->hackers=value;stats->blunt=value;
            stats->heavyWeapons=value;stats->polearms=value;stats->unarmed=value;
            stats->perception=value;stats->bows=value;stats->turrets=value;
        }
        for(unsigned int i=0;i<actual;++i)roadAmbushEngage(static_cast<Character*>(attackers->activePlatoon->things[i]));
        std::ostringstream confirmation;confirmation<<"MISSION REGIONAL RAID spawned count="<<actual<<" position="<<pos<<" target="<<escort->getHandle().toString();DebugLog(confirmation.str());
        if(report)report->reason.clear();
        ++journeyData.ambushes;
        escort->sayALine(Loc::text("ui.an_ambush_get_ready"), true);
        ou->showPlayerAMessage(Loc::text("ui.ambush_enemy_bandits_attack_the_convoy"), true);
        return true;
    }

    void releaseWaitingHere()
    {
        for(size_t i=0;i<waitingHere.size();++i){
            Character* c=waitingHere[i].actor.getCharacter();if(!c)continue;
            c->removeJob(HOLD_POSITION);c->removeJob(MOVE_CUS_ORDERED);
            if(AI* ai=c->getAI()){
                ai->setManuveringFreedomLevel(waitingHere[i].movement);
                ai->setCenterOfMovement(waitingHere[i].center);
                ai->setCenterOfMovementTarget(waitingHere[i].centerTarget);
            }
        }
        waitingHere.clear();
    }

    void rememberWaitingHere(Character* c)
    {
        if(!c||c->isDead()||!c->getAI())return;
        WaitingHereState state;state.actor=c->getHandle();state.position=c->getPosition();
        state.movement=c->getAI()->ordersMovement;state.center=c->getAI()->centerOfMovement;
        state.centerTarget=c->getAI()->centerOfMovementTarget;state.returning=false;
        waitingHere.push_back(state);
        c->removeJob(MOVE_CUS_ORDERED);c->getMovement()->halt();
        c->addJob(HOLD_POSITION,0,false,false,state.position);
    }

    void enforceWaitingHere()
    {
        if(!missionPaused&&!missionPending)return;
        for(size_t i=0;i<waitingHere.size();++i){
            WaitingHereState& state=waitingHere[i];Character* c=state.actor.getCharacter();
            if(!c||c->isDead())continue;
            bool rescueAssigned=false;for(size_t r=0;r<missionRescue.tasks.size();++r)if(missionRescue.tasks[r].helper==c->getHandle())rescueAssigned=true;
            if(rescueAssigned){c->removeJob(HOLD_POSITION);if(AI* ai=c->getAI()){ai->setManuveringFreedomLevel(state.movement);ai->setCenterOfMovement(state.center);ai->setCenterOfMovementTarget(state.centerTarget);}continue;}
            const TaskType blocked[]={WANDERER,WANDER_TOWN,WANDERING_TRADER,PATROL,PATROL_TOWN,FOLLOW_PLAYER_ORDER,FOLLOW_SQUADLEADER,FOLLOW_WHILE_TALKING};
            for(size_t j=0;j<sizeof(blocked)/sizeof(blocked[0]);++j)c->removeJob(blocked[j]);
            if(AI* ai=c->getAI()){
                hand none;none.setNull();ai->setCenterOfMovementTarget(none);
                ai->setCenterOfMovement(state.position);ai->setManuveringFreedomLevel(AI::HOLD_GROUND);
            }
            if(c->isInCombatMode(true,true)||c->isBeingCarried()||c->getMedical()->isUnconcious()){
                c->removeJob(MOVE_CUS_ORDERED);state.returning=false;continue;
            }
            // Only a return to the saved anchor is allowed, never the mission destination.
            if(c->getPosition().squaredDistance(state.position)>0.25f){
                if(!state.returning){c->removeJob(HOLD_POSITION);c->removeJob(MOVE_CUS_ORDERED);c->addJob(MOVE_CUS_ORDERED,0,false,false,state.position);state.returning=true;}
                c->getMovement()->setRoadPreference(0.0f);
                // The return MOVE order owns this anchor; do not create another road target.
            }else{
                c->removeJob(MOVE_CUS_ORDERED);c->getMovement()->halt();
                if(state.returning){c->addJob(HOLD_POSITION,0,false,false,state.position);state.returning=false;}
            }
        }
    }

    void clearMissionFollow()
    {
        missionClearGuidanceTag();missionRescue.navigationRP=MissionNavigationRP::State<Ogre::Vector3>();
        for(size_t i=0;i<missionFollowers.size();++i){Character* c=missionFollowers[i].getCharacter();if(c)missionClearTravel(c,"stop mission follow");}
        missionFollowers.clear();missionFollowTarget.setNull();missionFollowing=false;
    }

    #include "MissionRescuePace.h"

    void setMissionPace(EscortPace::State pace)
    {
        if(!missionActive||contractLifecycle!=CONTRACT_ACTIVE )return;
        const EscortPace::State previous=missionPace;missionPace=pace;if(pace==EscortPace::Accelerated)missionForcedPace=true;applyMissionPace();
        std::ostringstream log;log<<"Mission pace: id="<<currentMissionFiscalId<<" actor="<<(escort?escort->getName():"<missing>")<<" "<<(int)previous<<" -> "<<(int)pace;DebugLog(log.str());
    }

    bool missionMemberIsCarried(Character* member)
    {
        if(!member||!member->isBeingCarried())return false;
        if(ou&&ou->player)for(unsigned int i=0;i<ou->player->playerCharacters.size();++i){Character* carrier=ou->player->playerCharacters[i];if(carrier&&carrier->isCarryingSomething&&carrier->getCarryingObject()==member)return true;}
        for(size_t i=0;i<progressMembers.size();++i){Character* carrier=progressMembers[i].getCharacter();if(carrier&&carrier!=member&&carrier->isCarryingSomething&&carrier->getCarryingObject()==member)return true;}
        return member->isBeingCarried();
    }

    #include "MissionRescueRuntime.h"
    #include "MissionAICleanup.h"
    #include "MissionLegacyRuntime.h"
    #include "MissionTravelRecovery.h"
    #include "MissionScienceMovement.h"
    #include "MissionRecovery.h"
    #define MERCENARIE_CARAVAN_SPEECH 1
    #include "CaravanSpeech.h"
    #include "CaravanNegotiationVariants.h"
    #include "CaravanTradeRuntime.h"
    #include "MissionNavigationRPRuntime.h"
    #include "CaravanCustomersRuntime.h"
    #include "CaravanDeliveryRuntime.h"
    #define MERCENARIE_CARAVAN_BATTLE_CONTROL 1
    #include "CaravanAmbushTargets.h"
    #include "MissionGroupDefence.h"
    #include "CaravanHomeAmbushSpawn.h"
    #include "CaravanHomeAmbushRuntime.h"

    Character* playerCarrierOfMissionMember()
    {
        if(!ou||!ou->player)return 0;
        for(unsigned int p=0;p<ou->player->playerCharacters.size();++p){Character* carrier=ou->player->playerCharacters[p];if(!carrier||!carrier->isCarryingSomething)continue;Character* carried=carrier->getCarryingObject().getCharacter();for(size_t i=0;i<progressMembers.size();++i)if(carried&&progressMembers[i].getCharacter()==carried)return carrier;}
        return 0;
    }

    bool abnormalPlayerCarry(Character* carrier)
    {
        if(!carrier)return false;
        float nearestGroup=1e30f;bool other=false;
        for(size_t i=0;i<progressMembers.size();++i){Character* member=progressMembers[i].getCharacter();if(!member||member->isDead()||member->isBeingCarried())continue;other=true;nearestGroup=std::min(nearestGroup,carrier->getPosition().squaredDistance(member->getPosition()));}
        float destinationDistance=carrier->getPosition().squaredDistance(destination);
        bool progressing=carriedDestinationDistance<0||destinationDistance<carriedDestinationDistance-10000.0f;
        bool movingAway=carriedDestinationDistance>=0&&destinationDistance>carriedDestinationDistance+10000.0f;
        carriedDestinationDistance=destinationDistance;
        if(destinationDistance<=2250000.0f||progressing)return false;
        return other?nearestGroup>6250000.0f:movingAway;
    }

    void cancelMission(const std::string& reasonKey);
    void failKidnappingContract()
    {
        // Normal failure is -8 local / -2 global. Add -7 / -2 so this major
        // fault totals -15 local and -4 global without erasing progression.
        escortReputation=GuildProgression::clampRep(escortReputation-2.0f);
        changeLocalReputation(-7.0f,Loc::text("ui.client_considered_kidnapped"),false,false);
        cancelMission("ui.contract_lost_client_considered_kidnapped");
    }

    void followConversationPartner(Dialogue* dialogue)
    {
        if(!missionActive||contractLifecycle!=CONTRACT_ACTIVE||!escort||!dialogue||!isMissionCommandSpeaker(dialogue->getCharacter()))return;
        // Only the actual dialogue counterpart; never fall back to selection or squad leader.
        hand targetHandle=dialogue->getConversationTarget();Character* target=targetHandle.getCharacter();
        if(!target||target->isDead()||!ou||!ou->player)return;
        bool playerOwned=false;for(unsigned int p=0;p<ou->player->playerCharacters.size();++p)if(ou->player->playerCharacters[p]==target)playerOwned=true;
        if(!playerOwned)return;
        clearMissionFollow();releaseWaitingHere();missionPaused=false;missionRescue.motionSuspended=false;missionRescue.passage.clear();missionRescue.leaderWatch=MissionMotionPolicy::Watch();
        missionFollowTarget=targetHandle;missionFollowing=true;waitingForPlayer=false;
        Character* commandActor=missionTemporaryLeader?missionTemporaryLeader:escort;
        commandActor->removeJob(MOVE_CUS_ORDERED);commandActor->removeJob(HOLD_POSITION);commandActor->removeJob(FOLLOW_SQUADLEADER);commandActor->removeJob(BODYGUARD);
        missionClearTravel(commandActor,"follow player command");missionIssueOrder(commandActor,FOLLOW_PLAYER_ORDER,target,target->getPosition());
        missionFollowers.push_back(commandActor->getHandle());
        updateMissionFormation(0);
        commandActor->sayALine(Loc::text("ui.i_will_follow_you"),true);
    }

    void pauseMission()
    {
        if (!missionActive || contractLifecycle != CONTRACT_ACTIVE || !escort || missionPaused) return;
        missionGroup.tick(escort,progressMembers,false,0);
        clearMissionFollow();
        missionPaused = true;
        waitingHere.clear();rememberWaitingHere(escort);
        for(size_t i=0;i<progressMembers.size();++i){Character* member=progressMembers[i].getCharacter();if(member&&member!=escort&&!member->isDead())rememberWaitingHere(member);}
        enforceWaitingHere();
        escort->sayALine(Loc::text("ui.all_right_i_m_waiting_for_you_here"), true);
    }

    void resumeMission()
    {
        if (!missionActive || contractLifecycle != CONTRACT_ACTIVE || !escort || (!missionPaused&&!missionFollowing)) return;
        const bool wasFollowing=missionFollowing;
        const bool wasNavigationSuspended=missionRescue.motionSuspended;
        if(wasNavigationSuspended)missionRescue.legacy=MissionLegacyNavigation::State<Ogre::Vector3>();
        clearMissionFollow();
        if(wasFollowing||wasNavigationSuspended){missionRescue.roadPlanned=false;missionRescue.roadPoints.clear();missionRescue.roadNext=0;missionRescue.roadProjectedIndex=(size_t)-1;missionRescue.localRecovery=false;}
        missionPaused = false;
        releaseWaitingHere();
        escort->removeJob(HOLD_POSITION);
        const std::vector<Character*>& members=scientificMission?scientificMembers:caravanMembers;
        for(size_t i=0;i<members.size();++i)if(members[i]&&members[i]!=escort)members[i]->removeJob(HOLD_POSITION);
        escort->sayALine(Loc::text("ui.very_well_let_us_continue_on_our_way"), true);
        missionRescue.motionSuspended=false;missionRescue.passage.clear();missionRescue.leaderWatch=MissionMotionPolicy::Watch();missionRescue.routeRetry=MissionMotionPolicy::RouteRetry();missionRescue.failedExitDoors.clear();missionRescue.regroupSeconds=0;missionGroup.reset();
        resumeRecoveredMissionPhase();
    }

    void cancelMission(const std::string& reasonKey)
    {
        clearCurrentPersonnel();
        const std::string reason=Loc::text(reasonKey.c_str());
        const bool refused=reasonKey=="ui.contract_refused_no_amount_was_paid";
        contractLifecycle = CONTRACT_FAILED;
        recordFiscalFailure(reasonKey.find("ui.contract_canceled_")==0?FENTRY_CANCELLED:FENTRY_FAILED);
        if(advancePaid>0 && ou && ou->player && ou->player->participant && ou->player->participant->factionOwnerships)
        {
            financeChange(-advancePaid,Finance::Contract,"finance.advance_return");
            ou->showPlayerAMessage(Loc::text("ui.the_deposit_was_refunded_to_the_customer"),true);
        }
        if ((missionActive || advancePaid > 0) && reasonKey!="ui.test_contract_cleaned_without_additional_consequences" && !currentContract.settlementPaid && !rewardedContractIds.count(currentMissionFiscalId) && !progressWriteBlocked)
        {
            bool abandoned=refused;
            GuildProgression::Result result=GuildProgression::failure(abandoned);rewardedContractIds.insert(currentMissionFiscalId);
            escortReputation=GuildProgression::clampRep(escortReputation+result.global);
            changeLocalReputation((float)result.local, Loc::text("ui.contract_fails_or_abandons"));
            ++failedContracts;
            archiveEscort(reasonKey.find("ui.contract_canceled_")==0?4:2,-1,-1);
            saveReputations();
        }
        ou->showPlayerAMessage(reason, true);
        if(escort){escort->sayALine(refused?Loc::text("ui.i_will_find_another_company_have_a_good_trip"):Loc::text("ui.this_contract_has_ended_we_are_leaving_again"),true);completedEscort=escort;completedCaravan.clear();for(size_t i=0;i<progressMembers.size();++i){Character* member=progressMembers[i].getCharacter();if(member)completedCaravan.push_back(member);}if(completedCaravan.empty())completedCaravan.push_back(escort);completedCleanupClock=60.0f;queueQuestCleanup();}
        finalizeMissionAI();
        missionActive = false; missionPending = false; missionPaused = false; missionPace = EscortPace::Normal; missionForcedPace=false; missionCasualtyWaiting=false; missionTemporaryLeader=0; clearMissionRescue(); missionSeparation=EscortMissionRules::Separation(); carriedDestinationDistance=-1;
        waitingForPlayer = false; leavingBuilding = false; wasInCombat = false;
        unconsciousSeconds = 0.0f; carriedByPlayerSeconds = 0.0f;
        missionElapsed = 0.0f; escortWasKnockedOut = false; journeyCombatCount = 0; advancePaid = 0;proximitySeconds=journeySeconds=0.0f;travelIncidentTriggered=false;lastImportantAlert.clear();
        contractOriginFaction = 0; escort = 0;escortHandle.setNull();caravanMembers.clear();caravanMission=false;caravanReturning=false;scientificMembers.clear();scientificMission=false;scientificResearching=false;scientificReturning=false;scientificInsideDiscovery=false;
        contractLifecycle = CONTRACT_NONE;
        updateTrackerUI();
    }

    bool escortIsInCage()
    {
        if (!escort || !escort->getBody()) return false;
        Building* subject = escort->getBody()->getCurrentSubject().getBuilding();
        return subject && subject->getSpecialFunction() == BF_CAGE;
    }

    bool playerIsCarryingEscort()
    {
        if (!escort || !escort->isBeingCarried()) return false;
        for (unsigned int i = 0; i < ou->player->playerCharacters.size(); ++i)
        {
            Character* player = ou->player->playerCharacters[i];
            if (player && player->isCarryingSomething && player->getCarryingObject() == escort)
                return true;
        }
        return false;
    }

    void enterDefensiveCombat()
    {
        if (!escort || wasInCombat) return;
        wasInCombat = true;
        ++journeyCombatCount;
        missionClearTravel(escort,"combat interrupts mission travel");
        missionRescue.leaderWatch.fresh();
        // Native combat remains free to block and retaliate.
    }

    void leaveDefensiveCombat()
    {
        if (!escort || !wasInCombat) return;
        wasInCombat = false;
        // Do not remove foreign HOLD orders or override native combat preferences.
        const char* remarks[] = {
            Loc::text("ui.is_that_all_i_have_known_more_dangerous_goats"),
            Loc::text("ui.they_should_have_stayed_home"),
            Loc::text("ui.nice_try_maybe_their_next_attempt_will_be_better"),
            Loc::text("ui.i_think_they_already_regret_choosing_us"),
            Loc::text("ui.at_least_they_made_the_journey_less_boring"),
            Loc::text("ui.thanks_i_d_rather_travel_with_you_than_with")
        };
        escort->sayALine(remarks[UtilityT::randomInt(0, 5)], true);
        resumeRecoveredMissionPhase();
    }

    void resetFinishedMission()
    {
        finalizeMissionAI();
        missionGroup.reset();
        contractLifecycle = CONTRACT_COMPLETED;
        clearCurrentPersonnel();
        missionActive = false;
        missionPending = false;
        missionPace = EscortPace::Normal;
        missionForcedPace=false;missionCasualtyWaiting=false;missionTemporaryLeader=0;clearMissionRescue();missionSeparation=EscortMissionRules::Separation();carriedDestinationDistance=-1;
        waitingForPlayer = false;
        leavingBuilding = false;
        stationaryClock = 0.0f;
        missionPaused = false;
        wasInCombat = false;
        unconsciousSeconds = 0.0f;
        carriedByPlayerSeconds = 0.0f;
        advancePaid = 0;
        missionElapsed = 0.0f; escortWasKnockedOut = false; journeyCombatCount = 0;proximitySeconds=journeySeconds=0.0f;travelIncidentTriggered=false;lastImportantAlert.clear();
        contractOriginFaction = 0;
        caravanMembers.clear();caravanMission=false;caravanReturning=false;scientificMembers.clear();scientificMission=false;scientificResearching=false;scientificReturning=false;scientificEntryAttempted=false;scientificInsideDiscovery=false;scientificWillEnter=false;scientificEntryResolved=false;
        escort = 0;escortHandle.setNull();
        contractLifecycle = CONTRACT_NONE;
        updateTrackerUI();
    }

    bool grantRareItemReward()
    {
        if(!ou||!ou->theFactory)return false;float distance=0;Character* recipient=nearestPlayer(destination,distance);if(!recipient)return false;
        const char* ids[]={"209-gamedata.base","515-gamedata.base","1359-gamedata.base","43959-rebirth.mod","44919-rebirth.mod","51403-rebirth.mod"};
        const char* id=ids[UtilityT::randomInt(0,5)];GameData* data=ou->gamedata.getData(id,ITEM);if(!data)data=ou->gamedata.getData(id,MAP_ITEM);if(!data)return false;
        Item* item=ou->theFactory->createItem(data,hand(),0,0,0,0);return item&&recipient->giveItem(item,true,true);
    }

    void observeProgressMembers()
    {
        if(!missionActive)return;
        for(size_t i=0;i<progressMembers.size();++i){Character* member=progressMembers[i].getCharacter();if(!member)continue;
            if(member->isDead())progressDeadMembers.insert((unsigned int)i);
            if(!member->isAnimal()&&member->getMedical()&&member->getMedical()->isUnconcious())progressAnyClientKo=true;
        }
        // Keep the durable loss indices, but never retain dead actor pointers
        // in travel/restore lists after the engine unloads their corpses.
        for(size_t i=0;i<caravanMembers.size();)if(caravanMembers[i]&&caravanMembers[i]!=escort&&caravanMembers[i]->isDead())caravanMembers.erase(caravanMembers.begin()+i);else ++i;
        for(size_t i=0;i<scientificMembers.size();)if(scientificMembers[i]&&scientificMembers[i]!=escort&&scientificMembers[i]->isDead())scientificMembers.erase(scientificMembers.begin()+i);else ++i;
        for(size_t i=0;i<waitingHere.size();){Character* c=waitingHere[i].actor.getCharacter();if(c&&c!=escort&&c->isDead())waitingHere.erase(waitingHere.begin()+i);else ++i;}
        for(size_t i=0;i<missionFollowers.size();){Character* c=missionFollowers[i].getCharacter();if(c&&c!=escort&&c->isDead())missionFollowers.erase(missionFollowers.begin()+i);else ++i;}
    }

    GuildProgression::Result missionProgressResult()
    {
        observeProgressMembers();int losses=(int)progressDeadMembers.size();bool resolved=progressRosterKnown;
        for(size_t i=0;i<progressMembers.size();++i)if(!progressMembers[i].getCharacter()&&!progressDeadMembers.count((unsigned int)i))resolved=false;
        ContractFactors::Quote cargo;bool intact=caravanMission&&ContractFactors::decode(currentContract.routeRegions,cargo)&&cargo.count>0&&countContractCargo(caravanMembers,cargo.item)>=cargo.count;
        GuildProgression::Result result=GuildProgression::success((int)currentContract.type,currentContract.distanceKm,currentContract.dangerLevel,losses,resolved&&!progressAnyClientKo&&!escortWasKnockedOut,intact);
        if(!resolved&&losses==0){result.xp-=10;result.quality-=10;result.local-=1;}
        return result;
    }

    int calculateGuildXpReward()
    {
        GuildProgression::Result result=missionProgressResult();progressReportLocal=result.local;progressReportGlobal=result.global;return result.xp;
    }

void settleSuccessfulContract(bool requestMoney);
void finalCashClicked(MyGUI::WidgetPtr);
void finalHalfCashClicked(MyGUI::WidgetPtr);
void finalReputationClicked(MyGUI::WidgetPtr);
void finalWindowButtonPressed(MyGUI::Window*,const std::string&);
void toggleFinalBonus(MyGUI::WidgetPtr);
#include "MissionReportView.h"
    #include "MissionBonusRuntime.h"
    bool missionCompletionNotificationsEnabled(){return clientOptions.showNotifications&&clientOptions.notifyMissionComplete;}
#include "MissionSettlementRuntime.h"

    void finalCashClicked(MyGUI::WidgetPtr){if(!finalWindow||!finalWindow->getVisible())return;if(reportReadOnly){finalWindow->setVisible(false);reportReadOnly=false;if(!negotiationWasPaused)ou->userPause(false);return;}requestedFinalBonusPercent=100;settleSuccessfulContract(true);}
    void finalHalfCashClicked(MyGUI::WidgetPtr){if(reportReadOnly||!finalWindow||!finalWindow->getVisible()||guildLevel()<1)return;unsigned int available=0;for(int i=0;i<MissionBonuses::Count;++i)if(finalBonusChoice.amounts[i]>0)available|=1u<<i;finalBonusChoice.selected=finalBonusChoice.selected==available?0:available;refreshFinalBonuses();}
    void finalReputationClicked(MyGUI::WidgetPtr){if(reportReadOnly||!finalWindow||!finalWindow->getVisible())return;settleSuccessfulContract(false);}
    void finalWindowButtonPressed(MyGUI::Window*,const std::string&){if(reportReadOnly){finalWindow->setVisible(false);reportReadOnly=false;if(!negotiationWasPaused)ou->userPause(false);return;}settleSuccessfulContract(false);}

    void createFinalWindow(){buildFinalReport();}

    void finishMission(bool success)
    {
        if(currentContract.settlementPaid||rewardedContractIds.count(currentMissionFiscalId)||(finalWindow&&finalWindow->getVisible()))return;
        missionGroup.tick(escort,progressMembers,false,0);
        clearMissionFollow();
        if(!success||!escort||escort->isDead()){cancelMission("ui.contract_fails_the_traveler_is_dead");return;}
        MedicalSystem* medical=escort?escort->getMedical():0;finalHealthy=medical&&!medical->isCrippled()&&medical->getOverallHealthRating()>=0.75f;finalFast=expectedTravelTime>0&&missionElapsed<=expectedTravelTime;
        {const int baseTip=UtilityT::random(0.0f,100.0f)<10.0f?UtilityT::randomInt(1,10)*100:0;finalClientTip=ContractRewards::apply(baseTip,ContractRewards::snapshot(currentContract.routeRegions));}pendingGuildXp=calculateGuildXpReward();int satisfaction=1+(finalHealthy?1:0)+(!escortWasKnockedOut?1:0)+(finalFast?1:0)+(journeyCombatCount<=1?1:0);satisfaction=std::max(1,std::min(5,satisfaction));
        reportEndHour=currentGameHours;
        prepareFinalBonuses();
        createFinalWindow();if(!finalWindow){settleSuccessfulContract(false);return;}
        negotiationWasPaused=ou->isPaused();ou->userPause(true);refreshFinalBonuses();finalWindow->setVisible(true);
    }

    void beginJourney();

    void closeNegotiation(bool resumeGame)
    {
        closePersonnelDialog();
        negotiationOpen = false;
        if (negotiationWindow) negotiationWindow->setVisible(false);
        if (contractDecisionWindow) contractDecisionWindow->setVisible(false);
        if (resumeGame && ou && !negotiationWasPaused) ou->userPause(false);
    }

    void suspendNegotiation()
    {
        if(!negotiationOpen) return;
        negotiationInsistence=0;
        negotiationSuspended=true;
        closeNegotiation(true);
        contractLifecycle=CONTRACT_CLIENT_MEETING;
        if(ou)ou->showPlayerAMessage(Loc::text("ui.negotiation_suspended_speak_to_the_traveler_again_to_take"),true);
    }

    #include "NegotiationView.h"

    void updateNegotiationPrice(size_t position)
    {
        int percent = static_cast<int>(position) - 20;
        calculateEstimatedContract(currentContract,percent);
        proposedReward=currentContract.totalPay;
        currentContract.bonusPay=0;
        currentContract.advance=currentContract.totalPay*negotiatedAdvancePercent/100;
        currentContract.finalPay=currentContract.totalPay-currentContract.advance;
        refreshNegotiationV9Price(percent);
        if(reactionText&&!counterOfferActive){float repEffect=std::max(-5.0f,std::min(5.0f,-percent*0.25f));char consequence[4096];sprintf_s(consequence,mercenarieLocalize(Loc::text("ui.expected_reaction_s_consequences_d_cats_estimated_reputation_1f")).c_str(),mercenarieLocalize(percent<=0?Loc::text("v8.mood.0"):percent<=10?Loc::text("v8.mood.1"):percent<=25?Loc::text("v8.mood.2"):Loc::text("v8.mood.3")).c_str(),currentContract.totalPay,repEffect);negotiationReaction(consequence);}
    }

    void bonusClicked(MyGUI::WidgetPtr sender)
    {
        const int flags[]={EB_HEALTHY,EB_HALF_NOW,EB_FAST,EB_NO_KO,EB_DANGER,EB_SUPPLIES};
        if(sender==bonusButtons[1]){if(negotiatedAdvancePercent==0)negotiatedAdvancePercent=10;else if(negotiatedAdvancePercent==10)negotiatedAdvancePercent=25;else if(negotiatedAdvancePercent==25)negotiatedAdvancePercent=50;else negotiatedAdvancePercent=0;if(negotiatedAdvancePercent)currentContract.selectedBonuses|=EB_HALF_NOW;else currentContract.selectedBonuses&=~EB_HALF_NOW;char advanceLabel[80];sprintf_s(advanceLabel,mercenarieLocalize(Loc::text("ui.advance_d")).c_str(),negotiatedAdvancePercent);MercenarieFonts::caption(bonusButtons[1],advanceLabel);bonusButtons[1]->setStateSelected(negotiatedAdvancePercent>0);updateNegotiationPrice(priceSlider?priceSlider->getScrollPosition():20);return;}
        for(int i=0;i<6;++i)if(sender==bonusButtons[i]){bool selected=!bonusButtons[i]->getStateSelected();bonusButtons[i]->setStateSelected(selected);if(selected)currentContract.selectedBonuses|=flags[i];else currentContract.selectedBonuses&=~flags[i];break;}
        updateNegotiationPrice(priceSlider?priceSlider->getScrollPosition():20);
    }

    void negotiationSliderChanged(MyGUI::ScrollBar*, size_t position)
    {
        updateNegotiationPrice(position);
    }

    void negotiationMinusClicked(MyGUI::WidgetPtr)
    {
        if(!priceSlider) return;
        size_t position=priceSlider->getScrollPosition();
        position=position<5?0:position-5;
        priceSlider->setScrollPosition(position);
        updateNegotiationPrice(position);
    }

    void negotiationPlusClicked(MyGUI::WidgetPtr)
    {
        if(!priceSlider) return;
        size_t position=priceSlider->getScrollPosition();
        position=std::min<size_t>(45,position+5);
        priceSlider->setScrollPosition(position);
        updateNegotiationPrice(position);
    }

    void negotiationSliderHovered(MyGUI::WidgetPtr,MyGUI::WidgetPtr)
    {
        if(negotiationSliderPanel)negotiationSliderPanel->setColour(MyGUI::Colour(1.0f,0.82f,0.46f));
        if(negotiationSliderTrack)negotiationSliderTrack->setColour(MyGUI::Colour(1.0f,0.62f,0.08f));
        if(priceSlider)priceSlider->setColour(MyGUI::Colour(1.0f,0.92f,0.32f));
        if(negotiationSliderTitle)negotiationSliderTitle->setTextColour(MyGUI::Colour(1.0f,0.94f,0.48f));
    }

    void negotiationSliderUnhovered(MyGUI::WidgetPtr,MyGUI::WidgetPtr)
    {
        if(negotiationSliderPanel)negotiationSliderPanel->setColour(MyGUI::Colour::White);
        if(negotiationSliderTrack){int percent=priceSlider?static_cast<int>(priceSlider->getScrollPosition())-20:0;if(percent<=10)negotiationSliderTrack->setColour(MyGUI::Colour(0.32f,0.72f,0.28f));else if(percent<=25)negotiationSliderTrack->setColour(MyGUI::Colour(0.92f,0.58f,0.08f));else negotiationSliderTrack->setColour(MyGUI::Colour(0.82f,0.22f,0.12f));}
        if(priceSlider)priceSlider->setColour(MyGUI::Colour(1.0f,0.78f,0.18f));
        if(negotiationSliderTitle)negotiationSliderTitle->setTextColour(MyGUI::Colour(1.0f,0.72f,0.20f));
    }

    void acceptNegotiatedContract(int agreedReward,bool goodwill){if(!requestPersonnelSelection(agreedReward,goodwill))acceptNegotiatedContractFinal(agreedReward,goodwill);}
    void acceptNegotiatedContractFinal(int agreedReward, bool goodwill)
    {
        int agreedPercent=baseReward?static_cast<int>((agreedReward-baseReward)*100/baseReward):0;
        ContractRewards::freeze(currentContract.routeRegions,contractRewardPercent);
        calculateEstimatedContract(currentContract,agreedPercent);
        applyContractRewardSnapshot(currentContract);
        missionReward=currentContract.totalPay;
        currentContract.advance=currentContract.totalPay*negotiatedAdvancePercent/100;
        currentContract.finalPay=currentContract.totalPay-currentContract.advance;
        healthBonus = missionReward / 4;
        advancePaid = currentContract.advance;
        if(!currentContract.advancePaidOnce&&advancePaid>0){payrollContractReward("advance:"+currentMissionFiscalId,advancePaid,"finance.advance");currentContract.advancePaidOnce=true;}
        if (goodwill)
        {
            CityMemory& memory = cityMemories[ReputationIdentity::key(originCity)];
            if (memory.abuses > 0) --memory.abuses;
            if (escort) escort->sayALine(Loc::text("ui.a_fair_price_i_will_remember_it"), true);
        }
        closeNegotiation(true);
        char agreement[180];sprintf_s(agreement,mercenarieLocalize(Loc::text("ui.agreement_reached_advance_of_d_d_cats_the_balance")).c_str(),negotiatedAdvancePercent,advancePaid);ou->showPlayerAMessage(agreement, true);
        beginJourney();
    }

    void layoutNegotiationButtons(bool showCounter);

    float negotiationChance(int percent)
    {
        float chance = 1.0f;
        if (percent > 0 && percent <= 10) chance = 0.90f;
        else if (percent <= 25) chance = 0.70f - (percent - 10) * 0.022f;
        else if (percent > 25) chance = 0.22f - (percent - 25) * 0.006f;
        chance += personalityAcceptance;
        chance += GuildProgression::chanceBonus(localReputation(),escortReputation);
        if (rareContract) chance += 0.10f;
        if (chance < 0.03f) chance = 0.03f;
        if (chance > 0.97f) chance = 0.97f;
        return chance;
    }

    void proposePriceClicked(MyGUI::WidgetPtr)
    {
        if (!negotiationOpen) return;
        if(counterOfferActive){++negotiationInsistence;if(negotiationInsistence>2){if(escort)escort->sayALine(Loc::text("ui.i_have_already_made_two_efforts_the_discussion_is"),true);suspendNegotiation();return;}}
        int percent = baseReward ? ((proposedReward - baseReward) * 100 / baseReward) : 0;
        bool withinBudget = proposedReward <= static_cast<int>(baseReward * clientBudgetMultiplier);
        int bonusCount=0;for(int mask=currentContract.selectedBonuses;mask;mask>>=1)bonusCount+=mask&1;
        float chance = std::max(.03f,std::min(.97f,EscortEconomy::acceptance(percent,bonusCount,0,0,currentContract.wealth,currentContract.personality,currentContract.danger,currentContract.prestigious)+GuildProgression::chanceBonus(localReputation(),escortReputation)));
        if (counterOfferActive) chance *= 0.45f;
        if (percent <= 0 || (withinBudget && UtilityT::random(0.0f, 1.0f) <= chance))
        {
            const char* accepted[] = {
                Loc::text("ui.deal_concluded_you_know_how_to_negotiate"), Loc::text("ui.agreed_but_you_will_have_to_earn_that_amount"),
                Loc::text("ui.i_accept_do_not_make_me_regret_this_concession")
            };
            if (escort) escort->sayALine(accepted[UtilityT::randomInt(0, 2)], true);
            acceptNegotiatedContract(proposedReward, percent <= 0);
            return;
        }

        CityMemory& memory = cityMemories[ReputationIdentity::key(originCity)];
        if (percent >= 25) { ++memory.abuses; memory.recovery = 3; }
        counterOffer = std::min(static_cast<int>(baseReward * clientBudgetMultiplier),
            baseReward + (proposedReward - baseReward) / 2);
        counterOfferActive = true;
        char text[4096];
        sprintf_s(text, mercenarieLocalize(Loc::text("ui.d_cats_i_can_offer_d_cats_attempts_used")).c_str(), proposedReward, counterOffer,negotiationInsistence,negotiationInsistence>=2?mercenarieLocalize(Loc::text("ui.further_insistence_will_void_the_contract")).c_str():"");
        if (reactionText) negotiationReaction(text);
        if (counterButton) { MercenarieFonts::caption(counterButton,Loc::text("ui.accept_the_counteroffer")); layoutNegotiationButtons(true); }
        if (proposeButton) { MercenarieFonts::caption(proposeButton,Loc::text("ui.insist_risk_cancellation")); negotiationFitButton(proposeButton); }
    }

    void acceptCounterClicked(MyGUI::WidgetPtr)
    {
        if (counterOfferActive) acceptNegotiatedContract(counterOffer, false);
    }

    void refuseNegotiationClicked(MyGUI::WidgetPtr)
    {
        if(!missionPending||missionActive||!escort)return;
        std::vector<Character*> departing=scientificMission?scientificMembers:caravanMembers;
        departing.push_back(escort);
        std::vector<RefusedDeparture> queued;
        Town* town=contractOriginTown?contractOriginTown->isTown():0;
        Ogre::Vector3 exit=town?town->getPositionOutsideTownGates(1200.0f):escort->getPosition()+Ogre::Vector3(1200,0,1200);
        for(size_t i=0;i<departing.size();++i){Character* c=departing[i];if(!c||c->isDead())continue;
            bool duplicate=false;for(size_t j=0;j<queued.size();++j)if(queued[j].actor.getCharacter()==c)duplicate=true;
            if(duplicate)continue;
            RefusedDeparture d;d.actor=c->getHandle();d.rendezvous=c->getPosition();d.target=exit;d.retry=0;queued.push_back(d);
        }
        if (escort) escort->sayALine(Loc::text("ui.in_this_case_our_matter_ends_here"), true);
        closeNegotiation(true);
        cancelMission("ui.contract_refused_no_amount_was_paid");
        // Refusal has its own distance-based cleanup, never the generic 25-second timer.
        completedEscort=0;completedCaravan.clear();completedCleanupClock=0;
        for(size_t i=0;i<queued.size();++i){Character* c=queued[i].actor.getCharacter();if(!c)continue;
            c->removeJob(HOLD_POSITION);c->removeJob(FOLLOW_PLAYER_ORDER);c->removeJob(MOVE_CUS_ORDERED);
            c->addJob(MOVE_CUS_ORDERED,0,false,false,queued[i].target);
            refusedDepartures.push_back(queued[i]);
        }
    }

    void updateRefusedDepartures(float elapsed)
    {
        static std::map<hand,Ogre::Vector3> lastPositions;
        if(refusedDepartures.empty()){lastPositions.clear();return;}
        for(int i=static_cast<int>(refusedDepartures.size())-1;i>=0;--i){
            RefusedDeparture& d=refusedDepartures[i];d.resolutionRetry-=elapsed;if(d.resolutionRetry>0)continue;Character* c=SavedActorResolution::find(d.actor);
            if(!c){d.resolutionRetry=1;continue;} // Streaming is not evidence that this actor was deleted.
            if(c->isDead()||c->isBeingCarried()||c->isInCombatMode(true,true))continue;
            // Departure belongs to this actor, never to the current contract.
            if(c->getMedical()&&c->getMedical()->isUnconcious())continue;
            const TaskType blocked[]={HOLD_POSITION,FOLLOW_PLAYER_ORDER,FOLLOW_SQUADLEADER,FOLLOW_WHILE_TALKING,WANDERER,WANDER_TOWN};
            for(size_t j=0;j<sizeof(blocked)/sizeof(blocked[0]);++j)c->removeJob(blocked[j]);
            if(AI* ai=c->getAI()){hand none;none.setNull();ai->setCenterOfMovementTarget(none);ai->setManuveringFreedomLevel(AI::ROAM_FAR);}
            bool distant=!c->getMovement()->isIndoors();
            if(!ou||!ou->player)distant=false;
            else for(unsigned int p=0;p<ou->player->playerCharacters.size();++p){Character* player=ou->player->playerCharacters[p];if(player&&!player->isDead()&&player->getPosition().squaredDistance(c->getPosition())<1000000.0f)distant=false;}
            if(distant){Faction* owner=c->getFaction();if(owner){owner->destroyObject(c);refusedDepartures.erase(refusedDepartures.begin()+i);}continue;}
            d.retry+=elapsed;
            if(d.retry>=8.0f){d.retry=0;
                bool stuck=lastPositions.count(d.actor)&&lastPositions[d.actor].squaredDistance(c->getPosition())<1.0f;
                lastPositions[d.actor]=c->getPosition();
                const bool reached=c->getPosition().squaredDistance(d.target)<100.0f;
                if(stuck||reached||c->getMovement()->pathFailed()||!c->getMovement()->isCurrentlyMoving()){
                if(stuck){TownBase* base=c->getCurrentTownLocation();Town* town=base?base->isTown():0;if(town)d.target=town->getPositionOutsideTownGates(1200.0f);}
                if(reached){Ogre::Vector3 direction=c->getPosition()-d.rendezvous;direction.y=0;if(direction.squaredLength()<1.0f)direction=Ogre::Vector3(1,0,1);direction.normalise();d.target=c->getPosition()+direction*1200.0f;}
                Ogre::Vector3 waypoint=d.target;
                if(c->getMovement()->isIndoors())findExteriorWaypoint(c,waypoint);
                c->removeJob(HOLD_POSITION);c->removeJob(FOLLOW_SQUADLEADER);c->removeJob(WANDERER);c->removeJob(WANDER_TOWN);
                c->getMovement()->invalidatePath();c->removeJob(MOVE_CUS_ORDERED);c->addJob(MOVE_CUS_ORDERED,0,false,false,waypoint);
            }}
        }
    }

    void returnToContractsClicked(MyGUI::WidgetPtr)
    {
        negotiationSuspended=true; closeNegotiation(false);
        if(contractsWindow){updateContractsBoard();contractsWindow->setVisible(true);}
    }

    void layoutNegotiationButtons(bool showCounter)
    {
        if(!negotiationWindow||!proposeButton||!counterButton||!returnButton||!refuseButton)return;
        if(negotiationCounterPanel)negotiationCounterPanel->setVisible(showCounter);
        counterButton->setVisible(showCounter);
        negotiationFitButton(proposeButton);negotiationFitButton(returnButton);negotiationFitButton(refuseButton);negotiationFitButton(counterButton);
    }

    void negotiationWindowButtonPressed(MyGUI::Window*, const std::string&)
    {
        suspendNegotiation();
    }

    void basicContractAccepted(MyGUI::WidgetPtr)
    {
        if(!missionPending || guildLevel()>=2)return;
        currentContract.selectedBonuses=0;
        currentContract.negotiatedRate=currentContract.initialRate;
        currentContract.bonusPay=0;
        currentContract.totalPay=baseReward;
        currentContract.advance=0;
        currentContract.finalPay=baseReward;
        negotiatedAdvancePercent=0;
        acceptNegotiatedContract(baseReward,true);
    }

    void basicContractRefused(MyGUI::WidgetPtr)
    {
        refuseNegotiationClicked(0);
    }

    void basicContractWindowPressed(MyGUI::Window*,const std::string&)
    {
        suspendNegotiation();
    }

    #include "BasicContractView.h"

    void openBasicContractDecision()
    {
        resetPersonnelChoice();createBasicContractDecisionUI();
        if(!contractDecisionWindow){beginJourney();return;}
        negotiationWasPaused=ou->isPaused();ou->userPause(true);negotiationOpen=true;contractLifecycle=CONTRACT_NEGOTIATING;
        currentContract.selectedBonuses=0;currentContract.negotiatedRate=currentContract.initialRate;currentContract.bonusPay=0;currentContract.totalPay=baseReward;currentContract.advance=0;currentContract.finalPay=baseReward;negotiatedAdvancePercent=0;
        contractDecisionWindow->setVisible(true);
    }

    #include "NegotiationBuildView.h"

    void openNegotiation()
    {
        if (!missionPending || !escort || negotiationOpen) return;
        if(guildLevel()<2){openBasicContractDecision();return;}
        contractLifecycle = CONTRACT_NEGOTIATING;
        resetPersonnelChoice();createNegotiationUI();
        if (!negotiationWindow) { beginJourney(); return; }
        negotiationWasPaused = ou->isPaused(); ou->userPause(true);
        negotiationOpen = true; counterOfferActive = false;
        if(!negotiationSuspended){negotiationInsistence=0;currentContract.selectedBonuses=0;negotiatedAdvancePercent=0;for(int i=0;i<6;++i)if(bonusButtons[i])bonusButtons[i]->setStateSelected(false);if(bonusButtons[1])MercenarieFonts::caption(bonusButtons[1],Loc::text("ui.feed_0_max_50"));priceSlider->setScrollPosition(20);}
        if(priceSlider)priceSlider->setEnabled(true);if(sliderMinusButton)sliderMinusButton->setEnabled(true);if(sliderPlusButton)sliderPlusButton->setEnabled(true);if(negotiationSliderTrack)negotiationSliderTrack->setEnabled(true);for(int i=0;i<6;++i)if(bonusButtons[i])bonusButtons[i]->setEnabled(true);
        updateNegotiationPrice(priceSlider?priceSlider->getScrollPosition():20); negotiationSuspended=false;
        negotiationReaction(Loc::text("ui.move_the_slider_from_20_to_25_the_client"));
        MercenarieFonts::caption(proposeButton,Loc::text("negotiation.design.validate")); layoutNegotiationButtons(false);
        MercenarieFonts::caption(refuseButton,Loc::text("ui.refuse_the_contract")); negotiationFitButton(refuseButton); negotiationWindow->setVisible(true);
    }

    void startMission(Character* guildMaster, const std::string& choice, bool destinationAlreadyChosen)
    {
        if(!questCapacityAvailable())return;
        if (missionActive || missionPending)
        {
            ou->showPlayerAMessage(Loc::text("ui.an_escort_contract_is_already_in_progress"), true);
            return;
        }

        if (!guildMaster)
        {
            ou->showPlayerAMessage(Loc::text("ui.the_contract_giver_cannot_be_found"), true);
            return;
        }
        TownBase* origin = guildMaster->getCurrentTownLocation();
        originCity = origin ? frenchPlaceName(origin->getName()) : Loc::text("ui.unknown_city");
        if(!destinationAlreadyChosen){if(scientificMission)chooseRuinDestination(choice,guildMaster->getPosition());else chooseDestination(choice,guildMaster->getPosition());}
        if(scientificMission&&!ContractDestinationRules::scientific(destinationTown)){ErrorLog("V9 science: rejected invalid mission objective");return;}
        caravanOrigin=guildMaster->getPosition();caravanReturning=false;caravanMembers.clear();
        scientificOrigin=origin?origin->getPosition():guildMaster->getPosition();scientificMembers.clear();scientificResearching=false;scientificReturning=false;scientificEntryAttempted=false;scientificInsideDiscovery=false;scientificWillEnter=false;scientificEntryResolved=false;
        if(scientificMission)escortSquad="880121-Guild Escort Contracts.mod";
        else if(caravanMission)escortSquad="880080-Guild Escort Contracts.mod";
        else if(!selectedProfileSquad.empty())escortSquad=selectedProfileSquad.c_str();
        else if(choice=="tier_near")escortSquad="880010-Guild Escort Contracts.mod";
        else if(choice=="tier_medium")escortSquad="880030-Guild Escort Contracts.mod";
        else escortSquad="880040-Guild Escort Contracts.mod";

        float prestigeChance = 5.0f;
        rareContract = selectedProfileRarity>0||(UtilityT::random(0.0f, 100.0f) < prestigeChance&&selectedProfileSquad.empty());

        GameData* squadData = ou->gamedata.getData(escortSquad, SQUAD_TEMPLATE);
        GameData* questFactionData=ou->gamedata.getData(questEscortFactionId(),FACTION);
        Faction* faction = questFactionData?ou->factionMgr->getOrCreateFaction(questFactionData):0;
        Town* town = shou->townList->getTownBySID(destinationTown);
        if (!squadData || !faction || !town || !guildMaster)
        {
            ErrorLog("Guild Escort: mission data unavailable");
            ou->showPlayerAMessage(Loc::text("ui.the_contract_cannot_start_missing_data"), true);
            return;
        }

        contractOriginFaction = guildMaster->getFaction();
        copyOriginEnemies(faction, contractOriginFaction);

        Ogre::Vector3 spawn = guildMaster->getPosition();
        Ogre::Vector3 outsideSpawn;
        if (findExteriorWaypoint(guildMaster, outsideSpawn))
            spawn = outsideSpawn;
        else
        {
            // Secours pour un donneur situe dehors ou dans un batiment sans porte lisible.
            spawn.x += 4.0f;
            spawn.z += 4.0f;
        }
        BoardOffer* acceptedOffer=destinationAlreadyChosen?&boardOffers[selectedOffer]:0;
        ContractGroupPlan::Plan plannedGroup;std::vector<ContractGroupPlan::Binding> groupBindings;
        const bool hasGroupPlan=acceptedOffer&&acceptedOffer->routeRegions.find(";GROUP1=")!=std::string::npos;
        if(caravanMission&&hasGroupPlan&&ContractGroupPlan::decode(acceptedOffer->routeRegions,plannedGroup)){
            ContractGroupPlan::limitCaravan(plannedGroup);
            acceptedOffer->groupSize=plannedGroup.people();acceptedOffer->caravanSize=EscortEconomy::caravanSizeFor(plannedGroup.people()+plannedGroup.animals());
            size_t begin=acceptedOffer->routeRegions.find(";GROUP1="),end=acceptedOffer->routeRegions.find(';',begin+8);
            acceptedOffer->routeRegions.replace(begin,end-begin+1,ContractGroupPlan::encode(plannedGroup));
        }
        if(hasGroupPlan&&(!ContractGroupPlan::decode(acceptedOffer->routeRegions,plannedGroup)||!bindOfferGroup(squadData,plannedGroup,groupBindings,caravanMission))){
            ErrorLog("Guild Escort: planned group no longer matches template; offer not spawned");
            ou->showPlayerAMessage(Loc::text("ui.this_group_s_template_has_changed_wait_for_new"),true);return;
        }
        Platoon* platoon=0;
        {ContractGroupPlan::ScopedCounts fixedCounts(groupBindings);
            // Covers legacy/direct starts too; shared template is restored after spawning.
            std::vector<ContractGroupPlan::Binding> caravanBindings;
            if(caravanMission)for(int role=0;role<2;++role){const char* list=role?"animals":"squad";int left=role?2:4;Ogre::vector<GameDataReference>::type* refs=squadData->getReferenceListIfExists(list)?squadData->_getReferenceList_nonConst(list):0;
                if(refs)for(size_t i=0;i<refs->size();++i){int* values=(*refs)[i].values.value;
                    int high=std::max(0,std::min(left,values[1])),low=std::max(0,std::min(high,values[0]));
                    int count=low==high?low:UtilityT::randomInt(low,high);caravanBindings.push_back(ContractGroupPlan::Binding(values,count));left-=count;}}
            ContractGroupPlan::ScopedCounts cappedCaravan(caravanBindings);
            platoon = ou->theFactory->createRandomSquad(
                faction, spawn, 0, 1, 0, squadData, 0, 0, 0,
                true, hand(), town, 1.0f, SQ_ROAMING, false);
        }

        if (!platoon || !platoon->activePlatoon || platoon->activePlatoon->things.size() == 0)
        {
            ErrorLog("Guild Escort: spawned squad is empty");
            ou->showPlayerAMessage(Loc::text("ui.the_traveler_could_not_appear"), true);
            return;
        }

        // Apply the client profile once at creation; saved characters keep their progression.
        for(unsigned int i=0;i<platoon->activePlatoon->things.size();++i){
            Character* client=static_cast<Character*>(platoon->activePlatoon->things[i]);
            giveMissionSpawnKits(client);
            if(!client||client->isAnimal())continue;
            CharStats* stats=client->getStats();if(!stats)continue;
            const float value=UtilityT::random(25.0f,35.0f);
            stats->_strength=value;stats->_dexterity=value;
            stats->_toughness=UtilityT::random(45.0f,55.0f);
            stats->__meleeAttack=value;stats->_meleeDefence=value;stats->dodging=value;
            stats->katanas=value;stats->sabres=value;stats->hackers=value;stats->blunt=value;
            stats->heavyWeapons=value;stats->polearms=value;stats->unarmed=value;
            stats->perception=value;stats->bows=value;stats->turrets=value;
            stats->_athletics=value;stats->medic=value;
        }
        escort = static_cast<Character*>(platoon->activePlatoon->things[0]);
        escortHandle = escort;
        platoon->activePlatoon->setSquadLeader(escort);
        // Native roaming squads may otherwise be culled when their zone unloads.
        // The normal mission cleanup still clears persistence by destroying the
        // group only after completion/failure/cancellation.
        platoon->setPersistentSquad(true);
        if(caravanMission)for(unsigned int i=0;i<platoon->activePlatoon->things.size();++i)caravanMembers.push_back(static_cast<Character*>(platoon->activePlatoon->things[i]));
        if(scientificMission)for(unsigned int i=0;i<platoon->activePlatoon->things.size();++i){Character* scientist=static_cast<Character*>(platoon->activePlatoon->things[i]);scientificMembers.push_back(scientist);GameData* tools=ou->gamedata.getData("43404-changes_otto.mod",ITEM);if(tools&&scientist){Item* item=ou->theFactory->createItem(tools,hand(),0,0,0,0);if(item)scientist->giveItem(item,true,true);}}
        int personality = UtilityT::randomInt(0, 3);
        missionGroup.reset();finalBonusChoice.reset();missionPace=EscortPace::Normal;missionForcedPace=false;missionCasualtyWaiting=false;missionTemporaryLeader=0;clearMissionRescue();missionSeparation=EscortMissionRules::Separation();carriedDestinationDistance=-1;currentContract=EscortContractData();journeyData=EscortJourneyData();currentMissionFiscalId=makeMissionFiscalId();progressMembers.clear();progressDeadMembers.clear();progressAnyClientKo=false;progressRosterKnown=true;progressReportLocal=progressReportGlobal=0;for(unsigned int memberIndex=0;memberIndex<platoon->activePlatoon->things.size();++memberIndex)progressMembers.push_back(static_cast<Character*>(platoon->activePlatoon->things[memberIndex])->getHandle());
        if(hasGroupPlan){char groupLog[192];sprintf_s(groupLog,"Guild Escort group plan: people=%d animals=%d spawned=%u",plannedGroup.people(),plannedGroup.animals(),static_cast<unsigned int>(platoon->activePlatoon->things.size()));DebugLog(groupLog);
            if(platoon->activePlatoon->things.size()!=plannedGroup.people()+plannedGroup.animals()){ErrorLog("Guild Escort: engine changed planned group count");ou->showPlayerAMessage(Loc::text("ui.group_test_the_engine_created_a_different_count_please"),true);}
        }
        currentContract.origin=originCity;currentContract.destination=destinationName;
        currentContract.distanceKm=std::max(1.0f,selectedDistance/1000.0f)*((caravanMission||scientificMission)?2.0f:1.0f);
        currentContract.type=scientificMission?MCT_SCIENCE:caravanMission?MCT_CARAVAN:MCT_ESCORT;
        currentContract.source=acceptedOffer?(MercContractSource)acceptedOffer->source:(isGuildVisitor(guildMaster)?MCS_GUILD_HOUSE:MCS_TAVERN);
        currentContract.rarity=acceptedOffer?(MercContractRarity)acceptedOffer->rarity:MCR_COMMON;
        currentContract.dangerLevel=acceptedOffer?acceptedOffer->dangerLevel:(rareContract?5:choice=="tier_long"?4:choice=="tier_medium"?3:2);
        currentContract.danger=EscortEconomy::dangerMultiplier(currentContract.dangerLevel);
        currentContract.environmentTags=acceptedOffer?acceptedOffer->environmentTags:0;
        currentContract.routeRegions=acceptedOffer?acceptedOffer->routeRegions:"";
        currentContract.originId=contractOriginTown&&contractOriginTown->getGameData()?contractOriginTown->getGameData()->stringID:"";
        currentContract.destinationId=destinationTown;
        currentContract.wealth=selectedProfileName=="Noble"?ECW_NOBLE:selectedProfileName=="Marchand"?ECW_MERCHANT:(EscortClientWealth)UtilityT::randomInt(0,3);
        currentContract.personality=(EscortClientPersonality)personality;
        currentContract.prestigious=rareContract;
        currentContract.urgent=UtilityT::random(0.0f,100.0f)<12.0f;
        currentContract.groupSize=scientificMission?static_cast<int>(scientificMembers.size()):caravanMission?static_cast<int>(caravanMembers.size()):std::max(1,selectedProfileGroupSize);
        if(caravanMission){const char* cargoTypes[]={Loc::text("ui.basic_cargo"),Loc::text("ui.joint_cargo"),Loc::text("ui.large_cargo"),Loc::text("ui.valuable_cargo"),Loc::text("ui.exceptional_cargo")};int cargo=acceptedOffer?acceptedOffer->cargoClass:UtilityT::randomInt(0,4);caravanCargoType=cargoTypes[cargo];currentContract.cargoClass=(MercCargoClass)cargo;currentContract.caravanSize=acceptedOffer?(MercCaravanSize)acceptedOffer->caravanSize:EscortEconomy::caravanSizeFor(currentContract.groupSize);caravanCargoValue=EscortEconomy::cargoBonus(currentContract.cargoClass);currentContract.cargoValue=caravanCargoValue;caravanInitialMembers=static_cast<int>(caravanMembers.size());}
        if(scientificMission){currentContract.studyClass=acceptedOffer?(MercStudyClass)acceptedOffer->studyClass:(MercStudyClass)std::min(3,missionTierIndex);currentContract.studyDuration=acceptedOffer?(MercStudyDuration)acceptedOffer->studyDuration:(MercStudyDuration)std::min(3,missionTierIndex);}
        if (personality == 0) { clientBudgetMultiplier = 1.50f; personalityAcceptance = 0.10f; }
        else if (personality == 1) { clientBudgetMultiplier = 1.30f; personalityAcceptance = 0.03f; }
        else if (personality == 2) { clientBudgetMultiplier = 1.18f; personalityAcceptance = -0.02f; }
        else { clientBudgetMultiplier = 1.10f; personalityAcceptance = -0.08f; }
        // Reputation affects trust, not budget.
        CityMemory& localMemory = cityMemories[ReputationIdentity::key(originCity)];
        if (localMemory.recovery > 0) --localMemory.recovery;
        else if (localMemory.abuses > 0) --localMemory.abuses;
        currentContract.urgent=visitorOfferUrgent||currentContract.urgent;
        if(acceptedOffer&&acceptedOffer->routeRegions.find("V6EST:")==0){currentContract.basePay=acceptedOffer->estimatedPay;currentContract.distanceKm=std::max(1.0f,acceptedOffer->distance/1000.0f)*((caravanMission||scientificMission)?2.0f:1.0f);}
        ContractFactors::Quote acceptedFactors;if(caravanMission&&ContractFactors::decode(currentContract.routeRegions,acceptedFactors)){
            provisionContractCargo(acceptedFactors);int loaded=std::min(acceptedFactors.count,countContractCargo(caravanMembers,acceptedFactors.item));
            GameData* cargo=ou->gamedata.getData(acceptedFactors.item,ITEM);caravanCargoType=cargo?cargo->name:acceptedFactors.item;caravanCargoValue=loaded*acceptedFactors.unitValue;currentContract.cargoValue=caravanCargoValue;
            if(loaded<acceptedFactors.count){
                acceptedFactors.count=loaded;acceptedFactors.cargoPercent=0;if(!loaded){acceptedFactors.item="-";acceptedFactors.unitValue=0;}
                size_t at=currentContract.routeRegions.find(";FACT1="),end=currentContract.routeRegions.find(';',at+7);currentContract.routeRegions.replace(at,end-at+1,ContractFactors::encode(acceptedFactors));
                at=currentContract.routeRegions.find(";PRICE=");if(at!=std::string::npos){end=currentContract.routeRegions.find(';',at+7);currentContract.routeRegions.erase(at,end-at+1);}currentContract.basePay=0;
            }
        }
        calculateEstimatedContract(currentContract,0);missionReward=currentContract.totalPay;baseReward=currentContract.basePay;
        healthBonus = baseReward / 4;
        advancePaid = 0;
        if (findExteriorWaypoint(guildMaster, outsideSpawn))
            escort->getMovement()->_setPositionAndTeleport(outsideSpawn, 0);
        // A town's centre is frequently behind a gate that a neutral travelling
        // squad cannot enter.  The contract is fulfilled at Kenshi's own outside-
        // gate waypoint; scientific sites keep their exact ruin position.
        destination = scientificMission?town->getPosition():town->getPositionOutsideTownGates(18.0f);
        scientificRuinCenter=destination;
        recentDestinations[destinationName]=3;
        for(std::map<std::string,int>::iterator it=recentDestinations.begin();it!=recentDestinations.end();++it) if(it->first!=destinationName && it->second>0)--it->second;
        clearCurrentPersonnel();
        missionActive = false;
        missionPending = true;
        contractLifecycle = CONTRACT_CLIENT_MEETING;
        waitingForPlayer = false;
        leavingBuilding = false;
        updateClock = 0.0f;
        stationaryClock = 0.0f;
        missionPaused = false;
        midpointChecked = false;
        quarterSpeech = false;
        finalSpeech = false;
        wasInCombat = false;
        unconsciousSeconds = 0.0f;
        carriedByPlayerSeconds = 0.0f;
        escort->getMovement()->halt();
        // The meeting/negotiation is not an active journey. Anchor the whole party
        // after its final spawn placement, until beginJourney explicitly releases it.
        clearMissionFollow();releaseWaitingHere();rememberWaitingHere(escort);
        const std::vector<Character*>& pendingMembers=scientificMission?scientificMembers:caravanMembers;
        for(size_t i=0;i<pendingMembers.size();++i)if(pendingMembers[i]!=escort)rememberWaitingHere(pendingMembers[i]);
        enforceWaitingHere();
        updateTrackerUI();
        std::string message = scientificMission?Loc::text("ui.the_scientific_team_is_waiting_for_you_in_front"):caravanMission?Loc::text("ui.the_caravan_awaits_you_in_front_of_the_building"):Loc::text("ui.the_traveler_is_waiting_for_you_in_front_of");
        message += destinationName;
        message += Loc::text("ui.payment_8827682");
        char priceText[4096]; sprintf_s(priceText, mercenarieLocalize(Loc::text("ui.d_cats_before_negotiation")).c_str(), baseReward);
        message += priceText;
        message += Loc::text("ui.he_will_wait_for_you_if_you_are_too");
        if (rareContract) message += Loc::text("ui.rare_contract_double_danger_and_reward");
        ou->showPlayerAMessage(message, true);
        DebugLog("Guild Escort: test contract started");
    }

    void beginJourney()
    {
        if (!missionPending || !escort || escort->isDead()) return;
        releaseWaitingHere();
        missionPending = false;
        missionActive = true;
        contractLifecycle = CONTRACT_ACTIVE;
        waitingForPlayer = false;
        missionPaused = false;
        journeyStart = escort->getPosition();
        lastJourneyPosition=journeyStart;
        journeyDistanceSquared = journeyStart.squaredDistance(destination);
        missionElapsed = 0.0f;
        reportStartHour=currentGameHours;reportEndHour=-1;
        expectedTravelTime = std::max(600.0f, selectedDistance / 4.0f)*((caravanMission||scientificMission)?2.0f:1.0f)+(scientificMission?300.0f:0.0f);
        escortWasKnockedOut = false;
        journeyCombatCount = 0;
        proximitySeconds=journeySeconds=0.0f;travelIncidentTriggered=false;lastImportantAlert.clear();importantAlertClock=0.0f;
        leavingBuilding = escort->getMovement()->isIndoors();
        if(leavingBuilding&&!findExteriorWaypoint(escort,exitWaypoint)){missionSuspendMotion();return;}
        issueTravelOrder(leavingBuilding ? exitWaypoint : destination);
        {std::ostringstream route;route<<"MISSION journey started source="<<(currentContract.source==MCS_GUILD_HOUSE?"guild-house":"tavern")<<" active="<<missionActive<<" pending="<<missionPending<<" leaving="<<leavingBuilding<<" target="<<destination.x<<","<<destination.z;DebugLog(route.str());}
        if (leavingBuilding)
            ou->showPlayerAMessage(Loc::text("ui.the_escort_begins_the_traveler_first_reaches_the_exit"), true);
        else
            ou->showPlayerAMessage(Loc::text("ui.the_escort_begins_stay_close_to_the_traveler"), true);
        DebugLog("Guild Escort: journey command accepted");
        std::ostringstream rewardLog;rewardLog<<"Contract accepted: id="<<currentMissionFiscalId<<" rewardPercent="<<ContractRewards::snapshot(currentContract.routeRegions)<<" finalCats="<<missionReward;DebugLog(rewardLog.str());seedEscort();saveReputations();
    }

    MyGUI::ScrollView* developerStatusScroll=0;
    void updateDeveloperStatus()
    {
        if(!developerStatus)return;std::stringstream s;
        s<<Loc::text("ui.level")<<guildLevel()<<Loc::text("ui.xp_ecb654b")<<guildPoints<<Loc::text("ui.local_reputation")<<localReputation()
         <<Loc::text("ui.clients")<<guildVisitors.size()<<Loc::text("ui.contract")<<(missionActive?Loc::text("common.actif"):missionPending?Loc::text("ui.negotiating"):Loc::text("ui.none"))
         <<Loc::text("ui.home")<<(guildBuilding?currentGuildHouseName:Loc::text("ui.not_detected"))<<Loc::text("ui.welcome")<<(guildClientSystemActive?Loc::text("common.actif"):Loc::text("common.inactif"))
         <<Loc::text("ui.furniture_simulation")<<(developerFurnitureOverride<0?Loc::text("common.normale"):developerFurnitureOverride==0?Loc::text("developer.locked"):Loc::text("ui.search_completed"))
         <<Loc::text("ui.mercenary_time")<<static_cast<int>(developerTimeOffsetHours)<<Loc::text("ui.h");
        s<<Loc::text("ui.tax_uc")<<fiscalLedger.debt(FISCAL_UC)<<Loc::text("ui.cats_0b5f3f7")<<fiscalStateName(fiscalLedger.organisations[0].state)<<Loc::text("ui.guild")<<fiscalLedger.debt(FISCAL_MERCENARY_GUILD)<<Loc::text("ui.cats_0b5f3f7")<<fiscalStateName(fiscalLedger.organisations[1].state)<<"]";
        MercenarieFonts::caption(developerStatus,wrapRegisterCaption(developerStatus,s.str(),developerStatus->getFontHeight()));
        if(developerStatusScroll){int height=std::max(60,developerStatus->getTextSize().height+4);developerStatus->setSize(developerStatus->getWidth(),height);developerStatusScroll->setCanvasSize(developerStatus->getWidth(),height);developerStatusScroll->setVisibleVScroll(height>developerStatusScroll->getHeight());}
    }

    void setDeveloperGuildLevel(int level)
    {
        int oldLevel=guildLevel();level=std::max(0,std::min(10,level));guildPoints=guildThresholdForLevel(level);saveReputations();appliedGuildUnlockLevel=-1;updateGuildResearchAccess();scanGuildFurniture();updateDeveloperStatus();if(level>oldLevel)showGuildLevelUpWindow(oldLevel,level);
    }

    void unlockAllMercenarieResearch()
    {
        if(!ou||!ou->player||!ou->player->technology)return;
        lektor<GameData*> researches;ou->gamedata.getDataOfType(researches,RESEARCH);int unlocked=0;
        for(unsigned int i=0;i<researches.size();++i)
        {
            GameData* research=researches[i];if(!research)continue;
            const bool mercenarieRecord=research->stringID.find("-Guild Escort Contracts.mod")!=std::string::npos;
            const bool transferredGuildRecord=research->stringID.find("-Holy Nation Mercenary Plastron.mod")!=std::string::npos;
            if(!mercenarieRecord&&!transferredGuildRecord)continue;
            if(ou->player->technology->finished.find(research)!=ou->player->technology->finished.end())continue;
            if(ou->player->technology->finished.insert(research).second)++unlocked;
            const char* unlockLists[]={"enable buildings","enable armour","enable weapons","enable weapon models","enable items","enable crafting"};
            for(unsigned int listIndex=0;listIndex<sizeof(unlockLists)/sizeof(unlockLists[0]);++listIndex)
            {
                const Ogre::vector<GameDataReference>::type* refs=research->getReferenceListIfExists(unlockLists[listIndex]);
                if(!refs)continue;
                for(unsigned int refIndex=0;refIndex<refs->size();++refIndex)if((*refs)[refIndex].ptr)ou->player->technology->enabledObjects.insert((*refs)[refIndex].ptr);
            }
        }
        ou->player->technology->changedSoUpdateGUI=true;
        developerFurnitureOverride=-1;scanGuildFurniture();
        std::stringstream message;message<<Loc::text("ui.the_mercenarie_research_unlocked")<<unlocked<<Loc::text("ui.news_discoveries_are_recorded_with_the_backup");
        ou->showPlayerAMessage(message.str(),true);
    }

    Character* developerContractGiver()
    {
        if(resolveContractBarman())return resolveContractBarman();if(!guildVisitors.empty())return guildVisitors[0].leader;return 0;
    }

    void prepareRoadTestContract(MyGUI::Widget*)
    {
        if(!routeTestEnabled()||!ou)return;
        if(missionActive||missionPending){ou->showPlayerAMessage(Loc::text("ui.an_escort_already_exists_finish_it_before_creating_a"),true);return;}
        Character* giver=resolveContractBarman();
        TownBase* origin=giver?giver->getCurrentTownLocation():0;
        Town* target=shou&&shou->townList?shou->townList->getTownBySID("12628-Newwworld.mod"):0;
        if(!origin||!origin->getGameData()||origin->getGameData()->stringID!="18919-Newwworld.mod"||!target){ou->showPlayerAMessage(Loc::text("ui.talk_to_a_barman_in_the_hub_first_test"),true);return;}
        if(selectedOffer<0||selectedOffer>=6)return;
        // Supply deterministic metadata without replacing or saving a board offer.
        BoardOffer previous=boardOffers[selectedOffer];
        BoardOffer test;test.townId="12628-Newwworld.mod";test.townName=frenchPlaceName(target->getName());test.dangerLevel=2;
        boardOffers[selectedOffer]=test;
        caravanMission=false;scientificMission=false;selectedProfileRarity=0;selectedProfileGroupSize=1;selectedProfileName=Loc::text("ui.civilian");selectedProfileSquad="880101-Guild Escort Contracts.mod";missionTierIndex=0;
        destinationTown="12628-Newwworld.mod";destinationNameStorage=test.townName;destinationName=destinationNameStorage.c_str();selectedDistance=origin->getPosition().distance(target->getPosition());
        startMission(giver,"tier_near",true);
        boardOffers[selectedOffer]=previous;
        if(missionPending){if(contractsWindow)contractsWindow->setVisible(false);if(mercenarieLauncherMenu)mercenarieLauncherMenu->setVisible(false);}
    }

    void developerStartContract(int type)
    {
        Character* giver=developerContractGiver();if(!giver){if(ou)ou->showPlayerAMessage(Loc::text("ui.impossible_test_first_talk_to_a_bartender_or_bring"),true);return;}
        if(missionActive||missionPending){if(ou)ou->showPlayerAMessage(Loc::text("ui.a_contract_is_already_present_clean_it_before_this"),true);return;}
        caravanMission=type==1;scientificMission=type==2;selectedProfileRarity=0;selectedProfileGroupSize=type==0?1:type==1?5:2;selectedProfileName=type==0?Loc::text("ui.civilian"):type==1?Loc::text("ui.test_caravan"):Loc::text("ui.scientific_test_team");selectedProfileSquad=type==0?"880101-Guild Escort Contracts.mod":type==1?"880080-Guild Escort Contracts.mod":"880121-Guild Escort Contracts.mod";
        if(scientificMission)chooseRuinDestination("tier_near",giver->getPosition());else chooseDestination("tier_near",giver->getPosition());startMission(giver,"tier_near",true);updateDeveloperStatus();
    }

#include "MissionPlatoonPrototype.h"
#include "MissionAbsencePrototype.h"

    bool delegatedAbsenceActive(){return MissionAbsencePrototype::state.active||MissionAbsencePrototype::persistent.active;}
    bool delegatedCharacterAbsent(Character* character){return MissionAbsencePrototype::isAbsent(character);}

    void delegationRosterConfirm(MyGUI::WidgetPtr)
    {
        if(MissionAbsencePrototype::persistent.active)return;
        if(!GuildLevel3Access::available(guildLevel())){if(ou)ou->showPlayerAMessage(Loc::text("options.delegation.level3"),true);return;}
        if(delegationConfirming||!ou||!ou->player)return;
        std::vector<Character*> chosen;int currentlyAvailable=0;for(size_t i=0;i<delegationRosterCharacters.size();++i){Character* c=delegationRosterCharacters[i];bool valid=c&&!c->isDead()&&!delegatedCharacterAbsent(c);if(valid)++currentlyAvailable;if(delegationRosterSelected[i]&&valid)chosen.push_back(c);}
        if(chosen.empty()||chosen.size()>30||chosen.size()>=static_cast<size_t>(currentlyAvailable)){ou->showPlayerAMessage(Loc::text("v8.literal.059"),true);return;}
        if(clientOptions.confirmDelegate&&!delegationFinalArmed){delegationFinalArmed=true;if(missionBookDelegationContext&&missionBookDelegateButton)MercenarieFonts::caption(missionBookDelegateButton,Loc::text("options.confirm"));else if(delegationConfirmButton)MercenarieFonts::caption(delegationConfirmButton,Loc::text("options.confirm"));return;}delegationFinalArmed=false;
        delegationConfirming=true;refreshDelegationRosterCards();
        DelegatedMissionState pending;pending.kind=delegationOfferKind;pending.offerIdentity=delegationOfferIdentity;pending.issuerIdentity=delegationIssuerIdentity;pending.boardKey=delegationBoardKey;{std::ostringstream group;group<<"delegated-"<<(unsigned long long)(currentGameHours*3600.0)<<"-"<<UtilityT::randomInt(100000,999999);pending.groupId=group.str();}
        DelegatedMissionTiming::ActivityType activity=DelegatedMissionTiming::ActivityUndefined;double totalKm=0;
        if(delegationOfferKind==1){std::map<std::string,CityContractBoard>::iterator board=savedContractBoards.find(delegationBoardKey);if(board==savedContractBoards.end()||delegationOfferIndex<0||delegationOfferIndex>=6||!board->second.offers[delegationOfferIndex].available){delegationConfirming=false;refreshDelegationRosterCards();return;}BoardOffer& offer=board->second.offers[delegationOfferIndex];std::ostringstream identity;identity<<delegationBoardKey<<"#"<<delegationOfferIndex<<"#"<<board->second.expiresAt<<"#"<<offer.townId<<"#"<<offer.missionType;if(identity.str()!=delegationOfferIdentity){delegationConfirming=false;refreshDelegationRosterCards();return;}activity=offer.missionType==MCT_ESCORT?DelegatedMissionTiming::ActivityEscort:offer.missionType==MCT_CARAVAN?DelegatedMissionTiming::ActivityCaravan:offer.missionType==MCT_MAIL?DelegatedMissionTiming::ActivityMailDelivery:DelegatedMissionTiming::ActivityScientificExpedition;totalKm=std::max(0.0f,offer.distance)*(offer.missionType==MCT_MAIL?1.0:2.0)/1000.0;pending.reward=displayedContractCats(offer.estimatedPay);pending.title=missionBookTypeName(offer.missionType);pending.origin=missionBookOfficeCity;pending.destination=offer.townName;pending.difficulty=offer.dangerLevel;pending.guildXp=GuildProgression::success(offer.missionType,offer.distance/1000.0f*(offer.missionType==MCT_CARAVAN||offer.missionType==MCT_SCIENCE?2:1),offer.dangerLevel,0,true,false).xp;pending.clientName=offer.profile.empty()?(Loc::text("v8.literal.006")):offer.profile;}
        else if(delegationOfferKind==2){if(!prepareDelegatedBounty(pending,totalKm,activity)){delegationConfirming=false;refreshDelegationRosterCards();return;}}
        else{delegationConfirming=false;refreshDelegationRosterCards();return;}
        try{pending.timing=DelegatedMissionTiming::start(activity,totalKm,currentGameHours);}catch(...){delegationConfirming=false;refreshDelegationRosterCards();return;}pending.sent=(int)chosen.size();pending.officeKey=missionBookOfficeKey;pending.guildXp=GuildProgression::delegatedXp(pending.guildXp);pending.successChance=std::min(92.0f,65.0f+(float)chosen.size()*6.0f);pending.successRoll=UtilityT::random(0.0f,100.0f);pending.resultRolled=true;pending.success=pending.successRoll<pending.successChance;pending.reputationDelta=pending.success?1:-1;pending.outcomeCode=pending.success?"roll_below_chance":"roll_at_or_above_chance";
        std::vector<Character*> previouslySelected;for(ogre_unordered_set<hand>::type::const_iterator it=ou->player->selectedCharacters.begin();it!=ou->player->selectedCharacters.end();++it)if(it->getCharacter())previouslySelected.push_back(it->getCharacter());for(size_t i=0;i<previouslySelected.size();++i)ou->player->unselectPlayerCharacter(previouslySelected[i]);for(size_t i=0;i<chosen.size();++i)ou->player->selectObject(chosen[i],i!=0);
        if(!MissionAbsencePrototype::depart(pending.groupId)){delegationConfirming=false;refreshDelegationRosterCards();return;}
        payrollRememberParticipants(pending.groupId,chosen);
        seedDelegated(pending);
        if(delegationOfferKind==1){savedContractBoards[delegationBoardKey].offers[delegationOfferIndex].available=false;savedContractBoards[delegationBoardKey].offers[delegationOfferIndex].story="CONTRAT DELEGUE";saveContractBoards();}
        else consumeDelegatedBounty();
        delegatedMissions.push_back(pending);saveReputations();missionBookDelegationContext=false;if(delegationRosterWindow)delegationRosterWindow->setVisible(false);if(clientOptions.showNotifications)ou->showPlayerAMessage(Loc::text("delegated.started"),true);
    }

    bool delegatedPaymentVisitor(const std::string& groupId,Character** actor=0){const std::string key="PAY:"+groupId;for(size_t i=0;i<guildVisitors.size();++i)if(guildVisitors[i].offerKey==key){if(actor)*actor=guildVisitors[i].leader;return true;}return false;}
    std::string delegatedReportBody(const DelegatedMissionState& m)
    {
        std::ostringstream out;const int hours=std::max(0,(int)std::floor((m.completedAt-m.timing.startedAtWorldHour)+.5));
        out<<Loc::text("delegated.report.title")<<"\n\n"<<m.title<<"\n\n"<<Loc::text("delegated.report.result")<<" "<<(m.success?Loc::text("delegated.report.success"):Loc::text("delegated.report.failure"))
           <<"\n"<<Loc::text("delegated.report.client")<<" "<<m.clientName<<"\n"<<Loc::text("delegated.report.office")<<" "<<m.origin<<"\n"<<Loc::text("delegated.report.destination")<<" "<<m.destination
           <<"\n"<<Loc::text("delegated.report.difficulty")<<" "<<missionBookDifficulty(m.difficulty)<<"\n"<<Loc::text("delegated.report.duration")<<" "<<(hours/24)<<" j "<<(hours%24)<<Loc::text("ui.h")
           <<"\n\n"<<Loc::text("delegated.report.sent")<<" "<<m.sent<<"\n"<<Loc::text("delegated.report.returned")<<" "<<m.returned<<"/"<<m.sent<<"\n"<<Loc::text("delegated.report.injured")<<" "<<m.injured
           <<"\n"<<Loc::text("delegated.report.amputations")<<" "<<m.amputations<<"\n"<<Loc::text("delegated.report.dead")<<" "<<m.dead
           <<"\n\n"<<Loc::text("delegated.report.reward")<<" "<<(m.success?m.reward:0)<<" Cats\n"<<Loc::text("delegated.report.guild_xp")<<" "<<(m.success?m.guildXp:0)
           <<"\n"<<Loc::text("delegated.report.reputation")<<" "<<(m.reputationDelta>=0?"+":"")<<m.reputationDelta
           <<"\n"<<Loc::text("delegated.report.payment")<<" "<<(m.paymentState==3?Loc::text("delegated.report.received"):m.success?Loc::text("delegated.report.pending"):Loc::text("delegated.report.none"))
           <<"\n\n"<<Loc::text("delegated.report.reason")<<"\n"<<Loc::text("delegated.report.roll_explanation")<<" "<<(int)m.successRoll<<" / "<<(int)m.successChance<<". ";
        if(m.injured||m.dead||m.amputations)out<<Loc::text("delegated.report.casualties");else out<<Loc::text("delegated.report.no_incident");
        if(m.success&&m.paymentState==1)out<<"\n\n"<<Loc::text("delegated.report.client_queued");else if(m.success&&m.paymentState==2)out<<"\n\n"<<Loc::text("delegated.report.client_enroute");
        return out.str();
    }
    void closeDelegatedReport(MyGUI::WidgetPtr){if(delegatedReportWindow)delegatedReportWindow->setVisible(false);delegatedReportIndex=-1;if(ou&&!delegatedReportWasPaused)ou->userPause(false);}
    void closeDelegatedReportWindow(MyGUI::Window*,const std::string&){closeDelegatedReport(0);}
    void showDelegatedReport(int index)
    {
        if(index<0||index>=(int)delegatedMissions.size()||!delegatedMissions[index].completed||!MyGUI::Gui::getInstancePtr())return;
        if(!delegatedReportWindow){const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();int w=std::min(900,view.width-40),h=std::min(820,view.height-40);delegatedReportWindow=MyGUI::Gui::getInstancePtr()->createWidget<MyGUI::Window>("Kenshi_WindowCX",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","DelegatedMissionReport");delegatedReportWindow->eventWindowButtonPressed+=MyGUI::newDelegate(closeDelegatedReportWindow);MyGUI::Widget* c=delegatedReportWindow->getClientWidget();applyMercenarieFrame(c,true);delegatedReportText=c->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",28,22,w-76,h-125,MyGUI::Align::Default);delegatedReportText->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);delegatedReportText->setTextColour(MyGUI::Colour(.92f,.88f,.74f));delegatedReportCloseButton=c->createWidget<MyGUI::Button>("Kenshi_Button1",w-260,h-90,210,48,MyGUI::Align::Default);delegatedReportCloseButton->eventMouseButtonClick+=MyGUI::newDelegate(closeDelegatedReport);}
        delegatedReportIndex=index;delegatedReportWasPaused=ou&&ou->isPaused();MercenarieFonts::caption(delegatedReportWindow,Loc::text("delegated.report.title"));MercenarieFonts::caption(delegatedReportText,delegatedReportBody(delegatedMissions[index]));MercenarieFonts::caption(delegatedReportCloseButton,Loc::text("delegated.report.close"));delegatedReportWindow->setVisible(true);if(ou)ou->userPause(true);
    }
    void ensureDelegatedPaymentClient(DelegatedMissionState& mission)
    {
        if(!mission.completed||!mission.success||mission.paymentState==3)return;
        if(delegatedPaymentVisitor(mission.groupId)){mission.paymentState=2;return;}
        mission.paymentState=1;if(mission.officeKey.empty()||mission.officeKey!=currentGuildHouseKey||!guildBuilding||!guildClientChair||guildWaitingChairs.empty())return;
        const int capacity=1+(int)guildWaitingChairs.size();if(guildVisitorPeopleCount()>=capacity)return;
        size_t before=guildVisitors.size();spawnGuildVisitor(true);if(guildVisitors.size()!=before+1)return;
        GuildVisitor& visitor=guildVisitors.back();visitor.offerKey="PAY:"+mission.groupId;visitor.houseKey=mission.officeKey;visitor.type=Loc::text("delegated.payment.client_type");visitor.patience=1.0e9f;visitor.remarkClock=1.0e9f;visitor.urgent=visitor.vip=visitor.exceptional=false;mission.paymentState=2;seatGuildVisitors(true);saveReputations();
        DebugLog(std::string("Delegated payment client spawned group=")+mission.groupId+" office="+mission.officeKey);
    }
    void updateDelegatedMission()
    {
        if(MissionAbsencePrototype::persistent.active)return;
        for(size_t i=0;i<delegatedMissions.size();++i){DelegatedMissionState& mission=delegatedMissions[i];if(mission.completed){ensureDelegatedPaymentClient(mission);continue;}if(!mission.timing.active||!DelegatedMissionTiming::completed(mission.timing,currentGameHours))continue;if(!MissionAbsencePrototype::returnGroup(mission.groupId))continue;mission.timing.active=false;mission.completed=true;guildPayroll.complete(mission.groupId);mission.completedAt=currentGameHours;mission.returned=mission.sent;mission.injured=mission.amputations=mission.dead=0;mission.paymentState=mission.success?1:0;const std::string resultId="delegated-result:"+mission.groupId+":"+mission.offerIdentity;rewardedContractIds.insert(resultId);if(mission.success){++successfulContracts;GuildProgression::award(guildPoints,guildPrestige,mission.guildXp);escortReputation=GuildProgression::clampRep(escortReputation+(float)mission.reputationDelta);archiveDelegated(mission,1);}else{++failedContracts;escortReputation=GuildProgression::clampRep(escortReputation+(float)mission.reputationDelta);archiveDelegated(mission,2);}if(contractHistory.size()>50)contractHistory.resize(50);saveReputations();ensureDelegatedPaymentClient(mission);mission.reportShown=true;if(clientOptions.showNotifications&&clientOptions.notifyDelegatedComplete)showDelegatedReport((int)i);DebugLog(std::string("Delegated mission completed group=")+mission.groupId+" payment="+(mission.success?"pending":"none"));}
        int completedCount=0;for(int i=(int)delegatedMissions.size()-1;i>=0;--i)if(delegatedMissions[i].completed&&++completedCount>50&&delegatedMissions[i].paymentState!=1&&delegatedMissions[i].paymentState!=2)delegatedMissions.erase(delegatedMissions.begin()+i);
    }
    void updateMailMissions(float elapsed)
    {
        static float clock=0;clock-=elapsed;if(clock>0)return;clock=1.0f;restoreMailLetterIdentities();for(size_t c=0;c<mailContracts.size();++c){MailContracts::Contract& mission=mailContracts[c];if(!MailContracts::expired(mission,currentGameHours))continue;for(size_t s=0;s<mission.steps.size();++s)if(!mission.steps[s].delivered)removeMailLetter(mission.steps[s]);mission.status=MailContracts::MailFailed;++failedContracts;escortReputation=GuildProgression::clampRep(escortReputation-2);float& local=localReputations[ReputationIdentity::key(mission.originTownName)];local=GuildProgression::clampRep(local-6);archiveMail(mission,2);if(contractHistory.size()>50)contractHistory.resize(50);saveReputations();if(ou)ou->showPlayerAMessage(Loc::text("v8.literal.060"),true);DebugLog(std::string("Mail expired: contract=")+mission.contractId);}
    }

    void developerFinishMission()
    {
        if(!ou||!ou->player||!ou->player->participant)return;
        if(!missionActive||missionPending||!escort||currentContract.settlementPaid){
            ou->showPlayerAMessage(Loc::text("ui.accept_the_client_s_contract_first_no_active_mission"),true);return;
        }
        if(developerWindow)developerWindow->setVisible(false);
        // Reuse an already prepared report: no second roll of tips/XP/discovery.
        if(!(finalWindow&&finalWindow->getVisible())){
            if(caravanMission)caravanReturning=true;
            if(scientificMission){scientificResearching=false;scientificResearchSeconds=0;scientificReturning=true;}
            // Logical arrival only: never teleport, heal, refill cargo or fabricate
            // discoveries/combat. Existing losses still affect normal rewards.
            journeyData.distanceTravelled=std::max(journeyData.distanceTravelled,currentContract.distanceKm*1000.0f);
            DebugLog("Guild Escort CHEAT: simulated completed journey; normal settlement");
            finishMission(true);
        }
        // finishMission can already settle if the report UI cannot be created.
        // Arrival simulation stops at the report. The player chooses the bonuses.
    }

    void developerMissionDebug(bool raid);
    void updateMissionDebugButtons();
    void missionDebugTooltip(MyGUI::Widget*,const MyGUI::ToolTipInfo&);
    MyGUI::Button* developerRaidButton=0;
    MyGUI::Button* developerKoButton=0;

    MyGUI::Window* developerFinishPicker=0;
    void showDeveloperFinishPicker();
    void developerAdvanceWorldTime(float hours)
    {
        if(hours<=0)return;
        // The developer clock also runs while simulation is paused. Establish
        // the current cycle before jumping, then service every crossed deadline
        // here instead of waiting for another simulation update.
        payrollSync();
        developerTimeOffsetHours+=hours;currentGameHours+=hours;
        payrollSync();updatePayrollWindows();
        updateDelegatedMission();updateMailMissions(2.0f);
        if(guildWindow&&guildWindow->getVisible())updateGuildMenu();
        if(missionBookContentText)refreshMissionBook();
        updateTrackerUI();
    }
    void developerAction(MyGUI::WidgetPtr sender)
    {
        if(!clientOptions.developerMode)return;
        if(!sender)return;MyGUI::Button* actionButton=static_cast<MyGUI::Button*>(sender);std::string action=actionButton->getUserString("ActionId");
        if(action=="dev.0")setDeveloperGuildLevel(guildLevel()+1);
        else if(action=="dev.1")setDeveloperGuildLevel(guildLevel()-1);
        else if(action=="dev.2"){int oldLevel=guildLevel();GuildProgression::award(guildPoints,guildPrestige,100);int newLevel=guildLevel();saveReputations();if(newLevel>oldLevel){appliedGuildUnlockLevel=-1;updateGuildResearchAccess();showGuildLevelUpWindow(oldLevel,newLevel);}}
        else if(action=="dev.3"){guildPoints=std::max(0,guildPoints-100);saveReputations();}
        else if(action=="dev.4")setDeveloperGuildLevel(1);
        else if(action=="dev.5")setDeveloperGuildLevel(3);
        else if(action=="dev.6")setDeveloperGuildLevel(5);
        else if(action=="dev.7")setDeveloperGuildLevel(10);
        else if(action=="dev.8"){scanGuildFurniture();spawnGuildVisitor(true);}
        else if(action=="dev.9"){scanGuildFurniture();for(int i=0;i<3;++i)spawnGuildVisitor(true);}
        else if(action=="dev.10"){scanGuildFurniture();if(ou)ou->showPlayerAMessage(guildBuilding?Loc::text("ui.house_detected_verification_of_places_completed"):Loc::text("ui.no_house_place_both_chairs_in_the_same_player"),true);}
        else if(action=="dev.11"){while(!guildVisitors.empty()){if(guildVisitors[0].restoredLeader.pending||guildVisitors[0].restoredMembers.pending||!guildVisitors[0].leader)guildVisitors.erase(guildVisitors.begin());else removeGuildVisitor(guildVisitors[0].leader,false);}}
        else if(action=="dev.12"){loadContractBoards();for(std::map<std::string,CityContractBoard>::iterator i=savedContractBoards.begin();i!=savedContractBoards.end();++i)i->second.expiresAt=0;saveContractBoards();Character* giver=developerContractGiver();if(giver&&!missionActive&&!missionPending)openContractsBoard(giver,false);}
        else if(action=="dev.13")developerStartContract(0);
        else if(action=="dev.14")developerStartContract(1);
        else if(action=="dev.15")developerStartContract(2);
        else if(action=="dev.16"||action=="dev.17"||action=="dev.18")showDeveloperFinishPicker();
        else if(action=="dev.19"){if(missionActive||missionPending)cancelMission("ui.contract_fails_via_test_menu");}
        else if(action=="dev.20"){if(missionActive||missionPending)cancelMission("ui.test_contract_cleaned_without_additional_consequences");}
        else if(action=="dev.21")changeLocalReputation(10.0f,Loc::text("ui.test_tool"));
        else if(action=="dev.22")changeLocalReputation(-10.0f,Loc::text("ui.test_tool"));
        else if(action=="dev.23"){localReputations[ReputationIdentity::key(originCity)]=0;saveReputations();}
        else if(action=="dev.24"){developerFurnitureOverride=0;scanGuildFurniture();}
        else if(action=="dev.25"){developerFurnitureOverride=1;scanGuildFurniture();}
        else if(action=="dev.26"){developerFurnitureOverride=-1;scanGuildFurniture();}
        else if(action=="dev.27")unlockAllMercenarieResearch();
        else if(action=="dev.28")developerAdvanceWorldTime(24.0f);
        else if(action=="dev.29")developerAdvanceWorldTime(48.0f);
        else if(action=="dev.30"){FiscalRelation u=actualFiscalRelation(FISCAL_UC),m=actualFiscalRelation(FISCAL_MERCENARY_GUILD);std::string id=makeMissionFiscalId();fiscalLedger.create(id,Loc::text("ui.test_contract"),Loc::text("common.commun"),Loc::text("ui.menu_p"),"Heft","Squin",currentGameHours,10000,0,0,10000,true,u,m);for(int i=0;i<2;++i)if(fiscalLedger.organisations[i].nextCollectionHour<=0)fiscalLedger.organisations[i].nextCollectionHour=currentGameHours+48;saveFiscalLedger();}
        else if(action=="dev.31"){fiscalLedger.organisations[0].nextCollectionHour=currentGameHours;fiscalLedger.organisations[0].state=FSTATE_COLLECTION_DUE;saveFiscalLedger();}
        else if(action=="dev.32"){fiscalLedger.organisations[1].nextCollectionHour=currentGameHours;fiscalLedger.organisations[1].state=FSTATE_COLLECTION_DUE;saveFiscalLedger();}
        else if(action=="dev.33"){for(int i=0;i<2;++i)if(fiscalLedger.organisations[i].debt>=5000){FiscalOrganisationState& s=fiscalLedger.organisations[i];s.state=s.raidsWon==0?FSTATE_RAID_1_PENDING:s.raidsWon==1?FSTATE_RAID_2_PENDING:FSTATE_RAID_3_PENDING;s.deadlineHour=currentGameHours;break;}saveFiscalLedger();}
        else if(action=="dev.34"){fiscalLedger=FiscalLedger();saveFiscalLedger();}
        else if(action=="dev.35")developerMissionDebug(false);
        else if(action=="dev.38")developerMissionDebug(true);
        else if(action=="dev.36")MissionAbsencePrototype::depart();
        else if(action=="dev.37")MissionAbsencePrototype::returnCharacter();
        else if(action=="dev.39"){GuardSalute::releaseSelected();GuardRest::apply();}
        else if(action=="dev.40"){GuardRest::releaseSelected();GuardSalute::apply();}
        updateDeveloperStatus();
    }

    void developerWindowButtonPressed(MyGUI::Window*,const std::string&){if(developerWindow)developerWindow->setVisible(false);}

    int developerRowHeight=34;
    MyGUI::ImageBox* developerIcon(MyGUI::Widget* parent,int icon,int x,int y,int size,const MyGUI::Colour& colour)
    {
        MyGUI::ImageBox* image=parent->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);
        image->setImageTexture("MercenarieDeveloperUI.png");image->setImageCoord(MyGUI::IntCoord(icon*32,144,32,32));image->setColour(colour);image->setNeedMouseFocus(false);return image;
    }
    MyGUI::Button* developerButton(MyGUI::Widget* parent,int x,int y,int w,const char* text,const char* actionId,int category)
    {
        int action=atoi(actionId+4);std::string skin=action==38?"MercenarieDeveloperRaid":"MercenarieDeveloperButton"+registerNumber(category);
        MyGUI::Button* b=parent->createWidget<MyGUI::Button>(skin,x,y,w,developerRowHeight,MyGUI::Align::Default);
        MercenarieFonts::caption(b,text);b->setFontHeight(w<210?12:13);b->setTextAlign(MyGUI::Align::Center);
        // Wrap using the actual font metrics and reserved icon region; all actions keep the same height.
        MyGUI::TextBox* measure=parent->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",0,0,w-38,30,MyGUI::Align::Default);
        measure->setFontName("Kenshi_StandardFont_Medium");
        MercenarieFonts::caption(measure,text);measure->setVisible(false);
        int font=w<210?12:13;std::string caption=wrapRegisterCaption(measure,text,font);
        MercenarieFonts::caption(b,caption);MyGUI::Gui::getInstance().destroyWidget(measure);
        b->setUserString("ActionId",actionId);b->eventMouseButtonClick+=MyGUI::newDelegate(developerAction);
        b->setNeedToolTip(true);b->setUserString("developerCaption",text);
        b->setUserString("debugTip",action==38?"developer.raid.tip":action==35?"developer.ko.tip":action==28?"developer.time24.tip":action==29?"developer.time48.tip":"");
        b->eventToolTip+=MyGUI::newDelegate(missionDebugTooltip);
        int icon=category==0?(action==0?4:action==1?5:action<4?6:7):category==1?(action<10?1:7):category==2?(action==38?8:action==35?9:2):(action>=28&&action<=29?10:action>=30&&action<=34?11:3);
        developerIcon(b,icon,6,(developerRowHeight-18)/2,18,action==38?registerAmber:MyGUI::Colour(.72f,.73f,.71f));return b;
    }

    void createDeveloperMenu()
    {
        if(!clientOptions.developerMode)return;
        if(developerWindow||!MyGUI::Gui::getInstancePtr())return;
        MyGUI::Gui* g=MyGUI::Gui::getInstancePtr();const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();
        MyGUI::ResourceManager::getInstance().load("GuildRegisterSkins.xml");MyGUI::ResourceManager::getInstance().load("MercenarieOverviewSkins.xml");MyGUI::ResourceManager::getInstance().load("MercenarieDeveloperSkins.xml");
        int w=std::min(1120,view.width-24),h=std::min(700,view.height-24);
        developerWindow=g->createWidget<MyGUI::Window>("MercenarieGuildWindow",(view.width-w)/2,(view.height-h)/2,w,h,MyGUI::Align::Default,"Window","MercenarieDeveloperMenu");
        developerWindow->eventWindowButtonPressed+=MyGUI::newDelegate(developerWindowButtonPressed);
        MyGUI::ImageBox* logo=developerWindow->createWidget<MyGUI::ImageBox>("ImageBox",10,3,32,32,MyGUI::Align::Default);logo->setImageTexture("LevelSun.png");logo->setColour(registerAmber);logo->setNeedMouseFocus(false);
        registerText(developerWindow,54,5,148,28,19,Loc::text("developer.brand"),registerIvory);
        registerText(developerWindow,208,5,w-260,28,19,Loc::text("developer.title"),registerAmber);
        registerSolid(developerWindow,2,37,w-4,1,MyGUI::Colour(.65f,.35f,.10f));
        MyGUI::Widget* c=developerWindow->getClientWidget();int cw=c->getWidth(),ch=c->getHeight();
        registerText(c,8,4,cw-16,20,15,Loc::text("ui.development_and_test_tools"),registerAmber);
        registerText(c,8,24,cw-16,20,13,Loc::text("developer.warning"),MyGUI::Colour(.86f,.76f,.56f));
        int gap=8,colW=(cw-16-gap*3)/4,panelH=ch-128;
        // Reserve the scrollbar width when measuring every label, so one shared
        // row height is safe in all four columns, including community translations.
        developerRowHeight=34;
        const char* actionKeys[]={"ui.1_level_a57ed8c","ui.1_level","ui.100_xp","ui.100_xp_afa99eb","ui.level_1","ui.level_3","ui.level_5","ui.level_10","ui.summon_1_client","ui.summon_3_clients","ui.test_the_chairs","ui.remove_test_clients","ui.simulate_locked_search","ui.simulate_search_completed","ui.return_to_normal_mode","ui.unlock_mod_research","ui.renew_the_6_offers","ui.generate_escort","ui.generate_caravan","ui.generate_expedition","developer.finish_mission","ui.contract_failure","ui.clean_contract","debug.raid","debug.ko","ui.10_reputation","ui.10_reputation_9a508a0","ui.neutral_reputation","ui.simulate_24_h","ui.simulate_48_h","ui.create_tax_debt","ui.trigger_uc_collection","ui.trigger_guild_collection","ui.simulate_next_raid","ui.reset_tax_system","developer.depart","developer.return","developer.guard_rest","developer.guard_salute"};
        MyGUI::TextBox* measure=c->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",0,0,colW-70,100,MyGUI::Align::Default);measure->setFontName("Kenshi_StandardFont_Medium");measure->setVisible(false);
        for(size_t k=0;k<sizeof(actionKeys)/sizeof(actionKeys[0]);++k){MercenarieFonts::caption(measure,wrapRegisterCaption(measure,Loc::text(actionKeys[k]),colW-32<210?12:13));developerRowHeight=std::max(developerRowHeight,measure->getTextSize().height+4);}
        g->destroyWidget(measure);int rowStep=developerRowHeight+4;
        const char* titles[]={Loc::text("ui.guild_level"),Loc::text("developer.clients"),Loc::text("common.contrats"),Loc::text("ui.reputation_time")};
        const MyGUI::Colour colours[]={MyGUI::Colour(.94f,.56f,.12f),MyGUI::Colour(.44f,.75f,.30f),MyGUI::Colour(.15f,.69f,.86f),MyGUI::Colour(.68f,.40f,.78f)};
        for(int i=0;i<4;++i){
            MyGUI::Widget* panel=c->createWidget<MyGUI::Widget>("TheMercenarie_Panel",8+i*(colW+gap),50,colW,panelH,MyGUI::Align::Default);
            optionKitBorder(panel,colours[i]);developerIcon(panel,i,7,8,24,colours[i]);
            MyGUI::TextBox* title=registerText(panel,36,4,colW-42,32,colW<210?12:15,titles[i],colours[i]);title->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);
            MercenarieFonts::caption(title,wrapRegisterCaption(title,titles[i],colW<210?12:15));registerSolid(panel,1,39,colW-2,1,colours[i]);
            MyGUI::ScrollView* body=panel->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",5,44,colW-10,panelH-49,MyGUI::Align::Default);
            MercenarieNativeInput::bind(body);body->setVisibleHScroll(false);body->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);
            int count=i==3?14:i==2?9:8;bool scroll=count*rowStep-4>body->getHeight();int buttonW=colW-14-(scroll?18:0);
            body->setVisibleVScroll(scroll);body->setCanvasSize(buttonW,count*rowStep-4);
            if(i==0){const char* a[]={Loc::text("ui.1_level_a57ed8c"),Loc::text("ui.1_level"),Loc::text("ui.100_xp"),Loc::text("ui.100_xp_afa99eb"),Loc::text("ui.level_1"),Loc::text("ui.level_3"),Loc::text("ui.level_5"),Loc::text("ui.level_10")};const char* ids[]={"dev.0","dev.1","dev.2","dev.3","dev.4","dev.5","dev.6","dev.7"};for(int j=0;j<8;++j)developerButton(body,0,j*rowStep,buttonW,a[j],ids[j],i);}
            if(i==1){const char* a[]={Loc::text("ui.summon_1_client"),Loc::text("ui.summon_3_clients"),Loc::text("ui.test_the_chairs"),Loc::text("ui.remove_test_clients"),Loc::text("ui.simulate_locked_search"),Loc::text("ui.simulate_search_completed"),Loc::text("ui.return_to_normal_mode"),Loc::text("ui.unlock_mod_research")};const char* ids[]={"dev.8","dev.9","dev.10","dev.11","dev.24","dev.25","dev.26","dev.27"};for(int j=0;j<8;++j)developerButton(body,0,j*rowStep,buttonW,a[j],ids[j],i);}
            if(i==2){const char* a[]={Loc::text("ui.renew_the_6_offers"),Loc::text("ui.generate_escort"),Loc::text("ui.generate_caravan"),Loc::text("ui.generate_expedition"),Loc::text("developer.finish_mission"),Loc::text("ui.contract_failure"),Loc::text("ui.clean_contract"),Loc::text("debug.raid"),Loc::text("debug.ko")};const char* ids[]={"dev.12","dev.13","dev.14","dev.15","dev.16","dev.19","dev.20","dev.38","dev.35"};for(int j=0;j<9;++j){MyGUI::Widget* row=body->createWidget<MyGUI::Widget>("",0,j*rowStep,buttonW,developerRowHeight,MyGUI::Align::Default);MyGUI::Button* button=developerButton(row,0,0,buttonW,a[j],ids[j],i);if(j==7||j==8){row->setNeedToolTip(true);row->setUserString("debugTip",j==7?"developer.raid.tip":"developer.ko.tip");row->eventToolTip+=MyGUI::newDelegate(missionDebugTooltip);}if(j==7)developerRaidButton=button;if(j==8)developerKoButton=button;}}
            if(i==3){const char* a[]={Loc::text("developer.guard_rest"),Loc::text("developer.guard_salute"),Loc::text("ui.10_reputation"),Loc::text("ui.10_reputation_9a508a0"),Loc::text("ui.neutral_reputation"),Loc::text("ui.simulate_24_h"),Loc::text("ui.simulate_48_h"),Loc::text("ui.create_tax_debt"),Loc::text("ui.trigger_uc_collection"),Loc::text("ui.trigger_guild_collection"),Loc::text("ui.simulate_next_raid"),Loc::text("ui.reset_tax_system"),Loc::text("developer.depart"),Loc::text("developer.return")};const char* ids[]={"dev.39","dev.40","dev.21","dev.22","dev.23","dev.28","dev.29","dev.30","dev.31","dev.32","dev.33","dev.34","dev.36","dev.37"};for(int j=0;j<14;++j)developerButton(body,0,j*rowStep,buttonW,a[j],ids[j],i);}
        }

        registerSolid(c,8,ch-73,cw-16,1,MyGUI::Colour(.65f,.35f,.10f));
        developerStatusScroll=c->createWidget<MyGUI::ScrollView>("TheMercenarie_ScrollView",10,ch-68,cw-20,64,MyGUI::Align::Default);MercenarieNativeInput::bind(developerStatusScroll);developerStatusScroll->setVisibleHScroll(false);developerStatusScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);
        developerStatus=developerStatusScroll->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText",0,0,cw-42,60,MyGUI::Align::Default,"MercenarieTestStatus");developerStatus->setFontHeight(cw<850?11:13);developerStatus->setTextColour(MyGUI::Colour(.73f,.76f,.72f));developerStatus->setNeedMouseFocus(false);developerStatusScroll->setVisibleVScroll(false);developerStatusScroll->setCanvasSize(cw-42,60);developerWindow->setVisible(false);
    }

    void updateDeveloperMenuHotkey()
    {
        updateMissionDebugButtons();
        if(!clientOptions.developerMode)return;
        if(!key||!key->keyboard)return;int binding=clientOptions.bindings[ClientOptions::OpenCheatMenu];bool down=binding&&key->keyboard->isKeyDown((OIS::KeyCode)binding);if(down&&!pWasDown&&!negotiationOpen&&!gbModalOpen()){createDeveloperMenu();if(developerWindow){bool show=!developerWindow->getVisible();if(show)updateDeveloperStatus();developerWindow->setVisible(show);}}pWasDown=down;
    }
    void updateAutopilotHotkey(){if(!key||!key->keyboard)return;int binding=clientOptions.bindings[ClientOptions::OpenAutopilot];bool down=binding&&key->keyboard->isKeyDown((OIS::KeyCode)binding);if(down&&!autopilotWasDown&&!negotiationOpen&&!gbModalOpen())openMercenarieAutopilot(0);autopilotWasDown=down;}

    void resetMercenarieImportClicked(MyGUI::WidgetPtr){resetMercenarieOnImport=!resetMercenarieOnImport;cleanupMercenarieOnImport=false;if(resetMercenarieImportCheck)resetMercenarieImportCheck->setStateSelected(resetMercenarieOnImport);if(cleanupMercenarieImportCheck)cleanupMercenarieImportCheck->setStateSelected(false);}
    void cleanupMercenarieImportClicked(MyGUI::WidgetPtr){cleanupMercenarieOnImport=!cleanupMercenarieOnImport;resetMercenarieOnImport=false;if(cleanupMercenarieImportCheck)cleanupMercenarieImportCheck->setStateSelected(cleanupMercenarieOnImport);if(resetMercenarieImportCheck)resetMercenarieImportCheck->setStateSelected(false);}
    void cleanupFiscalParty(){const std::string seatQueueKey=guildFiscalQueueKey();for(size_t i=0;i<fiscalParty.members.size();++i){Character* c=fiscalParty.members[i].isNull()?0:fiscalParty.members[i].getCharacter();if(c&&c->getFaction()){releaseGuildVisitorSeat(c);c->getFaction()->destroyObject(c);}}fiscalParty=FiscalParty();guildSeatTickets.erase(seatQueueKey);refreshGuildSeatQueue();}
    bool spawnFiscalParty(FiscalOrganisation o,int raid)
    {
        if(!fiscalParty.leader.isNull()||!guildBuilding||!ou||!shou)return false;
        const char* factionId=o==FISCAL_UC?"defaultEmpireFactionSID":"1214-gamedata.base";
        const char* squadId=0;
        if(o==FISCAL_UC){const char* ids[]={"880209-Guild Escort Contracts.mod","880210-Guild Escort Contracts.mod","880211-Guild Escort Contracts.mod","880212-Guild Escort Contracts.mod"};squadId=ids[std::max(0,std::min(3,raid))];}
        else{const char* ids[]={"880219-Guild Escort Contracts.mod","880220-Guild Escort Contracts.mod","880221-Guild Escort Contracts.mod","880222-Guild Escort Contracts.mod"};squadId=ids[std::max(0,std::min(3,raid))];}
        Faction* faction=ou->factionMgr->getFactionByStringID(factionId);GameData* squad=ou->gamedata.getData(squadId,SQUAD_TEMPLATE);TownBase* baseTown=guildBuilding->getCurrentTownLocation();Town* town=dynamic_cast<Town*>(baseTown);
        if(!faction||!squad||!town)return false;
        Ogre::Vector3 spawn=town->getPositionOutsideTownGates(45.0f);Platoon* p=ou->theFactory->createRandomSquad(faction,spawn,0,1,0,squad,0,0,0,true,hand(),town,1.0f,SQ_ROAMING,false);
        if(!p||!p->activePlatoon||p->activePlatoon->things.size()==0)return false;
        fiscalParty.organisation=(int)o;fiscalParty.raid=raid;fiscalTargetOffice=guildBuilding->getHandle();
        for(size_t i=0;i<p->activePlatoon->things.size();++i){Character* c=static_cast<Character*>(p->activePlatoon->things[i]);fiscalParty.members.push_back(hand(c));if(i==0)fiscalParty.leader=hand(c);updateGuildOfficeTravel(c,guildBuilding,0);}
        Character* leader=fiscalParty.leader.getCharacter();if(!leader){cleanupFiscalParty();return false;}fiscalParty.lastPosition=leader->getPosition();FiscalOrganisationState& state=fiscalLedger.organisations[(int)o];state.activeRaid=raid;state.state=FSTATE_COLLECTOR_TRAVELLING;saveFiscalLedger();return true;
    }
    bool fiscalPlayerDefeated(){if(!ou||!ou->player)return false;bool found=false;for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(!c||c->isDead())continue;if(guildBuilding&&c->getPosition().squaredDistance(guildBuilding->getPosition())>9000000.0f)continue;found=true;if(c->getMedical()&&!c->getMedical()->isUnconcious())return false;}return found;}
    void startFiscalCombat(){if(fiscalParty.organisation<0||fiscalParty.raid<=0||!ou||!ou->player)return;Character* leader=fiscalParty.leader.isNull()?0:fiscalParty.leader.getCharacter();Faction* f=leader?leader->getFaction():0;if(f&&f->relations)f->relations->setRelation(ou->player->participant,-100.0f);for(size_t i=0;i<fiscalParty.members.size();++i){Character* c=fiscalParty.members[i].isNull()?0:fiscalParty.members[i].getCharacter();if(c&&!c->isDead())c->addJob(CHOOSE_ENEMY_AND_ATTACK,0,false,false,guildBuilding?guildBuilding->getPosition():c->getPosition());}FiscalOrganisationState& s=fiscalLedger.organisations[fiscalParty.organisation];s.state=fiscalParty.raid==1?FSTATE_RAID_1_COMBAT:fiscalParty.raid==2?FSTATE_RAID_2_COMBAT:FSTATE_RAID_3_COMBAT;saveFiscalLedger();}
    void updateFiscalParty(float elapsed)
    {
        fiscalParty.restoreRetry-=elapsed;if(fiscalParty.restoreRetry>0)return;
        for(size_t i=0;i<fiscalParty.members.size();++i)if(!fiscalParty.members[i].isNull()&&!SavedActorResolution::find(fiscalParty.members[i])){fiscalParty.restoreRetry=1;return;}
        Character* leader=fiscalParty.leader.isNull()?0:fiscalParty.leader.getCharacter();if(!leader&&!fiscalParty.leader.isNull()){fiscalParty.restoreRetry=1;return;} // Preserve the saved collector while streaming.
        if(!leader){if(fiscalParty.organisation>=0&&fiscalParty.organisation<2){FiscalOrganisationState& lost=fiscalLedger.organisations[fiscalParty.organisation];lost.state=FSTATE_WAITING_FOR_PLAYER;lost.nextCollectionHour=currentGameHours+1.0;saveFiscalLedger();}fiscalParty=FiscalParty();return;}
        FiscalOrganisationState& s=fiscalLedger.organisations[fiscalParty.organisation];
        Building* office=fiscalTargetOffice.getBuilding();
        if(!office&&guildBuilding&&guildHouseKey(guildBuilding)==s.targetHouseId){fiscalTargetOffice=guildBuilding->getHandle();office=guildBuilding;}
        for(size_t i=0;i<fiscalParty.members.size();++i)if(s.state==FSTATE_COLLECTOR_TRAVELLING||s.state==FSTATE_DIALOGUE)updateGuildOfficeTravel(fiscalParty.members[i].getCharacter(),office,elapsed);
        if(s.state==FSTATE_COLLECTOR_TRAVELLING){Ogre::Vector3 pos=leader->getPosition();if(office&&guildOfficeArrived(leader,office)){for(size_t i=0;i<fiscalParty.members.size();++i){Character* c=fiscalParty.members[i].isNull()?0:fiscalParty.members[i].getCharacter();if(c&&guildOfficeArrived(c,office))c->getMovement()->halt();}fiscalParty.waitingForConversation=true;fiscalParty.seatRefresh=0;s.state=FSTATE_DIALOGUE;s.newTaxesAtLastVisit=s.debt;saveFiscalLedger();seatFiscalCollector();leader->sayALine(Loc::text("ui.the_tax_collector_has_arrived_speak_to_me_at"),true);if(ou)ou->showPlayerAMessage(Loc::text("ui.a_faction_tax_collector_is_waiting_at_the_dedicated"),true);return;}return;}
        if(s.state==FSTATE_DIALOGUE&&fiscalParty.waitingForConversation){fiscalParty.seatRefresh-=elapsed;if(fiscalParty.seatRefresh<=0){seatFiscalCollector();fiscalParty.seatRefresh=8.0f;}return;}
        if(s.state==FSTATE_RAID_1_COMBAT||s.state==FSTATE_RAID_2_COMBAT||s.state==FSTATE_RAID_3_COMBAT){if(fiscalPlayerDefeated()){s.state=FSTATE_PLAYER_DEFEATED;s.nextCollectionHour=currentGameHours+168.0;s.deadlineHour=s.nextCollectionHour;leader->sayALine(Loc::text("ui.pitiful_we_will_return_in_seven_days_if_you"),true);cleanupFiscalParty();saveFiscalLedger();return;}bool capable=false;for(size_t i=0;i<fiscalParty.members.size();++i){Character* c=fiscalParty.members[i].isNull()?0:fiscalParty.members[i].getCharacter();if(c&&!c->isDead()&&c->getMedical()&&!c->getMedical()->isUnconcious()){capable=true;break;}}if(!capable){++s.raidsWon;s.activeRaid=0;if(s.raidsWon>=3){fiscalLedger.becomeRebel((FiscalOrganisation)fiscalParty.organisation);ou->showPlayerAMessage(Loc::text("ui.tax_rebellion_the_three_collection_teams_were_rejected_this"),true);}else{s.state=s.raidsWon==1?FSTATE_RAID_1_DEFEATED:FSTATE_RAID_2_DEFEATED;s.nextCollectionHour=currentGameHours+48.0;s.preArrivalNotified=false;}cleanupFiscalParty();saveFiscalLedger();}}
    }
    bool fiscalPlayerNearHeadquarters(){if(!guildBuilding)return false;float distance=0;Character* p=nearestPlayer(guildBuilding->getPosition(),distance);return p&&distance<=9000000.0f;}
    void updateFiscalSystem()
    {
        if(currentGameHours<=0)return;bool eventBusy=(fiscalCollectorWindow&&fiscalCollectorWindow->getVisible())||fiscalParty.leader;
        for(int i=0;i<2;++i){FiscalOrganisationState& s=fiscalLedger.organisations[i];if(s.rebel||s.debt<=0)continue;if(s.nextCollectionHour<=0)s.nextCollectionHour=currentGameHours+(guildHouseOperational?48.0:168.0);if(!guildHouseOperational)continue;double remaining=s.nextCollectionHour-currentGameHours;if(remaining<=24.0&&!s.preArrivalNotified&&remaining>0){char b[4096];sprintf_s(b,mercenarieLocalize(Loc::text("ui.tax_notice_a_s_collector_will_arrive_in_about")).c_str(),fiscalOrgName((FiscalOrganisation)i));ou->showPlayerAMessage(b,true);s.preArrivalNotified=true;saveFiscalLedger();}if(currentGameHours<s.nextCollectionHour||eventBusy)continue;if(!fiscalPlayerNearHeadquarters()){s.state=FSTATE_WAITING_FOR_PLAYER;continue;}int raid=0;if(s.state==FSTATE_WARNING){s.state=FSTATE_RAID_1_PENDING;s.raidsWon=0;raid=1;}else if(s.state==FSTATE_RAID_1_DEFEATED){s.state=FSTATE_RAID_2_PENDING;raid=2;}else if(s.state==FSTATE_RAID_2_DEFEATED){s.state=FSTATE_RAID_3_PENDING;raid=3;}else if(s.state==FSTATE_PLAYER_DEFEATED){raid=std::max(1,std::min(3,s.activeRaid?s.activeRaid:s.raidsWon+1));s.state=raid==1?FSTATE_RAID_1_PENDING:raid==2?FSTATE_RAID_2_PENDING:FSTATE_RAID_3_PENDING;ou->showPlayerAMessage(Loc::text("ui.the_collection_team_returns_as_promised_pay_now_or"),true);}else if(s.state==FSTATE_COLLECTOR_TRAVELLING||s.state==FSTATE_DIALOGUE||s.state==FSTATE_RAID_1_PENDING||s.state==FSTATE_RAID_1_COMBAT||s.state==FSTATE_RAID_2_PENDING||s.state==FSTATE_RAID_2_COMBAT||s.state==FSTATE_RAID_3_PENDING||s.state==FSTATE_RAID_3_COMBAT)raid=std::max(0,std::min(3,s.activeRaid));else s.state=FSTATE_COLLECTION_DUE;s.targetHouseId=designatedGuildHouseKey;if(!spawnFiscalParty((FiscalOrganisation)i,raid)){s.state=FSTATE_WAITING_FOR_PLAYER;s.nextCollectionHour=currentGameHours+1.0;}eventBusy=true;}
    }
    void showMercenarieImportOption(SaveManager* manager){if(!manager||!manager->importMenu||!manager->importMenu->newGameOptions)return;ImportGameMenu* menu=manager->importMenu;MyGUI::Widget* root=reinterpret_cast<MercenarieLayoutAccess*>(menu->newGameOptions)->rootWidget();if(!root)return;if(configuredMercenarieImportMenu!=menu||!resetMercenarieImportCheck){int y=std::max(20,root->getHeight()-105);resetMercenarieImportCheck=root->createWidget<MyGUI::Button>("Kenshi_TickBoxSkin",28,y,38,38,MyGUI::Align::Left|MyGUI::Align::Bottom,"ResetMercenarieImportCheck");resetMercenarieImportCheck->eventMouseButtonClick+=MyGUI::newDelegate(resetMercenarieImportClicked);resetMercenarieImportLabel=root->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",76,y+2,root->getWidth()-104,38,MyGUI::Align::HStretch|MyGUI::Align::Bottom,"ResetMercenarieImportLabel");cleanupMercenarieImportCheck=root->createWidget<MyGUI::Button>("Kenshi_TickBoxSkin",28,y+44,38,38,MyGUI::Align::Left|MyGUI::Align::Bottom,"CleanupMercenarieImportCheck");cleanupMercenarieImportCheck->eventMouseButtonClick+=MyGUI::newDelegate(cleanupMercenarieImportClicked);
        cleanupMercenarieImportLabel=root->createWidget<MyGUI::TextBox>("Kenshi_TextboxStandardText_Large",76,y+46,root->getWidth()-104,38,MyGUI::Align::HStretch|MyGUI::Align::Bottom,"CleanupMercenarieImportLabel");configuredMercenarieImportMenu=menu;}
        cleanupMercenarieOnImport=false;cleanupMercenarieImportCheck->setStateSelected(false);cleanupMercenarieImportCheck->setVisible(true);MercenarieFonts::caption(cleanupMercenarieImportLabel,Loc::text("cleanup.option"));cleanupMercenarieImportLabel->setTextColour(MyGUI::Colour(0.93f,0.70f,0.34f));cleanupMercenarieImportLabel->setVisible(true);resetMercenarieOnImport=false;resetMercenarieImportCheck->setStateSelected(false);resetMercenarieImportCheck->setVisible(true);MercenarieFonts::caption(resetMercenarieImportLabel,Loc::text("ui.reset_the_mercenarie"));resetMercenarieImportLabel->setTextColour(MyGUI::Colour(0.93f,0.70f,0.34f));resetMercenarieImportLabel->setVisible(true);}
    void hideMercenarieImportOption(){if(cleanupMercenarieImportCheck)cleanupMercenarieImportCheck->setVisible(false);if(cleanupMercenarieImportLabel)cleanupMercenarieImportLabel->setVisible(false);if(resetMercenarieImportCheck)resetMercenarieImportCheck->setVisible(false);if(resetMercenarieImportLabel)resetMercenarieImportLabel->setVisible(false);}
    void resetAllMercenarieProgress(){releaseAllPersonnel();completedEscortRestore=SavedActorResolution::Actor();completedCaravanRestore=caravanRestore=scientificRestore=SavedActorResolution::Group();resetQuestContexts();MissionAbsencePrototype::returnCharacters();delegatedMission.clear();delegatedMissions.clear();for(size_t c=0;c<mailContracts.size();++c)for(size_t s=0;s<mailContracts[c].steps.size();++s)if(!mailContracts[c].steps[s].delivered)removeMailLetter(mailContracts[c].steps[s]);mailContracts.clear();while(!guildVisitors.empty()){if(guildVisitors[0].restoredLeader.pending||guildVisitors[0].restoredMembers.pending||!guildVisitors[0].leader)guildVisitors.erase(guildVisitors.begin());else removeGuildVisitor(guildVisitors[0].leader,false);}guildVisitors.clear();departingVisitors.clear();missionActive=false;missionPending=false;missionPaused=false;contractLifecycle=CONTRACT_NONE;escort=0;escortHandle.setNull();contractBarman=0;contractBarmanHandle.setNull();contractOriginTown=0;caravanMembers.clear();scientificMembers.clear();caravanMission=false;scientificMission=false;negotiationOpen=false;negotiationSuspended=false;if(negotiationWindow)negotiationWindow->setVisible(false);guildPoints=0;guildPrestige=0;escortReputation=0;progressWriteBlocked=false;rewardedContractIds.clear();successfulContracts=0;failedContracts=0;totalContractCats=0;totalAdvances=0;totalBonuses=0;totalTips=0;contractHistory.clear();contractSeeds.clear();localReputations.clear();cityMemories.clear();archivedReputationAliases.clear();guildHouseNames.clear();guildHouseCities.clear();guildHouseOriginalNames.clear();designatedGuildHouseKey.clear();operationalAnnouncementKey.clear();designatedGuildHouseHandle.setNull();guildBuilding=0;guildClientChair=0;guildClientChairs.clear();guildFiscalChair=0;guildWaitingChairs.clear();guildSeatReservations.clear();guildSeatTickets.clear();guildTravelRecords.clear();fiscalTargetOffice.setNull();guildSeatNextTicket=1;guildClientSystemActive=false;guildHouseOperational=false;currentGuildHouseKey.clear();currentGuildHouseName.clear();savedContractBoards.clear();guildRerolls=ContractReroll::Charges();resetV9PendingActions();developerTimeOffsetHours=0;developerFurnitureOverride=-1;currentContract=EscortContractData();journeyData=EscortJourneyData();fiscalLedger=FiscalLedger();saveFiscalLedger();saveReputations();saveContractBoards();if(ou&&ou->player&&ou->player->technology){lektor<GameData*> researches;ou->gamedata.getDataOfType(researches,RESEARCH);const char* lists[]={"enable buildings","enable armour","enable weapons","enable weapon models","enable items","enable crafting"};for(unsigned int i=0;i<researches.size();++i){GameData* r=researches[i];if(!r)continue;if(r->stringID.find("-Guild Escort Contracts.mod")==std::string::npos&&r->stringID.find("-Holy Nation Mercenary Plastron.mod")==std::string::npos)continue;ou->player->technology->finished.erase(r);for(unsigned int l=0;l<sizeof(lists)/sizeof(lists[0]);++l){const Ogre::vector<GameDataReference>::type* refs=r->getReferenceListIfExists(lists[l]);if(refs)for(unsigned int j=0;j<refs->size();++j)if((*refs)[j].ptr)ou->player->technology->enabledObjects.erase((*refs)[j].ptr);}}ou->player->technology->changedSoUpdateGUI=true;}}
}

#include "v5/BountyRuntime.h"
#define MERCENARIE_ESCORT_PERSONNEL 1
#include "QuestContexts.h"
#include "MissionDebug.h"
#include "MissionMapOverlay.h"
#include "GuildContractsRuntime.h"
namespace {
bool acceptMissionBookSecurityPersonal(int index){
    if(index<0||index>=6||!missionBookSelectionValid())return false;
    Character* selectedIssuer=bountyViewingIssuer.isNull()?0:bountyViewingIssuer.getCharacter();if(!selectedIssuer)return false;
    const std::string identity=missionBookSecurityOfferIds[index];
    const bool wasVisible=contractsWindow&&contractsWindow->getVisible();
    if(contractsWindow)contractsWindow->setVisible(false);if(bountyWindow)bountyWindow->setVisible(false);
    // Same quest-slot preparation as the officer; the book itself must not lock it.
    bool prepared=prepareBountyQuest();if(contractsWindow)contractsWindow->setVisible(wasVisible);if(!prepared)return false;
    openBountyOffers(selectedIssuer);if(bountyWindow)bountyWindow->setVisible(false);if(!bountyAccept)return false;
    std::map<std::string,MercenarieV5::BountyBoard>::const_iterator board=bountyWorld.boards.find(bountyViewingIssuer.toString());if(board==bountyWorld.boards.end())return false;
    bountyDisplayedOffers=board->second.offers;bountySelection=-1;
    for(size_t i=0;i<bountyDisplayedOffers.size();++i)if(bountyDisplayedOffers[i].id==identity)bountySelection=(int)i;
    if(bountySelection<0)return false;
    bool context=missionBookDelegationContext;missionBookDelegationContext=false;acceptBountyOffer(bountyAccept);missionBookDelegationContext=context;
    return bountyWorld.contract.occupied()&&bountyWorld.contract.offer.id==identity;
}

bool prepareMissionBookSecurityIdentity(int index){if(index<0||index>=6||missionBookSecurityOfferIds[index].empty()||bountyViewingIssuer.isNull())return false;delegationOfferKind=2;delegationOfferIndex=index;delegationOfferIdentity=missionBookSecurityOfferIds[index];delegationIssuerIdentity=bountyViewingIssuer.toString();return true;}
bool beginMissionBookSecurityDelegation(int index){if(index<0||index>=6||missionBookSecurityOfferIds[index].empty()||bountyViewingIssuer.isNull())return false;delegationOfferIdentity=missionBookSecurityOfferIds[index];delegationIssuerIdentity=bountyViewingIssuer.toString();beginDelegationSelection(2,index);return true;}
void missionBookBoardTabClicked(MyGUI::WidgetPtr sender){for(int i=0;i<6;++i)if(sender==missionBookBoardTabs[i]){missionBookTab=i;break;}refreshMissionBookContractHub();}
bool missionBookSameTown(Character*);
void collectMissionBookSecurityIssuers();
#include "MissionBookPoolRuntime.h"
#include "V9Runtime.h"
#include "GuildPayrollView.h"
void seedDelegated(const DelegatedMissionState& pending){
 GuildHistory::Snapshot seed;seed.id=pending.groupId+":"+pending.offerIdentity;seed.status=0;seed.accepted=pending.timing.startedAtWorldHour;seed.estimateMin=pending.timing.estimateEarliestReturnWorldHour-seed.accepted;seed.estimateMax=pending.timing.estimateLatestReturnWorldHour-seed.accepted;seed.giverId=pending.issuerIdentity;seed.origin=pending.origin;seed.destination=pending.destination;seed.reward=pending.reward;
 if(pending.kind==1){const BoardOffer& offer=savedContractBoards[delegationBoardKey].offers[delegationOfferIndex];seed.type=historyType(offer.missionType);seed.giver=1;seed.destinationId=offer.townId;seed.difficulty=offer.dangerLevel;}
 else {seed.type="bounty";seed.giver=2;std::map<std::string,MercenarieV5::BountyBoard>::const_iterator board=bountyWorld.boards.find(pending.issuerIdentity);if(board!=bountyWorld.boards.end())for(size_t i=0;i<board->second.offers.size();++i){const MercenarieV5::BountyOffer& offer=board->second.offers[i];if(offer.id==pending.offerIdentity){seed.target=offer.targetName;seed.destinationId=offer.areaId;seed.difficulty=1+2*MercenarieV5::bountyDifficulty(offer);}}}
 contractSeeds[seed.id]=seed;
}
bool prepareDelegatedBounty(DelegatedMissionState& pending,double& totalKm,DelegatedMissionTiming::ActivityType& activity){std::map<std::string,MercenarieV5::BountyBoard>::iterator board=bountyWorld.boards.find(delegationIssuerIdentity);if(board==bountyWorld.boards.end()||board->second.refreshDue(currentGameHours))return false;MercenarieV5::BountyOffer* offer=0;for(size_t i=0;i<board->second.offers.size();++i)if(board->second.offers[i].id==delegationOfferIdentity){offer=&board->second.offers[i];break;}if(!offer)return false;Character* issuer=bountyViewingIssuer.isNull()?0:bountyViewingIssuer.getCharacter();if(!issuer||issuer->getHandle().toString()!=delegationIssuerIdentity)return false;Town* area=shou&&shou->townList?shou->townList->getTownBySID(offer->areaId):0;if(!area)return false;BoardOffer route;analyseContractRoute(issuer->getPosition(),area->getPosition(),route);totalKm=BountyPresentationRules::roundTripKilometres(route.distance);activity=DelegatedMissionTiming::ActivityBountyHunt;pending.reward=displayedContractCats(ContractReroll::bountyReward(offer->amount,offer->id));pending.title=Loc::text("ui.bounty_hunt")+std::string(" - ")+offer->targetName;pending.origin=missionBookOfficeCity;pending.destination=frenchPlaceName(area->getName());pending.difficulty=MercenarieV5::bountyDifficulty(*offer);pending.guildXp=30+ContractReroll::bountyReward(offer->amount,offer->id)/500;pending.clientName=issuer->getName();return true;}
void consumeDelegatedBounty(){std::map<std::string,MercenarieV5::BountyBoard>::iterator b=bountyWorld.boards.find(delegationIssuerIdentity);if(b!=bountyWorld.boards.end())b->second.consume(delegationOfferIdentity,currentGameHours);}

bool missionBookSameTown(Character* actor){
    TownBase* town=actor?actor->getCurrentTownLocation():0;
    return town&&town->getGameData()&&!missionBookTownId.empty()&&town->getGameData()->stringID==missionBookTownId;
}
void collectMissionBookSecurityIssuers(){
    missionBookSecurityIssuers.clear();std::map<std::string,hand> found;
    // Persistent offer identities also find officers outside the active platoon scan.
    for(std::map<std::string,MercenarieV5::BountyBoard>::const_iterator b=bountyWorld.boards.begin();b!=bountyWorld.boards.end();++b)
        for(size_t i=0;i<b->second.offers.size();++i){hand h=MercenarieV5::bountyHandle(b->second.offers[i].issuer);Character* c=h.isNull()?0:h.getCharacter();MercenarieV5::IssuerFaction kind;
            if(missionBookSameTown(c)&&MercenarieV5::bountyIssuer(c,kind))found[h.toString()]=h;}
    const lektor<Faction*>* factions=ou&&ou->factionMgr?ou->factionMgr->getAllFactions():0;
    if(factions)for(unsigned int f=0;f<factions->size();++f){Faction* faction=(*factions)[f];if(!faction)continue;
        for(unsigned int p=0;p<faction->activePlatoons.size();++p){Platoon* platoon=faction->activePlatoons[p];if(!platoon||!platoon->activePlatoon)continue;
            for(unsigned int i=0;i<platoon->activePlatoon->things.size();++i){Character* actor=dynamic_cast<Character*>(platoon->activePlatoon->things[i]);MercenarieV5::IssuerFaction kind;
                if(missionBookSameTown(actor)&&MercenarieV5::bountyIssuer(actor,kind))found[actor->getHandle().toString()]=actor->getHandle();}}}
    for(std::map<std::string,hand>::const_iterator i=found.begin();i!=found.end();++i)missionBookSecurityIssuers.push_back(i->second);
}
Character* missionBookTownBountyIssuer(){collectMissionBookSecurityIssuers();return missionBookSecurityIssuers.empty()?0:missionBookSecurityIssuers[0].getCharacter();}
void synchronizeMissionBookSecurityBoard(){Character* issuer=missionBookTownBountyIssuer();if(!issuer||bountyWorld.contract.occupied())return;openBountyOffers(issuer);if(bountyWindow)bountyWindow->setVisible(false);}
std::string missionBookSecurityText(const std::string& officeCity){missionBookEntries.clear();synchronizeMissionBookSecurityBoard();Character* giver=bountyViewingIssuer.isNull()?0:bountyViewingIssuer.getCharacter();TownBase* town=giver?giver->getCurrentTownLocation():0;if(!giver||!town||frenchPlaceName(town->getName())!=officeCity)return Loc::text("v8.literal.061");for(size_t i=0;i<bountyDisplayedOffers.size()&&i<6;++i){const MercenarieV5::BountyOffer& offer=bountyDisplayedOffers[i];Town* area=shou&&shou->townList?shou->townList->getTownBySID(offer.areaId):0;std::ostringstream row;row<<Loc::text("ui.bounty_hunt")<<" | "<<giver->getName()<<" | "<<offer.targetName<<" | "<<(area?frenchPlaceName(area->getName()):Loc::text("ui.unknown_location"))<<" | "<<bountyDifficultyName(offer)<<" | "<<displayedContractCats(offer.amount)<<Loc::text("ui.cats");missionBookEntries.push_back(MissionBookEntry(2,(int)i,row.str()));}return missionBookEntries.empty()?(Loc::text("v8.literal.061")):std::string();}
void openMissionBookBountyDetail(int index){Character* giver=missionBookTownBountyIssuer();if(!giver){missionBookDelegationContext=false;return;}openBountyOffers(giver);if(index<0||index>=(int)bountyDisplayedOffers.size()){missionBookDelegationContext=false;return;}bountySelection=index;updateBountyDossier();if(bountyAccept)MercenarieFonts::caption(bountyAccept,Loc::text("v8.literal.062"));}
int overviewEscortCount(bool expeditions){int count=0;for(int i=0;i<maximumActiveQuests;++i){const bool active=i==selectedEscortQuest?(missionActive||missionPending):escortQuests[i].occupied();const bool paid=i==selectedEscortQuest?currentContract.settlementPaid:escortQuests[i].v_currentContract.settlementPaid;const bool scientific=i==selectedEscortQuest?scientificMission:escortQuests[i].v_scientificMission;const hand& h=i==selectedEscortQuest?escortHandle:escortQuests[i].v_escortHandle;Character* actor=h.isNull()?0:h.getCharacter();if(active&&!paid&&(!actor||!actor->isDead())&&(!expeditions||scientific))++count;}return count;}
int overviewBountyCount(){int count=0;for(int i=0;i<maximumActiveQuests;++i){const MercenarieV5::BountyWorldState& state=i==selectedBountyQuest?bountyWorld:bountyQuests[i];if(state.contract.occupied())++count;}return count;}
std::string registerBountySummary(){
    for(int i=0;i<maximumActiveQuests;++i){const MercenarieV5::BountyWorldState& state=i==selectedBountyQuest?bountyWorld:bountyQuests[i];
        if(!state.contract.occupied())continue;
        std::stringstream s;s<<Loc::text("ui.bounty_hunt")<<"\n\n"<<state.contract.offer.targetName<<"\n\n"<<registerNumber(state.contract.offer.amount)<<" Cats\n";
        s<<Loc::text(state.suspended?"ui.waiting":"ui.search_for_target");return s.str();
    }return "";
}
}
#include "DeveloperFinishPicker.h"
#include "ArtisanNative.h"
#include "MissionPersistence.h"
#include "CaravanScenarioPicker.h"
typedef std::map<float,Tasker*,std::less<float>,Ogre::STLAllocator<std::pair<float const,Tasker*>,Ogre::GeneralAllocPolicy> > MissionNativeGoalScores;
#include "MissionNativeGoalRuntime.h"
#include "MissionOffscreenRuntime.h"
#include "MissionMotionDiagnostic.h"
#include "MissionSpeechRuntime.h"
#include "MissionPaceGuard.h"
#include <kenshi/gui/OrdersPanel.h>
#include "EscortPersonnelPlan.h"
namespace {
#include "EscortPersonnelRuntime.h"
#include "EscortPersonnelUI.h"
}
#include "CaravanNativeFollowHook.h"
namespace {bool financeReady(){return !MercenarieCleanup::disabled&&!missionWorldChanging&&!missionRestorePending&&!progressWriteBlocked&&!progressLoadFault&&!activeMercenarieSaveSlot.empty()&&fiscalLedger.cash.available;}}
#include "GuildInvestmentPersistence.h"
#include "GuildInvestmentRuntime.h"
#include "RealEstateRuntime.h"
#include "BuildingPublicAccess.h"
#include "GuildImportRuntime.h"
#include "CleanupImportRuntime.h"

void (*saveManagerShowImportOriginal)(SaveManager*);
void saveManagerShowImportHook(SaveManager* manager){if(!saveManagerShowImportOriginal)return;saveManagerShowImportOriginal(manager);showMercenarieImportOption(manager);}
int (*saveManagerImportGameOriginal)(SaveManager*,const std::string&,const std::string&,int);
int saveManagerImportGameHook(SaveManager* manager,const std::string& location,const std::string& name,int flags){
    MercenariePerf::Event perf("IMPORT","preflight-and-native-call");
    if(!saveManagerImportGameOriginal){reportPersistenceError("IMPORT",SaveDiagnostics::HookUnavailable,"native import hook unavailable",location+"/"+name,"persistence.native_import_failed");return -1;}
    const bool remove=cleanupMercenarieOnImport;
    const bool reset=!remove&&(resetMercenarieOnImport||(flags&RESET_MERCENARIE_IMPORT_FLAG)!=0);
    if((remove||reset)&&!confirmCleanupImport(manager,location,name,flags,remove))return -1;
    bool disabled=false;const std::string sourceSlot=SaveTransaction::parent(GuildSavePaths::directory(location,name));
    flags&=~RESET_MERCENARIE_IMPORT_FLAG;
    GuildImportData imported;
    // Read the selected save before the engine starts replacing the world.
    try{flushPendingMissionSave();if(pendingMissionSave)throw std::runtime_error("save completion/publication pending; import deferred");SaveTransaction::preflight(sourceSlot,reset||remove,false);
        // A valid tombstone is authoritative. Never fall back to old companions.
        disabled=remove||(!reset&&SaveTransaction::disabledSlot(sourceSlot));
        if(remove)logCleanupInventory(GuildSavePaths::directory(location,name));
        imported=readGuildImportData(location,name,reset||disabled);}
    catch(const std::exception& e){ErrorLog(std::string("Guild Escort: import cancelled: ")+e.what());reportPersistenceException("IMPORT",e,location+"/"+name,"ui.import_cancelled_guild_progress_could_not_be_read_keep");return -1;}
    missionWorldChanging=true;missionRestorePending=false;missionRestoreBytes.clear();
    releaseMissionUIBeforeLoad();discardMissionWorldReferences();
    artisanReset();resetMailItemIdentities();MercenarieCleanup::disabled=disabled;MercenarieCleanup::protectedSource.clear();int result;
    try{MercenariePerf::Mode native(true);result=saveManagerImportGameOriginal(manager,location,name,flags);}
    catch(const std::exception& e){missionWorldChanging=false;progressLoadFault=progressWriteBlocked=true;ErrorLog(std::string("[CLEANUP] FAILED phase=native-import ")+e.what());reportPersistenceException("IMPORT",e,sourceSlot,"cleanup.error");return -1;}
    catch(...){missionWorldChanging=false;progressLoadFault=progressWriteBlocked=true;ErrorLog("[CLEANUP] FAILED phase=native-import unknown exception");return -1;}
    hideMercenarieImportOption();resetMercenarieOnImport=false;cleanupMercenarieOnImport=false;
    if(result!=0&&result!=2){missionWorldChanging=false;progressLoadFault=progressWriteBlocked=true;ErrorLog("[CLEANUP] FAILED phase=native-import return-code");std::ostringstream detail;detail<<"native import returned "<<result;reportPersistenceError("IMPORT",SaveDiagnostics::NativeFailed,detail.str(),location+"/"+name,"persistence.native_import_failed");return result;}
    if(disabled){
        try{
            clearMercenarieForCleanup();MercenarieCleanup::protectedSource=SaveTransaction::canonical(sourceSlot);
            const std::string session=GuildSavePaths::sessionDirectory();
            if(session.empty()||!GuildSavePaths::ensureDirectory(session))throw std::runtime_error("cleanup session unavailable");
            SaveTransaction::write(session+"/"+MercenarieCleanup::markerName(),MercenarieCleanup::marker());
            progressLoadFault=progressWriteBlocked=false;progressVirginWorld=false;missionWorldChanging=false;
            DebugLog("[CLEANUP] disable marker written (isolated session; committed to new slot on save)");DebugLog("[CLEANUP] imported world disabled; awaiting native save under a new name");
            if(ou)ou->showPlayerAMessage(Loc::text("cleanup.success"),true);return result;
        }catch(const std::exception& e){missionWorldChanging=false;progressLoadFault=progressWriteBlocked=true;ErrorLog(std::string("[CLEANUP] FAILED phase=session ")+e.what());reportPersistenceException("IMPORT",e,sourceSlot,"cleanup.error");return -1;}
    }
    // An imported world writes to its own session directory until an explicit
    // save. Neither the previous world nor the import source is a write target.
    progressLoadFault=false;progressWriteBlocked=false;progressVirginWorld=false;
    requestMercenarieDataSlot("","");activeMercenarieSaveSlot.clear();configureMercenarieDataSlot();
    try{
        if(progressWriteBlocked)throw std::runtime_error("import session directory unavailable");
        unpackGuildImportSnapshot(imported,reset);
        discardMissionWorldReferences();bountyWorld=MercenarieV5::BountyWorldState();
        missionRestorePending=false;missionRestoreBytes.clear();restoredFinalVisible=false;restoredFinalCaption.clear();
        clearGuildProgressForImport();
        if(reset){
            resetAllMercenarieProgress();
            if(!GuildProgression::validProgress(readMissionFile(reputationFile)))throw std::runtime_error("reset guild progression could not be staged");
        }
        else {
            if(!imported.reputation.empty()){std::istringstream in(imported.reputation);readReputations(in);}
            if(progressWriteBlocked)throw std::runtime_error("imported guild progression invalid");
            if(!atomicMissionFile(fiscalFile,imported.fiscal)||!atomicMissionFile(contractBoardsFile,imported.boards)||
               !atomicMissionFile(reputationFile,serializeReputations()))throw std::runtime_error("imported guild data could not be staged");
            loadFiscalLedger();loadContractBoards();
        }
        if(!reset){guildPayroll=imported.payroll;artisanLedger=imported.artisan;developerTimeOffsetHours=imported.clock;importedDomainsSuspended=true;importedArtisans=ImportRecovery::artisanKeys(artisanLedger);importedArtisanHour=ou&&!imported.recoveryPending?WorldServiceClock::now(ou->getTimeStamp_inGameHours().getTotalHours(),developerTimeOffsetHours):-1;}
        else importedDomainsSuspended=false;
        if(importedDomainsSuspended&&ou)ou->showPlayerAMessage(Loc::text("save.import.suspended"),true);
        estateState=RealEstate::load(reset?std::string():imported.estate);
        if(estateEnabled())estateState.suspendAll(ou?WorldServiceClock::now(ou->getTimeStamp_inGameHours().getTotalHours(),developerTimeOffsetHours):0);
        snapshotReputation=serializeReputations();snapshotFiscal=readMissionFile(fiscalFile);snapshotBoards=readMissionFile(contractBoardsFile);
        diagnosticTrace("IMPORT guild progression ready",reputationFile.c_str());
    }catch(const std::exception& e){
        discardMissionWorldReferences();bountyWorld=MercenarieV5::BountyWorldState();
        missionRestorePending=false;missionRestoreBytes.clear();progressLoadFault=true;progressWriteBlocked=true;
        ErrorLog(std::string("Guild Escort: imported progress unavailable: ")+e.what());
        reportPersistenceException("IMPORT",e,location+"/"+name,"ui.guild_progress_could_not_be_restored_after_import_guild");
    }
    scheduleGuildFurnitureRecovery();missionWorldChanging=false;if(reset&&ou&&!progressWriteBlocked)ou->showPlayerAMessage(Loc::text("cleanup.reset.success"),true);return result;
}

// V4: stock stays owned by the NPC, including categories not currently shown.
const char* guildStockPrefixes[3]={"MercenarieArmourDay_","MercenarieHeadDay_","MercenarieLegsDay_"};


int guildVendorItemCategory(Item* item)
{
    if(!item||!item->getGameData())return -1;
    const std::string& id=item->getGameData()->stringID;
    if(id=="981100-Guild Escort Contracts.mod"||id=="981101-Guild Escort Contracts.mod")return 0;
    if(id=="910002-Holy Nation Mercenary Plastron.mod"||id=="910005-Holy Nation Mercenary Plastron.mod"||id=="980001-Holy Nation Mercenary Plastron.mod"||id=="980010-Guild Escort Contracts.mod")return 0;
    if(id=="910007-Holy Nation Mercenary Plastron.mod"||id=="910012-Holy Nation Mercenary Plastron.mod")return 1;
    if(id=="910004-Holy Nation Mercenary Plastron.mod"||id=="910009-Holy Nation Mercenary Plastron.mod"||id=="980003-Holy Nation Mercenary Plastron.mod")return 2;
    if(id=="990002-Holy Nation Mercenary Plastron.mod"||id=="990003-Holy Nation Mercenary Plastron.mod")return 3;
    return -1;
}

void collectGuildVendorItems(Inventory* inventory,std::vector<std::pair<InventorySection*,Item*> >& found)
{
    if(!inventory)return;
    lektor<InventorySection*>& sections=inventory->getAllSections();
    for(unsigned int s=0;s<sections.size();++s){InventorySection* section=sections[s];if(!section)continue;const Ogre::vector<InventorySection::SectionItem>::type& items=section->getItems();for(size_t i=0;i<items.size();++i){Item* item=items[i].item;if(item&&!item->isEquipped&&guildVendorItemCategory(item)>=0)found.push_back(std::make_pair(section,item));}}
}

InventorySection* guildVendorSection(Inventory* inventory,int category,int day)
{
    lektor<InventorySection*>& sections=inventory->getAllSections();
    for(unsigned int i=0;i<sections.size();++i)
        if(sections[i]&&sections[i]->name.find(guildStockPrefixes[category])==0)return sections[i];
    std::stringstream name;name<<guildStockPrefixes[category]<<-1;
    return inventory->initialiseNewSection(name.str(),40,40,ATTACH_NONE,false,true,false,0);
}

bool moveGuildStock(InventorySection* from,InventorySection* to,Item* item)
{
    if(from==to)return true;
    // Reserve room before detaching. Never drop or destroy an item on transfer failure.
    if(!to->hasRoomForItem(item->getGameData(),item->quantity)){
        if(to->height>=400)return false;
        to->resize(to->width,to->height+40,false);
    }
    if(!from->removeItem(item))return false;
    if(to->addItem(item,item->quantity))return true;
    if(!from->addItem(item,item->quantity))ErrorLog("Mercenarie V4: failed to restore stock transfer");
    return false;
}

// Two singleton native lists: each generates exactly one possible weapon type.
// Called only by the existing stock-generation transaction, never on each frame
// or when reopening an already generated trading day.
void fillGuildWeaponStock(Inventory* inventory,Character* trader)
{
    const char* lists[]={"981120-Guild Escort Contracts.mod","981121-Guild Escort Contracts.mod"};
    for(int i=0;i<2;++i){
        GameData* list=ou->gamedata.getData(lists[i],VENDOR_LIST);
        if(list)inventory->fillFromVendorList(list,trader->getFaction());
        else ErrorLog("Guild weapons: missing singleton stock list");
    }
}

void selectGuildVendorCategory(Dialogue* dialogue,int category)
{
    diagnosticTrace("VENDOR open");
    Character* trader=dialogue?dialogue->getCharacter():0;
    if(!trader||!trader->getGameData()||trader->getGameData()->stringID!="970002-Holy Nation Mercenary Plastron.mod"||category<0||category>2)return;
    Inventory* inventory=trader->getInventory();if(!inventory||!ou)return;
    diagnosticTrace("VENDOR inventory resolved");
    int day=std::max(0,static_cast<int>(currentGameHours/24.0));
    InventorySection* stock[3];
    for(int i=0;i<3;++i){stock[i]=guildVendorSection(inventory,i,day);if(!stock[i])return;}
    std::vector<std::pair<InventorySection*,Item*> > found;
    collectGuildVendorItems(inventory,found);
    for(size_t i=0;i<found.size();++i){
        int target=guildVendorItemCategory(found[i].second);
        if(target>=0&&target<3&&!moveGuildStock(found[i].first,stock[target],found[i].second))
            ErrorLog("Mercenarie V4: stock capacity reached; item retained in original section");
    }
    const char* lists[]={"970010-Guild Escort Contracts.mod","970011-Guild Escort Contracts.mod","970012-Guild Escort Contracts.mod"};
    for(int i=0;i<3;++i){
        int stockDay=atoi(stock[i]->name.substr(std::string(guildStockPrefixes[i]).size()).c_str());
        // Migrate existing stock without replacing its quantities or quality.
        // Empty new categories must be populated on the first visit, not tomorrow.
        if(stockDay<0&&!stock[i]->getItems().empty()){
            if(i==0){
                for(int j=0;j<3;++j)stock[j]->setEnabled(j==i);
                fillGuildWeaponStock(inventory,trader);
            }
            std::stringstream name;name<<guildStockPrefixes[i]<<day;stock[i]->name=name.str();continue;
        }
        // Backwards time (loading an older save) is not a new trading day.
        if(day>stockDay){
            GameData* list=ou->gamedata.getData(lists[i],VENDOR_LIST);
            if(list){
                stock[i]->clearAllItems(true,false);
                for(int j=0;j<3;++j)stock[j]->setEnabled(j==i);
                inventory->fillFromVendorList(list,trader->getFaction());
                if(i==0)fillGuildWeaponStock(inventory,trader);
                std::stringstream name;name<<guildStockPrefixes[i]<<day;stock[i]->name=name.str();
            }
        }
    }
    // Vendor generation can use a vanilla inventory section: classify it again.
    found.clear();collectGuildVendorItems(inventory,found);
    for(size_t i=0;i<found.size();++i){int target=guildVendorItemCategory(found[i].second);if(target>=0&&target<3)moveGuildStock(found[i].first,stock[target],found[i].second);}
    for(int i=0;i<3;++i)stock[i]->setEnabled(i==category);
    inventory->notifyModified();
    diagnosticTrace("VENDOR category completed");
}

void (*doActionsOriginal)(Dialogue*, DialogLineData*);
    std::string fiscalDialogueAmount(FiscalOrganisation o)
    {std::ostringstream out;out<<Loc::text("fiscal.dialogue.amount_prefix")<<fiscalLedger.debt(o)<<Loc::text("fiscal.dialogue.cats_suffix");return out.str();}
    std::string fiscalDialogueDetail(FiscalOrganisation o)
    {
        long long taxable=0,due=0;int entries=0,minRate=101,maxRate=-1;
        for(size_t i=0;i<fiscalLedger.entries.size();++i){const FiscalEntry& e=fiscalLedger.entries[i];int tax=o==FISCAL_UC?e.ucTax:e.guildTax,paid=o==FISCAL_UC?e.ucPaid:e.guildPaid,rate=o==FISCAL_UC?e.ucRate:e.guildRate;int outstanding=std::max(0,tax-paid);if(!outstanding)continue;++entries;taxable+=e.taxableIncome;due+=outstanding;minRate=std::min(minRate,rate);maxRate=std::max(maxRate,rate);}
        std::ostringstream out;out<<Loc::text("fiscal.dialogue.detail_entries_prefix")<<entries<<(entries==1?Loc::text("fiscal.dialogue.detail_entry_singular"):Loc::text("fiscal.dialogue.detail_entry_plural"))<<taxable<<Loc::text("fiscal.dialogue.cats_sentence");if(minRate<=maxRate)out<<Loc::text("fiscal.dialogue.rate_prefix")<<minRate<<(minRate==maxRate?"%":"% - "+FiscalLedger::toString((unsigned long)maxRate)+"%")<<". ";out<<Loc::text("fiscal.dialogue.total_prefix")<<due<<Loc::text("fiscal.dialogue.cats_suffix");return out.str();
    }
    void fiscalDialogueReply(Dialogue* dialogue,const std::string& text,bool continueConversation)
    {
        if(!dialogue)return;dialogue->npcReplyText=text;dialogue->replyIds.clear();dialogue->responses.clear();dialogue->threadMessages.push_back(Dialogue::DT_CLEAR_RESPONSES);dialogue->threadMessages.push_back(Dialogue::DT_SET_NPC_REPLY);
        if(!continueConversation){dialogue->threadMessages.push_back(Dialogue::DT_END_DIALOG);return;}
        const char* ids[]={"880234-Guild Escort Contracts.mod","880235-Guild Escort Contracts.mod","880236-Guild Escort Contracts.mod","880237-Guild Escort Contracts.mod","880238-Guild Escort Contracts.mod"};
        const char* labels[]={"ui.very_well_how_much_do_i_owe_you","ui.explain_how_this_amount_was_calculated","ui.i_do_not_have_that_amount_right_now","ui.i_refuse_to_pay","ui.i_will_come_back_to_you"};
        for(int i=0;i<5;++i){dialogue->replyIds.push_back(ids[i]);dialogue->responses.push_back(Loc::text(labels[i]));}
        if(fiscalParty.organisation>=0&&fiscalParty.organisation<2&&fiscalLedger.debt((FiscalOrganisation)fiscalParty.organisation)>0){dialogue->replyIds.push_back("mercenarie.fiscal.pay");dialogue->responses.push_back(Loc::text("fiscal.dialogue.pay_now"));}
        dialogue->threadMessages.push_back(Dialogue::DT_SET_RESPONSES);
        std::ostringstream trace;trace<<"FISCAL native reply queued responses="<<dialogue->responses.size()<<" text="<<text;DebugLog(trace.str());
    }
    bool handleFiscalDialogueChoice(Dialogue* dialogue,Character* actor,const std::string& actionId)
    {
        const bool nativeChoice=actionId=="880234-Guild Escort Contracts.mod"||actionId=="880235-Guild Escort Contracts.mod"||actionId=="880236-Guild Escort Contracts.mod"||actionId=="880237-Guild Escort Contracts.mod"||actionId=="880238-Guild Escort Contracts.mod";
        const bool paymentChoice=actionId=="mercenarie.fiscal.pay";
        if(!nativeChoice&&!paymentChoice)return false;
        Character* collector=fiscalParty.leader.isNull()?0:fiscalParty.leader.getCharacter();
        if(!dialogue||!actor||actor!=collector||fiscalParty.organisation<0||fiscalParty.organisation>=2)return false;
        FiscalOrganisation o=(FiscalOrganisation)fiscalParty.organisation;FiscalOrganisationState& state=fiscalLedger.organisations[fiscalParty.organisation];
        std::ostringstream trace;trace<<"FISCAL handler begin id="<<actionId<<" organisation="<<fiscalParty.organisation<<" debt="<<fiscalLedger.debt(o);DebugLog(trace.str());
        if(actionId=="880234-Guild Escort Contracts.mod")fiscalDialogueReply(dialogue,fiscalDialogueAmount(o),true);
        else if(actionId=="880235-Guild Escort Contracts.mod")fiscalDialogueReply(dialogue,fiscalDialogueDetail(o),true);
        else if(actionId=="880236-Guild Escort Contracts.mod"){
            if(fiscalParty.raid>0||state.extensionUsed)fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.no_more_time"),true);
            else{state.firstIntroductionDone=true;state.extensionUsed=true;state.state=FSTATE_EXTENSION;state.deadlineHour=currentGameHours+168.0;state.nextCollectionHour=state.deadlineHour;state.preArrivalNotified=false;saveFiscalLedger();fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.seven_days"),false);departFiscalParty();}
        }
        else if(actionId=="880237-Guild Escort Contracts.mod"){
            state.firstIntroductionDone=true;
            if(fiscalParty.raid>0){fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.refuse_combat"),false);startFiscalCombat();}
            else{if(state.debt<5000){state.state=FSTATE_NORMAL;state.nextCollectionHour=currentGameHours+168.0;state.extensionUsed=false;state.preArrivalNotified=false;fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.debt_recorded"),false);}else{state.state=FSTATE_WARNING;state.deadlineHour=currentGameHours+48.0;state.nextCollectionHour=state.deadlineHour;state.preArrivalNotified=false;fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.refusal_warning"),false);}saveFiscalLedger();departFiscalParty();}
        }
        else if(actionId=="880238-Guild Escort Contracts.mod")fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.remain_available"),false);
        else{
            long long debt=fiscalLedger.debt(o);int money=financeBalance();
            if(debt<=0)fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.nothing_due"),true);
            else if(money<debt){std::ostringstream reply;reply<<Loc::text("fiscal.dialogue.insufficient_prefix")<<money<<Loc::text("fiscal.dialogue.insufficient_middle")<<debt<<Loc::text("fiscal.dialogue.cats_suffix");fiscalDialogueReply(dialogue,reply.str(),true);}
            else if(!financePayDebt(o))fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.payment_failed"),true);
            else{fiscalDialogueReply(dialogue,Loc::text("fiscal.dialogue.payment_confirmed"),false);saveFiscalLedger();updateFinancesPage();departFiscalParty();}
        }
        return true;
    }
bool handleNegotiationOpen93(Dialogue* dialogue,const std::string& id){
    if(id!="880029-Guild Escort Contracts.mod"&&id!="880039-Guild Escort Contracts.mod"&&id!="880049-Guild Escort Contracts.mod")return false;
    Character* actor=dialogue?dialogue->getCharacter():0;
    if(!actor||actor->isDead())return true;
    selectQuestActor(actor);
    if(missionPending&&actor==escort){
        dialogue->endDialogue(true);
        openNegotiation();
        return true;
    }
    if(isGuildVisitor(actor)){selectGuildVisitorOffer(actor);dialogue->endDialogue(true);openContractsBoard(actor,false);return true;}
    DebugLog("Negotiation open rejected: speaker has no pending personal contract");
    if(ou)ou->showPlayerAMessage(Loc::engine().language=="fr"?"Aucun contrat en attente de discussion avec ce client. Consultez le suivi de mission.":"No contract is awaiting discussion with this client. Check the mission tracker.",true);
    return true;
}
void doActionsHook(Dialogue* dialogue, DialogLineData* line)
{
    if(MercenarieCleanup::disabled){if(doActionsOriginal)doActionsOriginal(dialogue,line);return;}
    MercenariePerf::Event perf("DIALOGUE","actions");
    if(!doActionsOriginal||!dialogue)return;
    if(missionWorldChanging||missionRestorePending){{MercenariePerf::Mode native(true);doActionsOriginal(dialogue,line);}return;}
    const std::string actionId=line?line->getStringID():std::string();
    if(handleNegotiationOpen93(dialogue,actionId))return;
    if(isContractIntroduction(actionId)&&missionNativeGoalOwner(dialogue->getCharacter())>=0)return;
    selectQuestActor(dialogue->getCharacter());
    if(missionActive&&dialogue->getCharacter()!=escort&&rescueRetired(dialogue->getCharacter())){rescueRetireDialogue(dialogue->getCharacter());return;}
    Character* actor=dialogue->getCharacter();hand actorHandle;if(actor)actorHandle=actor->getHandle();else actorHandle.setNull();
    diagnosticTrace("DIALOGUE actions begin",actionId.c_str());
    // These five lines are fully handled here. Their FCS branches contain native
    // end-dialogue actions; calling them before the reply made the injected answer
    // arrive after Kenshi had already closed the conversation.
    if(handleFiscalDialogueChoice(dialogue,actor,actionId))return;
    // Capture the counterpart before native actions can close/change the conversation.
    if(line&&line->getStringID()=="880062-Guild Escort Contracts.mod")followConversationPartner(dialogue);
    if(line){const std::string id=line->getStringID();if(id=="970006-Holy Nation Mercenary Plastron.mod")selectGuildVendorCategory(dialogue,0);else if(id=="970013-Guild Escort Contracts.mod")selectGuildVendorCategory(dialogue,1);else if(id=="970014-Guild Escort Contracts.mod")selectGuildVendorCategory(dialogue,2);}
    if(missionActive&&contractLifecycle==CONTRACT_ACTIVE&&isMissionCommandSpeaker(actor)){
        if(actionId=="880060-Guild Escort Contracts.mod")pauseMission();
        else if(actionId=="880061-Guild Escort Contracts.mod")resumeMission();
        return; // Mission leader has no trading, contract-offer or hiring actions.
    }
    {MercenariePerf::Mode native(true);doActionsOriginal(dialogue, line);}
    diagnosticTrace("DIALOGUE native actions returned",actionId.c_str());
    actor=actorHandle.isNull()?0:actorHandle.getCharacter();
    if(!actor||actor->isDead())return;
    if(actionId=="881500-Guild Escort Contracts.mod"){openBountyOffers(actor);return;}
    if(actionId=="881501-Guild Escort Contracts.mod"){handOverBounty(actor);return;}
    if (!actionId.empty())
    {
        const std::string& id = actionId;
        if (id == "880017-Guild Escort Contracts.mod" || id == "880019-Guild Escort Contracts.mod")
        {Character* giver=actor;if(isGuildVisitor(giver))selectGuildVisitorOffer(giver);else visitorOfferUrgent=visitorOfferVip=visitorOfferExceptional=false;openContractsBoard(giver,false);}
        if (id == "880070-Guild Escort Contracts.mod")
            openContractsBoard(actor,true);
        if (id == "880090-Guild Escort Contracts.mod")
            openContractsBoard(actor,false,true);
        if (id == "880021-Guild Escort Contracts.mod" ||
            id == "880022-Guild Escort Contracts.mod" ||
            id == "880023-Guild Escort Contracts.mod")
            openContractsBoard(actor,false);
        if(isContractIntroduction(id))
        {Character* giver=actor;if(isGuildVisitor(giver)){selectGuildVisitorOffer(giver);openContractsBoard(giver,false);}}
        if (id == "880029-Guild Escort Contracts.mod" ||
            id == "880039-Guild Escort Contracts.mod" ||
            id == "880049-Guild Escort Contracts.mod")
        {Character* giver=actor;if(isGuildVisitor(giver)){selectGuildVisitorOffer(giver);openContractsBoard(giver,false);}else openNegotiation();}
        if (id == "880060-Guild Escort Contracts.mod") pauseMission();
        if (id == "880061-Guild Escort Contracts.mod") resumeMission();
    }
}

bool (*dialogueSendEventOriginal)(Dialogue*, Character*, EventTriggerEnum);
bool dialogueSendEventHook(Dialogue* dialogue, Character* target, EventTriggerEnum eventType)
{
    if(MercenarieCleanup::disabled)return dialogueSendEventOriginal&&dialogueSendEventOriginal(dialogue,target,eventType);
    MercenariePerf::Event perf("DIALOGUE","talk",eventType==EV_PLAYER_TALK_TO_ME);
    diagnosticTrace("DIALOGUE event begin");
    if(!dialogueSendEventOriginal||!dialogue)return false;
    if(missionWorldChanging||missionRestorePending){MercenariePerf::Mode native(true);return dialogueSendEventOriginal(dialogue,target,eventType);}
    selectQuestActor(dialogue->getCharacter());
    if(missionActive&&dialogue->getCharacter()!=escort&&rescueRetired(dialogue->getCharacter())){rescueRetireDialogue(dialogue->getCharacter());return false;}
    if(eventType==EV_PLAYER_TALK_TO_ME&&missionActive&&isMissionCommandSpeaker(dialogue->getCharacter())){
        GameData* conversation=ou?ou->gamedata.getData("880024-Guild Escort Contracts.mod",DIALOGUE):0;
        if(!conversation)return false;
        dialogue->clearConversationList(EV_PLAYER_TALK_TO_ME);
        dialogue->addConversation(conversation,EV_PLAYER_TALK_TO_ME);
    }
    Character* speaker=dialogue?dialogue->getCharacter():0;Character* collector=fiscalParty.leader.isNull()?0:fiscalParty.leader.getCharacter();
    if(!ZetaFixRules::mayStartVisitorDialogue(isDepartingVisitor(speaker))){diagnosticTrace("DIALOGUE blocked for departing visitor");return false;}
    if(speaker&&speaker==collector&&fiscalParty.organisation>=0&&fiscalParty.organisation<2&&fiscalParty.waitingForConversation)
    {
        FiscalOrganisation o=(FiscalOrganisation)fiscalParty.organisation;FiscalOrganisationState& state=fiscalLedger.organisations[fiscalParty.organisation];GameData* greeting=ou?ou->gamedata.getData("880232-Guild Escort Contracts.mod",DIALOGUE_LINE):0;
        if(greeting){std::stringstream text;if(!state.firstIntroductionDone){text<<(Loc::text("ui.i_represent"))<<mercenarieLocalize(fiscalOrgName(o))<<(Loc::text("ui.your_guild_now_operates_an_official_guild_house_so"))<<(o==FISCAL_UC?20:10)<<(gMercenarieEnglish?Loc::text("ui.reduced_to_5_for_an_allied_guild_a_collector_3f97493"):Loc::text("ui.reduced_to_5_for_an_allied_guild_a_collector"));}else{text<<(Loc::text("ui.i_represent"))<<mercenarieLocalize(fiscalOrgName(o))<<(Loc::text("ui.i_am_here_to_collect_the_share_owed_on"));}greeting->sdata["text0"]=text.str();}
    }
    diagnosticTrace("DIALOGUE event entering native");
    bool result;{MercenariePerf::Mode native(true);result=dialogueSendEventOriginal(dialogue, target, eventType);}
    diagnosticTrace("DIALOGUE event returned");
    return result;
}

void (*listPlayerRepliesOriginal)(Dialogue*);
void (*bountyReplyClickedOriginal)(Dialogue*,int);
    bool handleDelegatedPaymentReply(Dialogue* dialogue,const std::string& id)
    {
        const std::string prefix="mercenarie.delegated.pay|";if(id.find(prefix)!=0||!dialogue)return false;const std::string group=id.substr(prefix.size());Character* client=dialogue->getCharacter();
        if(guildVisitorOfferKey(client)!="PAY:"+group)return true;
        for(size_t i=0;i<delegatedMissions.size();++i){DelegatedMissionState& mission=delegatedMissions[i];if(mission.groupId!=group)continue;if(!mission.completed||!mission.success||mission.paymentState==3)return true;
            if(ou&&ou->player&&ou->player->participant&&ou->player->participant->factionOwnerships)payrollContractReward("delegated:"+mission.groupId,mission.reward,"finance.delegated",mission.groupId);mission.paymentState=3;totalContractCats+=mission.reward;saveReputations();dialogue->endDialogue(true);if(ou)ou->showPlayerAMessage(Loc::text("delegated.payment.received"),true);DebugLog(std::string("Delegated payment settled group=")+group+" cats="+FiscalLedger::toString((unsigned long)mission.reward));removeGuildVisitor(client,false);return true;}
        return true;
    }
    bool mailRecipientEligible(Character* recipient,const MailContracts::Contract& contract,const MailContracts::Step& step)
    {if(!recipient||recipient->isDead()||contract.status!=MailContracts::MailActive||step.delivered||mailRole(recipient)!=step.recipientRole)return false;TownBase* town=recipient->getCurrentTownLocation();if(!town||!town->getGameData()||town->getGameData()->stringID!=step.townId)return false;return step.recipientId.find("UNBOUND:")==0||step.recipientId==recipient->getHandle().toString();}
    bool mailDialogueCandidate(Character* recipient,Character* carrier,MailContracts::Contract& contract,MailContracts::Step& step)
    {if(!carrier||!mailRecipientEligible(recipient,contract,step))return false;Item* item=0;InventorySection* section=0;return mailFindItemOn(carrier,step.letterHandle,item,section)&&mailItemStep(item)==step.stepId;}
    std::string mailReplyId(const MailContracts::Contract& contract,const MailContracts::Step& step){return std::string("mercenarie.mail.deliver|")+contract.contractId+"|"+step.stepId;}

    std::string mailDeliveryBlock89(Character* recipient,MailContracts::Contract& mission,MailContracts::Step& step,Character*& carrier){
        if(missionWorldChanging||missionRestorePending||progressWriteBlocked||progressLoadFault)return Loc::text("mail89.wait");
        if(mission.status!=MailContracts::MailActive||step.delivered||mission.delegated)return Loc::text("mail89.inactive");
        if(!recipient||recipient->isDead()||mailRole(recipient)!=step.recipientRole)return Loc::text("mail89.recipient");
        TownBase* town=recipient->getCurrentTownLocation();
        if(!town||!town->getGameData()||town->getGameData()->stringID!=step.townId)return std::string(Loc::text("mail89.town"))+step.townName;
        if(!mailRecipientEligible(recipient,mission,step))return Loc::text("mail89.bound");
        Item* item=0;InventorySection* section=0;
        if(!mailResolveCarrier(step,carrier,item,section))return Loc::text("mail89.letter");
        if(carrier->isDead()||carrier->isBeingCarried()||(carrier->getMedical()&&carrier->getMedical()->isUnconcious())||(recipient->getMedical()&&recipient->getMedical()->isUnconcious()))return Loc::text("mail89.unavailable");
        if(carrier->getPosition().squaredDistance(recipient->getPosition())>625.0f)return std::string(Loc::text("mail89.near"))+carrier->getName();
        if(carrier->isInCombatMode(true,true)||recipient->isInCombatMode(true,true))return Loc::text("mail89.combat");
        if(recipient->getFaction()&&carrier->getFaction()&&recipient->getFaction()->relations&&recipient->getFaction()->relations->isEnemy(carrier->getFaction()))return Loc::text("mail89.hostile");
        return "";
    }
    bool performMailDelivery89(Character* recipient,MailContracts::Contract& mission,MailContracts::Step& step){
        Character* carrier=0;std::string reason=mailDeliveryBlock89(recipient,mission,step,carrier);
        if(!reason.empty()){if(ou)ou->showPlayerAMessage(reason,true);return false;}
                Item* item=0;InventorySection* section=0;if(!mailFindItemOn(carrier,step.letterHandle,item,section))return false;const std::string recipientHandle=recipient->getHandle().toString();if(step.recipientId.find("UNBOUND:")==0){step.recipientId=recipientHandle;step.recipientName=recipient->getName();}
                MailContracts::DeliveryResult result=MailContracts::deliver(mission,mission.contractId,step.letterHandle,recipientHandle,currentGameHours);if(result!=MailContracts::DeliveryAccepted)return false;if(carrier->getInventory())carrier->getInventory()->removeItemAutoDestroy(item,1);
                std::ostringstream notice;notice<<(Loc::text("v8.literal.063"))<<step.townName<<" ("<<MailContracts::deliveredCount(mission)<<"/"<<mission.steps.size()<<")";ou->showPlayerAMessage(notice.str(),true);DebugLog(std::string("Mail delivery: contract=")+mission.contractId+" step="+step.stepId+" recipient="+recipientHandle);
                if(mission.status==MailContracts::MailCompleted&&MailContracts::settleReward(mission)&&!rewardedContractIds.count(mission.contractId)){rewardedContractIds.insert(mission.contractId);if(ou&&ou->player&&ou->player->participant&&ou->player->participant->factionOwnerships)payrollContractReward("mail:"+mission.contractId,mission.reward,"finance.mail");GuildProgression::award(guildPoints,guildPrestige,mission.guildXp);escortReputation=GuildProgression::clampRep(escortReputation+1);float& local=localReputations[ReputationIdentity::key(mission.originTownName)];local=GuildProgression::clampRep(local+(float)mission.reputation);++successfulContracts;totalContractCats+=mission.reward;archiveMail(mission,1);if(contractHistory.size()>50)contractHistory.resize(50);saveReputations();ou->showPlayerAMessage(Loc::text("v8.literal.064"),true);}

        updateTrackerUI();return true;
    }
    bool handleMailReply(Dialogue* dialogue,const std::string& id){
        const std::string prefix="mercenarie.mail.deliver|";if(id.find(prefix)!=0||!dialogue)return false;
        size_t split=id.find('|',prefix.size());if(split==std::string::npos)return true;
        std::string contractId=id.substr(prefix.size(),split-prefix.size()),stepId=id.substr(split+1);
        for(size_t c=0;c<mailContracts.size();++c)if(mailContracts[c].contractId==contractId)
            for(size_t k=0;k<mailContracts[c].steps.size();++k)if(mailContracts[c].steps[k].stepId==stepId){if(performMailDelivery89(dialogue->getCharacter(),mailContracts[c],mailContracts[c].steps[k]))dialogue->endDialogue(true);return true;}
        if(ou)ou->showPlayerAMessage(Loc::text("mail89.inactive"),true);return true;
    }
#include "MailDeliveryView89.h"

void bountyReplyClickedHook(Dialogue* dialogue,int index){
    MercenariePerf::Event perf("DIALOGUE","reply-click");
    if(!dialogue||!bountyReplyClickedOriginal)return;
    if(MercenarieCleanup::disabled){bountyReplyClickedOriginal(dialogue,index);return;}
    if(!missionWorldChanging&&!missionRestorePending&&index>=0&&(size_t)index<dialogue->replyIds.size()){
        const std::string id=dialogue->replyIds[index];
        if(handleNegotiationOpen93(dialogue,id))return;
        if(isContractIntroduction(id)&&missionNativeGoalOwner(dialogue->getCharacter())>=0){dialogue->endDialogue(true);return;}
        selectQuestActor(dialogue->getCharacter());
        if(id=="mercenarie.mission.recover"){
            if(recoverMissionOrders(dialogue->getCharacter()))ou->showPlayerAMessage(Loc::text("mission.recovery.started"),true);
            dialogue->endDialogue(true);return;
        }
        if(missionActive&&contractLifecycle==CONTRACT_ACTIVE&&isMissionCommandSpeaker(dialogue->getCharacter())){
            DebugLog(std::string("MISSION COMMAND clicked id=")+id);
            if(id=="880060-Guild Escort Contracts.mod")pauseMission();
            else if(id=="880061-Guild Escort Contracts.mod")resumeMission();
            else if(id=="880062-Guild Escort Contracts.mod")followConversationPartner(dialogue);
            else if(id=="mercenarie.follow.stop")resumeMission();
            else if(id=="mercenarie.pace.up")setMissionPace(EscortPace::Accelerated);
            else if(id=="mercenarie.pace.down")setMissionPace(EscortPace::Normal);
            dialogue->endDialogue(true);
            return; // Never resolve injected replies against a native child index.
        }
        diagnosticTrace("BOUNTY clicked reply",id.c_str());
        if(handleDelegatedPaymentReply(dialogue,id))return;
        if(handleMailReply(dialogue,id))return;
        if(artisanReply(dialogue,id))return;
    selectQuestActor(dialogue->getCharacter());
    if(missionActive&&dialogue->getCharacter()!=escort&&rescueRetired(dialogue->getCharacter())){rescueRetireDialogue(dialogue->getCharacter());return;}
        Character* clickedSpeaker=dialogue->getCharacter();const bool clickedVisitor=isGuildVisitor(clickedSpeaker);
        if(id.find("88023")==0||id=="mercenarie.fiscal.pay")DebugLog(std::string("FISCAL clicked reply id=")+id);
        if(handleFiscalDialogueChoice(dialogue,clickedSpeaker,id))return;
        if(BetaFixRules::showGuildVisitorOffer(clickedVisitor,id)){
            selectGuildVisitorOffer(clickedSpeaker);
            dialogue->endDialogue(true);
            openContractsBoard(clickedSpeaker,false);
            return;
        }
        if(BetaFixRules::rejectWaitingVisitor(clickedVisitor,id)){
            const std::string offerKey=guildVisitorOfferKey(clickedSpeaker);
            if(!offerKey.empty()){savedContractBoards.erase(offerKey);saveContractBoards();}
            const char* reactions[]={Loc::text("guild.visitor.refusal.0"),Loc::text("guild.visitor.refusal.1"),Loc::text("guild.visitor.refusal.2"),Loc::text("guild.visitor.refusal.3"),Loc::text("guild.visitor.refusal.4"),Loc::text("guild.visitor.refusal.5")};
            if(clickedSpeaker)clickedSpeaker->sayALine(reactions[UtilityT::randomInt(0,5)],true);
            dialogue->endDialogue(true);
            removeGuildVisitor(clickedSpeaker,false);
            return;
        }
    if(BetaFixRules::interceptBarmanQuestReply(id)&&isBarman(dialogue->getCharacter())){openContractsBoard(dialogue->getCharacter(),false);return;}
        if(isMissionCommandSpeaker(dialogue->getCharacter())&&missionActive&&contractLifecycle==CONTRACT_ACTIVE){
            if(id=="mercenarie.pace.up"){setMissionPace(EscortPace::Accelerated);dialogue->endDialogue(true);return;}
            if(id=="mercenarie.pace.down"){setMissionPace(EscortPace::Normal);dialogue->endDialogue(true);return;}
        }
        if(id=="880063-Guild Escort Contracts.mod"&&dialogue->getCharacter()==escort&&missionPending){refuseNegotiationClicked(0);return;}
        if(id=="881500-Guild Escort Contracts.mod"||id=="881501-Guild Escort Contracts.mod"){
            Character* giver=dialogue->getCharacter();MercenarieV5::IssuerFaction faction;
            if(MercenarieV5::bountyIssuer(giver,faction)){
                if(id=="881500-Guild Escort Contracts.mod")openBountyOffers(giver);else handOverBounty(giver);
            }
            return; // Synthetic replies must not be resolved against vanilla child indices.
        }
    }
    {MercenariePerf::Mode native(true);bountyReplyClickedOriginal(dialogue,index);}
    if(!missionWorldChanging&&!missionRestorePending)observeVanillaBountyDelivery(dialogue->getCharacter());
}
void setMissionReply(Dialogue* d,const char* id,const char* fr,const char* en){d->replyIds.push_back(id);d->responses.push_back(gMercenarieEnglish?en:fr);}
// listPlayerReplies publishes the native menu before returning. Keep the final
// captions and click IDs paired, then republish changes through the native UI.
// A scope guard also covers custom reply branches which return early.
class MissionReplyPublication
{
    Dialogue* dialogue;
    Ogre::vector<std::string>::type nativeIds, nativeResponses;
public:
    explicit MissionReplyPublication(Dialogue* d):dialogue(d),nativeIds(d->replyIds),nativeResponses(d->responses){}
    ~MissionReplyPublication()
    {
        if(dialogue->conversationHasEnded())return;
        if(missionNativeGoalOwner(dialogue->getCharacter())>=0 && dialogue->replyIds.size()==dialogue->responses.size()){
            size_t kept=0;
            for(size_t i=0;i<dialogue->replyIds.size();++i){
                const std::string& id=dialogue->replyIds[i];
                if(isContractIntroduction(id)||id=="880063-Guild Escort Contracts.mod")continue;
                if(kept!=i){dialogue->replyIds[kept]=id;dialogue->responses[kept]=dialogue->responses[i];}
                ++kept;
            }
            dialogue->replyIds.resize(kept);dialogue->responses.resize(kept);
        }
        if(dialogue->replyIds.size()==dialogue->responses.size() &&
           (nativeIds!=dialogue->replyIds || nativeResponses!=dialogue->responses))
            dialogue->setResponesGUI();
    }
};
void listPlayerRepliesHook(Dialogue* dialogue)
{
    if(MercenarieCleanup::disabled){if(listPlayerRepliesOriginal)listPlayerRepliesOriginal(dialogue);return;}
    MercenariePerf::Event perf("DIALOGUE","replies");
    diagnosticTrace("DIALOGUE replies begin");
    if(!listPlayerRepliesOriginal||!dialogue)return;
    if(missionWorldChanging||missionRestorePending){{MercenariePerf::Mode native(true);listPlayerRepliesOriginal(dialogue);}
    artisanReplies(dialogue);return;}
    selectQuestActor(dialogue?dialogue->getCharacter():0);
    if(missionActive&&dialogue->getCharacter()!=escort&&rescueRetired(dialogue->getCharacter())){rescueRetireDialogue(dialogue->getCharacter());return;}
    prepareContractClientDialogue(dialogue);

    // Native UI construction no longer edits/restores the shared dialogue data.
    {MercenariePerf::Mode native(true);listPlayerRepliesOriginal(dialogue);}
    MissionReplyPublication publishReplies(dialogue);
    artisanReplies(dialogue);
    Character* speaker=dialogue->getCharacter();
    if(speaker){const std::string paymentKey=guildVisitorOfferKey(speaker);if(paymentKey.find("PAY:")==0){dialogue->replyIds.clear();dialogue->responses.clear();const std::string group=paymentKey.substr(4);dialogue->replyIds.push_back("mercenarie.delegated.pay|"+group);dialogue->responses.push_back(Loc::text("delegated.payment.accept"));diagnosticTrace("DIALOGUE delegated payment reply");return;}}
    Character* collector=fiscalParty.leader.isNull()?0:fiscalParty.leader.getCharacter();if(speaker&&speaker==collector&&fiscalParty.organisation>=0&&fiscalParty.organisation<2&&fiscalLedger.debt((FiscalOrganisation)fiscalParty.organisation)>0){bool present=false;for(size_t i=0;i<dialogue->replyIds.size();++i)if(dialogue->replyIds[i]=="mercenarie.fiscal.pay")present=true;if(!present)setMissionReply(dialogue,"mercenarie.fiscal.pay",Loc::text("fiscal.dialogue.pay_now"),Loc::text("fiscal.dialogue.pay_now"));}
    if(speaker&&mailRole(speaker)!=MailContracts::RoleUndefined){for(size_t c=0;c<mailContracts.size();++c){MailContracts::Contract& mission=mailContracts[c];if(mission.status!=MailContracts::MailActive||mission.delegated)continue;for(size_t k=0;k<mission.steps.size();++k){MailContracts::Step& step=mission.steps[k];if(step.delivered)continue;std::string id=mailReplyId(mission,step);bool present=false;for(size_t r=0;r<dialogue->replyIds.size();++r)if(dialogue->replyIds[r]==id)present=true;if(!present){dialogue->replyIds.push_back(id);dialogue->responses.push_back(std::string(Loc::text("mail.deliver.reply"))+" - "+step.townName);}}}}

    if(speaker&&isBarman(speaker)){
        bool present=false;for(size_t i=0;i<dialogue->replyIds.size();++i)if(dialogue->replyIds[i]=="880017-Guild Escort Contracts.mod")present=true;
        if(!present)setMissionReply(dialogue,"880017-Guild Escort Contracts.mod",Loc::text("ui.do_you_have_any_contracts_for_my_guild"),Loc::text("ui.do_you_have_contracts_for_my_guild"));
    }
    if(speaker&&speaker->getGameData()){
        MercenarieV5::IssuerFaction faction;const bool eligible=MercenarieV5::bountyIssuer(speaker,faction);
        std::stringstream report;report<<"actor="<<speaker->getGameData()->stringID<<" faction="<<(speaker->getFaction()&&speaker->getFaction()->data?speaker->getFaction()->data->stringID:"none")<<" eligible="<<eligible;
        diagnosticTrace("BOUNTY replies",report.str().c_str());
        if(eligible&&ou&&ou->gamedata.getData("881500-Guild Escort Contracts.mod",DIALOGUE_LINE)){
            bool present=false;for(size_t i=0;i<dialogue->replyIds.size();++i)if(dialogue->replyIds[i]=="881500-Guild Escort Contracts.mod")present=true;
            if(!present)setMissionReply(dialogue,"881500-Guild Escort Contracts.mod",MercenarieV5::bountyText(MercenarieV5::AskContracts,true),MercenarieV5::bountyText(MercenarieV5::AskContracts,false));
            diagnosticTrace("BOUNTY offer present");
        }else if(eligible)diagnosticTrace("BOUNTY dialogue record missing");
    }
    if(BetaFixRules::guildVisitorOfferDialogue(isGuildVisitor(speaker))&&missionNativeGoalOwner(speaker)<0){
        dialogue->replyIds.clear();dialogue->responses.clear();
        setMissionReply(dialogue,"mercenarie.guildvisitor.show",Loc::text("guild.visitor.show_offer"),Loc::text("guild.visitor.show_offer"));
        setMissionReply(dialogue,"mercenarie.guildvisitor.refuse",Loc::text("guild.visitor.refuse_offer"),Loc::text("guild.visitor.refuse_offer"));
    }else if(speaker&&speaker==escort&&missionPending&&missionNativeGoalOwner(speaker)<0){
        dialogue->replyIds.clear();dialogue->responses.clear();
        setMissionReply(dialogue,"880027-Guild Escort Contracts.mod",Loc::text("ui.that_is_us_let_us_discuss_the_contract_and"),Loc::text("ui.that_is_us_let_us_discuss_the_contract_and"));
        const char* frRefusals[]={Loc::text("ui.bad_news_we_will_not_be_taking_this_contract"),Loc::text("ui.we_are_no_longer_interested_in_this_contract"),Loc::text("ui.sorry_find_another_company"),Loc::text("ui.we_will_pass_after_all_find_some_other_mercenaries")};
        const char* enRefusals[]={Loc::text("ui.bad_news_we_will_not_be_taking_this_contract"),Loc::text("ui.we_are_no_longer_interested_in_this_contract"),Loc::text("ui.sorry_find_another_company"),Loc::text("ui.we_will_pass_after_all_find_some_other_mercenaries")};
        int refusalIndex=UtilityT::randomInt(0,3);
        setMissionReply(dialogue,"880063-Guild Escort Contracts.mod",frRefusals[refusalIndex],enRefusals[refusalIndex]);
    }else if(isMissionCommandSpeaker(speaker)&&missionActive&&contractLifecycle==CONTRACT_ACTIVE){
        dialogue->replyIds.clear();dialogue->responses.clear();
        if(missionFollowing)setMissionReply(dialogue,"mercenarie.follow.stop",Loc::text("mission.follow.stop_resume"),Loc::text("mission.follow.stop_resume"));
        else setMissionReply(dialogue,"880062-Guild Escort Contracts.mod",Loc::text("ui.follow_me"),Loc::text("ui.follow_me"));
        setMissionReply(dialogue,"880060-Guild Escort Contracts.mod",Loc::text("ui.wait_here_for_a_moment"),Loc::text("ui.wait_here_for_a_moment"));
        setMissionReply(dialogue,"880061-Guild Escort Contracts.mod",Loc::text("ui.we_can_continue_on_our_way"),Loc::text("ui.we_can_continue_on_our_way"));
        if(missionPace==EscortPace::Normal)setMissionReply(dialogue,"mercenarie.pace.up",Loc::text("ui.pick_up_the_pace"),Loc::text("ui.pick_up_the_pace"));
        else setMissionReply(dialogue,"mercenarie.pace.down",Loc::text("ui.slow_down"),Loc::text("ui.slow_down"));
        if(canRecoverMission(speaker))setMissionReply(dialogue,"mercenarie.mission.recover",Loc::text("mission.recovery.reply"),Loc::text("mission.recovery.reply"));
        setMissionReply(dialogue,"184-gamedata.quack",Loc::text("ui.see_you_later"),Loc::text("ui.see_you_later"));
    }else if(speaker&&speaker->getGameData()&&speaker->getGameData()->stringID=="970002-Holy Nation Mercenary Plastron.mod"){
        dialogue->replyIds.clear();dialogue->responses.clear();
        setMissionReply(dialogue,"970006-Holy Nation Mercenary Plastron.mod",Loc::text("ui.i_would_like_to_buy_armour"),Loc::text("ui.i_would_like_to_buy_armour"));
        setMissionReply(dialogue,"970014-Guild Escort Contracts.mod",Loc::text("ui.i_would_like_to_buy_trousers"),Loc::text("ui.i_would_like_to_buy_trousers"));
        setMissionReply(dialogue,"970013-Guild Escort Contracts.mod",Loc::text("ui.i_would_like_to_buy_a_helmet"),Loc::text("ui.i_would_like_to_buy_a_helmet"));
        setMissionReply(dialogue,"970007-Holy Nation Mercenary Plastron.mod",Loc::text("ui.no_thank_you"),Loc::text("ui.no_thank_you"));
    }
    if(canRecoverMission(speaker)){
        bool present=false;for(size_t i=0;i<dialogue->replyIds.size();++i)if(dialogue->replyIds[i]=="mercenarie.mission.recover")present=true;
        if(!present)setMissionReply(dialogue,"mercenarie.mission.recover",Loc::text("mission.recovery.reply"),Loc::text("mission.recovery.reply"));
    }
    diagnosticTrace("DIALOGUE replies completed");
}

// Filter the native result, not the shared source list being traversed by Kenshi.
bool appendMercenarieReply(lektor<DialogLineData*>& out,const char* id)
{
    if(!ou||out.count>out.maxSize||out.maxSize>4096||(out.maxSize&&!out.stuff))return false;
    for(unsigned int i=0;i<out.count;++i)if(out.stuff[i]&&out.stuff[i]->getStringID()==id)return true;
    GameData* data=ou->gamedata.getData(id,DIALOGUE_LINE);
    DialogLineData* reply=data?DialogDataManager::getData(data):0;
    if(!reply)return false;
    if(out.count==out.maxSize){
        // Same allocator and memory category as lektor; no stack-owned backing array.
        Ogre::STLAllocator<DialogLineData*,Ogre::CategorisedAllocPolicy<Ogre::MEMCATEGORY_GENERAL> > allocator;
        const unsigned int capacity=std::max(8u,out.maxSize*2);
        DialogLineData** replacement=allocator.allocate(capacity);
        if(!replacement)return false;
        for(unsigned int i=0;i<out.count;++i)replacement[i]=out.stuff[i];
        if(out.stuff)allocator.deallocate(out.stuff,out.maxSize);
        out.stuff=replacement;out.maxSize=capacity;
    }
    out.stuff[out.count++]=reply;
    return true;
}
void (*getPlayerRepliesOriginal)(DialogLineData*,lektor<DialogLineData*>&,Dialogue*,Character*);
void filterAcceptedMissionReplies(lektor<DialogLineData*>& out){
    unsigned int kept=0;
    for(unsigned int i=0;i<out.count;++i){DialogLineData* choice=out.stuff[i];if(choice&&!isContractIntroduction(choice->getStringID())&&choice->getStringID()!="880063-Guild Escort Contracts.mod")out.stuff[kept++]=choice;}
    out.count=kept;
}
void getPlayerRepliesHook(DialogLineData* node,lektor<DialogLineData*>& out,Dialogue* dialogue,Character* target)
{
    if(MercenarieCleanup::disabled){if(getPlayerRepliesOriginal)getPlayerRepliesOriginal(node,out,dialogue,target);return;}
    if(!getPlayerRepliesOriginal)return;
    if(missionWorldChanging||missionRestorePending){getPlayerRepliesOriginal(node,out,dialogue,target);return;}
    selectQuestActor(dialogue?dialogue->getCharacter():0);
    Character* before=dialogue?dialogue->getCharacter():0;
    const std::string nodeId=node?node->getStringID():"";
    hand speakerHandle;if(before)speakerHandle=before->getHandle();else speakerHandle.setNull();
    getPlayerRepliesOriginal(node,out,dialogue,target);
    if(out.count>out.maxSize||(out.count&&!out.stuff))return;
    Character* speaker=speakerHandle.isNull()?0:speakerHandle.getCharacter();
    if(speaker&&isBarman(speaker))appendMercenarieReply(out,"880017-Guild Escort Contracts.mod");
    MercenarieV5::IssuerFaction bountyFaction;
    if(speaker&&MercenarieV5::bountyIssuer(speaker,bountyFaction)){
        GameData* data=ou?ou->gamedata.getData("881500-Guild Escort Contracts.mod",DIALOGUE_LINE):0;
        if(data){
            const char* text=MercenarieV5::bountyText(MercenarieV5::AskContracts,!gMercenarieEnglish);
            data->sdata["text0"]=text;
            DialogLineData* reply=DialogDataManager::getData(data);
            if(reply&&reply->texts)reply->texts[0]=text;
            appendMercenarieReply(out,"881500-Guild Escort Contracts.mod");
        }
        if(bountyWorld.contract.state==MercenarieV5::BountyActive&&MercenarieV5::bountyIdentity(speaker->getHandle())==bountyWorld.contract.offer.issuer&&ou&&ou->player){
            for(size_t i=0;i<ou->player->playerCharacters.size();++i)if(MercenarieV5::playerCarriesBounty(bountyWorld.contract,ou->player->playerCharacters[i],ou->player->getFaction())){
                GameData* handover=ou->gamedata.getData("881501-Guild Escort Contracts.mod",DIALOGUE_LINE);
                if(handover){const char* caption=MercenarieV5::bountyText(MercenarieV5::HavePrisoner,!gMercenarieEnglish);handover->sdata["text0"]=caption;DialogLineData* line=DialogDataManager::getData(handover);if(line&&line->texts)line->texts[0]=caption;appendMercenarieReply(out,"881501-Guild Escort Contracts.mod");}break;
            }
        }
    }
    if(speaker&&nodeId=="970005-Holy Nation Mercenary Plastron.mod"){
        appendMercenarieReply(out,"970006-Holy Nation Mercenary Plastron.mod");
        appendMercenarieReply(out,"970013-Guild Escort Contracts.mod");
        appendMercenarieReply(out,"970014-Guild Escort Contracts.mod");
    }
    // Resolve ownership independently of the selected quest: a locked UI or a
    // group member must never expose another quest's pending introduction.
    const bool acceptedSpeaker=missionNativeGoalOwner(speaker)>=0;
    if(acceptedSpeaker){filterAcceptedMissionReplies(out);return;}
    bool visitor=speaker&&isGuildVisitor(speaker);
    Character* current=escortHandle.isNull()?0:escortHandle.getCharacter();
    if(!visitor&&(!speaker||speaker!=current))return;
    unsigned int kept=0;
    for(unsigned int i=0;i<out.count;++i){
        DialogLineData* choice=out.stuff[i];if(!choice)continue;
        const std::string id=choice->getStringID();
        if(BetaFixRules::missionChoiceVisible(visitor,missionPending,missionActive,missionPaused,missionFollowing,id))
            out.stuff[kept++]=choice;
    }
    out.count=kept;
    diagnosticTrace("DIALOGUE reply result filtered");
}

void (*mainLoopOriginal)(GameWorld*, float);
void (*mercenarieGuiFrameOriginal)(MyGUI::Gui*,float);
void mercenarieGuiFrameHook(MyGUI::Gui* gui,float elapsed){mercenarieGuiFrameOriginal(gui,elapsed);detectMercenarieLanguage();if(MercenarieCleanup::disabled)return;updateRerollPopupLayout();MercenarieFonts::languageChoice(gui->findWidget<MyGUI::ComboBox>("MercenarieLanguageChoice",false));MainMenuNews::update();MainMenuBackground::update();if(ou&&ArtisanClock::canObserveUi(artisanWindow!=0,!activeMercenarieSaveSlot.empty(),missionWorldChanging,missionRestorePending))artisanTick();}
void (*mercenarieGuiShutdownOriginal)(MyGUI::Gui*);
void mercenarieGuiShutdownHook(MyGUI::Gui* gui){releaseMissionUIBeforeLoad();MainMenuNews::guiShuttingDown();mercenarieGuiShutdownOriginal(gui);}
#include "MissionUIAccess.h"
#include "GuildBodyCompatibilityRuntime.h"
#include "GuildSkeletonScaleRuntime.h"
void mainLoopHook(GameWorld* world, float frameTime)
{
    if(!MercenarieCleanup::disabled)applyGuildBodyCompatibility();
    mainLoopOriginal(world, frameTime);
    if(MercenarieCleanup::disabled){flushPendingMissionSave();cleanupDisabledFrame();return;}
    if(world)currentGameHours=WorldServiceClock::now(world->getTimeStamp_inGameHours().getTotalHours(),developerTimeOffsetHours);
    MissionAbsencePrototype::update(frameTime);
    if(missionWorldChanging)return;
    updateMercenarieUIShell(world);
    GuardRest::update();GuardSalute::update();
    flushPendingMissionSave();
    const bool alreadyHasProgressSlot=!activeMercenarieSaveSlot.empty();
    if(configureMercenarieDataSlot())
    {
        // A save-as rename is not a world load. Retain the live progression;
        // load/new-game hooks explicitly clear the slot before changing worlds.
        if(alreadyHasProgressSlot){saveReputations();saveFiscalLedger();saveContractBoards();}
        else {
        rewardedContractIds.clear();fiscalLedger=FiscalLedger();successfulContracts=failedContracts=0;totalContractCats=totalAdvances=totalBonuses=totalTips=0;escortReputation=0;guildPoints=0;guildPrestige=0;guildInvestmentNextHour=0;contractHistory.clear();contractSeeds.clear();localReputations.clear();cityMemories.clear();archivedReputationAliases.clear();guildHouseNames.clear();guildHouseCities.clear();guildHouseOriginalNames.clear();designatedGuildHouseKey.clear();operationalAnnouncementKey.clear();designatedGuildHouseHandle.setNull();savedContractBoards.clear();contractBoardsLoaded=false;loadReputations();loadFiscalLedger();loadContractBoards();scheduleGuildFurnitureRecovery();
        }
    }
    updateGuildFurnitureRecovery(frameTime);
    updateMissionBookLevelAccess(); // Progress is loaded even when saved mission actors are still streaming.
    if(missionRestorePending&&!restoreMissionState()){
        missionRestoreClock+=frameTime;
        if(missionRestoreWaitingForGroup&&missionRestoreClock>15&&!missionRestoreNotice){missionRestoreNotice=true;diagnosticTrace("MISSION awaiting saved actors");if(ou)ou->showPlayerAMessage(Loc::text("ui.guild_mission_waiting_for_the_saved_group_to_load"),true);}
        return;
    }
    updateSavedAbsenceRestore(frameTime);
    // The restored archive can replace the world's saved developer offset.
    if(world)currentGameHours=WorldServiceClock::now(world->getTimeStamp_inGameHours().getTotalHours(),developerTimeOffsetHours);
    if(progressLoadFault)progressWriteBlocked=true;
    static bool progressWarning=false;
    if(progressWriteBlocked){if(!progressWarning&&ou){progressWarning=true;reportPersistenceError("PROGRESS",SaveDiagnostics::ProgressUnavailable,"guild actions blocked; inspect preceding persistence error",reputationFile,"ui.guild_progress_unavailable_guild_actions_paused_to_protect_your");}return;}progressWarning=false;
    std::string oldLanguage=Loc::engine().language;detectMercenarieLanguage();if(oldLanguage!=Loc::engine().language)fcsLanguageApplied=false;
    mercenarieProtectedNames.clear();
    if(ou&&ou->player){if(ou->player->getFaction())mercenarieProtectedNames.push_back(ou->player->getFaction()->name);for(size_t i=0;i<ou->player->playerCharacters.size();++i){Character* c=ou->player->playerCharacters[i];if(c)mercenarieProtectedNames.push_back(c->getName());}}
    for(std::map<std::string,std::string>::const_iterator i=guildHouseNames.begin();i!=guildHouseNames.end();++i)mercenarieProtectedNames.push_back(i->second);
    applyFcsRuntimeLanguage();
    updateGuildResearchAccess();
    updateRefusedDepartures(frameTime); // Must run even if the new client is not loaded.


    artisanTick();
    estateTick();
    payrollSync();updatePayrollWindows();
    financeObserve();
    updateDelegatedMission();updateMailMissions(frameTime);
    fiscalUpdateClock-=frameTime;if(fiscalUpdateClock<=0){fiscalUpdateClock=1.0;updateFiscalSystem();}
    updateFiscalParty(frameTime);
    initialiseTrackerUI();createDynamicTracker();updateAutopilotUI(frameTime);
    updateTrackerUI();
    if(optionsRebuildRequested){optionsRebuildRequested=false;rebuildGuildMenuForViewport();createGuildMenu();if(guildWindow){guildWindow->setVisible(true);guildTabClicked(guildTabButtons[5]);}return;}
    const bool assigningGuildKey=mercenarieUIKeyCaptureAtFrameStart;
    updateGuildLevelUpUI();
    if(assigningGuildKey&&key&&key->keyboard){int cheat=clientOptions.bindings[ClientOptions::OpenCheatMenu],autopilot=clientOptions.bindings[ClientOptions::OpenAutopilot];pWasDown=cheat&&key->keyboard->isKeyDown((OIS::KeyCode)cheat);autopilotWasDown=autopilot&&key->keyboard->isKeyDown((OIS::KeyCode)autopilot);}
    else{updateDeveloperMenuHotkey();updateAutopilotHotkey();}
    gbRecenter();
    const bool buildingEscapeConsumed=gbKeyboard();
    updateDefineGuildHouseButton();
    updateMissionBookNativeUse(frameTime);
    guildFacilityScanClock-=frameTime;if(guildFacilityScanClock<=0.0f){guildFacilityScanClock=10.0f;refreshGuildFurniture(false);}updateGuildVisitors(frameTime);
    if (guildWindow && guildWindow->getVisible()) updateGuildMenu();
    refreshGuildInvestment();
    if(contractsWindow&&contractsWindow->getVisible()&&missionBookDelegationContext&&missionBookViewportChanged())updateContractsBoard();
    if(contractsWindow&&contractsWindow->getVisible()&&missionBookPresentationActive)refreshMissionBookVisualDetails();
    contractBoardUiClock-=frameTime;if(contractBoardUiClock<=0.0f&&contractsWindow&&contractsWindow->getVisible()&&!currentBoardKey.empty() ){contractBoardUiClock=1.0f;if(missionBookDelegationContext){tickMissionBookPool();}else {loadContractBoards();std::map<std::string,CityContractBoard>::iterator activeBoard=savedContractBoards.find(currentBoardKey);if(activeBoard!=savedContractBoards.end()){if(activeBoard->second.expiresAt<=currentGameHours&&contractBarman&&!missionActive&&!missionPending)openContractsBoard(contractBarman,false);else if(contractsRefreshText){int remaining=std::max(0,static_cast<int>(activeBoard->second.expiresAt-currentGameHours));std::stringstream refresh;refresh<<std::count_if(boardOffers,boardOffers+6,boardOfferAvailable)<<" "<<mercenarieLocalize(Loc::text("ui.offers"))<<"  -  "<<mercenarieLocalize(Loc::text("ui.next_rotation_in"))<<" "<<(remaining/24)<<" "<<mercenarieLocalize(Loc::text("ui.days"))<<" "<<(remaining%24)<<Loc::text("ui.h");MercenarieFonts::caption(contractsRefreshText,refresh.str());if(missionBookDelegationContext)refreshMissionBookOfferCounts();}}}}
    if(key && key->keyboard)
    {
        bool escapeDown=key->keyboard->isKeyDown(OIS::KC_ESCAPE);
        if(escapeDown && !escapeWasDown && !assigningGuildKey && !buildingEscapeConsumed)
        {
            if(guildLevelUpWindow && guildLevelUpWindow->getVisible()) guildLevelUpClosed(0);
            else if(contractDecisionWindow && contractDecisionWindow->getVisible()) suspendNegotiation();
            else if(negotiationWindow && negotiationWindow->getVisible()) suspendNegotiation();
            else if(contractsWindow && contractsWindow->getVisible()) contractsWindowButtonPressed(contractsWindow,"close");
            else if(guildWindow && guildWindow->getVisible()){
                if(guildInvestmentOverlay&&guildInvestmentOverlay->getVisible())closeGuildInvestment(0);
                else guildWindow->setVisible(false);
            }
            else if(finalWindow && finalWindow->getVisible()) finalWindowButtonPressed(finalWindow,"close");
        }
        escapeWasDown=escapeDown;
    }
    tickAllQuests(frameTime);tickEscortPersonnel();
    updateCaravanTradeHUD();
    tickCaravanCustomers(frameTime);
}

class MapScreen;
namespace {
void tickEscortMission(float frameTime){
    if((missionActive||missionPending)&&!escortHandle.isNull()){
        Character* resolved=escortHandle.getCharacter();if(!resolved)return;escort=resolved;
    }
    if(completedEscort||completedEscortRestore.pending||completedCaravanRestore.pending)
    {
        if(!completedEscortRestore.resolve(completedEscort,frameTime)||!completedCaravanRestore.resolve(completedCaravan,frameTime))return;
        completedCleanupClock-=frameTime;
        if(completedCleanupClock<=0.0f)
        {
            completedEscort=0;
            DepartingVisitors departing;for(unsigned int i=0;i<completedCaravan.size();++i){Character* leaving=completedCaravan[i];if(!leaving)continue;leaving->removeJob(BODYGUARD);leaving->removeJob(FOLLOW_SQUADLEADER);leaving->removeJob(FOLLOW_PLAYER_ORDER);leaving->removeJob(MOVE_CUS_ORDERED);leaving->removeJob(HOLD_POSITION);disableDepartingVisitorDialogue(leaving);departing.members.push_back(leaving);}if(!departing.members.empty())departingVisitors.push_back(departing);
            completedCaravan.clear();
        }
    }
    if(finalWindow&&finalWindow->getVisible())return; // Never re-roll an unsettled report.
    if(!missionActive&&missionFollowing)clearMissionFollow();
    if(!missionPaused&&!missionPending&&!waitingHere.empty())releaseWaitingHere();
    if ((!missionActive && !missionPending) || !escort) return;

    enforceWaitingHere(); // Every frame, not the one-second mission tick.
    // Native goals may change pace between mission ticks. Keep travel pace
    // stable each frame instead of letting the leader sprint for a full tick.
    if(missionActive&&!missionCasualtyWaiting)applyMissionPace();
    missionMotionSample();
    // Release movement immediately, without waiting for the one-second tick.
    const bool combatPriority=missionCombatThreat(escort);
    if(combatPriority){enterDefensiveCombat();missionYieldTravelToCombat(escort);}
    for(size_t i=0;i<progressMembers.size();++i)missionYieldTravelToCombat(progressMembers[i].getCharacter());
    if(!missionGuidancePending()&&!missionRescue.legacy.active&&!caravanTradeActive()&&!combatPriority&&!missionDoorApproachTick(frameTime)){
        const bool gateTargetChanged=missionGateTick(frameTime);
        if(gateTargetChanged||missionRoadAdvance()){
            if(!missionDoorApproachTick(0,true))issueTravelOrder(destination,"next road point");
        }
    }
    updateClock += frameTime;
    if (updateClock < 1.0f) return;
    const float elapsed = updateClock;
    updateClock = 0.0f;
    if (missionActive) missionElapsed += elapsed;
    missionRescue.waitNoticeClock=std::max(0.0f,missionRescue.waitNoticeClock-elapsed);

    if(missionActive){
        missionRescue.navigationLogClock-=elapsed;
        if(missionRescue.navigationLogClock<=0){
            missionOrderTrace(escort,"navigation sample");
            std::ostringstream status;status<<"MISSION NAV STATE id="<<currentMissionFiscalId<<" casualty_wait="<<missionCasualtyWaiting<<" regroup="<<missionRescue.regrouping<<" paused="<<missionPaused<<" following="<<missionFollowing<<" leaving_building="<<leavingBuilding<<" exit="<<exitWaypoint;DebugLog(status.str());
            missionRescue.navigationLogClock=5;
        }
    }
    observeProgressMembers();
    if(escort->getMedical()&&escort->getMedical()->isUnconcious())escortWasKnockedOut=true;
    if(missionActive&&!progressMembers.empty()&&progressDeadMembers.size()==progressMembers.size()){cancelMission("ui.contract_canceled_the_traveler_is_dead");return;}
    const bool groupBattleActive=tickMissionGroupDefence();
    if(missionRescue.motionSuspended){missionWaitReason(MissionMotionPolicy::Suspended);return;}
    suspendCaravanTrade();
    tickCaravanHomeAmbushFlight(elapsed);
    const bool houseBattleActive=tickCaravanAmbushBattle(elapsed);
    missionRescue.navigationRP.awaitingGuide=missionGuidancePending();
    if(missionActive&&tickMissionRescue(elapsed)){missionRescue.leaderWatch.fresh();return;}
    if(houseBattleActive||groupBattleActive)return;
    missionNavigationRPTick(elapsed);
    if(missionGuidancePending())return;
    updateMissionFormation(elapsed);

    if (escortIsInCage())
    {
        cancelMission("ui.contract_canceled_the_traveler_was_put_in_a_cage");
        return;
    }

    importantAlertClock=std::max(0.0f,importantAlertClock-elapsed);
    if (escort->getMedical()->isUnconcious()) { unconsciousSeconds += elapsed; escortWasKnockedOut = true;if(importantAlertClock<=0.0f&&lastImportantAlert!="ko"){ou->showPlayerAMessage(Loc::text("ui.contract_alert_the_customer_is_unconscious"),true);lastImportantAlert="ko";importantAlertClock=30.0f;} }
    else unconsciousSeconds = 0.0f;
    // KO alone never cancels the contract. Carrying is legitimate assistance;
    // only a persistent abnormal separation is treated as kidnapping below.

    // Combat interrupts both autonomous travel and FOLLOW before either can
    // repair its movement order. Native combat owns movement until it ends.
    if(missionCombatThreat(escort)){enterDefensiveCombat();missionRescue.leaderWatch.fresh();return;}
    if(wasInCombat)leaveDefensiveCombat();
    if(tickCaravanTrade(elapsed))return;
    const bool routeTestInProgress=tickRouteTest(elapsed);
    if (missionPaused){
        missionRescue.leaderWatch.fresh();missionWaitReason(MissionMotionPolicy::Player);return;
    }
    if(missionFollowing&&!scientificResearching&&!leavingBuilding){
        Character* target=missionFollowTarget.getCharacter();
        if(target&&target->isDead()){clearMissionFollow();pauseMission();}
        else if(target&&missionFollowingAtDestination()){
            if((!caravanMission&&!scientificMission)||(caravanMission&&caravanReturning)||(scientificMission&&scientificReturning)){finishMission(true);return;}
            else if(scientificMission&&!scientificReturning){beginScientificActivity();return;}
            else if(caravanMission&&!caravanReturning){beginCaravanTrade();return;}
        }
        if(!target)missionWaitReason(MissionMotionPolicy::Unavailable);
        else if(!escort->isInCombatMode(true,true)&&!escort->getMedical()->isUnconcious()&&!scientificResearching){
            if(escort->getPosition().squaredDistance(target->getPosition())>1600){
                const MissionMotionPolicy::Action action=missionRescue.leaderWatch.tick(escort->getPosition(),target->getPosition(),escort->getMovement()->isCurrentlyMoving(),escort->getMovement()->pathFailed()||!missionFollowerTargetValid(escort,target,false),elapsed,true);
                missionRecoverFollower(escort,target,action,false);
            }else missionRescue.leaderWatch=MissionMotionPolicy::Watch();
        }else missionRescue.leaderWatch.fresh();
        // An unloaded target is not a reason to substitute another player or resume travel.
        return;
    }
    if (missionPending) return;

    Character* playerCarrier=playerCarrierOfMissionMember();
    EscortMissionRules::SeparationResult separation=EscortMissionRules::updateSeparation(missionSeparation,abnormalPlayerCarry(playerCarrier),elapsed);
    if(separation==EscortMissionRules::SeparationWarn)ou->showPlayerAMessage(Loc::text("ui.you_are_moving_away_with_a_client_return_to_the_mission"),true);
    else if(separation==EscortMissionRules::SeparationFail){failKidnappingContract();return;}
    if(!playerCarrier)carriedDestinationDistance=-1;

    const Ogre::Vector3 position = escort->getPosition();
    if(missionActive){journeyData.distanceTravelled+=Ogre::Math::Sqrt(position.squaredDistance(lastJourneyPosition));lastJourneyPosition=position;journeyData.seconds=missionElapsed;journeyData.combats=journeyCombatCount;journeyData.knockedOut=escortWasKnockedOut;if(selectedDistance>0&&journeyData.distanceTravelled>selectedDistance*1.35f)journeyData.detours=1;if(currentContract.danger>=1.5f)journeyData.dangerousRegions=1;journeySeconds+=elapsed;}

    if(scientificResearching)
    {
        scientificResearchSeconds-=elapsed;scientificMoveClock-=elapsed;scientificCommentClock-=elapsed;
        if(!scientificEntryAttempted && scientificResearchSeconds<150.0f)
        {
            scientificEntryAttempted=true;scientificWillEnter=UtilityT::random(0.0f,100.0f)<35.0f;
            if(scientificWillEnter){scientificEntryClock=22.0f;escort->sayALine(Loc::text("ui.this_door_may_still_give_way_i_will_try"),true);missionClearTravel(escort,"science entry");if(missionScienceSelectBuilding())missionIssueOrder(escort,UNLOCK_DOOR_HERE,0,missionScienceEntryPoint());else scientificEntryClock=0;ou->showPlayerAMessage(Loc::text("ui.the_chief_scientist_tries_to_pick_the_entrance_to"),true);}
        }
        if(scientificWillEnter&&!scientificEntryResolved)
        {
            scientificEntryClock-=elapsed;
            if(missionScienceInsideExpected()){scientificEntryResolved=true;scientificInsideDiscovery=true;escort->sayALine(Loc::text("ui.the_lock_gave_way_these_rooms_could_contain_vital"),true);ou->showPlayerAMessage(Loc::text("ui.entry_successful_the_expedition_continues_its_search_inside"),true);scientificMoveClock=0.0f;}
            else if(scientificEntryClock<=0.0f){scientificEntryResolved=true;escort->sayALine(Loc::text("ui.impossible_the_mechanism_is_too_damaged_let_s_continue"),true);ou->showPlayerAMessage(Loc::text("ui.lockpick_fails_search_continues_around_the_ruins"),true);scientificMoveClock=0.0f;}
        }
        if(scientificMoveClock<=0.0f && (!scientificWillEnter||scientificEntryResolved))
        {
            scientificMoveClock=UtilityT::random(10.0f,18.0f);float angle=UtilityT::random(0.0f,6.28318f);float radius=scientificInsideDiscovery?UtilityT::random(3.0f,10.0f):UtilityT::random(12.0f,34.0f);Ogre::Vector3 point=scientificRuinCenter;point.x+=Ogre::Math::Cos(angle)*radius;point.z+=Ogre::Math::Sin(angle)*radius;
            Ogre::Vector3 valid;
            if(escort&&!escort->isDead()&&missionSciencePoint(point,valid)){missionClearTravel(escort,"science valid local point");missionIssueOrder(escort,MOVE_CUS_ORDERED,0,valid);missionRescue.localRecoveryTarget=valid;}
            updateMissionFormation(0);
        }
        if(scientificCommentClock<=0.0f)
        {
            const char* observations[]={Loc::text("ui.these_foundations_of_this_site_are_much_older_than"),Loc::text("ui.look_at_these_marks_on_the_ruins_someone_wanted"),Loc::text("ui.this_structure_has_stood_for_centuries_its_function_must"),Loc::text("ui.write_everything_down_this_site_could_change_our_reading"),Loc::text("ui.these_debris_are_not_arranged_randomly_a_battle_or"),Loc::text("ui.the_materials_from_this_ruin_reveal_lost_technology")};
            scientificCommentClock=UtilityT::random(24.0f,38.0f);Character* speaker=scientificMembers.empty()?escort:scientificMembers[UtilityT::randomInt(0,static_cast<int>(scientificMembers.size())-1)];if(speaker)speaker->sayALine(observations[UtilityT::randomInt(0,5)],true);
        }
        if(scientificResearchSeconds<=0.0f)
        {
            scientificResearching=false;scientificReturning=true;destination=scientificOrigin;journeyStart=position;journeyDistanceSquared=position.squaredDistance(destination);quarterSpeech=false;midpointChecked=false;finalSpeech=false;leavingBuilding=escort->getMovement()->isIndoors();missionRescue.failedExitDoors.clear();missionRescue.leaderWatch=MissionMotionPolicy::Watch();if(leavingBuilding&&!findExteriorWaypoint(escort,exitWaypoint))missionSuspendMotion();else resumeRecoveredMissionPhase();escort->sayALine(Loc::text("ui.we_have_obtained_enough_information_it_s_time_to"),true);ou->showPlayerAMessage(Loc::text("ui.new_objective_accompany_the_scientists_back_to_their_city"),true);
        }
        return;
    }

    if (leavingBuilding)
    {
        if (!escort->getMovement()->isIndoors())
        {
            leavingBuilding = false;
            missionRescue.passage.clear();missionRescue.localRecovery=false;missionRescue.leaderWatch=MissionMotionPolicy::Watch();missionRescue.failedExitDoors.clear();
            resumeRecoveredMissionPhase();
            ou->showPlayerAMessage(Loc::text("ui.the_traveler_has_gone_out_the_journey_to_the"), true);
        }
        else
        {
            missionWaitReason(MissionMotionPolicy::Exit);
            missionTickTravelRecovery(elapsed,true);
        }
        return;
    }

    if (!routeTestInProgress && position.squaredDistance(destination) <= (scientificMission&&!scientificReturning?6400.0f:250000.0f))
    {
        if(scientificMission && !scientificReturning)
        {
            beginScientificActivity();return;
        }
        if(caravanMission && !caravanReturning)
        {
            beginCaravanTrade();return;
        }
        finishMission(true);
        return;
    }

    const float remainingSquared = position.squaredDistance(destination);
    if (!quarterSpeech && journeyDistanceSquared > 0.0f &&
        remainingSquared <= journeyDistanceSquared * 0.5625f)
    {
        quarterSpeech = true;
        escort->sayALine(Loc::text("ui.we_are_making_good_progress_please_stay_close_to"), true);
    }
    if (!midpointChecked && journeyDistanceSquared > 0.0f &&
        remainingSquared <= journeyDistanceSquared * 0.25f)
    {
        midpointChecked = true;
        if (rareContract || UtilityT::random(0.0f, 100.0f) < 40.0f)
            spawnBanditAmbush();
        else
            escort->sayALine(Loc::text("ui.we_are_halfway_there_let_s_remain_vigilant"), true);
    }
    if (!finalSpeech && journeyDistanceSquared > 0.0f &&
        remainingSquared <= journeyDistanceSquared * 0.0625f)
    {
        finalSpeech = true;
        escort->sayALine(Loc::text("ui.we_are_approaching_our_destination"), true);
    }

    float playerDistance = 0.0f;
    Character* player = nearestPlayer(position, playerDistance);
    if (!player){missionRescue.leaderWatch.fresh();missionWaitReason(MissionMotionPolicy::Player);return;}
    if(playerDistance<2250000.0f)proximitySeconds+=elapsed;

    if(!travelIncidentTriggered&&missionElapsed>90.0f&&UtilityT::random(0.0f,100.0f)<0.7f)
    {
        travelIncidentTriggered=true;int incident=UtilityT::randomInt(0,3);if(incident==0){journeyData.detours=1;escort->sayALine(Loc::text("ui.the_main_passage_looks_dangerous_let_us_take_a"),true);ou->showPlayerAMessage(Loc::text("ui.incident_dangerous_detour_reported_on_the_road"),true);}else if(incident==1){escort->sayALine(Loc::text("ui.this_storm_reduces_visibility_tighten_the_formation"),true);ou->showPlayerAMessage(Loc::text("ui.incident_poor_visibility_the_group_is_slowing_down"),true);}else if(incident==2){escort->sayALine(Loc::text("ui.a_local_patrol_controls_the_road_let_s_stay"),true);ou->showPlayerAMessage(Loc::text("ui.incident_control_of_a_local_faction"),true);}else{escort->sayALine(Loc::text("ui.i_hear_an_aggressive_beast_nearby_prepare_yourself"),true);spawnBanditAmbush();}}

    if (!waitingForPlayer && playerDistance > 9000000.0f)
    {
        escort->getMovement()->halt();
        escort->removeJob(MOVE_CUS_ORDERED);
        waitingForPlayer = true;
        missionRescue.leaderWatch.fresh();missionWaitReason(MissionMotionPolicy::Player);
        ou->showPlayerAMessage(Loc::text("ui.the_traveler_is_waiting_for_you_come_back_to"), true);
        lastImportantAlert="separe";importantAlertClock=30.0f;
    }
    else if (waitingForPlayer && playerDistance < 3240000.0f)
    {
        waitingForPlayer = false;
        resumeRecoveredMissionPhase();
        ou->showPlayerAMessage(Loc::text("ui.the_traveler_hits_the_road_again"), true);
    }
    else if (!waitingForPlayer)
    {
        MedicalSystem* escortMedical=escort->getMedical();bool needsSlowPace=(playerDistance>2250000.0f)||(escortMedical&&escortMedical->getOverallHealthRating()<0.65f);applyMissionPace();if(needsSlowPace&&importantAlertClock<=0.0f&&lastImportantAlert!="allure"){ou->showPlayerAMessage(Loc::text("ui.adaptive_pace_the_client_slows_down_when_an_important"),true);lastImportantAlert="allure";importantAlertClock=45.0f;}
        if(!routeTestInProgress)missionTickTravelRecovery(elapsed,false);
    }
}
}

__declspec(dllexport) void startPlugin()
{
    DebugLog("Mercenarie UI access fix 2026-09-29 build " __DATE__ " " __TIME__);
    static bool started=false;
    if(started){ErrorLog("Mercenarie: duplicate startPlugin ignored");return;}
    missionMotionDiagnosticEnabled=std::ifstream("mods/Guild Escort Contracts/enable-motion-diagnostics.flag").good();
    if(missionMotionDiagnosticEnabled){
        if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharMovement::_NV_halt),&missionMotionHaltHook,&missionMotionHaltOriginal))ErrorLog("TM-MOTION native halt hook unavailable");
        else DebugLog("TM-MOTION enabled: bounded native halt stacks and 60 seconds of motion samples per mission");
    }
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::isImmuneToOffscreenMode),&missionOffscreenImmuneHook,&missionOffscreenImmuneOriginal))
        ErrorLog("MISSION OFFSCREEN: full native simulation hook unavailable");
    else DebugLog("MISSION OFFSCREEN: full native simulation enabled for active mission NPCs");
    started=true;
    GuildSkeletonScale::install();
    applyGuildBodyCompatibility();
    HMODULE localizationModule=0;wchar_t localizationPath[32768]={0};
    DWORD localizationPathLength=0;
    if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&startPlugin),&localizationModule))localizationPathLength=GetModuleFileNameW(localizationModule,localizationPath,32768);
    if(localizationPathLength&&localizationPathLength<32768){
        std::string modulePath=UnicodeFileSystem::utf8(std::wstring(localizationPath,localizationPathLength));size_t slash=modulePath.find_last_of("\\/");
        if(slash!=std::string::npos){Loc::engine().directory=modulePath.substr(0,slash)+"/Localization";Loc::engine().loaded=false;}
    }
    diagnosticEnabled=std::ifstream("mods/Guild Escort Contracts/enable-dialogue-diagnostics.flag").good();{std::istringstream options(readClientOptionsBytes());std::string line;while(std::getline(options,line))if(line=="detailedLogs=1"){diagnosticEnabled=true;break;}}
    diagnosticTrace("PLUGIN start V4 lifecycle audit");
    detectMercenarieLanguage();
    if(!Loc::engine().error.empty())ErrorLog("Mercenarie localization fallback: "+Loc::engine().error);
    DebugLog(std::string("The Mercenarie build=")+MercenarieSaveBuild+" save-format=MERCENARIE-MISSION-V4-1");
    configureMercenarieDataSlot();
    loadReputations();
    loadFiscalLedger();
    installCaravanNativeFollow();installPersonnelUI();
    // Install float first: the enum guard uses its original trampoline.
    if(KenshiLib::SUCCESS==KenshiLib::AddHook(
        KenshiLib::GetRealAddress(static_cast<void (AbstractMovementBase::*)(float)>(&AbstractMovementBase::setDesiredSpeed)),
        &missionSpeedFloatHook,&missionSpeedFloatOriginal)){
        if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&AbstractMovementBase::_NV_setDesiredSpeed),
            &missionSpeedEnumHook,&missionSpeedEnumOriginal))ErrorLog("Mission pace: enum speed guard unavailable");
    }else ErrorLog("Mission pace: numeric speed guard unavailable");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(
        KenshiLib::GetRealAddress(static_cast<void (Dialogue::*)(const std::string&,DialogLineData*)>(&Dialogue::say)),
        &missionSayHook,&missionSayOriginal))ErrorLog("Mission speech: could not hook localized native barks");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
        KenshiLib::GetRealAddress(&Dialogue::sendEvent), &dialogueSendEventHook, &dialogueSendEventOriginal))
        ErrorLog("Guild Escort: could not hook dialogue event");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
        KenshiLib::GetRealAddress(&Dialogue::listPlayerReplies), &listPlayerRepliesHook, &listPlayerRepliesOriginal))
        ErrorLog("Guild Escort: could not hook player reply list");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
        KenshiLib::GetRealAddress(&Dialogue::_doActions), &doActionsHook, &doActionsOriginal))
        ErrorLog("Guild Escort: could not hook dialogue actions");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
        KenshiLib::GetRealAddress(&DialogLineData::getPlayerReplies), &getPlayerRepliesHook, &getPlayerRepliesOriginal))
        ErrorLog("Mercenarie: could not hook reply result filtering");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(
        KenshiLib::GetRealAddress(&GameWorld::_NV_mainLoop_GPUSensitiveStuff), &mainLoopHook, &mainLoopOriginal))
        ErrorLog("Guild Escort: could not hook game loop");
    // MyGUI exports are not KenshiLib stubs. Hook the actual DLL exports,
    // including frames before any save/world exists; avoid Gui member offsets.
    HMODULE myguiModule=GetModuleHandleA("MyGUIEngine_x64.dll");
    FARPROC newsFrame=myguiModule?GetProcAddress(myguiModule,"?frameEvent@Gui@MyGUI@@QEAAXM@Z"):0;
    FARPROC newsShutdown=myguiModule?GetProcAddress(myguiModule,"?shutdown@Gui@MyGUI@@QEAAXXZ"):0;
    DebugLog(std::string("Mercenarie news: content-version=")+MainMenuNewsRules::CurrentNewsVersion);
    if(!newsFrame||KenshiLib::SUCCESS!=KenshiLib::AddHook(newsFrame,&mercenarieGuiFrameHook,&mercenarieGuiFrameOriginal))
        ErrorLog("Mercenarie news: could not hook MyGUI frame lifecycle");
    else DebugLog("Mercenarie news: pre-save MyGUI frame hook installed");
    if(!newsShutdown||KenshiLib::SUCCESS!=KenshiLib::AddHook(newsShutdown,&mercenarieGuiShutdownHook,&mercenarieGuiShutdownOriginal))
        ErrorLog("Mercenarie news: could not hook MyGUI shutdown lifecycle");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PortraitMainCellView::update),&missionPortraitUpdateHook,&missionPortraitUpdateOriginal))ErrorLog("Mission absence: could not hook portrait cells");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PlayerInterface::selectObject),&missionSelectObjectHook,&missionSelectObjectOriginal))ErrorLog("Mission absence: could not hook object selection");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PlayerInterface::selectPlayerCharacter),&missionSelectPlayerCharacterHook,&missionSelectPlayerCharacterOriginal))ErrorLog("Mission absence: could not hook portrait selection");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&PlayerInterface::_selectPlayerCharacter),&missionSelectPlayerObjectHook,&missionSelectPlayerObjectOriginal))ErrorLog("Mission absence: could not hook player selection");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::addOrder),&missionAddOrderHook,&missionAddOrderOriginal))ErrorLog("Mission absence: could not hook player orders");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::addJob),&missionAddJobHook,&missionAddJobOriginal))ErrorLog("Mission absence: could not hook character jobs");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&AITaskSytem::runGoals),&missionRunGoalsHook,&missionRunGoalsOriginal))ErrorLog("MISSION NATIVE GOAL FILTER FAILED: autonomous travel is not isolated");
    else DebugLog("MISSION NATIVE GOAL FILTER installed");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&UseableStuff::_NV_tryOperate),&missionBookTryOperateHook,&missionBookTryOperateOriginal))ErrorLog("Mission Book: could not hook native tryOperate");
    void (Dialogue::*bountyClickMethod)(int)=&Dialogue::replyClicked;
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(bountyClickMethod),&bountyReplyClickedHook,&bountyReplyClickedOriginal))ErrorLog("Guild bounty: could not hook reply selection");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MapScreen::update),&bountyMapUpdateHook,&bountyMapUpdateOriginal))ErrorLog("Guild bounty: could not hook camp map marker");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Platoon::_NV_reCheckPersistenceOnUnload),&bountyPersistenceHook,&bountyPersistenceOriginal))ErrorLog("Guild bounty: could not protect mission persistence");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Platoon::_NV_periodicUpdate_unloaded),&bountyUnloadedHook,&bountyUnloadedOriginal))ErrorLog("Guild bounty: could not protect unloaded camp position");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::setPrisonMode),&bountyPrisonHook,&bountyPrisonOriginal))ErrorLog("Guild bounty: could not hook player cage detection");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Item::_NV_serialiseInInventory),&mailItemSaveHook,&mailItemSaveOriginal))ErrorLog("Mail: cannot hook item metadata save");
    if(KenshiLib::SUCCESS!=KenshiLib::AddHook(KenshiLib::GetRealAddress(&Item::_NV_loadFromSerialiseInInventory),&mailItemLoadHook,&mailItemLoadOriginal))ErrorLog("Mail: cannot hook item metadata load");
    nativeSaveCompletionHookReady=KenshiLib::SUCCESS==KenshiLib::AddHook(KenshiLib::GetRealAddress(&SaveFileSystem::sync),&missionSaveSyncHook,&missionSaveSyncOriginal);
    nativeSerializationHookReady=KenshiLib::SUCCESS==KenshiLib::AddHook(KenshiLib::GetRealAddress(&GameDataContainer::save),&missionNativeSerializeHook,&missionNativeSerializeOriginal);
    if(!nativeSerializationHookReady)ErrorLog("MercenarieSave: native serialization hook unavailable; saves protected");
    if(!nativeSaveCompletionHookReady)ErrorLog("Mercenarie: native save completion hook unavailable; saves protected");
    else DebugLog("MercenarieSave: asynchronous native completion hook installed");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&SaveManager::saveGame),&missionSaveHook,&missionSaveOriginal))ErrorLog("Guild Escort: could not hook mission save");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&SaveManager::loadGame),&missionLoadHook,&missionLoadOriginal))ErrorLog("Guild Escort: could not hook mission load");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&SaveManager::newGame),&mercenarieNewGameHook,&mercenarieNewGameOriginal))ErrorLog("Guild Escort: could not isolate new game progression");
    estateInstallHooks();
    installBuildingPublicAccess();
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&SaveManager::showImport),&saveManagerShowImportHook,&saveManagerShowImportOriginal))ErrorLog("Guild Escort: could not hook import screen");
    if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&SaveManager::importGame),&saveManagerImportGameHook,&saveManagerImportGameOriginal))ErrorLog("Guild Escort: could not hook import completion");
}



