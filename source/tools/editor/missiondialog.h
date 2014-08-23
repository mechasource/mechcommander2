//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_MISSIONDIALOG_H__7B092FC7_3350_44B4_881D_79E7AF47E4DA__INCLUDED_)
#define AFX_MISSIONDIALOG_H__7B092FC7_3350_44B4_881D_79E7AF47E4DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MissionDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMissionDialog dialog

class CMissionDialog : public CDialog
{
// Construction
public:
	CMissionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMissionDialog)
	enum { IDD = IDD_MISSION_DIALOG };
	CString	m_MissionFileEdit;
	CString	m_PurchaseFileEdit;
	BOOL	m_LogisticsCheck;
	BOOL	m_MandatoryCheck;
	BOOL	m_PilotPromotionCheck;
	BOOL	m_PurchasingCheck;
	BOOL	m_SalvageCheck;
	BOOL	m_SelectionScreenCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMissionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMissionDialog)
	afx_msg void OnMiMissionFileBrowseButton();
	afx_msg void OnMiPurchaseFileBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MISSIONDIALOG_H__7B092FC7_3350_44B4_881D_79E7AF47E4DA__INCLUDED_)
