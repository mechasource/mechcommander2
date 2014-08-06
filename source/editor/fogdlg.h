#ifndef FOGDLG_H
#define FOGDLG_H

/*************************************************************************************************\
FogDlg.h			: Interface for the FogDlg component.  This thing lets the user input fog
						settings
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdafx.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// FogDlg dialog

class FogDlg : public CDialog
{
// Construction
public:
	FogDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FogDlg)
	enum { IDD = IDD_FOG };
	BYTE	m_blue;
	BYTE	m_green;
	BYTE	m_red;
	float	m_start;
	float	m_end;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FogDlg)
	afx_msg void OnChangeBlue1();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CBrush brush;
};


#endif  // end of file ( FogDlg.h )
