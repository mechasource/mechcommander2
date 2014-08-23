//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_VIEWCONDITIONORACTIONDLG_H__9A818E75_A25C_4F93_9BC7_F990A52B8CD8__INCLUDED_)
#define AFX_VIEWCONDITIONORACTIONDLG_H__9A818E75_A25C_4F93_9BC7_F990A52B8CD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewConditionOrActionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ViewConditionOrActionDlg dialog

class ViewConditionOrActionDlg : public CDialog
{
// Construction
public:
	ViewConditionOrActionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ViewConditionOrActionDlg)
	enum { IDD = IDD_VIEW_CONDITION_OR_ACTION };
	CString	m_Edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ViewConditionOrActionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ViewConditionOrActionDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWCONDITIONORACTIONDLG_H__9A818E75_A25C_4F93_9BC7_F990A52B8CD8__INCLUDED_)
