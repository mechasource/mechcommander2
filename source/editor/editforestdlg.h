//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_EDITFORESTDLG_H__DDB46B57_6D01_4DAF_8296_C5003400222B__INCLUDED_)
#define AFX_EDITFORESTDLG_H__DDB46B57_6D01_4DAF_8296_C5003400222B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditForestDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EditForestDlg dialog

class EditForestDlg : public CDialog
{
// Construction
public:
	EditForestDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(EditForestDlg)
	enum { IDD = IDD_FOREST_LIST };
	CListBox	m_fileList;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditForestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(EditForestDlg)
	virtual void OnOK();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnLoad();
	virtual BOOL OnInitDialog();
	afx_msg void OnSave();
	afx_msg void OnRename();
	afx_msg void OnSelchangeListFiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITFORESTDLG_H__DDB46B57_6D01_4DAF_8296_C5003400222B__INCLUDED_)
