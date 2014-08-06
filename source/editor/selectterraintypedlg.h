//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_SELECTTERRAINTYPEDLG_H__48CD1D5A_9E04_4C8A_95C4_39A1C6D659D0__INCLUDED_)
#define AFX_SELECTTERRAINTYPEDLG_H__48CD1D5A_9E04_4C8A_95C4_39A1C6D659D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectTerrainTypeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectTerrainTypeDlg dialog

class SelectTerrainTypeDlg : public CDialog
{
// Construction
public:
	SelectTerrainTypeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SelectTerrainTypeDlg)
	enum { IDD = IDD_SELECT_TERRAIN_TYPE };
	CListBox	m_terrainTypeList;
	//}}AFX_DATA

	long m_selectedTerrainType;
	long SelectedTerrainType() { return m_selectedTerrainType; }
	void SelectedTerrainType(long selectedTerrainType) { m_selectedTerrainType = selectedTerrainType; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectTerrainTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelectTerrainTypeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTTERRAINTYPEDLG_H__48CD1D5A_9E04_4C8A_95C4_39A1C6D659D0__INCLUDED_)
