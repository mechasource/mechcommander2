//***************************************************************************
//
//	multplyr.cpp - This file contains the MultiPlayer Class
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

// #include "mclib.h"

#include "crtdbg.h"

#ifndef MULTPLYR_H
#include "multplyr.h"
#endif

#ifndef TEAM_H
#include "team.h"
#endif

#include "mover.h"
#ifndef TURRET_H
#include "turret.h"
#endif

#ifndef MISSION_H
#include "mission.h"
#endif

#ifndef MECH_H
#include "mech.h"
#endif

#ifndef GAMEOBJ_H
#include "gameobj.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#ifndef LOGISTICSDATA_H
#include "LogisticsData.h"
#endif

#ifndef BLDNG_H
#include "bldng.h"
#endif

#ifndef TERROBJ_H
#include "terrobj.h"
#endif

#ifndef MOVE_H
#include "move.h"
#endif

#ifndef GROUP_H
#include "group.h"
#endif

#ifndef COMNDR_H
#include "comndr.h"
#endif

#ifndef CARNAGE_H
#include "carnage.h"
#endif

#ifndef ARTLRY_H
#include "artlry.h"
#endif

#ifndef CONTROLGUI_H
#include "controlgui.h"
#endif

#ifndef LOGISTICS_H
#include "logistics.h"
#endif

#include "MissionBegin.h"

#ifndef PREFS_H
#include "Prefs.h"
#endif

#ifndef MISSIONGUI_H
#include "missiongui.h"
#endif

#ifndef GAMELOG_H
#include "gamelog.h"
#endif

#include "dplay8.h"

#include "MPParameterScreen.h"

//#ifndef VERSION_H
//#include "version.h"
//#endif

#include "resource.h"

#ifdef USE_MISSION_RESULTS_SCREEN
extern bool EventsToMissionResultsScreen;
#else
bool EventsToMissionResultsScreen = false;
#endif

#ifdef USE_LOGISTICS
extern bool whackTimer;
void
CancelBool(int32_t value);
#endif

#ifndef MPPREFS_H
#include "mpprefs.h"
#endif

#ifdef USE_STRING_RESOURCES
extern HINSTANCE thisInstance;
int32_t
cLoadString(HINSTANCE hinstance, uint32_t uID, const std::wstring_view& lpBuffer, int32_t nBufferMax);
#endif

#include "gamesound.h"

#define MAX_MSG_SIZE 10240

extern CPrefs prefs;
extern bool quitGame;

extern float loadProgress;
extern bool aborted;

//***************************************************************************

uint32_t ServerPlayerNum = 1; // commanderId (or checkInId) of server
bool MultiPlayer::launchedFromLobby = false;
bool MultiPlayer::registerZone = false;

int32_t MultiPlayer::presetDropZones[MAX_MC_PLAYERS] = {-1, -1, -1, -1, -1, -1, -1, -1};
int32_t MultiPlayer::colors[MAX_COLORS] = {0};

float ResourceBuildingRefreshRate = 60.0f;

// {35DC7890-C5EF-4171-B0CF-4D5C7AE7C2D7}
static const GUID MC2GUID = {
	0x35dc7890, 0xc5ef, 0x4171, {0xb0, 0xcf, 0x4d, 0x5c, 0x7a, 0xe7, 0xc2, 0xd7}};
// {1F8251BB-1436-44b3-A5B0-0FCF6858C176}
static const GUID MC2DEMOGUID = {
	0x1f8251bb, 0x1436, 0x44b3, {0xa5, 0xb0, 0xf, 0xcf, 0x68, 0x58, 0xc1, 0x76}};

PVOID MC2NetLib = nullptr;
bool OnLAN = false;
NETMESSAGE ReceiveMsg;

//------------
// EXTERN vars
extern GameLog* CombatLog;
extern GameLog* NetLog;

extern bool LaunchedFromLobby;

extern UserHeapPtr systemHeap;

#ifdef _DEBUG
// extern DebugFileStream Debug;
#endif

extern const std::wstring_view& startupPakFile;

// ConnectUsingDialog is a function defined in dpdialog.cpp.
// It allows the user to choose the type of connection.
// extern HRESULT ConnectUsingDialog(HINSTANCE );

extern void
SortMoverList(int32_t numMovers, std::unique_ptr<Mover>* moverList, Stuff::Vector3D dest);

extern void
killTheGame();

#ifndef TERRAINEDIT
extern const std::unique_ptr<Debugger>& debugger;
#endif

//------------
// GLOBAL vars
MultiPlayer* MPlayer = nullptr;
// extern int32_t NumMissionScriptMessages;

/// extern Scenario* scenario;
/// extern Logistics* globalLogPtr;

void
DEBUGWINS_print(const std::wstring_view& s, int32_t window = 0);

//***************************************************************************
// MISC functions
//***************************************************************************

bool
StartupNetworking(void)
{
	return false;
}

//-----------------------------------------------------------------------------

void
ResetNetworking(void)
{
}

//-----------------------------------------------------------------------------

void
ShutdownNetworking(void)
{
}

//***************************************************************************
// WORLD CHUNK class
//***************************************************************************

PVOID
WorldChunk::operator new(size_t ourSize)
{
	PVOID result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void
WorldChunk::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

void
WorldChunk::buildMine(int32_t worldCellR, int32_t worldCellC, int32_t teamId,
	int32_t mineState, int32_t explosionType)
{
}

//---------------------------------------------------------------------------

void
WorldChunk::buildTerrainFire(GameObjectPtr object, int32_t seconds)
{
}

//---------------------------------------------------------------------------

void
WorldChunk::buildArtillery(
	int32_t commanderId, int32_t artilleryType, Stuff::Vector3D location, int32_t seconds)
{
}

//---------------------------------------------------------------------------

void
WorldChunk::buildMissionScriptMessage(int32_t messageCode, int32_t messageParam)
{
}

//---------------------------------------------------------------------------

void
WorldChunk::buildPilotKillStat(int32_t moverIndex, int32_t vehicleClass)
{
}

//--------------------------------------------------------------------------

void
WorldChunk::buildScore(int32_t commanderid, int32_t score)
{
}

//--------------------------------------------------------------------------

void
WorldChunk::buildKillLoss(int32_t killerCID, int32_t loserCID)
{
}

//--------------------------------------------------------------------------

void
WorldChunk::buildCaptureBuilding(BuildingPtr building, int32_t newCommanderID)
{
}

//---------------------------------------------------------------------------

void
WorldChunk::buildEndMission(void)
{
}

//--------------------------------------------------------------------------

void
WorldChunk::pack(void)
{
}

//---------------------------------------------------------------------------

void
WorldChunk::unpack(void)
{
}

//---------------------------------------------------------------------------

bool
WorldChunk::equalTo(WorldChunkPtr chunk)
{
	return false;
}

//***************************************************************************
// MECHCOMMANDER MESSAGE handlers
//***************************************************************************

//***************************************************************************
// MULTIPLAYER class
//***************************************************************************

PVOID
MultiPlayer::operator new(size_t ourSize)
{
	PVOID result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void
MultiPlayer::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

void
MultiPlayer::init(void)
{
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::setup(void)
{
	return (MPLAYER_NO_ERR);
}

//---------------------------------------------------------------------------

void
MultiPlayer::initUpdateFrequencies()
{
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::update(void)
{
	return (MPLAYER_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::beginSessionScan(const std::wstring_view& ipAddress, bool persistent)
{
	return (MPLAYER_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::endSessionScan(void)
{
	return (MPLAYER_NO_ERR);
}

//---------------------------------------------------------------------------

MC2Session*
MultiPlayer::getSessions(int32_t& sessionCount)
{
	return (nullptr);
}

//---------------------------------------------------------------------------

bool
MultiPlayer::hostSession(const std::wstring_view& sessionName, const std::wstring_view& playerName, int32_t mxPlayers)
{
	return (true);
}

//---------------------------------------------------------------------------

void
MultiPlayer::updateSessionData(MC2SessionData* sessionData)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::setLocked(bool set)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::setInProgress(bool set)
{
}
//---------------------------------------------------------------------------

void
MultiPlayer::setCancelled(bool set)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::addTeamScore(int32_t teamID, int32_t score)
{
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::joinSession(MC2Session* session, const std::wstring_view& playerName)
{
	return (MPLAYER_ERR_SESSION_NOT_FOUND);
}

//-----------------------------------------------------------------------------

int32_t
MultiPlayer::closeSession(void)
{
	return 0;
}

//-----------------------------------------------------------------------------

void
MultiPlayer::leaveSession(void)
{
}

//-----------------------------------------------------------------------------

int32_t
MultiPlayer::bootPlayer(NETPLAYER bootedPlayer)
{
	return (MPLAYER_NO_ERR);
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::waitTillStartLoading(void)
{
	return (false);
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::waitTillMechDataReceived(void)
{
	return (false);
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::waitTillMissionLoaded(void)
{
	return (false);
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::waitTillMissionSetup(void)
{
	return (false);
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::waitForSessionEntry(void)
{
	sessionEntry = -1;
	return (sessionEntry == 1);
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::playersReadyToLoad(void)
{
	return (true);
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::launchBrowser(const std::wstring_view& link)
{
	return (false);
}

//-----------------------------------------------------------------------------

void
MultiPlayer::initParametersScreen(void)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::setDefaultPlayerInfo(void)
{
}

//-----------------------------------------------------------------------------

int32_t
MultiPlayer::setClosestColor(int32_t colorIndex, int32_t commanderid)
{
	int32_t curIndex = colorIndex;
	return (curIndex);
}

//-----------------------------------------------------------------------------

int32_t
MultiPlayer::setNextFreeColor(int32_t commanderid)
{
	int32_t i = 0;
	return (i);
}

//-----------------------------------------------------------------------------

void
MultiPlayer::setPlayerBaseColor(int32_t commanderid, int32_t colorIndex)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::setPlayerTeam(int32_t commanderid, int32_t teamID)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::setColor(int32_t colorIndex, int32_t commanderid)
{
}

//-----------------------------------------------------------------------------

MC2Player GetPlayersList[MAX_MC_PLAYERS];

const MC2Player*
MultiPlayer::getPlayers(int32_t& playerCount)
{
	return (nullptr);
}

//-----------------------------------------------------------------------------

void
MultiPlayer::logMessage(NETMESSAGE* message, bool sent)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::sendMessage(
	NETPLAYER player, PVOID data, size_t dataSize, bool guaranteed, bool toSelf)
{
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::hostGame(const std::wstring_view& sessionName, const std::wstring_view& playerName, int32_t nPlayers)
{
	return (true);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::joinGame(const std::wstring_view& ipAddress, const std::wstring_view& sessionName, const std::wstring_view& playerName)
{
	return (MPLAYER_NO_ERR);
}

//-----------------------------------------------------------------------------

void
MultiPlayer::addToLocalMovers(std::unique_ptr<Mover> mover)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::removeFromLocalMovers(std::unique_ptr<Mover> mover)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::addToMoverRoster(std::unique_ptr<Mover> mover)
{
	for (size_t i = 0; i < MAX_MULTIPLAYER_MOVERS; i++)
		if (!moverRoster[i])
		{
			moverRoster[i] = mover;
			mover->setNetRosterIndex(i);
			return;
		}
	STOP(("MultiPlayer.addToMoverRoster: too many movers"));
}

//---------------------------------------------------------------------------

void
MultiPlayer::removeFromMoverRoster(std::unique_ptr<Mover> mover)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::addToPlayerMoverRoster(int32_t playerCommanderID, std::unique_ptr<Mover> mover)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::removeFromPlayerMoverRoster(std::unique_ptr<Mover> mover)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::addToTurretRoster(TurretPtr turret)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::initSpecialBuildings(char commandersToLoad[8][3])
{
}

//***************************************************************************
// WORLD CHUNK maintenance functions
//***************************************************************************

int32_t
MultiPlayer::addWorldChunk(WorldChunkPtr chunk)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addMissionScriptMessageChunk(int32_t code, int32_t param)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addArtilleryChunk(
	int32_t commanderId, int32_t artilleryType, Stuff::Vector3D location, int32_t seconds)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addMineChunk(
	int32_t tileR, int32_t tileC, int32_t teamId, int32_t mineState, int32_t explosionType)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addLightOnFireChunk(GameObjectPtr object, int32_t seconds)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addPilotKillStat(std::unique_ptr<Mover> mover, int32_t vehicleClass)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addScoreChunk(int32_t commanderid, int32_t score)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addKillLossChunk(int32_t killerCID, int32_t loserCID)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addEndMissionChunk(void)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addCaptureBuildingChunk(
	BuildingPtr building, int32_t prevCommanderID, int32_t newCommanderID)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::grabWorldChunks(uint32_t* packedChunkBuffer)
{
	return (0);
}

//---------------------------------------------------------------------------
// WEAPON HIT CHUNK maintenance functions
//---------------------------------------------------------------------------

int32_t
MultiPlayer::addWeaponHitChunk(WeaponHitChunkPtr chunk)
{
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::addWeaponHitChunk(
	GameObjectPtr target, WeaponShotInfoPtr shotInfo, bool isRefit)
{
	return (0);
}

//---------------------------------------------------------------------------

void
MultiPlayer::grabWeaponHitChunks(uint32_t* packedChunkBuffer, int32_t numChunks)
{
}

//---------------------------------------------------------------------------
// MESSAGE SENDERS
//---------------------------------------------------------------------------

void
MultiPlayer::sendPlayerInfo(NETPLAYER receiver)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendPlayerCID(NETPLAYER receiver, uint8_t subType, char CID)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendPlayerUpdate(NETPLAYER receiver, int32_t stage, int32_t newCommanderID)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendMissionSettingsUpdate(NETPLAYER receiver)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendChat(NETPLAYER receiver, char team, const std::wstring_view& chatMessage)
{
}

void
MultiPlayer::sendPlayerActionChat(NETPLAYER receiver, const std::wstring_view& playerName, uint32_t resID)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendPlayerCheckIn(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendPlayerSetup(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendPlayerInsignia(const std::wstring_view& insigniaFileName, puint8_t insigniaData, size_t dataSize)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendMissionSetup(NETPLAYER receiver, int32_t subType, CompressedMech* mechData)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::setServer(NETPLAYER player, char playerIPAddress[16])
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendStartMission(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendEndMission(int32_t result)
{
}

//---------------------------------------------------------------------------
extern std::unique_ptr<Mover>
BringInReinforcement(
	int32_t vehicleID, int32_t rosterIndex, int32_t commanderid, Stuff::Vector3D pos, bool exists);

void
MultiPlayer::sendReinforcement(int32_t vehicleID, int32_t rosterIndex, char pilotName[16],
	int32_t commanderid, Stuff::Vector3D pos, uint8_t stage)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::sendNewServer(void)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::sendLeaveSession(char subType, char commanderid)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::sendPlayerOrder(TacticalOrderPtr tacOrder, bool needsSelection, int32_t numMovers,
	std::unique_ptr<Mover>* moverList, int32_t numGroups, MoverGroupPtr* groupList, bool queuedOrder)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendHoldPosition(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendPlayerMoverGroup(
	int32_t groupId, int32_t numMovers, std::unique_ptr<Mover>* moverList, int32_t point)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendPlayerArtillery(int32_t striketype, Stuff::Vector3D location, int32_t seconds)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendMoverUpdate(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendTurretUpdate(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendMoverWeaponFireUpdate(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendTurretWeaponFireUpdate(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendMoverCriticalUpdate(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendWeaponHitUpdate(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::sendWorldUpdate(void)
{
}

//---------------------------------------------------------------------------
// MESSAGE HANDLERS
//---------------------------------------------------------------------------

void
MultiPlayer::handleChat(NETPLAYER sender, MCMSG_Chat* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleMissionSettingsUpdate(NETPLAYER sender, MCMSG_MissionSettingsUpdate* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerCID(NETPLAYER sender, MCMSG_PlayerCID* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerUpdate(NETPLAYER sender, MCMSG_PlayerUpdate* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleMissionSetup(NETPLAYER sender, MCMSG_MissionSetup* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerInfo(NETPLAYER sender, MCMSG_PlayerInfo* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerCheckIn(NETPLAYER sender, MCMSG_PlayerCheckIn* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerSetup(NETPLAYER sender, MCMSG_PlayerSetup* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerInsignia(NETPLAYER sender, MCMSG_PlayerInsignia* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleStartMission(NETPLAYER sender)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleEndMission(NETPLAYER sender, MCMSG_EndMission* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleReinforcement(NETPLAYER sender, MCMSG_Reinforcement* msg)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::handleNewServer(NETPLAYER sender, MCMSG_NewServer* msg)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::handleLeaveSession(NETPLAYER sender, MCMSG_LeaveSession* msg)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::handleHoldPosition(NETPLAYER sender, MCMSG_HoldPosition* msg)
{
}

//-----------------------------------------------------------------------------

void
MultiPlayer::handlePlayerOrder(NETPLAYER sender, MCMSG_PlayerOrder* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerMoverGroup(NETPLAYER sender, MCMSG_PlayerMoverGroup* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerArtillery(NETPLAYER sender, MCMSG_PlayerArtillery* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleMoverUpdate(NETPLAYER sender, MCMSG_MoverUpdate* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleTurretUpdate(NETPLAYER sender, MCMSG_TurretUpdate* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleMoverWeaponFireUpdate(NETPLAYER sender, MCMSG_MoverWeaponFireUpdate* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleTurretWeaponFireUpdate(NETPLAYER sender, MCMSG_TurretWeaponFireUpdate* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleMoverCriticalUpdate(NETPLAYER sender, MCMSG_MoverCriticalUpdate* msg)
{
}

//---------------------------------------------------------------------------

extern bool FromMP;

void
MultiPlayer::handleWeaponHitUpdate(NETPLAYER sender, MCMSG_WeaponHitUpdate* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleWorldUpdate(NETPLAYER sender, MCMSG_WorldUpdate* msg)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handlePlayerLeftSession(NETPLAYER leftPlayer)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::handleTerminateSession(void)
{
}

//---------------------------------------------------------------------------

bool
MultiPlayer::isServerMissing(void)
{
	return (false);
}

//---------------------------------------------------------------------------

bool
MultiPlayer::processGameMessage(NETMESSAGE* msg)
{
	return (true);
}

//---------------------------------------------------------------------------

void
MultiPlayer::processMessages(void)
{
}

//-----------------------------------------------------------------------------
// Misc. routines
//-----------------------------------------------------------------------------

void
MultiPlayer::buildMoverRosterRLE(void)
{
}

//---------------------------------------------------------------------------

int32_t
MultiPlayer::updateClients(bool forceIt)
{
	return (MPLAYER_NO_ERR);
}

//---------------------------------------------------------------------------

bool
MultiPlayer::calcMissionStatus(void)
{
	return (true);
}

//-----------------------------------------------------------------------------

int32_t __cdecl sortPlayerRanks(PCVOID player1, PCVOID player2)
{
	return (0);
}

//-----------------------------------------------------------------------------

void
MultiPlayer::calcPlayerRanks(void)
{
}

//-----------------------------------------------------------------------------

bool
MultiPlayer::allPlayersCheckedIn(void)
{
	return (true);
}

//---------------------------------------------------------------------------

bool
MultiPlayer::allPlayersReady(void)
{
	return (true);
}

//---------------------------------------------------------------------------

void
MultiPlayer::switchServers(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::calcDropZones(char dropZonesCID[MAX_MC_PLAYERS], char hqs[MAX_TEAMS])
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::initStartupParameters(bool fresh)
{
}

//---------------------------------------------------------------------------
extern const std::wstring_view&
GetTime();

extern const std::wstring_view& SpecialtySkillsTable[NUM_SPECIALTY_SKILLS];

int32_t
MultiPlayer::saveTranscript(const std::wstring_view& fileName, bool debugging)
{
	return (MPLAYER_NO_ERR);
}

//---------------------------------------------------------------------------

void
MultiPlayer::destroy(void)
{
}

//---------------------------------------------------------------------------

void
MultiPlayer::getChatMessages(const std::wstring_view&* buffer, int32_t* playerIDs, int32_t& count)
{
}

void
MultiPlayer::redistributeRP()
{
}

//***************************************************************************
