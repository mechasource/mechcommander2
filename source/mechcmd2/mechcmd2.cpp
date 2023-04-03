//--------------------------------------------------------------------------
//
// And away we go.  MechCommander 2 -- Electric Boogaloo
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

//#include "gameos.hpp"
//#include "toolos.hpp"
//#include "stuff/stuff.h"
//#include "mlr/mlr.h"
//#include "gosfx/gosfxheaders.h"

//-----------------------------------
// Include Files
// #include "mclib.h"

//#include "mission.h"
//#include "unitdesg.h"
//#include "logistics.h"
//#include "gamesound.h"
//#include "sounds.h"
//#include "multplyr.h"
//#include "team.h"
//#include "version.h"
//#include "turret.h"
//#include "OptionsScreenWrapper.h"
//#include "gamelog.h"
//#include "logisticsdialog.h"
#include "prefs.h"

extern CPrefs prefs;

//#include "resource.h"

//------------------------------------------------------------------------------------------------------------
// MechCmdr2 Global Instances of Things	- ugh!

// UserHeapPtr systemHeap = nullptr;
// UserHeapPtr guiHeap	= nullptr;

// FastFile** fastFiles = nullptr;
// int32_t numFastFiles = 0;
// int32_t maxFastFiles = 0;

uint32_t GameDifficulty = 0;
uint32_t gammaLevel = 0;
extern int32_t DigitalMasterVolume;
extern int32_t MusicVolume;
extern int32_t sfxVolume;
extern int32_t RadioVolume;
extern int32_t BettyVolume;
int32_t resolution = 0;
int32_t renderer = 0;
int32_t FilterState = gos_FilterNone;

bool quitGame = false;
bool justStartMission = false;
bool gamePaused = false;
bool hasGuardBand = false;
bool canMultiTextureDetail = false;
bool useUnlimitedAmmo = true;
bool useLeftRightMouseProfile = true; // if false, use old style commands
bool justResaveAllMaps = false;
extern bool useWaterInterestTexture;
extern bool useShadows;

extern bool aborted;
bool gotBombData = false;

TimerManagerPtr timerManager = nullptr;

bool initGameLogs = false;
bool initNetLog = false;
GameLog* NetLog = nullptr;
bool initCombatLog = false;
GameLog* CombatLog = nullptr;
bool initBugLog = false;
GameLog* BugLog = nullptr;
bool initLRMoveLog = false;

bool KillAmbientLight = false;

void InitDW();

extern uint32_t NumDevices;

uint32_t elementHeapSize = 1024000;
uint32_t maxElements = 2048;
uint32_t maxGroups = 1024;

uint32_t systemHeapSize = 8192000;
uint32_t guiHeapSize = 1023999;
uint32_t logisticsHeapSize = 4095999;
uint32_t missionHeapSize = 3072000;
uint32_t spriteDataHeapSize = 2048000;
uint32_t spriteHeapSize = 8192000;
uint32_t polyHeapSize = 1024000;

extern float ProcessorSpeed;
void __stdcall ExitGameOS();

uint32_t gosResourceHandle = 0;
HGOSFONT3D gosFontHandle = 0;
float gosFontScale = 1.0;
extern HGOSFONT3D FontHandle;
FloatHelpPtr globalFloatHelp = nullptr;
uint32_t currentFloatHelp = 0;
float MaxMinUV = 8.0f;

uint32_t BaseVertexcolour = 0x00000000; // This color is applied to all vertices
	// in game as Brightness correction.

enum
{
	CPU_UNKNOWN,
	CPU_PENTIUM,
	CPU_MMX,
	CPU_KATMAI
} Processor = CPU_PENTIUM; // Needs to be set when GameOS supports ProcessorID
	// -- MECHCMDR2
extern float frameRate;
void EnterWindowMode();

extern int32_t MaxMoveGoalChecks;
extern bool useSound;
extern bool useMusic;
int32_t MaxResourcePoints = -1;
bool ShowMovers = false;
bool EnemiesGoalPlan = false;
bool inViewMode = false;
extern bool CullPathAreas;
uint32_t viewObject = 0x0;
wchar_t missionName[1024];

extern wchar_t FileMissingString[];
extern wchar_t CDMissingString[];
extern wchar_t MissingTitleString[];

std::wstring_view ExceptionGameMsg = nullptr;

wchar_t buildNumber[80];

extern int32_t TERRAIN_TXM_SIZE;
int32_t ObjectTextureSize = 128;

extern uint32_t MultiPlayCommanderId;
extern bool useRealLOS;
bool reloadBounds = false;

extern int32_t DrawDebugCells;
extern int32_t GameVisibleVertices;

bool EULAShown = false;
SoundSystem* sndSystem;

extern bool gNoDialogs;

// DEBUG
#define MAX_SHAPES 0
TG_MultiShape testShape[36];
TG_AnimateShape testAnim[36];

Stuff::Vector3D pos[36] = {
	Stuff::Vector3D(-3400.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-3300.0f, 2700.0f, -1.0f), Stuff::Vector3D(-3300.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-3300.0f, 2900.0f, -1.0f), Stuff::Vector3D(-3300.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-3200.0f, 2700.0f, -1.0f), Stuff::Vector3D(-3200.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-3200.0f, 2900.0f, -1.0f), Stuff::Vector3D(-3200.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-3100.0f, 2700.0f, -1.0f), Stuff::Vector3D(-3100.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-3100.0f, 2900.0f, -1.0f), Stuff::Vector3D(-3100.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-3000.0f, 2700.0f, -1.0f), Stuff::Vector3D(-3000.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-3000.0f, 2900.0f, -1.0f), Stuff::Vector3D(-3000.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-2900.0f, 2700.0f, -1.0f), Stuff::Vector3D(-2900.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-2900.0f, 2900.0f, -1.0f), Stuff::Vector3D(-2900.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-2800.0f, 2700.0f, -1.0f), Stuff::Vector3D(-2800.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-2800.0f, 2900.0f, -1.0f), Stuff::Vector3D(-2800.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-2700.0f, 2700.0f, -1.0f), Stuff::Vector3D(-2700.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-2700.0f, 2900.0f, -1.0f), Stuff::Vector3D(-2700.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-2600.0f, 2700.0f, -1.0f), Stuff::Vector3D(-2600.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-2600.0f, 2900.0f, -1.0f), Stuff::Vector3D(-2600.0f, 3000.0f, -1.0f),
	Stuff::Vector3D(-2500.0f, 2700.0f, -1.0f), Stuff::Vector3D(-2500.0f, 2800.0f, -1.0f),
	Stuff::Vector3D(-2500.0f, 2900.0f, -1.0f)};

#ifdef LAB_ONLY
int32_t currentLineElement = 0;
LineElement* debugLines[10000];

#define ProfileTime(x, y) \
	x = GetCycles();      \
	y;                    \
	x = GetCycles() - x;
extern int64_t MCTimeMultiplayerUpdate;
#else
#define ProfileTime(x, y) y;
#endif

#define MAX_KILL_AT_START 100

int32_t NumDisableAtStart = 0;
int32_t DisableAtStart[MAX_KILL_AT_START];

OptionsScreenWrapper* optionsScreenWrapper = nullptr;
bool bInvokeOptionsScreenFlag = false;

bool SnifferMode = false;
gos_VERTEX* testVertex = nullptr;
uint16_t* indexArray = nullptr;
uint32_t testTextureHandle = 0xffffffff;
float totalTime = 0;
uint32_t numIterations = 4;
uint32_t curIteration = 0;
uint32_t curDevice = 0;
bool isUsingSoftware = false;
#define MAX_HARDWARE_CARDS 5
float trisPerSecond[MAX_HARDWARE_CARDS] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

extern bool loadInMissionSave;
extern wchar_t CDInstallPath[];

extern float averageFrameRate;
extern int32_t currentFrameNum;
extern float last30Frames[];

extern bool WindowsNT;

extern std::wstring_view* g_textureCache_FilenameOfLastLoadedTexture;

bool checkedBomb = false;

//***************************************************************************
// DEBUGGING WINDOWS
//***************************************************************************

#define NUM_DEBUG_WINDOWS 4
wchar_t DebugStatusBarString[256];
GameDebugWindow* DebugWindow[NUM_DEBUG_WINDOWS] = {nullptr, nullptr, nullptr, nullptr};
GameObjectPtr DebugGameObject[3] = {nullptr, nullptr, nullptr};
int32_t GameObjectWindowList[3] = {0, 0, 0};
int32_t NumGameObjectsToDisplay = 0;
bool DebugWindowOpen[NUM_DEBUG_WINDOWS] = {false, false, false, false};
bool DebugStatusBarOpen = false;
bool DebugScoreBoardOpen = false;
bool DebugHelpOpen = false;
void DEBUGWINS_print(std::wstring_view s, int32_t window = 0);

//---------------------------------------------------------------------------

void DEBUGWINS_init(void)
{

	sprintf(DebugStatusBarString, "DEBUG Status Bar: GLENNBA");

	DebugWindow[0] = new GameDebugWindow;
	DebugWindow[0]->init();
	DebugWindow[0]->setPos(10, 200);
	if (DebugWindowOpen[0])
		DebugWindow[0]->open();
	else
		DebugWindow[0]->close();

	DebugWindow[1] = new GameDebugWindow;
	DebugWindow[1]->init();
	DebugWindow[1]->setPos(10, 20);
	if (DebugWindowOpen[1])
		DebugWindow[1]->open();
	else
		DebugWindow[1]->close();

	DebugWindow[2] = new GameDebugWindow;
	DebugWindow[2]->init();
	DebugWindow[2]->setPos(220, 20);
	if (DebugWindowOpen[2])
		DebugWindow[2]->open();
	else
		DebugWindow[2]->close();

	DebugWindow[3] = new GameDebugWindow;
	DebugWindow[3]->init();
	DebugWindow[3]->setPos(430, 20);
	if (DebugWindowOpen[3])
		DebugWindow[3]->open();
	else
		DebugWindow[3]->close();
}

//---------------------------------------------------------------------------

void DEBUGWINS_destroy(void)
{

	for (size_t i = 0; i < NUM_DEBUG_WINDOWS; i++)
		if (DebugWindow[i])
		{
			DebugWindow[i]->destroy();
			delete DebugWindow[i];
			DebugWindow[i] = nullptr;
		}
}

void initDialogs()
{
	FullPathFileName path;
	path.init(artPath, "mcl_dialog", ".fit");

	FitIniFile file;
	if (NO_ERROR != file.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't open file % s", (std::wstring_view)path);
		Assert(0, 0, error);
		return;
	}

	LogisticsOKDialog::init(file);
	file.close();

	path.init(artPath, "mcl_sm", ".fit");

	if (NO_ERROR != file.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't open file %s", (std::wstring_view)path);
		Assert(0, 0, error);
		return;
	}

	LogisticsSaveDialog::init(file);
	file.close();

	path.init(artPath, "mcl_dialog_onebutton", ".fit");
	if (NO_ERROR != file.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't open file % s", (std::wstring_view)path);
		Assert(0, 0, error);
		return;
	}

	LogisticsOneButtonDialog::init(file);
}

void endDialogs()
{
	if (LogisticsOKDialog::s_instance)
	{
		delete LogisticsOKDialog::s_instance;
		LogisticsOKDialog::s_instance = nullptr;
	}

	if (LogisticsSaveDialog::s_instance)
	{
		delete LogisticsSaveDialog::s_instance;
		LogisticsSaveDialog::s_instance = nullptr;
	}

	if (LogisticsOneButtonDialog::s_instance)
	{
		delete LogisticsOneButtonDialog::s_instance;
		LogisticsOneButtonDialog::s_instance = nullptr;
	}

	if (LogisticsLegalDialog::s_instance)
	{
		delete LogisticsLegalDialog::s_instance;
		LogisticsLegalDialog::s_instance = nullptr;
	}
}

//---------------------------------------------------------------------------

void DEBUGWINS_toggle(bool* windowsOpen)
{

	for (size_t i = 0; i < NUM_DEBUG_WINDOWS; i++)
		if (windowsOpen[i])
			DebugWindow[i]->toggle();
	DebugHelpOpen = windowsOpen[NUM_DEBUG_WINDOWS];
	DebugStatusBarOpen = windowsOpen[NUM_DEBUG_WINDOWS + 1];
	DebugScoreBoardOpen = windowsOpen[NUM_DEBUG_WINDOWS + 2];
}

//---------------------------------------------------------------------------

void DEBUGWINS_display(bool* windowsOpen)
{

	for (size_t i = 0; i < NUM_DEBUG_WINDOWS; i++)
	{
		if (windowsOpen[i])
		{
			DebugWindowOpen[i] = true;
			DebugWindow[i]->open();
		}
		else
		{
			DebugWindowOpen[i] = false;
			DebugWindow[i]->close();
		}
	}
	DebugHelpOpen = windowsOpen[NUM_DEBUG_WINDOWS];
	DebugStatusBarOpen = windowsOpen[NUM_DEBUG_WINDOWS + 1];
	DebugScoreBoardOpen = windowsOpen[NUM_DEBUG_WINDOWS + 2];
}

//---------------------------------------------------------------------------

void DEBUGWINS_print(std::wstring_view s, int32_t window)
{
	DebugWindow[window]->print(s);
}

//---------------------------------------------------------------------------

void DEBUGWINS_setGameObject(int32_t debugObj, GameObjectPtr obj)
{

	if (debugObj == -1)
	{
		DebugGameObject[2] = DebugGameObject[1];
		DebugGameObject[1] = DebugGameObject[0];
		debugObj = 0;
	}
	DebugGameObject[debugObj] = obj;
}

//---------------------------------------------------------------------------

void DEBUGWINS_viewGameObject(int32_t debugObj)
{

	if (DebugGameObject[debugObj])
	{
		Stuff::Vector3D newPos = DebugGameObject[debugObj]->getPosition();
		eye->setPosition(newPos, false);
	}
}

//---------------------------------------------------------------------------

void DEBUGWINS_removeGameObject(GameObjectPtr obj)
{

	for (size_t i = 0; i < 3; i++)
		if (DebugGameObject[i] == obj)
		{
			DebugGameObject[i] = nullptr;
			DebugWindow[1 + i]->clear();
		}
}

//---------------------------------------------------------------------------

void DEBUGWINS_update(void)
{

	for (size_t i = 0; i < 3; i++)
		if (DebugGameObject[i])
			DebugGameObject[i]->updateDebugWindow(DebugWindow[1 + i]);
}

//---------------------------------------------------------------------------

void DEBUGWINS_renderSpecialWindows(void)
{

	gos_TextSetAttributes(DebugWindow[0]->font, 0xffffffff, 1.0, true, true, false, false);
	gos_TextSetRegion(0, 0, Environment.screenwidth, Environment.screenheight);
	gos_TextSetPosition(15, 10);
	if (DebugStatusBarOpen && DebugStatusBarString[0])
		gos_TextDraw(DebugStatusBarString);
	if (DebugScoreBoardOpen)
	{
		if (MPlayer)
		{
			int32_t curY = Environment.screenheight - 390;
			for (size_t i = 0; i < MPlayer->numTeams; i++)
			{
				wchar_t s[256];
				sprintf(s, "Team % d score = % d", i, MPlayer->teamScore[i]);
				gos_TextSetPosition(Environment.screenwidth - 380, curY);
				gos_TextDraw(s);
				curY += 10;
			}
			curY += 10;
			for (i = 0; i < MAX_MC_PLAYERS; i++)
				if (MPlayer->playerInfo[i].commanderid > -1)
				{
					wchar_t s[256];
					sprintf(s, "Player % d( % s) score = % d, % d kills, % d losses", i,
						MPlayer->playerInfo[i].name, MPlayer->playerInfo[i].score,
						MPlayer->playerInfo[i].kills, MPlayer->playerInfo[i].losses);
					gos_TextSetPosition(Environment.screenwidth - 380, curY);
					gos_TextDraw(s);
					curY += 10;
				}
		}
		else
		{
			gos_TextSetPosition(Environment.screenwidth - 275, Environment.screenheight - 120);
			gos_TextDraw("Single - player Mission");
		}
	}
	if (DebugHelpOpen)
	{
		gos_TextSetPosition(15, 10);
		gos_TextDraw("DEBUGGING COMMANDS");
		gos_TextSetPosition(15, 20);
		gos_TextDraw("---------------------------------------------------------"
					 "-------------------");

		gos_TextSetPosition(15, 30);
		gos_TextDraw("ALT - W");
		gos_TextSetPosition(140, 30);
		gos_TextDraw("Toggle Debug Windows");

		gos_TextSetPosition(15, 40);
		gos_TextDraw("ALT - T");
		gos_TextSetPosition(140, 40);
		gos_TextDraw("Teleport to mouse");

		gos_TextSetPosition(15, 50);
		gos_TextDraw("ALT - / ");
		gos_TextSetPosition(140, 50);
		gos_TextDraw("Quick Info");

		gos_TextSetPosition(15, 60);
		gos_TextDraw("ALT - SHFT - / ");
		gos_TextSetPosition(140, 60);
		gos_TextDraw("Select Debug Object");

		gos_TextSetPosition(15, 70);
		gos_TextDraw("ALT - #");
		gos_TextSetPosition(140, 70);
		gos_TextDraw("Center on Debug Object # (1 thru 3)");

		gos_TextSetPosition(15, 80);
		gos_TextDraw("ALT - CTRL - #");
		gos_TextSetPosition(140, 80);
		gos_TextDraw("Toggle Info for Debug Object(1 thru 3)");

		gos_TextSetPosition(15, 90);
		gos_TextDraw("ALT - M");
		gos_TextSetPosition(140, 90);
		gos_TextDraw("Show All Movers(toggle)");

		gos_TextSetPosition(15, 100);
		gos_TextDraw("ALT - G");
		gos_TextSetPosition(140, 100);
		gos_TextDraw("GlobalMap Move Log(toggle)");

		gos_TextSetPosition(15, 110);
		gos_TextDraw("ALT - B");
		gos_TextSetPosition(140, 110);
		gos_TextDraw("Enemies Brain Dead(toggle)");

		gos_TextSetPosition(15, 120);
		gos_TextDraw("ALT - P");
		gos_TextSetPosition(140, 120);
		gos_TextDraw("Use Goal Planning(toggle on selected movers)");

		gos_TextSetPosition(15, 130);
		gos_TextDraw("ALT - CTRL - P");
		gos_TextSetPosition(140, 130);
		gos_TextDraw("Computer AI Use Goal Planning(toggle)");

		gos_TextSetPosition(15, 140);
		gos_TextDraw("ALT - 4");
		gos_TextSetPosition(140, 140);
		gos_TextDraw("Damage Object for 1 point");

		gos_TextSetPosition(15, 150);
		gos_TextDraw("ALT - 5");
		gos_TextSetPosition(140, 150);
		gos_TextDraw("Damage Object for 4 points");

		gos_TextSetPosition(15, 160);
		gos_TextDraw("ALT - 6");
		gos_TextSetPosition(140, 160);
		gos_TextDraw("Damage Object for 9 points");

		gos_TextSetPosition(15, 170);
		gos_TextDraw("ALT - 7");
		gos_TextSetPosition(140, 170);
		gos_TextDraw("Damage Object for 16 points");

		gos_TextSetPosition(15, 180);
		gos_TextDraw("ALT - 8");
		gos_TextSetPosition(140, 180);
		gos_TextDraw("Damage Object for 25 points");

		gos_TextSetPosition(15, 190);
		gos_TextDraw("ALT - 9");
		gos_TextSetPosition(140, 190);
		gos_TextDraw("Damage Object for 36 points");

		gos_TextSetPosition(15, 200);
		gos_TextDraw("ALT - 0");
		gos_TextSetPosition(140, 200);
		gos_TextDraw("Disable Object");

		gos_TextSetPosition(15, 210);
		gos_TextDraw("ALT - V");
		gos_TextSetPosition(140, 210);
		gos_TextDraw("Display TacOrder Target in Debug Window");
	}
}

//---------------------------------------------------------------------------

void DEBUGWINS_render(void)
{

	for (size_t i = 0; i < NUM_DEBUG_WINDOWS; i++)
		DebugWindow[i]->render();
	DEBUGWINS_renderSpecialWindows();
}

//***************************************************************************

std::wstring_view
GetGameInformation()
{
	return (ExceptionGameMsg);
}

// int32_t cLoadString (HINSTANCE hinstance,  uint32_t uID, std::wstring_view lpBuffer,
// int32_t nBufferMax );

#define SnifferTime(x, y)
uint32_t startTime;
uint32_t endTime;

//---------------------------------------------------------------------------
void UpdateRenderers()
{
	if (!SnifferMode)
	{
		hasGuardBand = true;
		// hasGuardBand =
		// gos_GetMachineInformation(gos_Info_HasGuardBandClipping) != 0;
		canMultiTextureDetail = (gos_GetMachineInformation(gos_Info_CanMultitextureDetail) != 0);

		//---------------------------------------------------------------------------------
		// Doesn't work.  Always returns 0 for TNT in Win2K build 2183 with 3.55
		// detonator  Assume worst case is +/- 8.0 for now.  MaxMinUV =
		// gos_GetMachineInformation(gos_Info_GetMaximumUVSize);

		uint32_t bcolour = 0x0;
		if (eye && mission->isActive())
			bcolour = eye->fogcolour;

		gos_SetupViewport(1, 1.0, 1, bcolour, 0.0, 0.0, 1.0,
			1.0); // ALWAYS FULL SCREEN for now
		gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);

		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);

		gos_SetRenderState(gos_State_AlphaTest, TRUE);

		gos_SetRenderState(gos_State_Clipping, TRUE);

		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);

		gos_SetRenderState(gos_State_Dither, 1);

		//---------------------------------------------------------------------------------------
		// Tell the mission to render, the operation to render and the logistics
		// to render here.
		if (mission && (!optionsScreenWrapper || optionsScreenWrapper->isDone()))
			mission->render();

		if (logistics)
		{
			float viewMulX, viewMulY, viewAddX, viewAddY;
			gos_GetViewport(&viewMulX, &viewMulY, &viewAddX, &viewAddY);
			userInput->setViewport(viewMulX, viewMulY, viewAddX, viewAddY);

			logistics->render();
		}

		if (optionsScreenWrapper && !optionsScreenWrapper->isDone())
		{
			float viewMulX, viewMulY, viewAddX, viewAddY;
			gos_GetViewport(&viewMulX, &viewMulY, &viewAddX, &viewAddY);
			userInput->setViewport(viewMulX, viewMulY, viewAddX, viewAddY);

			optionsScreenWrapper->render();
		}

		//------------------------------------------------------------
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		userInput->render();

		DEBUGWINS_render();

#ifdef LAB_ONLY
		if (currentLineElement)
		{
			for (size_t i = 0; i < currentLineElement; i++)
			{
				debugLines[i]->draw();
			}
		}
#endif
	}
	else
	{
		if (gos_GetMachineInformation(gos_Info_GetDeviceLocalMemory, curDevice) >= 6291456)
		{
			startTime = timeGetTime();

			gos_SetupViewport(1, 1.0, 1, 0, 0.0, 0.0, 1.0, 1.0); // ALWAYS FULL SCREEN for now

			if (Environment.Renderer == 3)
			{
				gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
				gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
				gos_SetRenderState(gos_State_MonoEnable, 1);
				gos_SetRenderState(gos_State_Perspective, 0);
				gos_SetRenderState(gos_State_Clipping, 2);
				gos_SetRenderState(gos_State_AlphaTest, 0);
				gos_SetRenderState(gos_State_Specular, 0);
				gos_SetRenderState(gos_State_Dither, 0);
				gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
				gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
				gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
				gos_SetRenderState(gos_State_ZCompare, 1);
				gos_SetRenderState(gos_State_ZWrite, 1);
				// uint32_t fogcolour = 0x009f9f9f;
				// gos_SetRenderState( gos_State_Fog, (int32_t)&fogcolour);
			}
			else
			{
				gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
				gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
				gos_SetRenderState(gos_State_MonoEnable, 1);
				gos_SetRenderState(gos_State_Perspective, 1);
				gos_SetRenderState(gos_State_Clipping, 2);
				gos_SetRenderState(gos_State_AlphaTest, 1);
				gos_SetRenderState(gos_State_Specular, 1);
				gos_SetRenderState(gos_State_Dither, 1);
				gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulateAlpha);
				gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
				gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
				gos_SetRenderState(gos_State_ZCompare, 1);
				gos_SetRenderState(gos_State_ZWrite, 1);
				uint32_t fogcolour = 0x009f9f9f;
				gos_SetRenderState(gos_State_Fog, (int32_t)&fogcolour);
			}

			// Send down 5000 triangles
			int32_t nIterations = 5;
			for (size_t i = 0; i < nIterations; i++)
			{

				uint32_t totalvertices = 3000;
				gos_SetRenderState(gos_State_Texture, testTextureHandle);
				gos_RenderIndexedArray(testVertex, totalvertices, indexArray, totalvertices);
			}

			endTime = timeGetTime() - startTime;

			totalTime += endTime;
		}
	}
}

//---------------------------------------------------------------------------
#define GAME_REG_KEY "Software\\Microsoft\\Microsoft Games\\MechCommander2\\1.0"
typedef uint32_t(__stdcall* EBUPROC)(
	std::wstring_view lpRegKeyLocation, std::wstring_view lpEULAFileName, std::wstring_view lpWarrantyFileName, BOOL fCheckForFirstRun);

bool FirstRunEula(void)
{
	return (true);
}

std::iostream effectStream = nullptr;
extern MidLevelRenderer::MLRClipper* theClipper;

bool gameStarted = false;
void InitializeGameEngine()
{
	__asm push esi;

	// gotta have this no matter what
	gosResourceHandle = gos_OpenResourceDLL("mc2res.dll");

	// Check for enough SwapFile Space FIRST!!!
	// In order to do that, we must force Win2K/XP to enlarge
	// its swapfile at the get go to insure goodness and that
	// the message does not come up during game run.
	PVOID testMem = VirtualAlloc(nullptr, 123000000, MEM_COMMIT, PAGE_READWRITE);

	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);

	if (ms.dwTotalPageFile < 250000000)
	{
		wchar_t txt[4096];
		wchar_t msg[4096];
		cLoadString(IDS_SWAPFILE_TOO_SMALL, txt, 4095);
		sprintf(msg, txt, (ms.dwAvailPageFile / (1024 * 1024)));

		wchar_t caption[1024];
		cLoadString(IDS_SWAPFILE_CAPTION, caption, 1023);

		MessageBox(nullptr, msg, caption, MB_OK | MB_ICONWARNING);

		gos_TerminateApplication();
		return;
	}

	if (testMem)
		VirtualFree(testMem, 0, MEM_RELEASE);

	cLoadString(IDS_MC2_FILEMISSING, FileMissingString, 511);
	cLoadString(IDS_MC2_CDMISSING, CDMissingString, 1023);
	cLoadString(IDS_MC2_MISSING_TITLE, MissingTitleString, 255);

	if (!EULAShown)
	{
		if (!FirstRunEula())
		{
			gos_TerminateApplication();
			return;
		}
	}

	// Check for sufficient hard Drive space on drive game is running from
	wchar_t currentPath[1024];
	gos_GetCurrentPath(currentPath, 1023);
	int64_t driveSpace = gos_GetDriveFreeSpace(currentPath);
	if (driveSpace < (20 * 1024 * 1024))
	{
		wchar_t title[256];
		wchar_t msg[2048];
		cLoadString(IDS_GAME_HDSPACE_ERROR, title, 255);
		cLoadString(IDS_GAME_HDSPACE_MSG, msg, 2047);
		uint32_t result = MessageBox(nullptr, msg, title, MB_OKCANCEL | MB_ICONWARNING);
		if (result == IDCANCEL)
			ExitGameOS();
	}

	// Check if we are a Voodoo 3.  If so, ONLY allow editor to run IF
	// Starting resolution is 1024x768x16 or LOWER.  NO 32-BIT ALLOWED!
	if ((gos_GetMachineInformation(gos_Info_GetDeviceVendorID, 0) == 0x121a) && (gos_GetMachineInformation(gos_Info_GetDeviceDeviceID, 0) == 0x0005))
	{
		DEVMODE dev;
		memset(&dev, 0, sizeof(DEVMODE));
		dev.dmSize = sizeof(DEVMODE);
		dev.dmSpecVersion = DM_SPECVERSION;
		EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dev);

		if ((dev.dmPelswidth > 1024) || (dev.dmPelsheight > 768) || (dev.dmBitsPerPel > 16))
		{
			wchar_t title[256];
			wchar_t msg[2048];
			cLoadString(IDS_GAME_ERROR, title, 255);
			cLoadString(IDS_GAME_VOODOO3, msg, 2047);
			MessageBox(nullptr, msg, title, MB_OK | MB_ICONWARNING);
			ExitGameOS();
		}
	}

	// Then, we should check to see if the options.cfg exists.  if NOT,
	// Bring up a sniffer warning dialog, sniff, bring up sniffer end dialog and
	// end.
	if (!fileExists("options.cfg"))
	{
		// Must also check for a RIVA TNT card with driver 4.10.1.0131
		// If this card is in the machine with this driver, do NOT allow sniffer
		// to run. Just copy minprefs.cfg to options.cfg and move on.
		if ((gos_GetMachineInformation(gos_Info_GetDeviceVendorID, 0) == 0x10de) && (gos_GetMachineInformation(gos_Info_GetDeviceDeviceID, 0) == 0x0020) && (gos_GetMachineInformation(gos_Info_GetDeviceDriverVersionHi, 0) == 0x0004000a) && (gos_GetMachineInformation(gos_Info_GetDeviceDriverVersionLo, 0) == 0x00010083))
		{
			CopyFile("minprefs.cfg", "orgprefs.cfg", true);
			CopyFile("minprefs.cfg", "options.cfg", true);
		}
		else
		{
			// NEW Plan.  Regardless, copy the minprefs to options.  That way,
			// sniffer can only screw up once!!
			// After the first run, it will no longer run and the game will
			// continue with minprefs.
			SnifferMode = true;
			CopyFile("minprefs.cfg", "orgprefs.cfg", true);
			CopyFile("minprefs.cfg", "options.cfg", true);
		}
	}

	// Seed the random Number generator.
	gos_srand(timeGetTime());

	if (!SnifferMode)
	{
		//---------------------------------------------------------------------

		float doubleClickThreshold = 0.2f;
		int32_t dragThreshold = .016667;

		Environment.Key_Exit = -1; // so escape doesn't kill your app

		//-------------------------------------------------------------
		// Find the CDPath in the registry and save it off so I can
		// look in CD Install Path for files.

		// Changed for the shared source release, just set to current directory
		// uint32_t maxPathLength = 1023;
		// gos_LoadDataFromRegistry("CDPath", CDInstallPath, &maxPathLength);
		// if (!maxPathLength)
		//	strcpy(CDInstallPath,"..\\");
		strcpy(CDInstallPath, ".\\");

		//--------------------------------------------------------------
		// Start the SystemHeap and globalHeapList
		globalHeapList = new HeapList;
		gosASSERT(globalHeapList != nullptr);

		globalHeapList->init();
		globalHeapList->update(); // Run Instrumentation into GOS Debugger
			// Screen

		systemHeap = new UserHeap;
		gosASSERT(systemHeap != nullptr);

		systemHeap->init(systemHeapSize, "SYSTEM");

		// Start finding the Leaks
		//		systemHeap->startHeapMallocLog();

		// Check for OrgPrefs.cfg.  If this file is not present,
		// We need to create it because sniffer didn't run.
		// Create a new prefs.cfg, too.  This way, we can easily restore
		// them to a running state if they screw up their prefs.
		// All they have to do is delete the orgprefs.cfg file.
		// OR re-install!!
		//
		// ALWAYS use minPrefs.cfg here!
		if (!fileExists("orgprefs.cfg"))
		{
			CopyFile("minprefs.cfg", "orgprefs.cfg", true);
			CopyFile("minprefs.cfg", "options.cfg", true);
		}

		//--------------------------------------------------------------
		// Read in System.CFG
		FitIniFilePtr systemFile = new FitIniFile;

#ifdef _DEBUG
		int32_t systemOpenResult =
#endif
			systemFile->open("system.cfg");

#ifdef _DEBUG
		if (systemOpenResult != NO_ERROR)
		{
			wchar_t Buffer[256];
			gos_GetCurrentPath(Buffer, 256);
			STOP(("Cannot find \"system.cfg\" file in %s", Buffer));
		}
#endif

		{
#ifdef _DEBUG
			int32_t systemBlockResult =
#endif
				systemFile->seekBlock("systemHeap");
			gosASSERT(systemBlockResult == NO_ERROR);
			{
				int32_t result = systemFile->readIdULong("systemHeapSize", systemHeapSize);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdULong("guiHeapSize", guiHeapSize);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdULong("logisticsHeapSize", logisticsHeapSize);
				gosASSERT(SUCCEEDED(result));
			}

#ifdef _DEBUG
			HRESULT systemPathResult =
#endif
				systemFile->seekBlock("systemPaths");
			gosASSERT(systemPathResult == NO_ERROR);
			{
				HRESULT result = systemFile->readIdString("terrainPath", terrainPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("artPath", artPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("fontPath", fontPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("savePath", savePath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("spritePath", spritePath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("shapesPath", shapesPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("soundPath", soundPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("objectPath", objectPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("cameraPath", cameraPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("tilePath", tilePath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("missionPath", missionPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("warriorPath", warriorPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("profilePath", profilePath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("interfacepath", interfacePath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("moviepath", moviePath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("CDsoundPath", CDsoundPath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("CDmoviepath", CDmoviePath, 79);
				gosASSERT(SUCCEEDED(result));

				result = systemFile->readIdString("CDspritePath", CDspritePath, 79);
				gosASSERT(SUCCEEDED(result));
			}

#ifdef _DEBUG
			int32_t fastFileResult =
#endif
				systemFile->seekBlock("FastFiles");
			gosASSERT(fastFileResult == NO_ERROR);
			{
				int32_t result = systemFile->readIdLong("NumFastFiles", maxFastFiles);
				if (FAILED(result))
					maxFastFiles = 0;

#if CONSIDERED_OBSOLETE
				if (maxFastFiles)
				{
					fastFiles = (FastFile**)malloc(maxFastFiles * sizeof(FastFile*));
					memset(fastFiles, 0, maxFastFiles * sizeof(FastFile*));
					int32_t fileNum = 0;
					wchar_t fastFileId[10];
					wchar_t fileName[100];
					sprintf(fastFileId, "File%d", fileNum);
					while (systemFile->readIdString(fastFileId, fileName, 99) == NO_ERROR)
					{
						bool result = FastFileInit(fileName);
						if (!result)
							STOP(("Unable to startup fastfiles.  Probably an "
								  "old one in the directory!!"));
						fileNum++;
						sprintf(fastFileId, "File%d", fileNum);
					}
				}
			}
#endif
			HRESULT result = systemFile->seekBlock("UseMusic");
			if (SUCCEEDED(result))
				useMusic = TRUE;
			else
				useMusic = FALSE;

			result = systemFile->seekBlock("UseSound");
			if (SUCCEEDED(result))
			{
				useSound = TRUE;
			}
			else
			{
				useSound = FALSE;
				useMusic = FALSE;
			}

			result = systemFile->seekBlock("CameraSettings");
			if (SUCCEEDED(result))
			{
				result = systemFile->readIdFloat("MaxPerspective", Camera::MAX_PERSPECTIVE);
				if (FAILED(result))
					Camera::MAX_PERSPECTIVE = 88.0f;
				if (Camera::MAX_PERSPECTIVE > 90.0f)
					Camera::MAX_PERSPECTIVE = 90.0f;
				result = systemFile->readIdFloat("MinPerspective", Camera::MIN_PERSPECTIVE);
				if (FAILED(result))
					Camera::MIN_PERSPECTIVE = 18.0f;
				if (Camera::MIN_PERSPECTIVE < 0.0f)
					Camera::MIN_PERSPECTIVE = 0.0f;
				result = systemFile->readIdFloat("MaxOrtho", Camera::MAX_ORTHO);
				if (FAILED(result))
					Camera::MAX_ORTHO = 88.0f;
				if (Camera::MAX_ORTHO > 90.0f)
					Camera::MAX_ORTHO = 90.0f;
				result = systemFile->readIdFloat("MinOrtho", Camera::MIN_ORTHO);
				if (FAILED(result))
					Camera::MIN_ORTHO = 18.0f;
				if (Camera::MIN_ORTHO < 0.0f)
					Camera::MIN_ORTHO = 0.0f;
				result = systemFile->readIdFloat("AltitudeMinimum", Camera::AltitudeMinimum);
				if (FAILED(result))
					Camera::AltitudeMinimum = 560.0f;
				if (Camera::AltitudeMinimum < 110.0f)
					Camera::AltitudeMinimum = 110.0f;
				result = systemFile->readIdFloat("AltitudeMaximumHi", Camera::AltitudeMaximumHi);
				if (FAILED(result))
					Camera::AltitudeMaximumHi = 1600.0f;
				result = systemFile->readIdFloat("AltitudeMaximumLo", Camera::AltitudeMaximumLo);
				if (FAILED(result))
					Camera::AltitudeMaximumHi = 1500.0f;
			}
		}

		systemFile->close();
		delete systemFile;
		systemFile = nullptr;

		if (initGameLogs)
		{
			GameLog::setup();
			if (initNetLog && !NetLog)
			{
				NetLog = GameLog::getNewFile();
				if (!NetLog)
					Fatal(0, " Couldn't create Net Log ");
				int32_t err = NetLog->open("net.log");
				if (err)
					Fatal(0, " Couldn't open Net Log ");
			}
			if (initCombatLog && !CombatLog)
			{
				CombatLog = GameLog::getNewFile();
				if (!CombatLog)
					Fatal(0, " Couldn't create Combat Log ");
				int32_t err = CombatLog->open("combat.log");
				if (err)
					Fatal(0, " Couldn't open Combat Log ");
			}
			if (initBugLog && !BugLog)
			{
				BugLog = GameLog::getNewFile();
				if (!BugLog)
					Fatal(0, " Couldn't create Bug Log ");
				int32_t err = BugLog->open("bug.log");
				if (err)
					Fatal(0, " Couldn't open Bug Log ");
			}
			if (initLRMoveLog && !GlobalMap::logEnabled)
			{
				GlobalMap::toggleLog();
			}
		}

		//--------------------------------------------------------------
		// Read in Prefs.cfg
		bool fullScreen = false;
		FitIniFilePtr prefsFile = new FitIniFile;
		FitIniFilePtr optsFile = new FitIniFile;

		int32_t prefsOpenResult = prefsFile->open("prefs.cfg");

		gosASSERT(prefsOpenResult == NO_ERROR);

		prefsOpenResult = optsFile->open("options.cfg");

		gosASSERT(prefsOpenResult == NO_ERROR);

		{
#ifdef _DEBUG
			HRESULT prefsBlockResult =
#endif
				prefsFile->seekBlock("MechCommander2");
			optsFile->seekBlock("MechCommander2");
			gosASSERT(prefsBlockResult == NO_ERROR);
			{
				/*The following commented out options have been moved to
				"options.cfg" and are handled by the CPrefs class (in
				"prefs.h"). They have been moved to CPrefs because these options
				are potentially affected by the options screen and may need to
				support "apply, cancel and save" functionality.

				Understood.  Game must also know about them know or it cannot
				start up correctly!! -fs
				*/
				// store volume settings in global variable since soundsystem
				// does not exist yet.  These will be set in SoundSystem::init()
				HRESULT result = optsFile->readIdLong("DigitalMasterVolume", DigitalMasterVolume);
				if (FAILED(result))
					DigitalMasterVolume = 255;
				result = optsFile->readIdLong("MusicVolume", MusicVolume);
				if (FAILED(result))
					MusicVolume = 64;
				result = optsFile->readIdLong("RadioVolume", RadioVolume);
				if (FAILED(result))
					RadioVolume = 64;
				result = optsFile->readIdLong("SFXVolume", sfxVolume);
				if (FAILED(result))
					sfxVolume = 64;
				result = optsFile->readIdLong("BettyVolume", BettyVolume);
				if (FAILED(result))
					BettyVolume = 64;
				result = optsFile->readIdBoolean("Shadows", useShadows);
				if (FAILED(result))
					useShadows = true;
				result = optsFile->readIdBoolean("DetailTexture", useWaterInterestTexture);
				if (FAILED(result))
					useWaterInterestTexture = true;
				result = optsFile->readIdLong("Difficulty", GameDifficulty);
				if (FAILED(result))
					GameDifficulty = 1;
				result = optsFile->readIdBoolean("UnlimitedAmmo", useUnlimitedAmmo);
				if (FAILED(result))
					useUnlimitedAmmo = true;
				result = optsFile->readIdLong("Rasterizer", renderer);
				if (FAILED(result))
					renderer = 0;
				if ((renderer < 0) || (renderer > 3))
					renderer = 0;
				result = optsFile->readIdLong("Resolution", resolution);
				if (FAILED(result))
					resolution = 0;
				result = optsFile->readIdBoolean("FullScreen", fullScreen);
				if (FAILED(result))
					fullScreen = true;
				result = optsFile->readIdLong("Brightness", gammaLevel);
				if (FAILED(result))
					gammaLevel = 0;
				result =
					optsFile->readIdBoolean("useLeftRightMouseProfile", useLeftRightMouseProfile);
				if (FAILED(result))
					useLeftRightMouseProfile = true;
				bool asyncMouse = false;
				result = optsFile->readIdBoolean("useAsyncMouse", asyncMouse);
				if (FAILED(result))
					asyncMouse = false;
				mc2UseAsyncMouse = asyncMouse;
				int32_t filterSetting;
				result = prefsFile->readIdLong("FilterState", filterSetting);
				if (SUCCEEDED(result))
				{
					switch (filterSetting)
					{
					default:
					case 0:
						FilterState = gos_FilterNone;
						break;
					case 1:
						FilterState = gos_FilterBiLinear;
						break;
					case 2:
						FilterState = gos_FilterTriLinear;
						break;
					}
				}
				result = prefsFile->readIdLong("TerrainTextureRes", TERRAIN_TXM_SIZE);
				if (FAILED(result))
					TERRAIN_TXM_SIZE = 64;
				result = prefsFile->readIdLong("ObjectTextureRes", ObjectTextureSize);
				if (FAILED(result))
					ObjectTextureSize = 128;
				result = prefsFile->readIdFloat("DoubleClickThreshold", doubleClickThreshold);
				if (FAILED(result))
					doubleClickThreshold = 0.2f;
				result = prefsFile->readIdLong("DragThreshold", dragThreshold);
				if (FAILED(result))
					dragThreshold = .01667f;
				result = prefsFile->readIdULong("BaseVertexcolour", BaseVertexcolour);
				if (FAILED(result))
					BaseVertexcolour = 0x00000000;
				result = prefsFile->readIdBoolean("RealLOS", useRealLOS);
				if (FAILED(result))
					useRealLOS = true;
				result = prefsFile->readIdLong("GameVisibleVertices", GameVisibleVertices);
				if (FAILED(result))
					GameVisibleVertices = 30;
				result = prefsFile->readIdFloat("MaxClipDistance", Camera::MaxClipDistance);
				if (FAILED(result))
					Camera::MaxClipDistance = 3000.0f;
				result = prefsFile->readIdFloat("MinHazeDistance", Camera::MinHazeDistance);
				if (FAILED(result))
					Camera::MinHazeDistance = 2000.0f;
				result = prefsFile->readIdFloat("View0Zoom", Camera::cameraZoom[0]);
				if (FAILED(result))
					Camera::cameraZoom[0] = 1200.0f;
				result = prefsFile->readIdFloat("View0Tilt", Camera::cameraTilt[0]);
				if (FAILED(result))
					Camera::cameraTilt[0] = 35.0f;
				result = prefsFile->readIdFloat("View1Zoom", Camera::cameraZoom[1]);
				if (FAILED(result))
					Camera::cameraZoom[1] = 1200.0f;
				result = prefsFile->readIdFloat("View1Tilt", Camera::cameraTilt[1]);
				if (FAILED(result))
					Camera::cameraTilt[1] = 35.0f;
				result = prefsFile->readIdFloat("View2Zoom", Camera::cameraZoom[2]);
				if (FAILED(result))
					Camera::cameraZoom[2] = 1200.0f;
				result = prefsFile->readIdFloat("View2Tilt", Camera::cameraTilt[2]);
				if (FAILED(result))
					Camera::cameraTilt[2] = 35.0f;
				result = prefsFile->readIdFloat("View3Zoom", Camera::cameraZoom[3]);
				if (FAILED(result))
					Camera::cameraZoom[3] = 1200.0f;
				result = prefsFile->readIdFloat("View3Tilt", Camera::cameraTilt[3]);
				if (FAILED(result))
					Camera::cameraTilt[3] = 35.0f;
			}
		}

		prefsFile->close();
		optsFile->close();

		delete prefsFile;
		prefsFile = nullptr;

		delete optsFile;
		optsFile = nullptr;

		//---------------------------------------------------------------------
		// void __stdcall gos_SetScreenMode( uint32_t width, uint32_t height,
		// uint32_t bitDepth=16, uint32_t Device=0, bool disableZBuffer=0, bool
		// AntiAlias=0, bool RenderToVram=0, bool GotoFullScreen=0, int32_t
		// DirtyRectangle=0, bool GotoWindowMode=0, bool EnableStencil=0,
		// uint32_t Renderer=0 );

		/*The following commented out code is now handled by
		CPrefs::applyPrefs() (in "prefs.cpp").*/
		/*
		switch (resolution)
		{
			case 0:			//640by480
				if (renderer == 3)
					gos_SetScreenMode(640,480,16,0,0,0,true,fullScreen,0,!fullScreen,0,renderer);
				else
					gos_SetScreenMode(640,480,16,0,0,0,0,fullScreen,0,!fullScreen,0,renderer);
				break;

			case 1:			//800by600
				if (renderer == 3)
					gos_SetScreenMode(800,600,16,0,0,0,true,fullScreen,0,!fullScreen,0,renderer);
				else
					gos_SetScreenMode(800,600,16,0,0,0,0,fullScreen,0,!fullScreen,0,renderer);
				break;

			case 2:			//1024by768
				if (renderer == 3)
					gos_SetScreenMode(1024,768,16,0,0,0,true,fullScreen,0,!fullScreen,0,renderer);
				else
					gos_SetScreenMode(1024,768,16,0,0,0,0,fullScreen,0,!fullScreen,0,renderer);
				break;

			case 3:			//1280by1024
				if (renderer == 3)
					gos_SetScreenMode(1280,1024,16,0,0,0,true,fullScreen,0,!fullScreen,0,renderer);
				else
					gos_SetScreenMode(1280,1024,16,0,0,0,0,fullScreen,0,!fullScreen,0,renderer);
				break;

			case 4:			//1600by1200
				if (renderer == 3)
					gos_SetScreenMode(1600,1200,16,0,0,0,true,fullScreen,0,!fullScreen,0,renderer);
				else
					gos_SetScreenMode(1600,1200,16,0,0,0,0,fullScreen,0,!fullScreen,0,renderer);
				break;
		}
		*/

		/* load and apply options from "options.cfg" */
		prefs.load();
		prefs.applyPrefs();

		//--------------------------------------------------
		// Setup Mouse Parameters from Prefs.CFG
		userInput = new UserInput;
		userInput->init();
		userInput->setMouseDoubleClickThreshold(doubleClickThreshold);
		userInput->setMouseDragThreshold(dragThreshold);
		//--------------------------------------------------

		wchar_t temp[256];
		cLoadString(IDS_FLOAT_HELP_FONT, temp, 255);
		std::wstring_view pStr = strstr(temp, ",");
		if (pStr)
		{
			gosFontScale = -atoi(pStr + 1);
			*pStr = 0;
		}
		wchar_t path[256];
		strcpy(path, "assets\\graphics\\");
		strcat(path, temp);

		gosFontHandle = gos_LoadFont(path);
		FontHandle = gosFontHandle;

		globalFloatHelp = new FloatHelp(MAX_FLOAT_HELPS);

		//
		//----------------------------------
		// Start associated stuff.
		//----------------------------------

		//-------------------------------
		// Used to output debug stuff!
		// Mondo COOL!
		// simply do this in the code and stuff goes to the file called
		// mc2.output
		//		DEBUG_STREAM << thing_you_want_to_output
		//
		// IMPORTANT NOTE:
		Stuff::InitializeClasses();
		MidLevelRenderer::InitializeClasses(8192 * 4, 8192, 0, 0, true);
		gosFX::InitializeClasses();

		gos_PushCurrentHeap(MidLevelRenderer::Heap);

		MidLevelRenderer::TGAFilePool* pool = new MidLevelRenderer::TGAFilePool("data\\tgl\\128\\");
		MidLevelRenderer::MLRTexturePool::Instance = new MidLevelRenderer::MLRTexturePool(pool);

		MidLevelRenderer::MLRSortByOrder* cameraSorter =
			new MidLevelRenderer::MLRSortByOrder(MidLevelRenderer::MLRTexturePool::Instance);
		theClipper = new MidLevelRenderer::MLRClipper(0, cameraSorter);

		gos_PopCurrentHeap();

		//------------------------------------------------------
		// Start the GOS FX.
		gos_PushCurrentHeap(gosFX::Heap);

		gosFX::EffectLibrary::Instance = new gosFX::EffectLibrary();
		Check_Object(gosFX::EffectLibrary::Instance);

		FullPathFileName effectsName;
		effectsName.init(effectsPath, "mc2.fx", "");

		File effectFile;
		int32_t result = effectFile.open(effectsName);
		if (FAILED(result))
			STOP(("Could not find MC2.fx"));

		int32_t effectsSize = effectFile.fileSize();
		uint8_t* effectsData = (uint8_t*)systemHeap->Malloc(effectsSize);
		effectFile.read(effectsData, effectsSize);
		effectFile.close();

		effectStream = new std::iostream(effectsData, effectsSize);
		gosFX::EffectLibrary::Instance->Load(effectStream);

		gosFX::LightManager::Instance = new gosFX::LightManager();

		gos_PopCurrentHeap();

		systemHeap->Free(effectsData);

		//--------------------------------------------------------------
		// Start the GUI Heap.
		guiHeap = new UserHeap;
		gosASSERT(guiHeap != nullptr);

		guiHeap->init(guiHeapSize, "GUI");

		//------------------------------------------------
		// Fire up the MC Texture Manager.
		mcTextureManager = new MC_TextureManager;
		mcTextureManager->start();

		//--------------------------------------------------------------
		// Load up the mouse cursors
		userInput->initMouseCursors("cursors");
		userInput->mouseOff();
		userInput->setMouseCursor(mState_NORMAL);

		//------------------------------------------------
		// Give the Sound System a Whirl!
		soundSystem = new GameSoundSystem;
		soundSystem->init();
		((SoundSystem*)soundSystem)->init("sound");
		sndSystem = soundSystem; // for things in the lib that use sound

		//-----------------------------------------------
		// Only used by camera to retrieve screen coords.
		globalPane = new PANE;
		globalWindow = new WINDOW;

		globalPane->window = globalWindow;
		globalPane->x0 = 0;
		globalPane->y0 = 0;
		globalPane->x1 = Environment.screenwidth;
		globalPane->y1 = Environment.screenheight;

		globalWindow->buffer = nullptr; // This is set at the start of Renders.
			// For now we HOLD LOCK during entire old
			// 2D render test.  This will go away
			// once we hit 3D
		globalWindow->shadow = nullptr;
		globalWindow->stencil = nullptr;
		globalWindow->x_max = globalPane->x1 - globalPane->x0 - 1;
		globalWindow->y_max = globalPane->y1 - globalPane->y0 - 1;

		//---------------------------------------------------------
		// Start the Timers
		timerManager = new TimerManager;
		timerManager->init();

		//---------------------------------------------------------
		// Start the colour table code
		initcolourTables();

		//---------------------------------------------------------
		// Start the Mission, Scenario and Logistics classes here
		mission = new Mission;

		logistics = new Logistics;

		GameDebugWindow::setFont("assets\\graphics\\arial8.tga");
		DEBUGWINS_init();

		StartupNetworking();

#ifdef USE_movie
		movieSoundUseDirectSound(0);
#endif

		if (justStartMission)
		{
			logistics->setLogisticsState(log_STARTMISSIONFROMCMDLINE);
			wchar_t commandersToLoad[MAX_MC_PLAYERS][3] = {{0, 0, 0}, {1, 1, 1}, {2, 0, 2}, {3, 3, 3},
				{4, 4, 4}, {5, 5, 5}, {6, 6, 6}, {7, 7, 7}};
			mission->init(missionName, MISSION_LOAD_SP_QUICKSTART, 0, nullptr, commandersToLoad, 2);
			eye->activate();
			eye->update();
			mission->start();
		}
		else
		{
			int32_t param = log_SPLASH;
			if (MPlayer && MPlayer->launchedFromLobby)
				param = log_ZONE;
			logistics->start(param); // Always start with logistics in Splash Screen Mode
			Mission::initBareMinimum();
		}

		initDialogs();

		gos_EnableSetting(gos_Set_LoseFocusBehavior, 2);

		uint32_t numJoysticks = gosJoystick_CountJoysticks();

		for (size_t i = 0; i < numJoysticks; i++)
		{
			gosJoystick_Info joyInfo;
			gosJoystick_GetInfo(i, &joyInfo);
			// Search for the Attila Strategic Commander here.
			// Look for the Attila VidPid 0x045e0033 (Unique!)
			// Andy G did not do what I asked for here.  Will do in our version
			// when we branch. For NOW, ANY 3 Axis joystick with 15 buttons is a
			// Strategic Commander.
			if ((joyInfo.nAxes == 3) && (joyInfo.nButtons == 15))
			{
				userInput->addAttila(i);
				gosJoystick_SetPolling(i, true, 0.1f);
			}
		}

		// Time BOMB goes here.
		// Set Date and write Binary data to registry under key
		// GraphicsData!!
		SYSTEMTIME bombDate;
		uint32_t dataSize = sizeof(SYSTEMTIME);
		gos_LoadDataFromRegistry("GraphicsDataInit2", &bombDate, &dataSize);
		if (dataSize == 0)
		{
			bombDate.wYear = 2001;
			bombDate.wMonth = 6;
			bombDate.wDayOfWeek = 5;
			bombDate.wDay = 15;
			bombDate.wHour = 0;
			bombDate.wMinute = 0;
			bombDate.wSecond = 0;
			bombDate.wMilliseconds = 0;
			dataSize = sizeof(SYSTEMTIME);
			gos_SaveDataToRegistry("GraphicsDataInit2", &bombDate, dataSize);
		}

		gameStarted = true;
	}
	else
	{
		wchar_t msgBuffer[4096];
		wchar_t msgTitle[1024];
		cLoadString(IDS_SNIFFER_INIT_MSG, msgBuffer, 4095);
		cLoadString(IDS_SNIFFER_INIT_TITLE, msgTitle, 1023);
		MessageBox(nullptr, msgBuffer, msgTitle, MB_OK);
		//-------------------------------------------------------------
		// Find the CDPath in the registry and save it off so I can
		// look in CD Install Path for files.
		// Changed for the shared source release, just set to current directory
		// uint32_t maxPathLength = 1023;
		// gos_LoadDataFromRegistry("CDPath", CDInstallPath, &maxPathLength);
		// if (!maxPathLength)
		//	memset(CDInstallPath,0,1024);
		memset(CDInstallPath, 0, 1024);
		// Set ScreenMode to 800x600x16 FULLSCREEN!!!!
		// A window will return BAD DATA!!
		//
		// If this fails, GAME CANNOT RUN on their machine!!!!!
		//
		// Check which rasterizer comes up.  If software, tell prefs we cannot
		// run in hardware!!
		//
		while (((gos_GetMachineInformation(gos_Info_GetDeviceLocalMemory, curDevice) + gos_GetMachineInformation(gos_Info_GetDeviceAGPMemory, curDevice)) < 6291456) && (curDevice < gos_GetMachineInformation(gos_Info_NumberDevices)))
			curDevice++;
		if (curDevice < gos_GetMachineInformation(gos_Info_NumberDevices))
			gos_SetScreenMode(800, 600, 16, curDevice, 0, 0, 0, true, 0, 0, 0, 0);
		// Create about a thousand textured random triangles.
		testVertex = (gos_VERTEX*)malloc(sizeof(gos_VERTEX) * 3000);
		indexArray = (uint16_t*)malloc(sizeof(uint16_t) * 3000);
		for (size_t i = 0; i < 3000; i++)
		{
			testVertex[i].x = RandomNumber(1000) - 100;
			testVertex[i].y = RandomNumber(800) - 100;
			testVertex[i].z = ((float)RandomNumber(150) / 100.0f) - 0.25f;
			testVertex[i].rhw = 0.5f;
			testVertex[i].u = (float)RandomNumber(100) / 100.0f;
			testVertex[i].v = (float)RandomNumber(100) / 100.0f;
			testVertex[i].argb = 0x3fffffff;
			testVertex[i].frgb = 0x3f1f2f3f;
			indexArray[i] = i;
		}
		testTextureHandle = gos_NewTextureFromFile(gos_Texture_Solid, "testTxm.tga");
	}

	// Make any directories we need which should be empty.
	CreateDirectory(savePath, nullptr);
	CreateDirectory(transcriptsPath, nullptr);

	// Startup the Office Watson Handler.
	InitDW();

	__asm pop esi;
}

//---------------------------------------------------------------------------

void TerminateGameEngine()
{
	if (!gameStarted)
		return;
	if (!SnifferMode)
	{
		//--------------------------------------------------
		// Setup Mouse Parameters from Prefs.CFG
		if (userInput)
		{
			// Must call this to shut off the mouse async draw thread.
			// DO NOT DELETE Yet!!  Too many things assume this is around.
			userInput->destroy();
		}
		//--------------------------------------------------
		DEBUGWINS_destroy();
		ShutdownNetworking();
		delete theClipper;
		theClipper = nullptr;
		//---------------------------------------------------------
		// End the Mission, Operation and Logistics classes here
		if (mission)
		{
			mission->destroy(false);
			delete mission;
			mission = nullptr;
		}
		if (logistics)
		{
			logistics->destroy();
			delete logistics;
			logistics = nullptr;
		}
		endDialogs();
		if (optionsScreenWrapper)
		{
			optionsScreenWrapper->destroy();
			delete optionsScreenWrapper;
			optionsScreenWrapper = nullptr;
		}
		//---------------------------------------------------------
		// Start the colour table code
		destroycolourTables();
		GameLog::cleanup();
		//---------------------------------------------------------
		// End the Timers
		delete timerManager;
		timerManager = nullptr;
		//---------------------------------------------------------
		// TEST of PORT
		// Create VFX PANE and WINDOW to test draw of old terrain!
		if (globalPane)
		{
			delete globalPane;
			globalPane = nullptr;
		}
		if (globalWindow)
		{
			delete globalWindow;
			globalWindow = nullptr;
		}
		//-------------------------------------------------------------
		// Shut down the soundSytem for a change!
		if (soundSystem)
		{
			soundSystem->destroy();
			delete soundSystem;
			soundSystem = nullptr;
		}
		//------------------------------------------------
		// shutdown the MC Texture Manager.
		if (mcTextureManager)
		{
			mcTextureManager->destroy();
			delete mcTextureManager;
			mcTextureManager = nullptr;
		}
		delete globalFloatHelp;
		globalFloatHelp = nullptr;
		//--------------------------------------------------------------
		// End the SystemHeap and globalHeapList
		if (systemHeap)
		{
			systemHeap->destroy();
			delete systemHeap;
			systemHeap = nullptr;
		}
		if (guiHeap)
		{
			guiHeap->destroy();
			delete guiHeap;
			guiHeap = nullptr;
		}
		if (globalHeapList)
		{
			globalHeapList->destroy();
			delete globalHeapList;
			globalHeapList = nullptr;
		}
		//----------------------------------------------------
		// Shutdown the MLR and associated stuff libraries
		//----------------------------------------------------
		gos_PushCurrentHeap(gosFX::Heap);
		delete effectStream;
		delete gosFX::LightManager::Instance;
		gos_PopCurrentHeap();
		//
		//-------------------
		// Turn off libraries
		//-------------------
		//
		gosFX::TerminateClasses();
		MidLevelRenderer::TerminateClasses();
		Stuff::TerminateClasses();
		delete userInput;
		userInput = nullptr;
		// Redundant.  Something else is shutting this down.
		// GOS sure does think its bad to delete something multiple times
		// though.  Even though it simply never is!  gos_DeleteFont(gosFontHandle);
		gos_CloseResourceDLL(gosResourceHandle);
		//
		//--------------------------
		// Turn off the fast Files
		//--------------------------
		//
		FastFileFini();
		//
		// Just down any global allocations
		//
		if (g_textureCache_FilenameOfLastLoadedTexture)
		{
			delete g_textureCache_FilenameOfLastLoadedTexture;
			g_textureCache_FilenameOfLastLoadedTexture = nullptr;
		}
		if (ForceGroupIcon::jumpJetIcon)
		{
			delete ForceGroupIcon::jumpJetIcon;
			ForceGroupIcon::jumpJetIcon = nullptr;
		}
		if (MechIcon::s_MechTextures)
		{
			delete[] MechIcon::s_MechTextures;
			MechIcon::s_MechTextures = nullptr;
		}
		if (ForceGroupIcon::s_textureMemory)
		{
			delete[] ForceGroupIcon::s_textureMemory;
			ForceGroupIcon::s_textureMemory = nullptr;
		}
	}
	else
	{
		free(testVertex);
		testVertex = nullptr;
		free(indexArray);
		indexArray = nullptr;
	}
}

//-------------------------------
// Test TGL Parser.
bool extracted = false;

bool doTransformMath = true;

extern bool useFog;

extern int32_t terrainLineChanged;

float frameNum = 0.0f;

bool enoughTime = true;
int32_t enoughCount = 0;

//---------------------------------------------------------------------------
//
// No multi-thread now!
//
bool DoneSniffing = false;
void DoGameLogic()
{
	if (!SnifferMode)
	{
#ifdef LAB_ONLY // Used for debugging LOS
		currentLineElement = 0;
#endif
		if (MPlayer)
		{
			ProfileTime(MCTimeMultiplayerUpdate, MPlayer->update());
			if (MPlayer->waitingToStartMission)
			{
				if (MPlayer->startMission)
				{
					//					logistics->setLogisticsState(log_DONE); // can't
					// do this until screen animates out
					soundSystem->playBettySample(BETTY_NEW_CAMPAIGN);
					MPlayer->waitingToStartMission = false;
				}
			}
		}
		FloatHelp::resetAll();
		//-------------------------------------
		// Get me the current frameRate.
		// Convert to frameLength and any other timing stuff.
		// if (frameRate < 15.0)
		//	frameRate = 15.0;
		// FrameRate can be zero in GameOS for some unknown fu-cocked reason.
		if (frameRate < Stuff::SMALL)
			frameRate = 4.0f;
		frameLength = 1.0 / frameRate;
		if (frameLength > 0.25f)
			frameLength = 0.25f;
		// Calc out the average frame rate so we can decide if we should use the
		// movie Video or Just the Audio Track!
		currentFrameNum++;
		if (currentFrameNum == 30)
			currentFrameNum = 0;
		last30Frames[currentFrameNum] = frameRate;
		averageFrameRate = 0.0f;
		for (size_t fc = 0; fc < 30; fc++)
			averageFrameRate += last30Frames[fc];
		averageFrameRate /= 30.0f;
		if (userInput->getKeyDown(KEY_R) && userInput->ctrl() && !userInput->alt() && !userInput->shift())
		{
			doTransformMath ^= true;
			terrainLineChanged = turn;
		}
		if (doTransformMath)
		{
			//-------------------------------------
			// Poll devices for this frame.
			userInput->update();
			//----------------------------------------
			// Update the Sound System for this frame
			soundSystem->update();
			//----------------------------------------
			// Update all of the timers
			timerManager->update();
			//-----------------------------------------------------
			// Update Mission and Logistics here.
			if (logistics)
			{
				int32_t result = logistics->update();
				if (result == log_DONE)
				{
					logistics->stop();
				}
			}
			if ((true == bInvokeOptionsScreenFlag) || (userInput->getKeyDown(KEY_O) && userInput->ctrl() && !userInput->alt() && !userInput->shift()))
			{
				bInvokeOptionsScreenFlag = false;
				if ((!optionsScreenWrapper) || (optionsScreenWrapper->isDone()))
				{
					if (!optionsScreenWrapper)
					{
						optionsScreenWrapper = new OptionsScreenWrapper;
						optionsScreenWrapper->init();
					}
					if (mission && mission->missionInterface && !mission->missionInterface->isPaused())
						mission->missionInterface->togglePauseWithoutMenu();
					optionsScreenWrapper->begin();
				}
			}
			else if (mission && (!optionsScreenWrapper || optionsScreenWrapper->isDone()))
			{
				int32_t result = mission->update();
				if (result == 9999)
				{
					mission->destroy();
					// delete mission;
					// mission = nullptr;
					for (size_t i = 0; i < 3; i++)
					{
						DebugGameObject[i] = nullptr;
						DebugWindow[i + 1]->clear();
					}
					if (MPlayer && MPlayer->isServer())
						MPlayer->sendEndMission(1);
					quitGame = true;
				}
				else if (result != mis_PLAYING)
				{
					// if (MPlayer && MPlayer->isServer())
					//	MPlayer->sendEndMission(1);
					if (result < mis_PLAYER_DRAW)
						soundSystem->playBettySample(BETTY_MISSION_LOST);
					else if (result > mis_PLAYER_DRAW)
						soundSystem->playBettySample(BETTY_MISSION_WON);
					// Gotta get rid of the mission textures before Heidi starts
					// her stuff!
					// No RAM otherwise in mc2_18.
					mcTextureManager->flush(true);
					if (MPlayer && aborted)
					{
						MPlayer->closeSession();
						MPlayer->setMode(MULTIPLAYER_MODE_NONE);
						logistics->start(log_SPLASH);
					}
					else
						logistics->start(log_RESULTS); // Should be log_RESULTS!!!!!!
					aborted = false;
					if (!LogisticsData::instance->campaignOver() || MPlayer || LogisticsData::instance->isSingleMission())
					{
						mission->destroy();
						// delete mission;
						// mission = nullptr;
					}
					for (size_t i = 0; i < 3; i++)
					{
						DebugGameObject[i] = nullptr;
						DebugWindow[i + 1]->clear();
					}
				}
			}
			if (optionsScreenWrapper && !optionsScreenWrapper->isDone())
			{
				OptionsScreenWrapper::status_type result = optionsScreenWrapper->update();
				if (result == OptionsScreenWrapper::opt_DONE)
				{
					optionsScreenWrapper->end();
					if (mission && mission->missionInterface && mission->missionInterface->isPaused())
						mission->missionInterface->togglePauseWithoutMenu();
				}
			}
		}
		DEBUGWINS_update();
		//---------------------------------------------------
		// Update heap instrumentation.
#ifdef LAB_ONLY
		if (turn > 3)
			globalHeapList->update();
#endif
		//-----------------------------------------------------
		// Check the TimeBomb to see if we should go away
		/*
		if (!checkedBomb)
		{
			SYSTEMTIME checkTime;
			SYSTEMTIME bombTime;
			uint32_t dataSize = sizeof(SYSTEMTIME);
			GetSystemTime(&checkTime);

			if (!gotBombData)
			{
				gos_LoadDataFromRegistry("GraphicsDataInit2", &bombTime,
		&dataSize); gotBombData = true;
			}

			if (dataSize == 0)
			{
				STOP(("NO Graphics Data Initializer - Please Login with
		administrator privileges"));	//In Case they try to whack the registry
		entry while running!
			}

			if ((checkTime.wYear >= bombTime.wYear) &&
				(checkTime.wMonth >= bombTime.wMonth) &&
				(checkTime.wDay >= bombTime.wDay))
			{
				//If bomb goes off, set date to IMPOSSIBLE to achieve time!!
				// If they whack the entry after this, it doesn't matter!  The
		normal bomb will go off and then set this anyway.
				// If they whack the entry, then reset their system clock, then
		run the game, it will work.  Not much more I can do!
				// I could delete files but it won't matter, they have the CD!
				//
				// MUST change version numbers on fastfiles after Code Complete
		and after EACH Beta we release!! bombTime.wYear = 1950; bombTime.wMonth
		= 0; bombTime.wDay = 0;

				gos_SaveDataToRegistry("GraphicsDataInit2",  &bombTime,
		sizeof(SYSTEMTIME));

				STOP(("Time Bomb has exploded!"));

				quitGame = true;
			}

			checkedBomb = true;
		}
		*/
		//---------------------------------------------------------------
		// Somewhere in all of the updates, we have asked to be excused!
		if (quitGame)
		{
			// EnterWindowMode();				//Game crashes if _TerminateApp called from
			// fullScreen
			gos_TerminateApplication();
		}
		if (loadInMissionSave)
		{
			mission->load("data\\savegame\\testgame.ims");
			loadInMissionSave = false;
		}
	}
	else
	{
		if (!DoneSniffing)
		{
			for (size_t i = 0; i < 3000; i++)
			{
				testVertex[i].x = RandomNumber(1000) - 100;
				testVertex[i].y = RandomNumber(800) - 100;
				testVertex[i].z = ((float)RandomNumber(150) / 100.0f) - 0.25f;
				testVertex[i].rhw = 0.5f;
				testVertex[i].u = (float)RandomNumber(100) / 100.0f;
				testVertex[i].v = (float)RandomNumber(100) / 100.0f;
				testVertex[i].argb = 0x3fffffff;
				testVertex[i].frgb = 0x3f1f2f3f;
				indexArray[i] = i;
			}
			curIteration++;
			if (curIteration > numIterations)
			{
				if (gos_GetMachineInformation(gos_Info_GetDeviceLocalMemory, curDevice) >= 6291456)
					trisPerSecond[curDevice] = 5000.0f * numIterations / (totalTime / 1000);
				else
					trisPerSecond[curDevice] = 0.0f;
				curDevice++;
				if ((curDevice >= NumDevices) || (curDevice > MAX_HARDWARE_CARDS))
				{
					FILE* sniffData = fopen("sniff.dat", "wt");
					if (sniffData)
					{
						wchar_t sniff[2048];
						sprintf(sniff, "%f,%f,%f,%f,%f,%f\n", trisPerSecond[0], ProcessorSpeed,
							trisPerSecond[1], trisPerSecond[2], trisPerSecond[3], trisPerSecond[4]);
						fputs(sniff, sniffData);
						fclose(sniffData);
					}
					if ((trisPerSecond[0] > 1000000.0f) && (ProcessorSpeed > 595.0f))
					{
						CopyFile("hiPrefs.cfg", "orgprefs.cfg", false);
						CopyFile("hiPrefs.cfg", "options.cfg", false);
					}
					else if ((trisPerSecond[0] > 1000000.0f) && (ProcessorSpeed <= 595.0f))
					{
						CopyFile("h2Prefs.cfg", "orgprefs.cfg", false);
						CopyFile("h2Prefs.cfg", "options.cfg", false);
					}
					else if ((trisPerSecond[0] > 10000.0f) && (ProcessorSpeed > 595.0f))
					{
						CopyFile("midPrefs.cfg", "orgprefs.cfg", false);
						CopyFile("midPrefs.cfg", "options.cfg", false);
					}
					else if ((trisPerSecond[1] > 1000000.0f) && (ProcessorSpeed > 595.0f))
					{
						CopyFile("hiPrefs1.cfg", "orgprefs.cfg", false);
						CopyFile("hiPrefs1.cfg", "options.cfg", false);
					}
					else if ((trisPerSecond[1] > 1000000.0f) && (ProcessorSpeed <= 595.0f))
					{
						CopyFile("h2Prefs1.cfg", "orgprefs.cfg", false);
						CopyFile("h2Prefs1.cfg", "options.cfg", false);
					}
					else if ((trisPerSecond[1] > 10000.0f) && (ProcessorSpeed > 595.0f))
					{
						CopyFile("midPrefs1.cfg", "orgprefs.cfg", false);
						CopyFile("midPrefs1.cfg", "options.cfg", false);
					}
					else if (trisPerSecond[1] > 0.0f)
					{
						CopyFile("minPrefs1.cfg", "orgprefs.cfg", false);
						CopyFile("minPrefs1.cfg", "options.cfg", false);
					}
					else
					{
						CopyFile("minPrefs.cfg", "orgprefs.cfg", false);
						CopyFile("minPrefs.cfg", "options.cfg", false);
					}
					DoneSniffing = true;
				}
				else
				{
					// Don't sniff a below minspec card.  Crash O Roni!
					if (gos_GetMachineInformation(gos_Info_GetDeviceLocalMemory, curDevice) >= 6291456)
						gos_SetScreenMode(800, 600, 16, curDevice, 0, 0, 0, true, 0, 0, 0, 0);
					else
						curDevice++;
					curIteration = 0;
				}
			}
		}
		else
		{
			gos_SetScreenMode(800, 600, 16, 0, 0, 0, 0, false, 0, true, 0, 0);
			DoneSniffing = true;
			if (Environment.fullScreen == 0)
			{
				wchar_t msgBuffer[4096];
				wchar_t msgTitle[1024];
				cLoadString(IDS_SNIFFER_DONE_MSG, msgBuffer, 4095);
				cLoadString(IDS_SNIFFER_INIT_TITLE, msgTitle, 1023);
				MessageBox(nullptr, msgBuffer, msgTitle, MB_OK);
				gos_TerminateApplication();
			}
		}
	}
}

//---------------------------------------------------------------------------
int32_t
textToLong(std::wstring_view num)
{
	int32_t result = 0;
	//------------------------------------
	// Check if Hex Number
	std::wstring_view hexOffset = strstr(num, "0x");
	if (hexOffset == nullptr)
	{
		result = atol(num);
	}
	else
	{
		hexOffset += 2;
		int32_t numDigits = strlen(hexOffset) - 1;
		for (size_t i = 0; i <= numDigits; i++)
		{
			if (!isalnum(hexOffset[i]) || (isalpha(hexOffset[i]) && toupper(hexOffset[i]) > 'F'))
			{
				hexOffset[i] = 0; // we've reach a "wrong" character. Either
					// start of a comment or something illegal.
					// Either way, stop evaluation here.
				break;
			}
		}
		numDigits = strlen(hexOffset) - 1;
		int32_t power = 0;
		for (size_t count = numDigits; count >= 0; count--, power++)
		{
			uint8_t currentDigit = toupper(hexOffset[count]);
			if (currentDigit >= 'A' && currentDigit <= 'F')
			{
				result += (currentDigit - 'A' + 10) << (4 * power);
			}
			else if (currentDigit >= '0' && currentDigit <= '9')
			{
				result += (currentDigit - '0') << (4 * power);
			}
			else
			{
				//---------------------------------------------------------
				// There is a digit in here I don't understand.  Return 0.
				result = 0;
				break;
			}
		}
	}
	return (result);
}

//----------------------------------------------------------------------------
// Same command line Parser as MechCommander
void ParseCommandLine(std::wstring_view command_line)
{
	int32_t i;
	int32_t n_args = 0;
	int32_t index = 0;
	std::wstring_view argv[30];
	wchar_t tempCommandLine[4096];
	memset(tempCommandLine, 0, 4096);
	strncpy(tempCommandLine, command_line, 4095);
	while (tempCommandLine[index] != '\0') // until we null out
	{
		argv[n_args] = tempCommandLine + index;
		n_args++;
		while (tempCommandLine[index] != ' ' && tempCommandLine[index] != '\0')
		{
			index++;
		}
		while (tempCommandLine[index] == ' ')
		{
			tempCommandLine[index] = '\0';
			index++;
		}
	}
	i = 0;
	while (i < n_args)
	{
		if (strcmpi(argv[i], "-mission") == 0)
		{
			i++;
			if (i < n_args)
			{
				justStartMission = true;
				inViewMode = false;
				if (argv[i][0] == '"')
				{
					// They typed in a quote, keep reading argvs
					// until you find the close quote
					strcpy(missionName, &(argv[i][1]));
					bool scanName = true;
					while (scanName && (i < n_args))
					{
						i++;
						if (i < n_args)
						{
							strcat(missionName, " ");
							strcat(missionName, argv[i]);
							if (strstr(argv[i], "\"") != nullptr)
							{
								scanName = false;
								missionName[strlen(missionName) - 1] = 0;
							}
						}
						else
						{
							// They put a quote on the line with no space.
							//
							scanName = false;
							missionName[strlen(missionName) - 1] = 0;
						}
					}
				}
				else
					strcpy(missionName, argv[i]);
			}
		}
		else if (strcmpi(argv[i], "-viewer") == 0)
		{
			i++;
			if (i < n_args)
			{
				inViewMode = true;
				justStartMission = true;
				viewObject = textToLong(argv[i]);
				strcpy(missionName, "mis0101");
			}
		}
		else if (strcmpi(argv[i], "-nodialog") == 0)
		{
			gNoDialogs = true;
		}
		else if (strcmpi(argv[i], "-sniffer") == 0)
		{
			SnifferMode = true;
		}
		else if (strcmpi(argv[i], "-braindead") == 0)
		{
			i++;
			if (i < n_args)
			{
				int32_t teamID = textToLong(argv[i]);
				MechWarrior::brainsEnabled[teamID] = false;
			}
		}
		else if (strcmpi(argv[i], "-turrets_off") == 0)
		{
			i++;
			if (i < n_args)
			{
				int32_t teamID = textToLong(argv[i]);
				Turret::turretsEnabled[teamID] = false;
			}
		}
		else if (strcmpi(argv[i], "-debugwins") == 0)
		{
			i++;
			if (i < n_args)
			{
				int32_t winState = textToLong(argv[i]);
				if (winState == 1)
					DebugWindowOpen[0] = true;
				else if (winState == 2)
				{
					DebugWindowOpen[1] = true;
					DebugWindowOpen[2] = true;
					DebugWindowOpen[3] = true;
				}
				else if (winState == 3)
				{
					DebugWindowOpen[0] = true;
					DebugWindowOpen[1] = true;
					DebugWindowOpen[2] = true;
					DebugWindowOpen[3] = true;
				}
			}
		}
		else if (strcmpi(argv[i], "-objectwins") == 0)
		{
			i++;
			if (i < n_args)
			{
				int32_t partNumber = textToLong(argv[i]);
				if (NumGameObjectsToDisplay < 3)
					GameObjectWindowList[NumGameObjectsToDisplay++] = partNumber;
			}
		}
		else if (strcmpi(argv[i], "-debugcells") == 0)
		{
			i++;
			if (i < n_args)
			{
				int32_t setting = textToLong(argv[i]);
				if ((setting > 0) && (setting < 5))
					DrawDebugCells = setting;
			}
		}
		else if (strcmpi(argv[i], "-nopain") == 0)
		{
			i++;
			if (i < n_args)
			{
				int32_t teamID = textToLong(argv[i]);
				Team::noPain[teamID] = true;
			}
		}
		else if (strcmpi(argv[i], "-disable") == 0)
		{
			i++;
			if (i < n_args)
			{
				int32_t partID = textToLong(argv[i]);
				DisableAtStart[NumDisableAtStart++] = partID;
			}
		}
		else if (strcmpi(argv[i], "-log") == 0)
		{
			i++;
			initGameLogs = true;
			if (i < n_args)
			{
				if (strcmpi(argv[i], "net") == 0)
					initNetLog = true;
				if (strcmpi(argv[i], "weaponfire") == 0)
					initCombatLog = true;
				if (strcmpi(argv[i], "bugs") == 0)
					initBugLog = true;
				if (strcmpi(argv[i], "lrmove") == 0)
					initLRMoveLog = true;
			}
		}
		else if (strcmpi(argv[i], "-show") == 0)
		{
			i++;
			if (i < n_args)
			{
				if (strcmpi(argv[i], "movers") == 0)
					ShowMovers = true;
			}
		}
		else if (strcmpi(argv[i], "-goalplan") == 0)
		{
			i++;
			if (i < n_args)
			{
				if (strcmpi(argv[i], "enemies") == 0)
					EnemiesGoalPlan = true;
			}
		}
		else if (strcmpi(argv[i], "-killambient") == 0)
		{
			KillAmbientLight = true;
		}
		else if (strcmpi(argv[i], "-movegoals") == 0)
		{
			i++;
			if (i < n_args)
				MaxMoveGoalChecks = textToLong(argv[i]);
		}
		else if (strcmpi(argv[i], "-rps") == 0)
		{
			i++;
			if (i < n_args)
				MaxResourcePoints = textToLong(argv[i]);
		}
		else if (strcmpi(argv[i], "-registerzone") == 0)
		{
			MultiPlayer::registerZone = true;
		}
		else if (strcmpi(argv[i], "-dropzones") == 0)
		{
			i++;
			if (i < n_args)
			{
				int32_t numPlayers = strlen(argv[i]);
				for (size_t j = 0; j < numPlayers; j++)
					MultiPlayer::presetDropZones[j] = (argv[i][j] - '0');
			}
		}
		i++;
	}
}

bool notFirstTime = false;

//---------------------------------------------------------------------------
//
// Setup the GameOS structure -- This tells GameOS what I am using
//
void GetGameOSEnvironment(std::wstring_view commandline)
{
	ParseCommandLine(commandline);
	Environment.applicationName = "MechCommander2\\1.0"; // MP012001";
	Environment.debugLog = "";
	Environment.spew = "";
	Environment.TimeStampSpew = 0;
	Environment.GetGameInformation = GetGameInformation;
	Environment.UpdateRenderers = UpdateRenderers;
	Environment.InitializeGameEngine = InitializeGameEngine;
	Environment.DoGameLogic = DoGameLogic;
	Environment.TerminateGameEngine = TerminateGameEngine;
	Environment.Renderer = 0;
	Environment.allowMultipleApps = false;
	Environment.dontClearRegistry = true;
	if (useSound)
	{
		Environment.soundDisable = FALSE;
		Environment.soundHiFi = TRUE;
		Environment.soundChannels = 20;
	}
	else
	{
		Environment.soundDisable = TRUE;
		Environment.soundHiFi = FALSE;
		Environment.soundChannels = 0;
	}
	//--------------------------
	// Networking information...
	// MechCommander GUID = {09608800-4815-11d2-92D2-0060973CFB2C}
	// or {0x9608800, 0x4815, 0x11d2, {0x92, 0xd2, 0x0, 0x60, 0x97, 0x3c, 0xfb,
	// 0x2c}}
	Environment.NetworkGame = FALSE;
	Environment.NetworkMaxPlayers = MAX_COMMANDERS;
	Environment.NetworkGUID[0] = 0x09;
	Environment.NetworkGUID[1] = 0x60;
	Environment.NetworkGUID[2] = 0x88;
	Environment.NetworkGUID[3] = 0x00;
	Environment.NetworkGUID[4] = 0x48;
	Environment.NetworkGUID[5] = 0x15;
	Environment.NetworkGUID[6] = 0x11;
	Environment.NetworkGUID[7] = 0xd2;
	Environment.NetworkGUID[8] = 0x92;
	Environment.NetworkGUID[9] = 0xd2;
	Environment.NetworkGUID[10] = 0x00;
	Environment.NetworkGUID[11] = 0x60;
	Environment.NetworkGUID[12] = 0x97;
	Environment.NetworkGUID[13] = 0x3c;
	Environment.NetworkGUID[14] = 0xfb;
	Environment.NetworkGUID[15] = 0x2c;
	Environment.Key_FullScreen = 0;
	Environment.Key_SwitchMonitors = 0;
	Environment.Key_Exit = 0;
	Environment.screenwidth = 800;
	Environment.screenheight = 600;
	Environment.bitDepth = 16;
	Environment.fullScreen = 0;
	HKEY hKey;
	int32_t result;
	wchar_t pData[1024];
	uint32_t szData = 1023;
	result = RegOpenKey(HKEY_CURRENT_USER, GAME_REG_KEY, &hKey);
	if (ERROR_SUCCESS == result)
	{
		result = RegQueryValueEx(hKey, "FIRSTRUN", nullptr, nullptr, (PBYTE)pData, &szData);
		if ((result == ERROR_SUCCESS) && pData[0])
		{
			if (fileExists("options.cfg") && !SnifferMode)
#ifndef LAB_ONLY
				Environment.fullScreen = 1;
#endif
			EULAShown = true;
		}
		RegCloseKey(hKey);
	}
	Environment.version = versionStamp;
	Environment.FullScreenDevice = 0;
	Environment.AntiAlias = 0;
	Environment.disableZBuffer = 0;
	//
	// Texture infomation
	//
	Environment.Texture_S_256 = 6;
	Environment.Texture_S_128 = 1;
	Environment.Texture_S_64 = 0;
	Environment.Texture_S_32 = 1;
	Environment.Texture_S_16 = 5;
	Environment.Texture_K_256 = 2;
	Environment.Texture_K_128 = 5;
	Environment.Texture_K_64 = 5;
	Environment.Texture_K_32 = 5;
	Environment.Texture_K_16 = 5;
	Environment.Texture_A_256 = 0;
	Environment.Texture_A_128 = 1;
	Environment.Texture_A_64 = 5;
	Environment.Texture_A_32 = 1;
	Environment.Texture_A_16 = 0;
	Environment.RaidDataSource = "MechCommander 2:Raid4";
	Environment.RaidFilePath = "\\\\aas1\\MC2\\Test\\GOSRaid";
	Environment.RaidCustomFields = "Area=GOSRaid";
	Environment.DisableLowEndCard = 1;
	Environment.Suppress3DFullScreenWarning = 1;
	Environment.RenderToVram = 1;
	notFirstTime = true;
}

//***************************************************************************
