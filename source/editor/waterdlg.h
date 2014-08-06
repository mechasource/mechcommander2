//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_WATERDLG_H)
#define AFX_WATERDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WaterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// WaterDlg dialog

class WaterDlg : public CDialog
{
// Construction
public:
	WaterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(WaterDlg)
	enum { IDD = IDD_WAVES };
	float	amplitude;
	float	frequency;
	float	elevation;
	float	shallowElevation;
	float	alphaElevation;
	DWORD	alphaDeep;
	DWORD	alphaMiddle;
	DWORD	alphaShallow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WaterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(WaterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERDLG_H)
