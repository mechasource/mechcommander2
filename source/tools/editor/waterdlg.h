//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(AFX_WATERDLG_H)
#define AFX_WATERDLG_H

/////////////////////////////////////////////////////////////////////////////
// WaterDlg dialog

class WaterDlg : public CDialog
{
	// Construction
  public:
	WaterDlg(CWnd* pParent = nullptr); // standard constructor

	// Dialog Data
	//{{AFX_DATA(WaterDlg)
	enum
	{
		IDD = IDD_WAVES
	};
	float amplitude;
	float frequency;
	float elevation;
	float shallowElevation;
	float alphaElevation;
	uint32_t alphaDeep;
	uint32_t alphaMiddle;
	uint32_t alphaShallow;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WaterDlg)
  protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
													 //}}AFX_VIRTUAL

	// Implementation
  protected:
	// Generated message map functions
	//{{AFX_MSG(WaterDlg)
	virtual BOOL OnInitDialog(void);
	virtual void OnOK(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(AFX_WATERDLG_H)
