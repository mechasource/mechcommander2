//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_GROUPDIALOG_H__E1CC8418_9550_4243_8187_6746DEF69E20__INCLUDED_)
#define AFX_GROUPDIALOG_H__E1CC8418_9550_4243_8187_6746DEF69E20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupDialog.h : header file
//

#include "CampaignData.h"

/////////////////////////////////////////////////////////////////////////////
// CGroupDialog dialog

class CGroupDialog : public CDialog
{
// Construction
public:
	CGroupDialog(CWnd* pParent = NULL);   // standard constructor

	int m_TuneNumber;
	CMissionList m_MissionList;

// Dialog Data
	//{{AFX_DATA(CGroupDialog)
	enum { IDD = IDD_GROUP };
	CComboBox	m_TuneComboControl;
	CListBox	m_MissionListControl;
	CString	m_OperationFileEdit;
	CString	m_PreVideoFileEdit;
	CString	m_VideoFileEdit;
	CString	m_LabelEdit;
	int		m_NumMissionsToCompleteEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGroupDialog)
	afx_msg void OnGrOperationFileBrowseButton();
	afx_msg void OnGrPrevideoFileBrowseButton();
	afx_msg void OnGrVideoFileBrowseButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnGrAddButton();
	afx_msg void OnGrRemoveButton();
	afx_msg void OnGrEditButton();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPDIALOG_H__E1CC8418_9550_4243_8187_6746DEF69E20__INCLUDED_)
