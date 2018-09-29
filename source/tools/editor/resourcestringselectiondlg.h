//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(AFX_RESOURCESTRINGSELECTIONDLG_H__90F375DF_47EA_4C33_A4EB_281A4DADDC12__INCLUDED_)
#define AFX_RESOURCESTRINGSELECTIONDLG_H__90F375DF_47EA_4C33_A4EB_281A4DADDC12__INCLUDED_

//#include <elist.h>

/////////////////////////////////////////////////////////////////////////////
// ResourceStringSelectionDlg dialog

class ResourceStringSelectionDlg : public CDialog
{
	// Construction
public:
	ResourceStringSelectionDlg(CWnd* pParent = nullptr); // standard constructor

	int32_t m_BottomOfIDRange;
	int32_t m_TopOfIDRange;
	int32_t m_SelectedResourceStringID;

	// Dialog Data
	//{{AFX_DATA(ResourceStringSelectionDlg)
	enum
	{
		IDD = IDD_RESOURCE_STRING_SELECTION
	};
	CComboBox m_Combo;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ResourceStringSelectionDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
													 //}}AFX_VIRTUAL

	// Implementation
protected:
	class CIntList : public EList<int32_t, int32_t>
	{
	};
	CIntList m_ResourceStringIDs;

	// Generated message map functions
	//{{AFX_MSG(ResourceStringSelectionDlg)
	virtual BOOL OnInitDialog(void);
	virtual void OnOK(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(AFX_RESOURCESTRINGSELECTIONDLG_H__90F375DF_47EA_4C33_A4EB_281A4DADDC12__INCLUDED_)
