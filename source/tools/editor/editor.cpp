//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
#include "resource.h"

//#include "editorinterface.h"
//#include <gameos.hpp>
//#include <toolos.hpp>
//#include <mlr/mlr.hpp>
//#include <stuff/stuff.hpp>
//#include "editordata.h"
//#include "version.h"

// globals used for memory
UserHeapPtr systemHeap = nullptr;
UserHeapPtr guiHeap = nullptr;

float MaxMinUV = 8.0f;

Stuff::MemoryStream* effectStream = nullptr;

uint32_t systemHeapSize = 8192000;
uint32_t guiHeapSize = 1023999;
uint32_t tglHeapSize = 65536000;

uint32_t BaseVertexColor = 0x00000000;		//This color is applied to all vertices in game as Brightness correction.

int32_t gammaLevel = 0;
bool hasGuardBand = false;
extern int32_t terrainLineChanged; // the terrain uses this
//extern float frameNum;	// tiny geometry needs this
extern float frameRate; // tiny geometry needs this

extern bool InEditor;
extern char FileMissingString[];
extern char CDMissingString[];
extern char MissingTitleString[];

extern char CDInstallPath[];

uint32_t gosResourceHandle = 0;
HGOSFONT3D gosFontHandle = 0;
float gosFontScale = 1.0;
FloatHelpPtr globalFloatHelp = nullptr;
uint32_t currentFloatHelp = 0;
extern float CliffTerrainAngle;

extern bool gNoDialogs;

PSTR ExceptionGameMsg = nullptr; // some debugging thing I think

bool quitGame = FALSE;

bool gamePaused = FALSE;

bool reloadBounds = false;
bool justResaveAllMaps = false;

extern bool forceShadowBurnIn;
// these globals are necessary for fast files for some reason
FastFile**	 fastFiles = nullptr;
size_t 		numFastFiles = 0;
size_t		maxFastFiles = 0;

#define MAX_SHAPES	0

//Heidi, turn this FALSE to turn Fog of War ON!
extern uint8_t godMode;			//Can I simply see everything, enemy and friendly?

void InitDW();

TimerManagerPtr timerManager = nullptr;

int32_t FilterState = gos_FilterNone;

extern int32_t TERRAIN_TXM_SIZE;
int32_t ObjectTextureSize = 128;

Editor* editor = nullptr;

char missionName[1024] = "\0";

enum { CPU_UNKNOWN, CPU_PENTIUM, CPU_MMX, CPU_KATMAI } Processor = CPU_PENTIUM;		//Needs to be set when GameOS supports ProcessorID -- MECHCMDR2

MidLevelRenderer::MLRClipper*   theClipper = nullptr;

// called by gos
//---------------------------------------------------------------------------
PSTR GetGameInformation()
{
	return(ExceptionGameMsg);
}

// called by GOS when you need to draw
//---------------------------------------------------------------------------
//define RUNNING_REMOTELY
#ifdef RUNNING_REMOTELY
#include <windows.h> /* for declaration of Sleep() */
#endif /*RUNNING_REMOTELY*/
void UpdateRenderers()
{
#ifdef RUNNING_REMOTELY
	Sleep(0.25/*seconds*/ * 1000.0); /* limit the frame rate when displaying on remote console */
#endif /*RUNNING_REMOTELY*/
	hasGuardBand = true;
	uint32_t bColor = 0x0;
	if(eye)
		bColor = eye->fogColor;
	gos_SetupViewport(1, 1.0, 1, bColor, 0.0, 0.0, 1.0, 1.0);		//ALWAYS FULL SCREEN for now
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
	editor->render();
	globalFloatHelp->renderAll();
	turn++;
	reloadBounds = false;
	//reset the TGL RAM pools.
	colorPool->reset();
	vertexPool->reset();
	facePool->reset();
	shadowPool->reset();
	trianglePool->reset();
}

bool statisticsInitialized = false;
//------------------------------------------------------------
void DoGameLogic()
{
	globalFloatHelp->resetAll();
	//-------------------------------------
	// Get me the current frameRate.
	// Convert to frameLength and any other timing stuff.
	if(frameRate < 4.0)
		frameRate = 4.0;
	frameLength = 1.0f / frameRate;
	bool doTransformMath = true;
	if(doTransformMath)
	{
		//-------------------------------------
		// Poll devices for this frame.
		editor->update();
		//----------------------------------------
		// Update all of the timers
		timerManager->update();
	}
	//---------------------------------------------------
	// Update heap instrumentation.
	globalHeapList->update();
	if(justResaveAllMaps)
	{
		PostQuitMessage(0);
		quitGame = true;		//We just needed to resave the maps.  Exit immediately
	}
	//---------------------------------------------------------------
	// Somewhere in all of the updates, we have asked to be excused!
	if(quitGame)
	{
		//EnterWindowMode();				//Game crashes if _TerminateApp called from fullScreen
		gos_TerminateApplication();
	}
}

//---------------------------------------------------------------------------
void InitializeGameEngine()
{
	InEditor = true;
	if(fileExists("mc2resUS.dll"))
		gosResourceHandle = gos_OpenResourceDLL("mc2resUS.dll");
	else
		gosResourceHandle = gos_OpenResourceDLL("mc2res.dll");
	gameResourceHandle = gos_OpenResourceDLL("editores.dll");
	cLoadString(IDS_MC2_FILEMISSING, FileMissingString, 511);
	cLoadString(IDS_MC2_CDMISSING, CDMissingString, 1023);
	cLoadString(IDS_MC2_MISSING_TITLE, MissingTitleString, 255);
	char temp[256];
	cLoadString(IDS_FLOAT_HELP_FONT, temp, 255);
	PSTR pStr = strstr(temp, ",");
	if(pStr)
	{
		gosFontScale = atoi(pStr + 2);
		*pStr = 0;
	}
	char path [256];
	strcpy(path, "assets\\graphics\\");
	strcat(path, temp);
	gosFontHandle = gos_LoadFont(path);
	//Check if we are a Voodoo 3.  If so, ONLY allow editor to run IF
	// Starting resolution is 1024x768x16 or LOWER.  NO 32-BIT ALLOWED!
	if((gos_GetMachineInformation(gos_Info_GetDeviceVendorID, 0) == 0x121a) &&
			(gos_GetMachineInformation(gos_Info_GetDeviceDeviceID, 0) == 0x0005))
	{
		DEVMODE dev;
		memset(&dev, 0, sizeof(DEVMODE));
		dev.dmSize = sizeof(DEVMODE);
		dev.dmSpecVersion = DM_SPECVERSION;
		EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dev);
		if((dev.dmPelsWidth > 1024) || (dev.dmPelsHeight > 768) || (dev.dmBitsPerPel > 16))
		{
			char title[256];
			char msg[2048];
			cLoadString(IDS_EDITOR_ERROR, title, 255);
			cLoadString(IDS_EDITOR_VOODOO3, msg, 2047);
			MessageBox(nullptr, msg, title, MB_OK | MB_ICONWARNING);
			ExitGameOS();
		}
	}
	//-------------------------------------------------------------
	// Find the CDPath in the registry and save it off so I can
	// look in CD Install Path for files.
	//Changed for the shared source release, just set to current directory
	//uint32_t maxPathLength = 1023;
	//gos_LoadDataFromRegistry("CDPath", CDInstallPath, &maxPathLength);
	//if (!maxPathLength)
	//	strcpy(CDInstallPath,"..\\");
	strcpy(CDInstallPath, ".\\");
	int32_t lastCharacter = strlen(CDInstallPath) - 1;
	if(CDInstallPath[lastCharacter] != '\\')
	{
		CDInstallPath[lastCharacter + 1] = '\\';
		CDInstallPath[lastCharacter + 2] = 0;
	}
	//--------------------------------------------------------------
	// Start the SystemHeap and globalHeapList
	globalHeapList = new HeapList;
	gosASSERT(globalHeapList != nullptr);
	globalHeapList->init();
	globalHeapList->update();		//Run Instrumentation into GOS Debugger Screen
	systemHeap = new UserHeap;
	gosASSERT(systemHeap != nullptr);
	systemHeap->init(systemHeapSize, "SYSTEM");
	Stuff::InitializeClasses();
	MidLevelRenderer::InitializeClasses(1024);
	gosFX::InitializeClasses();
	gos_PushCurrentHeap(MidLevelRenderer::Heap);
	MidLevelRenderer::TGAFilePool* pool = new MidLevelRenderer::TGAFilePool("data\\Effects\\");
	MidLevelRenderer::MLRTexturePool::Instance = new MidLevelRenderer::MLRTexturePool(pool);
	MidLevelRenderer::MLRSortByOrder* cameraSorter = new MidLevelRenderer::MLRSortByOrder(MidLevelRenderer::MLRTexturePool::Instance);
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
	if(result != NO_ERROR)
		STOP(("Could not find MC2.fx"));
	int32_t effectsSize = effectFile.fileSize();
	puint8_t effectsData = (puint8_t)systemHeap->Malloc(effectsSize);
	effectFile.read(effectsData, effectsSize);
	effectFile.close();
	effectStream = new Stuff::MemoryStream(effectsData, effectsSize);
	gosFX::EffectLibrary::Instance->Load(effectStream);
	gosFX::LightManager::Instance = new gosFX::LightManager();
	gos_PopCurrentHeap();
	systemHeap->Free(effectsData);
	globalFloatHelp = new FloatHelp(MAX_FLOAT_HELPS);
	//---------------------------------------------------------------------
	float doubleClickThreshold = 0.2f;
	int32_t dragThreshold = 10;
	//--------------------------------------------------------------
	// Read in System.CFG
	FitIniFilePtr systemFile = new FitIniFile;
#ifdef _DEBUG
	int32_t systemOpenResult =
#endif
		systemFile->open("system.cfg");
#ifdef _DEBUG
	if(systemOpenResult != NO_ERROR)
	{
		char Buffer[256];
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
			result = systemFile->readIdString("tglPath", tglPath, 79);
			gosASSERT(result == NO_ERROR);
			result = systemFile->readIdString("texturePath", texturePath, 79);
			gosASSERT(result == NO_ERROR);
		}
#ifdef _DEBUG
		int32_t fastFileResult =
#endif
			systemFile->seekBlock("FastFiles");
		gosASSERT(fastFileResult == NO_ERROR);
		{
			int32_t result = systemFile->readIdLong("NumFastFiles", maxFastFiles);
			if(result != NO_ERROR)
				maxFastFiles = 0;
			if(maxFastFiles)
			{
				fastFiles = (FastFile**)malloc(maxFastFiles * sizeof(FastFile*));
				memset(fastFiles, 0, maxFastFiles * sizeof(FastFile*));
				int32_t fileNum = 0;
				char fastFileId[10];
				char fileName[100];
				sprintf(fastFileId, "File%d", fileNum);
				while(systemFile->readIdString(fastFileId, fileName, 99) == NO_ERROR)
				{
					bool result = FastFileInit(fileName);
					if(!result)
						STOP(("Unable to startup fastfiles.  Probably an old one in the directory!!"));
					fileNum++;
					sprintf(fastFileId, "File%d", fileNum);
				}
			}
		}
		result = systemFile->seekBlock("CameraSettings");
		if(result == NO_ERROR)
		{
			result = systemFile->readIdFloat("MaxPerspective", Camera::MAX_PERSPECTIVE);
			if(result != NO_ERROR)
				Camera::MAX_PERSPECTIVE = 88.0f;
			if(Camera::MAX_PERSPECTIVE > 90.0f)
				Camera::MAX_PERSPECTIVE = 90.0f;
			result = systemFile->readIdFloat("MinPerspective", Camera::MIN_PERSPECTIVE);
			if(result != NO_ERROR)
				Camera::MIN_PERSPECTIVE = 18.0f;
			if(Camera::MIN_PERSPECTIVE < 0.0f)
				Camera::MIN_PERSPECTIVE = 0.0f;
			result = systemFile->readIdFloat("MaxOrtho", Camera::MAX_ORTHO);
			if(result != NO_ERROR)
				Camera::MAX_ORTHO = 88.0f;
			if(Camera::MAX_ORTHO > 90.0f)
				Camera::MAX_ORTHO = 90.0f;
			result = systemFile->readIdFloat("MinOrtho", Camera::MIN_ORTHO);
			if(result != NO_ERROR)
				Camera::MIN_ORTHO = 18.0f;
			if(Camera::MIN_ORTHO < 0.0f)
				Camera::MIN_ORTHO = 0.0f;
			result = systemFile->readIdFloat("AltitudeMinimum", Camera::AltitudeMinimum);
			if(result != NO_ERROR)
				Camera::AltitudeMinimum = 560.0f;
			if(Camera::AltitudeMinimum < 110.0f)
				Camera::AltitudeMinimum = 110.0f;
			result = systemFile->readIdFloat("AltitudeMaximumHi", Camera::AltitudeMaximumHi);
			if(result != NO_ERROR)
				Camera::AltitudeMaximumHi = 1600.0f;
			result = systemFile->readIdFloat("AltitudeMaximumLo", Camera::AltitudeMaximumLo);
			if(result != NO_ERROR)
				Camera::AltitudeMaximumHi = 1500.0f;
		}
	}
	systemFile->close();
	delete systemFile;
	systemFile = nullptr;
	//--------------------------------------------------------------
	// Read in Prefs.cfg
	FitIniFilePtr prefs = new FitIniFile;
	Environment.Key_Exit = (uint32_t) - 1;
#ifdef _DEBUG
	int32_t prefsOpenResult =
#endif
		prefs->open("prefs.cfg");
	gosASSERT(prefsOpenResult == NO_ERROR);
	{
#ifdef _DEBUG
		int32_t prefsBlockResult =
#endif
			prefs->seekBlock("MechCommander2Editor");
		gosASSERT(prefsBlockResult == NO_ERROR);
		{
			int32_t result = prefs->readIdFloat("CliffTerrainAngle", CliffTerrainAngle);
			if(result != NO_ERROR)
				CliffTerrainAngle = 45.0f;
		}
		prefs->seekBlock("MechCommander2");
		gosASSERT(prefsBlockResult == NO_ERROR);
		{
			Environment.Key_SwitchMonitors = 0;
			Environment.Key_FullScreen = 0;
			int32_t bitD = 0;
			int32_t result = prefs->readIdLong("BitDepth", bitD);
			if(result != NO_ERROR)
				Environment.bitDepth				= 16;
			else if(bitD == 32)
				Environment.bitDepth				= 32;
			else
				Environment.bitDepth				= 16;
			//Editor can't draw fullscreen!
//			bool fullScreen;
//			result = prefs->readIdBoolean("FullScreen",	fullScreen);
//			if (result != NO_ERROR)
//				Environment.fullScreen = 1;
//			else
//				Environment.fullScreen = (fullScreen == TRUE);
			int32_t fullScreenCard;
			result = prefs->readIdLong("FullScreenCard", fullScreenCard);
			if(result != NO_ERROR)
				Environment.FullScreenDevice		= 0;
			else
				Environment.FullScreenDevice		= fullScreenCard;
			int32_t rasterizer;
			result = prefs->readIdLong("Rasterizer", rasterizer);
			if(result != NO_ERROR)
				Environment.Renderer				= 0;
			else
				Environment.Renderer				= rasterizer;
			int32_t filterSetting;
			result = prefs->readIdLong("FilterState", filterSetting);
			if(result == NO_ERROR)
			{
				switch(filterSetting)
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
			if(result != NO_ERROR)
				TERRAIN_TXM_SIZE = 64;
			result = prefs->readIdLong("ObjectTextureRes", ObjectTextureSize);
			if(result != NO_ERROR)
				ObjectTextureSize = 128;
			result = prefs->readIdLong("Brightness", gammaLevel);
			if(result != NO_ERROR)
				gammaLevel = 0;
			result = prefs->readIdFloat("DoubleClickThreshold", doubleClickThreshold);
			if(result != NO_ERROR)
				doubleClickThreshold = 0.2f;
			result = prefs->readIdLong("DragThreshold", dragThreshold);
			if(result != NO_ERROR)
				dragThreshold = 10;
			result = prefs->readIdULong("BaseVertexColor", BaseVertexColor);
			if(result != NO_ERROR)
				BaseVertexColor = 0x00000000;
		}
	}
	prefs->close();
	delete prefs;
	prefs = nullptr;
	//---------------------------------------------------------------------
	//--------------------------------------------------
	// Setup Mouse Parameters from Prefs.CFG
	//--------------------------------------------------
	//---------------------------------------------------------
	// Start the Tiny Geometry Layer Heap.
	TG_Shape::tglHeap = new UserHeap;
	TG_Shape::tglHeap->init(tglHeapSize, "TinyGeom");
	//Start up the TGL RAM pools.
	colorPool 		= new TG_VertexPool;
	colorPool->init(20000);
	vertexPool 		= new TG_GOSVertexPool;
	vertexPool->init(20000);
	facePool 		= new TG_DWORDPool;
	facePool->init(40000);
	shadowPool 		= new TG_ShadowPool;
	shadowPool->init(20000);
	trianglePool 	= new TG_TrianglePool;
	trianglePool->init(20000);
	//--------------------------------------------------------------
	//------------------------------------------------
	// Fire up the MC Texture Manager.
	mcTextureManager = new MC_TextureManager;
	mcTextureManager->start();
	//Startup the vertex array pool
	mcTextureManager->startVertices(500000);
	//--------------------------------------------------------------
	// Load up the mouse cursors
	godMode = true;
	MOVE_init(30);
	//--------------------------
	// Create and Load the master Effects File
	weaponEffects = new WeaponEffects;
	weaponEffects->init("Effects");
	editor = new Editor();
	if(justResaveAllMaps)
	{
		editor->resaveAll();
	}
	{
		FullPathFileName guiloader;
		guiloader.init(artPath, "editorGui", ".fit");
		FitIniFile loader;
		int32_t result = loader.open(guiloader);
		gosASSERT(result == NO_ERROR);
		editor->init((PSTR)guiloader);
	}
	//---------------------------------------------------------
	// Start the Timers
	timerManager = new TimerManager;
	timerManager->init();
	//---------------------------------------------------------
	// Start the Color table code
	initColorTables();
	if(!statisticsInitialized)
	{
		statisticsInitialized = true;
		HeapList::initializeStatistics();
	}
	//Startup the Office Watson Handler.
	InitDW();
}

bool alreadyTermed = false;
void TerminateGameEngine()
{
	if(alreadyTermed)
		return;
	else
		alreadyTermed = true;
	//---------------------------------------------------------
	// End the Color table code
	destroyColorTables();
	//---------------------------------------------------------
	// End the Timers
	delete timerManager;
	timerManager = nullptr;
	if(editor)
		delete editor;
	editor = nullptr;
	//--------------------------
	// master Effects File 	if (weaponEffects)
	delete weaponEffects;
	weaponEffects = nullptr;
	MOVE_cleanup();
	//------------------------------------------------
	// shutdown the MC Texture Manager.
	if(mcTextureManager)
	{
		mcTextureManager->freeVertices();
		mcTextureManager->destroy();
		delete mcTextureManager;
		mcTextureManager = nullptr;
	}
	//---------------------------------------------------------
	// End the Tiny Geometry Layer Heap.
	if(TG_Shape::tglHeap)
	{
		//Shut down the TGL RAM pools.
		if(colorPool)
		{
			colorPool->destroy();
			delete colorPool;
			colorPool = nullptr;
		}
		if(vertexPool)
		{
			vertexPool->destroy();
			delete vertexPool;
			vertexPool = nullptr;
		}
		if(facePool)
		{
			facePool->destroy();
			delete facePool;
			facePool = nullptr;
		}
		if(shadowPool)
		{
			shadowPool->destroy();
			delete shadowPool;
			shadowPool = nullptr;
		}
		if(trianglePool)
		{
			trianglePool->destroy();
			delete trianglePool;
			trianglePool = nullptr;
		}
		TG_Shape::tglHeap->destroy();
		delete TG_Shape::tglHeap;
		TG_Shape::tglHeap = nullptr;
	}
	//
	//--------------------------
	// Turn off the fast Files
	//--------------------------
	//
	FastFileFini();
	delete globalFloatHelp;
	globalFloatHelp = nullptr;
	//----------------------------------------------------
	// Shutdown the MLR and associated stuff libraries
	//----------------------------------------------------
	gos_PushCurrentHeap(gosFX::Heap);
	delete effectStream;
	effectStream = nullptr;
	delete gosFX::LightManager::Instance;
	gosFX::LightManager::Instance = nullptr;
	gos_PopCurrentHeap();
	delete theClipper;
	theClipper = nullptr;
	delete MidLevelRenderer::MLRTexturePool::Instance;
	MidLevelRenderer::MLRTexturePool::Instance = nullptr;
	gosFX::TerminateClasses();
	MidLevelRenderer::TerminateClasses();
	Stuff::TerminateClasses();
	//--------------------------------------------------------------
	// End the SystemHeap and globalHeapList
	if(systemHeap)
	{
		systemHeap->destroy();
		delete systemHeap;
		systemHeap = nullptr;
	}
	if(globalHeapList)
	{
		globalHeapList->destroy();
		delete globalHeapList;
		globalHeapList = nullptr;
	}
	//---------------------------------------------------------
	gos_DeleteFont(gosFontHandle);
	gos_CloseResourceDLL(gameResourceHandle);
	gos_CloseResourceDLL(gosResourceHandle);
	/*terminating stuff not initialized explicitly by InitializeGameEngine()*/
	//---------------------------------------------------------
	// TEST of PORT
	// Create VFX PANE and WINDOW to test draw of old terrain!
	if(globalPane)
	{
		delete globalPane;
		globalPane = nullptr;
	}
	if(globalWindow)
	{
		delete globalWindow;
		globalWindow = nullptr;
	}
}

//----------------------------------------------------------------------------
// Same command line Parser as MechCommander
void ParseCommandLine(PSTR command_line)
{
	int32_t i;
	int32_t n_args = 0;
	int32_t index = 0;
	PSTR argv[30];
	char tempCommandLine[4096];
	memset(tempCommandLine, 0, 4096);
	strncpy(tempCommandLine, command_line, 4095);
	while(tempCommandLine[index] != '\0')   // until we null out
	{
		argv[n_args] = tempCommandLine + index;
		n_args++;
		while(tempCommandLine[index] != ' ' && tempCommandLine[index] != '\0')
		{
			index++;
		}
		while(tempCommandLine[index] == ' ')
		{
			tempCommandLine[index] = '\0';
			index++;
		}
	}
	i = 0;
	while(i < n_args)
	{
		if(strcmpi(argv[i], "-burnInShadows") == 0)
		{
			forceShadowBurnIn = true;
		}
		else if(strcmpi(argv[i], "-resaveall") == 0)
		{
			justResaveAllMaps = true;
		}
		else if(strcmpi(argv[i], "-nodialog") == 0)
		{
			gNoDialogs = true;
		}
		else if(strcmpi(argv[i], "-mission") == 0)
		{
			i++;
			if(i < n_args)
			{
				if(argv[i][0] == '"')
				{
					// They typed in a quote, keep reading argvs
					// until you find the close quote
					strcpy(missionName, &(argv[i][1]));
					bool scanName = true;
					while(scanName && (i < n_args))
					{
						i++;
						if(i < n_args)
						{
							strcat(missionName, " ");
							strcat(missionName, argv[i]);
							if(strstr(argv[i], "\"") != nullptr)
							{
								scanName = false;
								missionName[strlen(missionName) - 1] = 0;
							}
						}
						else
						{
							//They put a quote on the line with no space.
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
		i++;
	}
}

//---------------------------------------------------------------------
void GetGameOSEnvironment(PSTR CommandLine)
{
	ParseCommandLine(CommandLine);
	Environment.applicationName			= "MC2 Mission Editor";
	Environment.debugLog				= "";			//"DebugLog.txt";
	Environment.memoryTraceLevel		= 5;
	Environment.spew					= ""; //"GameOS_Texture GameOS_DirectDraw GameOS_Direct3D ";
	Environment.TimeStampSpew			= 0;
	Environment.GetGameInformation		= GetGameInformation;
	Environment.UpdateRenderers			= UpdateRenderers;
	Environment.InitializeGameEngine	= InitializeGameEngine;
	Environment.DoGameLogic				= DoGameLogic;
	Environment.TerminateGameEngine		= TerminateGameEngine;
	Environment.soundDisable			= TRUE;
	Environment.soundHiFi				= FALSE;
	Environment.soundChannels			= 24;
	Environment.dontClearRegistry		= true;
	Environment.allowMultipleApps		= false;
	Environment.version					= versionStamp;
	Environment.AntiAlias				= 0;
//
// Texture infomation
//
	Environment.Texture_S_256			= 6;
	Environment.Texture_S_128			= 1;
	Environment.Texture_S_64			= 0;
	Environment.Texture_S_32			= 1;
	Environment.Texture_S_16			= 5;
	Environment.Texture_K_256			= 2;
	Environment.Texture_K_128			= 5;
	Environment.Texture_K_64			= 5;
	Environment.Texture_K_32			= 5;
	Environment.Texture_K_16			= 5;
	Environment.Texture_A_256			= 0;
	Environment.Texture_A_128			= 1;
	Environment.Texture_A_64			= 5;
	Environment.Texture_A_32			= 1;
	Environment.Texture_A_16			= 0;
	Environment.bitDepth				= 16;
	Environment.RaidDataSource			= "MechCommander 2:Raid4";
	Environment.RaidFilePath			= "\\\\aas1\\MC2\\Test\\GOSRaid";
	Environment.RaidCustomFields		= "Area=GOSRaid";
	DEVMODE dev;
	memset(&dev, 0, sizeof(DEVMODE));
	dev.dmSize = sizeof(DEVMODE);
	dev.dmSpecVersion = DM_SPECVERSION;
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dev);
	Environment.screenWidth = dev.dmPelsWidth;
	Environment.screenHeight = dev.dmPelsHeight;
	if(((512 > Environment.screenWidth) || (384 > Environment.screenHeight) || (1600 < Environment.screenWidth) || (1200 < Environment.screenHeight)) && (0 == dev.dmDeviceName[0]))
	{
		/* might be a buggy driver reporting incorrectly (like the permedia2 win95 driver */
		Environment.screenWidth = GetDeviceCaps(GetDC(nullptr), HORZRES);
		Environment.screenHeight = GetDeviceCaps(GetDC(nullptr), VERTRES);
	}
	//CHeck for non-standard resolutions.  Like LapTops or ATI cards or BOTH!!
	if(Environment.screenWidth < 512)
	{
		Environment.screenWidth = 512;
		Environment.screenHeight = 384;
	}
	else if((Environment.screenWidth > 512) && (Environment.screenWidth < 640))
	{
		Environment.screenWidth = 640;
		Environment.screenHeight = 480;
	}
	else if((Environment.screenWidth > 640) && (Environment.screenWidth < 800))
	{
		Environment.screenWidth = 512;
		Environment.screenHeight = 384;
	}
	else if((Environment.screenWidth > 800) && (Environment.screenWidth < 1024))
	{
		Environment.screenWidth = 800;
		Environment.screenHeight = 600;
	}
	else if((Environment.screenWidth > 1024) && (Environment.screenWidth < 1280))
	{
		Environment.screenWidth = 1024;
		Environment.screenHeight = 768;
	}
	else if((Environment.screenWidth > 1280) && (Environment.screenWidth < 1600))
	{
		Environment.screenWidth = 1280;
		Environment.screenHeight = 1024;
	}
	else if(Environment.screenWidth > 1600)
	{
		Environment.screenWidth = 1600;
		Environment.screenHeight = 1200;
	}
	Environment.Suppress3DFullScreenWarning = 0;
	EditorData::setMapName(nullptr);
}



