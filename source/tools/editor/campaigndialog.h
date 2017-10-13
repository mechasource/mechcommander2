//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(                                                                  \
	AFX_CAMPAIGNDIALOG_H__911BFC1D_2B46_40F8_A288_1E0BDB371BDE__INCLUDED_)
#define AFX_CAMPAIGNDIALOG_H__911BFC1D_2B46_40F8_A288_1E0BDB371BDE__INCLUDED_

//#include "campaigneditor.h"
//#include "campaigndata.h"

/////////////////////////////////////////////////////////////////////////////
// CCampaignDialog dialog

class CCampaignDialog : public CDialog
{
	// Construction
  public:
	CCampaignDialog(CWnd* pParent = nullptr); // standard constructor

	CCampaignData m_CampaignData;
	CCampaignData m_LastSavedCampaignData;

	void UpdateNameDisplay(void);
	int32_t PromptAndSaveIfNecessary(void);
	int32_t SaveAs(void);
	int32_t Save(void);

	CString m_NameUnlocalizedText;
	bool m_NameUseResourceString;
	int32_t m_NameResourceStringID;

	// Dialog Data
	//{{AFX_DATA(CCampaignDialog)
	enum
	{
		IDD = IDD_CAMPAIGN
	};
	CListBox m_GroupListControl;
	int32_t m_CBillsEdit;
	CString m_FinalVideoEdit;
	CString m_NameEdit;
	CString m_NameResourceStringIDEdit;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCampaignDialog)
  protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
													 //}}AFX_VIRTUAL

	// Implementation
  protected:
	// Generated message map functions
	//{{AFX_MSG(CCampaignDialog)
	afx_msg void OnCaFinalVideoBrowseButton(void);
	afx_msg void OnCaExit(void);
	afx_msg void OnCaAddButton(void);
	afx_msg void OnCaSaveAsButton(void);
	afx_msg void OnCaLoadButton(void);
	afx_msg void OnCaEditButton(void);
	afx_msg void OnCaRemoveButton(void);
	afx_msg void OnCaMoveUpButton(void);
	afx_msg void OnCaMoveDownButton(void);
	afx_msg void OnCaNameEditButton(void);
	afx_msg void OnCaSaveButton(void);
	virtual BOOL OnInitDialog(void);
	afx_msg void OnClose(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(AFX_CAMPAIGNDIALOG_H__911BFC1D_2B46_40F8_A288_1E0BDB371BDE__INCLUDED_)
