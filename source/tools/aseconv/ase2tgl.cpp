//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// ..\..\mclib\$(ConfigurationName)\mclib.lib
// ..\..\gameos\lib\$(ConfigurationName)\gameos.lib
// ..\..\gameos\lib\$(ConfigurationName)\dllplatform.lib
// ..\..\gameos\lib\$(ConfigurationName)\mfcplatform.lib
// ..\..\mclib\gosfx\$(ConfigurationName)\gosfx.lib
// ..\..\mclib\mlr\$(ConfigurationName)\mlr.lib
// ..\..\mclib\stuff\$(ConfigurationName)\stuff.lib
// ..\..\gameos\lib\$(ConfigurationName)\zlib.lib

#include "stdafx.h"

#include "version.h"

#include "../../ARM/Microsoft.Xna.Arm.h"
using namespace Microsoft::Xna::Arm;

HINSTANCE hInst = nullptr;
uint32_t gosResourceHandle = 0;

Stuff::MemoryStream* effectStream = nullptr;

extern char CDInstallPath[];

bool hasGuardBand = false;
bool justResaveAllMaps = false;
Camera* eye = nullptr;
enum { CPU_UNKNOWN, CPU_PENTIUM, CPU_MMX, CPU_KATMAI } Processor = CPU_PENTIUM; //Needs to be set when GameOS supports ProcessorID -- MECHCMDR2

float MaxMinUV = 8.0f;

uint32_t BaseVertexColor = 0x00000000;

static PCSTR lpszAppName = "MechCmdr2";

UserHeapPtr systemHeap = nullptr;
UserHeapPtr guiHeap = nullptr;

float gosFontScale = 1.0f;

extern bool silentMode;
bool useLOSAngle = false;
static bool createARM = false;

IProviderEngine* armProvider = nullptr;

uint32_t tglHeapSize = 16386000;

FastFile** fastFiles = nullptr;
int32_t numFastFiles = 0;
int32_t maxFastFiles = 0;

HWND appWnd = nullptr;

extern PSTR MechAnimationNames[MaxGestures];

int32_t ObjectTextureSize = 128;
bool reloadBounds = false;
MidLevelRenderer::MLRClipper* theClipper = nullptr;
HGOSFONT3D gosFontHandle = 0;
extern HGOSFONT3D FontHandle;
FloatHelpPtr globalFloatHelp = nullptr;
uint32_t currentFloatHelp = 0;

char fileName[1024];
char listName[1024];

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
	while(tempCommandLine[index] != '\0')  // until we null out
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
		if(strcmpi(argv[i], "-file") == 0)
		{
			i++;
			if(i < n_args)
			{
				if(argv[i][0] == '"')
				{
					// They typed in a quote, keep reading argvs
					// until you find the close quote
					strcpy(fileName, &(argv[i][1]));
					bool scanName = true;
					while(scanName && (i < n_args))
					{
						i++;
						if(i < n_args)
						{
							strcat(fileName, " ");
							strcat(fileName, argv[i]);
							if(strstr(argv[i], "\"") != nullptr)
							{
								scanName = false;
								fileName[strlen(fileName) - 1] = 0;
							}
						}
						else
						{
							//They put a quote on the line with no space.
							//
							scanName = false;
							fileName[strlen(fileName) - 1] = 0;
						}
					}
				}
				else
					strcpy(fileName, argv[i]);
			}
		}
		if(strcmpi(argv[i], "-list") == 0)
		{
			i++;
			if(i < n_args)
			{
				if(argv[i][0] == '"')
				{
					// They typed in a quote, keep reading argvs
					// until you find the close quote
					strcpy(listName, &(argv[i][1]));
					bool scanName = true;
					while(scanName && (i < n_args))
					{
						i++;
						if(i < n_args)
						{
							strcat(listName, " ");
							strcat(listName, argv[i]);
							if(strstr(argv[i], "\"") != nullptr)
							{
								scanName = false;
								listName[strlen(listName) - 1] = 0;
							}
						}
						else
						{
							//They put a quote on the line with no space.
							//
							scanName = false;
							listName[strlen(listName) - 1] = 0;
						}
					}
				}
				else
					strcpy(listName, argv[i]);
			}
		}
		if(strcmpi(argv[i], "-arm") == 0)
		{
			createARM = true;
		}
		i++;
	}
}

//-----------------------------
int32_t convertASE2TGL(PSTR file)
{
	//---------------------------------------------------
	// Get all of the .ASE files in the tgl directory.
	char findString[1024];
	char armFileName[1024];
	if(file[0] == '\0')
	{
		sprintf(findString, "%s*.ini", tglPath);
	}
	else
	{
		strcpy(findString, tglPath);
		strcat(findString, file);
	}
	int32_t count = 0;
	WIN32_FIND_DATA findResult;
	HANDLE searchHandle = FindFirstFile(findString, &findResult);
	do
	{
		if((findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			//-----------------------------------
			// Search for TGLData
			FullPathFileName iniName;
			iniName.init(tglPath, findResult.cFileName, "");
			FitIniFile iniFile;
			int32_t result = iniFile.open(iniName);
			if(result != NO_ERROR)
				return result;
			// ARM
			IProviderAssetPtr iniAsset = armProvider->OpenAsset((PSTR)iniName, AssetType_Physical,
										 ProviderType_Primary);
			iniAsset->AddProperty("Type", "Object Definition");
			iniAsset->AddProperty("Version", "1.0");
			TG_TypeMultiShape* shape = nullptr;
			result = iniFile.seekBlock("TGLData");
			if(result == NO_ERROR)
			{
				char fileName[1024];
				result = iniFile.readIdString("FileName", fileName, 1023);
				if(result != NO_ERROR)
				{
					//---------------------------------------------
					// We have LODs -- handle differently
					// We will get animation from LAST LOD loaded
					int32_t i = 0;
					char fileCheck[1024];
					sprintf(fileCheck, "FileName%d", i);
					result = iniFile.readIdString(fileCheck, fileName, 1023);
					while(result == NO_ERROR)
					{
						if(shape)
						{
							delete shape;
							shape = nullptr;
						}
						char aseName[1024];
						sprintf(aseName, "%s%s%s", tglPath, fileName, ".ase");
						//---------------------------------------------------------------------------------------------
						// Load Base Shape or LOD 0 Shape.
						shape = new TG_TypeMultiShape;
						gosASSERT(shape != nullptr);
						printf("Processing Main Shape %s\n", aseName);
						char lodID[4];
						sprintf(lodID, "%02d", count);
						IProviderRelationshipPtr armLink = iniAsset->AddRelationship("LOD Shape", aseName);
						armLink->AddProperty("LOD", lodID);
						shape->LoadTGMultiShapeFromASE(aseName, true, armProvider);
						i++;
						sprintf(fileCheck, "FileName%d", i);
						result = iniFile.readIdString(fileCheck, fileName, 1023);
					}
				}
				char aseName[1024];
				sprintf(aseName, "%s%s%s", tglPath, fileName, ".ase");
				//---------------------------------------------------------------------------------------------
				// Load Base Shape or LOD 0 Shape.
				shape = new TG_TypeMultiShape;
				gosASSERT(shape != nullptr);
				printf("Processing Main Shape %s\n", aseName);
				IProviderRelationshipPtr armLink = iniAsset->AddRelationship("Main Shape", aseName);
				shape->LoadTGMultiShapeFromASE(aseName, true, armProvider);
				//-------------------------------------------
				// Gotta make the special shadow shape now!!
				// MUST use its own shape or animation below
				// will go straight to HELL!!
				result = iniFile.readIdString("ShadowName", fileName, 1023);
				if(result == NO_ERROR)
				{
					char aseName[1024];
					sprintf(aseName, "%s%s%s", tglPath, fileName, ".ase");
					//---------------------------------------------------------------------------------------------
					// Load Base Shape or LOD 0 Shape.
					TG_TypeMultiShapePtr shadowShape = new TG_TypeMultiShape;
					gosASSERT(shadowShape != nullptr);
					printf("Processing Shadow Shape %s\n", aseName);
					IProviderRelationshipPtr armLink = iniAsset->AddRelationship("Shadow Shape", aseName);
					shadowShape->LoadTGMultiShapeFromASE(aseName, true, armProvider);
					delete shadowShape;
					shadowShape = nullptr;
				}
				int32_t i = 0;
				char animCheck[1024];
				sprintf(animCheck, "Animation:%d", i);
				result = iniFile.seekBlock(animCheck);
				while(result == NO_ERROR)  //This thing has animations. Process them!
				{
					char fileName[1024];
					result = iniFile.readIdString("AnimationName", fileName, 1023);
					if(result == NO_ERROR)
					{
						FullPathFileName aseName;
						aseName.init(tglPath, fileName, ".ase");
						TG_AnimateShape* anim = new TG_AnimateShape;
						gosASSERT(mechAnim != nullptr);
						//-----------------------------------------------
						// Skip this one if its already a binary file.
						// Happens ALOT!
						printf("Processing Animation %s\n", aseName);
						IProviderRelationshipPtr armLink = iniAsset->AddRelationship("Animation", (PSTR)aseName);
						anim->LoadTGMultiShapeAnimationFromASE(aseName, shape, true);
						delete anim;
						anim = nullptr;
					}
					i++;
					sprintf(animCheck, "Animation:%d", i);
					result = iniFile.seekBlock(animCheck);
				}
				if(!i)  //No Animations, BUT they may mean we are a MECH!!!
				{
					if(iniFile.seekBlock("Gestures0") == NO_ERROR)
					{
						//We ARE a mech. Load all of the animations for this mech and write 'em out.
						for(size_t i = 0; i < MaxGestures; i++)
						{
							char name[MAX_PATH];
							_splitpath(findResult.cFileName, nullptr, nullptr, name, nullptr);
							char mechFileName[1024];
							sprintf(mechFileName, "%s%s%s.ase", tglPath, name, MechAnimationNames[i]);
							TG_AnimateShape* anim = new TG_AnimateShape;
							gosASSERT(anim != nullptr);
							//-----------------------------------------------
							// Skip this one if its already a binary file.
							// Happens ALOT!
							printf("Processing Animation %s\n", mechFileName);
							IProviderRelationshipPtr armLink = iniAsset->AddRelationship("Animation", mechFileName);
							anim->LoadTGMultiShapeAnimationFromASE(mechFileName, shape, true);
							delete anim;
							anim = nullptr;
						}
					}
				}
				if(!i)  //No Animations, BUT they may mean we are a MECH!!!
				{
					if(iniFile.seekBlock("Gestures0") == NO_ERROR)
					{
						//We ARE a mech. Load all of the destroyed shapes for this mech and write 'em out.
						for(size_t i = MaxGestures; i < MaxGestures + 2; i++)
						{
							char name[MAX_PATH];
							_splitpath(findResult.cFileName, nullptr, nullptr, name, nullptr);
							char mechFileName[1024];
							sprintf(mechFileName, "%s%s%s.ase", tglPath, name, MechAnimationNames[i]);
							//-----------------------------------------------
							shape = new TG_TypeMultiShape;
							gosASSERT(shape != nullptr);
							printf("Processing Animation %s\n", mechFileName);
							IProviderRelationshipPtr armLink = iniAsset->AddRelationship("Destroyed Shape", mechFileName);
							shape->LoadTGMultiShapeFromASE(mechFileName, true, armProvider);
							delete shape;
							shape = nullptr;
						}
					}
				}
				if(iniFile.seekBlock("TGLDamage") == NO_ERROR)
				{
					char fileName[1024];
					result = iniFile.readIdString("FileName", fileName, 1023);
					if(result == NO_ERROR)
					{
						if(shape)
						{
							delete shape;
							shape = nullptr;
						}
						char aseName[1024];
						sprintf(aseName, "%s%s%s", tglPath, fileName, ".ase");
						//---------------------------------------------------------------------------------------------
						// Load Base Shape or LOD 0 Shape.
						shape = new TG_TypeMultiShape;
						gosASSERT(shape != nullptr);
						printf("Processing Damage Shape %s\n", aseName);
						IProviderRelationshipPtr armLink = iniAsset->AddRelationship("Damage Shape", aseName);
						shape->LoadTGMultiShapeFromASE(aseName, true, armProvider);
					}
					//-------------------------------------------
					// Gotta make the special shadow shape now!!
					// MUST use its own shape or animation below
					// will go straight to HELL!!
					result = iniFile.readIdString("ShadowName", fileName, 1023);
					if(result == NO_ERROR)
					{
						char aseName[1024];
						sprintf(aseName, "%s%s%s", tglPath, fileName, ".ase");
						//---------------------------------------------------------------------------------------------
						// Load Base Shape or LOD 0 Shape.
						TG_TypeMultiShapePtr shadowShape = new TG_TypeMultiShape;
						gosASSERT(shadowShape != nullptr);
						printf("Processing Damage Shadow Shape %s\n", aseName);
						IProviderRelationshipPtr armLink = iniAsset->AddRelationship("Damage Shadow Shape", aseName);
						shadowShape->LoadTGMultiShapeFromASE(aseName, true, armProvider);
						delete shadowShape;
						shadowShape = nullptr;
					}
				}
			}
			delete shape;
			shape = nullptr;
			iniAsset->Close();
		}
	}
	while(FindNextFile(searchHandle, &findResult) != 0);
	FindClose(searchHandle);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd,
						 uint32_t message,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return (GameOSWinProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

//-----------------------------
int32_t APIENTRY WinMain(HINSTANCE hInstance,
						 HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine,
						 int32_t nCmdShow)
{
	WNDCLASS wc;
	if(!hPrevInstance)
	{
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = (WNDPROC)WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
		wc.lpszClassName = lpszAppName;
		if(RegisterClass(&wc) == 0)
			return false;
	}
	hInst = hInstance;
	char appTitle[1024];
	sprintf(appTitle, "MechCommander 2 Data Editor %s", versionStamp);
	appWnd = CreateWindow(
				 lpszAppName,
				 appTitle,
				 WS_OVERLAPPEDWINDOW,
				 CW_USEDEFAULT, CW_USEDEFAULT,
				 640, 480,
				 nullptr,
				 nullptr,
				 hInstance,
				 nullptr
			 );
	if(appWnd == nullptr)
		return false;
	globalHeapList = new HeapList;
	assert(globalHeapList != nullptr);
	systemHeap = new UserHeap;
	assert(systemHeap != nullptr);
	systemHeap->init(2048000);
	//---------------------------------------------------------
	// Start the Tiny Geometry Layer Heap.
	TG_Shape::tglHeap = new UserHeap;
	TG_Shape::tglHeap->init(tglHeapSize, "TinyGeom");
	//--------------------------------------------------------------
	// Read in System.CFG
	FitIniFilePtr systemFile = new FitIniFile;
#ifdef _DEBUG
	int32_t systemOpenResult =
#endif
		systemFile->open("system.cfg");
#ifdef _DEBUG
	assert(systemOpenResult == NO_ERROR);
#endif
	{
#ifdef _DEBUG
		int32_t systemPathResult =
#endif
			systemFile->seekBlock("systemPaths");
		assert(systemPathResult == NO_ERROR);
		{
			int32_t result = systemFile->readIdString("terrainPath", terrainPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("artPath", artPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("fontPath", fontPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("savePath", savePath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("spritePath", spritePath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("shapesPath", shapesPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("soundPath", soundPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("objectPath", objectPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("cameraPath", cameraPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("tilePath", tilePath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("missionPath", missionPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("warriorPath", warriorPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("profilePath", profilePath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("interfacepath", interfacePath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("moviepath", moviePath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("tglpath", tglPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("texturepath", texturePath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("CDsoundPath", CDsoundPath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("CDmoviepath", CDmoviePath, 79);
			assert(result == NO_ERROR);
			result = systemFile->readIdString("CDspritePath", CDspritePath, 79);
			assert(result == NO_ERROR);
		}
	}
	systemFile->close();
	delete systemFile;
	systemFile = nullptr;
	//
	// Init GameOS with window created
	//
	//InitGameOS( hInstance, appWnd, lpCmdLine );
	Platform = Platform_DLL;
	//-------------------------------------------------------------
	// Find the CDPath in the registry and save it off so I can
	// look in CD Install Path for files.
	//Changed for the shared source release, just set to current directory
	//uint32_t maxPathLength = 1023;
	//gos_LoadDataFromRegistry("CDPath", CDInstallPath, &maxPathLength);
	//if (!maxPathLength)
	// strcpy(CDInstallPath,"..\\");
	strcpy(CDInstallPath, ".\\");
	//-------------------------------------------------------
	// Check if we are running this from the command line
	// with ASE2TGL as the command line parameter. If so,
	// for each .INI file in data\tgl, find the corresponding
	// .ASE file and convert to .TGL. Then Exit!
	silentMode = true;
	memset(fileName, 0, sizeof(fileName));
	memset(listName, 0, sizeof(listName));
	ParseCommandLine(lpCmdLine);
	// Initialize COM and create an instance of the InterfaceImplementation class:
	CoInitialize(nullptr);
	armProvider = CreateProviderEngine("AseConv", versionStamp);
	assert(armProvider);
	if(listName[0] == 0)
	{
		convertASE2TGL(fileName);
	}
	else
	{
		//
		// A list file was provided
		//
		File file;
		if(file.open(listName) == NO_ERROR)
		{
			while(!file.eof())
			{
				char line[1024];
				file.readLine((puint8_t)line, 1024);
				if(line[0] != 0)
					convertASE2TGL(line);
			}
			file.close();
		}
	}
	/*
	//Time BOMB goes here.
	// Set Date and write Binary data to registry under key
	// GraphicsDataInit!!
	SYSTEMTIME bombDate;
	uint32_t dataSize = sizeof(SYSTEMTIME);
	gos_LoadDataFromRegistry("GraphicsDataInit", &bombDate, &dataSize);
	if (dataSize == 0)
	{
	bombDate.wYear = 2001;
	bombDate.wMonth = 3;
	bombDate.wDayOfWeek = 4;
	bombDate.wDay = 31;
	bombDate.wHour = 0;
	bombDate.wMinute = 0;
	bombDate.wSecond = 0;
	bombDate.wMilliseconds = 0;

	dataSize = sizeof(SYSTEMTIME);
	gos_SaveDataToRegistry("GraphicsDataInit", &bombDate, dataSize);
	}
	*/
	//
	// Exit app
	//
	CoUninitialize();
	//ExitGameOS();
	return 0;
}

uint32_t Seed;

//
//
//
void UpdateRenderers()
{
}

void DoGameLogic()
{
}


//
// Setup the GameOS structure
//
void GetGameOSEnvironment(PSTR CommandLine)
{
	CommandLine = CommandLine;
	Environment.applicationName = "MechCmdr2";
	Environment.screenWidth = 640;
	Environment.screenHeight = 480;
	Environment.bitDepth = 16;
	Environment.DoGameLogic = DoGameLogic;
	Environment.UpdateRenderers = UpdateRenderers;
	Environment.version = versionStamp;
}


