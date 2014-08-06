//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_NEWSINGLEMISSION_H__B7294D0A_D020_48C8_B409_E2EA0323998E__INCLUDED_)
#define AFX_NEWSINGLEMISSION_H__B7294D0A_D020_48C8_B409_E2EA0323998E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// newsinglemission.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NewSingleMission dialog

class NewSingleMission : public CDialog
{
// Construction
public:
	NewSingleMission(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(NewSingleMission)
	enum { IDD = IDD_NEW_SINGLEPLAYER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewSingleMission)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(NewSingleMission)
	afx_msg void OnLoadMission();
	afx_msg void OnNewmission();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSINGLEMISSION_H__B7294D0A_D020_48C8_B409_E2EA0323998E__INCLUDED_)
