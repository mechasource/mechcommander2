//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(AFX_BOOLEANFLAGISSETDIALOG_H__08B5296B_F425_414C_92EE_86E619EC994B__INCLUDED_)
#define AFX_BOOLEANFLAGISSETDIALOG_H__08B5296B_F425_414C_92EE_86E619EC994B__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CBooleanFlagIsSetDialog dialog
DISABLE_WARNING_PUSH(4626) // assignment operator could not be generated (due to MFC design)
class CBooleanFlagIsSetDialog : public CDialog
{
	// Construction
public:
	CBooleanFlagIsSetDialog(CWnd* pParent = nullptr); // standard constructor

	// Dialog Data
	//{{AFX_DATA(CBooleanFlagIsSetDialog)
	// enum { IDD = IDD_FLAG_IS_SET_DIALOG };
	int32_t m_Index;
	const std::wstring_view& m_FlagID;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBooleanFlagIsSetDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBooleanFlagIsSetDialog)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
DISABLE_WARNING_POP
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(AFX_BOOLEANFLAGISSETDIALOG_H__08B5296B_F425_414C_92EE_86E619EC994B__INCLUDED_)
