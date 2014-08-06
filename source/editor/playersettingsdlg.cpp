//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// PlayerSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PlayerSettingsDlg.h"

#include "EditorData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlayerSettingsDlg dialog


PlayerSettingsDlg::PlayerSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PlayerSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PlayerSettingsDlg)
	m_playerEdit = 0;
	//}}AFX_DATA_INIT
	m_oldDefaultTeam = 0;
	m_numTeams = 8/*hardcoded*/;
}


void PlayerSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PlayerSettingsDlg)
	DDX_Control(pDX, IDC_PLAYER_SETTINGS_DEFAULT_TEAM_COMBO, m_defaultTeamComboBox);
	DDX_Text(pDX, IDC_PLAYER_SETTINGS_PLAYER_EDIT, m_playerEdit);
	DDV_MinMaxInt(pDX, m_playerEdit, 1, 8);
	//}}AFX_DATA_MAP

	m_newDefaultTeam = m_defaultTeamComboBox.GetCurSel();
	if (0 >= m_newDefaultTeam) {
		m_newDefaultTeam = m_defaultTeamComboBox.GetCurSel();
	} else {
		m_newDefaultTeam = m_defaultTeamComboBox.GetCurSel();
	}
}


BEGIN_MESSAGE_MAP(PlayerSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(PlayerSettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlayerSettingsDlg message handlers

BOOL PlayerSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_defaultTeamComboBox.SetCurSel(m_oldDefaultTeam);
	if (0 < m_numTeams) {
		int i;
		for (i = m_defaultTeamComboBox.GetCount() - 1; i >= m_numTeams; i--) {
			m_defaultTeamComboBox.DeleteString(i);
		}
	}

	if (EditorData::instance->IsSinglePlayer() && (1 == m_playerEdit)) {
		m_defaultTeamComboBox.EnableWindow(FALSE);
	} else {
		m_defaultTeamComboBox.EnableWindow(TRUE);
	}
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
