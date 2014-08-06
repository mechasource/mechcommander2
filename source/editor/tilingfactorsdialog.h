//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_TILINGFACTORSDIALOG_H__6448B5D2_29B1_452A_AE46_4E493B6C5EE6__INCLUDED_)
#define AFX_TILINGFACTORSDIALOG_H__6448B5D2_29B1_452A_AE46_4E493B6C5EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TilingFactorsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TilingFactorsDialog dialog

class TilingFactorsDialog : public CDialog
{
// Construction
public:
	TilingFactorsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TilingFactorsDialog)
	enum { IDD = IDD_TILING_FACTORS_DIALOG };
	float	m_TerrainDetailTilingFactor;
	float	m_WaterDetailTilingFactor;
	float	m_WaterTilingFactor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TilingFactorsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TilingFactorsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TILINGFACTORSDIALOG_H__6448B5D2_29B1_452A_AE46_4E493B6C5EE6__INCLUDED_)
