//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_SELECTSLOPEDIALOG_H__49EB9E50_A5AD_4B8C_925A_7D0CD9AD616B__INCLUDED_)
#define AFX_SELECTSLOPEDIALOG_H__49EB9E50_A5AD_4B8C_925A_7D0CD9AD616B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectSlopeDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectSlopeDialog dialog

class SelectSlopeDialog : public CDialog
{
// Construction
public:
	SelectSlopeDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SelectSlopeDialog)
	enum { IDD = IDD_SELECT_SLOPE_DIALOG };
	float	m_MaxEdit;
	float	m_MinEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectSlopeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelectSlopeDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSLOPEDIALOG_H__49EB9E50_A5AD_4B8C_925A_7D0CD9AD616B__INCLUDED_)
