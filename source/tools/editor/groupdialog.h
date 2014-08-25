//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(AFX_GROUPDIALOG_H__E1CC8418_9550_4243_8187_6746DEF69E20__INCLUDED_)
#define AFX_GROUPDIALOG_H__E1CC8418_9550_4243_8187_6746DEF69E20__INCLUDED_

//#include "campaigndata.h"

/////////////////////////////////////////////////////////////////////////////
// CGroupDialog dialog

class CGroupDialog : public CDialog
{
// Construction
public:
	CGroupDialog(CWnd* pParent = NULL);   // standard constructor

	int32_t m_TuneNumber;
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
	int32_t		m_NumMissionsToCompleteEdit;
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
	afx_msg void OnGrOperationFileBrowseButton(void);
	afx_msg void OnGrPrevideoFileBrowseButton(void);
	afx_msg void OnGrVideoFileBrowseButton(void);
	virtual BOOL OnInitDialog(void);
	afx_msg void OnGrAddButton(void);
	afx_msg void OnGrRemoveButton(void);
	afx_msg void OnGrEditButton(void);
	virtual void OnOK(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPDIALOG_H__E1CC8418_9550_4243_8187_6746DEF69E20__INCLUDED_)
