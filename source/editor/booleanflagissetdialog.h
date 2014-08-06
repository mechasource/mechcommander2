//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_BOOLEANFLAGISSETDIALOG_H__08B5296B_F425_414C_92EE_86E619EC994B__INCLUDED_)
#define AFX_BOOLEANFLAGISSETDIALOG_H__08B5296B_F425_414C_92EE_86E619EC994B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BooleanFlagIsSetDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBooleanFlagIsSetDialog dialog

class CBooleanFlagIsSetDialog : public CDialog
{
// Construction
public:
	CBooleanFlagIsSetDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBooleanFlagIsSetDialog)
	enum { IDD = IDD_FLAG_IS_SET_DIALOG };
	int		m_Value;
	CString	m_FlagID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBooleanFlagIsSetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBooleanFlagIsSetDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOOLEANFLAGISSETDIALOG_H__08B5296B_F425_414C_92EE_86E619EC994B__INCLUDED_)
