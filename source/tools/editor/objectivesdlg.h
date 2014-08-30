//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(AFX_OBJECTIVESDLG_H__2D45CD01_42FA_4FF0_AB18_BBBAA9B0E3A5__INCLUDED_)
#define AFX_OBJECTIVESDLG_H__2D45CD01_42FA_4FF0_AB18_BBBAA9B0E3A5__INCLUDED_

//#include "objective.h"

/////////////////////////////////////////////////////////////////////////////
// ObjectivesDlg dialog

class ObjectivesDlg : public CDialog
{
// Construction
public:
	ObjectivesDlg(CWnd* pParent = nullptr);   // standard constructor

	CObjectives m_ModifiedObjectives;

	int32_t nSelectionIndex;

// Dialog Data
	//{{AFX_DATA(ObjectivesDlg)
	enum { IDD = IDD_OBJECTIVES };
	CButton	m_EditButton;
	CButton	m_AddButton;
	CListBox	m_List;
	int32_t		m_TeamEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ObjectivesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ObjectivesDlg)
	afx_msg void OnObjectivesAddButton(void);
	afx_msg void OnObjectivesRemoveButton(void);
	afx_msg void OnObjectivesEditButton(void);
	afx_msg void OnObjectivesCopyButton(void);
	afx_msg void OnObjectivesMoveUpButton(void);
	afx_msg void OnObjectivesMoveDownButton(void);
	virtual BOOL OnInitDialog(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTIVESDLG_H__2D45CD01_42FA_4FF0_AB18_BBBAA9B0E3A5__INCLUDED_)
