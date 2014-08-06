//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_CAMPAIGNDIALOG_H__911BFC1D_2B46_40F8_A288_1E0BDB371BDE__INCLUDED_)
#define AFX_CAMPAIGNDIALOG_H__911BFC1D_2B46_40F8_A288_1E0BDB371BDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CampaignDialog.h : header file
//

//include "CampaignEditor.h"
#include "resource.h"
#include "CampaignData.h"

/////////////////////////////////////////////////////////////////////////////
// CCampaignDialog dialog

class CCampaignDialog : public CDialog
{
// Construction
public:
	CCampaignDialog(CWnd* pParent = NULL);   // standard constructor

	CCampaignData m_CampaignData;
	CCampaignData m_LastSavedCampaignData;

	void UpdateNameDisplay();
	int PromptAndSaveIfNecessary();
	int SaveAs();
	int Save();

	CString m_NameUnlocalizedText;
	bool m_NameUseResourceString;
	int m_NameResourceStringID;

// Dialog Data
	//{{AFX_DATA(CCampaignDialog)
	enum { IDD = IDD_CAMPAIGN };
	CListBox	m_GroupListControl;
	int		m_CBillsEdit;
	CString	m_FinalVideoEdit;
	CString	m_NameEdit;
	CString	m_NameResourceStringIDEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCampaignDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCampaignDialog)
	afx_msg void OnCaFinalVideoBrowseButton();
	afx_msg void OnCaExit();
	afx_msg void OnCaAddButton();
	afx_msg void OnCaSaveAsButton();
	afx_msg void OnCaLoadButton();
	afx_msg void OnCaEditButton();
	afx_msg void OnCaRemoveButton();
	afx_msg void OnCaMoveUpButton();
	afx_msg void OnCaMoveDownButton();
	afx_msg void OnCaNameEditButton();
	afx_msg void OnCaSaveButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMPAIGNDIALOG_H__911BFC1D_2B46_40F8_A288_1E0BDB371BDE__INCLUDED_)
