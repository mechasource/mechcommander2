//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// Viewer.cpp : Defines the class behaviors for the application.
//

#include "stdinc.h"

//#ifndef VIEWER
//#define VIEWER
//#endif
//

//#ifndef VERSION_H
//#include "version.h"
//#endif

#include "gameos.hpp"
#include "toolos.hpp"
#include "resource.h"
#include "mclib.h"
#include "gamesound.h"
//#include <windows.h>
#include "mission.h"
#include "mechlopedia.h"
#include "logisticsdata.h"
#include "prefs.h"

CPrefs prefs;

SoundSystem* sndSystem = nullptr;

MultiPlayer* MPlayer = nullptr;

// CPrefs prefs;

extern float frameRate;

// globals used for memory
UserHeapPtr systemHeap = nullptr;
UserHeapPtr guiHeap = nullptr;

int32_t GameVisibleVertices = 30;
bool useLeftRightMouseProfile = true;

float MaxMinUV = 8.0f;

uint32_t systemHeapSize = 65535000;
uint32_t guiHeapSize = 1023999;
uint32_t tglHeapSize = 32767000;
bool GeneralAlarm = 0;

extern int32_t DigitalMasterVolume;
extern int32_t MusicVolume;
extern int32_t sfxVolume;
extern int32_t RadioVolume;

extern wchar_t FileMissingString[];
extern wchar_t CDMissingString[];
extern wchar_t MissingTitleString[];

extern wchar_t CDInstallPath[];

int32_t FilterState = gos_FilterNone;
int32_t gammaLevel = 0;
int32_t renderer = 0;
int32_t GameDifficulty = 0;
int32_t resolution = 0;
bool useUnlimitedAmmo = true;

Camera* eye = nullptr;
uint32_t BaseVertexcolour = 0;

enum
{
	CPU_UNKNOWN,
	CPU_PENTIUM,
	CPU_MMX,
	CPU_KATMAI
} Processor = CPU_PENTIUM; // Needs to be set when GameOS supports ProcessorID
	// -- MECHCMDR2

bool reloadBounds = false;
int32_t ObjectTextureSize = 128;
// wchar_t missionName[1024];
float gosFontScale = 1.0;

float doubleClickThreshold;
float dragThreshold;

// uint32_t gosResourceHandle = 0;
HGOSFONT3D gosFontHandle = nullptr;

bool quitGame = false;

// these globals are necessary for fast files for some reason
FastFile** fastFiles = nullptr;
size_t numFastFiles = 0;
size_t maxFastFiles = 0;

std::wstring_view ExceptionGameMsg = "";

bool justResaveAllMaps = 0;
bool useLOSAngle = 0;

std::iostream effectStream = nullptr;
extern MidLevelRenderer::MLRClipper* theClipper;

Mechlopedia* pMechlopedia;
LogisticsData* pLogData;

std::wstring_view SpecialtySkillsTable[NUM_SPECIALTY_SKILLS] = {
	"LightMechSpecialist", "LaserSpecialist", "LightACSpecialist", "MediumACSpecialist",
	"SRMSpecialist", "SmallArmsSpecialist", "SensorProfileSpecialist",
	"ToughnessSpecialist", // Thoughness Specialty

	"MediumMechSpecialist", "PulseLaserSpecialist", "ERLaserSpecialist", "LRMSpecialist",
	"Scout", // Scouting Specialty
	"LongJump", // Jump Jet Specialty

	"HevayMechSpecialist", // Heavy mech Specialty
	"PPCSpecialist", "HeavyACSpecialist", "ShortRangeSpecialist", "MediumRangeSpecialist",
	"LongRangeSpecialist",

	"AssaultMechSpecialist", "GaussSpecialist",
	"SharpShooter", // Sharpshooter specialty
};

// called by gos
//---------------------------------------------------------------------------
std::wstring_view __stdcall GetGameInformation()
{
	return (ExceptionGameMsg);
}

// called by GOS when you need to draw
//---------------------------------------------------------------------------
void __stdcall UpdateRenderers()
{
	uint32_t bcolour = 0x0;
	gos_SetupViewport(1, 1.0, 1, bcolour, 0.0, 0.0, 1.0, 1.0); // ALWAYS FULL SCREEN for now
	gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
	gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
	gos_SetRenderState(gos_State_AlphaTest, TRUE);
	gos_SetRenderState(gos_State_Clipping, TRUE);
	gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
	gos_SetRenderState(gos_State_Dither, 1);
	float viewMulX, viewMulY, viewAddX, viewAddY;
	gos_GetViewport(&viewMulX, &viewMulY, &viewAddX, &viewAddY);
	//------------------------------------------------------------
	gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	pMechlopedia->render();
	gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	userInput->setViewport(viewMulX, viewMulY, viewAddX, viewAddY);
	userInput->render();
}

//------------------------------------------------------------
void __stdcall DoGameLogic()
{
	//---------------------------------------------------------------
	// Somewhere in all of the updates, we have asked to be excused!
	if (quitGame)
	{
		// EnterWindowMode(); //Game crashes if _TerminateApp called from
		// fullScreen
		gos_TerminateApplication();
	}
	if (frameRate < Stuff::SMALL)
		frameRate = 4.0f;
	frameLength = 1.0 / frameRate;
	if (frameLength > 0.25f)
		frameLength = 0.25f;
	userInput->update();
	soundSystem->update();
	pMechlopedia->update();
	if (LogisticsScreen::RUNNING != pMechlopedia->getStatus())
		quitGame = true;
}

//---------------------------------------------------------------------------
void __stdcall InitializeGameEngine()
{
	gosResourceHandle = gos_OpenResourceDLL("mc2res.dll");
	wchar_t temp[256];
	cLoadString(IDS_FLOAT_HELP_FONT, temp, 255);
	std::wstring_view pStr = strstr(temp, ",");
	if (pStr)
	{
		gosFontScale = atoi(pStr + 2);
		*pStr = 0;
	}
	wchar_t path[256];
	strcpy(path, "assets\\graphics\\");
	strcat(path, temp);
	gosFontHandle = gos_LoadFont(path);
	//-------------------------------------------------------------
	// Find the CDPath in the registry and save it off so I can
	// look in CD Install Path for files.
	// Changed for the shared source release, just set to current directory
	// uint32_t maxPathLength = 1023;
	// gos_LoadDataFromRegistry("CDPath", CDInstallPath, &maxPathLength);
	// if (!maxPathLength)
	// strcpy(CDInstallPath,"..\\");
	strcpy(CDInstallPath, ".\\");
	cLoadString(IDS_MC2_FILEMISSING, FileMissingString, 511);
	cLoadString(IDS_MC2_CDMISSING, CDMissingString, 1023);
	cLoadString(IDS_MC2_MISSING_TITLE, MissingTitleString, 255);
	//--------------------------------------------------------------
	// Start the SystemHeap and globalHeapList
	globalHeapList = new HeapList;
	gosASSERT(globalHeapList != nullptr);
	globalHeapList->init();
	globalHeapList->update(); // Run Instrumentation into GOS Debugger Screen
	systemHeap = new UserHeap;
	gosASSERT(systemHeap != nullptr);
	systemHeap->init(systemHeapSize, "SYSTEM");
	float doubleClickThreshold = 0.2f;
	int32_t dragThreshold = .016667;
	//--------------------------------------------------------------
	// Read in System.CFG
	FitIniFile systemFile;
#ifdef _DEBUG
	int32_t systemOpenResult =
#endif
		systemFile.open("system.cfg");
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
			systemFile.seekBlock("systemHeap");
		gosASSERT(systemBlockResult == NO_ERROR);
		{
			int32_t result = systemFile.readIdULong("systemHeapSize", systemHeapSize);
			result;
			gosASSERT(result == NO_ERROR);
		}
#ifdef _DEBUG
		int32_t systemPathResult =
#endif
			systemFile.seekBlock("systemPaths");
		gosASSERT(systemPathResult == NO_ERROR);
		{
			int32_t result = systemFile.readIdString("terrainPath", terrainPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("artPath", artPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("fontPath", fontPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("savePath", savePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("spritePath", spritePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("shapesPath", shapesPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("soundPath", soundPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("objectPath", objectPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("cameraPath", cameraPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("tilePath", tilePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("missionPath", missionPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("warriorPath", warriorPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("profilePath", profilePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("interfacepath", interfacePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("moviepath", moviePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("CDsoundPath", CDsoundPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("CDmoviepath", CDmoviePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("tglPath", tglPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile.readIdString("texturePath", texturePath, 79);
			gosASSERT(result == NO_ERROR);
		}
#ifdef _DEBUG
		int32_t fastFileResult =
#endif
			systemFile.seekBlock("FastFiles");
		gosASSERT(fastFileResult == NO_ERROR);
		{
			int32_t result = systemFile.readIdLong("NumFastFiles", maxFastFiles);
			if (result != NO_ERROR)
				maxFastFiles = 0;
			if (maxFastFiles)
			{
				fastFiles = (FastFile**)malloc(maxFastFiles * sizeof(FastFile*));
				memset(fastFiles, 0, maxFastFiles * sizeof(FastFile*));
				int32_t fileNum = 0;
				wchar_t fastFileId[10];
				wchar_t fileName[100];
				sprintf(fastFileId, "File%d", fileNum);
				while (systemFile.readIdString(fastFileId, fileName, 99) == NO_ERROR)
				{
					bool result = FastFileInit(fileName);
					if (!result)
						STOP(("Unable to startup fastfiles. Probably an old "
							  "one in the directory!!"));
					fileNum++;
					sprintf(fastFileId, "File%d", fileNum);
				}
			}
		}
	}
	systemFile.close();
	//--------------------------------------------------------------
	// Read in Prefs.cfg
	bool fullScreen = false;
	FitIniFilePtr prefs = new FitIniFile;
#ifdef _DEBUG
	int32_t prefsOpenResult =
#endif
		prefs->open("prefs.cfg");
	gosASSERT(prefsOpenResult == NO_ERROR);
	{
#ifdef _DEBUG
		int32_t prefsBlockResult =
#endif
			prefs->seekBlock("MechCommander2");
		gosASSERT(prefsBlockResult == NO_ERROR);
		{
			int32_t filterSetting;
			int32_t result = prefs->readIdLong("FilterState", filterSetting);
			if (result == NO_ERROR)
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
			result = prefs->readIdLong("TerrainTextureRes", TERRAIN_TXM_SIZE);
			if (result != NO_ERROR)
				TERRAIN_TXM_SIZE = 64;
			result = prefs->readIdLong("ObjectTextureRes", ObjectTextureSize);
			if (result != NO_ERROR)
				ObjectTextureSize = 128;
			result = prefs->readIdLong("Brightness", gammaLevel);
			if (result != NO_ERROR)
				gammaLevel = 0;
			// store volume settings in global variable since soundsystem
			// does not exist yet. These will be set in SoundSystem::init()
			result = prefs->readIdLong("DigitalMasterVolume", DigitalMasterVolume);
			if (result != NO_ERROR)
				DigitalMasterVolume = 255;
			result = prefs->readIdLong("MusicVolume", MusicVolume);
			if (result != NO_ERROR)
				MusicVolume = 64;
			result = prefs->readIdLong("RadioVolume", RadioVolume);
			if (result != NO_ERROR)
				RadioVolume = 64;
			result = prefs->readIdLong("SFXVolume", sfxVolume);
			if (result != NO_ERROR)
				sfxVolume = 64;
			result = prefs->readIdFloat("DoubleClickThreshold", doubleClickThreshold);
			if (result != NO_ERROR)
				doubleClickThreshold = 0.2f;
			result = prefs->readIdLong("DragThreshold", dragThreshold);
			if (result != NO_ERROR)
				dragThreshold = .016667;
			result = prefs->readIdULong("BaseVertexcolour", BaseVertexcolour);
			if (result != NO_ERROR)
				BaseVertexcolour = 0x00000000;
			result = prefs->readIdBoolean("FullScreen", fullScreen);
			if (result != NO_ERROR)
				fullScreen = true;
			result = prefs->readIdLong("Rasterizer", renderer);
			if (result != NO_ERROR)
				renderer = 0;
			if ((renderer < 0) || (renderer > 3))
				renderer = 0;
		}
	}
	prefs->close();
	delete prefs;
	prefs = nullptr;
	//-------------------------------
	// Used to output debug stuff!
	// Mondo COOL!
	// simply do this in the code and stuff goes to the file called mc2.output
	// DEBUG_STREAM << thing_you_want_to_output
	//
	// IMPORTANT NOTE:
	Stuff::InitializeClasses();
	MidLevelRenderer::InitializeClasses(8192 * 4, 1024, 0, 0, true);
	gosFX::InitializeClasses();
	gos_PushCurrentHeap(MidLevelRenderer::Heap);
	MidLevelRenderer::TGAFilePool* pool = new MidLevelRenderer::TGAFilePool("data\\Effects\\");
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
	if (result != NO_ERROR)
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
	//------------------------------------------------
	// Fire up the MC Texture Manager.
	mcTextureManager = new MC_TextureManager;
	mcTextureManager->start();
	// Startup the vertex array pool
	mcTextureManager->startVertices(500000);
	//--------------------------------------------------
	// Setup Mouse Parameters from Prefs.CFG
	userInput = new UserInput;
	userInput->init();
	userInput->setMouseDoubleClickThreshold(doubleClickThreshold);
	userInput->setMouseDragThreshold(dragThreshold);
	userInput->initMouseCursors("cursors");
	userInput->setMouseCursor(mState_NORMAL);
	userInput->mouseOn();
	// now the sound system
	soundSystem = new GameSoundSystem;
	soundSystem->init();
	((SoundSystem*)soundSystem)->init("sound");
	sndSystem = soundSystem; // for things in the lib that use sound
	soundSystem->playDigitalMusic(LOGISTICS_LOOP);
	pLogData = new LogisticsData;
	pLogData->init();
	pMechlopedia = new Mechlopedia;
	pMechlopedia->init();
	pMechlopedia->begin();
}

void __stdcall TerminateGameEngine()
{
	if (pMechlopedia)
		delete pMechlopedia;
	if (userInput)
		delete userInput;
	if (soundSystem)
		delete soundSystem;
	if (pLogData)
		delete pLogData;
	//------------------------------------------------
	// shutdown the MC Texture Manager.
	if (mcTextureManager)
	{
		mcTextureManager->destroy();
		delete mcTextureManager;
		mcTextureManager = nullptr;
	}
	//--------------------------------------------------------------
	// End the SystemHeap and globalHeapList
	if (systemHeap)
	{
		systemHeap->destroy();
		delete systemHeap;
		systemHeap = nullptr;
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
	// Redundant. Something else is shutting this down.
	// GOS sure does think its bad to delete something multiple times though.
	// Even though it simply never is!
	// gos_DeleteFont(gosFontHandle);
	gos_CloseResourceDLL(gosResourceHandle);
	//
	//--------------------------
	// Turn off the fast Files
	//--------------------------
	//
	FastFileFini();
}

//---------------------------------------------------------------------
void __stdcall GetGameOSEnvironment(std::wstring_view commandline)
{
	Environment.applicationName = "MechCommander 2 Encyclopedia";
	Environment.debugLog = ""; //"DebugLog.txt";
	Environment.memoryTraceLevel = 5;
	Environment.spew = ""; //"GameOS_Texture GameOS_DirectDraw GameOS_Direct3D ";
	Environment.TimeStampSpew = 0;
	Environment.GetGameInformation = GetGameInformation;
	Environment.UpdateRenderers = UpdateRenderers;
	Environment.InitializeGameEngine = InitializeGameEngine;
	Environment.DoGameLogic = DoGameLogic;
	Environment.TerminateGameEngine = TerminateGameEngine;
	if (useSound)
	{
		Environment.soundDisable = false;
		Environment.soundHiFi = TRUE;
		Environment.soundChannels = 24;
	}
	else
	{
		Environment.soundDisable = TRUE;
		Environment.soundHiFi = false;
		Environment.soundChannels = 0;
	}
	Environment.version = versionStamp;
	Environment.AntiAlias = 0;
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
	Environment.bitDepth = 16;
	Environment.RaidDataSource = "MechCommander 2:Raid4";
	Environment.RaidFilePath = "\\\\aas1\\MC2\\Test\\GOSRaid";
	Environment.RaidCustomFields = "Area=GOSRaid";
	Environment.screenwidth = 800;
	Environment.screenheight = 600;
}
