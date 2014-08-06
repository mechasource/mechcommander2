//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// MissionSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MissionSettingsDlg.h"
#include "assert.h"

#include "EditorData.h" /* just for definition of GAME_MAX_PLAYERS */
#include "ResourceStringSelectionDlg.h"
#include "../MCLib/Utilities.h" /*for cLoadString*/
#include "UserTextEdit.h"

/* mh: I notice the compiler seems complain about DEBUG_NEW not being defined
  sometimes. I don't why it does this sometimes and sometimes not. It doesn't seem
  to do this when I do a clean rebuild. */
#if 0 /*gos doesn't like this */
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif /*gos doesn't like this */

/////////////////////////////////////////////////////////////////////////////
// MissionSettingsDlg dialog


MissionSettingsDlg::MissionSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MissionSettingsDlg::IDD, pParent)
{
	m_MissionNameResourceStringID = 0;
	m_BlurbResourceStringID = 0;
	m_Blurb2ResourceStringID = 0;

	//{{AFX_DATA_INIT(MissionSettingsDlg)
	m_AuthorEdit = _T("");
	m_MaxPlayers = 0;
	m_MaxTeams = 0;
	m_TimeLimit = -1.0;
	m_SinglePlayerCheck = FALSE;
	m_InitialResourcePoints = 0;
	m_ScenarioTune = _T("");
	m_VideoFilename = _T("");
	m_CBills = 0;
	m_NumRPBuildings = 0;
	m_DownloadUrlEdit = _T("");
	m_MissionType = -1;
	m_AirStrikeCheck = FALSE;
	m_AllTech = FALSE;
	m_MineLayerCheck = FALSE;
	m_NoVariantsCheck = FALSE;
	m_RepairVehicleCheck = FALSE;
	m_ResourceBuildingCheck = FALSE;
	m_SalvageCraftCheck = FALSE;
	m_ScoutCopterCheck = FALSE;
	m_SensorProbeCheck = FALSE;
	m_UnlimitedAmmoCheck = FALSE;
	m_DropWeightLimit = 0;
	m_ArtilleryPieceCheck = FALSE;
	m_RPsForMechsCheck = FALSE;
	m_MissionNameEdit = _T("");
	m_BlurbEdit = _T("");
	m_Blurb2Edit = _T("");
	m_BlurbResourceStringIDEdit = _T("");
	m_Blurb2ResourceStringIDEdit = _T("");
	m_MissionNameResourceStringIDEdit = _T("");
	//}}AFX_DATA_INIT
}


void MissionSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MissionSettingsDlg)
	DDX_Control(pDX, IDC_NUM_RP_BUILDINGS_SPIN, m_NumRPBuildingsSpin);
	DDX_Control(pDX, IDC_MAX_PLAYERS_SPIN, m_MaxPlayersSpin);
	DDX_Control(pDX, IDC_MAX_TEAMS_SPIN, m_MaxTeamsSpin);
	DDX_Text(pDX, IDC_AUTHOR_EDIT, m_AuthorEdit);
	DDX_Text(pDX, IDC_MAX_PLAYERS_EDIT, m_MaxPlayers);
	DDV_MinMaxInt(pDX, m_MaxPlayers, 2, 8);
	DDX_Text(pDX, IDC_MAX_TEAMS_EDIT, m_MaxTeams);
	DDV_MinMaxInt(pDX, m_MaxTeams, 2, 8);
	DDX_Text(pDX, IDC_TIME_LIMIT_EDIT, m_TimeLimit);
	DDV_MinMaxInt(pDX, m_TimeLimit, -1, 2147483647);
	DDX_Check(pDX, IDC_SINGLE_PLAYER_CHECK, m_SinglePlayerCheck);
	DDX_Text(pDX, IDC_INITIAL_RESOURCE_POINTS_EDIT, m_InitialResourcePoints);
	DDX_CBString(pDX, IDC_SCENARIO_TUNE_COMBO, m_ScenarioTune);
	DDX_Text(pDX, IDC_VIDEO_FILENAME_EDIT, m_VideoFilename);
	DDX_Text(pDX, IDC_INITIAL_CBILLS_EDIT, m_CBills);
	DDX_Text(pDX, IDC_NUM_RP_BUILDINGS_EDIT, m_NumRPBuildings);
	DDV_MinMaxInt(pDX, m_NumRPBuildings, 0, 32);
	DDX_Text(pDX, IDC_DOWNLOAD_URL_EDIT, m_DownloadUrlEdit);
	DDX_CBIndex(pDX, IDC_MISSION_TYPE_COMBO, m_MissionType);
	DDX_Check(pDX, IDC_AIR_STRIKE_CHECK, m_AirStrikeCheck);
	DDX_Check(pDX, IDC_ALL_TECH_CHECK, m_AllTech);
	DDX_Check(pDX, IDC_MINE_LAYER_CHECK, m_MineLayerCheck);
	DDX_Check(pDX, IDC_NO_VARIANTS_CHECK, m_NoVariantsCheck);
	DDX_Check(pDX, IDC_REPAIR_VEHICLE_CHECK, m_RepairVehicleCheck);
	DDX_Check(pDX, IDC_RESOURCE_BUILDING_CHECK, m_ResourceBuildingCheck);
	DDX_Check(pDX, IDC_SALVAGE_CRAFT_CHECK, m_SalvageCraftCheck);
	DDX_Check(pDX, IDC_SCOUT_COPTER_CHECK, m_ScoutCopterCheck);
	DDX_Check(pDX, IDC_SENSOR_PROBE_CHECK, m_SensorProbeCheck);
	DDX_Check(pDX, IDC_UNLIMITED_AMMO_CHECK, m_UnlimitedAmmoCheck);
	DDX_Text(pDX, IDC_DROP_WEIGHT_EDIT, m_DropWeightLimit);
	DDV_MinMaxInt(pDX, m_DropWeightLimit, 30, 999);
	DDX_Check(pDX, IDC_ARTILLERY_PIECE_CHECK, m_ArtilleryPieceCheck);
	DDX_Check(pDX, IDC_RPS_FOR_MECHS_CHECK, m_RPsForMechsCheck);
	DDX_Text(pDX, IDC_MISSION_NAME_EDIT, m_MissionNameEdit);
	DDX_Text(pDX, IDC_BLURB_EDIT, m_BlurbEdit);
	DDX_Text(pDX, IDC_BLURB2_EDIT, m_Blurb2Edit);
	DDX_Text(pDX, IDC_BLURB_RESOURCE_STRING_ID_EDIT, m_BlurbResourceStringIDEdit);
	DDX_Text(pDX, IDC_BLURB2_RESOURCE_STRING_ID_EDIT, m_Blurb2ResourceStringIDEdit);
	DDX_Text(pDX, IDC_MISSION_NAME_RESOURCE_STRING_ID_EDIT, m_MissionNameResourceStringIDEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MissionSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(MissionSettingsDlg)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	ON_BN_CLICKED(IDC_MISSION_NAME_EDIT_BUTTON, OnMissionNameEditButton)
	ON_BN_CLICKED(IDC_BLURB_EDIT_BUTTON, OnBlurbEditButton)
	ON_BN_CLICKED(IDC_BLURB2_EDIT_BUTTON, OnBlurb2EditButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MissionSettingsDlg message handlers

static BOOL CSLoadString(int resourceID, CString &targetStr) {
	char szTmp[16384/*max string length*/];
	cLoadString( resourceID, szTmp, 16384/*max string length*/ );
	targetStr = szTmp;
	CString tmpStr;
	tmpStr.Format("mc2res.dll:%d Not defined", resourceID);
	if (0 == strcmp(tmpStr.GetBuffer(0), szTmp)) {
		return (0);
	}
	return (!0);
}

void MissionSettingsDlg::UpdateMissionNameDisplay() {
	UpdateData(TRUE);
	if (m_MissionNameUseResourceString) {
		m_MissionNameResourceStringIDEdit.Format("%d", m_MissionNameResourceStringID);
		int ret = CSLoadString(m_MissionNameResourceStringID, m_MissionNameEdit);
		if (0 == ret) {
			m_MissionNameEdit = _TEXT("");
		}
	} else {
		m_MissionNameResourceStringIDEdit = _TEXT("");
		m_MissionNameEdit = m_MissionNameUnlocalizedText;
	}
	UpdateData(FALSE);
}

void MissionSettingsDlg::UpdateBlurbDisplay() {
	UpdateData(TRUE);
	if (m_BlurbUseResourceString) {
		m_BlurbResourceStringIDEdit.Format("%d", m_BlurbResourceStringID);
		int ret = CSLoadString(m_BlurbResourceStringID, m_BlurbEdit);
		if (0 == ret) {
			m_BlurbEdit = _TEXT("");
		}
	} else {
		m_BlurbResourceStringIDEdit = _TEXT("");
		m_BlurbEdit = m_BlurbUnlocalizedText;
	}
	UpdateData(FALSE);
}

void MissionSettingsDlg::UpdateBlurb2Display() {
	UpdateData(TRUE);
	if (m_Blurb2UseResourceString) {
		m_Blurb2ResourceStringIDEdit.Format("%d", m_Blurb2ResourceStringID);
		int ret = CSLoadString(m_Blurb2ResourceStringID, m_Blurb2Edit);
		if (0 == ret) {
			m_Blurb2Edit = _TEXT("");
		}
	} else {
		m_Blurb2ResourceStringIDEdit = _TEXT("");
		m_Blurb2Edit = m_Blurb2UnlocalizedText;
	}
	UpdateData(FALSE);
}

BOOL MissionSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_MaxTeamsSpin.SetRange(2, GAME_MAX_PLAYERS);
	m_MaxPlayersSpin.SetRange(2, GAME_MAX_PLAYERS);
	m_NumRPBuildingsSpin.SetRange(0, 32/*max random RP buildings*/);

	UpdateMissionNameDisplay();
	UpdateBlurbDisplay();
	UpdateBlurb2Display();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MissionSettingsDlg::OnBrowseButton() 
{
	UpdateData(TRUE);
	while (true) {
		CFileDialog selectAVIFileDialog(TRUE,_T("AVI"),_T("*.AVI"),
						 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
						 _T("Movie (*.AVI)|*.AVI|"));
		selectAVIFileDialog.m_ofn.lpstrInitialDir = moviePath;
		if( selectAVIFileDialog.DoModal()==IDOK ) {
			CString pathname = selectAVIFileDialog.GetPathName();
			int CurrentDirectoryBufferLength = GetCurrentDirectory(0, 0);
			assert(1 <= CurrentDirectoryBufferLength);
			//TCHAR *CurrentDirectoryBuffer = new TCHAR[CurrentDirectoryBufferLength];
			TCHAR *CurrentDirectoryBuffer = (TCHAR *)malloc(sizeof(TCHAR) * CurrentDirectoryBufferLength);
			int ret = GetCurrentDirectory(CurrentDirectoryBufferLength, CurrentDirectoryBuffer);
			assert(CurrentDirectoryBufferLength - 1 == ret);
			ret = -1;
			if (pathname.GetLength() > (CurrentDirectoryBufferLength - 1)) {
				ret = _tcsnicmp(CurrentDirectoryBuffer, pathname.GetBuffer(0), CurrentDirectoryBufferLength - 1);
			}
			//delete [] CurrentDirectoryBuffer; CurrentDirectoryBuffer = 0;
			free(CurrentDirectoryBuffer); CurrentDirectoryBuffer = 0;
			if (0 == ret) {
				m_VideoFilename = (pathname.GetBuffer(0) + (CurrentDirectoryBufferLength - 1) + 1);
				break;
			} else {
				AfxMessageBox(IDS_MUST_BE_IN_SUBDIRECTORY);
			}
		} else {
			break;
		}
	}

	UpdateData(FALSE);
}

void MissionSettingsDlg::OnMissionNameEditButton() 
{
	CUserTextEdit userTextEditDialog;
	userTextEditDialog.m_UnlocalizedText = m_MissionNameUnlocalizedText;
	userTextEditDialog.m_UseResourceString = m_MissionNameUseResourceString;
	userTextEditDialog.m_ResourceStringID = m_MissionNameResourceStringID;
	int ret = userTextEditDialog.DoModal();
	if (IDOK == ret) {
		m_MissionNameUnlocalizedText = userTextEditDialog.m_UnlocalizedText;
		m_MissionNameUseResourceString = userTextEditDialog.m_UseResourceString;
		m_MissionNameResourceStringID = userTextEditDialog.m_ResourceStringID;
		UpdateMissionNameDisplay();
	}
}

void MissionSettingsDlg::OnBlurbEditButton() 
{
	CUserTextEdit userTextEditDialog;
	userTextEditDialog.m_UnlocalizedText = m_BlurbUnlocalizedText;
	userTextEditDialog.m_UseResourceString = m_BlurbUseResourceString;
	userTextEditDialog.m_ResourceStringID = m_BlurbResourceStringID;
	int ret = userTextEditDialog.DoModal();
	if (IDOK == ret) {
		m_BlurbUnlocalizedText = userTextEditDialog.m_UnlocalizedText;
		m_BlurbUseResourceString = userTextEditDialog.m_UseResourceString;
		m_BlurbResourceStringID = userTextEditDialog.m_ResourceStringID;
		UpdateBlurbDisplay();
	}
}

void MissionSettingsDlg::OnBlurb2EditButton() 
{
	CUserTextEdit userTextEditDialog;
	userTextEditDialog.m_UnlocalizedText = m_Blurb2UnlocalizedText;
	userTextEditDialog.m_UseResourceString = m_Blurb2UseResourceString;
	userTextEditDialog.m_ResourceStringID = m_Blurb2ResourceStringID;
	int ret = userTextEditDialog.DoModal();
	if (IDOK == ret) {
		m_Blurb2UnlocalizedText = userTextEditDialog.m_UnlocalizedText;
		m_Blurb2UseResourceString = userTextEditDialog.m_UseResourceString;
		m_Blurb2ResourceStringID = userTextEditDialog.m_ResourceStringID;
		UpdateBlurb2Display();
	}
}
