//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(AFX_EDITFORESTDLG_H__DDB46B57_6D01_4DAF_8296_C5003400222B__INCLUDED_)
#define AFX_EDITFORESTDLG_H__DDB46B57_6D01_4DAF_8296_C5003400222B__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// EditForestDlg dialog

class EditForestDlg : public CDialog
{
	// Construction
public:
	EditForestDlg(CWnd* pParent = nullptr); // standard constructor

	// Dialog Data
	//{{AFX_DATA(EditForestDlg)
	enum
	{
		IDD = IDD_FOREST_LIST
	};
	CListBox m_fileList;
	CString m_Name;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditForestDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(EditForestDlg)
	virtual void OnOK(void);
	afx_msg void OnEdit(void);
	afx_msg void OnDelete(void);
	afx_msg void OnLoad(void);
	virtual BOOL OnInitDialog(void);
	afx_msg void OnSave(void);
	afx_msg void OnRename(void);
	afx_msg void OnSelchangeListFiles(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(AFX_EDITFORESTDLG_H__DDB46B57_6D01_4DAF_8296_C5003400222B__INCLUDED_)
