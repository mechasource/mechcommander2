//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "EditorInterface.h"
#include "dstd.h"

//#pragma warning( disable:4201 )
#include "mmsystem.h"
//#pragma warning( default:4201 )

#ifndef EDITORCAMERA_H
#include "EditorCamera.h"
#endif

#ifndef EDITORMESSAGES_H
#include "EditorMessages.h"
#endif

#ifndef EDITORDATA_H
#include "editordata.h"
#endif

#ifndef OVERLAYBRUSH_H
#include "OverlayBrush.h"
#endif

#ifndef ACTION_H
#include "action.h"
#endif

#ifndef TERRAINBRUSH_H
#include "TerrainBrush.h"
#endif

#ifndef UTILITIES_H
#include "utilities.h"
#endif

#ifndef BUILDINGBRUSH_H
#include "buildingbrush.h"
#endif

#ifndef ERASER_H
#include "Eraser.h"
#endif

#ifndef SELECTIONBRUSH_H
#include "SelectionBrush.h"
#endif

#ifndef FLATTENBRUSH_H
#include "FlattenBrush.h"
#endif

#ifndef HEIGHTDLG_H
#include "heightDlg.h"
#endif

#include "SelectSlopeDialog.h"

#ifndef TERRAINDLG_H
#include "TerrainDlg.h"
#endif

#ifndef MAPSIZEDLG_H
#include "MapsizeDlg.h"
#endif

#include "resource.h"

#ifndef SUNDLG_H
#include "sunDlg.h"
#endif

#ifndef FOGDLG_H
#include "FogDlg.h"
#endif

#ifndef AFX_WATERDLG_H
#include "WaterDlg.h"
#endif

#ifndef VERTEX_H
#include "Vertex.h"
#endif

#include "NewSingleMission.h"

#ifndef SINGLEVALUEDLG_H
#include "SingleValueDlg.h"
#endif

#include "MissionSettingsDlg.h"

#include "PlayerSettingsDlg.h"

#include "SelectTerrainTypeDlg.h"

#include "FractalDialog.h"

#ifndef EDITOROBJECTS_H
#include "EditorObjects.h"
#endif

#ifndef MESSAGEBOX_H
#include "messagebox.h"
#endif

#ifndef DRAGTOOL_H
#include "dragtool.h"
#endif

#ifndef DAMAGEBRUSH_H
#include "damagebrush.h"
#endif

#ifndef MINEBRUSH_H
#include "MineBrush.h"
#endif

#include "resource.h"

#ifndef LINKBRUSH_H
#include "Linkbrush.h"
#endif
#ifndef DROPZONEBRUSH_H
#include "DropZoneBrush.h"
#endif

#include "UnitSettingsDlg.h"
#include "buildingsettingsdlg.h"
#include "TilingFactorsDialog.h"

#include "MFCPlatform.hpp"

#include "MainFrm.h"

#include "EditorObjects.h"
#include "ForestDlg.h"
#include "editforestdlg.h"

#include "campaigndialog.h"

extern bool silentMode;

// ARM
extern wchar_t versionStamp[];

#include "../ARM/Microsoft.Xna.Arm.h"
//using namespace Microsoft::Xna::Arm;
IProviderEngine* armProvider;

//#pragma warning( disable:4244 )

//-----------------------------------
// Frank at work!
CameraPtr eye = nullptr;

extern bool drawTerrainTiles;
extern int32_t terrainLineChanged;
extern bool drawTerrainOverlays;
extern bool renderObjects;
extern bool renderTrees;
extern bool drawTerrainGrid;
extern bool drawLOSGrid;
extern bool useClouds;
extern bool useFog;
extern bool useShadows;
extern bool useWaterInterestTexture;
int32_t lightState = 0;
extern bool useFaceLighting;
extern bool useVertexLighting;
extern bool reloadBounds;
bool s_bSensorMapEnabled = false;
extern bool justResaveAllMaps;
extern bool bIsLoading;
EditorInterface* EditorInterface::s_instance = nullptr;

extern volatile int32_t ProcessingError;

const float HSCROLLBAR_RANGE = 30000.0;
const float VSCROLLBAR_RANGE = 30000.0;

extern wchar_t missionName[];

static wchar_t szTGAFilter[] = "TGA Files (*.TGA)|*.tga||";
static wchar_t szPAKFilter[] = "Pak Files (*.PAK)|*.pak||";

static bool windowSizeChanged = false;
static float g_newwidth = 0.0;
static float g_newheight = 0.0;

// EDITOR stuff

void Editor::init(std::wstring_view loader)
{
	volatile float crap = 0;
	bool bOK = false;
	if (!eye)
		eye = new EditorCamera;
	Pilot::initPilots();
	FullPathFileName bdgFileName;
	bdgFileName.init(artPath, "Buildings", ".csv");
	FullPathFileName objFileName;
	objFileName.init(objectPath, "Object2", ".pak");
	EditorObjectMgr* pMgr = EditorObjectMgr::instance();
	pMgr->init(bdgFileName, objFileName);
	FullPathFileName mPath;
	mPath.init(missionPath, missionName, ".pak");
	bool bCanceled = false;
	if (!justResaveAllMaps)
	{
		if (fileExists(mPath))
		{
			bOK = EditorData::initTerrainFromPCV(mPath);
		}
		else
		{
			NewSingleMission dlg;
			bool resolved = false;
			while (!resolved)
			{
				int32_t retVal = dlg.DoModal();
				if (retVal == IDCANCEL)
				{
					EditorInterface::instance()->SetBusyMode();
					resolved = true;
					gos_TerminateApplication();
					PostQuitMessage(0);
					bOK = true;
					bCanceled = true;
					EditorInterface::instance()->UnsetBusyMode();
				}
				else if (retVal == ID_NEWMISSION)
				{
					resolved = true;
					while (!bOK)
					{
						TerrainDlg dlg;
						dlg.terrain = 0;
						if (IDOK == dlg.DoModal())
						{
							MapSizeDlg msdlg;
							msdlg.mapSize = 0;
							if (IDOK == msdlg.DoModal())
							{
								wchar_t path[256];
								strcpy(path, cameraPath);
								strcat(path, "cameras.fit");
								FitIniFile camFile;
								if (NO_ERROR != camFile.open(path))
								{
									STOP(("Need Camera File "));
								}
								EditorInterface::instance()->SetBusyMode();
								eye->init(&camFile);
								bOK = EditorData::initTerrainFromTGA(
									msdlg.mapSize, 0, 0, dlg.terrain);
								EditorInterface::instance()->UnsetBusyMode();
							}
							else
							{
								resolved = false;
								break;
							}
						}
						else // They pressed cancel.  Let 'em cancel, dammit!
						{
							resolved = false;
							break;
						}
					}
				}
				else
				{
					resolved = true;
					CFileDialog fileDlg(1, "pak", nullptr,
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szPAKFilter);
					fileDlg.m_ofn.lpstrInitialDir = missionPath;
					bOK = false;
					while (!bOK)
					{
						if (IDOK == fileDlg.DoModal())
						{
							EditorInterface::instance()->SetBusyMode();
							std::wstring_view pFile = fileDlg.m_ofn.lpstrFile;
							bOK = EditorData::initTerrainFromPCV(pFile);
							EditorInterface::instance()->UnsetBusyMode();
						}
						else
						{
							resolved = false;
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		bOK = true;
	}
	if (bCanceled)
	{
		/*We are in the middle of GOS initialization, so even though we are
		going to exit the app as soon as possible, we'll allow the GOS
		initialization to continue.*/
		if (!land)
		{
			land = new Terrain();
			land->init(0, (PacketFile*)nullptr, EDITOR_VISIBLE_VERTICES, crap, 100);
		}
	}
	if (bOK)
		EditorInterface::instance()->init(loader);
	PlaySound("SystemDefault", nullptr, SND_ASYNC);
}

void Editor::destroy(void)
{
	if (EditorInterface::instance())
	{
		EditorInterface::instance()->terminate();
	}
	/*
	EditorData::clear()
	clears the ActionUndoMgr,
	terminates and deletes "land",
	and clears EditorObjectMgr
	among other things
	*/
	EditorData::clear();
	if (eye)
		delete eye;
	eye = nullptr;
}

void Editor::resaveAll(void)
{
	// Startup the editor.
	if (!eye)
		eye = new EditorCamera;
	// ARM
	CoInitialize(nullptr);
	armProvider = CreateProviderEngine("MC2Editor", versionStamp);
	silentMode = true; // shut up the warnings from tgl export
	wchar_t campaignFiles[2][256] = {0};
	strcpy(campaignFiles[0], "Data\\Campaign\\campaign.fit");
	strcpy(campaignFiles[1], "Data\\Campaign\\tutorial.fit");
	for (size_t i = 0; i < 2; i++) // once for the campaign, then the tutorial
	{
		CCampaignData campaignData;
		campaignData.Read(campaignFiles[i]);
		IProviderAssetPtr campaignAssetPtr =
			armProvider->OpenAsset(campaignFiles[i], AssetType_Physical, ProviderType_Primary);
		campaignAssetPtr->AddProperty("Type", "Campaign");
		int32_t count = 0;
		wchar_t buf[512] = {0};
		// for all groups
		for (EList<CGroupData, CGroupData>::EConstIterator iter = campaignData.m_GroupList.Begin();
			 !iter.IsDone(); count++, iter++)
		{
			CGroupData groupData = (*iter);
			// Make a virtual group name
			strcpy(buf, campaignFiles[i]);
			int32_t campaignLen = strlen(buf);
			buf[campaignLen - 4] = '_';
			buf[campaignLen - 3] = 0;
			strcat(buf, "group");
			wchar_t groupId[4] = {0};
			sprintf(groupId, "%02d", count + 1);
			strcat(buf, groupId);
			// Add a relationship from the campaign to the virtual group
			campaignAssetPtr->AddRelationship("group", buf);
			// Open the virtual asset
			IProviderAssetPtr groupAssetPtr =
				armProvider->OpenAsset(buf, AssetType_Virtual, ProviderType_Primary);
			groupAssetPtr->AddProperty("Type", "Mission group");
			// Add all the relationships
			if (groupData.m_OperationFile != "")
			{
				strcpy(buf, "Data\\Art\\");
				strcat(buf, groupData.m_OperationFile.GetBuffer());
				strcat(buf, ".fit");
				groupAssetPtr->AddRelationship("Operation", buf);
			}
			if (groupData.m_ABLScript != "")
			{
				strcpy(buf, "Data\\Missions\\");
				strcat(buf, groupData.m_ABLScript.GetBuffer());
				strcat(buf, ".abl");
				groupAssetPtr->AddRelationship("ABLScript", buf);
			}
			if (groupData.m_PreVideoFile != "")
			{
				strcpy(buf, "Data\\Movies\\");
				strcat(buf, groupData.m_PreVideoFile.GetBuffer());
				// The only instances of PreVideoFile I've seen had the
				// extension already
				groupAssetPtr->AddRelationship("PreVideo", buf);
			}
			if (groupData.m_VideoFile != "")
			{
				strcpy(buf, "Data\\Movies\\");
				strcat(buf, groupData.m_VideoFile.GetBuffer());
				strcat(buf, ".bik");
				groupAssetPtr->AddRelationship("Video", buf);
			}
			// for all missions part of this group
			for (EList<CMissionData, CMissionData>::EConstIterator iter2 =
					 groupData.m_MissionList.Begin();
				 !iter2.IsDone(); iter2++)
			{
				CMissionData missionData = *(iter2);
				if (missionData.m_MissionFile != "")
				{
					strcpy(buf, "Data\\Missions\\");
					strcat(buf, missionData.m_MissionFile.GetBuffer());
					strcat(buf, ".fit");
					groupAssetPtr->AddRelationship("Mission", buf);
				}
				if (missionData.m_PurchaseFile != "")
				{
					strcpy(buf, "Data\\Missions\\");
					strcat(buf, missionData.m_PurchaseFile.GetBuffer());
					strcat(buf, ".fit");
					IProviderRelationshipPtr rel =
						groupAssetPtr->AddRelationship("PurchaseFile", buf);
					rel->AddProperty("Mission", missionData.m_MissionFile.GetBuffer());
				}
			}
			groupAssetPtr->Close();
		}
		campaignAssetPtr->Close();
	}
	Pilot::initPilots();
	wchar_t bdgFileName[256];
	strcpy(bdgFileName, artPath);
	strcat(bdgFileName, "Buildings.csv");
	_strlwr(bdgFileName);
	wchar_t objFileName[256];
	strcpy(objFileName, objectPath);
	strcat(objFileName, "object2.pak");
	_strlwr(objFileName);
	EditorObjectMgr* pMgr = EditorObjectMgr::instance();
	pMgr->init(bdgFileName, objFileName);
	//----------------------------------------------------------------------------
	// Recurse through the data\missions directory and resave every .PAK you
	// find!
	wchar_t findString[512];
	sprintf(findString, "%s*.pak", missionPath);
	WIN32_FIND_DATA findResult;
	HANDLE searchHandle = FindFirstFile(findString, &findResult);
	do
	{
		if ((findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			wchar_t baseName[1024];
			_splitpath(findResult.cFileName, nullptr, nullptr, baseName, nullptr);
			FullPathFileName pakName;
			pakName.init(missionPath, baseName, ".pak");
			FullPathFileName fitName;
			fitName.init(missionPath, baseName, ".fit");
			if (fileExists(pakName))
			{
				// Force Attributes to RW!
				SetFileAttributes(pakName, FILE_ATTRIBUTE_NORMAL);
				SetFileAttributes(fitName, FILE_ATTRIBUTE_NORMAL);
				int32_t result = EditorData::initTerrainFromPCV(pakName);
				if (result)
					data.save(pakName);
				printf("Resaved Map %s", pakName);
				// Force Back to RO!
				// Leave RW per bug 1252
				//				SetFileAttributes(pakName,FILE_ATTRIBUTE_READONLY);
				//				SetFileAttributes(fitName,FILE_ATTRIBUTE_READONLY);
			}
		}
	} while (FindNextFile(searchHandle, &findResult) != 0);
	FindClose(searchHandle);
	// ARM
	CoUninitialize();
}

void Editor::render()
{
	if (eye)
		eye->render();
}

void Editor::update()
{
	if (windowSizeChanged)
	{
		windowSizeChanged = false;
		gos_SetScreenMode(g_newwidth, g_newheight);
	}
	// interMgr.update();
	mcTextureManager->clearArrays();
	eye->update();
	if (land->terrainTextures2 && !(land->terrainTextures2->colorMapStarted) && (EditorInterface::instance()))
	{
		EditorInterface::instance()->SetBusyMode(false /*no redraw*/);
		land->update();
		EditorInterface::instance()->UnsetBusyMode();
	}
	else
	{
		land->update();
	}
	land->clearObjBlocksActive();
	land->geometry();
	EditorInterface::instance()->update();
	EditorObjectMgr::instance()->update();
}

//--------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(EditorInterface, CWnd)
	//{{AFX_MSG_MAP(EditorInterface)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SYSKEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_REFRESHTACMAP, OnViewRefreshtacmap)
	ON_UPDATE_COMMAND_UI(ID_MISSION_PLAYER_PLAYER3, OnUpdateMissionPlayerPlayer3)
	ON_UPDATE_COMMAND_UI(ID_MISSION_PLAYER_PLAYER4, OnUpdateMissionPlayerPlayer4)
	ON_UPDATE_COMMAND_UI(ID_MISSION_PLAYER_PLAYER5, OnUpdateMissionPlayerPlayer5)
	ON_UPDATE_COMMAND_UI(ID_MISSION_PLAYER_PLAYER6, OnUpdateMissionPlayerPlayer6)
	ON_UPDATE_COMMAND_UI(ID_MISSION_PLAYER_PLAYER7, OnUpdateMissionPlayerPlayer7)
	ON_UPDATE_COMMAND_UI(ID_MISSION_PLAYER_PLAYER8, OnUpdateMissionPlayerPlayer8)
	ON_UPDATE_COMMAND_UI(ID_MISSION_TEAM_TEAM3, OnUpdateMissionTeamTeam3)
	ON_UPDATE_COMMAND_UI(ID_MISSION_TEAM_TEAM4, OnUpdateMissionTeamTeam4)
	ON_UPDATE_COMMAND_UI(ID_MISSION_TEAM_TEAM5, OnUpdateMissionTeamTeam5)
	ON_UPDATE_COMMAND_UI(ID_MISSION_TEAM_TEAM6, OnUpdateMissionTeamTeam6)
	ON_UPDATE_COMMAND_UI(ID_MISSION_TEAM_TEAM7, OnUpdateMissionTeamTeam7)
	ON_UPDATE_COMMAND_UI(ID_MISSION_TEAM_TEAM8, OnUpdateMissionTeamTeam8)
	ON_WM_DESTROY()
	ON_COMMAND(ID_FOREST_TOOL, OnForestTool)
	ON_COMMAND(ID_OTHER_EDITFORESTS, OnOtherEditforests)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHICCAMERA, OnViewOrthographiccamera)
	ON_COMMAND(ID_VIEW_SHOWPASSABILITYMAP, OnViewShowpassabilitymap)
	ON_WM_MBUTTONUP()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(0, 0xffff, UpdateButton)
	ON_COMMAND_RANGE(0, 0xffff, OnCommand)
END_MESSAGE_MAP()

EditorInterface::EditorInterface()
{
	bThisIsInitialized = false;
	painting = false;
	curBrush = nullptr;
	selecting = true;
	realRotation = 0.0;
	currentBrushID = IDS_SELECT;
	currentBrushMenuID = ID_OTHER_SELECT;
	smoothRadius = 2;
	dragging = false;
	prevBrush = nullptr;
	prevSelecting = false;
	highlighted = false;
	prevPainting = false;
	prevDragging = false;
	gosASSERT(!s_instance);
	s_instance = this;
	bSmooth = false;
	lastKey = -1;
	lastClickPos.x = lastClickPos.y = lastClickPos.z = 0.;
	hCursor = 0;
	bObjectSelectOnlyMode = false;
	menus = nullptr;
	m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
	m_hSplashBitMap = nullptr;
	m_hBusyCursor = nullptr;
	m_AppIsBusy = 0;
	m_bInRunGameOSLogicCall = false;
	rightDrag = 0;
}

EditorInterface::~EditorInterface()
{
	if (bThisIsInitialized)
	{
		terminate();
	}
	if (m_hBusyCursor)
	{
		/*this is done here because m_hBusyCursor is sometimes used before
		EditorInterface is "initialized" and after it is "terminated" */
		DestroyCursor(m_hBusyCursor);
		m_hBusyCursor = nullptr;
	}
	if (m_hSplashBitMap)
	{
		DeleteObject(m_hSplashBitMap);
		m_hSplashBitMap = nullptr;
	}
	s_instance = nullptr;
}

void EditorInterface::terminate()
{
	if (bThisIsInitialized)
	{
		bThisIsInitialized = false;
		if (curBrush)
			delete curBrush;
		curBrush = nullptr;
		if (EditorObjectMgr::instance())
		{
			int32_t count = EditorObjectMgr::instance()->getBuildingGroupCount();
			for (size_t i = 0; i < count + 1; ++i)
			{
				delete menus[i];
				menus[i] = nullptr;
			}
		}
		free(menus);
		menus = nullptr;
		objectivesEditState.Clear();
	}
}

void EditorInterface::init(std::wstring_view fileName)
{
	SetBusyMode(false /*no redraw*/);
	FitIniFile loader;
	loader.open(const_cast<std::wstring_view>(fileName));
	int32_t result = loader.seekBlock("CameraData");
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("ScrollIncrement", scrollInc);
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("RotationIncrement", rotationInc);
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("ZoomIncrement", zoomInc);
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("ScrollLeft", screenScrollLeft);
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("ScrollRight", screenScrollRight);
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("ScrollUp", screenScrollUp);
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("ScrollDown", screenScrollDown);
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("BaseFrameLength", baseFrameLength);
	gosASSERT(result == NO_ERROR);
	result = loader.readIdFloat("RotationDegPerSec", degPerSecRot);
	gosASSERT(result == NO_ERROR);
	float missionDragThreshold;
	result = loader.readIdFloat("MouseDragThreshold", missionDragThreshold);
	gosASSERT(result == NO_ERROR);
	float missionDblClkThreshold;
	result = loader.readIdFloat("MouseDoubleClickThreshold", missionDblClkThreshold);
	gosASSERT(result == NO_ERROR);
	addBuildingsToNewMenu();
	curBrush = new SelectionBrush(false, -1);
	ChangeCursor(IDC_MC2ARROW);
	syncScrollBars();
	initTacMap();
	UnsetBusyMode();
	bThisIsInitialized = true;
}

void EditorInterface::addBuildingsToNewMenu()
{
	EditorObjectMgr* pMgr = EditorObjectMgr::instance();
	wchar_t BuildingHeader[64];
	wchar_t TerrainHeader[64];
	cLoadString(IDS_BUILDINGS, BuildingHeader, 64, gameResourceHandle);
	cLoadString(IDS_TERRAINS, TerrainHeader, 64, gameResourceHandle);
	CMenu* pMenu = new CMenu;
	pMenu->CreatePopupMenu();
	// now i need to add the buildings to the toolbars/menus
	int32_t groupCount = pMgr->getBuildingGroupCount();
	std::wstring_view* pNames = (std::wstring_view*)malloc(sizeof(std::wstring_view) * groupCount);
	pMgr->getBuildingGroupNames(pNames, groupCount);
	menus = (CMenu**)malloc(sizeof(CMenu*) * (groupCount + 2));
	menus[0] = pMenu;
	int32_t id = IDS_OBJECT_200;
	for (size_t i = 0; i < groupCount; ++i)
	{
		CMenu* pChildMenu = new CMenu;
		pChildMenu->CreatePopupMenu();
		menus[i + 1] = pChildMenu;
		// now add sub items to the group
		int32_t buildingCount = pMgr->getNumberBuildingsInGroup(i);
		std::wstring_view* pBuildingNames = (std::wstring_view*)malloc(sizeof(std::wstring_view) * buildingCount);
		pMgr->getBuildingNamesInGroup(i, pBuildingNames, buildingCount);
		for (size_t j = 0; j < buildingCount; ++j, ++id)
		{
			CString oldName;
			bool bAdded = 0;
			int32_t count = pChildMenu->GetMenuItemCount();
			for (size_t k = 0; k < count; ++k)
			{
				pChildMenu->GetMenuString(k, oldName, MF_BYPOSITION);
				if (oldName > pBuildingNames[j])
				{
					pChildMenu->InsertMenu(k, MF_BYPOSITION, id, pBuildingNames[j]);
					bAdded = 1;
					break;
				}
			}
			if (!bAdded)
				pChildMenu->AppendMenu(MF_STRING, id, pBuildingNames[j]);
		}
		CString oldName;
		bool bAdded = 0;
		int32_t count = pMenu->GetMenuItemCount();
		for (size_t k = 0; k < count; ++k)
		{
			pMenu->GetMenuString(k, oldName, MF_BYPOSITION);
			if (oldName > pNames[i])
			{
				pMenu->InsertMenu(
					k, MF_BYPOSITION | MF_POPUP, (uint32_t)pChildMenu->m_hMenu, pNames[i]);
				bAdded = 1;
				break;
			}
		}
		if (!bAdded)
			pMenu->AppendMenu(MF_POPUP, (uint32_t)pChildMenu->m_hMenu, pNames[i]);
		free(pBuildingNames);
	}
	AfxGetMainWnd()->GetMenu()->InsertMenu(
		5, MF_BYPOSITION | MF_POPUP, (uint32_t)pMenu->m_hMenu, BuildingHeader);
	CMenu* pChildMenu = new CMenu;
	pChildMenu->CreatePopupMenu();
	menus[i + 1] = pChildMenu;
	int32_t numTerrains = 0;
	numTerrains = land->terrainTextures->getNumTypes();
	for (i = groupCount; i < numTerrains + groupCount; i++)
	{
		wchar_t buffer[256];
		int32_t nameID = land->terrainTextures->getTextureNameID(i - groupCount);
		bool continueFlag = true;
		switch (nameID)
		{
		case 10020 /*Cement 1*/:
		case 10021 /*Cement 1 (angled)*/:
		case 10022 /*Cement 2*/:
		case 10023 /*Cement 2 (angled)*/:
		case 10024 /*Cement 3*/:
		case 10025 /*Cement 3 (angled)*/:
		case 10026 /*Cement 1 (crumbled)*/:
		case 10027 /*Cement 2 (crumbled)*/:
		case 10069 /*Cement 3 (crumbled)*/:
			continueFlag = false;
			break;
		};
		if (continueFlag)
			continue;
		cLoadString(land->terrainTextures->getTextureNameID(i - groupCount), buffer, 256);
		int32_t count = pChildMenu->GetMenuItemCount();
		bool bPlaced = 0;
		CString newStr(buffer);
		for (size_t j = 0; j < count; ++j)
		{
			CString tmp;
			pChildMenu->GetMenuString(j, tmp, MF_BYPOSITION);
			if (tmp > newStr)
			{
				pChildMenu->InsertMenu(
					j, MF_BYPOSITION, ID_TERRAINS_BLUEWATER + i - groupCount, buffer);
				bPlaced = 1;
				break;
			}
		}
		if (!bPlaced)
			pChildMenu->AppendMenu(MF_STRING, ID_TERRAINS_BLUEWATER + i - groupCount, buffer);
	}
	AfxGetMainWnd()->GetMenu()->InsertMenu(
		4, MF_BYPOSITION | MF_POPUP, (uint32_t)pChildMenu->m_hMenu, TerrainHeader);
	AfxGetMainWnd()->DrawMenuBar();
	free(pNames);
}

//--------------------------------------------------------------------------------------
void EditorInterface::handleNewMenuMessage(int32_t specificMessage)
{
	if (!eye || !eye->active)
		return;
	if (EditorInterface::instance()->ObjectSelectOnlyMode())
	{
		PlaySound("SystemDefault", nullptr, SND_ASYNC);
		return;
	}
	// special check for building range
	if (specificMessage >= IDS_OBJECT_200 && specificMessage <= 30800)
	{
		paintBuildings(specificMessage);
	}
	if (specificMessage >= ID_TERRAINS_BLUEWATER - 1 && specificMessage <= ID_TERRAINS_BLUEWATER + 255)
	{
		PaintTerrain(specificMessage - ID_TERRAINS_BLUEWATER);
		return;
	}
	else if (specificMessage >= ID_ALIGNMENT_TEAM1 && specificMessage <= ID_ALIGNMENT_TEAM1 + 9)
	{
		Alignment(specificMessage);
	}
	else
	{
		switch (specificMessage)
		{
		case ID_FILE_ASSIGNHEIGHTMAP:
		case ID_FOG:
		case ID_OTHER_RELOADBASETEXTURE:
		case ID_OTHER_SETBASETEXTURENAME:
		case ID_LIGHT:
		case ID_WAVES:
		case ID_OTHER_ASSIGNHEIGHT:
		case ID_MISSION_SETTINGS:
		case ID_MISSION_PLAYER_PLAYER1:
		case ID_MISSION_PLAYER_PLAYER2:
		case ID_MISSION_PLAYER_PLAYER3:
		case ID_MISSION_PLAYER_PLAYER4:
		case ID_MISSION_PLAYER_PLAYER5:
		case ID_MISSION_PLAYER_PLAYER6:
		case ID_MISSION_PLAYER_PLAYER7:
		case ID_MISSION_PLAYER_PLAYER8:
		case ID_OTHER_SELECTDETAILTEXTURE:
		case ID_OTHER_SELECTWATERTEXTURE:
		case ID_OTHER_SELECTWATERDETAILTEXTURE:
		case ID_OTHER_TEXTURETILINGFACTORS:
		case ID_SKY_SKY1:
		case ID_SKY_SKY2:
		case ID_SKY_SKY3:
		case ID_SKY_SKY4:
		case ID_SKY_SKY5:
		case ID_SKY_SKY6:
		case ID_SKY_SKY7:
		case ID_SKY_SKY8:
		case ID_SKY_SKY9:
		case ID_SKY_SKY10:
		case ID_SKY_SKY11:
		case ID_SKY_SKY12:
		case ID_SKY_SKY13:
		case ID_SKY_SKY14:
		case ID_SKY_SKY15:
		case ID_SKY_SKY16:
		case ID_SKY_SKY17:
		case ID_SKY_SKY18:
		case ID_SKY_SKY19:
		case ID_SKY_SKY20:
		case ID_SKY_SKY21:
		case ID_OTHER_REFRACTALIZETERRAIN:
		/*not sure these are still used*/
		case ID_PURGE_TRANSITIONS:
		case ID_SHOW_TRANSITIONS:
		case ID_DROPZONES_ADD:
		case ID_DROPZONES_VTOL:
		case IDS_SAVE_CAMERAS:
		{
			if (EditorData::instance)
			{
				EditorData::instance->MissionNeedsSaving(true);
			}
		}
		break;
		}
		switch (specificMessage)
		{
		case ID_FILE_NEW2:
		{
			New();
		}
		break;
		case ID_FILE_OPEN2:
		{
			FileOpen();
		}
		break;
		case ID_FILE_SAVEAS:
		{
			SaveAs();
		}
		break;
		case ID_FILE_SAVE:
		{
			Save();
		}
		break;
		case ID_FILE_QUICKSAVE:
		{
			QuickSave();
		}
		break;
		case ID_FILE_ASSIGNHEIGHTMAP:
		{
			NewheightMap();
		}
		break;
		case ID_FILE_EXIT:
		{
			Quit();
		}
		break;
		case ID_EDIT_UNDO2:
		{
			Undo();
		}
		break;
		case ID_EDIT_REDO2:
		{
			Redo();
		}
		break;
		case ID_EDIT_COPY2:
		{
			;
		}
		break;
		case ID_OVERLAYS_DIRTROAD:
		{
			PaintDirtRoad();
		}
		break;
		case ID_OVERLAYS_PAEVEDROAD:
		{
			PaintPaved();
		}
		break;
		case ID_OVERLAYS_ROUGH:
		{
			PaintRocks();
		}
		break;
		case ID_OVERLAYS_TWOLANEDIRTROAD:
		{
			PaintTwoLaneDirtRoad();
		}
		break;
		case ID_OVERLAYS_DAMAGEDROAD:
		{
			PaintDamagedRoad();
		}
		break;
		case ID_OVERLAYS_RUNWAY:
		{
			PaintRunway();
		}
		break;
		case ID_OVERLAYS_BRIDGE:
		{
			PaintBridge();
		}
		break;
		case ID_OVERLAYS_DAMAGEDBRIDGE:
		{
			PaintDamagedBridge();
		}
		break;
		case ID_OTHER_ERASE:
		{
			Erase();
		}
		break;
		case ID_OTHER_SELECT:
		{
			Select();
		}
		break;
		case ID_OTHER_FLATTEN:
		{
			Flatten();
		}
		break;
		case IDS_SELECT_SLOPES:
			SelectSlopes();
			break;
		case IDS_SELECT_ALTITUDE:
			SelectAltitude();
			break;
		case ID_SELECT_TERRAIN_TYPE:
			SelectTerrainType();
			break;
		case ID_FOG:
			Fog();
			break;
		case ID_OTHER_RELOADBASETEXTURE:
			ReloadBaseTexture();
			break;
		case ID_OTHER_SETBASETEXTURENAME:
			SetBaseTexture();
			break;
		case ID_PURGE_TRANSITIONS:
			PurgeTransitions();
			break;
		case ID_SHOW_TRANSITIONS:
			ShowTransitions();
			break;
		case ID_LIGHT:
			Light();
			break;
		case IDS_SAVE_CAMERAS:
			SaveCameras();
			break;
		case ID_WAVES:
			Waves();
			break;
		case ID_DRAGSMOOTH:
			DragSmooth();
			break;
		case ID_DRAGNORMAL:
			DragRough();
			break;
		case ID_OTHER_ASSIGNHEIGHT:
			AssignElevation();
			break;
		case ID_SMOOTH_RADIUS:
			SmoothRadius();
			break;
		case ID_SAVE_HEIGHT_MAP:
			SaveheightMap();
			break;
		case ID_MISSION_SETTINGS:
			MissionSettings();
			break;
		case ID_MISSION_TEAM_TEAM1:
			Team(0);
			break;
		case ID_MISSION_TEAM_TEAM2:
			Team(1);
			break;
		case ID_MISSION_TEAM_TEAM3:
			Team(2);
			break;
		case ID_MISSION_TEAM_TEAM4:
			Team(3);
			break;
		case ID_MISSION_TEAM_TEAM5:
			Team(4);
			break;
		case ID_MISSION_TEAM_TEAM6:
			Team(5);
			break;
		case ID_MISSION_TEAM_TEAM7:
			Team(6);
			break;
		case ID_MISSION_TEAM_TEAM8:
			Team(7);
			break;
		case ID_MISSION_PLAYER_PLAYER1:
			Player(0);
			break;
		case ID_MISSION_PLAYER_PLAYER2:
			Player(1);
			break;
		case ID_MISSION_PLAYER_PLAYER3:
			Player(2);
			break;
		case ID_MISSION_PLAYER_PLAYER4:
			Player(3);
			break;
		case ID_MISSION_PLAYER_PLAYER5:
			Player(4);
			break;
		case ID_MISSION_PLAYER_PLAYER6:
			Player(5);
			break;
		case ID_MISSION_PLAYER_PLAYER7:
			Player(6);
			break;
		case ID_MISSION_PLAYER_PLAYER8:
			Player(7);
			break;
		case ID_OTHER_DAMAGE:
			Damage(1);
			break;
		case ID_OTHER_REPAIR:
			Damage(0);
			break;
		case ID_OTHER_LINK:
			Link(true);
			break;
		case ID_OTHER_UNLINK:
			Link(false);
			break;
		case ID_OTHER_LAYMINES:
			LayMines();
			break;
		case ID_OTHER_SELECTDETAILTEXTURE:
			SelectDetailTexture();
			break;
		case ID_OTHER_SELECTWATERTEXTURE:
			SelectWaterTexture();
			break;
		case ID_OTHER_SELECTWATERDETAILTEXTURE:
			SelectWaterDetailTexture();
			break;
		case ID_OTHER_TEXTURETILINGFACTORS:
			TextureTilingFactors();
			break;
		case ID_DROPZONES_ADD:
			DropZone(false);
			break;
		case ID_DROPZONES_VTOL:
			DropZone(true);
			break;
		case ID_UNITSETTINGS:
			UnitSettings();
			break;
		case ID_SKY_SKY1:
			SetSky(1);
			break;
		case ID_SKY_SKY2:
			SetSky(2);
			break;
		case ID_SKY_SKY3:
			SetSky(3);
			break;
		case ID_SKY_SKY4:
			SetSky(4);
			break;
		case ID_SKY_SKY5:
			SetSky(5);
			break;
		case ID_SKY_SKY6:
			SetSky(6);
			break;
		case ID_SKY_SKY7:
			SetSky(7);
			break;
		case ID_SKY_SKY8:
			SetSky(8);
			break;
		case ID_SKY_SKY9:
			SetSky(9);
			break;
		case ID_SKY_SKY10:
			SetSky(10);
			break;
		case ID_SKY_SKY11:
			SetSky(11);
			break;
		case ID_SKY_SKY12:
			SetSky(12);
			break;
		case ID_SKY_SKY13:
			SetSky(13);
			break;
		case ID_SKY_SKY14:
			SetSky(14);
			break;
		case ID_SKY_SKY15:
			SetSky(15);
			break;
		case ID_SKY_SKY16:
			SetSky(16);
			break;
		case ID_SKY_SKY17:
			SetSky(17);
			break;
		case ID_SKY_SKY18:
			SetSky(18);
			break;
		case ID_SKY_SKY19:
			SetSky(19);
			break;
		case ID_SKY_SKY20:
			SetSky(20);
			break;
		case ID_SKY_SKY21:
			SetSky(21);
			break;
		case ID_OTHER_REFRACTALIZETERRAIN:
			RefractalizeTerrain(1);
			break;
		case ID_CAMPAIGN_EDITOR:
			CampaignEditor();
			break;
		}
	}
}

//--------------------------------------------------------------------------------------
int32_t
EditorInterface::Quit()
{
	int32_t res = PromptAndSaveIfNecessary();
	if (IDCANCEL != res)
	{
		SetBusyMode();
		gos_TerminateApplication();
		PostQuitMessage(0);
		UnsetBusyMode();
	}
	return 1;
}

//--------------------------------------------------------------------------------------
int32_t
EditorInterface::PromptAndSaveIfNecessary()
{
	int32_t res = IDNO;
	bool endFlag = false;
	while (!endFlag)
	{
		endFlag = true;
		if (EditorInterface::instance() && EditorInterface::instance()->ThisIsInitialized() && EditorData::instance)
		{
			if (EditorInterface::instance()->undoMgr.ThereHasBeenANetChangeFromWhenLastSaved())
			{
				res = AfxMessageBox(IDS_DO_YOU_WANT_TO_SAVE_YOUR_CHANGES, MB_YESNOCANCEL);
			}
			else if (EditorData::instance->MissionNeedsSaving())
			{
				res = AfxMessageBox(IDS_DO_YOU_WANT_TO_SAVE_THIS_MISSION, MB_YESNOCANCEL);
			}
		}
		if (IDYES == res)
		{
			SetBusyMode();
			int32_t saveRes = EditorInterface::instance()->Save();
			UnsetBusyMode();
			if (IDCANCEL == saveRes)
			{
				endFlag = false;
			}
		}
	}
	return res;
}

//--------------------------------------------------------------------------------------
int32_t
EditorInterface::New()
{
	int32_t res = PromptAndSaveIfNecessary();
	if (IDCANCEL == res)
	{
		return false;
	}
	bool bOK = false;
	while (!bOK)
	{
		TerrainDlg dlg;
		dlg.terrain = 0;
		if (IDOK == dlg.DoModal())
		{
			MapSizeDlg msdlg;
			msdlg.mapSize = 0;
			if (IDOK == msdlg.DoModal())
			{
				SetBusyMode();
				eye->reset();
				bOK = EditorData::initTerrainFromTGA(msdlg.mapSize, 0, 0, dlg.terrain);
				UnsetBusyMode();
			}
			else
			{
				return true;
			}
		}
		else // They pressed cancel.  Let 'em cancel, dammit!
		{
			return true;
		}
	}
	tacMap.UpdateMap();
	syncScrollBars();
	return true;
}

//--------------------------------------------------------------------------------------
int32_t
EditorInterface::FileOpen()
{
	int32_t res = PromptAndSaveIfNecessary();
	if (IDCANCEL == res)
	{
		return false;
	}
	CFileDialog fileDlg(
		1, "pak", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szPAKFilter);
	fileDlg.m_ofn.lpstrInitialDir = missionPath;
	if (IDOK == fileDlg.DoModal())
	{
		SetBusyMode();
		std::wstring_view pFile = fileDlg.m_ofn.lpstrFile;
		EditorData::initTerrainFromPCV(pFile);
		tacMap.UpdateMap();
		syncScrollBars();
		PlaySound("SystemDefault", nullptr, SND_ASYNC);
		UnsetBusyMode();
	}
	return true;
}

//--------------------------------------------------------------------------------------

void EditorInterface::handleLeftButtonDown(int32_t PosX, int32_t PosY)
{
	if (!eye || !eye->active)
		return;
	Stuff::Vector3D vector;
	Stuff::Vector2DOf<int32_t> v2(PosX, PosY);
	eye->inverseProject(v2, vector);
	if (curBrush)
	{
		painting = true;
		curBrush->beginPaint();
		if (curBrush->canPaint(vector, PosX, PosY, 0))
			curBrush->paint(vector, PosX, PosY);
	}
	lastClickPos = vector;
}

void EditorInterface::handleMouseMove(int32_t PosX, int32_t PosY)
{
	if (!eye || !eye->active)
		return;
	Stuff::Vector3D vector;
	Stuff::Vector2DOf<int32_t> v2(PosX, PosY);
	eye->inverseProject(v2, vector);
	if (curBrush && (painting))
	{
		if (curBrush->canPaint(vector, PosX, PosY, 0))
			curBrush->paint(vector, PosX, PosY);
	}
	//------------------------------------------------
	// Right drag is camera rotation and tilt now.
	if (rightDrag)
	{
		int32_t mouseXDelta = PosX - lastX;
		float actualRot = rotationInc * 0.1f * abs(mouseXDelta);
		if (mouseXDelta > 0)
		{
			eye->rotateRight(actualRot);
		}
		else if (mouseXDelta < 0)
		{
			eye->rotateLeft(actualRot);
		}
		int32_t mouseYDelta = PosY - lastY;
		float actualTilt = rotationInc * 0.1f * abs(mouseYDelta);
		if (mouseYDelta > 0)
		{
			eye->tiltDown(actualTilt);
		}
		else if (mouseYDelta < 0)
		{
			eye->tiltUp(actualTilt);
		}
		lastX = PosX;
		lastY = PosY;
		SafeRunGameOSLogic();
		tacMap.RedrawWindow();
		syncScrollBars();
	}
	wchar_t buffer2[256];
	sprintf(buffer2, "%.3f, %.3f", vector.x, vector.y);
	// need to put this value in the appropriate place.
	((MainFrame*)AfxGetMainWnd())
		->m_wndDlgBar.GetDlgItem(IDC_COORDINATES_EDIT)
		->SetWindowText(buffer2);
	wchar_t buffer3[256];
	sprintf(buffer3, "%.3f", vector.z);
	// need to put this value in the appropriate place.
	((MainFrame*)AfxGetMainWnd())
		->m_wndDlgBar.GetDlgItem(IDC_ALTITUDE_EDIT)
		->SetWindowText(buffer3);
	vector -= lastClickPos;
	float distance = vector.GetLength();
	wchar_t buffer[256];
	sprintf(buffer, "%.3f", distance);
	// need to put this value in the appropriate place.
	((MainFrame*)AfxGetMainWnd())->m_wndDlgBar.GetDlgItem(IDC_DISTANCE_EDIT)->SetWindowText(buffer);
	// IF there is a selected object, find distance to it from camera.
	float eyeDistance = 0.0f;
	int32_t selectionCount = EditorObjectMgr::instance()->getSelectionCount();
	if (selectionCount)
	{
		EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjectsList =
			EditorObjectMgr::instance()->getSelectedObjectList();
		EditorObjectMgr::EDITOR_OBJECT_LIST::EIterator it = selectedObjectsList.Begin();
		const EditorObject* pInfo = (*it);
		if (pInfo)
		{
			Stuff::Point3D eyePosition(eye->getCameraOrigin());
			Stuff::Point3D objPosition;
			objPosition.x = -pInfo->appearance()->position.x;
			objPosition.y = pInfo->appearance()->position.z;
			objPosition.z = pInfo->appearance()->position.y;
			Stuff::Vector3D Distance;
			Distance.Subtract(objPosition, eyePosition);
			eyeDistance = Distance.GetApproximateLength();
		}
	}
	// need to put this value in the appropriate place.
	sprintf(buffer, "%.3f", eyeDistance);
	((MainFrame*)AfxGetMainWnd())
		->m_wndDlgBar.GetDlgItem(IDC_OBJDISTANCEEDIT)
		->SetWindowText(buffer);
}

void EditorInterface::handleLeftButtonUp(int32_t PosX, int32_t PosY)
{
	if (!eye || !eye->active)
		return;
	if (curBrush && painting)
	{
		painting = false;
		Action* pAction = curBrush->endPaint();
		if (pAction)
			undoMgr.AddAction(pAction);
	}
}

//--------------------------------------------------------------------------------------
// THE EDITOR CLASS, kind of like the mission in MCommander
void EditorInterface::handleKeyDown(int32_t Key)
{
	if (!eye || !eye->active)
		return;
	if (DebuggerActive)
		return;
	//----------------------
	// Adjust for frameRate
	float frameFactor = frameLength / baseFrameLength;
	float scrollFactor = scrollInc / eye->getScaleFactor() * frameFactor;
	bool shiftDn = GetAsyncKeyState(KEY_LSHIFT) ? true : false;
	bool ctrlDn = GetAsyncKeyState(KEY_LCONTROL) ? true : false;
	int32_t altDn = GetAsyncKeyState(KEY_LMENU) ? true : false;
	if (Key == KEY_SPACE)
	{
		if (!dragging)
		{
			prevBrush = curBrush;
			prevSelecting = selecting;
			prevPainting = painting;
			prevDragging = dragging;
			curBrush = new DragTool;
			oldCursor = curCursorID;
			ChangeCursor(IDC_HAND_MC);
			dragging = true;
		}
	}
	if (currentBrushID >= IDS_OBJECT_200 && currentBrushID <= IDS_OBJECT_200 + 600)
	{
		if (Key == KEY_LBRACKET)
		{
			((BuildingBrush*)curBrush)->rotateBrush(1);
		}
		else if (Key == KEY_RBRACKET)
		{
			((BuildingBrush*)curBrush)->rotateBrush(-1);
		}
	}
	else
	{
		if (Key == KEY_LBRACKET)
		{
			rotateSelectedObjects(1);
		}
		else if (Key == KEY_RBRACKET)
		{
			rotateSelectedObjects(-1);
		}
	}
	if (lastKey != Key) // only want to do these if something has changed
	{
		if (Key == KEY_ESCAPE)
		{
			Select();
			DragRough();
			if (EditorInterface::instance()->ObjectSelectOnlyMode())
			{
				ReleaseCapture();
				EditorInterface::instance()->Team(
					EditorInterface::instance()->objectivesEditState.alignment);
			}
		}
		if (GetAsyncKeyState(KEY_T) && ctrlDn && altDn && !shiftDn)
		{
			drawTerrainTiles ^= TRUE;
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_O) && ctrlDn && altDn && !shiftDn)
		{
			drawTerrainOverlays ^= TRUE;
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_S) && ctrlDn && altDn && !shiftDn)
		{
			renderObjects ^= TRUE;
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_G) && ctrlDn && altDn && !shiftDn)
		{
			// drawTerrainGrid ^= TRUE;
			OnViewShowpassabilitymap();
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_L) && ctrlDn && altDn && !shiftDn)
		{
			drawLOSGrid ^= TRUE;
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_Q) && ctrlDn && altDn && !shiftDn)
		{
			land->reCalcLight(true);
		}
		if (GetAsyncKeyState(KEY_C) && ctrlDn && altDn && !shiftDn)
		{
			useClouds ^= true;
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_F) && ctrlDn && altDn && !shiftDn)
		{
			useFog ^= true;
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_P) && ctrlDn && altDn && !shiftDn)
		{
			// eye->usePerspective ^= true;
			OnViewOrthographiccamera();
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_R) && ctrlDn && altDn && !shiftDn)
		{
			reloadBounds = true;
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_V) && ctrlDn && altDn && !shiftDn)
		{
			useWaterInterestTexture ^= true;
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_W) && ctrlDn && altDn && !shiftDn)
		{
			Terrain::mapData->recalcWater();
			terrainLineChanged = turn;
		}
		if (GetAsyncKeyState(KEY_D) && ctrlDn && altDn && !shiftDn)
		{
			useShadows ^= true;
			terrainLineChanged = turn;
		}
	}
	//------------------------
	// Keyboard Checks First
	bool scrollUp = (GetAsyncKeyState(KEY_UP) && !shiftDn && !ctrlDn && !altDn);
	bool scrollDn = (GetAsyncKeyState(KEY_DOWN) && !shiftDn && !ctrlDn && !altDn);
	bool scrollLf = (GetAsyncKeyState(KEY_LEFT) && !shiftDn && !ctrlDn && !altDn);
	bool scrollRt = (GetAsyncKeyState(KEY_RIGHT) && !shiftDn && !ctrlDn && !altDn);
	bool zoomOut = (GetAsyncKeyState(KEY_SUBTRACT) && !shiftDn && !ctrlDn && !altDn);
	bool zoomIn = (GetAsyncKeyState(KEY_ADD) && !shiftDn && !ctrlDn && !altDn);
	bool rotateLf = (GetAsyncKeyState(KEY_LEFT) && shiftDn && !ctrlDn && !altDn);
	bool rotateRt = (GetAsyncKeyState(KEY_RIGHT) && shiftDn && !ctrlDn && !altDn);
	bool rotateLightLf = (GetAsyncKeyState(KEY_LEFT) && !shiftDn && ctrlDn && !altDn);
	bool rotateLightRt = (GetAsyncKeyState(KEY_RIGHT) && !shiftDn && ctrlDn && !altDn);
	bool rotateLightUp = (GetAsyncKeyState(KEY_UP) && !shiftDn && ctrlDn && !altDn);
	bool rotateLightDn = (GetAsyncKeyState(KEY_DOWN) && !shiftDn && ctrlDn && !altDn);
	bool tiltUp = (GetAsyncKeyState(KEY_DOWN) && shiftDn && !ctrlDn && !altDn);
	bool tiltDn = (GetAsyncKeyState(KEY_UP) && shiftDn && !ctrlDn && !altDn);
	bool tiltNormal = (GetAsyncKeyState(KEY_HOME) && !shiftDn && !ctrlDn && !altDn);
	// Update the Camera based on Input
	if (scrollLf)
	{
		eye->moveLeft(scrollFactor);
		syncScrollBars();
	}
	if (scrollRt)
	{
		eye->moveRight(scrollFactor);
		syncScrollBars();
	}
	if (scrollDn)
	{
		eye->moveDown(scrollFactor);
		syncScrollBars();
	}
	if (scrollUp)
	{
		eye->moveUp(scrollFactor);
		syncScrollBars();
	}
	if (zoomOut)
	{
		eye->ZoomOut(zoomInc * frameFactor * eye->getScaleFactor());
		if (eye->getScaleFactor() <= 0.3)
			renderTrees = false;
		syncScrollBars();
		tacMap.RedrawWindow();
	}
	if (zoomIn)
	{
		eye->ZoomIn(zoomInc * frameFactor * eye->getScaleFactor());
		if (eye->getScaleFactor() > 0.3)
			renderTrees = true;
		syncScrollBars();
		tacMap.RedrawWindow();
	}
	if (tiltDn)
	{
		eye->tiltDown(scrollFactor * frameFactor * 10.0f);
		syncScrollBars();
		tacMap.RedrawWindow();
	}
	if (tiltUp)
	{
		eye->tiltUp(scrollFactor * frameFactor * 10.0f);
		syncScrollBars();
		tacMap.RedrawWindow();
	}
	if (tiltNormal)
	{
		eye->tiltNormal();
		syncScrollBars();
		tacMap.RedrawWindow();
	}
	if (rotateLf)
	{
		realRotation += degPerSecRot * frameFactor;
		if (realRotation >= rotationInc)
		{
			eye->rotateLeft(rotationInc);
			realRotation = 0;
		}
		syncScrollBars();
	}
	if (rotateRt)
	{
		realRotation -= degPerSecRot * frameFactor;
		if (realRotation <= -rotationInc)
		{
			eye->rotateRight(rotationInc);
			realRotation = 0;
		}
		syncScrollBars();
	}
	//------------------------------------------------
	// TEST LIGHTING MODEL
	if (rotateLightRt)
	{
		eye->rotateLightRight(rotationInc);
	}
	if (rotateLightLf)
	{
		eye->rotateLightLeft(rotationInc);
	}
	if (rotateLightUp)
	{
		eye->rotateLightUp(rotationInc);
	}
	if (rotateLightDn)
	{
		eye->rotateLightDown(rotationInc);
	}
	lastKey = Key;
	if (scrollUp || scrollDn || scrollLf || scrollRt || zoomOut || zoomIn || rotateLf || rotateRt || rotateLightLf || rotateLightRt || rotateLightUp || rotateLightDn || tiltUp || tiltDn || tiltNormal)
	{
		SafeRunGameOSLogic();
	}
	//------------------------------------------------
	// IF there is a selected object, find distance to it from camera.
	wchar_t buffer[256];
	float eyeDistance = 0.0f;
	int32_t selectionCount = EditorObjectMgr::instance()->getSelectionCount();
	if (selectionCount)
	{
		EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjectsList =
			EditorObjectMgr::instance()->getSelectedObjectList();
		EditorObjectMgr::EDITOR_OBJECT_LIST::EIterator it = selectedObjectsList.Begin();
		const EditorObject* pInfo = (*it);
		if (pInfo)
		{
			Stuff::Point3D eyePosition(eye->getCameraOrigin());
			Stuff::Point3D objPosition;
			objPosition.x = -pInfo->appearance()->position.x;
			objPosition.y = pInfo->appearance()->position.z;
			objPosition.z = pInfo->appearance()->position.y;
			Stuff::Vector3D Distance;
			Distance.Subtract(objPosition, eyePosition);
			eyeDistance = Distance.GetApproximateLength();
		}
	}
	// need to put this value in the appropriate place.
	sprintf(buffer, "%.3f", eyeDistance);
	((MainFrame*)AfxGetMainWnd())
		->m_wndDlgBar.GetDlgItem(IDC_OBJDISTANCEEDIT)
		->SetWindowText(buffer);
}

void EditorInterface::KillCurBrush()
{
	if (curBrush) // might want to do a check to make sure this guy isn't being
		// used
	{
		delete curBrush;
	}
	curBrush = nullptr;
	selecting = false;
	ChangeCursor(IDC_MC2ARROW);
}

int32_t
EditorInterface::PaintDirtRoad()
{
	return PaintOverlay(DIRT_ROAD, PAINT_OVERLAY_DIRT);
}

int32_t
EditorInterface::PaintRocks()
{
	return PaintOverlay(ROUGH, PAINT_OVERLAY_ROUGH);
}

int32_t
EditorInterface::PaintPaved()
{
	return PaintOverlay(PAVED_ROAD, PAINT_OVERLAY_PAVED);
}

int32_t
EditorInterface::PaintTwoLaneDirtRoad()
{
	return PaintOverlay(TWO_LANE_DIRT_ROAD, PAINT_OVERLAY_PAVED);
}

int32_t
EditorInterface::PaintDamagedRoad()
{
	return PaintOverlay(DAMAGED_ROAD, PAINT_OVERLAY_PAVED);
}

int32_t
EditorInterface::PaintRunway()
{
	return PaintOverlay(RUNWAY, PAINT_OVERLAY_PAVED);
}

int32_t
EditorInterface::PaintBridge()
{
	return PaintOverlay(OBRIDGE, PAINT_OVERLAY_PAVED);
}

int32_t
EditorInterface::PaintDamagedBridge()
{
	return PaintOverlay(DAMAGED_BRIDGE, PAINT_OVERLAY_PAVED);
}

int32_t
EditorInterface::PaintOverlay(int32_t type, int32_t message)
{
	KillCurBrush();
	curBrush = new OverlayBrush(type);
	ChangeCursor(IDC_PAINT);
	currentBrushID = message;
	return true;
}

int32_t
EditorInterface::PaintTerrain(int32_t type)
{
	if (selecting && (land->hasSelection()))
	{
		TerrainBrush brush(type);
		Action* pRetAction = brush.applyToSelection();
		if (pRetAction)
			undoMgr.AddAction(pRetAction);
	}
	else
	{
		KillCurBrush();
		curBrush = new TerrainBrush(type);
		ChangeCursor(IDC_PAINT);
		currentBrushID = type;
	}
	return true;
}

int32_t
EditorInterface::SaveAs()
{
	int32_t retVal = IDOK;
	CFileDialog fileDlg(
		0, "pak", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szPAKFilter);
	{
		/* if the mission directory doesn't exist, we attempt to create it */
		int32_t curDirStrSize = GetCurrentDirectory(0, nullptr);
		TCHAR* curDirStr = (TCHAR*)gos_Malloc(curDirStrSize);
		GetCurrentDirectory(curDirStrSize, curDirStr);
		BOOL result = SetCurrentDirectory(missionPath);
		SetCurrentDirectory(curDirStr);
		delete curDirStr;
		curDirStr = 0;
		if (0 == result)
		{
			gosASSERT(false);
			CreateDirectory(missionPath, nullptr);
			CreateDirectory(warriorPath, nullptr);
			CreateDirectory(terrainPath, nullptr);
			CreateDirectory(texturePath, nullptr);
		}
	}
	fileDlg.m_ofn.lpstrInitialDir = missionPath;
	retVal = fileDlg.DoModal();
	if (IDOK == retVal)
	{
		SetBusyMode();
		std::wstring_view pFile = fileDlg.m_ofn.lpstrFile;
		//-----------------------------------------------------
		// New Stuff here.
		// Must resave the following for the new map methods to insure goodness.
		// 	-Base height Map (in terrainPath)
		//	-Hi-res height Map (in terrainPath)
		//	-colour Map (texturePath)
		//	-colour Map Burnin (texturePath)
		//	-Detail Map (texturePath)
		//	-Water Map (texturePath)
		//	-Water Detail Maps (texturePath)
		wchar_t name[1024];
		wchar_t name2[1024];
		_splitpath(pFile, nullptr, nullptr, name2, nullptr);
		if (EditorData::instance->getMapName())
			_splitpath(EditorData::instance->getMapName(), nullptr, nullptr, name, nullptr);
		else
			strcpy(name, name2);
		FullPathFileName oldBaseFile;
		oldBaseFile.init(terrainPath, name, ".tga");
		FullPathFileName newBaseFile;
		newBaseFile.init(terrainPath, name2, ".tga");
		SetFileAttributes(newBaseFile, FILE_ATTRIBUTE_NORMAL);
		CopyFile(oldBaseFile, newBaseFile, false);
		oldBaseFile.init(terrainPath, name, ".height.tga");
		newBaseFile.init(terrainPath, name2, ".height.tga");
		SetFileAttributes(newBaseFile, FILE_ATTRIBUTE_NORMAL);
		CopyFile(oldBaseFile, newBaseFile, false);
		oldBaseFile.init(texturePath, name, ".tga");
		newBaseFile.init(texturePath, name2, ".tga");
		SetFileAttributes(newBaseFile, FILE_ATTRIBUTE_NORMAL);
		CopyFile(oldBaseFile, newBaseFile, false);
		oldBaseFile.init(texturePath, name, ".burnin.tga");
		newBaseFile.init(texturePath, name2, ".burnin.tga");
		SetFileAttributes(newBaseFile, FILE_ATTRIBUTE_NORMAL);
		CopyFile(oldBaseFile, newBaseFile, false);
		oldBaseFile.init(texturePath, name, ".detail.tga");
		newBaseFile.init(texturePath, name2, ".detail.tga");
		SetFileAttributes(newBaseFile, FILE_ATTRIBUTE_NORMAL);
		CopyFile(oldBaseFile, newBaseFile, false);
		oldBaseFile.init(texturePath, name, ".water.tga");
		newBaseFile.init(texturePath, name2, ".water.tga");
		SetFileAttributes(newBaseFile, FILE_ATTRIBUTE_NORMAL);
		CopyFile(oldBaseFile, newBaseFile, false);
		for (size_t i = 0; i < MAX_WATER_DETAIL_TEXTURES; i++)
		{
			wchar_t detailExt[256];
			sprintf(detailExt, ".water%04d.tga", i);
			oldBaseFile.init(texturePath, name, detailExt);
			newBaseFile.init(texturePath, name2, detailExt);
			SetFileAttributes(newBaseFile, FILE_ATTRIBUTE_NORMAL);
			CopyFile(oldBaseFile, newBaseFile, false);
		}
		// Then do a regular save of everything else!!
		EditorData::instance->save(pFile);
		UnsetBusyMode();
	}
	return retVal;
}

int32_t
EditorInterface::Save()
{
	std::wstring_view pFileName = EditorData::instance->getMapName();
	if (pFileName && (0 != strcmp("data\\missions\\newmap.pak", pFileName)))
	{
		EditorData::instance->save(pFileName);
	}
	else
		return SaveAs();
	return IDOK;
}

int32_t
EditorInterface::QuickSave()
{
	std::wstring_view pFileName = EditorData::instance->getMapName();
	if (pFileName && (0 != strcmp("data\\missions\\newmap.pak", pFileName)))
		EditorData::instance->quickSave(pFileName);
	else
		return SaveAs();
	return true;
}

int32_t
EditorInterface::Undo()
{
	if (undoMgr.HaveUndo())
		undoMgr.Undo();
	return true;
}

int32_t
EditorInterface::Redo()
{
	if (undoMgr.HaveRedo())
		undoMgr.Redo();
	return true;
}

int32_t
EditorInterface::paintBuildings(int32_t message)
{
	EditorObjectMgr* pMgr = EditorObjectMgr::instance();
	int32_t groupCount = pMgr->getBuildingGroupCount();
	int32_t id = IDS_OBJECT_200;
	for (size_t i = 0; i < groupCount; ++i)
	{
		int32_t buildCount = pMgr->getNumberBuildingsInGroup(i);
		if (id <= message && message < id + buildCount)
		{
			int32_t alignment = EDITOR_TEAM2;
			for (size_t j = 0; j < 9; ++j)
			{
				if (GetParent()->GetMenu()->GetMenuState(ID_ALIGNMENT_TEAM1 + j, MF_BYCOMMAND) & MF_CHECKED)
				{
					if (j != 8)
						alignment = EDITOR_TEAM1 + j;
					else
						alignment = EDITOR_TEAMNONE;
				}
			}
			// Check groups for movers.
			// If they are Movers, they must NOT be neutral.
			// Bring up a dialog and DO NOT create a brush!!
			if (((i == 4) || (i == 6)) && (alignment == EDITOR_TEAMNONE))
			{
				EMessageBox(IDS_NO_NEUTRAL_MOVERS, IDS_ERROR, MB_OK);
				return false;
			}
			KillCurBrush();
			curBrush = new BuildingBrush(i, message - id, alignment);
			currentBrushID = message;
			currentBrushMenuID = message;
			break;
		}
		id += buildCount;
	}
	return true;
}

int32_t
EditorInterface::Erase()
{
	if (selecting && (EditorObjectMgr::instance()->hasSelection() || land->hasSelection()))
	{
		Eraser tmp;
		Action* pRetAction = tmp.applyToSelection();
		if (pRetAction)
			undoMgr.AddAction(pRetAction);
	}
	else if (0 == dynamic_cast<Eraser*>(curBrush))
	{
		KillCurBrush();
		curBrush = new Eraser();
		ChangeCursor(IDC_ERASER);
		currentBrushID = ID_OTHER_ERASE;
		currentBrushMenuID = ID_OTHER_ERASE;
	}
	return true;
}

void EditorInterface::update()
{
	if (!bThisIsInitialized)
		return;
	if (curBrush)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		curBrush->update(pt.x, pt.y);
	}
}

void EditorInterface::render()
{
	if (!bThisIsInitialized)
		return;
	ModifyStyle(0, WS_HSCROLL | WS_VSCROLL);
	Stuff::Vector3D worldPos;
	Stuff::Vector2DOf<int32_t> screenPos;
	screenPos.x = Environment.screenwidth / 2;
	screenPos.y = Environment.screenheight / 2;
	// eye->inverseProject( screenPos, worldPos );
	/*if ( worldPos.x != 0.0 || worldPos.y != 0.0 )
	{

		worldPos.y += 128.f;

		Stuff::Vector4D screenPos2;

		eye->projectZ( worldPos, screenPos2 );

		gos_VERTEX vertices[2];

		memset( vertices, 0, sizeof( vertices ) );

		float val = 1.4142135623730950488016887242097f;

		Stuff::Vector2DOf<float> screenVector;
		screenVector.x = screenPos.x - screenPos2.x;
		screenVector.y = screenPos.y - screenPos2.y;

		screenVector.Normalize( screenVector );

		Rotate( screenVector, 45.f );


		vertices[0].x = (float)screenPos.x;
		vertices[0].y = (float)screenPos.y;
		vertices[0].argb = 0xffff0000;
		vertices[0].rhw = 1.0;

		vertices[1].x = screenPos2.x;
		vertices[1].y = screenPos2.y;
		vertices[1].argb = 0xffff0000;
		vertices[1].rhw = 1.0;

		gos_DrawLines( vertices, 2 );

		Stuff::Vector4D arrowVector;
		arrowVector.x = screenVector.x + val;
		arrowVector.x = screenVector.x;

		vertices[0].x = screenPos2.x + 10 * screenVector.x;
		vertices[0].y = screenPos2.y + 10 * screenVector.y;

		gos_DrawLines( vertices, 2 );

		Rotate( screenVector, -90.f );
		vertices[0].x = screenPos2.x + 10 * screenVector.x;
		vertices[0].y = screenPos2.y + 10 * screenVector.y;
		gos_DrawLines( vertices, 2 );
		}*/
	if (curBrush)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		// curBrush->beginPaint();
		curBrush->render(pt.x, pt.y);
		// curBrush->endPaint();
		if (painting && !dragging)
		{
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			int32_t PosX = (int32_t)pt.x;
			int32_t PosY = (int32_t)pt.y;
			// scroll if painting
			int32_t scrollLf = (PosX <= (screenScrollLeft));
			int32_t scrollRt = (PosX >= (width() - screenScrollRight));
			int32_t scrollUp = (PosY <= (screenScrollUp));
			int32_t scrollDn = (PosY >= (height() - screenScrollDown));
			float frameFactor = frameLength / baseFrameLength;
			float scrollFactor = scrollInc / eye->getScaleFactor() * frameFactor;
			if (scrollLf)
			{
				eye->moveLeft(scrollFactor);
				syncScrollBars();
			}
			if (scrollRt)
			{
				eye->moveRight(scrollFactor);
				syncScrollBars();
			}
			if (scrollDn)
			{
				eye->moveDown(scrollFactor);
				syncScrollBars();
			}
			if (scrollUp)
			{
				eye->moveUp(scrollFactor);
				syncScrollBars();
			}
		}
	}
}

int32_t
EditorInterface::NewheightMap()
{
	CFileDialog fileDlg(
		1, "tga", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szTGAFilter);
	fileDlg.m_ofn.lpstrInitialDir = terrainPath;
	bool endFlag = false;
	while (!endFlag)
	{
		endFlag = true;
		if (IDOK == fileDlg.DoModal())
		{
			std::wstring_view pFile = fileDlg.m_ofn.lpstrFile;
			if (pFile)
			{
				File tgaFile;
				gosASSERT(strstr(pFile, ".tga") || strstr(pFile, ".TGA"));
				int32_t result = tgaFile.open(const_cast<std::wstring_view>(pFile));
				gosASSERT(result == NO_ERROR);
				struct TGAFileHeader theader;
				tgaFile.read((uint8_t*)&theader, sizeof(TGAFileHeader));
				if ((theader.width != land->realVerticesMapSide) || (theader.height != land->realVerticesMapSide))
				{
					/*wrong size*/
					AfxMessageBox(IDS_WRONG_SIZE_HEIGHT_MAP);
					endFlag = false;
					continue;
				}
			}
			else
			{
				gosASSERT(false);
				endFlag = false;
				continue;
			}
			heightDlg htDlg;
			int32_t tileR, tileC;
			htDlg.SetMin((int32_t)land->getLowestVertex(tileR, tileC));
			htDlg.SetMax((int32_t)land->getHighestVertex(tileR, tileC));
			bool chkStatus = false;
			if (IDOK == htDlg.DoModal())
			{
				SetBusyMode();
				chkStatus =
					EditorData::reassignheightsFromTGA(pFile, htDlg.GetMin(), htDlg.GetMax());
				UnsetBusyMode();
			}
			if (chkStatus)
				tacMap.UpdateMap();
		}
	}
	return true;
}

#define MAX_THRESHOLD 5
#define MAX_NOISE 5
#define MIN_THRESHOLD 1
#define MIN_NOISE 0

int32_t
EditorInterface::RefractalizeTerrain(int32_t Threshold)
{
	FractalDlg fDlg;
	fDlg.SetThreshold(Terrain::fractalThreshold);
	fDlg.SetNoise(Terrain::fractalNoise);
	if (IDOK == fDlg.DoModal())
	{
		SetBusyMode();
		Terrain::fractalThreshold = fDlg.GetThreshold();
		Terrain::fractalNoise = fDlg.GetNoise();
		if (Terrain::fractalThreshold > MAX_THRESHOLD)
			Terrain::fractalThreshold = MAX_THRESHOLD;
		if (Terrain::fractalThreshold < MIN_THRESHOLD)
			Terrain::fractalThreshold = MIN_THRESHOLD;
		if (Terrain::fractalNoise > MAX_NOISE)
			Terrain::fractalNoise = MAX_NOISE;
		if (Terrain::fractalNoise < MIN_NOISE)
			Terrain::fractalNoise = MIN_NOISE;
		if (Terrain::terrainTextures2)
			Terrain::terrainTextures2->refractalizeBaseMesh(
				Terrain::terrainName, Terrain::fractalThreshold, Terrain::fractalNoise);
		UnsetBusyMode();
	}
	return true;
}

int32_t
EditorInterface::SetSky(int32_t skyId)
{
	EditorData::instance->TheSkyNumber(skyId);
	return true;
}

int32_t
EditorInterface::Select()
{
	if (currentBrushID == IDS_SELECT)
	{
		land->unselectAll();
		EditorObjectMgr::instance()->unselectAll();
	}
	else
	{
		KillCurBrush();
		int32_t radius =
			GetParent()->GetMenu()->GetMenuState(ID_DRAGNORMAL, MF_BYCOMMAND) & MF_CHECKED
			? -1
			: smoothRadius;
		curBrush = new SelectionBrush(false, radius);
		currentBrushID = IDS_SELECT;
		currentBrushMenuID = ID_OTHER_SELECT;
		ChangeCursor(IDC_MC2ARROW);
	}
	selecting = true;
	return true;
}

int32_t
EditorInterface::Flatten()
{
	if (selecting && (land->hasSelection()))
	{
		FlattenBrush brush;
		Action* pAction = brush.applyToSelection();
		if (pAction)
		{
			undoMgr.AddAction(pAction);
		}
	}
	else if (0 == dynamic_cast<FlattenBrush*>(curBrush))
	{
		KillCurBrush();
		curBrush = new FlattenBrush();
		ChangeCursor(IDC_FLATTEN);
		currentBrushID = IDS_FLATTEN;
		currentBrushMenuID = ID_OTHER_FLATTEN;
	}
	return true;
}

int32_t
EditorInterface::SaveCameras()
{
	if (!EditorData::getMapName())
	{
		// message box that you must save here
		EMessageBox(IDS_SAVE_CAMERA_FAIL, IDS_ERROR, MB_OK);
	}
	else
	{
		wchar_t base[256];
		strcpy(base, cameraPath);
		strcat(base, EditorData::getMapName());
		strcat(base, "cam");
		strcat(base, ".fit");
		FitIniFile file;
		file.open(base);
		return eye->save(&file);
	}
	return true;
}

int32_t
EditorInterface::SelectSlopes()
{
	SelectSlopeDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		float minAngle = dlg.m_MinEdit;
		float maxAngle = dlg.m_MaxEdit;
		float minheight =
			float(fabs(float(tan(minAngle * float(PI) / 180.f) * land->worldUnitsPerVertex)));
		float maxheight =
			float(fabs(float(tan(maxAngle * float(PI) / 180.f) * land->worldUnitsPerVertex)));
		float centerElv;
		float tmpElv;
		int32_t left;
		int32_t right;
		int32_t top;
		int32_t bottom;
		land->unselectAll();
		for (size_t j = 0; j < land->realVerticesMapSide; ++j)
		{
			for (size_t i = 0; i < land->realVerticesMapSide; ++i)
			{
				left = i > 0 ? i - 1 : 0;
				right = i < land->realVerticesMapSide ? i + 1 : i;
				top = j > 0 ? j - 1 : 0;
				bottom = j < land->realVerticesMapSide ? j + 1 : j;
				centerElv = land->getVertexheight(j * land->realVerticesMapSide + i);
				tmpElv = land->getVertexheight(top * land->realVerticesMapSide + left);
				if (fabs(tmpElv - centerElv) >= minheight && fabs(tmpElv - centerElv) <= maxheight)
				{
					land->selectVertex(j, i);
				}
				else
				{
					tmpElv = land->getVertexheight(top * land->realVerticesMapSide + right);
					if (fabs(tmpElv - centerElv) >= minheight && fabs(tmpElv - centerElv) <= maxheight)
					{
						land->selectVertex(j, i);
					}
					else
					{
						tmpElv = land->getVertexheight(bottom * land->realVerticesMapSide + right);
						if (fabs(tmpElv - centerElv) >= minheight && fabs(tmpElv - centerElv) <= maxheight)
						{
							land->selectVertex(j, i);
						}
						else
						{
							tmpElv =
								land->getVertexheight(bottom * land->realVerticesMapSide + left);
							if (fabs(tmpElv - centerElv) >= minheight && fabs(tmpElv - centerElv) <= maxheight)
							{
								land->selectVertex(j, i);
							}
						}
					}
				}
			}
		} // done looping over vertices
	}
	selecting = true;
	return true;
}

int32_t
EditorInterface::SelectAltitude()
{
	heightDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		float minheight = (float)dlg.GetMin();
		float maxheight = (float)dlg.GetMax();
		float centerElv;
		land->unselectAll();
		for (size_t j = 0; j < land->realVerticesMapSide; ++j)
		{
			for (size_t i = 0; i < land->realVerticesMapSide; ++i)
			{
				centerElv = land->getVertexheight(j * land->realVerticesMapSide + i);
				if ((centerElv >= minheight) && (centerElv <= maxheight))
				{
					land->selectVertex(j, i);
				}
			}
		} // done looping over vertices
	}
	selecting = true;
	return true;
}

int32_t
EditorInterface::SelectTerrainType()
{
	SelectTerrainTypeDlg dlg;
	dlg.SelectedTerrainType(-1);
	if ((dlg.DoModal() == IDOK) && (ID_TERRAINS_BLUEWATER <= dlg.SelectedTerrainType()))
	{
		const int32_t selectedTerrainType = dlg.SelectedTerrainType() - ID_TERRAINS_BLUEWATER;
		land->unselectAll();
		for (size_t j = 0; j < land->realVerticesMapSide; ++j)
		{
			for (size_t i = 0; i < land->realVerticesMapSide; ++i)
			{
				int32_t tmp = land->getTerrain(j, i);
				if (land->getTerrain(j, i) == selectedTerrainType)
				{
					land->selectVertex(j, i);
				}
			}
		} // done looping over vertices
	}
	selecting = true;
	return true;
}

int32_t
EditorInterface::PurgeTransitions()
{
	land->purgeTransitions();
	highlighted = false;
	Terrain::mapData->unhighlightAll();
	return true;
}

int32_t
EditorInterface::ShowTransitions()
{
	highlighted ^= true;
	if (highlighted)
		Terrain::mapData->highlightAllTransitionsOver2();
	else
		Terrain::mapData->unhighlightAll();
	return true;
}

int32_t
EditorInterface::Fog()
{
	FogDlg dlg;
	dlg.m_blue = (eye->dayFogcolour) & 0xff;
	dlg.m_green = (eye->dayFogcolour >> 8) & 0xff;
	dlg.m_red = (eye->dayFogcolour >> 16) & 0xff;
	dlg.m_start = eye->fogStart;
	dlg.m_end = eye->fogFull;
	if (IDOK == dlg.DoModal())
	{
		eye->dayFogcolour =
			((uint32_t)dlg.m_blue) + (((uint32_t)dlg.m_green) << 8) + (((uint32_t)dlg.m_red) << 16);
		eye->fogcolour = eye->dayFogcolour;
		eye->fogStart = dlg.m_start;
		eye->fogFull = dlg.m_end;
		//		land->reCalcLight();
		//		eye->updateDaylight();
	}
	return true;
}

int32_t
EditorInterface::Light()
{
	SunDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		/*
		SafeRunGameOSLogic();
		land->reCalcLight();
		SafeRunGameOSLogic();
		tacMap.UpdateMap();
		*/
	}
	return true;
}

int32_t
EditorInterface::Waves()
{
	WaterDlg dlg;
	dlg.alphaDeep = Terrain::alphaDeep;
	dlg.alphaElevation = Terrain::mapData->alphaDepth;
	dlg.alphaMiddle = Terrain::alphaMiddle;
	dlg.alphaShallow = Terrain::alphaEdge;
	dlg.amplitude = Terrain::waterAmplitude;
	dlg.elevation = Terrain::mapData->waterDepth;
	dlg.frequency = Terrain::waterFreq;
	dlg.shallowElevation = Terrain::mapData->shallowDepth;
	if (IDOK == dlg.DoModal())
	{
		Terrain::alphaDeep = dlg.alphaDeep;
		Terrain::alphaMiddle = dlg.alphaMiddle;
		Terrain::alphaEdge = dlg.alphaShallow;
		Terrain::waterAmplitude = dlg.amplitude;
		Terrain::waterElevation = dlg.elevation;
		Terrain::waterFreq = dlg.frequency;
		Terrain::mapData->shallowDepth = dlg.shallowElevation;
		Terrain::mapData->alphaDepth = dlg.alphaElevation;
		Terrain::mapData->waterDepth = dlg.elevation;
		land->recalcWater();
		tacMap.UpdateMap();
	}
	return true;
}

int32_t
EditorInterface::DragSmooth()
{
	if (IDS_SELECT == currentBrushID)
	{
		currentBrushID = -1;
		currentBrushMenuID = -1;
		GetParent()->GetMenu()->CheckMenuItem(ID_DRAGSMOOTH, MF_BYCOMMAND | MF_CHECKED);
		GetParent()->GetMenu()->CheckMenuItem(ID_DRAGNORMAL, MF_BYCOMMAND | MF_UNCHECKED);
		Select();
	}
	return true;
}
int32_t
EditorInterface::DragRough()
{
	if (IDS_SELECT == currentBrushID)
	{
		currentBrushID = -1;
		currentBrushMenuID = -1;
		GetParent()->GetMenu()->CheckMenuItem(ID_DRAGSMOOTH, MF_BYCOMMAND | MF_UNCHECKED);
		GetParent()->GetMenu()->CheckMenuItem(ID_DRAGNORMAL, MF_BYCOMMAND | MF_CHECKED);
		Select();
	}
	return true;
}

int32_t
EditorInterface::AssignElevation()
{
	if (!land->hasSelection())
	{
		// nothing to assign heights to, let the user know
		wchar_t buffer[256];
		cLoadString(IDS_NO_VERTEX_SEL, buffer, 256, gameResourceHandle);
		MessageBox(buffer);
	}
	else
	{
		FlattenBrush tmp;
		float val = tmp.getAverageheightOfSelection();
		SingleValueDlg dlg(IDS_ASSIGN_ELEVATION, IDS_ELEVATION, (int32_t)val);
		dlg.SetVal((int32_t)val);
		if (IDOK == dlg.DoModal())
		{
			ActionPaintTile* pAction = new ActionPaintTile;
			for (size_t j = 0; j < land->realVerticesMapSide; ++j)
			{
				for (size_t i = 0; i < land->realVerticesMapSide; ++i)
				{
					if (land->isVertexSelected(j, i))
					{
						land->setVertexheight(
							j * land->realVerticesMapSide + i, (float)dlg.GetVal());
					}
				}
			}
			// Action* pAction = tmp.applyheightToSelection( (float)dlg.GetVal()
			// );
			if (pAction)
				undoMgr.AddAction(pAction);
			/*Designers say refreshing the tacmap takes too int32_t. User can do
			 * it manually.*/
			// tacMap.UpdateMap();
		}
	}
	return true;
}

int32_t
EditorInterface::SmoothRadius()
{
	SingleValueDlg dlg(IDS_SMOOTH_RADIUS, IDS_RADIUS, smoothRadius);
	if (IDOK == dlg.DoModal())
	{
		smoothRadius = dlg.GetVal();
		if (IDS_SELECT == currentBrushID)
		{
			currentBrushID = -1;
			currentBrushMenuID = -1;
			Select();
		}
	}
	return true;
}

int32_t
EditorInterface::Alignment(int32_t specific)
{
	for (size_t i = 0; i < 9; ++i)
		GetParent()->GetMenu()->CheckMenuItem(ID_ALIGNMENT_TEAM1 + i, MF_BYCOMMAND | MF_UNCHECKED);
	GetParent()->GetMenu()->CheckMenuItem(specific, MF_BYCOMMAND | MF_CHECKED);
	if (currentBrushID >= IDS_OBJECT_200 && currentBrushID <= 30800)
	{
		int32_t tmp = currentBrushID;
		currentBrushID = 0;
		paintBuildings(tmp);
	}
	return true;
}

int32_t
EditorInterface::SaveheightMap()
{
	CreateDirectory(terrainPath, nullptr);
	CFileDialog fileDlg(
		0, "tga", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szTGAFilter);
	fileDlg.m_ofn.lpstrInitialDir = terrainPath;
	if (IDOK == fileDlg.DoModal())
	{
		std::wstring_view pFile = fileDlg.m_ofn.lpstrFile;
		File file;
		if (NO_ERROR != file.create((std::wstring_view)pFile))
		{
			EMessageBox(IDS_INVALID_FILE, IDS_CANT_SAVE, MB_OK);
			return false;
		}
		SetBusyMode();
		bool ret = EditorData::saveheightMap(&file);
		UnsetBusyMode();
		return ret;
	}
	return true;
}

int32_t
EditorInterface::MissionSettings()
{
	MissionSettingsDlg dlg;
	dlg.m_MissionNameUnlocalizedText = EditorData::instance->MissionName().Data();
	dlg.m_MissionNameUseResourceString = EditorData::instance->MissionNameUseResourceString();
	dlg.m_MissionNameResourceStringID = EditorData::instance->MissionNameResourceStringID();
	dlg.m_AuthorEdit = EditorData::instance->Author().Data();
	dlg.m_BlurbUnlocalizedText = EditorData::instance->Blurb().Data();
	dlg.m_BlurbUseResourceString = EditorData::instance->BlurbUseResourceString();
	dlg.m_BlurbResourceStringID = EditorData::instance->BlurbResourceStringID();
	dlg.m_Blurb2UnlocalizedText = EditorData::instance->Blurb2().Data();
	dlg.m_Blurb2UseResourceString = EditorData::instance->Blurb2UseResourceString();
	dlg.m_Blurb2ResourceStringID = EditorData::instance->Blurb2ResourceStringID();
	dlg.m_TimeLimit = EditorData::instance->TimeLimit();
	dlg.m_DropWeightLimit = EditorData::instance->DropWeightLimit();
	dlg.m_InitialResourcePoints = EditorData::instance->InitialResourcePoints();
	dlg.m_SinglePlayerCheck = (BOOL)EditorData::instance->IsSinglePlayer();
	dlg.m_MaxTeams = EditorData::instance->MaxTeams();
	dlg.m_MaxPlayers = EditorData::instance->MaxPlayers();
	dlg.m_ScenarioTune.Empty();
	dlg.m_ScenarioTune.Format(_T("%d"), EditorData::instance->ScenarioTune());
	dlg.m_VideoFilename = EditorData::instance->VideoFilename().Data();
	dlg.m_CBills = EditorData::instance->CBills();
	dlg.m_NumRPBuildings = EditorData::instance->NumRandomRPbuildings();
	dlg.m_DownloadUrlEdit = EditorData::instance->DownloadURL().Data();
	dlg.m_MissionType = EditorData::instance->MissionType();
	dlg.m_AirStrikeCheck = EditorData::instance->AirStrikesEnabledDefault();
	dlg.m_MineLayerCheck = EditorData::instance->MineLayersEnabledDefault();
	dlg.m_ScoutCopterCheck = EditorData::instance->ScoutCoptersEnabledDefault();
	dlg.m_SensorProbeCheck = EditorData::instance->SensorProbesEnabledDefault();
	dlg.m_UnlimitedAmmoCheck = EditorData::instance->UnlimitedAmmoEnabledDefault();
	dlg.m_AllTech = EditorData::instance->AllTechEnabledDefault();
	dlg.m_RepairVehicleCheck = EditorData::instance->RepairVehicleEnabledDefault();
	dlg.m_SalvageCraftCheck = EditorData::instance->SalvageCraftEnabledDefault();
	dlg.m_ResourceBuildingCheck = EditorData::instance->ResourceBuildingsEnabledDefault();
	dlg.m_NoVariantsCheck = EditorData::instance->NoVariantsEnabledDefault();
	dlg.m_ArtilleryPieceCheck = EditorData::instance->ArtilleryPieceEnabledDefault();
	dlg.m_RPsForMechsCheck = EditorData::instance->RPsForMechsEnabledDefault();
	if (IDOK == dlg.DoModal())
	{
		EditorData::instance->MissionName(dlg.m_MissionNameUnlocalizedText.GetBuffer(0));
		EditorData::instance->MissionNameUseResourceString(dlg.m_MissionNameUseResourceString);
		EditorData::instance->MissionNameResourceStringID(dlg.m_MissionNameResourceStringID);
		EditorData::instance->Author(dlg.m_AuthorEdit.GetBuffer(0));
		EditorData::instance->Blurb(dlg.m_BlurbUnlocalizedText.GetBuffer(0));
		EditorData::instance->BlurbUseResourceString(dlg.m_BlurbUseResourceString);
		EditorData::instance->BlurbResourceStringID(dlg.m_BlurbResourceStringID);
		EditorData::instance->Blurb2(dlg.m_Blurb2UnlocalizedText.GetBuffer(0));
		EditorData::instance->Blurb2UseResourceString(dlg.m_Blurb2UseResourceString);
		EditorData::instance->Blurb2ResourceStringID(dlg.m_Blurb2ResourceStringID);
		EditorData::instance->TimeLimit(dlg.m_TimeLimit);
		EditorData::instance->DropWeightLimit(dlg.m_DropWeightLimit);
		EditorData::instance->InitialResourcePoints(dlg.m_InitialResourcePoints);
		EditorData::instance->IsSinglePlayer((bool)dlg.m_SinglePlayerCheck);
		EditorData::instance->MaxTeams(dlg.m_MaxTeams);
		EditorData::instance->MaxPlayers(dlg.m_MaxPlayers);
		EditorData::instance->ScenarioTune(atoi(dlg.m_ScenarioTune.GetBuffer(0)));
		EditorData::instance->VideoFilename(dlg.m_VideoFilename.GetBuffer(0));
		EditorData::instance->CBills(dlg.m_CBills);
		EditorData::instance->NumRandomRPbuildings(dlg.m_NumRPBuildings);
		EditorData::instance->DownloadURL(dlg.m_DownloadUrlEdit.GetBuffer(0));
		EditorData::instance->MissionType(dlg.m_MissionType);
		EditorData::instance->AirStrikesEnabledDefault(dlg.m_AirStrikeCheck);
		EditorData::instance->MineLayersEnabledDefault(dlg.m_MineLayerCheck);
		EditorData::instance->ScoutCoptersEnabledDefault(dlg.m_ScoutCopterCheck);
		EditorData::instance->SensorProbesEnabledDefault(dlg.m_SensorProbeCheck);
		EditorData::instance->UnlimitedAmmoEnabledDefault(dlg.m_UnlimitedAmmoCheck);
		EditorData::instance->AllTechEnabledDefault(dlg.m_AllTech);
		EditorData::instance->RepairVehicleEnabledDefault(dlg.m_RepairVehicleCheck);
		EditorData::instance->SalvageCraftEnabledDefault(dlg.m_SalvageCraftCheck);
		EditorData::instance->ResourceBuildingsEnabledDefault(dlg.m_ResourceBuildingCheck);
		EditorData::instance->NoVariantsEnabledDefault(dlg.m_NoVariantsCheck);
		EditorData::instance->ArtilleryPieceEnabledDefault(dlg.m_ArtilleryPieceCheck);
		EditorData::instance->RPsForMechsEnabledDefault(dlg.m_RPsForMechsCheck);
	}
	return true;
}

int32_t
EditorInterface::Team(int32_t team)
{
	static CTeams originalTeams;
	if (!ObjectSelectOnlyMode())
	{
		originalTeams = EditorData::instance->TeamsRef();
	}
	EditorData::instance->DoTeamDialog(team);
	if (!ObjectSelectOnlyMode())
	{
		if (!(originalTeams == EditorData::instance->TeamsRef()))
		{
			TeamsAction* pTeamsAction = new TeamsAction(originalTeams);
			undoMgr.AddAction(pTeamsAction);
		}
		originalTeams.Clear();
	}
	return true;
}

int32_t
EditorInterface::Player(int32_t player)
{
	PlayerSettingsDlg dlg;
	dlg.m_playerEdit = player + 1;
	dlg.m_oldDefaultTeam = EditorData::instance->PlayersRef().PlayerRef(player).DefaultTeam();
	dlg.m_numTeams = EditorData::instance->MaxTeams();
	if ((IDOK == dlg.DoModal()) && (0 <= dlg.m_newDefaultTeam))
	{
		EditorData::instance->PlayersRef().PlayerRef(player).DefaultTeam(dlg.m_newDefaultTeam);
	}
	return true;
}

static const float SQRT_2 = 1.4142135623730950488016887242097f;
/* make the horizontal scroll bar reflect the current camera position */
void EditorInterface::syncHScroll()
{
	/* this calculation was based in the code for Camera::moveRight(float
	 * amount) */
	Stuff::Vector3D direction;
	if (!eye->usePerspective)
	{
		direction.x = 1.0;
		direction.y = 0.0;
		direction.z = 0.0;
	}
	else
	{
		direction.x = -1.0;
		direction.y = 0.0;
		direction.z = 0.0;
	}
	float worldCameraRotation = eye->getCameraRotation();
	OppRotate(direction, worldCameraRotation);
	Stuff::Vector3D eyeDisplacement = eye->getPosition();
	/* maxVisual was taken from Camera::setPosition(). */
	float maxVisual = (Terrain::worldUnitsMapSide / 2) - Terrain::worldUnitsPerVertex;
	float bound = SQRT_2 * maxVisual;
	float scrollPos = ((direction * eyeDisplacement) / bound * 0.5f + 0.5f) * HSCROLLBAR_RANGE;
	SetScrollRange(SB_HORZ, 0, HSCROLLBAR_RANGE, true);
	SetScrollPos(SB_HORZ, (int32_t)scrollPos);
	{
		/* figure out what proportion of the map is visible */
		Stuff::Vector2DOf<int32_t> screen;
		Stuff::Vector3D world1, world2;
		screen.y = Environment.screenheight / 2;
		screen.x = 1;
		eye->inverseProject(screen, world1);
		screen.x = Environment.screenwidth - 1;
		eye->inverseProject(screen, world2);
		float dx = world2.x - world1.x;
		float dy = world2.y - world1.y;
		float span = sqrt(dx * dx + dy * dy);
		/* make the size of the scroll bar proportional to the proportion of the
		 * map that's visible */
		SCROLLINFO si;
		GetScrollInfo(SB_HORZ, &si, SIF_PAGE);
		si.nPage = HSCROLLBAR_RANGE * span / (2.0 * bound);
		si.fMask = SIF_PAGE;
		SetScrollInfo(SB_HORZ, &si);
	}
	tacMap.RedrawWindow();
}

/* make the vertical scroll bar reflect the current camera position */
void EditorInterface::syncVScroll()
{
	/* this calculation was based in the code for Camera::moveRight(float
	 * amount) */
	Stuff::Vector3D direction;
	if (!eye->usePerspective)
	{
		direction.x = 0.0;
		direction.y = -1.0;
		direction.z = 0.0;
	}
	else
	{
		direction.x = 0.0;
		direction.y = 1.0;
		direction.z = 0.0;
	}
	float worldCameraRotation = eye->getCameraRotation();
	OppRotate(direction, worldCameraRotation);
	Stuff::Vector3D eyeDisplacement = eye->getPosition();
	/* maxVisual was taken from Camera::setPosition(). */
	float maxVisual = (Terrain::worldUnitsMapSide / 2) - Terrain::worldUnitsPerVertex;
	float bound = SQRT_2 * maxVisual;
	float scrollPos = ((direction * eyeDisplacement) / bound * 0.5f + 0.5f) * VSCROLLBAR_RANGE;
	SetScrollRange(SB_VERT, 0, VSCROLLBAR_RANGE, false);
	SetScrollPos(SB_VERT, (int32_t)scrollPos);
	{
		/* figure out what proportion of the map is visible */
		Stuff::Vector2DOf<int32_t> screen;
		Stuff::Vector3D world1, world2;
		screen.x = Environment.screenwidth / 2;
		screen.y = 1;
		eye->inverseProject(screen, world1);
		screen.y = Environment.screenheight - 1;
		eye->inverseProject(screen, world2);
		float dx = world2.x - world1.x;
		float dy = world2.y - world1.y;
		float span = sqrt(dx * dx + dy * dy);
		/* make the size of the scroll bar proportional to the proportion of the
		 * map that's visible */
		SCROLLINFO si;
		GetScrollInfo(SB_VERT, &si, SIF_PAGE);
		si.nPage = VSCROLLBAR_RANGE * span / (2.0 * bound);
		si.fMask = SIF_PAGE;
		SetScrollInfo(SB_VERT, &si);
	}
	tacMap.RedrawWindow();
}

void EditorInterface::SetBusyMode(bool bRedrawWindow)
{
	if (bRedrawWindow)
	{
		// SendMessage(WM_PAINT, 0, 0);
		// tacMap.SendMessage(WM_PAINT, 0, 0);
		RedrawWindow();
		tacMap.RedrawWindow();
	}
	if (0 == m_AppIsBusy)
	{
		if (!m_hBusyCursor)
		{
#ifndef _DEBUG /*the debugger seems to encounter an infinite loop of user                          \
				  breaks (int32_t 3) ntdll when executing this line*/
			m_hBusyCursor =
				(HCURSOR)LoadImage(nullptr, "bmech.ani", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
#endif
		}
		if (m_hBusyCursor)
		{
			::SetCursor(m_hBusyCursor);
		}
		else
		{
			::SetCursor(LoadCursor(nullptr, IDC_WAIT));
		}
	}
	m_AppIsBusy += 1;
}

void EditorInterface::UnsetBusyMode()
{
	if (1 == m_AppIsBusy)
	{
		if (hCursor)
		{
			::SetCursor(hCursor);
		}
	}
	m_AppIsBusy -= 1;
}

int32_t
EditorInterface::Damage(bool bDamage)
{
	if (selecting && EditorObjectMgr::instance()->hasSelection())
	{
		DamageBrush tmp(bDamage);
		Action* pRetAction = tmp.applyToSelection();
		if (pRetAction)
			undoMgr.AddAction(pRetAction);
	}
	else
	{
		DamageBrush* pCurDamageBrush = dynamic_cast<DamageBrush*>(curBrush);
		if ((0 == pCurDamageBrush) || (pCurDamageBrush->damage != bDamage))
		{
			KillCurBrush();
			curBrush = new DamageBrush(bDamage);
			ChangeCursor(IDC_HAMMER + !bDamage);
			currentBrushID = ID_OTHER_DAMAGE + bDamage;
		}
	}
	return true;
}

int32_t
EditorInterface::LayMines()
{
	if (selecting && (land->hasSelection()))
	{
		MineBrush brush;
		Action* pRetAction = brush.applyToSelection();
		if (pRetAction)
			undoMgr.AddAction(pRetAction);
	}
	else if (0 == dynamic_cast<MineBrush*>(curBrush))
	{
		KillCurBrush();
		curBrush = new MineBrush();
		ChangeCursor(IDC_HAMMER); // need a minebrush cursor
		currentBrushID = ID_OTHER_LAYMINES;
	}
	return true;
}

int32_t
EditorInterface::SelectDetailTexture()
{
	CFileDialog fileDlg(TRUE, "tga", nullptr, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szTGAFilter);
	fileDlg.m_ofn.lpstrInitialDir = texturePath;
	if (IDOK == fileDlg.DoModal())
	{
		CString path = fileDlg.m_ofn.lpstrFile;
		path.MakeLower();
		if (land->terrainTextures2 && (land->terrainTextures2->colorMapStarted))
		{
			land->terrainTextures2->resetDetailTexture(path.GetBuffer(0));
			EditorData::instance->DetailTextureNeedsSaving(true);
		}
	}
	return true;
}

int32_t
EditorInterface::SelectWaterTexture()
{
	CFileDialog fileDlg(TRUE, "tga", nullptr, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szTGAFilter);
	fileDlg.m_ofn.lpstrInitialDir = texturePath;
	if (IDOK == fileDlg.DoModal())
	{
		CString path = fileDlg.m_ofn.lpstrFile;
		path.MakeLower();
		if (land->terrainTextures2 && (land->terrainTextures2->colorMapStarted))
		{
			land->terrainTextures2->resetWaterTexture(path.GetBuffer(0));
			EditorData::instance->WaterTextureNeedsSaving(true);
		}
	}
	return true;
}

//---------------------------------------------------------------------------
inline bool
colorMapIsOKFormat(std::wstring_view fileName)
{
	uint32_t localcolourMapSizeCheck = land->realVerticesMapSide * 12.8;
	File tgaFile;
	int32_t result = tgaFile.open(fileName);
	if (result == NO_ERROR)
	{
		struct TGAFileHeader tgaHeader;
		tgaFile.read((uint8_t*)&tgaHeader, sizeof(TGAFileHeader));
		if ((tgaHeader.image_type == UNC_TRUE) && (tgaHeader.width == tgaHeader.height) && (tgaHeader.width == localcolourMapSizeCheck))
			return true;
		tgaFile.close();
	}
	return false;
}

int32_t
EditorInterface::SetBaseTexture()
{
	CFileDialog fileDlg(TRUE, "tga", nullptr, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szTGAFilter);
	fileDlg.m_ofn.lpstrInitialDir = texturePath;
	if (IDOK == fileDlg.DoModal())
	{
		CString path = fileDlg.m_ofn.lpstrFile;
		path.MakeLower();
		// Check that this is a valid colormap of EXACTLY the same size!!!
		if (colorMapIsOKFormat(path))
		{
			wchar_t name[1024];
			_splitpath(path, nullptr, nullptr, name, nullptr);
			std::wstring_view testLoc = strstr(name, ".burnin");
			if (testLoc)
				testLoc[0] = 0; // Prune off the burnin name.
			land->setcolourMapName(name);
			if (land->terrainTextures2 && (land->terrainTextures2->colorMapStarted))
			{
				if (land->colorMapName)
					land->terrainTextures2->resetBaseTexture(land->colorMapName);
				else
					land->terrainTextures2->resetBaseTexture(land->terrainName);
			}
		}
		else
		{
			EMessageBox(IDS_INVALID_COLOR_MAP, IDS_ERROR, MB_OK);
		}
	}
	return true;
}

int32_t
EditorInterface::ReloadBaseTexture()
{
	if (land->terrainTextures2 && (land->terrainTextures2->colorMapStarted))
	{
		if (land->colorMapName)
			land->terrainTextures2->resetBaseTexture(land->colorMapName);
		else
			land->terrainTextures2->resetBaseTexture(land->terrainName);
	}
	return true;
}

int32_t
EditorInterface::SelectWaterDetailTexture()
{
	CFileDialog fileDlg(TRUE, "tga", nullptr, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szTGAFilter);
	fileDlg.m_ofn.lpstrInitialDir = texturePath;
	if (IDOK == fileDlg.DoModal())
	{
		CString path = fileDlg.m_ofn.lpstrFile;
		path.MakeLower();
		if (land->terrainTextures2 && (land->terrainTextures2->colorMapStarted))
		{
			land->terrainTextures2->resetWaterDetailTextures(path.GetBuffer(0));
			EditorData::instance->WaterDetailTextureNeedsSaving(true);
		}
	}
	return true;
}

int32_t
EditorInterface::TextureTilingFactors()
{
	TilingFactorsDialog tilingFactorsDlg;
	tilingFactorsDlg.m_TerrainDetailTilingFactor = land->terrainTextures2->getDetailTilingFactor();
	tilingFactorsDlg.m_WaterTilingFactor = land->terrainTextures2->getWaterTextureTilingFactor();
	tilingFactorsDlg.m_WaterDetailTilingFactor =
		land->terrainTextures2->getWaterDetailTilingFactor();
	if (IDOK == tilingFactorsDlg.DoModal())
	{
		land->terrainTextures2->setDetailTilingFactor(tilingFactorsDlg.m_TerrainDetailTilingFactor);
		land->terrainTextures2->setWaterTextureTilingFactor(tilingFactorsDlg.m_WaterTilingFactor);
		land->terrainTextures2->setWaterDetailTilingFactor(
			tilingFactorsDlg.m_WaterDetailTilingFactor);
	}
	return true;
}

int32_t
EditorInterface::Link(bool bLink)
{
	LinkBrush* pCurLinkBrush = dynamic_cast<LinkBrush*>(curBrush);
	if ((0 == pCurLinkBrush) || (pCurLinkBrush->bLink != bLink))
	{
		KillCurBrush();
		curBrush = new LinkBrush(bLink);
		ChangeCursor(IDC_LINK + !bLink);
		currentBrushID = ID_OTHER_LINK + bLink;
	}
	return 1;
}

int32_t
EditorInterface::DropZone(bool bVTol)
{
	KillCurBrush();
	int32_t alignment = EDITOR_TEAM1;
	// you'll need this for multiplayer
	/*for ( int32_t j = 0; j < 9; ++j )
	{
		if ( GetParent()->GetMenu()->GetMenuState( ID_ALIGNMENT_TEAM1 + j,
	MF_BYCOMMAND ) & MF_CHECKED )
		{
			if (j != 8)
				alignment = EDITOR_TEAM1 + j;
			else
				alignment = EDITOR_TEAMNONE;
		}
	}*/
	curBrush = new DropZoneBrush(alignment, bVTol);
	currentBrushID = ID_DROPZONES_ADD + bVTol;
	ChangeCursor(IDC_DROPZONE);
	return true;
}

int32_t
EditorInterface::CampaignEditor()
{
	CCampaignDialog campaignDialog;
	campaignDialog.DoModal();
	return 1;
}

int32_t
EditorInterface::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!tacMap.m_hWnd)
	{
		tacMap.Create(IDD_TACMAP, this);
		tacMap.ShowWindow(true);
		tacMap.RedrawWindow();
	}
	else
	{
		gosASSERT(false);
	}
	return 0;
}

extern bool gActive;
extern bool gGotFocus;
extern Editor* editor;

LRESULT
EditorInterface::WindowProc(uint32_t message, WPARAM wparam, LPARAM lparam)
{
	// We're not ready to draw anything.
	// If you let an event through it'll probably crash!
	//
	if (message == WM_KEYDOWN)
		handleKeyDown(wparam);
	if (message == WM_CREATE)
	{
		gActive = true;
		gGotFocus = true;
	}
	if (WM_MOUSEWHEEL == message)
	{
		// int32_t i = 17;
	}
	int32_t retVal = 0;
	retVal = CWnd::WindowProc(message, wparam, lparam);
	if (message == WM_KEYDOWN && wparam == VK_SCROLL)
		return 0;
	if ((eye || message == WM_MOVE))
	{
		switch (message)
		{
		case WM_SYSCOLORCHANGE:
		case WM_DISPLAYCHANGE:
		case WM_SETCURSOR:
		case WM_ACTIVATEAPP:
		case WM_KILLFOCUS:
		case WM_SETFOCUS:
		case WM_MOVE:
		case WM_ERASEBKGND:
		case 0x20b:
		case 0x20c:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_CLOSE:
			retVal = GameOSWinProc(m_hWnd, message, wparam, lparam);
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		{
			retVal = GameOSWinProc(m_hWnd, message, wparam, lparam);
		}
		break;
		case WM_SIZE:
		{
			float w = LOWORD(lparam);
			float h = HIWORD(lparam);
			windowSizeChanged = true;
			g_newwidth = w + 16 /*scrollbar thickness*/;
			g_newheight = h + 16 /*scrollbar thickness*/;
			retVal = GameOSWinProc(m_hWnd, message, wparam, lparam);
			if (editor && bThisIsInitialized)
			{
				editor->update();
			}
			// EditorObjectMgr::instance()->update();
		}
		break;
		}
	}
	return retVal;
}

afx_msg void
EditorInterface::OnCommand(WPARAM wparam)
{
	handleNewMenuMessage(wparam);
}

void EditorInterface::UpdateButton(CCmdUI* pButton)
{
	pButton->Enable(true);
	if (pButton->m_nID == ID_DROPZONES_ADD || pButton->m_nID == ID_DROPZONES_VTOL)
	{
		Stuff::Vector3D pos;
		pos.x = -1;
		pos.y = -1;
		if (!EditorObjectMgr::instance()->canAddDropZone(
				pos, 0, (pButton->m_nID - ID_DROPZONES_ADD) ? true : false))
			pButton->Enable(false);
	}
}

void EditorInterface::OnLButtonDown(uint32_t nFlags, CPoint point)
{
	SetCapture();
	handleLeftButtonDown(point.x, point.y);
	SPEW((0, "GotClick"));
}

void EditorInterface::OnLButtonUp(uint32_t nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	handleLeftButtonUp(point.x, point.y);
	ReleaseCapture();
}

void EditorInterface::OnMouseMove(uint32_t nFlags, CPoint point)
{
	handleMouseMove(point.x, point.y);
}

void EditorInterface::ChangeCursor(int32_t ID)
{
	if (hCursor)
		DestroyCursor(hCursor);
	hCursor = AfxGetApp()->LoadCursor(ID);
	if (0 >= m_AppIsBusy)
	{
		::SetCursor(hCursor);
	}
	curCursorID = ID;
}

BOOL EditorInterface::OnSetCursor(CWnd* pWnd, uint32_t nHitTest, uint32_t message)
{
	if (0 < m_AppIsBusy)
	{
		if (m_hBusyCursor)
		{
			::SetCursor(m_hBusyCursor);
		}
		else
		{
			::SetCursor(LoadCursor(nullptr, IDC_WAIT));
		}
	}
	else if (hCursor)
	{
		::SetCursor(hCursor);
		return true;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL EditorInterface::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass =
		AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, // use any window styles
			nullptr, (HBRUSH)(COLOR_WINDOW + 1));
	return CWnd ::PreCreateWindow(cs);
}

void EditorInterface::OnKeyUp(uint32_t nChar, uint32_t nRepCnt, uint32_t nFlags)
{
	if (nChar == KEY_SPACE)
	{
		KillCurBrush();
		curBrush = prevBrush;
		selecting = prevSelecting;
		painting = prevPainting;
		dragging = prevDragging;
		prevBrush = nullptr;
		prevSelecting = false;
		prevPainting = false;
		prevDragging = false;
		ChangeCursor(oldCursor);
		syncScrollBars();
		// dragging = false;
	}
	lastKey = -1;
	CWnd ::OnKeyUp(nChar, nRepCnt, nFlags);
}

void EditorInterface::OnRButtonDown(uint32_t nFlags, CPoint point)
{
	SetCapture();
	lastX = point.x;
	lastY = point.y;
	rightDrag = true;
	lastRightClickTime = timeGetTime();
}
void EditorInterface::OnRButtonUp(uint32_t nFlags, CPoint point)
{
	rightDrag = false;
	ReleaseCapture();
	if (timeGetTime() - lastRightClickTime < 200)
	{
		Select();
	}
	else
	{
		tacMap.RedrawWindow();
		syncScrollBars();
	}
	// is there anything under the mouse
	/*Stuff::Vector3D pos;
	Stuff::Vector2DOf<int32_t> screen;
	screen.x = point.x;
	screen.y = point.y;
	eye->inverseProject( screen, pos );

	EditorObject* pObject = EditorObjectMgr::instance()->getObjectAtPosition(
	pos ); Unit* pUnit = dynamic_cast<Unit*>(pObject);

	if ( pUnit )
	{
		CMenu Menu;
		Menu.LoadMenu( IDR_POPUP );
		CMenu* pPopUp = Menu.GetSubMenu( 0 );

		ClientToScreen( &point );

		pPopUp->TrackPopupMenu( nFlags, point.x, point.y, this );

		CWnd ::OnRButtonUp(nFlags, point);
	}*/
}

BOOL EditorInterface::OnMouseWheel(uint32_t nFlags, int16_t zDelta, CPoint pt)
{
	//--------------------------------------------------
	// Zoom Camera based on Mouse Wheel input.
	int32_t mouseWheelDelta = zDelta; // 240 is the weird increment that the mouse wheel is in
	if (mouseWheelDelta)
	{
		float actualZoom = zoomInc * abs(mouseWheelDelta) * 0.0001f * eye->getScaleFactor();
		if (mouseWheelDelta < 0)
		{
			eye->ZoomOut(actualZoom);
		}
		else
		{
			eye->ZoomIn(actualZoom);
		}
	}
	// int32_t middleClicked = nFlags & MK_MBUTTON;
	int32_t middleClicked = (nFlags & MK_MBUTTON) && (nFlags & MK_RBUTTON); // it's too easy to accidentally press the middle
		// button while scrolling on some mice
	//-----------------------------------------------------------------
	// If middle mouse button is pressed, go to normal tilt, 50% zoom
	if (middleClicked)
	{
		eye->tiltNormal();
		eye->ZoomNormal();
	}
	// IF there is a selected object, find distance to it from camera.
	float eyeDistance = 0.0f;
	int32_t selectionCount = EditorObjectMgr::instance()->getSelectionCount();
	if (selectionCount)
	{
		EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjectsList =
			EditorObjectMgr::instance()->getSelectedObjectList();
		EditorObjectMgr::EDITOR_OBJECT_LIST::EIterator it = selectedObjectsList.Begin();
		const EditorObject* pInfo = (*it);
		if (pInfo)
		{
			Stuff::Point3D eyePosition(eye->getCameraOrigin());
			Stuff::Point3D objPosition;
			objPosition.x = -pInfo->appearance()->position.x;
			objPosition.y = pInfo->appearance()->position.z;
			objPosition.z = pInfo->appearance()->position.y;
			Stuff::Vector3D Distance;
			Distance.Subtract(objPosition, eyePosition);
			eyeDistance = Distance.GetApproximateLength();
		}
	}
	// need to put this value in the appropriate place.
	wchar_t buffer[1024];
	sprintf(buffer, "%.3f", eyeDistance);
	((MainFrame*)AfxGetMainWnd())
		->m_wndDlgBar.GetDlgItem(IDC_OBJDISTANCEEDIT)
		->SetWindowText(buffer);
	tacMap.RedrawWindow();
	syncScrollBars();
	return CWnd ::OnMouseWheel(nFlags, zDelta, pt);
}

void EditorInterface::OnHScroll(uint32_t nSBCode, uint32_t nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar != nullptr && pScrollBar->SendChildNotifyLastMsg())
		return; // eat it
	// ignore scroll bar msgs from other controls
	if (pScrollBar != GetScrollBarCtrl(SB_HORZ))
		return;
	//	OnScroll(MAKEWORD(nSBCode, -1), nPos);
	SCROLLINFO sInfo;
	sInfo.cbSize = sizeof(SCROLLINFO);
	sInfo.fMask = SIF_POS | SIF_PAGE;
	sInfo.nMin = 0;
	sInfo.nMax = 0;
	sInfo.nPage = 0;
	sInfo.nPos = 0;
	sInfo.nTrackPos = 0;
	CPoint pt;
	GetScrollInfo(SB_HORZ, &sInfo);
	pt.x = sInfo.nPos;
	switch (nSBCode)
	{
	case SB_LINELEFT:
		pt.x += (-0.01 * HSCROLLBAR_RANGE);
		pt.y = 0;
		break;
	case SB_LINERIGHT:
		pt.x += (0.01 * HSCROLLBAR_RANGE);
		pt.y = 0;
		break;
	case SB_PAGELEFT:
		pt.x -= sInfo.nPage;
		break;
	case SB_PAGERIGHT:
		pt.x += sInfo.nPage;
		break;
	case SB_THUMBPOSITION:
		pt.x = nPos;
		break;
	case SB_THUMBTRACK:
		pt.x = nPos;
		break;
	default:
		break;
	}
	/* this calculation was based in the code for Camera::moveRight(float
	 * amount) */
	Stuff::Vector3D direction;
	if (!eye->usePerspective)
	{
		direction.x = 1.0;
		direction.y = 0.0;
		direction.z = 0.0;
	}
	else
	{
		direction.x = -1.0;
		direction.y = 0.0;
		direction.z = 0.0;
	}
	float worldCameraRotation = eye->getCameraRotation();
	OppRotate(direction, worldCameraRotation);
	Stuff::Vector3D eyeDisplacement = eye->getPosition();
	/* maxVisual was taken from Camera::setPosition(). */
	float maxVisual = (Terrain::worldUnitsMapSide / 2) - Terrain::worldUnitsPerVertex;
	float bound = SQRT_2 * maxVisual;
	float amount = bound * 2.0f * (((float)pt.x) / HSCROLLBAR_RANGE - 0.5f);
	float amountMore = amount - direction * eyeDisplacement;
	eye->moveRight(amountMore);
	sInfo.nPos = pt.x;
	sInfo.fMask = SIF_POS;
	this->SetScrollPos(SB_HORZ, pt.x);
	CWnd ::OnHScroll(nSBCode, nPos, pScrollBar);
	SafeRunGameOSLogic();
	tacMap.RedrawWindow();
	syncScrollBars();
}

void EditorInterface::OnVScroll(uint32_t nSBCode, uint32_t nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO sInfo;
	sInfo.cbSize = sizeof(SCROLLINFO);
	sInfo.fMask = SIF_POS | SIF_PAGE;
	sInfo.nMin = 0;
	sInfo.nMax = 0;
	sInfo.nPage = 0;
	sInfo.nPos = 0;
	sInfo.nTrackPos = 0;
	CPoint pt;
	GetScrollInfo(SB_VERT, &sInfo);
	pt.y = sInfo.nPos;
	switch (nSBCode)
	{
	case SB_LINELEFT:
		pt.x = 0;
		pt.y += (-0.01 * VSCROLLBAR_RANGE);
		break;
	case SB_LINERIGHT:
		pt.x = 0;
		pt.y += (0.01 * VSCROLLBAR_RANGE);
		break;
	case SB_PAGELEFT:
		pt.y -= sInfo.nPage;
		break;
	case SB_PAGERIGHT:
		pt.y += sInfo.nPage;
		break;
	case SB_THUMBPOSITION:
		pt.y = nPos;
		break;
	case SB_THUMBTRACK:
		pt.y = nPos;
		break;
	default:
		break;
	}
	/* this calculation was based in the code for Camera::moveRight(float
	 * amount) */
	Stuff::Vector3D direction;
	if (!eye->usePerspective)
	{
		direction.x = 0.0;
		direction.y = -1.0;
		direction.z = 0.0;
	}
	else
	{
		direction.x = 0.0;
		direction.y = 1.0;
		direction.z = 0.0;
	}
	float worldCameraRotation = eye->getCameraRotation();
	OppRotate(direction, worldCameraRotation);
	Stuff::Vector3D eyeDisplacement = eye->getPosition();
	/* maxVisual was taken from Camera::setPosition(). */
	float maxVisual = (Terrain::worldUnitsMapSide / 2) - Terrain::worldUnitsPerVertex;
	float bound = SQRT_2 * maxVisual;
	float amount = bound * 2.0f * ((float)pt.y / VSCROLLBAR_RANGE - 0.5f);
	float amountMore = amount - direction * eyeDisplacement;
	eye->moveDown(amountMore);
	SetScrollPos(SB_VERT, pt.y);
	CWnd ::OnVScroll(nSBCode, nPos, pScrollBar);
	SafeRunGameOSLogic();
	tacMap.RedrawWindow();
	syncScrollBars();
}

void EditorInterface::OnSysKeyDown(uint32_t nChar, uint32_t nRepCnt, uint32_t nFlags)
{
	handleKeyDown(nChar);
	CWnd ::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

int32_t
EditorInterface::UnitSettings()
{
	EList<Unit*, Unit*> list;
	// ok, find the rest of the selected units
	EditorObjectMgr::instance()->getSelectedUnits(list);
	if (list.Count())
	{
		// dialog for units (mechs and vehicles)
		UnitSettingsDlg dlg(list, undoMgr);
		if (IDOK == dlg.DoModal())
		{
		}
	}
	EditorObjectMgr::EDITOR_OBJECT_LIST list2 =
		EditorObjectMgr::instance()->getSelectedObjectList();
	if (list2.Count() > list.Count())
	{
		EditorObjectMgr::EDITOR_OBJECT_LIST::EIterator iter;
		for (iter = list2.End(); !iter.IsDone(); iter--)
		{
			if ((0 != dynamic_cast<Unit*>(*iter)) || (0 != dynamic_cast<::DropZone*>(*iter)))
			{
				list2.Delete(iter);
			}
		}
		if (list2.Count())
		{
			// dialog for buildings (not units, not dropzones)
			BuildingSettingsDlg dlg(list2, undoMgr);
			if (IDOK == dlg.DoModal())
			{
			}
		}
	}
	return true;
}

void EditorInterface::initTacMap()
{
	// Why don't we just load it from the frickin' file?
	// We go through all the damned trouble to save it every time!
	// This takes a int32_t time.
	// -fs
	uint8_t* pData = nullptr;
	int32_t size = 0;
	FullPathFileName mPath;
	bool bFile = false;
	if (EditorData::instance->getMapName())
	{
		mPath.init(missionPath, (std::wstring_view)EditorData::instance->getMapName(), ".pak");
		bFile = true;
	}
	if (bFile && fileExists(mPath))
	{
		PacketFile file;
		file.open(mPath);
		EditorData::instance->loadTacMap(&file, pData, size, 128);
		file.close();
	}
	else
	{
		EditorData::instance->makeTacMap(pData, size, 128);
	}
	tacMap.SetData(pData, size);
	free(pData);
	pData = nullptr;
	tacMap.RedrawWindow();
}

BOOL EditorInterface::PreTranslateMessage(MSG* pMsg)
{
	if (m_hAccelTable)
	{
		if (::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
			return (TRUE);
	}
	return CWnd ::PreTranslateMessage(pMsg);
}

void EditorInterface::OnLButtonDblClk(uint32_t nFlags, CPoint point)
{
	Stuff::Vector3D pos;
	Stuff::Vector2DOf<int32_t> screen;
	screen.x = point.x;
	screen.y = point.y;
	eye->inverseProject(screen, pos);
	EditorObject* pObject = EditorObjectMgr::instance()->getObjectAtPosition(pos);
	if (nullptr != pObject)
	{
		UnitSettings();
	}
	CWnd ::OnLButtonDblClk(nFlags, point);
}

void EditorInterface::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	static bool bFirstLoad = true;
	if (bFirstLoad)
	{
		if (!bThisIsInitialized)
		{
			/*paint splash screen*/
			if (!m_hSplashBitMap)
			{
				m_hSplashBitMap =
					(HBITMAP)LoadImage(nullptr, "esplash.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			}
			if (m_hSplashBitMap)
			{
				CRect rcClient;
				GetClientRect(rcClient);
				BITMAP bm_struct;
				CBitmap* pbmOld = nullptr;
				CDC dcMem;
				dcMem.CreateCompatibleDC(&dc);
				{
					CBitmap* pSplashBitmap = CBitmap::FromHandle(m_hSplashBitMap);
					pSplashBitmap->GetBitmap(&bm_struct);
					pbmOld = dcMem.SelectObject(pSplashBitmap);
				}
				int32_t gbmLeft = rcClient.right / 2 - bm_struct.bmwidth / 2;
				int32_t gbmTop = rcClient.bottom / 2 - bm_struct.bmheight / 2;
				int32_t gbmRight = gbmLeft + bm_struct.bmwidth;
				int32_t gbmBottom = gbmTop + bm_struct.bmheight;
				CRect rcTmp;
				rcTmp = rcClient;
				rcTmp.bottom = gbmTop;
				dc.FillSolidRect(&rcTmp, RGB(0, 0, 0));
				rcTmp = rcClient;
				rcTmp.top = gbmBottom;
				dc.FillSolidRect(&rcTmp, RGB(0, 0, 0));
				rcTmp = rcClient;
				rcTmp.right = gbmLeft;
				dc.FillSolidRect(&rcTmp, RGB(0, 0, 0));
				rcTmp = rcClient;
				rcTmp.left = gbmRight;
				dc.FillSolidRect(&rcTmp, RGB(0, 0, 0));
				dc.BitBlt(rcClient.right / 2 - bm_struct.bmwidth / 2,
					rcClient.bottom / 2 - bm_struct.bmheight / 2, bm_struct.bmwidth,
					bm_struct.bmheight, &dcMem, 0, 0, SRCCOPY);
				dcMem.SelectObject(pbmOld);
				dcMem.DeleteDC();
			}
		}
		else
		{
			bFirstLoad = false;
		}
	}
	if (ProcessingError || !bThisIsInitialized || bIsLoading)
	{
		return;
	}
	SafeRunGameOSLogic();
	/* This hack is here because syncScrollBars() depends on the function
	Camera::inverseProject(...) which, for some reason, doesn't return the
	correct value until four frames have been drawn.*/
	if (4 == turn)
	{
		syncScrollBars();
	}
	// Do not call CWnd ::OnPaint() for painting messages
}

void EditorInterface::OnViewRefreshtacmap()
{
	tacMap.UpdateMap();
}

bool EditorInterface::SafeRunGameOSLogic()
{
	if (bThisIsInitialized && (nullptr != land))
	{
		if (!m_bInRunGameOSLogicCall)
		{
			m_bInRunGameOSLogicCall = true;
			RunGameOSLogic();
			m_bInRunGameOSLogicCall = false;
			return true;
		}
	}
	return false;
}

void EditorInterface::rotateSelectedObjects(int32_t direction)
{
	ModifyBuildingAction* pAction = new ModifyBuildingAction;
	EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjects;
	selectedObjects = EditorObjectMgr::instance()->getSelectedObjectList();
	EditorObjectMgr::EDITOR_OBJECT_LIST::EIterator iter = selectedObjects.Begin();
	while (!iter.IsDone())
	{
		pAction->addBuildingInfo(*(*iter));
		int32_t id = (*iter)->getID();
		int32_t fitID = EditorObjectMgr::instance()->getFitID(id);
		if ((EditorObjectMgr::WALL == (*iter)->getSpecialType()) || (33 /*repair bay*/ == fitID))
		{
			(*iter)->appearance()->rotation += direction * 90;
		}
		else
		{
			(*iter)->appearance()->rotation += direction * 45;
		}
		iter++;
	}
	undoMgr.AddAction(pAction);
	pAction = nullptr;
}

static void
UpdateMissionPlayerPlayer(int32_t player, CCmdUI* pCmdUI)
{
	if (EditorData::instance->MaxPlayers() < player)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

void EditorInterface::OnUpdateMissionPlayerPlayer3(CCmdUI* pCmdUI)
{
	UpdateMissionPlayerPlayer(3, pCmdUI);
}

void EditorInterface::OnUpdateMissionPlayerPlayer4(CCmdUI* pCmdUI)
{
	UpdateMissionPlayerPlayer(4, pCmdUI);
}

void EditorInterface::OnUpdateMissionPlayerPlayer5(CCmdUI* pCmdUI)
{
	UpdateMissionPlayerPlayer(5, pCmdUI);
}

void EditorInterface::OnUpdateMissionPlayerPlayer6(CCmdUI* pCmdUI)
{
	UpdateMissionPlayerPlayer(6, pCmdUI);
}

void EditorInterface::OnUpdateMissionPlayerPlayer7(CCmdUI* pCmdUI)
{
	UpdateMissionPlayerPlayer(7, pCmdUI);
}

void EditorInterface::OnUpdateMissionPlayerPlayer8(CCmdUI* pCmdUI)
{
	UpdateMissionPlayerPlayer(8, pCmdUI);
}

static void
UpdateMissionTeamTeam(int32_t team, CCmdUI* pCmdUI)
{
	if (EditorData::instance->MaxTeams() < team)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

void EditorInterface::OnUpdateMissionTeamTeam3(CCmdUI* pCmdUI)
{
	UpdateMissionTeamTeam(3, pCmdUI);
}

void EditorInterface::OnUpdateMissionTeamTeam4(CCmdUI* pCmdUI)
{
	UpdateMissionTeamTeam(4, pCmdUI);
}

void EditorInterface::OnUpdateMissionTeamTeam5(CCmdUI* pCmdUI)
{
	UpdateMissionTeamTeam(5, pCmdUI);
}

void EditorInterface::OnUpdateMissionTeamTeam6(CCmdUI* pCmdUI)
{
	UpdateMissionTeamTeam(6, pCmdUI);
}

void EditorInterface::OnUpdateMissionTeamTeam7(CCmdUI* pCmdUI)
{
	UpdateMissionTeamTeam(7, pCmdUI);
}

void EditorInterface::OnUpdateMissionTeamTeam8(CCmdUI* pCmdUI)
{
	UpdateMissionTeamTeam(8, pCmdUI);
}

void EditorInterface::OnDestroy()
{
	CWnd ::OnDestroy();
	if (tacMap.m_hWnd)
	{
		tacMap.DestroyWindow();
	}
	else
	{
		gosASSERT(false);
	}
}

//#pragma warning( default:4244 )

void EditorInterface::OnForestTool()
{
	if (EditorInterface::instance()->ObjectSelectOnlyMode())
	{
		return;
	}
	ForestDlg dlg;
	auto count = 0;
	float xAvg = 0;
	float yAvg = 0;
	// figure out selection
	for (size_t j = 0; j < land->realVerticesMapSide; ++j)
	{
		for (size_t i = 0; i < land->realVerticesMapSide; ++i)
		{
			if (land->isVertexSelected(j, i))
			{
				Stuff::Vector3D pos;
				land->tileCellToWorld(j, i, 0, 0, pos);
				xAvg += pos.x;
				yAvg += pos.y;
				count++;
			}
		}
	}
	float centerX = xAvg / count;
	float centerY = yAvg / count;
	float dist = 0;
	for (j = 0; j < land->realVerticesMapSide; ++j)
	{
		for (size_t i = 0; i < land->realVerticesMapSide; ++i)
		{
			if (land->isVertexSelected(j, i))
			{
				Stuff::Vector3D pos;
				land->tileCellToWorld(j, i, 0, 0, pos);
				float deltaX = pos.x - centerX;
				float deltaY = pos.y - centerY;
				float tmpDist = deltaX * deltaX + deltaY * deltaY;
				if (tmpDist > dist)
					dist = tmpDist;
			}
		}
	}
	dlg.m_xLoc = centerX;
	dlg.m_yLoc = centerY;
	dist += 128 * 128; // always make as big as one tile
	dlg.m_radius = (float)sqrt(dist);
	if (IDOK == dlg.DoModal())
	{
		EditorObjectMgr::instance()->createForest(dlg.forest);
	}
}

void EditorInterface::OnOtherEditforests()
{
	if (EditorInterface::instance()->ObjectSelectOnlyMode())
	{
		return;
	}
	EditForestDlg dlg;
	dlg.DoModal();
}

bool TeamsAction::undo()
{
	CTeams swap = EditorData::instance->TeamsRef();
	EditorData::instance->TeamsRef() = PreviousTeams();
	PreviousTeams(swap);
	return true;
}

CTeams
TeamsAction::PreviousTeams()
{
	m_previousTeams.RestoreObjectPointerReferencesFromNotedPositions();
	return m_previousTeams;
}

void TeamsAction::PreviousTeams(const CTeams& teams)
{
	m_previousTeams = teams;
	/*Some of the objective conditions have pointers to objects. These pointers
	may become invalid in the case that an object is deleted then restored (via
	undo/redo), so we store the positions of the objects referred to so that we
	can use them later to retrieve valid pointers to the objects. Btw, we do
	know that the object positions will be valid when we need to infer the
	pointers from them. */
	m_previousTeams.NoteThePositionsOfObjectsReferenced();
}

void EditorInterface::OnViewOrthographiccamera()
{
	if (GetParent()->GetMenu()->GetMenuState(ID_VIEW_ORTHOGRAPHICCAMERA, MF_BYCOMMAND) & MF_CHECKED)
	{
		GetParent()->GetMenu()->CheckMenuItem(
			ID_VIEW_ORTHOGRAPHICCAMERA, MF_BYCOMMAND | MF_UNCHECKED);
		eye->usePerspective = true;
	}
	else
	{
		GetParent()->GetMenu()->CheckMenuItem(
			ID_VIEW_ORTHOGRAPHICCAMERA, MF_BYCOMMAND | MF_CHECKED);
		eye->usePerspective = false;
	}
	eye->rotateRight(180.0 /*degrees*/);
	SafeRunGameOSLogic();
	syncScrollBars();
}

void EditorInterface::OnViewShowpassabilitymap()
{
	if (GetParent()->GetMenu()->GetMenuState(ID_VIEW_SHOWPASSABILITYMAP, MF_BYCOMMAND) & MF_CHECKED)
	{
		GetParent()->GetMenu()->CheckMenuItem(
			ID_VIEW_SHOWPASSABILITYMAP, MF_BYCOMMAND | MF_UNCHECKED);
		drawTerrainGrid = false;
	}
	else
	{
		GetParent()->GetMenu()->CheckMenuItem(
			ID_VIEW_SHOWPASSABILITYMAP, MF_BYCOMMAND | MF_CHECKED);
		drawTerrainGrid = true;
	}
}

void EditorInterface::OnMButtonUp(uint32_t nFlags, CPoint point)
{
	if (ThisIsInitialized() && eye)
	{
		eye->allNormal();
	}
	CWnd ::OnMButtonUp(nFlags, point);
}
