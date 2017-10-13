//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(                                                                  \
	AFX_MISSIONSETTINGSDLG_H__5A7645E1_81E6_419B_9FBD_C269597F248C__INCLUDED_)
#define AFX_MISSIONSETTINGSDLG_H__5A7645E1_81E6_419B_9FBD_C269597F248C__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// MissionSettingsDlg dialog

class MissionSettingsDlg : public CDialog
{
	// Construction
  public:
	MissionSettingsDlg(CWnd* pParent = nullptr); // standard constructor

	void UpdateMissionNameDisplay(void);
	void UpdateBlurbDisplay(void);
	void UpdateBlurb2Display(void);

	CString m_MissionNameUnlocalizedText;
	bool m_MissionNameUseResourceString;
	int32_t m_MissionNameResourceStringID;

	CString m_BlurbUnlocalizedText;
	bool m_BlurbUseResourceString;
	int32_t m_BlurbResourceStringID;

	CString m_Blurb2UnlocalizedText;
	bool m_Blurb2UseResourceString;
	int32_t m_Blurb2ResourceStringID;

	// Dialog Data
	//{{AFX_DATA(MissionSettingsDlg)
	enum
	{
		IDD = IDD_MISSION_SETTINGS
	};
	CSpinButtonCtrl m_NumRPBuildingsSpin;
	CSpinButtonCtrl m_MaxPlayersSpin;
	CSpinButtonCtrl m_MaxTeamsSpin;
	CString m_AuthorEdit;
	int32_t m_MaxPlayers;
	int32_t m_MaxTeams;
	int32_t m_TimeLimit;
	BOOL m_SinglePlayerCheck;
	int32_t m_InitialResourcePoints;
	CString m_ScenarioTune;
	CString m_VideoFilename;
	int32_t m_CBills;
	int32_t m_NumRPBuildings;
	CString m_DownloadUrlEdit;
	int32_t m_MissionType;
	BOOL m_AirStrikeCheck;
	BOOL m_AllTech;
	BOOL m_MineLayerCheck;
	BOOL m_NoVariantsCheck;
	BOOL m_RepairVehicleCheck;
	BOOL m_ResourceBuildingCheck;
	BOOL m_SalvageCraftCheck;
	BOOL m_ScoutCopterCheck;
	BOOL m_SensorProbeCheck;
	BOOL m_UnlimitedAmmoCheck;
	int32_t m_DropWeightLimit;
	BOOL m_ArtilleryPieceCheck;
	BOOL m_RPsForMechsCheck;
	CString m_MissionNameEdit;
	CString m_BlurbEdit;
	CString m_Blurb2Edit;
	CString m_BlurbResourceStringIDEdit;
	CString m_Blurb2ResourceStringIDEdit;
	CString m_MissionNameResourceStringIDEdit;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MissionSettingsDlg)
  protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
													 //}}AFX_VIRTUAL

	// Implementation
  protected:
	// Generated message map functions
	//{{AFX_MSG(MissionSettingsDlg)
	virtual BOOL OnInitDialog(void);
	afx_msg void OnBrowseButton(void);
	afx_msg void OnMissionNameEditButton(void);
	afx_msg void OnBlurbEditButton(void);
	afx_msg void OnBlurb2EditButton(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(AFX_MISSIONSETTINGSDLG_H__5A7645E1_81E6_419B_9FBD_C269597F248C__INCLUDED_)
