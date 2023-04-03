//--------------------------------------------------------------------------
//
// And away we go.  MechCommander II GOSScript Test Shell
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

//-----------------------------------
// Include Files
// #include "mclib.h"

#ifndef LOGISTICS_H
#include "logistics.h"
#endif

#include "gamesound.h"
#ifndef VERSION_H
#include "version.h"
#endif

//#include "gameos.hpp"
#include "toolos.hpp"
#include "gosscript/gosscriptheaders.hpp"
#include "stuff/stuff.h"

//------------------------------------------------------------------------------------------------------------
// MechCmdr2 Global Instances of Things
UserHeapPtr systemHeap = nullptr;
UserHeapPtr guiHeap = nullptr;

FastFile** fastFiles = nullptr;
int32_t numFastFiles = 0;
int32_t maxFastFiles = 0;

int32_t GameDifficulty = 0;
int32_t gammaLevel = 0;
int32_t DigitalMasterVolume = 0;
int32_t MusicVolume = 0;
int32_t SFXVolume = 0;
int32_t RadioVolume = 0;
int32_t resolution = 0;
int32_t FilterState = gos_FilterNone;
bool quitGame = FALSE;
bool justStartMission = FALSE;
bool gamePaused = FALSE;
bool hasGuardBand = false;
bool useUnlimitedAmmo = true;

TimerManagerPtr timerManager = nullptr;

uint32_t elementHeapSize = 1024000;
uint32_t maxElements = 2048;
uint32_t maxGroups = 1024;

uint32_t systemHeapSize = 3071999;
uint32_t guiHeapSize = 1023999;
uint32_t logisticsHeapSize = 4095999;
uint32_t missionHeapSize = 1023999;
uint32_t spriteDataHeapSize = 2048000;
uint32_t spriteHeapSize = 8192000;
uint32_t polyHeapSize = 1024000;
uint32_t tglHeapSize = 32767000;

uint32_t gosResourceHandle = 0;
HGOSFONT3D gosFontHandle = 0;
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

extern bool useSound;
extern bool useMusic;

bool inViewMode = false;
uint32_t viewObject = 0x0;
wchar_t missionName[1024];

std::wstring_view ExceptionGameMsg = nullptr;

wchar_t buildNumber[80];

extern int32_t TERRAIN_TXM_SIZE;
int32_t ObjectTextureSize = 128;

extern uint32_t MultiPlayCommanderId;
extern bool useRealLOS;
bool reloadBounds = false;

// DEBUG

CameraPtr eye = nullptr;
bool useRealLOS = true;

//***************************************************************************

std::wstring_view
GetGameInformation()
{
	return (ExceptionGameMsg);
}

// int32_t cLoadString (HINSTANCE hinstance,  uint32_t uID, std::wstring_view lpBuffer,
// int32_t nBufferMax );

//---------------------------------------------------------------------------

void UpdateRenderers()
{
	hasGuardBand = gos_GetMachineInformation(gos_Info_HasGuardBandClipping) != 0;
	//---------------------------------------------------------------------------------
	// Doesn't work.  Always returns 0 for TNT in Win2K build 2183 with 3.55
	// detonator  Assume worst case is +/- 8.0 for now.  MaxMinUV =
	// gos_GetMachineInformation(gos_Info_GetMaximumUVSize);
	uint32_t bcolour = 0x0;
	if (eye)
		bcolour = eye->fogcolour;
	gos_SetupViewport(1, 1.0, 1, bcolour, 0.0, 0.0, 1.0, 1.0); // ALWAYS FULL SCREEN for now
	gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
	gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
	gos_SetRenderState(gos_State_AlphaTest, TRUE);
	gos_SetRenderState(gos_State_Clipping, TRUE);
	gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
	gos_SetRenderState(gos_State_Dither, 1);
	if (logistics)
	{
		float viewMulX, viewMulY, viewAddX, viewAddY;
		gos_GetViewport(&viewMulX, &viewMulY, &viewAddX, &viewAddY);
		// userInput->setViewport(viewMulX,viewMulY,viewAddX,viewAddY);
		logistics->render();
	}
	//------------------------------------------------------------
	gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	// userInput->render();
	//------------------------------------------------------------
	gos_SetRenderState(gos_State_Filter, gos_FilterNone);
}

//---------------------------------------------------------------------------
void InitializeGameEngine()
{
	//---------------------------------------------------------------------
	float doubleClickThreshold = 0.2f;
	int32_t dragThreshold = 10;
	Environment.Key_Exit = -1; // so escape doesn't kill your app
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
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdULong("guiHeapSize", guiHeapSize);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdULong("logisticsHeapSize", logisticsHeapSize);
			gosASSERT(result == NO_ERROR);
		}
#ifdef _DEBUG
		int32_t systemPathResult =
#endif
			systemFile->seekBlock("systemPaths");
		gosASSERT(systemPathResult == NO_ERROR);
		{
			int32_t result = systemFile->readIdString("terrainPath", terrainPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("artPath", artPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("fontPath", fontPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("savePath", savePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("spritePath", spritePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("shapesPath", shapesPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("soundPath", soundPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("objectPath", objectPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("cameraPath", cameraPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("tilePath", tilePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("missionPath", missionPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("warriorPath", warriorPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("profilePath", profilePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("interfacepath", interfacePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("moviepath", moviePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("CDsoundPath", CDsoundPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("CDmoviepath", CDmoviePath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("CDspritePath", CDspritePath, 79);
			gosASSERT(result == NO_ERROR);
		}
#ifdef _DEBUG
		int32_t fastFileResult =
#endif
			systemFile->seekBlock("FastFiles");
		gosASSERT(fastFileResult == NO_ERROR);
		{
			int32_t result = systemFile->readIdLong("NumFastFiles", maxFastFiles);
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
				while (systemFile->readIdString(fastFileId, fileName, 99) == NO_ERROR)
				{
					FastFileInit(fileName);
					fileNum++;
					sprintf(fastFileId, "File%d", fileNum);
					numFastFiles++;
				}
			}
		}
		int32_t result = systemFile->seekBlock("UseMusic");
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
	}
	systemFile->close();
	delete systemFile;
	systemFile = nullptr;
	//--------------------------------------------------------------
	// Read in Prefs.cfg
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
			HRESULT result = prefs->readIdLong("FilterState", filterSetting);

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
			result = prefs->readIdLong("TerrainTextureRes", TERRAIN_TXM_SIZE);
			if (result != NO_ERROR)
				TERRAIN_TXM_SIZE = 64;
			result = prefs->readIdLong("Resolution", resolution);
			if (result != NO_ERROR)
				resolution = 0;
			result = prefs->readIdLong("ObjectTextureRes", ObjectTextureSize);
			if (result != NO_ERROR)
				ObjectTextureSize = 128;
			result = prefs->readIdLong("Difficulty", GameDifficulty);
			if (result != NO_ERROR)
				GameDifficulty = 1;
			result = prefs->readIdLong("Brightness", gammaLevel);
			if (result != NO_ERROR)
				gammaLevel = 0;
			// store volume settings in global variable since soundsystem
			// does not exist yet.  These will be set in SoundSystem::init()
			result = prefs->readIdLong("DigitalMasterVolume", DigitalMasterVolume);
			if (result != NO_ERROR)
				DigitalMasterVolume = 255;
			result = prefs->readIdLong("MusicVolume", MusicVolume);
			if (result != NO_ERROR)
				MusicVolume = 64;
			result = prefs->readIdLong("RadioVolume", RadioVolume);
			if (result != NO_ERROR)
				RadioVolume = 64;
			result = prefs->readIdLong("SFXVolume", SFXVolume);
			if (result != NO_ERROR)
				SFXVolume = 64;
			result = prefs->readIdFloat("DoubleClickThreshold", doubleClickThreshold);
			if (result != NO_ERROR)
				doubleClickThreshold = 0.2f;
			result = prefs->readIdLong("DragThreshold", dragThreshold);
			if (result != NO_ERROR)
				dragThreshold = 10;
			result = prefs->readIdULong("BaseVertexcolour", BaseVertexcolour);
			if (result != NO_ERROR)
				BaseVertexcolour = 0x00000000;
			result = prefs->readIdBoolean("UnlimitedAmmo", useUnlimitedAmmo);
			if (result != NO_ERROR)
				useUnlimitedAmmo = true;
			result = prefs->readIdBoolean("RealLOS", useRealLOS);
			if (result != NO_ERROR)
				useRealLOS = true;
		}
	}
	prefs->close();
	delete prefs;
	prefs = nullptr;
	//---------------------------------------------------------------------
	switch (resolution)
	{
	case 0: // 640by480
		gos_SetScreenMode(640, 480);
		break;
	case 1: // 800by600
		gos_SetScreenMode(800, 600);
		break;
	case 2: // 1024by768
		gos_SetScreenMode(1024, 768);
		break;
	case 3: // 1280by1024
		gos_SetScreenMode(1280, 1024);
		break;
	case 4: // 1600by1200
		gos_SetScreenMode(1600, 1200);
		break;
	}
	//--------------------------------------------------
	// Setup Mouse Parameters from Prefs.CFG
	// userInput = new UserInput;
	// userInput->init();
	// userInput->setMouseDoubleClickThreshold(doubleClickThreshold);
	// userInput->setMouseDragThreshold(dragThreshold);
	//--------------------------------------------------
	gosResourceHandle = gos_OpenResourceDLL("mc2res.dll");
	gosFontHandle = gos_LoadFont("assets\\graphics\\arial8.tga");
	globalFloatHelp = new FloatHelp[MAX_FLOAT_HELPS];
	//
	//----------------------------------
	// Start associated stuff.
	//----------------------------------
	Stuff::InitializeClasses();
	//--------------------------------------------------------------
	// Start the GUI Heap.
	systemHeap = new UserHeap;
	gosASSERT(systemHeap != nullptr);
	systemHeap->init(systemHeapSize, "SYSTEM");
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
	// userInput->initMouseCursors("cursors");
	// userInput->mouseOff();
	// userInput->setMouseCursor(mState_NORMAL);
	//------------------------------------------------
	// Give the Sound System a Whirl!
	// soundSystem = new GameSoundSystem;
	// soundSystem->init();
	//((SoundSystem *)soundSystem)->init("sound");
	//---------------------------------------------------------
	// Start the Mission, Scenario and Logistics classes here
	logistics = new Logistics;
	logistics->start(log_SPLASH); // Always start with logistics in Splash Screen Mode
}

//---------------------------------------------------------------------------

void TerminateGameEngine()
{
	gosScript_ShutdownProcessor();
	//---------------------------------------------------------
	// End the Mission, Operation and Logistics classes here
	if (logistics)
	{
		logistics->destroy();
		delete logistics;
		logistics = nullptr;
	}
	//-------------------------------------------------------------
	// Shut down the soundSytem for a change!
	// if (soundSystem)
	//{
	//	soundSystem->destroy();
	//
	//	delete soundSystem;
	//	soundSystem = nullptr;
	//}
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
	//
	//-------------------
	// Turn off libraries
	//-------------------
	//
	Stuff::TerminateClasses();
	// Redundant.  Something else is shutting this down.
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

//---------------------------------------------------------------------------
//
// No multi-thread now!
//

void DoGameLogic()
{
	//-------------------------------------
	// Get me the current frameRate.
	// Convert to frameLength and any other timing stuff.
	// if (frameRate < 15.0)
	//	frameRate = 15.0;
	frameLength = 1.0 / frameRate;
	if (frameLength > 0.25f)
		frameLength = 0.25f;
	//-------------------------------------
	// Poll devices for this frame.
	// userInput->update();
	//----------------------------------------
	// Update the Sound System for this frame
	// soundSystem->update();
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
	//---------------------------------------------------
	// Update heap instrumentation.
	if (turn > 3)
		globalHeapList->update();
	//---------------------------------------------------------------
	// Somewhere in all of the updates, we have asked to be excused!
	if (quitGame)
	{
		// EnterWindowMode();				//Game crashes if _TerminateApp called from
		// fullScreen
		gos_TerminateApplication();
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
	Environment.applicationName = "MC2GOSScriptShell";
	Environment.directoryPath = "Logistics";
	Environment.debugLog = ""; //;		"DebugLog.txt";
	Environment.spew = "GameOS_ScriptsOUtput"; //"GameOS_Texture
		// GameOS_DirectDraw
		// GameOS_Direct3D ";
	Environment.TimeStampSpew = 0;
	Environment.GetGameInformation = GetGameInformation;
	Environment.UpdateRenderers = UpdateRenderers;
	Environment.InitializeGameEngine = InitializeGameEngine;
	Environment.DoGameLogic = DoGameLogic;
	Environment.TerminateGameEngine = TerminateGameEngine;
	if (useSound)
	{
		Environment.soundDisable = FALSE;
		Environment.soundHiFi = TRUE;
		Environment.soundChannels = 24;
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
	Environment.NetworkGame = false;
	Environment.NetworkMaxPlayers = 0;
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
	Environment.screenwidth = 640;
	Environment.screenheight = 480;
	Environment.bitDepth = 16;
	Environment.fullScreen = 0;
	Environment.version = versionStamp;
	Environment.FullScreenDevice = 0;
	Environment.Renderer = 0;
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
	notFirstTime = true;
}

//***************************************************************************
