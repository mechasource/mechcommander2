//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(AFX_PLAYERSETTINGSDLG_H__E406C43E_70BF_49B1_A3E1_5305FF16D93B__INCLUDED_)
#define AFX_PLAYERSETTINGSDLG_H__E406C43E_70BF_49B1_A3E1_5305FF16D93B__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// PlayerSettingsDlg dialog

class PlayerSettingsDlg : public CDialog
{
	// Construction
public:
	PlayerSettingsDlg(CWnd* pParent = nullptr); // standard constructor

	// Dialog Data
	//{{AFX_DATA(PlayerSettingsDlg)
	enum
	{
		IDD = IDD_PLAYER_SETTINGS
	};
	CComboBox m_defaultTeamComboBox;
	int32_t m_playerEdit;
	//}}AFX_DATA
	int32_t m_oldDefaultTeam;
	int32_t m_newDefaultTeam;
	int32_t m_numTeams;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PlayerSettingsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
													 //}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PlayerSettingsDlg)
	virtual BOOL OnInitDialog(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(AFX_PLAYERSETTINGSDLG_H__E406C43E_70BF_49B1_A3E1_5305FF16D93B__INCLUDED_)
