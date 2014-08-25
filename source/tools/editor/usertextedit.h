//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#if !defined(AFX_USERTEXTEDIT_H__5C5823FE_6B1B_469D_B5E8_223BE4A723AD__INCLUDED_)
#define AFX_USERTEXTEDIT_H__5C5823FE_6B1B_469D_B5E8_223BE4A723AD__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CUserTextEdit dialog

class CUserTextEdit : public CDialog
{
// Construction
public:
	CUserTextEdit(CWnd* pParent = NULL);   // standard constructor
	void UpdateTextDisplay(void);

	CString m_UnlocalizedText;
	bool m_UseResourceString;
	int32_t m_ResourceStringID;

// Dialog Data
	//{{AFX_DATA(CUserTextEdit)
	enum { IDD = IDD_USER_TEXT_EDIT_DIALOG };
	CString	m_Edit;
	CString	m_ResourceStringIDEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserTextEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserTextEdit)
	afx_msg void OnUserTextEditEnterTextButton(void);
	afx_msg void OnUserTextEditSelectResourceStringButton(void);
	virtual BOOL OnInitDialog(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERTEXTEDIT_H__5C5823FE_6B1B_469D_B5E8_223BE4A723AD__INCLUDED_)
